#ifndef TREADPROZESSORS_H
#define TREADPROZESSORS_H

#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>
#include <iostream>
//#include <stringstream>

#include "AllProzessors.h"


template <typename DependentProcessor>
class ThreadedCommandProcessor : public CommandProcessor
{
public:
    ThreadedCommandProcessor(const std::string& aName, int aThreadsCount = 1);
    ~ThreadedCommandProcessor();

    void ProcessBatch(const CommandBatch& aCommandBatch) override;

    void Stop() override;

private:
    void ThreadProc(const std::string& aName);

    void ProcessQueue(std::unique_lock<std::mutex>& lk, CommandProcessor& aDependentProcessor);

    int mThreadsCount;
    std::queue<CommandBatch> mQueue;
    std::condition_variable mCondition;
    std::mutex mQueueMutex;
    std::atomic_bool mDone{false};

    std::vector<std::thread> mThreads;
};


template <typename DependentProcessor>
ThreadedCommandProcessor<DependentProcessor>::ThreadedCommandProcessor(const std::string& aName, int aThreadsCount)
    : CommandProcessor(aName)
    , mThreadsCount(aThreadsCount)
{
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor::ThreadedCommandProcessor, this==" << this << std::endl;
#endif
    for (int i = 0; i < mThreadsCount; ++i)
    {
        /// Гребаный поток!!!! Почему не запускается !!!!
        std::stringstream name;
        name << aName << i;
        mThreads.emplace_back(&ThreadedCommandProcessor::ThreadProc, this, name.str());
    }
}

template<typename DependentProcessor>
ThreadedCommandProcessor<DependentProcessor>::~ThreadedCommandProcessor()
{
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor::~ThreadedCommandProcessor, this==" << this << std::endl;
#endif
    Stop();

    mDone = true;
    mCondition.notify_all();
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop2()" << this << std::endl;
#endif
    for (auto& thread : mThreads)
        if (thread.joinable())
            thread.join();
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ProcessBatch(const CommandBatch& aCommandBatch)
{
    if (mDone.load())
        return;
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor notified - start, this==" << this << std::endl;
#endif
    {
        std::lock_guard<std::mutex> lk(mQueueMutex);
        mQueue.push(aCommandBatch);
    }
    mCondition.notify_all();
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor notified, this==" << this << std::endl;
#endif
    CommandProcessor::ProcessBatch(aCommandBatch);
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::Stop()
{
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop()" << this << std::endl;
#endif
    CommandProcessor::Stop();
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor<DependentProcessor>::Stop2()" << this << std::endl;
#endif
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ThreadProc(const std::string& aName)
{
#ifdef DEBUG_CONS
    std::cout << "ThreadedCommandProcessor::ThreadProc start, this==" << this << std::endl;
#endif
    try
    {
        DependentProcessor dependentProcessor(aName);

#ifdef DEBUG_CONS
        std::cout << "ThreadedCommandProcessor::ThreadProc 1, this==" << this << " " << aProcessor->mContext << std::endl;
#endif
        while (!mDone.load())
        {
            std::unique_lock<std::mutex> lk(mQueueMutex);
            while (mQueue.empty() && !mDone.load())
                mCondition.wait(lk);
            ProcessQueue(lk, dependentProcessor);
        }
        dependentProcessor.Stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
#ifdef DEBUG_PRINT
    std::cout << "ThreadedCommandProcessor::ThreadProc end, this==" << aProcessor << std::endl;
#endif
}

template <typename DependentProcessor>
void ThreadedCommandProcessor<DependentProcessor>::ProcessQueue(std::unique_lock<std::mutex>& lk, CommandProcessor& aDependentProcessor)
{
    std::queue<CommandBatch> queue;
    std::swap(mQueue, queue);
    lk.unlock();
    while (!queue.empty())
    {
        auto commandBatch = queue.front();
        queue.pop();
        aDependentProcessor.ProcessBatch(commandBatch);
    }
}



#endif // TREADPROZESSORS_H

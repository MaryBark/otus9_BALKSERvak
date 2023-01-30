#ifndef ALLPROZESSORS_H
#define ALLPROZESSORS_H


#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <optional>
#include <memory>
#include <string>
#include <deque>


/// Можно было бы рассовать каждый класс по файлам, и это было бы правильно, но потом запутаюсь из коммитить...


///--------------------------------------------------------------
/// Command     CommandBatch
///--------------------------------------------------------------

using Timestamp = std::chrono::system_clock::time_point;

struct Command
{
    std::string mText;
    Timestamp mTimestamp;
};

struct CommandBatch
{
    std::vector<Command> mCommands;
    Timestamp mTimestamp;

    std::size_t Size() const
    {
        return mCommands.size();
    }
};

using BulkQueue = std::deque<std::string>;






class ICommandProcessor
{
public:
    virtual ~ICommandProcessor() = default;

    virtual void StartBlock() = 0;
    virtual void FinishBlock() = 0;

    virtual void ProcessLine(const std::string&) = 0;
    virtual void ProcessCommand(const Command&) = 0;
    virtual void ProcessBatch(const CommandBatch&) = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
};






using CommandProcessors = std::vector<std::shared_ptr<ICommandProcessor>>;

class CommandProcessor : public ICommandProcessor
{
public:
    CommandProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors = CommandProcessors());

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessLine(const std::string& aLine) override;

    void ProcessCommand(const Command& aCommand) override;

    void ProcessBatch(const CommandBatch& aCommandBatch) override;

    void Start() override;
    void Stop() override;

protected:
    std::string mName;

private:
    CommandProcessors mDependentCommandProcessors;
};



class BatchCommandProcessor : public CommandProcessor
{
public:
    BatchCommandProcessor(const std::string& aName, std::size_t aBulkSize, const CommandProcessors& aDependentCommandProcessors);

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessCommand(const Command& aCommand) override;
    void Stop() override;

private:
    void ClearBatch();
    void DumpBatch();

    void ThreadProc();

    std::size_t mBulkSize;
    bool mBlockForced;
    std::vector<Command> mCommandBatch;
    std::thread mAutoDumpThread;
    std::optional<std::chrono::system_clock::time_point> mLastCommandTimestamp;
    std::mutex mBatchMutex;
    std::atomic<bool> mDone;
};




class InputProcessor : public CommandProcessor
{
public:
    InputProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors);

    void ProcessLine(const std::string& aLine) override;

private:
    int mBlockDepth{0};
};





//class AllProzessors
//{
//public:
//    AllProzessors();
//};

#endif // ALLPROZESSORS_H

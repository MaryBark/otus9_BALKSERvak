#include "AllProzessors.h"
#include <iostream>
#include <assert.h>

//AllProzessors::AllProzessors()
//{

//}


/////////////////////////////////////////////////////////////////////////////////
///
/// \brief CommandProcessor
///
////////////////////////////////////////////////////////////////////////////////


CommandProcessor::CommandProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors)
    : mName(aName)
    , mDependentCommandProcessors(aDependentCommandProcessors)
{
}

void CommandProcessor::StartBlock()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->StartBlock();
}

void CommandProcessor::FinishBlock()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->FinishBlock();
}

void CommandProcessor::ProcessLine(const std::string& aLine)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->ProcessLine(aLine);
}

void CommandProcessor::ProcessCommand(const Command& aCommand)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->ProcessCommand(aCommand);
}

void CommandProcessor::ProcessBatch(const CommandBatch& aCommandBatch)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
    {
        dependentCommandProcessor->ProcessBatch(aCommandBatch);
    }
}

void CommandProcessor::Start()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->Start();
}

void CommandProcessor::Stop()
{
#ifdef DEBUG_CONS
    std::cout << "CommandProcessor::Stop1()" << this << std::endl;
#endif
    Stop();
#ifdef DEBUG_CONS
    std::cout << "CommandProcessor::Stop2()" << this << std::endl;
#endif
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->Stop();
#ifdef DEBUG_CONS
    std::cout << "CommandProcessor::Stop3()" << this << std::endl;
#endif
}





/////////////////////////////////////////////////////////////////////////////
///
///                           BatchCommandProcessor
///
/////////////////////////////////////////////////////////////////////////////

BatchCommandProcessor::BatchCommandProcessor(const std::string& aName, std::size_t aBulkSize, const CommandProcessors& aDependentCommandProcessors)
    : CommandProcessor(aName, aDependentCommandProcessors)
    , mBulkSize(aBulkSize)
    , mBlockForced(false)
    , mAutoDumpThread(&BatchCommandProcessor::ThreadProc, this)
    , mDone(false)
{
}

void BatchCommandProcessor::StartBlock()
{
    mBlockForced = true;
    DumpBatch();
}

void BatchCommandProcessor::FinishBlock()
{
    mBlockForced = false;
    DumpBatch();
}

void BatchCommandProcessor::ProcessCommand(const Command& aCommand)
{
    mCommandBatch.push_back(aCommand);

    if (!mBlockForced && mCommandBatch.size() >= mBulkSize)
    {
        DumpBatch();
    }

    mLastCommandTimestamp = std::chrono::system_clock::now();
}

void BatchCommandProcessor::Stop()
{
    if (!mBlockForced)
        DumpBatch();
    CommandProcessor::Stop();
    mDone = true;
}

void BatchCommandProcessor::ClearBatch()
{
    mCommandBatch.clear();
}

void BatchCommandProcessor::DumpBatch()
{
    std::unique_lock<std::mutex> lk(mBatchMutex);

    if (mCommandBatch.empty())
        return;
    CommandBatch commandBatch{mCommandBatch, mCommandBatch[0].mTimestamp};
    CommandProcessor::ProcessBatch(commandBatch);
    ClearBatch();
}

void BatchCommandProcessor::ThreadProc()
{
    try
    {
        while (!mDone.load())
        {
            auto timestamp = std::chrono::system_clock::now();

            if (mLastCommandTimestamp && timestamp - *mLastCommandTimestamp > std::chrono::milliseconds{400})
                DumpBatch();

            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}






////////////////////////////////////////////////////////////////////
///
///                      InputProcessor
///
////////////////////////////////////////////////////////////////////


InputProcessor::InputProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors)
    : CommandProcessor(aName, aDependentCommandProcessors)
{
    assert(aDependentCommandProcessors.size() != 0);
}

void InputProcessor::ProcessLine(const std::string& aLine)
{
    if (aLine == "{")
    {
        if (mBlockDepth++ == 0)
        {
#ifdef DEBUG_CONS
            std::cout << "InputProcessor::ProcessLine, startBlock, line = " << aLine << std::endl;
#endif
            CommandProcessor::StartBlock();
        }
    }
    else if (aLine == "}")
    {
        if (--mBlockDepth == 0)
        {
#ifdef DEBUG_CONS
            std::cout << "InputProcessor::ProcessLine, finishBlock, line = " << aLine << std::endl;
#endif
            CommandProcessor::FinishBlock();
        }
    }
    else
    {
        Command command{aLine, std::chrono::system_clock::now()};
#ifdef DEBUG_CONS
        std::cout << "InputProcessor::ProcessCommand, line = \'" << aLine << "\', length = " << aLine.length() << std::endl;
#endif
        CommandProcessor::ProcessCommand(command);
    }
    CommandProcessor::ProcessLine(aLine);
}


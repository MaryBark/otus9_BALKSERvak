#include "OutPuts2Console.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

OutPuts2Console::OutPuts2Console(const std::string& aName)
    : CommandProcessor(aName)
{}

OutPuts2Console::~OutPuts2Console()
{}

void OutPuts2Console::ProcessBatch(const CommandBatch& aCommandBatch)
{
    Command command = MakeCommandFromBatch(aCommandBatch);

    std::cout << command.mText << std::endl;

    CommandProcessor::ProcessBatch(aCommandBatch);
}

static std::string Join(const std::vector<Command>& v)
{
    std::stringstream ss;
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(i != 0)
            ss << ", ";
        ss << v[i].mText;
    }
    return ss.str();
}

Command MakeCommandFromBatch(const CommandBatch& aCommandBatch)
{
    std::string output = "bulk: " + Join(aCommandBatch.mCommands);
    Command command{output, aCommandBatch.mTimestamp};
    return command;
}


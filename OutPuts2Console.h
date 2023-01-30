#ifndef OUTPUTS2CONSOLE_H
#define OUTPUTS2CONSOLE_H

#include "AllProzessors.h"

/// божественная функция
Command MakeCommandFromBatch(const CommandBatch& aCommandBatch);


class OutPuts2Console : public CommandProcessor
{
public:
    OutPuts2Console(const std::string& aName);
    ~OutPuts2Console();

    void ProcessBatch(const CommandBatch& aCommandBatch) override;
};

#endif // OUTPUTS2CONSOLE_H

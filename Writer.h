#ifndef WRITER_H
#define WRITER_H

#pragma once

#include <string>

#include "AllProzessors.h"

class Writer : public CommandProcessor
{
public:
    Writer(const std::string& aName);

    void ProcessBatch(const CommandBatch& aCommandBatch) override;

private:
    std::string GetFilename(const Command& aCommand);

    static const char Separator = '-';

    int mBatchCounter;
};


#endif // WRITER_H

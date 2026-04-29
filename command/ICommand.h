#pragma once
#include <string>
using namespace std;

/**
 * ICommand - Command Pattern Interface
 * ------------------------------------
 * Wraps each high-level kiosk operation in a command object so execution
 * and logging can be handled consistently.
 */
class ICommand
{
public:
    virtual bool execute() = 0;
    virtual string getName() const = 0;
    virtual ~ICommand() = default;
};

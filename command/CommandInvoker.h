#pragma once
#include "ICommand.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/**
 * CommandInvoker - Command Pattern Invoker
 * ----------------------------------------
 * Executes commands and maintains a lightweight history for diagnostics.
 */
class CommandInvoker
{
    vector<string> executionLog;

public:
    bool executeCommand(ICommand &command)
    {
        cout << "[CommandInvoker] Executing command: "
             << command.getName() << endl;

        bool success = command.execute();
        executionLog.push_back(command.getName() +
                               string(success ? " -> SUCCESS" : " -> FAILED"));

        cout << "[CommandInvoker] Result: "
             << (success ? "SUCCESS" : "FAILED") << endl;
        return success;
    }

    const vector<string> &getExecutionLog() const
    {
        return executionLog;
    }
};

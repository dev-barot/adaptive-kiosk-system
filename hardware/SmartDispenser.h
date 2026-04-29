#pragma once
#include "IDispenser.h"
#include <chrono>
#include <iostream>
#include <thread>
using namespace std;

/**
 * SmartDispenser - Simulated Hardware Device
 * ------------------------------------------
 * Supports clean success, recoverable failure, and fatal failure flows.
 */
class SmartDispenser : public IDispenser
{
    string currentFailureMode = "NONE";

public:
    bool dispense(const string &itemId,
                  int qty,
                  const string &failureMode = "NONE",
                  int responseDelayMs = 0,
                  int timeoutMs = 3000) override
    {
        currentFailureMode = failureMode;
        cout << "[SmartDispenser] Dispensing '" << itemId
             << "' x" << qty << endl;

        if (responseDelayMs > 0)
        {
            cout << "[SmartDispenser] Simulating hardware response delay: "
                 << responseDelayMs << "ms" << endl;
            this_thread::sleep_for(chrono::milliseconds(responseDelayMs));
            if (responseDelayMs > timeoutMs)
            {
                currentFailureMode = "TIMEOUT";
                cout << "[SmartDispenser] Timed out after "
                     << timeoutMs << "ms." << endl;
                return false;
            }
        }

        if (failureMode == "NONE")
        {
            cout << "[SmartDispenser] Dispense completed successfully." << endl;
            return true;
        }

        if (failureMode == "RECOVERABLE")
        {
            cout << "[SmartDispenser] Jam detected. Recovery may succeed." << endl;
            return false;
        }

        cout << "[SmartDispenser] Critical hardware fault detected." << endl;
        return false;
    }

    bool retryDispense(const string &itemId, int qty) override
    {
        cout << "[SmartDispenser] Retrying dispense for '" << itemId
             << "' x" << qty << endl;

        if (currentFailureMode == "RECOVERABLE")
        {
            currentFailureMode = "NONE";
            cout << "[SmartDispenser] Retry succeeded." << endl;
            return true;
        }

        cout << "[SmartDispenser] Retry failed." << endl;
        return false;
    }

    bool recalibrate() override
    {
        cout << "[SmartDispenser] Recalibrating hardware..." << endl;

        if (currentFailureMode == "FATAL")
        {
            cout << "[SmartDispenser] Recalibration failed - technician needed." << endl;
            return false;
        }

        cout << "[SmartDispenser] Recalibration completed." << endl;
        return true;
    }
};

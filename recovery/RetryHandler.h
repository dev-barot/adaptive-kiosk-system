#pragma once
#include "IHardwareHandler.h"
#include <iostream>
using namespace std;

class RetryHandler : public IHardwareHandler
{
public:
    bool handle(IDispenser *dispenser,
                const string &itemId,
                int qty) override
    {
        cout << "[RetryHandler] Attempting automatic retry..." << endl;
        if (dispenser->retryDispense(itemId, qty))
        {
            cout << "[RetryHandler] Failure resolved by retry." << endl;
            return true;
        }

        if (next != nullptr)
            return next->handle(dispenser, itemId, qty);
        return false;
    }
};

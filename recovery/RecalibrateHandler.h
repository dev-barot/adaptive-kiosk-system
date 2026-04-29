#pragma once
#include "IHardwareHandler.h"
#include <iostream>
using namespace std;

class RecalibrateHandler : public IHardwareHandler
{
public:
    bool handle(IDispenser *dispenser,
                const string &itemId,
                int qty) override
    {
        cout << "[RecalibrateHandler] Attempting recalibration..." << endl;
        if (dispenser->recalibrate() && dispenser->retryDispense(itemId, qty))
        {
            cout << "[RecalibrateHandler] Failure resolved after recalibration." << endl;
            return true;
        }

        if (next != nullptr)
            return next->handle(dispenser, itemId, qty);
        return false;
    }
};

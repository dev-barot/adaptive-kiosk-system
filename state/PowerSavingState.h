#pragma once
#include "IKioskState.h"
#include "../pricing/DiscountPricing.h"
#include <iostream>
using namespace std;

/**
 * PowerSavingState - State Pattern (Concrete State)
 * --------------------------------------------------
 * Low-activity mode. Purchases allowed but capped at 3 units.
 * Uses DiscountPricing (20% off) to encourage clearing stock.
 *
 * Pattern: State (Concrete State)
 */
class PowerSavingState : public IKioskState
{
    DiscountPricing pricing;
    static const int MAX_QTY = 3;

public:
    bool handle(Kiosk *kiosk, const string &itemId, int qty) override
    {
        if (qty > MAX_QTY)
        {
            cout << "[PowerSavingState] Purchase of " << qty
                 << " units DENIED - limit is " << MAX_QTY
                 << " per transaction in power-saving mode." << endl;
            return false;
        }
        cout << "[PowerSavingState] Purchase request for '" << itemId
             << "' x" << qty << " - ALLOWED (20% discount applied)" << endl;
        return true;
    }

    IPricingStrategy *getPricingPolicy() override { return &pricing; }
    string getStateName() const override { return "POWER_SAVING"; }
};

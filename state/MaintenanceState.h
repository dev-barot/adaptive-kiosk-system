#pragma once
#include "IKioskState.h"
#include "../pricing/StandardPricing.h"
#include <iostream>
using namespace std;

/**
 * MaintenanceState - State Pattern (Concrete State)
 * --------------------------------------------------
 * Hardware maintenance mode. All purchases are blocked.
 * Pricing is irrelevant but returns StandardPricing as a safe default.
 *
 * Pattern: State (Concrete State)
 */
class MaintenanceState : public IKioskState
{
    StandardPricing pricing;

public:
    bool handle(Kiosk *kiosk, const string &itemId, int qty) override
    {
        cout << "[MaintenanceState] ALL purchases BLOCKED - kiosk under maintenance." << endl;
        return false;
    }

    IPricingStrategy *getPricingPolicy() override { return &pricing; }
    string getStateName() const override { return "MAINTENANCE"; }
};

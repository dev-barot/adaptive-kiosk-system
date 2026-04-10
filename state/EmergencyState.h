#pragma once
#include "IKioskState.h"
#include "../pricing/EmergencyPricing.h"
#include <iostream>
using namespace std;

/**
 * EmergencyState - State Pattern (Concrete State)
 * ------------------------------------------------
 * Emergency lockdown mode. Enforces strict purchase limits to prevent
 * hoarding. Uses EmergencyPricing (50% off) for essential relief items.
 *
 * Path A Requirement: "During emergency mode, a user should not be able
 * to purchase more than a limited quantity of essential items."
 *
 * Pattern: State (Concrete State)
 */
class EmergencyState : public IKioskState
{
    EmergencyPricing pricing;
    static const int EMERGENCY_LIMIT = 2; // max 2 units per transaction

public:
    bool handle(Kiosk *kiosk, const string &itemId, int qty) override
    {
        if (qty > EMERGENCY_LIMIT)
        {
            cout << "[EmergencyState] Purchase of " << qty
                 << " units DENIED - emergency limit is "
                 << EMERGENCY_LIMIT << " units per transaction." << endl;
            return false;
        }
        cout << "[EmergencyState] Purchase of '" << itemId << "' x" << qty
             << " - ALLOWED at 50% emergency price." << endl;
        return true;
    }

    IPricingStrategy *getPricingPolicy() override { return &pricing; }
    string getStateName() const override { return "EMERGENCY"; }
};

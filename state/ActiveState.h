#pragma once
#include "IKioskState.h"
#include "../pricing/StandardPricing.h"
#include <iostream>
using namespace std;

/**
 * ActiveState - State Pattern (Concrete State)
 * ---------------------------------------------
 * Normal operating mode. No purchase restrictions.
 * Uses StandardPricing — full price for all items.
 *
 * Pattern: State (Concrete State)
 * Transitions to: EmergencyState (on emergency event),
 *                 MaintenanceState (on hardware failure),
 *                 PowerSavingState (on low-activity trigger)
 */
class ActiveState : public IKioskState
{
    StandardPricing pricing;

public:
    bool handle(Kiosk *kiosk, const string &itemId, int qty) override
    {
        // Active mode: no purchase restrictions beyond stock availability
        cout << "[ActiveState] Purchase request for '" << itemId
             << "' x" << qty << " - ALLOWED (no restrictions)" << endl;
        return true;
    }

    IPricingStrategy *getPricingPolicy() override
    {
        return &pricing;
    }

    string getStateName() const override { return "ACTIVE"; }
};

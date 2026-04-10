#pragma once
#include <string>
#include "../pricing/IPricingStrategy.h"
using namespace std;

// Forward declaration — avoids circular dependency between Kiosk and its states
class Kiosk;

/**
 * IKioskState - State Pattern (Abstract State)
 * ---------------------------------------------
 * Defines the contract for all kiosk operational modes.
 * Each concrete state controls:
 *   - how purchase requests are handled (purchase limits, restrictions)
 *   - which pricing policy is active in that mode
 *
 * Pattern: State (Abstract State)
 * Context: Kiosk
 * Concrete States: ActiveState, PowerSavingState, MaintenanceState, EmergencyState
 *
 * Design note: getPricingPolicy() ties the State Pattern to the Strategy Pattern.
 * When the kiosk transitions states, the pricing strategy changes automatically —
 * no conditional logic needed in the Kiosk class.
 */
class IKioskState
{
public:
    /**
     * Handle a user purchase request in this state.
     * Each state enforces its own purchase rules and restrictions.
     * @param kiosk   - the context (passed for state transitions)
     * @param itemId  - product being purchased
     * @param qty     - quantity requested
     * @return true if purchase is allowed in this state
     */
    virtual bool handle(Kiosk *kiosk, const string &itemId, int qty) = 0;

    /**
     * Returns the pricing strategy active in this state.
     * Kiosk delegates pricing to the current state automatically.
     */
    virtual IPricingStrategy *getPricingPolicy() = 0;

    /**
     * Human-readable name of this state (for logging and display).
     */
    virtual string getStateName() const = 0;

    virtual ~IKioskState() = default;
};

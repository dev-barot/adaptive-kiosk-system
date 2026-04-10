/**
 * =============================================================================
 * AURA RETAIL OS - Subtask 2 Simulation
 * Group 5: Lazy Constructors | PATH A: Adaptive Autonomous System
 * =============================================================================
 *
 * Patterns demonstrated in this simulation:
 *
 *  1. STATE PATTERN       - Kiosk transitions: Active → PowerSaving → Emergency → Maintenance
 *                           Each state enforces its own rules and pricing automatically.
 *
 *  2. OBSERVER PATTERN    - EventManager broadcasts events to InventoryMonitor,
 *                           CityMonitoringCenter, and MaintenanceService.
 *                           Subsystems react independently - zero direct coupling.
 *
 *  3. STRATEGY PATTERN    - IPricingStrategy switches automatically with state.
 *                           Standard (x1.0) → Discount (x0.8) → Emergency (x0.5).
 *
 *  4. COMPOSITE PATTERN   - ProductBundle contains IndividualProducts (and other
 *                           bundles). calculateFinalPrice() recurses transparently.
 *
 *  5. SINGLETON PATTERN   - CentralRegistry: single global config instance,
 *                           verified by calling getInstance() twice.
 *
 * Simulation Scenarios (Path A requirements):
 *   A. Normal purchase in Active mode
 *   B. Mode switch to PowerSaving - discount pricing, qty limit enforced
 *   C. Emergency mode activated - 50% pricing, strict qty cap, event broadcast
 *   D. Over-limit purchase attempt in Emergency mode - denied
 *   E. Out-of-stock scenario - derived stock check fires LowStockEvent
 *   F. Maintenance mode - all purchases blocked, HardwareFailureEvent fired
 *   G. Composite bundle price calculated across pricing strategies
 * =============================================================================
 */

#include <iostream>
using namespace std;

// Infrastructure
#include "core/CentralRegistry.h"

// Observer Pattern
#include "observer/EventManager.h"
#include "observer/Subscribers.h"

// Inventory
#include "inventory/ReservationManager.h"
#include "inventory/InventoryManager.h"

// State Pattern
#include "state/ActiveState.h"
#include "state/PowerSavingState.h"
#include "state/EmergencyState.h"
#include "state/MaintenanceState.h"

// Kiosk (State context)
#include "core/Kiosk.h"

// Product (Composite + Strategy)
#include "product/IndividualProduct.h"
#include "product/ProductBundle.h"
#include "pricing/StandardPricing.h"
#include "pricing/DiscountPricing.h"
#include "pricing/EmergencyPricing.h"

// ─────────────────────────────────────────────────────────────
void printSeparator(const string &title)
{
    cout << "\n=============================================" << endl;
    cout << "  " << title << endl;
    cout << "=============================================" << endl;
}
// ─────────────────────────────────────────────────────────────

int main()
{
    cout << "=============================================" << endl;
    cout << "       AURA RETAIL OS - SUBTASK 2 DEMO         " << endl;
    cout << "  PATH A: Adaptive Autonomous System           " << endl;
    cout << "  Group 5: Lazy Constructors                   " << endl;
    cout << "=============================================" << endl;

    // =========================================================
    // SINGLETON: CentralRegistry - verify single instance
    // =========================================================
    printSeparator("PATTERN: Singleton - CentralRegistry");

    CentralRegistry *reg1 = CentralRegistry::getInstance();
    CentralRegistry *reg2 = CentralRegistry::getInstance();

    cout << "reg1 address: " << reg1 << endl;
    cout << "reg2 address: " << reg2 << endl;
    cout << "Same instance: " << (reg1 == reg2 ? "YES " : "NO ") << endl;
    cout << "System mode  : " << reg1->getConfig("system_mode") << endl;
    cout << "Kiosk version: " << reg1->getConfig("kiosk_version") << endl;

    // =========================================================
    // OBSERVER: Wire up EventManager with subscribers
    // =========================================================
    printSeparator("PATTERN: Observer - EventManager Setup");

    EventManager eventManager;
    InventoryMonitor    invMonitor;
    CityMonitoringCenter cityMonitor;
    MaintenanceService  maintService;

    // Subscribe each observer to relevant event types
    eventManager.subscribe("LowStockEvent",          &invMonitor);
    eventManager.subscribe("TransactionCompleted",   &invMonitor);
    eventManager.subscribe("EmergencyModeActivated", &cityMonitor);
    eventManager.subscribe("HardwareFailureEvent",   &cityMonitor);
    eventManager.subscribe("HardwareFailureEvent",   &maintService);
    eventManager.subscribe("TransactionCompleted",   &cityMonitor);
    eventManager.subscribe("PurchaseDenied",         &cityMonitor);

    cout << "Subscribers registered:" << endl;
    cout << "  InventoryMonitor     -> LowStockEvent, TransactionCompleted" << endl;
    cout << "  CityMonitoringCenter -> EmergencyModeActivated, HardwareFailureEvent, TransactionCompleted, PurchaseDenied" << endl;
    cout << "  MaintenanceService   -> HardwareFailureEvent" << endl;

    // =========================================================
    // INVENTORY: Setup with ReservationManager composition
    // =========================================================
    printSeparator("Inventory Initialization");

    ReservationManager reservation;
    InventoryManager   inventory(&reservation);   // composed - derived stock = raw - reserved

    inventory.addStock("water",     10);
    inventory.addStock("food",       5);
    inventory.addStock("medkit",     3);
    inventory.addStock("battery",    2);

    cout << "Stock initialized:" << endl;
    cout << "  water  : " << inventory.getDerivedStock("water")   << " units" << endl;
    cout << "  food   : " << inventory.getDerivedStock("food")    << " units" << endl;
    cout << "  medkit : " << inventory.getDerivedStock("medkit")  << " units" << endl;
    cout << "  battery: " << inventory.getDerivedStock("battery") << " units" << endl;

    // =========================================================
    // STATE: Create state objects
    // =========================================================
    ActiveState      activeState;
    PowerSavingState powerSavingState;
    EmergencyState   emergencyState;
    MaintenanceState maintenanceState;

    // =========================================================
    // KIOSK: Start in ActiveState
    // =========================================================
    printSeparator("Kiosk Created - ActiveState");

    Kiosk kiosk("KIOSK-01", &activeState, &eventManager, &inventory, &reservation);

    // =========================================================
    // SCENARIO A: Normal purchase in Active mode
    // =========================================================
    printSeparator("SCENARIO A: Normal Purchase (Active Mode)");
    cout << "Attempt: buy 2x water at standard price (base Rs.100 -> Rs.100 each)" << endl;
    kiosk.processUserRequest("water", 2);

    cout << "\nAttempt: buy 1x food" << endl;
    kiosk.processUserRequest("food", 1);

    // =========================================================
    // SCENARIO B: Transition to PowerSaving - discount pricing
    // =========================================================
    printSeparator("SCENARIO B: Power-Saving Mode (Discount Pricing)");
    kiosk.transitionState(&powerSavingState);
    reg1->setConfig("system_mode", "POWER_SAVING");

    cout << "\nAttempt: buy 2x water (should be allowed, 20% discount)" << endl;
    kiosk.processUserRequest("water", 2);

    cout << "\nAttempt: buy 5x water (exceeds PowerSaving limit of 3 - should be denied)" << endl;
    kiosk.processUserRequest("water", 5);

    // =========================================================
    // SCENARIO C: Emergency mode activated - event broadcast
    // =========================================================
    printSeparator("SCENARIO C: Emergency Mode Activated");
    kiosk.transitionState(&emergencyState);
    reg1->setConfig("system_mode", "EMERGENCY");

    cout << "\nAttempt: buy 1x medkit at emergency price (base Rs.100 -> Rs.50)" << endl;
    kiosk.processUserRequest("medkit", 1);

    // =========================================================
    // SCENARIO D: Over-limit purchase in Emergency mode
    // =========================================================
    printSeparator("SCENARIO D: Emergency Limit Enforcement");
    cout << "Attempt: buy 3x food (exceeds emergency limit of 2 - should be denied)" << endl;
    kiosk.processUserRequest("food", 3);

    cout << "\nAttempt: buy 2x food (within emergency limit - should be allowed)" << endl;
    kiosk.processUserRequest("food", 2);

    // =========================================================
    // SCENARIO E: LowStockEvent fires automatically
    // =========================================================
    printSeparator("SCENARIO E: Low Stock Detection & Event");
    cout << "Current battery stock: " << inventory.getDerivedStock("battery") << endl;
    cout << "Attempt: buy 1x battery (stock will hit threshold, firing LowStockEvent)" << endl;
    kiosk.processUserRequest("battery", 1);

    // =========================================================
    // SCENARIO F: Maintenance mode - all purchases blocked
    // =========================================================
    printSeparator("SCENARIO F: Maintenance Mode - All Purchases Blocked");
    kiosk.transitionState(&maintenanceState);
    reg1->setConfig("system_mode", "MAINTENANCE");

    cout << "\nAttempt: buy 1x water (should be blocked in maintenance mode)" << endl;
    kiosk.processUserRequest("water", 1);

    // =========================================================
    // SCENARIO G: Composite Pattern - Bundle price calculation
    // =========================================================
    printSeparator("SCENARIO G: Composite Pattern - Product Bundles");

    StandardPricing stdPricing;
    DiscountPricing discPricing;
    EmergencyPricing emgPricing;

    // Individual products with different strategies
    IndividualProduct p1(50.0f,  &stdPricing);   // ₹50  standard
    IndividualProduct p2(100.0f, &discPricing);  // ₹80  (20% off)
    IndividualProduct p3(200.0f, &emgPricing);   // ₹100 (50% off)

    // Simple bundle
    ProductBundle basicKit;
    basicKit.add(&p1);
    basicKit.add(&p2);
    cout << "BasicKit (p1 standard + p2 discount): Rs."
         << basicKit.calculateFinalPrice() << endl;

    // Nested bundle - bundle inside a bundle
    ProductBundle emergencyKit;
    emergencyKit.add(&basicKit);  // nested bundle
    emergencyKit.add(&p3);
    cout << "EmergencyKit (basicKit + p3 emergency): Rs."
         << emergencyKit.calculateFinalPrice() << endl;
    cout << "  (Demonstrates Composite: nested bundle calculated recursively)" << endl;

    // =========================================================
    // FINAL SUMMARY
    // =========================================================
    printSeparator("SIMULATION COMPLETE");
    cout << "Patterns demonstrated:" << endl;
    cout << "  State Pattern    - 4 states: Active, PowerSaving, Emergency, Maintenance" << endl;
    cout << "  Observer Pattern - EventManager + 3 subscribers, 6 event types" << endl;
    cout << "  Strategy Pattern - Pricing switches automatically with state" << endl;
    cout << "  Composite Pattern- Nested bundles, recursive price calculation" << endl;
    cout << "  Singleton Pattern- CentralRegistry, same instance verified" << endl;
    cout << "\nPath A Requirements met:" << endl;
    cout << "  Dynamic pricing switches at runtime" << endl;
    cout << "  Kiosk operational modes (4 states)" << endl;
    cout << "  Emergency purchase limits enforced" << endl;
    cout << "  Derived stock = raw stock - reservations" << endl;
    cout << "  Event-driven communication (zero direct subsystem coupling)" << endl;
    cout << "  LowStockEvent fired automatically on threshold breach" << endl;
    cout << "  EmergencyModeActivated overrides operations immediately" << endl;

    return 0;
}

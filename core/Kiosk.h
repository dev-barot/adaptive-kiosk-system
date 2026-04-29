#pragma once
#include "../hardware/DiagnosticsService.h"
#include <algorithm>
#include <string>
#include <iostream>
#include "../state/IKioskState.h"
#include "../observer/EventManager.h"
#include "../inventory/InventoryManager.h"
#include "../inventory/ReservationManager.h"
#include "../product/ProductCatalog.h"
using namespace std;

/**
 * Kiosk - State Pattern (Context)
 * --------------------------------
 * The central kiosk controller. Holds a pointer to the current IKioskState
 * and delegates all purchase-handling and pricing decisions to it.
 *
 * When transitionState() is called, the kiosk swaps its state object and
 * publishes an event — no conditionals, no mode flags, no if/else chains.
 *
 * Also integrates with:
 *   - EventManager  (Observer Pattern) — publishes stock and mode events
 *   - InventoryManager / ReservationManager — enforces stock constraints
 *
 * Pattern: State (Context), uses Observer (EventManager)
 */
class Kiosk
{
    string kioskID;
    string operationalStatus;
    IKioskState *currentState;

    EventManager *eventManager;
    InventoryManager *inventory;
    ReservationManager *reservation;
    DiagnosticsService *diagnosticsService;
    ProductCatalog *productCatalog;

public:
    Kiosk(const string &id,
          IKioskState *initialState,
          EventManager *em,
          InventoryManager *inv,
          ReservationManager *res,
          ProductCatalog *catalog,
          DiagnosticsService *diag)
        : kioskID(id),
          operationalStatus("online"),
          currentState(initialState),
          eventManager(em),
          inventory(inv),
          reservation(res),
          productCatalog(catalog),
          diagnosticsService(diag)
    {
        cout << "[Kiosk:" << kioskID << "] Initialized in state: "
             << currentState->getStateName() << endl;
    }

    /**
     * Transition to a new operational state.
     * Publishes an event so all observers are notified immediately.
     *
     * Path A Requirement: "Certain system events (e.g. EmergencyModeActivated)
     * should override normal operations immediately."
     */
    void transitionState(IKioskState *newState)
    {
        string oldName = currentState->getStateName();
        currentState = newState;
        string newName = currentState->getStateName();

        cout << "\n[Kiosk:" << kioskID << "] State transition: "
             << oldName << " --> " << newName << endl;

        // Publish appropriate event based on new state
        if (newName == "EMERGENCY")
            eventManager->publish("EmergencyModeActivated",
                "Kiosk " + kioskID + " entered Emergency Lockdown.");
        else if (newName == "MAINTENANCE")
            eventManager->publish("HardwareFailureEvent",
                "Kiosk " + kioskID + " entered Maintenance mode.");
        else
            eventManager->publish("KioskStateChanged",
                "Kiosk " + kioskID + " transitioned to " + newName);
    }

    /**
     * Process a user purchase request.
     *
     * Full flow:
     *   1. Delegate to current state — checks mode-specific rules
     *   2. Check derived stock (raw stock minus reservations)
     *   3. Reserve units to prevent double-selling
     *   4. Reduce stock
     *   5. Release reservation (consumed by the reduction)
     *   6. Publish events (TransactionCompleted, LowStockEvent if needed)
     *
     * Path A Requirement: "Inventory updates must occur only when a
     * transaction completes successfully."
     */
    bool preparePurchase(const string &itemId, int qty, float &total)
    {
        // First check if the kiosk itself is offline
        if (getOperationalStatus() == "OFFLINE") {
            cout << "[Kiosk:" << kioskID << "] DENIED - kiosk is OFFLINE due to critical failure." << endl;
            return false;
        }

        cout << "\n[Kiosk:" << kioskID << "] Processing purchase: '"
             << itemId << "' x" << qty
             << "  (Mode: " << currentState->getStateName() << ")" << endl;

        // Step 1: State-level gate (purchase limits, mode restrictions)
        if (!currentState->handle(this, itemId, qty))
        {
            eventManager->publish("PurchaseDenied",
                "Item: " + itemId + " Qty: " + to_string(qty) +
                " Reason: State restriction in " + currentState->getStateName());
            return false;
        }

        // Step 2/3: Atomically check derived stock and reserve units.
        // This prevents two concurrent purchases from seeing the same stock.
        if (!inventory->tryReserveAvailable(itemId, qty))
        {
            int derived = inventory->getDerivedStock(itemId);
            cout << "[Kiosk:" << kioskID << "] DENIED - insufficient derived stock. "
                 << "Available: " << derived << ", Requested: " << qty << endl;
            eventManager->publish("PurchaseDenied",
                "Item: " + itemId + " - insufficient stock (" +
                to_string(derived) + " available)");
            return false;
        }

        cout << "[Kiosk:" << kioskID << "] Reserved " << qty
             << " units of '" << itemId << "'." << endl;

        // Step 4: Apply pricing from current state's strategy
        // (In a full system, IndividualProduct would use this strategy directly)
        float basePrice = productCatalog != nullptr
            ? productCatalog->getBasePrice(itemId)
            : 100.0f;
        float unitPrice = currentState->getPricingPolicy()->apply(basePrice);
        total = unitPrice * qty;
        cout << "[Kiosk:" << kioskID << "] Pricing strategy: "
             << currentState->getStateName()
             << " | Unit price: " << unitPrice
             << " | Total: " << total << endl;

        cout << "[Kiosk:" << kioskID << "] Awaiting payment and hardware confirmation." << endl;
        return true;
    }

    void completePreparedPurchase(const string &itemId, int qty, float total)
    {
        // Step 5: Reduce stock permanently
        inventory->reduceStock(itemId, qty);

        // Step 6: Release reservation (consumed)
        reservation->release(itemId, qty);

        cout << "[Kiosk:" << kioskID << "] Purchase SUCCESSFUL. "
             << "Remaining stock: " << inventory->getDerivedStock(itemId) << endl;

        // Step 7: Publish completion event
        eventManager->publish("TransactionCompleted",
            "Item: " + itemId + " Qty: " + to_string(qty) +
            " Total: " + to_string(total));

        // Step 8: Check for low stock and publish warning if needed
        if (inventory->getDerivedStock(itemId) <= 2)
        {
            eventManager->publish("LowStockEvent",
                "'" + itemId + "' stock critically low: " +
                to_string(inventory->getDerivedStock(itemId)) + " units remaining.");
        }
    }

    void rollbackPreparedPurchase(const string &itemId, int qty)
    {
        int reservedQty = reservation->getReserved(itemId);
        int releasable = min(reservedQty, qty);
        if (releasable > 0)
        {
            reservation->release(itemId, releasable);
            cout << "[Kiosk:" << kioskID << "] Rolled back reservation for '"
                 << itemId << "' x" << releasable << endl;
        }
    }

    bool processUserRequest(const string &itemId, int qty)
    {
        float total = 0.0f;
        if (!preparePurchase(itemId, qty, total))
            return false;

        completePreparedPurchase(itemId, qty, total);
        return true;
    }

    /**
     * Updates operational status (e.g. "online", "degraded", "offline").
     * Future: publish KioskStatusChanged event here.
     */
    void updateStatus(const string &status)
    {
        operationalStatus = status;
        cout << "[Kiosk:" << kioskID << "] Status updated to: " << status << endl;
    }

    string getKioskID() const { return kioskID; }
    string getCurrentStateName() const { return currentState->getStateName(); }
    string getOperationalStatus() const 
    {
        if (currentState->getStateName() == "MAINTENANCE")
            return "MAINTENANCE";
            
        // Check the actual critical dispenser module registered by the factories.
        // Product-specific modules are still validated by RuleEngine.
        if (diagnosticsService != nullptr && !diagnosticsService->isModuleReady("STANDARD_DISPENSER"))
            return "OFFLINE";
            
        return operationalStatus; 
    }
    IPricingStrategy *getCurrentPricingPolicy() { return currentState->getPricingPolicy(); }
};

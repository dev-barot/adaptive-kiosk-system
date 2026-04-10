#pragma once
#include <string>
#include <iostream>
#include "../state/IKioskState.h"
#include "../observer/EventManager.h"
#include "../inventory/InventoryManager.h"
#include "../inventory/ReservationManager.h"
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

public:
    Kiosk(const string &id,
          IKioskState *initialState,
          EventManager *em,
          InventoryManager *inv,
          ReservationManager *res)
        : kioskID(id),
          operationalStatus("online"),
          currentState(initialState),
          eventManager(em),
          inventory(inv),
          reservation(res)
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
    void processUserRequest(const string &itemId, int qty)
    {
        cout << "\n[Kiosk:" << kioskID << "] Processing purchase: '"
             << itemId << "' x" << qty
             << "  (Mode: " << currentState->getStateName() << ")" << endl;

        // Step 1: State-level gate (purchase limits, mode restrictions)
        if (!currentState->handle(this, itemId, qty))
        {
            eventManager->publish("PurchaseDenied",
                "Item: " + itemId + " Qty: " + to_string(qty) +
                " Reason: State restriction in " + currentState->getStateName());
            return;
        }

        // Step 2: Stock availability check using derived stock
        int derived = inventory->getDerivedStock(itemId);
        if (derived < qty)
        {
            cout << "[Kiosk:" << kioskID << "] DENIED - insufficient derived stock. "
                 << "Available: " << derived << ", Requested: " << qty << endl;
            eventManager->publish("PurchaseDenied",
                "Item: " + itemId + " — insufficient stock (" +
                to_string(derived) + " available)");
            return;
        }

        // Step 3: Reserve units (concurrency lock)
        reservation->reserve(itemId, qty);
        cout << "[Kiosk:" << kioskID << "] Reserved " << qty
             << " units of '" << itemId << "'." << endl;

        // Step 4: Apply pricing from current state's strategy
        // (In a full system, IndividualProduct would use this strategy directly)
        float unitPrice = currentState->getPricingPolicy()->apply(100.0f); // placeholder base
        float total = unitPrice * qty;
        cout << "[Kiosk:" << kioskID << "] Pricing strategy: "
             << currentState->getStateName()
             << " | Unit price: " << unitPrice
             << " | Total: " << total << endl;

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
    IPricingStrategy *getCurrentPricingPolicy() { return currentState->getPricingPolicy(); }
};

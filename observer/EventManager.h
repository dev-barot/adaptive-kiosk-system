#pragma once
#include "ISubscriber.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

/**
 * EventManager - Observer Pattern (Subject / Event Bus)
 * ------------------------------------------------------
 * Central publish-subscribe bus that decouples event producers from consumers.
 * Any subsystem can publish events; any module can subscribe to specific types.
 *
 * Supported event types (examples):
 *   - "LowStockEvent"           → notify supply chain / monitoring
 *   - "EmergencyModeActivated"  → override normal operations immediately
 *   - "HardwareFailureEvent"    → trigger Chain of Responsibility
 *   - "TransactionCompleted"    → log to CityMonitoringCenter
 *   - "PurchaseDenied"          → log for analytics
 *
 * Pattern: Observer (Subject / Concrete Subject)
 * Future: Add priority queue so EmergencyModeActivated fires before others.
 */
class EventManager
{
    // Map of eventType → list of subscribers
    map<string, vector<ISubscriber *>> subscribers;

public:
    /**
     * Subscribe to a specific event type.
     * @param eventType  - the event to listen for
     * @param subscriber - the handler to call when the event fires
     */
    void subscribe(const string &eventType, ISubscriber *subscriber)
    {
        subscribers[eventType].push_back(subscriber);
    }

    /**
     * Publish an event to all registered subscribers for that type.
     * Subscribers are notified in registration order.
     * @param eventType - the event being fired
     * @param payload   - event data / description string
     */
    void publish(const string &eventType, const string &payload)
    {
        cout << "[EventManager] Publishing '" << eventType << "': " << payload << endl;

        auto it = subscribers.find(eventType);
        if (it == subscribers.end() || it->second.empty())
        {
            cout << "[EventManager] No subscribers for '" << eventType << "'" << endl;
            return;
        }

        for (ISubscriber *sub : it->second)
            sub->onEvent(eventType, payload);
    }
};

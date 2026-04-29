#pragma once
#include "ISubscriber.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

/**
 * EventManager - Observer Pattern (Subject / Event Bus)
 * ------------------------------------------------------
 * Central publish-subscribe bus that decouples event producers from consumers.
 * It also supports priority processing for queued events, allowing emergency
 * events to override normal operational notifications in Path A simulations.
 */
class EventManager
{
    map<string, vector<ISubscriber *>> subscribers;

    struct EventRecord
    {
        string eventType;
        string payload;
        int priority;
        int sequence;
    };

    vector<EventRecord> pendingEvents;
    int nextSequence = 0;

    int priorityFor(const string &eventType) const
    {
        if (eventType == "EmergencyModeActivated")
            return 100;
        if (eventType == "HardwareFailureEvent" || eventType == "RecoveryFailed")
            return 80;
        if (eventType == "PaymentFailed" || eventType == "PaymentRolledBack")
            return 70;
        if (eventType == "LowStockEvent")
            return 50;
        return 10;
    }

public:
    void subscribe(const string &eventType, ISubscriber *subscriber)
    {
        subscribers[eventType].push_back(subscriber);
    }

    void publish(const string &eventType, const string &payload)
    {
        int priority = priorityFor(eventType);
        cout << "[EventManager] Publishing '" << eventType << "' priority="
             << priority << ": " << payload << endl;

        auto it = subscribers.find(eventType);
        if (it == subscribers.end() || it->second.empty())
        {
            cout << "[EventManager] No subscribers for '" << eventType << "'" << endl;
            return;
        }

        for (ISubscriber *sub : it->second)
            sub->onEvent(eventType, payload);
    }

    void queueEvent(const string &eventType, const string &payload)
    {
        int priority = priorityFor(eventType);
        pendingEvents.push_back({eventType, payload, priority, nextSequence++});
        cout << "[EventManager] Queued '" << eventType
             << "' priority=" << priority << endl;
    }

    void processQueuedEvents()
    {
        stable_sort(pendingEvents.begin(), pendingEvents.end(),
                    [](const EventRecord &left, const EventRecord &right)
                    {
                        if (left.priority == right.priority)
                            return left.sequence < right.sequence;
                        return left.priority > right.priority;
                    });

        cout << "[EventManager] Processing " << pendingEvents.size()
             << " queued events by priority." << endl;
        for (const EventRecord &event : pendingEvents)
            publish(event.eventType, event.payload);
        pendingEvents.clear();
    }
};

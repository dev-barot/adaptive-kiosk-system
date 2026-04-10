#pragma once
#include "ISubscriber.h"
#include <iostream>
using namespace std;

/**
 * InventoryMonitor - Observer Pattern (Concrete Observer)
 * --------------------------------------------------------
 * Reacts to stock-level events. In a full system this would
 * trigger automatic reorder requests to the supply chain.
 *
 * Pattern: Observer (Concrete Observer)
 * Subscribes to: "LowStockEvent", "TransactionCompleted"
 */
class InventoryMonitor : public ISubscriber
{
public:
    void onEvent(const string &eventType, const string &payload) override
    {
        if (eventType == "LowStockEvent")
            cout << "  [InventoryMonitor] LOW STOCK ALERT: " << payload
                 << " - triggering reorder request." << endl;
        else if (eventType == "TransactionCompleted")
            cout << "  [InventoryMonitor] Stock log updated: " << payload << endl;
        else
            cout << "  [InventoryMonitor] Event received: " << eventType << endl;
    }
};

/**
 * CityMonitoringCenter - Observer Pattern (Concrete Observer)
 * -----------------------------------------------------------
 * Represents the city-level monitoring infrastructure.
 * Logs all critical events for city-wide situational awareness.
 *
 * Pattern: Observer (Concrete Observer)
 * Subscribes to: "EmergencyModeActivated", "HardwareFailureEvent", "TransactionCompleted"
 */
class CityMonitoringCenter : public ISubscriber
{
public:
    void onEvent(const string &eventType, const string &payload) override
    {
        if (eventType == "EmergencyModeActivated")
            cout << "  [CityMonitor] EMERGENCY ALERT received: " << payload
                 << " - dispatching response team." << endl;
        else if (eventType == "HardwareFailureEvent")
            cout << "  [CityMonitor] Hardware failure logged: " << payload
                 << " - scheduling technician." << endl;
        else
            cout << "  [CityMonitor] Event logged: [" << eventType << "] " << payload << endl;
    }
};

/**
 * MaintenanceService - Observer Pattern (Concrete Observer)
 * ----------------------------------------------------------
 * Reacts to hardware failure events and maintenance triggers.
 *
 * Pattern: Observer (Concrete Observer)
 * Subscribes to: "HardwareFailureEvent", "MaintenanceModeActivated"
 */
class MaintenanceService : public ISubscriber
{
public:
    void onEvent(const string &eventType, const string &payload) override
    {
        cout << "  [MaintenanceService] Maintenance triggered for: "
             << eventType << " | " << payload << endl;
    }
};

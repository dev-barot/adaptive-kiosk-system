#pragma once
#include <string>
using namespace std;

/**
 * ISubscriber - Observer Pattern (Observer Interface)
 * ----------------------------------------------------
 * All event subscribers must implement onEvent().
 * The EventManager calls onEvent() when a matching event is published.
 *
 * Pattern: Observer (Observer / Listener)
 * Used by: InventoryMonitor, CityMonitoringCenter, MaintenanceService
 */
class ISubscriber
{
public:
    /**
     * Called by EventManager when an event this subscriber is registered for fires.
     * @param eventType  - e.g. "LowStockEvent", "EmergencyModeActivated"
     * @param payload    - human-readable event data / description
     */
    virtual void onEvent(const string &eventType, const string &payload) = 0;
    virtual ~ISubscriber() = default;
};

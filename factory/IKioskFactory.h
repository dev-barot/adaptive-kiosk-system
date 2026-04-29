#pragma once
#include "../core/Kiosk.h"
#include "../core/KioskInterface.h"
#include "../hardware/DiagnosticsService.h"
#include "../hardware/HardwareStatus.h"
#include "../inventory/InventoryManager.h"
#include "../inventory/InventoryPolicy.h"
#include "../inventory/ReservationManager.h"
#include "../observer/EventManager.h"
#include "../observer/Subscribers.h"
#include "../product/ProductCatalog.h"
#include "../state/ActiveState.h"
#include "../state/EmergencyState.h"
#include "../state/MaintenanceState.h"
#include "../state/PowerSavingState.h"
#include <memory>
#include <string>
using namespace std;

/**
 * KioskRuntime - Owns all components created for one kiosk instance.
 */
struct KioskRuntime
{
    string kioskType;
    string kioskId;

    unique_ptr<EventManager> eventManager;
    unique_ptr<ReservationManager> reservationManager;
    unique_ptr<InventoryManager> inventoryManager;
    unique_ptr<HardwareStatus> hardwareStatus;
    unique_ptr<DiagnosticsService> diagnosticsService;
    unique_ptr<InventoryPolicy> inventoryPolicy;
    unique_ptr<ProductCatalog> productCatalog;

    unique_ptr<InventoryMonitor> inventoryMonitor;
    unique_ptr<CityMonitoringCenter> cityMonitoringCenter;
    unique_ptr<MaintenanceService> maintenanceService;

    unique_ptr<ActiveState> activeState;
    unique_ptr<PowerSavingState> powerSavingState;
    unique_ptr<EmergencyState> emergencyState;
    unique_ptr<MaintenanceState> maintenanceState;

    unique_ptr<Kiosk> kiosk;
    unique_ptr<KioskInterface> kioskInterface;

    void wireObservers()
    {
        eventManager->subscribe("LowStockEvent", inventoryMonitor.get());
        eventManager->subscribe("TransactionCompleted", inventoryMonitor.get());
        eventManager->subscribe("EmergencyModeActivated", cityMonitoringCenter.get());
        eventManager->subscribe("HardwareFailureEvent", cityMonitoringCenter.get());
        eventManager->subscribe("HardwareFailureEvent", maintenanceService.get());
        eventManager->subscribe("TransactionCompleted", cityMonitoringCenter.get());
        eventManager->subscribe("PurchaseDenied", cityMonitoringCenter.get());
        eventManager->subscribe("TechnicianAlert", cityMonitoringCenter.get());
        eventManager->subscribe("TechnicianAlert", maintenanceService.get());
        eventManager->subscribe("RecoveryFailed", cityMonitoringCenter.get());
        eventManager->subscribe("RecoverySucceeded", cityMonitoringCenter.get());
        eventManager->subscribe("PaymentFailed", cityMonitoringCenter.get());
        eventManager->subscribe("PaymentRolledBack", cityMonitoringCenter.get());
        eventManager->subscribe("InventoryRestocked", inventoryMonitor.get());
        eventManager->subscribe("TransactionRefunded", inventoryMonitor.get());
    }
};

/**
 * IKioskFactory - Abstract Factory for kiosk variants.
 */
class IKioskFactory
{
protected:
    KioskRuntime createBaseRuntime(const string &kioskId,
                                   const string &kioskType) const
    {
        KioskRuntime runtime;
        runtime.kioskId = kioskId;
        runtime.kioskType = kioskType;

        runtime.eventManager = make_unique<EventManager>();
        runtime.reservationManager = make_unique<ReservationManager>();
        runtime.inventoryManager = make_unique<InventoryManager>(runtime.reservationManager.get());
        runtime.hardwareStatus = make_unique<HardwareStatus>();
        runtime.diagnosticsService = make_unique<DiagnosticsService>(runtime.hardwareStatus.get());
        runtime.inventoryPolicy = make_unique<InventoryPolicy>(kioskType + "Policy");
        runtime.productCatalog = make_unique<ProductCatalog>();

        runtime.inventoryMonitor = make_unique<InventoryMonitor>();
        runtime.cityMonitoringCenter = make_unique<CityMonitoringCenter>();
        runtime.maintenanceService = make_unique<MaintenanceService>();

        runtime.activeState = make_unique<ActiveState>();
        runtime.powerSavingState = make_unique<PowerSavingState>();
        runtime.emergencyState = make_unique<EmergencyState>();
        runtime.maintenanceState = make_unique<MaintenanceState>();

        runtime.kiosk = make_unique<Kiosk>(kioskId,
                                           runtime.activeState.get(),
                                           runtime.eventManager.get(),
                                           runtime.inventoryManager.get(),
                                           runtime.reservationManager.get(),
                                           runtime.productCatalog.get(),
                                           runtime.diagnosticsService.get());

        runtime.wireObservers();
        return runtime;
    }

public:
    virtual KioskRuntime create(const string &kioskId,
                                const string &inventoryFile,
                                const string &transactionFile) const = 0;
    virtual string getFactoryName() const = 0;
    virtual ~IKioskFactory() = default;
};

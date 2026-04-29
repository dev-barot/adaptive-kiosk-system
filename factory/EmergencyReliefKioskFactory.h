#pragma once
#include "IKioskFactory.h"
#include "../persistence/InventoryRepository.h"

class EmergencyReliefKioskFactory : public IKioskFactory
{
public:
    KioskRuntime create(const string &kioskId,
                        const string &inventoryFile,
                        const string &transactionFile) const override
    {
        KioskRuntime runtime = createBaseRuntime(kioskId, "EmergencyReliefKiosk");

        runtime.hardwareStatus->setModuleStatus("STANDARD_DISPENSER", true);
        runtime.hardwareStatus->setModuleStatus("BATTERY_BIN", true);
        runtime.hardwareStatus->setModuleStatus("REFRIGERATION", false);

        runtime.productCatalog->registerProduct({"water", "Emergency Water Pack", 50.0f, "STANDARD_DISPENSER", true});
        runtime.productCatalog->registerProduct({"food", "Dry Food Ration", 110.0f, "STANDARD_DISPENSER", true});
        runtime.productCatalog->registerProduct({"medkit", "Emergency Medkit", 180.0f, "STANDARD_DISPENSER", true});
        runtime.productCatalog->registerProduct({"battery", "Power Cell", 75.0f, "BATTERY_BIN", true});

        runtime.inventoryPolicy->allowItem("water");
        runtime.inventoryPolicy->allowItem("food");
        runtime.inventoryPolicy->allowItem("medkit");
        runtime.inventoryPolicy->allowItem("battery");

        InventoryRepository(inventoryFile).load(runtime.inventoryManager.get(), runtime.productCatalog.get());

        runtime.kioskInterface = make_unique<KioskInterface>(runtime.kiosk.get(),
                                                             runtime.inventoryManager.get(),
                                                             runtime.reservationManager.get(),
                                                             runtime.eventManager.get(),
                                                             runtime.productCatalog.get(),
                                                             runtime.inventoryPolicy.get(),
                                                             runtime.diagnosticsService.get(),
                                                             transactionFile,
                                                             inventoryFile);
        return runtime;
    }

    string getFactoryName() const override
    {
        return "EmergencyReliefKioskFactory";
    }
};

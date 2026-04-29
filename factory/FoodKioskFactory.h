#pragma once
#include "IKioskFactory.h"
#include "../persistence/InventoryRepository.h"

class FoodKioskFactory : public IKioskFactory
{
public:
    KioskRuntime create(const string &kioskId,
                        const string &inventoryFile,
                        const string &transactionFile) const override
    {
        KioskRuntime runtime = createBaseRuntime(kioskId, "FoodKiosk");

        runtime.hardwareStatus->setModuleStatus("STANDARD_DISPENSER", true);
        runtime.hardwareStatus->setModuleStatus("REFRIGERATION", false);

        runtime.productCatalog->registerProduct({"water", "Water Bottle", 40.0f, "STANDARD_DISPENSER", true});
        runtime.productCatalog->registerProduct({"food", "Meal Pack", 120.0f, "STANDARD_DISPENSER", true});
        runtime.productCatalog->registerProduct({"snack", "Campus Snack Box", 85.0f, "STANDARD_DISPENSER", false});

        runtime.inventoryPolicy->allowItem("water");
        runtime.inventoryPolicy->allowItem("food");
        runtime.inventoryPolicy->allowItem("snack");

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
        return "FoodKioskFactory";
    }
};

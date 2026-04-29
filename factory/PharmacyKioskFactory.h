#pragma once
#include "IKioskFactory.h"
#include "../persistence/InventoryRepository.h"

class PharmacyKioskFactory : public IKioskFactory
{
public:
    KioskRuntime create(const string &kioskId,
                        const string &inventoryFile,
                        const string &transactionFile) const override
    {
        KioskRuntime runtime = createBaseRuntime(kioskId, "PharmacyKiosk");

        runtime.hardwareStatus->setModuleStatus("STANDARD_DISPENSER", true);
        runtime.hardwareStatus->setModuleStatus("REFRIGERATION", true);

        runtime.productCatalog->registerProduct({"painkiller", "Painkiller Pack", 140.0f, "STANDARD_DISPENSER", true});
        runtime.productCatalog->registerProduct({"insulin", "Insulin Kit", 340.0f, "REFRIGERATION", true});
        runtime.productCatalog->registerProduct({"medkit", "Medical Relief Kit", 220.0f, "STANDARD_DISPENSER", true});

        runtime.inventoryPolicy->allowItem("painkiller", true);
        runtime.inventoryPolicy->allowItem("insulin", true);
        runtime.inventoryPolicy->allowItem("medkit", true);

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
        return "PharmacyKioskFactory";
    }
};

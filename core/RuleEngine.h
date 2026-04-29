#pragma once
#include "Kiosk.h"
#include "../hardware/DiagnosticsService.h"
#include "../inventory/InventoryManager.h"
#include "../inventory/InventoryPolicy.h"
#include "../product/ProductCatalog.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * RuleEngine - Validation Layer
 * -----------------------------
 * Performs general validation before a request becomes an executable command.
 * State-specific restrictions remain owned by the State Pattern classes.
 */
class RuleEngine
{
public:
    bool validatePurchase(Kiosk *kiosk,
                          InventoryManager *inventory,
                          ProductCatalog *catalog,
                          InventoryPolicy *policy,
                          DiagnosticsService *diagnostics,
                          const string &itemId,
                          int qty) const
    {
        if (kiosk == nullptr || inventory == nullptr || catalog == nullptr ||
            policy == nullptr || diagnostics == nullptr)
        {
            cout << "[RuleEngine] Purchase denied - missing system dependency." << endl;
            return false;
        }

        if (qty <= 0)
        {
            cout << "[RuleEngine] Purchase denied - quantity must be positive." << endl;
            return false;
        }

        if (!catalog->hasProduct(itemId))
        {
            cout << "[RuleEngine] Purchase denied - unknown catalog item '"
                 << itemId << "'." << endl;
            return false;
        }

        if (!policy->isAllowed(itemId))
        {
            cout << "[RuleEngine] Purchase denied - item '" << itemId
                 << "' not supported by policy " << policy->getPolicyName() << "." << endl;
            return false;
        }

        string requiredHardware = catalog->getRequiredHardware(itemId);
        if (!diagnostics->canDispenseProduct(requiredHardware))
        {
            cout << "[RuleEngine] Purchase denied - required hardware module '"
                 << requiredHardware << "' is unavailable." << endl;
            return false;
        }

        int available = inventory->getDerivedStock(itemId);
        if (available <= 0)
        {
            cout << "[RuleEngine] Purchase denied - no stock available for '"
                 << itemId << "'." << endl;
            return false;
        }

        if (available < qty)
        {
            cout << "[RuleEngine] Purchase denied - requested qty " << qty
                 << " exceeds derived stock " << available << "." << endl;
            return false;
        }

        cout << "[RuleEngine] Purchase passed validation for '"
             << itemId << "' x" << qty << "." << endl;
        return true;
    }

    bool validateRefund(const string &itemId, int qty) const
    {
        if (itemId.empty())
        {
            cout << "[RuleEngine] Refund denied - item id is missing." << endl;
            return false;
        }

        if (qty <= 0)
        {
            cout << "[RuleEngine] Refund denied - quantity must be positive." << endl;
            return false;
        }

        cout << "[RuleEngine] Refund passed validation for '"
             << itemId << "' x" << qty << "." << endl;
        return true;
    }

    bool validateRestock(const string &itemId, int qty) const
    {
        if (itemId.empty())
        {
            cout << "[RuleEngine] Restock denied - item id is missing." << endl;
            return false;
        }

        if (qty <= 0)
        {
            cout << "[RuleEngine] Restock denied - quantity must be positive." << endl;
            return false;
        }

        cout << "[RuleEngine] Restock passed validation for '"
             << itemId << "' x" << qty << "." << endl;
        return true;
    }
};

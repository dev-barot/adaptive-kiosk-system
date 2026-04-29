#pragma once
#include "TransactionManager.h"
#include "../command/CommandInvoker.h"
#include "../command/PurchaseItemCommand.h"
#include "../command/RefundCommand.h"
#include "../command/RestockCommand.h"
#include <vector>

/**
 * KioskInterface - Facade Pattern
 * -------------------------------
 * External callers interact with this simplified API instead of reaching
 * directly into kiosk internals.
 */
class KioskInterface
{
    RuleEngine ruleEngine;
    TransactionManager transactionManager;
    CommandInvoker commandInvoker;

public:
    KioskInterface(Kiosk *kiosk,
                   InventoryManager *inventory,
                   ReservationManager *reservation,
                   EventManager *eventManager,
                   ProductCatalog *productCatalog,
                   InventoryPolicy *inventoryPolicy,
                   DiagnosticsService *diagnosticsService,
                   const string &transactionFile,
                   const string &inventoryFile)
        : transactionManager(kiosk,
                             inventory,
                             reservation,
                             eventManager,
                             &ruleEngine,
                             productCatalog,
                             inventoryPolicy,
                             diagnosticsService,
                             transactionFile,
                             inventoryFile) {}

    bool purchaseItem(const string &itemId,
                      int qty,
                      const string &paymentMethod = "UPI",
                      const string &failureMode = "NONE",
                      bool simulatePaymentFailure = false,
                      int hardwareDelayMs = 0,
                      int hardwareTimeoutMs = 3000)
    {
        PurchaseItemCommand command(&transactionManager,
                                    itemId,
                                    qty,
                                    paymentMethod,
                                    failureMode,
                                    simulatePaymentFailure,
                                    hardwareDelayMs,
                                    hardwareTimeoutMs);
        return commandInvoker.executeCommand(command);
    }

    bool refundTransaction(const string &itemId, int qty)
    {
        RefundCommand command(&transactionManager, itemId, qty);
        return commandInvoker.executeCommand(command);
    }

    bool restockInventory(const string &itemId, int qty)
    {
        RestockCommand command(&transactionManager, itemId, qty);
        return commandInvoker.executeCommand(command);
    }

    void runDiagnostics()
    {
        transactionManager.runDiagnostics();
    }

    const vector<string> &getCommandHistory() const
    {
        return commandInvoker.getExecutionLog();
    }

    const vector<string> &getTransactionHistory() const
    {
        return transactionManager.getTransactionLog();
    }
};

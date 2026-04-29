#pragma once
#include "RuleEngine.h"
#include "../memento/TransactionCaretaker.h"
#include "../memento/TransactionMemento.h"
#include "../payment/PaymentManager.h"
#include "../persistence/InventoryRepository.h"
#include "../persistence/TransactionRepository.h"
#include "../hardware/SmartDispenser.h"
#include "../hardware/DiagnosticsService.h"
#include "../recovery/RetryHandler.h"
#include "../recovery/RecalibrateHandler.h"
#include "../recovery/AlertHandler.h"
#include "../inventory/InventoryManager.h"
#include "../inventory/InventoryPolicy.h"
#include "../inventory/ReservationManager.h"
#include "../observer/EventManager.h"
#include "../product/ProductCatalog.h"
#include <atomic>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

/**
 * TransactionManager - Phase 1 Orchestrator
 * -----------------------------------------
 * Coordinates validation, delegates work to the kiosk core, and records
 * a simple transaction history. Later phases can extend this with memento,
 * payment handling, and persistence.
 */
class TransactionManager
{
    Kiosk *kiosk;
    InventoryManager *inventory;
    ReservationManager *reservation;
    EventManager *eventManager;
    RuleEngine *ruleEngine;
    ProductCatalog *productCatalog;
    InventoryPolicy *inventoryPolicy;
    DiagnosticsService *diagnosticsService;
    PaymentManager paymentManager;
    SmartDispenser dispenser;
    RetryHandler retryHandler;
    RecalibrateHandler recalibrateHandler;
    AlertHandler alertHandler;
    TransactionRepository transactionRepository;
    InventoryRepository inventoryRepository;
    vector<string> transactionLog;
    mutable mutex transactionLogMutex;
    inline static atomic<int> nextTransactionNumber {1};

    string createTransactionId() const
    {
        ostringstream id;
        id << "TXN-" << nextTransactionNumber.fetch_add(1);
        return id.str();
    }

    void restoreSnapshot(const TransactionMemento &snapshot)
    {
        inventory->setRawStock(snapshot.getItemId(), snapshot.getRawStock());
        reservation->setReserved(snapshot.getItemId(), snapshot.getReservedStock());
        kiosk->updateStatus(snapshot.getKioskStatus());
        logEntry("Snapshot restored -> Item: " + snapshot.getItemId());
        inventoryRepository.save(inventory, productCatalog);
    }

    void logEntry(const string &entry)
    {
        lock_guard<mutex> lock(transactionLogMutex);
        transactionLog.push_back(entry);
        transactionRepository.append("TransactionManager", "INFO", entry);
        cout << "[TransactionManager] " << entry << endl;
    }

    void auditTransaction(const string &transactionId,
                          const string &operation,
                          const string &status,
                          const string &itemId,
                          int qty,
                          float amount,
                          const string &paymentMethod,
                          const string &phase,
                          const string &reason,
                          const string &details)
    {
        transactionRepository.appendAudit(transactionId,
                                          operation,
                                          status,
                                          itemId,
                                          qty,
                                          amount,
                                          paymentMethod,
                                          phase,
                                          reason,
                                          details);
        cout << "[Audit:" << transactionId << "] "
             << phase << " -> " << status;
        if (!reason.empty())
            cout << " (" << reason << ")";
        cout << endl;
    }

public:
    TransactionManager(Kiosk *k,
                       InventoryManager *inv,
                       ReservationManager *res,
                       EventManager *em,
                       RuleEngine *rules,
                       ProductCatalog *catalog,
                       InventoryPolicy *policy,
                       DiagnosticsService *diagnostics,
                       const string &transactionFile,
                       const string &inventoryFile)
        : kiosk(k),
          inventory(inv),
          reservation(res),
          eventManager(em),
          ruleEngine(rules),
          productCatalog(catalog),
          inventoryPolicy(policy),
          diagnosticsService(diagnostics),
          alertHandler(em),
          transactionRepository(transactionFile),
          inventoryRepository(inventoryFile)
    {
        retryHandler.setNext(&recalibrateHandler);
        recalibrateHandler.setNext(&alertHandler);
        transactionRepository.initialize();
    }

    bool processPurchase(const string &itemId,
                         int qty,
                         const string &paymentMethod,
                         const string &failureMode,
                         bool simulatePaymentFailure,
                         int hardwareDelayMs = 0,
                         int hardwareTimeoutMs = 3000)
    {
        string transactionId = createTransactionId();
        logEntry("Purchase requested -> " + transactionId +
                 " Item: " + itemId +
                 " Qty: " + to_string(qty) +
                 " Payment: " + paymentMethod);
        auditTransaction(transactionId, "Purchase", "STARTED", itemId, qty, 0.0f,
                         paymentMethod, "REQUESTED", "", "Purchase request accepted by facade");

        if (!ruleEngine->validatePurchase(kiosk,
                                          inventory,
                                          productCatalog,
                                          inventoryPolicy,
                                          diagnosticsService,
                                          itemId,
                                          qty))
        {
            auditTransaction(transactionId, "Purchase", "FAILED", itemId, qty, 0.0f,
                             paymentMethod, "VALIDATION_FAILED", "RuleEngine rejected request",
                             "No payment or inventory mutation performed");
            logEntry("Purchase rejected by RuleEngine -> Item: " + itemId);
            return false;
        }
        auditTransaction(transactionId, "Purchase", "PASSED", itemId, qty, 0.0f,
                         paymentMethod, "VALIDATED", "", "All static and derived constraints passed");

        TransactionMemento snapshot(itemId,
                                    inventory->getRawStock(itemId),
                                    reservation->getReserved(itemId),
                                    kiosk->getOperationalStatus());
        logEntry("Snapshot saved before payment and dispense -> Item: " + itemId);
        auditTransaction(transactionId, "Purchase", "SAVED", itemId, qty, 0.0f,
                         paymentMethod, "SNAPSHOT_SAVED", "", "Rollback point captured");

        float total = 0.0f;
        if (!kiosk->preparePurchase(itemId, qty, total))
        {
            auditTransaction(transactionId, "Purchase", "FAILED", itemId, qty, total,
                             paymentMethod, "RESERVATION_FAILED", "State or stock restriction",
                             "Reservation was not created");
            logEntry("Purchase rejected by kiosk core -> Item: " + itemId);
            return false;
        }
        auditTransaction(transactionId, "Purchase", "PASSED", itemId, qty, total,
                         paymentMethod, "RESERVED", "", "Stock reserved atomically");

        if (!paymentManager.processPayment(paymentMethod, total, simulatePaymentFailure))
        {
            eventManager->publish("PaymentFailed",
                                  "Payment failed for '" + itemId +
                                  "' using " + paymentMethod);
            kiosk->rollbackPreparedPurchase(itemId, qty);
            restoreSnapshot(snapshot);
            auditTransaction(transactionId, "Purchase", "ROLLED_BACK", itemId, qty, total,
                             paymentMethod, "PAYMENT_FAILED", "Payment provider failure",
                             "Reservation released and snapshot restored");
            logEntry("Purchase rolled back after payment failure -> Item: " + itemId);
            return false;
        }
        auditTransaction(transactionId, "Purchase", "PASSED", itemId, qty, total,
                         paymentMethod, "PAYMENT_CONFIRMED", "", "Payment adapter approved charge");

        eventManager->publish("PaymentProcessed",
                              "Payment received for '" + itemId +
                              "' using " + paymentMethod);

        if (!dispenser.dispense(itemId, qty, failureMode, hardwareDelayMs, hardwareTimeoutMs))
        {
            eventManager->publish("HardwareFailureEvent",
                                  "Dispense failure while processing '" + itemId + "'.");
            auditTransaction(transactionId, "Purchase", "FAILED", itemId, qty, total,
                             paymentMethod, "DISPENSE_FAILED", failureMode,
                             "Recovery chain started");

            bool recovered = retryHandler.handle(&dispenser, itemId, qty);
            if (!recovered)
            {
                paymentManager.refundPayment(paymentMethod, total);
                eventManager->publish("PaymentRolledBack",
                                      "Refund issued for failed purchase of '" + itemId + "'.");
                kiosk->rollbackPreparedPurchase(itemId, qty);
                restoreSnapshot(snapshot);
                kiosk->updateStatus("degraded");
                eventManager->publish("RecoveryFailed",
                                      "Automatic recovery failed for '" + itemId + "'.");
                auditTransaction(transactionId, "Purchase", "ROLLED_BACK", itemId, qty, total,
                                 paymentMethod, "ROLLBACK_COMPLETE", "Unrecoverable hardware failure",
                                 "Payment refunded; reservation and snapshot restored");
                logEntry("Purchase rolled back after unrecoverable hardware failure -> Item: " + itemId);
                return false;
            }

            eventManager->publish("RecoverySucceeded",
                                  "Automatic recovery completed for '" + itemId + "'.");
            auditTransaction(transactionId, "Purchase", "RECOVERED", itemId, qty, total,
                             paymentMethod, "RECOVERY_COMPLETE", "",
                             "Recovery chain completed before commit");
            logEntry("Hardware recovery succeeded -> Item: " + itemId);
        }

        kiosk->completePreparedPurchase(itemId, qty, total);
        inventoryRepository.save(inventory, productCatalog);
        auditTransaction(transactionId, "Purchase", "SUCCESS", itemId, qty, total,
                         paymentMethod, "COMMITTED", "",
                         "Inventory reduced and transaction completed atomically");
        logEntry("Purchase completed -> SUCCESS | Item: " + itemId);
        return true;
    }

    bool processRefund(const string &itemId, int qty)
    {
        logEntry("Refund requested -> Item: " + itemId +
                 " Qty: " + to_string(qty));

        if (productCatalog == nullptr || !productCatalog->hasProduct(itemId) ||
            !inventoryPolicy->isAllowed(itemId))
        {
            logEntry("Refund rejected - item not supported by this kiosk -> Item: " + itemId);
            return false;
        }

        if (!ruleEngine->validateRefund(itemId, qty))
        {
            logEntry("Refund rejected by RuleEngine -> Item: " + itemId);
            return false;
        }

        inventory->addStock(itemId, qty);
        inventoryRepository.save(inventory, productCatalog);
        eventManager->publish("TransactionRefunded",
                              "Item: " + itemId + " Qty: " + to_string(qty));
        transactionRepository.append("Refund",
                                     "SUCCESS",
                                     "Item=" + itemId + " Qty=" + to_string(qty));
        logEntry("Refund completed -> Item: " + itemId +
                 " | New stock: " + to_string(inventory->getDerivedStock(itemId)));
        return true;
    }

    bool processRestock(const string &itemId, int qty)
    {
        logEntry("Restock requested -> Item: " + itemId +
                 " Qty: " + to_string(qty));

        if (productCatalog == nullptr || !productCatalog->hasProduct(itemId) ||
            !inventoryPolicy->isAllowed(itemId))
        {
            logEntry("Restock rejected - item not supported by this kiosk -> Item: " + itemId);
            return false;
        }

        if (!ruleEngine->validateRestock(itemId, qty))
        {
            logEntry("Restock rejected by RuleEngine -> Item: " + itemId);
            return false;
        }

        inventory->addStock(itemId, qty);
        inventoryRepository.save(inventory, productCatalog);
        eventManager->publish("InventoryRestocked",
                              "Item: " + itemId + " Qty: " + to_string(qty));
        transactionRepository.append("Restock",
                                     "SUCCESS",
                                     "Item=" + itemId + " Qty=" + to_string(qty));
        logEntry("Restock completed -> Item: " + itemId +
                 " | New stock: " + to_string(inventory->getDerivedStock(itemId)));
        return true;
    }

    void runDiagnostics() const
    {
        cout << "[TransactionManager] Diagnostics report" << endl;
        cout << "  Current kiosk state : " << kiosk->getCurrentStateName() << endl;
        cout << "  Kiosk status        : " << kiosk->getOperationalStatus() << endl;
        cout << "  Inventory policy    : " << inventoryPolicy->getPolicyName() << endl;
        cout << "  Diagnostics         : " << diagnosticsService->buildReport() << endl;
        cout << "  Logged operations   : " << transactionLog.size() << endl;
        cout << "  Snapshot scope      : per transaction" << endl;
    }

    const vector<string> &getTransactionLog() const
    {
        return transactionLog;
    }
};

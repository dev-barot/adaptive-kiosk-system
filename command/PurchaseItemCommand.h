#pragma once
#include "ICommand.h"
#include "../core/TransactionManager.h"
#include <string>
using namespace std;

class PurchaseItemCommand : public ICommand
{
    TransactionManager *transactionManager;
    string itemId;
    int qty;
    string paymentMethod;
    string failureMode;
    bool simulatePaymentFailure;
    int hardwareDelayMs;
    int hardwareTimeoutMs;

public:
    PurchaseItemCommand(TransactionManager *tm,
                        const string &id,
                        int quantity,
                        const string &method,
                        const string &hardwareFailureMode,
                        bool paymentFailure,
                        int delayMs = 0,
                        int timeoutMs = 3000)
        : transactionManager(tm),
          itemId(id),
          qty(quantity),
          paymentMethod(method),
          failureMode(hardwareFailureMode),
          simulatePaymentFailure(paymentFailure),
          hardwareDelayMs(delayMs),
          hardwareTimeoutMs(timeoutMs) {}

    bool execute() override
    {
        return transactionManager->processPurchase(itemId,
                                                  qty,
                                                  paymentMethod,
                                                  failureMode,
                                                  simulatePaymentFailure,
                                                  hardwareDelayMs,
                                                  hardwareTimeoutMs);
    }

    string getName() const override
    {
        return "PurchaseItemCommand";
    }
};

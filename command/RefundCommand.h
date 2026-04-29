#pragma once
#include "ICommand.h"
#include "../core/TransactionManager.h"
#include <string>
using namespace std;

class RefundCommand : public ICommand
{
    TransactionManager *transactionManager;
    string itemId;
    int qty;

public:
    RefundCommand(TransactionManager *tm, const string &id, int quantity)
        : transactionManager(tm), itemId(id), qty(quantity) {}

    bool execute() override
    {
        return transactionManager->processRefund(itemId, qty);
    }

    string getName() const override
    {
        return "RefundCommand";
    }
};

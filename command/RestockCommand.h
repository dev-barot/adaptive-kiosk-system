#pragma once
#include "ICommand.h"
#include "../core/TransactionManager.h"
#include <string>
using namespace std;

class RestockCommand : public ICommand
{
    TransactionManager *transactionManager;
    string itemId;
    int qty;

public:
    RestockCommand(TransactionManager *tm, const string &id, int quantity)
        : transactionManager(tm), itemId(id), qty(quantity) {}

    bool execute() override
    {
        return transactionManager->processRestock(itemId, qty);
    }

    string getName() const override
    {
        return "RestockCommand";
    }
};

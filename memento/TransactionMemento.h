#pragma once
#include <string>
using namespace std;

/**
 * TransactionMemento - Memento Pattern
 * ------------------------------------
 * Captures the minimal state required to restore a transaction if a
 * hardware or payment failure occurs before the purchase is committed.
 */
class TransactionMemento
{
    string itemId;
    int rawStock;
    int reservedStock;
    string kioskStatus;

public:
    TransactionMemento(const string &id,
                       int raw,
                       int reserved,
                       const string &status)
        : itemId(id), rawStock(raw), reservedStock(reserved), kioskStatus(status) {}

    const string &getItemId() const { return itemId; }
    int getRawStock() const { return rawStock; }
    int getReservedStock() const { return reservedStock; }
    const string &getKioskStatus() const { return kioskStatus; }
};

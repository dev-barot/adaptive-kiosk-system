#pragma once
#include "TransactionMemento.h"
#include <memory>

/**
 * TransactionCaretaker - Memento Pattern Caretaker
 * ------------------------------------------------
 * Stores the latest transaction snapshot so it can be restored on failure.
 */
class TransactionCaretaker
{
    unique_ptr<TransactionMemento> snapshot;

public:
    void save(const TransactionMemento &memento)
    {
        snapshot = make_unique<TransactionMemento>(memento);
    }

    bool hasSnapshot() const
    {
        return snapshot != nullptr;
    }

    const TransactionMemento &getSnapshot() const
    {
        return *snapshot;
    }

    void clear()
    {
        snapshot.reset();
    }
};

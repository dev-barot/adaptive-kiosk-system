#pragma once
#include <map>
#include <string>
#include <stdexcept>
using namespace std;

/**
 * ReservationManager - Inventory & Concurrency Subsystem
 * -------------------------------------------------------
 * Temporarily "locks" stock units during an in-progress transaction,
 * preventing double-selling when multiple users interact simultaneously.
 *
 * Workflow:
 *   1. reserve(id, qty)   — lock stock before purchase attempt
 *   2. (transaction executes)
 *   3a. release(id, qty)  — unlock if transaction fails / rolls back
 *   3b. leave locked      — if purchase succeeded, InventoryManager
 *                           permanently reduces stock (reservation consumed)
 *
 * Future: Add reserve(id, qty, userID) for per-user tracking (Subtask 3+)
 */
class ReservationManager
{
    map<string, int> reserved;

public:
    void reserve(const string &id, int qty)
    {
        if (qty <= 0)
            throw invalid_argument("reserve: qty must be positive.");
        reserved[id] += qty;
    }

    void release(const string &id, int qty)
    {
        if (qty <= 0)
            throw invalid_argument("release: qty must be positive.");
        if (reserved[id] < qty)
            throw runtime_error("release: Cannot release " + to_string(qty) +
                                " units - only " + to_string(reserved[id]) +
                                " reserved for '" + id + "'.");
        reserved[id] -= qty;
    }

    int getReserved(const string &id) const
    {
        auto it = reserved.find(id);
        return (it != reserved.end()) ? it->second : 0;
    }
};

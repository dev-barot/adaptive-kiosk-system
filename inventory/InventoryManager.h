#pragma once
#include "ReservationManager.h"
#include <map>
#include <mutex>
#include <string>
#include <stdexcept>
using namespace std;

/**
 * InventoryManager - Inventory & Concurrency Subsystem
 * -----------------------------------------------------
 * Manages raw physical stock levels for all products in the kiosk.
 * Now composes ReservationManager so getDerivedStock() correctly
 * subtracts reserved quantities — matching the class diagram.
 *
 * Pattern: composes ReservationManager (as per class diagram)
 */
class InventoryManager
{
    map<string, int> stock;
    ReservationManager *reservation;
    mutable mutex stockMutex;

public:
    /**
     * @param res - ReservationManager to use for derived stock calculation.
     *              Must outlive this InventoryManager instance.
     */
    InventoryManager(ReservationManager *res) : reservation(res) {}

    void addStock(const string &id, int qty)
    {
        if (qty <= 0)
            throw invalid_argument("addStock: qty must be positive.");
        lock_guard<mutex> lock(stockMutex);
        stock[id] += qty;
    }

    bool isAvailable(const string &id, int qty) const
    {
        return getDerivedStock(id) >= qty;
    }

    void reduceStock(const string &id, int qty)
    {
        lock_guard<mutex> lock(stockMutex);
        auto it = stock.find(id);
        if (it == stock.end() || it->second < qty)
            throw runtime_error("reduceStock: Insufficient stock for product '" + id + "'.");
        stock[id] -= qty;
    }

    int getRawStock(const string &id) const
    {
        lock_guard<mutex> lock(stockMutex);
        auto it = stock.find(id);
        return (it != stock.end()) ? it->second : 0;
    }

    void setRawStock(const string &id, int qty)
    {
        if (qty < 0)
            throw invalid_argument("setRawStock: qty cannot be negative.");
        lock_guard<mutex> lock(stockMutex);
        stock[id] = qty;
    }

    bool tryReserveAvailable(const string &id, int qty)
    {
        if (qty <= 0)
            throw invalid_argument("tryReserveAvailable: qty must be positive.");

        lock_guard<mutex> lock(stockMutex);
        auto it = stock.find(id);
        int raw = (it != stock.end()) ? it->second : 0;
        int available = raw - reservation->getReserved(id);
        if (available < qty)
            return false;

        reservation->reserve(id, qty);
        return true;
    }

    /**
     * Effective available stock = raw stock - reserved units.
     * This is the value the RuleEngine and purchase flow should use.
     */
    int getDerivedStock(const string &id) const
    {
        lock_guard<mutex> lock(stockMutex);
        auto it = stock.find(id);
        int raw = (it != stock.end()) ? it->second : 0;
        int res = reservation->getReserved(id);
        return raw - res;
    }
};

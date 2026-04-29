#pragma once
#include "../inventory/InventoryManager.h"
#include "../product/ProductCatalog.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

/**
 * InventoryRepository - CSV Persistence for InventoryManager
 */
class InventoryRepository
{
    string filePath;

public:
    explicit InventoryRepository(const string &path) : filePath(path) {}

    void load(InventoryManager *inventory,
              const ProductCatalog *catalog) const
    {
        if (inventory == nullptr)
            return;

        ifstream input(filePath);
        if (!input.is_open())
            return;

        string line;
        getline(input, line); // header
        while (getline(input, line))
        {
            if (line.empty())
                continue;

            stringstream ss(line);
            string itemId;
            string qtyText;
            getline(ss, itemId, ',');
            getline(ss, qtyText);

            if (itemId.empty())
                continue;
            if (catalog != nullptr && !catalog->hasProduct(itemId))
                continue;

            inventory->setRawStock(itemId, stoi(qtyText));
        }
    }

    void save(const InventoryManager *inventory,
              const ProductCatalog *catalog) const
    {
        if (inventory == nullptr || catalog == nullptr)
            return;

        ofstream output(filePath);
        output << "item_id,quantity\n";
        for (const string &itemId : catalog->getItemIds())
            output << itemId << "," << inventory->getRawStock(itemId) << "\n";
    }
};

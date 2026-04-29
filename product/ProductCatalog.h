#pragma once
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

/**
 * ProductCatalog - Product Metadata Repository
 * --------------------------------------------
 * Stores product definitions used by pricing, validation, and hardware checks.
 * The inventory keeps quantities, while the catalog keeps descriptive metadata.
 */
class ProductCatalog
{
public:
    struct ProductRecord
    {
        string itemId;
        string displayName;
        float basePrice = 0.0f;
        string requiredHardware;
        bool essential = false;
    };

private:
    map<string, ProductRecord> products;

public:
    void registerProduct(const ProductRecord &record)
    {
        products[record.itemId] = record;
    }

    bool hasProduct(const string &itemId) const
    {
        return products.find(itemId) != products.end();
    }

    const ProductRecord &getProduct(const string &itemId) const
    {
        auto it = products.find(itemId);
        if (it == products.end())
            throw runtime_error("ProductCatalog: Unknown item '" + itemId + "'");
        return it->second;
    }

    float getBasePrice(const string &itemId) const
    {
        return getProduct(itemId).basePrice;
    }

    string getDisplayName(const string &itemId) const
    {
        return getProduct(itemId).displayName;
    }

    string getRequiredHardware(const string &itemId) const
    {
        return getProduct(itemId).requiredHardware;
    }

    bool isEssential(const string &itemId) const
    {
        return getProduct(itemId).essential;
    }

    vector<string> getItemIds() const
    {
        vector<string> ids;
        for (const auto &entry : products)
            ids.push_back(entry.first);
        return ids;
    }
};

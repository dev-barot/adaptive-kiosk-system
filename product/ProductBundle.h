#pragma once
#include "Product.h"
#include <vector>
#include <stdexcept>
using namespace std;

/**
 * ProductBundle - Composite Pattern (Composite Node)
 * ---------------------------------------------------
 * A bundle contains other Products (individual or nested bundles).
 * calculateFinalPrice() recursively sums all children, so the
 * caller doesn't need to know whether it holds singles or bundles.
 *
 * Pattern: Composite (Composite Node)
 */
class ProductBundle : public Product
{
    vector<Product *> items;

public:
    /**
     * Adds a product (individual or another bundle) to this bundle.
     * @param product - must not be nullptr
     */
    void add(Product *product)
    {
        if (product == nullptr)
            throw invalid_argument("Cannot add null product to bundle.");
        items.push_back(product);
    }

    /**
     * Recursively sums the final prices of all items.
     * Each item applies its own pricing strategy.
     */
    float calculateFinalPrice() override
    {
        float total = 0.0f;
        for (Product *p : items)
            total += p->calculateFinalPrice();
        return total;
    }
};

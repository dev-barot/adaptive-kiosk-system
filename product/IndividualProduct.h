#pragma once
#include "Product.h"
#include "../pricing/IPricingStrategy.h"

/**
 * IndividualProduct - Composite Pattern (Leaf) + Strategy Pattern (Context)
 * --------------------------------------------------------------------------
 * Represents a single purchasable item. Delegates pricing to an
 * IPricingStrategy, which can be swapped at runtime.
 *
 * Pattern: Composite (Leaf), Strategy (Context)
 */
class IndividualProduct : public Product
{
    IPricingStrategy *strategy;

public:
    IndividualProduct(float p, IPricingStrategy *s) : strategy(s)
    {
        basePrice = p;
    }

    /**
     * Sets a new pricing strategy at runtime.
     * Called by IKioskState when mode transitions occur.
     */
    void setPricingStrategy(IPricingStrategy *s)
    {
        strategy = s;
    }

    float calculateFinalPrice() override
    {
        return strategy->apply(basePrice);
    }
};

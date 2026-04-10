#pragma once
#include "IPricingStrategy.h"

/**
 * DiscountPricing - Strategy Pattern (Concrete)
 * Applies a 20% discount. Used in Power-Saving mode.
 * Future: make discountRate configurable via constructor.
 */
class DiscountPricing : public IPricingStrategy
{
public:
    float apply(float price) override { return price * 0.8f; }
};

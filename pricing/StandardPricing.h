#pragma once
#include "IPricingStrategy.h"

// Pattern: Strategy (Concrete) - returns price unchanged
class StandardPricing : public IPricingStrategy
{
public:
    float apply(float price) override { return price; }
};

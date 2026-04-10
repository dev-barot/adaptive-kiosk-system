#pragma once
#include "IPricingStrategy.h"

// Pattern: Strategy (Concrete) - 50% price reduction for emergency relief
class EmergencyPricing : public IPricingStrategy
{
public:
    float apply(float price) override { return price * 0.5f; }
};

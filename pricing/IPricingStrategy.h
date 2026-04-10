#pragma once

/**
 * IPricingStrategy - Strategy Pattern Interface
 * --------------------------------------------
 * Defines the contract for all pricing algorithms.
 * Any new pricing type (SurgePricing, LoyaltyPricing, etc.)
 * must inherit from this and implement apply().
 *
 * Pattern: Strategy
 * Used by: IndividualProduct, IKioskState
 */
class IPricingStrategy
{
public:
    virtual float apply(float price) = 0;
    virtual ~IPricingStrategy() = default;
};

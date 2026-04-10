#pragma once

/**
 * Product - Composite Pattern (Component)
 * ----------------------------------------
 * Abstract base for both individual products and bundles.
 * basePrice stores the raw price before any strategy is applied.
 *
 * Pattern: Composite (Component)
 * Subclasses: IndividualProduct (Leaf), ProductBundle (Composite)
 */
class Product
{
protected:
    float basePrice = 0.0f;

public:
    virtual float calculateFinalPrice() = 0;
    virtual ~Product() = default;
};

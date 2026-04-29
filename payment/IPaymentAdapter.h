#pragma once
#include <string>
using namespace std;

/**
 * IPaymentAdapter - Adapter Pattern Interface
 * -------------------------------------------
 * Normalizes different payment providers behind one common interface.
 */
class IPaymentAdapter
{
public:
    virtual bool processPayment(float amount, bool simulateFailure = false) = 0;
    virtual bool refundPayment(float amount) = 0;
    virtual string getAdapterName() const = 0;
    virtual ~IPaymentAdapter() = default;
};

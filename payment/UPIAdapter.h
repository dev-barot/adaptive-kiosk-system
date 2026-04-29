#pragma once
#include "IPaymentAdapter.h"
#include <iostream>
using namespace std;

class UPIAdapter : public IPaymentAdapter
{
public:
    bool processPayment(float amount, bool simulateFailure = false) override
    {
        cout << "[UPIAdapter] Processing UPI payment of Rs." << amount << endl;
        if (simulateFailure)
        {
            cout << "[UPIAdapter] Payment failed at the gateway." << endl;
            return false;
        }
        cout << "[UPIAdapter] Payment approved." << endl;
        return true;
    }

    bool refundPayment(float amount) override
    {
        cout << "[UPIAdapter] Refund issued for Rs." << amount << endl;
        return true;
    }

    string getAdapterName() const override
    {
        return "UPI";
    }
};

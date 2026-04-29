#pragma once
#include "IPaymentAdapter.h"
#include <iostream>
using namespace std;

class CardAdapter : public IPaymentAdapter
{
public:
    bool processPayment(float amount, bool simulateFailure = false) override
    {
        cout << "[CardAdapter] Authorizing card payment of Rs." << amount << endl;
        if (simulateFailure)
        {
            cout << "[CardAdapter] Card authorization failed." << endl;
            return false;
        }
        cout << "[CardAdapter] Card payment approved." << endl;
        return true;
    }

    bool refundPayment(float amount) override
    {
        cout << "[CardAdapter] Card refund issued for Rs." << amount << endl;
        return true;
    }

    string getAdapterName() const override
    {
        return "CARD";
    }
};

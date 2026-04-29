#pragma once
#include "IPaymentAdapter.h"
#include <iostream>
using namespace std;

class WalletAdapter : public IPaymentAdapter
{
public:
    bool processPayment(float amount, bool simulateFailure = false) override
    {
        cout << "[WalletAdapter] Charging wallet for Rs." << amount << endl;
        if (simulateFailure)
        {
            cout << "[WalletAdapter] Wallet transaction failed." << endl;
            return false;
        }
        cout << "[WalletAdapter] Wallet payment approved." << endl;
        return true;
    }

    bool refundPayment(float amount) override
    {
        cout << "[WalletAdapter] Wallet refund issued for Rs." << amount << endl;
        return true;
    }

    string getAdapterName() const override
    {
        return "WALLET";
    }
};

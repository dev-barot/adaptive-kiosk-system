#pragma once
#include "UPIAdapter.h"
#include "CardAdapter.h"
#include "WalletAdapter.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * PaymentManager - Adapter Coordinator
 * ------------------------------------
 * Selects the right payment adapter while keeping transaction code
 * independent of provider-specific behavior.
 */
class PaymentManager
{
    UPIAdapter upiAdapter;
    CardAdapter cardAdapter;
    WalletAdapter walletAdapter;

    IPaymentAdapter *resolveAdapter(const string &method)
    {
        if (method == "UPI")
            return &upiAdapter;
        if (method == "CARD")
            return &cardAdapter;
        if (method == "WALLET")
            return &walletAdapter;
        return nullptr;
    }

public:
    bool processPayment(const string &method,
                        float amount,
                        bool simulateFailure = false)
    {
        IPaymentAdapter *adapter = resolveAdapter(method);
        if (adapter == nullptr)
        {
            cout << "[PaymentManager] Unknown payment method: " << method << endl;
            return false;
        }

        cout << "[PaymentManager] Selected adapter: "
             << adapter->getAdapterName() << endl;
        return adapter->processPayment(amount, simulateFailure);
    }

    bool refundPayment(const string &method, float amount)
    {
        IPaymentAdapter *adapter = resolveAdapter(method);
        if (adapter == nullptr)
        {
            cout << "[PaymentManager] Refund skipped - unknown payment method: "
                 << method << endl;
            return false;
        }

        return adapter->refundPayment(amount);
    }
};

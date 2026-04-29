#pragma once
#include <string>
using namespace std;

/**
 * IDispenser - Hardware Abstraction Interface
 * -------------------------------------------
 * Abstracts dispensing hardware so recovery handlers can act without
 * depending on a concrete hardware implementation.
 */
class IDispenser
{
public:
    virtual bool dispense(const string &itemId,
                          int qty,
                          const string &failureMode = "NONE",
                          int responseDelayMs = 0,
                          int timeoutMs = 3000) = 0;
    virtual bool retryDispense(const string &itemId, int qty) = 0;
    virtual bool recalibrate() = 0;
    virtual ~IDispenser() = default;
};

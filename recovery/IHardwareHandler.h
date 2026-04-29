#pragma once
#include "../hardware/IDispenser.h"
#include <string>
using namespace std;

/**
 * IHardwareHandler - Chain of Responsibility Base
 * -----------------------------------------------
 * Each handler tries to recover from a hardware fault before passing
 * control to the next handler in the chain.
 */
class IHardwareHandler
{
protected:
    IHardwareHandler *next = nullptr;

public:
    void setNext(IHardwareHandler *handler)
    {
        next = handler;
    }

    virtual bool handle(IDispenser *dispenser,
                        const string &itemId,
                        int qty) = 0;

    virtual ~IHardwareHandler() = default;
};

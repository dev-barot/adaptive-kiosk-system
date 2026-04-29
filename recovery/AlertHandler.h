#pragma once
#include "IHardwareHandler.h"
#include "../observer/EventManager.h"
#include <iostream>
using namespace std;

class AlertHandler : public IHardwareHandler
{
    EventManager *eventManager;

public:
    AlertHandler(EventManager *em) : eventManager(em) {}

    bool handle(IDispenser *dispenser,
                const string &itemId,
                int qty) override
    {
        (void)dispenser;
        cout << "[AlertHandler] Escalating unresolved hardware issue to technician." << endl;
        if (eventManager != nullptr)
        {
            eventManager->publish("TechnicianAlert",
                                  "Manual intervention required for '" + itemId +
                                  "' x" + to_string(qty));
        }
        return false;
    }
};

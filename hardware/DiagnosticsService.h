#pragma once
#include "HardwareStatus.h"
#include <string>
using namespace std;

/**
 * DiagnosticsService - Hardware Readiness Checks
 * ----------------------------------------------
 * Centralizes readiness checks used by validation and demo diagnostics.
 */
class DiagnosticsService
{
    HardwareStatus *hardwareStatus;

public:
    explicit DiagnosticsService(HardwareStatus *status) : hardwareStatus(status) {}

    bool isModuleReady(const string &moduleName) const
    {
        return hardwareStatus != nullptr &&
               hardwareStatus->isModuleAvailable(moduleName);
    }

    bool canDispenseProduct(const string &requiredHardware) const
    {
        return requiredHardware.empty() || requiredHardware == "NONE" ||
               isModuleReady(requiredHardware);
    }

    string buildReport() const
    {
        if (hardwareStatus == nullptr)
            return "Diagnostics unavailable";
        return "Hardware modules: " + hardwareStatus->summary();
    }
};

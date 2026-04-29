#pragma once
#include <map>
#include <string>
using namespace std;

/**
 * HardwareStatus - Hardware Module Registry
 * -----------------------------------------
 * Tracks which hardware modules are installed and currently operational.
 */
class HardwareStatus
{
    map<string, bool> modules;

public:
    void setModuleStatus(const string &moduleName, bool available)
    {
        modules[moduleName] = available;
    }

    bool isModuleAvailable(const string &moduleName) const
    {
        if (moduleName.empty() || moduleName == "NONE")
            return true;

        auto it = modules.find(moduleName);
        return it != modules.end() && it->second;
    }

    string summary() const
    {
        if (modules.empty())
            return "No hardware modules registered";

        string result;
        for (const auto &module : modules)
        {
            if (!result.empty())
                result += ", ";
            result += module.first + string(module.second ? "=ONLINE" : "=OFFLINE");
        }
        return result;
    }
};

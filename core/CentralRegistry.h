#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
using namespace std;

/**
 * CentralRegistry - Singleton Pattern
 * -------------------------------------
 * Single shared configuration and system-state store.
 * All modules (pricing, rules, inventory) read the same global
 * config from here — ensures consistent behavior across the system.
 *
 * Enforced by:
 *   - Private constructor (prevents direct instantiation)
 *   - Deleted copy constructor and assignment operator (prevents cloning)
 *   - Static getInstance() as the ONLY access point
 *
 * Pattern: Singleton
 * Used by: KioskManager, RuleEngine, EventManager setup
 */
class CentralRegistry
{
    static CentralRegistry *instance;
    map<string, string> configData;

    // Private constructor — cannot be instantiated externally
    CentralRegistry()
    {
        // Default system configuration
        configData["system_mode"]       = "ACTIVE";
        configData["emergency_limit"]   = "2";
        configData["low_stock_threshold"] = "2";
        configData["kiosk_version"]     = "2.0";
    }

public:
    // Delete copy and assignment — enforces single instance
    CentralRegistry(const CentralRegistry &) = delete;
    CentralRegistry &operator=(const CentralRegistry &) = delete;

    /**
     * Returns the single global instance of CentralRegistry.
     * Creates it on first call (lazy initialization).
     */
    static CentralRegistry *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new CentralRegistry();
            cout << "[CentralRegistry] Singleton instance created." << endl;
        }
        return instance;
    }

    /**
     * Retrieve a configuration value by key.
     * @throws runtime_error if key does not exist
     */
    string getConfig(const string &key) const
    {
        auto it = configData.find(key);
        if (it == configData.end())
            throw runtime_error("CentralRegistry: Unknown config key '" + key + "'");
        return it->second;
    }

    /**
     * Set or update a configuration value.
     * Used when system mode changes (e.g., switching to emergency).
     */
    void setConfig(const string &key, const string &value)
    {
        configData[key] = value;
        cout << "[CentralRegistry] Config updated: " << key << " = " << value << endl;
    }

    const map<string, string> &getAllConfig() const
    {
        return configData;
    }
};

// Static member definition — must be in exactly one translation unit
CentralRegistry *CentralRegistry::instance = nullptr;

#pragma once
#include <set>
#include <stdexcept>
#include <string>
using namespace std;

/**
 * InventoryPolicy - Kiosk-Type Validation Profile
 * -----------------------------------------------
 * Defines which items a kiosk type can offer and whether a product should
 * be restricted to verified transactions.
 */
class InventoryPolicy
{
    string policyName;
    set<string> allowedItems;
    set<string> verificationRequired;

public:
    explicit InventoryPolicy(const string &name = "GenericPolicy")
        : policyName(name) {}

    void allowItem(const string &itemId, bool requiresVerification = false)
    {
        allowedItems.insert(itemId);
        if (requiresVerification)
            verificationRequired.insert(itemId);
    }

    bool isAllowed(const string &itemId) const
    {
        return allowedItems.find(itemId) != allowedItems.end();
    }

    bool requiresVerification(const string &itemId) const
    {
        return verificationRequired.find(itemId) != verificationRequired.end();
    }

    const string &getPolicyName() const
    {
        return policyName;
    }
};

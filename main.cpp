#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <limits>
using namespace std;

#include "core/CentralRegistry.h"
#include "factory/EmergencyReliefKioskFactory.h"
#include "factory/FoodKioskFactory.h"
#include "factory/PharmacyKioskFactory.h"
#include "persistence/ConfigRepository.h"
#include "persistence/TransactionRepository.h"
#include "pricing/DiscountPricing.h"
#include "pricing/EmergencyPricing.h"
#include "pricing/StandardPricing.h"
#include "product/IndividualProduct.h"
#include "product/ProductBundle.h"

namespace
{
const string DATA_DIR = "data/";

void printSeparator(const string &title)
{
    cout << "\n============================================================" << endl;
    cout << title << endl;
    cout << "============================================================" << endl;
}

void printInventory(KioskRuntime &runtime)
{
    cout << "Inventory snapshot for " << runtime.kioskType << ":" << endl;
    for (const string &itemId : runtime.productCatalog->getItemIds())
    {
        cout << "  - " << setw(12) << left << itemId
             << " stock=" << runtime.inventoryManager->getDerivedStock(itemId)
             << " price=Rs." << runtime.productCatalog->getBasePrice(itemId)
             << endl;
    }
}

void printFactorySummary(KioskRuntime &runtime, const string &factoryName)
{
    cout << factoryName << " created " << runtime.kioskType
         << " (" << runtime.kioskId << ")" << endl;
    cout << "  Policy      : " << runtime.inventoryPolicy->getPolicyName() << endl;
    cout << "  Diagnostics : " << runtime.diagnosticsService->buildReport() << endl;
    printInventory(runtime);
}

void saveSystemConfig(const ConfigRepository &repository)
{
    repository.save(CentralRegistry::getInstance());
}

string getInput(const string &prompt)
{
    cout << prompt;
    string input;
    getline(cin >> ws, input);
    return input;
}

int getIntInput(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        int value;
        if (cin >> value)
        {
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number." << endl;
    }
}

void runAutomatedScenarios(KioskRuntime &pharmacy, KioskRuntime &food, KioskRuntime &emergency, ConfigRepository &configRepository)
{
    printSeparator("2. PHARMACY COMPATIBILITY CHECK");
    cout << "Attempt: buy refrigerated insulin from pharmacy kiosk" << endl;
    pharmacy.kioskInterface->purchaseItem("insulin", 1, "CARD");

    printSeparator("3. FOOD KIOSK QUICK FLOW");
    cout << "Attempt: buy 2x snack from food kiosk" << endl;
    food.kioskInterface->purchaseItem("snack", 2, "UPI");
    cout << "Attempt: restock 3x food" << endl;
    food.kioskInterface->restockInventory("food", 3);
    food.kioskInterface->runDiagnostics();

    printSeparator("4. EMERGENCY RELIEF - PATH A CORE SCENARIOS");
    emergency.kioskInterface->runDiagnostics();

    cout << "\nScenario A: Normal purchase in ACTIVE mode" << endl;
    emergency.kioskInterface->purchaseItem("water", 2, "UPI");

    cout << "\nScenario B: POWER_SAVING mode with discounted pricing" << endl;
    emergency.kiosk->transitionState(emergency.powerSavingState.get());
    CentralRegistry::getInstance()->setConfig("system_mode", "POWER_SAVING");
    saveSystemConfig(configRepository);
    emergency.kioskInterface->purchaseItem("food", 2, "CARD");

    cout << "\nScenario C: EMERGENCY mode with quantity cap and emergency pricing" << endl;
    emergency.kiosk->transitionState(emergency.emergencyState.get());
    CentralRegistry::getInstance()->setConfig("system_mode", "EMERGENCY");
    saveSystemConfig(configRepository);
    emergency.kioskInterface->purchaseItem("medkit", 1, "WALLET");

    cout << "\nScenario D: Emergency over-limit purchase denied" << endl;
    emergency.kioskInterface->purchaseItem("food", 3, "UPI");

    cout << "\nScenario E: Low-stock warning after battery purchase" << endl;
    emergency.kioskInterface->purchaseItem("battery", 2, "CARD");

    cout << "\nScenario F: Recoverable hardware fault resolved by recovery chain" << endl;
    emergency.kioskInterface->purchaseItem("water", 1, "CARD", "RECOVERABLE");

    cout << "\nScenario G: Delayed hardware response times out and rolls back" << endl;
    emergency.kioskInterface->purchaseItem("water", 1, "CARD", "NONE", false, 1500, 500);

    cout << "\nScenario H: Fatal hardware fault triggers refund and rollback" << endl;
    emergency.kioskInterface->purchaseItem("battery", 1, "WALLET", "FATAL");

    cout << "\nScenario I: Simulated payment failure rolls back cleanly" << endl;
    emergency.kioskInterface->purchaseItem("food", 1, "UPI", "NONE", true);

    cout << "\nScenario J: Concurrent purchases cannot oversell reserved stock" << endl;
    emergency.inventoryManager->setRawStock("battery", 2);
    emergency.reservationManager->setReserved("battery", 0);
    atomic<int> successfulConcurrentPurchases{0};
    auto concurrentPurchase = [&]()
    {
        if (emergency.kioskInterface->purchaseItem("battery", 2, "CARD"))
            ++successfulConcurrentPurchases;
    };
    thread userOne(concurrentPurchase);
    thread userTwo(concurrentPurchase);
    userOne.join();
    userTwo.join();
    cout << "Concurrent purchase success count: "
         << successfulConcurrentPurchases.load()
         << " | Remaining derived stock: "
         << emergency.inventoryManager->getDerivedStock("battery") << endl;

    cout << "\nScenario K: Priority event processing handles emergency first" << endl;
    emergency.eventManager->queueEvent("TransactionCompleted", "Routine sale completed.");
    emergency.eventManager->queueEvent("LowStockEvent", "Water is near threshold.");
    emergency.eventManager->queueEvent("EmergencyModeActivated", "City flood alert received.");
    emergency.eventManager->processQueuedEvents();

    cout << "\nScenario L: MAINTENANCE mode blocks purchases" << endl;
    emergency.kiosk->transitionState(emergency.maintenanceState.get());
    CentralRegistry::getInstance()->setConfig("system_mode", "MAINTENANCE");
    saveSystemConfig(configRepository);
    emergency.kioskInterface->purchaseItem("water", 1, "UPI");

    cout << "\nScenario M: Admin commands in maintenance mode" << endl;
    emergency.kioskInterface->restockInventory("battery", 3);
    emergency.kioskInterface->refundTransaction("water", 1);
    emergency.kioskInterface->runDiagnostics();

    printSeparator("5. COMPOSITE + STRATEGY PRICE DEMO");
    StandardPricing standardPricing;
    DiscountPricing discountPricing;
    EmergencyPricing emergencyPricing;

    IndividualProduct waterPack(emergency.productCatalog->getBasePrice("water"), &standardPricing);
    IndividualProduct rationPack(emergency.productCatalog->getBasePrice("food"), &discountPricing);
    IndividualProduct medicalPack(emergency.productCatalog->getBasePrice("medkit"), &emergencyPricing);

    ProductBundle basicReliefKit;
    basicReliefKit.add(&waterPack);
    basicReliefKit.add(&rationPack);

    ProductBundle cityResponseKit;
    cityResponseKit.add(&basicReliefKit);
    cityResponseKit.add(&medicalPack);

    cout << "Basic relief kit final price  : Rs." << basicReliefKit.calculateFinalPrice() << endl;
    cout << "City response kit final price : Rs." << cityResponseKit.calculateFinalPrice() << endl;

    printSeparator("6. PERSISTENCE CHECK");
    vector<string> auditRows = TransactionRepository(DATA_DIR + "emergency_transactions.csv").loadAll();
    cout << "Persisted emergency transaction rows: " << auditRows.size() << endl;
    cout << "Current registry mode saved as: "
         << CentralRegistry::getInstance()->getConfig("system_mode") << endl;

    printSeparator("7. FINAL STATUS");
    cout << "Project now demonstrates:" << endl;
    cout << "  * Abstract Factory for kiosk creation" << endl;
    cout << "  * Facade + RuleEngine + Command transaction flow" << endl;
    cout << "  * Memento rollback, payment adapters, and recovery chain" << endl;
    cout << "  * State-driven pricing and operational restrictions" << endl;
    cout << "  * Thread-safe reservations for concurrent transaction protection" << endl;
    cout << "  * Delayed hardware timeout handling with rollback" << endl;
    cout << "  * Priority Observer events for emergency overrides" << endl;
    cout << "  * Composite product bundles" << endl;
    cout << "  * CSV persistence for config, inventory, and transactions" << endl;
}

void interactiveKioskMenu(KioskRuntime &runtime, ConfigRepository &configRepository)
{
    while (true)
    {
        printSeparator("INTERACTIVE KIOSK: " + runtime.kioskType + " (" + runtime.kioskId + ")");
        cout << "1. Purchase Item" << endl;
        cout << "2. Restock Item" << endl;
        cout << "3. Run Diagnostics" << endl;
        cout << "4. Switch Mode (State)" << endl;
        cout << "5. View Inventory" << endl;
        cout << "6. Back to Main Menu" << endl;

        int choice = getIntInput("Select action: ");

        if (choice == 1)
        {
            cout << "\n--- Catalog Summary ---" << endl;
            for (const string &id : runtime.productCatalog->getItemIds())
            {
                cout << "  - " << id << " (Stock: " << runtime.inventoryManager->getDerivedStock(id) << ")" << endl;
            }
            string itemId = getInput("\nEnter Item ID to buy: ");
            int qty = getIntInput("Enter Quantity: ");
            string payment = getInput("Enter Payment Method (CARD/UPI/WALLET): ");
            runtime.kioskInterface->purchaseItem(itemId, qty, payment);
        }
        else if (choice == 2)
        {
            string itemId = getInput("Enter Item ID: ");
            int qty = getIntInput("Enter Quantity: ");
            runtime.kioskInterface->restockInventory(itemId, qty);
        }
        else if (choice == 3)
        {
            runtime.kioskInterface->runDiagnostics();
        }
        else if (choice == 4)
        {
            cout << "1. ACTIVE" << endl;
            cout << "2. POWER_SAVING" << endl;
            cout << "3. EMERGENCY" << endl;
            cout << "4. MAINTENANCE" << endl;
            int stateChoice = getIntInput("Select Mode: ");
            if (stateChoice == 1)
            {
                runtime.kiosk->transitionState(runtime.activeState.get());
                CentralRegistry::getInstance()->setConfig("system_mode", "ACTIVE");
            }
            else if (stateChoice == 2)
            {
                runtime.kiosk->transitionState(runtime.powerSavingState.get());
                CentralRegistry::getInstance()->setConfig("system_mode", "POWER_SAVING");
            }
            else if (stateChoice == 3)
            {
                runtime.kiosk->transitionState(runtime.emergencyState.get());
                CentralRegistry::getInstance()->setConfig("system_mode", "EMERGENCY");
            }
            else if (stateChoice == 4)
            {
                runtime.kiosk->transitionState(runtime.maintenanceState.get());
                CentralRegistry::getInstance()->setConfig("system_mode", "MAINTENANCE");
            }
            saveSystemConfig(configRepository);
            cout << "System mode updated." << endl;
        }
        else if (choice == 5)
        {
            printInventory(runtime);
        }
        else if (choice == 6)
        {
            break;
        }
    }
}
}

int main()
{
    cout << "============================================================" << endl;
    cout << "AURA RETAIL OS - FINAL PATH A DEMONSTRATION" << endl;
    cout << "Adaptive Autonomous System | Group 5" << endl;
    cout << "============================================================" << endl;

    ConfigRepository configRepository(DATA_DIR + "config.csv");
    configRepository.load(CentralRegistry::getInstance());

    printSeparator("1. ABSTRACT FACTORY BOOTSTRAP");

    PharmacyKioskFactory pharmacyFactory;
    FoodKioskFactory foodFactory;
    EmergencyReliefKioskFactory emergencyFactory;

    KioskRuntime pharmacy = pharmacyFactory.create("PHARM-01",
                                                   DATA_DIR + "pharmacy_inventory.csv",
                                                   DATA_DIR + "pharmacy_transactions.csv");
    KioskRuntime food = foodFactory.create("FOOD-01",
                                           DATA_DIR + "food_inventory.csv",
                                           DATA_DIR + "food_transactions.csv");
    KioskRuntime emergency = emergencyFactory.create("ER-01",
                                                     DATA_DIR + "emergency_inventory.csv",
                                                     DATA_DIR + "emergency_transactions.csv");

    printFactorySummary(pharmacy, pharmacyFactory.getFactoryName());
    printFactorySummary(food, foodFactory.getFactoryName());
    printFactorySummary(emergency, emergencyFactory.getFactoryName());

    while (true)
    {
        printSeparator("MAIN MENU - AURA RETAIL OS");
        cout << "1. Interactive Pharmacy Kiosk" << endl;
        cout << "2. Interactive Food Kiosk" << endl;
        cout << "3. Interactive Emergency Relief Kiosk" << endl;
        cout << "4. Run All Automated Test Scenarios" << endl;
        cout << "5. Exit" << endl;

        int choice = getIntInput("Select option: ");

        if (choice == 1)
        {
            interactiveKioskMenu(pharmacy, configRepository);
        }
        else if (choice == 2)
        {
            interactiveKioskMenu(food, configRepository);
        }
        else if (choice == 3)
        {
            interactiveKioskMenu(emergency, configRepository);
        }
        else if (choice == 4)
        {
            runAutomatedScenarios(pharmacy, food, emergency, configRepository);
        }
        else if (choice == 5)
        {
            cout << "Exiting AURA Retail OS..." << endl;
            break;
        }
    }

    return 0;
}

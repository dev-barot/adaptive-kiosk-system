# 📘 Design Report: Aura Retail OS
### **Plan A: Adaptive Autonomous System Implementation**

---

## 1. Executive Summary
Aura Retail OS is designed as a resilient, autonomous infrastructure for smart cities. The system leverages advanced Object-Oriented Programming (OOP) principles and design patterns to ensure high availability, fault tolerance, and intelligent adaptation to environmental changes (like city-wide emergencies).

---

## 2. Design Patterns Implemented

| Pattern | Implementation Detail | Role in Plan A |
| :--- | :--- | :--- |
| **Abstract Factory** | `IKioskFactory`, `PharmacyKioskFactory`, `FoodKioskFactory` | Decouples system initialization from specific product/policy logic. |
| **State Pattern** | `IKioskState`, `ActiveState`, `EmergencyState` | Allows the kiosk to change its behavior dynamically based on system health or city status. |
| **Strategy Pattern** | `IPricingStrategy`, `StandardPricing`, `EmergencyPricing` | Swaps pricing algorithms at runtime without modifying the product classes. |
| **Memento Pattern** | `TransactionMemento`, `TransactionCaretaker` | Provides a "Undo" capability for transactions, ensuring atomicity during hardware failure. |
| **Chain of Responsibility** | `RecoveryHandler`, `RetryHandler`, `AlertHandler` | Implements an autonomous self-healing sequence for hardware faults. |
| **Observer Pattern** | `EventManager`, `CityMonitoringCenter` | Enables real-time, priority-based event broadcasting for remote monitoring. |
| **Facade** | `KioskInterface` | Simplifies the complex internal manager logic into a single point of entry for the UI. |
| **Command Pattern** | `RestockCommand`, `RefundCommand` | Encapsulates operational actions as objects for logging and delayed execution. |
| **Composite Pattern** | `ProductBundle`, `IndividualProduct` | Allows treating single items and complex bundles (e.g., Relief Kits) uniformly. |
| **Singleton** | `CentralRegistry` | Provides a thread-safe, global configuration store. |

---

## 3. Core OOP Concepts Applied

### A. Abstraction
Hardware interfaces (Dispenser, Payment) are abstracted, allowing the core logic to remain independent of specific hardware vendors.

### B. Encapsulation
Each core component (Inventory, Reservation, Transaction) encapsulates its state. For instance, the `InventoryManager` is the only component that can mutate stock levels, ensuring data integrity.

### C. Polymorphism
The system interacts with `IKioskState` and `IPricingStrategy` interfaces. At runtime, these are swapped out for concrete implementations (e.g., `EmergencyState`), causing the system to behave differently without changing the caller's code.

### D. Inheritance
The `RecoveryHandler` uses inheritance to share base logic for linking handlers in the chain, while concrete handlers implement specific logic for `Retry` or `Recalibrate`.

---

## 4. Simulation Demonstration Walkthrough

### 1. CLI Simulation (Backend)
The backend simulation (`main.cpp`) is the definitive proof of the system's logic.

**Steps to Demonstrate**:
1.  **Build**: `g++ -std=c++17 main.cpp -o aura.exe`
2.  **Execute**: `./aura.exe`
3.  **Key Observations**:
    -   **State Transition**: Observe the kiosk moving from `ACTIVE` to `EMERGENCY` mode.
    -   **Pricing Shift**: Notice the unit price for a "Medkit" dropping by 50% in emergency mode.
    -   **Self-Healing**: Look for "Hardware Retry Succeeded" logs during Scenario F.
    -   **Rollback Integrity**: In Scenario G, observe that a "Hardware Timeout" results in "Snapshot Restored", ensuring the user isn't charged for an undispensed item.

### 2. Management Dashboard (Frontend)
The frontend provides a visual layer to the same autonomous logic.

**Steps to Demonstrate**:
1.  **Launch**: Open `frontend/index.html`.
2.  **Scenario: The "Atomic" Rollback**:
    -   Click **"Hardware Latency Test"**.
    -   Click **"Confirm Purchase"**.
    -   Watch the progress bars. When the "Hardware" step hits a timeout, observe the **Rollback Signal** and the stock count returning to its original value.
3.  **Scenario: Concurrency Race**:
    -   Click **"Concurrent Race"**.
    -   Observe the audit log showing two simultaneous requests where one is granted a lock and the other is denied, preventing over-selling.

---

## 5. Persistence & Data Integrity
The system persists all state to CSV files in the `data/` directory. Even if the process crashes, the **Audit Trail** and **Inventory State** are preserved, allowing for post-event analysis.

---

**Submitted by Group 5 - IT620**

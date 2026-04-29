# 🌌 Aura Retail OS: Adaptive Autonomous Kiosk System
### **Smart-City Modular Infrastructure (Plan A Implementation)**

Aura Retail OS is a sophisticated software architecture designed for the autonomous modular retail infrastructure of a future smart city. It fulfills **Plan A (Adaptive Autonomous System)** requirements, focusing on intelligent adaptation, fault tolerance, and resilient distributed operations.

---

## 🏛️ System Architecture: The "Plan A" Vision

Plan A focuses on creating a system that isn't just a static vending machine, but an **adaptive autonomous agent** capable of responding to city-wide events, hardware failures, and shifting environmental demands.

### 🧠 Core Pillars of Plan A
1.  **Resilience**: Autonomous rollback and self-healing when hardware fails.
2.  **Adaptability**: Dynamic state-swapping (Emergency vs. Power-Saving) with real-time pricing and restriction shifts.
3.  **Autonomy**: Decoupled event-driven synchronization with a city-wide monitoring center.

---

## 🛠️ Design Patterns & OOP Implementation

The system is built on **10+ Design Patterns** to ensure a loosely coupled, highly adaptive architecture.

| Feature | Pattern | Role in Plan A |
| :--- | :--- | :--- |
| **Autonomous Creation** | **Abstract Factory** | Dynamically bootstraps Pharmacy, Food, or Emergency kiosks with specific policies. |
| **Operational Modes** | **State Pattern** | Adapts behavior (Active, Power-Saving, Emergency, Maintenance) at runtime. |
| **Dynamic Pricing** | **Strategy Pattern** | Swaps pricing policies instantly based on system state or product bundles. |
| **Transaction Atomicity**| **Memento** | Captures state snapshots to allow "Atomic Transactions" (rollback on failure). |
| **Self-Healing Logic** | **Chain of Responsibility** | Sequential recovery logic: `Retry` → `Recalibrate` → `Alert`. |
| **Smart City Sync** | **Observer** | Priority-based event broadcasting to city monitoring centers. |
| **Unified Access** | **Facade** | Provides a simplified `KioskInterface` for the frontend and main simulation. |
| **Modular Operations** | **Command** | Encapsulates admin actions like `Restock` and `Refund` as discrete objects. |
| **Complex Products** | **Composite** | Allows creation of `ProductBundles` (e.g., Relief Kits) from individual items. |
| **Global Registry** | **Singleton** | Centralized thread-safe configuration management. |

### 💎 Important OOP Concepts
-   **Abstraction**: Hidden complexities of hardware and payment are tucked behind interfaces (`IPaymentProvider`, `IDispenser`).
-   **Encapsulation**: Each manager (Inventory, Reservation, Transaction) strictly owns its data, exposed only via valid methods.
-   **Polymorphism**: The `Kiosk` object behaves differently depending on whether it's in `EmergencyState` or `ActiveState` without changing its core interface.
-   **Inheritance**: Clean hierarchy in `RecoveryHandler` and `KioskFactory` ensures code reuse and logical extension.

---

## ✨ Unique Engineering Flavors

### 🛡️ Transaction Atomicity (Memento-Powered Rollback)
Most vending systems fail partially (take money but don't dispense). Aura OS uses a **Memento snapshot** before every purchase. If a hardware timeout occurs or the payment adapter errors, the system performs an **autonomous rollback**, restoring inventory and refunding credits instantly.

### 🔒 Thread-Safe Concurrent Reservations
Designed for high-density cities, the `InventoryManager` uses **Mutex Locks** and a **Derived Stock** calculation. It ensures that even if two users tap "Buy" at the exact same millisecond, the system creates atomic reservations, preventing any "double-sell" scenarios.

### 🧠 Intelligent Operational Status
Kiosk health isn't a simple flag; it's a **derived attribute**. The system computes its status at runtime by cross-referencing hardware diagnostics, connectivity, and current system mode.

---

## 🚀 Getting Started

### 1. Backend Simulation (C++)
The C++ core demonstrates 13+ complex scenarios including hardware failures, concurrent races, and emergency overrides.

**Requirements**: G++ compiler (supporting C++17).

```bash
# Compile the main simulation
g++ -std=c++17 main.cpp -o aura.exe

# Run the demonstration
./aura.exe
```

### 2. Visual Management Dashboard (Frontend)
The frontend is a **Real-Time Glassmorphism UI** that visualizes the internal logic of the system.

1.  Navigate to the `frontend/` folder.
2.  Open **`index.html`** in any modern web browser.
3.  **How to use**:
    -   **Select a Scenario**: Choose between *Standard*, *Emergency*, or *Timeout* modes.
    -   **Trigger Logic**: Click the scenario buttons to see the backend flow (Validation → Reservation → Payment → Hardware).
    -   **Visual Feedback**: Watch the **Event Feed** and **Audit Log** for real-time logic execution.
    -   **Concurrency Race**: Click the "Concurrent Race" button to see how the system blocks simultaneous double-purchases.

---

## 📂 Project Structure

-   `core/`: The "Brain" - Transaction orchestration, RuleEngine, and CentralRegistry.
-   `factory/`: "Birth" logic - Abstract factories for specialized kiosk types.
-   `state/` & `pricing/`: "Adaptation" logic - State and Strategy implementations.
-   `hardware/` & `recovery/`: "Resilience" - Hardware abstraction and self-healing chains.
-   `observer/`: "Communication" - Priority-based event bus for city-wide sync.
-   `memento/`: "Stability" - Snapshot and rollback definitions.
-   `persistence/`: "Memory" - CSV-based repositories for audit trails and state.
-   `frontend/`: "Vision" - Real-time management dashboard with modern aesthetics.

---

**Lazy Constructors - IT620 Project**  
*Building resilient infrastructure for the future city.*

# Aura Retail OS: Adaptive Autonomous Kiosk System
**Smart-City Modular Infrastructure (Path A Implementation)**

Aura Retail OS is a sophisticated software architecture designed for the autonomous modular retail infrastructure of a future smart city. It fulfills **Path A (Adaptive Autonomous System)** requirements for the IT620 OOP course, focusing on intelligent adaptation, fault tolerance, and resilient distributed operations.

---

## 🏛️ System Architecture & "Path A" Alignment

The system is built on **10+ Design Patterns** to ensure a loosely coupled, highly adaptive architecture.

| Feature | Pattern | Role in Path A |
| :--- | :--- | :--- |
| **Autonomous Creation** | Abstract Factory | Dynamically bootstraps Pharmacy, Food, or Emergency kiosks. |
| **Operational Modes** | State Pattern | Adapts behavior (Active, Power-Saving, Emergency, Maintenance). |
| **Dynamic Pricing** | Strategy Pattern | Swaps pricing policies instantly based on system state. |
| **Fault Tolerance** | Memento | Ensures "Atomic Transactions" by rolling back on hardware failure. |
| **Self-Healing** | Chain of Responsibility | Autonomous recovery sequence (Retry → Recalibrate → Alert). |
| **Smart City Sync** | Observer | Real-time event broadcasting to monitoring centers. |
| **Standardized Ops** | Facade | Provides a unified interface for the frontend and users. |
| **Modular Logic** | Command | Encapsulates all retail actions (Purchase, Refund, Restock). |

---

## ✨ Standout Engineering Flavors

### 🛡️ Transaction Atomicity (Memento)
The system creates a state snapshot before every purchase. If a hardware timeout or payment failure occurs mid-transaction, the system performs an **autonomous rollback**, restoring inventory and refunding virtual credits.

### 🔒 Thread-Safe Reservations
Designed for high-density smart cities, the `InventoryManager` uses **Mutex Locks** and a **Reservation System**. It calculates "Derived Stock" to ensure zero over-selling even during simultaneous concurrent purchase requests.

### 🧠 Intelligent Operational Status
Kiosk health isn't a simple flag; it's a **derived attribute**. The system computes its status at runtime by cross-referencing hardware diagnostics, connectivity, and current system mode.

---

## 📂 Project Structure

- `core/`: Transaction orchestration and validation.
- `factory/`: Abstract Factory for kiosk initialization.
- `state/` & `pricing/`: Adaptation logic for modes and strategies.
- `hardware/` & `recovery/`: Hardware abstraction and self-healing chains.
- `observer/`: Priority-based event bus for city-wide sync.
- `persistence/`: CSV-based repositories for state and logs.
- `frontend/`: Real-time management dashboard (Glassmorphism UI).

---

## 🚀 Getting Started

### 1. Backend Simulation (C++)
To run the full suite of 13+ demonstration scenarios (including hardware failures, emergency overrides, and concurrent races):

```bash
# Compile
g++ -std=c++17 main.cpp -o aura.exe

# Run
./aura.exe
```

### 2. Visual Management Dashboard (Frontend)
To see the system logic visualized in a professional real-time dashboard:
1. Navigate to the `frontend/` folder.
2. Open `index.html` in any modern web browser.
3. Use the **Scenario Buttons** to trigger backend logic simulations (Normal, Emergency, Latency, etc.).

---

## 📊 Persistence & Logs
The system maintains a real-world data footprint in the `data/` directory:
- **`monitoring_audit.log`**: Every critical system event is logged with a timestamp for city situational awareness.
- **`*_inventory.csv`**: Real-time stock levels persisted across system restarts.
- **`*_transactions.csv`**: Full audit trail of all financial operations.

---

**Lazy Constructors - IT620 Project**  
*Building resilient infrastructure for the future city.*

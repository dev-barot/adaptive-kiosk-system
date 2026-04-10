# Adaptive Autonomous Kiosk System

## 📌 Project Overview
This project is a partial implementation of an **Adaptive Autonomous Kiosk System** developed as part of an academic assignment (Subtask 2).

The system is designed using multiple **object-oriented design patterns** and demonstrates adaptive behavior through a **console-based simulation**.

The goal of this stage is to validate the architecture and show how different components interact dynamically.

---

## 🧠 Implemented Design Patterns

The following design patterns are implemented in the system:

- **State Pattern**
  - Handles different kiosk modes: Active, Power-Saving, Maintenance, Emergency

- **Strategy Pattern**
  - Dynamic pricing based on system conditions (Standard, Discount, Emergency)

- **Observer Pattern**
  - Event-driven system using EventManager and subscribers

- **Composite Pattern**
  - Supports individual products and bundled products uniformly

- **Singleton Pattern**
  - CentralRegistry maintains global system state

- **Adapter Pattern**
  - Payment interfaces (UPI, Card, Wallet)

- **Command & Memento (Structure Level)**
  - Included in design for transaction handling and rollback (partial implementation)

---

## ⚙️ Simulation Features

The system includes a working console simulation that demonstrates:

- State transitions (Active → Emergency → Maintenance)
- Dynamic pricing updates
- Inventory management and stock updates
- Event triggering and notification
- Product bundles and price calculation

---

## 📁 Project Structure
adaptive-kiosk-system/
│
├── core/
├── inventory/
├── pricing/
├── product/
├── state/
├── observer/
├── main.cpp
├── README.md

---

## ▶️ How to Run

### 1. Compile the project
```bash
g++ main.cpp -o kiosk
kiosk.exe
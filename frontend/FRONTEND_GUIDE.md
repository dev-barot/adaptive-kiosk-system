# 🖥️ Aura OS Visual Management Dashboard
### **Frontend Interface for Plan A Implementation**

The Aura OS Frontend serves as a real-time visualization layer for the adaptive autonomous system logic. While the backend handles the complex multi-threaded operations, this dashboard allows an evaluator to trigger and observe those scenarios in a clean, human-readable format.

---

## 🎨 Design Philosophy
The UI follows a **Glassmorphism Design Language**:
- **Transparency & Frost**: Modern aesthetic using backdrop-filters.
- **Dynamic Signals**: Color-coded status lights (Green: Success, Red: Error, Blue: Processing).
- **Responsive Inventory**: Real-time stock bars that deplete as you interact.

---

## 🕹️ How to Use the Dashboard

### 1. The Scenario Selector (Top Panel)
Clicking these buttons reconfigures the system's "Mental State" without reloading:
- **Normal Purchase**: Standard flow, full price, no limits.
- **Emergency Protocol**: Immediately swaps to **Emergency State**.
    - Prices drop by 50%.
    - A Quantity Cap (Max 2 units) is enforced by the RuleEngine.
- **Hardware Latency Test**: Simulates a slow dispenser response.
    - If it exceeds the timeout threshold, you will witness the **Autonomous Rollback** in action.
- **Concurrent Race**: Simulates two users trying to buy the same limited stock at the same time.
    - Shows how the **Reservation Lock** blocks the second user.

### 2. Transaction Controls (Center Panel)
- **Select Product**: Pick what to buy.
- **Quantity**: Input units. Note: In Emergency Mode, values > 2 will be rejected by the frontend validation logic.
- **Confirm Purchase**: Start the sequence.

### 3. Logic Monitoring (Bottom Panels)
- **Event Feed**: High-level system notifications (e.g., "Payment Processed", "Low Stock Warning").
- **Detailed Audit Log**: A "developer-view" of exact timestamps and internal status transitions.

---

## 🧪 Key "Plan A" Behaviors to Observe

1.  **The Rollback Flow**: 
    - Trigger "Hardware Latency Test".
    - Click "Confirm Purchase".
    - Watch the **Rollback Signal** turn blue.
    - Observe that the stock is restored to its previous value because the hardware failed to dispense.
2.  **Adaptive Pricing**:
    - Toggle between "Normal" and "Emergency" scenarios.
    - Watch the "Price" labels in the Inventory list update dynamically.
3.  **The Concurrency Lock**:
    - Click "Concurrent Race".
    - You will see two logs appear simultaneously: one saying "LOCK ACQUIRED" and the other "LOCK DENIED".

---

## 🛠️ Technical Details
- **Technology**: Vanilla HTML5, CSS3 (Modern Flex/Grid), and ES6+ JavaScript.
- **State Management**: Implements a mini-version of the backend's State and Observer patterns to ensure UI consistency.
- **No Dependencies**: Runs entirely in the browser without needing `npm` or `node_modules`.

---

*Part of the Aura Retail OS Suite*

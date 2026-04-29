const initialInventory = {
  water: { label: "Water (Essential)", stock: 12, max: 12, price: 50, critical: true },
  food: { label: "Ration Pack", stock: 8, max: 8, price: 110, critical: true },
  medkit: { label: "Emergency Medkit", stock: 5, max: 5, price: 180, critical: true },
  battery: { label: "Energy Cell", stock: 6, max: 6, price: 75, critical: false }
};

const state = {
  mode: "ACTIVE",
  status: "ONLINE",
  pricePolicy: "Standard",
  inventory: structuredClone(initialInventory),
  events: [],
  audit: [],
  eventCount: 0,
  txn: 1,
  walletBalance: 1200,
  running: false,
  selectedScenario: "normal",
  hardware: {
    dispenser: true,
    payment: true,
    refrigeration: true
  }
};

const els = {
  modeText: document.querySelector("#modeText"),
  statusText: document.querySelector("#statusText"),
  screenMode: document.querySelector("#screenMode"),
  screenMessage: document.querySelector("#screenMessage"),
  transactionTitle: document.querySelector("#transactionTitle"),
  transactionBadge: document.querySelector("#transactionBadge"),
  priceBadge: document.querySelector("#priceBadge"),
  inventoryList: document.querySelector("#inventoryList"),
  eventFeed: document.querySelector("#eventFeed"),
  eventCount: document.querySelector("#eventCount"),
  auditLog: document.querySelector("#auditLog"),
  auditBadge: document.querySelector("#auditBadge"),
  configTitle: document.querySelector("#configTitle"),
  purchaseFieldset: document.querySelector("#purchaseFieldset"),
  productSelect: document.querySelector("#productSelect"),
  quantityInput: document.querySelector("#quantityInput"),
  walletBalance: document.querySelector("#walletBalance"),
  confirmPurchaseBtn: document.querySelector("#confirmPurchaseBtn"),
  paymentSignal: document.querySelector("#paymentSignal"),
  hardwareSignal: document.querySelector("#hardwareSignal"),
  rollbackSignal: document.querySelector("#rollbackSignal"),
  tray: document.querySelector("#dispenseTray"),
  productOutput: document.querySelector("#productOutput"),
  productOutputLabel: document.querySelector("#productOutputLabel"),
  buttons: document.querySelectorAll(".scenario")
};

const wait = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

function transactionId() {
  return `TXN-${String(state.txn++).padStart(3, "0")}`;
}

function setMode(mode) {
  state.mode = mode;
  state.pricePolicy = mode === "EMERGENCY" ? "Emergency (-50%)" : (mode === "POWER_SAVING" ? "Discount (-20%)" : "Standard");
  els.modeText.textContent = mode;
  els.screenMode.textContent = mode;
  els.priceBadge.textContent = state.pricePolicy;
}

function setStatus(status) {
  state.status = status;
  els.statusText.textContent = status;
}

function configureScenario(name, buttonId) {
  if (state.running) return;
  state.selectedScenario = name;
  
  els.buttons.forEach(b => b.classList.remove("active"));
  document.querySelector(buttonId).classList.add("active");

  const config = {
    normal: { title: "Standard Purchase", mode: "ACTIVE", qty: 2 },
    emergency: { title: "Emergency Protocol", mode: "EMERGENCY", qty: 3 },
    timeout: { title: "Hardware Latency Test", mode: "EMERGENCY", qty: 1 }
  }[name];

  els.configTitle.textContent = config.title;
  setMode(config.mode);
  setStatus("ONLINE");
  els.quantityInput.value = config.qty;
  
  clearSteps();
  clearSignals();
  els.screenMessage.textContent = "Awaiting parameters";
  els.transactionBadge.textContent = "Ready";
  els.auditBadge.textContent = "Passive";
}

function renderInventory() {
  els.inventoryList.innerHTML = Object.entries(state.inventory).map(([id, item]) => {
    const pct = Math.round((item.stock / item.max) * 100);
    const price = computePrice(item.price);
    return `
      <div class="inventory-item">
        <div class="inventory-top">
          <strong>${item.label}</strong>
          <span>Rs.${price} | ${item.stock} Units</span>
        </div>
        <div class="stock-bar"><span style="width:${pct}%"></span></div>
      </div>
    `;
  }).join("");
}

function computePrice(base) {
  if (state.mode === "EMERGENCY") return base * 0.5;
  if (state.mode === "POWER_SAVING") return base * 0.8;
  return base;
}

function pushEvent(type, detail, level = "info") {
  state.eventCount++;
  state.events.unshift({ type, detail, level });
  state.events = state.events.slice(0, 15);
  renderEvents();
}

function renderEvents() {
  els.eventCount.textContent = `${state.eventCount} Events`;
  els.eventFeed.innerHTML = state.events.map(ev => `
    <div class="event ${ev.level}">
      <strong>${ev.type}</strong>
      <div class="muted">${ev.detail}</div>
    </div>
  `).join("");
}

function audit(message, status = "") {
  state.audit.unshift({ message, status });
  state.audit = state.audit.slice(0, 10);
  els.auditLog.innerHTML = state.audit.map(a => `
    <div class="audit ${a.status}">${a.message}</div>
  `).join("");
}

function clearSteps() {
  document.querySelectorAll(".step").forEach(s => s.classList.remove("live", "failed"));
}

function markStep(name, type = "live") {
  const el = document.querySelector(`[data-step="${name}"]`);
  if (el) el.classList.add(type);
}

function clearSignals() {
  [els.paymentSignal, els.hardwareSignal, els.rollbackSignal].forEach(s => s.classList.remove("live", "error"));
  els.tray.classList.remove("dispensing");
  els.productOutput.className = "product-output";
  els.productOutputLabel.textContent = "Product";
}

function showDispensedProduct(itemId, itemLabel, qty) {
  els.tray.classList.remove("dispensing");
  els.productOutput.className = `product-output ${itemId}`;
  els.productOutputLabel.textContent = `${itemLabel} x${qty}`;
  void els.tray.offsetWidth;
  els.tray.classList.add("dispensing");
}

function setControlsLocked(locked) {
  state.running = locked;
  els.purchaseFieldset.disabled = locked;
  els.buttons.forEach(button => {
    button.disabled = locked;
  });
  document.querySelector("#resetBtn").disabled = false;
}

async function purchaseFlow({ itemId, qty, failure = "NONE", delay = 0 }) {
  setControlsLocked(true);
  
  const id = transactionId();
  const item = state.inventory[itemId];
  const total = computePrice(item.price) * qty;
  
  els.transactionTitle.textContent = `${id} | ${item.label} x${qty}`;
  els.transactionBadge.textContent = "Processing";
  els.screenMessage.textContent = "Validating RuleEngine...";
  audit(`${id} STARTED`, "info");
  await wait(600);

  if (!Number.isFinite(qty) || qty <= 0) {
    markStep("validated", "failed");
    pushEvent("PurchaseDenied", "Quantity must be greater than zero.", "warning");
    audit(`${id} DENIED: Invalid quantity`, "fail");
    els.screenMessage.textContent = "Invalid Quantity";
    els.transactionBadge.textContent = "Denied";
    finishPurchase();
    return;
  }

  // Derived Attribute Check
  if (state.mode === "EMERGENCY" && qty > 2) {
    markStep("validated", "failed");
    pushEvent("PurchaseDenied", `Emergency limit (2) exceeded for ${item.label}.`, "warning");
    audit(`${id} DENIED: Limit`, "fail");
    els.screenMessage.textContent = "Limit Exceeded";
    els.transactionBadge.textContent = "Denied";
    finishPurchase();
    return;
  }

  markStep("validated");
  audit(`${id} VALIDATED`, "success");
  await wait(500);

  // Reservation Check
  if (item.stock < qty) {
    markStep("reserved", "failed");
    pushEvent("LowStockEvent", `Insufficient stock for ${item.label}.`, "warning");
    audit(`${id} FAILED: Stock`, "fail");
    els.screenMessage.textContent = "Stock Unavailable";
    els.transactionBadge.textContent = "Denied";
    finishPurchase();
    return;
  }

  item.stock -= qty; // Atomic Reservation
  renderInventory();
  markStep("reserved");
  audit(`${id} RESERVED`, "success");
  els.screenMessage.textContent = "Authorizing Payment...";
  await wait(600);

  // Payment Adapter
  els.paymentSignal.classList.add("live");
  markStep("paid");
  pushEvent("PaymentProcessed", `Wallet authorized Rs.${total}. Balance not deducted until commit.`);
  audit(`${id} WALLET_AUTHORIZED`, "success");
  await wait(600);

  // Hardware Interface
  els.hardwareSignal.classList.add("live");
  els.screenMessage.textContent = "Interfacing Hardware...";
  await wait(delay || 800);

  if (failure === "TIMEOUT") {
    els.hardwareSignal.classList.add("error");
    markStep("dispensed", "failed");
    pushEvent("HardwareFailureEvent", "Dispenser timeout detected. Starting recovery...", "priority");
    audit(`${id} HARDWARE_FAULT`, "fail");
    await wait(1000);

    // Rollback Memento
    item.stock += qty;
    els.rollbackSignal.classList.add("live");
    pushEvent("PaymentRolledBack", `Transaction atomicity preserved. Wallet unchanged at Rs.${state.walletBalance}.`, "warning");
    audit(`${id} ROLLED_BACK: stock restored and wallet unchanged`, "fail");
    els.screenMessage.textContent = "Transaction Failed";
    els.transactionBadge.textContent = "Rolled Back";
    setStatus("DEGRADED");
    renderInventory();
    finishPurchase();
    return;
  }

  showDispensedProduct(itemId, item.label, qty);
  markStep("dispensed");
  audit(`${id} DISPENSED`, "success");
  await wait(1000);

  // Commit
  state.walletBalance -= total;
  els.walletBalance.textContent = `Rs.${state.walletBalance}`;
  markStep("committed");
  pushEvent("TransactionCompleted", `Successfully committed ${item.label} x${qty}.`);
  audit(`${id} COMMITTED`, "success");
  els.screenMessage.textContent = "Transaction Complete";
  els.transactionBadge.textContent = "Success";
  finishPurchase();
}

function finishPurchase() {
  setControlsLocked(false);
}

async function concurrentScenario() {
  if (state.running) return;
  state.inventory.battery.stock = 2;
  renderInventory();
  
  const idA = transactionId();
  const idB = transactionId();
  els.transactionTitle.textContent = `RACE: ${idA} vs ${idB}`;
  els.screenMessage.textContent = "Concurrent requests received";
  audit("CONCURRENCY RACE DETECTED", "warning");
  await wait(800);

  markStep("validated");
  audit("LOCK ACQUIRED by " + idA, "success");
  audit("LOCK DENIED to " + idB, "fail");
  
  pushEvent("PurchaseValidated", "User A acquired reservation lock.");
  pushEvent("PurchaseDenied", "User B blocked by active lock.", "warning");
  
  await purchaseFlow({ itemId: "battery", qty: 2 });
}

function resetSystem() {
  state.inventory = structuredClone(initialInventory);
  state.events = [];
  state.audit = [];
  state.eventCount = 0;
  state.txn = 1;
  state.walletBalance = 1200;
  setStatus("ONLINE");
  setMode("ACTIVE");
  clearSteps();
  clearSignals();
  renderInventory();
  renderEvents();
  els.auditLog.innerHTML = "";
  els.walletBalance.textContent = `Rs.1200`;
  els.screenMessage.textContent = "System Ready";
  els.transactionTitle.textContent = "Awaiting Request";
  els.transactionBadge.textContent = "Idle";
  els.auditBadge.textContent = "Passive";
}

document.querySelector("#normalBtn").addEventListener("click", () => configureScenario("normal", "#normalBtn"));
document.querySelector("#emergencyBtn").addEventListener("click", () => configureScenario("emergency", "#emergencyBtn"));
document.querySelector("#timeoutBtn").addEventListener("click", () => configureScenario("timeout", "#timeoutBtn"));
document.querySelector("#concurrentBtn").addEventListener("click", concurrentScenario);
els.confirmPurchaseBtn.addEventListener("click", () => purchaseFlow({
  itemId: els.productSelect.value,
  qty: parseInt(els.quantityInput.value),
  failure: state.selectedScenario === "timeout" ? "TIMEOUT" : "NONE",
  delay: state.selectedScenario === "timeout" ? 1500 : 0
}));
document.querySelector("#resetBtn").addEventListener("click", resetSystem);

resetSystem();
renderInventory();

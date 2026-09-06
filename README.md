# CyberEdge-001 firmware — architecture skeleton

FreeRTOS on ESP32-S3 (PlatformIO, Arduino framework). This is the
structural scaffold from your phase 1–3 plan, wired end-to-end with
placeholders — nothing here does real driver I/O or real attacks yet.
What it does prove out: state machine → guardrail → module dispatch → IPC
back to UI, as one working, testable loop.

## Data flow

```
UI (touch input)
  -> ipc::uiSelectQueue
  -> state_machine::task()                 [only writer of startRequestQueue]
       -> guardrail::evaluate()            [synchronous, single choke point]
            -> mac_whitelist::isAuthorized()   (WiFi/BT modules)
            -> rfid_policy::isCloneable()      (RFID module)
       -> ipc::startRequestQueue
  -> module_registry::dispatcherTask()     [only reader of startRequestQueue]
       -> per-module command queue
  -> module task (wifi_deauth / evil_twin / bt_deauth / rfid_clone)
  -> ipc::resultQueue
  -> state_machine::task()
  -> ipc::uiNotifyQueue
  -> ui_manager::task() redraw
```

## BadUSB status

Removed from V1 scope, but not deleted from the codebase:
- `ModuleId::BADUSB_HID` stays in the enum so switch statements remain
  exhaustive (compiler catches you if you forget a case later).
- `module_registry::init()` only registers it if `ENABLE_BADUSB` is 1
  (default 0 in `platformio.ini`).
- `guardrail::evaluate()` hard-denies it independently of the registry
  flag — so even a build-flag mistake can't authorize it.

Re-adding it later means: write `badusb_hid_task`, flip `ENABLE_BADUSB`
to 1, and — separately — design its actual guardrail (arm switch /
on-device payload confirmation / allowlisted payloads, discussed in
chat). Don't just flip the flag without that third piece.

## Three additions in this version

1. **RFID ownership check (`safety/rfid_whitelist.*`).** Card *type*
   (SAK) and card *ownership* (UID) are now two independent checks in
   `guardrail::evaluate()` — both must pass. Previously any plain
   Mifare card would clone; now only cards whose UID was explicitly
   registered via `rfid_whitelist::addEntry()` are cloneable. Mirrors
   `mac_whitelist` exactly (same RAM-only caveat, same TODO on NVS
   persistence).

2. **Hard timeout on every module (`core/module_support.h`).**
   `runPlaceholderAttackLoop()` computes a deadline from
   `ATTACK_MAX_DURATION_MS` (default 30s, in `config.h`) and every
   module's loop respects it — nothing can run unbounded. Currently a
   stub that "succeeds" on its first iteration; when you replace a
   module's TODO with real logic, keep checking the deadline between
   iterations rather than doing one long blocking operation.

3. **On-device explainer / teaching layer (`ipc::explainerQueue`,
   `ModuleExplainerEvent`).** Separate bus from the pass/fail result —
   each module posts STARTING / MECHANISM / SUCCEEDED / FAILED text at
   the relevant points, and `guardrail` denials post a FAILED-phase
   explainer too (so a block is a teaching moment, not just an error
   code). All text is `PLACEHOLDER:`-prefixed in every module and in
   `state_machine.cpp::explainerTextFor()` — search for that prefix
   when you're ready to write the real copy. `ui_manager::showExplainer()`
   is currently a no-op stub; wire it to a real panel once you pick
   TFT_eSPI or LVGL (see chat for that discussion).

4. **Audit log to flash (`safety/audit_log.*`, LittleFS).** Every
   guardrail denial and every completed module run is appended to
   `/audit.log` as a CSV-ish line: timestamp (seconds since boot —
   TODO: swap for real wall-clock once NTP/RTC exists), module,
   verdict/status, target MAC or RFID UID (hex), duration in ms. This
   is what backs the "accountable, not just restricted" framing —
   worth citing directly in your paper. Requires
   `board_build.filesystem = littlefs` in `platformio.ini` (already
   added). Log rotation past `AUDIT_LOG_MAX_BYTES` is a marked TODO —
   currently grows unbounded.

   **Bug caught while wiring this in:** `handleResult()` previously
   fired on *every* `ModuleResultEvent`, including each module's
   intermediate `STARTED` message — which would have cleared
   `BIT_MODULE_BUSY` and logged a near-zero-duration audit entry the
   instant a module started, not when it actually finished. Now gated
   on `isTerminal()` (COMPLETED/ABORTED/FAILED only). Worth knowing
   about even though it's already fixed here, since it's the kind of
   bug that only becomes visible once you're actually reading the
   audit trail.

## What's real vs. placeholder right now

Real (structural, compiles and runs as-is once you fill in board id):
- FreeRTOS task topology and all IPC queues/event group
- State machine transitions
- Guardrail dispatch logic and both policy checks (MAC whitelist,
  RFID SAK classification)
- Module registry + single-instance dispatch enforcement

Placeholder (explicitly marked `TODO` in-file):
- All four module tasks report `COMPLETED` without touching hardware
- Display/RFID/nRF drivers in `drivers/drivers.cpp` are no-ops
- `StartRequest` payload (`targetMac`, `rfidSak`) is zeroed in
  `state_machine.cpp::handleSelection()` — wire this to the real
  consent-entry UI next, which is also why `mac_whitelist` currently
  denies everything by default (no entries added yet)
- MAC whitelist is RAM-only; NVS persistence noted as an open decision,
  not an oversight

## Probable next steps

1. **Phase 1 (drivers):** fill in `drivers/drivers.cpp` against
   actual display/RFID/nRF part numbers and libraries; nothing else in
   this tree needs to change to accommodate that.
2. Wire the UI's consent-entry screen to populate `StartRequest` in
   `state_machine.cpp` instead of the current zeroed placeholder, and
   add a UI flow for `mac_whitelist::addEntry()`.
3. **Phase 2/IPC split** is already reflected here — the
   queue/event-group topology in `core/ipc.*` is the bus; treat any
   future module as "implement `ModuleTaskFn`, register it," not as a
   reason to touch the core.
4. Replace one module's `TODO` (start with `rfid_clone`, since your
   hardware/library familiarity is probably furthest along there) with
   real logic, and use it to validate the whole loop against real
   hardware before doing the rest.

#pragma once
#include <cstdint>

// ---- System states -------------------------------------------------------
enum class SystemState : uint8_t {
    BOOT,
    MAIN_MENU,
    MODULE_SELECT,
    CONSENT_CHECK,      // guardrail gate is being evaluated
    ATTACK_RUNNING,
    RESULT,
    ERROR
};

// ---- Module identifiers ---------------------------------------------------
// BadUSB kept in the enum (so code that switches on ModuleId stays exhaustive)
// but never registered in module_registry while ENABLE_BADUSB is 0.
enum class ModuleId : uint8_t {
    NONE = 0,
    WIFI_DEAUTH,
    EVIL_TWIN,
    BT_DEAUTH,
    RFID_CLONE,
    BADUSB_HID
};

// ---- UI -> State machine: user picked a module ----------------------------
struct UiSelectEvent {
    ModuleId module;
};

// ---- State machine -> Guardrail: request to authorize + start a module ----
struct StartRequest {
    ModuleId module;
    // Consent payload — populated by the UI before the request is issued.
    // Only the fields relevant to the target module's policy check are read.
    uint8_t  targetMac[6];      // for WIFI_DEAUTH / BT_DEAUTH / EVIL_TWIN
    uint16_t rfidSak;           // for RFID_CLONE (card type check)
    uint8_t  rfidUid[10];       // for RFID_CLONE (ownership check) — 10 bytes
                                 // covers single/double/triple-size UIDs
    uint8_t  rfidUidLen;
};

// ---- Guardrail decision -----------------------------------------------
enum class GuardrailResult : uint8_t {
    AUTHORIZED,
    DENIED_NOT_WHITELISTED,
    DENIED_PROTECTED_CARD_TYPE,
    DENIED_CARD_NOT_WHITELISTED,
    DENIED_MODULE_DISABLED,
    DENIED_UNKNOWN
};

// ---- Module -> State machine: run result -----------------------------
enum class ModuleStatus : uint8_t {
    STARTED,
    RUNNING,
    COMPLETED,
    ABORTED,
    FAILED
};

struct ModuleResultEvent {
    ModuleId     module;
    ModuleStatus status;
    // Small fixed buffer for a human-readable status string shown on screen.
    char         message[48];
};

// ---- Module -> State machine/UI: educational commentary --------------
// Separate from ModuleResultEvent on purpose: this is teaching content
// meant to render in its own explainer panel, not squeezed into the same
// short status line. A module posts one of these at each phase below.
enum class ExplainerPhase : uint8_t {
    STARTING,    // what's about to run
    MECHANISM,   // why this attack works against this target class
    SUCCEEDED,   // what a successful outcome means
    FAILED       // likely causes of failure
};

struct ModuleExplainerEvent {
    ModuleId       module;
    ExplainerPhase phase;
    char           text[160];   // placeholder teaching text — edit freely
};

#include "state_machine.h"
#include "ipc.h"
#include "module_support.h"
#include "../safety/guardrail.h"
#include "../safety/mac_whitelist.h"
#include "../safety/rfid_whitelist.h"
#include "../safety/audit_log.h"
#include "../modules/module_registry.h"

// Placeholder teaching text shown when the guardrail denies a request —
// itself a teaching moment ("this was blocked, and here's why"), not just
// a bare error code. Edit these to whatever level of detail suits the UI.
static const char* explainerTextFor(GuardrailResult verdict) {
    switch (verdict) {
        case GuardrailResult::DENIED_NOT_WHITELISTED:
            return "PLACEHOLDER: This target's MAC isn't on your registered "
                   "device list. Register a device you own from the consent "
                   "screen before running this module against it.";
        case GuardrailResult::DENIED_PROTECTED_CARD_TYPE:
            return "PLACEHOLDER: This card reports an ISO14443-4 SAK bit, "
                   "which marks payment/secure smartcards (EMV, DESFire, "
                   "access badges with real crypto) — cloning those is "
                   "blocked regardless of ownership.";
        case GuardrailResult::DENIED_CARD_NOT_WHITELISTED:
            return "PLACEHOLDER: This card's UID isn't on your registered "
                   "practice-card list. Register your own practice card "
                   "from the consent screen first.";
        case GuardrailResult::DENIED_MODULE_DISABLED:
            return "PLACEHOLDER: This module is disabled in this build.";
        default:
            return "PLACEHOLDER: Request denied for an unrecognized reason.";
    }
}

namespace state_machine {

static SystemState g_state = SystemState::BOOT;

// Only one module runs at a time (BIT_MODULE_BUSY enforces that), so a
// single "currently dispatched" record is enough to compute duration when
// the result comes back — no need for a table keyed by module.
static TickType_t   g_dispatchStartTick = 0;
static StartRequest g_dispatchedReq{};

static void notifyUi(SystemState s) {
    g_state = s;
    xQueueSend(ipc::uiNotifyQueue, &s, 0);
}

SystemState currentState() { return g_state; }

// Handles one UI selection end-to-end: consent check -> guardrail -> dispatch.
// This is the single funnel every attack module execution passes through.
static void handleSelection(const UiSelectEvent& sel) {
    notifyUi(SystemState::CONSENT_CHECK);

    // TODO: pull the actual consent payload (target MAC / RFID SAK+UID) from
    // the UI's consent-entry screen. Placeholder zeroed request for now —
    // the guardrail will correctly DENY this until real UI wiring exists.
    StartRequest req{};
    req.module = sel.module;

    GuardrailResult verdict = guardrail::evaluate(req);

    if (verdict != GuardrailResult::AUTHORIZED) {
        ModuleResultEvent res{};
        res.module = sel.module;
        res.status = ModuleStatus::ABORTED;
        snprintf(res.message, sizeof(res.message), "denied: code %d", (int)verdict);
        xQueueSend(ipc::resultQueue, &res, 0);
        module_support::postExplainer(sel.module, ExplainerPhase::FAILED,
                                       explainerTextFor(verdict));
        audit_log::recordDenied(sel.module, verdict);
        notifyUi(SystemState::RESULT);
        return;
    }

    g_dispatchStartTick = xTaskGetTickCount();
    g_dispatchedReq     = req;

    xEventGroupSetBits(ipc::systemEventGroup, ipc::BIT_MODULE_BUSY);
    notifyUi(SystemState::ATTACK_RUNNING);

    // Guardrail cleared it — this is the ONLY call site in the firmware
    // that is allowed to actually start a module.
    xQueueSend(ipc::startRequestQueue, &req, 0);
}

static bool isTerminal(ModuleStatus s) {
    return s == ModuleStatus::COMPLETED || s == ModuleStatus::ABORTED || s == ModuleStatus::FAILED;
}

// Only fires on a module's FINAL result. STARTED/RUNNING are live-status
// updates, not completions — treating them as terminal (as an earlier
// version of this function did) would clear BIT_MODULE_BUSY and write an
// audit log entry the instant a module started, not when it actually
// finished.
static void handleResult(const ModuleResultEvent& res) {
    if (!isTerminal(res.status)) {
        // TODO: forward res.message to the UI as a live "running..." line
        // if you want intermediate status visible, separate from the
        // explainer panel.
        return;
    }

    uint32_t durationMs = (xTaskGetTickCount() - g_dispatchStartTick) * portTICK_PERIOD_MS;
    audit_log::recordRun(res.module, g_dispatchedReq, durationMs, res.status);

    xEventGroupClearBits(ipc::systemEventGroup, ipc::BIT_MODULE_BUSY);
    notifyUi(SystemState::RESULT);
}

void task(void* pvParameters) {
    ipc::init();
    mac_whitelist::init();
    rfid_whitelist::init();
    audit_log::init();
    module_registry::init();     // registers every enabled module's task

    notifyUi(SystemState::MAIN_MENU);

    UiSelectEvent sel;
    ModuleResultEvent res;

    for (;;) {
        if (xQueueReceive(ipc::uiSelectQueue, &sel, pdMS_TO_TICKS(50)) == pdTRUE) {
            handleSelection(sel);
        }
        if (xQueueReceive(ipc::resultQueue, &res, 0) == pdTRUE) {
            handleResult(res);
        }
    }
}

} // namespace state_machine

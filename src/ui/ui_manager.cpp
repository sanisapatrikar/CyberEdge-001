#include "ui_manager.h"
#include "../core/ipc.h"
#include "../core/event_types.h"
#include "../drivers/drivers.h"

namespace ui_manager {

// TODO: replace with real redraw calls once the display driver + a menu
// widget library are wired in (Phase 4 of the plan).
static void redrawFor(SystemState s) {
    switch (s) {
        case SystemState::BOOT:           /* splash screen */          break;
        case SystemState::MAIN_MENU:      /* module list */            break;
        case SystemState::MODULE_SELECT:  /* consent-entry screen */   break;
        case SystemState::CONSENT_CHECK:  /* "checking..." spinner */  break;
        case SystemState::ATTACK_RUNNING: /* live status view */       break;
        case SystemState::RESULT:         /* result banner */          break;
        case SystemState::ERROR:          /* error screen */           break;
    }
}

// TODO: replace with a real explainer panel widget once the display/GUI
// library (e.g. LVGL) is wired in. This is the on-device teaching layer —
// what's running, why it works, why it succeeded or failed — separate from
// the short pass/fail banner driven by redrawFor(SystemState).
static void showExplainer(const ModuleExplainerEvent& ev) {
    // TODO: render ev.text in the explainer panel, tagged by ev.phase.
}

void task(void* pvParameters) {
    drivers::display::init();

    SystemState state;
    ModuleExplainerEvent explainerEv;
    for (;;) {
        if (xQueueReceive(ipc::uiNotifyQueue, &state, pdMS_TO_TICKS(20)) == pdTRUE) {
            redrawFor(state);
        }
        if (xQueueReceive(ipc::explainerQueue, &explainerEv, pdMS_TO_TICKS(20)) == pdTRUE) {
            showExplainer(explainerEv);
        }

        // TODO: poll touchscreen for user input here; on a menu selection,
        // build a UiSelectEvent and:
        //   xQueueSend(ipc::uiSelectQueue, &sel, 0);
    }
}

} // namespace ui_manager

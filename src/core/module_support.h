#pragma once
#include "event_types.h"
#include "ipc.h"
#include "../../include/config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstdio>

// Small shared helpers so every module follows the same teaching + timeout
// pattern instead of four slightly-different copies of the same loop.
namespace module_support {

inline void postExplainer(ModuleId module, ExplainerPhase phase, const char* text) {
    ModuleExplainerEvent ev{};
    ev.module = module;
    ev.phase  = phase;
    snprintf(ev.text, sizeof(ev.text), "%s", text);
    xQueueSend(ipc::explainerQueue, &ev, 0);
}

// Placeholder "do the attack" loop that respects ATTACK_MAX_DURATION_MS.
// Real modules should replace the body with actual attack iterations —
// keep checking xTaskGetTickCount() against the deadline between
// iterations so a real, longer-running attack still gets cut off on time.
// Returns true if it completed "successfully" before the deadline, false
// if it hit the timeout without a result.
inline bool runPlaceholderAttackLoop(uint32_t maxDurationMs = ATTACK_MAX_DURATION_MS) {
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(maxDurationMs);

    while (xTaskGetTickCount() < deadline) {
        // TODO: one real attack iteration goes here (e.g. transmit a
        // deauth burst, attempt one RFID read/write cycle). This stub
        // simulates a single iteration completing the attack immediately;
        // a real module will loop here, checking the deadline each pass,
        // and break early on its own success/failure condition instead of
        // always running to the timeout.
        vTaskDelay(pdMS_TO_TICKS(500));
        return true;
    }
    return false; // hit ATTACK_MAX_DURATION_MS without a result
}

} // namespace module_support

#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include "event_types.h"

// All cross-task communication funnels through these queues/groups.
// No task other than the state machine is allowed to write to
// startRequestQueue — that's what makes the guardrail a true choke point.

namespace ipc {

extern QueueHandle_t uiSelectQueue;      // UI            -> State machine
extern QueueHandle_t startRequestQueue;  // State machine  -> Guardrail/dispatch
extern QueueHandle_t resultQueue;        // Modules        -> State machine
extern QueueHandle_t uiNotifyQueue;      // State machine  -> UI (state changes)
extern QueueHandle_t explainerQueue;     // Modules        -> UI (teaching content, direct)

// Bit in this group is set while any module is actively running, enforcing
// "only one attack module executes at a time" without a shared mutex.
extern EventGroupHandle_t systemEventGroup;
constexpr EventBits_t BIT_MODULE_BUSY = (1 << 0);

void init();

} // namespace ipc

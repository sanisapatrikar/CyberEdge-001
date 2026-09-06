#pragma once
#include "event_types.h"

// Owns SystemState and is the ONLY code path allowed to post to
// ipc::startRequestQueue. UI code must go through postUiSelect(); it has
// no other way to trigger a module.
namespace state_machine {

void task(void* pvParameters);   // FreeRTOS task entry point
SystemState currentState();

} // namespace state_machine

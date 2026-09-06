#include <Arduino.h>
#include "core/state_machine.h"
#include "ui/ui_manager.h"
#include "../include/config.h"

void setup() {
    Serial.begin(115200);

    xTaskCreate(state_machine::task, "state_machine", STACK_SIZE_STATE_MACHINE,
                nullptr, TASK_PRIORITY_STATE_MACHINE, nullptr);

    xTaskCreate(ui_manager::task, "ui_manager", STACK_SIZE_UI_TASK,
                nullptr, TASK_PRIORITY_UI, nullptr);

    // module_registry::init() and ipc::init() are called from within
    // state_machine::task() at startup, so nothing else needs kicking off
    // here — setup()'s only job is bringing the two top-level tasks up.
}

void loop() {
    // Intentionally empty — everything runs in FreeRTOS tasks.
    vTaskDelay(portMAX_DELAY);
}

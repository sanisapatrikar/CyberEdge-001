#include "module_registry.h"
#include "module_interface.h"
#include "../core/ipc.h"
#include "../../include/config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>

// Placeholder module entry points — real attack logic goes here later.
// Each blocks on its own command queue and only acts once woken by the
// dispatcher below, which itself only forwards requests the guardrail
// already authorized.
extern void wifi_deauth_task(void* pvParameters);
extern void evil_twin_task(void* pvParameters);
extern void bt_deauth_task(void* pvParameters);
extern void rfid_clone_task(void* pvParameters);
#if ENABLE_BADUSB
extern void badusb_hid_task(void* pvParameters);
#endif

namespace module_registry {

namespace {

constexpr size_t MAX_MODULES = 8;
ModuleDescriptor s_descriptors[MAX_MODULES];
QueueHandle_t    s_cmdQueues[MAX_MODULES];
size_t           s_count = 0;

void registerModule(ModuleId id, const char* name, ModuleTaskFn fn) {
    if (s_count >= MAX_MODULES) return; // TODO: log overflow
    s_descriptors[s_count] = { id, name, fn, STACK_SIZE_MODULE_TASK, TASK_PRIORITY_MODULE };
    s_cmdQueues[s_count]   = xQueueCreate(1, sizeof(StartRequest));
    xTaskCreate(fn, name, STACK_SIZE_MODULE_TASK, s_cmdQueues[s_count], TASK_PRIORITY_MODULE, nullptr);
    s_count++;
}

int indexOf(ModuleId id) {
    for (size_t i = 0; i < s_count; i++) {
        if (s_descriptors[i].id == id) return (int)i;
    }
    return -1;
}

// Forwards guardrail-authorized requests to the target module's own queue.
// This is the only consumer of ipc::startRequestQueue.
void dispatcherTask(void* pvParameters) {
    StartRequest req;
    for (;;) {
        if (xQueueReceive(ipc::startRequestQueue, &req, portMAX_DELAY) == pdTRUE) {
            int idx = indexOf(req.module);
            if (idx >= 0) {
                xQueueSend(s_cmdQueues[idx], &req, 0);
            } else {
                ModuleResultEvent res{};
                res.module = req.module;
                res.status = ModuleStatus::FAILED;
                snprintf(res.message, sizeof(res.message), "module not registered");
                xQueueSend(ipc::resultQueue, &res, 0);
            }
        }
    }
}

} // namespace

void init() {
#if ENABLE_WIFI_DEAUTH
    registerModule(ModuleId::WIFI_DEAUTH, "wifi_deauth", wifi_deauth_task);
#endif
#if ENABLE_EVIL_TWIN
    registerModule(ModuleId::EVIL_TWIN, "evil_twin", evil_twin_task);
#endif
#if ENABLE_BT_DEAUTH
    registerModule(ModuleId::BT_DEAUTH, "bt_deauth", bt_deauth_task);
#endif
#if ENABLE_RFID_CLONE
    registerModule(ModuleId::RFID_CLONE, "rfid_clone", rfid_clone_task);
#endif
#if ENABLE_BADUSB
    registerModule(ModuleId::BADUSB_HID, "badusb_hid", badusb_hid_task);
#else
    // Deliberately not registered. guardrail::evaluate() also hard-denies
    // BADUSB_HID independently — belt and suspenders, not either/or.
#endif

    xTaskCreate(dispatcherTask, "module_dispatch", STACK_SIZE_MODULE_TASK, nullptr, TASK_PRIORITY_MODULE, nullptr);
}

} // namespace module_registry

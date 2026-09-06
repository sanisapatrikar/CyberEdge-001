#include "../core/event_types.h"
#include "../core/ipc.h"
#include "../core/module_support.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

using namespace module_support;

void bt_deauth_task(void* pvParameters) {
    auto cmdQueue = static_cast<QueueHandle_t>(pvParameters);
    StartRequest req;

    for (;;) {
        if (xQueueReceive(cmdQueue, &req, portMAX_DELAY) == pdTRUE) {
            ModuleResultEvent res{};
            res.module = ModuleId::BT_DEAUTH;
            res.status = ModuleStatus::STARTED;
            snprintf(res.message, sizeof(res.message), "bt_deauth: placeholder");
            xQueueSend(ipc::resultQueue, &res, 0);

            postExplainer(ModuleId::BT_DEAUTH, ExplainerPhase::STARTING,
                "PLACEHOLDER: Sending Bluetooth disconnect/deauth signalling "
                "at the target MAC via the nRF module.");
            postExplainer(ModuleId::BT_DEAUTH, ExplainerPhase::MECHANISM,
                "PLACEHOLDER: Fill in once the real bt_deauth logic is "
                "written — the mechanism differs depending on whether this "
                "targets classic BT or BLE.");

            // TODO: real nRF-module-driven BT/BLE deauth targeting
            // req.targetMac goes inside this helper's loop body.
            bool succeeded = runPlaceholderAttackLoop();

            if (succeeded) {
                postExplainer(ModuleId::BT_DEAUTH, ExplainerPhase::SUCCEEDED,
                    "PLACEHOLDER: describe what a successful disconnect "
                    "confirms about the target once the real attack exists.");
                res.status = ModuleStatus::COMPLETED;
                snprintf(res.message, sizeof(res.message), "bt_deauth: done (stub)");
            } else {
                postExplainer(ModuleId::BT_DEAUTH, ExplainerPhase::FAILED,
                    "PLACEHOLDER: list likely failure causes once the real "
                    "attack exists (e.g. pairing mode required, out of range).");
                res.status = ModuleStatus::ABORTED;
                snprintf(res.message, sizeof(res.message), "bt_deauth: timed out (stub)");
            }
            xQueueSend(ipc::resultQueue, &res, 0);
        }
    }
}

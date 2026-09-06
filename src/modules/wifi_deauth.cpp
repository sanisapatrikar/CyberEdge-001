#include "../core/event_types.h"
#include "../core/ipc.h"
#include "../core/module_support.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

using namespace module_support;

// pvParameters is this module's own command queue, wired up by
// module_registry::registerModule(). It only receives requests the
// guardrail already authorized — no direct path exists to reach this task.
void wifi_deauth_task(void* pvParameters) {
    auto cmdQueue = static_cast<QueueHandle_t>(pvParameters);
    StartRequest req;

    for (;;) {
        if (xQueueReceive(cmdQueue, &req, portMAX_DELAY) == pdTRUE) {
            ModuleResultEvent res{};
            res.module = ModuleId::WIFI_DEAUTH;
            res.status = ModuleStatus::STARTED;
            snprintf(res.message, sizeof(res.message), "wifi_deauth: placeholder");
            xQueueSend(ipc::resultQueue, &res, 0);

            postExplainer(ModuleId::WIFI_DEAUTH, ExplainerPhase::STARTING,
                "PLACEHOLDER: Sending 802.11 deauthentication frames to the "
                "target MAC to force it off the network.");
            postExplainer(ModuleId::WIFI_DEAUTH, ExplainerPhase::MECHANISM,
                "PLACEHOLDER: Pre-802.11w networks don't authenticate "
                "management frames, so a deauth frame can be spoofed as if "
                "sent by the legitimate AP.");

            // TODO: real deauth logic (esp_wifi_80211_tx with a crafted
            // deauth frame targeting req.targetMac) goes inside this
            // helper's loop body — see module_support.h.
            bool succeeded = runPlaceholderAttackLoop();

            if (succeeded) {
                postExplainer(ModuleId::WIFI_DEAUTH, ExplainerPhase::SUCCEEDED,
                    "PLACEHOLDER: Target disconnected — confirms it doesn't "
                    "enforce Protected Management Frames (802.11w).");
                res.status = ModuleStatus::COMPLETED;
                snprintf(res.message, sizeof(res.message), "wifi_deauth: done (stub)");
            } else {
                postExplainer(ModuleId::WIFI_DEAUTH, ExplainerPhase::FAILED,
                    "PLACEHOLDER: Likely causes — target enforces 802.11w, "
                    "target out of range, or reconnected faster than the "
                    "deauth rate.");
                res.status = ModuleStatus::ABORTED;
                snprintf(res.message, sizeof(res.message), "wifi_deauth: timed out (stub)");
            }
            xQueueSend(ipc::resultQueue, &res, 0);
        }
    }
}

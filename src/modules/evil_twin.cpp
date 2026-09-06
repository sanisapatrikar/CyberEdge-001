#include "../core/event_types.h"
#include "../core/ipc.h"
#include "../core/module_support.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

using namespace module_support;

void evil_twin_task(void* pvParameters) {
    auto cmdQueue = static_cast<QueueHandle_t>(pvParameters);
    StartRequest req;

    for (;;) {
        if (xQueueReceive(cmdQueue, &req, portMAX_DELAY) == pdTRUE) {
            ModuleResultEvent res{};
            res.module = ModuleId::EVIL_TWIN;
            res.status = ModuleStatus::STARTED;
            snprintf(res.message, sizeof(res.message), "evil_twin: placeholder");
            xQueueSend(ipc::resultQueue, &res, 0);

            postExplainer(ModuleId::EVIL_TWIN, ExplainerPhase::STARTING,
                "PLACEHOLDER: Broadcasting a SoftAP with the same SSID as "
                "the target network to lure the target device to connect.");
            postExplainer(ModuleId::EVIL_TWIN, ExplainerPhase::MECHANISM,
                "PLACEHOLDER: Clients auto-reconnect to the strongest "
                "signal broadcasting a known SSID, with no certificate "
                "pinning at the SSID level to tell real and cloned APs apart.");

            // TODO: real SoftAP clone of req.targetMac's associated SSID +
            // captive portal goes inside this helper's loop body.
            bool succeeded = runPlaceholderAttackLoop();

            if (succeeded) {
                postExplainer(ModuleId::EVIL_TWIN, ExplainerPhase::SUCCEEDED,
                    "PLACEHOLDER: Target associated with the twin AP — "
                    "confirms it has no cert-based network verification.");
                res.status = ModuleStatus::COMPLETED;
                snprintf(res.message, sizeof(res.message), "evil_twin: done (stub)");
            } else {
                postExplainer(ModuleId::EVIL_TWIN, ExplainerPhase::FAILED,
                    "PLACEHOLDER: Likely causes — target uses WPA2/3-"
                    "Enterprise with certificate validation, or the twin's "
                    "signal was weaker than the real AP's.");
                res.status = ModuleStatus::ABORTED;
                snprintf(res.message, sizeof(res.message), "evil_twin: timed out (stub)");
            }
            xQueueSend(ipc::resultQueue, &res, 0);
        }
    }
}

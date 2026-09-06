#include "../core/event_types.h"
#include "../core/ipc.h"
#include "../core/module_support.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

using namespace module_support;

void rfid_clone_task(void* pvParameters) {
    auto cmdQueue = static_cast<QueueHandle_t>(pvParameters);
    StartRequest req;

    for (;;) {
        if (xQueueReceive(cmdQueue, &req, portMAX_DELAY) == pdTRUE) {
            ModuleResultEvent res{};
            res.module = ModuleId::RFID_CLONE;
            res.status = ModuleStatus::STARTED;
            snprintf(res.message, sizeof(res.message), "rfid_clone: placeholder");
            xQueueSend(ipc::resultQueue, &res, 0);

            postExplainer(ModuleId::RFID_CLONE, ExplainerPhase::STARTING,
                "PLACEHOLDER: Reading the practice card's block data via "
                "the RFID module to prepare a clone.");
            postExplainer(ModuleId::RFID_CLONE, ExplainerPhase::MECHANISM,
                "PLACEHOLDER: Mifare Classic uses Crypto1, a cryptographically "
                "broken cipher, with no mutual authentication beyond it. "
                "That's exactly why the safety layer blocks ISO14443-4 "
                "cards (payment/secure) but allows this card type.");

            // TODO: real read/clone via the RFID driver goes inside this
            // helper's loop body. Also: re-derive SAK/UID from the live
            // scan here rather than trusting req.rfidSak/rfidUid alone —
            // those already passed guardrail::evaluate(), but a value
            // that crossed a queue boundary shouldn't be the sole guard
            // against a stale or swapped card at execution time.
            bool succeeded = runPlaceholderAttackLoop();

            if (succeeded) {
                postExplainer(ModuleId::RFID_CLONE, ExplainerPhase::SUCCEEDED,
                    "PLACEHOLDER: Clone written successfully — confirms the "
                    "card had no authentication beyond the broken cipher.");
                res.status = ModuleStatus::COMPLETED;
                snprintf(res.message, sizeof(res.message), "rfid_clone: done (stub)");
            } else {
                postExplainer(ModuleId::RFID_CLONE, ExplainerPhase::FAILED,
                    "PLACEHOLDER: Likely causes — card misaligned with the "
                    "reader, blank clone card not present, or the card used "
                    "non-default keys.");
                res.status = ModuleStatus::ABORTED;
                snprintf(res.message, sizeof(res.message), "rfid_clone: timed out (stub)");
            }
            xQueueSend(ipc::resultQueue, &res, 0);
        }
    }
}

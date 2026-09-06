#include "ipc.h"
#include "../../include/config.h"

namespace ipc {

QueueHandle_t uiSelectQueue     = nullptr;
QueueHandle_t startRequestQueue = nullptr;
QueueHandle_t resultQueue       = nullptr;
QueueHandle_t uiNotifyQueue     = nullptr;
QueueHandle_t explainerQueue    = nullptr;
EventGroupHandle_t systemEventGroup = nullptr;

void init() {
    uiSelectQueue     = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(UiSelectEvent));
    startRequestQueue = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(StartRequest));
    resultQueue       = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(ModuleResultEvent));
    uiNotifyQueue      = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(SystemState));
    explainerQueue    = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(ModuleExplainerEvent));
    systemEventGroup  = xEventGroupCreate();

    // TODO: assert all handles non-null; on ESP32-S3 this should never fail
    // unless heap is already exhausted, but fail loudly rather than silently
    // dropping IPC if it does.
}

} // namespace ipc

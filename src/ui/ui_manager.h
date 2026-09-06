#pragma once

// UI only ever does two things: (1) read ipc::uiNotifyQueue and redraw for
// the new SystemState, (2) post UiSelectEvent to ipc::uiSelectQueue when the
// user picks something. It has no other access to modules or the guardrail.
namespace ui_manager {

void task(void* pvParameters);

} // namespace ui_manager

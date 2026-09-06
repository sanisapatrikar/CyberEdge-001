#pragma once
#include "../core/event_types.h"

// Creates tasks for every module compiled in via config.h's ENABLE_* flags,
// and routes ipc::startRequestQueue entries to the right one.
namespace module_registry {

void init();

} // namespace module_registry

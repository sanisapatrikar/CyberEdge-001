#pragma once
#include "../core/event_types.h"

// Every attack module is a FreeRTOS task with this signature, created once
// at boot (module_registry::init) and normally blocked on its own command
// queue. It becomes runnable only when the state machine — after guardrail
// clearance — posts a StartRequest naming it.
using ModuleTaskFn = void (*)(void* pvParameters);

struct ModuleDescriptor {
    ModuleId      id;
    const char*   name;
    ModuleTaskFn  taskFn;
    uint32_t      stackSize;
    UBaseType_t   priority;
};

#pragma once
#include "../core/event_types.h"

// The guardrail is intentionally a plain function, not a task: it must run
// synchronously, in the state machine's own call stack, so there is no way
// for a module to be dispatched without first passing through evaluate().
namespace guardrail {

GuardrailResult evaluate(const StartRequest& req);

} // namespace guardrail

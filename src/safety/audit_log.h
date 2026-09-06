#pragma once
#include "../core/event_types.h"
#include <cstdint>

// Persists a timestamped record of every guardrail decision and every
// module run to flash (LittleFS). This is what turns the safety story
// from "we prevent misuse" into "we prevent AND make misuse accountable" —
// the guardrail can be argued about, but the log is a fact trail.
//
// Deliberately logs BOTH denials and authorized runs: a log that only
// records successful attacks would hide exactly the attempts you'd most
// want visible (someone trying against an unregistered target).
namespace audit_log {

void init();  // mounts LittleFS, creates the log file if absent

// Called immediately when guardrail::evaluate() denies a request.
void recordDenied(ModuleId module, GuardrailResult verdict);

// Called when a module run finishes (or times out), whether it
// succeeded, aborted, or failed.
void recordRun(ModuleId module, const StartRequest& req,
               uint32_t durationMs, ModuleStatus finalStatus);

} // namespace audit_log

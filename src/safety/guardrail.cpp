#include "guardrail.h"
#include "mac_whitelist.h"
#include "rfid_policy.h"
#include "rfid_whitelist.h"
#include "../../include/config.h"

namespace guardrail {

// Every module type maps to exactly one hardcoded check below. Adding a
// module means adding a case here — there is no "default allow" path.
GuardrailResult evaluate(const StartRequest& req) {
    switch (req.module) {

        case ModuleId::WIFI_DEAUTH:
        case ModuleId::EVIL_TWIN:
        case ModuleId::BT_DEAUTH:
#if !(ENABLE_WIFI_DEAUTH) && !(ENABLE_EVIL_TWIN) && !(ENABLE_BT_DEAUTH)
            return GuardrailResult::DENIED_MODULE_DISABLED;
#endif
            if (!mac_whitelist::isAuthorized(req.targetMac)) {
                return GuardrailResult::DENIED_NOT_WHITELISTED;
            }
            return GuardrailResult::AUTHORIZED;

        case ModuleId::RFID_CLONE:
#if !ENABLE_RFID_CLONE
            return GuardrailResult::DENIED_MODULE_DISABLED;
#endif
            // Two independent checks, both must pass: card TYPE (not a
            // payment/secure card) and card OWNERSHIP (UID is one the
            // owner deliberately registered). Type alone would authorize
            // cloning any random practice-type card, not just your own.
            if (!rfid_policy::isCloneable(req.rfidSak)) {
                return GuardrailResult::DENIED_PROTECTED_CARD_TYPE;
            }
            if (!rfid_whitelist::isAuthorized(req.rfidUid, req.rfidUidLen)) {
                return GuardrailResult::DENIED_CARD_NOT_WHITELISTED;
            }
            return GuardrailResult::AUTHORIZED;

        case ModuleId::BADUSB_HID:
            // Not just gated — entirely removed from V1. This branch exists
            // so the switch stays exhaustive; it must never return AUTHORIZED.
            return GuardrailResult::DENIED_MODULE_DISABLED;

        default:
            return GuardrailResult::DENIED_UNKNOWN;
    }
}

} // namespace guardrail

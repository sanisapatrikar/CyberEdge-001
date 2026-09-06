#include "rfid_policy.h"

namespace rfid_policy {

// Known-safe practice card SAK values (plain Mifare, no ISO14443-4 stack).
// Values per NXP AN10834 SAK coding.
constexpr uint16_t SAK_MIFARE_ULTRALIGHT = 0x00;
constexpr uint16_t SAK_MIFARE_CLASSIC_1K = 0x08;
constexpr uint16_t SAK_MIFARE_CLASSIC_4K = 0x18;

// Bit 5 (0x20) set in the SAK means "compliant with ISO/IEC 14443-4" —
// this is the layer EMV payment cards, DESFire, and most access-control
// smartcards run on. Treat ANY card asserting this bit as protected,
// rather than maintaining an allowlist of specific payment ATS/AID values
// — that list will always be incomplete and stale.
constexpr uint16_t SAK_ISO14443_4_BIT = 0x20;

bool isCloneable(uint16_t sak) {
    if (sak & SAK_ISO14443_4_BIT) {
        return false; // protected: ISO14443-4 stack present (payment/secure)
    }
    switch (sak) {
        case SAK_MIFARE_ULTRALIGHT:
        case SAK_MIFARE_CLASSIC_1K:
        case SAK_MIFARE_CLASSIC_4K:
            return true;
        default:
            // Unrecognized SAK: fail closed. Extend this list deliberately
            // as you test against real practice cards rather than
            // widening the default.
            return false;
    }
}

} // namespace rfid_policy

#pragma once
#include <cstdint>
#include <cstddef>

// Mirrors mac_whitelist, but for RFID card UIDs. Closes the gap where
// rfid_policy only checked *card type* (payment vs. practice) with no
// check that the card is one you actually own — SAK alone would let you
// clone any random plain Mifare card, not just your own practice cards.
// Same caveat as mac_whitelist applies: this is self-attested consent
// (owner scans + registers their own card), not cryptographic proof of
// ownership.
namespace rfid_whitelist {

constexpr size_t MAX_ENTRIES  = 16;
constexpr size_t MAX_UID_LEN  = 10; // covers single/double/triple UIDs

void init();

// Called from the UI's "register my practice card" flow.
bool addEntry(const uint8_t* uid, uint8_t uidLen);
bool removeEntry(const uint8_t* uid, uint8_t uidLen);

// Called only from guardrail::evaluate().
bool isAuthorized(const uint8_t* uid, uint8_t uidLen);

} // namespace rfid_whitelist

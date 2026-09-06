#pragma once
#include <cstdint>
#include <cstddef>

// Self-attested consent store: the owner enters a MAC and confirms
// ownership via the UI. This deters accidental/casual misuse — it is
// NOT cryptographic proof of ownership. Document that distinction
// honestly in the writeup rather than overclaiming what this prevents.
namespace mac_whitelist {

constexpr size_t MAX_ENTRIES = 16;

void init();

// Called from the UI's consent-entry screen after the owner confirms.
bool addEntry(const uint8_t mac[6]);
bool removeEntry(const uint8_t mac[6]);

// Called only from guardrail::evaluate().
bool isAuthorized(const uint8_t mac[6]);

} // namespace mac_whitelist

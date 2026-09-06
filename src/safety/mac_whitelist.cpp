#include "mac_whitelist.h"
#include <cstring>

namespace mac_whitelist {

static uint8_t s_entries[MAX_ENTRIES][6];
static bool    s_used[MAX_ENTRIES] = {false};

void init() {
    // TODO: load persisted entries from NVS (Arduino `Preferences` lib) so
    // whitelisted devices survive reboot. Currently RAM-only — cleared on
    // every power cycle, which is safe-by-default but inconvenient; decide
    // deliberately whether persistence is worth the added attack surface
    // (a device that "remembers" targets is a slightly different risk
    // profile than one that requires re-consent every session).
    for (bool& used : s_used) used = false;
}

bool addEntry(const uint8_t mac[6]) {
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        if (!s_used[i]) {
            memcpy(s_entries[i], mac, 6);
            s_used[i] = true;
            return true;
        }
    }
    return false; // table full
}

bool removeEntry(const uint8_t mac[6]) {
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        if (s_used[i] && memcmp(s_entries[i], mac, 6) == 0) {
            s_used[i] = false;
            return true;
        }
    }
    return false;
}

bool isAuthorized(const uint8_t mac[6]) {
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        if (s_used[i] && memcmp(s_entries[i], mac, 6) == 0) {
            return true;
        }
    }
    return false;
}

} // namespace mac_whitelist

#include "rfid_whitelist.h"
#include <cstring>

namespace rfid_whitelist {

namespace {
struct Entry {
    uint8_t uid[MAX_UID_LEN];
    uint8_t len;
    bool    used;
};
Entry s_entries[MAX_ENTRIES];
}

void init() {
    // TODO: same persistence decision as mac_whitelist — RAM-only for now,
    // cleared every reboot. Keep both consistent when you revisit this.
    for (auto& e : s_entries) e.used = false;
}

bool addEntry(const uint8_t* uid, uint8_t uidLen) {
    if (uidLen == 0 || uidLen > MAX_UID_LEN) return false;
    for (auto& e : s_entries) {
        if (!e.used) {
            memcpy(e.uid, uid, uidLen);
            e.len  = uidLen;
            e.used = true;
            return true;
        }
    }
    return false; // table full
}

bool removeEntry(const uint8_t* uid, uint8_t uidLen) {
    for (auto& e : s_entries) {
        if (e.used && e.len == uidLen && memcmp(e.uid, uid, uidLen) == 0) {
            e.used = false;
            return true;
        }
    }
    return false;
}

bool isAuthorized(const uint8_t* uid, uint8_t uidLen) {
    for (auto& e : s_entries) {
        if (e.used && e.len == uidLen && memcmp(e.uid, uid, uidLen) == 0) {
            return true;
        }
    }
    return false;
}

} // namespace rfid_whitelist

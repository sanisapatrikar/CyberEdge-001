#include "audit_log.h"
#include "../../include/config.h"
#include <FS.h>
#include <LittleFS.h>
#include <cstdio>
#include <ctime>

namespace audit_log {

namespace {

const char* moduleName(ModuleId m) {
    switch (m) {
        case ModuleId::WIFI_DEAUTH: return "wifi_deauth";
        case ModuleId::EVIL_TWIN:   return "evil_twin";
        case ModuleId::BT_DEAUTH:   return "bt_deauth";
        case ModuleId::RFID_CLONE:  return "rfid_clone";
        case ModuleId::BADUSB_HID:  return "badusb_hid";
        default:                    return "unknown";
    }
}

const char* verdictName(GuardrailResult v) {
    switch (v) {
        case GuardrailResult::AUTHORIZED:               return "AUTHORIZED";
        case GuardrailResult::DENIED_NOT_WHITELISTED:    return "DENIED_NOT_WHITELISTED";
        case GuardrailResult::DENIED_PROTECTED_CARD_TYPE:return "DENIED_PROTECTED_CARD_TYPE";
        case GuardrailResult::DENIED_CARD_NOT_WHITELISTED:return "DENIED_CARD_NOT_WHITELISTED";
        case GuardrailResult::DENIED_MODULE_DISABLED:    return "DENIED_MODULE_DISABLED";
        default:                                         return "DENIED_UNKNOWN";
    }
}

const char* statusName(ModuleStatus s) {
    switch (s) {
        case ModuleStatus::COMPLETED: return "COMPLETED";
        case ModuleStatus::ABORTED:   return "ABORTED";
        case ModuleStatus::FAILED:    return "FAILED";
        default:                      return "IN_PROGRESS";
    }
}

void macToHex(const uint8_t mac[6], char* out, size_t outLen) {
    snprintf(out, outLen, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void uidToHex(const uint8_t* uid, uint8_t len, char* out, size_t outLen) {
    size_t pos = 0;
    for (uint8_t i = 0; i < len && pos + 2 < outLen; i++) {
        pos += snprintf(out + pos, outLen - pos, "%02X", uid[i]);
    }
    out[pos] = '\0';
}

// TODO: this returns seconds-since-boot, not wall-clock time — the board
// has no RTC/NTP sync wired up yet. Swap for time(nullptr) once the device
// gets real time (NTP over the WiFi module, or an RTC module) so log
// entries are actually dateable, not just orderable.
uint32_t timestamp() {
    return millis() / 1000;
}

void appendLine(const char* line) {
    // TODO: rotate/truncate once the file exceeds AUDIT_LOG_MAX_BYTES —
    // currently unbounded growth. A simple rotate-to-.old-and-truncate on
    // exceeding the cap is enough; don't need anything fancier for V1.
    File f = LittleFS.open(AUDIT_LOG_PATH, "a");
    if (!f) return; // TODO: surface a mount/write failure to the UI/ERROR state
    f.println(line);
    f.close();
}

} // namespace

void init() {
    if (!LittleFS.begin(true /* formatOnFail */)) {
        // TODO: surface this to the UI as a real ERROR state instead of
        // silently continuing without logging — a safety feature that
        // fails silently isn't much of a safety feature.
        return;
    }
    if (!LittleFS.exists(AUDIT_LOG_PATH)) {
        File f = LittleFS.open(AUDIT_LOG_PATH, "w");
        if (f) f.close();
    }
}

void recordDenied(ModuleId module, GuardrailResult verdict) {
    char line[128];
    snprintf(line, sizeof(line), "%lu,%s,%s",
             (unsigned long)timestamp(), moduleName(module), verdictName(verdict));
    appendLine(line);
}

void recordRun(ModuleId module, const StartRequest& req,
               uint32_t durationMs, ModuleStatus finalStatus) {
    char macHex[18] = {0};
    char uidHex[24] = {0};
    macToHex(req.targetMac, macHex, sizeof(macHex));
    uidToHex(req.rfidUid, req.rfidUidLen, uidHex, sizeof(uidHex));

    char line[192];
    snprintf(line, sizeof(line), "%lu,%s,AUTHORIZED,%s,mac=%s,uid=%s,duration_ms=%lu",
             (unsigned long)timestamp(), moduleName(module), statusName(finalStatus),
             macHex, uidHex, (unsigned long)durationMs);
    appendLine(line);
}

} // namespace audit_log

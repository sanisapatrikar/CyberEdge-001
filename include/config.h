#pragma once

// ---- Module enable flags ----------------------------------------------
// BadUSB intentionally disabled for V1. The interface + registry slot stay
// in place so re-enabling later is a one-line flip, not a redesign.
#ifndef ENABLE_BADUSB
#define ENABLE_BADUSB 0
#endif

#define ENABLE_WIFI_DEAUTH 1
#define ENABLE_EVIL_TWIN   1
#define ENABLE_BT_DEAUTH   1
#define ENABLE_RFID_CLONE  1

// ---- Pin placeholders ---------------------------------------------------
// Fill in once you've locked the physical layout / module part numbers.
#define PIN_DISPLAY_CS   -1
#define PIN_DISPLAY_DC   -1
#define PIN_DISPLAY_RST  -1

#define PIN_RFID_CS      -1
#define PIN_RFID_RST     -1

#define PIN_NRF1_CE      -1
#define PIN_NRF1_CSN     -1
#define PIN_NRF2_CE      -1
#define PIN_NRF2_CSN     -1

// ---- FreeRTOS task sizing -----------------------------------------------
#define STACK_SIZE_STATE_MACHINE   4096
#define STACK_SIZE_MODULE_TASK     4096
#define STACK_SIZE_UI_TASK         3072

#define TASK_PRIORITY_STATE_MACHINE 3
#define TASK_PRIORITY_MODULE        2
#define TASK_PRIORITY_UI            2

#define IPC_QUEUE_LENGTH 8

// ---- Safety: hard time-box on every attack module -----------------------
// No module may run longer than this without operator re-authorization.
// Applies uniformly for now; split per-module if you find one type
// genuinely needs a different ceiling once real logic replaces the stubs.
#define ATTACK_MAX_DURATION_MS 30000

// ---- Audit log ------------------------------------------------------------
#define AUDIT_LOG_PATH        "/audit.log"
#define AUDIT_LOG_MAX_BYTES   (256 * 1024)   // rotate once the log passes this size

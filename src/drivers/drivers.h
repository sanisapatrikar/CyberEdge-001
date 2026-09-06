#pragma once

// Thin stubs so the rest of the firmware compiles against a stable
// interface before real driver libraries are wired in (Phase 1 of your
// plan). Fill each in against your actual part numbers/libraries.
namespace drivers {

namespace display {
    void init();     // TODO: TFT_eSPI / LVGL init against PIN_DISPLAY_*
}

namespace rfid {
    void init();      // TODO: MFRC522 init against PIN_RFID_*
    bool scan(uint16_t& outSak, uint8_t* outUid, uint8_t& outUidLen);
}

namespace nrf {
    void initModule1(); // TODO: RF24 init against PIN_NRF1_*
    void initModule2(); // TODO: RF24 init against PIN_NRF2_*
}

} // namespace drivers

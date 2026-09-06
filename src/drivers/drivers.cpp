#include "drivers.h"

namespace drivers {

namespace display {
    void init() {
        // TODO: replace with real display library init.
    }
}

namespace rfid {
    void init() {
        // TODO: MFRC522.PCD_Init() equivalent.
    }
    bool scan(uint16_t& outSak, uint8_t* outUid, uint8_t& outUidLen) {
        // TODO: real scan. Stub reports "no card present".
        outSak = 0;
        outUidLen = 0;
        return false;
    }
}

namespace nrf {
    void initModule1() {
        // TODO: real init.
    }
    void initModule2() {
        // TODO: real init.
    }
}

} // namespace drivers

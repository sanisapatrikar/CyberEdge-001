#pragma once
#include <cstdint>

// Classifies a scanned card by its SAK (Select Acknowledge) byte to decide
// whether cloning is permitted. Blocks known payment/secure card types;
// allows plain Mifare Classic/Ultralight practice cards.
namespace rfid_policy {

bool isCloneable(uint16_t sak);

} // namespace rfid_policy

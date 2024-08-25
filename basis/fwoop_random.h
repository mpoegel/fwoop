#pragma once

#include <cstdint>
#include <inttypes.h>

namespace fwoop {

class Random {
  private:
    static uint8_t *readRandomBytes(uint32_t numBytes);

  public:
    static uint16_t getUInt16();
    static uint32_t getUInt32();
};

} // namespace fwoop

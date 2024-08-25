#include <cstdint>
#include <fwoop_filereader.h>
#include <fwoop_random.h>
#include <sys/types.h>

namespace {
const std::string RANDOM_FN = "/dev/random";
}

namespace fwoop {

uint8_t *Random::readRandomBytes(uint32_t numBytes)
{
    FileReader fr(RANDOM_FN);
    if (fr.open() != 0) {
        // TODO handle error
        return nullptr;
    }
    uint8_t *res = fr.head(numBytes);
    fr.close();
    return res;
}

uint16_t Random::getUInt16()
{
    uint8_t *bytes = Random::readRandomBytes(2);
    if (bytes) {
        uint16_t r = (bytes[0] << 8) + bytes[1];
        delete[] bytes;
        return r;
    }
    return 0;
}

uint32_t Random::getUInt32()
{
    uint8_t *bytes = Random::readRandomBytes(4);
    if (bytes) {
        uint32_t r = (bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0];
        delete[] bytes;
        return r;
    }
    return 0;
}

} // namespace fwoop

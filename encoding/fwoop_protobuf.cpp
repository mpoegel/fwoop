#include <cstring>
#include <fwoop_protobuf.h>

#include <fwoop_log.h>

#include <cstdint>
#include <system_error>
#include <variant>

namespace fwoop {
namespace protobuf {

const char *ErrCategory::name() const noexcept { return "Protobuf"; }

std::string ErrCategory::message(int ev) const
{
    switch (static_cast<Message::DecodeResult>(ev)) {
    case Message::DecodeResult::UnknownField:
        return "unknown field";
    case Message::DecodeResult::WrongFieldType:
        return "wrong field type";
    case Message::DecodeResult::WantSubMessage:
        return "want sub message";
    default:
        return "unknown protobuf decode error";
    }
}

uint64_t Message::decodeUint64(uint8_t *buf, uint32_t bufLen)
{
    static constexpr uint8_t mask = 0b01111111;
    int64_t res = 0;
    for (unsigned int i = 0; i < bufLen; i++) {
        uint32_t shift = 0;
        if (i > 0) {
            shift = (i * 8) - 1;
        }
        uint64_t val = ((mask & buf[i]) << shift);
        res += val;
        Log::Debug("decodeUint64 ", int(buf[i]), " shift=", shift, " chunk=", val);
    }
    return res;
}

std::string Message::decodeString(uint8_t *buf, uint32_t bufLen) { return std::string(buf, buf + bufLen); }

std::error_code Message::decode(uint8_t *buf, uint32_t bufLen, uint32_t &bytesParsed)
{
    static constexpr uint8_t idMask = 0b01111000;
    static constexpr uint8_t typeMask = 0b00000111;
    static constexpr uint8_t msb = 0b10000000;
    uint32_t offset = 0;
    while (offset + 2 <= bufLen) {
        uint8_t id = (buf[offset] & idMask) >> 3;
        uint8_t valType = buf[offset++] & typeMask;
        uint32_t valLen = 0;
        while (buf[offset + valLen] & msb) {
            valLen++;
        }
        valLen++;
        Log::Debug("decode > id=", int(id), " valType=", int(valType), " valLen=", valLen);
        if (valType == 0) {
            auto val = decodeUint64(buf + offset, valLen);
            offset += valLen;
            setField(id, val);
        }
        if (valType == 2) {
            auto strLen = decodeUint64(buf + offset, valLen);
            offset += valLen;
            Log::Debug("decode > id=", int(id), " valType=", int(valType), " strLen=", strLen);
            auto val = decodeString(buf + offset, strLen);
            auto ec = setField(id, val);
            if (ec && ec.value() == DecodeResult::WantSubMessage) {
                Log::Debug("decoding sub message instead of string");
                setField(id, buf + offset, strLen);
            }
            offset += strLen;
        }
    }
    bytesParsed = offset;

    return std::error_code();
}

} // namespace protobuf
} // namespace fwoop

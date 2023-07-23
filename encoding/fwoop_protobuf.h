#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>
#include <unordered_map>
#include <variant>

namespace fwoop {
namespace protobuf {

struct ErrCategory : std::error_category {
    const char *name() const noexcept override;
    std::string message(int ev) const override;
};
const ErrCategory Error{};

class Message {
  public:
    typedef std::function<void(uint8_t id, uint8_t *data, uint32_t len)> VisitorFunc_t;

    enum DecodeResult {
        UnknownField = 1,
        WrongFieldType,
        WantSubMessage,
    };

  public:
    static uint64_t decodeUint64(uint8_t *buf, uint32_t bufLen);
    static std::string decodeString(uint8_t *buf, uint32_t bufLen);
    static Message *decodeSubMessage(uint8_t *buf, uint32_t bufLen);

    Message() = default;
    Message(const Message &rhs) = default;
    Message(Message &&rhs) = default;
    Message &operator=(const Message &rhs) = default;
    virtual ~Message() = 0;

    virtual void visitAll(const VisitorFunc_t &visitor) const;

    virtual std::error_code setField(uint32_t id, uint64_t value);
    virtual std::error_code setField(uint32_t id, const std::string &value);
    virtual std::error_code setField(uint32_t id, uint8_t *data, uint32_t len);

    std::error_code decode(uint8_t *buf, uint32_t bufLen, uint32_t &bytesParsed);
};

inline Message::~Message() {}

inline void Message::visitAll(const VisitorFunc_t &visitor) const {}

inline std::error_code Message::setField(uint32_t id, uint64_t value)
{
    return std::error_code(static_cast<int>(DecodeResult::UnknownField), Error);
}
inline std::error_code Message::setField(uint32_t id, const std::string &value)
{
    return std::error_code(static_cast<int>(DecodeResult::UnknownField), Error);
}
inline std::error_code Message::setField(uint32_t id, uint8_t *data, uint32_t len)
{
    return std::error_code(static_cast<int>(DecodeResult::UnknownField), Error);
}

} // namespace protobuf
} // namespace fwoop

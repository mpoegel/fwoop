#include <cstdint>
#include <fwoop_json.h>

#include <cctype>
#include <cmath>
#include <fwoop_log.h>
#include <memory>
#include <string.h>
#include <string>
#include <variant>

namespace fwoop {

namespace {

static const uint8_t START_OBJ = 0x7B;
static const uint8_t END_OBJ = 0x7D;
static const uint8_t QUOTE = 0x22;
static const uint8_t COLON = 0x3A;
static const uint8_t START_ARR = 0x5B;
static const uint8_t END_ARR = 0x5D;
static const uint8_t COMMA = 0x2C;

bool isWhitespace(uint8_t val)
{
    // TODO replace with isspace()
    return val == 0x20; // space
}

std::string decodeString(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    uint32_t index = 0;
    bytesParsed = 0;
    if (QUOTE != bytes[index]) {
        Log::Error("no start quote");
        return "";
    }
    index++;
    while (QUOTE != bytes[index] && index < bytesLen)
        index++;
    if (index == bytesLen) {
        Log::Error("no end quote");
        return "";
    }
    bytesParsed = index + 1;
    Log::Debug("string start: ", 1, ", end: ", index - 1);
    Log::Debug("string: ", std::string((char *)bytes + 1, (char *)bytes + index));
    return std::string((char *)bytes + 1, (char *)bytes + index);
}

bool decodeBool(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    static const char *TRUE = "true";
    static const unsigned int TRUE_LEN = strlen(TRUE);
    static const char *FALSE = "false";
    static const unsigned int FALSE_LEN = strlen(FALSE);
    bytesParsed = 0;
    if (bytesLen == TRUE_LEN && 0 == strncmp(TRUE, (char *)bytes, 4)) {
        bytesParsed = 4;
        return true;
    } else if (bytesLen == FALSE_LEN && 0 == strncmp(FALSE, (char *)bytes, 5)) {
        bytesParsed = 5;
        return false;
    }
    return false;
}

int decodeInt(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    bytesParsed = 0;
    if (0 == bytesLen) {
        return 0;
    }
    if (1 == bytesLen && isdigit(*bytes)) {
        bytesParsed = bytesLen;
        return *bytes - 0x30;
    }
    for (uint32_t i = 0; i < bytesLen; i++) {
        if (bytes[i] == '.') {
            // value is a double not an int
            return 0;
        }
    }
    uint8_t *tmp = new uint8_t[bytesLen];
    memcpy(tmp, bytes, bytesLen);
    int res = atoi((char *)tmp);
    if (res == 0) {
        return 0;
    }
    bytesParsed = bytesLen;
    return res;
}

double decodeDouble(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    // TODO refactor
    bytesParsed = 0;
    if (0 == bytesLen) {
        return 0;
    }
    double res = 0;
    bool isNegative = (*bytes == '-');
    bool afterDecimal = false;
    unsigned int beforeDecimal = 0;
    for (; beforeDecimal < bytesLen && bytes[beforeDecimal] != '.'; beforeDecimal++)
        ;
    double factor = isNegative ? pow(10, beforeDecimal - 2) : pow(10, beforeDecimal - 1);
    uint8_t i = isNegative ? 1 : 0;
    for (; i < bytesLen; i++) {
        if (bytes[i] == '.') {
            afterDecimal = true;
            factor = 10;
        } else if (!isdigit(bytes[i])) {
            return 0;
        } else if (afterDecimal) {
            res += (bytes[i] - 0x30) / factor;
            factor *= 10;
        } else {
            res += (bytes[i] - 0x30) * factor;
            factor /= 10;
        }
    }
    bytesParsed = bytesLen;
    return res;
}

JsonValue_t decodeValue(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    uint32_t &index = bytesParsed;
    index = 0;
    uint32_t tmpParsed = 0;
    if (QUOTE == bytes[index]) {
        std::string value = decodeString(bytes + index, bytesLen - index, tmpParsed);
        if (value.length() == 0) {
            Log::Error("no value string");
            return JsonValue_t();
        }
        index += tmpParsed;
        return JsonValue_t(value);
    } else if (START_ARR == bytes[index]) {
        auto val = JsonValue_t(std::make_shared<JsonArray>(bytes + index, bytesLen - index, tmpParsed));
        index += tmpParsed;
        return val;
    } else if (START_OBJ == bytes[index]) {
        auto val = JsonValue_t(std::make_shared<JsonObject>(bytes + index, bytesLen - index, tmpParsed));
        index += tmpParsed;
        return val;
    } else {
        // int or bool or double
        uint32_t end = index + 1;
        while (!isWhitespace(bytes[end]) && bytes[end] != COMMA && bytes[end] != END_OBJ && bytes[end] != END_ARR)
            end++;
        tmpParsed = 0;
        Log::Debug("look for value with length: ", end - index);
        if (bool value = decodeBool(bytes + index, end - index, tmpParsed); tmpParsed > 0) {
            return JsonValue_t(value);
        } else if (int value = decodeInt(bytes + index, end - index, tmpParsed); tmpParsed > 0) {
            return JsonValue_t(value);
        } else if (double value = decodeDouble(bytes + index, end - index, tmpParsed); tmpParsed > 0) {
            return JsonValue_t(value);
        } else {
            Log::Error("int/bool/double not found");
            return JsonValue_t();
        }
        index += tmpParsed;
    }
    return JsonValue_t();
}

} // namespace

std::ostream &operator<<(std::ostream &os, const JsonValue_t &val)
{
    if (std::holds_alternative<std::string>(val)) {
        os << "\"" << std::get<std::string>(val) << "\"";
    } else if (std::holds_alternative<int>(val)) {
        os << std::get<int>(val);
    } else if (std::holds_alternative<double>(val)) {
        os << std::get<double>(val);
    } else if (std::holds_alternative<bool>(val)) {
        os << (std::get<bool>(val) ? "true" : "false");
    }
    return os;
}

void JsonArray::decode(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    uint32_t &index = bytesParsed;
    index = 0;
    if (bytesLen == 0) {
        return;
    }
    while (isspace(bytes[index]) && index < bytesLen)
        index++;
    if (index >= bytesLen || START_ARR != bytes[index]) {
        Log::Error("no start array");
        return;
    }
    index++;
    while (isspace(bytes[index]) && index < bytesLen)
        index++;
    if (index >= bytesLen) {
        Log::Error("no start of value in array");
        return;
    }
    do {
        while (index < bytesLen && isspace(bytes[index]))
            index++;
        Log::Debug("looking for array value at index: ", index);
        uint32_t tmpParsed = 0;
        JsonValue_t value = decodeValue(bytes + index, bytesLen - index, tmpParsed);
        if (0 == tmpParsed) {
            Log::Error("no value found for array");
            return;
        }
        d_arr.emplace_back(value);
        index += tmpParsed;
        if (index >= bytesLen) {
            Log::Error("array ending not found");
            return;
        }
        Log::Debug("ending array value search iteration at index: ", index);
    } while (COMMA == bytes[index] && index++ < bytesLen);
    while (index < bytesLen && isspace(bytes[index]))
        index++;
    if (index >= bytesLen || END_ARR != bytes[index]) {
        Log::Error("bad array ending");
        return;
    }
    // success
    index++;
}

JsonArray::JsonArray() {}

JsonArray::JsonArray(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed) { decode(bytes, bytesLen, bytesParsed); }

JsonArray::JsonArray(JsonArray &rhs) {}

JsonArray &JsonArray::operator=(JsonArray rhs) { return *this; }

JsonArray::~JsonArray() {}

std::shared_ptr<JsonObject> JsonArray::getObject(unsigned int index) const
{
    std::shared_ptr<JsonObject> res;
    if (index >= d_arr.size()) {
        return res;
    }
    auto val = d_arr[index];
    if (std::holds_alternative<std::shared_ptr<JsonObject>>(val)) {
        return std::shared_ptr<JsonObject>(std::get<std::shared_ptr<JsonObject>>(val));
    }
    return res;
}

std::shared_ptr<JsonArray> JsonArray::getArray(unsigned int index) const
{
    std::shared_ptr<JsonArray> res;
    if (index >= d_arr.size()) {
        return res;
    }
    auto val = d_arr[index];
    if (std::holds_alternative<std::shared_ptr<JsonArray>>(val)) {
        return std::shared_ptr<JsonArray>(std::get<std::shared_ptr<JsonArray>>(val));
    }
    return res;
}

void JsonObject::decode(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    u_int32_t &index = bytesParsed;
    index = 0;
    while (isWhitespace(bytes[index]))
        index++;
    if (START_OBJ != bytes[index]) {
        Log::Error("no start object");
        return;
    }
    index++;

    do {
        // TODO handle escape characters
        // TODO handle out of bounds errors
        while (isWhitespace(bytes[index]))
            index++;
        uint32_t tmpParsed;
        std::string key = decodeString(bytes + index, bytesLen - index, tmpParsed);
        if (0 == key.length()) {
            Log::Error("no key");
            return;
        }
        index += tmpParsed;
        while (isWhitespace(bytes[index]))
            index++;
        if (COLON != bytes[index]) {
            Log::Error("no colon");
            return;
        }
        index++;
        while (isWhitespace(bytes[index]))
            index++;

        if (QUOTE == bytes[index]) {
            tmpParsed = 0;
            std::string value = decodeString(bytes + index, bytesLen - index, tmpParsed);
            if (value.length() == 0) {
                Log::Error("no value string");
                return;
            }
            d_valueMap.insert({key, value});
            index += tmpParsed;
        } else if (START_ARR == bytes[index]) {
            tmpParsed = 0;
            auto value = std::make_shared<JsonArray>(bytes + index, bytesLen - index, tmpParsed);
            if (0 == tmpParsed) {
                Log::Error("no array found");
                return;
            }
            d_valueMap.insert({key, value});
            index += tmpParsed;
        } else if (START_OBJ == bytes[index]) {
            tmpParsed = 0;
            auto value = std::make_shared<JsonObject>(bytes + index, bytesLen - index, tmpParsed);
            if (0 == tmpParsed) {
                Log::Error("no nested object found");
                return;
            }
            d_valueMap.insert({key, value});
            index += tmpParsed;
        } else {
            // int or bool or double
            uint32_t end = index + 1;
            while (!isWhitespace(bytes[end]) && bytes[end] != COMMA && bytes[end] != END_OBJ && bytes[end] != END_ARR)
                end++;
            tmpParsed = 0;
            Log::Debug("look for value with length: ", end - index);
            if (bool value = decodeBool(bytes + index, end - index, tmpParsed); tmpParsed > 0) {
                d_valueMap.insert({key, value});
            } else if (int value = decodeInt(bytes + index, end - index, tmpParsed); tmpParsed > 0) {
                d_valueMap.insert({key, value});
            } else if (double value = decodeDouble(bytes + index, end - index, tmpParsed); tmpParsed > 0) {
                d_valueMap.insert({key, value});
            } else {
                Log::Error("int/bool/double not found");
                return;
            }
            index += tmpParsed;
        }
        while (isWhitespace(bytes[index]))
            index++;
    } while (COMMA == bytes[index] && index++ < bytesLen);

    if (END_OBJ == bytes[index]) {
        // success!
        bytesParsed = index + 1;
    } else {
        Log::Error("failed, ended at index: ", index, " value: ", bytes[index]);
    }
    // failure -- don't update bytes Parsed
}

JsonObject::JsonObject() {}

JsonObject::JsonObject(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed)
{
    decode(bytes, bytesLen, bytesParsed);
}

JsonObject::JsonObject(JsonObject &rhs) {}

JsonObject &JsonObject::operator=(JsonObject rhs) { return *this; }

JsonObject::~JsonObject() {}

std::optional<std::string> JsonObject::getString(const std::string &key) const
{
    std::optional<std::string> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<std::string>(itr->second)) {
            return std::optional<std::string>(std::get<std::string>(itr->second));
        }
    }
    return res;
}

std::optional<int> JsonObject::getInt(const std::string &key) const
{
    std::optional<int> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<int>(itr->second)) {
            return std::optional<int>(std::get<int>(itr->second));
        }
    }
    return res;
}

std::optional<double> JsonObject::getDouble(const std::string &key) const
{
    std::optional<double> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<double>(itr->second)) {
            return std::optional<double>(std::get<double>(itr->second));
        }
    }
    return res;
}

std::optional<bool> JsonObject::getBool(const std::string &key) const
{
    std::optional<bool> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<bool>(itr->second)) {
            return std::optional<bool>(std::get<bool>(itr->second));
        }
    }
    return res;
}

std::shared_ptr<JsonArray> JsonObject::getArray(const std::string &key) const
{
    std::shared_ptr<JsonArray> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<std::shared_ptr<JsonArray>>(itr->second)) {
            return std::shared_ptr<JsonArray>(std::get<std::shared_ptr<JsonArray>>(itr->second));
        }
    }
    return res;
}

std::shared_ptr<JsonObject> JsonObject::getObject(const std::string &key) const
{
    std::shared_ptr<JsonObject> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<std::shared_ptr<JsonObject>>(itr->second)) {
            return std::shared_ptr<JsonObject>(std::get<std::shared_ptr<JsonObject>>(itr->second));
        }
    }
    return res;
}

} // namespace fwoop

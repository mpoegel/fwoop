#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fwoop {

class JsonObject;
class JsonArray;

typedef std::variant<std::string, int, double, bool, std::shared_ptr<JsonArray>, std::shared_ptr<JsonObject>>
    JsonValue_t;

class JsonArray {
  private:
    std::vector<JsonValue_t> d_arr;

    void decode(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed);

  public:
    JsonArray();
    JsonArray(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed);
    JsonArray(JsonArray &rhs);
    JsonArray(JsonArray &&rhs) = default;
    JsonArray &operator=(JsonArray rhs);
    ~JsonArray();

    unsigned int length() const;

    template <typename T> std::optional<T> get(unsigned int index) const;

    std::shared_ptr<JsonObject> getObject(unsigned int index) const;
    std::shared_ptr<JsonArray> getArray(unsigned int index) const;
};

class JsonObject {
  private:
    std::unordered_map<std::string, JsonValue_t> d_valueMap;

    void decode(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed);

  public:
    JsonObject();
    JsonObject(uint8_t *bytes, uint32_t bytesLen, uint32_t &bytesParsed);
    JsonObject(JsonObject &rhs);
    JsonObject &operator=(JsonObject rhs);
    ~JsonObject();

    uint8_t *encode(uint32_t &length);

    std::optional<std::string> getString(const std::string &key) const;
    std::optional<int> getInt(const std::string &key) const;
    std::optional<double> getDouble(const std::string &key) const;
    std::optional<bool> getBool(const std::string &key) const;

    template <typename T> std::optional<T> get(const std::string &key) const;

    std::shared_ptr<JsonArray> getArray(const std::string &key) const;
    std::shared_ptr<JsonObject> getObject(const std::string &key) const;
};

class Json {
  public:
  private:
    JsonObject d_document;

  public:
    static Json decode(uint8_t *jsonBytes, uint32_t length);

    uint8_t *encode(uint32_t &length);
};

inline unsigned int JsonArray::length() const { return d_arr.size(); }

template <typename T> std::optional<T> JsonArray::get(unsigned int index) const
{
    std::optional<T> res;
    if (index >= length()) {
        return res;
    }
    if (std::holds_alternative<T>(d_arr[index])) {
        return std::optional<T>(std::get<T>(d_arr[index]));
    }
    return res;
}

template <typename T> std::optional<T> JsonObject::get(const std::string &key) const
{
    std::optional<T> res;
    auto itr = d_valueMap.find(key);
    if (itr != d_valueMap.end()) {
        if (std::holds_alternative<T>(itr->second)) {
            return std::optional<T>(std::get<T>(itr->second));
        }
    }
    return res;
}

} // namespace fwoop

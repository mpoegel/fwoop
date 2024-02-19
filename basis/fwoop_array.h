#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <inttypes.h>
#include <string>

namespace fwoop {

class Array {
  private:
    uint8_t *d_data;
    uint32_t d_size;
    uint32_t d_actualSize;

  public:
    Array(uint32_t size);
    Array(const Array &rhs);
    ~Array();

    Array *operator=(const Array &rhs);
    uint8_t &operator[](uint32_t i);
    const uint8_t &operator[](uint32_t i) const;
    uint8_t *operator*();

    void extend(const Array &arr);
    void shrink(uint32_t newSize);
    uint32_t size() const;
    void enlarge(uint32_t newSize);
    void append(const std::string &str);
    void append(const char *str, uint32_t len);
    void append(uint8_t d);
    Array subArray(uint32_t start, uint32_t end) const;
    std::string toString() const;
    void clear();
    std::string toHex() const;
};

inline uint8_t &Array::operator[](uint32_t i) { return d_data[i]; }
inline const uint8_t &Array::operator[](uint32_t i) const { return d_data[i]; }
inline uint8_t *Array::operator*() { return d_data; }
inline uint32_t Array::size() const { return d_size; }
inline void Array::shrink(uint32_t newSize) { d_size = std::min(d_size, newSize); }
inline void Array::append(const std::string &str) { append(str.data(), str.length()); }
inline std::string Array::toString() const { return std::string((char *)d_data, d_size); }
inline void Array::clear() { memset(d_data, 0, d_actualSize); }

} // namespace fwoop

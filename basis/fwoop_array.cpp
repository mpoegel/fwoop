#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fwoop_array.h>

namespace fwoop {

Array::Array(uint32_t size) : d_size(size), d_actualSize(size) { d_data = new uint8_t[d_size]; }

Array::Array(const Array &rhs) : d_size(rhs.d_size), d_actualSize(rhs.d_actualSize)
{
    d_data = new uint8_t[d_actualSize];
    memcpy(d_data, rhs.d_data, rhs.d_size);
}

Array::~Array() { delete[] d_data; }

Array *Array::operator=(const Array &rhs)
{
    d_data = new uint8_t[d_actualSize];
    memcpy(d_data, rhs.d_data, rhs.d_size);
    return this;
}

void Array::extend(const Array &arr)
{
    uint32_t remaining = d_actualSize - d_size;
    if (arr.d_size > remaining) {
        enlarge(d_size + arr.size());
    }
    memcpy(d_data + d_size, arr.d_data, arr.d_size);
    d_size += arr.d_size;
}

void Array::enlarge(uint32_t newSize)
{
    if (newSize <= d_actualSize && newSize > d_size) {
        // resize not needed
        d_size = newSize;
    } else if (newSize > d_actualSize) {
        // new allocation needed
        uint8_t *newData = new uint8_t[newSize];
        memcpy(newData, d_data, d_actualSize);
        delete[] d_data;
        d_data = newData;
        d_actualSize = newSize;
    }
    // new size is smaller
}

void Array::append(char *str)
{
    uint32_t len = std::strlen(str);
    uint32_t remaining = d_actualSize - d_size;
    if (len > remaining) {
        enlarge(d_size + len);
    }
    memcpy(d_data + d_size, str, len);
    d_size += len;
}

std::string Array::toHex() const
{
    std::string res;
    res.reserve(d_size * 3);
    char buf[3];
    for (unsigned int i = 0; i < d_size; i++) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s%x", i > 0 ? " " : "", d_data[i]);
        res.append(buf, i > 0 ? 3 : 2);
    }
    return res;
}

} // namespace fwoop

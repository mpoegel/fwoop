#pragma once

#include <fwoop_httpheader.h>

#include <cstdint>
#include <cstring>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fwoop {

class HttpHPacker {
  private:
    static std::unordered_map<std::string, u_int8_t> s_staticTable;
    static const uint8_t STATIC_TABLE_SIZE = 61;

    typedef struct {
        std::list<std::string> queue;
        std::unordered_map<std::string, uint32_t> indexLookup;
        u_int32_t size;
    } DynamicTable_t;

    DynamicTable_t d_dynamicTable;
    u_int32_t      d_maxTableSize;

    uint32_t getEncodedLength(DynamicTable_t& dynamicTable, HttpHeaderVariant_t header, const std::string& headerValue) const;
    uint32_t encode(uint8_t *out, HttpHeaderVariant_t header, const std::string& headerValue);

  public:
    HttpHPacker();

    uint32_t getEncodedLength(const std::vector<HttpHeaderField_t>& headerFields) const;

    uint32_t encodeLength(const std::vector<HttpHeaderField_t>& headerFields);
    uint8_t *encode(const std::vector<HttpHeaderField_t>& headerFields);
};

}

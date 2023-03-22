#include <fwoop_httphpacker.h>

#include <fwoop_log.h>

namespace fwoop {

std::unordered_map<std::string, uint8_t> HttpHPacker::s_staticTable = {
    {":authority", 1},
    {":method-get", 2},
    {":method-post", 3},
    {":path-/", 4},
    {":path-/index.html", 5},
    {":scheme-http", 6},
    {":scheme-https", 7},
    {":status-200", 8},
    {":status-204", 9},
    {":status-206", 10},
    {":status-304", 11},
    {":status-400", 12},
    {":status-404", 13},
    {":status-500", 14},
    {"accept-charset", 15},
    {"accept-encoding-gzip, deflate", 16},
    {"accept-language", 17},
    {"accept-ranges", 18},
    {"accept", 19},
    {"access-control-allow-origin", 20},
    {"age", 21},
    {"allow", 22},
    {"authorization", 23},
    {"cache-control", 24},
    {"content-disposition", 25},
    {"content-encoding", 26},
    {"content-language", 27},
    {"content-length", 28},
    {"content-location", 29},
    {"content-range", 30},
    {"content-type", 31},
    {"cookie", 32},
    {"date", 33},
    {"etag", 34},
    {"expect", 35},
    {"expires", 36},
    {"from", 37},
    {"host", 38},
    {"if-match", 39},
    {"if-modified-since", 40},
    {"if-none-match", 41},
    {"if-range", 42},
    {"if-unmodified-since", 43},
    {"last-modified", 44},
    {"link", 45},
    {"location", 46},
    {"max-forwards", 47},
    {"proxy-authenticate", 48},
    {"proxy-authorization", 49},
    {"range", 50},
    {"referer", 51},
    {"refresh", 52},
    {"retry-after", 53},
    {"server", 54},
    {"set-cookie", 55},
    {"strict-transport-security", 56},
    {"transfer-encoding", 57},
    {"user-agent", 58},
    {"vary", 59},
    {"via", 60},
    {"www-authenticate", 61},
};

HttpHPacker::HttpHPacker() : d_maxTableSize(4096) { d_dynamicTable.size = 0; }

uint32_t HttpHPacker::getEncodedLength(DynamicTable_t &dynamicTable, HttpHeaderVariant_t header,
                                       const std::string &headerValue) const
{
    std::string headerStr;
    if (std::holds_alternative<HttpHeader>(header)) {
        headerStr = httpHeaderToString(std::get<HttpHeader>(header));
    } else {
        headerStr = std::get<std::string>(header);
    }
    const std::string compoundKey = headerStr + '-' + headerValue;

    if (s_staticTable.find(compoundKey) != s_staticTable.end()) {
        // (static) indexed header field representation
        return 1;
    }

    if (dynamicTable.indexLookup.find(compoundKey) != dynamicTable.indexLookup.end()) {
        // (dynamic) indexed header field representation
        return 1;
    }

    if (s_staticTable.find(headerStr) != s_staticTable.end()) {
        // literal header field with incremental indexing
        dynamicTable.indexLookup[compoundKey] = dynamicTable.queue.size();
        dynamicTable.queue.push_back(compoundKey);
        return 1 + 1 + headerValue.length();
    }

    // literal header field incremental indexing with new name
    dynamicTable.indexLookup[compoundKey] = dynamicTable.queue.size();
    dynamicTable.queue.push_back(compoundKey);
    return 1 + 1 + headerStr.length() + 1 + headerValue.length();
}

uint32_t HttpHPacker::encodeLength(const std::vector<HttpHeaderField_t> &headerFields)
{
    uint32_t len = 0;

    for (auto itr = headerFields.cbegin(); itr != headerFields.cend(); itr++) {
        len += getEncodedLength(d_dynamicTable, itr->first, itr->second);
    }
    return len;
}

uint32_t HttpHPacker::getEncodedLength(const std::vector<HttpHeaderField_t> &headerFields) const
{
    uint32_t len = 0;
    DynamicTable_t dynamicTable = d_dynamicTable;

    for (auto itr = headerFields.cbegin(); itr != headerFields.cend(); itr++) {
        len += getEncodedLength(dynamicTable, itr->first, itr->second);
    }
    return len;
}

uint32_t HttpHPacker::encode(uint8_t *out, HttpHeaderVariant_t header, const std::string &headerValue)
{
    const static uint8_t incrementalIndexMask = 1 << 6;
    const static uint8_t indexedMask = 1 << 7;

    std::string headerStr;
    if (std::holds_alternative<HttpHeader>(header)) {
        headerStr = httpHeaderToString(std::get<HttpHeader>(header));
    } else {
        headerStr = std::get<std::string>(header);
    }
    const std::string compoundKey = headerStr + '-' + headerValue;

    auto staticItr = s_staticTable.find(compoundKey);
    if (staticItr != s_staticTable.end()) {
        // (static) indexed header field representation
        out[0] = indexedMask | staticItr->second;
        return 1;
    }

    auto dynamItr = d_dynamicTable.indexLookup.find(compoundKey);
    if (dynamItr != d_dynamicTable.indexLookup.end()) {
        int index = d_dynamicTable.size - dynamItr->second + STATIC_TABLE_SIZE;
        // (dynamic) indexed header field representation
        out[0] = indexedMask | index;
        return 1;
    }

    staticItr = s_staticTable.find(headerStr);
    if (staticItr != s_staticTable.end()) {
        // literal header field with incremental indexing
        out[0] = incrementalIndexMask | staticItr->second;
        out[1] = headerValue.length();
        memcpy(out + 2, headerValue.data(), headerValue.length());

        d_dynamicTable.indexLookup[compoundKey] = d_dynamicTable.queue.size();
        d_dynamicTable.queue.push_back(compoundKey);
        d_dynamicTable.size++;
        return 1 + 1 + headerValue.length();
    }

    // literal header field incremental indexing with new name
    u_int32_t offset = 0;
    out[offset] = incrementalIndexMask;
    ++offset;
    out[offset] = headerStr.length();
    ++offset;
    memcpy(out + offset, headerStr.data(), headerStr.length());
    offset += headerStr.length();
    out[offset] = headerValue.length();
    ++offset;
    memcpy(out + offset, headerValue.data(), headerValue.length());

    d_dynamicTable.indexLookup[compoundKey] = d_dynamicTable.queue.size();
    d_dynamicTable.queue.push_back(compoundKey);
    d_dynamicTable.size++;
    return 1 + 1 + headerStr.length() + 1 + headerValue.length();
}

u_int8_t *HttpHPacker::encode(const std::vector<HttpHeaderField_t> &headerFields)
{
    uint32_t totalLen = getEncodedLength(headerFields);
    uint8_t *encoding = new u_int8_t[totalLen];

    uint32_t offset = 0;
    for (auto itr = headerFields.cbegin(); itr != headerFields.cend(); itr++) {
        uint32_t len = encode(encoding + offset, itr->first, itr->second);
        offset += len;
    }

    return encoding;
}

} // namespace fwoop

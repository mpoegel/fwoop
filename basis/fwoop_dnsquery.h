#pragma once

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

namespace fwoop {
namespace DNS {

/*
RFC 1035: https://www.rfc-editor.org/rfc/rfc1035
*/

enum RecordType {
    UndefinedType = 0x0,
    A = 0x1,      // host address
    NS = 0x2,     // authoritative mail server
    CNAME = 0x5,  // canonical name for an alias
    SOA = 0x6,    // start of a zone of authority
    WKS = 0x11,   // well known service description
    PTR = 0x12,   // domain name pointer
    HINFO = 0x13, // host information
    MINFO = 0x14, // mailbox or mail list information
    MX = 0x15,    // mail exchange
    TXT = 0x16,   // text strings
};

RecordType NewRecordType(uint16_t val);

enum ClassValue {
    UndefinedClass = 0x0,
    IN = 0x1, // internet
    CH = 0x3, // CHAOS
    HS = 0x4, // Hesiod
};

ClassValue NewClassValue(uint16_t val);

class Question {
  private:
    std::string d_name;
    RecordType d_type;
    ClassValue d_class;

  public:
    static std::shared_ptr<Question> parse(uint8_t *data, unsigned int dataLen, unsigned int &offset);

    Question(const std::string &name, RecordType qType = RecordType::A, ClassValue qClass = ClassValue::IN);
    ~Question() = default;
    Question(Question &rhs) = default;
    Question &operator=(const Question &rhs) const = delete;
    Question(Question &&rhs) = default;

    const std::string &name() const;
    RecordType type() const;
    ClassValue classValue() const;
    uint8_t *encode(unsigned int &outLen) const;
};

std::ostream &operator<<(std::ostream &os, const Question &question);

inline const std::string &Question::name() const { return d_name; }
inline RecordType Question::type() const { return d_type; }
inline ClassValue Question::classValue() const { return d_class; }

class ResourceRecord {
  private:
    static constexpr uint16_t s_maxDataLen = 256;
    std::string d_name;
    RecordType d_type;
    ClassValue d_class;
    uint32_t d_ttl;
    uint16_t d_rdLength;
    uint8_t d_rData[s_maxDataLen];

  public:
    static std::shared_ptr<ResourceRecord> parse(uint8_t *data, unsigned int dataLen, unsigned int &offset);

    ResourceRecord() = default;
    ~ResourceRecord() = default;
    ResourceRecord(ResourceRecord &rhs) = default;
    ResourceRecord &operator=(const ResourceRecord &rhs) const = delete;
    ResourceRecord(ResourceRecord &&rhs) = default;

    const std::string &name() const;
    RecordType type() const;
    ClassValue classValue() const;
    uint32_t timeToLive() const;
    std::string IP() const;
};

std::ostream &operator<<(std::ostream &os, const ResourceRecord &record);

inline const std::string &ResourceRecord::name() const { return d_name; }
inline RecordType ResourceRecord::type() const { return d_type; }
inline ClassValue ResourceRecord::classValue() const { return d_class; }
inline uint32_t ResourceRecord::timeToLive() const { return d_ttl; }

class Query {
  private:
    static Query *s_query_p;

    // TODO add record cache

    Query() = default;
    ~Query() = default;
    Query(Query &rhs) = default;
    Query &operator=(const Query &rhs) const = delete;
    Query(Query &&rhs) = default;

    Query &singleton();

    static uint8_t *encodeHostName(const std::string &hostname, unsigned int &outLen);

  public:
    static std::string getHostByName(const std::string &hostname);

    static std::shared_ptr<ResourceRecord> getRecord(const Question &question);
};

} // namespace DNS
} // namespace fwoop

#include "fwoop_array.h"
#include <fwoop_dnsquery.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

#include <fwoop_log.h>
#include <fwoop_socketio.h>
#include <fwoop_tokenizer.h>

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <unistd.h>

namespace fwoop {
namespace DNS {

RecordType NewRecordType(uint16_t val)
{
    switch (val) {
    case 1:
        return RecordType::A;
    case 2:
        return RecordType::NS;
    case 5:
        return RecordType::CNAME;
    case 6:
        return RecordType::SOA;
    case 11:
        return RecordType::WKS;
    case 12:
        return RecordType::PTR;
    case 13:
        return RecordType::HINFO;
    case 14:
        return RecordType::MINFO;
    case 15:
        return RecordType::MX;
    case 16:
        return RecordType::MX;
    default:
        return RecordType::UndefinedType;
    }
}

ClassValue NewClassValue(uint16_t val)
{
    switch (val) {
    case 1:
        return ClassValue::IN;
    case 3:
        return ClassValue::CH;
    case 4:
        return ClassValue::IN;
    default:
        return ClassValue::UndefinedClass;
    }
}

std::shared_ptr<Question> Question::parse(const Array &data, uint32_t &offset)
{
    if (data.size() == 0 || offset >= data.size()) {
        Log::Debug("not enough data to parse question");
        return nullptr;
    }

    std::string name;
    while (offset < data.size() && data[offset] != 0) {
        unsigned int labelLen = data[offset++];
        if (offset + labelLen >= data.size()) {
            // error
            Log::Debug("incomplete name in question");
            return nullptr;
        }
        name += std::string(data[offset], data[offset + labelLen]);
        name += ".";
        offset += labelLen;
    }
    if (offset + 4 >= data.size()) {
        // error
        Log::Debug("question missing type and/or class");
        return nullptr;
    }

    offset++;
    auto rType = NewRecordType((data[offset] << 8) + data[offset + 1]);
    offset += 2;
    auto rClass = NewClassValue((data[offset] << 8) + data[offset + 1]);
    offset += 2;

    auto question = std::make_shared<Question>(name, rType, rClass);
    return question;
}

Question::Question(const std::string &name, RecordType qType, ClassValue qClass)
    : d_name(name), d_type(qType), d_class(qClass)
{
}

Array Question::encode() const
{
    uint32_t outLen = d_name.length();
    outLen += std::count(d_name.begin(), d_name.end(), '.') + 2;
    outLen += 4;

    Array encoding(outLen);

    Tokenizer tok(d_name, '.');
    for (auto t = tok.begin(); t != tok.end(); ++t) {
        encoding.append((*t).length());
        encoding.append(*t);
    }
    encoding.append(uint8_t(0));

    encoding.append(d_type >> 8);
    encoding.append(d_type & 0xFF);
    encoding.append(d_class >> 8);
    encoding.append(d_class & 0xFF);

    return encoding;
}

std::ostream &operator<<(std::ostream &os, const Question &question)
{
    os << "[ name=" << question.name() << " type=" << question.type() << " class=" << question.classValue() << " ]";
    return os;
}

std::shared_ptr<ResourceRecord> ResourceRecord::parse(const Array &data, unsigned int &offset)
{
    auto record = std::make_shared<ResourceRecord>();
    if (data.size() == 0) {
        // error
        Log::Debug("not enough data to parse resource record");
        return nullptr;
    }

    unsigned int tmp = 0;
    while (offset < data.size() && data[offset] != 0) {
        unsigned int labelLen = data[offset++];
        if ((labelLen & 0xC0) == 0xC0) {
            // pointer
            unsigned int pointer = ((labelLen & 0x3F) << 8) + data[offset++];
            tmp = offset;
            offset = pointer;
            continue;
        }
        if (offset + labelLen >= data.size()) {
            // error
            Log::Debug("label length out of range");
            return nullptr;
        }
        record->d_name += std::string(data[offset], data[offset + labelLen]);
        record->d_name += ".";
        offset += labelLen;
    }
    if (tmp != 0) {
        offset = tmp;
    } else {
        offset++;
    }
    if (offset + 10 >= data.size()) {
        // error
        Log::Debug("record incomplete, missing type and/or value");
        return nullptr;
    }
    record->d_type = NewRecordType((data[offset] << 8) + data[offset + 1]);
    offset += 2;
    record->d_class = NewClassValue((data[offset] << 8) + data[offset + 1]);
    offset += 2;
    record->d_ttl = (data[offset] << 24) + (data[offset + 1] << 16) + (data[offset + 2] << 8) + data[offset + 3];
    offset += 4;

    record->d_rdLength = (data[offset] << 8) + data[offset + 1];
    offset += 2;
    if (offset + record->d_rdLength > data.size()) {
        // error
        Log::Debug("not enough left to parse rData");
        return nullptr;
    }
    record->d_rData = data.subArray(offset, std::min(record->d_rdLength, s_maxDataLen));
    offset += record->d_rdLength;
    return record;
}

ResourceRecord::ResourceRecord() : d_rData(s_maxDataLen) {}

std::string ResourceRecord::IP() const
{
    if (d_rdLength != 4) {
        return "";
    }
    std::string result;
    result.reserve(3 * 4 + 3);
    for (unsigned int i = 0; i < d_rdLength; i++) {
        if (i != 0) {
            result += ".";
        }
        result += std::to_string(d_rData[i]);
    }
    return result;
}

std::ostream &operator<<(std::ostream &os, const ResourceRecord &record)
{
    os << "[ name=" << record.name() << " type=" << record.type() << " class=" << record.classValue()
       << " ttl=" << record.timeToLive() << " IP=" << record.IP() << " ]";
    return os;
}

Query *Query::s_query_p = nullptr;
std::string Query::ServerAddress = "127.0.0.1";

Query &Query::singleton()
{
    if (s_query_p != nullptr) {
        return *s_query_p;
    }

    s_query_p = new Query();
    return *s_query_p;
}

Array Query::encodeHostName(const std::string &hostname)
{
    uint32_t outLen = hostname.length();
    outLen += std::count(hostname.begin(), hostname.end(), '.');
    Array encoding(outLen);

    Tokenizer tok(hostname, '.');
    for (auto t = tok.begin(); t != tok.end(); ++t) {
        encoding.append((*t).length());
        encoding.append(*t);
    }
    return encoding;
}

std::string Query::getHostByName(const std::string &hostname)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        Log::Error("failed to create socket, errno=", errno);
        return "";
    }

    const int opt = 1;
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        Log::Error("failed to setsockopt, errno=", errno);
        close(sockfd);
        return "";
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(53);

    if (inet_pton(AF_INET, ServerAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        Log::Error("invalid address or address not supported");
        close(sockfd);
        return "";
    }

    const unsigned int headerLen = 12;

    Array encodedHostName = encodeHostName(hostname);
    Log::Debug("encoding header len is ", encodedHostName.size());

    const unsigned int requestLen = headerLen + encodedHostName.size() + 4;
    Array request(requestLen);

    // TODO make random
    const uint16_t transactionID = 0x7598;
    const uint16_t flags = 0x0100;
    const uint16_t numQuestions = 0x1;
    const uint16_t answers = 0;
    const uint16_t authorities = 0;
    const uint16_t additional = 0;

    const uint16_t resourceRecordType = 0x1;  // A Record
    const uint16_t resourceRecordClass = 0x1; // IN (internet) Class

    // Query header
    request.append(transactionID >> 8);
    request.append(transactionID & 0xFF);
    request.append(flags >> 8);
    request.append(flags & 0xFF);
    request.append(numQuestions >> 8);
    request.append(numQuestions & 0xFF);
    request.append(answers >> 8);
    request.append(answers & 0xFF);
    request.append(authorities >> 8);
    request.append(authorities & 0xFF);
    request.append(additional >> 8);
    request.append(additional & 0xFF);
    // Queries
    request.extend(encodedHostName);
    request.append(resourceRecordType >> 8);
    request.append(resourceRecordType & 0xFF);
    request.append(resourceRecordClass >> 8);
    request.append(resourceRecordClass & 0xFF);

    Log::Debug("dns request: ", request.toHex());

    unsigned int bytesSend = sendto(sockfd, (const char *)*request, request.size(), MSG_CONFIRM,
                                    (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    Log::Debug("wrote ", bytesSend, " of ", request.size(), " bytes");

    Array response(512);
    socklen_t sockLen;
    int bytesRead = recvfrom(sockfd, *response, 512, MSG_WAITALL, (struct sockaddr *)&serv_addr, &sockLen);
    close(sockfd);
    if (bytesRead < 0) {
        Log::Error("read failed, errno=", errno, " errstr=", std::strerror(errno));
        return "";
    }
    Log::Debug("read ", bytesRead, " bytes in response");

    if (bytesRead < 12) {
        Log::Error("response too short");
        return "";
    }
    response.enlarge(bytesRead);

    const uint16_t rTransactionID = (response[0] << 8) + response[1];
    const uint16_t rFlags = (response[2] << 8) + response[3];
    const uint16_t rNumQuestions = (response[4] << 8) + response[5];
    const uint16_t rAnswers = (response[6] << 8) + response[7];
    const uint16_t rAuthority = (response[8] << 8) + response[9];
    const uint16_t rAdditional = (response[10] << 8) + response[11];

    Log::Debug("transactionID=", rTransactionID, " flags=", rFlags, " questions=", rNumQuestions, " answers=", rAnswers,
               " authority=", rAuthority, " additional=", rAdditional);

    uint32_t offset = 12;
    std::vector<std::shared_ptr<Question>> questions;
    for (unsigned int i = 0; i < rNumQuestions; i++) {
        Array arr = response.subArray(offset, response.size());
        auto question = Question::parse(arr, offset);
        if (question) {
            Log::Info("question: ", *question.get());
            questions.push_back(question);
        } else {
            Log::Error("failed to parse question");
        }
    }

    std::vector<std::shared_ptr<ResourceRecord>> records;
    for (unsigned int i = 0; i < rAnswers; i++) {
        Array arr = response.subArray(offset, response.size());
        auto record = ResourceRecord::parse(arr, offset);
        if (record) {
            Log::Info("record: ", *record.get());
            records.push_back(record);
        } else {
            Log::Error("failed to parse resource record");
        }
        return record->IP();
    }
    return "";
}

std::shared_ptr<ResourceRecord> Query::getRecord(const Question &question)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        Log::Error("failed to create socket, errno=", errno);
        return nullptr;
    }

    const int opt = 1;
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        Log::Error("failed to setsockopt, errno=", errno, " ", std::strerror(errno));
        close(sockfd);
        return nullptr;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(53);

    if (inet_pton(AF_INET, ServerAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        Log::Error("invalid address or address not supported");
        close(sockfd);
        return nullptr;
    }

    const unsigned int headerLen = 12;
    Array encodedQuestion = question.encode();

    const unsigned int requestLen = headerLen + encodedQuestion.size();
    Array request(requestLen);

    // TODO make random
    const uint16_t transactionID = 0x7598;
    const uint16_t flags = 0x0100;
    const uint16_t numQuestions = 0x1;
    const uint16_t answers = 0;
    const uint16_t authorities = 0;
    const uint16_t additional = 0;

    unsigned int offset = 0;
    // Query header
    request.append(transactionID >> 8);
    request.append(transactionID & 0xFF);
    request.append(flags >> 8);
    request.append(flags & 0xFF);
    request.append(numQuestions >> 8);
    request.append(numQuestions & 0xFF);
    request.append(answers >> 8);
    request.append(answers & 0xFF);
    request.append(authorities >> 8);
    request.append(authorities & 0xFF);
    request.append(additional >> 8);
    request.append(additional & 0xFF);
    // Queries
    request.extend(encodedQuestion);

    unsigned int bytesSend = sendto(sockfd, (const char *)*request, requestLen, MSG_CONFIRM,
                                    (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (bytesSend != requestLen) {
        close(sockfd);
        return nullptr;
    }

    Array response(512);
    socklen_t sockLen;
    int retries = 3;
    int bytesRead = -1;
    while (retries-- > 0 && bytesRead < 0) {
        bytesRead = recvfrom(sockfd, *response, 512, MSG_WAITALL, (struct sockaddr *)&serv_addr, &sockLen);
    }
    close(sockfd);
    if (bytesRead < 0) {
        Log::Error("read failed, errno=", errno, " ", std::strerror(errno));
        return nullptr;
    }

    if (bytesRead < 12) {
        Log::Error("response too short");
        return nullptr;
    }
    response.enlarge(bytesRead);

    const uint16_t rTransactionID = (response[0] << 8) + response[1];
    const uint16_t rFlags = (response[2] << 8) + response[3];
    const uint16_t rNumQuestions = (response[4] << 8) + response[5];
    const uint16_t rAnswers = (response[6] << 8) + response[7];
    const uint16_t rAuthority = (response[8] << 8) + response[9];
    const uint16_t rAdditional = (response[10] << 8) + response[11];
    offset = 12;

    if (rAnswers == 0) {
        Log::Debug("no answers for question");
        return nullptr;
    }

    std::vector<std::shared_ptr<Question>> questions;
    for (unsigned int i = 0; i < rNumQuestions; i++) {
        auto question = Question::parse(response, offset);
        if (question) {
            questions.push_back(question);
        } else {
            Log::Error("failed to parse question");
        }
    }

    std::vector<std::shared_ptr<ResourceRecord>> records;
    std::shared_ptr<ResourceRecord> result;
    for (unsigned int i = 0; i < rAnswers; i++) {
        auto record = ResourceRecord::parse(response, offset);
        if (record) {
            if (result == nullptr && record->type() == RecordType::A) {
                result = record;
            }
            records.push_back(record);
        } else {
            Log::Error("failed to parse resource record");
        }
    }
    return result;
}

} // namespace DNS
} // namespace fwoop

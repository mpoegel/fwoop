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

std::shared_ptr<Question> Question::parse(uint8_t *data, unsigned int dataLen, unsigned int &offset)
{
    if (dataLen == 0 || offset >= dataLen) {
        Log::Debug("not enough data to parse question");
        return nullptr;
    }

    std::string name;
    while (offset < dataLen && data[offset] != 0) {
        unsigned int labelLen = data[offset++];
        if (offset + labelLen >= dataLen) {
            // error
            Log::Debug("incomplete name in question");
            return nullptr;
        }
        name += std::string(data + offset, data + offset + labelLen);
        name += ".";
        offset += labelLen;
    }
    if (offset + 4 >= dataLen) {
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

uint8_t *Question::encode(unsigned int &outLen) const
{
    outLen = d_name.length();
    outLen += std::count(d_name.begin(), d_name.end(), '.') + 2;
    outLen += 4;

    uint8_t *encoding = new uint8_t[outLen];
    memset(encoding, 0, outLen);

    unsigned int offset = 0;
    Tokenizer tok(d_name, '.');
    for (auto t = tok.begin(); t != tok.end(); ++t) {
        encoding[offset++] = (*t).length();
        memcpy(encoding + offset, (*t).c_str(), (*t).length());
        offset += (*t).length();
    }
    encoding[offset++] = 0;

    encoding[offset++] = (d_type >> 8);
    encoding[offset++] = (d_type & 0xFF);
    encoding[offset++] = (d_class >> 8);
    encoding[offset++] = (d_class & 0xFF);

    return encoding;
}

std::ostream &operator<<(std::ostream &os, const Question &question)
{
    os << "[ name=" << question.name() << " type=" << question.type() << " class=" << question.classValue() << " ]";
    return os;
}

std::shared_ptr<ResourceRecord> ResourceRecord::parse(uint8_t *data, unsigned int dataLen, unsigned int &offset)
{
    auto record = std::make_shared<ResourceRecord>();
    if (dataLen == 0 || offset >= dataLen) {
        // error
        Log::Debug("not enough data to parse resource record");
        return nullptr;
    }

    unsigned int tmp = 0;
    while (offset < dataLen && data[offset] != 0) {
        unsigned int labelLen = data[offset++];
        if ((labelLen & 0xC0) == 0xC0) {
            // pointer
            unsigned int pointer = ((labelLen & 0x3F) << 8) + data[offset++];
            tmp = offset;
            offset = pointer;
            continue;
        }
        if (offset + labelLen >= dataLen) {
            // error
            Log::Debug("label length out of range");
            return nullptr;
        }
        record->d_name += std::string(data + offset, data + offset + labelLen);
        record->d_name += ".";
        offset += labelLen;
    }
    if (tmp != 0) {
        offset = tmp;
    } else {
        offset++;
    }
    if (offset + 10 >= dataLen) {
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
    if (offset + record->d_rdLength > dataLen) {
        // error
        Log::Debug("not enough left to parse rData");
        return nullptr;
    }
    memcpy(record->d_rData, data + offset, std::min(record->d_rdLength, s_maxDataLen));
    offset += record->d_rdLength;
    return record;
}

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

uint8_t *Query::encodeHostName(const std::string &hostname, unsigned int &outLen)
{
    outLen = hostname.length();
    outLen += std::count(hostname.begin(), hostname.end(), '.');

    unsigned int offset = 0;
    uint8_t *encoding = new uint8_t[outLen];
    memset(encoding, 0, outLen);
    Tokenizer tok(hostname, '.');
    for (auto t = tok.begin(); t != tok.end(); ++t) {
        encoding[offset++] = (*t).length();
        memcpy(encoding + offset, (*t).c_str(), (*t).length());
        offset += (*t).length();
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

    unsigned int hostNameLen = 0;
    uint8_t *encodedHostName = encodeHostName(hostname, hostNameLen);
    Log::Debug("encoding header len is ", hostNameLen);

    const unsigned int requestLen = headerLen + hostNameLen + 4;
    uint8_t *request = new uint8_t[requestLen];
    memset(request, 0, requestLen);

    // TODO make random
    const uint16_t transactionID = 0x7598;
    const uint16_t flags = 0x0100;
    const uint16_t numQuestions = 0x1;
    const uint16_t answers = 0;
    const uint16_t authorities = 0;
    const uint16_t additional = 0;

    const uint16_t resourceRecordType = 0x1;  // A Record
    const uint16_t resourceRecordClass = 0x1; // IN (internet) Class

    unsigned int offset = 0;
    // Query header
    request[offset++] = (transactionID >> 8);
    request[offset++] = (transactionID & 0xFF);
    request[offset++] = (flags >> 8);
    request[offset++] = (flags & 0xFF);
    request[offset++] = (numQuestions >> 8);
    request[offset++] = (numQuestions & 0xFF);
    request[offset++] = (answers >> 8);
    request[offset++] = (answers & 0xFF);
    request[offset++] = (authorities >> 8);
    request[offset++] = (authorities & 0xFF);
    request[offset++] = (additional >> 8);
    request[offset++] = (additional & 0xFF);
    // Queries
    memcpy(request + offset, encodedHostName, hostNameLen);
    delete[] encodedHostName;
    offset += hostNameLen;
    request[offset++] = (resourceRecordType >> 8);
    request[offset++] = (resourceRecordType & 0xFF);
    request[offset++] = (resourceRecordClass >> 8);
    request[offset++] = (resourceRecordClass & 0xFF);

    char buf[3];
    for (unsigned int i = 0; i < requestLen; ++i) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%x", request[i]);
        std::cout << buf << " ";
    }
    std::cout << '\n';

    unsigned int bytesSend = sendto(sockfd, (const char *)request, requestLen, MSG_CONFIRM,
                                    (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    Log::Debug("wrote ", bytesSend, " of ", requestLen, " bytes");
    delete[] request;

    uint8_t response[512];
    socklen_t sockLen;
    int bytesRead = recvfrom(sockfd, response, sizeof(response), MSG_WAITALL, (struct sockaddr *)&serv_addr, &sockLen);
    close(sockfd);
    if (bytesRead < 0) {
        Log::Error("read failed, errno=", errno);
        return "";
    }
    Log::Debug("read ", bytesRead, " bytes in response");

    if (bytesRead < 12) {
        Log::Error("response too short");
        return "";
    }

    offset = 0;
    const uint16_t rTransactionID = (response[0] << 8) + response[1];
    const uint16_t rFlags = (response[2] << 8) + response[3];
    const uint16_t rNumQuestions = (response[4] << 8) + response[5];
    const uint16_t rAnswers = (response[6] << 8) + response[7];
    const uint16_t rAuthority = (response[8] << 8) + response[9];
    const uint16_t rAdditional = (response[10] << 8) + response[11];

    Log::Debug("transactionID=", rTransactionID, " flags=", rFlags, " questions=", rNumQuestions, " answers=", rAnswers,
               " authority=", rAuthority, " additional=", rAdditional);

    offset = 12;
    std::vector<std::shared_ptr<Question>> questions;
    for (unsigned int i = 0; i < rNumQuestions; i++) {
        auto question = Question::parse(response, bytesRead, offset);
        if (question) {
            Log::Info("question: ", *question.get());
            questions.push_back(question);
        } else {
            Log::Error("failed to parse question");
        }
    }

    std::vector<std::shared_ptr<ResourceRecord>> records;
    for (unsigned int i = 0; i < rAnswers; i++) {
        auto record = ResourceRecord::parse(response, bytesRead, offset);
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
    unsigned int questionLen = 0;
    uint8_t *encodedQuestion = question.encode(questionLen);

    const unsigned int requestLen = headerLen + questionLen;
    uint8_t *request = new uint8_t[requestLen];
    memset(request, 0, requestLen);

    // TODO make random
    const uint16_t transactionID = 0x7598;
    const uint16_t flags = 0x0100;
    const uint16_t numQuestions = 0x1;
    const uint16_t answers = 0;
    const uint16_t authorities = 0;
    const uint16_t additional = 0;

    unsigned int offset = 0;
    // Query header
    request[offset++] = (transactionID >> 8);
    request[offset++] = (transactionID & 0xFF);
    request[offset++] = (flags >> 8);
    request[offset++] = (flags & 0xFF);
    request[offset++] = (numQuestions >> 8);
    request[offset++] = (numQuestions & 0xFF);
    request[offset++] = (answers >> 8);
    request[offset++] = (answers & 0xFF);
    request[offset++] = (authorities >> 8);
    request[offset++] = (authorities & 0xFF);
    request[offset++] = (additional >> 8);
    request[offset++] = (additional & 0xFF);
    // Queries
    memcpy(request + offset, encodedQuestion, questionLen);
    delete[] encodedQuestion;

    unsigned int bytesSend = sendto(sockfd, (const char *)request, requestLen, MSG_CONFIRM,
                                    (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (bytesSend != requestLen) {
        close(sockfd);
        return nullptr;
    }
    delete[] request;

    uint8_t response[512];
    socklen_t sockLen;
    int retries = 3;
    int bytesRead = -1;
    while (retries-- > 0 && bytesRead < 0) {
        bytesRead = recvfrom(sockfd, response, sizeof(response), MSG_WAITALL, (struct sockaddr *)&serv_addr, &sockLen);
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
        auto question = Question::parse(response, bytesRead, offset);
        if (question) {
            questions.push_back(question);
        } else {
            Log::Error("failed to parse question");
        }
    }

    std::vector<std::shared_ptr<ResourceRecord>> records;
    std::shared_ptr<ResourceRecord> result;
    for (unsigned int i = 0; i < rAnswers; i++) {
        auto record = ResourceRecord::parse(response, bytesRead, offset);
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

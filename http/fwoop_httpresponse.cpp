#include <fwoop_httpresponse.h>

#include <cstring>

#include <fwoop_filereader.h>
#include <fwoop_log.h>
#include <fwoop_tokenizer.h>

namespace fwoop {

HttpResponse::HttpResponse() : d_status(), d_body() {}

void HttpResponse::streamFile(const std::string &fileName)
{
    d_fileName = fileName;
    auto ext = FileReader::getExtension(d_fileName);
    if (ext == "html") {
        d_headers.push_back({HttpHeader::ContentType, "text/html"});
    } else {
        d_headers.push_back({HttpHeader::ContentType, "text/plain"});
    }
}

uint8_t *HttpResponse::encode(uint32_t &length) const
{
    static const std::string VERSION = "HTTP/1.1";
    length = VERSION.length() + 1 + d_status.length() + 2;

    uint8_t *fileContents = nullptr;
    uint32_t contentLength;
    if (d_fileName.length() > 0) {
        FileReader reader(d_fileName);
        if (0 != reader.open()) {
            Log::Error("could not open file=", d_fileName);
        } else {
            fileContents = reader.loadFile(contentLength);
        }
    } else {
        contentLength = d_body.length();
    }
    length += contentLength;

    std::vector<HttpHeaderField_t> headers(d_headers);
    if (contentLength > 0) {
        headers.push_back({HttpHeader::ContentLength, std::to_string(contentLength)});
    }

    for (auto header : headers) {
        if (std::holds_alternative<HttpHeader>(header.first)) {
            length += httpHeaderToString(std::get<HttpHeader>(header.first)).length() + 2;
        } else if (std::holds_alternative<HttpCustomHeader>(header.first)) {
            length += std::get<HttpCustomHeader>(header).length() + 2;
        }
        length += header.second.length() + 2;
    }
    length += 2;

    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    uint32_t offset = 0;
    memcpy(encoding, VERSION.data(), VERSION.length());
    offset += VERSION.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, d_status.data(), d_status.length());
    offset += d_status.length();
    encoding[offset++] = '\r';
    encoding[offset++] = '\n';
    for (auto header : headers) {
        if (std::holds_alternative<HttpHeader>(header.first)) {
            const std::string name = httpHeaderToString(std::get<HttpHeader>(header.first));
            memcpy(encoding + offset, name.data(), name.length());
            offset += name.length();
        } else if (std::holds_alternative<HttpCustomHeader>(header.first)) {
            const std::string name = std::get<HttpCustomHeader>(header.first);
            memcpy(encoding + offset, name.data(), name.length());
            offset += name.length();
        }
        encoding[offset++] = ':';
        encoding[offset++] = ' ';
        memcpy(encoding + offset, header.second.data(), header.second.length());
        offset += header.second.length();
        encoding[offset++] = '\r';
        encoding[offset++] = '\n';
    }
    encoding[offset++] = '\r';
    encoding[offset++] = '\n';

    if (nullptr != fileContents) {
        memcpy(encoding + offset, fileContents, contentLength);
        delete[] fileContents;
    } else {
        memcpy(encoding + offset, d_body.data(), d_body.length());
    }
    offset += contentLength;
    return encoding;
}

std::shared_ptr<HttpResponse> HttpResponse::parse(uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesParsed)
{
    auto resp = std::make_shared<HttpResponse>();

    bytesParsed = 0;

    std::string payload((char *)buffer, bufferSize);
    unsigned int end = payload.rfind("\r\n\r\n");
    if (end == std::string::npos) {
        return nullptr;
    } else {
        bytesParsed = end + 4;
    }

    payload.resize(end);
    Tokenizer tokr(payload, '\n');
    auto itr = tokr.begin();

    Tokenizer reqLineTokr(*itr, ' ');
    auto rlItr = reqLineTokr.begin();
    resp->d_version = HttpVersion::fromString(*rlItr);
    ++rlItr;
    ++rlItr;
    unsigned int statusEnd = (*rlItr).find('\r');
    resp->d_status = (*rlItr).substr(0, statusEnd);
    unsigned int contentLength = 0;

    for (++itr; itr != tokr.end(); ++itr) {
        size_t split = (*itr).find(':');
        if (split == std::string::npos) {
            Log::Warn("Received bad response header: ", *itr);
        } else {
            std::string name = (*itr).substr(0, split);
            unsigned int valueEnd = (*itr).rfind('\r');
            std::string value = (*itr).substr(split + 2, valueEnd - split - 2);
            HttpHeader httpName = stringToHttpHeader(name);
            if (httpName != HttpHeader::Undefined) {
                resp->d_headers.push_back({httpName, value});
            } else {
                resp->d_headers.push_back({name, value});
            }
            if (httpName == HttpHeader::ContentLength) {
                contentLength = std::atoi(value.c_str());
            }
        }
    }

    if (bytesParsed + contentLength > bufferSize) {
        Log::Warn("incomplete response body, missing ", bytesParsed + contentLength - bufferSize, " bytes");
        contentLength = std::min(bufferSize - bytesParsed, contentLength);
    }
    resp->d_body = std::string((char *)buffer + bytesParsed, contentLength);

    return resp;
}

std::ostream &operator<<(std::ostream &os, const HttpResponse &response)
{
    os << "[ version=" << response.getVersion() << " status=" << response.getStatus() << " headers=[ ";
    auto headers = response.getHeaders();
    for (unsigned int i = 0; i < headers.size(); ++i) {
        os << "[ name=" << headers[i].first << " value=" << headers[i].second << " ]";
        if (i < headers.size() - 1) {
            os << ", ";
        }
    }
    os << " ] file=" << response.getFile() << " body=" << response.getBody() << " ]";
    return os;
}

} // namespace fwoop

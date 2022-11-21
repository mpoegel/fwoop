#include <fwoop_httpserverevent.h>

#include <fwoop_httpresponse.h>
#include <fwoop_log.h>
#include <fwoop_socketio.h>

namespace fwoop {

HttpServerEvent::HttpServerEvent(int fd)
: d_fd(fd)
{}

HttpServerEvent::~HttpServerEvent()
{
    HttpResponse finalResponse;
    finalResponse.setStatus("204 No Content");
    uint32_t len;
    uint8_t *out = finalResponse.encode(len);
    int rc = SocketIO::write(d_fd, out, len);
    delete []out;
    if (0 != rc) {
        Log::Warn("failed to write final response");
    }
}

bool HttpServerEvent::pushEvent(const std::string& event, const std::string& data)
{
    static const char* EVENT_PREFIX = "event: ";
    static const char* DATA_PREFIX = "data: ";
    static const unsigned int EVENT_PREFIX_LEN = strlen(EVENT_PREFIX);
    static const unsigned int DATA_PREFIX_LEN = strlen(DATA_PREFIX);

    const unsigned int outLen = EVENT_PREFIX_LEN + DATA_PREFIX_LEN + event.length() + data.length() + 2;
    uint8_t *out = new uint8_t[outLen];
    memset(out, 0, outLen);
    unsigned int offset = 0;
    memcpy(out + offset, EVENT_PREFIX, EVENT_PREFIX_LEN);
    offset += EVENT_PREFIX_LEN;
    memcpy(out + offset, event.data(), event.length());
    offset += event.length();
    out[offset++] = '\n';
    memcpy(out + offset, DATA_PREFIX, DATA_PREFIX_LEN);
    offset += DATA_PREFIX_LEN;
    memcpy(out + offset, data.data(), data.length());
    offset += data.length();
    out[offset++] = '\n';

    int rc = SocketIO::write(d_fd, out, outLen);
    delete []out;
    return 0 == rc;
}

}

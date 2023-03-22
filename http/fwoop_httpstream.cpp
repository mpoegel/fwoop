#include <fwoop_httpstream.h>

namespace fwoop {

uint32_t HttpStream::s_nextID = 2;

HttpStream::HttpStream(int clientFd) : d_id(s_nextID), d_fd(clientFd) { s_nextID += 2; }

int HttpStream::open() { return 0; }

int HttpStream::close() { return 0; }

} // namespace fwoop

#include <fwoop_httpserver.h>
#include <fwoop_log.h>
#include <fwoop_socketio.h>
#include <fwoop_tokenizer.h>
#include <fwoop_httpframe.h>
#include <fwoop_httpheadersframe.h>
#include <fwoop_httpsettingsframe.h>
#include <fwoop_httpdataframe.h>
#include <fwoop_httpgoawayframe.h>
#include <fwoop_httphpacker.h>

#include <cstring>
#include <system_error>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace fwoop {

HttpServer::HttpServer(int port)
: d_port(port)
, d_serverFd(-1)
{}

HttpServer::~HttpServer()
{
    if (d_serverFd >= 0) {
        close(d_serverFd);
    }
}

int HttpServer::serve()
{
    d_serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (d_serverFd < 0) {
        std::cerr << "failed to create socket, errno=" << errno << '\n';
        return -1;
    }

    const int opt = 1;
    if (0 != setsockopt(d_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "failed to setsockopt, errno=" << errno << '\n';
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(d_port);

    if (0 != bind(d_serverFd, (struct sockaddr*)&addr, sizeof(addr))) {
        std::cerr << "failed to bind, errno=" << errno << '\n';
        return -1;
    }

    const int maxQueued = 1;
    if (0 != listen(d_serverFd, maxQueued)) {
        std::cerr << "failed to listen, errno" << errno << '\n';
        return -1;
    }

    struct sockaddr_in clientAddr;
    socklen_t addrLen;
    int clientFd = accept4(d_serverFd, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen, SOCK_NONBLOCK);
    if (clientFd < 0) {
        std::cerr << "failed to accept, errno" << errno << '\n';
        return -1;
    }

    return handleConnection(clientFd);
}


int HttpServer::handleConnection(int clientFd) const
{
    constexpr unsigned int bufferSize = 1024;
    uint8_t buffer[bufferSize];
    unsigned int bytesRead = 0;
    std::error_code ec = SocketIO::read(clientFd, buffer, bufferSize, bytesRead);
    if (ec && ec.value() != ETIMEDOUT) {
        close(clientFd);
    }
    unsigned int bytesParsed = 0;
    int rc;
    rc = parsePayloadBody(buffer, bytesRead, bytesParsed);

    std::string resp = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: h2c\r\n\r\n";
    std::cout << "> " << resp;

    rc = SocketIO::write(clientFd, (uint8_t*)resp.c_str(), resp.length());
    if (0 != rc) {
        close(clientFd);
        return -1;
    }
    ::usleep(5 * 1000);

    ec = SocketIO::read(clientFd, buffer, bufferSize, bytesRead);
    if (ec && ec.value() != ETIMEDOUT) {
        close(clientFd);
        return -1;
    }
    rc = parsePayloadBody(buffer, bytesRead, bytesParsed);

    std::cout << "parsed " << bytesParsed << " of " << bytesRead << " bytes read\n";
    
    if (bytesParsed == bytesRead) {
        ec = SocketIO::read(clientFd, buffer, bufferSize, bytesRead);
        if (ec && ec.value() != ETIMEDOUT) {
            close(clientFd);
        }
        bytesParsed = 0;
    }

    char buf[3];
    unsigned int offset = bytesParsed;
    unsigned int itrs = 0;
    unsigned int totalRead = bytesRead;
    unsigned int totalParsed = bytesParsed;
    std::unique_ptr<HttpFrame> settingsFrame;
    while (totalParsed < totalRead) {
        std::cout << "read buffer: ";
        for (unsigned int i = offset; i < bytesRead; ++i) {
            sprintf(buf, "%x", buffer[i]);
            std::cout << buf << " ";
        }
        std::cout << '\n';

        auto frame = HttpFrame::parse(buffer + offset, bytesRead, bytesParsed);
        totalParsed += bytesParsed;
        std::cout << "parsed " << totalParsed << " of " << totalRead << " bytes read\n";
        if (frame) {
            std::cout << "< ";
            frame->printHex();
            offset += bytesParsed;
            if (frame->type() == HttpFrame::Type::Settings) {
                settingsFrame.swap(frame);
            }
        } else {
            Log::Error("no frame");

            if (bytesRead > bytesParsed) {
                memcpy(buffer, buffer + bytesParsed, bytesRead - bytesParsed);
                offset = (bytesRead - bytesParsed);
            }

            ec = SocketIO::read(clientFd, buffer + offset, bufferSize - offset, bytesRead);
            if (ec) {
                if (ec.value() == ETIMEDOUT) {
                    break;
                } else {
                    close(clientFd);
                }
            }
            std::cout << "read " << bytesRead << " more bytes\n";
            totalRead += bytesRead;
            bytesParsed = 0;
        }
    }

    // std::cout << "2 read buffer: ";
    // char buf[3];
    // for (unsigned int i = bytesParsed; i < bytesRead; ++i) {
    //     sprintf(buf, "%x", buffer[i]);
    //     std::cout << buf << " ";
    // }
    // std::cout << '\n';

    // close(clientFd);
    // return 0;

    // auto frame = HttpFrame::parse(buffer, bytesRead + offset, bytesParsed);
    // if (frame) {
    //     std::cout << "< ";
    //     frame->printHex();
    // } else {
    //     std::cerr << "no frame\n";
    // }
    // std::cout << "parsed " << bytesParsed << " of " << bytesRead + offset << " bytes read\n";
    // std::cout << "read buffer: ";
    // for (unsigned int i = bytesParsed; i < bytesRead + offset; ++i) {
    //     sprintf(buf, "%x", buffer[i]);
    //     std::cout << buf << " ";
    // }
    // std::cout << '\n';

    auto settingsFrameResp = HttpSettingsFrame();
    settingsFrameResp.setAck();
    uint8_t *settingsFrameEnc = settingsFrame->encode();
    std::cout << "> ";
    settingsFrame->printHex();
    rc = SocketIO::write(clientFd, settingsFrameEnc, settingsFrame->encodingLength());
    delete settingsFrameEnc;
    if (0 != rc) {
        close(clientFd);
        return -1;
    }

    ec = SocketIO::read(clientFd, buffer, bufferSize, bytesRead);
    if (ec) {
        close(clientFd);
        return -1;
    }

    auto frame = HttpFrame::parse(buffer, bytesRead, bytesParsed);
    if (frame) {
        std::cout << "< ";
        frame->printHex();
    } else {
        std::cerr << "no frame 2\n";
    }

    // auto settingsFrameAck = HttpSettingsFrame();
    // settingsFrameAck.setAck();
    // std::cout << "> ";
    // settingsFrameAck.printHex();
    // uint8_t *settingsFrameAckEnc = settingsFrameAck.encode();
    // rc = SocketIO::write(clientFd, settingsFrameAckEnc, settingsFrameAck.encodingLength());
    // delete settingsFrameAckEnc;
    // if (0 != rc) {
    //     close(clientFd);
    //     return -1;
    // }
    // ::sleep(2);

    auto packer = std::make_shared<HttpHPacker>();
    auto headersFrame = std::make_shared<HttpHeadersFrame>(packer);
    headersFrame->setStreamID(1);
    headersFrame->addHeaderBlock(HttpHeader::Status, "200");
    // headersFrame->addHeaderBlock(HttpHeader::Location, "localhost");
    headersFrame->addFlag(HttpHeadersFrame::Flag::EndHeaders);

    uint8_t *headers = headersFrame->encode();
    std::cout << "> ";
    headersFrame->printHex();

    rc = SocketIO::write(clientFd, headers, headersFrame->encodingLength());
    delete headers;
    if (0 != rc) {
        close(clientFd);
        return -1;
    }

    auto dataFrame = HttpDataFrame();
    dataFrame.setStreamID(1);
    dataFrame.addFlag(HttpDataFrame::Flag::EndStream);
    std::string respData = "hello world!";
    dataFrame.setData((uint8_t*)respData.data(), respData.length());
    uint8_t *data = dataFrame.encode();
    std::cout << "> ";
    dataFrame.printHex();

    rc = SocketIO::write(clientFd, data, dataFrame.encodingLength());
    delete data;
    if (0 != rc) {
        close(clientFd);
        return -1;
    }

    auto goAwayFrame = HttpGoAwayFrame();
    goAwayFrame.setLastStreamID(1);
    uint8_t *goAway = goAwayFrame.encode();
    std::cout << "> ";
    goAwayFrame.printHex();

    rc = SocketIO::write(clientFd, goAway, goAwayFrame.encodingLength());
    delete goAway;
    if (0 != rc) {
        close(clientFd);
        return -1;
    }

    close(clientFd);
    Log::Info("all done!");

    return 0;
}

int HttpServer::parsePayloadBody(uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesParsed) const
{
    bytesParsed = 0;
    std::string payload((char*)buffer, bufferSize);
    unsigned int end = payload.rfind("\r\n\r\n");
    if (end == std::string::npos) {
        bytesParsed = bufferSize;
    } else {
        bytesParsed = end + 4;
    }

    payload.resize(bytesParsed);
    Tokenizer tokr(payload, '\n');
    for (auto itr = tokr.begin(); itr != tokr.end(); ++itr) {
        std::cout << "< " << *itr << '\n';
    }

    return 0;
}

}

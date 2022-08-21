#include <iostream>

#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_httpserver.h>
#include <fwoop_httpheader.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "USAGE: " << argv[0] << " <port> <http version>\n";
        return -1;
    }

    const int port = std::atoi(argv[1]);
    const int rawVersion = std::stoi(argv[2]);

    fwoop::HttpVersion version = fwoop::HTTP11;
    if (rawVersion == 1) {
        version = fwoop::HttpVersion::HTTP11;
    } else if (rawVersion == 2) {
        version = fwoop::HttpVersion::HTTP2;
    } else {
        std::cerr << "bad version: " << argv[2] << '\n';
    }

    fwoop::HttpHandlerFunc_t handler = [](const fwoop::HttpRequest& request, fwoop::HttpResponse& response) {
        response.setStatus("200 OK");
        response.addHeader(fwoop::HttpHeader::ContentType, "text/plain");

        const std::string content = "welcome, world!";
        response.setBody(content);
    };

    fwoop::HttpServer server(port, version);
    server.addRoute("/foo", handler);
    server.addStaticRoute("/bar", "../basis/testdata/test.txt");

    if (0 != server.serve()) {
        return 1;
    }

    return 0;
}

#include <iostream>

#include <fwoop_httpheader.h>
#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_httpserver.h>
#include <fwoop_httpserverevent.h>
#include <fwoop_httpversion.h>
#include <fwoop_openmetrics.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "USAGE: " << argv[0] << " <port> <http version>\n";
        return -1;
    }

    const int port = std::atoi(argv[1]);
    const int rawVersion = std::stoi(argv[2]);

    fwoop::HttpVersion::Value version = fwoop::HttpVersion::Http1_1;
    if (rawVersion == 1) {
        version = fwoop::HttpVersion::Http1_1;
    } else if (rawVersion == 2) {
        version = fwoop::HttpVersion::Http2;
    } else {
        std::cerr << "bad version: " << argv[2] << '\n';
    }

    fwoop::OpenMetricsPublisher metrics;
    metrics.start();
    auto counter = metrics.newCounter("requests", "hits");
    counter->addLabel({"path", "/foo"});
    counter->addLabel({"response", "200"});

    fwoop::HttpHandlerFunc_t handler = [&counter](const fwoop::HttpRequest &request, fwoop::HttpResponse &response) {
        response.setStatus("200 OK");
        response.addHeader(fwoop::HttpHeader::ContentType, "text/plain");

        const std::string content = "welcome, world!";
        response.setBody(content);
        counter->increment();
    };

    fwoop::HttpServerEventHandlerFunc_t eventHandler = [](const fwoop::HttpRequest &request,
                                                          fwoop::HttpServerEvent &serverEvent) {
        serverEvent.pushEvent("ping", "hello world");
        serverEvent.pushEvent("ping", "hello world");
        serverEvent.pushEvent("pong", "hello everyone");
    };

    fwoop::HttpServer server(port, version);
    server.addRoute("/foo", handler);
    server.addStaticRoute("/bar", "examples/example.html");
    server.addServerEventRoute("/pop", eventHandler);

    if (0 != server.serve()) {
        return 1;
    }

    return 0;
}

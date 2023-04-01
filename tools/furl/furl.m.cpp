#include <iostream>
#include <memory>

#include <fwoop_argparser.h>
#include <fwoop_httpclient.h>
#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_log.h>

int main(int argc, const char *argv[])
{
    auto args = fwoop::ArgParser(argv, argc);
    args.addPositionalArg("url", "", "URL of request");
    args.addNamedArg("verbose", "v", false, "enable verbose logs");

    std::error_code ec = args.parse();
    if (ec) {
        fwoop::Log::Error("invalid argument: ", ec.message());
        return 1;
    }
    bool isVerbose = args.getNamedArg<bool>("verbose");
    if (isVerbose) {
        fwoop::Log::SetThreshold(fwoop::Log::e_Debug);
    } else {
        fwoop::Log::SetThreshold(fwoop::Log::e_Info);
    }

    fwoop::Log::Debug("arguments: ", args);

    std::string url = args.getPositionalArg<std::string>("url");
    const auto splitOnIndex = url.find('/');
    std::string hostname;
    std::string path;
    if (splitOnIndex != std::string::npos) {
        hostname = url.substr(0, splitOnIndex);
        path = url.substr(splitOnIndex);
    } else {
        hostname = url;
        path = "/";
    }

    auto request = fwoop::HttpRequest();
    request.setMethod(fwoop::HttpRequest::Method::Get);
    request.setPath(path);
    request.setVersion(fwoop::HttpVersion::Http1_1);
    request.addHeader(fwoop::HttpHeader::Host, hostname);

    fwoop::Log::Debug("sending request: ", request);

    auto client = fwoop::HttpClient(hostname);
    auto response = std::make_shared<fwoop::HttpResponse>();

    ec = client.makeReqest(request, response);
    if (ec) {
        fwoop::Log::Error("request failed: ", ec);
        return 1;
    }

    fwoop::Log::Debug("response: ", *response);

    std::cout << response->getBody() << std::endl;

    return 0;
}

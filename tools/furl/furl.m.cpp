#include <cstdint>
#include <iostream>
#include <memory>

#include <fwoop_argparser.h>
#include <fwoop_httpclient.h>
#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_json.h>
#include <fwoop_log.h>

int main(int argc, const char *argv[])
{
    auto args = fwoop::ArgParser(argv, argc);
    args.addPositionalArg("url", "", "URL of request");
    args.addNamedArg("verbose", "v", false, "enable verbose logs");
    args.addNamedArg("json", "j", false, "parse response body as JSON");

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

    const uint32_t bodyLen = response->getBody().length();
    fwoop::Log::Debug("response body length: ", bodyLen);

    bool doParseJson = args.getNamedArg<bool>("json");
    if (doParseJson) {
        uint32_t bytesParsed;
        uint8_t *body = (uint8_t *)response->getBody().c_str();
        auto json = fwoop::JsonObject(body, bodyLen, bytesParsed);
        uint32_t prettyLen = 0;
        auto prettyJson = json.encode(prettyLen, 2);
        std::cout << std::string((char *)prettyJson, prettyLen) << std::endl;
        delete[] prettyJson;
    } else {
        std::cout << response->getBody() << std::endl;
    }

    return 0;
}

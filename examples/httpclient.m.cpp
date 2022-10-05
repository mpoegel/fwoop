#include <iostream>
#include <memory>

#include <fwoop_log.h>
#include <fwoop_httpclient.h>
#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>

int main(int argc, char* argv[])
{
    auto request = fwoop::HttpRequest();
    request.setMethod(fwoop::HttpRequest::Method::Get);
    request.setPath("/foo");

    fwoop::Log::Info("sending request: ", request);

    auto client = fwoop::HttpClient("localhost", 8088);
    auto response = std::make_shared<fwoop::HttpResponse>();

    std::error_code ec = client.makeReqest(request, response);
    if (ec) {
        fwoop::Log::Error("request failed: ", ec);
        return 1;
    }

    fwoop::Log::Info("response: ", *response);

    return 0;
}
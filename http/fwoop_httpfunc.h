#pragma once

#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_httpserverevent.h>

#include <functional>

namespace fwoop {

typedef std::function<void(const HttpRequest &, HttpResponse &)> HttpHandlerFunc_t;
typedef std::function<void(const HttpRequest &, HttpServerEvent &)> HttpServerEventHandlerFunc_t;

} // namespace fwoop

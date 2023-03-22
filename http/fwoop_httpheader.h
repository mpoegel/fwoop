#pragma once

#include <ostream>
#include <string>
#include <variant>

namespace fwoop {

enum HttpHeader {
    Undefined,
    Authority,
    Method,
    Path,
    Scheme,
    Status,
    AcceptCharset,
    AcceptEncoding,
    AcceptLanguage,
    AcceptRanges,
    Accept,
    AccessControlAllowOrigin,
    Age,
    Allow,
    Authorization,
    CacheControl,
    ContentDisposition,
    ContentEncoding,
    ContentLanguage,
    ContentLength,
    ContentLocation,
    ContentRange,
    ContentType,
    Cookie,
    Date,
    Etag,
    Expect,
    Expires,
    From,
    Host,
    IfMatch,
    IfModifiedSince,
    IfNoneMatch,
    IfRange,
    IfUnmodifiedSince,
    LastModified,
    Link,
    Location,
    MaxForwards,
    ProxyAuthenticate,
    ProxyAuthorization,
    Range,
    Referer,
    Refresh,
    RetryAfter,
    Server,
    SetCookie,
    StrictTransportSecurity,
    TransferEncoding,
    UserAgent,
    Vary,
    Via,
    WwwAuthenticate,
};

std::string httpHeaderToString(HttpHeader header);
HttpHeader stringToHttpHeader(const std::string &headerStr);

typedef std::string HttpCustomHeader;
typedef std::variant<HttpHeader, HttpCustomHeader> HttpHeaderVariant_t;
typedef std::pair<HttpHeaderVariant_t, std::string> HttpHeaderField_t;

std::ostream &operator<<(std::ostream &os, const HttpHeaderVariant_t &header);

} // namespace fwoop

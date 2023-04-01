#include <fwoop_httpheader.h>

#include <algorithm>
#include <cctype>

namespace fwoop {

std::string httpHeaderToString(HttpHeader header)
{
    switch (header) {
    case Authority:
        return ":authority";
    case Method:
        return ":method";
    case Path:
        return ":path";
    case Scheme:
        return ":scheme";
    case Status:
        return ":status";
    case AcceptCharset:
        return "accept-charset";
    case AcceptEncoding:
        return "accept-encoding";
    case AcceptLanguage:
        return "accept-language";
    case AcceptRanges:
        return "accept-ranges";
    case Accept:
        return "accept";
    case AccessControlAllowOrigin:
        return "access-control-allow-origin";
    case Age:
        return "age";
    case Allow:
        return "allow";
    case Authorization:
        return "authorization";
    case CacheControl:
        return "cache-control";
    case ContentDisposition:
        return "content-disposition";
    case ContentEncoding:
        return "content-encoding";
    case ContentLanguage:
        return "content-language";
    case ContentLength:
        return "content-length";
    case ContentLocation:
        return "content-location";
    case ContentRange:
        return "content-range";
    case ContentType:
        return "content-type";
    case Cookie:
        return "cookie";
    case Date:
        return "date";
    case Etag:
        return "etag";
    case Expect:
        return "expect";
    case Expires:
        return "expires";
    case From:
        return "from";
    case Host:
        return "host";
    case IfMatch:
        return "if-match";
    case IfModifiedSince:
        return "if-modified-since";
    case IfNoneMatch:
        return "if-none-match";
    case IfRange:
        return "if-range";
    case IfUnmodifiedSince:
        return "if-unmodified-since";
    case LastModified:
        return "last-modified";
    case Link:
        return "link";
    case Location:
        return "location";
    case MaxForwards:
        return "max-forwards";
    case ProxyAuthenticate:
        return "proxy-authenticate";
    case ProxyAuthorization:
        return "proxy-authorization";
    case Range:
        return "range";
    case Referer:
        return "referer";
    case Refresh:
        return "refresh";
    case RetryAfter:
        return "retry-after";
    case Server:
        return "server";
    case SetCookie:
        return "set-cookie";
    case StrictTransportSecurity:
        return "strict-transport-security";
    case TransferEncoding:
        return "transfer-encoding";
    case UserAgent:
        return "user-agent";
    case Vary:
        return "vary";
    case Via:
        return "via";
    case WwwAuthenticate:
        return "www-authenticate";
    }
    return "unknown";
}

HttpHeader stringToHttpHeader(const std::string &headerStr)
{
    auto header(headerStr);
    std::transform(header.begin(), header.end(), header.begin(), [](uint8_t c) { return std::tolower(c); });
    if (header == ":authority")
        return Authority;
    if (header == ":method")
        return Method;
    if (header == ":path")
        return Path;
    if (header == ":scheme")
        return Scheme;
    if (header == ":status")
        return Status;
    if (header == "accept-charset")
        return AcceptCharset;
    if (header == "accept-encoding")
        return AcceptEncoding;
    if (header == "accept-language")
        return AcceptLanguage;
    if (header == "accept-ranges")
        return AcceptRanges;
    if (header == "accept")
        return Accept;
    if (header == "access-control-allow-origin")
        return AccessControlAllowOrigin;
    if (header == "age")
        return Age;
    if (header == "allow")
        return Allow;
    if (header == "authorization")
        return Authorization;
    if (header == "cache-control")
        return CacheControl;
    if (header == "content-disposition")
        return ContentDisposition;
    if (header == "content-encoding")
        return ContentEncoding;
    if (header == "content-language")
        return ContentLanguage;
    if (header == "content-length")
        return ContentLength;
    if (header == "content-location")
        return ContentLocation;
    if (header == "content-range")
        return ContentRange;
    if (header == "content-type")
        return ContentType;
    if (header == "cookie")
        return Cookie;
    if (header == "date")
        return Date;
    if (header == "etag")
        return Etag;
    if (header == "expect")
        return Expect;
    if (header == "expires")
        return Expires;
    if (header == "from")
        return From;
    if (header == "host")
        return Host;
    if (header == "if-match")
        return IfMatch;
    if (header == "if-modified-since")
        return IfModifiedSince;
    if (header == "if-none-match")
        return IfNoneMatch;
    if (header == "if-range")
        return IfRange;
    if (header == "if-unmodified-since")
        return IfUnmodifiedSince;
    if (header == "last-modified")
        return LastModified;
    if (header == "link")
        return Link;
    if (header == "location")
        return Location;
    if (header == "max-forwards")
        return MaxForwards;
    if (header == "proxy-authenticate")
        return ProxyAuthenticate;
    if (header == "proxy-authorization")
        return ProxyAuthorization;
    if (header == "range")
        return Range;
    if (header == "referer")
        return Referer;
    if (header == "refresh")
        return Refresh;
    if (header == "retry-after")
        return RetryAfter;
    if (header == "server")
        return Server;
    if (header == "set-cookie")
        return SetCookie;
    if (header == "strict-transport-security")
        return StrictTransportSecurity;
    if (header == "transfer-encoding")
        return TransferEncoding;
    if (header == "user-agent")
        return UserAgent;
    if (header == "vary")
        return Vary;
    if (header == "via")
        return Via;
    if (header == "www-authenticate")
        return WwwAuthenticate;
    return Undefined;
}

std::ostream &operator<<(std::ostream &os, const HttpHeaderVariant_t &header)
{
    if (std::holds_alternative<HttpHeader>(header)) {
        os << httpHeaderToString(std::get<HttpHeader>(header));
    } else if (std::holds_alternative<HttpCustomHeader>(header)) {
        os << std::get<HttpCustomHeader>(header);
    }
    return os;
}

} // namespace fwoop

#include <fwoop_httpheader.h>

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
    if (headerStr == ":authority")
        return Authority;
    if (headerStr == ":method")
        return Method;
    if (headerStr == ":path")
        return Path;
    if (headerStr == ":scheme")
        return Scheme;
    if (headerStr == ":status")
        return Status;
    if (headerStr == "accept-charset")
        return AcceptCharset;
    if (headerStr == "accept-encoding")
        return AcceptEncoding;
    if (headerStr == "accept-language")
        return AcceptLanguage;
    if (headerStr == "accept-ranges")
        return AcceptRanges;
    if (headerStr == "accept")
        return Accept;
    if (headerStr == "access-control-allow-origin")
        return AccessControlAllowOrigin;
    if (headerStr == "age")
        return Age;
    if (headerStr == "allow")
        return Allow;
    if (headerStr == "authorization")
        return Authorization;
    if (headerStr == "cache-control")
        return CacheControl;
    if (headerStr == "content-disposition")
        return ContentDisposition;
    if (headerStr == "content-encoding")
        return ContentEncoding;
    if (headerStr == "content-language")
        return ContentLanguage;
    if (headerStr == "content-length")
        return ContentLength;
    if (headerStr == "content-location")
        return ContentLocation;
    if (headerStr == "content-range")
        return ContentRange;
    if (headerStr == "content-type")
        return ContentType;
    if (headerStr == "cookie")
        return Cookie;
    if (headerStr == "date")
        return Date;
    if (headerStr == "etag")
        return Etag;
    if (headerStr == "expect")
        return Expect;
    if (headerStr == "expires")
        return Expires;
    if (headerStr == "from")
        return From;
    if (headerStr == "host")
        return Host;
    if (headerStr == "if-match")
        return IfMatch;
    if (headerStr == "if-modified-since")
        return IfModifiedSince;
    if (headerStr == "if-none-match")
        return IfNoneMatch;
    if (headerStr == "if-range")
        return IfRange;
    if (headerStr == "if-unmodified-since")
        return IfUnmodifiedSince;
    if (headerStr == "last-modified")
        return LastModified;
    if (headerStr == "link")
        return Link;
    if (headerStr == "location")
        return Location;
    if (headerStr == "max-forwards")
        return MaxForwards;
    if (headerStr == "proxy-authenticate")
        return ProxyAuthenticate;
    if (headerStr == "proxy-authorization")
        return ProxyAuthorization;
    if (headerStr == "range")
        return Range;
    if (headerStr == "referer")
        return Referer;
    if (headerStr == "refresh")
        return Refresh;
    if (headerStr == "retry-after")
        return RetryAfter;
    if (headerStr == "server")
        return Server;
    if (headerStr == "set-cookie")
        return SetCookie;
    if (headerStr == "strict-transport-security")
        return StrictTransportSecurity;
    if (headerStr == "transfer-encoding")
        return TransferEncoding;
    if (headerStr == "user-agent")
        return UserAgent;
    if (headerStr == "vary")
        return Vary;
    if (headerStr == "via")
        return Via;
    if (headerStr == "www-authenticate")
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

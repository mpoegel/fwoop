#include <fwoop_httpheader.h>

namespace fwoop {

std::string httpHeaderToString(HttpHeader header)
{
    switch (header)
    {
        case Authority: return ":authority";
        case Method: return ":method";
        case Path: return ":path";
        case Scheme: return ":scheme";
        case Status: return ":status";
        case AcceptCharset: return "accept-charset";
        case AcceptEncoding: return "accept-encoding";
        case AcceptLanguage: return "accept-language";
        case AcceptRanges: return "accept-ranges";
        case Accept: return "accept";
        case AccessControlAllowOrigin: return "access-control-allow-origin";
        case Age: return "age";
        case Allow: return "allow";
        case Authorization: return "authorization";
        case CacheControl: return "cache-control";
        case ContentDisposition: return "content-disposition";
        case ContentEncoding: return "content-encoding";
        case ContentLanguage: return "content-language";
        case ContentLength: return "content-length";
        case ContentLocation: return "content-location";
        case ContentRange: return "content-range";
        case ContentType: return "content-type";
        case Cookie: return "cookie";
        case Date: return "date";
        case Etag: return "etag";
        case Expect: return "expect";
        case Expires: return "expires";
        case From: return "from";
        case Host: return "host";
        case IfMatch: return "if-match";
        case IfModifiedSince: return "if-modified-since";
        case IfNoneMatch: return "if-none-match";
        case IfRange: return "if-range";
        case IfUnmodifiedSince: return "if-unmodified-since";
        case LastModified: return "last-modified";
        case Link: return "link";
        case Location: return "location";
        case MaxForwards: return "max-forwards";
        case ProxyAuthenticate: return "proxy-authenticate";
        case ProxyAuthorization: return "proxy-authorization";
        case Range: return "range";
        case Referer: return "referer";
        case Refresh: return "refresh";
        case RetryAfter: return "retry-after";
        case Server: return "server";
        case SetCookie: return "set-cookie";
        case StrictTransportSecurity: return "strict-transport-security";
        case TransferEncoding: return "transfer-encoding";
        case UserAgent: return "user-agent";
        case Vary: return "vary";
        case Via: return "via";
        case WwwAuthenticate: return "www-authenticate";
    }
    return "unknown";
}

std::ostream& operator<<(std::ostream& os, const HttpHeaderVariant_t& header)
{
    if (std::holds_alternative<HttpHeader>(header)) {
        os << httpHeaderToString(std::get<HttpHeader>(header));
    } else if (std::holds_alternative<HttpCustomHeader>(header)) {
        os << std::get<HttpCustomHeader>(header);
    }
    return os;
}

}

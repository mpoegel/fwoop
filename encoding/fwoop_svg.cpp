#include <cstdint>
#include <fwoop_svg.h>
#include <string>

namespace fwoop {
namespace svg {

void Document::append(ElementPtr &elem) { d_body.push_back(elem); }

Array Document::encode() const
{
    Array arr(64);
    arr.append("<svg");
    uint32_t len = 5;
    if (width > 0) {
        std::string tmp(" width=\"" + std::to_string(width) + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    if (height > 0) {
        std::string tmp(" height=\"" + std::to_string(height) + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    static const std::string tmp(" xmlns=\"http://www.w3.org/2000/svg\">");
    arr.append(tmp);
    len += tmp.length();
    for (auto elem : d_body) {
        arr.extend(elem->encode());
    }
    static const std::string suffix("</svg>");
    arr.append(suffix);
    len += suffix.length();
    return arr;
}

Array Rect::encode() const
{
    Array arr(128);
    arr.append("<rect");
    uint32_t len = 5;
    if (width > 0) {
        std::string tmp(" width=\"" + std::to_string(width) + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    if (height > 0) {
        std::string tmp(" height=\"" + std::to_string(height) + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    if (x > 0) {
        std::string tmp(" x=\"" + std::to_string(x) + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    if (y > 0) {
        std::string tmp(" y=\"" + std::to_string(y) + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    if (fill.length() > 0) {
        std::string tmp(" fill=\"" + fill + '"');
        arr.append(tmp);
        len += tmp.length();
    }
    arr.append(" />");
    len += 3;
    arr.shrink(len);
    return arr;
}

} // namespace svg
} // namespace fwoop

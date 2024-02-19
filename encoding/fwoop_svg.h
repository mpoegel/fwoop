#pragma once

#include <fwoop_array.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace fwoop {
namespace svg {

class Element {
  public:
    virtual Array encode() const = 0;
};

typedef std::shared_ptr<Element> ElementPtr;

class Document : public Element {
  private:
    std::vector<ElementPtr> d_body;

  public:
    uint32_t width;
    uint32_t height;

    void append(ElementPtr &elem);
    Array encode() const override;
};

class Rect : public Element {
  public:
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    std::string fill;

    Array encode() const override;
};

} // namespace svg
} // namespace fwoop

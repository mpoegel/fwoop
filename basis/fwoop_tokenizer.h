#pragma once

#include <string>

namespace fwoop {

class Tokenizer {

  public:
    explicit Tokenizer(const std::string &parseString);

    std::string getToken();
};

}

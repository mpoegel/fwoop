#pragma once

#include <string>

namespace fwoop {

class Tokenizer {
  private:
    std::string d_parseString;
    char d_delim;

  public:
    class Iterator;

    Tokenizer(const std::string &parseString, char delim);
    ~Tokenizer();

    Iterator begin();
    Iterator end();

    bool operator==(const Tokenizer &rhs) const;
};

class Tokenizer::Iterator {
  private:
    Tokenizer &d_tokenizer;
    bool d_isEnd;
    std::size_t d_lastLoc;
    std::size_t d_loc;

  public:
    Iterator(Tokenizer &tokenizer, bool isEnd);
    ~Iterator();

    void operator++();
    std::string operator*() const;

    bool operator==(const Iterator &rhs) const;
    bool operator!=(const Iterator &rhs) const;
};

inline Tokenizer::~Tokenizer() {}

inline Tokenizer::Iterator::~Iterator() {}

inline bool Tokenizer::Iterator::operator!=(const Iterator &rhs) const { return !(*this == rhs); }

} // namespace fwoop

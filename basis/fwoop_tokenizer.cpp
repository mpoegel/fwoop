#include <fwoop_tokenizer.h>

namespace fwoop {

Tokenizer::Tokenizer(const std::string &parseString, char delim) : d_parseString(parseString), d_delim(delim) {}

Tokenizer::Iterator Tokenizer::begin() { return Iterator(*this, false); }

Tokenizer::Iterator Tokenizer::end() { return Iterator(*this, true); }

bool Tokenizer::operator==(const Tokenizer &rhs) const
{
    return d_parseString == rhs.d_parseString && d_delim == rhs.d_delim;
}

Tokenizer::Iterator::Iterator(Tokenizer &tokenizer, bool isEnd)
    : d_tokenizer(tokenizer), d_isEnd(isEnd), d_lastLoc(0), d_loc(0)
{
    if (d_isEnd) {
        d_loc = std::string::npos;
    } else {
        ++(*this);
    }
}

void Tokenizer::Iterator::operator++()
{
    if (d_loc == std::string::npos) {
        d_isEnd = true;
        return;
    }
    d_lastLoc = d_loc > 0 ? d_loc + 1 : 0;
    d_loc = d_tokenizer.d_parseString.find(d_tokenizer.d_delim, d_lastLoc);
}

std::string Tokenizer::Iterator::operator*() const
{
    return d_tokenizer.d_parseString.substr(d_lastLoc, d_loc - d_lastLoc);
}

bool Tokenizer::Iterator::operator==(const Iterator &rhs) const
{
    return d_tokenizer == rhs.d_tokenizer && d_isEnd == rhs.d_isEnd && d_loc == rhs.d_loc;
}

} // namespace fwoop

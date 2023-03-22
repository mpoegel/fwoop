#pragma once

#include <optional>
#include <string>
#include <system_error>
#include <unordered_map>
#include <variant>

namespace fwoop {

/**
 * @brief Template
 *
 * stmt -> variable | constant | expression
 * expression -> ifstatement endif | ifstatement elseStmt endif | forloop
 * ifstatement -> if variable comparator constant then constant | if constant comparator variable then constant | if
 * variable then constant elseStmt -> else constant comparator -> == | >= | <= | > | < forloop -> for scopeVariable in
 * variable stmt
 *
 */
class Template {
  public:
    typedef std::variant<std::string, int, double, bool> Variable_t;
    typedef std::unordered_map<std::string, Variable_t> Context_t;

  private:
    enum TemplateOperator {
        Undefined,
        LessThan,
        GreaterThan,
        Equal,
        LessThanOrEqual,
        GreaterThanOrEqual,
    };

    template <typename T> bool compare(TemplateOperator oper, const T &lhs, const T &rhs) const;

    uint8_t *d_bytes;
    unsigned int d_length;

    TemplateOperator parseOperator(unsigned int index, unsigned int &bytesParsed);

    Variable_t parseVariable(unsigned int index, const Context_t &context, unsigned int &bytesParsed);

    uint8_t *parseVariable(unsigned int index, const Context_t &context, unsigned int &bytesParsed,
                           unsigned int &outLength);

    Variable_t parseConstant(unsigned int index, unsigned int &bytesParsed);

    uint8_t *parseConditional(unsigned int index, const Context_t &context, unsigned int &bytesParsed,
                              unsigned int &outLength);

    uint8_t *parseKeyword(unsigned int index, const Context_t &context, unsigned int &bytesParsed,
                          unsigned int &outLength);

    bool isWhitespace(unsigned int index);

    void resize();

  public:
    Template(uint8_t *bytes, unsigned int bytesLength);
    ~Template();

    uint8_t *encode(const Context_t &context, unsigned int &encodingLength);
};

std::ostream &operator<<(std::ostream &os, const Template::Variable_t &var);

template <typename T> bool Template::compare(TemplateOperator oper, const T &lhs, const T &rhs) const
{
    switch (oper) {
    case TemplateOperator::Equal:
        return lhs == rhs;
    default:
        return false;
    }
}

} // namespace fwoop

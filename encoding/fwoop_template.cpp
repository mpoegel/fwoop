#include <fwoop_template.h>

#include <cstring>
#include <fwoop_log.h>
#include <string>

namespace fwoop {

namespace {

long unsigned int findWord(uint8_t *input, unsigned int inputLength, const char *word, unsigned int wordLength)
{
    if (inputLength < wordLength) {
        return std::string::npos;
    }
    for (unsigned int i = 0; i < inputLength - wordLength + 1; i++) {
        if (0 == memcmp(input + i, word, wordLength)) {
            return i;
        }
    }
    return std::string::npos;
}

} // namespace

std::ostream &operator<<(std::ostream &os, const Template::Variable_t &var)
{
    if (std::holds_alternative<std::string>(var)) {
        os << std::get<std::string>(var);
    } else if (std::holds_alternative<int>(var)) {
        os << std::get<int>(var);
    } else if (std::holds_alternative<double>(var)) {
        os << std::get<double>(var);
    } else {
        os << (std::get<bool>(var) ? "true" : "false");
    }
    return os;
}

Template::Template(uint8_t *bytes, unsigned int bytesLength) : d_length(bytesLength)
{
    d_bytes = new uint8_t[bytesLength];
    memcpy(d_bytes, bytes, bytesLength);
}

Template::~Template() { delete[] d_bytes; }

Template::TemplateOperator Template::parseOperator(unsigned int index, unsigned int &bytesParsed)
{
    static const char *LESS_EQUAL = "<=";
    static const char *EQUAL = "==";
    static const char *GREATER_EQUAL = ">=";
    if (d_length - index >= 2) {
        if (0 == memcmp(d_bytes + index, LESS_EQUAL, 2)) {
            bytesParsed = 2;
            return TemplateOperator::LessThanOrEqual;
        }
        if (0 == memcmp(d_bytes + index, EQUAL, 2)) {
            bytesParsed = 2;
            return TemplateOperator::Equal;
        }
        if (0 == memcmp(d_bytes + index, GREATER_EQUAL, 2)) {
            bytesParsed = 2;
            return TemplateOperator::GreaterThanOrEqual;
        }
    }
    if (d_length - index >= 1) {
        if (*(d_bytes + index) == '>') {
            bytesParsed = 1;
            return TemplateOperator::GreaterThan;
        }
        if (*(d_bytes + index) == '<') {
            bytesParsed = 1;
            return TemplateOperator::LessThan;
        }
    }

    return TemplateOperator::Undefined;
}

Template::Variable_t Template::parseVariable(unsigned int index, const Context_t &context, unsigned int &bytesParsed)
{
    for (auto itr = context.begin(); itr != context.end(); itr++) {
        // first check space requirement for variable name
        if (d_length - index < itr->first.length()) {
            continue;
        }
        std::string guess = std::string((char *)(d_bytes + index), itr->first.length());
        if (guess == itr->first) {
            bytesParsed = itr->first.length();
            return itr->second;
        }
    }
    bytesParsed = 0;
    return Variable_t();
}

uint8_t *Template::parseVariable(unsigned int index, const Context_t &context, unsigned int &bytesParsed,
                                 unsigned int &outLength)
{
    for (auto itr = context.begin(); itr != context.end(); itr++) {
        // first check space requirement for variable name
        if (d_length - index < itr->first.length()) {
            continue;
        }
        std::string guess = std::string((char *)(d_bytes + index), itr->first.length());
        if (guess == itr->first) {
            std::string val;
            if (std::holds_alternative<int>(itr->second)) {
                val = std::to_string(std::get<int>(itr->second));
            } else if (std::holds_alternative<double>(itr->second)) {
                val = std::to_string(std::get<double>(itr->second));
            } else if (std::holds_alternative<bool>(itr->second)) {
                val = std::get<bool>(itr->second) ? "true" : "false";
            } else {
                // assume we have a string
                std::string val = std::get<std::string>(itr->second);
            }
            uint8_t *result = new uint8_t[val.length()];
            memcpy(result, val.data(), val.length());
            bytesParsed = itr->first.length();
            outLength = val.length();
            return result;
        }
    }
    bytesParsed = 0;
    outLength = 0;
    return nullptr;
}

Template::Variable_t Template::parseConstant(unsigned int index, unsigned int &bytesParsed)
{
    Variable_t result;
    unsigned int tmpIndex = index;
    while (!isWhitespace(tmpIndex))
        tmpIndex++;
    unsigned int length = tmpIndex - index;
    if (0 == length) {
        return result;
    }
    std::string constant((char *)d_bytes + index, length);
    result = constant;
    return result;
}

uint8_t *Template::parseConditional(unsigned int index, const Context_t &context, unsigned int &bytesParsed,
                                    unsigned int &outLength)
{
    static const char *IF_KEYWORD = "if";
    static const char *THEN_KEYWORD = "then";
    static const char *ELSE_KEYWORD = "else";
    static const char *ENDIF_KEYWORD = "endif";
    if (0 != findWord(d_bytes + index, d_length - index, IF_KEYWORD, strlen(IF_KEYWORD))) {
        return nullptr;
    }
    long unsigned int thenIndex = findWord(d_bytes + index, d_length - index, THEN_KEYWORD, strlen(THEN_KEYWORD));
    if (std::string::npos == thenIndex) {
        return nullptr;
    }
    long unsigned int elseIndex =
        findWord(d_bytes + thenIndex, d_length - thenIndex, ELSE_KEYWORD, strlen(ELSE_KEYWORD));
    long unsigned int endifIndex =
        findWord(d_bytes + thenIndex, d_length - thenIndex, ENDIF_KEYWORD, strlen(ENDIF_KEYWORD));
    if (std::string::npos == endifIndex) {
        return nullptr;
    }
    bytesParsed = index - endifIndex + strlen(ENDIF_KEYWORD);

    unsigned int conditionIndex = strlen(IF_KEYWORD);
    while (isWhitespace(conditionIndex))
        conditionIndex++;

    Variable_t lhs;
    Variable_t rhs;
    TemplateOperator oper;

    if (std::string::npos != elseIndex) {
        elseIndex += thenIndex;
    }
    endifIndex += thenIndex;

    Log::Debug("then index=", thenIndex);
    Log::Debug("else index=", elseIndex);
    Log::Debug("endif index=", endifIndex);

    unsigned int lhsBytesParsed = 0;
    unsigned int lhsLength = 0;
    lhs = parseVariable(conditionIndex, context, lhsBytesParsed);
    if (lhsBytesParsed > 0) {
        Log::Debug("parsed lhs variable: ", lhs);
    } else {
        // parse a constant of any type
        lhs = parseConstant(conditionIndex, lhsBytesParsed);
        Log::Debug("parsed lhs constant: ", lhs);
    }
    conditionIndex += lhsBytesParsed;
    while (isWhitespace(conditionIndex))
        conditionIndex++;
    if (conditionIndex < thenIndex) {
        unsigned operBytesParsed = 0;
        oper = parseOperator(conditionIndex, operBytesParsed);
        if (oper == TemplateOperator::Undefined) {
            // parse error
        }
        Log::Debug("parsed operator: ", oper);
        conditionIndex += operBytesParsed;
        unsigned int rhsBytesParsed = 0;
        while (isWhitespace(conditionIndex))
            conditionIndex++;
        Variable_t rhs = parseVariable(conditionIndex, context, rhsBytesParsed);
        if (rhsBytesParsed > 0) {
            // check type matches lhs
            Log::Debug("parsed rhs variable: ", rhs);
        } else {
            // parse a constant of the same type as lhs
            rhs = parseConstant(conditionIndex, rhsBytesParsed);
            Log::Debug("parsed rhs constant: ", rhs);
        }

        bool compareResult = false;
        if (std::holds_alternative<std::string>(lhs)) {
            Log::Debug("string comparison");
            compareResult = compare(oper, std::get<std::string>(rhs), std::get<std::string>(lhs));
        } else {
            Log::Debug("unknown comparison");
            // handle other types
        }
        if (compareResult) {
            outLength = endifIndex - thenIndex - strlen(THEN_KEYWORD) - 2;
            uint8_t *result = new uint8_t[outLength];
            memcpy(result, d_bytes + thenIndex + strlen(THEN_KEYWORD) + 1, outLength);
            return result;
        } else if (std::string::npos == elseIndex) {
            // false and no else
            outLength = 0;
            uint8_t *result = new uint8_t[outLength];
            return result;
        } else {
            // false and else
            outLength = endifIndex - elseIndex - strlen(ELSE_KEYWORD) - 2;
            uint8_t *result = new uint8_t[outLength];
            memcpy(result, d_bytes + elseIndex + strlen(ELSE_KEYWORD) + 1, outLength);
            return result;
        }
    } else {
        // variable only, no operator or rhs
        if (!std::holds_alternative<bool>(lhs)) {
            // parse error
        }
        if (std::get<bool>(lhs)) {
            outLength = endifIndex - thenIndex - strlen(THEN_KEYWORD) - 2;
            uint8_t *result = new uint8_t[outLength];
            memcpy(result, d_bytes + thenIndex + strlen(THEN_KEYWORD) + 1, outLength);
            return result;
        } else if (std::string::npos == elseIndex) {
            // false and no else
            outLength = 0;
            uint8_t *result = new uint8_t[outLength];
            return result;
        } else {
            // false and else
            outLength = endifIndex - elseIndex - strlen(ELSE_KEYWORD) - 2;
            uint8_t *result = new uint8_t[outLength];
            memcpy(result, d_bytes + elseIndex + strlen(ELSE_KEYWORD) + 1, outLength);
            return result;
        }
    }

    uint8_t *result = new uint8_t[2];
    outLength = 2;
    memcpy(result, "hi", outLength);
    return result;
}

uint8_t *Template::parseKeyword(unsigned int index, const Context_t &context, unsigned int &bytesParsed,
                                unsigned int &outLength)
{
    return parseConditional(index, context, bytesParsed, outLength);
}

bool Template::isWhitespace(unsigned int index)
{
    return d_bytes[index] == 0x20; // space
}

uint8_t *Template::encode(const Context_t &context, unsigned int &encodingLength)
{
    unsigned int outLen = d_length * 2;
    uint8_t *out = new uint8_t[outLen];
    memset(out, 0, outLen);
    unsigned int outOffset = 0;

    unsigned int i = 0;
    while (i < d_length) {
        while (i < d_length && isWhitespace(i)) {
            out[outOffset++] = d_bytes[i++];
        }

        unsigned int variableLength = 0;
        unsigned int bytesParsed = 0;

        uint8_t *keywordExpanded = parseKeyword(i, context, bytesParsed, variableLength);
        if (keywordExpanded != nullptr) {
            memcpy(out + outOffset, keywordExpanded, variableLength);
            i += bytesParsed;
            outOffset += variableLength;
            delete[] keywordExpanded;
            continue;
        }

        uint8_t *variable = parseVariable(i, context, bytesParsed, variableLength);
        if (variable != nullptr) {
            memcpy(out + outOffset, variable, variableLength);
            i += bytesParsed;
            outOffset += variableLength;
            delete[] variable;
            continue;
        }

        while (i < d_length && !isWhitespace(i)) {
            out[outOffset++] = d_bytes[i++];
        }
    }

    encodingLength = outOffset;
    return out;
}

} // namespace fwoop

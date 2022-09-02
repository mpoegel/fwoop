#include <fwoop_argparser.h>

#include <fwoop_log.h>

namespace fwoop {

const ArgParserErrCategory ArgParserError{};

ArgParser::ArgParser(const char** argv, int argc)
: d_argv(argv)
, d_argc(argc)
{
}

std::error_code ArgParser::parse()
{
    bool inNamedArg = false;
    unsigned int nameArgIndex = 0;
    unsigned int posArgIndex = 0;
    for (unsigned int i = 0; i < d_argc; i++) {
        std::string arg(d_argv[i]);
        auto eqIndex = arg.find('=');
        bool hasDash = (arg[0] == '-');
        if (inNamedArg) {
            // named arg in two (part 2)
            if (std::holds_alternative<int>(d_namedArgs[nameArgIndex].value)) {
                if (arg == "0") {
                    d_namedArgs[nameArgIndex].value = 0;
                } else {
                    int val = std::atoi(arg.c_str());
                    if (val == 0) {
                        // bad arg: not an int
                        return std::error_code(static_cast<int>(ParseErr::InvalidType), ArgParserError);
                    }
                    d_namedArgs[nameArgIndex].value = val;
                }
            } else {
                // string value
                d_namedArgs[nameArgIndex].value = arg;
            }
            inNamedArg = false;
        } else if (hasDash && eqIndex != std::string::npos) {
            // named arg in one
            Log::Debug("has dash and = at ", eqIndex);
            // TODO
        } else if (hasDash) {
            std::string argName = arg.substr(1, arg.length() - 1);
            auto itr = d_argFlagIndex.find(argName);
            if (itr != d_argFlagIndex.end()) {
                if (std::holds_alternative<bool>(d_namedArgs[itr->second].value)) {
                    d_namedArgs[itr->second].value = true;
                } else {
                    // named arg in two (part 1)
                    // int or string type
                    inNamedArg = true;
                    nameArgIndex = itr->second;
                }
            } else {
                // bad arg: no argument by this name
                return std::error_code(static_cast<int>(ParseErr::ArgNotFound), ArgParserError);
            }
        } else {
            if (d_positionalArgIndex.size() <= posArgIndex) {
                // bad arg: too many positional args
                return std::error_code(static_cast<int>(ParseErr::TooManyPositionalArgs), ArgParserError);
            }
            // positional arg
            if (std::holds_alternative<bool>(d_positionalArgs[posArgIndex].value)) {
                // bad arg: positional arg cannot be a bool
                return std::error_code(static_cast<int>(ParseErr::PositionalBoolNotAllowed), ArgParserError);
            } else if (std::holds_alternative<int>(d_positionalArgs[posArgIndex].value)) {
                if (arg == "0") {
                    d_positionalArgs[posArgIndex].value = 0;
                } else {
                    int val = std::atoi(arg.c_str());
                    if (val == 0) {
                        // bad arg: not an int
                        return std::error_code(static_cast<int>(ParseErr::InvalidType), ArgParserError);
                    }
                    d_positionalArgs[posArgIndex].value = val;
                }
            } else {
                // string value
                d_positionalArgs[posArgIndex].value = arg;
            }
            posArgIndex++;
        }
    }
    return std::error_code();
}

const char* ArgParserErrCategory::name() const noexcept
{
    return "ArgParser";
}

std::string ArgParserErrCategory::message(int ev) const
{
    switch (static_cast<ArgParser::ParseErr>(ev)) {
        case ArgParser::ParseErr::InvalidType:
            return "invalid type";
        case ArgParser::ParseErr::ArgNotFound:
            return "argument not found";
        case ArgParser::ParseErr::TooManyPositionalArgs:
            return "too many positional arguments";
        case ArgParser::ParseErr::PositionalBoolNotAllowed:
            return "positional arguments cannot be boolean";
        default:
            return "unknown parse error";
    }
}

}

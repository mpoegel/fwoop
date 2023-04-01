#pragma once

#include <ostream>
#include <string>
#include <system_error>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fwoop {

/**
 * @brief Argument parser for command line arguments.
 *
 */
class ArgParser {
  private:
    typedef std::variant<std::string, int, bool> Argument_t;
    struct ArgumentData {
        Argument_t value;
        std::string help;
        std::string flag;
    };

    const char **d_argv;
    int d_argc;

    std::vector<ArgumentData> d_positionalArgs;
    std::vector<ArgumentData> d_namedArgs;
    std::unordered_map<std::string, unsigned int> d_positionalArgIndex;
    std::unordered_map<std::string, unsigned int> d_argNameIndex;
    std::unordered_map<std::string, unsigned int> d_argFlagIndex;

    friend std::ostream &operator<<(std::ostream &os, const ArgumentData);

  public:
    enum ParseErr {
        InvalidType = 1,
        ArgNotFound,
        TooManyPositionalArgs,
        PositionalBoolNotAllowed,
    };

    /**
     * @brief Construct a new Arg Parser object
     *
     * @param argv command line arguments
     * @param argc number of command line arguments
     */
    ArgParser(const char **argv, int argc);

    /**
     * @brief Attempt to parse the command line arguments that were given on construction.
     *
     * @return std::error_code
     */
    std::error_code parse();

    /**
     * @brief Add a named argument to parse.
     *
     * @tparam T argument type
     * @param name argument name, only alphanumeric characters and '-' or '_' are allowed
     * @param flag flag, only alphanumeric characters and '-' or '_' are allowed
     * @param defaultVal default value for the argument when not parsed
     * @param helpMsg help message to print for this argument
     */
    template <typename T>
    void addNamedArg(const std::string &name, const std::string &flag, const T &defaultVal, const std::string &helpMsg);

    template <typename T>
    void addPositionalArg(const std::string &name, const T &defaultVal, const std::string &helpMsg);

    template <typename T> T getNamedArg(const std::string &argName) const;

    template <typename T> T getPositionalArg(const std::string &argName) const;

    friend std::ostream &operator<<(std::ostream &os, const ArgParser &parser);
};

struct ArgParserErrCategory : std::error_category {
    const char *name() const noexcept override;
    std::string message(int ev) const override;
};

template <typename T>
void ArgParser::addNamedArg(const std::string &name, const std::string &flag, const T &defaultVal,
                            const std::string &helpMsg)
{
    unsigned int index = d_namedArgs.size();
    d_argNameIndex[name] = index;
    d_argFlagIndex[flag] = index;
    d_namedArgs.push_back(ArgumentData{Argument_t(defaultVal), helpMsg, flag});
}

template <typename T>
void ArgParser::addPositionalArg(const std::string &name, const T &defaultVal, const std::string &helpMsg)
{
    d_positionalArgIndex[name] = d_positionalArgs.size();
    d_positionalArgs.push_back(ArgumentData{Argument_t(defaultVal), helpMsg, ""});
}

template <typename T> T ArgParser::getNamedArg(const std::string &argName) const
{
    auto itr = d_argNameIndex.find(argName);
    if (itr == d_argNameIndex.end()) {
        return T();
    }
    return std::get<T>(d_namedArgs[itr->second].value);
}

template <typename T> T ArgParser::getPositionalArg(const std::string &argName) const
{
    auto itr = d_positionalArgIndex.find(argName);
    if (itr == d_positionalArgIndex.end()) {
        return T();
    }
    return std::get<T>(d_positionalArgs[itr->second].value);
}

} // namespace fwoop

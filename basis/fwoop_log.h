#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace fwoop {

class Log {
  public:
    enum Level {
        e_Debug = 1,
        e_Info  = 2,
        e_Warn  = 3,
        e_Error = 4,
    };

  private:
    static std::shared_ptr<Log> s_log;

    std::string d_formatStr;

    std::string levelToString(Level level) const;

    template<typename... Ts>
    std::string formatMsg(Level level, const char *fmt, Ts... args) const;

    void getCurrentTime(char *outBuf, unsigned int bufSize) const;

  public:
    static std::shared_ptr<Log> getDefault();

    template<typename... Ts>
    static void Debug(const char *fmt, Ts... args);

    template<typename... Ts>
    static void Info(const char *fmt, Ts... args);

    template<typename... Ts>
    static void Warn(const char *fmt, Ts... args);

    template<typename... Ts>
    static void Error(const char *fmt, Ts... args);

    static void Debug(const std::string &msg);
    static void Info(const std::string &msg);
    static void Warn(const std::string &msg);
    static void Error(const std::string &msg);

    Log();

    void setFormat(const std::string& formatStr);

    template<typename... Ts>
    void debug(const char *fmt, Ts... args);

    template<typename... Ts>
    void info(const char *fmt, Ts... args);

    template<typename... Ts>
    void warn(const char *fmt, Ts... args);

    template<typename... Ts>
    void error(const char *fmt, Ts... args);

    void debug(const std::string &msg);
    void info(const std::string &msg);
    void warn(const std::string &msg);
    void error(const std::string &msg);
};

inline
std::shared_ptr<Log> Log::getDefault()
{
    return s_log;
}

template<typename... Ts>
inline
void Log::Debug(const char *fmt, Ts... args)
{
    s_log->debug(fmt, args...);
}

template<typename... Ts>
inline
void Log::Info(const char *fmt, Ts... args)
{
    s_log->info(fmt, args...);
}

template<typename... Ts>
inline
void Log::Warn(const char *fmt, Ts... args)
{
    s_log->warn(fmt, args...);
}

template<typename... Ts>
inline
void Log::Error(const char *fmt, Ts... args)
{
    s_log->error(fmt, args...);
}

inline
void Log::Debug(const std::string &msg)
{
    s_log->debug(msg);
}

inline
void Log::Info(const std::string &msg)
{
    s_log->info(msg);
}

inline
void Log::Warn(const std::string &msg)
{
    s_log->warn(msg);
}

inline
void Log::Error(const std::string &msg)
{
    s_log->error(msg);
}

template<typename... Ts>
void Log::debug(const char *fmt, Ts... args)
{
    std::cout << formatMsg(Level::e_Debug, fmt, args...) << '\n';
}

template<typename... Ts>
void Log::info(const char *fmt, Ts... args)
{
    std::cout << formatMsg(Level::e_Info, fmt, args...) << '\n';
}

template<typename... Ts>
void Log::warn(const char *fmt, Ts... args)
{
    std::cerr << formatMsg(Level::e_Warn, fmt, args...) << '\n';
}

template<typename... Ts>
void Log::error(const char *fmt, Ts... args)
{
    std::cerr << formatMsg(Level::e_Error, fmt, args...) << '\n';
}

template<typename... Ts>
std::string Log::formatMsg(Level level, const char *fmt, Ts... args) const
{
    std::string result;
    char msgBuf[2048];
    int msgLen = sprintf(msgBuf, fmt, args...);

    unsigned int i = 0;
    while (i < d_formatStr.length()) {
        if (d_formatStr[i] == '%' && i < d_formatStr.length() - 1) {
            char c = d_formatStr[i + 1];
            if (c == 't') {
                char timeBuf[13];
                getCurrentTime(timeBuf, sizeof(timeBuf));
                result.append(timeBuf, sizeof(timeBuf));
            } else if (c == 'l') {
                result.append(levelToString(level));
            } else if (c == 'm') {
                result.append(msgBuf, msgLen);
            }
            i += 2;
        } else {
            result.push_back(d_formatStr[i]);
            ++i;
        }
    }

    return result;
}

}

#pragma once

#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

namespace fwoop {

class Log {
  public:
    enum Level {
        e_Debug = 1,
        e_Info = 2,
        e_Warn = 3,
        e_Error = 4,
    };

  private:
    static std::shared_ptr<Log> s_log;

    std::string d_formatStr;

    std::string levelToString(Level level) const;
    template <typename... Ts> std::string formatMsg(Level level, Ts... args) const;
    void getCurrentTime(char *outBuf, unsigned int bufSize) const;

  public:
    static std::shared_ptr<Log> getDefault();

    template <typename... Ts> static void Debug(Ts &&...args);

    template <typename... Ts> static void Info(Ts &&...args);

    template <typename... Ts> static void Warn(Ts &&...args);

    template <typename... Ts> static void Error(Ts &&...args);

    static void Debug(const std::string &msg);
    static void Info(const std::string &msg);
    static void Warn(const std::string &msg);
    static void Error(const std::string &msg);

    Log();

    void setFormat(const std::string &formatStr);

    template <typename... Ts> void debug(Ts &&...args);

    template <typename... Ts> void info(Ts &&...args);

    template <typename... Ts> void warn(Ts &&...args);

    template <typename... Ts> void error(Ts &&...args);

    void debug(const std::string &msg);
    void info(const std::string &msg);
    void warn(const std::string &msg);
    void error(const std::string &msg);
};

inline std::shared_ptr<Log> Log::getDefault() { return s_log; }

template <typename... Ts> inline void Log::Debug(Ts &&...args) { s_log->debug(args...); }

template <typename... Ts> inline void Log::Info(Ts &&...args) { s_log->info(args...); }

template <typename... Ts> inline void Log::Warn(Ts &&...args) { s_log->warn(args...); }

template <typename... Ts> inline void Log::Error(Ts &&...args) { s_log->error(args...); }

inline void Log::Debug(const std::string &msg) { s_log->debug(msg); }

inline void Log::Info(const std::string &msg) { s_log->info(msg); }

inline void Log::Warn(const std::string &msg) { s_log->warn(msg); }

inline void Log::Error(const std::string &msg) { s_log->error(msg); }

template <typename... Ts> void Log::debug(Ts &&...args) { std::cout << formatMsg(Level::e_Debug, args...) << '\n'; }

template <typename... Ts> void Log::info(Ts &&...args) { std::cout << formatMsg(Level::e_Info, args...) << '\n'; }

template <typename... Ts> void Log::warn(Ts &&...args) { std::cerr << formatMsg(Level::e_Warn, args...) << '\n'; }

template <typename... Ts> void Log::error(Ts &&...args) { std::cerr << formatMsg(Level::e_Error, args...) << '\n'; }

template <typename... Ts> std::string Log::formatMsg(Level level, Ts... args) const
{
    std::ostringstream oss;

    unsigned int i = 0;
    while (i < d_formatStr.length()) {
        if (d_formatStr[i] == '%' && i < d_formatStr.length() - 1) {
            char c = d_formatStr[i + 1];
            if (c == 't') {
                char timeBuf[13];
                getCurrentTime(timeBuf, sizeof(timeBuf));
                oss << timeBuf;
            } else if (c == 'l') {
                oss << levelToString(level);
            } else if (c == 'm') {
                (oss << ... << args);
            }
            i += 2;
        } else {
            oss << d_formatStr[i];
            ++i;
        }
    }

    return oss.str();
}

} // namespace fwoop

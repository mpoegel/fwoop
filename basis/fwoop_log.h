#pragma once

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
    std::string formatMsg(Level level, const char *fmt, const char *args...) const;
    void getCurrentTime(char *outBuf, unsigned int bufSize) const;

  public:
    static std::shared_ptr<Log> getDefault();
    static void Debug(const char *fmt, const char *args...);
    static void Info(const char *fmt, const char *args...);
    static void Warn(const char *fmt, const char *args...);
    static void Error(const char *fmt, const char *args...);

    static void Debug(const std::string &msg);
    static void Info(const std::string &msg);
    static void Warn(const std::string &msg);
    static void Error(const std::string &msg);

    Log();

    void setFormat(const std::string& formatStr);

    void debug(const char *fmt, const char *args...);
    void info(const char *fmt, const char *args...);
    void warn(const char *fmt, const char *args...);
    void error(const char *fmt, const char *args...);

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

inline
void Log::Debug(const char *fmt, const char *args...)
{
    s_log->debug(fmt, args);
}

inline
void Log::Info(const char *fmt, const char *args...)
{
    s_log->info(fmt, args);
}

inline
void Log::Warn(const char *fmt, const char *args...)
{
    s_log->warn(fmt, args);
}

inline
void Log::Error(const char *fmt, const char *args...)
{
    s_log->error(fmt, args);
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

}

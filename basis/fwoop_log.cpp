#include <fwoop_log.h>

#include <chrono>
#include <ctime>
#include <iostream>

namespace fwoop {
namespace {

const std::string DEFAULT_FORMAT_STR("%t %l %m");
const char DEFAULT_TIME_FORMAT[] = "%H:%M:%S";

}

std::shared_ptr<Log> Log::s_log = std::make_shared<Log>();

std::string Log::levelToString(Level level) const
{
    switch (level)
    {
    case Level::e_Debug:
        return "DEBUG";
    case Level::e_Info:
        return "INFO";
    case Level::e_Warn:
        return "WARN";
    case Level::e_Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

void Log::getCurrentTime(char *outBuf, unsigned int bufSize) const
{
    const auto now = std::chrono::system_clock::now();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    const time_t rawTime = std::chrono::system_clock::to_time_t(now);
    struct tm *timeInfo = localtime(&rawTime);
    strftime(outBuf, bufSize, DEFAULT_TIME_FORMAT, timeInfo);
    sprintf(outBuf, "%s.%03d", outBuf, millis);
}

std::string Log::formatMsg(Level level, const char *fmt, const char *args...) const
{
    std::string result;
    char msgBuf[2048];
    int msgLen = sprintf(msgBuf, fmt, args);

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

Log::Log()
: d_formatStr(DEFAULT_FORMAT_STR)
{
}

void Log::setFormat(const std::string& formatStr)
{
    // TODO validate format string
    d_formatStr = formatStr;
}

void Log::debug(const char *fmt, const char *args...)
{
    std::cout << formatMsg(Level::e_Debug, fmt, args) << '\n';
}

void Log::info(const char *fmt, const char *args...)
{
    std::cout << formatMsg(Level::e_Info, fmt, args) << '\n';
}

void Log::warn(const char *fmt, const char *args...)
{
    std::cerr << formatMsg(Level::e_Warn, fmt, args) << '\n';
}

void Log::error(const char *fmt, const char *args...)
{
    std::cerr << formatMsg(Level::e_Error, fmt, args) << '\n';
}

void Log::debug(const std::string& msg)
{
    std::cout << formatMsg(Level::e_Debug, msg.c_str(), nullptr) << '\n';
}

void Log::info(const std::string& msg)
{
    std::cout << formatMsg(Level::e_Info, msg.c_str(), nullptr) << '\n';
}

void Log::warn(const std::string& msg)
{
    std::cerr << formatMsg(Level::e_Warn, msg.c_str(), nullptr) << '\n';
}

void Log::error(const std::string& msg)
{
    std::cerr << formatMsg(Level::e_Error, msg.c_str(), nullptr) << '\n';
}

}

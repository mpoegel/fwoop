#include <fwoop_log.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>

namespace fwoop {
namespace {

const std::string DEFAULT_FORMAT_STR("%t %l %m");
const char DEFAULT_TIME_FORMAT[] = "%H:%M:%S";

} // namespace

std::shared_ptr<Log> Log::s_log = std::make_shared<Log>();

std::string Log::levelToString(Level level) const
{
    switch (level) {
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
    sprintf(outBuf, "%s.%03ld", outBuf, millis);
}

Log::Log() : d_formatStr(DEFAULT_FORMAT_STR), d_threshold(Level::e_Debug) {}

void Log::setFormat(const std::string &formatStr)
{
    // TODO validate format string
    d_formatStr = formatStr;
}

void Log::debug(const std::string &msg) { std::cout << formatMsg(Level::e_Debug, msg) << '\n'; }

void Log::info(const std::string &msg) { std::cout << formatMsg(Level::e_Info, msg) << '\n'; }

void Log::warn(const std::string &msg) { std::cerr << formatMsg(Level::e_Warn, msg) << '\n'; }

void Log::error(const std::string &msg) { std::cerr << formatMsg(Level::e_Error, msg) << '\n'; }

} // namespace fwoop

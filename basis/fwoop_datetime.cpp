#include <fwoop_datetime.h>

#include "fwoop_log.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <sys/types.h>
#include <vector>

namespace fwoop {

namespace {
constexpr uint32_t SecondsInMinute = 60;
constexpr uint32_t SecondsInHour = SecondsInMinute * 60;
constexpr uint32_t SecondsInDay = SecondsInHour * 24;
constexpr uint32_t SecondsInYear = SecondsInDay * 365;

const std::vector<uint32_t> SecondsPerMonth{SecondsInDay * 31, SecondsInDay * 28, SecondsInDay * 31, SecondsInDay * 30,
                                            SecondsInDay * 31, SecondsInDay * 30, SecondsInDay * 31, SecondsInDay * 31,
                                            SecondsInDay * 30, SecondsInDay * 31, SecondsInDay * 30, SecondsInDay * 31};

bool isLeapYear(uint32_t year) { return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); }

} // namespace

DateTime::DateTime(uint32_t unix_time) : d_time(unix_time) { recalculate(); }

void DateTime::recalculate()
{
    int64_t remaining = d_time;
    for (d_year = 1970; remaining > SecondsInYear; d_year++) {
        remaining -= SecondsInYear;
        remaining -= isLeapYear(d_year) ? SecondsInDay : 0;
    }

    for (d_month = 0; d_month < SecondsPerMonth.size() && remaining > 0; d_month++) {
        if (remaining - SecondsPerMonth[d_month] < 0) {
            break;
        }
        remaining -= SecondsPerMonth[d_month];
        remaining -= (d_month == 1 && isLeapYear(d_year) ? SecondsInDay : 0);
    };

    d_day = std::floor(remaining / SecondsInDay) + 1;
    remaining -= (d_day - 1) * SecondsInDay;

    d_hour = std::floor(remaining / SecondsInHour);
    remaining -= d_hour * SecondsInHour;

    d_minute = std::floor(remaining / SecondsInMinute);
    d_second = remaining - (d_minute * SecondsInMinute);
}

} // namespace fwoop

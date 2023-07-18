#include <fwoop_datetime.h>

#include "fwoop_log.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <sys/types.h>
#include <vector>

namespace fwoop {

namespace {
constexpr uint32_t DaysInWeek = 7;
constexpr uint32_t DaysInYear = 365;
constexpr uint32_t SecondsInMinute = 60;
constexpr uint32_t SecondsInHour = SecondsInMinute * 60;
constexpr uint32_t SecondsInDay = SecondsInHour * 24;
constexpr uint32_t SecondsInYear = SecondsInDay * DaysInYear;

const std::vector<uint8_t> DaysPerMonth{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const std::vector<uint32_t> SecondsPerMonth{
    SecondsInDay * DaysPerMonth[0], SecondsInDay *DaysPerMonth[1],  SecondsInDay *DaysPerMonth[2],
    SecondsInDay *DaysPerMonth[3],  SecondsInDay *DaysPerMonth[4],  SecondsInDay *DaysPerMonth[5],
    SecondsInDay *DaysPerMonth[6],  SecondsInDay *DaysPerMonth[7],  SecondsInDay *DaysPerMonth[8],
    SecondsInDay *DaysPerMonth[9],  SecondsInDay *DaysPerMonth[10], SecondsInDay *DaysPerMonth[11]};

bool isLeapYear(uint32_t year) { return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); }

} // namespace

DateTime DateTime::now() { return DateTime(std::time(nullptr)); }

DateTime::DateTime(uint32_t unix_time) : d_time(unix_time) { recalculate(); }

void DateTime::recalculate()
{
    int64_t remaining = d_time;
    int64_t daysSinceEpoch = 0;
    for (d_year = 1970; remaining > SecondsInYear; d_year++) {
        daysSinceEpoch += DaysInYear;
        remaining -= SecondsInYear;
        if (isLeapYear(d_year)) {
            daysSinceEpoch++;
            remaining -= SecondsInDay;
        }
    }

    for (d_month = 0; d_month < SecondsPerMonth.size() && remaining > 0; d_month++) {
        if (remaining - SecondsPerMonth[d_month] < 0) {
            break;
        }
        remaining -= SecondsPerMonth[d_month];
        daysSinceEpoch += DaysPerMonth[d_month];
        if (isLeapYear(d_year) && d_month == 1) {
            remaining -= SecondsInDay;
            daysSinceEpoch += 1;
        }
    };

    d_day = std::floor(remaining / SecondsInDay) + 1;
    remaining -= (d_day - 1) * SecondsInDay;

    daysSinceEpoch += d_day - 1;
    d_dow = (DayOfWeek)((daysSinceEpoch + Thursday) % DaysInWeek);

    d_hour = std::floor(remaining / SecondsInHour);
    remaining -= d_hour * SecondsInHour;

    d_minute = std::floor(remaining / SecondsInMinute);
    d_second = remaining - (d_minute * SecondsInMinute);
}

void DateTime::addDays(int16_t daysToAdd)
{
    d_time += (daysToAdd * SecondsInDay);
    recalculate();
}

void DateTime::addHours(int16_t hoursToAdd)
{
    d_time += (hoursToAdd * SecondsInHour);
    recalculate();
}

void DateTime::addMinutes(int16_t minutesToAdd)
{
    d_time += (minutesToAdd * SecondsInMinute);
    recalculate();
}

void DateTime::addSeconds(int64_t secondsToAdd) { d_time += secondsToAdd; }

} // namespace fwoop

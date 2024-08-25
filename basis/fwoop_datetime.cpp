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
    remaining += (int)(d_tzOffset * SecondsInHour);

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

    d_firstOfMonth = (DayOfWeek)((daysSinceEpoch + Thursday) % DaysInWeek);

    d_day = std::floor(remaining / SecondsInDay) + 1;
    remaining -= (d_day - 1) * SecondsInDay;

    daysSinceEpoch += d_day - 1;
    d_dow = (DayOfWeek)((daysSinceEpoch + Thursday) % DaysInWeek);

    d_weekOfMonth = std::floor((d_day + d_firstOfMonth + 1) / DaysInWeek);

    d_hour = std::floor(remaining / SecondsInHour);
    remaining -= d_hour * SecondsInHour;

    d_minute = std::floor(remaining / SecondsInMinute);
    d_second = remaining - (d_minute * SecondsInMinute);

    // assuming that DST never occurs near midnight
    updateDST();
    if (d_isDST) {
        d_hour += 1;
    }
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

void DateTime::updateDST()
{
    switch (d_tz) {
    default:
        break;
    case AMERICA_NEW_YORK:
        uint8_t shifted_dow = (d_dow - d_firstOfMonth + DaysInWeek) % DaysInWeek;
        uint8_t shifted_sunday = (Sunday - d_firstOfMonth + DaysInWeek) % DaysInWeek;
        // uint8_t hour = (d_isDST ? d_hour - 1 : d_hour);
        // begins at 2:00 a.m. (EST) on the second Sunday of March
        // ends at 2:00 a.m. (EDT) on the first Sunday of November (ends at 1:00 am EST)
        if (d_month > 2 && d_month < 10) {
            d_isDST = true;
        } else if (d_month == 2 && d_weekOfMonth > 2) {
            d_isDST = true;
        } else if (d_month == 2 && d_weekOfMonth == 2 && shifted_dow > shifted_sunday) {
            d_isDST = true;
        } else if (d_month == 2 && d_weekOfMonth == 2 && shifted_dow == shifted_sunday && d_hour > 2) {
            d_isDST = true;
        } else if (d_month == 2 && d_weekOfMonth == 2 && shifted_dow == shifted_sunday && d_hour == 2) {
            d_isDST = true;
        } else if (d_month == 10 && d_weekOfMonth == 1 && shifted_dow < shifted_sunday) {
            d_isDST = true;
        } else if (d_month == 10 && d_weekOfMonth == 1 && shifted_dow == shifted_sunday && d_hour < 1) {
            d_isDST = true;
        } else {
            d_isDST = false;
        }
    }
}

void DateTime::setTimezone(Timezone tz)
{
    d_tz = tz;
    switch (d_tz) {
    case Timezone::UTC:
        d_tzOffset = 0;
        break;
    case Timezone::AMERICA_NEW_YORK:
        d_tzOffset = -5;
        break;
    }
    recalculate();
}

std::string DateTime::dayOfWeekShortString() const
{
    switch (dayOfWeek()) {
    case fwoop::DateTime::Sunday:
        return "Sun";
    case fwoop::DateTime::Monday:
        return "Mon";
    case fwoop::DateTime::Tuesday:
        return "Tue";
    case fwoop::DateTime::Wednesday:
        return "Wed";
    case fwoop::DateTime::Thursday:
        return "Thu";
    case fwoop::DateTime::Friday:
        return "Fri";
    case fwoop::DateTime::Saturday:
        return "Sat";
    default:
        return "N/A";
    }
}

} // namespace fwoop

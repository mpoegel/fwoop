#pragma once

#include <cstdint>
#include <string>

namespace fwoop {

class DateTime {
  public:
    enum DayOfWeek : uint8_t {
        Sunday = 0,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
    };

    enum Timezone : uint8_t {
        UTC = 0,
        AMERICA_NEW_YORK = 1,
    };

  private:
    uint32_t d_time;
    Timezone d_tz;

    uint16_t d_year;
    uint8_t d_month;
    uint8_t d_day;
    uint8_t d_hour;
    bool d_isDST;
    uint8_t d_minute;
    uint8_t d_second;
    DayOfWeek d_dow;
    DayOfWeek d_firstOfMonth;
    uint8_t d_weekOfMonth;
    int8_t d_tzOffset;

    void recalculate();
    void updateDST();

  public:
    static DateTime now();

    DateTime(uint32_t unix_time);
    ~DateTime() = default;
    DateTime(const DateTime &rhs) = default;
    DateTime &operator=(const DateTime &rhs) = default;
    DateTime(DateTime &&rhs) = default;

    void addDays(int16_t daysToAdd);
    void addHours(int16_t hoursToAdd);
    void addMinutes(int16_t minutesToAdd);
    void addSeconds(int64_t secondsToAdd);
    void setTimezone(Timezone tz);

    const uint16_t year() const;
    const uint8_t month() const;
    const uint8_t day() const;
    const uint8_t hour() const;
    const uint8_t minute() const;
    const uint8_t second() const;
    const DayOfWeek dayOfWeek() const;
    std::string dayOfWeekShortString() const;
    const int8_t tzOffset() const;
    bool isDST() const;
};

inline const uint16_t DateTime::year() const { return d_year; }
inline const uint8_t DateTime::month() const { return d_month; }
inline const uint8_t DateTime::day() const { return d_day; }
inline const uint8_t DateTime::hour() const { return d_hour; }
inline const uint8_t DateTime::minute() const { return d_minute; }
inline const uint8_t DateTime::second() const { return d_second; }
inline const DateTime::DayOfWeek DateTime::dayOfWeek() const { return d_dow; }
inline const int8_t DateTime::tzOffset() const { return d_tzOffset; }
inline bool DateTime::isDST() const { return d_isDST; }

} // namespace fwoop

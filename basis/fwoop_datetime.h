#pragma once

#include <cstdint>

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

  private:
    uint32_t d_time;

    uint16_t d_year;
    uint8_t d_month;
    uint8_t d_day;
    uint8_t d_hour;
    uint8_t d_minute;
    uint8_t d_second;
    DayOfWeek d_dow;

    void recalculate();

  public:
    static DateTime now();

    DateTime(uint32_t unix_time);
    ~DateTime() = default;
    DateTime(const DateTime &rhs) = default;
    DateTime &operator=(const DateTime &rhs) = default;
    DateTime(DateTime &&rhs) = default;

    const uint16_t year();
    const uint8_t month();
    const uint8_t day();
    const uint8_t hour();
    const uint8_t minute();
    const uint8_t second();
    const DayOfWeek dayOfWeek();
};

inline const uint16_t DateTime::year() { return d_year; }
inline const uint8_t DateTime::month() { return d_month; }
inline const uint8_t DateTime::day() { return d_day; }
inline const uint8_t DateTime::hour() { return d_hour; }
inline const uint8_t DateTime::minute() { return d_minute; }
inline const uint8_t DateTime::second() { return d_second; }
inline const DateTime::DayOfWeek DateTime::dayOfWeek() { return d_dow; }

} // namespace fwoop

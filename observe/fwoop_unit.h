#pragma once

#include <string>

namespace fwoop {

class Unit {
    public:
    ~Unit() = default;

    virtual std::string toString() const = 0;

    protected:
    Unit() = default;
};

class TimeUnit : public Unit {
    public:
    enum Time {
        Nanosecond,
        Microsecond,
        Millisecond,
        Second,
        Minute,
        Hour,
        Day,
        Week,
        Month,
        Year,
    };

    private:
    Time d_time;

    public:
    TimeUnit(Time t);

    std::string toString() const;
};

} // namespace fwoop

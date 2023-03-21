#include <fwoop_unit.h>

namespace fwoop {

TimeUnit::TimeUnit(Time t) : d_time(t) {};

std::string TimeUnit::toString() const
{
    switch (d_time) {
    case Nanosecond: return "nanosecond";
    case Microsecond: return "microsecond";
    case Millisecond: return "millisecond";
    case Second: return "second";
    case Minute: return "minute";
    case Hour: return "hour";
    case Day: return "day";
    case Week: return "week";
    case Month: return "month";
    case Year: return "year";
    default: return "";
    }
}

} // namespace fwoop

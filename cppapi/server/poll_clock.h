#ifndef _POLL_CLOCK_H
#define _POLL_CLOCK_H

#include <chrono>
#include <ratio>

#include <idl/tango.h>

namespace Tango
{

using PollClock = std::chrono::steady_clock;

static_assert(
    std::ratio_less_equal<PollClock::duration::period, std::micro>::value,
    "Polling thread requires microsecond precision");

namespace __detail
{

using WallClock = std::chrono::system_clock;

// The conversion is based on the duration equality assumption:
// (poll_ref - poll) == (wall_ref - wall)

inline WallClock::time_point poll_time_to_wall_time(PollClock::time_point poll)
{
    auto poll_ref = PollClock::now();
    auto wall_ref = WallClock::now();
    auto wall = wall_ref - (poll_ref - poll);
    // We must cast, on Windows system_clock has only 1ms precision.
    return std::chrono::time_point_cast<WallClock::duration>(wall);
}

inline PollClock::time_point wall_time_to_poll_time(WallClock::time_point wall)
{
    auto poll_ref = PollClock::now();
    auto wall_ref = WallClock::now();
    return poll_ref - (wall_ref - wall);
}

} // namespace __detail

inline TimeVal make_TimeVal(PollClock::time_point poll_tp)
{
    auto wall_tp = __detail::poll_time_to_wall_time(poll_tp);
    auto time_ns = std::chrono::nanoseconds(wall_tp.time_since_epoch()).count();
    TimeVal tv{};
    tv.tv_sec = time_ns / 1000000000;
    tv.tv_usec = (time_ns % 1000000000) / 1000.0;
    tv.tv_nsec = 0; // Intentionally set to 0
    return tv;
}

inline PollClock::time_point make_poll_time(::timeval tv)
{
    auto wall_time = __detail::WallClock::time_point{
          std::chrono::seconds(tv.tv_sec)
        + std::chrono::microseconds(tv.tv_usec)};
    return __detail::wall_time_to_poll_time(wall_time);
}

inline PollClock::time_point make_poll_time(TimeVal tv)
{
    // We must cast, on Windows we do not have required ns precision.
    auto wall_duration = std::chrono::duration_cast<__detail::WallClock::duration>(
          std::chrono::seconds(tv.tv_sec)
        + std::chrono::microseconds(tv.tv_usec)
        + std::chrono::nanoseconds(tv.tv_nsec));
    auto wall_time = __detail::WallClock::time_point{wall_duration};
    return __detail::wall_time_to_poll_time(wall_time);
}

} // namespace Tango

#endif

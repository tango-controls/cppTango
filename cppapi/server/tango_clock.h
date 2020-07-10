#ifndef _TANGO_CLOCK_H
#define _TANGO_CLOCK_H

#include <chrono>
#include <ratio>
#include <type_traits>

#include <idl/tango.h>

namespace Tango
{

using PollClock = std::chrono::steady_clock;

static_assert(
    std::ratio_less_equal<PollClock::duration::period, std::micro>::value,
    "Polling thread requires microsecond precision");

namespace detail
{

template <typename A, typename B>
using SameClocks = std::is_same<typename A::clock, typename B::clock>;

template <typename DstTp, typename SrcTp>
typename std::enable_if<SameClocks<DstTp, SrcTp>::value, DstTp>::type
convert_time_point(SrcTp src)
{
    return std::chrono::time_point_cast<typename DstTp::duration>(src);
}

template <typename DstTp, typename SrcTp>
typename std::enable_if<!SameClocks<DstTp, SrcTp>::value, DstTp>::type
convert_time_point(SrcTp src)
{
    // The conversion is based on the duration equality assumption:
    // (src_ref - src) == (dst_ref - dst)

    auto src_ref = SrcTp::clock::now();
    auto dst_ref = DstTp::clock::now();
    auto dst = dst_ref - (src_ref - src);
    return std::chrono::time_point_cast<typename DstTp::duration>(dst);
}

template <typename TS, typename TU, typename TN>
std::chrono::system_clock::time_point make_system_time(TS sec, TU usec, TN nsec)
{
    using namespace std::chrono;
    auto timestamp = seconds(sec) + microseconds(usec) + nanoseconds(nsec);
    auto timestamp_sys = duration_cast<system_clock::duration>(timestamp);
    return system_clock::time_point{timestamp_sys};
}

} // namespace detail

template <typename Clock, typename Dur>
TimeVal make_TimeVal(std::chrono::time_point<Clock, Dur> tp)
{
    constexpr std::chrono::nanoseconds::rep NANOS_IN_SEC = 1000*1000*1000;
    constexpr std::chrono::nanoseconds::rep NANOS_IN_USEC = 1000;

    auto wall_tp = detail::convert_time_point<std::chrono::system_clock::time_point>(tp);
    auto time_ns = std::chrono::nanoseconds(wall_tp.time_since_epoch()).count();
    TimeVal tv{};
    tv.tv_sec = time_ns / NANOS_IN_SEC;
    tv.tv_usec = (time_ns % NANOS_IN_SEC) / NANOS_IN_USEC;
    tv.tv_nsec = (time_ns % NANOS_IN_SEC) % NANOS_IN_USEC;
    return tv;
}

template <typename Clock, typename Dur>
::timeval make_timeval(std::chrono::time_point<Clock, Dur> tp)
{
    auto tv = make_TimeVal(tp);
    ::timeval result{};
    result.tv_sec = tv.tv_sec;
    result.tv_usec = tv.tv_usec;
    return result;
}

inline PollClock::time_point make_poll_time(::timeval tv)
{
    auto sys_time = detail::make_system_time(tv.tv_sec, tv.tv_usec, 0);
    return detail::convert_time_point<PollClock::time_point>(sys_time);
}

inline PollClock::time_point make_poll_time(TimeVal tv)
{
    auto sys_time = detail::make_system_time(tv.tv_sec, tv.tv_usec, tv.tv_nsec);
    return detail::convert_time_point<PollClock::time_point>(sys_time);
}

inline std::chrono::system_clock::time_point make_system_time(::timeval tv)
{
    return detail::make_system_time(tv.tv_sec, tv.tv_usec, 0);
}

} // namespace Tango

#endif

/*
 * TimeStamp.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4tango/TimeStamp.hh>

#include <cstring>

#ifdef LOG4TANGO_HAVE_GETTIMEOFDAY
# include <sys/time.h>
#else
# ifdef LOG4TANGO_HAVE_FTIME
#   include <sys/timeb.h>
# else
#   include <time.h>
# endif
#endif

namespace log4tango {

TimeStamp TimeStamp::_start_stamp;

TimeStamp::TimeStamp() 
{
#ifdef LOG4TANGO_HAVE_GETTIMEOFDAY
  struct timeval tv;
  ::gettimeofday(&tv, NULL);
  _seconds = tv.tv_sec;
  _micro_seconds = tv.tv_usec;
#else
# ifdef LOG4TANGO_HAVE_FTIME
    struct timeb tb;
    ::ftime(&tb);
    _seconds = tb.time;
    _micro_seconds = 1000 * tb.millitm;
# else
    _seconds = ::time(NULL);
    _micro_seconds = 0;
# endif
#endif
}

} // namespace log4tango



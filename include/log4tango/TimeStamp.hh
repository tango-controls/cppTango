/*
 * TimeStamp.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_TIMESTAMP_H
#define _LOG4TANGO_TIMESTAMP_H

#include <log4tango/Portability.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// Class : TimeStamp (timestamp abstraction)
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT TimeStamp 
{
public:
  /**
     Constructs a TimeStamp representing 'now'.
  **/
  TimeStamp();

  /**
     Constructs a TimeStamp representing the given offset since the
     epoch ( 00:00:00 1970/1/1 UTC).
  **/
  TimeStamp(unsigned int seconds, unsigned int microseconds = 0);

  /**
     Returns the 'seconds' part of the TimeStamp.
  **/
  inline int get_seconds (void) const {
      return _seconds;
  };

  /** 
     Returns the 'subseconds' part of the TimeStamp in milliseconds,
     get_milliseconds() == get_microseconds() / 1000. 
  **/
  inline int get_milliseconds (void) const {
      return _micro_seconds / 1000;
  };

  /**
     Returns the subsecond part of the TimeStamp in microseconds.
     The actual precision of this value depends on the platform and
     may be in the order of milliseconds rather than microseconds.
   **/
  inline int get_microseconds (void) const {
      return _micro_seconds;
  };

  /**
     Returns a TimeStamp representing the time at which the application
     started.
  **/
  static inline const TimeStamp& get_start_time (void) {
      return _start_stamp;
  };

protected:
  static TimeStamp _start_stamp;
  int _seconds;
  int _micro_seconds;
};

} // namespace log4tango

#endif // _LOG4TANGO_TIMESTAMP_H


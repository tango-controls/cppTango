//
// TimeStamp.hh
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010
//					Synchrotron SOLEIL
//                	L'Orme des Merisiers
//                	Saint-Aubin - BP 48 - France
//
// This file is part of log4tango.
//
// Log4ango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Log4tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with Log4Tango.  If not, see <http://www.gnu.org/licenses/>.

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
     Copy Constructor.
  **/
  inline TimeStamp (const TimeStamp& t) 
    : _seconds(t._seconds), _micro_seconds(t._micro_seconds)   
  {
    //--noop    
  };
  
  /**
     Constructs a TimeStamp representing the given offset since the
     epoch ( 00:00:00 1970/1/1 UTC).
  **/
  inline TimeStamp(unsigned int seconds, unsigned int microseconds = 0)
    : _seconds(seconds), _micro_seconds(microseconds)   
  {
    //--noop    
  };
  
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

  /**
     Operator=
  **/
  inline void operator= (const TimeStamp& t) {
    _seconds = t._seconds;
    _micro_seconds = t._micro_seconds;
  };
  
protected:
  static TimeStamp _start_stamp;
  int _seconds;
  int _micro_seconds;
};

} // namespace log4tango

#endif // _LOG4TANGO_TIMESTAMP_H


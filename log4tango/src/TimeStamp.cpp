//
// TimeStamp.cpp
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



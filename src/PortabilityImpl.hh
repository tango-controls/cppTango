//
// PortabilityImpl.hh
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

#ifndef _LOG4TANGO_PORTABILITYIMPL_H
#define _LOG4TANGO_PORTABILITYIMPL_H

#include <log4tango/Portability.hh>

#ifdef LOG4TANGO_CSTDLIB_NOT_IN_STD
#include <cstdlib>
namespace std {
  static inline char *getenv(const char *name) { return ::getenv(name); };
  static inline int atoi(const char *nptr) { return ::atoi(nptr); };
  static inline unsigned long int
      strtoul(const char *nptr, char **endptr, int base) { 
      return ::strtol(nptr, endptr, base); 
  };
}
#endif
    
#ifdef LOG4TANGO_CSTRING_NOT_IN_STD
#include <cstring>
namespace std {
  static inline void *memmove(void *dest, const void *src, size_t n) {
      return ::memmove(dest, src, n);
  };
}
#endif

#ifdef LOG4TANGO_CTIME_NOT_IN_STD
#include <ctime>
namespace std {
  static inline size_t strftime(char *strDest, size_t maxsize, const char *format, const struct tm *timeptr ) {
    return ::strftime(strDest,maxsize,format,timeptr);
  }
  static inline struct tm *localtime( const time_t *timer ) { return ::localtime(timer); }
  }
#endif

#ifdef LOG4TANGO_CMATH_NOT_IN_STD
#include <cmath>
namespace std {
  static inline int abs(int i) { return ::abs(i); }
}
#endif

#endif // _LOG4TANGO_PORTABILITYIMPL_H

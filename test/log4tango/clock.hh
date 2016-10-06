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

#ifndef __CLOCK_H
#define __CLOCK_H

#ifdef LOG4TANGO_HAVE_STDINT_H
#include <stdint.h>
#endif // LOG4TANGO_HAVE_STDINT_H

#ifdef __osf__
    typedef long usec_t;    /* number of microseconds since 1970/01/01 */
#   define INT64_CONSTANT(val)  (val##L)
#else
    typedef int64_t usec_t;
#   define INT64_CONSTANT(val)  (val##LL)
#endif

class Clock
{
public:
    static bool		UsingCPU;
    static usec_t 	time(void);

    			Clock(void);
    			~Clock(void);

    bool		active(void) const { return _active; }
    usec_t		elapsed(void) const;
    usec_t		start(void);
    usec_t		reset(void) { return start(); }
    usec_t		stop(void);

private:
    usec_t		_start;    
    usec_t		_elapsed;
    bool		_active;
};

#endif

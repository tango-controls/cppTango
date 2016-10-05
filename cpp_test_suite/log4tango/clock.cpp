static const char rcsid[] = "$Id$";

// 
// Copyright (C) :  2004,2005,2006,2007,2008,2009,2010
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

#include <cstdlib>
#include <sys/time.h>			// for struct timeval
#ifdef __osf__
#    include <machine/builtins.h>       // for __RPCC()
#elif __linux__ && __i386__
#    define rdtscl(low) \
     __asm__ __volatile__("rdtsc" : "=a" (low) : : "edx")
#endif
#include <iostream>

#include "clock.hh"

namespace 
{
    const usec_t UsecPerSec = INT64_CONSTANT(1000000);
}

bool Clock::UsingCPU  = std::getenv("CLOCK_USE_CPU") ? true : false;

// -----------------------------------------------------------------------------
usec_t Clock::time(void)
{
    if (UsingCPU) {
	static bool warn = true;
		
	if (warn) {
	    std::cout << "Using CPU clock." << std::endl;
	    warn = false;
	}

#ifdef __osf__
	return (usec_t) __RPCC();
#elif __linux__ && __i386__
	{
	    unsigned long tsc;
			
	    rdtscl(tsc);
	    return (usec_t) tsc;
	}
#else
	{
	    std::cerr << "CPU clock not implemented for this architecture" << std::endl;
	    UsingCPU = false;
	    return Clock::time();
	}
#endif	
    } else {
	struct timeval tv;
	
	gettimeofday(&tv, NULL);	
	return (usec_t) (tv.tv_sec * UsecPerSec + tv.tv_usec);
    }
}

// -----------------------------------------------------------------------------
Clock::Clock(void)
    : _start(0),
      _elapsed(0),
      _active(false) 
{
    start();
}

// -----------------------------------------------------------------------------
Clock::~Clock(void)
{
    ;
}

// -----------------------------------------------------------------------------
usec_t Clock::elapsed(void) const
{
    if (!active())
	return _elapsed;

    return time() - _start;
}


// -----------------------------------------------------------------------------
usec_t Clock::start(void)
{
    _active = true;

    return _start = time();
}

// -----------------------------------------------------------------------------
usec_t Clock::stop(void)
{
    _elapsed = elapsed();
    _active  = false;

    return _elapsed;
}


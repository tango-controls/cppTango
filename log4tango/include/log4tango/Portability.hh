//
// Portability.hh
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010,2011,2012
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

#ifndef _LOG4TANGO_PORTABILITY_H
#define _LOG4TANGO_PORTABILITY_H

#include <log4tango/config.h>
#include <log4tango/Export.hh>

#if defined(_MSC_VER)

/* define to get around problems with ERROR in windows.h */
#ifndef LOG4TANGO_FIX_ERROR_COLLISION
# define LOG4TANGO_FIX_ERROR_COLLISION 1
#endif

#ifdef LOG4TANGO_USE_THREADING

// deal with ERROR #define

// This #includes windows.h with NOGDI and WIN32_LEAN_AND_MEAN
// #defined. If this is not what the user wants, #include
// windows.h before this file.

#ifndef _WINDOWS_
	#ifndef NOGDI
		#define NOGDI // circumvent the ERROR #define in windows.h
		#define LOG4TANGO_UNDEFINE_NOGDI
	#endif

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
		#define LOG4TANGO_UNDEFINE_WIN32_LEAN_AND_MEAN
	#endif

	#include <windows.h>

	#ifdef LOG4TANGO_UNDEFINE_NOGDI
		#undef NOGDI
	#endif

	#ifdef LOG4TANGO_UNDEFINE_WIN32_LEAN_AND_MEAN
		#undef WIN32_LEAN_AND_MEAN
	#endif

#endif
// done dealing with ERROR #define

#endif // LOG4TANGO_USE_THREADING

/* define mode_t */
namespace log4tango
{
typedef unsigned short mode_t;
}

#define LOG4TANGO_UNUSED(var) var

#else // _MSC_VER
	#ifdef __GNUC__
		#define LOG4TANGO_UNUSED(var) var __attribute__ ((unused))
	#else
		#define LOG4TANGO_UNUSED(var) var
	#endif
#endif // _MSC_VER

#endif // _LOG4TANGO_PORTABILITY_H

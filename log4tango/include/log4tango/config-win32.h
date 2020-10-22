//
// config-win32.hh
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

#ifndef _INCLUDE_LOG4TANGO_CONFIG_WIN32_H
#define _INCLUDE_LOG4TANGO_CONFIG_WIN32_H 1

/* manually edited from include/log4tango/config.h */

/* Define if you have the syslog function.  */
/* #undef LOG4TANGO_HAVE_SYSLOG */

/* Define if you have the <io.h> header file.  */
#ifndef LOG4TANGO_HAVE_IO_H
# define LOG4TANGO_HAVE_IO_H 1
#endif

/* Define if you have the <unistd.h> header file.  */
/* #undef LOG4TANGO_HAVE_UNISTD_H */

/* Define if you have the idsa library (-lidsa).  */
/* #undef LOG4TANGO_HAVE_LIBIDSA */

/* Define if you have the `strcasecmp' function. */
/* #undef LOG4TANGO_HAVE_STRCASECMP */

/* Name of package */
#ifndef LOG4TANGO_PACKAGE
# define LOG4TANGO_PACKAGE  "log4tango"
#endif

/* Version number of package */
#ifndef LOG4TANGO_VERSION
# define LOG4TANGO_VERSION  "0.3.4"
#endif

/* define if the compiler implements namespaces */
#ifndef LOG4TANGO_HAVE_NAMESPACES
# define LOG4TANGO_HAVE_NAMESPACES 1
#endif

/* define if the compiler has stringstream */
#ifndef LOG4TANGO_HAVE_SSTREAM
# define LOG4TANGO_HAVE_SSTREAM 1
#endif

/* define to get around problems with ERROR in windows.h */
#ifndef LOG4TANGO_FIX_ERROR_COLLISION
# define LOG4TANGO_FIX_ERROR_COLLISION 1
#endif

/* define WIN32 for Borland */
#ifndef WIN32
# define WIN32
#endif

/* use threads */
#ifndef LOG4TANGO_HAVE_THREADING
# define LOG4TANGO_HAVE_THREADING
#endif

/* use ms threads */
#ifndef LOG4TANGO_USE_MSTHREADS
# define LOG4TANGO_USE_MSTHREADS
#endif

/* supply DLL main */
#ifndef LOG4TANGO_SUPPLY_DLLMAIN
# define LOG4TANGO_SUPPLY_DLLMAIN
#endif

/* define mode_t. Move to Portability.hh if more platforms need it */
namespace log4tango
{
typedef unsigned short mode_t;
}

/* _INCLUDE_LOG4TANGO_CONFIG_WIN32_H */
#endif

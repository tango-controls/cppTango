/*
 * Export.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_EXPORT_H
#define _LOG4TANGO_EXPORT_H

#ifdef LOG4TANGO_HAS_DLL
#	ifdef LOG4TANGO_BUILD_DLL
#		define LOG4TANGO_EXPORT __declspec(dllexport)
#	else
#		define LOG4TANGO_EXPORT __declspec(dllimport)
#	endif
#else
#	define LOG4TANGO_EXPORT 
#endif

#endif // _LOG4TANGO_EXPORT_H


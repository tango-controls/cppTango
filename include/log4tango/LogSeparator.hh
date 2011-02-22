/*
 * LogSeparator.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LOG_SEPARATOR_H
#define _LOG4TANGO_LOG_SEPARATOR_H

#include <log4tango/Portability.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// Class : LogInitiator
//-----------------------------------------------------------------------------

#ifdef WIN32
	#ifdef _TANGO_LIB
class __declspec(dllimport) LogInitiator {
	#else
class LOG4TANGO_EXPORT LogInitiator {
	#endif
#else
class LOG4TANGO_EXPORT LogInitiator {
#endif
public:
  static LogInitiator _begin_log;
};

//-----------------------------------------------------------------------------
// Class : LogSeparator
//-----------------------------------------------------------------------------
#ifdef WIN32
	#ifdef _TANGO_LIB
class __declspec(dllimport) LogSeparator {
	#else
class LOG4TANGO_EXPORT LogSeparator {
	#endif
#else
class LOG4TANGO_EXPORT LogSeparator {
#endif
public:
  static LogSeparator _end_log;
};

} // namespace log4tango

#endif // _LOG4TANGO_LOG_SEPARATOR_H

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
class LOG4TANGO_EXPORT LogInitiator {
public:
  static LogInitiator _begin_log;
};

//-----------------------------------------------------------------------------
// Class : LogSeparator
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT LogSeparator {
public:
  static LogSeparator _end_log;
};

} // namespace log4tango

#endif // _LOG4TANGO_LOG_SEPARATOR_H

/*
 * LogStream.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LOG_STREAM_H
#define _LOG4TANGO_LOG_STREAM_H

#include <log4tango/Portability.hh>
#ifdef LOG4TANGO_HAVE_SSTREAM
# include <iostream>
#else
# include <iostream.h>
#endif
#include <log4tango/threading/Threading.hh>
#include <log4tango/LogStreambuf.hh>
#include <log4tango/LogSeparator.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// Class : LogStream
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT LogStream
{
protected:

  threading::RecursiveMutex _rmutex;

  std::ostream _ostream;

public:
  
  LogStream(LogStreamBuf* stream_buf);

  virtual ~LogStream();

  inline LogStream& operator<< (const LogInitiator&) {
    return *this;
  }
  
  template<typename T> LogStream& operator<< (const T& t) {
    _rmutex.lock();
    _ostream << t;
    return *this;
  }

  inline LogStream& operator<< (LogSeparator&) {
    _rmutex.lock();
    _ostream.flush();
    _rmutex.unlockn();
    return *this;
  }
};

inline std::ostream& 
operator<< (std::ostream& o_str, LogSeparator&)
{
  o_str.flush();
	return o_str; 
}

} // namespace log4tango

#endif // _LOG4TANGO_LOG_STREAM_H

//
// LogStream.hh
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

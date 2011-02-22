//
// LogStreambuf.hh
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
 
#ifndef _LOG4TANGO_STREAM_BUFFER_H
#define _LOG4TANGO_STREAM_BUFFER_H

#include <log4tango/Portability.hh>
#include <log4tango/Logger.hh>

//-----------------------------------------------------------------------------
// #DEFINES
//-----------------------------------------------------------------------------
#define kDEFAULT_BUFFER_SIZE 512

namespace log4tango {

//-----------------------------------------------------------------------------
// Class : LogStreamBuf
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT LogStreamBuf : public std::streambuf
{
  friend class LogStream;

public:

  LogStreamBuf (Logger* logger, 
                Level::Value level,
                bool filter = true,
                size_t bsize = kDEFAULT_BUFFER_SIZE);

  virtual ~LogStreamBuf();

protected:

   virtual std::streamsize xsputn (const char*, std::streamsize);


   virtual int sync (void);

private:

  int flush_buffer (void);

  char *_buffer;

  Logger* _logger;

  Level::Value _level;

  bool _filter;
};

} // namespace log4tango

#endif // _LOG4TANGO_STREAM_BUFFER_H



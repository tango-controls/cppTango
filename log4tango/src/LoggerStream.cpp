//
// LoggerStream.cpp
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

#include "PortabilityImpl.hh"

#ifdef LOG4TANGO_HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <log4tango/LoggerStream.hh>
#include <log4tango/Logger.hh>

namespace log4tango {

LoggerStream::LoggerStream (Logger& logger, Level::Value level, bool filter) 
  : _logger(logger),
    _level(level),
    _filter(filter),
    _buffer(new std::ostringstream)
{

}

LoggerStream::~LoggerStream() 
{ 
  flush();
#ifdef LOG4TANGO_HAVE_SSTREAM
  if (_buffer) {
    delete _buffer;
    _buffer = 0;
  }
#endif
}

void LoggerStream::flush (void) 
{
#ifdef LOG4TANGO_HAVE_SSTREAM
  if (_buffer && _buffer->tellp() > 0)
#else
  if (_buffer && _buffer->pcount())
#endif
  {
    if (_filter) {
      _logger.log(_level, _buffer->str());
    } else {
      _logger.log_unconditionally(_level, _buffer->str());
    }
#ifdef LOG4TANGO_HAVE_SSTREAM
    _buffer->str("");
#else
    delete _buffer;
    _buffer = 0;
#endif
  }
}

} // namespace log4tango

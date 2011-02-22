/*
 * LoggerStream.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

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

//
// Logger.cpp
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
# include <unistd.h>
#endif

#include <log4tango/Logger.hh>
#ifdef LOG4TANGO_LOGGERS_USE_LOGSTREAM  
# include <log4tango/LogStream.hh>
#endif
#ifdef LOG4TANGO_HAS_NDC
# include <log4tango/NDC.hh>
#endif
#include "StringUtil.hh"

namespace log4tango {

Logger::Logger(const std::string& name, Level::Value level) 
  : _name(name), _level(level)
{
#ifdef LOG4TANGO_LOGGERS_USE_LOGSTREAM  
   LogStreamBuf* stream_buf;
   stream_buf = new LogStreamBuf(this, Level::DEBUG);
  _log_streams[_DEBUG_STREAM_ID] = new LogStream(stream_buf);
   stream_buf = new LogStreamBuf(this, Level::INFO);
  _log_streams[_INFO_STREAM_ID] = new LogStream(stream_buf);
   stream_buf = new LogStreamBuf(this, Level::WARN);
  _log_streams[_WARN_STREAM_ID] = new LogStream(stream_buf);
   stream_buf = new LogStreamBuf(this, Level::ERROR);
  _log_streams[_ERROR_STREAM_ID] = new LogStream(stream_buf);
  stream_buf = new LogStreamBuf(this, Level::FATAL);
  _log_streams[_FATAL_STREAM_ID] = new LogStream(stream_buf);
#else
  //no-op
#endif // LOG4TANGO_LOGGERS_USE_LOGSTREAM 
}

Logger::~Logger() 
{
#ifdef LOG4TANGO_LOGGERS_USE_LOGSTREAM 
  for (int i = _FATAL_STREAM_ID; i <= _DEBUG_STREAM_ID; i++) {
    if (_log_streams[i]) delete _log_streams[i];
    _log_streams[i] = 0;
  }
#endif
}
    
void Logger::set_level (Level::Value level)
{
  if ((level >= Level::OFF) && (level <= Level::DEBUG)) {
    _level = level;
    { //-- Begin critical section -----------------------------
      threading::ScopedLock guard(_appendersMutex); 
      if (!_appenders.empty()) {
        AppenderMapIterator i = _appenders.begin();
        AppenderMapIterator e = _appenders.end();
        for (; i != e ; ++i) {
          i->second->level_changed(_level);
        }
      }
    } //-- End critical section ------------------------------
  } 
}
       
void Logger::call_appenders (const LoggingEvent& event)
{
  std::vector<std::string> *bad_appenders = 0;
  { //-- Begin critical section -----------------------------
    threading::ScopedLock guard(_appendersMutex); 
    if (!_appenders.empty()) {
      AppenderMapIterator i = _appenders.begin();
      AppenderMapIterator e = _appenders.end();
      for (; i != e ; ++i) {
        if (i->second->append(event) == -1) {
          if (!bad_appenders) {
            bad_appenders = new std::vector<std::string>;
          }
          if (bad_appenders) {
            bad_appenders->push_back(i->second->get_name());
          }
        }
      }
    }
  } //-- End critical section ------------------------------
  if (bad_appenders) {
    for (unsigned int a = 0; a < bad_appenders->size(); a++) {
      remove_appender((*bad_appenders)[a]);
    }
    delete bad_appenders;
  }
}

void Logger::log_unconditionally(Level::Value level, 
                                   const char* format, ...) 
{
  va_list va;
  va_start(va, format);
  log_unconditionally(level, StringUtil::vform(format, va));
  va_end(va);
}

void Logger::log_unconditionally(Level::Value level, 
                                 const std::string& message) 
{
#ifdef LOG4TANGO_HAS_NDC
  LoggingEvent event(get_name(), message, NDC::get(), level);
#else
  LoggingEvent event(get_name(), message, level);
#endif
  call_appenders(event);
}

void Logger::log (Level::Value level, const char* format, ...)
{ 
  if (is_level_enabled(level)) {
    va_list va;
    va_start(va, format);
    log_unconditionally(level, StringUtil::vform(format, va));
    va_end(va);
  }
}
    
void Logger::debug (const char* format, ...) {
  if (is_level_enabled(Level::DEBUG)) {
    va_list va;
    va_start(va,format);
    log_unconditionally(Level::DEBUG, StringUtil::vform(format, va));
    va_end(va);
  }
}
    
void Logger::info (const char* format, ...) {
  if (is_level_enabled(Level::INFO)) {
    va_list va;
    va_start(va,format);
    log_unconditionally(Level::INFO, StringUtil::vform(format, va));
    va_end(va);
  }
}

void Logger::warn (const char* format, ...) {
  if (is_level_enabled(Level::WARN)) {
    va_list va;
    va_start(va,format);
    log_unconditionally(Level::WARN, StringUtil::vform(format, va));
    va_end(va);
  }
}

void Logger::error (const char* format, ...) {
  if (is_level_enabled(Level::ERROR)) {
    va_list va;
    va_start(va,format);
    log_unconditionally(Level::ERROR, StringUtil::vform(format, va));
    va_end(va);
  }
}

void Logger::fatal (const char* format, ...) {
  if (is_level_enabled(Level::FATAL)) {
    va_list va;
    va_start(va,format);
    log_unconditionally(Level::FATAL, StringUtil::vform(format, va));
    va_end(va);
  }
}

} // namespace log4tango


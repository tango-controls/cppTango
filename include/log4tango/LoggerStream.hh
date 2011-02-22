//
// LoggerStream.hh
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

#ifndef _LOG4TANGO_LOGGER_STREAM_H
#define _LOG4TANGO_LOGGER_STREAM_H

#include <log4tango/Portability.hh>
#include <log4tango/Level.hh>
#ifdef LOG4TANGO_HAVE_SSTREAM
# include <sstream>
#endif
#include <log4tango/LogSeparator.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// FORWARD DECLARATION
//----------------------------------------------------------------------------- 
class LOG4TANGO_EXPORT Logger;
class LoggerStream;

//-----------------------------------------------------------------------------
// DEFINE WHAT IS A LS_TERMINATOR 
//----------------------------------------------------------------------------- 
typedef LoggerStream& (*ls_terminator) (LoggerStream&);


typedef class std::ios_base&(*StdIosFlag)(class std::ios_base&);


//-----------------------------------------------------------------------------
// class : LoggerStream
//----------------------------------------------------------------------------- 
class LoggerStream 
{
public:

  /**
   * Construct a LoggerStream for given Logger with given level.
   * @param logger The logger this stream will send log messages to.
   * @param level The level the log messages will get or 
   * Level::NOTSET to silently discard any streamed in messages.
   * @param filter The filter flag
   **/
  LOG4TANGO_EXPORT LoggerStream(Logger& logger, Level::Value level, bool filter = true);

  /**
   * Destructor for LoggerStream&)
   **/
  LOG4TANGO_EXPORT ~LoggerStream();
  
  /**
   * Returns the destination Logger for this stream.
   * @returns The Logger.
   **/
  inline LOG4TANGO_EXPORT Logger& get_logger (void) const { 
    return _logger; 
  };

  /**
   * Returns the level for this stream.
   * @returns The level.
   **/
  inline LOG4TANGO_EXPORT Level::Value get_level (void) const { 
      return _level; 
  };

  /**
   * Streams in a Initiator. Just a trick to return a ref to self.
   * @param i The log initiator
   * @returns A reference to itself.
   **/
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (LogInitiator& i) {
    return *this;
  }
  
  /**
   * Streams in a Separator.Sends the contents of the stream buffer
   * to the Logger with set level and empties the buffer.
   * @param s The log separator
   * @returns A reference to itself.
   **/
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (LogSeparator& s) {
    flush();
    return *this;
  }

  /**
   * Streams in a ls_manipulator. Sends the contents of the stream buffer
   * to the Logger with set level and empties the buffer.
   * @param endoflog The log terminator
   * @returns A reference to itself.
   **/
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (ls_terminator endoflog) {
    flush();
    return *this;
  }

  /**
   * Flush the contents of the stream buffer to the Logger and
   * empties the buffer.
   **/
  LOG4TANGO_EXPORT void flush (void);

  /**
   * Streams in a std stream manipulator.
   * @param _F the manipulator function
   * @returns a reference to self.
   **/
#ifdef WIN32
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (std::ios_base&(_cdecl *_F)(std::ios_base&)) {
#else
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (std::ios_base&(*_F)(std::ios_base&)) {
#endif
#ifndef LOG4TANGO_HAVE_SSTREAM 
    if (!_buffer) 
      _buffer = new std::ostringstream;
#endif
    if (_buffer) 
      (*_F)(*(std::ios_base *)(_buffer));
    return *this;
  }

  /**
   * Stream in arbitrary types and objects.  
   * @param t The value or object to stream in.
   * @returns A reference to itself.
   **/
  template<typename T> LoggerStream& operator<< (const T& t) {
    if (_level != Level::OFF) {
#ifndef LOG4TANGO_HAVE_SSTREAM
      if (!_buffer) 
        _buffer = new std::ostringstream;
#endif
      if (_buffer) 
        (*_buffer) << t;
    }
    return *this;
  }
  
private:

  Logger& _logger;

  Level::Value _level;
  
  bool _filter;

  std::ostringstream* _buffer;
};

} // namespace log4tango

namespace std {
  //-- A dummy <ls_terminator>
  inline log4tango::LoggerStream& endl (log4tango::LoggerStream& ls) {
    return ls;
  }
} // namespace std

#endif // _LOG4TANGO_LOGGER_STREAM_H

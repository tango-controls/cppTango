/*
 * LoggerStream.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

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
   * @param separator The Initiator
   * @returns A reference to itself.
   **/
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (LogInitiator&) {
    return *this;
  }
  
  /**
   * Streams in a Separator.Sends the contents of the stream buffer
   * to the Logger with set level and empties the buffer.
   * @param separator The Separator
   * @returns A reference to itself.
   **/
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (LogSeparator&) {
    flush();
    return *this;
  }

  /**
   * Streams in a ls_manipulator. Sends the contents of the stream buffer
   * to the Logger with set level and empties the buffer.
   * @param separator The Separator
   * @returns A reference to itself.
   **/
  inline LOG4TANGO_EXPORT LoggerStream& operator<< (ls_terminator) {
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
   * @param the manipulator function
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

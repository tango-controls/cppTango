/*
 * Logger.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LOGGER_H
#define _LOG4TANGO_LOGGER_H

//-----------------------------------------------------------------------------
// IMPL. OPTION
//----------------------------------------------------------------------------- 
//#define LOG4TANGO_LOGGERS_USE_LOGSTREAM
 
#include <log4tango/Portability.hh>
#include <log4tango/AppenderAttachable.hh>
#include <log4tango/LoggingEvent.hh>
#include <log4tango/Level.hh>
#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM
# include <log4tango/LoggerStream.hh>
#endif

namespace log4tango {

#ifdef LOG4TANGO_LOGGERS_USE_LOGSTREAM
//-----------------------------------------------------------------------------
// FORWARD DECL.
//-----------------------------------------------------------------------------  
class LogStream;
#endif

//-----------------------------------------------------------------------------
// class : Logger
//-----------------------------------------------------------------------------   
class LOG4TANGO_EXPORT Logger : public AppenderAttachable
{
public:   

  /**
   * Constructor 
   * @param name the fully qualified name of this Logger
   * @param level the level for this Logger. Defaults to Level::OFF
   **/
  Logger(const std::string& name, 
         Level::Value level = Level::OFF);

  /**
   * Destructor 
   **/
  virtual ~Logger();

  /**
   * Return the logger name.
   * @returns The logger name.
   */       
  inline const std::string& get_name() const {
    return _name; 
  }
      
  /**
   * Set the level of this Logger (silently ignores invalid values)
   * @param level The level to set.
   **/
  void set_level (Level::Value level);

  /**
   * Returns the assigned Level, if any, for this Logger.
   * @return Level - the assigned Level, can be Level::NOTSET
   **/
  inline Level::Value get_level() const {
    return _level;
  }

  /** 
   * Returns true if the level of the Logger is equal to
   * or higher than given level.
   * @param level The level to compare with.
   * @returns whether logging is enable for this level.
   **/
  bool is_level_enabled (Level::Value level) const {
    return _level >= level;
  }

  /** 
   * Log a message with the specified level.
   * @param level The level of this log message.
   * @param string_format Format specifier for the log .
   * @param ... The arguments for string_format 
   **/  
  void log (Level::Value level, 
            const char* string_format, ...);

  /** 
   * Log a message with the specified level.
   * @param level The level of this log message.
   * @param message string to write in the log file
   **/  
  inline void log (Level::Value level, const std::string& message) 
  { 
    if (is_level_enabled(level)) {
      log_unconditionally(level, message);
    }
  }
 
  /** 
   * Log a message with the specified level without level checking.
   * @param level The level of this log message.
   * @param string_format Format specifier for the log .
   * @param ... The arguments for string_format 
   **/
  void log_unconditionally (Level::Value level, 
                            const char* string_format, ...);

  /** 
   * Log a message with the specified level without level checking.
   * @param level The level of this log message.
   * @param message string to write in the log file
   **/  
  void log_unconditionally (Level::Value level, 
                            const std::string& message);
                
  /** 
   * Log a message with debug level.
   * @param string_format Format specifier for the log.
   * @param ... The arguments for string_format 
   **/  
  void debug (const char* string_format, ...);

  /** 
   * Log a message with debug level.
   * @param message string to write in the log file
   **/  
  inline void debug (const std::string& message) {
    if (is_level_enabled(Level::DEBUG)) {
      log_unconditionally(Level::DEBUG, message);
    }
  }

  /**
   * Return true if the Logger will log messages with level DEBUG.
   * @returns Whether the Logger will log.
   **/ 
  inline bool is_debug_enabled (void) const { 
    return is_level_enabled (Level::DEBUG);
  };
     
#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM   
  /**
   * Return a LoggerStream with level DEBUG.
   * @returns The LoggerStream.
   **/
  inline LoggerStream debug_stream (void) {
    return LoggerStream(*this, Level::DEBUG, true);
  }
#else
  /**
   * Return the DEBUG LogStream.
   * @returns The DEBUG LogStream.
   **/
  inline LogStream& debug_stream (void) {
    return *_log_streams[_DEBUG_STREAM_ID];
  }
#endif

  /** 
   * Log a message with info level.
   * @param string_format Format specifier for the log.
   * @param ... The arguments for string_format 
   **/  
  void info (const char* string_format, ...);

  /** 
   * Log a message with info level.
   * @param message string to write in the log file
   **/  
  inline void info (const std::string& message) {
    if (is_level_enabled(Level::INFO)) {
      log_unconditionally(Level::INFO, message);
    }
  }

  /**
   * Return true if the Logger will log messages with level INFO.
   * @returns Whether the Logger will log.
   **/ 
  inline bool is_info_enabled (void) const { 
    return is_level_enabled(Level::INFO);
  };

#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM   
  /**
   * Return a LoggerStream with level INFO.
   * @returns The LoggerStream.
   **/
  inline LoggerStream info_stream (void) {
    return LoggerStream(*this, Level::INFO, true);
  }
#else
  /**
   * Return the INFO LogStream.
   * @returns The INFO LogStream.
   **/
  inline LogStream& info_stream (void) {
    return *_log_streams[_INFO_STREAM_ID];
  }
#endif
      
  /** 
   * Log a message with warn level.
   * @param string_format Format specifier for the log.
   * @param ... The arguments for string_format 
   **/  
  void warn (const char* string_format, ...);

  /** 
   * Log a message with warn level.
   * @param message string to write in the log file
   **/  
  inline void warn (const std::string& message) {
    if (is_level_enabled(Level::WARN)) {
      log_unconditionally(Level::WARN, message);
    }
  }

  /**
   * Return true if the Logger will log messages with level WARN.
   * @returns Whether the Logger will log.
   **/ 
  inline bool is_warn_enabled (void) const { 
    return is_level_enabled(Level::WARN);
  };

#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM   
  /**
   * Return a LoggerStream with level WARN.
   * @returns The LoggerStream.
   **/
  inline LoggerStream warn_stream (void) {
    return LoggerStream(*this, Level::WARN, true);
  };
#else
  /**
   * Return the WARN LogStream.
   * @returns The WARN LogStream.
   **/
  inline LogStream& warn_stream (void) {
    return *_log_streams[_WARN_STREAM_ID];
  }
#endif

  /** 
   * Log a message with error level.
   * @param string_format Format specifier for the log.
   * @param ... The arguments for string_format 
   **/  
  void error (const char* string_format, ...);

  /** 
   * Log a message with error level.
   * @param message string to write in the log file
   **/  
  inline void error (const std::string& message) {
    if (is_level_enabled(Level::ERROR)) {
      log_unconditionally(Level::ERROR, message);
    }
  }

  /**
   * Return true if the Logger will log messages with level ERROR.
   * @returns Whether the Logger will log.
   **/ 
  inline bool is_error_enabled (void) const { 
    return is_level_enabled(Level::ERROR);
  };

#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM   
  /**
   * Return a LoggerStream with level ERROR.
   * @returns The LoggerStream.
   **/
  inline LoggerStream error_stream (void) {
      return LoggerStream(*this, Level::ERROR, true);
  };
#else
  /**
   * Return the ERROR LogStream.
   * @returns The ERROR LogStream.
   **/
  inline LogStream& error_stream (void) {
    return *_log_streams[_ERROR_STREAM_ID];
  }
#endif

  /** 
   * Log a message with fatal level. 
   * @param string_format Format specifier for the log.
   * @param ... The arguments for string_format 
   **/  
  void fatal(const char* string_format, ...);

  /** 
   * Log a message with fatal level.
   * @param message string to write in the log file
   **/  
  inline void fatal (const std::string& message) {
    if (is_level_enabled(Level::FATAL)) {
      log_unconditionally(Level::FATAL, message);
    }
  }

  /**
   * Return true if the Logger will log messages with level FATAL.
   * @returns Whether the Logger will log.
   **/ 
  inline bool is_fatal_enabled (void) const { 
    return is_level_enabled(Level::FATAL);
  };
  
#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM   
  /**
   * Return a LoggerStream with level FATAL.
   * @returns The LoggerStream.
   **/
  inline LoggerStream fatal_stream (void) {
    return LoggerStream(*this, Level::FATAL, true);
  };
#else
  /**
   * Return the FATAL LogStream.
   * @returns The FATAL LogStream.
   **/
  inline LogStream& fatal_stream (void) {
    return *_log_streams[_FATAL_STREAM_ID];
  }
#endif

#ifndef LOG4TANGO_LOGGERS_USE_LOGSTREAM   
  /**
   * Return a LoggerStream with given Level.
   * @param level The Level of the LoggerStream.
   * @param filter The filter flag
   * @returns The requested LoggerStream.
   **/
  inline LoggerStream get_stream (Level::Value level, bool filter = true) {
    return LoggerStream(*this, level, filter);
  }
#endif

protected:

  /**
   * Call the appenders.
   *
   * @param event the LogginEvent to log.
   **/
  void call_appenders (const LoggingEvent& event);
  

private:

#ifdef LOG4TANGO_LOGGERS_USE_LOGSTREAM
  /** Stream ids. */
  enum {
    _FATAL_STREAM_ID = 0,
    _ERROR_STREAM_ID = 1,
    _WARN_STREAM_ID  = 2,
    _INFO_STREAM_ID  = 3,
    _DEBUG_STREAM_ID = 4
  };
#endif

  /** The name of this logger. */
  const std::string _name;

  /** The assigned level of this logger. */
  Level::Value _level;

#ifdef LOG4TANGO_LOGGERS_USE_LOGSTREAM
  /** The thread-safe streams of this logger. */
  LogStream *_log_streams[5];
#endif

  /* prevent copying and assignment */
  Logger (const Logger&);
  Logger& operator= (const Logger&);
};

} // namespace log4tango

#endif // _LOG4TANGO_LOGGER_H

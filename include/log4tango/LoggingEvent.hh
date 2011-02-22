/*
 * LoggingEvent.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LOGGINGEVENT_H
#define _LOG4TANGO_LOGGINGEVENT_H

#include <log4tango/Portability.hh>
#include <string>

#include <log4tango/Level.hh>
#include <log4tango/TimeStamp.hh>

namespace log4tango {

/**
 * The internal representation of logging events. When a affirmative
 * logging decision is made a <code>LoggingEvent</code> instance is
 * created. This instance is passed around the different log4tango
 * components.
 *
 * <p>This class is of concern to those wishing to extend log4tango. 
 **/
//-----------------------------------------------------------------------------
// struct : LoggingEvent
//-----------------------------------------------------------------------------   
struct LOG4TANGO_EXPORT LoggingEvent 
{
public:
  /**
   * Instantiate a LoggingEvent from the supplied parameters.
   *
   * <p>Except <code>timeStamp</code> all the other fields of
   * <code>LoggingEvent</code> are filled when actually needed.
   * <p>
   * @param logger The logger of this event.
   * @param message  The message of this event.
   * @param ndc The nested diagnostic context of this event (if supported). 
   * @param level The level of this event.
   **/
#ifdef LOG4TANGO_HAS_NDC
  LoggingEvent(const std::string& logger, 
               const std::string& message, 
               const std::string& ndc, 
               Level::Value level);
#else
  LoggingEvent(const std::string& logger, 
               const std::string& message, 
               Level::Value level);
#endif // LOG4TANGO_HAS_NDC

  /** The logger name. */
  const std::string logger_name;

  /** The application supplied message of logging event. */
  const std::string message;

#ifdef LOG4TANGO_HAS_NDC
  /** The nested diagnostic context (NDC) of logging event. */
  const std::string ndc;
#endif

  /** Level of logging event. */
  Level::Value level;

  /** Name of thread in which this logging event was generated */
  //std::string thread_name;

  /** id of thread in which this logging event was generated */
  long thread_id;

  /** The number of seconds elapsed since the epoch 
      (1/1/1970 00:00:00 UTC) until logging event was created. */
  TimeStamp timestamp;
};

} // namespace log4tango

#endif // _LOG4TANGO_LOGGINGEVENT_H


//
// LoggingEvent.hh
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

  /** Copy constructor */
  LoggingEvent(const LoggingEvent& event);
               
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
  
private:
  /** Prevent implicit copy */
  const LoggingEvent& operator= (const LoggingEvent&);
};

} // namespace log4tango

#endif // _LOG4TANGO_LOGGINGEVENT_H


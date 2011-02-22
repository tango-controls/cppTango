/*
 * Layout.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LAYOUT_H
#define _LOG4TANGO_LAYOUT_H

#include <log4tango/Portability.hh>
#include <log4tango/threading/Threading.hh>
#include <log4tango/LoggingEvent.hh>
#include <string>

namespace log4tango {

//-----------------------------------------------------------------------------
// class : Appender (abstract class)
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT Layout 
{
public:

  /**
   * Constructor for Layout.
   **/
  Layout() {};

  /**
   * Destructor for Layout.
   **/
  virtual ~Layout() {};

  /**
   * Formats the LoggingEvent data to a string that appenders can log.
   * Overload this method to create your own layout format.
   * @param event The LoggingEvent.
   * @returns an appendable string.
   **/
  virtual std::string format (const LoggingEvent& event);
};  
      
} // namespace log4tango

#endif // _LOG4TANGO_LAYOUT_H

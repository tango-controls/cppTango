/*
 * LoggingEvent.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4tango/LoggingEvent.hh>
#include <log4tango/threading/Threading.hh>

namespace log4tango {
   

LoggingEvent::LoggingEvent(const std::string& _logger_name, 
                           const std::string& _message,
#ifdef LOG4TANGO_HAS_NDC
                           const std::string& _ndc, 
#endif
                           Level::Value _level) 
  : logger_name(_logger_name),
    message(_message),
#ifdef LOG4TANGO_HAS_NDC
    ndc(_ndc),
#endif
    level(_level)
{
  //-- thread_name = threading::get_thread_id();
  thread_id =  threading::thread_id();
}

} // namespace log4tango 

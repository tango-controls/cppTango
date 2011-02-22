/*
 * Layout.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#ifdef LOG4TANGO_HAVE_SSTREAM
# include <sstream>
#endif
#include <log4tango/Level.hh>
#include <log4tango/Layout.hh>

namespace log4tango {

std::string Layout::format (const LoggingEvent& event) 
{
    const char sep = ' ';
    
    std::ostringstream message;
    
    message << event.timestamp.get_seconds()
            << sep
            << "[" << event.thread_id << "]"
            << sep
            << Level::get_name(event.level)
            << sep
            << event.logger_name
            << sep
#ifdef LOG4TANGO_HAS_NDC
            << event.ndc 
            << sep  
#endif
            << event.message
            << std::ends;
     
    return message.str();
}

} // namespace log4tango 


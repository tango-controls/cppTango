//
// LoggingEvent.cpp
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
#include <log4tango/LoggingEvent.hh>
#include <log4tango/threading/Threading.hh>

namespace log4tango {
   

LoggingEvent::LoggingEvent (const std::string& _logger_name, 
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

LoggingEvent::LoggingEvent (const LoggingEvent& _src)
 : logger_name(_src.logger_name),
   message(_src.message),
#ifdef LOG4TANGO_HAS_NDC
   ndc(_src.ndc),
#endif
   level(_src.level),
   thread_id(_src.thread_id),
   timestamp(_src.timestamp)
{
  //--noop
}

} // namespace log4tango 

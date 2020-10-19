//
// XMLLayout.cpp
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010,2011,2012
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

#include <log4tango/Portability.hh>
#include <log4tango/LoggingEvent.hh> 
#include <log4tango/XmlLayout.hh> 

#include <cmath>
#include <stdio.h>
#include <sstream>
#include <string>

namespace log4tango {

  XMLLayout::XMLLayout() 
  {
    // no-op
  }

  XMLLayout::~XMLLayout() 
  {
    // no-op 
  } 

  std::string XMLLayout::format (const LoggingEvent& event) 
  {
    auto id = []() -> std::string {
      std::ostringstream out;
      out << std::this_thread::get_id();
      return out.str();
    };

    std::string buf;
    buf.append("<log4j:event logger=\"");
    buf.append(event.logger_name);
    buf.append("\" timestamp=\"");
    double ts_ms = 1000. * event.timestamp.get_seconds();
    ts_ms += event.timestamp.get_milliseconds();
    buf.append(std::to_string(std::trunc(ts_ms)));
    buf.append("\" level=\"");
    buf.append(log4tango::Level::get_name(event.level));
    buf.append("\" thread=\"");
    buf.append(id());
    buf.append("\">\r\n");
    buf.append("<log4j:message><![CDATA[");
#if APPEND_ECAPING_CDATA_IMPLEMENTED
    appendEscapingCDATA(buf, event.message);	
#else
    buf.append(event.message);
#endif
    buf.append("]]></log4j:message>\r\n"); 
    buf.append("<log4j:NDC><![CDATA[");
    buf.append("]]></log4j:NDC>\r\n");	  
    buf.append("</log4j:event>\r\n\r\n");
    return buf;
  }
 
 /*
  * Ensures that embeded CDEnd strings (]]>) are handled properly
  * within message, NDC and throwable tag text.
  *
  * @param buf StringBuffer holding the XML data to this point.  The
  * initial CDStart (<![CDATA[) and final CDEnd (]]>) of the CDATA
  * section are the responsibility of the calling method.
  * @param str The String that is inserted into an existing CDATA Section within buf.  
  */

  void XMLLayout::appendEscapingCDATA(std::string buf, std::string str) 
  {
    const std::string CDATA_START("<![CDATA[");
    const std::string CDATA_END("]]>");
    const std::string CDATA_PSEUDO_END("]]&gt;");
    const std::string CDATA_EMBEDED_END = CDATA_END + CDATA_PSEUDO_END + CDATA_START;
    const int CDATA_END_LEN = CDATA_END.size();

    std::string::size_type end = str.find(CDATA_END);
    
    if (end == std::string::npos) {
      buf.append(str);
      return;
    }

    unsigned int start = 0;
    while (end != std::string::npos) {
      buf.append(str.substr(start, end - start));
      buf.append(CDATA_EMBEDED_END);
      start = end + CDATA_END_LEN;
      if (start < str.size()) {
	      end = str.find(CDATA_END, start);
      } else {
	      return;
      }
    }

    buf.append(str.substr(start));
  }

} // namespace log4tango 

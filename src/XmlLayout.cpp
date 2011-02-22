/*
 * XMLLayout.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <stdio.h>
#include <log4tango/threading/Threading.hh> 
#include <log4tango/LoggingEvent.hh> 
#include <log4tango/XmlLayout.hh> 

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
    std::string buf;
    buf.append("<log4j:event logger=\"");
    buf.append(event.logger_name);
    buf.append("\" timestamp=\"");
    char ts_str[32];
    ::sprintf(ts_str, "%.0f", 1000. * (double)event.timestamp.get_seconds());
    buf.append(ts_str);
    buf.append("\" level=\"");
    buf.append(log4tango::Level::get_name(event.level));
    buf.append("\" thread=\"");
    buf.append(log4tango::threading::get_thread_id());
    buf.append("\">\r\n");
    buf.append("<log4j:message><![CDATA[");
#if APPEND_ECAPING_CDATA_IMPLEMENTED
    appendEscapingCDATA(buf, event.message);	
#else
    buf.append(event.message);
#endif
    buf.append("]]></log4j:message>\r\n"); 
    buf.append("<log4j:NDC><![CDATA[");
#ifdef LOG4TANGO_HAS_NDC
# if APPEND_ECAPING_CDATA_IMPLEMENTED
    appendEscapingCDATA(buf, event.ndc);
# else
    buf.append(event.ndc);  
# endif
#endif	
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

/*
 * XMLLayout.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_XML_LAYOUT_H
#define _LOG4TANGO_XML_LAYOUT_H

#include <log4tango/Portability.hh>
#include <log4tango/Layout.hh>

namespace log4tango {

/**
* XMLLayout is a simple fixed format Layout implementation. 
**/
class LOG4TANGO_EXPORT XMLLayout : public Layout 
{
public:
  /**
   * Ctor.
   **/
  XMLLayout ();

  /**
   * Dtor.
   **/
  virtual ~XMLLayout ();

  /**
   * Formats the LoggingEvent in XML
   **/
  virtual std::string format (const LoggingEvent& event);

private:
  /**
   * Ensures that embeded CDEnd strings (]]>) are handled properly
   **/
  void appendEscapingCDATA(std::string buf, std::string str);
};

} // namespace log4tango

#endif // _LOG4TANGO_XML_LAYOUT_H

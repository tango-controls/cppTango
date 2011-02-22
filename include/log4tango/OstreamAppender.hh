/*
 * OstreamAppender.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_OSTREAMAPPENDER_H
#define _LOG4TANGO_OSTREAMAPPENDER_H

#include <log4tango/Portability.hh>
#include <string>
#include <iostream>
#include <log4tango/LayoutAppender.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// class : OstreamAppender (appends LoggingEvents to ostreams)
//-----------------------------------------------------------------------------   
class LOG4TANGO_EXPORT OstreamAppender : public LayoutAppender {
public:
  OstreamAppender(const std::string& name, std::ostream* stream);
  virtual ~OstreamAppender();

  virtual bool reopen();
  virtual void close();

protected:
  virtual int _append (const LoggingEvent& event);
  std::ostream* _stream;
};

} // namespace log4tango 

#endif // _LOG4TANGO_OSTREAMAPPENDER_HH

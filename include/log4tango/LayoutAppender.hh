/*
 * LayoutAppender.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_LAYOUTAPPENDER_H
#define _LOG4TANGO_LAYOUTAPPENDER_H

#include <log4tango/Portability.hh>
#include <string>
#include <log4tango/Appender.hh>
#include <log4tango/PatternLayout.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// class : LayoutAppender (superclass for appenders that require a Layout)
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT LayoutAppender : public Appender 
{
public:

  typedef Layout DefaultLayoutType;

  LayoutAppender(const std::string& name);

  virtual ~LayoutAppender();

  virtual bool requires_layout() const;

  virtual void set_layout (Layout* layout = 0);

protected:

  Layout& get_layout();

private:
  Layout* _layout;
};

} // namespace log4tango

#endif // _LOG4TANGO_LAYOUTAPPENDER_H


/*
 * LayoutAppender.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4tango/LayoutAppender.hh>

namespace log4tango {

LayoutAppender::LayoutAppender(const std::string& name) 
  : Appender(name), _layout(new DefaultLayoutType()) 
{
  // no-op
}

LayoutAppender::~LayoutAppender() 
{
  if (_layout) {
    delete _layout;
    _layout = 0;
  }
}

bool LayoutAppender::requires_layout (void) const 
{
  return true;
}

void LayoutAppender::set_layout (Layout* layout) 
{
  if (layout != _layout) {
    if (_layout) {
      delete _layout;
      _layout = 0;
    }
    _layout = (layout == 0) ? new DefaultLayoutType() : layout;
  }
}

Layout& LayoutAppender::get_layout (void) {
  return *_layout;
}

} // namespace log4tango

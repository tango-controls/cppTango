/*
 * Appender.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4tango/Appender.hh>

namespace log4tango {

Appender::Appender (const std::string& name) 
 : _name(name)
#if APPENDERS_HAVE_LEVEL_THRESHOLD
  ,_level(Level::DEBUG)
#endif
#if APPENDERS_HAVE_FILTERS
  ,_filter(0) 
#endif
{
  //no-op
}

Appender::~Appender() 
{
#if APPENDERS_HAVE_FILTERS
  if (_filter) {
    delete _filter;
  }
#endif
}
  
bool Appender::is_valid (void) const {
  return true;
}
    
bool Appender::reopen (void) 
{
 return true;
}
    
#if defined(APPENDERS_HAVE_LEVEL_THRESHOLD) || defined(APPENDERS_HAVE_FILTERS)
int Appender::append (const LoggingEvent& event) 
{
# if APPENDERS_HAVE_LEVEL_THRESHOLD
    if (event.level <= _level) 
    {
# endif
# if APPENDERS_HAVE_FILTERS
      if (!_filter || (_filter->decide(event) != Filter::DENY)) 
      {
# endif

        return _append(event);

# if APPENDERS_HAVE_FILTERS
      }
# endif
# if APPENDERS_HAVE_LEVEL_THRESHOLD
    }
# endif
  return 0;
}
#endif // APPENDERS_HAVE_LEVEL_THRESHOLD || APPENDERS_HAVE_FILTERS

#if APPENDERS_HAVE_LEVEL_THRESHOLD
void Appender::set_level (Level::Value level) 
{
  _level = level;
}

Level::Value Appender::get_level (void) const 
{
  return _level;
}
#endif // APPENDERS_HAVE_LEVEL_THRESHOLD   

#if APPENDERS_HAVE_FILTERS
void Appender::set_filter (Filter* filter) 
{
  if (_filter != filter) {
    if (_filter) {
      delete _filter;
    }
    _filter = filter;
  }
}
    
Filter* Appender::get_filter (void) 
{
  return _filter;
}
#endif // APPENDERS_HAVE_FILTERS

void Appender::level_changed (Level::Value new_level) 
{
  //--noop
}
  
} // namespace log4tango

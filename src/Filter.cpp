/*
 * Filter.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifdef APPENDERS_HAVE_FILTERS

#include "PortabilityImpl.hh"
#include <log4tango/Filter.hh>

namespace log4tango {

Filter::Filter() 
  : _chain(0) 
{
  //no-op
}

Filter::~Filter() 
{
  if (_chain) {
    delete _chain;
    _chain = 0;
  }
}

void Filter::set_chained_filter (Filter* filter) 
{
  if (filter != _chain) {
    if (_chain) {
      delete _chain;
    }
    _chain = filter;
  }
}

Filter* Filter::get_end_of_chain (void) 
{
  Filter* end = this;
  while(end->get_chained_filter()) {
    end = end->get_chained_filter();
  }
  return end;
}

void Filter::append_chained_filter (Filter* filter) 
{
  Filter* end = get_end_of_chain();
  end->set_chained_filter(filter);
}

Filter::Decision Filter::decide (const LoggingEvent& event) 
{
  Filter::Decision decision = _decide(event);
  if ((Filter::NEUTRAL == decision) && get_chained_filter()) {
    decision = get_chained_filter()->decide(event);
  }
  return decision;
}
    
} // namespace log4tango

#endif // APPENDERS_HAVE_FILTERS

//
// Filter.cpp
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

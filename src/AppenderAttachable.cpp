//
// AppenderAttachable.cpp
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
//


#include "PortabilityImpl.hh"
#include <log4tango/AppenderAttachable.hh>

namespace log4tango {

typedef AppenderMap::value_type AppenderMapValue;

AppenderAttachable::AppenderAttachable () 
{
  // no-op ctor.
}

AppenderAttachable::~AppenderAttachable () 
{
  remove_all_appenders();
}

void AppenderAttachable::add_appender (Appender* appender) 
{
  if (appender) {
    threading::ScopedLock guard(_appendersMutex);
    AppenderMapValue pair(appender->get_name(), appender);
    _appenders.insert(pair);
  }
}

AppenderList AppenderAttachable::get_all_appenders (void) 
{
  threading::ScopedLock guard(_appendersMutex); 
  AppenderList al(0);
  AppenderMapIterator it = _appenders.begin();
  AppenderMapIterator end = _appenders.end();
  for (; it != end ; ++it) {
    al.push_back(it->second);
  }
  return al;
}

Appender* AppenderAttachable::get_appender (const std::string& name) 
{
  threading::ScopedLock guard(_appendersMutex); 
  Appender* appender = 0;
  AppenderMapIterator it = _appenders.find(name);
  if (it != _appenders.end()) {
    return it->second;
  }
  return 0;
}

bool AppenderAttachable::is_attached (Appender* appender) 
{
  threading::ScopedLock guard(_appendersMutex); 
  if (appender && _appenders.find(appender->get_name()) != _appenders.end()) {
    return true;
  }
  return false;
}

void AppenderAttachable::remove_all_appenders (void) 
{
  threading::ScopedLock guard(_appendersMutex);
  AppenderMapIterator it = _appenders.begin();
  AppenderMapIterator end = _appenders.end();
  for (; it != end ; ++it) {
    delete it->second;
  }
  _appenders.clear();
}

void AppenderAttachable::remove_appender(Appender* appender) 
{
  if (appender) {
    remove_appender(appender->get_name());
  }
}

void AppenderAttachable::remove_appender(const std::string& name) 
{
  threading::ScopedLock guard(_appendersMutex); 
  AppenderMapIterator it = _appenders.find(name);
  if (it != _appenders.end()) {
    delete it->second;
    _appenders.erase(it);
  }
}

} // namespace log4tango



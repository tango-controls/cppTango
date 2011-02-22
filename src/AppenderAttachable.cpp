/*
 * AppenderAttachable.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

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



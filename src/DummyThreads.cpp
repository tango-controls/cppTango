/*
 * DummyThreads.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4tango/threading/Threading.hh>

#ifndef LOG4TANGO_HAVE_THREADING

namespace log4tango {
  
namespace threading {

std::string get_thread_id (void) 
{
  return std::string("unknown");
}

} // namespace threading

} // namespace log4tango

#endif // LOG4TANGO_HAVE_THREADING

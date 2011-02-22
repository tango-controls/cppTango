/*
 * MSThreads.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4tango/threading/Threading.hh>

#if defined(LOG4TANGO_HAVE_THREADING) && defined(LOG4TANGO_USE_MSTHREADS)

namespace log4tango {

namespace threading {

//+----------------------------------------------------------------------------
// method : get_thread_id
//----------------------------------------------------------------------------- 
std::string get_thread_id (void)  
{
  char buffer[16];
  sprintf(buffer, "%lu", ::GetCurrentThreadId());
  return std::string(buffer);
}

//+----------------------------------------------------------------------------
// method : thread_id
//----------------------------------------------------------------------------- 
long thread_id (void) 
{
  return static_cast<long>(::GetCurrentThreadId());
}

} // namespace threading

} // namespace log4tango

#endif // LOG4TANGO_HAVE_THREADING && LOG4TANGO_USE_MSTHREADS

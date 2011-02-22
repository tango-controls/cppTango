/*
 * Threading.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_THREADING_THREADING_H
#define _LOG4TANGO_THREADING_THREADING_H

#include <log4tango/Portability.hh>

#ifdef LOG4TANGO_HAVE_THREADING

# ifdef LOG4TANGO_USE_MSTHREADS
#   include <log4tango/threading/MSThreads.hh>
# endif
# ifdef LOG4TANGO_USE_PTHREADS
#   include <log4tango/threading/PThreads.hh>
# endif

#else 

# include <log4tango/threading/DummyThreads.hh>

#endif

#endif // _LOG4TANGO_THREADING_THREADING_H

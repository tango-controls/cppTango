/*
 * DummyThreads.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_THREADING_DUMMY_THREADS_H
#define _LOG4TANGO_THREADING_DUMMY_THREADS_H

#include <log4tango/Portability.hh>
#include <stdio.h>
#include <string>

namespace log4tango {

namespace threading {

std::string get_thread_id (void);

typedef int Mutex;

typedef int ScopedLock;

#ifdef LOG4TANGO_HAS_NDC
template<typename T> class ThreadLocalDataHolder 
{
public:
  typedef T data_type;

  inline ThreadLocalDataHolder () 
    : _data(0) 
  {
    //no-op
  };

  inline ~ThreadLocalDataHolder () {
    if (_data) {
      delete _data;
    }
  };
    
  inline T* get (void) const {
    return _data;
  };

  inline T* operator->() const { 
    return get(); 
  };

  inline T& operator*() const { 
    return *get(); 
  };

  inline T* release() {
    T* result = _data;
    _data = 0;
    return result;
  };

  inline void reset (T* p = 0) {
    if (_data) { 
      delete _data;
    }
    _data = p;
  };

private:            
    T* _data;            
};
#endif // #ifdef LOG4TANGO_HAS_NDC

} // namespace threading

} // namespace log4tango

#endif // _LOG4TANGO_THREADING_DUMMY_THREADS_H

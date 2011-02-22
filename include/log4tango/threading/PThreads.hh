/*
 * PThreads.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_THREADING_PTHREADS_H
#define _LOG4TANGO_THREADING_PTHREADS_H

#include <log4tango/Portability.hh>  
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <assert.h>

namespace log4tango {

namespace threading {

std::string get_thread_id (void);

long thread_id (void);

//-----------------------------------------------------------------------------
// Class : Mutex
//-----------------------------------------------------------------------------
class Mutex
{
private:
  pthread_mutex_t mutex;

  
public:
  inline Mutex() {
    ::pthread_mutex_init(&mutex, NULL);
  }
  
  inline ~Mutex() {
    ::pthread_mutex_destroy(&mutex);
  }
  
  inline void lock() {
    ::pthread_mutex_lock(&mutex);
  }

  inline void unlock() {
    ::pthread_mutex_unlock(&mutex);
  }
};

//-----------------------------------------------------------------------------
// Class : ScopedLock
//-----------------------------------------------------------------------------
class ScopedLock
{
private:
  Mutex& _mutex;

public:
  inline ScopedLock(Mutex &m) : _mutex(m) {
    _mutex.lock();
  }

  inline ~ScopedLock() {
    _mutex.unlock();
  }
};

//-----------------------------------------------------------------------------
// Class : RecursiveMutex
//-----------------------------------------------------------------------------
class RecursiveMutex
{
public:
  // ctor
  RecursiveMutex (void); 

  // dtor
  ~RecursiveMutex (void);

  // Locking an RecursiveMutex:
  // If <timeout_> is null (the default), <lock> blocks until 
  // the mutex is acquired and returns 1 (true). Otherwise, 
  // <lock> blocks until the mutex is acquired or times out 
  // after <timeout_> milliseconds in which case 0 (false) is 
  // returned. 
  int lock (long timeout_ = 0);
  
  // Releasing an RecursiveMutex:
  // Call unlock <recursion level> times (i.e. one call for 
  // each previous call to lock) or call unlockn just once. 
  // These two methods do nothing if the caller is not the
  // current owner of the mutex.
  void unlock (void); 
  void unlockn (void); 

protected:
  // guards the <recursion level> and <owner id>
  pthread_mutex_t guard_;

  // this condition variable suspends other waiting threads 
  // until the mutex is available
  pthread_cond_t mutex_available_;
  
private:
  // current level of the recursion
  long recursion_level_;

  // current owner of the lock.
  pthread_t owner_id_;
  
  // dummy copy constructor and operator= to prevent copying
  RecursiveMutex (const RecursiveMutex&);
  RecursiveMutex& operator= (const RecursiveMutex&);
};

//-----------------------------------------------------------------------------
// Class : ThreadLocalDataHolder
//-----------------------------------------------------------------------------
#ifdef LOG4TANGO_HAS_NDC
template<typename T> class ThreadLocalDataHolder {

private:
  pthread_key_t _key;

public:
  typedef T data_type;

  inline ThreadLocalDataHolder() {
    ::pthread_key_create(&_key, freeHolder);
  }

  inline static void freeHolder(void *p) {
    assert(p != NULL);
    delete reinterpret_cast<T *>(p);
  }

  inline ~ThreadLocalDataHolder() {
    T *data = get();
    if (data != NULL) {
      delete data;
    }
    ::pthread_key_delete(_key);
  }

  inline T* get() const {
    return reinterpret_cast<T *>(::pthread_getspecific(_key));
  }

  inline T* operator->() const { return get(); }
  inline T& operator*() const { return *get(); }

  inline T* release() {
   T* result = get();
   ::pthread_setspecific(_key, NULL);
   return result;
  }

  inline void reset(T* p = NULL) {
    T *data = get();
    if (data != NULL) {
      delete data;
    }
    ::pthread_setspecific(_key, p);
  }
};
#endif //LOG4TANGO_HAS_NDC

} // namespace threading

} // namespace log4tango

#endif  // _LOG4TANGO_THREADING_PTHREADS_H

//
// MSThreads.hh
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

#ifndef _LOG4TANGO_THREADING_MSTHREADS_H
#define _LOG4TANGO_THREADING_MSTHREADS_H

#include <string>

// deal with ERROR #define

// This #includes windows.h with NOGDI and WIN32_LEAN_AND_MEAN 
// #defined. If this is not what the user wants, #include 
// windows.h before this file.

#ifndef _WINDOWS_ 
#  ifndef NOGDI
#    define NOGDI  // circumvent the ERROR #define in windows.h
#    define LOG4TANGO_UNDEFINE_NOGDI
#  endif

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    define LOG4TANGO_UNDEFINE_WIN32_LEAN_AND_MEAN
#  endif

#  include <windows.h>

#  ifdef LOG4TANGO_UNDEFINE_NOGDI
#    undef NOGDI
#  endif

#  ifdef LOG4TANGO_UNDEFINE_WIN32_LEAN_AND_MEAN
#    undef WIN32_LEAN_AND_MEAN
#  endif

#endif 
// done dealing with ERROR #define

namespace log4tango {

namespace threading {

std::string get_thread_id (void);

long thread_id (void);

//-----------------------------------------------------------------------------
// Class : MSMutex 
//-----------------------------------------------------------------------------      
class LOG4TANGO_EXPORT Mutex 
{
public:

  Mutex() { 
    InitializeCriticalSection(&_criticalSection); 
  }

  ~Mutex() { 
    DeleteCriticalSection(&_criticalSection); 
  }

  inline LPCRITICAL_SECTION get_critical_section (void) {
    return &_criticalSection;
  }

private:
  Mutex(const Mutex&);
  Mutex operator=(const Mutex&);

  CRITICAL_SECTION _criticalSection;
};

//-----------------------------------------------------------------------------
// Class : ScopedLock 
//-----------------------------------------------------------------------------
class ScopedLock 
{
 public:

  ScopedLock (Mutex& mutex) {
    _criticalSection = mutex.get_critical_section();
    EnterCriticalSection(_criticalSection);
  }

  ~ScopedLock() { 
    LeaveCriticalSection(_criticalSection); 
  }

private:
  ScopedLock(const ScopedLock&);
  ScopedLock operator=(const ScopedLock&);

  LPCRITICAL_SECTION _criticalSection;
};

//-----------------------------------------------------------------------------
// Class : RecursiveMutex
//-----------------------------------------------------------------------------
class RecursiveMutex
{
public:
  // ctor
  RecursiveMutex (void) : recursion_level_(0) {
    ::InitializeCriticalSection(&guard_); 
  }

  // dtor
  ~RecursiveMutex (void) { 
    ::DeleteCriticalSection(&guard_); 
  }

  // Locking an RecursiveMutex:
  // If <timeout_> is null (the default), <lock> blocks until 
  // the mutex is acquired and returns 1 (true). Otherwise, 
  // <lock> blocks until the mutex is acquired or times out 
  // after <timeout_> milliseconds in which case 0 (false) is 
  // returned. 
  inline int lock (long timeout_ = 0) {
    ::EnterCriticalSection(&guard_);
    recursion_level_++; 
    return 0;
  }
  
  // Releasing an RecursiveMutex:
  // Call unlock <recursion level> times (i.e. one call for 
  // each previous call to lock) or call unlockn just once. 
  // These two methods do nothing if the caller is not the
  // current owner of the mutex.
  inline void unlock (void) {
    //-should work if called by owner 
    recursion_level_--;
    ::LeaveCriticalSection(&guard_); 
  }

  inline void unlockn (void) {
    //-should work if called by owner 
    while (recursion_level_ > 0) {
       recursion_level_--;
      ::LeaveCriticalSection(&guard_); 
    }
  }

protected:
  // guards the <recursion level>
  CRITICAL_SECTION guard_;
  
private:
  // current level of the recursion
  unsigned long recursion_level_;
  
  // dummy copy constructor and operator= to prevent copying
  RecursiveMutex (const RecursiveMutex&);
  RecursiveMutex& operator= (const RecursiveMutex&);
};

//-----------------------------------------------------------------------------
// Class : ThreadLocalDataHolder
//-----------------------------------------------------------------------------
/** 
 * This class holds Thread local data of type T, i.e. for each
 * thread a ThreadLocalDataHolder holds 0 or 1 instance of T. 
 * The held object must be heap allocated and will be deleted 
 * upon termination of the thread to which it belongs.
 **/
#ifdef LOG4TANGO_HAS_NDC
template<typename T> class ThreadLocalDataHolder 
{
public:
            
  inline ThreadLocalDataHolder() 
    : _key(TlsAlloc()) {
  };

  inline ~ThreadLocalDataHolder() { 
    TlsFree(_key); 
  };
            
  /**
   * Obtains the Object held for the current thread.
   * @return a pointer to the held Object or NULL if no
   * Object has been set for the current thread.
   **/
  inline T* get (void) const {
    return (T*)TlsGetValue(_key);
  };

  /**
   * Obtains the Object held for the current thread. 
   * Initially each thread holds NULL.
   * @return a pointer to the held Object or NULL if no
   * Object has been set for the current thread.
   **/
  inline T* operator->() const { 
    return get(); 
  };

  /**
   * Obtains the Object held for the current thread.
   * @pre get() != NULL
   * @return a reference to the held Object.
   **/
  inline T& operator*() const { 
    return *get(); 
  };

  /**
   * Releases the Object held for the current thread.
   * @post get() == NULL
   * @return a pointer to the Object thas was held for 
   * the current thread or NULL if no Object was held.
   **/
  inline T* release() {
    T* result = (T*)TlsGetValue(_key);
    TlsSetValue(_key, NULL);
    return result;
  };

  /**
   * Sets a new Object to be held for the current thread. A 
   * previously set Object will be deleted.
   * @param p the new object to hold.
   * @post get() == p
   **/
  inline void reset(T* p = NULL) {
    T* thing = (T*)TlsGetValue(_key);
    delete thing;
    TlsSetValue(_key, p);
  };

private:
            
    DWORD _key;          
};

#endif // LOG4TANGO_HAS_NDC

} // namespace threading

} // namespace log4tango

#endif // _LOG4TANGO_THREADING_MSTHREADS_H

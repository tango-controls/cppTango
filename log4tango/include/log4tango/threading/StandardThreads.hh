//
// StandardThreads.hh
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

#ifndef _LOG4TANGO_THREADING_STANARDTHREADS_H
#define _LOG4TANGO_THREADING_STANARDTHREADS_H

#include <string>
#include <mutex>

namespace log4tango {

namespace threading {

std::string get_thread_id();

using Mutex = std::mutex;
using RecursiveMutex = std::recursive_mutex;
using ScopedLock = std::lock_guard<std::mutex>;

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

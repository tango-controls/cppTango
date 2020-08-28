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

} // namespace threading

} // namespace log4tango

#endif // _LOG4TANGO_THREADING_MSTHREADS_H

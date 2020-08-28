//
// StandardThreads.cpp
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

#include <log4tango/threading/Threading.hh>

#ifdef LOG4TANGO_HAVE_THREADING

#include <thread>
#include <sstream>

namespace log4tango {

namespace threading {

//+----------------------------------------------------------------------------
// method : get_thread_id
//-----------------------------------------------------------------------------
std::string get_thread_id()
{
  std::ostringstream out;
  out << std::this_thread::get_id();
  return out.str();
}

} // namespace threading

} // namespace log4tango

#endif // LOG4TANGO_HAVE_THREADING

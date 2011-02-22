//
// FileAppender.cpp
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
//

#include "PortabilityImpl.hh"
#ifdef LOG4TANGO_HAVE_IO_H
# include <io.h>
#endif
#ifdef LOG4TANGO_HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <fcntl.h>
#include <log4tango/FileAppender.hh>

namespace log4tango {

FileAppender::FileAppender(const std::string& name, 
                           const std::string& file_name,
                           bool append,
                           mode_t mode) 
 : LayoutAppender(name),
   _file_name(file_name),
   _flags(O_CREAT | O_APPEND | O_WRONLY),
  _mode(mode) 
{
  if (!append) {
    _flags |= O_TRUNC;
  }
  _fd = ::open(_file_name.c_str(), _flags, _mode);
}
    
FileAppender::FileAppender (const std::string& name, int fd) 
 : LayoutAppender(name),
   _file_name(""),
   _fd(fd),
   _flags(O_CREAT | O_APPEND | O_WRONLY),
   _mode(00644) 
{
  //no-op
}
    
FileAppender::~FileAppender() 
{
  close();
}

void FileAppender::close (void) 
{
  if (_fd != -1) {
    ::close(_fd);
    _fd = -1;
  }
}

bool FileAppender::is_valid (void) const
{
  return (_fd < 0) ? false : true;
}
  
void FileAppender::set_append (bool append) 
{
  if (append) {
    _flags &= ~O_TRUNC;
  } else {
    _flags |= O_TRUNC;
  }
}

bool FileAppender::get_append (void) const 
{
  return (_flags & O_TRUNC) == 0;
}

void FileAppender::set_mode (mode_t mode) 
{
  _mode = mode;
}

mode_t FileAppender::get_mode (void) const 
{
  return _mode;
}

int FileAppender::_append (const LoggingEvent& event) 
{
  std::string message(get_layout().format(event));
  if (!::write(_fd, message.data(), message.length())) {
    return -1;
  }
  return 0;
}

bool FileAppender::reopen (void) 
{
  if (_file_name != "") {
    int fd = ::open(_file_name.c_str(), _flags, _mode);
    if (fd < 0) {
      return false;
    } else {
      if (_fd != -1) {
        ::close(_fd);
      }
      _fd = fd;
    }
  }
  return true;
}

} // namespace log4tango

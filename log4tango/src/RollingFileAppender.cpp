//
// RollingFileAppender.cpp
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

#include "PortabilityImpl.hh"
#ifdef LOG4TANGO_HAVE_IO_H
#    include <io.h>
#endif
#ifdef LOG4TANGO_HAVE_UNISTD_H
#    include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log4tango/RollingFileAppender.hh>
#include <log4tango/Logger.hh>
#ifdef LOG4TANGO_HAVE_SSTREAM
#include <sstream>
#endif

namespace log4tango {

    RollingFileAppender::RollingFileAppender(const std::string& name,
                                             const std::string& file_name, 
                                             size_t max_fs, 
                                             unsigned int max_bi,
                                             bool append,
                                             mode_t mode) :
        FileAppender(name, file_name, append, mode),
        _max_backup_index(max_bi),
        _max_file_size(max_fs) {
    }

    void RollingFileAppender::set_max_backup_index (unsigned int max_bi) { 
        _max_backup_index = max_bi; 
    }
    
    unsigned int RollingFileAppender::get_max_backup_index() const { 
        return _max_backup_index; 
    }

    void RollingFileAppender::set_maximum_file_size (size_t max_fs) {
        _max_file_size = max_fs;
    }

    size_t RollingFileAppender::get_max_file_size() const { 
        return _max_file_size; 
    }

    void RollingFileAppender::roll_over() {
        ::close(_fd);
        if (_max_backup_index > 0) {
            std::ostringstream old_name;
            old_name << _file_name << "." << _max_backup_index << std::ends;
            ::remove(old_name.str().c_str());
            size_t n = _file_name.length() + 1;
            for(unsigned int i = _max_backup_index; i > 1; i--) {
            	std::string newName = old_name.str();
                old_name.seekp(n, std::ios::beg);
                old_name << i-1 << std::ends;
                ::rename(old_name.str().c_str(), newName.c_str());
            }
            ::rename(_file_name.c_str(), old_name.str().c_str());
        }
        _fd = ::open(_file_name.c_str(), _flags, _mode);
    }

    int RollingFileAppender::_append(const LoggingEvent& event) {
        FileAppender::_append(event);
        off_t offset = ::lseek(_fd, 0, SEEK_END);
        if (offset < 0) {
            return -1;
        } else {
            if(static_cast<size_t>(offset) >= _max_file_size) {
                roll_over();
            }
        }
        return 0;
    }
}

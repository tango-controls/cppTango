//
// RollingFileAppender.hh
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

#ifndef _LOG4TANGO_ROLLINGFILEAPPENDER_H
#define _LOG4TANGO_ROLLINGFILEAPPENDER_H

#include <log4tango/Portability.hh>
#include <log4tango/FileAppender.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// class RollingFileAppender (olls over the logfile)
//----------------------------------------------------------------------------- 
class LOG4TANGO_EXPORT RollingFileAppender : public FileAppender
{
 public:

    RollingFileAppender(const std::string& name, 
                        const std::string& file_name,
                        size_t max_fs = 10*1024*1024, 
                        unsigned int max_bi = 1,
                        bool append = true,
                        mode_t mode = 00644);

    virtual void set_max_backup_index(unsigned int maxBackups);

    virtual unsigned int get_max_backup_index() const;

    virtual void set_maximum_file_size (size_t max_fs);

    virtual size_t get_max_file_size() const;

    virtual void roll_over();

protected:

    virtual int _append (const LoggingEvent& event);

    unsigned int _max_backup_index;

    size_t _max_file_size;
};

} // namespace log4tango

#endif // _LOG4TANGO_ROLLINGFILEAPPENDER_H

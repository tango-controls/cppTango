//
// OstreamAppender.cpp
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010,2011,2012
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

#include <log4tango/Portability.hh>
#ifdef LOG4TANGO_HAVE_UNISTD_H
#    include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log4tango/OstreamAppender.hh>

namespace log4tango {

    OstreamAppender::OstreamAppender(const std::string& name, std::ostream* stream) : 
        LayoutAppender(name),
        _stream(stream) {
    }
    
    OstreamAppender::~OstreamAppender() {
        close();
    }

    void OstreamAppender::close() {
        // empty
    }

    int OstreamAppender::_append (const LoggingEvent& event) {
        (*_stream) << get_layout().format(event);
        (*_stream) << std::endl;
        if (!_stream->good()) {
            return -1;
        }
        return 0;
    }

    bool OstreamAppender::reopen() {
        return true;
    }      
}

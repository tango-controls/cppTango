/*
 * OstreamAppender.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
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

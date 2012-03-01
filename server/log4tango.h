/*
 *
 * Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
 *						European Synchrotron Radiation Facility
 *                      BP 220, Grenoble 38043
 *                      FRANCE
 *
 * This file is part of Tango.
 *
 * Tango is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tango is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Tango.  If not, see <http://www.gnu.org/licenses/>.
 *
 * log4tango.h
 *
 * NL - SOLEIL.
 *
 */

#ifndef _LOG4TANGO_H_
#define _LOG4TANGO_H_

#ifdef TANGO_HAS_LOG4TANGO

//-------------------------------------------------------------
// REQUIRED LOG4TANGO STUFFS
//-------------------------------------------------------------
#include <log4tango/Logger.hh>
#include <log4tango/RollingFileAppender.hh>
#include <log4tango/OstreamAppender.hh>
#include <log4tango/PatternLayout.hh>
#include <log4tango/XmlLayout.hh>

//-------------------------------------------------------------
// REQUIRED TANGO LOGGING STUFFS
//-------------------------------------------------------------
#include <logstream.h>
#include <logging.h>

//-------------------------------------------------------------
// LOGGING MACROS (FOR DEVICE DEVELOPERS)
//-------------------------------------------------------------
#define LOG_FATAL(X) \
    get_logger()->fatal X

#define LOG_ERROR(X) \
    get_logger()->error X

#define LOG_WARN(X) \
    get_logger()->warn X

#define LOG_INFO(X) \
    get_logger()->info X

#define LOG_DEBUG(X) \
    get_logger()->debug X

#define FATAL_STREAM \
  if (get_logger()->is_fatal_enabled()) \
    get_logger()->fatal_stream() \
      << log4tango::LogInitiator::_begin_log

#define ERROR_STREAM \
  if (get_logger()->is_error_enabled()) \
    get_logger()->error_stream() \
      << log4tango::LogInitiator::_begin_log

#define WARN_STREAM \
  if (get_logger()->is_warn_enabled()) \
    get_logger()->warn_stream() \
      << log4tango::LogInitiator::_begin_log

#define INFO_STREAM \
  if (get_logger()->is_info_enabled()) \
    get_logger()->info_stream() \
      << log4tango::LogInitiator::_begin_log

#define DEBUG_STREAM \
  if (get_logger()->is_debug_enabled()) \
    get_logger()->debug_stream() \
      << log4tango::LogInitiator::_begin_log

#define ENDLOG \
  log4tango::LogSeparator::_end_log

//-------------------------------------------------------------
// A CLASS TO LOG IN THE NAME OF DEVICE (USING DEV'S LOGGER)
//-------------------------------------------------------------
namespace Tango {

class LogAdapter
{

public:

  LogAdapter(Tango::DeviceImpl *dev);

  virtual ~LogAdapter();

  inline log4tango::Logger* get_logger (void) {
    return logger_;
  }

private:
  log4tango::Logger* logger_;
};

} // namespace Tango

#endif // TANGO_HAS_LOG4TANGO

#endif // _LOG4TANGO_H_


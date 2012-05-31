static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         tangoappender.cpp
//
// description :
//
// project :      TANGO
//
// author(s) :    N.Leclercq - SOLEIL
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//
// $Revision$
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#include <iomanip>

#ifdef TANGO_HAS_LOG4TANGO

#include <tangoappender.h>

#define USE_ASYNC_CALL

namespace Tango
{
  TangoAppender::TangoAppender (const std::string& src_name,
                                const std::string& name,
                                const std::string& dev_name,
                                bool open_connection)
    : log4tango::Appender(name),
      _dev_name(dev_name),
      _src_name(src_name),
      _dev_proxy(0)
  {
    if (open_connection == true)
      reopen();
  }

  TangoAppender::~TangoAppender ()
  {
    close();
  }

  bool TangoAppender::requires_layout (void) const
  {
    return false;
  }

  void TangoAppender::set_layout (log4tango::Layout*)
  {
    // no-op
  }

  bool TangoAppender::is_valid (void) const
  {
    if (!_dev_proxy) {
      return false;
    }
    try {
       _dev_proxy->ping();
    } catch (...) {
      return false;
    }
    return true;
  }

  int TangoAppender::_append (const log4tango::LoggingEvent& event)
  {
    //------------------------------------------------------------
    //- DO NOT LOG FROM THIS METHOD !!!
    //------------------------------------------------------------
    if (!_dev_proxy) {
      //--DO NOT RETURN -1 (ERROR ALREADY HANDLED)
      return 0;
    }
    try {
      Tango::DevVarStringArray *dvsa = new Tango::DevVarStringArray(6);
      if (dvsa) {
        dvsa->length(6);
        double ts_ms = 1000. * event.timestamp.get_seconds();
        ts_ms += event.timestamp.get_milliseconds();
        TangoSys_OMemStream ts_ms_str;

        ts_ms_str << std::fixed
                  << std::noshowpoint
                  << std::setprecision(0)
                  << ts_ms
                  << ends;
        string st = ts_ms_str.str();
        (*dvsa)[0] = CORBA::string_dup(st.c_str());

        (*dvsa)[1] = CORBA::string_dup(log4tango::Level::get_name(event.level).c_str());
        (*dvsa)[2] = CORBA::string_dup(event.logger_name.c_str());
        (*dvsa)[3] = CORBA::string_dup(event.message.c_str());
        (*dvsa)[4] = CORBA::string_dup("");
        omni_thread* ct = omni_thread::self();
        if (ct) {
          TangoSys_OMemStream ctstr;
          ctstr << "@" << hex << event.thread_id << " [" << ct->id() << "]"<< ends;

          string st = ctstr.str();
          (*dvsa)[5] = CORBA::string_dup(st.c_str());
        } else {
          (*dvsa)[5] = CORBA::string_dup("unknown");
        }
        DeviceData argin;
        argin << dvsa;
#ifdef USE_ASYNC_CALL
        _dev_proxy->command_inout_asynch("Log", argin, true);
#else
        _dev_proxy->command_inout("Log", argin);
#endif
      }
    }
    catch (...) {
      close();
      return -1;
    }
    return 0;
  }

  bool TangoAppender::reopen (void)
  {
    bool result = true;
    try {
      close();
      _dev_proxy = new DeviceProxy(const_cast<std::string&>(_dev_name));
      try {
        DeviceData argin;
        argin << const_cast<std::string&>(_src_name);
#ifdef USE_ASYNC_CALL
        _dev_proxy->command_inout_asynch("Register", argin, true);
#else
       _dev_proxy->command_inout("Register", argin);
#endif
      }
      catch (...) {

      }
    }
    catch (...) {
      close();
      result = false;
    }
    return result;
  }

  void TangoAppender::close (void)
  {
    if (_dev_proxy) {
      try {
        DeviceData argin;
        argin << const_cast<std::string&>(_src_name);
#ifdef USE_ASYNC_CALL
        _dev_proxy->command_inout_asynch("UnRegister", argin, true);
#else
        _dev_proxy->command_inout("UnRegister", argin);
#endif
      }
      catch (...) {
        // Ignore error: some old logviewer may not support UnRegister
      }
      delete _dev_proxy;
      _dev_proxy = 0;
    }
  }

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO

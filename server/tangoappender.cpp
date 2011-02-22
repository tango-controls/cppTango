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
// $Revision$
//
// $Log$
// Revision 3.1  2003/04/03 10:35:04  nleclercq
// Few changes to test asynch logging
//
// Revision 3.0  2003/03/25 16:47:50  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.11  2003/03/20 10:41:14  nleclercq
// Final touch on bug fix (reentry pb)
//
// Revision 2.10  2003/03/19 17:52:34  nleclercq
// Few minor modifications
//
// Revision 2.9  2003/03/19 09:18:09  nleclercq
// *** empty log message ***
//
// Revision 2.8  2003/03/18 18:14:10  nleclercq
// Fixed reentry pb in log4tango::Logger::call_appenders (bug finally found)
//
// Revision 2.7  2003/03/18 14:51:09  nleclercq
// Added async call support
//
// Revision 2.6  2003/03/17 12:46:02  nleclercq
// Fixed #include order
//
// Revision 2.5  2003/03/17 08:51:16  taurel
// Adapt new error management to old Streams still used with gcc 2.95
//
// Revision 2.4  2003/03/14 15:58:52  nleclercq
// Changed error management
//
// Revision 2.3  2003/03/13 15:17:50  nleclercq
// Minor modifications on logging implementation
//
// Revision 2.2  2003/03/11 17:55:57  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:45  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
//
// copyleft :     European Synchrotron Radiation Facility
//                BP 220, Grenoble 38043
//                FRANCE
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
#include <iomanip>
#else
#include <iomanip.h>
#endif

#ifdef TANGO_HAS_LOG4TANGO

#include <tangoappender.h>

//#define USE_ASYNC_CALL

namespace Tango
{
  TangoAppender::TangoAppender (const std::string& src_name,
                                const std::string& name,
                                const std::string& dev_name,
                                bool open_connection)
    : log4tango::Appender(name), 
      _src_name(src_name), 
      _dev_name(dev_name), 
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
        ts_ms_str << std::fixed 
                  << std::noshowpoint 
                  << std::setprecision(0) 
                  << ts_ms 
                  << ends;
        (*dvsa)[0] = CORBA::string_dup(ts_ms_str.str().c_str());          
#else
        ts_ms_str.unsetf(ios::showpoint);
	      ts_ms_str.setf(ios::fixed);
        ts_ms_str << setprecision(0) 
                  << ts_ms 
                  << ends;
        (*dvsa)[0] = CORBA::string_dup(ts_ms_str.str()); 
#endif
        (*dvsa)[1] = CORBA::string_dup(log4tango::Level::get_name(event.level).c_str());
        (*dvsa)[2] = CORBA::string_dup(event.logger_name.c_str());
        (*dvsa)[3] = CORBA::string_dup(event.message.c_str());
        (*dvsa)[4] = CORBA::string_dup("");
        omni_thread* ct = omni_thread::self();
        if (ct) {
          TangoSys_OMemStream ctstr;
          ctstr << "@" << hex << event.thread_id << " [" << ct->id() << "]"<< ends;
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
          (*dvsa)[5] = CORBA::string_dup(ctstr.str().c_str());          
#else
          (*dvsa)[5] = CORBA::string_dup(ctstr.str()); 
#endif
        } else {
          (*dvsa)[5] = CORBA::string_dup("unknown");
        }
        DeviceData argin;
        argin << dvsa;
#ifdef USE_ASYNC_CALL 
        _dev_proxy->command_inout_asynch("Log", argin, false);
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
      delete _dev_proxy;
      _dev_proxy = 0;
    }
  }

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO

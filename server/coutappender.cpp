static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         coutappender.cpp
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
// Revision 3.0  2003/03/25 16:41:57  taurel
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
// Revision 2.2  2003/03/11 17:55:48  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:39  taurel
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

#if defined(TANGO_HAS_LOG4TANGO)

#include <coutappender.h>

namespace Tango
{
  CoutAppender::CoutAppender (const std::string& name)
    : log4tango::LayoutAppender(name) 
  {
    //no-op ctor
  }

  CoutAppender::~CoutAppender ()
  {
    //no-op dtor
  }

  int CoutAppender::_append (const log4tango::LoggingEvent& event)
  {
#ifdef WIN32
    CoutBuf *dbg_win; 
    try {
      dbg_win = Util::instance(false)->get_debug_object(); 
    } catch (...) {
      dbg_win = 0;
    }
    if (dbg_win)
      dbg_win->dbg_out(get_layout().format(event).c_str());
    else 
#endif
      ::printf("%s\n", get_layout().format(event).c_str());
    return 0;
  } 

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO

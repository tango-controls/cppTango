static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         tangorollingfileappender.cpp
//
// description :  Implementation of the DServer logging oriented commands
//
// project :      TANGO
//
// author(s) :    N.Leclercq - SOLEIL
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/03/25 16:48:04  taurel
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
// Revision 2.2  2003/03/11 17:55:58  nleclercq
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

#ifdef TANGO_HAS_LOG4TANGO

#include <tangorollingfileappender.h>

namespace Tango
{
  TangoRollingFileAppender::TangoRollingFileAppender(const std::string& name, 
                                                     const std::string& fileName,
                                                     size_t maxFileSize)
    : log4tango::RollingFileAppender(name, fileName, maxFileSize * 1024) 
  {
    // no-op
  }

  TangoRollingFileAppender::~TangoRollingFileAppender()
  {
    // no-op
  }
      
  bool TangoRollingFileAppender::isValid (void) const
  {
    return (_fd < 0) ? false : true;
  }

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO

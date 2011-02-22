static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         DServerLog.cpp
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
// Revision 3.1  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.0  2003/03/25 16:43:20  taurel
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
// Revision 2.2  2003/03/11 17:55:53  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:41  taurel
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

namespace Tango
{
//+----------------------------------------------------------------------------
//
// method : DServer::add_logging_target
//
//-----------------------------------------------------------------------------
void DServer::add_logging_target (const Tango::DevVarStringArray *argin)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::add_logging_target" << endl;
  Logging::add_logging_target(argin);
  cout4 << "Leaving DServer::add_logging_target" << endl;
}

//+----------------------------------------------------------------------------
//
// method : DServer::remove_logging_target
//
//-----------------------------------------------------------------------------
void DServer::remove_logging_target (const Tango::DevVarStringArray *argin)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::remove_logging_target" << endl;
  Logging::remove_logging_target(argin);
  cout4 << "Leaving DServer::remove_logging_target" << endl;
}

//+----------------------------------------------------------------------------
//
// method : DServer::get_logging_target
//
//-----------------------------------------------------------------------------
Tango::DevVarStringArray* DServer::get_logging_target (const std::string& dev_name)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::get_logging_target" << endl;
  DevVarStringArray* res = Logging::get_logging_target(dev_name);
  cout4 << "Leaving DServer::get_logging_target" << endl;
  return res;
}

//+----------------------------------------------------------------------------
//
// method : DServer::set_logging_level
//
//-----------------------------------------------------------------------------
void DServer::set_logging_level (const Tango::DevVarLongStringArray *argin)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::set_logging_level" << endl;
  Logging::set_logging_level(argin);
  cout4 << "Leaving DServer::set_logging_level" << endl;
}

//+----------------------------------------------------------------------------
//
// method : DServer::get_logging_level
//
//-----------------------------------------------------------------------------
DevVarLongStringArray* DServer::get_logging_level (const DevVarStringArray *argin)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::get_logging_level" << endl;
  DevVarLongStringArray* res = Logging::get_logging_level(argin);
  cout4 << "Leaving DServer::get_logging_level" << endl;
  return res;
}

//+----------------------------------------------------------------------------
//
// method : DServer::stop_logging
//
//-----------------------------------------------------------------------------
void  DServer::stop_logging (void)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::stop_logging" << endl;
  Logging::stop_logging();
  cout4 << "Leaving DServer::stop_logging" << endl;
}

//+----------------------------------------------------------------------------
//
// method : DServer::start_logging
//
//-----------------------------------------------------------------------------
void  DServer::start_logging (void)
{
  NoSyncModelTangoMonitor mon(this);
  cout4 << "Entering DServer::start_logging" << endl;
  Logging::start_logging();
  cout4 << "Leaving DServer::start_logging" << endl;
}


} // End of Tango namespace

#endif // TANGO_HAS_LOG4TANGO

//+=============================================================================
//
// file :	  DeviceLog.cpp
//
// description :  Logging oriented methods of the DeviceImpl class
//
// project :	  TANGO
//
// author(s) :	  N.Leclercq - SOLEIL
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
// Revision 3.14  2010/09/09 13:45:22  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.13  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.12  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.11  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.10  2008/03/11 14:38:25  taurel
// - Apply patches from Frederic Picca about compilation with gcc 4.2
//
// Revision 3.9  2007/10/26 11:30:35  taurel
// - Set admin device logging level if command line -v > 4
//
// Revision 3.8  2007/10/17 13:43:24  taurel
// - Admin device default logging level set to OFF
//
// Revision 3.7  2006/03/27 17:00:46  jensmeyer
// moved Makefile up
//
// Revision 3.6  2006/03/02 08:53:45  taurel
// - Reset the changes made between 3.4 and 3.5 for VC7 because it makes library
// logging messages invisible
//
// Revision 3.5  2006/02/17 16:54:11  jensmeyer
// Corrections when porting to VC7 under windows
//
// Revision 3.4  2005/07/28 07:34:41  taurel
// - Fix some incompatibility between 5.1 and 5.2
//
// Revision 3.3  2005/07/04 15:33:30  nleclercq
// Added command line logging level 5 for TANGO core debugging
//
// Revision 3.2  2005/02/25 13:28:51  nleclercq
// Added logging support in 'const' methods
//
// Revision 3.1  2003/05/28 14:55:09  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.0  2003/03/25 16:42:00  taurel
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
// Revision 2.3  2003/03/13 15:17:50  nleclercq
// Minor modifications on logging implementation
//
// Revision 2.2  2003/03/11 17:55:52  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:40  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


#include <logging.h>

namespace Tango
{

//+-------------------------------------------------------------------------
// method : DeviceImpl::get_logger_i
//--------------------------------------------------------------------------
log4tango::Logger* DeviceImpl::get_logger_i (void)
{
  try {
    // trace
    cout4 << "Entering DeviceImpl::get_logger_i" << std::endl;
    // instantiate the logger (
    // shame on me for a such huggly impl. but polymorphism
    // can't be used here !
    if (logger == 0) {
      if (device_class->get_name() == "DServer") {
        logger = Logging::get_core_logger();
      }
      else {
        // get device name
        std::string dev_name(device_name);
        // avoid case sensitive troubles
        std::transform(dev_name.begin(),
                       dev_name.end(),
                       dev_name.begin(),
                       ::tolower);
        // instantiate the logger using device name
        logger = new log4tango::Logger(dev_name);
        if (logger == 0) {
          logger = Logging::get_core_logger();
        }
        // set default level
        logger->set_level(log4tango::Level::WARN);
        // save current level
        saved_log_level = log4tango::Level::WARN;
      }
    }
    // trace
    cout4 << "Leaving DeviceImpl::get_logger_i" << std::endl;
  } catch (...) {
    // save our souls...
   logger = Logging::get_core_logger();
  }
  return logger;
}

//+-------------------------------------------------------------------------
// method : DeviceImpl::init_logger
//--------------------------------------------------------------------------
void DeviceImpl::init_logger (void)
{
  try {
    // trace
    cout4 << "Entering DeviceImpl::init_logger" << std::endl;
    // get Tango::Util instance
    Tango::Util *tg = Tango::Util::instance();
    // get cmd line logging level then ...
    int trace_level = tg->get_trace_level();
    // ... convert it to log4tango level
    log4tango::Level::Value cmd_line_level = log4tango::Level::OFF;
    if (trace_level > 4)
    	cmd_line_level = log4tango::Level::DEBUG;
    bool level_set_from_cmd_line = true;
    // are we initializing the dserver's logger
    log4tango::Logger* the_logger = get_logger();
    if (the_logger != Logging::get_core_logger())
    {
      // does the logging level set from cmd line?
      if (trace_level <= 0) {
        level_set_from_cmd_line = false;
        cmd_line_level = log4tango::Level::OFF;
      } else if (trace_level <= 2) {
        cmd_line_level = log4tango::Level::INFO;
      } else {
        cmd_line_level = log4tango::Level::DEBUG;
      }
      // add a console target if logging level set from cmd line
      if (level_set_from_cmd_line) {
        // add a console target if logging level set from cmd line
        Logging::add_logging_target(the_logger, kLogTargetConsole, 0);
      }
    }
    if (tg->_UseDb == false) {
      // done if we are not using the database
      if (level_set_from_cmd_line)
        the_logger->set_level(cmd_line_level);
      cout4 << "Leaving DeviceImpl::init_logger" << std::endl;
      return;
    }
    // get both logging level and targets from database
    DbData db_data;
    db_data.push_back(DbDatum("logging_level"));
    db_data.push_back(DbDatum("logging_target"));
    db_data.push_back(DbDatum("logging_rft"));
    try {
      db_dev->get_property(db_data);
    } catch (...) {
      // error: set logging level then return
      the_logger->set_level(cmd_line_level);
      return;
    }
    // set logging level (if not set from cmd line)
    std::string log_level_property;
    if (!level_set_from_cmd_line && db_data[0].is_empty() == false) {
      db_data[0] >> log_level_property;
      // avoid case sensitive troubles
      std::transform(log_level_property.begin(), log_level_property.end(),
                     log_level_property.begin(), ::toupper);
      cout4 << "Initial logging level set to [" << log_level_property << "]" << std::endl;
      // convert from string to log4tango level
      log4tango::Level::Value log4tango_level = log4tango::Level::WARN;
      try {
          log4tango_level = Logging::tango_to_log4tango_level(log_level_property, false);
      } catch (...) {
         // ignore exception
      }
      // set logger's level (from property)
      the_logger->set_level(log4tango_level);
    }
    else {
      // set logger's level (from cmd line)
		if (the_logger != Logging::get_core_logger())
      		the_logger->set_level(cmd_line_level);
    }
    // save current logging level
    saved_log_level = the_logger->get_level();
    // get rolling threshold for file targets
    long rft_property = static_cast<long>(kDefaultRollingThreshold);
    if (db_data[2].is_empty() == false) {
      db_data[2] >> rft_property;
    }
    // save current rolling threshold
    rft = static_cast<size_t>(rft_property);
    // set logging targets
    std::vector<std::string> log_target_property;
    if (db_data[1].is_empty() == false) {
      db_data[1] >> log_target_property;
      // attach targets to logger
      for (unsigned int i = 0; i < log_target_property.size(); i++) {
              Logging::add_logging_target(the_logger, log_target_property[i], 0);
      }
    }
    // set rolling file threshold for file targets
    Logging::set_rolling_file_threshold(the_logger, rft);
    // trace
    cout4 << "Leaving DeviceImpl::init_logger" << std::endl;
  }
  catch (...) {
    // igore any exception
  }
}

//+-------------------------------------------------------------------------
// method : DeviceImpl::start_logging
//--------------------------------------------------------------------------
void DeviceImpl::start_logging (void) {
  get_logger()->set_level(saved_log_level);
}

//+-------------------------------------------------------------------------
// method : DeviceImpl::stop_logging
//--------------------------------------------------------------------------
void DeviceImpl::stop_logging (void) {
  saved_log_level = get_logger()->get_level();
  get_logger()->set_level(log4tango::Level::OFF);
}

} // namespace Tango

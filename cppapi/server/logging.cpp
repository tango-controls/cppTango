//+=============================================================================
//
// file :   Logging.cpp
//
// description :  TLS helper class
//
// project :  TANGO
//
// author(s) :  N.Leclercq - SOLEIL
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


//-----------------------------------------------------------------------------
// HEADERS
//-----------------------------------------------------------------------------
#ifdef _TG_WINDOWS_
# include <direct.h>
#else
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <time.h>
# include <pwd.h>
#endif

#include <coutappender.h>
#include <tangoappender.h>
#include <tangorollingfileappender.h>

namespace Tango
{

//-----------------------------------------------------------------------------
// TANGO KERNEL CMD LINE DEBUGGING LEVEL
//-----------------------------------------------------------------------------
#define kTANGO_CORE_CMD_LINE_LEVEL 5

//-----------------------------------------------------------------------------
// LOCAL LOG MACRO (CAN'T USE LOGGING STUFFS DURING LOGGING STUFFS INIT STAGE!)
//-----------------------------------------------------------------------------
#define _VERBOSE(_MSG_) \
  if (Logging::_cmd_line_level >= kTANGO_CORE_CMD_LINE_LEVEL) \
    ::printf _MSG_;

//-----------------------------------------------------------------------------
// LOCAL CONSTs (should be a Logging static const member)
//-----------------------------------------------------------------------------
static const char* kDefaultTargetName  = "default";
static const char* kDefaultConsoleName = "cout";

//-----------------------------------------------------------------------------
// STATIC MEMBERS
//-----------------------------------------------------------------------------
// the DServer logger (use to output TANGO low level messages)
log4tango::Logger* _core_logger = 0;
// the logging path (use to store file logging targets)
std::string Logging::_log_path("");
// the <rollover> threshold for RollingFileAppender
size_t Logging::_rft = 0;
// the cmd line verbose level
int Logging::_cmd_line_level = 0;

//+----------------------------------------------------------------------------
// method :     Logging::init()
//-----------------------------------------------------------------------------
void Logging::init (const std::string& ds_name, // dserver name
                    int cmd_line_level,         // cmd. line verbose level
                    bool use_db,                // true if using the TANGO-db
                    Database &db,               // Db object or null
                    Util *tg)					// Tango::Util object
{
  // logging path env. var. name
  const char*  kTangoLogPathVar = "TANGO_LOG_PATH";
  // default logging path
#ifdef _TG_WINDOWS_
  const char* kDefaultTangoLogPath = "c:/tango";
#else
  const char* kDefaultTangoLogPath = "/tmp/tango";
#endif
  try {
    // store cmd line verbose level (set by user)
    Logging::_cmd_line_level = cmd_line_level;
    _VERBOSE(("Entering Logging::init\n"));
    // set default <rollover> threshold for RollingFileAppender
    Logging::_rft = kDefaultRollingThreshold;
    // try to get logging path from kTangoLogPathVar env. var.
    const int log_path_lookup_status = ApiUtil::get_env_var(kTangoLogPathVar, Logging::_log_path);
    if (log_path_lookup_status != 0 || Logging::_log_path.empty()) {
      // log path not set or empty, use default path
      Logging::_log_path = kDefaultTangoLogPath;
#ifndef _TG_WINDOWS_
      struct passwd* pw = getpwuid(getuid());
      if (pw) {
          Logging::_log_path = Logging::_log_path + '-' + pw->pw_name;
      }
#else
      BOOL ret;
      TCHAR buffer[128];
      DWORD nb = 128;
      ret = GetUserName(buffer,&nb);
      if (ret == 0){
          Logging::_log_path = Logging::_log_path + '-' + buffer;
      }
#endif
    }
    _VERBOSE(("\tTANGO_LOG_PATH is %s\n", Logging::_log_path.c_str()));
    // build logger name from dserver name
    std::string dserver_dev_name("dserver/" + ds_name);
    // instantiate the logger
    log4tango::Logger* logger = new log4tango::Logger(dserver_dev_name);
    // is logging level set from cmd line?
    bool level_set_from_cmd_line = (cmd_line_level >= kTANGO_CORE_CMD_LINE_LEVEL)
                                 ? true
                                 : false;
    _VERBOSE(("\tcmd line logging level is %d\n", cmd_line_level));
    // set logger's effective level
    if (level_set_from_cmd_line) {
      logger->set_level(log4tango::Level::DEBUG);
    }
    else {
      logger->set_level(log4tango::Level::ERROR);
    }
    // core-logger's targets
    std::vector<std::string> targets;
    if (use_db == true)
    {
      // get logging properties from database
      try
      {
        DbData db_data;
        // the logging path property (overwrites env.var.)
        db_data.push_back(DbDatum("logging_path"));
        // the core-logger's rolling file threshold
        db_data.push_back(DbDatum("logging_rft"));
        // the core-logger's logging level
        db_data.push_back(DbDatum("logging_level"));
        // the core-logger's logging target list
        db_data.push_back(DbDatum("logging_target"));
        // get properties from TANGO-db
        db.get_device_property(dserver_dev_name, db_data,tg->get_db_cache());
        // set logging path
        std::string level_str("WARN");
        if (db_data[0].is_empty() == false) {
          db_data[0] >> Logging::_log_path;
          _VERBOSE(("\tLogging::_log_path is %s (set from db)\n",
          Logging::_log_path.c_str()));
        }
        Logging::_rft = kDefaultRollingThreshold;
        // set rolling file threshold
        if (db_data[1].is_empty() == false) {
          unsigned long rtf = 0;
          db_data[1] >> rtf;
          Logging::_rft = static_cast<size_t>(rtf);
          if (rtf <  kMinRollingThreshold) {
            Logging::_rft = kMinRollingThreshold;
          }
          else if (rtf > kMaxRollingThreshold) {
            Logging::_rft = kMaxRollingThreshold;
          }
#ifdef TANGO_LONG64
          _VERBOSE(("\tRolling file threshold is %lu Kb\n", Logging::_rft));
#else
		  _VERBOSE(("\tRolling file threshold is %d Kb\n", Logging::_rft));
#endif
        }
        // set logging level (if not set from cmd line)
        if (! level_set_from_cmd_line)
        {
          std::string level_str("WARN");
          if (db_data[2].is_empty() == false)
            db_data[2] >> level_str;
          _VERBOSE(("\tproperty::level is %s\n", level_str.c_str()));
          log4tango::Level::Value level = Logging::tango_to_log4tango_level(level_str, false);
          _VERBOSE(("\teffective property::level is %s\n", log4tango::Level::get_name(level).c_str()));
          logger->set_level(level);
        }
        // get logging targets
        if (db_data[3].is_empty() == false)
          db_data[3] >> targets;
      } catch (...) {
        _VERBOSE(("\texception caught while handling logging properties\n"));
        // ignore any exception
      }
    }
    // create this <process> logging dir
    Logging::_log_path += "/" + ds_name;
    int res = Logging::create_log_dir(Logging::_log_path);
    _VERBOSE(("\tLogging::create_log_dir(%s) returned %d\n",
              Logging::_log_path.c_str(), res));
    // be sure there is a console target if level set from cmd line
    if (level_set_from_cmd_line) {
      Logging::add_logging_target(logger, kLogTargetConsole, 0);
      _VERBOSE(("\tadded console target (logging level set from cmd line)\n"));
    }
    for (unsigned int i = 0; i < targets.size(); i++) {
      _VERBOSE(("\tadding property::targets[%d]=%s\n", i, targets[i].c_str()));
      Logging::add_logging_target(logger, targets[i], 0);
    }
    Tango::_core_logger = logger;
    _VERBOSE(("Leaving Logging::init\n"));
  }
  catch (...) {
    _VERBOSE(("\tUnknown exception caught\n"));
    // ignore exception
  }
}

//+----------------------------------------------------------------------------
// method : Logging::cleanup
//-----------------------------------------------------------------------------
void Logging::cleanup (void)
{
    delete Tango::_core_logger;
    Tango::_core_logger = 0;
}

//+----------------------------------------------------------------------------
// method : Logging::add_logging_target
//-----------------------------------------------------------------------------
void Logging::add_logging_target (const Tango::DevVarStringArray *argin)
{
  try {
    // fight against the "zombie appender" syndrom
    Logging::kill_zombie_appenders();
    // trace
    cout4 << "Entering Logging::add_logging_target" << std::endl;
    // log input
    unsigned int i;
    for (i = 0; i < argin->length(); i++)  {
      cout4 << "input string = " << (*argin)[i].in() << std::endl;
    }
    // N x [device-name, target-type::target-name] expected
    // The length of the input sequence must be a multiple of 2
    if ((argin->length() % 2) != 0) {
      Except::throw_exception((const char *)API_MethodArgument,
                              (const char *)"Incorrect number of inout arguments",
                              (const char *)"Logging::add_logging_target");
    }
    // device name pattern
    std::string pattern;
    // the device list (devices which name matches pattern)
    std::vector<DeviceImpl*> dl;
    // for each tuple{dev_name, target_type, target_name} in argin
    for (i = 0; i < argin->length(); i += 2) {
      // get device name pattern;
      pattern = (*argin)[i];
      // convert to lower case
      std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
      // get devices which name matches the pattern pattern
      dl = Util::instance()->get_device_list(pattern);
      // throw an exception if the list is empty
      if (dl.empty()) {
        TangoSys_OMemStream o;
        o << "No device name matching pattern  <" << pattern << ">" << std::ends;
        Except::throw_exception((const char *)API_DeviceNotFound,o.str(),
                                (const char *)"Logging::add_logging_target");
      }
	  // Check that none of the concerned device(s) is locked by another client
	  DServer *adm_dev = Util::instance()->get_dserver_device();
	  for (unsigned int j = 0; j < dl.size(); j++) {
		adm_dev->check_lock_owner(dl[j],"add_logging_target",(dl[j]->get_name()).c_str());
	  }
      // for each device matching pattern...
      for (unsigned int j = 0; j < dl.size(); j++) {
        // ...add logging target
        Logging::add_logging_target(dl[j]->get_logger(), std::string((*argin)[i + 1]));
      }
    } // for each tuple
    // trace
    cout4 << "Leaving Logging::add_logging_target" << std::endl;
  }
  catch (std::exception& e) {
    TangoSys_OMemStream o;
    o << "std::exception caught [" << e.what() << "]" << std::ends;
    Except::throw_exception((const char *)API_InternalError, o.str(),
                            (const char *)"Logging::add_logging_target");
  }
}

//+-------------------------------------------------------------------------
// method : Logging::add_logging_target
//--------------------------------------------------------------------------
void Logging::add_logging_target(log4tango::Logger* logger,
                                 const std::string& tg_type,
                                 const std::string& tg_name,
                                 int throw_exception)
{
  std::string tg_type_name(tg_type);
  tg_type_name += kLogTargetSep + tg_name;
  Logging::add_logging_target(logger, tg_type_name, throw_exception);
}

//+-------------------------------------------------------------------------
// method : Logging::add_logging_target
//--------------------------------------------------------------------------
void Logging::add_logging_target(log4tango::Logger* logger,
                                 const std::string& in_type_name,
                                 int throw_exception)
{
  try {
    // trace
    cout4 << "Entering Logging::add_logging_target (internal impl)" << std::endl;
    // check input
    if (!logger) {
      //--TODO::better error handler needed?
      cout4 << "internal error (logger is null)" << std::endl;
      return;
    }
    // target type (as string)
    std::string ltg_type_str;
    // target name
    std::string ltg_name_str;
    // avoid case sensitive troubles
    std::string type_name(in_type_name);
    std::transform(type_name.begin(), type_name.end(), type_name.begin(), ::tolower);
    // split <in_type_name> into <type> and <name>
    Logging::get_target_type_and_name(type_name, ltg_type_str, ltg_name_str);
    // target type (as int)
    int ltg_type;
    if (ltg_type_str == kLogTargetConsole) {
      ltg_type = LOG_CONSOLE;
      // force ltg_name_str to kDefaultConsoleName
      ltg_name_str = kDefaultConsoleName;
    }
    else if (ltg_type_str == kLogTargetFile) {
      ltg_type = LOG_FILE;
    }
    else if (ltg_type_str == kLogTargetDevice) {
      ltg_type = LOG_DEVICE;
    }
    else {
      if (throw_exception) {
        TangoSys_OMemStream o;
        o << "Invalid logging target type specified (" << ltg_type_str << ")" << std::ends;
        Except::throw_exception((const char *)API_MethodArgument, o.str(),
                                (const char *)"DeviceImpl::add_logging_target");
      }
      return;
    }
    // build full path name (for file targets)
    std::string full_file_name;
    // build internal target name  (i.e. appender name)
    std::string appender_name(ltg_type_str + kLogTargetSep);
    switch (ltg_type) {
      case LOG_CONSOLE: {
        appender_name += kDefaultConsoleName;
      } break;
      case LOG_FILE: {
        if (ltg_name_str == kDefaultTargetName) {
          // use default path and file name
          full_file_name  = Logging::_log_path + "/";
          full_file_name += Logging::dev_to_file_name(logger->get_name()) + ".log";
        } else if (ltg_name_str.find('/') !=  std::string::npos) {
          // user specified a "custom" path and file name
          full_file_name = ltg_name_str;
        } else {
          // user specified a "custom" file name
          full_file_name  = Logging::_log_path + "/" + ltg_name_str;
        }
        ltg_name_str = full_file_name;
        appender_name += ltg_name_str;
      } break;
      case LOG_DEVICE: {
        if (ltg_name_str == kDefaultTargetName) {
          if (throw_exception) {
            TangoSys_OMemStream o;
            o << "Device target name must be specified (no default value)" << std::ends;
            Except::throw_exception((const char *)API_MethodArgument, o.str(),
                                    (const char *)"DeviceImpl::add_logging_target");
          }
          return;
        }
        appender_name += ltg_name_str;
      } break;
    } //  switch (ltg_type)
    // attach the appender to the logger if not already attached
    log4tango::Appender* appender = logger->get_appender(appender_name);
    if (!appender) {
      cout4 << "Adding logging target " << appender_name << " to " << logger->get_name() << std::endl;
      // instantiate the appender (i.e. the target) and the layout (if needed)
      switch (ltg_type) {
        case LOG_CONSOLE: {
          appender = new CoutAppender(appender_name);
          if (appender == 0) {
            cout4 << "Internal error (Appender instantiation failed)" << std::endl;
            if (throw_exception) {
            TangoSys_OMemStream o;
            o << "Out of memory error" << std::ends;
            Except::throw_exception((const char *)API_MemoryAllocation, o.str(),
                                    (const char *)"DeviceImpl::add_logging_target");
            }
            break;
          }
        } break; // case LOG_CONSOLE
        case LOG_FILE: {
          appender = new TangoRollingFileAppender(appender_name, full_file_name, Logging::_rft);
          if (appender == 0) {
            if (throw_exception) {
              TangoSys_OMemStream o;
              o << "Out of memory error" << std::ends;
              Except::throw_exception((const char *)API_MemoryAllocation, o.str(),
                                      (const char *)"DeviceImpl::add_logging_target");
            }
            break;
          }
          if (appender->is_valid() == false) {
            delete appender;
            appender = 0;
            if (throw_exception) {
              TangoSys_OMemStream o;
              o << "Could not open logging file " << full_file_name << std::ends;
              Except::throw_exception((const char *)API_CannotOpenFile, o.str(),
                                      (const char *)"DeviceImpl::add_logging_target");
            }
            break;
          }
          log4tango::XMLLayout *layout = new log4tango::XMLLayout();
          if (layout == 0) {
            delete appender;
            appender = 0;
            if (throw_exception) {
              TangoSys_OMemStream o;
              o << "Out of memory error" << std::ends;
              Except::throw_exception((const char *)API_MemoryAllocation, o.str(),
                                      (const char *)"DeviceImpl::add_logging_target");
            }
            break;
          }
          appender->set_layout(layout);
        } break;  // case LOG_FILE
        case LOG_DEVICE: {
          appender = new TangoAppender(logger->get_name(), appender_name, ltg_name_str);
          if (appender == 0) {
            if (throw_exception) {
              TangoSys_OMemStream o;
              o << "Out of memory error" << std::ends;
              Except::throw_exception((const char *)API_MemoryAllocation, o.str(),
                                      (const char *)"DeviceImpl::add_logging_target");
            }
            break;
          }
          if (appender->is_valid() == false) {
            delete appender;
            appender = 0;
            if (throw_exception) {
              TangoSys_OMemStream o;
              o << "Could not connect to log consumer " << ltg_name_str << std::ends;
              Except::throw_exception((const char *)API_ConnectionFailed, o.str(),
                                      (const char *)"DeviceImpl::add_logging_target");
            }
            break;
          }
        } break; // case LOG_DEVICE
      }
      // attach the appender to the logger
      if (appender) {
        logger->add_appender(appender);
      }
    }
    else {
      cout4 << "Target " << appender_name
            << " is already attached to "
            << logger->get_name() << std::endl;
    }
    cout4 << "Leaving Logging::add_logging_target (internal impl)" << std::endl;
  }
  catch (std::exception& e) {
    if (throw_exception) {
      TangoSys_OMemStream o;
      o << "std::exception caught [" << e.what() << "]" << std::ends;
      Except::throw_exception((const char *)API_StdException,
                              o.str(),
                              (const char *)"Logging::add_logging_target");
    }
  }
}

//+----------------------------------------------------------------------------
// method : Logging::remove_logging_target
//-----------------------------------------------------------------------------
void Logging::remove_logging_target (const Tango::DevVarStringArray *argin)
{
  try {
    // fight against the "zombie appender" syndrom
    Logging::kill_zombie_appenders();
    // trace
    cout4 << "Entering Logging::remove_logging_target" << std::endl;
    // log input
    unsigned int i;
    for (i = 0; i < argin->length(); i++) {
      cout4 << "Input string = " << (*argin)[i].in() << std::endl;
    }
    // N x [device-name, target-type, target-name] expected
    // The length of the input sequence must a multiple of 3
    if ((argin->length() % 2) != 0) {
      Except::throw_exception((const char *)API_WrongNumberOfArgs,
                              (const char *)"Incorrect number of inout arguments",
                              (const char *)"Logging::remove_logging_target");
    }
    // a logger
    log4tango::Logger* logger;
    // input arg target_type::target::name as std::string
    std::string type_name;
    // dev_name pattern
    std::string pattern;
    // target type (as int and string)
    int tg_type = 0;
    std::string tg_type_str;
    // target name or pattern
    std::string tg_name;
    // the "remove all targets of type <tg_type_str>" flag
    int remove_all_targets;
    // the device list
    std::vector<DeviceImpl*> dl(0);
    // for each tuple{dev_name, target_type::target_name}
    for (i = 0; i < argin->length();)
    {
      // get device name wildcard;
      pattern = (*argin)[i++];
      // convert to lower case
      std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
      // get devices which name matches the pattern pattern
      dl = Util::instance()->get_device_list(pattern);
      // throw an exception if the list is empty
      if (dl.empty()) {
              TangoSys_OMemStream o;
              o << "No device name matching pattern  <" << pattern << ">" << std::ends;
              Except::throw_exception((const char *)API_DeviceNotFound,o.str(),
                                      (const char *)"Logging::remove_logging_target");
      }
	  // Check that none of the concerned device(s) is locked by another client
	  DServer *adm_dev = Util::instance()->get_dserver_device();
	  for (unsigned int j = 0; j < dl.size(); j++) {
		adm_dev->check_lock_owner(dl[j],"remove_logging_target",(dl[j]->get_name()).c_str());
	  }
      // get target type and name from argin (syntax type::name)
      type_name = (*argin)[i++];
      std::transform(type_name.begin(), type_name.end(), type_name.begin(), ::tolower);
      Logging::get_target_type_and_name(type_name, tg_type_str, tg_name);
      std::transform(tg_type_str.begin(), tg_type_str.end(), tg_type_str.begin(), ::tolower);
      // check target type
      if (tg_type_str == kLogTargetConsole) {
        tg_type = Tango::LOG_CONSOLE;
        // force target name to kDefaultConsoleName
        tg_name = kDefaultConsoleName;
      }
      else if (tg_type_str == kLogTargetFile) {
        tg_type = Tango::LOG_FILE;
      }
      else if (tg_type_str == kLogTargetDevice) {
        tg_type = Tango::LOG_DEVICE;
      }
      else {
        TangoSys_OMemStream o;
        o << "Logging target type <" << tg_type_str << "> not supported" << std::ends;
        Except::throw_exception((const char *)API_MethodArgument, o.str(),
                                (const char *)"Logging::remove_logging_target");
      }
      // do we have to remove all <tg_type_str> targets?
      remove_all_targets = (tg_name == "*") ? 1 : 0;
      // for each device matching pattern
      for (unsigned int j = 0; j < dl.size(); j++) {
        // get device's logger
        logger = dl[j]->get_logger();
        if (logger == 0) {
          TangoSys_OMemStream o;
          o << "Internal error (got a NULL logger)" << std::ends;
          Except::throw_exception((const char *)API_InternalError, o.str(),
                                  (const char *)"Logging::remove_logging_target");
        }
        // CASE I: remove ONE target of type <tg_type_str>
        if (remove_all_targets == 0) {
          // build full appender name (target_type+sep+target_name)
          std::string full_tg_name = tg_type_str + kLogTargetSep;
          // a special case : target is the a file
          if (tg_type == Tango::LOG_FILE) {
            if (tg_name == kDefaultTargetName) {
              // use both default path and file name
              full_tg_name += Logging::_log_path + "/";
              full_tg_name += Logging::dev_to_file_name(logger->get_name()) + ".log";
            } else if (tg_name.find('/') !=  std::string::npos) {
              // user specified a "custom" path and file name
              full_tg_name += tg_name;
            } else {
              // user specified a "custom" file name
              full_tg_name += Logging::_log_path + "/" + tg_name;
            }
          }
          else {
            full_tg_name += tg_name;
          }
          cout4 << "removing target " << full_tg_name
                << " from " << dl[j]->get_name() << std::endl;
          // remove appender from device's logger
          logger->remove_appender(full_tg_name);
        }
        // CASE II: remove ALL targets of type <tg_type_str>
        else {
          cout4 << "removing ALL <" << tg_type_str
                << "> targets from " << dl[j]->get_name() << std::endl;
          // get logger's appender list
          log4tango::AppenderList al = logger->get_all_appenders();
          // find appenders of type <tg_type_str> in <al>
          std::string::size_type idx;
          const std::string substr(tg_type_str + kLogTargetSep);
          // for each appender in <as>
          for (unsigned int a = 0; a < al.size(); a++) {
            idx = al[a]->get_name().find(substr);
            if (idx != std::string::npos) {
              cout4 << "\tremoving " << al[a]->get_name() << std::endl;
              logger->remove_appender(al[a]->get_name());
            }
          } // for each appender in <al>
        } // else (if remove_all_targets)
      } // for each device in <dl>
    } // for each tuple
    // trace
    cout4 << "Leaving Logging::remove_logging_target" << std::endl;
  }
  catch (std::exception& e) {
    TangoSys_OMemStream o;
    o << "std::exception caught [" << e.what() << "]" << std::ends;
    Except::throw_exception((const char *)API_StdException,
                            o.str(),
                            (const char *)"Logging::remove_logging_target");
  }
}

//+----------------------------------------------------------------------------
// method : Logging::get_logging_target
//-----------------------------------------------------------------------------
Tango::DevVarStringArray* Logging::get_logging_target (const std::string& dev_name)
{
  Tango::DevVarStringArray* ret = 0;
  try {
    // fight against the "zombie appender" syndrom
    Logging::kill_zombie_appenders();
    // trace
    cout4 << "Entering Logging::get_logging_target " << std::endl;
    // first check device name (does it exist?)
    DeviceImpl* dev = 0;
    try {
      dev = Tango::Util::instance()->get_device_by_name(dev_name);
    }
    catch (Tango::DevFailed &e) {
      TangoSys_OMemStream o;
      o << "Device " << dev_name << " not found" << std::ends;
      Except::re_throw_exception(e,
                                (const char *)API_DeviceNotFound,
                                o.str(),
                                (const char *)"Logging::get_logging_target");
    }
    // get device's logger
    log4tango::Logger *logger = dev->get_logger();
    if (logger == 0) {
      TangoSys_OMemStream o;
      o << "Could not instantiate logger (out of memory error)" << std::ends;
      Except::throw_exception((const char *)API_MemoryAllocation, o.str(),
                              (const char *)"Logging::get_logging_target");
    }
    // get logger's appender list
    log4tango::AppenderList al = logger->get_all_appenders();
    // instantiate the returned value
    ret = new Tango::DevVarStringArray(al.size());
    if (ret == 0) {
      TangoSys_OMemStream o;
      Except::throw_exception((const char *)API_MemoryAllocation,
                              "Out of memory error (DevVarStringArray allocation failed)",
                              (const char *)"Logging::get_logging_target");
    }
    // set CORBA::sequence size to its max size
    ret->length(al.size());
    // populate the CORBA::sequence
    for (unsigned int i = 0; i != al.size(); i++) {
      cout4 << "\tadding " << al[i]->get_name() << " to the returned target list" << std::endl;
      (*ret)[i] = Tango::string_dup(al[i]->get_name().c_str());
    } // for i
    // trace
    cout4 << "Leaving Logging::get_logging_target " << std::endl;
  }
  catch (std::exception& e) {
    TangoSys_OMemStream o;
    o << "std::exception caught [" << e.what() << "]" << std::ends;
    Except::throw_exception((const char *)API_StdException,
                            o.str(),(const char *)"Logging::get_logging_target");
  }
  return ret;
}

//+----------------------------------------------------------------------------
// method : Logging::set_logging_level
//-----------------------------------------------------------------------------
void Logging::set_logging_level (const DevVarLongStringArray *argin)
{
  try {
    // trace
    cout4 << "Entering Logging::set_logging_level" << std::endl;
    // log input
    unsigned int i;
    for (i = 0; i < argin->svalue.length(); i++)
      cout4 << "Input string = " << argin->svalue[i].in() << std::endl;
    for (i = 0; i < argin->lvalue.length(); i++)
      cout4 << "Input long = " << argin->lvalue[i] << std::endl;
    // check input
    if (argin->svalue.length() != argin->lvalue.length()) {
      Except::throw_exception((const char *)API_IncompatibleCmdArgumentType,
                              (const char *)"Imcompatible command argument type,\
                                    long and string arrays must have the same length",
                              (const char *)"Logging::set_logging_level");
    }
    // the device name wildcard
    std::string pattern;
    // a device list
    std::vector<DeviceImpl*> dl(0);
    // a logger
    log4tango::Logger *logger = 0;
    // for each entry in argin->svalue
    for (i = 0; i < argin->svalue.length(); i++) {
      // check logging level
      if (argin->lvalue[i] < Tango::LOG_OFF || argin->lvalue[i] > Tango::LOG_DEBUG) {
        Except::throw_exception((const char *)API_MethodArgument,
                                (const char *)"Invalid argument for command,\
                                      logging level out of range",
                                (const char *)"Logging::set_logging_level");
      }
      // get ith wilcard
      pattern = argin->svalue[i];
      // convert to lower case
      std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
      // get devices which name matches the pattern pattern
      dl = Util::instance()->get_device_list(pattern);
	  // Check that none of the concerned device(s) is locked by another client
	  DServer *adm_dev = Util::instance()->get_dserver_device();
	  for (unsigned int j = 0; j < dl.size(); j++) {
		adm_dev->check_lock_owner(dl[j],"set_logging_level",(dl[j]->get_name()).c_str());
	  }
      // for each device in dl
      for (unsigned int j = 0; j < dl.size(); j++) {
        // get device's logger (created if does not already exist)
        logger = dl[j]->get_logger();
        if (logger == 0) {
          //--TODO::change the following message
          Except::throw_exception((const char *)API_MemoryAllocation,
                                  "out of memory error",
                                  (const char *)"Logging::set_logging_level");
        }
        // map TANGO level to log4tango level
        log4tango::Level::Value log4tango_level =
            Logging::tango_to_log4tango_level(static_cast<Tango::LogLevel>(argin->lvalue[i]));
        // set logger's level
        logger->set_level(log4tango_level);
      } // for j
    } // for i
    // trace
    cout4 << "Leaving Logging::set_logging_level" << std::endl;
  }
  catch (std::exception& e) {
    TangoSys_OMemStream o;
    o << "std::exception caught [" << e.what() << "]" << std::ends;
    Except::throw_exception((const char *)API_StdException,
                            o.str(), (const char *)"Logging::set_logging_level");
  }
}

//+----------------------------------------------------------------------------
// method : Logging::get_logging_level
//-----------------------------------------------------------------------------
DevVarLongStringArray* Logging::get_logging_level (const DevVarStringArray *argin)
{
  Tango::DevVarLongStringArray *ret = 0;
  try {
    // trace
    cout4 << "Entering Logging::get_logging_level" << std::endl;
    unsigned int i, j;
    for (i = 0; i < argin->length(); i++) {
      cout4 << "Input string = " << (*argin)[i].in() << std::endl;
    }
    // instance the returned CORBA::seq
    ret = new Tango::DevVarLongStringArray;
    if (ret == 0) {
      TangoSys_OMemStream o;
      Except::throw_exception((const char *)API_MemoryAllocation,
                              "out of memory error",
                              (const char *)"Logging::get_logging_level");
    }
    // a TANGO logging level
    Tango::LogLevel tango_level;
    // the device name wildcard
    std::string pattern;
    // a device list
    std::vector<DeviceImpl*> dl(0);
    // a logger
    log4tango::Logger *logger = 0;
    for (i = 0; i < argin->length(); i++) {
      // get ith wilcard
      pattern = (*argin)[i];
      // convert to lower case
      std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
      // get devices which name matches the pattern pattern
      dl = Util::instance()->get_device_list(pattern);
      // for each device in dl
      for (j = 0; j < dl.size(); j++) {
        // get device's logger (created if does not already exist)
        logger = dl[j]->get_logger();
        if (logger == 0) {
        TangoSys_OMemStream o;
        //--TODO: change the following message
        o << "out of memory error" << std::ends;
        Except::throw_exception((const char *)API_MemoryAllocation,
                                "out of memory error",
                                (const char *)"Logging::get_logging_level");
        }
        // map log4tango level to TANGO log level
        tango_level = Logging::log4tango_to_tango_level(logger->get_level());
        // set max size and size of each array in the struct
        ret->lvalue.length(ret->lvalue.length() + 1);
        ret->svalue.length(ret->svalue.length() + 1);
        // populate ret
        ret->svalue[ret->svalue.length() - 1] = Tango::string_dup(dl[j]->get_name().c_str());
        ret->lvalue[ret->lvalue.length() - 1] = tango_level;
      } // for j
    } // for i
    // trace
    cout4 << "Leaving Logging::get_logging_level" << std::endl;
  }
  catch (std::exception& e) {
    TangoSys_OMemStream o;
    o << "std::exception caught [" << e.what() << "]" << std::ends;
    Except::throw_exception((const char *)API_StdException,
                            o.str(),(const char *)"Logging::get_logging_level");
  }
  return ret;
}

//+----------------------------------------------------------------------------
// method : Logging::stop_logging
//-----------------------------------------------------------------------------
void  Logging::stop_logging (void)
{
  cout4 << "Entering Logging::stop_logging" << std::endl;
  // get all devices
  std::vector<DeviceImpl*> dl =
    Util::instance()->get_device_list(std::string("*"));
  for (unsigned int i = 0; i < dl.size(); i++) {
    dl[i]->stop_logging();
  }
  cout4 << "Leaving Logging::stop_logging" << std::endl;
}

//+----------------------------------------------------------------------------
// method : Logging::start_logging
//-----------------------------------------------------------------------------
void  Logging::start_logging (void)
{
  cout4 << "Entering Logging::start_logging" << std::endl;
  // get all devices
  std::vector<DeviceImpl*> dl =
            Util::instance()->get_device_list(std::string("*"));
  for (unsigned int i = 0; i < dl.size(); i++) {
    dl[i]->start_logging();
  }
  cout4 << "Leaving Logging::start_logging" << std::endl;
}

//+----------------------------------------------------------------------------
// method : Logging::kill_zombie_appenders
//-----------------------------------------------------------------------------
void Logging::kill_zombie_appenders (void)
{
  cout4 << "Entering kill_zombie_appenders" << std::endl;
  // get all devices
  std::vector<DeviceImpl*> dl(0);
  dl = Util::instance()->get_device_list("*");
  // for each device in <dl>
  log4tango::AppenderList al;
  log4tango::Logger *logger = 0;
  unsigned int i, j;
  for (i = 0; i < dl.size(); i++) {
    // get device's logger
    logger = dl[i]->get_logger();
    if (logger) {
      // get logger's appender list
      al = logger->get_all_appenders();
      // for each appender in <al>
      for (j = 0; j < al.size(); j++) {
        if (al[j]->is_valid() == false) {
          cout4 << "Removing zombie appender "
                << dl[i]->get_name()
                << "::"
                << al[j]->get_name()
                << std::endl;
          logger->remove_appender(al[j]);
        }
      } // for each appender in <al>
    } // if logger
  } // for each device in <dl>
  cout4 << "Leaving kill_zombie_appenders" << std::endl;
}

//+----------------------------------------------------------------------------
// method : Logging::set_rolling_file_threshold
//-----------------------------------------------------------------------------
void Logging::set_rolling_file_threshold(log4tango::Logger* logger, size_t rtf)
{
  // check input: logger
  if (!logger) return;
  // check input: rtf
  if (rtf <  kMinRollingThreshold) {
    rtf = kMinRollingThreshold;
  }
  else if (rtf > kMaxRollingThreshold) {
    rtf = kMaxRollingThreshold;
  }
  // misc. var to find file targets in appender list
  std::string::size_type idx;
  TangoRollingFileAppender *rfa;
  std::string prefix = std::string(kLogTargetFile) + kLogTargetSep;
  // get logger's appender list
  log4tango::AppenderList al = logger->get_all_appenders();
  // for each appender in <al>
  for (unsigned int i = 0; i < al.size();  i++) {
    // is it a file target?
    idx = al[i]->get_name().find(prefix);
    if (idx != std::string::npos) {
      rfa = reinterpret_cast<TangoRollingFileAppender*>(al[i]);
      // change its rtf
      rfa->set_maximum_file_size(rtf * 1024);
    }
  }
}

//+----------------------------------------------------------------------------
// method : Logging::tango_to_log4tango_level
//-----------------------------------------------------------------------------
log4tango::Level::Value Logging::tango_to_log4tango_level (Tango::LogLevel tango_level,
                                                           bool throw_exception)
{
  log4tango::Level::Value log4tango_level;
  switch (tango_level) {
    case Tango::LOG_OFF:
      log4tango_level = log4tango::Level::OFF;
      break;
    case Tango::LOG_FATAL:
      log4tango_level = log4tango::Level::FATAL;
      break;
    case Tango::LOG_ERROR:
      log4tango_level = log4tango::Level::ERROR;
      break;
    case Tango::LOG_WARN:
      log4tango_level = log4tango::Level::WARN;
      break;
    case Tango::LOG_INFO:
      log4tango_level = log4tango::Level::INFO;
      break;
    case Tango::LOG_DEBUG:
      log4tango_level = log4tango::Level::DEBUG;
      break;
    default:
      if (throw_exception == true) {
        TangoSys_OMemStream o;
        o << "Invalid logging level specified" << std::ends;
        Except::throw_exception((const char *)API_MethodArgument,
                                o.str(),(const char *)"Logging::tango_to_log4tango_level");
      }
      log4tango_level = log4tango::Level::WARN;
      break;
  } // switch
  return log4tango_level;
}

//+----------------------------------------------------------------------------
// method : Logging::tango_to_log4tango_level  (string version)
//-----------------------------------------------------------------------------
log4tango::Level::Value Logging::tango_to_log4tango_level (const std::string& tango_level,
                                                           bool throw_exception)
{
  log4tango::Level::Value log4tango_level;
  if (tango_level == "OFF") {
      log4tango_level = log4tango::Level::OFF;
  } else if (tango_level == "FATAL") {
    log4tango_level = log4tango::Level::FATAL;
  } else if (tango_level == "ERROR") {
    log4tango_level = log4tango::Level::ERROR;
  } else if (tango_level == "WARN") {
    log4tango_level = log4tango::Level::WARN;
  } else if (tango_level == "INFO") {
    log4tango_level = log4tango::Level::INFO;
  } else if (tango_level == "DEBUG") {
    log4tango_level = log4tango::Level::DEBUG;
  } else {
    if (throw_exception == true) {
      TangoSys_OMemStream o;
      o << "Invalid logging level specified" << std::ends;
      Except::throw_exception((const char *)API_MethodArgument,
                              o.str(),(const char *)"Logging::tango_to_log4tango_level");
    }
    log4tango_level = log4tango::Level::WARN;
  }
  return log4tango_level;
}

//+----------------------------------------------------------------------------
// method : Logging::log4tango_to_tango_level
//-----------------------------------------------------------------------------
Tango::LogLevel Logging::log4tango_to_tango_level (log4tango::Level::Value log4tango_level)
{
  Tango::LogLevel tango_level;
  switch (log4tango_level) {
    case log4tango::Level::FATAL:
      tango_level = Tango::LOG_FATAL;
      break;
    case log4tango::Level::ERROR:
      tango_level = Tango::LOG_ERROR;
      break;
    case log4tango::Level::WARN:
      tango_level = Tango::LOG_WARN;
      break;
    case log4tango::Level::INFO:
      tango_level = Tango::LOG_INFO;
      break;
    case log4tango::Level::DEBUG:
      tango_level = Tango::LOG_DEBUG;
      break;
    case log4tango::Level::OFF:
    default:
      tango_level = Tango::LOG_OFF;
      break;
  } // switch
  return tango_level;
}

//+----------------------------------------------------------------------------
// method : Logging::dev_to_file_name
//-----------------------------------------------------------------------------
std::string Logging::dev_to_file_name (const std::string& _dev_name)
{
  cout4 << "Entering Logging::dev_to_file_name (input " << _dev_name << ")" << std::endl;
  std::string file_name(_dev_name);
  std::transform(file_name.begin(), file_name.end(), file_name.begin(), ::tolower);
  std::string::size_type pos = 0;
  do  {
    pos = file_name.find('/', pos);
    if (pos == std::string::npos) {
      break;
    }
    file_name.replace(pos, 1, "_", 1);
    pos++;
  } while (1);
  cout4 << "Leaving Logging::dev_to_file_name (output "
        << file_name << ")" << std::endl;
  return file_name;
}

//+----------------------------------------------------------------------------
// method : Logging::get_target_type_and_name
//-----------------------------------------------------------------------------
int Logging::get_target_type_and_name (const std::string& input,
                                       std::string& type,
                                       std::string& name)
{
  cout4 << "Logging::get_target_type_and_name (input " << input << ")" << std::endl;
  std::string::size_type pos = input.find(kLogTargetSep, 0);
  if (pos == std::string::npos) {
    type = input;
    name = kDefaultTargetName;
  }
  else {
    type.assign(input.begin(), input.begin() + pos);
    name.assign(input.begin() + pos + ::strlen(kLogTargetSep), input.end());
    if (name.size() == 0) {
      name = kDefaultTargetName;
    }
  }
  cout4 << "Logging::get_target_type_and_name (output "
        << type << " "  << name << ")" << std::endl;
  return 0;
}

//+----------------------------------------------------------------------------
// method : Logging::create_log_dir (called @ startup by Logging::init)
//-----------------------------------------------------------------------------
int Logging::create_log_dir (const std::string& full_path)
{
  //-_VERBOSE(("\tEntering Logging::create_log_dir (input %s)\n", full_path.c_str()));
  std::string::size_type pos = full_path.rfind('/');
  if (pos != std::string::npos) {
    std::string sub_path;
    sub_path.assign(full_path.begin(), full_path.begin() + pos);
    if (sub_path.size()) {
        Logging::create_log_dir(sub_path);
    }
  }
#ifdef _TG_WINDOWS_
  int res = ::mkdir(full_path.c_str());
#else
  int res = ::mkdir(full_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
  //-_VERBOSE(("\tLeaving Logging::create_log_dir (::mkdir returned %d)\n", res));
  return res;
}

//+----------------------------------------------------------------------------
// method : LogAdapter::LogAdapter
//-----------------------------------------------------------------------------
LogAdapter::LogAdapter (Tango::DeviceImpl *dev)
{
  if (dev) {
    logger_ = dev->get_logger();
  } else {
    logger_ = API_LOGGER;
  }
}

//+----------------------------------------------------------------------------
// method : LogAdapter::~LogAdapter
//-----------------------------------------------------------------------------
LogAdapter::~LogAdapter ( )
{
 //no-op dtor
}

} // namespace Tango

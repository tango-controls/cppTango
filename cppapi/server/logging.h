//+=============================================================================
//
// file :   Logging.h
//
// description :  TLS helper class (pseudo-singleton)
//
// project :    TANGO
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
// $Revision$
//
//-=============================================================================

#ifndef _LOGGING_H_
#define _LOGGING_H_

#ifdef TANGO_HAS_LOG4TANGO

// A shortcut to the core logger ------------------------------
#define API_LOGGER Tango::Logging::get_core_logger()

#ifdef _TANGO_LIB //== compiling TANGO lib ====================

//-- Overwrite std::cout  -------------------------------------
#define cout \
    if (API_LOGGER) \
      API_LOGGER->get_stream(log4tango::Level::INFO, false) \
        << log4tango::LogInitiator::_begin_log

#endif //== compiling TANGO lib ===============================

// Map. cout1..2 to INFO level --------------------------------
#define cout1 \
  if (API_LOGGER && API_LOGGER->is_info_enabled()) \
    API_LOGGER->info_stream() \
      << log4tango::LogInitiator::_begin_log
#define cout2 cout1

// Map. cout3..5 to DEBUG level -------------------------------
#define cout3 \
  if (API_LOGGER && API_LOGGER->is_debug_enabled()) \
    API_LOGGER->debug_stream() \
      << log4tango::LogInitiator::_begin_log
#define cout4 cout3
#define cout5 cout4

namespace Tango {

class Util;

#if defined (_TG_WINDOWS_) && !defined(_TANGO_LIB) && defined(TANGO_HAS_DLL)
 extern __declspec(dllimport) log4tango::Logger* _core_logger;
#else
 extern log4tango::Logger* _core_logger;
#endif

class Logging
{
public:
  /**
   * Initializes the Tango Logging service (TLS)
   **/
  static void init (const std::string& ds_name,
                    int cmd_line_level,
                    bool use_tango_db,
                    Database &db,
                    Util *tg);
  /**
   * Shutdown the Tango Logging service
   **/
  static void cleanup (void);

  /**
   * Returns the core logger substitute
   **/
  inline static log4tango::Logger* get_core_logger (void) {
    return _core_logger;
  }

  /**
   * Implementation of the DServer AddLoggingTarget Tango command
   **/
  static void add_logging_target (const DevVarStringArray *argin);

  /**
   * Implementation of the DServer AddLoggingTarget Tango command
   **/
  static void add_logging_target (log4tango::Logger* logger,
                                  const std::string& tg_type,
                                  const std::string& tg_name,
                                  int throw_exception = 1);

  /**
   * Implementation of the DServer AddLoggingTarget Tango command
   **/
  static void add_logging_target (log4tango::Logger* logger,
                                  const std::string& tg_type_name,
                                  int throw_exception = 1);

  /**
   * Implementation of the DServer RemoveLoggingTarget Tango command
   **/
  static void remove_logging_target (const DevVarStringArray *argin);

  /**
   * Implementation of the DServer GetLoggingTarget Tango command
   **/
  static DevVarStringArray* get_logging_target (const std::string& dev_name);

  /**
   * Implementation of the DServer SetLoggingLevel Tango command
   **/
  static void set_logging_level (const DevVarLongStringArray *argin);

  /**
   * Implementation of the DServer GetLoggingLevel Tango command
   **/
  static DevVarLongStringArray* get_logging_level (const DevVarStringArray *argin);

  /**
   * Implementation of the DServer StartLogging Tango command
   **/
  static void  start_logging (void);

  /**
   * Implementation of the DServer StopLogging Tango command
   **/
  static void  stop_logging (void);

  /**
   * Converts a Tango logging level into a log4tango level
   **/
  static log4tango::Level::Value tango_to_log4tango_level (Tango::LogLevel tango_level,
                                                           bool throw_exception = true);

  /**
   * Converts a Tango logging level into a log4tango level
   **/
  static log4tango::Level::Value tango_to_log4tango_level (const std::string& tango_level,
                                                           bool throw_exception = true);

  /**
   * Converts a log4tango level into a Tango logging level
   **/
  static Tango::LogLevel log4tango_to_tango_level (log4tango::Level::Value log4tango_level);


  /**
   * Modify the rolling file threshold of the given Logger
   **/
  static void set_rolling_file_threshold(log4tango::Logger* logger, size_t rtf);

private:
  /**
   *
   **/
  static void kill_zombie_appenders (void);

  /**
   *
   **/
  static std::string dev_to_file_name (const std::string& _dev_name);

  /**
   *
   **/
  static int get_target_type_and_name (const std::string& input,
                                       std::string& type,
                                       std::string& name);

  /**
   *
   **/
  static int create_log_dir (const std::string& full_path);

  /**
   * Protect the Logging class against instanciation and copy
   **/
   Logging (const Logging&);
   ~Logging();
   const Logging& operator= (const Logging&);

  /**
   *
   **/
   static std::string _log_path;

  /**
   *
   **/
   static size_t _rft;

  /**
   *
   **/
   static int _cmd_line_level;
};

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO

#endif // _LOGGING_H_

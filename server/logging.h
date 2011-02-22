//+=============================================================================
//
// file :	  Logging.h
//
// description :  TLS helper class (pseudo-singleton)
//
// project :	  TANGO
//
// author(s) :	  N.Leclercq - SOLEIL
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/03/25 16:44:06  taurel
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
// Revision 2.4  2003/03/13 15:17:49  nleclercq
// Minor modifications on logging implementation
//
// Revision 2.3  2003/03/12 15:30:00  nleclercq
// cout multi-targets logging restricted to TANGO lib
//
// Revision 2.2  2003/03/11 17:55:55  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:43  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
//
// copyleft :	  European Synchrotron Radiation Facility
//		  BP 220, Grenoble 38043
//		  FRANCE
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

class Logging
{
public:
  /**
   * Initializes the Tango Logging service (TLS)
   **/
  static void init (const std::string& ds_name, 
                    int cmd_line_level,
                    bool use_tango_db,
		    Database &db);        
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
  static std::string Logging::dev_to_file_name (const std::string& _dev_name);

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
   static log4tango::Logger* _core_logger;

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

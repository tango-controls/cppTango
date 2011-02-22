/*
 * Appender.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4tango/Portability.hh>
#include <string>
#include <map>
#include <set>
#include <log4tango/Level.hh>
#include <log4tango/Layout.hh>
#include <log4tango/LoggingEvent.hh>
#include <log4tango/threading/Threading.hh>

#ifndef _LOG4TANGO_APPENDER_H
#define _LOG4TANGO_APPENDER_H

namespace log4tango {

//-----------------------------------------------------------------------------
// class : Appender
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT Appender
{
protected:
  /**
   * Constructor for Appender. Will only be used in 
   * getAppender() (and in derived classes of course).
   * @param name The name of this Appender.
   **/
  Appender (const std::string& name);

public:
  /**
   * Destructor for Appender.
   **/
  virtual ~Appender ();
        
  /**
   * Log in Appender specific way. Returns -1 on error, 0 otherwise.
   * @param event  The LoggingEvent to log. 
   **/
#if defined(APPENDERS_HAVE_LEVEL_THRESHOLD) || defined(APPENDERS_HAVE_FILTERS)
  int append (const LoggingEvent& event);
#else
  inline int append (const LoggingEvent& event) {
    return _append(event);
  } 
#endif

  /**
   * Reopens the output destination of this Appender, e.g. the logfile 
   * or TCP socket.
   * @returns false if an error occured during reopening, true otherwise.
   **/
  virtual bool reopen (void);

  /**
   * Release any resources allocated within the appender such as file
   * handles, network connections, etc.
   **/
  virtual void close (void) = 0;

  /**
   * Check if the appender uses a layout.
   * 
   * @returns true if the appender implementation requires a layout.
   **/
  virtual bool requires_layout (void) const = 0;

  /**
   * Change the layout.
   **/
  virtual void set_layout (Layout* layout = 0) = 0;

  /**
   * Returns this appender name.
   **/
  inline const std::string& get_name (void) const {
    return _name;
  }

  /**
   * Check if the appender is valid (for instance the underlying connection is ok)
   * This default implementation always return true. Overload to define your own
   * behaviour.
   * 
   * @returns true if the appender is valid, false otherwise.
   **/
  virtual bool is_valid (void) const;
  
#ifdef APPENDERS_HAVE_LEVEL_THRESHOLD
  /**
   * Set the threshold level of this Appender. The Appender will not
   * appender LoggingEvents with a level lower than the threshold.
   * Use Level::NOTSET to disable level checking.
   * @param level The level to set.
   **/
  void set_level (Level::Value level);

  /**
   * Get the threshold level of this Appender.
   * @returns the threshold
   **/
  Level::Value get_level (void) const;
#endif // APPENDERS_HAVE_LEVEL_THRESHOLD

#ifdef APPENDERS_HAVE_FILTERS
  /**
   * Set a Filter for this appender. 
   **/
  virtual void set_filter (Filter* filter);

  /**
   * Get the Filter for this appender.
   * @returns the filter, or NULL if no filter has been set.
   **/
  virtual Filter* get_filter (void);
#endif // APPENDERS_HAVE_FILTERS

protected:
  /**
   * Log in Appender specific way. Subclasses of Appender should
   * implement this method to perform actual logging.
   * @param event  The LoggingEvent to log. 
   **/
  virtual int _append(const LoggingEvent& event) = 0;
         
private:
  /**
   * The appender name
   **/ 
  const std::string _name;

#ifdef APPENDERS_HAVE_LEVEL_THRESHOLD
  /**
   * The appender logging level
   **/ 
  Level::Value _level;
#endif

#ifdef APPENDERS_HAVE_FILTERS
  /**
   * The appender filter list
   **/ 
  Filter* _filter;
#endif
};

} // namespace log4tango

#endif // _LOG4TANGO_APPENDER_H

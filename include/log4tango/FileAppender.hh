/*
 * FileAppender.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_FILEAPPENDER_H
#define _LOG4TANGO_FILEAPPENDER_H

#include <log4tango/Portability.hh>
#include <log4tango/LayoutAppender.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// class : FileAppender
//-----------------------------------------------------------------------------   
class LOG4TANGO_EXPORT FileAppender : public LayoutAppender 
{
public:

  /**
  Constructs a FileAppender.
  @param name the name of the Appender.
  @param fileName the name of the file to which the Appender has 
  to log.
  @param append whether the Appender has to truncate the file or
  just append to it if it already exists. Defaults to 'true'.
  @param mode file mode to open the logfile with. Defaults to 00644.
  **/  
  FileAppender(const std::string& name, const std::string& fileName,
               bool append = true, mode_t mode = 00644);

  /**
  Constructs a FileAppender to an already open file descriptor.
  @param name the name of the Appender.
  @param fd the file descriptor to which the Appender has to log.
  **/
  FileAppender(const std::string& name, int fd);

  /**
   Destructor
  **/
  virtual ~FileAppender();
        
  /**
  Reopens the logfile. 
  This can be useful for logfiles that are rotated externally,
  e.g. by logrotate. This method is a NOOP for FileAppenders that
  have been constructed with a file descriptor.           
  @returns true if the reopen succeeded.
  **/
  virtual bool reopen (void);

  /**
  Closes the logfile.
  **/
  virtual void close (void);
  
 /**
   * Check if the appender is valid.
   * 
   * @returns true if the appender is valid, false otherwise.
   **/
  virtual bool is_valid (void) const;
  
  /**
  Sets the append vs truncate flag.
  NB. currently the FileAppender opens the logfile in the 
  constructor. Therefore this method is too late to influence the 
  first file opening. We'll need something similar to log4j's
  activateOptions().
  @param append false to truncate, true to append
  **/
  virtual void set_append (bool append);

  /**
  Gets the value of the 'append' option.
  **/
  virtual bool get_append (void) const;

  /**
  Sets the file open mode.
  **/
  virtual void set_mode (mode_t mode);

  /**
  Gets the file open mode.
  **/
  virtual mode_t get_mode() const;

protected:

  virtual int _append (const LoggingEvent& event);

  const std::string _file_name;
  int _fd;
  int _flags;
  mode_t _mode;
};

} // namespace log4tango

#endif // _LOG4TANGO_FILEAPPENDER_H

/*
 * LogStreambuf.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */
 
#ifndef _LOG4TANGO_STREAM_BUFFER_H
#define _LOG4TANGO_STREAM_BUFFER_H

#include <log4tango/Portability.hh>
#include <log4tango/Logger.hh>

//-----------------------------------------------------------------------------
// #DEFINES
//-----------------------------------------------------------------------------
#define kDEFAULT_BUFFER_SIZE 512

namespace log4tango {

//-----------------------------------------------------------------------------
// Class : LogStreamBuf
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT LogStreamBuf : public std::streambuf
{
  friend class LogStream;

public:

  LogStreamBuf (Logger* logger, 
                Level::Value level,
                bool filter = true,
                size_t bsize = kDEFAULT_BUFFER_SIZE);

  virtual ~LogStreamBuf();

protected:

   virtual std::streamsize xsputn (const char*, std::streamsize);


   virtual int sync (void);

private:

  int flush_buffer (void);

  char *_buffer;

  Logger* _logger;

  Level::Value _level;

  bool _filter;
};

} // namespace log4tango

#endif // _LOG4TANGO_STREAM_BUFFER_H



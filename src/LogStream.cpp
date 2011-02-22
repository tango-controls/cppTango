/*
 * LogStream.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */
 
#include "PortabilityImpl.hh"
#include <log4tango/LogStream.hh>

namespace log4tango {

//+----------------------------------------------------------------------------
// method : LogStream::LogStream
//-----------------------------------------------------------------------------
LogStream::LogStream (LogStreamBuf* log_stream_buf) 
  : _ostream(log_stream_buf)
{
  //no-op
}

//+----------------------------------------------------------------------------
// method : LogStream::~LogStream
//-----------------------------------------------------------------------------
LogStream::~LogStream() 
{ 
  //no-op
}

} // namespace log4tango

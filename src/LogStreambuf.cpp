//
// LogStreambuf.cpp
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010
//					Synchrotron SOLEIL
//                	L'Orme des Merisiers
//                	Saint-Aubin - BP 48 - France
//
// This file is part of log4tango.
//
// Log4ango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Log4tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with Log4Tango.  If not, see <http://www.gnu.org/licenses/>.
 
#include "PortabilityImpl.hh"
#include <log4tango/LogStreambuf.hh>

namespace log4tango {

//+----------------------------------------------------------------------------
// method : LogStreamBuf::LogStreamBuf
//----------------------------------------------------------------------------- 
LogStreamBuf::LogStreamBuf(Logger* logger, 
                           Level::Value level,
                           bool filter,
                           size_t bsize)
 : _buffer(0), _logger(logger), _level(level), _filter(filter)
{
//
// Allocate the buffer
//
  _buffer = new char[bsize];

//
// Set streambuf pointers
//
  if (_buffer) {
    setp(_buffer, _buffer + bsize);
  }
}

//+----------------------------------------------------------------------------
// method : LogStreamBuf::~LogStreamBuf
//----------------------------------------------------------------------------- 
LogStreamBuf::~LogStreamBuf()
{
//
// Flush buffer
//
  sync();

//
// Release buffer
//
  if (_buffer) {
    delete[] _buffer;
    _buffer = 0;
  }
}

//+----------------------------------------------------------------------------
// method : LogStreamBuf::xsputn
//-----------------------------------------------------------------------------
std::streamsize LogStreamBuf::xsputn (const char *_in, std::streamsize _in_size)
{
  if (!_buffer) {
    return 0;
  }

  std::streamsize _M, _Ns;
    
  for (_Ns = 0; 0 < _in_size;)
      
    if ((_M = (epptr() - pptr())))
    {
      if (_in_size < _M) _M = _in_size;

      ::memcpy(pptr(), _in, _M);
  
      _in += _M, _Ns += _M, _in_size -= _M, pbump(_M);
    } 

    else if (flush_buffer() == -1) 

      break;

    else 

      ++_in, ++_Ns, --_in_size;

    return _Ns;
}

//+----------------------------------------------------------------------------
// method : LogStreamBuf::flush_buffer
//-----------------------------------------------------------------------------
int LogStreamBuf::flush_buffer (void)
{
//
// Be sure we have a valid buffer
//
  if (!_buffer) {
    return -1;
  }

//
// Get current # chars in buffer
//

  long num_chars = pptr() - pbase();

//
// Send buffer to logger's targets
//

  try {
    if (_logger) {
      if (_filter) {
        _logger->log(_level, std::string(pbase(), num_chars));
      } else {
        _logger->log_unconditionally(_level, std::string(pbase(), num_chars));
      }
    }
  } catch (...) {
    // ignore exception
  }

//
// Reset put pointer
//

  pbump(-num_chars);


  return num_chars;
}

//+----------------------------------------------------------------------------
// method : LogStreamBuf::sync
//-----------------------------------------------------------------------------
int LogStreamBuf::sync (void)
{
//
// Flush the buffer
//

  flush_buffer();

  return 0;
}

} // namespace log4tango 

//+=============================================================================
//
// file :	        logstream.h
//
// description :  A TLS helper class 
//
// project :	    TANGO
//
// author(s) :	  N.Leclercq - SOLEIL
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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

#ifndef _LOG_STREAM_H
#define _LOG_STREAM_H

#ifdef TANGO_HAS_LOG4TANGO

//-----------------------------------------------------------------------------
// FORWARD DECLARATIONS
//-----------------------------------------------------------------------------
namespace log4tango {
 class LoggerStream;
} // namespace log4tango

namespace Tango {
  class Attr;
  class DevFailed;
  class Attribute;
  class AttrManip;
  class AttrProperty;
  class DevVarCharArray;
  class DevVarShortArray;
  class DevVarLongArray;
  class DevVarFloatArray;
  class DevVarDoubleArray;
  class DevVarUShortArray;
  class DevVarULongArray;
  class DevVarStringArray;
} // namespace Tango

//-----------------------------------------------------------------------------
// MISC. OPERATORS TO PUSH TANGO TYPES INTO LOG4TANGO STREAMS
//-----------------------------------------------------------------------------
namespace Tango {

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevFailed&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarCharArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarShortArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarLongArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarFloatArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarDoubleArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarUShortArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarULongArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const DevVarStringArray&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const Attribute&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const Attr&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const AttrManip&);

log4tango::LoggerStream& operator<< (log4tango::LoggerStream&, const AttrProperty&);

} // Tango namespace

#endif // TANGO_HAS_LOG4TANGO

#endif // _LOG_STREAM_H

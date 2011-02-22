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
// copyleft :	    European Synchrotron Radiation Facility
//		            BP 220, Grenoble 38043
//		            FRANCE
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

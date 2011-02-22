//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
//						Synchrotron SOLEIL
//                		L'Orme des Merisiers
//                		Saint-Aubin - BP 48 - France
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.

//-----------------------------------------------------------------------------
// WINDOWS PRAGMA
//-----------------------------------------------------------------------------
#if defined (WIN32)
# pragma warning (disable : 4286)
#endif

//-----------------------------------------------------------------------------
// DEFINE A SPECIAL VERSION OF TANGO ERROR_STREAM MACRO SO THAT FOLLOWING 
// MACROS CAN BE SAFELY USED FROM C++ (PURE) CLIENT WHERE THERE IS NO 
// INSTANCIATED LOGGER
//-----------------------------------------------------------------------------
#define _ERROR_STREAM \
  if (get_logger() && get_logger()->is_error_enabled()) \
    get_logger()->error_stream() << log4tango::LogInitiator::_begin_log

//=============================================================================
// _HANDLE_DEV_EXCEPTION MACRO
//=============================================================================

#define _HANDLE_DEV_EXCEPTION(_cmd, _origin) _HANDLE_DEV_EXCEPTION_FILE_LINE(_cmd,_origin, __FILE__, __LINE__) 

#define _HANDLE_DEV_EXCEPTION_FILE_LINE(_cmd, _origin, _file, _line) \
	catch (Tango::ConnectionFailed &e)  { \
     TangoSys_OMemStream d; \
    d << "Tango::ConnectionFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_CONNECTION_FAILED"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::CommunicationFailed &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::CommunicationFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_COMMUNICATION_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::WrongNameSyntax &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::WrongNameSyntax exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_WRONG_NAME_SYNTAX_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::NonDbDevice &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::NonDbDevice exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_NON_DB_DEVICE_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::WrongData &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::WrongData exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::NonSupportedFeature &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::NonSupportedFeature exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_NON_SUPPORTED_FEATURE_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::AsynCall &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::AsynCall exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_ASYNC_CALL_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::AsynReplyNotArrived &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::EventSystemFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_ASYNC_REPLY_NOT_ARRIVED_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::EventSystemFailed &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::EventSystemFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_EVENT_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::DevFailed &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::DevFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    _ERROR_STREAM << e << std::endl; \
    Tango::Except::re_throw_exception(e, \
            static_cast<const char*>("TANGO_DEVICE_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (const std::bad_alloc &) \
  { \
    TangoSys_OMemStream d; \
    d << "std::bad_alloc exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    Tango::DevErrorList errors(1); \
    errors.length(1); \
    errors[0].severity = Tango::ERR; \
    errors[0].reason = CORBA::string_dup("OUT_OF_MEMORY"); \
    errors[0].desc = CORBA::string_dup(d.str().c_str()); \
    errors[0].origin = CORBA::string_dup(o.str().c_str()); \
    Tango::DevFailed df(errors); \
    _ERROR_STREAM << df << std::endl; \
    throw df; \
  } \
  catch (...) \
  { \
    TangoSys_OMemStream d; \
    d << "unknown exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << " [" << _file << "::" << _line << "]" << std::ends; \
    Tango::DevErrorList errors(1); \
    errors.length(1); \
    errors[0].severity = Tango::ERR; \
    errors[0].reason = CORBA::string_dup("UNKNOWN_ERROR"); \
    errors[0].desc = CORBA::string_dup(d.str().c_str()); \
    errors[0].origin = CORBA::string_dup(o.str().c_str()); \
    Tango::DevFailed df(errors); \
    _ERROR_STREAM << df << std::endl; \
    throw df; \
  }

//=============================================================================
// _DEV_TRY MACRO
//=============================================================================
#define _DEV_TRY_FILE_LINE(_invoke, _cmd, _origin, _file, _line) \
  try \
  { \
    _invoke; \
  } \
  _HANDLE_DEV_EXCEPTION_FILE_LINE(_cmd, _origin, _file, _line)

//=============================================================================
// _DEV_TRY MACRO
//=============================================================================
#define _DEV_TRY(_invoke, _cmd, _origin) \
  try \
  { \
    _invoke; \
  } \
  _HANDLE_DEV_EXCEPTION(_cmd, _origin)


//=============================================================================
// _HANDLE_ASL_EXCEPTION_REACTION MACRO
//=============================================================================
#define _HANDLE_DEV_EXCEPTION_REACTION(_cmd, _origin, _reaction) \
  catch (...) \
  { \
    _reaction; \
    _DEV_TRY(throw, _cmd, _origin)  \
  }

//=============================================================================
// _DEV_TRY_ACTION MACRO
//=============================================================================
#define _DEV_TRY_ACTION(_invoke, _cmd, _origin, _action ) \
  try { \
    _DEV_TRY(_invoke, _cmd, _origin) \
    _action; \
  } \
  catch (...) { \
    _action; \
    throw; \
  }

//=============================================================================
// _DEV_TRY_REACTION MACRO
//=============================================================================
#define _DEV_TRY_REACTION(_invoke, _cmd, _origin, _reaction) \
  try { \
    _DEV_TRY(_invoke, _cmd, _origin) \
  } \
  catch (...) { \
    _reaction; \
    throw; \
  }



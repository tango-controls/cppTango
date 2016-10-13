//
// apiexcept.h - include file for TANGO device api exceptions
//
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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


#ifndef _APIEXCEPT_H
#define _APIEXCEPT_H

#include <tango/server/except.h>
#include <vector>

using namespace std;

namespace Tango {

/***************************************************************************
*
*		Exception classes used for the write_attribute call
*
****************************************************************************/

/**
 * An exception class
 *
 * This class is used as exception for the DeviceProxy::write_attribute call()
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class NamedDevFailed
{
public:
	string		    name;           ///< The name of the attribute which fails
	long		    idx_in_call;    ///< Index in the write_attributes method parameter vector of the attribute which failed.
	DevErrorList	err_stack;      ///< The error stack

/// @privatesection

	NamedDevFailed(const DevErrorList &err,const string &na,long idx):name(na),idx_in_call(idx),err_stack(err) {}
	NamedDevFailed();
};

/**
 * An exception class
 *
 * This class is used as exception for the DeviceProxy::write_attribute call()
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class NamedDevFailedList: public DevFailed
{
public:
/**
 * Get faulty attribute number
 *
 * Returns the number of attributes which failed during the write_attribute call.
 *
 * @return The number of attribute(s) which fail during the write_attribute call
 */
	size_t get_faulty_attr_nb() {return err_list.size();}
/**
 * Check if the call failed
 *
 * This method returns true if at least one attribute failed during the call
 *
 * @return A boolean set to true if the call failed for at least one attribute
 */
	bool call_failed() {if ((err_list.empty()==true) && (errors.length()!=0))return true;else return false;}

	vector<NamedDevFailed>	err_list;   ///< There is one element in this vector for each attribute which failed during its writing.

/// @privatesection

	NamedDevFailedList(const MultiDevFailed &,string,const char *,const char *);
	NamedDevFailedList() {};
};


/****************************************************************************
*
*		Macro to define API specific exception and their methods
*
*****************************************************************************/
//TODO replace macro with base class (Tango::Except)
//#define MAKE_EXCEPT(E,I) \
//class E: public Tango::DevFailed \
//{ \
//public: \
//	E(const DevErrorList& err):DevFailed(err) {} \
//}; \
//class I \
//{ \
//public: \
//	static inline void throw_exception(const char *reason,const string &desc,const char *origin, \
//					   ErrSeverity sever = ErrSeverity::ERR) \
//	{\
//		Tango::DevErrorList errors(1);\
//		errors.length(1);\
//		errors[0].desc = string_dup(desc.c_str()); \
//		errors[0].severity = sever; \
//		errors[0].reason = string_dup(reason);\
//		errors[0].origin = string_dup(origin);\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void throw_exception(const char *reason,char *desc,const char *origin,\
//					   ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(1);\
//		errors.length(1);\
//		errors[0].desc = string_dup(desc);\
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup(reason);\
//		errors[0].origin = string_dup(origin);\
//		delete[] desc;\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void throw_exception(const char *reason,const char *desc,const char *origin,\
//					   ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(1);\
//		errors.length(1);\
//		errors[0].desc = string_dup(desc);\
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup(reason);\
//		errors[0].origin = string_dup(origin);\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void re_throw_exception(CORBA::SystemException &cex,\
//					      const string &reason,const string &desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		Tango::Except::the_mutex.lock(); \
//		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
//		errors[0].desc = string_dup(tmp);\
//		Tango::Except::the_mutex.unlock(); \
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin);\
//		errors[1].desc = string_dup(desc.c_str());\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason.c_str());\
//		errors[1].origin = string_dup(origin);\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void re_throw_exception(CORBA::SystemException &cex,\
//					      char *reason,char *desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		Tango::Except::the_mutex.lock(); \
//		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
//		errors[0].desc = string_dup(tmp);\
//		Tango::Except::the_mutex.unlock(); \
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin);\
//		errors[1].desc = string_dup(desc);\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason);\
//		errors[1].origin = string_dup(origin);\
//		delete[] desc;\
//		delete[] reason;\
//		throw Tango::E(errors);\
//	}\
//	static inline void re_throw_exception(CORBA::SystemException &cex,\
//					      const char *reason,const string &desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		Tango::Except::the_mutex.lock(); \
//		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
//		errors[0].desc = string_dup(tmp);\
//		Tango::Except::the_mutex.unlock(); \
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin);\
//		errors[1].desc = string_dup(desc.c_str());\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason);\
//		errors[1].origin = string_dup(origin);\
//		throw Tango::E(errors);\
//	}\
//	static inline void re_throw_exception(CORBA::SystemException &cex,\
//					      const char *reason,const string &desc,\
//					      const string &origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		Tango::Except::the_mutex.lock(); \
//		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
//		errors[0].desc = string_dup(tmp);\
//		Tango::Except::the_mutex.unlock(); \
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin.c_str());\
//		errors[1].desc = string_dup(desc.c_str());\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason);\
//		errors[1].origin = string_dup(origin.c_str());\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void re_throw_exception(CORBA::SystemException &cex,\
//					      const char *reason,char *desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		Tango::Except::the_mutex.lock(); \
//		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
//		errors[0].desc = string_dup(tmp);\
//		Tango::Except::the_mutex.unlock(); \
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin);\
//		errors[1].desc = string_dup(desc);\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason);\
//		errors[1].origin = string_dup(origin);\
//		delete[] desc;\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void re_throw_exception(Tango::E &ex,\
//					      const char *reason,char *desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		long nb_err = ex.errors.length();\
//		ex.errors.length(nb_err + 1);\
//		ex.errors[nb_err].severity = sever;\
//		ex.errors[nb_err].desc = string_dup(desc);\
//		delete[] desc;\
//		ex.errors[nb_err].origin = string_dup(origin);\
//		ex.errors[nb_err].reason = string_dup(reason);\
//		throw ex;\
//	}\
//\
//	static inline void re_throw_exception(Tango::E &ex,\
//					      const char *reason,const string &desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		long nb_err = ex.errors.length();\
//		ex.errors.length(nb_err + 1);\
//		ex.errors[nb_err].severity = sever;\
//		ex.errors[nb_err].desc = string_dup(desc.c_str());\
//		ex.errors[nb_err].origin = string_dup(origin);\
//		ex.errors[nb_err].reason = string_dup(reason);\
//		throw ex;\
//	}\
//	static inline void re_throw_exception(Tango::DevFailed &ex,\
//					      const char *reason,char *desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		long nb_err = ex.errors.length();\
//		ex.errors.length(nb_err + 1);\
//		ex.errors[nb_err].severity = sever;\
//		ex.errors[nb_err].desc = string_dup(desc);\
//		delete[] desc;\
//		ex.errors[nb_err].origin = string_dup(origin);\
//		ex.errors[nb_err].reason = string_dup(reason);\
//		throw ex;\
//	}\
//\
//	static inline void re_throw_exception(Tango::DevFailed &ex,\
//					      const char *reason,const string &desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		long nb_err = ex.errors.length();\
//		ex.errors.length(nb_err + 1);\
//		ex.errors[nb_err].severity = sever;\
//		ex.errors[nb_err].desc = string_dup(desc.c_str());\
//		ex.errors[nb_err].origin = string_dup(origin);\
//		ex.errors[nb_err].reason = string_dup(reason);\
//		throw ex;\
//	}\
//	static inline void re_throw_exception(Tango::DevFailed &ex,\
//					      const char *reason,const char *desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		long nb_err = ex.errors.length();\
//		ex.errors.length(nb_err + 1);\
//		ex.errors[nb_err].severity = sever;\
//		ex.errors[nb_err].desc = string_dup(desc);\
//		ex.errors[nb_err].origin = string_dup(origin);\
//		ex.errors[nb_err].reason = string_dup(reason);\
//		throw ex;\
//	}\
//\
//	static inline void re_throw_exception(char *CORBA_error_desc,\
//					      const char *reason,char *desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		errors[0].desc = string_dup(CORBA_error_desc);\
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin);\
//		errors[1].desc = string_dup(desc);\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason);\
//		errors[1].origin = string_dup(origin);\
//		delete[] desc;\
//		throw Tango::E(errors);\
//	}\
//\
//	static inline void re_throw_exception(char *CORBA_error_desc,\
//					      const char *reason,const string &desc,\
//					      const char *origin,\
//					      ErrSeverity sever = ErrSeverity::ERR)\
//	{\
//		Tango::DevErrorList errors(2);\
//		errors.length(2);\
//		errors[0].desc = string_dup(CORBA_error_desc);\
//		errors[0].severity = sever;\
//		errors[0].reason = string_dup("API_CorbaException");\
//		errors[0].origin = string_dup(origin);\
//		errors[1].desc = string_dup(desc.c_str());\
//		errors[1].severity = sever;\
//		errors[1].reason = string_dup(reason);\
//		errors[1].origin = string_dup(origin);\
//		throw Tango::E(errors);\
//	}\
//};
//
//MAKE_EXCEPT(ConnectionFailed,ApiConnExcept)
//MAKE_EXCEPT(CommunicationFailed,ApiCommExcept)
//MAKE_EXCEPT(WrongNameSyntax,ApiWrongNameExcept)
//MAKE_EXCEPT(NonDbDevice,ApiNonDbExcept)
//MAKE_EXCEPT(WrongData,ApiDataExcept)
//MAKE_EXCEPT(NonSupportedFeature,ApiNonSuppExcept)
//MAKE_EXCEPT(AsynCall,ApiAsynExcept)
//MAKE_EXCEPT(AsynReplyNotArrived,ApiAsynNotThereExcept)
//MAKE_EXCEPT(EventSystemFailed,EventSystemExcept)
//MAKE_EXCEPT(DeviceUnlocked,DeviceUnlockedExcept)
//MAKE_EXCEPT(NotAllowed,NotAllowedExcept)

//
// Define macros for the management of the Corba TRANSIENT exception
//

#define TRANSIENT_NOT_EXIST_EXCEPT(E,CLASS,NAME,OBJ) \
	if (E.minor() == omni::TRANSIENT_CallTimedout) \
	{ \
\
		bool need_reconnect = false; \
		omniORB::setClientConnectTimeout(NARROW_CLNT_TIMEOUT); \
		try \
		{ \
			Device_var dev = Device::_duplicate(OBJ->device); \
			dev->ping(); \
		} \
		catch(CORBA::TRANSIENT &trans_ping) \
		{ \
			if (trans_ping.minor() == omni::TRANSIENT_ConnectFailed || \
				trans_ping.minor() == omni::TRANSIENT_CallTimedout  || \
                (OBJ->ext->has_alt_adr == true && trans_ping.minor() == omni::TRANSIENT_CallTimedout)) \
			{ \
				need_reconnect = true; \
			} \
		} \
		catch(...) {} \
		omniORB::setClientConnectTimeout(0); \
\
		if (need_reconnect == false) \
		{ \
			TangoSys_OMemStream desc; \
			desc << "Timeout (" << OBJ->timeout << " mS) exceeded on device " << OBJ->dev_name(); \
			desc << ends; \
			TangoSys_OMemStream ori; \
			ori << CLASS << ":" << NAME << ends; \
			ApiCommExcept::re_throw_exception(E, \
						  (const string& )"API_DeviceTimedOut", \
						  desc.str(), ori.str());\
		}\
	} \
\
	OBJ->set_connection_state(CONNECTION_NOTOK); \
	ctr++; \
\
	if ((OBJ->tr_reco == false) || \
	   ((ctr == 2) && (OBJ->tr_reco == true))) \
	{ \
\
		TangoSys_OMemStream desc; \
		desc << "Failed to execute " << NAME << " on device " << OBJ->dev_name(); \
		desc << ends; \
		TangoSys_OMemStream ori; \
		ori << CLASS << ":" << NAME << ends; \
		ApiCommExcept::re_throw_exception(E, \
						   (const char*)"API_CommunicationFailed", \
                        			   desc.str(),ori.str()); \
	}


#define TRANSIENT_NOT_EXIST_EXCEPT_CMD(E) \
	if (E.minor() == omni::TRANSIENT_CallTimedout) \
	{ \
\
		bool need_reconnect = false; \
		try \
		{ \
			Device_var dev = Device::_duplicate(device); \
			dev->ping(); \
		} \
		catch(CORBA::TRANSIENT &trans_ping) \
		{ \
			if (trans_ping.minor() == omni::TRANSIENT_ConnectFailed || \
                trans_ping.minor() == omni::TRANSIENT_CallTimedout || \
                (ext->has_alt_adr == true && trans_ping.minor() == omni::TRANSIENT_CallTimedout)) \
			{ \
				need_reconnect = true; \
			} \
		} \
		catch(...) {} \
\
		if (need_reconnect == false) \
		{ \
			TangoSys_OMemStream desc; \
			desc << "Timeout (" << timeout << " mS) exceeded on device " << dev_name(); \
			desc << ", command " << command << ends; \
			ApiCommExcept::re_throw_exception(E, \
						  (const string& )"API_DeviceTimedOut", \
						  desc.str(), \
						  (const string& )"Connection::command_inout()"); \
		}\
	} \
\
	set_connection_state(CONNECTION_NOTOK); \
	if (get_lock_ctr() != 0) \
	{ \
		set_lock_ctr(0); \
		TangoSys_OMemStream desc; \
		desc << "Device " << dev_name() << " has lost your lock(s) (server re-start?) while executing command " << command << ends; \
		DeviceUnlockedExcept::re_throw_exception(E,(const char*)DEVICE_UNLOCKED_REASON, \
					desc.str(), (const char*)"Connection::command_inout()"); \
	} \
	ctr++; \
\
	if ((tr_reco == false) || \
	   ((ctr == 2) && (tr_reco == true))) \
	{ \
		TangoSys_OMemStream desc; \
		desc << "Failed to execute command_inout on device " << dev_name(); \
		desc << ", command " << command << ends; \
		ApiCommExcept::re_throw_exception(E, \
				   (const char*)"API_CommunicationFailed", \
                    		   desc.str(), \
				   (const char*)"Connection::command_inout()"); \
	}


} // End of Tango namespace

#endif /* _APIEXCEPT_H */

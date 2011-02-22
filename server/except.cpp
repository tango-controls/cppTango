static const char *RcsId = "$Header$";

//+=============================================================================
//
// file :               except.cpp
//
// description :        C++ source for all the utilities used by Tango device
//			server and mainly for the Tango class
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:16:22  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:06  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:19  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:54  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:41  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:13  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:11  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:14  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:45  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:16  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:21  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:43  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:16  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <tango_config.h>
#include <except.h>

#if ((defined WIN32) || (defined __SUNPRO_CC))
	#include <iostream>
	#include <sstream>
	#include <fstream>
#else
	#include <iostream.h>
	#include <strstream.h>
#endif

namespace Tango
{

char Except::mess[256];

//+----------------------------------------------------------------------------
//
// method : 		print_exception
// 
// description : 	This method prints the information embedded in
//			a Tango exception.
//
// in :			e : Reference to the exception object
//
//-----------------------------------------------------------------------------

void Except::print_exception(const CORBA::Exception &e)
{

	const CORBA::SystemException *se;
	const CORBA::UserException *ue;

//
// For a CORBA::SystemException, use the OB function
//

	if ((se = dynamic_cast<const CORBA::SystemException *>(&e)) != NULL)
	{
		cerr << print_CORBA_SystemException(se) << endl;
	}

//	
// If it is a CORBA::UserException
//

	else if ((ue = dynamic_cast<const CORBA::UserException *>(&e)) != NULL)
	{
		const Tango::DevFailed *te;

//				
// Print the Tango::DevFailed exception contents
//

		if ((te = dynamic_cast<const Tango::DevFailed *>(&e)) != NULL)
		{
			for (long i =0;i < te->errors.length();i++)
			{
				cerr << "Tango exception" << endl;
				cerr << "Severity = ";
				switch (te->errors[i].severity)
				{
					case Tango::WARN :
						cerr << "WARNING ";
						break;
						
					case Tango::ERR :
						cerr << "ERROR ";
						break;
					
					case Tango::PANIC :
						cerr << "PANIC ";
						break;
						
					default :
						cerr << "Unknown severity code";
						break;
				}
				cerr << endl;
				cerr << "Error reason = " << te->errors[i].reason.in() << endl;
				cerr << "Desc : " << te->errors[i].desc.in() << endl;
				cerr << "Origin : " << te->errors[i].origin.in() << endl;
				cerr << endl;
			}
		}

//		
// For an unknown CORBA::UserException
//

		else
		{
			cerr << "Unknown CORBA user exception" << endl;
		}
	}

//	
// For an unknown exception type
//

	else
	{
		cerr << "Unknown exception type !!!!!!" << endl;
	}

}

//+----------------------------------------------------------------------------
//
// method : 		print_CORBA_SystemException
// 
// description : 	This method prints the information embedded in
//			a CORBA system exception.
//
// in :			e : Pointer to the exception object
//
//-----------------------------------------------------------------------------

char *Except::print_CORBA_SystemException(const CORBA::SystemException *e)
{
	const CORBA::UNKNOWN *unk;
	const CORBA::BAD_PARAM *bad;
	const CORBA::NO_MEMORY *mem;
	const CORBA::IMP_LIMIT *lim;
	const CORBA::COMM_FAILURE *comm;
	const CORBA::INV_OBJREF *inv;
	const CORBA::NO_PERMISSION *perm;
	const CORBA::INTERNAL *inter;
	const CORBA::MARSHAL *mar;
	const CORBA::INITIALIZE *ini;
	const CORBA::NO_IMPLEMENT *impl;
	const CORBA::BAD_TYPECODE *type;
	const CORBA::BAD_OPERATION *op;
	const CORBA::NO_RESOURCES *res;
	const CORBA::NO_RESPONSE *resp;
	const CORBA::BAD_INV_ORDER *inv_ord;
	const CORBA::TRANSIENT *tra;
	const CORBA::OBJ_ADAPTER *adap;
	const CORBA::OBJECT_NOT_EXIST *not_ex;
	const CORBA::INV_POLICY *pol;

//
// It seems that omniORB for Win32 is not compiled with the RTTI enabled
// (/GR option). We can't use dynamic_casting here.
// We are using CORBA _downcast() method !!!
//

	if ((unk = CORBA::UNKNOWN::_downcast(e)) != 0)
	{
		::strcpy(mess,"UNKNOWN CORBA system exception");
	}
	else if ((bad = CORBA::BAD_PARAM::_downcast(e)) != 0)
	{
		::strcpy(mess,"BAD_PARAM CORBA system exception: ");
		::strcat(mess,bad->NP_minorString());
	}
	else if ((mem = CORBA::NO_MEMORY::_downcast(e)) != 0)
	{
		::strcpy(mess,"NO_MEMORY CORBA system exception: ");
		::strcat(mess,mem->NP_minorString());
	}
	else if ((lim = CORBA::IMP_LIMIT::_downcast(e)) != 0)
	{
		::strcpy(mess,"IMP_LIMIT CORBA system exception: ");
		::strcat(mess,lim->NP_minorString());
	}
	else if ((comm = CORBA::COMM_FAILURE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"COMM_FAILURE CORBA system exception: ");
		const char *tmp = comm->NP_minorString();
		if (tmp == NULL)
		{
			::strcat(mess,"Unknown minor code: ");
			TangoSys_MemStream st;
			st << hex << comm->minor() << dec << ends;
#if ((defined WIN32) || (defined __SUNPRO_CC))
			::strcat(mess,st.str().c_str());
#else
			::strcat(mess,st.str());
#endif
		}
		else
			::strcat(mess,tmp);
	}
	else if ((inv = CORBA::INV_OBJREF::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INV_OBJREF CORBA system exception: ");
		::strcat(mess,inv->NP_minorString());
	}
	else if ((perm = CORBA::NO_PERMISSION::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_PERMISSION CORBA system exception: ");
		::strcat(mess,perm->NP_minorString());
	}
	else if ((inter = CORBA::INTERNAL::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INTERNAL CORBA system exception: ");
		::strcat(mess,inter->NP_minorString());
	}
	else if ((mar = CORBA::MARSHAL::_downcast(e)) != NULL)
	{
		::strcpy(mess,"MARSHAL CORBA system exception: ");
		::strcat(mess,mar->NP_minorString());
	}
	else if ((ini = CORBA::INITIALIZE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INITIALIZE CORBA system exception: ");
		::strcat(mess,ini->NP_minorString());
	}
	else if ((impl = CORBA::NO_IMPLEMENT::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_IMPLEMENT CORBA system exception: ");
		::strcat(mess,impl->NP_minorString());
	}
	else if ((type = CORBA::BAD_TYPECODE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"BAD_TYPECODE CORBA system exception: ");
		::strcat(mess,type->NP_minorString());
	}
	else if ((op = CORBA::BAD_OPERATION::_downcast(e)) != NULL)
	{
		::strcpy(mess,"BAD_OPERATION CORBA system exception: ");
		::strcat(mess,op->NP_minorString());
	}
	else if ((res = CORBA::NO_RESOURCES::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_RESOURCES CORBA system exception: ");
		::strcat(mess,res->NP_minorString());
	}
	else if ((resp = CORBA::NO_RESPONSE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_RESPONSE CORBA system exception: ");
		::strcat(mess,resp->NP_minorString());
	}
	else if ((inv_ord = CORBA::BAD_INV_ORDER::_downcast(e)) != NULL)
	{
		::strcpy(mess,"BAD_INV_ORDER CORBA system exception: ");
		::strcat(mess,inv_ord->NP_minorString());
	}
	else if ((tra = CORBA::TRANSIENT::_downcast(e)) != NULL)
	{
		::strcpy(mess,"TRANSIENT CORBA system exception: ");
		const char *tmp = tra->NP_minorString();
		if (tmp == NULL)
		{
			::strcat(mess,"Unknown minor code: ");
			TangoSys_MemStream st;
			st << hex << tra->minor() << dec << ends;
#if ((defined WIN32) || (defined __SUNPRO_CC))
			::strcat(mess,st.str().c_str());
#else
			::strcat(mess,st.str());
#endif
		}
		else
			::strcat(mess,tmp);
	}
	else if ((adap = CORBA::OBJ_ADAPTER::_downcast(e)) != NULL)
	{
		::strcpy(mess,"OBJ_ADAPTER CORBA system exception: ");
		::strcat(mess,adap->NP_minorString());
	}
	else if ((not_ex = CORBA::OBJECT_NOT_EXIST::_downcast(e)) != NULL)
	{
		::strcpy(mess,"OBJECT_NOT_EXIST CORBA system exception: ");
		::strcat(mess,not_ex->NP_minorString());
	}
	else if ((pol = CORBA::INV_POLICY::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INV_POLICY CORBA system exception: ");
		::strcat(mess,pol->NP_minorString());
	}	
	else
		::strcpy(mess,"CORBA unknown system exception !!!!!!!!");
	
	return mess;
}

//+----------------------------------------------------------------------------
//
// method : 		throw_exception
// 
// description : 	These methods throws a Tango DevFailed exception from
//			a CORBA system exception.
//			Note that there is no CORBA::string_dup memory
//			for the reason field because it it is returned by
//			the print_CORBA_SystemException which in turn is
//			using an OB method (_OB_defaultToString) which
//			internally used a CORBA::String_var and its method
//			_retn() to return the string. Therefore, its
//			memory is allocated using CORBA::string_alloc.
//
// in :			e : Pointer to the exception object
//
//-----------------------------------------------------------------------------

void Except::throw_exception(const CORBA::SystemException &c_ex,const char *origin)
{
	Tango::DevErrorList errors(1);
	
	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].origin = CORBA::string_dup(origin);
	errors[0].reason = CORBA::string_dup("API_CorbaSysException");
	errors[0].desc = print_CORBA_SystemException(&c_ex);
	
	throw Tango::DevFailed(errors);
}

void Except::throw_exception(const CORBA::SystemException &c_ex,char *origin)
{
	Tango::DevErrorList errors(1);
	
	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].origin = CORBA::string_dup(origin);
	delete [] origin;
	errors[0].reason = CORBA::string_dup("API_CorbaSysException");
	errors[0].desc = print_CORBA_SystemException(&c_ex);
	
	throw Tango::DevFailed(errors);
}

void Except::throw_exception(const CORBA::SystemException &c_ex,const string &origin)
{
	Tango::DevErrorList errors(1);
	
	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].origin = CORBA::string_dup(origin.c_str());
	errors[0].reason = CORBA::string_dup("API_CorbaSysException");
	errors[0].desc = print_CORBA_SystemException(&c_ex);
	
	throw Tango::DevFailed(errors);
}

} // End of Tango namespace

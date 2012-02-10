static const char *RcsId = "$Id$\n$Name$";

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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango_config.h>
#include <except.h>
#include <apiexcept.h>

#include <iostream>
#include <sstream>
#include <fstream>

namespace Tango
{

char Except::mess[256];
omni_mutex Except::the_mutex;

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
		const Tango::NamedDevFailedList *mdf;

//
// Print the Tango::NamedDevFailedList exception contents
//

		if ((mdf = dynamic_cast<const Tango::NamedDevFailedList *>(&e)) != NULL)
		{
			cerr << "Tango NamedDevFailedList exception" << endl;
			for (unsigned long i = 0;i < mdf->err_list.size();i++)
			{
				cerr << "   Exception for object " << mdf->err_list[i].name << endl;
				cerr << "   Index of object in call (starting at 0) = " << mdf->err_list[i].idx_in_call << endl;
				for (unsigned long j =0;j < mdf->err_list[i].err_stack.length();j++)
				{
					cerr << "       Severity = ";
					switch (mdf->err_list[i].err_stack[j].severity)
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
					cerr << "       Error reason = " << mdf->err_list[i].err_stack[j].reason.in() << endl;
					cerr << "       Desc : " << mdf->err_list[i].err_stack[j].desc.in() << endl;
					cerr << "       Origin : " << mdf->err_list[i].err_stack[j].origin.in() << endl;
				}
			}
			cerr << "   Summary exception" << endl;
			for (unsigned long j =0;j < mdf->errors.length();j++)
			{
				cerr << "       Severity = ";
				switch (mdf->errors[j].severity)
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
				cerr << "       Error reason = " << mdf->errors[j].reason.in() << endl;
				cerr << "       Desc : " << mdf->errors[j].desc.in() << endl;
				cerr << "       Origin : " << mdf->errors[j].origin.in() << endl;
				cerr << endl;
			}

		}

//
// Print the Tango::DevFailed exception contents
//

		else if ((te = dynamic_cast<const Tango::DevFailed *>(&e)) != NULL)
		{
			for (unsigned long i =0;i < te->errors.length();i++)
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
		const char *err_msg = bad->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((mem = CORBA::NO_MEMORY::_downcast(e)) != 0)
	{
		::strcpy(mess,"NO_MEMORY CORBA system exception: ");
		const char *err_msg = mem->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((lim = CORBA::IMP_LIMIT::_downcast(e)) != 0)
	{
		::strcpy(mess,"IMP_LIMIT CORBA system exception: ");
		const char *tmp = lim->NP_minorString();
		if (tmp == NULL)
		{
			::strcat(mess,"Unknown minor code: ");
			TangoSys_MemStream st;
			st << hex << lim->minor() << dec << ends;
			string s = st.str();
			::strcat(mess,s.c_str());
		}
		else
			::strcat(mess,tmp);
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

			string s = st.str();
			::strcat(mess,s.c_str());
		}
		else
			::strcat(mess,tmp);
	}
	else if ((inv = CORBA::INV_OBJREF::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INV_OBJREF CORBA system exception: ");
		const char *err_msg = inv->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((perm = CORBA::NO_PERMISSION::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_PERMISSION CORBA system exception: ");
		const char *err_msg = perm->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((inter = CORBA::INTERNAL::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INTERNAL CORBA system exception: ");
		const char *err_msg = inter->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((mar = CORBA::MARSHAL::_downcast(e)) != NULL)
	{
		::strcpy(mess,"MARSHAL CORBA system exception: ");
		const char *err_msg = mar->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((ini = CORBA::INITIALIZE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INITIALIZE CORBA system exception: ");
		const char *err_msg = ini->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((impl = CORBA::NO_IMPLEMENT::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_IMPLEMENT CORBA system exception: ");
		const char *err_msg = impl->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((type = CORBA::BAD_TYPECODE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"BAD_TYPECODE CORBA system exception: ");
		const char *err_msg = type->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((op = CORBA::BAD_OPERATION::_downcast(e)) != NULL)
	{
		::strcpy(mess,"BAD_OPERATION CORBA system exception: ");
		const char *err_msg = op->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((res = CORBA::NO_RESOURCES::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_RESOURCES CORBA system exception: ");
		const char *err_msg = res->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((resp = CORBA::NO_RESPONSE::_downcast(e)) != NULL)
	{
		::strcpy(mess,"NO_RESPONSE CORBA system exception: ");
		const char *err_msg = resp->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((inv_ord = CORBA::BAD_INV_ORDER::_downcast(e)) != NULL)
	{
		::strcpy(mess,"BAD_INV_ORDER CORBA system exception: ");
		const char *err_msg = inv_ord->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
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

			string s = st.str();
			::strcat(mess,s.c_str());
		}
		else
			::strcat(mess,tmp);
	}
	else if ((adap = CORBA::OBJ_ADAPTER::_downcast(e)) != NULL)
	{
		::strcpy(mess,"OBJ_ADAPTER CORBA system exception: ");
		const char *err_msg = adap->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((not_ex = CORBA::OBJECT_NOT_EXIST::_downcast(e)) != NULL)
	{
		::strcpy(mess,"OBJECT_NOT_EXIST CORBA system exception: ");
		const char *err_msg = not_ex->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else if ((pol = CORBA::INV_POLICY::_downcast(e)) != NULL)
	{
		::strcpy(mess,"INV_POLICY CORBA system exception: ");
		const char *err_msg = pol->NP_minorString();
		if ( err_msg == NULL )
			::strcat(mess,"ORB has returned NULL pointer !");
		else
			::strcat(mess,err_msg);
	}
	else
		::strcpy(mess,"CORBA unknown system exception !!!!!!!!");

	return mess;
}

//+----------------------------------------------------------------------------
//
// method : 		print_error_stack
//
// description : 	This method prints the a Tango error stack
//
// in :			e : Reference to the error stack
//
//-----------------------------------------------------------------------------

void Except::print_error_stack(const Tango::DevErrorList &e)
{

	for (unsigned long i = 0;i < e.length();i++)
	{
		cerr << "Tango error stack" << endl;
		cerr << "Severity = ";
		switch (e[i].severity)
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
		cerr << "Error reason = " << e[i].reason.in() << endl;
		cerr << "Desc : " << e[i].desc.in() << endl;
		cerr << "Origin : " << e[i].origin.in() << endl;
		cerr << endl;
	}
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

//+----------------------------------------------------------------------------
//
// method : 		Compare two Tango DevFailed exceptions for equality
//
// description : 	The two DevFailed exceptions are verified by comparing the
// reason, origin, description and severity of all exceptions in the error stack.
// The strings reason, origin and description are compared literally.
//
// in :			ex1 The first DevFailed exception
//             ex1 The first DevFailed exception
//
// return:     a boolean set to true if the two exceptions are equal
//-----------------------------------------------------------------------------

bool Except::compare_exception(Tango::DevFailed &ex1, Tango::DevFailed &ex2)
{
	// check the length of the exception stack

	unsigned long nb_err = ex1.errors.length();
	if ( nb_err != ex2.errors.length() )
		{
		return false;
		}

	// check all exceptions in the stack

	for (unsigned long i=0; i<nb_err; i++)
		{
		// check the severity
		if ( ex1.errors[i].severity != ex2.errors[i].severity )
			{
			return false;
			}

		// check the origin
		string org1 = ex1.errors[i].origin.in();
		string org2 = ex2.errors[i].origin.in();
		if ( org1 != org2 )
			{
			return false;
			}

		// check the reason
		string re1 = ex1.errors[i].reason.in();
		string re2 = ex2.errors[i].reason.in();
		if ( re1 != re2 )
			{
			return false;
			}

		// check the description
		string desc1 = ex1.errors[i].desc.in();
		string desc2 = ex2.errors[i].desc.in();
		if ( desc1 != desc2 )
			{
			return false;
			}
		}

	return true;
}

} // End of Tango namespace

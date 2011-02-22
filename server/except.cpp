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

#include <except.h>

namespace Tango
{

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
				cerr << "Error reason = " << te->errors[i].reason << endl;
				cerr << "Desc : " << te->errors[i].desc << endl;
				cerr << "Origin : " << te->errors[i].origin << endl;
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
	
	if ((unk = dynamic_cast<const CORBA::UNKNOWN *>(e)) != NULL)
	{
		return unk->_OB_defaultToString("UNKNOWN");
	}
	else if ((bad = dynamic_cast<const CORBA::BAD_PARAM *>(e)) != NULL)
	{
		return bad->_OB_defaultToString("BAD_PARAM");
	}
	else if ((mem = dynamic_cast<const CORBA::NO_MEMORY *>(e)) != NULL)
	{
		return mem->_OB_defaultToString("NO_MEMORY");
	}
	else if ((lim = dynamic_cast<const CORBA::IMP_LIMIT *>(e)) != NULL)
	{
		return lim->_OB_defaultToString("IMP_LIMIT");
	}
	else if ((comm = dynamic_cast<const CORBA::COMM_FAILURE *>(e)) != NULL)
	{
		return comm->_OB_defaultToString("COMM_FAILURE");
	}
	else if ((inv = dynamic_cast<const CORBA::INV_OBJREF *>(e)) != NULL)
	{
		return inv->_OB_defaultToString("INV_OBJREF");
	}
	else if ((perm = dynamic_cast<const CORBA::NO_PERMISSION *>(e)) != NULL)
	{
		return perm->_OB_defaultToString("NO_PERMISSION");
	}
	else if ((inter = dynamic_cast<const CORBA::INTERNAL *>(e)) != NULL)
	{
		return inter->_OB_defaultToString("INTERNAL");
	}
	else if ((mar = dynamic_cast<const CORBA::MARSHAL *>(e)) != NULL)
	{
		return mar->_OB_defaultToString("MARSHAL");
	}
	else if ((ini = dynamic_cast<const CORBA::INITIALIZE *>(e)) != NULL)
	{
		return ini->_OB_defaultToString("INITIALIZE");
	}
	else if ((impl = dynamic_cast<const CORBA::NO_IMPLEMENT *>(e)) != NULL)
	{
		return impl->_OB_defaultToString("NO_IMPLEMENT");
	}
	else if ((type = dynamic_cast<const CORBA::BAD_TYPECODE *>(e)) != NULL)
	{
		return type->_OB_defaultToString("BAD_TYPECODE");
	}
	else if ((op = dynamic_cast<const CORBA::BAD_OPERATION *>(e)) != NULL)
	{
		return op->_OB_defaultToString("BAD_OPERATION");
	}
	else if ((res = dynamic_cast<const CORBA::NO_RESOURCES *>(e)) != NULL)
	{
		return res->_OB_defaultToString("NO_RESOURCES");
	}
	else if ((resp = dynamic_cast<const CORBA::NO_RESPONSE *>(e)) != NULL)
	{
		return resp->_OB_defaultToString("NO_RESPONSE");
	}
	else if ((inv_ord = dynamic_cast<const CORBA::BAD_INV_ORDER *>(e)) != NULL)
	{
		return inv_ord->_OB_defaultToString("BAD_INV_ORDER");
	}
	else if ((tra = dynamic_cast<const CORBA::TRANSIENT *>(e)) != NULL)
	{
		return tra->_OB_defaultToString("TRANSIENT");
	}
	else if ((adap = dynamic_cast<const CORBA::OBJ_ADAPTER *>(e)) != NULL)
	{
		return adap->_OB_defaultToString("OBJ_ADAPTER");
	}
	else if ((not_ex = dynamic_cast<const CORBA::OBJECT_NOT_EXIST *>(e)) != NULL)
	{
		return not_ex->_OB_defaultToString("OBJECT_NOT_EXIST");
	}
	else if ((pol = dynamic_cast<const CORBA::INV_POLICY *>(e)) != NULL)
	{
		return pol->_OB_defaultToString("INV_POLOCY");
	}
	
	return "CORBA unknown system exception !!!!!!!!";
}

//+----------------------------------------------------------------------------
//
// method : 		print_CORBA_SystemException
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
	errors[0].desc = CORBA::string_dup("API_CorbaSysException");
	errors[0].reason = print_CORBA_SystemException(&c_ex);
	
	throw Tango::DevFailed(errors);
}

void Except::throw_exception(const CORBA::SystemException &c_ex,char *origin)
{
	Tango::DevErrorList errors(1);
	
	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].origin = CORBA::string_dup(origin);
	delete [] origin;
	errors[0].desc = CORBA::string_dup("API_CorbaSysException");
	errors[0].reason = print_CORBA_SystemException(&c_ex);
	
	throw Tango::DevFailed(errors);
}

void Except::throw_exception(const CORBA::SystemException &c_ex,string &origin)
{
	Tango::DevErrorList errors(1);
	
	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].origin = CORBA::string_dup(origin.c_str());
	errors[0].desc = CORBA::string_dup("API_CorbaSysException");
	errors[0].reason = print_CORBA_SystemException(&c_ex);
	
	throw Tango::DevFailed(errors);
}

} // End of Tango namespace

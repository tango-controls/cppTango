static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               apiexcept.cpp
//
// description :        C++ source code for the NamedDevFailed and
//			MultiObjDevFailed classes.
//			These classes are used for exception thrown by the
//			write_attributes call where you can have one exception
//			for several devices which failed during the call
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/08/21 06:58:41  taurel
// Initial commit
//
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <apiexcept.h>

namespace Tango
{

NamedDevFailed::NamedDevFailed()
{
}


//+-------------------------------------------------------------------------
//
// method : 		NamedDevFailedList::NamedDevFailedList
// 
// description : 	Constructor for the NamedDevFailedList class.
//			This exception is constructed from the CORBA
//			MultiDevFailed exception. Remember that CORBA does not
//			support inheriting for exceptions.
//			Within this exception, you find one NamedDevFailed
//			object for each device which failed and one DevFailed
//			object to summarize the exception
//
// argument : in : - corba_ex : The CORBA exception
//		   - dev_name : The device name
//		   - op_name : The API CORBA operation interface where  the
//			       exception has been catched
//		   - reason : The summary DevFailed exception reason field	
//
//--------------------------------------------------------------------------


NamedDevFailedList::NamedDevFailedList(const Tango::MultiDevFailed &corba_ex,
				       string dev_name,
				       const char *op_name,
				       const char *reason)
{
	unsigned int i;
	unsigned long nb_obj_failed = corba_ex.errors.length();
	
	for (i = 0;i < nb_obj_failed;i++)
	{
		NamedDevFailed tmp_err(corba_ex.errors[i].err_list,
				       corba_ex.errors[i].name.in(),
				       corba_ex.errors[i].index_in_call);
		err_list.push_back(tmp_err);
	}

//
// Build an exception summary (as string) in the DevFailed part of this
// exception.
//
	
	TangoSys_OMemStream desc;
	desc << "Failed to execute " << op_name << " on device " << dev_name;
	desc << ", object(s) ";
	for (i = 0;i < nb_obj_failed;i++)
	{
		desc << corba_ex.errors[i].name;
		if (i != nb_obj_failed - 1)
			desc << ", ";
	}
	desc << ends;

	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].reason = CORBA::string_dup(reason);
	errors[0].origin = CORBA::string_dup(op_name);
	
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
	string st = desc.str();
	errors[0].desc = CORBA::string_dup(st.c_str());
#else
	char *tmp_str = desc.str();
	errors[0].desc = CORBA::string_dup(tmp_str);
	delete[]tmp_str;
#endif
}


} // End of Tango namespace

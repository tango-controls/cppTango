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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
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
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <apiexcept.h>

namespace Tango
{

NamedDevFailed::NamedDevFailed():idx_in_call(0)
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

	string st = desc.str();
	errors[0].desc = CORBA::string_dup(st.c_str());
}


} // End of Tango namespace

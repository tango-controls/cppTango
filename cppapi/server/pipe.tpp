
//+==================================================================================================================
//
// file :               Pipe.tpp
//
// description :        C++ source code for the template methods of the Pipe class.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2014
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision: 25781 $
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

namespace Tango
{

#ifdef HAS_VARIADIC_TEMPLATE
template <typename T,typename ... Args>
void Pipe::set_value(vector<string> &blob_elt_names,T *val,Args ...args)
{
	cout << "In set_value variadic template method: " << sizeof...(args) << " data in pack, rec_count = " << rec_count << endl;

//
// If it is the first call for the set_value() method (variadic template) check that the nb of args is coherent with
// the data_elt names
// Also copy the data elements name
//

	if (rec_count == 0)
	{
		unsigned int nb_vargs = sizeof...(args);
		if (nb_vargs + 1 != blob_elt_names.size())
		{
			stringstream ss;
			ss << "Data blob for pipe " << name << " is supposed to have " << blob_elt_names.size() << " data elements but only ";
			ss << nb_vargs + 1 << " argument(s) given to set_value() method";

			Tango::Except::throw_exception(API_PipeWrongArgNumber,ss.str(),"Pipe::set_value()");
		}

		pe_out_names = blob_elt_names;
		ret_data->data_blob.length(pe_out_names.size());
	}

	set_value(val,v_dep[rec_count].size,v_dep[rec_count].rel);
	rec_count++;
	set_value(blob_elt_names,args...);
}
#endif // HAS_VARIADIC_TEMPLATE

} // End of Tango namespace

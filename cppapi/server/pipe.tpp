
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

	if (rec_count == 0)
		pe_out_names = blob_elt_names;

	set_value(val);
	rec_count++;
	set_value(blob_elt_names,args...);
}
#endif // HAS_VARIADIC_TEMPLATE

} // End of Tango namespace

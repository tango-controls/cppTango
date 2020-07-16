//+===================================================================================================================
//
// file :               w_attrsetval.tpp
//
// description :        C++ source code for the WAttribute class template methods
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2014,2015
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
// $Revision: 17240 $
//
//-===================================================================================================================

#ifndef _WATTRSETVAL_TPP
#define _WATTRSETVAL_TPP

#include <type_traits>

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		WAttribute::get_write_value()
//
// description :
//		Get value written by the client. These two templated methods are used for enumerated attribute
//
// args :
//		out :
// 			- data : User data to be initialized
//
//-----------------------------------------------------------------------------------------------------------------


template <typename T>
void WAttributePrivate::get_write_value(T &data)
{
//
// First some check on user type
//

	T dum;
	check_type(dum,"WAttribute::get_write_value");

//
// Then, init user data
//

	data = static_cast<T>(short_val);
}


template <typename T>
void WAttributePrivate::get_write_value(const T *&ptr)
{
//
// First some check on user type
//

	T dum;
	check_type(dum,"WAttribute::get_write_value");

//
// Then, init user data
//

	ptr = (const T *)short_ptr;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		WAttribute::check_type()
//
// description :
//		Check user type given to the template get_write_value() method
//
// args :
//		in :
// 			- dummy : Dummy data used to pass type
//			- origin : Calling method name (for exception)
//
//-----------------------------------------------------------------------------------------------------------------

template <typename T>
void WAttributePrivate::check_type(T &TANGO_UNUSED(dummy), const std::string &origin)
{
	bool short_enum = std::is_same<short,typename std::underlying_type<T>::type>::value;
	bool uns_int_enum = std::is_same<unsigned int,typename std::underlying_type<T>::type>::value;

	if (short_enum == false && uns_int_enum == false)
	{
		std::stringstream ss;
		ss << "Invalid enumeration type. Supported types are C++11 scoped enum with short as underlying data type\n";
		ss << "or old enum";

		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),origin);
	}

//
// Check if the input type is an enum and if it is from the valid type
//

	if (std::is_enum<T>::value == false)
	{
		Except::throw_exception(API_IncompatibleArgumentType,
								"The input argument data type is not an enumeration",origin);
	}

	Tango::DeviceClass *dev_class;
	try
	{
		DeviceImpl *dev = get_att_device();
		dev_class = dev->get_device_class();
	}
	catch (Tango::DevFailed &e)
	{
		std::string reas = e.errors[0].reason.in();
		if (reas == API_DeviceNotFound)
		{
			Util *tg = Util::instance();
			const std::vector<Tango::DeviceClass *> *cl_list_ptr = tg->get_class_list();
			dev_class = (*cl_list_ptr)[cl_list_ptr->size() - 2];
		}
		else
			throw;
	}

	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);

	if (att.same_type(typeid(T)) == false)
	{
		std::stringstream ss;
		ss << "Invalid enumeration type. Requested enum type is " << att.get_enum_type();
		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),origin);
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		WAttribute::set_write_value()
//
// description :
//		Set written part of a WAttribute. These two templated methods are used for enumerated attribute
//
// args :
//		in :
// 			- val : Data to be set as written value
//
//-----------------------------------------------------------------------------------------------------------------

template <typename T>
void WAttributePrivate::set_write_value(T val)
{
	check_type(val,"WAttribute::set_write_value");

	CORBA::Any tmp_any;
	Tango::DevVarShortArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = (short)val;

	tmp_any <<= tmp_seq;

	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

template <typename T>
void WAttributePrivate::set_write_value(T *val, long x, long y)
{
	T dum;
	check_type(dum,"WAttribute::set_write_value");

	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	short *ptr = new short [nb_data];
	for (int i = 0;i < nb_data;i++)
		ptr[i] = (short)val[i];

	try
	{
		Tango::DevVarShortArray tmp_seq(nb_data,nb_data,ptr,false);

		CORBA::Any 	tmp_any;
		tmp_any <<= tmp_seq;
		check_written_value(tmp_any, x, y);
		copy_data(tmp_any);
		set_user_set_write_value(true);

		delete [] ptr;
	}
	catch (...)
	{
		delete [] ptr;
		throw;
	}
}

template <typename T>
void WAttributePrivate::set_write_value(std::vector<T> &val, long x, long y)
{
	T dum;
	check_type(dum,"WAttribute::set_write_value");

	short *ptr = new short [val.size()];
	for (unsigned int i = 0;i < val.size();i++)
		ptr[i] = (short)val[i];

	try
	{
		Tango::DevVarShortArray tmp_seq(val.size(),val.size(),ptr,false);

		CORBA::Any 	tmp_any;
		tmp_any <<= tmp_seq;

		check_written_value(tmp_any, x, y);
		copy_data(tmp_any);
		set_user_set_write_value(true);

		delete [] ptr;
	}
	catch (...)
	{
		delete [] ptr;
		throw;
	}
}

} // End of Tango namespace
#endif // _WATTRSETVAL_TPP

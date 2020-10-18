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

inline DevShort WAttributePrivate::get_write_value_enum_impl(const std::type_info& ti)
{
	check_enum_type(ti, "WAttribute::get_write_value");
	return short_val;
}

inline const DevShort* WAttributePrivate::get_write_value_ptr_enum_impl(const std::type_info& ti)
{
	check_enum_type(ti, "WAttribute::get_write_value");
	return short_ptr;
}

inline void WAttributePrivate::check_enum_type(const std::type_info& ti, const std::string &origin)
{
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

	if (att.same_type(ti) == false)
	{
		std::stringstream ss;
		ss << "Invalid enumeration type. Requested enum type is " << att.get_enum_type();
		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),origin);
	}
}

inline void WAttributePrivate::set_write_value_enum_impl(const std::type_info& ti, short val)
{
	check_enum_type(ti, "WAttribute::set_write_value");

	CORBA::Any tmp_any;
	Tango::DevVarShortArray tmp_seq(1);
	tmp_seq.length(1);
	tmp_seq[0] = val;

	tmp_any <<= tmp_seq;

	check_written_value(tmp_any,1,0);
	copy_data(tmp_any);
	set_user_set_write_value(true);
}

inline void WAttributePrivate::set_write_value_enum_impl(
	const std::type_info& ti,
	const std::function<short(std::size_t)>& get_enum_value,
	long x, long y)
{
	check_enum_type(ti, "WAttribute::set_write_value");

	long nb_data;

	if (y == 0)
		nb_data = x;
	else
		nb_data = x * y;

	short *ptr = new short [nb_data];
	for (int i = 0;i < nb_data;i++)
		ptr[i] = get_enum_value(i);

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

inline void WAttributePrivate::set_write_value_enum_impl(
	const std::type_info& ti,
	const std::function<short(std::size_t)>& get_enum_value,
	std::size_t data_length,
	long x, long y)
{
	check_enum_type(ti, "WAttribute::set_write_value");

	short *ptr = new short [data_length];
	for (unsigned int i = 0;i < data_length;i++)
		ptr[i] = get_enum_value(i);

	try
	{
		Tango::DevVarShortArray tmp_seq(data_length, data_length, ptr, false);

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

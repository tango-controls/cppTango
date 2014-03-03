//+===================================================================================================================
//
// file :               attsetval.tpp
//
// description :        C++ source code for the Attribute class template methods when they are not specialized and
//						related to attribute value setting
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
// $Revision: 17240 $
//
//-===================================================================================================================

#ifndef _ATTRSETVAL_TPP
#define _ATTRSETVAL_TPP

#ifdef HAS_TYPE_TRAITS
	#include <type_traits>
#endif

namespace Tango
{

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_value
//
// description :
//		Set the attribute read value and quality. This method automatically set the date when it has been called
//		This method is overloaded several times for all the supported attribute data type. Nevertheless, one
//		template method is defined (this one) which willbe called for all data types with no overload.
//		This is the case for enumeration data type
//
// argument :
// 		in :
//			- enum_ptr : The attribute read value
//			- x : The attribute x dimension (default is 1)
//			- y : The atttribute y dimension (default is 0)
//			- release : A flag set to true if memory must be de-allocated (default is false)
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
void Attribute::set_value(T *enum_ptr,long x,long y,bool release)
{
//
// Throw exception if attribute data type is not correct
//

	if (data_type != Tango::DEV_ENUM)
	{
		SAFE_DELETE(enum_ptr);

		stringstream o;
		o << "Invalid data type for attribute " << name << ends;

		Except::throw_exception(API_AttrOptProp,o.str(),"Attribute::set_value()");
	}

#ifdef HAS_UNDERLYING
	bool short_enum = is_same<short,typename underlying_type<T>::type>::value;
	bool uns_int_enum = is_same<unsigned int,typename underlying_type<T>::type>::value;

	if (short_enum == false && uns_int_enum == false)
	{
		SAFE_DELETE(enum_ptr);

		stringstream ss;
		ss << "Invalid enumeration type. Supported types are C++11 scoped enum with short as underlying data type\n";
		ss << "or old enum";

		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),"Attribute::set_value()");
	}
#endif // HAS_UNDERLYING

//
// Check if the input type is an enum and if it is from the valid type
//

#ifdef HAS_TYPE_TRAITS
	if (is_enum<T>::value == false)
	{
		SAFE_DELETE(enum_ptr);
		Except::throw_exception(API_IncompatibleArgumentType,
								"The input argument data type is not an enumeration",
								"Attribute::set_value()");
	}
#endif // HAS_TYPE_TRAITS

	DeviceImpl *dev=get_att_device();
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);

	if (att.same_type(typeid(T)) == false)
	{
		SAFE_DELETE(enum_ptr);

		stringstream ss;
		ss << "Invalid enumeration type. Requested enum type is " << att.get_enum_type();
		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(enum_ptr);

		stringstream o;
		o << "Data size for attribute " << name << " exceeds given limit" << ends;

		Except::throw_exception(API_AttrOptProp,o.str(),"Attribute::set_value()");
	}

//
// Compute data size and set default quality to valid.
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// Throw exception if pointer is null and data_size != 0
//

	if (data_size != 0)
	{
		CHECK_PTR(enum_ptr,name);
	}

//
// If the data is wanted from the DevState command, store it in a sequence. If the attribute  has an associated
// writable attribute, store data in a temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	enum_sh = static_cast<short>(*enum_ptr);
//	enum_sh = *enum_ptr;
	short *p_data = &enum_sh;

	if (date == false)
	{
		value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_sh[0] = *p_data;
				SAFE_DELETE(p_data);
			}
			else
			{
				value.sh_seq = new Tango::DevVarShortArray(data_size);
				value.sh_seq->length(data_size);
				::memcpy(value.sh_seq->get_buffer(false),p_data,data_size * sizeof(Tango::DevShort));
				if (release == true)
					delete [] p_data;
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				Tango::DevShort *tmp_ptr = new Tango::DevShort[1];
				*tmp_ptr = *p_data;
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,tmp_ptr,release);
				if (is_fwd_att() == true)
					delete [] p_data;
				else
					delete p_data;
			}
			else
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Reset alarm flags
//

	alarm.reset();

//
// Get time
//

	set_time();
}

} // End of Tango namespace
#endif // _ATTRSETVAL_TPP

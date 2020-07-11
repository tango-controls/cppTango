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

#ifndef _ATTRSETVAL_TPP
#define _ATTRSETVAL_TPP

#include <type_traits>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */

namespace Tango
{

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::set_value
//
// description :
//		Set the attribute read value and quality. This method automatically set the date when it has been called
//
//		This method is overloaded several times for all the supported attribute data type. Nevertheless, one
//		template method is defined (this one) which will be called for all data types with no overload.
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

		std::stringstream o;
		o << "Invalid data type for attribute " << name << std::ends;

		Except::throw_exception(API_AttrOptProp,o.str(),"Attribute::set_value()");
	}

	bool short_enum = std::is_same<short,typename std::underlying_type<T>::type>::value;
	bool uns_int_enum = std::is_same<unsigned int,typename std::underlying_type<T>::type>::value;

	if (short_enum == false && uns_int_enum == false)
	{
		SAFE_DELETE(enum_ptr);

		std::stringstream ss;
		ss << "Invalid enumeration type. Supported types are C++11 scoped enum with short as underlying data type\n";
		ss << "or old enum";

		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),"Attribute::set_value()");
	}

//
// Check if the input type is an enum and if it is from the valid type
//

	if (std::is_enum<T>::value == false)
	{
		SAFE_DELETE(enum_ptr);
		Except::throw_exception(API_IncompatibleArgumentType,
								"The input argument data type is not an enumeration",
								"Attribute::set_value()");
	}

//
// Check if enum labels are defined
//

	if (enum_labels.size() == 0)
	{
		SAFE_DELETE(enum_ptr);

		std::stringstream ss;
		ss << "Attribute " << name << " data type is enum but no enum labels are defined!";

		Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::set_value()");
	}

//
// Check enum type
//

	DeviceImpl *dev = get_att_device();
	Tango::DeviceClass *dev_class = dev->get_device_class();
	Tango::MultiClassAttribute *mca = dev_class->get_class_attr();
	Tango::Attr &att = mca->get_attr(name);

	if (att.same_type(typeid(T)) == false)
	{
		SAFE_DELETE(enum_ptr);

		std::stringstream ss;
		ss << "Invalid enumeration type. Requested enum type is " << att.get_enum_type();
		Except::throw_exception(API_IncompatibleArgumentType,ss.str(),"Attribute::set_value()");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		SAFE_DELETE(enum_ptr);

		std::stringstream o;
		o << "Data size for attribute " << name << " exceeds given limit" << std::ends;

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

	if (data_size > enum_nb)
	{
		if (enum_nb != 0)
			delete [] loc_enum_ptr;
		loc_enum_ptr = new short [data_size];
		enum_nb = data_size;
	}

	short max_val = (short)enum_labels.size() - 1;
	for (int i = 0;i < data_size;i++)
	{
		loc_enum_ptr[i] = (short)enum_ptr[i];
		if (loc_enum_ptr[i] < 0 || loc_enum_ptr[i] > max_val)
		{
			SAFE_DELETE(enum_ptr);
			enum_nb = 0;

			std::stringstream ss;
			ss << "Wrong value for attribute " << name;
			ss << ". Element " << i << " (value = " << loc_enum_ptr[i] << ") is negative or above the limit defined by the enum (" << max_val << ").";
			delete [] loc_enum_ptr;

			Except::throw_exception(API_AttrOptProp,ss.str(),"Attribute::set_value()");
		}
	}

	SAFE_DELETE(enum_ptr);

	if (date == false)
	{
		value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,loc_enum_ptr,false);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			if (data_format == Tango::SCALAR)
			{
				tmp_sh[0] = *loc_enum_ptr;
			}
			else
			{
				value.sh_seq = new Tango::DevVarShortArray(data_size);
				value.sh_seq->length(data_size);
				::memcpy(value.sh_seq->get_buffer(false),loc_enum_ptr,data_size * sizeof(Tango::DevShort));
			}
		}
		else
		{
			if ((data_format == Tango::SCALAR) && (release == true))
			{
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,loc_enum_ptr,false);
			}
			else
				value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,loc_enum_ptr,release);
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

template <typename T>
void Attribute::set_value_date_quality(T *p_data,time_t t,Tango::AttrQuality qual,long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}

#ifdef _TG_WINDOWS_
template <typename T>
void Attribute::set_value_date_quality(T *p_data,struct _timeb &t,Tango::AttrQuality qual,long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#else
template <typename T>
void Attribute::set_value_date_quality(T *p_data,struct timeval &t,Tango::AttrQuality qual,long x,long y,bool release)
{
	set_value(p_data,x,y,release);
	set_quality(qual,false);
	set_date(t);

	if (qual == Tango::ATTR_INVALID)
	{
		if (!((is_writ_associated() == true) && (data_format == Tango::SCALAR)))
			delete_seq();
	}
}
#endif

} // End of Tango namespace
#endif // _ATTRSETVAL_TPP

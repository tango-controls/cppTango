//+===================================================================================================================
//
// file :               devapi_att.tpp
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

#ifndef _DEVAPI_ATTR_TPP
#define _DEVAPI_ATTR_TPP

#ifdef HAS_TYPE_TRAITS
	#include <type_traits>
#endif

namespace Tango
{

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceAttribute::DeviceAttribute
//
// description :
//		The DeviceAttribute class constructors used for enumeration. They are implemented as template methods
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
DeviceAttribute::DeviceAttribute(string &_name,T _val):ext(new DeviceAttributeExt)
{
	name = _name;
	base_val(_val);
}

template <typename T>
DeviceAttribute::DeviceAttribute(const char *_name,T _val):ext(new DeviceAttributeExt)
{
	name = _name;
	base_val(_val);
}

template <typename T>
void DeviceAttribute::base_val(T _val)
{
	dim_x = 1;
	dim_y = 0;
	w_dim_x = 0;
	w_dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(1);
	ShortSeq[0] = static_cast<short>(_val);
}

//-----------------------------------------------------------------------------------------------------------------

template <typename T>
DeviceAttribute::DeviceAttribute(string &_name,vector<T> &_val):ext(new DeviceAttributeExt)
{
	name = _name;
	dim_x = _val.size();
	base_vect(_val);
}

template <typename T>
DeviceAttribute::DeviceAttribute(const char *_name,vector<T> &_val):ext(new DeviceAttributeExt)
{
	name = _name;
	dim_x = _val.size();
	base_vect(_val);
}

template <typename T>
void DeviceAttribute::base_vect(vector<T> &_val)
{
	dim_y = 0;
	w_dim_x = 0;
	w_dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(_val.size());
	for (size_t loop = 0;loop < _val.size();loop++)
		ShortSeq[loop] = static_cast<short>(_val[loop]);
}

//-----------------------------------------------------------------------------------------------------------------

template <typename T>
DeviceAttribute::DeviceAttribute(string &_name,vector<T> &_val,int _x,int _y):ext(new DeviceAttributeExt)
{
	name = _name;
	dim_x = _x;
	dim_y = _y;
	base_vect_size(_val);
}

template <typename T>
DeviceAttribute::DeviceAttribute(const char *_name,vector<T> &_val,int _x,int _y):ext(new DeviceAttributeExt)
{
	name = _name;
	dim_x = _x;
	dim_y = _y;
	base_vect_size(_val);
}

template <typename T>
void DeviceAttribute::base_vect_size(vector<T> &_val)
{
	w_dim_x = 0;
	w_dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;
	d_state_filled = false;
	exceptions_flags.set(failed_flag);
	exceptions_flags.set(isempty_flag);
	ShortSeq = new(DevVarShortArray);
	ShortSeq->length(_val.size());
	for (size_t loop = 0;loop < _val.size();loop++)
		ShortSeq[loop] = static_cast<short>(_val[loop]);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceAttribute::operator >>
//
// description :
//		Extractor method used for enumeration. It is implemented as a template method
//
// argument :
// 		out :
//			- datum : The enumeration to be filled in
//
//-------------------------------------------------------------------------------------------------------------------


template <typename T>
bool DeviceAttribute::operator >> (T &datum)
{
	bool ret = true;

//
// Some check on provided type (for modern compiler...)
//

	ret = template_type_check(datum);

//
// check for available data
//

	if (ret == true)
	{
		ret = check_for_data();
		if (ret == false)
			return false;

		if (ShortSeq.operator->() != NULL)
		{
			if (ShortSeq->length() != 0)
				datum = static_cast<T>(ShortSeq[0]);
			else
				ret = false;
		}
		else
		{
			// check the wrongtype_flag
			ret = check_wrong_type_exception();
		}
	}

	return ret;
}

template <typename T>
bool DeviceAttribute::operator >> (vector<T> &datum)
{
	bool ret = true;

//
// Some check on provided type (for modern compiler...)
//

	T dummy;
	ret = template_type_check(dummy);

//
// check for available data
//

	ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			datum.resize(ShortSeq->length());
			for (size_t i=0; i<ShortSeq->length(); i++)
				datum[i] = static_cast<T>(ShortSeq[i]);
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

template <typename T>
bool DeviceAttribute::extract_read (vector<T> &_data)
{
	bool ret = true;

//
// Some check on provided type (for modern compiler...)
//

	T dummy;
	ret = template_type_check(dummy);

//
// check for available data
//

	ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			size_t length = get_nb_read();
			_data.resize(length);
         	for (size_t i=0; i<length; i++)
         		_data[i] = static_cast<T>(ShortSeq[i]);
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

template <typename T>
bool DeviceAttribute::extract_set(vector<T> &_data)
{
	bool ret = true;

//
// Some check on provided type (for modern compiler...)
//

	T dummy;
	ret = template_type_check(dummy);

//
// check for available data
//

	ret = check_for_data();
	if ( ret == false)
		return false;

	if (ShortSeq.operator->() != NULL)
	{
		if (ShortSeq->length() != 0)
		{
			// check the size of the setpoint values
			long read_length = check_set_value_size (ShortSeq->length());

			// copy the set point values to the vector
			_data.resize(ShortSeq->length() - read_length);
			unsigned int k = 0;
         	for (unsigned int i=read_length; i<ShortSeq->length(); i++, k++)
         		_data[k] = static_cast<T>(ShortSeq[i]);
		}
		else
			ret = false;
	}
	else
	{
		// check the wrongtype_flag
		ret = check_wrong_type_exception();
	}
	return ret;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceAttribute::operator <<
//
// description :
//		Insertor method used for enumeration. It is implemented as a template method
//
// argument :
// 		out :
//			- datum : The enumeration to be filled in
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
void DeviceAttribute::operator << (T datum)
{
	template_type_check(datum);

	dim_x = 1;
	dim_y = 0;
	w_dim_x = 0;
	w_dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

    DevVarShortArray *short_vararr = new(DevVarShortArray);
    short_vararr->length(1);
    (*short_vararr)[0] = static_cast<short>(datum);
    ShortSeq = short_vararr;

	del_mem(Tango::DEV_SHORT);
}

template <typename T>
void DeviceAttribute::operator << (vector<T> &_datum)
{
	T dummy;
	template_type_check(dummy);

	dim_x = _datum.size();
	dim_y = 0;
	w_dim_x = 0;
	w_dim_y = 0;
	quality = Tango::ATTR_VALID;
	data_format = Tango::FMT_UNKNOWN;

	if (ShortSeq.operator->() == NULL)
	{
		DevVarShortArray *short_vararr = new(DevVarShortArray);
		ShortSeq = short_vararr;
	}
	ShortSeq->length(_datum.size());
	for (size_t loop = 0;loop < _datum.size();loop++)
		ShortSeq[loop] = static_cast<short>(_datum[loop]);

	del_mem(Tango::DEV_SHORT);
}

template <typename T>
void DeviceAttribute::insert(vector<T> &_datum,int _x,int _y)
{
	*this << _datum;
	dim_x = _x;
	dim_y = _y;
}

template <typename T>
bool DeviceAttribute::template_type_check(T &_datum)
{
#ifdef HAS_UNDERLYING
	bool short_enum = is_same<short,typename underlying_type<T>::type>::value;
	bool uns_int_enum = is_same<unsigned int,typename underlying_type<T>::type>::value;

	if (short_enum == false && uns_int_enum == false)
	{
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception("API_IncompatibleAttrArgumentType",
			"Type provided for insertion is not a valid enum. Only C enum or C++11 enum with underlying short data type are supported",
			"DeviceAttribute::operator<<");
		}

		return false;
	}
#endif // HAS_UNDERLYING

#ifdef HAS_TYPE_TRAITS
	if (is_enum<T>::value == false)
	{
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception("API_IncompatibleAttrArgumentType",
			"Type provided for insertion is not an enumeration",
			"DeviceAttribute::operator<<");
		}

		return false;
	}
#endif // HAS_TYPE_TRAITS

	return true;
}

} // End of Tango namespace
#endif // _DEVAPI_ATTR_TPP

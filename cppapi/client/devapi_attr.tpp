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

#ifndef _DEVAPI_ATTR_TPP
#define _DEVAPI_ATTR_TPP


namespace Tango
{

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

#ifdef HAS_UNDERLYING
	bool short_enum = is_same<short,typename underlying_type<T>::type>::value;
	bool uns_int_enum = is_same<unsigned int,typename underlying_type<T>::type>::value;

	if (short_enum == false && uns_int_enum == false)
	{
		ret = check_wrong_type_exception();
	}
#endif // HAS_UNDERLYING

#ifdef HAS_TYPE_TRAITS
	if (is_enum<T>::value == false)
	{
		ret = check_wrong_type_exception();
	}
#endif // HAS_TYPE_TRAITS

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

//
// Some check on provided type (for modern compiler...)
//

#ifdef HAS_UNDERLYING
	bool short_enum = is_same<short,typename underlying_type<T>::type>::value;
	bool uns_int_enum = is_same<unsigned int,typename underlying_type<T>::type>::value;

	if (short_enum == false && uns_int_enum == false)
	{
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception("API_IncompatibleAttrArgumentType",
			"Type provided for insertion is not a valid enum. Only C enum or C++11 enum with underlying short data type are supported",
			"DeviceAttribute::operator>>");
		}
	}
#endif // HAS_UNDERLYING

#ifdef HAS_TYPE_TRAITS
	if (is_enum<T>::value == false)
	{
		if (exceptions_flags.test(wrongtype_flag))
		{
			ApiDataExcept::throw_exception("API_IncompatibleAttrArgumentType",
			"Type provided for insertion is not an enumeration",
			"DeviceAttribute::operator>>");
		}
	}
#endif // HAS_TYPE_TRAITS

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

} // End of Tango namespace
#endif // _DEVAPI_ATTR_TPP

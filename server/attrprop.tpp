//+============================================================================
//
// file :               AttrProp.tpp
//
// description :        C++ source code for the MultiAttrProp class template
//                      methods
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012
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
// $Revision: 17240 $
//
//-============================================================================

#ifndef _ATTRPROP_TPP
#define _ATTRPROP_TPP

namespace Tango
{

//
// AttrProp template specialisation for DevUChar
//

template<>
inline AttrProp<DevUChar>::AttrProp(const DevUChar &value) : val(value), is_value(true), ext(Tango_NullPtr)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	st << (short)value; // to represent the numeric value
	str = st.str();
}

template<>
inline AttrProp<DevUChar>& AttrProp<DevUChar>::operator=(const DevUChar &value)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	st << (short)value; // to represent the numeric value
	str = st.str();
	val = value;
	is_value = true;
	return *this;
}

template<>
inline void AttrProp<DevUChar>::set_val(const DevUChar &value)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	st << (short)value; // to represent the numeric value
	str = st.str();
	val = value;
	is_value = true;
}

//
// DoubleAttrProp template specialisation for DevUChar
//

template<>
inline DoubleAttrProp<DevUChar>::DoubleAttrProp(const vector<DevUChar> &values) : val(values), is_value(true)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	for(size_t i = 0; i < values.size(); i++)
	{
		if(i > 0)
			st << ",";
		st << (short)values[i]; // to represent the numeric value
	}
	str = st.str();
}

template<>
inline DoubleAttrProp<DevUChar>::DoubleAttrProp(const DevUChar &value) : is_value(true) {
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	st << (short)value; // to represent the numeric value
	str = st.str();
	val.push_back(value);
}

template<>
inline DoubleAttrProp<DevUChar>& DoubleAttrProp<DevUChar>::operator=(const vector<DevUChar> &values)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	for(size_t i = 0; i < values.size(); i++)
	{
		if(i > 0)
			st << ",";
		st << (short)values[i]; // to represent the numeric value
	}
	str = st.str();
	val = values;
	is_value = true;
	return *this;
}

template<>
inline DoubleAttrProp<DevUChar>& DoubleAttrProp<DevUChar>::operator=(const DevUChar &value)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	st << (short)value; // to represent the numeric value
	str = st.str();
	val.push_back(value);
	is_value = true;
	return *this;
}

template<>
inline void DoubleAttrProp<DevUChar>::set_val(const vector<DevUChar> &values)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	for(size_t i = 0; i < values.size(); i++)
	{
		if(i > 0)
			st << ",";
		st << (short)values[i]; // to represent the numeric value
	}
	str = st.str();
	val = values;
	is_value = true;
}

template<>
inline void DoubleAttrProp<DevUChar>::set_val(const DevUChar &value)
{
	TangoSys_MemStream st;
	st.precision(TANGO_FLOAT_PRECISION);
	st << (short)value; // to represent the numeric value
	str = st.str();
	val.push_back(value);
	is_value = true;
}

//
// MultiAttrProp template specialisation for DevEncoded
//

template <>
class MultiAttrProp<DevEncoded>
{
public:
/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Default constructor.
 */
        MultiAttrProp() {}
//@}
/**@name Class data members */
//@{
/**
 * Attribute label
 */
		string 					            label;
/**
 * Attribute description
 */
		string 					            description;
/**
 * Attribute unit
 */
		string 					            unit;
/**
 * Attribute standard_unit
 */
		string 					            standard_unit;
/**
 * Attribute display_unit
 */
		string 					            display_unit;
/**
 * Attribute format
 */
		string 					            format;
/**
 * Attribute min_value
 */
        AttrProp<DevUChar>                 	min_value;
/**
 * Attribute max_value
 */
        AttrProp<DevUChar>                 	max_value;
/**
 * Attribute min_alarm
 */
        AttrProp<DevUChar>                 	min_alarm;
/**
 * Attribute max_alarm
 */
        AttrProp<DevUChar>                 	max_alarm;
/**
 * Attribute min_warning
 */
        AttrProp<DevUChar>                 	min_warning;
/**
 * Attribute max_warning
 */
        AttrProp<DevUChar>                 	max_warning;
/**
 * Attribute delta_t
 */
        AttrProp<DevLong>					delta_t;
/**
 * Attribute delta_val
 */
        AttrProp<DevUChar>                 	delta_val;
/**
 * Attribute event_period
 */
        AttrProp<DevLong>                 	event_period;
/**
 * Attribute archive_period
 */
        AttrProp<DevLong>                 	archive_period;
/**
 * Attribute rel_change
 */
        DoubleAttrProp<DevDouble>	        rel_change;
/**
 * Attribute abs_change
 */
        DoubleAttrProp<DevDouble>	        abs_change;
/**
 * Attribute archive_rel_change
 */
        DoubleAttrProp<DevDouble>	        archive_rel_change;
/**
 * Attribute archive_abs_change
 */
        DoubleAttrProp<DevDouble>	        archive_abs_change;
//@}
private:

//
// The extension class
//

	class MultiAttrPropExt {};

#ifdef HAS_UNIQUE_PTR
	unique_ptr<MultiAttrPropExt>	ext;           // Class extension
#else
	MultiAttrPropExt				*ext;
#endif
};

} // End of Tango namespace
#endif // _ATTRPROP_TPP

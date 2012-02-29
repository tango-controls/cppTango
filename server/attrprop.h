///=============================================================================
//
// file :		AttrProp.h
//
// description :	Include file for the AttrProp, DoubleAttrProp and MultiAttrProp classes.
//			Three classes are declared in this file :
//				The AttrProp class
//				The DoubleAttrProp class
//				The MultiAttrProp class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision: 19431 $
//
//=============================================================================

#ifndef _ATTRPROP_H
#define _ATTRPROP_H

#include <tango.h>
#include <attrdesc.h>
#include <encoded_attribute.h>
#include <functional>
#include <time.h>
#include <iterator>

#ifdef _TG_WINDOWS_
	#include <sys/types.h>
	#include <sys/timeb.h>
#endif

namespace Tango
{

//=============================================================================
//
//			The AttrProp class
//
//
// description :	This is a template class which holds a value of attribute
//			property and its string representation.
//
//=============================================================================

/**
 * This class represents a Tango attribute property.
 *
 * $Author: trogucki $
 * $Revision: 19431 $
 */

template <typename T>
class AttrProp
{
public:
/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Default constructor.
 */
	AttrProp() {}
/**
 * Create a new AttrProp object.
 *
 * @param value The attribute property value.
 */
	AttrProp(const T &value) : val(value), is_value(true) {
		TangoSys_MemStream st;
		st << value;
		str = st.str();
	}
/**
 * Create a new AttrProp object.
 *
 * @param value_str The 'C string' representation of attribute property.
 */
	AttrProp(const char *value_str) : str(string(value_str)), is_value(false) {}
/**
 * Create a new AttrProp object.
 *
 * @param value_str The string representation of attribute property value.
 */
	AttrProp(const string &value_str) : str(value_str), is_value(false) {}
//@}

	operator string()
	{
		return str;
	}
	operator const char *()
	{
		return str.c_str();
	}

/**@name Assignment operators
 * These operators allow to assign the value of the property by providing
 * the value or its string representation.
 */
//@{
/**
 * Assign the value of the attribute property.
 *
 * @param value A value of the attribute property.
 *
 * @return AttrProp object with both value and its string representation set.
 */
	AttrProp &operator=(const T &value)
	{
		TangoSys_MemStream st;
		st << value;
		str = st.str();
		val = value;
		is_value = true;
		return *this;
	}
/**
 * Assign the string representation of the attribute property value.
 *
 * @param value_str A 'C string' representation of the attribute property value.
 *
 * @return AttrProp object with the string representation of its value set.
 */
	AttrProp &operator=(const char *value_str)
	{
		str = value_str;
		is_value = false;
		return *this;
	}
/**
 * Assign the string representation of the attribute property value.
 *
 * @param value_str A string representation of the attribute property value.
 *
 * @return AttrProp object with the string representation of its value set.
 */
	AttrProp &operator=(const string &value_str)
	{
		str = value_str;
		is_value = false;
		return *this;
	}
//@}

/**@name Get/Set object members.
 * These methods allow the external world to get/set AttrProp instance
 * data members
 */
//@{
/**
 * Get the attribute property value.
 *
 * @return The attribute property value.
 */
	T get_val() {return val;}
/**
 * Get string representation of the attribute property value.
 *
 * @return The string representation of the attribute property value.
 */
	string &get_str() {return str;}
/**
 * Set the attribute property value.
 *
 * The value is automatically converted to its string representation.
 *
 * @param value The value of the attribute property.
 */
	void set_val(const T &value)
	{
		TangoSys_MemStream st;
		st << value;
		str = st.str();
		val = value;
		is_value = true;
	}
/**
 * Set string representation of the attribute property value.
 *
 * @param value_str The the 'C string' representation of the attribute property value.
 */
	void set_str(const char *value_str) {val = string(value_str); is_value = false;}
/**
 * Set string representation of the attribute property value.
 *
 * @param value_str The the string representation of the attribute property value.
 */
	void set_str(const string &value_str) {val = value_str; is_value = false;}
//@}

/**@name Check method
 * A method returning a boolean flag set to true if
 * the attribute property value has been assigned.
 */
//@{
/**
 * Check if the attribute property value has been assigned.
 *
 * This method returns a boolean set to true if the attribute property value has been assigned.
 *
 * @return A boolean set to true if the attribute property value has been assigned
 */
	bool is_val() {return is_value;}
//@}

private:
	T val;
	string str;
	bool is_value;

//
// The extension class
//

	class AttrPropExt {};

#ifdef HAS_UNIQUE_PTR
    unique_ptr<AttrPropExt>		ext;           // Class extension
#else
    AttrPropExt					*ext;
#endif
};

//=============================================================================
//
//			The DoubleAttrProp class
//
//
// description :	This is a template class which holds values of a compound
//			attribute property (like rel_change, abs_change, archive_rel_change,
//			archive_abs_change) which consists of two values, and its string
//			representation.
//
//=============================================================================

/**
 * This class represents a Tango compound attribute property which consists of
 * two values.
 *
 * $Author: trogucki $
 * $Revision: 19431 $
 */

template <typename T>
class DoubleAttrProp
{
public:
/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Default constructor.
 */
	DoubleAttrProp() {}
/**
 * Create a new DoubleAttrProp object.
 *
 * @param values A vector containing two values of
 * the compound attribute property.
 */
	DoubleAttrProp(const vector<T> &values) : val(values), is_value(true) {
		TangoSys_MemStream st;
		for(size_t i = 0; i < values.size(); i++)
		{
			if(i > 0)
				st << ",";
			st << values[i];
		}
		str = st.str();
	}
/**
 * Create a new DoubleAttrProp object.
 *
 * @param value The figure assigned to both values of the compound attribute property.
 */
	DoubleAttrProp(const T &value) : is_value(true) {
		TangoSys_MemStream st;
		st << value;
		str = st.str();
		val.push_back(value);
	}
/**
 * Create a new DoubleAttrProp object.
 *
 * @param value_str The 'C string' representation of values of the compound attribute property.
 */
	DoubleAttrProp(const char *value_str) : str(string(value_str)), is_value(false) {}
/**
 * Create a new DoubleAttrProp object.
 *
 * @param value_str The string representation of values of the compound attribute property.
 */
	DoubleAttrProp(const string &value_str) : str(value_str), is_value(false) {}
//@}

	operator string()
	{
		return str;
	}
	operator const char *()
	{
		return str.c_str();
	}

/**@name Assignment operators
 * These operators allow to assign the values of the compound attribute property
 * by providing the values or their string representations.
 */
//@{
/**
 * Assign the values of the compound attribute property.
 *
 * @param values A vector containing compound attribute property values.
 *
 * @return DoubleAttrProp object with both values of the compound attribute property
 * and their string representation set.
 */
	DoubleAttrProp & operator=(const vector<T> &values)
	{
		TangoSys_MemStream st;
		for(size_t i = 0; i < values.size(); i++)
		{
			if(i > 0)
				st << ",";
			st << values[i];
		}
		str = st.str();
		val = values;
		is_value = true;
		return *this;
	}
/**
 * Assign the values of the compound attribute property.
 *
 * @param value A figure representing both values of the compound attribute property.
 *
 * @return DoubleAttrProp object with both values of the compound attribute property
 * and their string representation set.
 */
	DoubleAttrProp & operator=(const T &value)
	{
		TangoSys_MemStream st;
		st << value;
		str = st.str();
		val.push_back(value);
		is_value = true;
		return *this;
	}
/**
 * Assign the values of the compound attribute property.
 *
 * @param value_str A 'C string' representation of values of the compound attribute property.
 *
 * @return DoubleAttrProp object with string representation of values of
 * the compound attribute property set.
 */
	DoubleAttrProp & operator=(const char *value_str)
	{
		str = value_str;
		is_value = false;
		return *this;
	}
/**
 * Assign the values of the compound attribute property.
 *
 * @param value_str A string representation of values of the compound attribute property.
 *
 * @return DoubleAttrProp object with string representation of values of
 * the compound attribute property set.
 */
	DoubleAttrProp & operator=(const string &value_str)
	{
		str = value_str;
		is_value = false;
		return *this;
	}
//@}

/**@name Get/Set object members.
 * These methods allow the external world to get/set DoubleAttrProp instance
 * data members
 */
//@{
/**
 * Get the vector containing the compound attribute property values.
 *
 * @return The vector containing the compound attribute property values.
 */
	vector<T> get_val() {return val;}
/**
 * Get string representation of the compound attribute property values.
 *
 * @return The string representation of the compound attribute property values.
 */
	string &get_str() {return str;}
/**
 * Set the compound attribute property values.
 *
 * The values are automatically converted to their string representation.
 *
 * @param values The vector containing the compound attribute property values.
 */
	void set_val(const vector<T> &values)
	{
		TangoSys_MemStream st;
		for(size_t i = 0; i < values.size(); i++)
		{
			if(i > 0)
				st << ",";
			st << values[i];
		}
		str = st.str();
		val = values;
		is_value = true;
	}
/**
 * Set the compound attribute property values.
 *
 * The figure provided is set for both values of the compound attribute property
 * and is automatically converted to its string representation.
 *
 * @param value The figure representing both values of the compound attribute property.
 */
	void set_val(const T &value)
	{
		TangoSys_MemStream st;
		st << value;
		str = st.str();
		val.push_back(value);
		is_value = true;
	}
/**
 * Set string representation of the compound attribute property values.
 *
 * @param value_str The 'C string' representation of the compound attribute property values.
 */
	void set_str(const char *value_str) {val = value_str; is_value = false;}
/**
 * Set string representation of the compound attribute property values.
 *
 * @param value_str The string representation of the compound attribute property values.
 */
	void set_str(const string &value_str) {val = value_str; is_value = false;}
//@}

/**@name Check method
 * A method returning a boolean flag set to true if
 * the compound attribute property values have been assigned.
 */
//@{
/**
 * Check if the compound attribute property values have been assigned.
 *
 * This method returns a boolean set to true if the compound attribute property values
 * have been assigned.
 *
 * @return A boolean set to true if the compound attribute property values have been assigned
 */
	bool is_val() {return is_value;}
//@}

private:
        vector<T> val;
        string str;
        bool is_value;

//
// The extension class
//

	class DoubleAttrPropExt {};

#ifdef HAS_UNIQUE_PTR
	unique_ptr<DoubleAttrPropExt>	ext;           // Class extension
#else
	DoubleAttrPropExt				*ext;
#endif
};

//=============================================================================
//
//			The MultiAttrProp class
//
//
// description :	This is a template class which holds values of modifiable
//		attribute properties.
//
//=============================================================================

/**
 * This class represents Tango modifiable attribute properties grouped in
 * one object to facilitate setting and getting attribute properties in one go.
 *
 * $Author: trogucki $
 * $Revision: 19431 $
 */

template <typename T>
class MultiAttrProp
{
public:
/**@name Class data members */
//@{
/**
 * Attribute label
 */
		string 					label;
/**
 * Attribute description
 */
		string 					description;
/**
 * Attribute unit
 */
		string 					unit;
/**
 * Attribute standard_unit
 */
		string 					standard_unit;
/**
 * Attribute display_unit
 */
		string 					display_unit;
/**
 * Attribute format
 */
		string 					format;
/**
 * Attribute min_value
 */
        AttrProp<T>                 min_value;
/**
 * Attribute max_value
 */
        AttrProp<T>                 max_value;
/**
 * Attribute min_alarm
 */
        AttrProp<T>                 min_alarm;
/**
 * Attribute max_alarm
 */
        AttrProp<T>                 max_alarm;
/**
 * Attribute min_warning
 */
        AttrProp<T>                 min_warning;
/**
 * Attribute max_warning
 */
        AttrProp<T>                 max_warning;
/**
 * Attribute delta_t
 */
        AttrProp<T>                 delta_t;
/**
 * Attribute delta_val
 */
        AttrProp<T>                 delta_val;
/**
 * Attribute event_period
 */
        AttrProp<T>                 event_period;
/**
 * Attribute archive_period
 */
        AttrProp<T>                 archive_period;
/**
 * Attribute rel_change
 */
        DoubleAttrProp<T>	        rel_change;
/**
 * Attribute abs_change
 */
        DoubleAttrProp<T>	        abs_change;
/**
 * Attribute archive_rel_change
 */
        DoubleAttrProp<T>	        archive_rel_change;
/**
 * Attribute archive_abs_change
 */
        DoubleAttrProp<T>	        archive_abs_change;
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

#endif // _ATTRPROP_H

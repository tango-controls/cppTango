//=============================================================================
//
// file :		w_attribute.h
//
// description :	Include file for the WAttribute classes.
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
// $Revision$
//
//=============================================================================

#ifndef _WATTRIBUTE_H
#define _WATTRIBUTE_H

#include <tango.h>
#include <attrdesc.h>
#include <functional>
#include <time.h>

namespace Tango
{

//=============================================================================
//
//			The WAttribute class
//
//
// description :	This class inherits the Attribute class. There is one
//			instance of this class for each writable attribute
//
//=============================================================================


/**
 * This class represents a writable attribute. It inherits from the Attribute
 * class and only add what is specific to writable attribute.
 *
 * $Author$
 * $Revision$
 */

class WAttribute:public Attribute
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Create a new Writable Attribute object.
 *
 * @param prop_list The attribute properties list. Each property is an object
 * of the AttrProperty class
 * @param tmp_attr The temporary attribute object built from user parameters
 * @param dev_name The device name
 * @param idx The index of the related Attr object in the MultiClassAttribute
 *            vector of Attr object
 */
	WAttribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class
 */
//@{
/**
 * The WAttribute desctructor.
 */
	~WAttribute();
//@}

/**@name Attribute configuration methods
 * Miscellaneous methods dealing with attribute min and max value property
 */
//@{
/**
 * Check if the attribute has a minimum value.
 *
 * @return A boolean set to true if the attribute has a minimum value
 * defined
 */
	bool is_min_value() {return check_min_value;}
/**
 * Set attribute minimum value
 *
 * @param min_value Reference to a variable which represents the new min value
 */
	template <typename T>
	void set_min_value(const T &min_value);

	void set_min_value(char *min_value);
	void set_min_value(const char *min_value);
/**
 * Gets attribute minimum value or throws an exception if the
 * attribute does not have a minimum value
 *
 * @param min_value Reference to a variable which value will be set to the attribute's
 * minimum value
 */
	template <typename T>
	void get_min_value(T &min_value);
/**
 * Check if the attribute has a maximum value.
 *
 * @return check_max_value A boolean set to true if the attribute has a maximum value
 * defined
 */
	bool is_max_value() {return check_max_value;}
/**
 * Set attribute maximum value
 *
 * @param max_value Reference to a variable which represents the new max value
 */
	template <typename T>
	void set_max_value(const T &max_value);

	void set_max_value(char *max_value);
	void set_max_value(const char *max_value);
/**
 * Get attribute maximum value or throws an exception if the
 * attribute does not have a maximum value
 *
 * @param max_value Reference to a variable which value will be set to the attribute's
 * maximum value
 */
	template <typename T>
	void get_max_value(T &max_value);
//@}

/**@name Get new value for attribute
 * Miscellaneous method to retrieve from the WAttribute object the new value
 * for the attribute.
 */
 //@{
 /**
 * Retrieve the new value length (data number) for writable attribute.
 *
 * @return  The new value data length
 */
 	long get_write_value_length();

 /**
 * Retrieve the date of the last attribute writing. This is set only
 * if the attribute has a read different than set alarm. Otherwise,
 * date is set to 0.
 *
 * @return  The written date
 */
 	struct timeval &get_write_date() {return write_date;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevShort &val) {val = short_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevShort and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer wich will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevShort *&ptr) {ptr = short_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevLong &val) {val = long_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevLong *&ptr) {ptr = long_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64.
 *
 * @param val A reference to a Tango::DevLong64 data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevLong64 &val) {val = w_ext->long64_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64 and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevLong64 *&ptr) {ptr = w_ext->long64_ptr;}


/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevFloat.
 *
 * @param val A reference to a Tango::DevFloat data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevFloat &val) {val = float_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevFloat and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevFloat *&ptr) {ptr = float_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevDouble &val) {val = double_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevDouble and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevDouble *&ptr) {ptr = double_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevString &val) {val = str_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevString and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::ConstDevString *&ptr) {ptr = str_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevBoolean.
 *
 * @param val A reference to a Tango::DevBoolean data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevBoolean &val) {val = boolean_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevBoolean and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevBoolean *&ptr) {ptr = boolean_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUShort.
 *
 * @param val A reference to a Tango::DevUShort data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevUShort &val) {val = ushort_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUShort and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevUShort *&ptr) {ptr = ushort_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUChar.
 *
 * @param val A reference to a Tango::DevUChar data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevUChar &val) {val = uchar_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevUChar and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevUChar *&ptr) {ptr = uchar_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevULong.
 *
 * @param val A reference to a Tango::DevULong data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevULong &val) {val = w_ext->ulong_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevULong and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevULong *&ptr) {ptr = w_ext->ulong_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevULong64.
 *
 * @param val A reference to a Tango::DevULong64 data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevULong64 &val) {val = w_ext->ulong64_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64 and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevULong64 *&ptr) {ptr = w_ext->ulong64_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevState.
 *
 * @param val A reference to a Tango::DevState data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevState &val) {val = w_ext->dev_state_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevLong64 and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevState *&ptr) {ptr = w_ext->state_ptr;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevEncoded.
 *
 * @param val A reference to a Tango::DevEncoded data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevEncoded &val) {val = encoded_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is
 * Tango::DevEncoded and the attribute is SPECTRUM or IMAGE.
 *
 * @param ptr Reference to a pointer which will be set to point to the data
 * to be written into the attribute. This pointer points into attribute
 * internal memory which must not be freed.
 */
	void get_write_value(const Tango::DevEncoded *&ptr) {ptr = encoded_ptr;}


//@}

/**@name Set new value for attribute
 * Miscellaneous method to set a WAttribute value
 */
 //@{
/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data
 */
	void set_write_value(Tango::DevShort val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevShort.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevShort *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevShort.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevShort> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data
 */
	void set_write_value(Tango::DevLong val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevLong *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevLong> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevLong64.
 *
 * @param val A reference to a Tango::DevLong64 data
 */
	void set_write_value(Tango::DevLong64 val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong64.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevLong64 *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevLong64.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevLong64> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble
 */
	void set_write_value(Tango::DevDouble val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevDouble.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevDouble *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevDouble.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevDouble> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString
 */
	void set_write_value(Tango::DevString val);
/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevString.
 *
 * @param val A reference to a std::string
 */
	void set_write_value(string &val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevString.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(Tango::DevString *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevString.
 *
 * @param val A vector of string containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<string> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevFloat.
 *
 * @param val A reference to a Tango::DevFloat
 */
	void set_write_value(Tango::DevFloat val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevFloat.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevFloat *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevFloat.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevFloat> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevBoolean.
 *
 * @param val A reference to a Tango::DevBoolean
 */
	void set_write_value(Tango::DevBoolean val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevBoolean.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevBoolean *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevBoolean.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevBoolean> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevUShort.
 *
 * @param val A reference to a Tango::DevUShort
 */
	void set_write_value(Tango::DevUShort val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUShort.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevUShort *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUShort.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevUShort> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevUChar.
 *
 * @param val A reference to a Tango::DevUChar
 */
	void set_write_value(Tango::DevUChar val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUChar.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevUChar *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevUChar.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevUChar> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevULong.
 *
 * @param val A reference to a Tango::DevULong data
 */
	void set_write_value(Tango::DevULong val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevULong *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevULong> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevULong64.
 *
 * @param val A reference to a Tango::DevULong64 data
 */
	void set_write_value(Tango::DevULong64 val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong64.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0

 */
	void set_write_value(Tango::DevULong64 *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevULong64.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevULong64> &val, long x = 1, long y = 0);

/**
 * Set the writable scalar attribute value when the attribute data type is
 * Tango::DevState.
 *
 * @param val A reference to a Tango::DevState data
 */
	void set_write_value(Tango::DevState val);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevState.
 *
 * @param val A reference to the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(Tango::DevState *val, long x = 1, long y = 0);
/**
 * Set the writable spectrum or image attribute value when the attribute
 * data type is Tango::DevState.
 *
 * @param val A vector containing the attribute set value
 * @param x The attribute set value x length. Default value is 1
 * @param y The attribute set value y length. Default value is 0
 */
	void set_write_value(vector<Tango::DevState> &val, long x = 1, long y = 0);
//@}

	void set_write_value(Tango::DevEncoded *, long x = 1,long y = 0); // Dummy method for compiler

	virtual void set_rvalue();

	void rollback();

	void check_written_value(const CORBA::Any &,unsigned long,unsigned long);
	void check_written_value(const DevVarEncodedArray &,unsigned long,unsigned long);
	void check_written_value(const AttrValUnion &,unsigned long,unsigned long);

	void copy_data(const CORBA::Any &);
	void copy_data(const Tango::AttrValUnion &);

	long get_w_dim_x() {return w_dim_x;}
	long get_w_dim_y() {return w_dim_y;}

	void set_user_set_write_value(bool val) {w_ext->uswv = val;}
	bool get_user_set_write_value() {return w_ext->uswv;}

	Tango::DevVarShortArray *get_last_written_sh() {return &short_array_val;}
	Tango::DevVarLongArray *get_last_written_lg() {return &long_array_val;}
	Tango::DevVarDoubleArray *get_last_written_db() {return &double_array_val;}
	Tango::DevVarStringArray *get_last_written_str() {return &str_array_val;}
	Tango::DevVarFloatArray *get_last_written_fl() {return &float_array_val;}
	Tango::DevVarBooleanArray *get_last_written_boo() {return &boolean_array_val;}
	Tango::DevVarUShortArray *get_last_written_ush() {return &ushort_array_val;}
	Tango::DevVarCharArray *get_last_written_uch() {return &uchar_array_val;}
	Tango::DevVarLong64Array *get_last_written_lg64() {return &w_ext->long64_array_val;}
	Tango::DevVarULong64Array *get_last_written_ulg64() {return &w_ext->ulong64_array_val;}
	Tango::DevVarULongArray *get_last_written_ulg() {return &w_ext->ulong_array_val;}
	Tango::DevVarStateArray *get_last_written_state() {return &w_ext->state_array_val;}
	Tango::DevEncoded &get_last_written_encoded() {return encoded_val;}

	bool is_memorized() {return memorized;}
	void set_memorized(bool mem) {memorized = mem;}
	bool is_memorized_init() {return memorized_init;}
	void set_memorized_init(bool mem_init) {memorized_init = mem_init;}
	string &get_mem_value() {return mem_value;}
	void set_mem_value(const string &new_val) {mem_value = new_val;}
	void set_written_date();
    bool mem_value_below_above(MinMaxValueCheck,string &);

protected:
	virtual bool check_rds_alarm();

private:

//
// The extension class
//

    class WAttributeExt
    {
    public:
        WAttributeExt() {}

        Tango::DevLong64			long64_val;
        Tango::DevLong64			old_long64_val;
        Tango::DevULong				ulong_val;
        Tango::DevULong				old_ulong_val;
        Tango::DevULong64			ulong64_val;
        Tango::DevULong64			old_ulong64_val;
        Tango::DevState				dev_state_val;
        Tango::DevState				old_dev_state_val;

        Tango::DevVarLong64Array	long64_array_val;
        Tango::DevVarULongArray		ulong_array_val;
        Tango::DevVarULong64Array	ulong64_array_val;
        Tango::DevVarStateArray		state_array_val;

        const Tango::DevLong64		*long64_ptr;
        const Tango::DevULong		*ulong_ptr;
        const Tango::DevULong64		*ulong64_ptr;
        const Tango::DevState		*state_ptr;

        bool						uswv;					// User set_write_value
    };

// Defined prior to Tango IDL release 3

	Tango::DevShort 		short_val;
	Tango::DevShort			old_short_val;

	Tango::DevLong			long_val;
	Tango::DevLong			old_long_val;

	Tango::DevDouble		double_val;
	Tango::DevDouble		old_double_val;

	Tango::DevString		str_val;
	Tango::DevString		old_str_val;

	Tango::DevFloat			float_val;
	Tango::DevFloat			old_float_val;

	Tango::DevBoolean		boolean_val;
	Tango::DevBoolean		old_boolean_val;

	Tango::DevUShort		ushort_val;
	Tango::DevUShort		old_ushort_val;

	Tango::DevUChar			uchar_val;
	Tango::DevUChar			old_uchar_val;

	Tango::DevEncoded		encoded_val;
	Tango::DevEncoded		old_encoded_val;

// Added for Tango IDL release 3

	long						w_dim_y;
	long						w_dim_x;

	Tango::DevVarShortArray 	short_array_val;
	Tango::DevVarLongArray		long_array_val;
	Tango::DevVarDoubleArray 	double_array_val;
	Tango::DevVarStringArray	str_array_val;
	Tango::DevVarFloatArray 	float_array_val;
	Tango::DevVarBooleanArray	boolean_array_val;
	Tango::DevVarUShortArray 	ushort_array_val;
	Tango::DevVarCharArray		uchar_array_val;

	const Tango::DevShort  		*short_ptr;
	const Tango::DevLong  		*long_ptr;
	const Tango::DevDouble  	*double_ptr;
	const Tango::ConstDevString	*str_ptr;
	const Tango::DevFloat  		*float_ptr;
	const Tango::DevBoolean		*boolean_ptr;
	const Tango::DevUShort  	*ushort_ptr;
	const Tango::DevUChar		*uchar_ptr;
	const Tango::DevEncoded		*encoded_ptr;

	bool						string_allocated;
	bool 						memorized;
	bool 						memorized_init;
	string						mem_value;
	struct timeval				write_date;

#ifdef HAS_UNIQUE_PTR
    unique_ptr<WAttributeExt>   w_ext;           // Class extension
#else
	WAttributeExt				*w_ext;
#endif
};


#define COMPUTE_TIME_DIFF(RESULT,BEFORE,AFTER) \
long bef = ((BEFORE.tv_sec - 1095000000) * 1000) + (BEFORE.tv_usec / 1000); \
long after = ((AFTER.tv_sec - 1095000000) * 1000) + (AFTER.tv_usec / 1000); \
RESULT = after - bef;

} // End of Tango namespace

#endif // _WATTRIBUTE_H

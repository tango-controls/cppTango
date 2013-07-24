//////////////////////////////////////////////////////////////////
//
// DeviceAttribute.h - include file for TANGO device api class DeviceAttribute
//
//
// Copyright (C) :      2012,2013
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
// $Revision: 20437 $
//
///////////////////////////////////////////////////////////////

#ifndef _DEVICEATTRIBUTE_H
#define _DEVICEATTRIBUTE_H


/****************************************************************************************
 * 																						*
 * 					The DeviceAttribute class											*
 * 					-------------------------											*
 * 																						*
 ***************************************************************************************/


/**
 * Fundamental type for sending an dreceiving data to and from device attributes.
 *
 * This is the fundamental type for sending and receiving data to and from device attributes. The values can be
 * inserted and extracted using the operators << and >> respectively and insert() for mixed data types. There
 * are two ways to check if the extraction operator succeed :
 * @li 1. By testing the extractor operators return value. All the extractors operator returns a boolean value set
 * to false in case of problem.
 * @li 2. By asking the DeviceAttribute object to throw exception in case of problem. By default, DeviceAttribute
 * throws exception :
 * @li (a) when the user try to extract data and the server reported an error when the attribute was read.
 * @li (b) When the user try to extract data from an empty DeviceAttribute
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DeviceAttribute
{

public :
///@privatesection
//
// constructor methods
//
	enum except_flags
	{
		isempty_flag = 0,
		wrongtype_flag,
		failed_flag,
		unknown_format_flag,
		numFlags
	};


	DeviceAttribute(const DeviceAttribute&);
	DeviceAttribute & operator=(const DeviceAttribute &);
#ifdef HAS_RVALUE
	DeviceAttribute(DeviceAttribute &&);
	DeviceAttribute & operator=(DeviceAttribute &&);
#endif

	void deep_copy(const DeviceAttribute &);

	DeviceAttribute(AttributeValue);

///@publicsection
/**@name Constructors */
//@{
/**
 * Create a DeviceAttribute object.
 *
 * Default constructor. The instance is empty
 *
 */
	DeviceAttribute();
/**
 * Create a DeviceAttribute object from attribute name and value for scalar attribute
 *
 * Create a DeviceAttribute object from attribute name and value for @b scalar attribute.
 * These constructors exists for the following data type:
 * @li DeviceAttribute(string &, bool);
 * @li DeviceAttribute(string &, short);
 * @li DeviceAttribute(string &, DevLong);
 * @li DeviceAttribute(string &, DevLong64);
 * @li DeviceAttribute(string &, float);
 * @li DeviceAttribute(string &, double);
 * @li DeviceAttribute(string &, unsigned char);
 * @li DeviceAttribute(string &, unsigned short);
 * @li DeviceAttribute(string &, DevULong);
 * @li DeviceAttribute(string &, DevULong64);
 * @li DeviceAttribute(string &, string &);
 * @li DeviceAttribute(string &, DevState);
 * @li DeviceAttribute(string &, DevEncoded &);
 * @n @n
 * @li DeviceAttribute(const char *, bool);
 * @li DeviceAttribute(const char *, short);
 * @li DeviceAttribute(const char *, DevLong);
 * @li DeviceAttribute(const char *, DevLong64);
 * @li DeviceAttribute(const char *, float);
 * @li DeviceAttribute(const char *, double);
 * @li DeviceAttribute(const char *, unsigned char);
 * @li DeviceAttribute(const char *, unsigned short);
 * @li DeviceAttribute(const char *, DevULong);
 * @li DeviceAttribute(const char *, DevULong64);
 * @li DeviceAttribute(const char *, string &);
 * @li DeviceAttribute(const char *, DevState);
 * @li DeviceAttribute(const char *,DevEncoded &);
 *
 * @param [in] name The attribute name
 * @param [in] val The attribute value
 */
	DeviceAttribute(string &name, short val);
/**
 * Create a DeviceAttribute object from attribute name and value for spectrum attribute
 *
 * Create a DeviceAttribute object from attribute name and value for @b spectrum attribute.
 * These constructors exists for the following data type:
 * @li DeviceAttribute(string &, vector<bool> &);
 * @li DeviceAttribute(string &, vector<short> &);
 * @li DeviceAttribute(string &, vector<DevLong> &);
 * @li DeviceAttribute(string &, vector<DevLong64>&);
 * @li DeviceAttribute(string &, vector<float> &);
 * @li DeviceAttribute(string &, vector<double> &);
 * @li DeviceAttribute(string &, vector<unsigned char> &);
 * @li DeviceAttribute(string &, vector<unsigned short> &);
 * @li DeviceAttribute(string &, vector<DevULong> &);
 * @li DeviceAttribute(string &, vector<DevULong64>&);
 * @li DeviceAttribute(string &, vector<string> & );
 * @li DeviceAttribute(string &, vector<DevState> &);
 * @n @n
 * @li DeviceAttribute(const char *, vector<bool> &);
 * @li DeviceAttribute(const char *, vector<short> &);
 * @li DeviceAttribute(const char *, vector<DevLong> &);
 * @li DeviceAttribute(const char *, vector<DevLong64>&);
 * @li DeviceAttribute(const char *, vector<float> &);
 * @li DeviceAttribute(const char *, vector<double> &);
 * @li DeviceAttribute(const char *, vector<unsigned char> &);
 * @li DeviceAttribute(const char *, vector<unsigned short> &);
 * @li DeviceAttribute(const char *, vector<DevULong> &);
 * @li DeviceAttribute(const char *, vector<DevULong64>&);
 * @li DeviceAttribute(const char *, vector<string> & );
 * @li DeviceAttribute(const char *, vector<DevState> &);
 *
 * @param [in] name The attribute name
 * @param [in] val The attribute value
 */
	DeviceAttribute(string &name, vector<short> &val);
/**
 * Create a DeviceAttribute object from attribute name and value for image attribute
 *
 * Create a DeviceAttribute object from attribute name and value for @b image attribute.
 * These constructors have two more parameters allowing the user to define the x and y image
 * dimensions.
 * These constructors exists for the following data type:
 * @li DeviceAttribute(string &, vector<bool> &, int, int);
 * @li DeviceAttribute(string &, vector<short> &, int, int);
 * @li DeviceAttribute(string &, vector<DevLong> &, int, int);
 * @li DeviceAttribute(string &, vector<DevLong64>&, int, int);
 * @li DeviceAttribute(string &, vector<float> &, int, int);
 * @li DeviceAttribute(string &, vector<double> &, int, int);
 * @li DeviceAttribute(string &, vector<unsigned char> &, int, int);
 * @li DeviceAttribute(string &, vector<unsigned short> &, int, int);
 * @li DeviceAttribute(string &, vector<DevULong> &, int, int);
 * @li DeviceAttribute(string &, vector<DevULong64>&, int, int);
 * @li DeviceAttribute(string &, vector<string> &, int, int );
 * @li DeviceAttribute(string &, vector<DevState> &, int, int);
 * @n @n
 * DeviceAttribute(const char *, vector<bool> &, int, int);
 * @li DeviceAttribute(const char *, vector<short> &, int, int);
 * @li DeviceAttribute(const char *, vector<DevLong> &, int, int);
 * @li DeviceAttribute(const char *, vector<DevLong64>&, int, int);
 * @li DeviceAttribute(const char *, vector<float> &, int, int);
 * @li DeviceAttribute(const char *, vector<double> &, int, int);
 * @li DeviceAttribute(const char *, vector<unsigned char> &, int, int);
 * @li DeviceAttribute(const char *, vector<unsigned short> &, int, int);
 * @li DeviceAttribute(const char *, vector<DevULong> &, int, int);
 * @li DeviceAttribute(const char *, vector<DevULong64>&, int, int);
 * @li DeviceAttribute(const char *, vector<string> & , int, int);
 * @li DeviceAttribute(const char *, vector<DevState) &, int, int);
 *
 * @param [in] name The attribute name
 * @param [in] val The attribute value
 * @param [in] dim_x The attribute X dimension
 * @param [in] dim_y The attribute Y dimension
 */
	DeviceAttribute(string &name, vector<short> &val,int dim_x,int dim_y);
//@}
///@privatesection
	DeviceAttribute(string&, DevLong);
	DeviceAttribute(string&, double);
	DeviceAttribute(string&, string&);
	DeviceAttribute(string&, const char *);
	DeviceAttribute(string&, float);
	DeviceAttribute(string&, bool);
	DeviceAttribute(string&, unsigned short);
	DeviceAttribute(string&, unsigned char);
	DeviceAttribute(string&, DevLong64);
	DeviceAttribute(string&, DevULong);
	DeviceAttribute(string&, DevULong64);
	DeviceAttribute(string&, DevState);
	DeviceAttribute(string&, DevEncoded &);

	DeviceAttribute(string&, vector<DevLong> &);
	DeviceAttribute(string&, vector<double> &);
	DeviceAttribute(string&, vector<string> &);
	DeviceAttribute(string&, vector<float> &);
	DeviceAttribute(string&, vector<bool> &);
	DeviceAttribute(string&, vector<unsigned short> &);
	DeviceAttribute(string&, vector<unsigned char> &);
	DeviceAttribute(string&, vector<DevLong64> &);
	DeviceAttribute(string&, vector<DevULong> &);
	DeviceAttribute(string&, vector<DevULong64> &);
	DeviceAttribute(string&, vector<DevState> &);

	DeviceAttribute(string&, vector<DevLong> &,int,int);
	DeviceAttribute(string&, vector<double> &,int,int);
	DeviceAttribute(string&, vector<string> &,int,int);
	DeviceAttribute(string&, vector<float> &,int,int);
	DeviceAttribute(string&, vector<bool> &,int,int);
	DeviceAttribute(string&, vector<unsigned short> &,int,int);
	DeviceAttribute(string&, vector<unsigned char> &,int,int);
	DeviceAttribute(string&, vector<DevLong64> &,int,int);
	DeviceAttribute(string&, vector<DevULong> &,int,int);
	DeviceAttribute(string&, vector<DevULong64> &,int,int);
	DeviceAttribute(string&, vector<DevState> &,int,int);

	DeviceAttribute(const char *, short);
	DeviceAttribute(const char *, DevLong);
	DeviceAttribute(const char *, double);
	DeviceAttribute(const char *, string&);
	DeviceAttribute(const char *, const char *);
	DeviceAttribute(const char *, float);
	DeviceAttribute(const char *, bool);
	DeviceAttribute(const char *, unsigned short);
	DeviceAttribute(const char *, unsigned char);
	DeviceAttribute(const char *, DevLong64);
	DeviceAttribute(const char *, DevULong);
	DeviceAttribute(const char *, DevULong64);
	DeviceAttribute(const char *, DevState);
	DeviceAttribute(const char *, DevEncoded &);

	DeviceAttribute(const char *, vector<short> &);
	DeviceAttribute(const char *, vector<DevLong> &);
	DeviceAttribute(const char *, vector<double> &);
	DeviceAttribute(const char *, vector<string> &);
	DeviceAttribute(const char *, vector<float> &);
	DeviceAttribute(const char *, vector<bool> &);
	DeviceAttribute(const char *, vector<unsigned short> &);
	DeviceAttribute(const char *, vector<unsigned char> &);
	DeviceAttribute(const char *, vector<DevLong64> &);
	DeviceAttribute(const char *, vector<DevULong> &);
	DeviceAttribute(const char *, vector<DevULong64> &);
	DeviceAttribute(const char *, vector<DevState> &);

	DeviceAttribute(const char *, vector<short> &,int,int);
	DeviceAttribute(const char *, vector<DevLong> &,int,int);
	DeviceAttribute(const char *, vector<double> &,int,int);
	DeviceAttribute(const char *, vector<string> &,int,int);
	DeviceAttribute(const char *, vector<float> &,int,int);
	DeviceAttribute(const char *, vector<bool> &,int,int);
	DeviceAttribute(const char *, vector<unsigned short> &,int,int);
	DeviceAttribute(const char *, vector<unsigned char> &,int,int);
	DeviceAttribute(const char *, vector<DevLong64> &,int,int);
	DeviceAttribute(const char *, vector<DevULong> &,int,int);
	DeviceAttribute(const char *, vector<DevULong64> &,int,int);
	DeviceAttribute(const char *, vector<DevState> &,int,int);

	~DeviceAttribute();

	AttrQuality 		quality;
	AttrDataFormat		data_format;
	string 				name;
	int 				dim_x;
	int 				dim_y;
	int 				w_dim_x;
	int 				w_dim_y;
	TimeVal 			time;

    void set_w_dim_x(int val) {w_dim_x = val;}
    void set_w_dim_y(int val) {w_dim_y = val;}
    void set_error_list(DevErrorList *ptr) {err_list = ptr;}

	DevVarEncodedArray_var &get_Encoded_data() {return EncodedSeq;}
    DevErrorList_var &get_error_list() {return err_list;}

	DevVarLongArray_var 	LongSeq;
	DevVarShortArray_var 	ShortSeq;
	DevVarDoubleArray_var 	DoubleSeq;
	DevVarStringArray_var 	StringSeq;
	DevVarFloatArray_var	FloatSeq;
	DevVarBooleanArray_var	BooleanSeq;
	DevVarUShortArray_var	UShortSeq;
	DevVarCharArray_var		UCharSeq;
	DevVarLong64Array_var	Long64Seq;
	DevVarULongArray_var	ULongSeq;
	DevVarULong64Array_var	ULong64Seq;
	DevVarStateArray_var	StateSeq;
	DevVarEncodedArray_var	EncodedSeq;

	DevErrorList_var		err_list;

//
// For the state attribute
//

	DevState				d_state;
	bool					d_state_filled;

//
// Insert operators for  C++ types
//

///@publicsection
/**@name Inserters and Extractors */
//@{
/**
 * Insert attribute data
 *
 * Special care has been taken to avoid memory copy between the network layer and the user application.
 * Nevertheless, C++ vector types are not the CORBA native type and one copy is unavoidable when using
 * vectors. Using the native TANGO CORBA sequence types in most cases avoid any copy but needs some
 * more care about memory usage.
 * @li <B>For insertion into DeviceAttribute instance from TANGO CORBA sequence pointers, the DeviceAttribute
 * object takes ownership of the pointed to memory. This means that the pointed
 * to memory will be freed when the DeviceAttribute object is destroyed or when another data is
 * inserted into it.</B>
 * @li <B>The insertion into DeviceAttribute instance from TANGO CORBA sequence reference copy
 * the data into the DeviceAttribute object.</B>
 *
 * Insert operators for the following scalar C++ types
 * @li bool
 * @li short
 * @li DevLong
 * @li DevLong64
 * @li float
 * @li double
 * @li unsigned char
 * @li unsigned short
 * @li DevULong
 * @li DevULong64
 * @li string
 * @li DevState
 * @li DevEncoded
 * @li DevString
 * @li const char *
 *
 * Insert operators for the following C++ vector types for @b spectrum attributes :
 * @li vector<bool>
 * @li vector<short>
 * @li vector<DevLong>
 * @li vector<DevLong64>
 * @li vector<float>
 * @li vector<double>
 * @li vector<unsigned char>
 * @li vector<unsigned short>
 * @li vector<DevULong>
 * @li vector<DevULong64>
 * @li vector<string>
 * @li vector<DevState>
 *
 * Insert operators for @b spectrum attribute and for the following types by pointer (with memory ownership transfert) :
 * @li DevVarBooleanArray *
 * @li DevVarShortArray *
 * @li DevVarLongArray *
 * @li DevVarLong64Array *
 * @li DevVarFloatArray *
 * @li DevVarDoubleArray *
 * @li DevVarUCharArray *
 * @li DevVarUShortArray *
 * @li DevVarULongArray *
 * @li DevVarULong64Array *
 * @li DevVarStringArray *
 * @li DevVarStateArray *
 *
 * Insert operators for @b spectrum attribute and for the following types by reference :
 * @li const DevVarBooleanArray &
 * @li const DevVarShortArray &
 * @li const DevVarLongArray &
 * @li const DevVarLong64Array&
 * @li const DevVarFloatArray &
 * @li const DevVarDoubleArray &
 * @li const DevVarUCharArray &
 * @li const DevVarUShortArray &
 * @li const DevVarULongArray &
 * @li const DevVarULong64Array&
 * @li const DevVarStringArray &
 * @li const DevVarStateArray &
 *
 * Here is an example of creating, inserting and extracting some DeviceAttribute types :
 * @code
 * DeviceAttribute my_short, my_long, my_string;
 * DeviceAttribute my_float_vector, my_double_vector;
 * string a_string;
 * short a_short;
 * DevLong a_long;
 * vector<float> a_float_vector;
 * vector<double> a_double_vector;
 *
 * my_short << 100; // insert a short
 * my_short >> a_short; // extract a short
 *
 * my_long << 1000; // insert a long
 * my_long >> a_long; // extract a DevLong
 *
 * my_string << string("estas lista a bailar el tango ?"); // insert a string
 * my_string >> a_string; // extract a string
 *
 * my_float_vector << a_float_vector // insert a vector of floats
 * my_float_vector >> a_float_vector; // extract a vector of floats
 *
 * my_double_vector << a_double_vector; // insert a vector of doubles
 * my_double_vector >> a_double_vector; // extract a vector of doubles
 * //
 * // Extract read and set value of an attribute separately
 * // and get their dimensions
 * //
 * vector<float> r_float_vector, w_float_vector;
 *
 * my_float_vector.extract_read (r_float_vector) // extract read values
 * int dim_x = my_float_vector.get_dim_x(); // get x dimension
 * int dim_y = my_float_vector.get_dim_y(); // get y dimension
 *
 * my_float_vector.extract_set (w_float_vector) // extract set values
 * int w_dim_x = my_float_vector.get_written_dim_x(); // get x dimension
 * int W_dim_y = my_float_vector.get_written_dim_y(); // get y dimension
 * //
 * // Example of memory management with TANGO sequence types without memory leaks
 * //
 * for (int i = 0;i < 10;i++)
 * {
 *    DeviceAttribute da;
 *    DevVarLongArray *out;
 *    try
 *    {
 *       da = device->read_attribute("Attr");
 *       da >> out;
 *    }
 *    catch(DevFailed &e)
 *    {
 *       ....
 *    }
 *    cout << "Received value = " << (*out)[0];
 *    delete out;
 * }
 * @endcode
 *
 * @param [in] val The attribute value
 * @exception WrongData if requested
 */
	void operator << (short val);
/**
 * Insert attribute data for DevEncoded attribute
 *
 * Insert attribute data when the attribute data type is DevEncoded
 * @n Similar methods with following signature also exist
 * @li <B> insert(const string &str, vector<unsigned char> &data);</B>
 * @li <B> insert(const char *str, DevVarCharArray *data);</B>
 *
 * These three methods do not take ownership of the memory used for the data buffer.
 * @n See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [in] str The DevEncoded string
 * @param [in] data The DevEncoded data pointer
 * @param [in] length The DevEncoded data length
 * @exception WrongData if requested
 */
	void insert(const char *str,unsigned char *data,unsigned int length);
/**
 * Insert attribute data for image attribute (from C++ vector)
 *
 * Insert methods for the following C++ vector types for image attributes allowing the specification of
 * the x and y image dimensions :
 * @li insert(vector<bool> &,int, int)
 * @li insert(vector<short> &,int, int)
 * @li insert(vector<DevLong>&,int, int)
 * @li insert(vector<DevLong64>&,int, int)
 * @li insert(vector<float> &,int, int)
 * @li insert(vector<double> &,int, int)
 * @li insert(vector<unsigned char> &,int, int)
 * @li insert(vector<unsigned short> &,int, int)
 * @li insert(vector<DevULong>&,int, int)
 * @li insert(vector<DevULong64>&,int, int)
 * @li insert(vector<string> &,int, int)
 * @li insert(vector<DevState> &,int, int)

 * See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [in] datum The attribute data
 * @param [in] dim_x The attribute X dimension
 * @param [in] dim_y The attribute Y dimension
 * @exception WrongData if requested
 */
	void insert(vector<short> &datum,int dim_x,int dim_y);
/**
 * Insert attribute data for image attribute (from CORBA sequence by reference)
 *
 * Insert methods for @b image attribute and for the following types by reference.
 * These method allow the programmer to define the x and y image dimensions. The following methods are defined :
 * @li const DevVarBooleanArray &
 * @li const DevVarShortArray &
 * @li const DevVarLongArray &
 * @li const DevVarLong64Array&
 * @li const DevVarFloatArray &
 * @li const DevVarDoubleArray &
 * @li const DevVarUCharArray &
 * @li const DevVarUShortArray &
 * @li const DevVarULongArray &
 * @li const DevVarULong64Array&
 * @li const DevVarStringArray &
 * @li const DevVarStateArray &

 * See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [in] datum The attribute data
 * @param [in] dim_x The attribute X dimension
 * @param [in] dim_y The attribute Y dimension
 * @exception WrongData if requested
 */
	void insert(const DevVarShortArray &datum,int dim_x,int dim_y);
/**
 * Insert attribute data for image attribute (from CORBA sequence by pointer)
 *
 * Insert methods for @b image attribute and pointers. The DeviceAttribute object tkes ownership of the
 * given memory. These method allow the programmer to define the x
 * and y image dimensions. The following methods are defined :
 * @li insert(DevVarBooleanArray *, int , int )
 * @li insert(DevVarShortArray *, int , int )
 * @li insert(DevVarLongArray *, int , int )
 * @li insert(DevVarLong64Array *, int, int )
 * @li insert(DevVarFloatArray *, int , int )
 * @li insert(DevVarDoubleArray *, int , int )
 * @li insert(DevVarUCharArray *, int , int )
 * @li insert(DevVarUShortArray *, int , int )
 * @li insert(DevVarULongArray *, int , int )
 * @li insert(DevVarULong64Array *, int, int )
 * @li insert(DevVarStringArray *, int , int )
 * @li insert(DevVarStateArray *, int, int)

 * See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [in] datum The attribute data
 * @param [in] dim_x The attribute X dimension
 * @param [in] dim_y The attribute Y dimension
 * @exception WrongData if requested
 */
	void insert(DevVarShortArray *datum,int dim_x,int dim_y);
/**
 * Extract attribute data
 *
 * Special care has been taken to avoid memory copy between the network layer and the user application.
 * Nevertheless, C++ vector types are not the CORBA native type and one copy is unavoidable when using
 * vectors. Using the native TANGO CORBA sequence types in most cases avoid any copy but needs some
 * more care about memory usage.
 * @li <B>For extraction into TANGO CORBA sequence types, the extraction method consumes the
 * memory allocated to store the data and it is the caller responsibility to delete this memory.</B>
 *
 * Extract operators for the following scalar C++ types
 * @li bool
 * @li short
 * @li DevLong
 * @li DevLong64
 * @li float
 * @li double
 * @li unsigned char
 * @li unsigned short
 * @li DevULong
 * @li DevULong64
 * @li string
 * @li DevState
 * @li DevEncoded
 *
 * Extract operators for the following C++ vector types for @b spectrum and @b image attributes :
 * @li vector<bool>
 * @li vector<short>
 * @li vector<DevLong>
 * @li vector<DevLong64>
 * @li vector<float>
 * @li vector<double>
 * @li vector<unsigned char>
 * @li vector<unsigned short>
 * @li vector<DevULong>
 * @li vector<DevULong64>
 * @li vector<string>
 * @li vector<DevState>
 *
 * Extract operators for the following CORBA sequence types <B>with memory consumption</B> :
 * @li DevVarBooleanArray *
 * @li DevVarShortArray *
 * @li DevVarLongArray *
 * @li DevVarLong64Array *
 * @li DevVarFloatArray *
 * @li DevVarDoubleArray *
 * @li DevVarUCharArray *
 * @li DevVarUShortArray *
 * @li DevVarULongArray *
 * @li DevVarULong64Array *
 * @li DevVarStringArray *
 * @li DevVarStateArray *
 * @li DevVarEncodedArray *

 * See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [out] data The attribute data
 * @exception WrongData if requested, DevFailed from device
 */
	bool operator >> (short &data);
/**
 * Extract attribute data for DevEncoded attribute
 *
 * Extract attribute data when the attribute data type is DevEncoded
 * It's the user responsability to release the memory pointed to by the two
 * pointers method parameter.
 * @n Similar method with following signature also exist
 * @b extract(string &,vector<unsigned char> &);
 * @n See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [out] str The DevEncoded string
 * @param [out] data The DevEncoded data pointer
 * @param [out] length The DevEncoded data length
 * @exception WrongData if requested, DevFailed from device
 */
	bool extract(const char *&str,unsigned char *&data,unsigned int &length);
/**
 * Extract only read part of attribute data
 *
 * Extract methods to extract only the read value of an attribute into a C++ vector. The dimension of
 * the read value can be read by using the methods get_dim_x() and get_dim_y() or get_r_dimension().
 * The methods use the same return values as the extraction operators with exceptions triggered by the
 * exception flags. This method exist for the following data type:
 * @li bool DeviceAttribute::extract_read (vector<bool>&);
 * @li bool DeviceAttribute::extract_read (vector<short>&);
 * @li bool DeviceAttribute::extract_read (vector<DevLong>&);
 * @li bool DeviceAttribute::extract_read (vector<DevLong64>&);
 * @li bool DeviceAttribute::extract_read (vector<float>&);
 * @li bool DeviceAttribute::extract_read (vector<double>&);
 * @li bool DeviceAttribute::extract_read (vector<unsigned char>&);
 * @li bool DeviceAttribute::extract_read (vector<unsigned short>&);
 * @li bool DeviceAttribute::extract_read (vector<DevULong>&);
 * @li bool DeviceAttribute::extract_read (vector<DevULong64>&);
 * @li bool DeviceAttribute::extract_read (vector<string>&);
 * @li bool DeviceAttribute::extract_read (vector<DevState>&);
 * @li bool DeviceAttribute::extract_read(string&, vector<unsigned char> &);

 * See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [out] data The attribute data
 * @exception WrongData if requested, DevFailed from device
 */
	bool extract_read (vector<string> &data);
/**
 * Extract only written part of attribute data
 *
 * Extract methods to extract only the set value of an attribute into a C++ vector. The dimension
 * of the set value can be read by using the methods get_written_dim_x() and get_written_dim_y()
 * or get_w_dimension(). The methods use the same return values as the extraction operators with
 * exceptions triggered by the exception flags. This method exist for the following data type:
 * @li bool DeviceAttribute::extract_set (vector<bool>&);
 * @li bool DeviceAttribute::extract_set (vector<short>&);
 * @li bool DeviceAttribute::extract_set (vector<DevLong>&);
 * @li bool DeviceAttribute::extract_set (vector<DevLong64>&);
 * @li bool DeviceAttribute::extract_set (vector<float>&);
 * @li bool DeviceAttribute::extract_set (vector<double>&);
 * @li bool DeviceAttribute::extract_set (vector<unsigned char>&);
 * @li bool DeviceAttribute::extract_set (vector<unsigned short>&);
 * @li bool DeviceAttribute::extract_set (vector<DevULong>&);
 * @li bool DeviceAttribute::extract_set (vector<DevULong64>&);
 * @li bool DeviceAttribute::extract_set (vector<string>&);
 * @li bool DeviceAttribute::extract_set (vector<DevState>&);
 * @li bool DeviceAttribute::extract_set(string &, vector<unsigned char> &);

 * See DeviceAttribute::operator<< for example of inserting and extracting data to/from DeviceAttribute instance
 *
 * @param [out] data The attribute data
 * @exception WrongData if requested, DevFailed from device
 */
	bool extract_set  (vector<string> &data);
//@}
///@privatesection
//	void operator << (short);
	void operator << (DevLong);
	void operator << (double);
	void operator << (string &);
	void operator << (float);
	void operator << (bool);
	void operator << (unsigned short);
	void operator << (unsigned char);
	void operator << (DevLong64);
	void operator << (DevULong);
	void operator << (DevULong64);
	void operator << (DevState);
	void operator << (DevEncoded &);
	void operator << (DevString);
	void operator << (const char *);

	void operator << (vector<short> &);
	void operator << (vector<DevLong> &);
	void operator << (vector<double> &);
	void operator << (vector<string> &);
	void operator << (vector<float> &);
	void operator << (vector<bool> &);
	void operator << (vector<unsigned short> &);
	void operator << (vector<unsigned char> &);
	void operator << (vector<DevLong64> &);
	void operator << (vector<DevULong> &);
	void operator << (vector<DevULong64> &);
	void operator << (vector<DevState> &);

	void operator << (const DevVarShortArray &datum);
	void operator << (const DevVarLongArray &datum);
	void operator << (const DevVarDoubleArray &datum);
	void operator << (const DevVarStringArray &datum);
	void operator << (const DevVarFloatArray &datum);
	void operator << (const DevVarBooleanArray &datum);
	void operator << (const DevVarUShortArray &datum);
	void operator << (const DevVarCharArray &datum);
	void operator << (const DevVarLong64Array &datum);
	void operator << (const DevVarULongArray &datum);
	void operator << (const DevVarULong64Array &datum);
	void operator << (const DevVarStateArray &datum);

	void operator << (DevVarShortArray *datum);
	void operator << (DevVarLongArray *datum);
	void operator << (DevVarDoubleArray *datum);
	void operator << (DevVarStringArray *datum);
	void operator << (DevVarFloatArray *datum);
	void operator << (DevVarBooleanArray *datum);
	void operator << (DevVarUShortArray *datum);
	void operator << (DevVarCharArray *datum);
	void operator << (DevVarLong64Array *datum);
	void operator << (DevVarULongArray *datum);
	void operator << (DevVarULong64Array *datum);
	void operator << (DevVarStateArray *datum);

//
// Insert methods
//

//	void insert(vector<short> &,int,int);
	void insert(vector<DevLong> &,int,int);
	void insert(vector<double> &,int,int);
	void insert(vector<string> &,int,int);
	void insert(vector<float> &,int,int);
	void insert(vector<bool> &,int,int);
	void insert(vector<unsigned short> &,int,int);
	void insert(vector<unsigned char> &,int,int);
	void insert(vector<DevLong64> &,int,int);
	void insert(vector<DevULong> &,int,int);
	void insert(vector<DevULong64> &,int,int);
	void insert(vector<DevState> &,int,int);

//	void insert(const DevVarShortArray &datum,int,int);
	void insert(const DevVarLongArray &datum,int,int);
	void insert(const DevVarDoubleArray &datum,int,int);
	void insert(const DevVarStringArray &datum,int,int);
	void insert(const DevVarFloatArray &datum,int,int);
	void insert(const DevVarBooleanArray &datum,int,int);
	void insert(const DevVarUShortArray &datum,int,int);
	void insert(const DevVarCharArray &datum,int,int);
	void insert(const DevVarLong64Array &datum,int,int);
	void insert(const DevVarULongArray &datum,int,int);
	void insert(const DevVarULong64Array &datum,int,int);
	void insert(const DevVarStateArray &datum,int,int);

//	void insert(DevVarShortArray *datum,int,int);
	void insert(DevVarLongArray *datum,int,int);
	void insert(DevVarDoubleArray *datum,int,int);
	void insert(DevVarStringArray *datum,int,int);
	void insert(DevVarFloatArray *datum,int,int);
	void insert(DevVarBooleanArray *datum,int,int);
	void insert(DevVarUShortArray *datum,int,int);
	void insert(DevVarCharArray *datum,int,int);
	void insert(DevVarLong64Array *datum,int,int);
	void insert(DevVarULongArray *datum,int,int);
	void insert(DevVarULong64Array *datum,int,int);
	void insert(DevVarStateArray *datum,int,int);

	void insert(char *&,unsigned char *&,unsigned int);     // Deprecated. For compatibility purpose
//	void insert(const char *str,unsigned char *data,unsigned int length);
	void insert(const string &,vector<unsigned char> &);
	void insert(string &,vector<unsigned char> &);      // Deprecated. For compatibility purpose
	void insert(const char *,DevVarCharArray *);

//
// Extract operators for  C++ types
//

//	bool operator >> (short &);
	bool operator >> (DevLong &);
	bool operator >> (double &);
	bool operator >> (string&);
	bool operator >> (float &);
	bool operator >> (bool &);
	bool operator >> (unsigned short &);
	bool operator >> (unsigned char &);
	bool operator >> (DevLong64 &);
	bool operator >> (DevULong &);
	bool operator >> (DevULong64 &);
	bool operator >> (DevState &);
	bool operator >> (DevEncoded &);

	bool operator >> (vector<string>&);
	bool operator >> (vector<short>&);
	bool operator >> (vector<DevLong>&);
	bool operator >> (vector<double>&);
	bool operator >> (vector<float>&);
	bool operator >> (vector<bool>&);
	bool operator >> (vector<unsigned short>&);
	bool operator >> (vector<unsigned char>&);
	bool operator >> (vector<DevLong64>&);
	bool operator >> (vector<DevULong>&);
	bool operator >> (vector<DevULong64>&);
	bool operator >> (vector<DevState>&);

	bool operator >> (DevVarShortArray* &datum);
	bool operator >> (DevVarLongArray* &datum);
	bool operator >> (DevVarDoubleArray* &datum);
	bool operator >> (DevVarStringArray* &datum);
	bool operator >> (DevVarFloatArray* &datum);
	bool operator >> (DevVarBooleanArray* &datum);
	bool operator >> (DevVarUShortArray* &datum);
	bool operator >> (DevVarCharArray* &datum);
	bool operator >> (DevVarLong64Array * &datum);
	bool operator >> (DevVarULongArray * &datum);
	bool operator >> (DevVarULong64Array * &datum);
	bool operator >> (DevVarStateArray * &datum);
	bool operator >> (DevVarEncodedArray *&datum);

//
// Extract_xxx methods
//

//	bool extract_read (vector<string>&);
	bool extract_read (vector<short>&);
	bool extract_read (vector<DevLong>&);
	bool extract_read (vector<double>&);
	bool extract_read (vector<float>&);
	bool extract_read (vector<bool>&);
	bool extract_read (vector<unsigned short>&);
	bool extract_read (vector<unsigned char>&);
	bool extract_read (vector<DevLong64>&);
	bool extract_read (vector<DevULong>&);
	bool extract_read (vector<DevULong64>&);
	bool extract_read (vector<DevState>&);
	bool extract_read (string &,vector<unsigned char> &);

//	bool extract_set  (vector<string>&);
	bool extract_set  (vector<short>&);
	bool extract_set  (vector<DevLong>&);
	bool extract_set  (vector<double>&);
	bool extract_set  (vector<float>&);
	bool extract_set  (vector<bool>&);
	bool extract_set  (vector<unsigned short>&);
	bool extract_set  (vector<unsigned char>&);
	bool extract_set  (vector<DevLong64>&);
	bool extract_set  (vector<DevULong>&);
	bool extract_set  (vector<DevULong64>&);
	bool extract_set  (vector<DevState>&);
	bool extract_set  (string &,vector<unsigned char> &);

//	bool extract(const char *&,unsigned char *&,unsigned int &);
	bool extract(char *&,unsigned char *&,unsigned int &);      // Deprecated. For compatibility purpose
	bool extract(string &,vector<unsigned char> &);

///@publicsection

/**@name Exception and error related methods methods
 */
//@{
/**
 * Set exception flag
 *
 * Is a method which allows the user to switch on/off exception throwing for trying to extract data from an
 * empty DeviceAttribute object. The default is to not throw exception. The following flags are supported :
 * @li @b isempty_flag - throw a WrongData exception (reason = API_EmptyDbDatum) if user tries to extract
 *       data from an empty DeviceAttribute object. By default, this flag is set
 * @li @b wrongtype_flag - throw a WrongData exception (reason = API_IncompatibleArgumentType) if user
 *       tries to extract data with a type different than the type used for insertion. By default, this flag
 *       is not set
 * @li @b failed_flag - throw an exception when the user try to extract data fromthe DeviceAttribute object and
 * an error was reported by the server when the user try to read the attribute. The type of the exception
 * thrown is the type of the error reported by the server. By default, this flag is set.
 * @li @b unknown_format_flag - throw an exception when the user try to get the attribute data format from
 * the DeviceAttribute object when this information is not yet available. This information is available
 * only after the read_attribute call has been sucessfully executed. The type of the exception thrown is
 * WrongData exception (reason = API_EmptyDeviceAttribute). By default, this flag is not set.
 *
 * @param [in] fl The exception flag
 */
	void exceptions(bitset<numFlags> fl) {exceptions_flags = fl;}
/**
 * Get exception flag
 *
 * Returns the whole exception flags.
 * The following is an example of how to use these exceptions related methods
 * @code
 * DeviceAttribute da;
 *
 * bitset<DeviceAttribute::numFlags> bs = da.exceptions();
 * cout << "bs = " << bs << endl;
 *
 * da.set_exceptions(DeviceAttribute::wrongtype_flag);
 * bs = da.exceptions();
 *
 * cout << "bs = " << bs << endl;
 * @endcode
 *
 * @return The exception flag
 */
	bitset<numFlags> exceptions() {return exceptions_flags;}
/**
 * Reset one exception flag
 *
 * Resets one exception flag
 *
 * @param [in] fl The exception flag
 */
	void reset_exceptions(except_flags fl) {exceptions_flags.reset((size_t)fl);}
/**
 * Set one exception flag
 *
 * Sets one exception flag. See DeviceAttribute::exceptions() for a usage example.
 *
 * @param [in] fl The exception flag
 */
	void set_exceptions(except_flags fl) {exceptions_flags.set((size_t)fl);}
/**
 * Check if the call failed
 *
 * Returns a boolean set to true if the server report an error when the attribute was read.
 *
 * @return A boolean set to true if the call failed
 */
	bool has_failed() {DevErrorList *tmp;if ((tmp=err_list.operator->())==NULL)return false;
	                   else{if (tmp->length() != 0)return true;else return false;}}
/**
 * Get the error stack
 *
 * Returns the error stack reported by the server when the attribute was read.
 * The following is an example of the three available ways to get data out of a DeviceAttribute object
 * @code
 * DeviceAttribute da;
 * vector<short> attr_data;
 *
 * try
 * {
 *    da = device->read_attribute("Attr");
 *    da >> attr_data;
 * }
 * catch (DevFailed &e)
 * {
 *    ....
 * }
 *
 * ------------------------------------------------------------------------
 *
 * DeviceAttribute da;
 * vector<short> attr_data;
 *
 * da.reset_exceptions(DeviceAttribute::failed_flag);
 *
 * try
 * {
 *     da = device->read_attribute("Attr");
 * }
 * catch (DevFailed &e)
 * {
 *     .....
 * }
 *
 * if (!(da >> attr_data))
 * {
 *     DevErrorList &err = da.get_err_stack();
 *     .....
 * }
 * else
 * {
 *     .....
 * }
 *
 * ----------------------------------------------------------------------
 *
 * DeviceAttribute da;
 * vector<short> attr_data;
 *
 * try
 * {
 *     da = device->read_attribute("Attr");
 * }
 * catch (DevFailed &e)
 * {
 *     ......
 * }
 *
 * if (da.has_failed())
 * {
 *     DevErrorList &err = da.get_err_stack();
 *     ....
 * }
 * else
 * {
 *     da >> attr_data;
 * }
 * @endcode
 * The first way uses the default behaviour of the DeviceAttribute
 * object which is to throw an exception when the user try to extract data when the server reports an error
 * when the attribute was read. In the second way, the DeviceAttribute object
 * now does not throw "DevFailed" exception any more and the return value of the extractor operator is checked.
 * IN the last case, the attribute data validity is checked before
 * trying to extract them.
 *
 * @return The error stack
 */
	const DevErrorList &get_err_stack() {return err_list.in();}
//@}
/**@name Miscellaneous methods */
//@{
/**
 * Check is the instance is empty
 *
 * is_empty() is a boolean method which returns true or false depending on whether the DeviceAttribute
 * object contains data or not. It can be used to test whether the DeviceAttribute has been initialized or not
 * e.g.
 * @code
 * string parity;
 * DeviceAttribute sl_parity = my_device->read_attribute("parity");
 * if (! sl_read.is_empty())
 * {
 *     sl_parity >> parity;
 * }
 * else
 * {
 *     cout << " no parity attribute defined for serial line !" << endl;
 * }
 * @endcode
 *
 * @return Boolean set to true if the instance is empty
 * @exception WrongData if requested
 */
	bool is_empty();
/**
 * Returns the name of the attribute
 *
 * Returns the name of the attribute
 *
 * @return The attribute name
 */
	string &get_name() {return name;}
/**
 * Set attribute name
 *
 * Set attribute name
 *
 * @param na The attribute name
 */
	void set_name(string &na) {name =  na;}
/**
 * Set attribute name
 *
 * Set attribute name
 *
 * @param na The attribute name
 */
	void set_name(const char *na) {string str(na);name = str;}
/**
 * Get attribute X dimension
 *
 * Returns the attribute read x dimension
 *
 * @return The attribute X dimension
 */
	int get_dim_x() {return dim_x;}
/**
 * Get attribute Y dimension
 *
 * Returns the attribute read y dimension
 *
 * @return The attribute Y dimension
 */
	int get_dim_y() {return dim_y;}
/**
 * Get the attribute write X dimension
 *
 * Returns the attribute write x dimension
 *
 * @return The attribute write X dimension
 */
	int get_written_dim_x() {return w_dim_x;}
/**
 * Get the attribute write Y dimension
 *
 * Returns the attribute write y dimension
 *
 * @return The attribute write Y dimension
 */
	int get_written_dim_y() {return w_dim_y;}
/**
 * Get the attribute read dimensions
 *
 * Returns the attribute read dimensions
 *
 * @return The attribute read dimensions
 */
	AttributeDimension get_r_dimension();
/**
 * Get the attribute write dimensions
 *
 * Returns the attribute write dimensions
 *
 * @return The attribute write dimensions
 */
	AttributeDimension get_w_dimension();
/**
 * Get the number of read value
 *
 * Returns the number of read values
 *
 * @return The read value number
 */
	long get_nb_read();
/**
 * Get the number of written value
 *
 * Returns the number of written values. Here is an example of these last methods usage.
 * @code
 * DeviceAttribute da;
 * vector<short> attr_data;
 *
 * try
 * {
 *    da = device->read_attribute("Attr");
 *    da >> attr_data;
 * }
 * catch (DevFailed &e)
 * {
 *    ....
 * }
 *
 * long read = da.get_nb_read();
 * long written = da.get_nb_written();
 *
 * for (long i = 0;i < read;i++)
 *    cout << "Read value " << i+1 << " = " << attr_data[i] << endl;
 *
 * for (long j = 0; j < written;j++)
 *    cout << "Last written value " << j+1 << " = " << attr_data[j + read] << endl;
 * @endcode
 *
 * @return The read value number
 */
	long get_nb_written();
/**
 * Get attribute quality factor
 *
 * Returns the quality of the attribute: an enumerate type which can be one of:
 * @li ATTR_VALID
 * @li ATTR_INVALID
 * @li ATTR_ALARM
 * @li ATTR_CHANGING
 * @li ATTR_WARNING
 *
 * @return The attribute quality
 */
	AttrQuality &get_quality() {return quality;}
/**
 * Get attribute data type
 *
 * Returns the type of the attribute data.
 *
 * @return The attribute data type
 */
	int get_type();
/**
 * Get attribute data format
 *
 * Returns the attribute data format. Note that this information is valid only after the call to the device has
 * been executed. Otherwise the FMT_UNKNOWN value of the AttrDataFormat enumeration is returned or
 * an exception is thrown according to the object exception flags.
 *
 * @return The attribute data format
 */
	AttrDataFormat get_data_format();
/**
 * Get attribute read date
 *
 * Returns a reference to the time when the attribute was read in server
 *
 * @return The attribute read date
 */
	TimeVal &get_date() {return time;}
//@}
///@privatesection
	friend ostream &operator<<(ostream &,DeviceAttribute &);

protected :
///@privatesection
	bitset<numFlags> 	exceptions_flags;
	void del_mem(int);
	bool check_for_data();
	bool check_wrong_type_exception();
	int  check_set_value_size(int seq_length);

    class DeviceAttributeExt
    {
    public:
        DeviceAttributeExt() {};
        DeviceAttributeExt & operator=(const DeviceAttributeExt &);

        void deep_copy(const DeviceAttributeExt &);
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DeviceAttributeExt>  ext;
#else
	DeviceAttributeExt	            *ext;		// Class extension
#endif
};

#endif /* _DEVICEATTRIBUTE_H */

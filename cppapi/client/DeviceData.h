//////////////////////////////////////////////////////////////////
//
// DeviceData.h - include file for TANGO device api class DeviceData
//
//
// Copyright (C) :      2012
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

#ifndef _DEVICEDATA_H
#define _DEVICEDATA_H


/****************************************************************************************
 * 																						*
 * 					The DeviceData class					   						    *
 * 					--------------------											    *
 * 																						*
 ***************************************************************************************/


/**
 * Fundamental type for sending and receiving data from device commands
 *
 * This is the fundamental type for sending and receiving data from device commands. The values can be
 * inserted and extracted using the operators << and >> respectively and insert() for mixed data types. A
 * status flag indicates if there is data in the DbDatum object or not. An additional flag allows the user to
 * activate exceptions.
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DeviceData
{

public :
///@privatesection
//
// constructor methods
//
	enum except_flags
	{
		isempty_flag,
		wrongtype_flag,
		numFlags
	};

//	DeviceData();
	DeviceData(const DeviceData &);
	DeviceData & operator=(const DeviceData &);
#ifdef HAS_RVALUE
	DeviceData(DeviceData &&);
	DeviceData & operator=(DeviceData &&);
#endif
	~DeviceData();

	CORBA::Any_var any;

///@publicsection
/**@name Constructors */
//@{
/**
 * Create a DeviceData object.
 *
 * Default constructor. The instance is empty
 *
 */
    DeviceData();
//@}

/**@name Inserters and Extractors */
//@{
/**
 * The insert operators
 *
 * The insert operators are specified for the following C++ types :
 * @li bool
 * @li short
 * @li unsigned short
 * @li DevLong
 * @li DevULong
 * @li DevLong64
 * @li DevULong64
 * @li DevState
 * @li DevEncoded
 * @li float
 * @li double
 * @li string
 * @li char* (insert only)
 * @li const char *
 * @li vector<unsigned char>
 * @li vector<string>
 * @li vector<short>
 * @li vector<unsigned short>
 * @li vector<DevLong>
 * @li vector<DevULong>
 * @li vector<DevLong64>
 * @li vector<DevULong64>
 * @li vector<float>
 * @li vector<double>
 *
 * Operators exist for inserting the native TANGO CORBA sequence types. These can be
 * useful for programmers who want to use the TANGO api internally in their device servers and do not want
 * to convert from CORBA to C++ types. Insert and extract operators exist for the following types :
 * @li DevVarUCharArray *
 * @li DevVarShortArray *
 * @li DevVarUShortArray *
 * @li DevVarLongArray *
 * @li DevVarULongArray *
 * @li DevVarLong64Array *
 * @li DevVarULong64Array *
 * @li DevVarFloatArray *
 * @li DevVarDoubleArray *
 * @li DevVarStringArray *
 * @li DevVarLongStringArray *
 * @li DevVarDoubleStringArray *
 *
 * <B> Insertion by pointers takes full ownership of the pointed to memory. The insertion copy the data in
 * the DeviceData object and delete the pointed to memory. Therefore, the memory is not more usable
 * after the insertion.</B>
 *
 * Operators also exist for inserting TANGO CORBA sequence type by reference. The insertion copy the
 * data into the DeviceData object. Insert operator exist for the following types :
 * @li DevVarUCharArray &
 * @li DevVarShortArray &
 * @li DevVarUShortArray &
 * @li DevVarLongArray &
 * @li DevVarULongArray &
 * @li DevVarLong64Array&
 * @li DevVarULong64Array&
 * @li DevVarFloatArray &
 * @li DevVarDoubleArray &
 * @li DevVarStringArray &
 * @li DevVarLongStringArray&
 * @li DevVarDoubleStringArray&
 *
 * Special care has been taken to avoid memory copy between the network layer and the user application.
 * Nevertheless, C++ vector types are not the CORBA native type and one copy is unavoidable when using
 * vectors. Using the native TANGO CORBA sequence types avoid any copy. When using these TANGO
 * CORBA sequence types, insertion into the DeviceData object consumes the memory pointed to by the
 * pointer. After the insertion, it is not necessary to delete the memory. It will be done by the destruction of
 * the DeviceData object. For extraction, the pointer used for the extraction points into memory inside the
 * DeviceData object and you should not delete it
 * Here is an example of creating, inserting and extracting some data type from/into DeviceData object :
 * @code
 * DeviceData my_short, my_long, my_string;
 * DeviceData my_float_vector, my_double_vector;
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
 * my_long >> a_long; // extract a long
 *
 * my_string << string(“estas lista a bailar el tango ?”); // insert a string
 * my_string >> a_string; // extract a string
 *
 * my_float_vector << a_float_vector // insert a vector of floats
 * my_float_vector >> a_float_vector; // extract a vector of floats
 *
 * my_double_vector << a_double_vector; // insert a vector of doubles
 * my_double_vector >> a_double_vector; // extract a vector of doubles
 *
 * //
 * // Example of memory management with TANGO sequence types without memory leaks
 * //
 *
 * for (int i = 0;i < 10;i++)
 * {
 *    DeviceData din,dout;
 *    DevVarLongArray *in = new DevVarLongArray();
 *    in->length(2);
 *    (*in)[0] = 2;
 *    (*in)[1] = 4;
 *    din << in;
 *    try
 *    {
 *       dout = device->command_inout(“Cmd”,din);
 *    }
 *    catch(DevFailed &e)
 *    {
 *       ....
 *    }
 *    const DevVarLongArray *out;
 *    dout >> out;
 *    cout << “Received value = “ << (*out)[0];
 * }
 * @endcode
 *
 * @param [in] datum The data to be inserted
 * @exception WrongData if requested
 */
	void operator << (bool datum) {any <<= CORBA::Any::from_boolean(datum);}
/**
 * Insert data into a DeviceData for the DevVarLongStringArray data type
 *
 * Insert data into a DeviceData for the DevVarLongStringArray data type
 *
 * @param [in] vl The long vector to be inserted
 * @param [in] vs The string vector to be inserted
 * @exception WrongData if requested
 */
	void insert(vector<DevLong> &vl, vector<string>&vs);
/**
 * Insert data into a DeviceData for the DevVarDoubleStringArray data type
 *
 * Insert data into a DeviceData for the DevVarDoubleStringArray data type
 *
 * @param [in] vd The double vector to be inserted
 * @param [in] vs The string vector to be inserted
 * @exception WrongData if requested
 */
	void insert(vector<double> &vd, vector<string> &vs);
/**
 * Insert data into a DeviceData for the DevEncoded data type
 *
 * Insert data into a DeviceData for the DevEncoded data type
 * @n Similar methods with different parameters data type exist for
 * inserting data for a DevEncoded data type
 * @li <B>void insert(const char *str, DevVarCharArray *data);</B>
 * @li <B>void insert(const char *str, unsigned char *data,unsigned int length);</B>
 *
 * These three methods do not take ownership of the memory used for the data buffer.
 *
 * @param [in] str The string part of the DevEncoded instance
 * @param [in] buffer The data part of the DevEncoded instance
 * @exception WrongData if requested
 */
	void insert(const string &str,vector<unsigned char> &buffer);
/**
 * The extract operators
 *
 * The extract operators are specified for the following C++ types :
 * @li bool
 * @li short
 * @li unsigned short
 * @li DevLong
 * @li DevULong
 * @li DevLong64
 * @li DevULong64
 * @li float
 * @li double
 * @li string
 * @li char* (insert only)
 * @li const char *
 * @li DevEncoded
 * @li DevState
 * @li vector<unsigned char>
 * @li vector<string>
 * @li vector<short>
 * @li vector<unsigned short>
 * @li vector<DevLong>
 * @li vector<DevULong>
 * @li vector<DevLong64>
 * @li vector<DevULong64>
 * @li vector<float>
 * @li vector<double>
 *
 * Operators exist for extracting the native TANGO CORBA sequence types. These can be
 * useful for programmers who want to use the TANGO api internally in their device servers and do not want
 * to convert from CORBA to C++ types. Insert and extract operators exist for the following types :
 * @li const DevVarUCharArray *
 * @li const DevVarShortArray *
 * @li const DevVarUShortArray *
 * @li const DevVarLongArray *
 * @li const DevVarULongArray *
 * @li const DevVarLong64Array *
 * @li const DevVarULong64Array *
 * @li const DevVarFloatArray *
 * @li const DevVarDoubleArray *
 * @li const DevVarStringArray *
 * @li const DevVarLongStringArray *
 * @li const DevVarDoubleStringArray *
 *
 * <B>Note that when using extraction by pointers, the pointed to memory is inside
 * the DeviceData object and its lifetime is the same than the DeviceData object lifetime.</B>
 *
 * Special care has been taken to avoid memory copy between the network layer and the user application.
 * Nevertheless, C++ vector types are not the CORBA native type and one copy is unavoidable when using
 * vectors. Using the native TANGO CORBA sequence types avoid any copy. When using these TANGO
 * CORBA sequence types, for extraction, the pointer used for the extraction points into memory inside the
 * DeviceData object and you should not delete it
 * See DeviceData::operator<< for inserters and extractors usage example
 *
 * @param [out] datum The variable which will be initalized
 * @return Boolean set to false if the extraction failed
 * @exception WrongData if requested
 */
	bool operator >> (bool &datum);
/**
 * Extract data from a DeviceData for the DevVarLongStringArray data type
 *
 * Extract data from a DeviceData for the DevVarLongStringArray data type
 *
 * @param [out] vl The long vector which will be inialized
 * @param [out] vs The string vector to be initialized
 * @return Boolean set to false if the extraction failed
 * @exception WrongData if requested
 */
	bool extract(vector<DevLong> &vl, vector<string> &vs);
/**
 * Extract data from a DeviceData for the DevVarDoubleStringArray data type
 *
 * Extract data from a DeviceData for the DevVarDoubleStringArray data type
 *
 * @param [out] vd The double vector to be initialized
 * @param [out] vs The string vector to be initialized
 * @return Boolean set to false if the extraction failed
 * @exception WrongData if requested
 */
	bool extract(vector<double> &vd, vector<string> &vs);
/**
 * Extract data from a DeviceData for the DevEncoded data type
 *
 * Extract command data when the command data type is DevEncoded
 * Similar method with following signature also exist
 * @li <B>extract(string &str,vector<unsigned char> &data);</B>
 *
 * @param [out] str The DevEncoded string
 * @param [out] data The DevEncoded data pointer
 * @param [out] length The DevEncoded data length
 * @return Boolean set to false if the extraction failed
 * @exception WrongData if requested
 */
    bool extract(const char *&str,const unsigned char *&data,unsigned int &length);
//@}

///@privatesection
//
// insert methods for native C++ types
//
//	void operator << (bool datum) {any <<= CORBA::Any::from_boolean(datum);}
	void operator << (short datum) {any <<= datum;}
	void operator << (unsigned short datum) {any <<= datum;}
	void operator << (DevLong datum) {any <<= datum;}
	void operator << (DevULong datum) {any <<= datum;}
	void operator << (DevLong64 datum) {any <<= datum;}
	void operator << (DevULong64 datum) {any <<= datum;}
	void operator << (float datum) {any <<= datum;}
	void operator << (double datum) {any <<= datum;}
	void operator << (char *&datum) {any <<= datum;}
	void operator << (const char *&datum) {any <<= datum;}
	void operator << (string &datum) {any <<= datum.c_str();}
	void operator << (vector<unsigned char>&);
	void operator << (vector<string>&);
	void operator << (vector<short>&);
	void operator << (vector<unsigned short>&);
	void operator << (vector<DevLong> &);
	void operator << (vector<DevULong> &);
	void operator << (vector<DevLong64> &);
	void operator << (vector<DevULong64> &);
	void operator << (vector<float>&);
	void operator << (vector<double>&);
	void operator << (DevState datum) {(any.inout()) <<= datum;}
	void operator << (DevEncoded &datum) {(any.inout()) <<= datum;}

//	void insert(vector<DevLong>&, vector<string>&);
//	void insert(vector<double>&, vector<string>&);

//	void insert(const string &,vector<unsigned char>&);
	void insert(const char *,DevVarCharArray *);
	void insert(const char *,unsigned char *,unsigned int);

//
// insert methods for TANGO CORBA sequence types
//

	inline void operator << (DevVarCharArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarShortArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarUShortArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLong64Array *datum) { any.inout() <<= datum;}
	inline void operator << (DevVarULongArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarULong64Array* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarFloatArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarStringArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongStringArray* datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleStringArray* datum) { any.inout() <<= datum;}

	inline void operator << (DevVarCharArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarShortArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarUShortArray datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLong64Array &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarULongArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarULong64Array &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarFloatArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarStringArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarLongStringArray &datum) { any.inout() <<= datum;}
	inline void operator << (DevVarDoubleStringArray &datum) { any.inout() <<= datum;}

//
// extract methods for native C++ types
//

//	bool operator >> (bool&);
	bool operator >> (short&);
	bool operator >> (unsigned short&);
	bool operator >> (DevLong&);
	bool operator >> (DevULong&);
	bool operator >> (DevLong64&);
	bool operator >> (DevULong64&);
	bool operator >> (float&);
	bool operator >> (double&);
	bool operator >> (const char*&);
	bool operator >> (string&);

	bool operator >> (vector<unsigned char>&);
	bool operator >> (vector<string>&);
	bool operator >> (vector<short>&);
	bool operator >> (vector<unsigned short>&);
	bool operator >> (vector<DevLong>&);
	bool operator >> (vector<DevULong>&);
	bool operator >> (vector<DevLong64>&);
	bool operator >> (vector<DevULong64>&);
	bool operator >> (vector<float>&);
	bool operator >> (vector<double>&);
	bool operator >> (DevState&);
//	bool extract(vector<DevLong>&, vector<string>&);
//	bool extract(vector<double>&, vector<string>&);

//  bool extract(const char *&,unsigned char *&,unsigned int &);
    bool extract(string &,vector<unsigned char> &);

//
// extract methods for TANGO CORBA sequence types
//

	bool operator >> (const DevVarCharArray* &datum);
	bool operator >> (const DevVarShortArray* &datum);
	bool operator >> (const DevVarUShortArray* &datum);
	bool operator >> (const DevVarLongArray* &datum);
	bool operator >> (const DevVarLong64Array* &datum);
	bool operator >> (const DevVarULongArray* &datum);
	bool operator >> (const DevVarULong64Array* &datum);
	bool operator >> (const DevVarFloatArray* &datum);
	bool operator >> (const DevVarDoubleArray* &datum);
	bool operator >> (const DevVarStringArray* &datum);
	bool operator >> (const DevVarLongStringArray* &datum);
	bool operator >> (const DevVarDoubleStringArray* &datum);

	bool operator >> (const DevEncoded* &datum);
	bool operator >> (DevEncoded &datum);

///@publicsection
/**@name Exception and error related methods methods
 */
//@{
/**
 * Set exception flag
 *
 * Is a method which allows the user to switch on/off exception throwing for trying to extract data from an
 * empty DeviceData object. The default is to not throw exception. The following flags are supported :
 * @li @b isempty_flag - throw a WrongData exception (reason = API_EmptyDbDatum) if user tries to extract
 *       data from an empty DeviceAttribute object. By default, this flag is set
 * @li @b wrongtype_flag - throw a WrongData exception (reason = API_IncompatibleArgumentType) if user
 *       tries to extract data with a type different than the type used for insertion. By default, this flag
 *       is not set
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
 * DeviceData dd;
 *
 * bitset<DeviceData::numFlags> bs = dd.exceptions();
 * cout << "bs = " << bs << endl;
 *
 * da.set_exceptions(DeviceData::wrongtype_flag);
 * bs = dd.exceptions();
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
 * Sets one exception flag. See DeviceData::exceptions() for a usage example.
 *
 * @param [in] fl The exception flag
 */
	void set_exceptions(except_flags fl) {exceptions_flags.set((size_t)fl);}
//@}

/**@name miscellaneous methods */
//@{
/**
 * Check if the DeviceData instance is empty
 *
 * is_empty() is a boolean method which returns true or false depending on whether the DeviceData object
 * contains data or not. It can be used to test whether the DeviceData has been initialized or not e.g.
 * @code
 * string string_read;
 *
 * DeviceData sl_read = my_device->command_inout(“ReadLine”);
 *
 * if (! sl_read.is_empty())
 * {
 *    sl_read >> string_read;
 * }
 * else
 * {
 *    cout << “ no data read from serial line !” << endl;
 * }
 * @endcode
 *
 * @return Boolean set to true is the instance is empty
 */
	bool is_empty() {return any_is_null();}
/**
 * Get Tango data type of the included data
 *
 * This method returns the Tango data type of the data inside the DeviceData object
 *
 * @return The data type
 */
	int get_type();
//@}

/**
 * Print a DeviceData instance
 *
 * Is an utility function to easily print the contents of a DeviceData object. This function knows all types
 * which could be inserted in a DeviceData object and print them accordingly. A special string is printed if
 * the DeviceData object is empty
 * @code
 * DeviceProxy *dev = new DeviceProxy(“...”);
 * DeviceData out;
 *
 * out = dev->command_inout(“MyCommand”);
 * cout << “Command returned: ” << out << endl;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] dd The instance to be printed
 */
	friend ostream &operator<<(ostream &str,DeviceData &dd);

protected :
///@privatesection
	bool any_is_null();

	bitset<numFlags> 	exceptions_flags;

private:
    class DeviceDataExt
    {
    public:
        DeviceDataExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DeviceDataExt>   ext;
#else
	DeviceDataExt		        *ext;			// Class extension
#endif
};



#endif /* _DEVICEDATA_H */

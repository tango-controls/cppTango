//===================================================================================================================
//
// DevicePipe.h - include file for TANGO device api class DevicePipe
//
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
// $Revision$
//
//===================================================================================================================

#ifndef _DEVICEPIPE_H
#define _DEVICEPIPE_H

/**
 * Fundamental type for extracting data from a device pipe blob
 *
 * This is the fundamental type for extracting data from a device pipe blob
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

template <typename T>
struct DataElement
{
/**
 * Create a DataElement object.
 *
 * Create a DataElement object for device pipe blob extraction
 *
 * @param [in] name The data element name
 * @param [in] value The data element value
 */
	DataElement(const string &name,T value);
/**
 * Create a DataElement object.
 *
 * Create a DataElement object for device pipe blob extraction
 *
 * @param [in] name The data element name
 */
	DataElement(const string &name);
/**
 * Create a DataElement object.
 *
 * Create a DataElement object for device pipe blob extraction. Usefull for extraction into TANGO CORBA
 * sequence. See DevicePipeBlob extraction method
 *
 * @param [in] value The data element value
 */
	DataElement(T value);

	DataElement();

	string		name;		///< The data element name
	T			value;		///< The data element value
};

template <typename T>
DataElement<T>::DataElement(const string &_na,T _val):name(_na),value(_val)
{
}

template <typename T>
DataElement<T>::DataElement(const string &_na):name(_na)
{
}

template <typename T>
DataElement<T>::DataElement(T _val):value(_val)
{
}

template <typename T>
DataElement<T>::DataElement()
{
}

/**
 * A device pipe blob
 *
 * A device pipe blob. A blob is used to pack data to be sent through device pipe
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DevicePipeBlob
{
public:
///@privatesection

	enum except_flags
	{
		isempty_flag,
		wrongtype_flag,
		notenoughde_flag,
		blobdenamenotset_flag,
		mixing_flag,
		numFlags
	};

///@publicsection
/**@name Constructors */
//@{
/**
 * Create a DevicePipeBlob object.
 *
 * Default constructor.
 *
 */
	DevicePipeBlob();
/**
 * Create a DevicePipeBlob object with name
 *
 * Create one instance of the DevicePipeBlob class and set its name
 *
 * @param [in] blob_name The blob name
 */
	DevicePipeBlob(const string &blob_name);
//@}

/**@name Get/Set methods */
//@{
/**
 * Set blob name
 *
 * Set the blob name
 *
 * @param [in] blob_name The blob name
 */
	void set_name(const string &blob_name) {name=blob_name;}
/**
 * Get blob name
 *
 * Get the blob name
 *
 * @return The blob name
 */
	const string &get_name() {return name;}
//@}


/**@name Inserting data into a DevicePipeBlob
 */
//@{
/**
 * Insert data into a data blob
 *
 * According to the data to be inserted into the  blob data element, several kinds of insetor methods have been
 * implemented. You can insert data from:
 * @li Scalar data type
 * @li vector
 * @li TANGO CORBA sequence types (by reference)
 * @li TANGO CORBA sequence types (by pointer)
 * @li DataElement<T> with T being scalar data type
 * @li DataElement<T> with T being vector
 * @li DataElement<T> with T being TANGO CORBA sequence type (by reference)
 * @li DataElement<T> with T being TANGO CORBA sequence type (by pointer)
 *
 * When inserting data using a DataElement<T> instance, the data element name is also set.
 * <B>For insertion from TANGO CORBA sequence type pointer, the insertion method consumes the
 * memory allocated to store the data and it will be freed by the Tango layer.</B>
 *
 * Insert operators for the following scalar C++ types (and DataElement<T>)
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
 * @li DevString
 * @li string
 * @li DevState
 * @li DevEncoded
 *
 * Insert operators for the following C++ vector types  (and DataElement<T>)
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
 * @li vector<DevSstring>
 * @li vector<string>
 * @li vector<DevState>
 *
 * Insert operators for the following CORBA sequence types (and DataElement<T>):
 * @li DevVarBooleanArray &
 * @li DevVarShortArray &
 * @li DevVarLongArray &
 * @li DevVarLong64Array &
 * @li DevVarFloatArray &
 * @li DevVarDoubleArray &
 * @li DevVarUCharArray &
 * @li DevVarUShortArray &
 * @li DevVarULongArray &
 * @li DevVarULong64Array &
 * @li DevVarStringArray &
 * @li DevVarStateArray &
 *
 * Insert operators for the following CORBA sequence types <B>with memory consumption</B>  (and DataElement<T>):
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
 * Here is an example of inserting data into a DevicePipeBlob instance. We insert
 * 3 data element into the pipe blob with a DevLong, a vector of doubles and finally an array of 100 unsigned short
 * @code
 * DevicePipeBlob dpb("MyBlob");
 *
 * vector<string> de_names = {"FirstDE","SecondDE","ThirdDE"};
 * dpb.set_data_elt_names(de_names);
 *
 * DevLong dl = 666;
 * vector<double> v_db = {1.11,2.22};
 * unsigned short *array = new unsigned short [100]; 	// The array is populated by a way or another
 *
 * DevVarUShortArray *dvush = create_DevVarUShortArray(array,100);
 *
 * try
 * {
 *    dpb << dl << v_db << dvush;
 * }
 * catch (DevFailed &e)
 * {
 *    cout << "DevicePipeBlob insertion failed" << endl;
 *    ....
 * }
 *
 * @endcode
 * The same example of inserting data into a DevicePipeBlob instance when we want to set the data element name.
 * @code
 * DevicePipeBlob dpb("MyBlob");
 *
 * DataElement<DevLong> de_dl("FirstDE",666);
 *
 * vector<double> v_db = {1.11,2.22};
 * DataElement<vector<double> > de_v_db("SecondDE",v_db);
 *
 * unsigned short *array = new unsigned short [100]; 	// The array is populated by a way or another
 * DevVarUShortArray *dvush = create_DevVarUShortArray(array,100);
 * DataElement<DevVarUShortArray *> de_dvush("ThirdDE",array);
 *
 * try
 * {
 *    dpb << de_dl << de_v_db << de_dvush;
 * }
 * catch (DevFailed &e)
 * {
 *    cout << "DevicePipeBlob insertion failed" << endl;
 *    ....
 * }
 *
 * ...
 * @endcode
 * It is also possible to do the insertion in a third way
 * @code
 * DevicePipeBlob dpb("MyBlob");
 *
 * vector<string> de_names{"FirstDE","SecondDE","ThirdDE"};
 * dpb.set_data_elt_names(de_names);
 *
 * DevLong dl = 666;
 * vector<double> v_db = {1.11,2.22};
 * unsigned short *array = new unsigned short [100]; 	// The array is populated by a way or another
 *
 * DevVarUShortArray *dvush = create_DevVarUShortArray(array,100);
 *
 * dpb["FirstDE"] << dl;
 * dpb["SecondDE"] << v_db;
 * dpb["ThirdDE"] << dvush;
 *
 * @endcode
 *
 * @param [in] datum The data to be inserted into the DevicePipeBlob
 * @exception WrongData if requested
 */
	DevicePipeBlob & operator << (short &datum);
/**
 * Set blob data element number
 *
 * Set the blob data element number
 *
 * @param [in] nb The blob data element number
 */
	void set_data_elt_nb(size_t nb);
/**
 * Set blob data element number and names
 *
 * Set the blob data element number and names. The data element number is the number of names in the input
 * parameter.
 *
 * @param [in] names The blob data element names
 */
	void set_data_elt_names(vector<string> &names);
//@}

/**@name Extracting data from a DevicePipeBlob
 */
//@{
/**
 * Extract data from a data blob
 *
 * According to the data inside blob data element, several kinds of extractor methods have been implemented. You
 * can extract data into:
 * @li Scalar data type
 * @li vector
 * @li TANGO CORBA sequence types
 * @li DataElement<T> with T being scalar data type
 * @li DataElement<T> with T being vector
 * @li DataElement<T> with T being TANGO CORBA sequence type
 *
 * When extracting data using a DataElement<T> instance, the data element name is also returned.
 * <B>For extraction into C++ vector, data are copied into the vector. It is not the case for extraction into TANGO
 * CORBA sequence type. For extraction into TANGO CORBA sequence types, the extraction method consumes the
 * memory allocated to store the data and it is the caller responsibility to delete this memory.</B>
 *
 * Extract operators for the following scalar C++ types (and DataElement<T>)
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
 * Extract operators for the following C++ vector types  (and DataElement<T>)
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
 * Extract operators for the following CORBA sequence types <B>with memory consumption</B>  (and DataElement<T>):
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
 * Here is an example of extracting data from a DevicePipeBlob instance. We know that the DevicePipeBlob contains
 * 3 data element with a DevLong, an array of doubles and finally an array of unsigned short
 * @code
 * DevicePipeBlob dpb = .....
 *
 * DevLong dl;
 * vector<double> v_db;
 * DevVarUShortArray *dvush = new DevVarUShortArray();
 *
 * try
 * {
 *    dpb >> dl >> v_db >> dvush;
 * }
 * catch (DevFailed &e)
 * {
 *    cout << "DevicePipeBlob extraction failed" << endl;
 *    ....
 * }
 *
 * delete dvush;
 * @endcode
 * The same example of extracting data from a DevicePipeBlob instance when we want to retrieve the data element name.
 * @code
 * DevicePipeBlob dpb = .....
 *
 * DataElement<DevLong> de_dl;
 * DataElement<vector<double> > de_v_db;
 * DataElement<DevVarUShortArray *> de_dvush(new DevVarUShortArray());
 *
 * try
 * {
 *    dpb >> de_dl >> de_v_db >> de_dvush;
 * }
 * catch (DevFailed &e)
 * {
 *    cout << "DevicePipeBlob extraction failed" << endl;
 *    ....
 * }
 *
 * cout << "Data element name = " << de_dl.name << " - Value = " << de_dl.value << endl;
 * ...
 * delete de_dvush.value;
 * @endcode
 * It is also possible to do the extraction in a generic way
 * @code
 * DevicePipeBlob dpb = .....
 *
 * size_t nb_de = dpb.get_data_elt_nb();
 * for (size_t loop = 0;loop < nb;loop++)
 * {
 *     int data_type = dpb.get_data_elt_type(loop);
 *     string de_name = dpb.get_data_elt_name(loop);
 *     switch(data_type)
 *     {
 *        case DEV_LONG:
 *        {
 *            DevLong lg;
 *            dpb >> lg;
 *        }
 *        break;
 *
 *        case DEVVAR_DOUBLEARRAY:
 *        {
 *            vector<double> v_db;
 *            dpb >> v_db;
 *        }
 *        break;
 *        ....
 *     }
 * ...
 * }
 * @endcode
 * Note that instead of using DevLong and vector<double> data, the extraction can be done using DataElement<T>
 * instances. In this case, the call to the get_data_elt_name() method becomes useless.
 *
 * @param [out] datum The blob data
 * @exception WrongData if requested
 */
	DevicePipeBlob & operator >> (short &datum);
/**
 * Get blob data element number
 *
 * Get the blob data element number
 *
 * @return The blob data element number
 */
	size_t get_data_elt_nb();
/**
 * Get blob data elements name
 *
 * Get the blob data elements name
 *
 * @return The blob data elements name
 */
	vector<string> get_data_elt_names();
/**
 * Get blob data element name
 *
 * Get the blob data element name for a single data element
 *
 * @param [in] ind The data element index within the blob
 * @return The blob data element name
 */
	string get_data_elt_name(size_t ind);
/**
 * Get blob data element value type
 *
 * Get the blob data element value type for a single data element
 *
 * @param [in] ind The data element index within the blob
 * @return The blob data element value type
 */
	int get_data_elt_type(size_t ind);
//@}

/**@name Exception and error related methods methods
 */
//@{
/**
 * Set exception flag
 *
 * It's a method which allows the user to switch on/off exception throwing when trying to extract data from a
 * DevicePipeBlob object. The following flags are supported :
 * @li @b isempty_flag - throw a WrongData exception (reason = API_EmptyDataElement) if user
 *       tries to extract data from one empty blob data element. By default, this flag
 *       is set
 * @li @b wrongtype_flag - throw a WrongData exception (reason = API_IncompatibleArgumentType) if user
 *       tries to extract data with a type different than the type used for insertion. By default, this flag
 *       is set
 * @li @b notenoughde_flag - throw a WrongData exception (reason = API_PipeWrongArg) if user
 *       tries to extract data from a DevicePipeBlob for a data element which does not exist. By default, this flag
 *       is set
 * @li @b blobdenamenotset_flag - Throw a WrongData exception (reason = API_PipeNoDataElement) if user tries to
 *       insert data into the blob while the name or number of data element has not been set with methods
 *       set_data_elt_nb() or set_data_elt_names()
 * @li @b mixing_flag - Throw a WrongData exception (reason = API_NotSupportedFeature) if user tries to mix
 *		  insertion/extraction method (<< or >>) with operator[]
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
 * DevicePipeBlob dpb;
 *
 * bitset<DevicePipeBlob::numFlags> bs = dpb.exceptions();
 * cout << "bs = " << bs << endl;
 *
 * dpb.set_exceptions(DevicePipeBlob::wrongtype_flag);
 * bs = dpb.exceptions();
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
 * Sets one exception flag. See DevicePipeBlob::exceptions() for a usage example.
 *
 * @param [in] fl The exception flag
 */
	void set_exceptions(except_flags fl) {exceptions_flags.set((size_t)fl);}
/**
 * Check insertion/extraction success
 *
 * Allow the user to check if insertion/extraction into/from DevicePipeBlob instance was successfull. This
 * method has to be used when exceptions are disabled.
 *
 * @return True if insertion/extraction has failed
 */
	bool has_failed();
/**
 * Get instance insertion/extraction state
 *
 * Allow the user to find out what was the reason of insertion/extraction into/from DevicePipeBlob failure. This
 * method has to be used when exceptions are disabled.
 * Here is an example of how methods has_failed() and state() could be used
 * @code
 * DevicePipeBlob dpb = ....
 *
 * bitset<DevicePipeBlob::numFlags> bs;
 * bs.reset();
 * dpb.exceptions(bs);
 *
 * DevLong dl;
 * dpb >> dl;
 *
 * if (dpb.has_failed() == true)
 * {
 *    bitset<DevicePipeBlob::numFlags> bs_err = dpb.state();
 *    if (bs_err.test(DevicePipeBlob::isempty_flag) == true)
 *        .....
 * }
 * @endcode
 *
 * @return The error bit set.
 */
	bitset<numFlags> state() {return ext_state;}
//@}

///@privatesection
	~DevicePipeBlob();
	DevicePipeBlob(const DevicePipeBlob &);
	DevicePipeBlob & operator=(const DevicePipeBlob &);
#ifdef HAS_RVALUE
	DevicePipeBlob(DevicePipeBlob &&);
	DevicePipeBlob & operator=(DevicePipeBlob &&);
#endif

	DevicePipeBlob & operator << (DevBoolean &);
//	DevicePipeBlob & operator << (short &);
	DevicePipeBlob & operator << (DevLong &);
	DevicePipeBlob & operator << (DevLong64 &);
	DevicePipeBlob & operator << (float &);
	DevicePipeBlob & operator << (double &);
	DevicePipeBlob & operator << (DevUChar &);
	DevicePipeBlob & operator << (DevUShort &);
	DevicePipeBlob & operator << (DevULong &);
	DevicePipeBlob & operator << (DevULong64 &);
	DevicePipeBlob & operator << (DevString &);
	DevicePipeBlob & operator << (DevState &);
	DevicePipeBlob & operator << (DevEncoded &);
	DevicePipeBlob & operator << (const string &);

	DevicePipeBlob & operator << (DevicePipeBlob &);

	DevicePipeBlob & operator << (vector<DevBoolean> &);
	DevicePipeBlob & operator << (vector<short> &);
	DevicePipeBlob & operator << (vector<DevLong> &);
	DevicePipeBlob & operator << (vector<DevLong64> &);
	DevicePipeBlob & operator << (vector<float> &);
	DevicePipeBlob & operator << (vector<double> &);
	DevicePipeBlob & operator << (vector<DevUChar> &);
	DevicePipeBlob & operator << (vector<DevUShort> &);
	DevicePipeBlob & operator << (vector<DevULong> &);
	DevicePipeBlob & operator << (vector<DevULong64> &);
	DevicePipeBlob & operator << (vector<DevString> &);
	DevicePipeBlob & operator << (vector<DevState> &);
	DevicePipeBlob & operator << (vector<DevEncoded> &);
	DevicePipeBlob & operator << (vector<string> &);

	DevicePipeBlob & operator << (DevVarBooleanArray &);
	DevicePipeBlob & operator << (DevVarShortArray &);
	DevicePipeBlob & operator << (DevVarLongArray &);
	DevicePipeBlob & operator << (DevVarLong64Array &);
	DevicePipeBlob & operator << (DevVarFloatArray &);
	DevicePipeBlob & operator << (DevVarDoubleArray &);
	DevicePipeBlob & operator << (DevVarUCharArray &);
	DevicePipeBlob & operator << (DevVarUShortArray &);
	DevicePipeBlob & operator << (DevVarULongArray &);
	DevicePipeBlob & operator << (DevVarULong64Array &);
	DevicePipeBlob & operator << (DevVarStringArray &);
	DevicePipeBlob & operator << (DevVarStateArray &);
	DevicePipeBlob & operator << (DevVarEncodedArray &);

	DevicePipeBlob & operator << (DevVarBooleanArray *);
	DevicePipeBlob & operator << (DevVarShortArray *);
	DevicePipeBlob & operator << (DevVarLongArray *);
	DevicePipeBlob & operator << (DevVarLong64Array *);
	DevicePipeBlob & operator << (DevVarFloatArray *);
	DevicePipeBlob & operator << (DevVarDoubleArray *);
	DevicePipeBlob & operator << (DevVarUCharArray *);
	DevicePipeBlob & operator << (DevVarUShortArray *);
	DevicePipeBlob & operator << (DevVarULongArray *);
	DevicePipeBlob & operator << (DevVarULong64Array *);
	DevicePipeBlob & operator << (DevVarStringArray *);
	DevicePipeBlob & operator << (DevVarStateArray *);
	DevicePipeBlob & operator << (DevVarEncodedArray *);

//-------------------------------------------------------------------------------------------------

	DevicePipeBlob & operator >> (DevBoolean &);
//	DevicePipeBlob & operator >> (short &);
	DevicePipeBlob & operator >> (DevLong &);
	DevicePipeBlob & operator >> (DevLong64 &);
	DevicePipeBlob & operator >> (float &);
	DevicePipeBlob & operator >> (double &);
	DevicePipeBlob & operator >> (DevUChar &);
	DevicePipeBlob & operator >> (DevUShort &);
	DevicePipeBlob & operator >> (DevULong &);
	DevicePipeBlob & operator >> (DevULong64 &);
	DevicePipeBlob & operator >> (DevString &);
	DevicePipeBlob & operator >> (DevState &);
	DevicePipeBlob & operator >> (DevEncoded &);
	DevicePipeBlob & operator >> (string &);

	DevicePipeBlob & operator >> (DevicePipeBlob &);

	DevicePipeBlob & operator >> (vector<DevBoolean> &);
	DevicePipeBlob & operator >> (vector<short> &);
	DevicePipeBlob & operator >> (vector<DevLong> &);
	DevicePipeBlob & operator >> (vector<DevLong64> &);
	DevicePipeBlob & operator >> (vector<float> &);
	DevicePipeBlob & operator >> (vector<double> &);
	DevicePipeBlob & operator >> (vector<DevUChar> &);
	DevicePipeBlob & operator >> (vector<DevUShort> &);
	DevicePipeBlob & operator >> (vector<DevULong> &);
	DevicePipeBlob & operator >> (vector<DevULong64> &);
	DevicePipeBlob & operator >> (vector<string> &);
	DevicePipeBlob & operator >> (vector<DevState> &);
//	DevicePipeBlob & operator >> (vector<DevEncoded> &);

	DevicePipeBlob & operator >> (DevVarBooleanArray *);
	DevicePipeBlob & operator >> (DevVarShortArray *);
	DevicePipeBlob & operator >> (DevVarLongArray *);
	DevicePipeBlob & operator >> (DevVarLong64Array *);
	DevicePipeBlob & operator >> (DevVarFloatArray *);
	DevicePipeBlob & operator >> (DevVarDoubleArray *);
	DevicePipeBlob & operator >> (DevVarUCharArray *);
	DevicePipeBlob & operator >> (DevVarUShortArray *);
	DevicePipeBlob & operator >> (DevVarULongArray *);
	DevicePipeBlob & operator >> (DevVarULong64Array *);
	DevicePipeBlob & operator >> (DevVarStringArray *);
	DevicePipeBlob & operator >> (DevVarStateArray *);
	DevicePipeBlob & operator >> (DevVarEncodedArray *);

	DevicePipeBlob &operator[](const string &);

	const char *get_current_delt_name() {return (*extract_elt_array)[extract_ctr].name.in();}
	void set_current_delt_name(const string &);

	size_t get_extract_ind_from_name(const string &);
	size_t get_insert_ind_from_name(const string &);

	void reset_insert_ctr() {insert_ctr=0;}
	DevVarPipeDataEltArray *get_insert_data() {return insert_elt_array;}
	const DevVarPipeDataEltArray *get_extract_data() {return extract_elt_array;}

	void set_extract_data(const DevVarPipeDataEltArray *_ptr) {extract_elt_array=_ptr;}
	void reset_insert_data_ptr() {insert_elt_array=Tango_nullptr;}

	void reset_extract_ctr() {extract_ctr=0;}
	void set_extract_delete(bool _b) {extract_delete=_b;}

	void print(ostream &,int,bool);

protected:
///@privatesection
	void throw_type_except(const string &,const string &);
	void throw_too_many(const string &,bool);
	void throw_is_empty(const string &);
	void throw_name_not_set(const string &);
	void throw_mixing(const string &);

private:
	string							name;					// The blob name
	bitset<numFlags> 				exceptions_flags;		// Exception flag
	bitset<numFlags> 				ext_state;				// Extraction state
	bool							failed;					// Failed flag

	DevVarPipeDataEltArray			*insert_elt_array;		// Ptr for data to be inserted (client write/Server read)
	int								insert_ctr;				// Ctr for inserting data elt
	int								insert_ind;

	const DevVarPipeDataEltArray	*extract_elt_array;		// Ptr for data to be extracted (client read/Server write)
	int								extract_ctr;			// Ctr for extracting data elt
	bool							extract_delete;			// Flag to force extract ptr delete
	int								extract_ind;

    class DevicePipeBlobExt
    {
    public:
        DevicePipeBlobExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DevicePipeBlobExt>   	ext;
#else
	DevicePipeBlobExt		        	*ext;				// Class extension
#endif
};


/****************************************************************************************
 * 																						*
 * 					The DevicePipe class					   						    *
 * 					--------------------											    *
 * 																						*
 ***************************************************************************************/


/**
 * Fundamental type for sending/receiving data from device pipes
 *
 * This is the fundamental type for sending/receiving data to/from device pipe.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DevicePipe
{

public :

///@publicsection
/**@name Constructors */
//@{
/**
 * Create a DevicePipe object.
 *
 * Default constructor. The instance is empty
 *
 */
    DevicePipe();
/**
 * Create a DevicePipe object with name
 *
 * Create one instance of the DevicePipe class and set its name
 *
 * @param [in] pipe_name The pipe name
 */
    DevicePipe(const string &pipe_name);
/**
 * Create a DevicePipe object with name and root blob name.
 *
 * Create one instance of the DevicePipe class and set its name and its root blob name
 *
 * @param [in] pipe_name The pipe name
 * @param [in] root_blob_name The root blob name
 */
    DevicePipe(const string &pipe_name,const string &root_blob_name);
//@}

/**@name Get/Set methods */
//@{
/**
 * Set pipe name
 *
 * Set the device pipe name
 *
 * @param [in] pipe_name The pipe name
 */
	void set_name(const string &pipe_name) {name=pipe_name;}
/**
 * Get pipe name
 *
 * Set the device pipe name
 *
 * @return The pipe name
 */
	const string &get_name() {return name;}

/**
 * Set root blob name
 *
 * Set the root blob name
 *
 * @param [in] root_blob_name The root blob name
 */
	void set_root_blob_name(const string &root_blob_name) {the_root_blob.set_name(root_blob_name);}
/**
 * Get root blob name
 *
 * Get the root blob name
 *
 * @return The root blob name
 */
	const string &get_root_blob_name() {return the_root_blob.get_name();}
//@}

/**@name Inserting data into a DevicePipe
 */
//@{
#ifdef GEN_DOC
/**
 * Insert data into a device pipe
 *
 * Inserting data into a DevicePipe instance is simlar to inserting data into a DevicePipeBlob class instance.
 * See doc of DevicePipeBlob class insertion methods (DevicePipeBlob::operator<<) to get a complete documentation on
 * how to insert data into a DevicePipe
 *
 * @param [in] datum The data to be inserted into the DevicePipe
 * @exception WrongData if requested
 */
	DevicePipe & operator << (short &datum);
#endif
/**
 * Set blob data element number
 *
 * Set the blob data element number
 *
 * @param [in] nb The blob data element number
 */
	void set_data_elt_nb(size_t nb) {the_root_blob.set_data_elt_nb(nb);}
/**
 * Set blob data element number and names
 *
 * Set the blob data element number and names. The data element number is the number of names in the input
 * parameter.
 *
 * @param [in] names The blob data element names
 */
	void set_data_elt_names(vector<string> &names) {the_root_blob.set_data_elt_names(names);}
//@}

/**@name Extracting data from a DevicePipe
 */
//@{
#ifdef GEN_DOC
/**
 * Extract data from a device pipe
 *
 * Extracting data from a DevicePipe instance is simlar to extracting data from a DevicePipeBlob class instance.
 * See doc of DevicePipeBlob class extraction methods (DevicePipeBlob::operator>>) to get a complete documentation on
 * how to extract data from a DevicePipe
 *
 * @param [in] datum The pipe data
 * @exception WrongData if requested
 */
	DevicePipe & operator >> (short &datum);
#endif
/**
 * Get root blob data element number
 *
 * Get the root blob data element number
 *
 * @return The root blob data element number
 */
	size_t get_data_elt_nb() {return the_root_blob.get_data_elt_nb();}
/**
 * Get root blob data elements name
 *
 * Get the root blob data elements name
 *
 * @return The root blob data elements name
 */
	vector<string> get_data_elt_names() {return the_root_blob.get_data_elt_names();}
/**
 * Get root blob data element name
 *
 * Get root blob data element name for a single data element
 *
 * @param [in] ind The data element index within the root blob
 * @return The root blob data element name
 */
	string get_data_elt_name(size_t ind) {return the_root_blob.get_data_elt_name(ind);}
/**
 * Get root blob data element value type
 *
 * Get root blob data element value type for a single data element
 *
 * @param [in] ind The data element index within the root blob
 * @return The root blob data element value type
 */
	int get_data_elt_type(size_t ind) {return the_root_blob.get_data_elt_type(ind);}
//@}


/**@name Exception and error related methods methods
 */
//@{
/**
 * Set exception flag
 *
 * It's a method which allows the user to switch on/off exception throwing when trying to insert/extract data from a
 * DevicePipe object. The following flags are supported :
 * @li @b isempty_flag - throw a WrongData exception (reason = API_EmptyDataElement) if user
 *       tries to extract data from one empty pipe data element. By default, this flag
 *       is set
 * @li @b wrongtype_flag - throw a WrongData exception (reason = API_IncompatibleArgumentType) if user
 *       tries to extract data with a type different than the type used for insertion. By default, this flag
 *       is set
 * @li @b notenoughde_flag - throw a WrongData exception (reason = API_PipeWrongArg) if user
 *       tries to extract data from a DevicePipe for a data element which does not exist. By default, this flag
 *       is set
 * @li @b blobdenamenotset_flag - Throw a WrongData exception (reason = API_PipeNoDataElement) if user tries to
 *       insert data into the blob while the name or number of data element has not been set with methods
 *       set_data_elt_nb() or set_data_elt_names()
 * @li @b mixing_flag - Throw a WrongData exception (reason = API_NotSupportedFeature) if user tries to mix
 *		  insertion/extraction method (<< or >>) with operator[]
 *
 * @param [in] fl The exception flag
 */
	void exceptions(bitset<DevicePipeBlob::numFlags> fl) {the_root_blob.exceptions(fl);}
/**
 * Get exception flag
 *
 * Returns the whole exception flags.
 * The following is an example of how to use these exceptions related methods
 * @code
 * DevicePipe dp;
 *
 * bitset<DevicePipeBlob::numFlags> bs = dp.exceptions();
 * cout << "bs = " << bs << endl;
 *
 * dp.set_exceptions(DevicePipeBlob::wrongtype_flag);
 * bs = dp.exceptions();
 *
 * cout << "bs = " << bs << endl;
 * @endcode
 *
 * @return The exception flag
 */
	bitset<DevicePipeBlob::numFlags> exceptions() {return the_root_blob.exceptions();}
/**
 * Reset one exception flag
 *
 * Resets one exception flag
 *
 * @param [in] fl The exception flag
 */
	void reset_exceptions(DevicePipeBlob::except_flags fl) {the_root_blob.reset_exceptions(fl);}
/**
 * Set one exception flag
 *
 * Sets one exception flag. See DevicePipe::exceptions() for a usage example.
 *
 * @param [in] fl The exception flag
 */
	void set_exceptions(DevicePipeBlob::except_flags fl) {the_root_blob.set_exceptions(fl);}
/**
 * Check insertion/extraction success
 *
 * Allow the user to check if insertion/extraction into/from DevicePipe instance was successfull. This
 * method has to be used when exceptions are disabled.
 *
 * @return True if insertion/extraction has failed
 */
	bool has_failed() {return the_root_blob.has_failed();}
/**
 * Get instance insertion/extraction state
 *
 * Allow the user to find out what was the reason of insertion/extraction into/from DevicePipe failure. This
 * method has to be used when exceptions are disabled.
 * Here is an example of how methods has_failed() and state() could be used
 * @code
 * DevicePipe dpb = ....
 *
 * bitset<DevicePipeBlob::numFlags> bs;
 * bs.reset();
 * dpb.exceptions(bs);
 *
 * DevLong dl;
 * dpb >> dl;
 *
 * if (dpb.has_failed() == true)
 * {
 *    bitset<DevicePipeBlob::numFlags> bs_err = dpb.state();
 *    if (dpb.test(DevicePipeBlob::isempty_flag) == true)
 *        .....
 * }
 * @endcode
 *
 * @return The error bit set.
 */
	bitset<DevicePipeBlob::numFlags> state() {return the_root_blob.state();}
//@}

/**
 * Print a DevicePipe instance
 *
 * Is an utility function to easily print the contents of a DevicePipe object. This function knows all types
 * which could be inserted in a DevicePipe object and print them accordingly. A special string is printed if
 * the DevicePipe object is empty
 * @code
 * DeviceProxy *dev = new DeviceProxy(“...”);
 * DevicePipe out;
 *
 * out = dev->read_pipe(“MyPipe”);
 * cout << “Pipe content: ” << out << endl;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] dd The instance to be printed
 */
	friend ostream &operator<<(ostream &str,DevicePipe &dd);

public :
///@privatesection
	DevicePipe(const DevicePipe &);
	DevicePipe & operator=(const DevicePipe &);
#ifdef HAS_RVALUE
	DevicePipe(DevicePipe &&);
	DevicePipe & operator=(DevicePipe &&);
#endif
	~DevicePipe();

	void set_time(TimeVal &_ti) {time=_ti;}
	DevicePipeBlob &get_root_blob() {return the_root_blob;}

	DevicePipe &operator[](const string &);

private:
	DevicePipeBlob			the_root_blob;			// Root blob
	string 					name;					// Pipe name
	TimeVal 				time;					// When pipe has been read

    class DevicePipeExt
    {
    public:
        DevicePipeExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DevicePipeExt>   ext;
#else
	DevicePipeExt		        *ext;				// Class extension
#endif
};

/****************************************************************************************
 * 																						*
 * 			Some DevicePipe, DevicePipeBlob and DataElement helper functions			*
 * 			----------------------------------------------------------------			*
 * 																						*
 ***************************************************************************************/

DevicePipe &operator>>(DevicePipe &_dp,char *&datum);

//
// For DataElement printing
//

template <typename T>
ostream &operator<<(ostream &,DataElement<T> &);

template <typename T>
ostream &operator<<(ostream &,DataElement<vector<T> > &);

template <typename T>
ostream &operator<<(ostream &,DataElement<T *> &);

//
// For DevicePipe insertion
//

template <typename T>
DevicePipe &operator<<(DevicePipe &,T &);

template <typename T>
DevicePipe &operator<<(DevicePipe &,T *);

template <typename T>
DevicePipe &operator<<(DevicePipe &, DataElement<T> &);

//
// For DevicePipe extraction
//

template <typename T>
DevicePipe &operator>>(DevicePipe &,T &);

template <typename T>
DevicePipe &operator>>(DevicePipe &,T *);

template <typename T>
DevicePipe &operator>>(DevicePipe &, DataElement<T> &);

//
// For DevicePipeBlob insertion
//

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &,T &);

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &,T *);

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &,DataElement<T> &);

//
// For DevicePipeBlob extraction
//

template <typename T>
DevicePipeBlob &operator>>(DevicePipeBlob &,T &);

template <typename T>
DevicePipeBlob &operator>>(DevicePipeBlob &,T *);

template <typename T>
DevicePipeBlob &operator>>(DevicePipeBlob &, DataElement<T> &);



/****************************************************************************************
 * 																						*
 * 			Some macros (shame on me, but I am too lazy)								*
 * 			-------------------------------------------									*
 * 																						*
 ***************************************************************************************/

//
// A is the required value for the IDL enum descriminator
// B is the IDL enum method to get data
// C is data type name
//

#define EXTRACT_BASIC_TYPE(A,B,C) \
	failed = false; \
	ext_state.reset(); \
\
	if (extract_ctr > (int)extract_elt_array->length() - 1) \
		ext_state.set(notenoughde_flag); \
	else if (extract_ctr == -1 && extract_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		int ind; \
		if (extract_ind != -1) \
			ind = extract_ind; \
		else \
			ind = extract_ctr; \
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[ind].value); \
		AttributeDataType adt = uni_ptr->_d(); \
		if (adt != A) \
		{ \
			if (adt == ATT_NO_DATA) \
			{ \
				if ((*extract_elt_array)[ind].inner_blob.length() == 0) \
					ext_state.set(isempty_flag); \
				else \
					ext_state.set(wrongtype_flag); \
			} \
			else \
				ext_state.set(wrongtype_flag); \
		} \
		else \
		{ \
			datum = (uni_ptr->B())[0]; \
			if (extract_ind != -1) \
				extract_ind = -1; \
			else \
				extract_ctr++; \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(isempty_flag) == true && exceptions_flags.test(isempty_flag) == true) \
		throw_is_empty("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator>>",true); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true) \
		throw_type_except(C,"operator>>");


//
// A is the required value for the IDL enum descriminator
// B is the IDL enum method to get data
// C is the CORBA sequence type name
// D is data type name
//

#define EXTRACT_VECTOR_TYPE(A,B,C,D) \
	failed = false; \
	ext_state.reset(); \
\
	if (extract_ctr > (int)extract_elt_array->length() - 1) \
		ext_state.set(notenoughde_flag); \
	else if (extract_ctr == -1 && extract_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		int ind; \
		if (extract_ind != -1) \
			ind = extract_ind; \
		else \
			ind = extract_ctr; \
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[ind].value); \
		AttributeDataType adt = uni_ptr->_d(); \
		if (adt != A) \
		{ \
			if (adt == ATT_NO_DATA) \
			{ \
				if ((*extract_elt_array)[ind].inner_blob.length() == 0) \
					ext_state.set(isempty_flag); \
				else \
					ext_state.set(wrongtype_flag); \
			} \
			else \
				ext_state.set(wrongtype_flag); \
		} \
		else \
		{ \
			const C &dvsa = uni_ptr->B(); \
			datum << dvsa; \
			if (extract_ind != -1) \
				extract_ind = -1; \
			else \
				extract_ctr++; \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(isempty_flag) == true && exceptions_flags.test(isempty_flag) == true) \
		throw_is_empty("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator>>",true); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true) \
		throw_type_except(D,"operator>>");

//
// A is the required value for the IDL enum descriminator
// B is the IDL enum method to get data
// C is the CORBA sequence type name
// D is data type name
//

#define EXTRACT_SEQ_PTR_TYPE(A,B,C,D) \
	failed = false; \
	ext_state.reset(); \
\
	if (extract_ctr > (int)extract_elt_array->length() - 1) \
		ext_state.set(notenoughde_flag); \
	else if (extract_ctr == -1 && extract_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		int ind; \
		if (extract_ind != -1) \
			ind = extract_ind; \
		else \
			ind = extract_ctr; \
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[ind].value); \
		AttributeDataType adt = uni_ptr->_d(); \
		if (adt != A) \
		{ \
			if (adt == ATT_NO_DATA) \
			{ \
				if ((*extract_elt_array)[ind].inner_blob.length() == 0) \
					ext_state.set(isempty_flag); \
				else \
					ext_state.set(wrongtype_flag); \
			} \
			else \
				ext_state.set(wrongtype_flag); \
		} \
		else \
		{ \
			C &dvsa = const_cast<C &>(uni_ptr->B()); \
			CORBA::Long max,len; \
			max = dvsa.maximum(); \
			len = dvsa.length(); \
			datum->replace(max,len,dvsa.get_buffer((CORBA::Boolean)true),true); \
			if (extract_ind != -1) \
				extract_ind = -1; \
			else \
				extract_ctr++; \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(isempty_flag) == true && exceptions_flags.test(isempty_flag) == true) \
		throw_is_empty("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator>>",true); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true) \
		throw_type_except(D,"operator>>");


//
// A is the sequence CORBA name
// B is the IDL enum method to set data
//

#define INSERT_BASIC_TYPE(A,B) \
	failed = false; \
	ext_state.reset(); \
\
	if (insert_elt_array == Tango_nullptr) \
		ext_state.set(blobdenamenotset_flag); \
	else if (insert_ctr == -1 && insert_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		size_t nb_insert = insert_elt_array->length(); \
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1) \
			ext_state.set(notenoughde_flag); \
		else \
		{ \
			A dvsa; \
			dvsa.length(1); \
			dvsa[0] = datum; \
\
			if (insert_ind != -1) \
			{ \
				(*insert_elt_array)[insert_ind].value.B(dvsa); \
                (*insert_elt_array)[insert_ind].inner_blob_name = CORBA::string_dup(SCALAR_PIPE); \
				insert_ind = -1; \
			} \
			else \
			{ \
				(*insert_elt_array)[insert_ctr].value.B(dvsa); \
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(SCALAR_PIPE); \
				insert_ctr++; \
			} \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true) \
		throw_name_not_set("operator<<"); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator<<",false);


//
// A is the sequence CORBA name
// B is the IDL enum method to set data
//

#define INSERT_VECTOR_TYPE(A,B) \
	failed = false; \
	ext_state.reset(); \
\
	if (insert_elt_array == Tango_nullptr) \
		ext_state.set(blobdenamenotset_flag); \
	else if (insert_ctr == -1 && insert_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		size_t nb_insert = insert_elt_array->length(); \
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1) \
			ext_state.set(notenoughde_flag); \
		else \
		{ \
			A dvsa; \
			if (insert_ind != -1) \
			{ \
				(*insert_elt_array)[insert_ind].value.B(dvsa); \
				A &dvsb = (*insert_elt_array)[insert_ind].value.B(); \
				dvsb.replace(datum.size(),datum.size(),&datum[0],false); \
                (*insert_elt_array)[insert_ind].inner_blob_name = CORBA::string_dup(ARRAY_PIPE); \
				insert_ind = -1; \
			} \
			else \
			{ \
				(*insert_elt_array)[insert_ctr].value.B(dvsa); \
				A &dvsb = (*insert_elt_array)[insert_ctr].value.B(); \
				dvsb.replace(datum.size(),datum.size(),&datum[0],false); \
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(ARRAY_PIPE); \
				insert_ctr++; \
			} \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true) \
		throw_name_not_set("operator<<"); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator<<",false);



//
// A is the sequence CORBA name
// B is the IDL enum method to set data
//

#define INSERT_SEQ_TYPE(A,B) \
	failed = false; \
	ext_state.reset(); \
\
	if (insert_elt_array == Tango_nullptr) \
		ext_state.set(blobdenamenotset_flag); \
	else if (insert_ctr == -1 && insert_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		size_t nb_insert = insert_elt_array->length(); \
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1) \
			ext_state.set(notenoughde_flag); \
		else \
		{ \
			CORBA::Long max,len; \
			max = datum.maximum(); \
			len = datum.length(); \
			A dvsa; \
			if (insert_ind != -1) \
			{ \
				(*insert_elt_array)[insert_ind].value.B(dvsa); \
				A &dvsb = (*insert_elt_array)[insert_ind].value.B(); \
				dvsb.replace(max,len,datum.get_buffer(),false); \
                (*insert_elt_array)[insert_ind].inner_blob_name = CORBA::string_dup(ARRAY_PIPE); \
				insert_ind = -1; \
			} \
			else \
			{ \
				(*insert_elt_array)[insert_ctr].value.B(dvsa); \
				A &dvsb = (*insert_elt_array)[insert_ctr].value.B(); \
				dvsb.replace(max,len,datum.get_buffer(),false); \
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(ARRAY_PIPE); \
				insert_ctr++; \
			} \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true) \
		throw_name_not_set("operator<<"); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator<<",false);



//
// A is the sequence CORBA name
// B is the IDL enum method to set data
//

#define INSERT_SEQ_PTR_TYPE(A,B) \
	failed = false; \
	ext_state.reset(); \
\
	if (insert_elt_array == Tango_nullptr) \
		ext_state.set(blobdenamenotset_flag); \
	else if (insert_ctr == -1 && insert_ind == -1) \
		ext_state.set(mixing_flag); \
	else \
	{ \
		size_t nb_insert = insert_elt_array->length(); \
		if (nb_insert == 0 || insert_ctr > (int)nb_insert - 1) \
			ext_state.set(notenoughde_flag); \
		else \
		{ \
			A dvsa; \
			CORBA::Long max,len; \
			max = datum->maximum(); \
			len = datum->length(); \
			bool rel = datum->release(); \
			if (rel == false) \
			{ \
				datum->replace(max,len,datum->get_buffer(),true); \
			} \
			if (insert_ind != -1) \
			{ \
				(*insert_elt_array)[insert_ind].value.B(dvsa); \
				A &dvsb = (*insert_elt_array)[insert_ind].value.B(); \
				dvsb.replace(max,len,datum->get_buffer((CORBA::Boolean)true),true); \
                (*insert_elt_array)[insert_ind].inner_blob_name = CORBA::string_dup(ARRAY_PIPE); \
				insert_ind = -1; \
			} \
			else \
			{\
				(*insert_elt_array)[insert_ctr].value.B(dvsa); \
				A &dvsb = (*insert_elt_array)[insert_ctr].value.B(); \
				dvsb.replace(max,len,datum->get_buffer((CORBA::Boolean)true),true); \
                (*insert_elt_array)[insert_ctr].inner_blob_name = CORBA::string_dup(ARRAY_PIPE); \
				insert_ctr++; \
			} \
	\
			delete datum; \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(blobdenamenotset_flag) == true && exceptions_flags.test(blobdenamenotset_flag) == true) \
		throw_name_not_set("operator<<"); \
\
	if (ext_state.test(mixing_flag) == true && exceptions_flags.test(mixing_flag) == true) \
		throw_mixing("operator>>"); \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator<<",false);


#endif /* _DEVICEPIPE_H */

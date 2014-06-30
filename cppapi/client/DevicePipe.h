//===================================================================================================================
//
// DevicePipe.h - include file for TANGO device api class DevicePipe
//
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
DataElement<T>::DataElement()
{
}

/**
 * Fundamental type for inserting data into a device pipe blob
 *
 * This is the fundamental type for inserting data into a device pipe blob
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

template <typename T>
struct WDataElement: public DataElement<T>
{
/**
 * Create a WDataElement object.
 *
 * Create a WDataElement object for device pipe blob insertion
 *
 * @param [in] name The data element name
 * @param [in] value The data element value
 * @param [in] size The data element size (when array is used). Default value is 1
 * @param [in] rel The data element release flag. If true, Tango will free memory used by data elemt value after using
 * it. Dafault value is false
 */
	WDataElement(const string &name,T value,size_t size=1,bool rel=false);
/**
 * Create a WDataElement object.
 *
 * Create a WDataElement object for device pipe blob insertion
 *
 * @param [in] name The data element name
 */
	WDataElement(const string &name);

	size_t		size;		///< Data element size
	bool		release;	///< Data element release flag
};

template <typename T>
WDataElement<T>::WDataElement(const string &_na,T _val,size_t _s,bool _r):DataElement<T>(_na,_val),size(_s),release(_r)
{
}

template <typename T>
WDataElement<T>::WDataElement(const string &_na):DataElement<T>(_na),size(1),release(false)
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
/**
 * Get blob data element number
 *
 * Get the blob data element number
 *
 * @return The blob data element number
 */
	size_t get_data_elt_nb() {return extract_elt_array->length();}
/**
 * Get blob data elements name
 *
 * Get the blob data elements name
 *
 * @return The blob data elements name
 */
	vector<string> get_data_elt_name();
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
	int get_data_elt_type(size_t _ind);

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
//@}


	~DevicePipeBlob();
	DevicePipeBlob(const DevicePipeBlob &);
	DevicePipeBlob & operator=(const DevicePipeBlob &);
#ifdef HAS_RVALUE
	DevicePipeBlob(DevicePipeBlob &&);
	DevicePipeBlob & operator=(DevicePipeBlob &&);
#endif

	void set_data_elt_nb(size_t _nb);
	void set_data_elt_names(vector<string> &);




	DevicePipeBlob & operator << (short &);
	DevicePipeBlob & operator << (DevLong &);
	DevicePipeBlob & operator << (double &);
	DevicePipeBlob & operator << (DevicePipeBlob &);

	DevicePipeBlob & operator >> (short &);
	DevicePipeBlob & operator >> (vector<short> &);
	DevicePipeBlob & operator >> (DevLong &);
	DevicePipeBlob & operator >> (double &);
	DevicePipeBlob & operator >> (DevicePipeBlob &);

	bool has_failed() {return failed;}
	bitset<numFlags> state() {return ext_state;}

	DevicePipeBlob &operator[](const string &);

	const char *get_current_delt_name() {return (*extract_elt_array)[extract_ctr].name.in();}
	void set_current_delt_name(const string &_na) {(*insert_elt_array)[insert_ctr].name = CORBA::string_dup(_na.c_str());}

	size_t get_extract_ind_from_name(const string &);

	void reset_insert_ctr() {insert_ctr=0;}
	DevVarPipeDataEltArray *get_insert_data() {return insert_elt_array;}
	const DevVarPipeDataEltArray *get_extract_data() {return extract_elt_array;}

	void set_extract_data(const DevVarPipeDataEltArray *_ptr) {extract_elt_array=_ptr;}
	void reset_extract_ctr() {extract_ctr=0;}
	void set_extract_delete(bool _b) {extract_delete=_b;}

	void print(ostream &,int,bool);

protected:
	void throw_type_except(const string &,const string &);
	void throw_too_many(const string &,bool);
	void throw_is_empty(const string &);

private:
	string							name;					// The blob name
	bitset<numFlags> 				exceptions_flags;		// Exception flag
	bitset<numFlags> 				ext_state;				// Extraction state
	bool							failed;					// Failed flag

	DevVarPipeDataEltArray			*insert_elt_array;		// Ptr for data to be inserted (client write/Server read)
	size_t							insert_ctr;				// Ctr for inserting data elt

	const DevVarPipeDataEltArray	*extract_elt_array;		// Ptr for data to be extracted (client read/Server write)
	size_t							extract_ctr;			// Ctr for extracting data elt
	bool							extract_delete;			// Flag to force extract ptr delete

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

	vector<string> get_data_elt_name() {return the_root_blob.get_data_elt_name();}
	size_t get_data_elt_nb() {return the_root_blob.get_data_elt_nb();}

	string get_data_elt_name(size_t _ind) {return the_root_blob.get_data_elt_name(_ind);}
	int get_data_elt_type(size_t _ind) {return the_root_blob.get_data_elt_type(_ind);}

	const string &get_root_blob_name() {return the_root_blob.get_name();}

	DevicePipe &operator[](const string &);
	bool has_failed() {return the_root_blob.has_failed();}

	void set_data_elt_nb(size_t _nb) {the_root_blob.set_data_elt_nb(_nb);}







	void set_root_blob_data_elt_names(vector<string> &v_n) {the_root_blob.set_data_elt_names(v_n);}

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
DevicePipe &operator<<(DevicePipe &, WDataElement<T> &);

//
// For DevicePipe extraction
//

template <typename T>
DevicePipe &operator>>(DevicePipe &,T &);

template <typename T>
DevicePipe &operator>>(DevicePipe &, DataElement<T> &);

//
// For DevicePipeBlob insertion
//

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &,WDataElement<T> &);

//
// For DevicePipeBlob extraction
//

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
	if (extract_ctr > extract_elt_array->length() - 1) \
		ext_state.set(notenoughde_flag); \
	else \
	{ \
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[extract_ctr].value); \
		AttributeDataType adt = uni_ptr->_d(); \
		if (adt != A) \
		{ \
			if (adt == ATT_NO_DATA) \
			{ \
				if ((*extract_elt_array)[extract_ctr].inner_blob.length() == 0) \
					ext_state.set(isempty_flag); \
			} \
			else \
				ext_state.set(wrongtype_flag); \
		} \
		else \
		{ \
			datum = (uni_ptr->B())[0]; \
			extract_ctr++; \
		} \
	} \
\
	if (ext_state.any() == true) \
		failed = true; \
\
	if (ext_state.test(notenoughde_flag) == true && exceptions_flags.test(notenoughde_flag) == true) \
		throw_too_many("operator>>",true); \
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
	if (extract_ctr > extract_elt_array->length() - 1) \
		ext_state.set(notenoughde_flag); \
	else \
	{ \
		const AttrValUnion *uni_ptr = &((*extract_elt_array)[extract_ctr].value); \
		AttributeDataType adt = uni_ptr->_d(); \
		if (adt != A) \
		{ \
			if (adt == ATT_NO_DATA) \
			{ \
				if ((*extract_elt_array)[extract_ctr].inner_blob.length() == 0) \
					ext_state.set(isempty_flag); \
			} \
			else \
				ext_state.set(wrongtype_flag); \
		} \
		else \
		{ \
			const C &dvsa = uni_ptr->B(); \
			datum << dvsa; \
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
	if (ext_state.test(wrongtype_flag) == true && exceptions_flags.test(wrongtype_flag) == true) \
		throw_type_except(D,"operator>>");

//
// A is the sequence CORBA name
// B is the IDL enum method to set data
//

#define INSERT_BASIC_TYPE(A,B) \
	if (insert_ctr > insert_elt_array->length() - 1) \
		throw_too_many("operator<<",false); \
\
	A dvsa; \
	dvsa.length(1); \
	dvsa[0] = datum; \
\
	(*insert_elt_array)[insert_ctr].value.B(dvsa); \
	insert_ctr++;


#endif /* _DEVICEPIPE_H */

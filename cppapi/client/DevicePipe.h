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

template <typename T>
struct DataElement
{

	DataElement(const string &,T);
	DataElement();

	string		name;
	T			value;
};

template <typename T>
DataElement<T>::DataElement(const string &_na,T _val):name(_na),value(_val)
{
}

template <typename T>
DataElement<T>::DataElement()
{
}

template <typename T>
struct WDataElement: public DataElement<T>
{
	WDataElement(const string &,T,size_t,bool);

	size_t		size;
	bool		release;
};

template <typename T>
WDataElement<T>::WDataElement(const string &_na,T _val,size_t _s,bool _r):DataElement<T>(_na,_val),size(_s),release(_r)
{
}


class DevicePipeBlob
{
public:
	DevicePipeBlob(const string &_na):name(_na),insert_elt_array(Tango_nullptr),insert_ctr(0),extract_elt_array(Tango_nullptr),extract_ctr(0),extract_delete(false),ext(Tango_nullptr) {}
	DevicePipeBlob():insert_elt_array(Tango_nullptr),insert_ctr(0),extract_elt_array(Tango_nullptr),extract_ctr(0),extract_delete(false),ext(Tango_nullptr) {}

	~DevicePipeBlob();

	void set_name(const string &_na) {name=_na;}
	const string &get_name() {return name;}

	size_t get_data_elt_nb() {return extract_elt_array->length();}
	vector<string> get_data_elt_name();
	string get_data_elt_name(size_t _ind);
	int get_data_elt_type(size_t _ind);




	void set_data_elt_name(size_t _ind);
	void set_data_elt_names(vector<string> &);




	void operator << (short &);
	void operator << (DevLong &);
	void operator << (double &);
	void operator << (DevicePipeBlob &);

	bool operator >> (short &);
	bool operator >> (vector<short> &);
	bool operator >> (DevLong &);
	bool operator >> (double &);
	bool operator >> (DevicePipeBlob &);

	DevicePipeBlob &operator[](const string &);

	const char *get_current_delt_name() {return (*extract_elt_array)[extract_ctr].name.in();}
	size_t get_extract_ind_from_name(const string &);

	void reset_insert_ctr() {insert_ctr=0;}
	DevVarPipeDataEltArray *get_insert_data() {return insert_elt_array;}
	const DevVarPipeDataEltArray *get_extract_data() {return extract_elt_array;}

	void set_extract_data(const DevVarPipeDataEltArray *_ptr) {extract_elt_array=_ptr;}
	void reset_extract_ctr() {extract_ctr=0;}
	void set_extract_delete(bool _b) {extract_delete=_b;}

	void print(ostream &,int);

protected:
	void throw_type_except(const string &,const string &);
	void throw_too_many(const string &);

private:
	string							name;					// The blob name

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
 * Fundamental type for receiving data from device pipes
 *
 * This is the fundamental type for receiving data from device pipes.
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
///@privatesection

	DevicePipe(const DevicePipe &);

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

/**@name Exception and error related methods
 */
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
	~DevicePipe();

	void set_time(TimeVal &_ti) {time=_ti;}

	vector<string> get_data_elt_name() {return the_root_blob.get_data_elt_name();}
	size_t get_data_elt_nb() {return the_root_blob.get_data_elt_nb();}

	string get_data_elt_name(size_t _ind) {return the_root_blob.get_data_elt_name(_ind);}
	int get_data_elt_type(size_t _ind) {return the_root_blob.get_data_elt_type(_ind);}

	DevicePipe &operator[](const string &);



	void set_root_blob_data_elt_names(vector<string> &v_n) {the_root_blob.set_data_elt_names(v_n);}



	DevicePipeBlob			the_root_blob;			// Root blob

private:
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
 * 					Some DevicePipe and DataElement helper functions	   			    *
 * 					-------------------------------						    		    *
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

//
// For DevicePipe extraction
//

template <typename T>
DevicePipe &operator>>(DevicePipe &,T &);

template <typename T>
DevicePipe &operator>>(DevicePipe &, DataElement<T> &);


#endif /* _DEVICEPIPE_H */

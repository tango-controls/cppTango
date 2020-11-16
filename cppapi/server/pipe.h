//===================================================================================================================
//
// file :		Pipe.h
//
// description :	Include file for the Pipe class.
//
// project :		TANGO
//
// author(s) :		E.Taurel
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
//
//===================================================================================================================

#ifndef _PIPE_H
#define _PIPE_H

#include <tango.h>

#include <stdarg.h>

namespace Tango
{



/**
 * This class is a class representing a pipe in the TANGO device server pattern. It is an abstract class.
 * It is the root class for all pipe related classes.
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class Pipe
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Pipe object.
 *
 * The default constructor
 */
	Pipe():ext(new PipeExt) {}

/**
 * Constructs a newly allocated Pipe object from its
 * name and its display level
 *
 * @param 	name	The pipe name
 * @param	level	The pipe display level
 * @param	pwt		The pipe R/W type (default to READ)
 *
 */
	Pipe(const std::string &name,const Tango::DispLevel level,const PipeWriteType pwt=PIPE_READ);
//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
    virtual ~Pipe() {}
//@}

/**@name Miscellaneous methods */
//@{

//@}

/**@name Get/Set object members.
 * These methods allows the external world to get/set Pipe instance
 * data members
 */
//@{
/**
 * Return the pipe name.
 *
 * @return The pipe name
 */
	std::string &get_name() {return name;}

/**
 * Set the pipe name.
 *
 * @param new_name The new pipe name
 */
	void set_name(std::string &new_name) {name=new_name;}

/**
 * Set default attribute properties
 *
 * @param	prop	The user default property class
 */
 	void set_default_properties(UserDefaultPipeProp &prop);

/**
 * Return the pipe name in lower case letters.
 *
 * @return The pipe name
 */
	std::string &get_lower_name() {return lower_name;}

/**
 * Return the root data blob name.
 *
 * @return The data blob name
 */
	const std::string &get_root_blob_name() {return the_blob.get_name();}

/**
 * Set the root data blob name.
 *
 * @param [in] name The root data blob name
 */
	void set_root_blob_name(const std::string &name) {the_blob.set_name(name);}

/**
 * Return the pipe description.
 *
 * @return The pipe description
 */
	std::string &get_desc() {return desc;}

/**
 * Return the pipe label.
 *
 * @return The pipe label
 */
	std::string &get_label() {return label;}

/**
 * Return the pipe display level.
 *
 * @return The pipe display level
 */
	Tango::DispLevel get_disp_level() {return disp_level;}

/**
 * Get the pipe writable type (RO/RW).
 *
 * @return The pipe write type.
 */
	Tango::PipeWriteType get_writable() {return writable;}

/**
 * Set pipe serialization model
 *
 * This method allows the user to choose the pipe serialization model.
 *
 * @param [in] ser_model The new serialisation model. The serialization model must be
 * one of PIPE_BY_KERNEL, PIPE_BY_USER or PIPE_NO_SYNC
 */
	void set_pipe_serial_model(PipeSerialModel ser_model);
/**
 * Get pipe serialization model
 *
 * Get the pipe serialization model
 *
 * @return The pipe serialization model
 */
	PipeSerialModel get_pipe_serial_model() {return pipe_serial_model;}
/**
 * Set pipe user mutex
 *
 * This method allows the user to give to the pipe object the pointer to
 * the omni_mutex used to protect its buffer. The mutex has to be locked when passed
 * to this method. The Tango kernel will unlock it when the data will be transferred
 * to the client.
 *
 * @param [in] mut_ptr The user mutex pointer
 */
	void set_user_pipe_mutex(omni_mutex *mut_ptr) {user_pipe_mutex = mut_ptr;}
//@}

/**@name Inserting data into a DevicePipe
 */
//@{
#ifdef GEN_DOC
/**
 * Insert data into a device pipe
 *
 * Inserting data into a Pipe instance is simlar to inserting data into a DevicePipeBlob class instance.
 * See doc of DevicePipeBlob class insertion methods (DevicePipeBlob::operator<<) to get a complete documentation on
 * how to insert data into a Pipe object
 *
 * @param [in] datum The data to be inserted into the Pipe object
 * @exception WrongData if requested
 */
	Pipe & operator << (short &datum);
#endif
/**
 * Set blob data element number
 *
 * Set the blob data element number
 *
 * @param [in] nb The blob data element number
 */
	void set_data_elt_nb(size_t nb) {the_blob.set_data_elt_nb(nb);}
/**
 * Set blob data element number and names
 *
 * Set the blob data element number and names. The data element number is the number of names in the input
 * parameter.
 *
 * @param [in] names The blob data element names
 */
	void set_data_elt_names(std::vector<std::string> &names) {the_blob.set_data_elt_names(names);}
/**
 * Get blob data element number
 *
 * Get the blob data element number
 *
 * @return The blob data element number
 */
	size_t get_data_elt_nb() {return the_blob.get_data_elt_nb();}
//@}


/**@name Exception and error related methods methods
 */
//@{
/**
 * Set exception flag
 *
 * It's a method which allows the user to switch on/off exception throwing when trying to insert/extract data from a
 * Pipe object. The following flags are supported :
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
	void exceptions(std::bitset<DevicePipeBlob::numFlags> fl) {the_blob.exceptions(fl);}
/**
 * Get exception flag
 *
 * Returns the whole exception flags.
 * The following is an example of how to use these exceptions related methods
 * @code
 * Pipe pi;
 *
 * std::bitset<DevicePipeBlob::numFlags> bs = pi.exceptions();
 * cout << "bs = " << bs << std::endl;
 *
 * pi.set_exceptions(DevicePipeBlob::wrongtype_flag);
 * bs = pi.exceptions();
 *
 * cout << "bs = " << bs << std::endl;
 * @endcode
 *
 * @return The exception flag
 */
	std::bitset<DevicePipeBlob::numFlags> exceptions() {return the_blob.exceptions();}
/**
 * Reset one exception flag
 *
 * Resets one exception flag
 *
 * @param [in] fl The exception flag
 */
	void reset_exceptions(DevicePipeBlob::except_flags fl) {the_blob.reset_exceptions(fl);}
/**
 * Set one exception flag
 *
 * Sets one exception flag. See DevicePipeBlob::exceptions() for a usage example.
 *
 * @param [in] fl The exception flag
 */
	void set_exceptions(DevicePipeBlob::except_flags fl) {the_blob.set_exceptions(fl);}
/**
 * Check insertion/extraction success
 *
 * Allow the user to check if insertion/extraction into/from Pipe instance was successfull. This
 * method has to be used when exceptions are disabled.
 *
 * @return True if insertion/extraction has failed
 */
	bool has_failed() {return the_blob.has_failed();}
/**
 * Get instance insertion/extraction state
 *
 * Allow the user to find out what was the reason of insertion/extraction into/from Pipe failure. This
 * method has to be used when exceptions are disabled.
 * Here is an example of how methods has_failed() and state() could be used
 * @code
 * Pipe dpb = ....
 *
 * std::bitset<DevicePipeBlob::numFlags> bs;
 * bs.reset();
 * dpb.exceptions(bs);
 *
 * DevLong dl;
 * dpb >> dl;
 *
 * if (dpb.has_failed() == true)
 * {
 *    std::bitset<DevicePipeBlob::numFlags> bs_err = dpb.state();
 *    if (dpb.test(DevicePipeBlob::isempty_flag) == true)
 *        .....
 * }
 * @endcode
 *
 * @return The error bit set.
 */
	std::bitset<DevicePipeBlob::numFlags> state() {return the_blob.state();}
//@}

/**@name set_value methods.
 * Methods to set the value of the data blob to be transported by the pipe
 */
//@{

//@}

/// @privatesection
	virtual bool is_allowed (DeviceImpl *dev,PipeReqType) {(void)dev;return true;}
	virtual void read(DeviceImpl *) {}

	Pipe &operator[](const std::string &);

	bool get_value_flag() {return value_flag;}
	void set_value_flag(bool val) {value_flag = val;}
	void set_time();
	Tango::TimeVal &get_when() {return when;}
	void set_returned_data_ptr(DevPipeData *_p) {ret_data=_p;}
	void set_label(const std::string &_lab) {label=_lab;}
	void set_desc(const std::string &_desc) {desc=_desc;}
	bool is_label_lib_default() {return label==name;}
	bool is_desc_lib_default() {return desc==DescNotSpec;}
	void set_upd_properties(const PipeConfig &,DeviceImpl *);
	void set_properties(const Tango::PipeConfig &,DeviceImpl *,std::vector<Attribute::AttPropDb> &);
	void upd_database(std::vector<Attribute::AttPropDb> &,std::string &);
	std::vector<PipeProperty> &get_user_default_properties() {return user_def_prop;}
	void set_one_str_prop(const char *,const CORBA::String_member &,std::string &,std::vector<Attribute::AttPropDb> &,std::vector<PipeProperty> &,std::vector<PipeProperty> &,const char *);
	bool prop_in_list(const char *,std::string &,size_t,std::vector<PipeProperty> &);

	DevicePipeBlob &get_blob() {return the_blob;}

	omni_mutex *get_pipe_mutex() {return &pipe_mutex;}
	omni_mutex *get_user_pipe_mutex() {return user_pipe_mutex;}

	void fire_event(DeviceImpl *,DevFailed *);
#ifdef _TG_WINDOWS_
	void fire_event(DeviceImpl *,DevicePipeBlob *,bool);
#else
	void fire_event(DeviceImpl *_dev,DevicePipeBlob *_dat,bool bo) {struct timeval now;gettimeofday(&now,NULL);fire_event(_dev,_dat,now,bo);}
#endif
	void fire_event(DeviceImpl *,DevicePipeBlob *,struct timeval &,bool);

	void set_event_subscription(time_t _t) {event_subscription = _t;}
	bool is_pipe_event_subscribed() const;

	friend class EventSupplier;
	friend class ZmqEventSupplier;

protected:
/**@name Class data members */
//@{
/**
 * The pipe name
 */
	std::string 					name;
/**
 * The pipe name in lower case
 */
	std::string 					lower_name;
/**
 * The pipe description
 */
	std::string					desc;
/**
 * The pipe label
 */
	std::string					label;
/**
 * The pipe display level
 */
    Tango::DispLevel		disp_level;
/**
 * The pipe R/W type
 */
    Tango::PipeWriteType	writable;
//@}

	DevicePipeBlob			the_blob;

private:
    class PipeExt
    {
    public:
        PipeExt() {}
    };

    std::unique_ptr<PipeExt>          ext;           	// Class extension

	bool						value_flag;			// Flag set when pipe value is set
	Tango::TimeVal				when;				// Date associated to the pipe
	Tango::DevPipeData 			*ret_data;			// Pointer for read data

	std::vector<std::string> 				pe_out_names;		// Data elements name
	int 						rec_count;			// Data elements ctr

    PipeSerialModel				pipe_serial_model;	// Flag for attribute serialization model

	omni_mutex					pipe_mutex;			// Mutex to protect the pipe shared data buffer
	omni_mutex					*user_pipe_mutex;	// Ptr for user mutex in case he manages exclusion

    time_t						event_subscription;	// Last time() a subscription was made
    std::vector<PipeProperty>		user_def_prop;		// User default properties
};

template <typename T>
Pipe &operator<<(Pipe &,T &);

template <typename T>
Pipe &operator<<(Pipe &,T *);

template <typename T>
Pipe &operator<<(Pipe &, DataElement<T> &);


//
// Throw exception if pointer is null
//

#define PIPE_CHECK_PTR(A,B,C) \
	if (A == NULL) \
	{ \
		std::stringstream o; \
		o << "Data pointer for pipe " << B << ", data element " << C << " is NULL!"; \
		Except::throw_exception(API_PipeOptProp,o.str(),"Pipe::set_value()"); \
	} \
	else \
		(void)0


//
// Some inline methods
//

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::prop_in_list
//
// description :
//		Search for a property in a list
//		A similar method exists in Attribute class!!
//
// args:
//		in :
//			- prop_name : The property name
//          - list_size : The size list
//          - list : The list
//      out :
//			- prop_str : String initialized with prop. value (if found)
//
//------------------------------------------------------------------------------------------------------------------

inline bool Pipe::prop_in_list(const char *prop_name,std::string &prop_str,size_t list_size,std::vector<PipeProperty> &list)
{
    bool ret = false;

    if (list_size != 0)
    {
        size_t i;
        for (i = 0;i < list_size;i++)
        {
            if (list[i].get_name() == prop_name)
                break;
        }
        if (i != list_size)
        {
            prop_str = list[i].get_value();
            ret = true;
        }
    }

    return ret;
}

} // End of Tango namespace

#endif // _PIPE_H

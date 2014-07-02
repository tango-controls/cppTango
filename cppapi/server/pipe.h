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

#ifndef _PIPE_H
#define _PIPE_H

#include <tango.h>

#include <stdarg.h>

namespace Tango
{

#ifndef HAS_LAMBDA_FUNC
//
// Binary function objects to be used by the find_if algorithm.
// The find_if algo. want to have a predicate, this means that the return value must be a boolean (R is its name).
// The find_if algo. needs a unary predicate. This function object is a binary function object.
// It must be used with the bind2nd function adapter
//

template <typename A1,typename A2,typename R>
struct WantedPipe : public binary_function<A1,A2,R>
{
	R operator() (A1 pipe_ptr, A2 name) const
	{
		if (::strlen(name) != pipe_ptr->get_lower_name().size())
			return false;
        string tmp_name(name);
		transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		return pipe_ptr->get_lower_name() == tmp_name;
	}
};
#endif


/**
 * This class is a class representing a pipe in the TANGO device server pattern. It is an abstract class.
 * It is the root class for all pipe related classes.
 *
 * $Author$
 * $Revision$
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
 * name, its description, its label and its display level
 *
 * @param 	na		The pipe name
 * @param	desc	The pipe description
 * @param	label 	The pipe label
 * @param	level	The pipe display level
 * @param	pwt		The pipe R/W type (default to READ)
 *
 */
	Pipe(const string &na,const string &desc,const string &label,const Tango::DispLevel level,const PipeWriteType pwt=PIPE_READ);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
#ifdef HAS_UNIQUE_PTR
    virtual ~Pipe() {}
#else
	virtual ~Pipe() {delete ext;}
#endif
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
	string &get_name() {return name;}

/**
 * Set the pipe name.
 *
 * @param new_name The new pipe name
 */
	void set_name(string &new_name) {name=new_name;}

/**
 * Return the pipe name in lower case letters.
 *
 * @return The pipe name
 */
	string &get_lower_name() {return lower_name;}

/**
 * Return the data blob name.
 *
 * @return The data blob name
 */
	string &get_blob_name() {return blob_name;}

/**
 * Return the pipe description.
 *
 * @return The pipe description
 */
	string &get_desc() {return desc;}

/**
 * Set the pipe description field.
 *
 * @param new_desc The new description
 */
	void set_desc(const char *new_desc) {desc = new_desc;}

/**
 * Return the pipe label.
 *
 * @return The pipe label
 */
	string &get_label() {return label;}

/**
 * Set the pipe label field.
 *
 * @param new_label The new label
 */
	void set_label(const char *new_label) {label = new_label;}

/**
 * Return the pipe display level.
 *
 * @return The pipe display level
 */
	Tango::DispLevel get_disp_level() {return disp_level;}

/**
 * Set the pipe display level.
 *
 * @param new_level The pipe display level
 */
	void set_disp_level(Tango::DispLevel new_level) {disp_level = new_level;}

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
 * @param ser_model The new serialisation model. The serialization model must be
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
 * @param mut_ptr The user mutex pointer
 */
	void set_user_pipe_mutex(omni_mutex *mut_ptr) {user_pipe_mutex = mut_ptr;}
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
 *
 * @param [in] fl The exception flag
 */
	void exceptions(bitset<DevicePipeBlob::numFlags> fl) {the_blob.exceptions(fl);}
/**
 * Get exception flag
 *
 * Returns the whole exception flags.
 * The following is an example of how to use these exceptions related methods
 * @code
 * Pipe pi;
 *
 * bitset<DevicePipeBlob::numFlags> bs = pi.exceptions();
 * cout << "bs = " << bs << endl;
 *
 * pi.set_exceptions(DevicePipeBlob::wrongtype_flag);
 * bs = pi.exceptions();
 *
 * cout << "bs = " << bs << endl;
 * @endcode
 *
 * @return The exception flag
 */
	bitset<DevicePipeBlob::numFlags> exceptions() {return the_blob.exceptions();}
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
 *
 * @return The error bit set.
 */
	bitset<DevicePipeBlob::numFlags> state() {return the_blob.state();}
//@}

/**@name set_value methods.
 * Methods to set the value of the data blob to be transported by the pipe
 */
//@{

//@}

/// @privatesection
	virtual bool is_allowed (DeviceImpl *dev,PipeReqType) {(void)dev;return true;}
	virtual void read(DeviceImpl *) {}

	bool get_value_flag() {return value_flag;}
	void set_value_flag(bool val) {value_flag = val;}
	void set_time();
	Tango::TimeVal &get_when() {return when;}
	void set_returned_data_ptr(DevPipeData *_p) {ret_data=_p;}

	DevicePipeBlob &get_blob() {return the_blob;}
	void set_data_elt_nb(size_t _nb) {the_blob.set_data_elt_nb(_nb);}
	void set_data_elt_names(vector<string> &_v_names) {the_blob.set_data_elt_names(_v_names);}
	void set_root_blob_name(const string &root_blob_name) {the_blob.set_name(root_blob_name);}

	omni_mutex *get_pipe_mutex() {return &pipe_mutex;}
	omni_mutex *get_user_pipe_mutex() {return user_pipe_mutex;}

protected:
/**@name Class data members */
//@{
/**
 * The pipe name
 */
	string 					name;
/**
 * The pipe name in lower case
 */
	string 					lower_name;
/**
 * The pipe description
 */
	string					desc;
/**
 * The pipe label
 */
	string					label;
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

#ifdef HAS_UNIQUE_PTR
    unique_ptr<PipeExt>          ext;           	// Class extension
#else
	PipeExt		                *ext;
#endif

	bool						value_flag;			// Flag set when pipe value is set
	Tango::TimeVal				when;				// Date associated to the pipe
	Tango::DevPipeData 			*ret_data;			// Pointer for read data

	vector<string> 				pe_out_names;		// Data elements name
	int 						rec_count;			// Data elements ctr

	string						blob_name;			// The blob name
    PipeSerialModel				pipe_serial_model;	// Flag for attribute serialization model

	omni_mutex					pipe_mutex;			// Mutex to protect the pipe shared data buffer
	omni_mutex					*user_pipe_mutex;	// Ptr for user mutex in case he manages exclusion
};

template <typename T>
Pipe &operator<<(Pipe &,T &);

template <typename T>
Pipe &operator<<(Pipe &,T *);

template <typename T>
Pipe &operator<<(Pipe &, WDataElement<T> &);


//
// Throw exception if pointer is null
//

#define PIPE_CHECK_PTR(A,B,C) \
	if (A == NULL) \
	{ \
		stringstream o; \
		o << "Data pointer for pipe " << B << ", data element " << C << " is NULL!"; \
		Except::throw_exception(API_PipeOptProp,o.str(),"Pipe::set_value()"); \
	} \
	else \
		(void)0

} // End of Tango namespace

#endif // _PIPE_H

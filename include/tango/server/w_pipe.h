//===================================================================================================================
//
// file :		w_pipe.h
//
// description :	Include file for the WPipe class.
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
// $Revision$
//
//===================================================================================================================

#ifndef _W_PIPE_H
#define _W_PIPE_H

#include <tango.h>

namespace Tango
{

/**
 * This class is a class representing a writable pipe in the TANGO device server pattern. It is an base class.
 * It is the a root class for pipe related classes.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class WPipe: public Pipe
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated WPipe object.
 *
 * The default constructor
 */
	WPipe():w_ext(new WPipeExt) {}

/**
 * Constructs a newly allocated WPipe object from its
 * name, its description, its label and its display level
 *
 * @param 	na		The pipe name
 * @param	level	The pipe display level
 *
 */
	WPipe(const string &na,const Tango::DispLevel level);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
#ifdef HAS_UNIQUE_PTR
    virtual ~WPipe() {}
#else
	virtual ~WPipe() {delete w_ext;}
#endif
//@}

/**@name Get/Set methods.
 * Methods to get the value of the data blob transported by the pipe
 */
//@{
/**
 * Get root blob name
 *
 * Get the root blob name
 *
 * @return The root blob name
 */
	string get_root_blob_name() {return the_blob.get_name();}
//@}

/**@name Extracting data from a WPipe
 */
//@{
#ifdef GEN_DOC
/**
 * Extract data from a device pipe
 *
 * Extracting data from a WPipe instance is simlar to extracting data from a DevicePipeBlob class instance.
 * See doc of DevicePipeBlob class extraction methods (DevicePipeBlob::operator>>) to get a complete documentation on
 * how to extract data from a WPipe
 *
 * @param [in] datum The pipe data
 * @exception WrongData if requested
 */
	WPipe & operator >> (short &datum);
#endif
/**
 * Get root blob data element number
 *
 * Get the root blob data element number
 *
 * @return The root blob data element number
 */
	size_t get_data_elt_nb() {return the_blob.get_data_elt_nb();}
/**
 * Get root blob data elements name
 *
 * Get the root blob data elements name
 *
 * @return The root blob data elements name
 */
	vector<string> get_data_elt_names() {return the_blob.get_data_elt_names();}
/**
 * Get root blob data element name
 *
 * Get the root blob data element name for a single data element
 *
 * @param [in] ind The data element index within the root blob
 * @return The root blob data element name
 */
	string get_data_elt_name(size_t ind) {return the_blob.get_data_elt_name(ind);}
/**
 * Get root blob data element value type
 *
 * Get the root blob data element value type for a single data element
 *
 * @param [in] ind The data element index within the root blob
 * @return The blob data element value type
 */
	int get_data_elt_type(size_t ind) {return the_blob.get_data_elt_type(ind);}
//@}

/// @privatesection
	virtual void write(DeviceImpl *) {}


	WPipe &operator[](const string &);

private:
    class WPipeExt
    {
    public:
        WPipeExt() {}
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<WPipeExt>			w_ext;           // Class extension
#else
	WPipeExt		 				*w_ext;
#endif

};


template <typename T>
WPipe &operator>>(WPipe &,T &);

template <typename T>
WPipe &operator>>(WPipe &,T *);

template <typename T>
WPipe &operator>>(WPipe &, DataElement<T> &);


} // End of Tango namespace

#endif // _W_PIPE_H

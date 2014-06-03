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
 *
 */
	Pipe(const string &na,const string &desc,const string &label,const Tango::DispLevel level);
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
/**
 * Check if it is allowed to read the pipe in the actual device state.
 *
 * This method is automatically called by the TANGO core classes when the associated pipe is read by a client to check
 * if the pipe is allowed in the actual device state. This method is the default is_allowed method which
 * always allows the pipe to be read. It is possible to re-define it if this default behaviour does not fullfill
 * the device needs.
 *
 * @param dev The device on which the pipe must be read
 * @return A boolean set to true is the pipe is allowed. Otherwise, the return value is false.
 */
	virtual bool is_allowed (DeviceImpl *dev) {(void)dev;return true;}
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
//@}

/**@name set_value methods.
 * Methods to set the value of the data blob to be transported by the pipe
 */
//@{

//@}

protected:
/**@name Class data members */
//@{
/**
 * The pipe name
 */
	string 				name;
/**
 * The pipe name in lower case
 */
	string 				lower_name;
/**
 * The pipe description
 */
	string				desc;
/**
 * The pipe label
 */
	string				label;
/**
 * The pipe display level
 */
    Tango::DispLevel	disp_level;
//@}

private:
    class PipeExt
    {
    public:
        PipeExt() {}
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<PipeExt>          ext;           // Class extension
#else
	PipeExt		                *ext;
#endif

};


} // End of Tango namespace

#endif // _PIPE_H

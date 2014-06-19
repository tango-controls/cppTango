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
 * @param	desc	The pipe description
 * @param	label 	The pipe label
 * @param	level	The pipe display level
 *
 */
	WPipe(const string &na,const string &desc,const string &label,const Tango::DispLevel level);
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

/**@name get_value methods.
 * Methods to get the value of the data blob transported by the pipe
 */
//@{

//@}

/// @privatesection
	virtual void write(DeviceImpl *) {}

	struct SvrPipeDataElt
	{
		string						name;
		long						type;
		WPipe						*upper;

		DevVarLongArray_var 		LongSeq;
		DevVarShortArray_var 		ShortSeq;
		DevVarDoubleArray_var 		DoubleSeq;

		SvrPipeDataElt(const string &_na,long _ty,WPipe *_ptr):name(_na),type(_ty),upper(_ptr) {}
		SvrPipeDataElt(const string &_na,WPipe *_ptr):name(_na),upper(_ptr) {}

		bool operator >> (short &);
		bool operator >> (DevLong &);
		bool operator >> (double &);
	};

	vector<SvrPipeDataElt>		v_elt;

	size_t get_data_elt_nb() {return v_elt.size();}
	string &get_data_elt_name(size_t);
	int get_data_elt_type(size_t);

	SvrPipeDataElt &get_data_elt(size_t);
	SvrPipeDataElt &get_data_elt(const string &);

	void get_write_value(const string &,short &);
	void get_write_value(size_t,short &);

	void get_write_value(const string &,double &);
	void get_write_value(size_t,double &);

	void get_write_value(const string &,DevLong &);
	void get_write_value(size_t,DevLong &);

	void throw_type_except(const string &,const string &);

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

WPipe &operator>>(WPipe &,short &);
WPipe &operator>>(WPipe &,double &);
WPipe &operator>>(WPipe &,DevLong &);

} // End of Tango namespace

#endif // _W_PIPE_H

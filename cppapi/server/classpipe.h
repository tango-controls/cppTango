//=================================================================================================================
//
// file :		ClassPipe.h
//
// description :	Include file for the Tango pipe.
//					MultiClassAttribute : A class to manage all the class level definition of pipes.
//					There is one instance of this class for each Tango class
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

#ifndef _CLASS_PIPE_H
#define _CLASS_PIPE_H

#include <tango.h>

namespace Tango
{

//=================================================================================================================
//
//			The PipeProperty class
//
//
// description :	This class is used to store a pair of pipe prop name, pipe prop value. Both name and value are
//					stored as strings
//
//=================================================================================================================

class PipeProperty
{
public:
	PipeProperty(const string &name,const string &value):prop_name(name),prop_value(value) {}
	~PipeProperty() {};

	const string &get_value() {return prop_value;}
	const string &get_name() {return prop_name;}

private:
	string			prop_name;
	string			prop_value;
};

//=================================================================================================================
//
//			The MultiClassPipe class
//
//
// description :
//
//=================================================================================================================

class DeviceClass;

class MultiClassPipe
{
public:
	MultiClassPipe();
	~MultiClassPipe();

	void init_class_pipe(DeviceClass *);

	vector<Tango::PipeProperty> &get_prop_list(const string &);
/*	PipeProperty &get_prop(const string &prop_name);*/

protected:
	map<string,vector<Tango::PipeProperty> >		pipe_prop_list;			// pipe_name - prop list
};

} // End of Tango namespace

#endif // _CLASS_ATTRIBUTE_H

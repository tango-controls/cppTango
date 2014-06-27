static const char *RcsId = "$Id$\n$Name$";

//+==================================================================================================================
//
// file :               Pipe.cpp
//
// description :        C++ source code for the Pipe class. The Pipe class is the root class for all derived
//						Pipe classes.
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::Pipe
//
// description :
//		Constructor for abstract class Pipe
//
//-----------------------------------------------------------------------------------------------------------------

Pipe::Pipe(const string &_name,const string &_desc,const string &_label,Tango::DispLevel _level,PipeWriteType _pwt)
:name(_name),desc(_desc),label(_label),disp_level(_level),writable(_pwt),ext(new PipeExt)
{
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		Pipe::set_time
//
// description :
//		Set the date in the pipe object
//
//--------------------------------------------------------------------------------------------------------------------

void Pipe::set_time()
{
#ifdef _TG_WINDOWS_
	struct _timeb t;
	_ftime(&t);

	when.tv_sec = (CORBA::Long)t.time;
	when.tv_usec = (CORBA::Long)(t.millitm * 1000);
	when.tv_nsec = 0;
#else
	struct timezone tz;
	struct timeval tv;
	gettimeofday(&tv,&tz);

	when.tv_sec = (CORBA::Long)tv.tv_sec;
	when.tv_usec = (CORBA::Long)tv.tv_usec;
	when.tv_nsec = 0;
#endif
}

// TODO: Test with release flag set


} // End of Tango namespace

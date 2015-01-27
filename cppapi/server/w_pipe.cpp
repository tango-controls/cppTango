static const char *RcsId = "$Id$\n$Name$";

//+==================================================================================================================
//
// file :               W_pipe.cpp
//
// description :        C++ source code for the WPipe class. The WPipe class is a root class for derived
//						Pipe classes.
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
//		WPipe::WPipe
//
// description :
//		Constructor for base class WPipe
//
//-----------------------------------------------------------------------------------------------------------------

WPipe::WPipe(const string &_name,Tango::DispLevel _level)
:Pipe(_name,_level,PIPE_READ_WRITE),w_ext(new WPipeExt)
{
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		WPipe::operator[]
//
// description :
//		Retrieve one data element from the WPipe from its name
//
//-----------------------------------------------------------------------------------------------------------------

WPipe &WPipe::operator[](const string &_na)
{
	get_blob().operator[](_na);
	return *this;
}


} // End of Tango namespace

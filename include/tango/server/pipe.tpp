
//+==================================================================================================================
//
// file :               Pipe.tpp
//
// description :        C++ source code for the template methods of the Pipe class.
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
// $Revision: 25781 $
//
//-==================================================================================================================

#ifndef _PIPE_TPP
#define _PIPE_TPP


namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	<<
//
// description :
//		Helper function to ease data insertion into Pipe root blob
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
Pipe &operator<<(Pipe &_dp,T &datum)
{
	_dp.get_blob().operator<<(datum);
	_dp.set_value_flag(true);
	return _dp;
}

template <typename T>
Pipe &operator<<(Pipe &_dp,T *datum)
{
	_dp.get_blob().operator<<(datum);
	_dp.set_value_flag(true);
	return _dp;
}

template <typename T>
Pipe &operator<<(Pipe &_dp,DataElement<T> &datum)
{
	_dp.get_blob().set_current_delt_name(datum.name);
	_dp.get_blob().operator<<(datum.value);
	_dp.set_value_flag(true);
	return _dp;
}

} // End of Tango namespace
#endif // _PIPE_TPP

//+===================================================================================================================
//
// file :               w_pipe.tpp
//
// description :        C++ source code for the WPipe class template methods/functions
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
// $Revision: 17240 $
//
//-===================================================================================================================

#ifndef _W_PIPE_TPP
#define _W_PIPE_TPP


namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	>>
//
// description :
//		Helper function to ease data extraction from WPipe root blob
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
WPipe &operator>>(WPipe &_dp,T &datum)
{
	_dp.get_blob().operator>>(datum);
	return _dp;
}

template <typename T>
WPipe &operator>>(WPipe &_dp,T *datum)
{
	_dp.get_blob().operator>>(datum);
	return _dp;
}

template <typename T>
WPipe &operator>>(WPipe &_dp,DataElement<T> &datum)
{
	datum.name = _dp.get_blob().get_current_delt_name();
	_dp.get_blob().operator>>(datum.value);
	return _dp;
}

} // End of Tango namespace
#endif // _W_PIPE_TPP

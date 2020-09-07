//+===================================================================================================================
//
// file :               devapi_pipe.tpp
//
// description :        C++ source code for the DevicePipe class template methods/functions
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

#ifndef _DEVAPI_PIPE_TPP
#define _DEVAPI_PIPE_TPP


namespace Tango
{
//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	<<
//
// description :
//		Helper function to ease data insertion into DevicePipe/DevicePipeBlob instance
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
DevicePipe &operator<<(DevicePipe &_dp,T &datum)
{
	_dp.get_root_blob().operator<<(datum);
	return _dp;
}

template <typename T>
DevicePipe &operator<<(DevicePipe &_dp,T *datum)
{
	_dp.get_root_blob().operator<<(datum);
	return _dp;
}

template <typename T>
DevicePipe &operator<<(DevicePipe &_dp,DataElement<T> &datum)
{
	_dp.get_root_blob().set_current_delt_name(datum.name);
	_dp.get_root_blob().operator<<(datum.value);
	return _dp;
}

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &_dp,T &datum)
{
	_dp.operator<<(datum);
	return _dp;
}

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &_dp,T *datum)
{
	_dp.operator<<(datum);
	return _dp;
}

template <typename T>
DevicePipeBlob &operator<<(DevicePipeBlob &_dp,DataElement<T> &datum)
{
	_dp.set_current_delt_name(datum.name);
	_dp.operator<<(datum.value);
	return _dp;
}

//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	>>
//
// description :
//		Helper function to ease data extraction from DevicePipe root blob
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
DevicePipe &operator>>(DevicePipe &_dp,T &datum)
{
	_dp.get_root_blob().operator>>(datum);
	return _dp;
}

template <typename T>
DevicePipe &operator>>(DevicePipe &_dp,T *datum)
{
	_dp.get_root_blob().operator>>(datum);
	return _dp;
}

template <typename T>
DevicePipe &operator>>(DevicePipe &_dp,DataElement<T> &datum)
{
	datum.name = _dp.get_root_blob().get_current_delt_name();
	_dp.get_root_blob().operator>>(datum.value);
	return _dp;
}

template <typename T>
DevicePipeBlob &operator>>(DevicePipeBlob &_dp,T &datum)
{
	_dp.operator>>(datum);
	return _dp;
}

template <typename T>
DevicePipeBlob &operator>>(DevicePipeBlob &_dp,T *datum)
{
	_dp.operator>>(datum);
	return _dp;
}

template <typename T>
DevicePipeBlob &operator>>(DevicePipeBlob &_dp,DataElement<T> &datum)
{
	datum.name = _dp.get_current_delt_name();
	_dp.operator>>(datum.value);
	return _dp;
}

//+------------------------------------------------------------------------------------------------------------------
//
// Function
// 		operator overloading : 	>>
//
// description :
//		Helper function to ease printing of DataElement class instances: For basic type, for vector of basic type
//		and for CORBA sequence ptr
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
std::ostream &operator<<(std::ostream &str, const DataElement<T> &dd)
{
	str << "Name = " << dd.name << " - Value = " << dd.value;
	return str;
}

template <>
std::ostream &operator<<(std::ostream &str, const DataElement<DevString> &dd)
{
	str << "Name = " << dd.name << " - Value = " << dd.value;
	return str;
}

template <typename T>
std::ostream &operator<<(std::ostream &str, const DataElement<std::vector<T> > &dd)
{
	str << "Name = " << dd.name << " - Value = ";
	for (size_t loop = 0;loop < dd.value.size();loop++)
	{
		str << dd.value[loop];
		if (loop <= dd.value.size() - 2)
			str << ", ";
	}
	return str;
}

template <typename T>
std::ostream &operator<<(std::ostream &str, const DataElement<T *> &dd)
{
	str << "Name = " << dd.name << " - Value = ";
	for (size_t loop = 0;loop < dd.value->length();loop++)
	{
		str << (*dd.value)[loop];
		if (loop <= dd.value->length() - 2)
			str << ", ";
	}
	return str;
}

template <>
std::ostream &operator<<(std::ostream &str, const DataElement<DevicePipeBlob> &dd)
{
	str << "Name = " << dd.name << "- Value = ";
	dd.value.print(str,0,true);
	dd.value.print(str,0,false);
	return str;
}

} // End of Tango namespace
#endif // _DEVAPI_PIPE_TPP

//+===================================================================================================================
//
// file :               FWdAttribute.tpp
//
// description :        C++ source code for the FwdAttribute class template methods when they are not specialized
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2011,2012,2013
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

#ifndef _FWDATTRIBUTE_TPP
#define _FWDATTRIBUTE_TPP

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::set_local_attribute
//
// description :
//		Set value in the local attribute
//
// argument :
//		in :
//			- da : The DeviceAttribute coming from the root device read_attribute()
//
//--------------------------------------------------------------------------------------------------------------------

template<typename T>
void FwdAttribute::set_local_attribute(DeviceAttribute &da,T* &seq_ptr)
{
	qual = da.get_quality();

	TimeVal local_tv = da.get_date();
	tv.tv_sec = local_tv.tv_sec;
	tv.tv_usec = local_tv.tv_usec;

	da >> seq_ptr;

	if (writable == READ_WRITE || writable == READ_WITH_WRITE)
	{
		set_write_value(seq_ptr->get_buffer() + da.get_nb_read(),da.get_written_dim_x(),da.get_written_dim_y());
	}
	set_value_date_quality(seq_ptr->get_buffer(true),tv,qual,da.get_dim_x(),da.get_dim_y(),true);

	delete seq_ptr;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::propagate_writen_data
//
// description :
//		Pass the data received by a writable attribute into a DeviceAttribute used to write another attribute
//
// argument :
//		in :
//			- attr : The attribute
//			- ptr : Intermidiate pointer
//			- seq_ptr : Poniter to sequence used to fill in the DeviceAttribute
//		out :
//			- da : The DeviceAttribute coming from the root device read_attribute()
//
//--------------------------------------------------------------------------------------------------------------------

template<typename T,typename V>
void FwdAttribute::propagate_writen_data(DeviceAttribute &da,WAttribute &attr,T *&ptr,V *&seq_ptr)
{
	attr.get_write_value(const_cast<const T* &>(ptr));
	int data_length = attr.get_write_value_length();

	seq_ptr = new V(data_length,data_length,ptr,false);
	da << seq_ptr;
}

template<>
void FwdAttribute::propagate_writen_data(DeviceAttribute &da,WAttribute &attr,ConstDevString *&ptr,DevVarStringArray *&seq_ptr)
{
	const ConstDevString *tmp_ptr = const_cast<const ConstDevString *>(ptr);
	attr.get_write_value(tmp_ptr);
	int data_length = attr.get_write_value_length();

	seq_ptr = new DevVarStringArray(data_length,data_length,const_cast<DevString *>(tmp_ptr),false);
	da << seq_ptr;
}


} // End of Tango namespace
#endif // _FWDATTRIBUTE_TPP

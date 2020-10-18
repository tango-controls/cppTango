//+===================================================================================================================
//
// file :               fwdattribute_spec.tpp
//
// description :        C++ source code for the FwdAttribute class template methods specialization
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2013,2014,2015
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

#ifndef _FWDATTRIBUTE_SPEC_TPP
#define _FWDATTRIBUTE_SPEC_TPP

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		FwdAttribute::propagate_writen_data
//
// description :
//		Pass the data received by a writable attribute into a DeviceAttribute used to write another attribute
//		Specialization for string data type.
//
// argument :
//		in :
//			- attr : The attribute
//			- ptr : Intermediate pointer
//			- seq_ptr : Poniter to sequence used to fill in the DeviceAttribute
//		out :
//			- da : The DeviceAttribute coming from the root device read_attribute()
//
//--------------------------------------------------------------------------------------------------------------------

template<>
void FwdAttributePrivate::propagate_writen_data(DeviceAttribute &da,WAttribute &attr,ConstDevString *&ptr,DevVarStringArray *&seq_ptr)
{
	const ConstDevString *tmp_ptr = const_cast<const ConstDevString *>(ptr);
	attr.get_write_value(tmp_ptr);
	long data_length = attr.get_impl().get_write_value_length();
	long w_dim_x = attr.get_impl().get_w_dim_x();
	long w_dim_y = attr.get_impl().get_w_dim_y();

	seq_ptr = new DevVarStringArray(data_length,data_length,const_cast<DevString *>(tmp_ptr),false);
	da.insert(seq_ptr,w_dim_x,w_dim_y);
}

template<>
void FwdAttributePrivate::propagate_writen_data(DeviceAttribute &da,WAttribute &attr,DevEncoded *&ptr,DevVarEncodedArray *&seq_ptr)
{
	attr.get_write_value(const_cast<const DevEncoded* &>(ptr));
	int data_length = attr.get_impl().get_write_value_length();

	seq_ptr = new DevVarEncodedArray(data_length,data_length,ptr,false);
	da << seq_ptr;
}

} // End of Tango namespace
#endif // _FWDATTRIBUTE_SPEC_TPP

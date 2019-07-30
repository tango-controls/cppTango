//+==================================================================================================================
//
// file :               PollThread.tpp
//
// description :        C++ source code for the Pollthread class template methods
//
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2015
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
//
//-==================================================================================================================


namespace Tango
{

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::robb_data
//
// description :
//
// argument :
//		in :
//			- attr_value : The poor attribute value on which we are going to robb data
//          - new_attr_value : The robber
//
//------------------------------------------------------------------------------------------------------------------

template <typename T>
void PollThread::robb_data(T &attr_value,T &new_attr_value)
{
    switch (attr_value.value._d())
    {
        case ATT_BOOL:
        {
            DevVarBooleanArray &union_seq = attr_value.value.bool_att_value();
            DevVarBooleanArray tmp_seq(1);
            new_attr_value.value.bool_att_value(tmp_seq);
            DevVarBooleanArray &new_union_seq = new_attr_value.value.bool_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_SHORT:
        {
            DevVarShortArray &union_seq = attr_value.value.short_att_value();
            DevVarShortArray tmp_seq(1);
            new_attr_value.value.short_att_value(tmp_seq);
            DevVarShortArray &new_union_seq = new_attr_value.value.short_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_LONG:
        {
            DevVarLongArray &union_seq = attr_value.value.long_att_value();
            DevVarLongArray tmp_seq(1);
            new_attr_value.value.long_att_value(tmp_seq);
            DevVarLongArray &new_union_seq = new_attr_value.value.long_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_LONG64:
        {
            DevVarLong64Array &union_seq = attr_value.value.long64_att_value();
            DevVarLong64Array tmp_seq(1);
            new_attr_value.value.long64_att_value(tmp_seq);
            DevVarLong64Array &new_union_seq = new_attr_value.value.long64_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_FLOAT:
        {
            DevVarFloatArray &union_seq = attr_value.value.float_att_value();
            DevVarFloatArray tmp_seq(1);
            new_attr_value.value.float_att_value(tmp_seq);
            DevVarFloatArray &new_union_seq = new_attr_value.value.float_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_DOUBLE:
        {
            DevVarDoubleArray &union_seq = attr_value.value.double_att_value();
            DevVarDoubleArray tmp_seq(1);
            new_attr_value.value.double_att_value(tmp_seq);
            DevVarDoubleArray &new_union_seq = new_attr_value.value.double_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_UCHAR:
        {
            DevVarUCharArray &union_seq = attr_value.value.uchar_att_value();
            DevVarUCharArray tmp_seq(1);
            new_attr_value.value.uchar_att_value(tmp_seq);
            DevVarUCharArray &new_union_seq = new_attr_value.value.uchar_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_USHORT:
        {
            DevVarUShortArray &union_seq = attr_value.value.ushort_att_value();
            DevVarUShortArray tmp_seq(1);
            new_attr_value.value.ushort_att_value(tmp_seq);
            DevVarUShortArray &new_union_seq = new_attr_value.value.ushort_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_ULONG:
        {
            DevVarULongArray &union_seq = attr_value.value.ulong_att_value();
            DevVarULongArray tmp_seq(1);
            new_attr_value.value.ulong_att_value(tmp_seq);
            DevVarULongArray &new_union_seq = new_attr_value.value.ulong_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_ULONG64:
        {
            DevVarULong64Array &union_seq = attr_value.value.ulong64_att_value();
            DevVarULong64Array tmp_seq(1);
            new_attr_value.value.ulong64_att_value(tmp_seq);
            DevVarULong64Array &new_union_seq = new_attr_value.value.ulong64_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_STRING:
        {
            DevVarStringArray &union_seq = attr_value.value.string_att_value();
            DevVarStringArray tmp_seq(1);
            new_attr_value.value.string_att_value(tmp_seq);
            DevVarStringArray &new_union_seq = new_attr_value.value.string_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case ATT_STATE:
        {
            DevVarStateArray &union_seq = attr_value.value.state_att_value();
            DevVarStateArray tmp_seq(1);
            new_attr_value.value.state_att_value(tmp_seq);
            DevVarStateArray &new_union_seq = new_attr_value.value.state_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;

        case DEVICE_STATE:
        {
            DevState union_state = attr_value.value.dev_state_att();
            new_attr_value.value.dev_state_att(union_state);
        }
        break;

        case ATT_NO_DATA:
        {
            new_attr_value.value.union_no_data(true);
        }
        break;

        case ATT_ENCODED:
        {
            DevVarEncodedArray &union_seq = attr_value.value.encoded_att_value();
            DevVarEncodedArray tmp_seq(1);
            new_attr_value.value.encoded_att_value(tmp_seq);
            DevVarEncodedArray &new_union_seq = new_attr_value.value.encoded_att_value();
            unsigned long len = union_seq.length();
            new_union_seq.replace(len,len,union_seq.get_buffer(false),false);
        }
        break;
    }
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::copy_remaining
//
// description :
//      Copy from one AttributeValue_X structure to another one data elements which are not the data themselves
//
// argument :
//		in :
//			- attr_value : The origin attribute value
//      out :
//          - new_attr_value : The new attribute value
//
//------------------------------------------------------------------------------------------------------------------

template <typename T>
void PollThread::copy_remaining(T &old_attr_value,T &new_attr_value)
{
    new_attr_value.quality = old_attr_value.quality;
    new_attr_value.data_format = old_attr_value.data_format;
    new_attr_value.time = old_attr_value.time;
    new_attr_value.r_dim = old_attr_value.r_dim;
    new_attr_value.w_dim = old_attr_value.w_dim;
    new_attr_value.err_list = old_attr_value.err_list;

    char *tmp_ptr = old_attr_value.name._retn();
    new_attr_value.name = tmp_ptr;

    omni_mutex *tmp_mut = old_attr_value.get_attr_mutex();
    if (tmp_mut != NULL)
    {
        new_attr_value.set_attr_mutex(tmp_mut);
        old_attr_value.set_attr_mutex(NULL);
    }

}

} // End of Tango namespace

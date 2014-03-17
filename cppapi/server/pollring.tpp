static const char *RcsId = "$Id$";

//+==================================================================================================================
//
// file :               PollRing.tpp
//
// description :        C++ source code for the PollRing class template methods
//
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


namespace Tango
{

//-------------------------------------------------------------------------
//
// method : 		PollRing::force_copy_data
//
// description : 	Since IDL 4, attributes are transferred on the net using
//					a IDL union. In some cases, the sequence within the union simply
//					points to the user data (no copy), therefore, this method
//					force the user data to be copied
//
//					To do this copy we:
//					1 - Really copy the data within a temporary sequence
//					2 - Force memory freeing (if required by user) using the
//						union sequence replace call
//					3 - Transfer the data from the temporary sequence within the
//						union sequence using again the sequence replace call
//						in order not to trigger a real data copy
//
// argument : in : 	- attr_value_4 : The exception to be stored
//
//--------------------------------------------------------------------------

template <typename T>
void PollRing::force_copy_data(T *attr_value)
{
	for (unsigned long loop = 0;loop < attr_value->length();loop++)
	{
		switch ((*attr_value)[loop].value._d())
		{
			case ATT_BOOL:
			{
				DevVarBooleanArray &union_seq = (*attr_value)[loop].value.bool_att_value();
				DevVarBooleanArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_SHORT:
			{
				DevVarShortArray &union_seq = (*attr_value)[loop].value.short_att_value();
				DevVarShortArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_LONG:
			{
				DevVarLongArray &union_seq = (*attr_value)[loop].value.long_att_value();
				DevVarLongArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_LONG64:
			{
				DevVarLong64Array &union_seq = (*attr_value)[loop].value.long64_att_value();
				DevVarLong64Array tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_FLOAT:
			{
				DevVarFloatArray &union_seq = (*attr_value)[loop].value.float_att_value();
				DevVarFloatArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_DOUBLE:
			{
				DevVarDoubleArray &union_seq = (*attr_value)[loop].value.double_att_value();
				DevVarDoubleArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_UCHAR:
			{
				DevVarCharArray &union_seq = (*attr_value)[loop].value.uchar_att_value();
				DevVarCharArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_USHORT:
			{
				DevVarUShortArray &union_seq = (*attr_value)[loop].value.ushort_att_value();
				DevVarUShortArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_ULONG:
			{
				DevVarULongArray &union_seq = (*attr_value)[loop].value.ulong_att_value();
				DevVarULongArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_ULONG64:
			{
				DevVarULong64Array &union_seq = (*attr_value)[loop].value.ulong64_att_value();
				DevVarULong64Array tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case ATT_STRING:
			{
				const DevVarStringArray &union_seq = (*attr_value)[loop].value.string_att_value();
				DevVarStringArray tmp_seq = union_seq;
				(const_cast<DevVarStringArray &>(union_seq)).replace(0,0,NULL,true);
				(*attr_value)[loop].value.string_att_value(tmp_seq);
			}
			break;

			case ATT_STATE:
			{
				DevVarStateArray &union_seq = (*attr_value)[loop].value.state_att_value();
				DevVarStateArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);
				unsigned long len = tmp_seq.length();
				union_seq.replace(len,len,tmp_seq.get_buffer(true),true);
			}
			break;

			case DEVICE_STATE:
			case ATT_NO_DATA:
			break;

			case ATT_ENCODED:
			{
				DevVarEncodedArray &union_seq = (*attr_value)[loop].value.encoded_att_value();
				DevVarEncodedArray tmp_seq(union_seq);
				union_seq.replace(0,0,NULL,true);

				union_seq.length(tmp_seq.length());

				union_seq[0].encoded_format = CORBA::string_dup(tmp_seq[0].encoded_format);
				unsigned long nb_data = tmp_seq[0].encoded_data.length();
				union_seq[0].encoded_data.replace(nb_data,nb_data,tmp_seq[0].encoded_data.get_buffer(true),true);

				if (tmp_seq.length() == 2)
				{
					union_seq[1].encoded_format = CORBA::string_dup(tmp_seq[1].encoded_format);
					unsigned long nb_data = tmp_seq[1].encoded_data.length();
					union_seq[1].encoded_data.replace(nb_data,nb_data,tmp_seq[1].encoded_data.get_buffer(true),true);
				}
			}
			break;
		}
	}
}

} // End of Tango namespace


//+==================================================================================================================
//
// C++ source code file for TANGO api class ApiUtil template methods
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// original 	- May 2002
//
// $Revision: 25091 $
//
//+==================================================================================================================

namespace Tango
{

//-----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::attr_to_device_base()
//
// description :
//
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void ApiUtil::attr_to_device_base(const T *attr_value,DeviceAttribute *dev_attr)
{

	CORBA::Long *tmp_lo;
	CORBA::Short *tmp_sh;
	CORBA::Double *tmp_db;
	char **tmp_str;
	CORBA::Float *tmp_fl;
	CORBA::Boolean *tmp_boo;
	CORBA::UShort *tmp_ush;
	CORBA::Octet *tmp_uch;
	CORBA::LongLong *tmp_lolo;
	CORBA::ULong *tmp_ulo;
	CORBA::ULongLong *tmp_ulolo;
	Tango::DevState *tmp_state;
	Tango::DevEncoded *tmp_enc;

	CORBA::ULong max,len;

	dev_attr->name = attr_value->name;
	dev_attr->quality = attr_value->quality;
	dev_attr->data_format = attr_value->data_format;
	dev_attr->time = attr_value->time;
	dev_attr->dim_x = attr_value->r_dim.dim_x;
	dev_attr->dim_y = attr_value->r_dim.dim_y;
	dev_attr->set_w_dim_x(attr_value->w_dim.dim_x);
	dev_attr->set_w_dim_y(attr_value->w_dim.dim_y);
	dev_attr->err_list = new DevErrorList(attr_value->err_list);

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		switch (attr_value->value._d())
		{
			case ATT_BOOL:
			{
				const DevVarBooleanArray &tmp_seq = attr_value->value.bool_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_boo = (const_cast<DevVarBooleanArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
				}
				else
				{
					tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq.get_buffer());
					dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,false);
				}
			}
			break;

			case ATT_SHORT:
			{
				const DevVarShortArray &tmp_seq = attr_value->value.short_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_sh = (const_cast<DevVarShortArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
				}
				else
				{
					tmp_sh = const_cast<CORBA::Short *>(tmp_seq.get_buffer());
					dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
				}
			}
			break;

			case ATT_LONG:
			{
				const DevVarLongArray &tmp_seq = attr_value->value.long_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_lo = (const_cast<DevVarLongArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
				}
				else
				{
					tmp_lo = const_cast<CORBA::Long *>(tmp_seq.get_buffer());
					dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
				}
			}
			break;

			case ATT_LONG64:
			{
				const DevVarLong64Array &tmp_seq = attr_value->value.long64_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_lolo = (const_cast<DevVarLong64Array &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,true);
				}
				else
				{
					tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq.get_buffer());
					dev_attr->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,false);
				}
			}
			break;

			case ATT_FLOAT:
			{
				const DevVarFloatArray &tmp_seq = attr_value->value.float_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_fl = (const_cast<DevVarFloatArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
				}
				else
				{
					tmp_fl = const_cast<CORBA::Float *>(tmp_seq.get_buffer());
					dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,false);
				}
			}
			break;

			case ATT_DOUBLE:
			{
				const DevVarDoubleArray &tmp_seq = attr_value->value.double_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_db = (const_cast<DevVarDoubleArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
				}
				else
				{
					tmp_db = const_cast<CORBA::Double *>(tmp_seq.get_buffer());
					dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
				}
			}
			break;

			case ATT_UCHAR:
			{
				const DevVarCharArray &tmp_seq = attr_value->value.uchar_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_uch = (const_cast<DevVarCharArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
				}
				else
				{
					tmp_uch = const_cast<CORBA::Octet *>(tmp_seq.get_buffer());
					dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,false);
				}
			}
			break;

			case ATT_USHORT:
			{
				const DevVarUShortArray &tmp_seq = attr_value->value.ushort_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_ush = (const_cast<DevVarUShortArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
				}
				else
				{
					tmp_ush = const_cast<CORBA::UShort *>(tmp_seq.get_buffer());
					dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,false);
				}
			}
			break;

			case ATT_ULONG:
			{
				const DevVarULongArray &tmp_seq = attr_value->value.ulong_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_ulo = (const_cast<DevVarULongArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
				}
				else
				{
					tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq.get_buffer());
					dev_attr->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
				}
			}
			break;

			case ATT_ULONG64:
			{
				const DevVarULong64Array &tmp_seq = attr_value->value.ulong64_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_ulolo = (const_cast<DevVarULong64Array &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
				}
				else
				{
					tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq.get_buffer());
					dev_attr->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
				}
			}
			break;

			case ATT_STRING:
			{
				const DevVarStringArray &tmp_seq = attr_value->value.string_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_str = (const_cast<DevVarStringArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,true);
				}
				else
				{
					tmp_str = const_cast<char **>(tmp_seq.get_buffer());
					dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,false);
				}
			}
			break;

			case ATT_STATE:
			{
				const DevVarStateArray &tmp_seq = attr_value->value.state_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_state = (const_cast<DevVarStateArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
				}
				else
				{
					tmp_state = const_cast<Tango::DevState *>(tmp_seq.get_buffer());
					dev_attr->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
				}
			}
			break;

			case DEVICE_STATE:
			{
				dev_attr->d_state = attr_value->value.dev_state_att();
				dev_attr->d_state_filled = true;
			}
			break;

			case ATT_ENCODED:
			{
				const DevVarEncodedArray &tmp_seq = attr_value->value.encoded_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_enc = (const_cast<DevVarEncodedArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->EncodedSeq = new DevVarEncodedArray(max,len,tmp_enc,true);
				}
				else
				{
					tmp_enc = const_cast<Tango::DevEncoded *>(tmp_seq.get_buffer());
					dev_attr->EncodedSeq = new DevVarEncodedArray(max,len,tmp_enc,false);
				}
			}
			break;

			case ATT_NO_DATA:
			break;
		}
	}
}



} // End of tango namespace

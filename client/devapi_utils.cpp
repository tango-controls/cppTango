static const char *RcsId = "$Id$";

//
// devapi_utils.cpp 	- C++ source code file for TANGO device api
//
// programmer(s)	-Emmanuel Taurel(taurel@esrf.fr)
//
// original 		- November 2007
//
// Copyright (C) :      2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


using namespace CORBA;

namespace Tango
{


//-----------------------------------------------------------------------------
//
// DeviceProxy::from_hist4_2_AttHistory()
//
// Convert the attribute history as returned by a IDL 4 device to the classical
// DeviceAttributeHistory format
//
//-----------------------------------------------------------------------------

void DeviceProxy::from_hist4_2_AttHistory(DevAttrHistory_4_var &hist_4,vector<DeviceAttributeHistory> *ddh)
{

//
// Check received data validity
//

	if ((hist_4->quals.length() != hist_4->quals_array.length()) ||
		(hist_4->r_dims.length() != hist_4->r_dims_array.length()) ||
		(hist_4->w_dims.length() != hist_4->w_dims_array.length()) ||
		(hist_4->errors.length() != hist_4->errors_array.length()))
	{
		Tango::Except::throw_exception((const char *)"API_WrongHistoryDataBuffer",
									   (const char *)"Data buffer received from server is not valid !",
									   (const char *)"DeviceProxy::from_hist4_2_AttHistory");
	}

//
// Get history depth
//

	unsigned int h_depth = hist_4->dates.length();

//
// Copy date and name in each History list element
//

	unsigned int loop;
	for (loop = 0;loop < h_depth;loop++)
	{
		(*ddh)[loop].time = hist_4->dates[loop];
		(*ddh)[loop].name = hist_4->name.in();
	}

//
// Copy the attribute quality factor
//

	int k;

	for (loop = 0;loop < hist_4->quals.length();loop++)
	{
		int nb_elt = hist_4->quals_array[loop].nb_elt;
		int start = hist_4->quals_array[loop].start;

		for (k = 0;k < nb_elt;k++)
			(*ddh)[start - k].quality = hist_4->quals[loop];
	}

//
// Copy read dimension
//

	for (loop = 0;loop < hist_4->r_dims.length();loop++)
	{
		int nb_elt = hist_4->r_dims_array[loop].nb_elt;
		int start = hist_4->r_dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].dim_x = hist_4->r_dims[loop].dim_x;
			(*ddh)[start - k].dim_y = hist_4->r_dims[loop].dim_y;
		}
	}

//
// Copy write dimension
//

	for (loop = 0;loop < hist_4->w_dims.length();loop++)
	{
		int nb_elt = hist_4->w_dims_array[loop].nb_elt;
		int start = hist_4->w_dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].set_w_dim_x(hist_4->w_dims[loop].dim_x);
			(*ddh)[start - k].set_w_dim_y(hist_4->w_dims[loop].dim_y);
		}
	}

//
// Copy errors
//

	for (loop = 0;loop < hist_4->errors.length();loop++)
	{
		int nb_elt = hist_4->errors_array[loop].nb_elt;
		int start = hist_4->errors_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].failed(true);
			DevErrorList &err_list = (*ddh)[start - k].get_error_list();
			err_list.length(hist_4->errors[loop].length());
			for (unsigned int g = 0;g < hist_4->errors[loop].length();g++)
			{
				err_list[g] = (hist_4->errors[loop])[g];
			}
		}
	}

//
// Get data type and data ptr
//

	const Tango::DevVarDoubleArray *tmp_db;
	const Tango::DevVarShortArray *tmp_sh;
	const Tango::DevVarLongArray *tmp_lg;
	const Tango::DevVarLong64Array *tmp_lg64;
	const Tango::DevVarStringArray *tmp_str;
	const Tango::DevVarFloatArray *tmp_fl;
	const Tango::DevVarBooleanArray *tmp_boo;
	const Tango::DevVarUShortArray *tmp_ush;
	const Tango::DevVarCharArray *tmp_uch;
	const Tango::DevVarULongArray *tmp_ulg;
	const Tango::DevVarULong64Array *tmp_ulg64;
	const Tango::DevVarStateArray *tmp_state;
	const Tango::DevVarEncodedArray *tmp_enc;

	long data_type = -1;
	unsigned int seq_size = 0;

	CORBA::TypeCode_var ty = hist_4->value.type();
	if (ty->kind() != tk_null)
	{
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();

		switch (ty_seq->kind())
		{
			case tk_long:
			data_type = DEV_LONG;
			hist_4->value >>= tmp_lg;
			seq_size = tmp_lg->length();
			break;

			case tk_longlong:
			data_type = DEV_LONG64;
			hist_4->value >>= tmp_lg64;
			seq_size = tmp_lg64->length();
			break;

			case tk_short:
			data_type = DEV_SHORT;
			hist_4->value >>= tmp_sh;
			seq_size = tmp_sh->length();
			break;

			case tk_double:
			data_type = DEV_DOUBLE;
			hist_4->value >>= tmp_db;
			seq_size = tmp_db->length();
			break;

			case tk_string:
			data_type = DEV_STRING;
			hist_4->value >>= tmp_str;
			seq_size = tmp_str->length();
			break;

			case tk_float:
			data_type = DEV_FLOAT;
			hist_4->value >>= tmp_fl;
			seq_size = tmp_fl->length();
			break;

			case tk_boolean:
			data_type = DEV_BOOLEAN;
			hist_4->value >>= tmp_boo;
			seq_size = tmp_boo->length();
			break;

			case tk_ushort:
			data_type = DEV_USHORT;
			hist_4->value >>= tmp_ush;
			seq_size = tmp_ush->length();
			break;

			case tk_octet:
			data_type = DEV_UCHAR;
			hist_4->value >>= tmp_uch;
			seq_size = tmp_uch->length();
			break;

			case tk_ulong:
			data_type = DEV_ULONG;
			hist_4->value >>= tmp_ulg;
			seq_size = tmp_ulg->length();
			break;

			case tk_ulonglong:
			data_type = DEV_ULONG64;
			hist_4->value >>= tmp_ulg64;
			seq_size = tmp_ulg64->length();
			break;

			case tk_enum:
			data_type = DEV_STATE;
			hist_4->value >>= tmp_state;
			seq_size = tmp_state->length();
			break;

			case tk_struct:
			data_type = DEV_ENCODED;
			hist_4->value >>= tmp_enc;
			seq_size = tmp_enc->length();
			break;

			default:
			break;
		}
	}

//
// Copy data
//

	int base = seq_size;
	for (loop = 0;loop < h_depth;loop++)
	{

		if (((*ddh)[loop].failed() == true) || ((*ddh)[loop].quality == Tango::ATTR_INVALID))
			continue;

//
// Get the data length for this record
//

		int r_dim_x = (*ddh)[loop].dim_x;
		int r_dim_y = (*ddh)[loop].dim_y;
		int w_dim_x = (*ddh)[loop].get_written_dim_x();
		int w_dim_y = (*ddh)[loop].get_written_dim_y();

		int data_length;
		(r_dim_y == 0) ? data_length = r_dim_x : data_length = r_dim_x * r_dim_y;
		(w_dim_y == 0) ? data_length += w_dim_x : data_length += (w_dim_x * w_dim_y);

//
// Real copy now
//

		int ll = 0;
		switch (data_type)
		{
			case DEV_SHORT:
			(*ddh)[loop].ShortSeq = new DevVarShortArray();
			(*ddh)[loop].ShortSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ShortSeq[ll] = (*tmp_sh)[(base - data_length) + ll];
			break;

			case DEV_LONG:
			(*ddh)[loop].LongSeq = new DevVarLongArray();
			(*ddh)[loop].LongSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].LongSeq[ll] = (*tmp_lg)[(base - data_length) + ll];
			break;

			case DEV_LONG64:
			(*ddh)[loop].set_Long64_data(new DevVarLong64Array());
			(*ddh)[loop].get_Long64_data()->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				((*ddh)[loop].get_Long64_data())[ll] = (*tmp_lg64)[(base - data_length) + ll];
			break;

			case DEV_FLOAT:
			(*ddh)[loop].FloatSeq = new DevVarFloatArray();
			(*ddh)[loop].FloatSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].FloatSeq[ll] = (*tmp_fl)[(base - data_length) + ll];
			break;

			case DEV_DOUBLE:
			(*ddh)[loop].DoubleSeq = new DevVarDoubleArray();
			(*ddh)[loop].DoubleSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].DoubleSeq[ll] = (*tmp_db)[(base - data_length) + ll];
			break;

			case DEV_STRING:
			(*ddh)[loop].StringSeq = new DevVarStringArray();
			(*ddh)[loop].StringSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].StringSeq[ll] = (*tmp_str)[(base - data_length) + ll];
			break;

			case DEV_BOOLEAN:
			(*ddh)[loop].BooleanSeq = new DevVarBooleanArray();
			(*ddh)[loop].BooleanSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].BooleanSeq[ll] = (*tmp_boo)[(base - data_length) + ll];
			break;

			case DEV_USHORT:
			(*ddh)[loop].UShortSeq = new DevVarUShortArray();
			(*ddh)[loop].UShortSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].UShortSeq[ll] = (*tmp_ush)[(base - data_length) + ll];
			break;

			case DEV_UCHAR:
			(*ddh)[loop].UCharSeq = new DevVarUCharArray();
			(*ddh)[loop].UCharSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].UCharSeq[ll] = (*tmp_uch)[(base - data_length) + ll];
			break;

			case DEV_ULONG:
			(*ddh)[loop].set_ULong_data(new DevVarULongArray());
			(*ddh)[loop].get_ULong_data()->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				((*ddh)[loop].get_ULong_data())[ll] = (*tmp_ulg)[(base - data_length) + ll];
			break;

			case DEV_ULONG64:
			(*ddh)[loop].set_ULong64_data(new DevVarULong64Array());
			(*ddh)[loop].get_ULong64_data()->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				((*ddh)[loop].get_ULong64_data())[ll] = (*tmp_ulg64)[(base - data_length) + ll];
			break;

			case DEV_STATE:
			(*ddh)[loop].set_State_data(new DevVarStateArray());
			(*ddh)[loop].get_State_data()->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				((*ddh)[loop].get_State_data())[ll] = (*tmp_state)[(base - data_length) + ll];
			break;

			case DEV_ENCODED:
			(*ddh)[loop].set_Encoded_data(new DevVarEncodedArray());
			(*ddh)[loop].get_Encoded_data()->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				((*ddh)[loop].get_Encoded_data())[ll] = (*tmp_enc)[(base - data_length) + ll];
			break;
		}

		base = base - ll;

	}

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::from_hist4_2_DataHistory()
//
// Convert the command history as returned by a IDL 4 device to the classical
// DeviceDataHistory format
//
//-----------------------------------------------------------------------------

void DeviceProxy::from_hist4_2_DataHistory(DevCmdHistory_4_var &hist_4,vector<DeviceDataHistory> *ddh)
{

//
// Check received data validity
//

	if ((hist_4->dims.length() != hist_4->dims_array.length()) ||
		(hist_4->errors.length() != hist_4->errors_array.length()))
	{
		Tango::Except::throw_exception((const char *)"API_WrongHistoryDataBuffer",
									   (const char *)"Data buffer received from server is not valid !",
									   (const char *)"DeviceProxy::from_hist4_2_DataHistory");
	}

//
// Get history depth
//

	unsigned int h_depth = hist_4->dates.length();

//
// Copy date in each history list element
//

	unsigned int loop;
	int k;

	for (loop = 0;loop < h_depth;loop++)
	{
		(*ddh)[loop].set_date(hist_4->dates[loop]);
	}

//
// Copy errors
//

	for (loop = 0;loop < hist_4->errors.length();loop++)
	{
		int nb_elt = hist_4->errors_array[loop].nb_elt;
		int start = hist_4->errors_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].failed(true);
			DevErrorList_var del(&hist_4->errors[loop]);
			(*ddh)[start - k].errors(del);
			del._retn();
		}
	}

//
// Create a temporary sequence with record dimension
//

	Tango::AttributeDimList ad(h_depth);
	ad.length(h_depth);

	for (loop = 0;loop < hist_4->dims.length();loop++)
	{
		int nb_elt = hist_4->dims_array[loop].nb_elt;
		int start = hist_4->dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			ad[start - k].dim_x = hist_4->dims[loop].dim_x;
			ad[start - k].dim_y = hist_4->dims[loop].dim_y;
		}
	}

//
// Get data ptr and data size
//

	const Tango::DevVarDoubleArray *tmp_db;
	const Tango::DevVarShortArray *tmp_sh;
	const Tango::DevVarLongArray *tmp_lg;
	const Tango::DevVarLong64Array *tmp_lg64;
	const Tango::DevVarStringArray *tmp_str;
	const Tango::DevVarFloatArray *tmp_fl;
	const Tango::DevVarBooleanArray *tmp_boo;
	const Tango::DevVarUShortArray *tmp_ush;
	const Tango::DevVarCharArray *tmp_uch;
	const Tango::DevVarULongArray *tmp_ulg;
	const Tango::DevVarULong64Array *tmp_ulg64;
	const Tango::DevVarStateArray *tmp_state;
	const Tango::DevVarLongStringArray *tmp_lg_str;
	const Tango::DevVarDoubleStringArray *tmp_db_str;
	const Tango::DevVarEncodedArray *tmp_enc;

	unsigned int seq_size = 0;
	unsigned int seq_size_str = 0;
	unsigned int seq_size_num = 0;

	switch (hist_4->cmd_type)
	{
		case DEV_LONG:
		case DEVVAR_LONGARRAY:
		hist_4->value >>= tmp_lg;
		seq_size = tmp_lg->length();
		break;

		case DEV_LONG64:
		case DEVVAR_LONG64ARRAY:
		hist_4->value >>= tmp_lg64;
		seq_size = tmp_lg64->length();
		break;

		case DEV_SHORT:
		case DEVVAR_SHORTARRAY:
		hist_4->value >>= tmp_sh;
		seq_size = tmp_sh->length();
		break;

		case DEV_DOUBLE:
		case DEVVAR_DOUBLEARRAY:
		hist_4->value >>= tmp_db;
		seq_size = tmp_db->length();
		break;

		case DEV_STRING:
		case DEVVAR_STRINGARRAY:
		hist_4->value >>= tmp_str;
		seq_size = tmp_str->length();
		break;

		case DEV_FLOAT:
		case DEVVAR_FLOATARRAY:
		hist_4->value >>= tmp_fl;
		seq_size = tmp_fl->length();
		break;

		case DEV_BOOLEAN:
		hist_4->value >>= tmp_boo;
		seq_size = tmp_boo->length();
		break;

		case DEV_USHORT:
		case DEVVAR_USHORTARRAY:
		hist_4->value >>= tmp_ush;
		seq_size = tmp_ush->length();
		break;

		case DEVVAR_CHARARRAY:
		hist_4->value >>= tmp_uch;
		seq_size = tmp_uch->length();
		break;

		case DEV_ULONG:
		case DEVVAR_ULONGARRAY:
		hist_4->value >>= tmp_ulg;
		seq_size = tmp_ulg->length();
		break;

		case DEV_ULONG64:
		case DEVVAR_ULONG64ARRAY:
		hist_4->value >>= tmp_ulg64;
		seq_size = tmp_ulg64->length();
		break;

		case DEV_STATE:
		hist_4->value >>= tmp_state;
		seq_size = tmp_state->length();
		break;

		case DEVVAR_LONGSTRINGARRAY:
		hist_4->value >>= tmp_lg_str;
		seq_size_str = tmp_lg_str->svalue.length();
		seq_size_num = tmp_lg_str->lvalue.length();
		break;

		case DEVVAR_DOUBLESTRINGARRAY:
		hist_4->value >>= tmp_db_str;
		seq_size_str = tmp_db_str->svalue.length();
		seq_size_num = tmp_db_str->dvalue.length();
		break;

		case DEV_ENCODED:
		hist_4->value >>= tmp_enc;
		seq_size = tmp_enc->length();
		break;
	}

//
// Copy data
//

	int base = seq_size;
	int base_str = seq_size_str;
	int base_num = seq_size_num;

	for (loop = 0;loop < h_depth;loop++)
	{

		if ((*ddh)[loop].failed() == true)
			continue;

//
// Get the data length for this record
//

		int data_length = ad[loop].dim_x;
		int data_num_length = ad[loop].dim_y;

//
// Real copy now
//

		switch (hist_4->cmd_type)
		{
			case Tango::DEV_SHORT:
			{
				Tango::DevShort tmp_data = (*tmp_sh)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_SHORTARRAY:
			{
				const CORBA::Short *c_seq_buff = tmp_sh->get_buffer();
				CORBA::Short *seq_buff = const_cast<CORBA::Short*>(c_seq_buff);
				Tango::DevVarShortArray ShortSeq = DevVarShortArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= ShortSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_LONG:
			{
				Tango::DevLong tmp_data = (*tmp_lg)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_LONGARRAY:
			{
				const CORBA::Long *c_seq_buff = tmp_lg->get_buffer();
				CORBA::Long *seq_buff = const_cast<CORBA::Long*>(c_seq_buff);
				Tango::DevVarLongArray LongSeq = DevVarLongArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= LongSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_FLOAT:
			{
				Tango::DevFloat tmp_data = (*tmp_fl)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_FLOATARRAY:
			{
				const CORBA::Float *c_seq_buff = tmp_fl->get_buffer();
				CORBA::Float *seq_buff = const_cast<CORBA::Float*>(c_seq_buff);
				Tango::DevVarFloatArray FloatSeq = DevVarFloatArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= FloatSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_DOUBLE:
			{
				Tango::DevDouble tmp_data = (*tmp_db)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_DOUBLEARRAY:
			{
				const CORBA::Double *c_seq_buff = tmp_db->get_buffer();
				CORBA::Double *seq_buff = const_cast<CORBA::Double*>(c_seq_buff);
				Tango::DevVarDoubleArray DoubleSeq = DevVarDoubleArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= DoubleSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_LONG64:
			{
				Tango::DevLong64 tmp_data = (*tmp_lg64)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_LONG64ARRAY:
			{
				const CORBA::LongLong *c_seq_buff = tmp_lg64->get_buffer();
				CORBA::LongLong *seq_buff = const_cast<CORBA::LongLong*>(c_seq_buff);
				Tango::DevVarLong64Array Long64Seq = DevVarLong64Array(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= Long64Seq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_STRING:
			{
				Tango::ConstDevString tmp_data = (*tmp_str)[base - 1].in();
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_STRINGARRAY:
			{
				const Tango::ConstDevString *c_seq_buff = tmp_str->get_buffer();
				char **seq_buff = const_cast<char **>(c_seq_buff);
				Tango::DevVarStringArray StrSeq = DevVarStringArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= StrSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_BOOLEAN:
			{
				Tango::DevBoolean tmp_data = (*tmp_boo)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CORBA::Any::from_boolean(tmp_data);
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_USHORT:
			{
				Tango::DevUShort tmp_data = (*tmp_ush)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_USHORTARRAY:
			{
				const Tango::DevUShort *c_seq_buff = tmp_ush->get_buffer();
				Tango::DevUShort *seq_buff = const_cast<Tango::DevUShort*>(c_seq_buff);
				Tango::DevVarUShortArray UshSeq = DevVarUShortArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= UshSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_ULONG:
			{
				Tango::DevULong tmp_data = (*tmp_ulg)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_ULONGARRAY:
			{
				const Tango::DevULong *c_seq_buff = tmp_ulg->get_buffer();
				Tango::DevULong *seq_buff = const_cast<Tango::DevULong*>(c_seq_buff);
				Tango::DevVarULongArray UlgSeq = DevVarULongArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= UlgSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_ULONG64:
			{
				Tango::DevULong64 tmp_data = (*tmp_ulg64)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_ULONG64ARRAY:
			{
				const Tango::DevULong64 *c_seq_buff = tmp_ulg64->get_buffer();
				Tango::DevULong64 *seq_buff = const_cast<Tango::DevULong64*>(c_seq_buff);
				Tango::DevVarULong64Array Ulg64Seq = DevVarULong64Array(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= Ulg64Seq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_CHARARRAY:
			{
				const Tango::DevUChar *c_seq_buff = tmp_uch->get_buffer();
				Tango::DevUChar *seq_buff = const_cast<Tango::DevUChar*>(c_seq_buff);
				Tango::DevVarCharArray CharSeq = DevVarCharArray(data_length,data_length,&(seq_buff[base - data_length]),false);

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= CharSeq;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_STATE:
			{
				Tango::DevState tmp_data = (*tmp_state)[base - 1];
				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= tmp_data;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_LONGSTRINGARRAY:
			{
				Tango::DevVarLongStringArray *dvlsa = new Tango::DevVarLongStringArray();
				dvlsa->svalue.length(data_length);
				dvlsa->lvalue.length(data_num_length);

				int ll;
				for (ll = 0;ll < data_length;ll++)
					dvlsa->svalue[ll] = tmp_lg_str->svalue[(base_str - data_length) + ll];
				for (ll = 0;ll < data_num_length;ll++)
					dvlsa->lvalue[ll] = tmp_lg_str->lvalue[(base_num - data_num_length) + ll];

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= dvlsa;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEVVAR_DOUBLESTRINGARRAY:
			{
				Tango::DevVarDoubleStringArray *dvdsa = new Tango::DevVarDoubleStringArray();
				dvdsa->svalue.length(data_length);
				dvdsa->dvalue.length(data_num_length);

				int ll;
				for (ll = 0;ll < data_length;ll++)
					dvdsa->svalue[ll] = tmp_db_str->svalue[(base_str - data_length) + ll];
				for (ll = 0;ll < data_num_length;ll++)
					dvdsa->dvalue[ll] = tmp_db_str->dvalue[(base_num - data_num_length) + ll];

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= dvdsa;
				(*ddh)[loop].any = any_ptr;
			}
			break;

			case Tango::DEV_ENCODED:
			{
/*				Tango::DevEncoded &tmp_data = (*tmp_enc)[base - 1];

				Tango::DevEncoded EncSeq;
				EncSeq.encoded_format = CORBA::string_dup(tmp_data.encoded_format);
				unsigned int buffer_length = tmp_data.encoded_data.length();
				EncSeq.encoded_data.length(buffer_length);
				for (unsigned int ll = 0;ll << buffer_length;++ll)
					EncSeq.encoded_data[ll] = tmp_data.encoded_data[ll];*/

				CORBA::Any *any_ptr = new CORBA::Any();
				(*any_ptr) <<= (*tmp_enc)[base - 1];
				(*ddh)[loop].any = any_ptr;
			}
			break;
		}

		if ((hist_4->cmd_type == DEVVAR_LONGSTRINGARRAY) || (hist_4->cmd_type == DEVVAR_DOUBLESTRINGARRAY))
		{
			base_str = base_str - data_length;
			base_num = base_num - data_num_length;
		}
		else
			base = base - data_length;

	}
}

//-----------------------------------------------------------------------------
//
// Some operator method definition to make Python binding development easier
//
//-----------------------------------------------------------------------------

bool _DevCommandInfo::operator==(const _DevCommandInfo &dci)
{
	return cmd_tag == dci.cmd_tag && cmd_name == dci.cmd_name && in_type == dci.in_type && out_type == dci.out_type;
}

bool _CommandInfo::operator==(const _CommandInfo &ci)
{
	return _DevCommandInfo::operator==(ci) && disp_level == ci.disp_level;
}

bool _DeviceAttributeConfig::operator==(const _DeviceAttributeConfig &dac)
{
	  return name == dac.name &&
	         writable == dac.writable &&
	         data_format == dac.data_format &&
	         data_type == dac.data_type &&
	         max_dim_x == dac.max_dim_x &&
	         max_dim_y == dac.max_dim_y &&
	         description == dac.description &&
	         label == dac.label &&
	         unit == dac.unit &&
	         standard_unit == dac.standard_unit &&
	         display_unit == dac.display_unit &&
	         format == dac.format &&
	         min_value == dac.min_value &&
	         max_value == dac.max_value &&
	         min_alarm == dac.min_alarm &&
	         max_alarm == dac.max_alarm &&
	         writable_attr_name == dac.writable_attr_name &&
	         extensions == dac.extensions;
}

bool _AttributeInfo::operator==(const _AttributeInfo &ai)
{
	return DeviceAttributeConfig::operator==(ai) && disp_level == ai.disp_level;
}

bool _AttributeInfoEx::operator==(const _AttributeInfoEx &aie)
{
	return AttributeInfo::operator==(aie) && sys_extensions == aie.sys_extensions;
}


} // End of namespace

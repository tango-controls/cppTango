
//+==================================================================================================================
// devapi_utils.cpp 	- C++ source code file for TANGO device api
//
// programmer(s)	- Emmanuel Taurel(taurel@esrf.fr)
//
// original 		- March 2014
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
// GNU General Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision: 25126 $
//
//+==================================================================================================================


using namespace CORBA;

namespace Tango
{


//------------------------------------------------------------------------------------------------------------------
//
// method :
// 		DeviceProxy::from_hist_2_AttHistory()
//
// description :
// 		Convert the attribute history as returned by a IDL 4 device to the classical DeviceAttributeHistory format
//
//-------------------------------------------------------------------------------------------------------------------

template <typename T>
void DeviceProxy::from_hist_2_AttHistory(T &hist,std::vector<DeviceAttributeHistory> *ddh)
{

//
// Check received data validity
//

	if ((hist->quals.length() != hist->quals_array.length()) ||
		(hist->r_dims.length() != hist->r_dims_array.length()) ||
		(hist->w_dims.length() != hist->w_dims_array.length()) ||
		(hist->errors.length() != hist->errors_array.length()))
	{
		TANGO_THROW_EXCEPTION(API_WrongHistoryDataBuffer, "Data buffer received from server is not valid !");
	}

//
// Get history depth
//

	unsigned int h_depth = hist->dates.length();

//
// Copy date and name in each history list element
//

	unsigned int loop;
	for (loop = 0;loop < h_depth;loop++)
	{
		(*ddh)[loop].time = hist->dates[loop];
		(*ddh)[loop].name = hist->name.in();
	}

//
// Copy the attribute quality factor
//

	int k;

	for (loop = 0;loop < hist->quals.length();loop++)
	{
		int nb_elt = hist->quals_array[loop].nb_elt;
		int start = hist->quals_array[loop].start;

		for (k = 0;k < nb_elt;k++)
			(*ddh)[start - k].quality = hist->quals[loop];
	}

//
// Copy read dimension
//

	for (loop = 0;loop < hist->r_dims.length();loop++)
	{
		int nb_elt = hist->r_dims_array[loop].nb_elt;
		int start = hist->r_dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].dim_x = hist->r_dims[loop].dim_x;
			(*ddh)[start - k].dim_y = hist->r_dims[loop].dim_y;
		}
	}

//
// Copy write dimension
//

	for (loop = 0;loop < hist->w_dims.length();loop++)
	{
		int nb_elt = hist->w_dims_array[loop].nb_elt;
		int start = hist->w_dims_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].set_w_dim_x(hist->w_dims[loop].dim_x);
			(*ddh)[start - k].set_w_dim_y(hist->w_dims[loop].dim_y);
		}
	}

//
// Copy errors
//

	for (loop = 0;loop < hist->errors.length();loop++)
	{
		int nb_elt = hist->errors_array[loop].nb_elt;
		int start = hist->errors_array[loop].start;

		for (k = 0;k < nb_elt;k++)
		{
			(*ddh)[start - k].failed(true);
			DevErrorList &err_list = (*ddh)[start - k].get_error_list();
			err_list.length(hist->errors[loop].length());
			for (unsigned int g = 0;g < hist->errors[loop].length();g++)
			{
				err_list[g] = (hist->errors[loop])[g];
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

	CORBA::TypeCode_var ty = hist->value.type();
	if (ty->kind() != tk_null)
	{
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();

		switch (ty_seq->kind())
		{
			case tk_long:
			data_type = DEV_LONG;
			hist->value >>= tmp_lg;
			seq_size = tmp_lg->length();
			break;

			case tk_longlong:
			data_type = DEV_LONG64;
			hist->value >>= tmp_lg64;
			seq_size = tmp_lg64->length();
			break;

			case tk_short:
			data_type = DEV_SHORT;
			hist->value >>= tmp_sh;
			seq_size = tmp_sh->length();
			break;

			case tk_double:
			data_type = DEV_DOUBLE;
			hist->value >>= tmp_db;
			seq_size = tmp_db->length();
			break;

			case tk_string:
			data_type = DEV_STRING;
			hist->value >>= tmp_str;
			seq_size = tmp_str->length();
			break;

			case tk_float:
			data_type = DEV_FLOAT;
			hist->value >>= tmp_fl;
			seq_size = tmp_fl->length();
			break;

			case tk_boolean:
			data_type = DEV_BOOLEAN;
			hist->value >>= tmp_boo;
			seq_size = tmp_boo->length();
			break;

			case tk_ushort:
			data_type = DEV_USHORT;
			hist->value >>= tmp_ush;
			seq_size = tmp_ush->length();
			break;

			case tk_octet:
			data_type = DEV_UCHAR;
			hist->value >>= tmp_uch;
			seq_size = tmp_uch->length();
			break;

			case tk_ulong:
			data_type = DEV_ULONG;
			hist->value >>= tmp_ulg;
			seq_size = tmp_ulg->length();
			break;

			case tk_ulonglong:
			data_type = DEV_ULONG64;
			hist->value >>= tmp_ulg64;
			seq_size = tmp_ulg64->length();
			break;

			case tk_enum:
			data_type = DEV_STATE;
			hist->value >>= tmp_state;
			seq_size = tmp_state->length();
			break;

			case tk_struct:
			data_type = DEV_ENCODED;
			hist->value >>= tmp_enc;
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
			(*ddh)[loop].Long64Seq = new DevVarLong64Array();
			(*ddh)[loop].Long64Seq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].Long64Seq[ll] = (*tmp_lg64)[(base - data_length) + ll];
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
			(*ddh)[loop].ULongSeq = new DevVarULongArray();
			(*ddh)[loop].ULongSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ULongSeq[ll] = (*tmp_ulg)[(base - data_length) + ll];
			break;

			case DEV_ULONG64:
			(*ddh)[loop].ULong64Seq = new DevVarULong64Array();
			(*ddh)[loop].ULong64Seq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].ULong64Seq[ll] = (*tmp_ulg64)[(base - data_length) + ll];
			break;

			case DEV_STATE:
			(*ddh)[loop].StateSeq = new DevVarStateArray();
			(*ddh)[loop].StateSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].StateSeq[ll] = (*tmp_state)[(base - data_length) + ll];
			break;

			case DEV_ENCODED:
			(*ddh)[loop].EncodedSeq = new DevVarEncodedArray();
			(*ddh)[loop].EncodedSeq->length(data_length);

			for (ll = 0;ll < data_length;ll++)
				(*ddh)[loop].EncodedSeq[ll] = (*tmp_enc)[(base - data_length) + ll];
			break;
		}

		base = base - ll;

	}

}

} // End of namespace

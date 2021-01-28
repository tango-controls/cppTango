//+============================================================================
//
// file :               PollRing.cpp
//
// description :        C++ source code for the RingElt and PollRing
//			classes. These classes are used to implement the
//			tango polling ring buffer. There is one
//			polling ring for each Tango command or attribute
//			polled.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <pollring.h>
#include <pollring.tpp>


#ifdef _TG_WINDOWS_
#include <sys/types.h>
#endif /* _TG_WINDOWS_ */

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		RingElt::RingElt
//
// description : 	Constructor for the RingElt class.
//			This constructor simply set the internal value to their
//			default
//
//--------------------------------------------------------------------------

RingElt::RingElt()
{
	when = {};
	cmd_result = NULL;
	attr_value = NULL;
	attr_value_3 = NULL;
	attr_value_4 = NULL;
	attr_value_5 = NULL;
	except = NULL;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::PollRing
//
// description : 	Two constructors for the PollRing class. The first one
//			does not take any argument and construct a black box
//			with the default depth.
//			The second one create a black box with a depth defined
//			by the argument.
//
// argument : in : 	- max_size : The black box depth
//
//--------------------------------------------------------------------------

PollRing::PollRing():ring(DefaultPollRingDepth)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = DefaultPollRingDepth;
}

PollRing::PollRing(long max_size):ring(max_size)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = max_size;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::~PollRing
//
// description : 	The class destructor. It frees all the memory allocated
//			to store command/attribute result
//
//--------------------------------------------------------------------------

PollRing::~PollRing()
{
	cout4 << "In PollRing destructor" << std::endl;

	long nb = ring.size();

	for (long i = 0;i < nb;i++)
	{
		delete ring[i].cmd_result;
		delete ring[i].except;
		delete ring[i].attr_value;
		delete ring[i].attr_value_3;
		delete ring[i].attr_value_4;
		delete ring[i].attr_value_5;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::insert_data
//
// description : 	These methods insert a new element in the ring buffer
//			when its real data
//
// argument : in : 	- any_ptr : The Any returned by the command
//			- t : The date
//
//--------------------------------------------------------------------------


void PollRing::insert_data(CORBA::Any *any_ptr, PollClock::time_point t)
{

//
// Insert data in the ring
//

	delete(ring[insert_elt].cmd_result);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;

	ring[insert_elt].cmd_result = any_ptr;
	ring[insert_elt].when = t;

//
// Manage insert and read indexes
//

	inc_indexes();
}

void PollRing::insert_data(Tango::AttributeValueList *attr_val, PollClock::time_point t)
{

//
// Insert data in the ring
//

	delete(ring[insert_elt].attr_value);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;

	ring[insert_elt].attr_value = attr_val;
	ring[insert_elt].when = t;

//
// Manage insert and read indexes
//

	inc_indexes();
}

void PollRing::insert_data(Tango::AttributeValueList_3 *attr_val, PollClock::time_point t)
{

//
// Insert data in the ring
//

	delete(ring[insert_elt].attr_value_3);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;

	ring[insert_elt].attr_value_3 = attr_val;
	ring[insert_elt].when = t;

//
// Manage insert and read indexes
//

	inc_indexes();
}

void PollRing::insert_data(Tango::AttributeValueList_4 *attr_val, PollClock::time_point t, bool unlock)
{

//
// Insert data in the ring
//

	delete(ring[insert_elt].attr_value_4);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;

	ring[insert_elt].attr_value_4 = attr_val;
	ring[insert_elt].when = t;

	force_copy_data(ring[insert_elt].attr_value_4);

//
// Release attribute mutexes because the data are now copied
//

	if (unlock == true)
	{
		for (unsigned int loop = 0;loop < attr_val->length();loop++)
		{
			Tango::AttributeValue_4 *tmp_ptr = &((*attr_val)[loop]);
			(tmp_ptr)->rel_attr_mutex();
		}
	}

//
// Manage insert and read indexes
//

	inc_indexes();
}

void PollRing::insert_data(Tango::AttributeValueList_5 *attr_val, PollClock::time_point t, bool unlock)
{

//
// Insert data in the ring
//

	delete(ring[insert_elt].attr_value_5);
	delete(ring[insert_elt].except);
	ring[insert_elt].except = NULL;

	ring[insert_elt].attr_value_5 = attr_val;
	ring[insert_elt].when = t;

	force_copy_data(ring[insert_elt].attr_value_5);

//
// Release attribute mutexes because the data are now copied
//

	if (unlock == true)
	{
		for (unsigned int loop = 0;loop < attr_val->length();loop++)
		{
			Tango::AttributeValue_5 *tmp_ptr = &((*attr_val)[loop]);
			(tmp_ptr)->rel_attr_mutex();
		}
	}

//
// Manage insert and read indexes
//

	inc_indexes();
}

//-------------------------------------------------------------------------
//
// method : 		PollRing::insert_except
//
// description : 	This method insert a new element in the ring buffer
//			when this element is an exception
//
// argument : in : 	- ex : The exception to be stored
//			- t : The date
//
//--------------------------------------------------------------------------


void PollRing::insert_except(Tango::DevFailed *ex, PollClock::time_point t)
{

//
// Insert data in the ring
//

	delete(ring[insert_elt].except);
	if (ring[insert_elt].attr_value != NULL)
	{
		delete(ring[insert_elt].attr_value);
		ring[insert_elt].attr_value = NULL;
	}
	if (ring[insert_elt].cmd_result != NULL)
	{
		delete(ring[insert_elt].cmd_result);
		ring[insert_elt].cmd_result = NULL;
	}

	ring[insert_elt].except = ex;
	ring[insert_elt].when = t;

//
// Manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::inc_indexes
//
// description : 	This private method increment the indexes used to acces
//			the box itself. This is necessary because the box must
//			be managed as a circular buffer
//
//--------------------------------------------------------------------------


void PollRing::inc_indexes()
{
	insert_elt++;
	if (insert_elt == max_elt)
		insert_elt = 0;

	if (nb_elt != max_elt)
		nb_elt++;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_delta_t
//
// description : 	This method computes the delta time between records
//			in the ring buffer
//
// argument : 	in :	- nb : The number of delta t to be computed
//		out :	- res : A reference to the vector where result should
//				be stored
//
//--------------------------------------------------------------------------

std::vector<PollClock::duration> PollRing::get_delta_t(long nb)
{
    std::vector<PollClock::duration> res{};

//
// Throw exception if nothing in ring
//

	if (nb_elt < 2)
	{
		Except::throw_exception((const char *)API_PollRingBufferEmpty,
				        (const char *)"Not enough data stored yet in polling ring buffer",
				        (const char *)"PollRing::get_delta_t");
	}

//
// Get the buffer starting point
//

	long read_index,prev_read;
	if (insert_elt == 0)
	{
		read_index = max_elt - 1;
		prev_read = read_index - 1;
	}
	else
	{
		read_index = insert_elt - 1;
		if (read_index == 0)
			prev_read = max_elt - 1;
		else
			prev_read = read_index - 1;
	}

//
// Compute how many delta can be computed
//

	if (nb_elt <= nb)
		nb = nb_elt - 1;

//
// The delta t computing loop
//

	long i;
	for (i = 0;i < nb;i++)
	{
		auto t_ref = ring[read_index].when;
		auto t_prev = ring[prev_read].when;

		res.push_back(t_ref - t_prev);
		prev_read--;
		if (prev_read < 0)
			prev_read = max_elt - 1;
		read_index--;
		if (read_index < 0)
			read_index = max_elt - 1;
	}

	return res;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_insert_date
//
// description : 	This method returns the date of the last insert in the
//			ring buffer
//
//--------------------------------------------------------------------------

PollClock::time_point PollRing::get_last_insert_date()
{
	if (insert_elt == 0)
		return ring[max_elt - 1].when;
	else
		return ring[insert_elt - 1].when;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::is_last_an_error
//
// description : 	This method returns a boolean set to true if the last
//			data recorded into the ring buffer was an exception
//
//--------------------------------------------------------------------------

bool PollRing::is_last_an_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
			return false;
		else
			return true;
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
			return false;
		else
			return true;
	}
}

bool PollRing::is_last_cmd_an_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except != NULL)
			return true;
		else
			return false;
	}
	else
	{
		if (ring[insert_elt - 1].except != NULL)
			return true;
		else
			return false;
	}
}

bool PollRing::is_last_attr_an_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except != NULL)
			return true;
		else
		{
			if (ring[max_elt - 1].attr_value_3 != NULL)
			{
				if ((*(ring[max_elt - 1].attr_value_3))[0].err_list.length() != 0)
					return true;
				else
					return false;
			}
			else if (ring[max_elt - 1].attr_value_4 != NULL)
			{
				if ((*(ring[max_elt - 1].attr_value_4))[0].err_list.length() != 0)
					return true;
				else
					return false;
			}
			else
			{
				if ((*(ring[max_elt - 1].attr_value_5))[0].err_list.length() != 0)
					return true;
				else
					return false;
			}
		}
	}
	else
	{
		if (ring[insert_elt - 1].except != NULL)
			return true;
		else
		{
			if (ring[insert_elt - 1].attr_value_3 != NULL)
			{
				if ((*(ring[insert_elt - 1].attr_value_3))[0].err_list.length() != 0)
					return true;
				else
					return false;
			}
			else if (ring[insert_elt - 1].attr_value_4 != NULL)
			{
				if ((*(ring[insert_elt - 1].attr_value_4))[0].err_list.length() != 0)
					return true;
				else
					return false;
			}
			else
			{
				if ((*(ring[insert_elt - 1].attr_value_5))[0].err_list.length() != 0)
					return true;
				else
					return false;
			}
		}
	}
}


//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_except
//
// description : 	This method returns the exception recently stored in
//			the ring buffer.
//
//--------------------------------------------------------------------------

Tango::DevFailed *PollRing::get_last_except()
{
	if (insert_elt == 0)
		return ring[max_elt - 1].except;
	else
		return ring[insert_elt - 1].except;
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_attr_error
//
// description : 	This method returns the error stack for polled
//			attribute belonging to device implementing IDL
//			release 3 and more.
//
//--------------------------------------------------------------------------

Tango::DevErrorList &PollRing::get_last_attr_error()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].attr_value_3 != NULL)
			return (*(ring[max_elt - 1].attr_value_3))[0].err_list;
		else if (ring[max_elt - 1].attr_value_4 != NULL)
			return (*(ring[max_elt - 1].attr_value_4))[0].err_list;
		else
			return (*(ring[max_elt - 1].attr_value_5))[0].err_list;
	}
	else
	{
		if (ring[insert_elt - 1].attr_value_3 != NULL)
			return (*(ring[insert_elt - 1].attr_value_3))[0].err_list;
		else if (ring[insert_elt - 1].attr_value_4 != NULL)
			return (*(ring[insert_elt - 1].attr_value_4))[0].err_list;
		else
			return (*(ring[insert_elt - 1].attr_value_5))[0].err_list;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_cmd_result
//
// description : 	This method returns the exception recently stored in
//			the ring buffer.
//
//--------------------------------------------------------------------------

CORBA::Any *PollRing::get_last_cmd_result()
{
	CORBA::Any *tmp_any;

	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			tmp_any = ring[max_elt - 1].cmd_result;
			return new CORBA::Any(tmp_any->type(),
					      const_cast<void *>(tmp_any->value()),
					      false);
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			tmp_any = ring[insert_elt - 1].cmd_result;
			return new CORBA::Any(tmp_any->type(),
					      const_cast<void *>(tmp_any->value()),
					      false);
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}

}

//+-------------------------------------------------------------------------
//
// method : 		PollRing::get_last_attr_value
//
// description : 	This method returns the exception recently stored in
//			the ring buffer.
//
//--------------------------------------------------------------------------

Tango::AttributeValue &PollRing::get_last_attr_value()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			return (*(ring[max_elt - 1].attr_value))[0];
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			return (*(ring[insert_elt - 1].attr_value))[0];
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}

}

Tango::AttributeValue_3 &PollRing::get_last_attr_value_3()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			return (*(ring[max_elt - 1].attr_value_3))[0];
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			return (*(ring[insert_elt - 1].attr_value_3))[0];
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}

}

Tango::AttributeValue_4 &PollRing::get_last_attr_value_4()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			return (*(ring[max_elt - 1].attr_value_4))[0];
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			return (*(ring[insert_elt - 1].attr_value_4))[0];
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}

}

Tango::AttributeValue_5 &PollRing::get_last_attr_value_5()
{
	if (insert_elt == 0)
	{
		if (ring[max_elt - 1].except == NULL)
		{
			return (*(ring[max_elt - 1].attr_value_5))[0];
		}
		else
			throw Tango::DevFailed(*(ring[max_elt - 1].except));
	}
	else
	{
		if (ring[insert_elt - 1].except == NULL)
		{
			return (*(ring[insert_elt - 1].attr_value_5))[0];
		}
		else
			throw Tango::DevFailed(*(ring[insert_elt - 1].except));
	}

}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_cmd_history
//
// description : 	This method get command history from the ring buffer
//
// argument : in :	- n : record number
//			- ptr : Pointer to the sequence where command result
//				should be stored
//
//--------------------------------------------------------------------------

void PollRing::get_cmd_history(long n,Tango::DevCmdHistoryList *ptr)
{
	long i;

//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;

	long seq_index = n - 1;

//
// Read buffer
//

	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].time = make_TimeVal(ring[index].when);

		if (ring[index].except == NULL)
		{
			(*ptr)[seq_index].cmd_failed = false;
			(*ptr)[seq_index].value = *(ring[index].cmd_result);
			(*ptr)[seq_index].errors.length(0);
		}
		else
		{
			(*ptr)[seq_index].cmd_failed = true;
			(*ptr)[seq_index].errors = ring[index].except->errors;
		}

		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}

}

void PollRing::get_cmd_history(long n,Tango::DevCmdHistory_4 *ptr,Tango::CmdArgType &cmd_type)
{
	long i;

//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;

	long seq_index = n - 1;

	const Tango::DevVarCharArray *tmp_uch;
	const Tango::DevVarShortArray *tmp_sh;
	const Tango::DevVarLongArray *tmp_lg;
	const Tango::DevVarFloatArray *tmp_fl;
	const Tango::DevVarDoubleArray *tmp_db;
	const Tango::DevVarUShortArray *tmp_ush;
	const Tango::DevVarULongArray *tmp_ulg;
	const Tango::DevVarStringArray *tmp_str;
	const Tango::DevVarBooleanArray *tmp_boo;
	const Tango::DevVarLong64Array *tmp_lg64;
	const Tango::DevVarULong64Array *tmp_ulg64;
	const Tango::DevVarLongStringArray *tmp_lg_str;
	const Tango::DevVarDoubleStringArray *tmp_db_str;

//
// Compute the size of the global sequence
// We have two cases:
// 1 - For simple types where each record has a constant size
// 2 - For array types where each record may have different size
//

	long seq_size = 0;
	long seq_str_size = 0;
	long seq_number_size = 0;
	long error_nb = 0;

	if ((cmd_type <= DEV_STRING) || (cmd_type == DEV_STATE) || (cmd_type == CONST_DEV_STRING) ||
		(cmd_type == DEV_UCHAR) || (cmd_type == DEV_LONG64) || (cmd_type == DEV_ULONG64) ||
		(cmd_type == DEV_ENCODED))
	{
		for (i = 0;i < n;i++)
		{
			if (ring[index].except != NULL)
				error_nb++;

			if (index == 0)
				index = max_elt;
			index--;
		}
		seq_size = n - error_nb;
	}
	else
	{
		for (i = 0;i < n;i++)
		{
			if (ring[index].except == NULL)
			{
				int data_length = 0;

				switch(cmd_type)
				{
					case DEVVAR_CHARARRAY:
					(*ring[index].cmd_result) >>= tmp_uch;
					data_length = tmp_uch->length();
					break;

					case DEVVAR_SHORTARRAY:
					(*ring[index].cmd_result) >>= tmp_sh;
					data_length = tmp_sh->length();
					break;

					case DEVVAR_LONGARRAY:
					(*ring[index].cmd_result) >>= tmp_lg;
					data_length = tmp_lg->length();
					break;

					case DEVVAR_FLOATARRAY:
					(*ring[index].cmd_result) >>= tmp_fl;
					data_length = tmp_fl->length();
					break;

					case DEVVAR_DOUBLEARRAY:
					(*ring[index].cmd_result) >>= tmp_db;
					data_length = tmp_db->length();
					break;

					case DEVVAR_USHORTARRAY:
					(*ring[index].cmd_result) >>= tmp_ush;
					data_length = tmp_ush->length();
					break;

					case DEVVAR_ULONGARRAY:
					(*ring[index].cmd_result) >>= tmp_ulg;
					data_length = tmp_ulg->length();
					break;

					case DEVVAR_STRINGARRAY:
					(*ring[index].cmd_result) >>= tmp_str;
					data_length = tmp_str->length();
					break;

					case DEVVAR_BOOLEANARRAY:
					(*ring[index].cmd_result) >>= tmp_boo;
					data_length = tmp_boo->length();
					break;

					case DEVVAR_LONG64ARRAY:
					(*ring[index].cmd_result) >>= tmp_lg64;
					data_length = tmp_lg64->length();
					break;

					case DEVVAR_ULONG64ARRAY:
					(*ring[index].cmd_result) >>= tmp_ulg64;
					data_length = tmp_ulg64->length();
					break;

					case DEVVAR_LONGSTRINGARRAY:
					(*ring[index].cmd_result) >>= tmp_lg_str;
					seq_str_size = seq_str_size + tmp_lg_str->svalue.length();
					seq_number_size = seq_number_size + tmp_lg_str->lvalue.length();
					break;

					case DEVVAR_DOUBLESTRINGARRAY:
					(*ring[index].cmd_result) >>= tmp_db_str;
					seq_str_size = seq_str_size + tmp_db_str->svalue.length();
					seq_number_size = seq_number_size + tmp_db_str->dvalue.length();
					break;

					default:
					break;
				}
				seq_size = seq_size + data_length;
			}
			else
				error_nb++;

			if (index == 0)
				index = max_elt;
			index--;
		}
	}

//
// First, copy command output data type
//

	ptr->cmd_type = cmd_type;

//
// Read buffer
//

	Tango::DevVarCharArray *new_tmp_uch = NULL;
	Tango::DevVarShortArray *new_tmp_sh = NULL;
	Tango::DevVarLongArray *new_tmp_lg = NULL;
	Tango::DevVarFloatArray *new_tmp_fl = NULL;
	Tango::DevVarDoubleArray *new_tmp_db = NULL;
	Tango::DevVarUShortArray *new_tmp_ush = NULL;
	Tango::DevVarULongArray *new_tmp_ulg = NULL;
	Tango::DevVarStringArray *new_tmp_str = NULL;
	Tango::DevVarBooleanArray *new_tmp_boo = NULL;
	Tango::DevVarLong64Array *new_tmp_lg64 = NULL;
	Tango::DevVarULong64Array *new_tmp_ulg64 = NULL;
	Tango::DevVarLongStringArray *new_tmp_lg_str = NULL;
	Tango::DevVarDoubleStringArray *new_tmp_db_str = NULL;
	Tango::DevVarEncodedArray *new_tmp_enc = NULL;

	Tango::DevShort sh;
	Tango::DevBoolean boo;
	Tango::DevLong lg;
	Tango::DevLong64 lg64;
	Tango::DevFloat fl;
	Tango::DevDouble db;
	Tango::DevUShort ush;
	Tango::DevULong ulg;
	Tango::DevULong64 ulg64;
	Tango::ConstDevString str;
	const Tango::DevEncoded *enc;

	DevErrorList last_err_list;
	int errors_length = 1;
	AttributeDim last_dim;
	int dims_length = 1;
	last_dim.dim_x = -1;
	last_dim.dim_y = -1;

	bool previous_no_data = true;
	bool no_data = true;
	unsigned int ind_in_seq = 0;
	unsigned int ind_str_in_seq = 0;
	unsigned int ind_num_in_seq = 0;

	index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;

	for (i = 0;i < n;i++)
	{
		previous_no_data = no_data;
		no_data = false;

//
// Copy dates
//

		ptr->dates[seq_index] = make_TimeVal(ring[index].when);

//
// Error treatement
//

		if (ring[index].except != NULL)
		{
			bool new_err = false;

			if (previous_no_data == true)
			{
				if (ring[index].except->errors.length() != last_err_list.length())
					new_err = true;
				else
				{
					for (unsigned int k = 0;k < last_err_list.length();k++)
					{
						if (::strcmp(ring[index].except->errors[k].reason.in(),last_err_list[k].reason.in()) != 0)
						{
							new_err = true;
							break;
						}
						if (::strcmp(ring[index].except->errors[k].desc.in(),last_err_list[k].desc.in()) != 0)
						{
							new_err = true;
							break;
						}
						if (::strcmp(ring[index].except->errors[k].origin.in(),last_err_list[k].origin.in()) != 0)
						{
							new_err = true;
							break;
						}
						if (ring[index].except->errors[k].severity != last_err_list[k].severity)
						{
							new_err = true;
							break;
						}
					}
				}
			}
			else
				new_err = true;

			if (new_err == true)
			{
				if (ptr->errors.length() == 0)
					ptr->errors.length(error_nb);
				if (ptr->errors_array.length() == 0)
					ptr->errors_array.length(error_nb);
				last_err_list = ring[index].except->errors;
				ptr->errors[errors_length - 1] = last_err_list;
				ptr->errors_array[errors_length - 1].start = n - (i + 1);
				ptr->errors_array[errors_length - 1].nb_elt = 1;
				errors_length++;
				previous_no_data = no_data;
				no_data = true;
			}
			else
			{
				ptr->errors_array[errors_length - 2].nb_elt++;
				previous_no_data = no_data;
				no_data = true;
			}

			if ((last_dim.dim_x == 0) &&
				(last_dim.dim_y == 0))
			{
				ptr->dims_array[dims_length - 2].nb_elt++;
			}
			else
			{
				last_dim.dim_x = last_dim.dim_y = 0;
				ptr->dims.length(dims_length);
				ptr->dims[dims_length - 1] = last_dim;
				ptr->dims_array.length(dims_length);
				ptr->dims_array[dims_length - 1].start = n - (i + 1);
				ptr->dims_array[dims_length - 1].nb_elt = 1;
				dims_length++;
			}
		}

//
// The data
//

		if (ring[index].except == NULL)
		{
			switch(cmd_type)
			{
			case DEVVAR_CHARARRAY:
				if (new_tmp_uch == NULL)
				{
					new_tmp_uch = new DevVarCharArray();
					new_tmp_uch->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_uch;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_uch,tmp_uch,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_uch->length());
				break;

			case DEV_SHORT:
				if (new_tmp_sh == NULL)
				{
					new_tmp_sh = new DevVarShortArray();
					new_tmp_sh->length(seq_size);
				}
				(*ring[index].cmd_result) >>= sh;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_sh,sh,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_SHORTARRAY:
				if (new_tmp_sh == NULL)
				{
					new_tmp_sh = new DevVarShortArray();
					new_tmp_sh->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_sh;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_sh,tmp_sh,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_sh->length());
				break;

			case DEV_LONG:
				if (new_tmp_lg == NULL)
				{
					new_tmp_lg = new DevVarLongArray();
					new_tmp_lg->length(seq_size);
				}
				(*ring[index].cmd_result) >>= lg;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_lg,lg,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_LONGARRAY:
				if (new_tmp_lg == NULL)
				{
					new_tmp_lg = new DevVarLongArray();
					new_tmp_lg->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_lg;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_lg,tmp_lg,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_lg->length());
				break;

			case DEV_FLOAT:
				if (new_tmp_fl == NULL)
				{
					new_tmp_fl = new DevVarFloatArray();
					new_tmp_fl->length(seq_size);
				}
				(*ring[index].cmd_result) >>= fl;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_fl,fl,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_FLOATARRAY:
				if (new_tmp_fl == NULL)
				{
					new_tmp_fl = new DevVarFloatArray();
					new_tmp_fl->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_fl;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_fl,tmp_fl,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_fl->length());
				break;

			case DEV_DOUBLE:
				if (new_tmp_db == NULL)
				{
					new_tmp_db = new DevVarDoubleArray();
					new_tmp_db->length(seq_size);
				}
				(*ring[index].cmd_result) >>= db;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_db,db,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_DOUBLEARRAY:
				if (new_tmp_db == NULL)
				{
					new_tmp_db = new DevVarDoubleArray();
					new_tmp_db->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_db;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_db,tmp_db,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_db->length());
				break;

			case DEV_USHORT:
				if (new_tmp_ush == NULL)
				{
					new_tmp_ush = new DevVarUShortArray();
					new_tmp_ush->length(seq_size);
				}
				(*ring[index].cmd_result) >>= ush;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_ush,ush,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_USHORTARRAY:
				if (new_tmp_ush == NULL)
				{
					new_tmp_ush = new DevVarUShortArray();
					new_tmp_ush->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_ush;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_ush,tmp_ush,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_ush->length());
				break;

			case DEV_ULONG:
				if (new_tmp_ulg == NULL)
				{
					new_tmp_ulg = new DevVarULongArray();
					new_tmp_ulg->length(seq_size);
				}
				(*ring[index].cmd_result) >>= ulg;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_ulg,ulg,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_ULONGARRAY:
				if (new_tmp_ulg == NULL)
				{
					new_tmp_ulg = new DevVarULongArray();
					new_tmp_ulg->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_ulg;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_ulg,tmp_ulg,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_ulg->length());
				break;

			case DEV_STRING:
				if (new_tmp_str == NULL)
				{
					new_tmp_str = new DevVarStringArray();
					new_tmp_str->length(seq_size);
				}
				(*ring[index].cmd_result) >>= str;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_str,str,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_STRINGARRAY:
				if (new_tmp_str == NULL)
				{
					new_tmp_str = new DevVarStringArray();
					new_tmp_str->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_str;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_str,tmp_str,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_str->length());
				break;

			case DEV_BOOLEAN:
				if (new_tmp_boo == NULL)
				{
					new_tmp_boo = new DevVarBooleanArray();
					new_tmp_boo->length(seq_size);
				}
				(*ring[index].cmd_result) >>= boo;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_boo,boo,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_BOOLEANARRAY:
				if (new_tmp_boo == NULL)
				{
					new_tmp_boo = new DevVarBooleanArray();
					new_tmp_boo->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_boo;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_boo,tmp_boo,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_boo->length());
				break;

			case DEV_LONG64:
				if (new_tmp_lg64 == NULL)
				{
					new_tmp_lg64 = new DevVarLong64Array();
					new_tmp_lg64->length(seq_size);
				}
				(*ring[index].cmd_result) >>= lg64;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_lg64,lg64,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_LONG64ARRAY:
				if (new_tmp_lg64 == NULL)
				{
					new_tmp_lg64 = new DevVarLong64Array();
					new_tmp_lg64->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_lg64;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_lg64,tmp_lg64,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_lg64->length());
				break;

			case DEV_ULONG64:
				if (new_tmp_ulg64 == NULL)
				{
					new_tmp_ulg64 = new DevVarULong64Array();
					new_tmp_ulg64->length(seq_size);
				}
				(*ring[index].cmd_result) >>= ulg64;
				ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(new_tmp_ulg64,ulg64,ind_in_seq);
				MANAGE_DIM_SIMPLE();
				break;

			case DEVVAR_ULONG64ARRAY:
				if (new_tmp_ulg64 == NULL)
				{
					new_tmp_ulg64 = new DevVarULong64Array();
					new_tmp_ulg64->length(seq_size);
				}
				(*ring[index].cmd_result) >>= tmp_ulg64;
				ADD_ELT_DATA_TO_GLOBAL_SEQ(new_tmp_ulg64,tmp_ulg64,ind_in_seq);
				MANAGE_DIM_ARRAY((long)tmp_ulg64->length());
				break;

			case DEVVAR_LONGSTRINGARRAY:
				if (new_tmp_lg_str == NULL)
				{
					new_tmp_lg_str = new DevVarLongStringArray();
					new_tmp_lg_str->svalue.length(seq_str_size);
					new_tmp_lg_str->lvalue.length(seq_number_size);
				}
				(*ring[index].cmd_result) >>= tmp_lg_str;
				ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_REF(new_tmp_lg_str->svalue,tmp_lg_str->svalue,ind_str_in_seq);
				ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_REF(new_tmp_lg_str->lvalue,tmp_lg_str->lvalue,ind_num_in_seq);
				MANAGE_DIM_ARRAY_SEQ((long)tmp_lg_str->svalue.length(),(long)tmp_lg_str->lvalue.length());
				break;

			case DEVVAR_DOUBLESTRINGARRAY:
				if (new_tmp_db_str == NULL)
				{
					new_tmp_db_str = new DevVarDoubleStringArray();
					new_tmp_db_str->svalue.length(seq_str_size);
					new_tmp_db_str->dvalue.length(seq_number_size);
				}
				(*ring[index].cmd_result) >>= tmp_db_str;
				ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_REF(new_tmp_db_str->svalue,tmp_db_str->svalue,ind_str_in_seq);
				ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_REF(new_tmp_db_str->dvalue,tmp_db_str->dvalue,ind_num_in_seq);
				MANAGE_DIM_ARRAY_SEQ((long)tmp_db_str->svalue.length(),(long)tmp_db_str->dvalue.length());
				break;

			case DEV_ENCODED:
				if (new_tmp_enc == NULL)
				{
					new_tmp_enc = new DevVarEncodedArray();
					new_tmp_enc->length(seq_size);
				}
				(*ring[index].cmd_result) >>= enc;

				{
					unsigned int buffer_data_length = enc->encoded_data.length();
					Tango::DevVarCharArray &tmp_buffer = (*new_tmp_enc)[ind_in_seq].encoded_data;
					tmp_buffer.length(buffer_data_length);
					for (unsigned int k = 0;k < buffer_data_length;k++)
						tmp_buffer[k] = enc->encoded_data[k];
					(*new_tmp_enc)[ind_in_seq].encoded_format = Tango::string_dup(enc->encoded_format);
					ind_in_seq = ind_in_seq + 1;
				}
				MANAGE_DIM_SIMPLE();
				break;

			default:
				break;
			}
		}

//
// If it is the last point, insert the global sequence into the Any
//

		if (i == (n  - 1))
		{
			if (errors_length != (error_nb + 1))
			{
				ptr->errors.length(errors_length - 1);
				ptr->errors_array.length(errors_length - 1);
			}

			switch(cmd_type)
			{
			case Tango::DEVVAR_CHARARRAY:
				if (new_tmp_uch != NULL)
					ptr->value <<= new_tmp_uch;
				break;

			case Tango::DEVVAR_SHORTARRAY:
			case Tango::DEV_SHORT:
				if (new_tmp_sh != NULL)
					ptr->value <<= new_tmp_sh;
				break;

			case Tango::DEVVAR_LONGARRAY:
			case Tango::DEV_LONG:
				if (new_tmp_lg != NULL)
					ptr->value <<= new_tmp_lg;
				break;

			case Tango::DEVVAR_FLOATARRAY:
			case Tango::DEV_FLOAT:
				if (new_tmp_fl != NULL)
					ptr->value <<= new_tmp_fl;
				break;

			case Tango::DEVVAR_DOUBLEARRAY:
			case Tango::DEV_DOUBLE:
				if (new_tmp_db != NULL)
					ptr->value <<= new_tmp_db;
				break;

			case Tango::DEVVAR_USHORTARRAY:
			case Tango::DEV_USHORT:
				if (new_tmp_db != NULL)
					ptr->value <<= new_tmp_db;
				break;

			case Tango::DEVVAR_ULONGARRAY:
			case Tango::DEV_ULONG:
				if (new_tmp_db != NULL)
					ptr->value <<= new_tmp_db;
				break;

			case Tango::DEVVAR_STRINGARRAY:
			case Tango::DEV_STRING:
				if (new_tmp_str != NULL)
					ptr->value <<= new_tmp_str;
				break;

			case Tango::DEVVAR_BOOLEANARRAY:
			case Tango::DEV_BOOLEAN:
				if (new_tmp_boo != NULL)
					ptr->value <<= new_tmp_boo;
				break;

			case Tango::DEVVAR_LONG64ARRAY:
			case Tango::DEV_LONG64:
				if (new_tmp_lg64 != NULL)
					ptr->value <<= new_tmp_lg64;
				break;

			case Tango::DEVVAR_ULONG64ARRAY:
			case Tango::DEV_ULONG64:
				if (new_tmp_ulg64 != NULL)
					ptr->value <<= new_tmp_ulg64;
				break;

			case Tango::DEVVAR_LONGSTRINGARRAY:
				if (new_tmp_lg_str != NULL)
					ptr->value <<= new_tmp_lg_str;
				break;

			case Tango::DEVVAR_DOUBLESTRINGARRAY:
				if (new_tmp_db_str != NULL)
					ptr->value <<= new_tmp_db_str;
				break;

			case Tango::DEV_ENCODED:
				if (new_tmp_enc != NULL)
					ptr->value <<= new_tmp_enc;
				break;

			default:
				break;
			}
		}

//
// Manage index in buffer
//

		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}
}

//-------------------------------------------------------------------------
//
// method : 		PollObj::get_attr_history
//
// description : 	This method get attribute history from the ring buffer
//
// argument : in :	- n : record number
//			- ptr : Pointer to the sequence where attribute result
//				should be stored
//			- type : The attribute data type
//
//--------------------------------------------------------------------------

void PollRing::get_attr_history(long n,Tango::DevAttrHistoryList *ptr,long type)
{
	long i;

//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;

	long seq_index = n - 1;

//
// Read buffer
//

	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].value.time = make_TimeVal(ring[index].when);

		if (ring[index].except == NULL)
		{
			(*ptr)[seq_index].attr_failed = false;
			(*ptr)[seq_index].value.quality = (*ring[index].attr_value)[0].quality;
			(*ptr)[seq_index].value.dim_x = (*ring[index].attr_value)[0].dim_x;
			(*ptr)[seq_index].value.dim_y = (*ring[index].attr_value)[0].dim_y;
			(*ptr)[seq_index].value.name = Tango::string_dup((*ring[index].attr_value)[0].name);

			if ((*ptr)[seq_index].value.quality != Tango::ATTR_INVALID)
			{
				const Tango::DevVarDoubleArray *tmp_db;
				Tango::DevVarDoubleArray *new_tmp_db;
				const Tango::DevVarShortArray *tmp_sh;
				Tango::DevVarShortArray *new_tmp_sh;
				const Tango::DevVarLongArray *tmp_lg;
				Tango::DevVarLongArray *new_tmp_lg;
				const Tango::DevVarLong64Array *tmp_lg64;
				Tango::DevVarLong64Array *new_tmp_lg64;
				const Tango::DevVarStringArray *tmp_str;
				Tango::DevVarStringArray *new_tmp_str;
				const Tango::DevVarFloatArray *tmp_fl;
				Tango::DevVarFloatArray *new_tmp_fl;
				const Tango::DevVarBooleanArray *tmp_boo;
				Tango::DevVarBooleanArray *new_tmp_boo;
				const Tango::DevVarUShortArray *tmp_ush;
				Tango::DevVarUShortArray *new_tmp_ush;
				const Tango::DevVarCharArray *tmp_uch;
				Tango::DevVarCharArray *new_tmp_uch;
				const Tango::DevVarULongArray *tmp_ulg;
				Tango::DevVarULongArray *new_tmp_ulg;
				const Tango::DevVarULong64Array *tmp_ulg64;
				Tango::DevVarULong64Array *new_tmp_ulg64;
				const Tango::DevVarStateArray *tmp_state;
				Tango::DevVarStateArray *new_tmp_state;

				switch (type)
				{
				case Tango::DEV_SHORT :
				case Tango::DEV_ENUM :
					(*ring[index].attr_value)[0].value >>= tmp_sh;
					new_tmp_sh = new DevVarShortArray(
							tmp_sh->length(),
							tmp_sh->length(),
							const_cast<short *>(tmp_sh->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_sh;
					break;

				case Tango::DEV_DOUBLE :
					(*ring[index].attr_value)[0].value >>= tmp_db;
					new_tmp_db = new DevVarDoubleArray(
							tmp_db->length(),
							tmp_db->length(),
							const_cast<double *>(tmp_db->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_db;
					break;

				case Tango::DEV_LONG :
					(*ring[index].attr_value)[0].value >>= tmp_lg;
					new_tmp_lg = new DevVarLongArray(tmp_lg->length(),tmp_lg->length(),
										const_cast<DevLong *>(tmp_lg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg;
					break;

				case Tango::DEV_LONG64 :
					(*ring[index].attr_value)[0].value >>= tmp_lg64;
					new_tmp_lg64 = new DevVarLong64Array(tmp_lg64->length(),tmp_lg64->length(),
										const_cast<DevLong64 *>(tmp_lg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg64;
					break;

				case Tango::DEV_STRING :
					(*ring[index].attr_value)[0].value >>= tmp_str;
					new_tmp_str = new DevVarStringArray(
							tmp_str->length(),
							tmp_str->length(),
							const_cast<char **>(tmp_str->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_str;
					break;

				case Tango::DEV_FLOAT :
					(*ring[index].attr_value)[0].value >>= tmp_fl;
					new_tmp_fl = new DevVarFloatArray(
							tmp_fl->length(),
							tmp_fl->length(),
							const_cast<float *>(tmp_fl->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_fl;
					break;

				case Tango::DEV_BOOLEAN :
					(*ring[index].attr_value)[0].value >>= tmp_boo;
					new_tmp_boo = new DevVarBooleanArray(
							tmp_boo->length(),
							tmp_boo->length(),
							const_cast<DevBoolean *>(tmp_boo->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_boo;
					break;

				case Tango::DEV_USHORT :
					(*ring[index].attr_value)[0].value >>= tmp_ush;
					new_tmp_ush = new DevVarUShortArray(
							tmp_ush->length(),
							tmp_ush->length(),
							const_cast<DevUShort *>(tmp_ush->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_ush;
					break;

				case Tango::DEV_UCHAR :
					(*ring[index].attr_value)[0].value >>= tmp_uch;
					new_tmp_uch = new DevVarCharArray(
							tmp_uch->length(),
							tmp_uch->length(),
							const_cast<DevUChar *>(tmp_uch->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_uch;
					break;

				case Tango::DEV_ULONG :
					(*ring[index].attr_value)[0].value >>= tmp_ulg;
					new_tmp_ulg = new DevVarULongArray(tmp_ulg->length(),tmp_ulg->length(),
										const_cast<DevULong *>(tmp_ulg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg;
					break;

				case Tango::DEV_ULONG64 :
					(*ring[index].attr_value)[0].value >>= tmp_ulg64;
					new_tmp_ulg64 = new DevVarULong64Array(tmp_ulg64->length(),tmp_ulg64->length(),
										const_cast<DevULong64 *>(tmp_ulg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg64;
					break;

				case Tango::DEV_STATE :
					(*ring[index].attr_value)[0].value >>= tmp_state;
					new_tmp_state = new DevVarStateArray(tmp_state->length(),tmp_state->length(),
										const_cast<DevState *>(tmp_state->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_state;
					break;
				}
			}
			(*ptr)[seq_index].errors.length(0);
		}
		else
		{
			(*ptr)[seq_index].attr_failed = true;
			(*ptr)[seq_index].errors = ring[index].except->errors;

			(*ptr)[seq_index].value.quality = Tango::ATTR_INVALID;
			(*ptr)[seq_index].value.dim_x = 0;
			(*ptr)[seq_index].value.dim_y = 0;
		}

		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}

}

void PollRing::get_attr_history(long n,Tango::DevAttrHistoryList_3 *ptr,long type)
{
	long i;

//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;

	long seq_index = n - 1;

//
// Read buffer
//

	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].value.time = make_TimeVal(ring[index].when);

		if (ring[index].except == NULL)
		{
			(*ptr)[seq_index].value.err_list = (*ring[index].attr_value_3)[0].err_list;
			(*ptr)[seq_index].value.quality = (*ring[index].attr_value_3)[0].quality;
			(*ptr)[seq_index].value.r_dim = (*ring[index].attr_value_3)[0].r_dim;
			(*ptr)[seq_index].value.w_dim = (*ring[index].attr_value_3)[0].w_dim;
			(*ptr)[seq_index].value.name = Tango::string_dup((*ring[index].attr_value_3)[0].name);


			(*ptr)[seq_index].attr_failed = false;

			if ((*ptr)[seq_index].value.quality != Tango::ATTR_INVALID)
			{
				const Tango::DevVarDoubleArray *tmp_db;
				Tango::DevVarDoubleArray *new_tmp_db;
				const Tango::DevVarShortArray *tmp_sh;
				Tango::DevVarShortArray *new_tmp_sh;
				const Tango::DevVarLongArray *tmp_lg;
				Tango::DevVarLongArray *new_tmp_lg;
				const Tango::DevVarLong64Array *tmp_lg64;
				Tango::DevVarLong64Array *new_tmp_lg64;
				const Tango::DevVarStringArray *tmp_str;
				Tango::DevVarStringArray *new_tmp_str;
				const Tango::DevVarFloatArray *tmp_fl;
				Tango::DevVarFloatArray *new_tmp_fl;
				const Tango::DevVarBooleanArray *tmp_boo;
				Tango::DevVarBooleanArray *new_tmp_boo;
				const Tango::DevVarUShortArray *tmp_ush;
				Tango::DevVarUShortArray *new_tmp_ush;
				const Tango::DevVarCharArray *tmp_uch;
				Tango::DevVarCharArray *new_tmp_uch;
				const Tango::DevVarULongArray *tmp_ulg;
				Tango::DevVarULongArray *new_tmp_ulg;
				const Tango::DevVarULong64Array *tmp_ulg64;
				Tango::DevVarULong64Array *new_tmp_ulg64;
				const Tango::DevVarStateArray *tmp_state;
				Tango::DevVarStateArray *new_tmp_state;


				switch (type)
				{
				case Tango::DEV_SHORT :
				case Tango::DEV_ENUM :
					(*ring[index].attr_value_3)[0].value >>= tmp_sh;
					new_tmp_sh = new DevVarShortArray(
							tmp_sh->length(),
							tmp_sh->length(),
							const_cast<short *>(tmp_sh->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_sh;
					break;

				case Tango::DEV_DOUBLE :
					(*ring[index].attr_value_3)[0].value >>= tmp_db;
					new_tmp_db = new DevVarDoubleArray(
							tmp_db->length(),
							tmp_db->length(),
							const_cast<double *>(tmp_db->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_db;
					break;

				case Tango::DEV_LONG :
					(*ring[index].attr_value_3)[0].value >>= tmp_lg;
					new_tmp_lg = new DevVarLongArray(tmp_lg->length(),tmp_lg->length(),
							const_cast<DevLong *>(tmp_lg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg;
					break;

				case Tango::DEV_LONG64 :
					(*ring[index].attr_value_3)[0].value >>= tmp_lg64;
					new_tmp_lg64 = new DevVarLong64Array(tmp_lg64->length(),tmp_lg64->length(),
							const_cast<DevLong64 *>(tmp_lg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_lg64;
					break;

				case Tango::DEV_STRING :
					(*ring[index].attr_value_3)[0].value >>= tmp_str;
					new_tmp_str = new DevVarStringArray(
							tmp_str->length(),
							tmp_str->length(),
							const_cast<char **>(tmp_str->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_str;
					break;

				case Tango::DEV_FLOAT :
					(*ring[index].attr_value_3)[0].value >>= tmp_fl;
					new_tmp_fl = new DevVarFloatArray(
							tmp_fl->length(),
							tmp_fl->length(),
							const_cast<float *>(tmp_fl->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_fl;
					break;

				case Tango::DEV_BOOLEAN :
					(*ring[index].attr_value_3)[0].value >>= tmp_boo;
					new_tmp_boo = new DevVarBooleanArray(
							tmp_boo->length(),
							tmp_boo->length(),
							const_cast<DevBoolean *>(tmp_boo->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_boo;
					break;

				case Tango::DEV_USHORT :
					(*ring[index].attr_value_3)[0].value >>= tmp_ush;
					new_tmp_ush = new DevVarUShortArray(
							tmp_ush->length(),
							tmp_ush->length(),
							const_cast<DevUShort *>(tmp_ush->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_ush;
					break;

				case Tango::DEV_UCHAR :
					(*ring[index].attr_value_3)[0].value >>= tmp_uch;
					new_tmp_uch = new DevVarCharArray(
							tmp_uch->length(),
							tmp_uch->length(),
							const_cast<DevUChar *>(tmp_uch->get_buffer()),
							false);
					(*ptr)[seq_index].value.value <<= new_tmp_uch;
					break;

				case Tango::DEV_ULONG :
					(*ring[index].attr_value_3)[0].value >>= tmp_ulg;
					new_tmp_ulg = new DevVarULongArray(tmp_ulg->length(),tmp_ulg->length(),
							const_cast<DevULong *>(tmp_ulg->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg;
					break;

				case Tango::DEV_ULONG64 :
					(*ring[index].attr_value_3)[0].value >>= tmp_ulg64;
					new_tmp_ulg64 = new DevVarULong64Array(tmp_ulg64->length(),tmp_ulg64->length(),
							const_cast<DevULong64 *>(tmp_ulg64->get_buffer()),false);
					(*ptr)[seq_index].value.value <<= new_tmp_ulg64;
					break;

				case Tango::DEV_STATE :
				{
					CORBA::TypeCode_var ty;
					ty = (*ring[index].attr_value_3)[0].value.type();

					if (ty->kind() == CORBA::tk_enum)
					{
						Tango::DevState tmp_ds;
						(*ring[index].attr_value_3)[0].value >>= tmp_ds;
						(*ptr)[seq_index].value.value <<= tmp_ds;
					}
					else
					{
						(*ring[index].attr_value_3)[0].value >>= tmp_state;
						new_tmp_state = new DevVarStateArray(tmp_state->length(),tmp_state->length(),
									const_cast<DevState *>(tmp_state->get_buffer()),false);
						(*ptr)[seq_index].value.value <<= new_tmp_state;
					}
					break;
				}
				}
			}
		}
		else
		{
			(*ptr)[seq_index].attr_failed = true;
			(*ptr)[seq_index].value.err_list = ring[index].except->errors;
			(*ptr)[seq_index].value.quality = Tango::ATTR_INVALID;
			clear_att_dim((*ptr)[seq_index].value);
		}


		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}

}

void PollRing::get_attr_history_43(long n,Tango::DevAttrHistoryList_3 *ptr,long type)
{
	long i;

//
// Set index to read ring and to initialised returned sequence
// In the returned sequence , indice 0 is the oldest data
//

	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;

	long seq_index = n - 1;

//
// Read buffer
//

	for (i = 0;i < n;i++)
	{
		(*ptr)[seq_index].value.time = make_TimeVal(ring[index].when);

		if (ring[index].except == NULL)
		{
			(*ptr)[seq_index].value.err_list = (*ring[index].attr_value_4)[0].err_list;
			(*ptr)[seq_index].value.quality = (*ring[index].attr_value_4)[0].quality;
			(*ptr)[seq_index].value.r_dim = (*ring[index].attr_value_4)[0].r_dim;
			(*ptr)[seq_index].value.w_dim = (*ring[index].attr_value_4)[0].w_dim;
			(*ptr)[seq_index].value.name = Tango::string_dup((*ring[index].attr_value_4)[0].name);


			(*ptr)[seq_index].attr_failed = false;

			if ((*ptr)[seq_index].value.quality != Tango::ATTR_INVALID)
			{
				switch (type)
				{
				case Tango::DEV_SHORT :
				case Tango::DEV_ENUM :
					{
						const DevVarShortArray &tmp_seq = (*ring[index].attr_value_4)[0].value.short_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_DOUBLE :
					{
						const DevVarDoubleArray &tmp_seq = 	(*ring[index].attr_value_4)[0].value.double_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_LONG :
					{
						const DevVarLongArray &tmp_seq = (*ring[index].attr_value_4)[0].value.long_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_LONG64 :
					{
						const DevVarLong64Array &tmp_seq = (*ring[index].attr_value_4)[0].value.long64_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_STRING :
					{
						const DevVarStringArray &tmp_seq = (*ring[index].attr_value_4)[0].value.string_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_FLOAT :
					{
						const DevVarFloatArray &tmp_seq = (*ring[index].attr_value_4)[0].value.float_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_BOOLEAN :
					{
						const DevVarBooleanArray &tmp_seq = (*ring[index].attr_value_4)[0].value.bool_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_USHORT :
					{
						const DevVarUShortArray &tmp_seq = (*ring[index].attr_value_4)[0].value.ushort_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_UCHAR :
					{
						const DevVarCharArray &tmp_seq = (*ring[index].attr_value_4)[0].value.uchar_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_ULONG :
					{
						const DevVarULongArray &tmp_seq = (*ring[index].attr_value_4)[0].value.ulong_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_ULONG64 :
					{
						const DevVarULong64Array &tmp_seq = (*ring[index].attr_value_4)[0].value.ulong64_att_value();
						(*ptr)[seq_index].value.value <<= tmp_seq;
					}
					break;

				case Tango::DEV_STATE :
					{
						if ((*ring[index].attr_value_4)[0].value._d() == DEVICE_STATE)
						{
							DevState tmp_ds = (*ring[index].attr_value_4)[0].value.dev_state_att();
							(*ptr)[seq_index].value.value <<= tmp_ds;
						}
						else
						{
							DevVarStateArray &tmp_seq = (*ring[index].attr_value_4)[0].value.state_att_value();
							(*ptr)[seq_index].value.value <<= tmp_seq;
						}
					}
					break;
				}
			}
		}
		else
		{
			(*ptr)[seq_index].attr_failed = true;
			(*ptr)[seq_index].value.err_list = ring[index].except->errors;
			(*ptr)[seq_index].value.quality = Tango::ATTR_INVALID;
			clear_att_dim((*ptr)[seq_index].value);
		}


		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}
}

} // End of Tango namespace

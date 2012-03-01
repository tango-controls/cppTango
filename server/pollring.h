//=============================================================================
//
// file :               PollRing.h
//
// description :        Include for the PollRing object. This class implements
//                      the polling ring buffer. Command result or attribute
//			values are stored in this buffer manages as a ring
//			buffer.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//=============================================================================

#ifndef _POLLRING_H
#define _POLLRING_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The RingElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================


class RingElt
{
public:
	RingElt();

	CORBA::Any					*cmd_result;
	Tango::AttributeValueList	*attr_value;
	Tango::AttributeValueList_3	*attr_value_3;
	Tango::AttributeValueList_4	*attr_value_4;
	Tango::DevFailed			*except;
	struct timeval				when;
};

inline bool operator<(const RingElt &,const RingElt &)
{
	return true;
}

inline bool operator==(const RingElt &,const RingElt &)
{
	return true;
}

//=============================================================================
//
//			The PollRing class
//
// description :	Class to implement the ring buffer itself. This is mainly
//			a vector of RingElt managed as a circular buffer
//
//=============================================================================

class PollRing
{
public:
	PollRing();
	PollRing(long);
	~PollRing();

	void insert_data(CORBA::Any *,struct timeval &);
	void insert_data(Tango::AttributeValueList *,struct timeval &);
	void insert_data(Tango::AttributeValueList_3 *,struct timeval &);
	void insert_data(Tango::AttributeValueList_4 *,struct timeval &,bool);
	void insert_except(Tango::DevFailed *,struct timeval &);

	void force_copy_data(Tango::AttributeValueList_4 *);

	void get_delta_t(vector<double> &,long nb);
	struct timeval get_last_insert_date();
	bool is_empty() {if (nb_elt == 0) return true;else return false;}

	bool is_last_an_error();
	bool is_last_cmd_an_error();
	bool is_last_attr_an_error();

	Tango::DevFailed *get_last_except();
	Tango::DevErrorList &get_last_attr_error();

	CORBA::Any *get_last_cmd_result();
	Tango::AttributeValue &get_last_attr_value();
	Tango::AttributeValue_3 &get_last_attr_value_3();
	Tango::AttributeValue_4 &get_last_attr_value_4();
	long get_nb_elt() {return nb_elt;}

	void get_cmd_history(long,Tango::DevCmdHistoryList *);
	void get_cmd_history(long,Tango::DevCmdHistory_4 *,Tango::CmdArgType &);

	void get_attr_history(long,Tango::DevAttrHistoryList *,long);
	void get_attr_history(long,Tango::DevAttrHistoryList_3 *,long);
	void get_attr_history(long,Tango::DevAttrHistory_4 *,long);

	void get_attr_history_43(long,Tango::DevAttrHistoryList_3 *,long);

private:
	void inc_indexes();

	vector<RingElt>		ring;
	long				insert_elt;
	long				nb_elt;
	long				max_elt;
};


#define ADD_ELT_DATA_TO_GLOBAL_SEQ(GLOB,ELT,IND) \
	{\
		unsigned int elt_data_length = ELT->length(); \
		for (unsigned int k = 0;k < elt_data_length;k++) \
			(*GLOB)[IND + k] = (*ELT)[k]; \
		IND = IND + elt_data_length; \
	}

#define ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_REF(GLOB,ELT,IND) \
	{\
		unsigned int elt_data_length = ELT.length(); \
		for (unsigned int k = 0;k < elt_data_length;k++) \
			GLOB[IND + k] = ELT[k]; \
		IND = IND + elt_data_length; \
	}

#define ADD_ELT_DATA_TO_GLOBAL_SEQ_BY_PTR_REF(GLOB,ELT,IND) \
	{\
		unsigned int elt_data_length = ELT.length(); \
		for (unsigned int k = 0;k < elt_data_length;k++) \
			(*GLOB)[IND + k] = ELT[k]; \
		IND = IND + elt_data_length; \
	}

#define MANAGE_DIM_ARRAY(LENGTH) \
	if (last_dim.dim_x == LENGTH) \
	{ \
		ptr->dims_array[dims_length - 2].nb_elt++; \
	} \
	else \
	{ \
		last_dim.dim_x = LENGTH; \
		last_dim.dim_y = 0; \
		ptr->dims.length(dims_length); \
		ptr->dims[dims_length - 1] = last_dim; \
		ptr->dims_array.length(dims_length); \
		ptr->dims_array[dims_length - 1].start = n - (i + 1); \
		ptr->dims_array[dims_length - 1].nb_elt = 1; \
		dims_length++;	\
	}

#define ADD_SIMPLE_DATA_TO_GLOBAL_SEQ(GLOB,ELT,IND) \
	(*GLOB)[IND] = ELT; \
	IND = IND + 1;

#define MANAGE_DIM_SIMPLE() \
	if (last_dim.dim_x == 1) \
	{ \
		ptr->dims_array[dims_length - 2].nb_elt++; \
	} \
	else \
	{ \
		last_dim.dim_x = 1; \
		last_dim.dim_y = 0; \
		ptr->dims.length(dims_length); \
		ptr->dims[dims_length - 1] = last_dim; \
		ptr->dims_array.length(dims_length); \
		ptr->dims_array[dims_length - 1].start = n - (i + 1); \
		ptr->dims_array[dims_length - 1].nb_elt = 1; \
		dims_length++;	\
	}

#define MANAGE_DIM_ARRAY_SEQ(LENGTH_STR,LENGTH_NUM) \
	if ((last_dim.dim_x == LENGTH_STR)  && (last_dim.dim_y == LENGTH_NUM))\
	{ \
		ptr->dims_array[dims_length - 2].nb_elt++; \
	} \
	else \
	{ \
		last_dim.dim_x = LENGTH_STR; \
		last_dim.dim_y = LENGTH_NUM; \
		ptr->dims.length(dims_length); \
		ptr->dims[dims_length - 1] = last_dim; \
		ptr->dims_array.length(dims_length); \
		ptr->dims_array[dims_length - 1].start = n - (i + 1); \
		ptr->dims_array[dims_length - 1].nb_elt = 1; \
		dims_length++;	\
	}

} // End of Tango namespace

#endif /* _POLLRING_ */

//=============================================================================
//
// file :               PollObj.h
//
// description :        Include for the PollObj object. This class implements
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

#ifndef _POLLOBJ_H
#define _POLLOBJ_H

#include <tango.h>
#include <pollring.h>

namespace Tango
{

//=============================================================================
//
//			The PollObj class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//			This class is protected against concurrent access by a
//			omni_mutex. We use the Thread-Safe interface pattern
//			to avoid "self-deadlock". This is why several methods
//			are splitted in two: One method called "meth" and
//			another one called "meth_i" (which is also sometimes
//			public !!)
//
//=============================================================================


class PollObj: public omni_mutex
{
public:
	PollObj(DeviceImpl *,PollObjType,const string &,int);
	PollObj(DeviceImpl *,PollObjType,const string &,int,long);

	void insert_data(CORBA::Any *,struct timeval &,struct timeval &);
	void insert_data(Tango::AttributeValueList *,struct timeval &,struct timeval &);
	void insert_data(Tango::AttributeValueList_3 *,struct timeval &,struct timeval &);
	void insert_data(Tango::AttributeValueList_4 *,struct timeval &,struct timeval &);
	void insert_except(Tango::DevFailed *,struct timeval &,struct timeval &);

	double get_authorized_delta() {return max_delta_t;}
	void update_upd(int);

	CORBA::Any *get_last_cmd_result();
	Tango::AttributeValue &get_last_attr_value(bool);
	Tango::AttributeValue_3 &get_last_attr_value_3(bool);
	Tango::AttributeValue_4 &get_last_attr_value_4(bool);

	bool is_ring_empty() {omni_mutex_lock(*this);return is_ring_empty_i();}
	bool is_ring_empty_i() {return ring.is_empty();}

	long get_upd() {omni_mutex_lock(*this);return get_upd_i();}
	long get_upd_i() {return ((upd.tv_sec * 1000) + (upd.tv_usec / 1000));}

	string &get_name() {omni_mutex_lock(*this);return get_name_i();}
	string &get_name_i() {return name;}

	inline double get_needed_time() {omni_mutex_lock(*this);return get_needed_time_i();}
	inline double get_needed_time_i()
	{
		return ((needed_time.tv_sec * 1000) + (needed_time.tv_usec / 1000.0));
	}

	inline PollObjType get_type() {omni_mutex_lock(*this);return get_type_i();}
	inline PollObjType get_type_i() {return type;}

	double get_last_insert_date() {omni_mutex_lock(*this);return get_last_insert_date_i();}
	double get_last_insert_date_i();

	bool is_last_an_error() {omni_mutex_lock(*this);return is_last_an_error_i();}
	bool is_last_an_error_i() {return ring.is_last_an_error();}
	bool is_last_an_error_i_3()
	{if (type==POLL_CMD)return ring.is_last_cmd_an_error();else return ring.is_last_attr_an_error();}

	Tango::DevFailed *get_last_except() {omni_mutex_lock(*this);return get_last_except_i();}
	Tango::DevFailed *get_last_except_i() {return ring.get_last_except();}
	Tango::DevErrorList &get_last_attr_error_i() {return ring.get_last_attr_error();}

	inline void get_delta_t(vector<double> &vd, long nb)
	{omni_mutex_lock(*this);get_delta_t_i(vd,nb);}
	inline void get_delta_t_i(vector<double> &vd,long nb)
	{ring.get_delta_t(vd,nb);}

	inline long get_elt_nb_in_buffer()
	{omni_mutex_lock(*this);return get_elt_nb_in_buffer_i();}
	inline long get_elt_nb_in_buffer_i()
	{return ring.get_nb_elt();}

	void get_cmd_history(long,Tango::DevCmdHistoryList *);
	void get_cmd_history(long,Tango::DevCmdHistory_4 *,Tango::CmdArgType &);

	void get_attr_history(long n,Tango::DevAttrHistoryList *ptr,long type);
	void get_attr_history(long n,Tango::DevAttrHistoryList_3 *ptr,long type);
	void get_attr_history(long n,Tango::DevAttrHistory_4 *ptr,long type);

	void get_attr_history_43(long n,Tango::DevAttrHistoryList_3 *ptr,long type);

protected:
	DeviceImpl		*dev;
	PollObjType		type;
	string 			name;
	struct timeval		upd;
	struct timeval		needed_time;
	double			max_delta_t;
	PollRing		ring;
};

inline bool operator<(const PollObj &,const PollObj &)
{
	return true;
}

inline bool operator==(const PollObj &,const PollObj &)
{
	return true;
}

} // End of Tango namespace

#endif /* _POLLOBJ_ */

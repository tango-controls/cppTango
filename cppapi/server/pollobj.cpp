//+==================================================================================================================
//
// file :               PollObj.cpp
//
// description :        C++ source code for the PollObj class. This class is used to store all data specific to one
//						polled object and which does not need to be stored in the ring buffer
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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <pollobj.h>
#include <pollring.h>
#include <pollring.tpp>


#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* _TG_WINDOWS_ */

namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::PollObj
//
// description :
//		Three constructors for the PollObj class. The first one constructs a PollObj instance with the
//		default polling ring depth. The second one create a PollObj instance with a specified polling ring depth
//
// argument :
//		in :
//			- d : The device pointer
//			- ty : The polled object type
//			- na : The polled object name
//			- user_upd : The polling update period (in mS)
//			- r_depth : The polling ring depth
//
//-------------------------------------------------------------------------------------------------------------------

PollObj::PollObj(DeviceImpl *d, PollObjType ty, const std::string &na, PollClock::duration user_upd) :
	dev(d),
	type(ty),
	name(na),
	upd(user_upd),
	needed_time(),
	max_delta_t(upd * dev->get_poll_old_factor()),
	ring(),
	fwd(false)
{
}

PollObj::PollObj(DeviceImpl *d, PollObjType ty, const std::string &na, PollClock::duration user_upd, long r_depth) :
	dev(d),
	type(ty),
	name(na),
	upd(user_upd),
	needed_time(),
	max_delta_t(upd * dev->get_poll_old_factor()),
	ring(r_depth),
	fwd(false)
{
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::insert_data
//
// description :
//		These methods insert a new element in the object ring buffer when its real data
//
// argument :
//		in :
//			- res : The Any returned by the command
//			- when : The date when data was read
//			- needed : The time needed to execute command/attribute reading
//
//--------------------------------------------------------------------------------------------------------------------

void PollObj::insert_data(CORBA::Any *res, PollClock::time_point when, PollClock::duration needed)
{
	omni_mutex_lock sync(*this);

	ring.insert_data(res,when);
	needed_time = needed;
}

void PollObj::insert_data(Tango::AttributeValueList *res, PollClock::time_point when, PollClock::duration needed)
{
	omni_mutex_lock sync(*this);

	ring.insert_data(res,when);
	needed_time = needed;
}

void PollObj::insert_data(Tango::AttributeValueList_3 *res, PollClock::time_point when, PollClock::duration needed)
{
	omni_mutex_lock sync(*this);

	ring.insert_data(res,when);
	needed_time = needed;
}

void PollObj::insert_data(Tango::AttributeValueList_4 *res, PollClock::time_point when, PollClock::duration needed)
{
	omni_mutex_lock sync(*this);

	ring.insert_data(res,when,true);
	needed_time = needed;
}

void PollObj::insert_data(Tango::AttributeValueList_5 *res, PollClock::time_point when, PollClock::duration needed)
{
	omni_mutex_lock sync(*this);

	ring.insert_data(res,when,true);
	needed_time = needed;
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::insert_except
//
// description :
//		This method insert a new element in the ring buffer when this element is an exception
//
// argument :
//		in :
//			- res : The DevFailed exception
//			- when : The date when the exception was thrown
//			- needed : The time needed for the command/attribute reading
//
//-------------------------------------------------------------------------------------------------------------------

void PollObj::insert_except(Tango::DevFailed *res,
			    PollClock::time_point when,
			    PollClock::duration needed)
{
	omni_mutex_lock sync(*this);

	ring.insert_except(res,when);
	needed_time = needed;
}


//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::get_last_insert_date
//
// description :
//		This method returns the date stored with the most recent record in the ring buffer (as a double in Sec)
//
//-------------------------------------------------------------------------------------------------------------------

PollClock::time_point PollObj::get_last_insert_date_i()
{
	return ring.get_last_insert_date();
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::get_last_cmd_result
//
// description :
//		This method returns the last data stored in ring for a polled command or throw an exception if the
//		command failed when it was executed
//
//-------------------------------------------------------------------------------------------------------------------

CORBA::Any *PollObj::get_last_cmd_result()
{
	omni_mutex_lock sync(*this);

	return ring.get_last_cmd_result();
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::get_last_attr_value
//
// description :
//		This method returns the last data stored in ring for a polled attribute or throw an exception if the
//		read attribuite operation failed when it was executed
//
//-------------------------------------------------------------------------------------------------------------------

Tango::AttributeValue &PollObj::get_last_attr_value(bool lock)
{
	if (lock == true)
		omni_mutex_lock sync(*this);

	return ring.get_last_attr_value();
}

Tango::AttributeValue_3 &PollObj::get_last_attr_value_3(bool lock)
{
	if (lock == true)
		omni_mutex_lock sync(*this);

	return ring.get_last_attr_value_3();
}

Tango::AttributeValue_4 &PollObj::get_last_attr_value_4(bool lock)
{
	if (lock == true)
		omni_mutex_lock sync(*this);

	return ring.get_last_attr_value_4();
}

Tango::AttributeValue_5 &PollObj::get_last_attr_value_5(bool lock)
{
	if (lock == true)
		omni_mutex_lock sync(*this);

	return ring.get_last_attr_value_5();
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::update_upd
//
// description :
//		This method update the polling update period
//
// argument :
//		in :
//			- new_upd : The new update period (in mS)
//
//-------------------------------------------------------------------------------------------------------------------

void PollObj::update_upd(PollClock::duration new_upd)
{
	upd = new_upd;
	max_delta_t = new_upd * dev->get_poll_old_factor();
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::get_cmd_history
//
// description :
//		This method get command history from the ring buffer
//
// argument :
//		in :
//			- n : recodr number
//			- ptr : Pointer to the sequence where command result should be stored
//
//-------------------------------------------------------------------------------------------------------------------

void PollObj::get_cmd_history(long n,Tango::DevCmdHistoryList *ptr)
{
	omni_mutex_lock sync(*this);

	ring.get_cmd_history(n,ptr);
}

void PollObj::get_cmd_history(long n,Tango::DevCmdHistory_4 *ptr,Tango::CmdArgType &loc_type)
{
	omni_mutex_lock sync(*this);

	ring.get_cmd_history(n,ptr,loc_type);
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollObj::get_attr_history
//
// description :
//		This method get command history from the ring buffer
//
// argument :
//		in :
//			- n : record number
//			- ptr : Pointer to the sequence where command result should be stored
//			- type : The attribute type
//
//-------------------------------------------------------------------------------------------------------------------

void PollObj::get_attr_history(long n,Tango::DevAttrHistoryList *ptr,long attr_type)
{
	omni_mutex_lock sync(*this);

	ring.get_attr_history(n,ptr,attr_type);
}

void PollObj::get_attr_history(long n,Tango::DevAttrHistoryList_3 *ptr,long attr_type)
{
	omni_mutex_lock sync(*this);

	ring.get_attr_history(n,ptr,attr_type);

//
// Add attribute name in case of the attribute failed when it was read.
// (This info is not stored in ring in case of attribute reading failure)
//

	for (long i = 0;i < n;i++)
	{
		if ((*ptr)[i].attr_failed == true)
		{
			(*ptr)[i].value.name = Tango::string_dup(name.c_str());
		}
	}
}

void PollObj::get_attr_history(long n,Tango::DevAttrHistory_4 *ptr,long attr_type,TANGO_UNUSED(AttrDataFormat attr_format))
{
	omni_mutex_lock sync(*this);

	ring.get_attr_history(n,ptr,attr_type);
}

void PollObj::get_attr_history(long n,Tango::DevAttrHistory_5 *ptr,long attr_type,AttrDataFormat attr_format)
{
	omni_mutex_lock sync(*this);

	ring.get_attr_history(n,ptr,attr_type);
	ptr->data_format = attr_format;
	ptr->data_type = attr_type;
}

void PollObj::get_attr_history_43(long n,Tango::DevAttrHistoryList_3 *ptr,long attr_type)
{
	omni_mutex_lock sync(*this);

	ring.get_attr_history_43(n,ptr,attr_type);
}


} // End of Tango namespace

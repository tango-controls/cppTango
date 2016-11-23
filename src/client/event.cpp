static const char *RcsId = "$Id: event.cpp 30155 2016-09-16 15:31:47Z bourtemb $";

//====================================================================================================================
//
//  file :  			event.cpp
//
//	description : 		C++ classes for implementing the event server and client singleton classes - EventConsumer.
//
//	author(s) : 		A.Gotz (goetz@esrf.fr)
//
//	original : 			7 April 2003
//
//  Copyright (C) :     2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision: 30155 $
//
//
//====================================================================================================================

#include <tango.h>
#include <tango/client/eventconsumer.h>
#include <tango/client/event.tpp>

#include <stdio.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace CORBA;

namespace Tango {


//+--------------------------------------------------------------------------------------------------------------------
//
// function :
//		leavefunc
//
// description :
//		This function will be executed at process exit or when the main returned.  It has to be executed to properly
//		shutdown and destroy the ORB used by as a server by the event system. The ORB loop is in EventConsumer thread.
//		Therefore, get a reference to it, shutdown the ORB and wait until the thread exit. It also destroys the
//		heartbeat filters
//
//--------------------------------------------------------------------------------------------------------------------

void leavefunc()
{
	// flag to avoid calling leavefunc twice
	static bool already_executed = false;

	Tango::ApiUtil *au = ApiUtil::instance();

	if (au->need_reset_already_flag() == true)
		already_executed = false;

//
// Kill locking threads (if any)
//

	if (already_executed == false)
		au->clean_locking_threads();

//
// Manage event stuff
//

	ZmqEventConsumer *zmq_ec = au->get_zmq_event_consumer();

	if (zmq_ec != NULL && already_executed == false)
	{
		zmq_ec->shutdown();
	}

//
// Shutdown and destroy the ORB
//

	if (already_executed == false)
	{
			TangORB_var orb = au->orb_provider()->get();
			orb->shutdown(true);
			orb->destroy();
		already_executed = true;
		au->need_reset_already_flag(false);
	}
}


void DelayedEventUnsubThread::run(TANGO_UNUSED(void *ptr))
{
	try
	{

//
// In case a callback is still in its execution, wait for it to terminate
//

		the_mon->timeout(3000);
		the_mon->get_monitor();
		the_mon->rel_monitor();

//
// Unsubscribe the event
//

		ev_cons->unsubscribe_event(-event_id);
	}
	catch(...) {}
}


void DelayedEventSubThread::run(TANGO_UNUSED(void *ptr))
{

//
// Subscribe to the event
//

    vector<string> v_s;
	try
	{
        DelayEvent de(ev_cons);
        WriterLock w(EventConsumer::get_map_modification_lock());

		ev_cons->connect_event(device,attribute,et,callback,ev_queue,v_s,event_name,ev_id);
	}
	catch (Tango::DevFailed &e)
	{

//
// when the subscribe event has not worked, store the connection data in a vector of not
// yet connected events.
// Retry to connect in the next heartbeat period.
//

		EventNotConnected conn_params;
		conn_params.device           = device;
		conn_params.attribute        = attribute;
		conn_params.event_type       = et;
		conn_params.event_name       = event_name;
		conn_params.callback         = callback;
		conn_params.ev_queue         = ev_queue;
		conn_params.filters          = v_s;
		conn_params.last_heartbeat   = time(NULL);
		conn_params.event_id = ev_id;

		ev_cons->add_not_connected_event(e,conn_params);
	}
}



/************************************************************************/
/*		       															*/
/* 			EventData class 											*/
/*			---------------												*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
// 	EventData constructor
//
//-----------------------------------------------------------------------

EventData::EventData(DeviceProxy *dev,string &nam,string &evt,
          Tango::DeviceAttribute *attr_value_in, DevErrorList &errors_in) :
          device(dev),attr_name(nam),event(evt),attr_value(attr_value_in),
          errors(errors_in)
{
	if (errors.length()==0)
		err=false;
	else err = true;

	set_time();
}

//+----------------------------------------------------------------------
//
// 	EventData copy constructor
//
//-----------------------------------------------------------------------

EventData::EventData(const EventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	if (sou.attr_value)
		attr_value = new DeviceAttribute(*(sou.attr_value));
	else
		attr_value = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	EventData assignement operator
//
//-----------------------------------------------------------------------

EventData & EventData::operator=(const EventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	if (ri.attr_value)
		attr_value = new DeviceAttribute(*(ri.attr_value));
	else
		attr_value = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;

	return *this;
}

//+----------------------------------------------------------------------
//
// 	EventData destructor
//
//-----------------------------------------------------------------------

EventData::~EventData()
{
    delete attr_value;
}


//+-------------------------------------------------------------------------
//
// method : 		EventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void EventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (DevLong)t.time;
		reception_date.tv_usec = (DevLong)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (DevLong)tv.tv_sec;
		reception_date.tv_usec = (DevLong)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}

FwdEventData::FwdEventData():EventData(),av_5(Tango_nullptr),event_data(Tango_nullptr)
{
}

FwdEventData::FwdEventData(DeviceProxy *dev,string &_s1,string &_s2,Tango::DeviceAttribute *_da,DevErrorList &_del) :
                  EventData(dev,_s1,_s2,_da,_del),av_5(Tango_nullptr),event_data(Tango_nullptr)
{
}

FwdEventData::FwdEventData(DeviceProxy *dev,string &_s1,string &_s2,Tango::DeviceAttribute *_da,DevErrorList &_del,zmq::message_t *_m) :
                  EventData(dev,_s1,_s2,_da,_del),av_5(Tango_nullptr),event_data(_m)
{
}
/************************************************************************/
/*		       															*/
/* 			AttrConfEventData class 									*/
/*			-----------------											*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  AttrConfEventData constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
                  Tango::AttributeInfoEx *attr_conf_in,DevErrorList &errors_in) :
                  device(dev),attr_name(nam),event(evt),attr_conf(attr_conf_in),
                  errors(errors_in)
{
	if (errors.length()==0)
		err=false;
	else
		err = true;

	set_time();
}

//+----------------------------------------------------------------------
//
//  AttrConfEventData copy constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(const AttrConfEventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	if (sou.attr_conf != NULL)
	{
		attr_conf = new (AttributeInfoEx);
		*attr_conf = *(sou.attr_conf);
	}
	else
		attr_conf = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	AttrConfEventData assignement operator
//
//-----------------------------------------------------------------------

AttrConfEventData & AttrConfEventData::operator=(const AttrConfEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	if (ri.attr_conf != NULL)
	{
		attr_conf = new (AttributeInfoEx);
		*attr_conf = *(ri.attr_conf);
	}
	else
		attr_conf = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;

	return *this;
}

//+----------------------------------------------------------------------
//
// 	AttrConfEventData destructor
//
//-----------------------------------------------------------------------

AttrConfEventData::~AttrConfEventData()
{
    delete attr_conf;
}

//+-------------------------------------------------------------------------
//
// method : 		AttrConfEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void AttrConfEventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (DevLong)t.time;
		reception_date.tv_usec = (DevLong)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (DevLong)tv.tv_sec;
		reception_date.tv_usec = (DevLong)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}

FwdAttrConfEventData::FwdAttrConfEventData():AttrConfEventData(),fwd_attr_conf(Tango_nullptr)
{
}


FwdAttrConfEventData::FwdAttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
                  Tango::AttributeInfoEx *attr_conf_in,DevErrorList &errors_in) :
                  AttrConfEventData(dev,nam,evt,attr_conf_in,errors_in),fwd_attr_conf(Tango_nullptr)
{
}

/************************************************************************/
/*		       															*/
/* 			DataReadyEventData class 									*/
/*			------------------											*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  DataReadyEventData constructor
//
//-----------------------------------------------------------------------

DataReadyEventData::DataReadyEventData(DeviceProxy *dev,AttDataReady *dr,string &evt,DevErrorList &errors_in)
:event(evt),errors(errors_in)
{
	device = dev;
	if (dr != NULL)
	{
		attr_name = dr->name.in();
		attr_data_type = dr->data_type;
		ctr = dr->ctr;
	}
	else
	{
		attr_name = "Unknown";
		attr_data_type = -1;
		ctr = -1;
	}

	if (errors.length()==0)
		err = false;
	else
		err = true;
}

//+----------------------------------------------------------------------
//
//  DataReadyEventData copy constructor
//
//-----------------------------------------------------------------------

DataReadyEventData::DataReadyEventData(const DataReadyEventData &sou)
{
	device = sou.device;
	ctr = sou.ctr;
	attr_name = sou.attr_name;
	event = sou.event;
	attr_data_type = sou.attr_data_type;
	reception_date = sou.reception_date;
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	DataReadyEventData assignement operator
//
//-----------------------------------------------------------------------

DataReadyEventData & DataReadyEventData::operator=(const DataReadyEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	ctr = ri.ctr;
	attr_data_type = ri.attr_data_type;
	attr_name = ri.attr_name;
	event = ri.event;

	reception_date = ri.reception_date;

	err = ri.err;
	errors = ri.errors;

	return *this;
}

//+-------------------------------------------------------------------------
//
// method : 		DataReadyEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void DataReadyEventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (DevLong)t.time;
		reception_date.tv_usec = (DevLong)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (DevLong)tv.tv_sec;
		reception_date.tv_usec = (DevLong)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}

/************************************************************************/
/*		       															*/
/* 			DevIntrChangeEventData class 								*/
/*			----------------------										*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  DevIntrChangeEventData constructor
//
//-----------------------------------------------------------------------

DevIntrChangeEventData::DevIntrChangeEventData(DeviceProxy *dev,string &evt,string &d_name,
								DevCmdInfoList_2 *c_list,AttributeConfigList_5 *a_list,
								bool d_s,DevErrorList &errors_in)
:event(evt),device_name(d_name),dev_started(d_s),errors(errors_in)
{
	device = dev;

	if (errors.length()==0)
		err = false;
	else
		err = true;

	if (err == false)
	{

//
// Convert first command list and then attribute list
//

		cmd_list.resize(c_list->length());
		for (size_t i=0; i < c_list->length(); i++)
		{
			cmd_list[i].cmd_name = (*c_list)[i].cmd_name;
			cmd_list[i].cmd_tag = (*c_list)[i].cmd_tag;
			cmd_list[i].in_type = (*c_list)[i].in_type;
			cmd_list[i].out_type = (*c_list)[i].out_type;
			cmd_list[i].in_type_desc = (*c_list)[i].in_type_desc.in();
			cmd_list[i].out_type_desc = (*c_list)[i].out_type_desc.in();
			cmd_list[i].disp_level = (*c_list)[i].level;
		}

		att_list.resize(a_list->length());
		AttributeConfigList_5_var a_list_var(a_list);

		for (size_t i=0; i<a_list->length(); i++)
		{
			COPY_BASE_CONFIG(att_list,a_list_var)

			for (size_t j=0; j<(*a_list)[i].sys_extensions.length(); j++)
			{
				att_list[i].sys_extensions[j] = (*a_list)[i].sys_extensions[j];
			}
			att_list[i].disp_level = (*a_list)[i].level;
			att_list[i].min_alarm = (*a_list)[i].att_alarm.min_alarm;
			att_list[i].max_alarm = (*a_list)[i].att_alarm.max_alarm;
			att_list[i].root_attr_name = (*a_list)[i].root_attr_name;
			if ((*a_list)[i].memorized == false)
				att_list[i].memorized	= NONE;
			else
			{
				if ((*a_list)[i].mem_init == false)
					att_list[i].memorized	= MEMORIZED;
				else
					att_list[i].memorized	= MEMORIZED_WRITE_INIT;
			}

			COPY_ALARM_CONFIG(att_list,a_list_var)

			COPY_EVENT_CONFIG(att_list,a_list_var)
		}

		a_list_var._retn();
	}
}

DevIntrChangeEventData::DevIntrChangeEventData(DeviceProxy *dev,string &evt,string &d_name,
								CommandInfoList *c_list,AttributeInfoListEx *a_list,
								bool d_s,DevErrorList &errors_in)
:event(evt),device_name(d_name),dev_started(d_s),errors(errors_in)
{
	device = dev;

	if (errors.length()==0)
		err = false;
	else
		err = true;

	if (err == false)
	{
		cmd_list = *c_list;
		att_list = *a_list;
	}
}

//+----------------------------------------------------------------------
//
//  DevIntrChangeEventData copy constructor
//
//-----------------------------------------------------------------------

DevIntrChangeEventData::DevIntrChangeEventData(const DevIntrChangeEventData &sou)
{
	device = sou.device;
	event = sou.event;
	device_name = sou.device_name;
	cmd_list = sou.cmd_list;
	att_list = sou.att_list;
	dev_started = sou.dev_started;
	reception_date = sou.reception_date;
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	DevIntrChangeEventData assignement operator
//
//-----------------------------------------------------------------------

DevIntrChangeEventData & DevIntrChangeEventData::operator=(const DevIntrChangeEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	event = ri.event;
	device_name = ri.device_name;
	cmd_list = ri.cmd_list;
	att_list = ri.att_list;
	dev_started = ri.dev_started;
	reception_date = ri.reception_date;

	err = ri.err;
	errors = ri.errors;

	return *this;
}

//+-------------------------------------------------------------------------
//
// method : 		DevIntrChangeEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void DevIntrChangeEventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (DevLong)t.time;
		reception_date.tv_usec = (DevLong)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (DevLong)tv.tv_sec;
		reception_date.tv_usec = (DevLong)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}

/************************************************************************/
/*		       															*/
/* 			PipepEventData class 										*/
/*			---------------												*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
// 	PipeEventData constructor
//
//-----------------------------------------------------------------------

PipeEventData::PipeEventData(DeviceProxy *dev,string &nam,string &evt,
          Tango::DevicePipe *pipe_value_in, DevErrorList &errors_in) :
          device(dev),pipe_name(nam),event(evt),pipe_value(pipe_value_in),
          errors(errors_in)
{
	if (errors.length()==0)
		err=false;
	else err = true;

	set_time();
}

//+----------------------------------------------------------------------
//
// 	PipeEventData copy constructor
//
//-----------------------------------------------------------------------

PipeEventData::PipeEventData(const PipeEventData &sou)
{
	device = sou.device;
	pipe_name = sou.pipe_name;
	event = sou.event;
	if (sou.pipe_value)
		pipe_value = new DevicePipe(*(sou.pipe_value));
	else
		pipe_value = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	PipeEventData assignement operator
//
//-----------------------------------------------------------------------

PipeEventData & PipeEventData::operator=(const PipeEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	pipe_name = ri.pipe_name;
	event = ri.event;
	if (ri.pipe_value)
		pipe_value = new DevicePipe(*(ri.pipe_value));
	else
		pipe_value = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;

	return *this;
}

//+----------------------------------------------------------------------
//
// 	PipeEventData destructor
//
//-----------------------------------------------------------------------

PipeEventData::~PipeEventData()
{
    delete pipe_value;
}


//+-------------------------------------------------------------------------
//
// method : 		PipeEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void PipeEventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (DevLong)t.time;
		reception_date.tv_usec = (DevLong)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (DevLong)tv.tv_sec;
		reception_date.tv_usec = (DevLong)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}

} /* End of Tango namespace */


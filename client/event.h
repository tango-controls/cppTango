////////////////////////////////////////////////////////////////////////////////
///
///  file       event.h
///
/// 	        C++ include file for implementing the TANGO event server and
///		        client singleton classes - EventSupplier and EventConsumer.
///             These classes are used to send events from the server
///             to the notification service and to receive events from
///             the notification service.
///
/// 		author(s) : A.Gotz (goetz@esrf.fr)
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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
///
/// 		original : 7 April 2003
///
/// 		$Revision$
///
/// 		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _EVENTAPI_H
#define _EVENTAPI_H

#include <attribute.h>
#include <except.h>

namespace Tango
{

#ifndef _USRDLL
extern "C"
{
#endif
void leavefunc();
void client_leavefunc();
#ifndef _USRDLL
}
#endif

#define 	CONF_TYPE_EVENT			"attr_conf"
#define		DATA_READY_TYPE_EVENT	"data_ready"
#define     ALL_EVENTS				0

/********************************************************************************
 * 																				*
 * 						EventData class											*
 * 																				*
 *******************************************************************************/

class EventData
{
public :
	EventData(DeviceProxy *dev,string &nam,string &evt,Tango::DeviceAttribute *attr_value_in,
	DevErrorList &errors_in);
	~EventData();
	EventData(const EventData &);
	EventData & operator=(const EventData &);

	DeviceProxy *device;
	string attr_name;
	string event;
	DeviceAttribute *attr_value;
	bool err;
	DevErrorList errors;
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}
private:
	void set_time();
};


/********************************************************************************
 * 																				*
 * 						EventDataList class										*
 * 																				*
 *******************************************************************************/
class EventDataList:public vector<EventData *>
{
public:
	EventDataList(): vector<EventData *>(0) {};
	~EventDataList()
		{
		if (size() > 0)
			{
			EventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}
			}
		}
	void clear()
		{
		if (size() > 0)
			{
			EventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}

			this->vector<EventData *>::clear();
			}
		}
};


/********************************************************************************
 * 																				*
 * 						AttrConfEventData class									*
 * 																				*
 *******************************************************************************/

class AttrConfEventData
{
public :
	AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
	                  Tango::AttributeInfoEx *attr_conf_in,
	                  DevErrorList &errors_in);
	~AttrConfEventData();
	AttrConfEventData(const AttrConfEventData &);
	AttrConfEventData & operator=(const AttrConfEventData &);

	DeviceProxy *device;
	string attr_name;
	string event;
	AttributeInfoEx *attr_conf;
	bool err;
	DevErrorList errors;
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}
private:
	void set_time();
};



/********************************************************************************
 * 																				*
 * 						AttrConfEventDataList class								*
 * 																				*
 *******************************************************************************/
class AttrConfEventDataList:public vector<AttrConfEventData *>
{
public:
	AttrConfEventDataList(): vector<AttrConfEventData *>(0) {};
	~AttrConfEventDataList()
		{
		if (size() > 0)
			{
			AttrConfEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}
			}
		}
	void clear()
		{
		if (size() > 0)
			{
			AttrConfEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
				{
				delete (*vpos);
				}

			this->vector<AttrConfEventData *>::clear();
			}
		}
};

/********************************************************************************
 * 																				*
 * 						DataReadyEventData class								*
 * 																				*
 *******************************************************************************/

class DataReadyEventData
{
public :
	DataReadyEventData(DeviceProxy *,AttDataReady *,string &evt,DevErrorList &);
	~DataReadyEventData() {};
	DataReadyEventData(const DataReadyEventData &);
	DataReadyEventData & operator=(const DataReadyEventData &);

	DeviceProxy 	*device;
	string 			attr_name;
	string			event;
	int 			attr_data_type;
	int 			ctr;

	bool 			err;
	DevErrorList 	errors;

	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal 	reception_date;
	Tango::TimeVal &get_date() {return reception_date;}
private:
	void set_time();
};

/********************************************************************************
 * 																				*
 * 						DataReadyEventDataList class							*
 * 																				*
 *******************************************************************************/

class DataReadyEventDataList:public vector<DataReadyEventData *>
{
public:
	DataReadyEventDataList(): vector<DataReadyEventData *>(0) {};
	~DataReadyEventDataList()
	{
		if (size() > 0)
		{
			DataReadyEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
			{
				delete (*vpos);
			}
		}
	}
	void clear()
	{
		if (size() > 0)
		{
			DataReadyEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); vpos++)
			{
				delete (*vpos);
			}

			this->vector<DataReadyEventData *>::clear();
		}
	}
}
;


/********************************************************************************
 * 																				*
 * 						EventQueue class										*
 * 																				*
 *******************************************************************************/
class EventQueue
{
public:
	EventQueue();
	EventQueue(long max_size);
	~EventQueue();

	void insert_event(EventData         	*new_event);
	void insert_event(AttrConfEventData 	*new_event);
	void insert_event(DataReadyEventData 	*new_event);

	int      size();
	TimeVal get_last_event_date();
	bool     is_empty() {if (event_buffer.size() == 0) return true;else return false;}

	void get_events(EventDataList         	&event_list);
	void get_events(AttrConfEventDataList 	&event_list);
	void get_events(DataReadyEventDataList	&event_list);
	void get_events(CallBack *cb);

private:
	void inc_indexes();

	vector<EventData *>         	event_buffer;
	vector<AttrConfEventData *> 	conf_event_buffer;
	vector<DataReadyEventData *>	ready_event_buffer;

	long	max_elt;
	long	insert_elt;
	long	nb_elt;

	omni_mutex	modification_mutex;
};


} // End of namespace


#endif // _EVENTAPI_H

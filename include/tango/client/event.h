//===================================================================================================================
//
//  file :      	event.h
//
// 	description :   C++ include file for implementing the TANGO event server and client singleton classes -
//					EventSupplier and EventConsumer.
//             		These classes are used to send events from the server to the notification service and to receive
//					events from the notification service.
//
//  author(s) : 	A.Gotz (goetz@esrf.fr)
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//===================================================================================================================

#ifndef _EVENTAPI_H
#define _EVENTAPI_H

#include <tango/server/attribute.h>
#include <tango/server/except.h>
#include <tango/server/tango_const.h>

#include <zmq.hpp>

namespace Tango
{
using Lock = std::unique_lock<std::mutex>;
#ifndef _USRDLL
extern "C"
{
#endif
void leavefunc();
#ifndef _USRDLL
}
#endif

/********************************************************************************
 * 																				*
 * 						EventData class											*
 * 																				*
 *******************************************************************************/

/**
 * Event callback execution data
 *
 * This class is used to pass data to the callback method when an event is sent to the client
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class EventData
{
public :
///@privatesection
	EventData() {}
	EventData(DeviceProxy *dev,string &nam,string &evt,Tango::DeviceAttribute *attr_value_in,DevErrorList &errors_in);

	~EventData();
	EventData(const EventData &);
	EventData & operator=(const EventData &);
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}

///@publicsection
	DeviceProxy     *device;        ///< The DeviceProxy object on which the call was executed
	string          attr_name;      ///< The attribute name
	string          event;          ///< The event name
	DeviceAttribute *attr_value;    ///< The attribute data
	bool            err;            ///< A boolean flag set to true if the request failed. False otherwise
	DevErrorList    errors;         ///< The error stack

private:
	void set_time();
};

class FwdEventData: public EventData
{
public:
	FwdEventData();
	FwdEventData(DeviceProxy *,string &,string &,Tango::DeviceAttribute *,DevErrorList &);
	FwdEventData(DeviceProxy *,string &,string &,Tango::DeviceAttribute *,DevErrorList &,zmq::message_t *);

	void set_av_5(const AttributeValue_5 *_p) {av_5 = _p;}
	const AttributeValue_5 *get_av_5() {return av_5;}
	zmq::message_t *get_zmq_mess_ptr() {return event_data;}

private:
	const AttributeValue_5	*av_5;
	zmq::message_t			*event_data;
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
			for (vpos=begin(); vpos!=end(); ++vpos)
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
			for (vpos=begin(); vpos!=end(); ++vpos)
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

/**
 * Attribute configuration change event callback execution data
 *
 * This class is used to pass data to the callback method when an attribute configuration event is sent to the
 * client
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class AttrConfEventData
{
public :
///@privatesection
	AttrConfEventData() {}
	AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
	                  Tango::AttributeInfoEx *attr_conf_in,
	                  DevErrorList &errors_in);
	~AttrConfEventData();
	AttrConfEventData(const AttrConfEventData &);
	AttrConfEventData & operator=(const AttrConfEventData &);
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}

///@publicsection
	DeviceProxy     *device;        ///< The DeviceProxy object on which the call was executed
	string          attr_name;      ///< The attribute name
	string          event;          ///< The event name
	AttributeInfoEx *attr_conf;     ///< The attribute configuration
	bool            err;            ///< A boolean flag set to true if the request failed. False otherwise
	DevErrorList    errors;         ///< The error stack

private:
	void set_time();
};


class FwdAttrConfEventData: public AttrConfEventData
{
public:
	FwdAttrConfEventData();
	FwdAttrConfEventData(DeviceProxy *,string &,string &,Tango::AttributeInfoEx *,DevErrorList &);

	void set_fwd_attr_conf(const AttributeConfig_5 *_p) {fwd_attr_conf = _p;}
	const AttributeConfig_5 *get_fwd_attr_conf() {return fwd_attr_conf;}

private:
	const AttributeConfig_5	*fwd_attr_conf;
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
			for (vpos=begin(); vpos!=end(); ++vpos)
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
			for (vpos=begin(); vpos!=end(); ++vpos)
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

/**
 * Data ready event callback execution data
 *
 * This class is used to pass data to the callback method when an attribute data ready event is sent to the client.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DataReadyEventData
{
public :
///@privatesection
	DataReadyEventData() {}
	DataReadyEventData(DeviceProxy *,AttDataReady *,string &evt,DevErrorList &);
	~DataReadyEventData() {};
	DataReadyEventData(const DataReadyEventData &);
	DataReadyEventData & operator=(const DataReadyEventData &);
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal 	reception_date;
	Tango::TimeVal &get_date() {return reception_date;}

///@publicsection
	DeviceProxy 	*device;            ///< The DeviceProxy object on which the call was executed
	string 			attr_name;          ///< The attribute name
	string			event;              ///< The event name
	int 			attr_data_type;     ///< The attribute data type
	int 			ctr;                ///< The user counter. Set to 0 if not defined when sent by the server

	bool 			err;                ///< A boolean flag set to true if the request failed. False otherwise
	DevErrorList 	errors;             ///< The error stack

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
			for (vpos=begin(); vpos!=end(); ++vpos)
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
			for (vpos=begin(); vpos!=end(); ++vpos)
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
 * 						DevIntrChangeEventData class							*
 * 																				*
 *******************************************************************************/

/**
 * Device interface change event callback execution data
 *
 * This class is used to pass data to the callback method when a device interface change event is sent to the client.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class DevIntrChangeEventData
{
public :
///@privatesection
	DevIntrChangeEventData() {}
	DevIntrChangeEventData(DeviceProxy *,string &,string &,DevCmdInfoList_2 *,AttributeConfigList_5 *,bool,DevErrorList &);
	DevIntrChangeEventData(DeviceProxy *,string &,string &,CommandInfoList *,AttributeInfoListEx *,bool,DevErrorList &);
	~DevIntrChangeEventData() {};
	DevIntrChangeEventData(const DevIntrChangeEventData &);
	DevIntrChangeEventData & operator=(const DevIntrChangeEventData &);
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal 	reception_date;
	Tango::TimeVal &get_date() {return reception_date;}

///@publicsection
	DeviceProxy 		*device;            ///< The DeviceProxy object on which the call was executed
	string				event;              ///< The event name
	string				device_name;		///< The device name
	CommandInfoList 	cmd_list;			///< Device command list info
	AttributeInfoListEx	att_list;			///< Device attribute list info
	bool				dev_started;		///< Device started flag (true when event sent due to device being (re)started
											///< and with only a possible but not sure interface change)

	bool 				err;                ///< A boolean flag set to true if the request failed. False otherwise
	DevErrorList 		errors;            	///< The error stack

private:
	void set_time();
};

/********************************************************************************
 * 																				*
 * 						DevIntrChangeEventDataList class						*
 * 																				*
 *******************************************************************************/

class DevIntrChangeEventDataList:public vector<DevIntrChangeEventData *>
{
public:
	DevIntrChangeEventDataList(): vector<DevIntrChangeEventData *>(0) {};
	~DevIntrChangeEventDataList()
	{
		if (size() > 0)
		{
			DevIntrChangeEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); ++vpos)
			{
				delete (*vpos);
			}
		}
	}
	void clear()
	{
		if (size() > 0)
		{
			DevIntrChangeEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); ++vpos)
			{
				delete (*vpos);
			}

			this->vector<DevIntrChangeEventData *>::clear();
		}
	}
};

/********************************************************************************
 * 																				*
 * 						PipeEventData class										*
 * 																				*
 *******************************************************************************/

/**
 * Pipe event callback execution data
 *
 * This class is used to pass data to the callback method when a pipe event is sent to the client
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class PipeEventData
{
public :
///@privatesection
	PipeEventData() {}
	PipeEventData(DeviceProxy *dev,string &nam,string &evt,Tango::DevicePipe *pipe_value_in,DevErrorList &errors_in);

	~PipeEventData();
	PipeEventData(const PipeEventData &);
	PipeEventData & operator=(const PipeEventData &);
	/**
	 * The date when the event arrived
	 */
	Tango::TimeVal reception_date;
	Tango::TimeVal &get_date() {return reception_date;}

///@publicsection
	DeviceProxy     *device;        ///< The DeviceProxy object on which the call was executed
	string          pipe_name;      ///< The pipe name
	string          event;          ///< The event name
	DevicePipe 		*pipe_value;   	///< The pipe data
	bool            err;            ///< A boolean flag set to true if the request failed. False otherwise
	DevErrorList    errors;         ///< The error stack

private:
	void set_time();
};

/********************************************************************************
 * 																				*
 * 						PipeEventDataList class									*
 * 																				*
 *******************************************************************************/

class PipeEventDataList:public vector<PipeEventData *>
{
public:
	PipeEventDataList(): vector<PipeEventData *>(0) {};
	~PipeEventDataList()
	{
		if (size() > 0)
		{
			PipeEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); ++vpos)
			{
				delete (*vpos);
			}
		}
	}
	void clear()
	{
		if (size() > 0)
		{
			PipeEventDataList::iterator vpos;
			for (vpos=begin(); vpos!=end(); ++vpos)
			{
				delete (*vpos);
			}

			this->vector<PipeEventData *>::clear();
		}
	}
};


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

	void insert_event(EventData         		*new_event);
	void insert_event(AttrConfEventData 		*new_event);
	void insert_event(DataReadyEventData 		*new_event);
	void insert_event(DevIntrChangeEventData 	*new_event);
	void insert_event(PipeEventData 			*new_event);

	int      size();
	TimeVal get_last_event_date();
	bool     is_empty() {if (event_buffer.empty() == true) return true;else return false;}

	void get_events(EventDataList         		&event_list);
	void get_events(AttrConfEventDataList 		&event_list);
	void get_events(DataReadyEventDataList		&event_list);
	void get_events(DevIntrChangeEventDataList  &event_list);
	void get_events(PipeEventDataList  			&event_list);
	void get_events(CallBack *cb);

private:
	void inc_indexes();

	vector<EventData *>         		event_buffer;
	vector<AttrConfEventData *> 		conf_event_buffer;
	vector<DataReadyEventData *>		ready_event_buffer;
	vector<DevIntrChangeEventData *>	dev_inter_event_buffer;
	vector<PipeEventData *>				pipe_event_buffer;

	long	max_elt;
	long	insert_elt;
	long	nb_elt;

	omni_mutex	modification_mutex;
};


} // End of namespace


#endif // _EVENTAPI_H

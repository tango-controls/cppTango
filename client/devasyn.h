//
// devsyn.h - include file for TANGO api device asynchronous calls
//
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


#ifndef _DEVASYN_H
#define _DEVASYN_H

#include <tango.h>

#include <map>

using namespace std;

namespace Tango {


//------------------------------------------------------------------------------

class DeviceProxy;
class Connection;
class DeviceData;
class DeviceAttribute;
class NamedDevFailedList;
class EventData;
class AttrConfEventData;
class DataReadyEventData;
class EventDataList;
class AttrConfEventDataList;
class DataReadyEventDataList;
class EventConsumer;
class EventConsumerKeepAliveThread;

/********************************************************************************
 *                                                                              *
 * 						CmdDoneEvent class										*
 * 																				*
 *******************************************************************************/

class CmdDoneEvent
{
public:
	CmdDoneEvent(DeviceProxy *dev,
		     string &cmd,
		     DeviceData &arg,
		     DevErrorList &err_in):device(dev),
					   cmd_name(cmd),
					   argout(arg),
					   errors(err_in)
	{if (errors.length()==0) err=false;else err=true;}

	Tango::DeviceProxy 	*device;
	string				&cmd_name;
	DeviceData			&argout;
	bool				err;
	DevErrorList		&errors;

private:
    class CmdDoneEventExt
    {
    public:
        CmdDoneEventExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<CmdDoneEventExt> ext;
#else
	CmdDoneEventExt		        *ext;
#endif
};

/********************************************************************************
 * 																				*
 * 						AttrReadEvent class										*
 * 																				*
 *******************************************************************************/

class AttrReadEvent
{
public:
	AttrReadEvent(DeviceProxy *dev,
		      vector<string> &att_names,
		      vector<DeviceAttribute> *arg,
		      DevErrorList &err_in):device(dev),
					    attr_names(att_names),
					    argout(arg),
					    errors(err_in)
	{if (errors.length()==0) err=false;else err=true;}

	Tango::DeviceProxy 		*device;
	vector<string>			&attr_names;
	vector<DeviceAttribute>	*argout;
	bool					err;
	DevErrorList			&errors;

private:
    class AttrReadEventExt
    {
    public:
        AttrReadEventExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<AttrReadEventExt>    ext;
#else
	AttrReadEventExt		        *ext;
#endif
};

/********************************************************************************
 * 																				*
 * 						AttrWrittenEvent class									*
 * 																				*
 *******************************************************************************/

class AttrWrittenEvent
{
public:
	AttrWrittenEvent(DeviceProxy *dev,
			 vector<string> &att_names,
			 NamedDevFailedList &err_in):device(dev),
			 		       attr_names(att_names),
					       errors(err_in)
	{err = errors.call_failed();}

	Tango::DeviceProxy	*device;
	vector<string>		&attr_names;
	bool				err;
	NamedDevFailedList	&errors;

private:
    class AttrWrittenEventExt
    {
    public:
        AttrWrittenEventExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<AttrWrittenEventExt> ext;
#else
	AttrWrittenEventExt	            *ext;
#endif
};

/********************************************************************************
 * 																				*
 * 						CallBack class											*
 * 																				*
 *******************************************************************************/

class CallBack
{

friend class EventConsumer;
friend class EventConsumerKeepAliveThread;

public:
	virtual ~CallBack() {};

	virtual void cmd_ended(CmdDoneEvent *) {};
	virtual void attr_read(AttrReadEvent *) {};
	virtual void attr_written(AttrWrittenEvent *) {};
	virtual void push_event(EventData *) {};
	virtual void push_event(AttrConfEventData *) {};
	virtual void push_event(DataReadyEventData *) {};

private:
    class CallBackExt
    {
    public:
        CallBackExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<CallBackExt> ext;
#else
	CallBackExt		        *ext;
#endif
};

//------------------------------------------------------------------------------

class UniqIdent: public omni_mutex
{
public:
	UniqIdent() {omni_mutex_lock(*this);ctr = 0;}
	long get_ident() {omni_mutex_lock sync(*this);return ++ctr;}

	long ctr;
};

class TgRequest
{
public:
	enum ReqType
	{
		CMD_INOUT,
		READ_ATTR,
		WRITE_ATTR_SINGLE,
		WRITE_ATTR
	};

	TgRequest(CORBA::Request_ptr re,ReqType ty):request(re),req_type(ty),cb_ptr(NULL),
						    					arrived(false),dev(NULL)
	{};

	TgRequest(CORBA::Request_ptr re,ReqType ty,CallBack *cb):request(re),req_type(ty),cb_ptr(cb),
							 								 arrived(false),dev(NULL)
	{};

	TgRequest(Tango::Connection *con,ReqType ty,CallBack *cb):request(NULL),req_type(ty),cb_ptr(cb),
							 								  arrived(false),dev(con)
	{};

	CORBA::Request_ptr	request;
	ReqType				req_type;
	CallBack			*cb_ptr;
	bool				arrived;
	Connection			*dev;
};

class AsynReq: public omni_mutex
{
public:
	AsynReq(UniqIdent *ptr):ui_ptr(ptr),cond(this) {};
	~AsynReq() {delete ui_ptr;}

	TgRequest &get_request(long);
	TgRequest &get_request(CORBA::Request_ptr);
	TgRequest *get_request(Tango::Connection *);

	long store_request(CORBA::Request_ptr,TgRequest::ReqType);
	void store_request(CORBA::Request_ptr,CallBack *,Connection *,TgRequest::ReqType);

	void remove_request(long);
	void remove_request(Connection *,CORBA::Request_ptr);

	size_t get_request_nb() {omni_mutex_lock(*this);return asyn_poll_req_table.size();}
	size_t get_cb_request_nb() {omni_mutex_lock(*this);return cb_req_table.size();}
	size_t get_cb_request_nb_i() {return cb_req_table.size();}

	void mark_as_arrived(CORBA::Request_ptr req);
	multimap<Connection *,TgRequest> &get_cb_dev_table() {return cb_dev_table;}

	void mark_as_cancelled(long);
	void mark_all_polling_as_cancelled();
	void wait() {cond.wait();}
	void signal() {omni_mutex_lock(*this);cond.signal();}

protected:
	map<long,TgRequest>			asyn_poll_req_table;
	UniqIdent 					*ui_ptr;

	multimap<Connection *,TgRequest>	cb_dev_table;
	map<CORBA::Request_ptr,TgRequest>	cb_req_table;

	vector<long>				cancelled_request;

private:
	omni_condition				cond;
	bool remove_cancelled_request(long);
};

} // End of Tango namespace

#endif /* _DEVASYN_H */

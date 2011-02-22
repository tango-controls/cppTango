//
// devsyn.h - include file for TANGO api device asynchronous calls 
//
// 

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
class EventConsumer;
class EventConsumerKeepAliveThread;

class CmdDoneEventExt;
class AttrReadEventExt;
class AttrWrittenEventExt;
class CallBackExt;

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
	string			&cmd_name;
	DeviceData		&argout;
	bool			err;
	DevErrorList		&errors;
	
	CmdDoneEventExt		*ext;
};

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

	Tango::DeviceProxy 	*device;
	vector<string>		&attr_names;
	vector<DeviceAttribute>	*argout;
	bool			err;
	DevErrorList		&errors;
	
	AttrReadEventExt	*ext;
};

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
	bool			err;
	NamedDevFailedList	&errors;
	
	AttrWrittenEventExt	*ext;
};

class CallBack
{

friend class EventConsumer;
friend class EventConsumerKeepAliveThread;

public:
	virtual void cmd_ended(CmdDoneEvent *) {};
	virtual void attr_read(AttrReadEvent *) {};
	virtual void attr_written(AttrWrittenEvent *) {};
	virtual void push_event(EventData *) {};
	
	CallBackExt		*ext;
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
	
	TgRequest(CORBA::Request_ptr re,ReqType ty):request(re),
						    req_type(ty),
						    cb_ptr(NULL),
						    arrived(false),
						    dev(NULL)
	{};
	TgRequest(CORBA::Request_ptr re,ReqType ty,CallBack *cb):request(re),
						    	  req_type(ty),
						    	  cb_ptr(cb),
							  arrived(false),
							  dev(NULL)
	{};
	TgRequest(Tango::Connection *con,ReqType ty,CallBack *cb):request(NULL),
						    	  req_type(ty),
						    	  cb_ptr(cb),
							  arrived(false),
							  dev(con)
	{};
		
	CORBA::Request_ptr	request;
	ReqType			req_type;
	CallBack		*cb_ptr;
	bool			arrived;
	Connection		*dev;
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
	
	long get_request_nb() {omni_mutex_lock(*this);return asyn_poll_req_table.size();}
	long get_cb_request_nb() {omni_mutex_lock(*this);return cb_req_table.size();}
	size_t get_cb_request_nb_i() {return cb_req_table.size();}
	
	void mark_as_arrived(CORBA::Request_ptr req);
	multimap<Connection *,TgRequest> &get_cb_dev_table() {return cb_dev_table;}

	void wait() {cond.wait();}
	void signal() {omni_mutex_lock(*this);cond.signal();}
			
protected:	
	map<long,TgRequest>			asyn_poll_req_table;
	UniqIdent 				*ui_ptr;
	
	multimap<Connection *,TgRequest>	cb_dev_table;
	map<CORBA::Request_ptr,TgRequest>	cb_req_table;
	
private:
	omni_condition				cond;
};

//-------------------------------------------------------------------------

// 
// Some extension classes foreseen for library binary compatibility
//

class CmdDoneEventExt
{
public:
	CmdDoneEventExt() {};
};

class AttrReadEventExt
{
public:
	AttrReadEventExt() {};
};

class AttrWrittenEventExt
{
public:
	AttrWrittenEventExt() {};
};

class CallBackExt
{
public:
	CallBackExt() {};
};


} // End of Tango namespace

#endif /* _DEVASYN_H */

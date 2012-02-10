////////////////////////////////////////////////////////////////////////////////
///
///  file       eventconsumer.h
///
/// 	        C++ include file for implementing the TANGO event related
///		        client classes - EventConsumer and others.
///             These classes are used to receive events from the server
///             and from the notification service.
///
///  author(s) : E.Taurel (taurel@esrf.fr)
//
// Copyright (C) :      2011,2012
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
/// 		original : 7 November 2011
///
/// 		$Revision$
///
/// 		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _EVENTCONSUMER_H
#define _EVENTCONSUMER_H

#include <attribute.h>
#include <except.h>
#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>
#include <omnithread.h>
#include <map>

#include <readers_writers_lock.h>

#include <zmq.hpp>


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
 * 						ZMQ event unmarshalling related classes					*
 * 																				*
 *******************************************************************************/


/***            TangoCdrMemoryStream                ***/

class TangoCdrMemoryStream : public cdrMemoryStream
{
public:
    TangoCdrMemoryStream(void *buf,size_t si):cdrMemoryStream(buf,si) {}
    TangoCdrMemoryStream():cdrMemoryStream(512,false) {}

//    void get_octet_array(_CORBA_Octet*,int,omni::alignment_t align=omni::ALIGN_1);
    void tango_get_octet_array(int size);
    void *get_end_out_buf() {return pd_outb_end;}
};

inline void TangoCdrMemoryStream::tango_get_octet_array(int size)
{
    pd_inb_mkr = (void*)((char *)pd_inb_mkr+size);
}

/***            ZmqAttrValUnion               ***/

class ZmqAttrValUnion:public AttrValUnion
{
public:
    void operator<<= (TangoCdrMemoryStream &);

    template <typename T,typename TA>
    void init_seq(char *,_CORBA_ULong &,TangoCdrMemoryStream &);

    template <typename T>
    void set_seq(T &) {cerr << "In default ZmqAttrValUnion::set_seq!" << endl;assert(false);}

    template <typename T>
    T &get_seq() {cerr << "In default ZmqAttrValUnion::get_seq!" << endl;assert(false);}
};

/***            ZmqAttributeValue_4               ***/

struct ZmqAttributeValue_4:public AttributeValue_4
{
    ZmqAttrValUnion     zvalue;
    void operator<<= (TangoCdrMemoryStream &);
};

/***    Macros to help coding       ***/

#ifndef Swap16
#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))
#else
#error "Swap16 has already been defined"
#endif

#ifndef Swap32
#define Swap32(l) ((((l) & 0xff000000) >> 24) | \
		   (((l) & 0x00ff0000) >> 8)  | \
		   (((l) & 0x0000ff00) << 8)  | \
		   (((l) & 0x000000ff) << 24))
#else
#error "Swap32 has already been defined"
#endif

// These template specialization allow us to
// set or get sequences within a AttrValUnion
// union. Union method to get/set sequence
// value are named short_att_value(), double_att_value(),
// float_att_value()....

#define SEQ_METH(NAME,TYPE) \
template <> \
inline void ZmqAttrValUnion::set_seq<TYPE>(TYPE &val) {NAME##_att_value(val);} \
\
template <> \
inline TYPE &ZmqAttrValUnion::get_seq<TYPE>() {return NAME##_att_value();}

/*** Macros call to generate required template specialisation   ***/

SEQ_METH(short,DevVarShortArray)
SEQ_METH(double,DevVarDoubleArray)
SEQ_METH(float,DevVarFloatArray)
SEQ_METH(ushort,DevVarUShortArray)
SEQ_METH(bool,DevVarBooleanArray)
SEQ_METH(long,DevVarLongArray)
SEQ_METH(long64,DevVarLong64Array)
SEQ_METH(ulong,DevVarULongArray)
SEQ_METH(uchar,DevVarUCharArray)
SEQ_METH(ulong64,DevVarULong64Array)
SEQ_METH(state,DevVarStateArray)

/**     Template methods definition     ***/

//
// Create a dummy empty sequence and init union
// with this dummy sequence (minimun data copy)
// Retrieve a reference to that sequence once it is
// in the union and replace its data pointer
// by the data in the CdrMemoryStream
// Use set_seq(), get_seq() specialized template
// created just above to set/get union sequences
//
// Also manage big and litle endian!!
//

template <typename T,typename TA>
inline void ZmqAttrValUnion::init_seq(char *base_ptr,_CORBA_ULong &length,TangoCdrMemoryStream &_n)
{
    TA dummy_val;
    set_seq<TA>(dummy_val);

    T *ptr = (T *)(base_ptr + _n.currentInputPtr());
    if (_n.unmarshal_byte_swap() == true)
    {
        if (sizeof(T) == 2)
        {
            for (_CORBA_ULong i = 0;i < length;i++)
            {
                _CORBA_UShort *tmp_ptr = (_CORBA_UShort *)ptr;
                _CORBA_UShort tt = *(tmp_ptr + i);
                *(tmp_ptr + i) = Swap16(tt);
            }
        }
        else if (sizeof(T) == 4)
        {
            for (_CORBA_ULong i = 0;i < length;i++)
            {
               _CORBA_ULong *tmp_ptr = (_CORBA_ULong *)ptr;
                _CORBA_ULong t = *(tmp_ptr + i);
                *(tmp_ptr + i) = Swap32(t);
            }
        }
        else if (sizeof(T) == 8)
        {
            _CORBA_ULong double_length = length * 2;
            for (_CORBA_ULong i = 0;i < double_length;i += 2)
            {
                _CORBA_ULong *tmp_ptr = (_CORBA_ULong *)ptr;
                _CORBA_ULong tl1 = *(tmp_ptr + i + 1);
                _CORBA_ULong tl2 = Swap32(tl1);
                tl1 = *(tmp_ptr + i);
                *(tmp_ptr + i) = tl2;
                *(tmp_ptr + i + 1) = Swap32(tl1);
            }
        }
    }

    TA &the_seq = get_seq<TA>();
    the_seq.replace(length,length,ptr,false);

    _n.tango_get_octet_array((length * sizeof(T)));
}

/********************************************************************************
 * 																				*
 * 						KeepAliveThCmd class									*
 * 																				*
 *******************************************************************************/

class KeepAliveThCmd:public omni_mutex
{
public :
	KeepAliveThCmd():cond(this) {};

	bool				cmd_pending;	// The new command flag
	KeepAliveCmdCode	cmd_code;		// The command code
	omni_condition 		cond;
};


/********************************************************************************
 * 																				*
 * 						Map structures      									*
 * 																				*
 *******************************************************************************/

typedef struct event_not_connected
{
	DeviceProxy 					*device;
	string 							attribute;
	EventType 						event_type;
	string 							event_name;
	int 						 	event_id;
	CallBack 						*callback;
	EventQueue                  	*ev_queue;
	vector<string> 					filters;
	time_t 							last_heartbeat;
} EventNotConnected;

typedef struct event_subscribe
{
	EventQueue						*ev_queue;
	CallBack						*callback;
	int								id;
} EventSubscribeStruct;

//------------------------ Event Callback related info --------------------------------------

typedef struct event_callback_base
{
 	DeviceProxy 					*device;
	string 							attr_name;
	string 							event_name;
	string 							channel_name;
	string                          fully_qualified_event_name;
	time_t 							last_subscribed;
	TangoMonitor					*callback_monitor;
	vector<EventSubscribeStruct>	callback_list;
} EventCallBackBase;

typedef struct event_callback_zmq
{
    DevLong                         device_idl;
    DevULong                        ctr;
}EventCallBackZmq;

typedef struct event_callback: public EventCallBackBase, public EventCallBackZmq
{
	string 							filter_constraint;
	CosNotifyFilter::FilterID 		filter_id;
	bool 							filter_ok;
} EventCallBackStruct;

//------------------------ Event Channel related info --------------------------------------

typedef struct event_channel_base
{
 	DeviceProxy 					*adm_device_proxy;
	string 							full_adm_name;
	time_t 							last_subscribed;
	time_t 							last_heartbeat;
	bool 							heartbeat_skipped;
	TangoMonitor					*channel_monitor;
	ChannelType                     channel_type;
} EventChannelBase;

typedef struct channel_struct: public EventChannelBase
{
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var structuredProxyPushSupplier;
	CosNotifyFilter::FilterID 		heartbeat_filter_id;
	string 							notifyd_host;
	bool 							notifd_failed;
	long 							has_notifd_closed_the_connection;
} EventChannelStruct;

typedef std::map<std::string,EventChannelStruct>::iterator EvChanIte;
typedef std::map<std::string,EventCallBackStruct>::iterator EvCbIte;


/********************************************************************************
 * 																				*
 * 						EventConsumer class										*
 * 																				*
 *******************************************************************************/

class EventConsumer
{

	typedef void    (*EventCallbackFunction)(string event_name,string event_type,Tango::DeviceAttribute *attr_value);

public :
	EventConsumer(ApiUtil *ptr);
	virtual ~EventConsumer() {}

	int connect_event(DeviceProxy *,const string &,EventType,CallBack *,EventQueue *,const vector<string> &,string &,int event_id = 0);
	void connect(DeviceProxy *,string &,DeviceData &,string &);

	void shutdown();
	void shutdown_keep_alive_thread();
    ChannelType get_event_system_for_event_id(int);
	virtual void cleanup_EventChannel_map() = 0;
    virtual void get_subscription_command_name(string &) = 0;

	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   CallBack *callback, const vector<string> &filters, bool stateless = false);
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   int event_queue_size, const vector<string> &filters, bool stateless = false);
	void unsubscribe_event(int event_id);

	// methods to access data in event queues

	void get_events (int event_id, EventDataList &event_list);
	void get_events (int event_id, AttrConfEventDataList &event_list);
	void get_events (int event_id, DataReadyEventDataList &event_list);
	void get_events (int event_id, CallBack *cb);
	int  event_queue_size(int event_id);
	TimeVal get_last_event_date(int event_id);
	bool is_event_queue_empty(int event_id);


	static KeepAliveThCmd                                   cmd;
	static EventConsumerKeepAliveThread 	                *keep_alive_thread;

protected :
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   CallBack *callback, EventQueue *ev_queue,
					   const vector<string> &filters, bool stateless = false);
	friend class EventConsumerKeepAliveThread;
	void attr_to_device(const AttributeValue *,const AttributeValue_3 *,long,DeviceAttribute *);
	void attr_to_device(const AttributeValue_4 *,DeviceAttribute *);
    void attr_to_device(const ZmqAttributeValue_4 *,DeviceAttribute *);
    void att_union_to_device(const AttrValUnion *union_ptr,DeviceAttribute *dev_attr);
	void conf_to_info(AttributeConfig_2 &,AttributeInfoEx **);

	static map<std::string,std::string> 					device_channel_map;     // key - device_name, value - channel name
	static map<std::string,EventChannelStruct> 				channel_map;            // key - channel_name, value - Event Channel info
	static map<std::string,EventCallBackStruct> 			event_callback_map;     // key - callback_key, value - Event CallBack info
	static ReadersWritersLock 								map_modification_lock;

	static vector<EventNotConnected> 						event_not_connected;
	static int 												subscribe_event_id; 	// unique event id
	static vector<string> 									env_var_fqdn_prefix;

	static omni_mutex										ev_consumer_inst_mutex;

	string													device_name;
	string 													att_name_lower;
	string													callback_key;

	int add_new_callback(EvCbIte &,CallBack *,EventQueue *,int);
	void get_fire_sync_event(DeviceProxy *,CallBack *,EventQueue *,EventType,string &,const string &,EventCallBackStruct &);

	virtual void connect_event_channel(string &,Database *,bool,DeviceData &) = 0;
    virtual void disconnect_event_channel(TANGO_UNUSED(string &channel_name)) {}
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &) = 0;
    virtual void disconnect_event(string &) {}

    virtual void set_channel_type(EventChannelStruct &) = 0;
};

/********************************************************************************
 * 																				*
 * 						NotifdEventConsumer class								*
 * 																				*
 *******************************************************************************/


class NotifdEventConsumer : public POA_CosNotifyComm::StructuredPushConsumer ,
                            public EventConsumer ,
                            public omni_thread
{
public :
	static NotifdEventConsumer *create();
    TANGO_IMP_EXP static void cleanup() {if (_instance != NULL){_instance=NULL;}}

	void push_structured_event(const CosNotification::StructuredEvent&);
	virtual void cleanup_EventChannel_map();

	void disconnect_structured_push_consumer();
	void offer_change(const CosNotification::EventTypeSeq &,const CosNotification::EventTypeSeq &);

    virtual void get_subscription_command_name(string &cmd) {cmd="EventSubscriptionChange";}

	CORBA::ORB_var 					orb_;

protected :
	NotifdEventConsumer(ApiUtil *ptr);
	virtual void connect_event_channel(string &,Database *,bool,DeviceData &);
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &);

    virtual void set_channel_type(EventChannelStruct &ecs) {ecs.channel_type = NOTIFD;}

private :

	TANGO_IMP static NotifdEventConsumer 					*_instance;

	CosNotifyChannelAdmin::EventChannel_var 				eventChannel;
	CosNotifyChannelAdmin::ConsumerAdmin_var 				consumerAdmin;
	CosNotifyChannelAdmin::ProxyID 							proxyId;
	CosNotifyChannelAdmin::ProxySupplier_var 				proxySupplier;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var	structuredProxyPushSupplier;
	CosNotifyChannelAdmin::EventChannelFactory_var 			eventChannelFactory;

	void *run_undetached(void *arg);
};


/********************************************************************************
 * 																				*
 * 						ZmqEventConsumer class  								*
 * 																				*
 *******************************************************************************/

class ZmqEventConsumer : public EventConsumer ,
                         public omni_thread
{
public :
	static ZmqEventConsumer *create();
    TANGO_IMP_EXP static void cleanup() {if (_instance != NULL){_instance=NULL;}}

	virtual void cleanup_EventChannel_map();
    virtual void get_subscription_command_name(string &cmd) {cmd="ZmqEventSubscriptionChange";}

	enum UserDataEventType
	{
	    ATT_CONF = 0,
	    ATT_READY,
	    ATT_VALUE
	};

protected :
	ZmqEventConsumer(ApiUtil *ptr);
	virtual void connect_event_channel(string &,Database *,bool,DeviceData &);
    virtual void disconnect_event_channel(string &channel_name);
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &);
    virtual void disconnect_event(string &);

    virtual void set_channel_type(EventChannelStruct &ecs) {ecs.channel_type = ZMQ;}

private :
	TANGO_IMP static ZmqEventConsumer       *_instance;
	zmq::context_t                          zmq_context;            // ZMQ context
	zmq::socket_t                           *heartbeat_sub_sock;    // heartbeat subscriber socket
	zmq::socket_t                           *control_sock;          // control socket
	zmq::socket_t                           *event_sub_sock;        // event subscriber socket

	map<string,zmq::socket_t *>             event_mcast;            // multicast socket(s)
	vector<string>                          connected_pub;          //
	vector<string>                          connected_heartbeat;    //

    AttributeValue_var                      av;
    AttributeValue_3_var                    av3;
    ZmqAttributeValue_4                     zav4;
    AttributeConfig_2_var                   ac2;
    AttributeConfig_3_var                   ac3;
    AttDataReady_var                        adr;
    DevErrorList_var                        del;

    int                                     old_poll_nb;

	void *run_undetached(void *arg);
	void push_heartbeat_event(string &);
    void push_zmq_event(string &,unsigned char,zmq::message_t &,bool,const DevULong &);
    bool process_ctrl(zmq::message_t &,zmq::pollitem_t *,int &);
    void process_heartbeat(zmq::message_t &,zmq::message_t &,zmq::message_t &);
    void process_event(zmq::message_t &,zmq::message_t &,zmq::message_t &,zmq::message_t &);
    void process_event(zmq_msg_t &,zmq_msg_t &,zmq_msg_t &,zmq_msg_t &);

    friend class DelayEvent;
};

class DelayEvent
{
public:
    DelayEvent(EventConsumer *);
    ~DelayEvent();

    void release();

private:
    bool                released;
    ZmqEventConsumer    *eve_con;
};

/********************************************************************************
 * 																				*
 * 						EventConsumerKeepAliveThread class						*
 * 																				*
 *******************************************************************************/

class EventConsumerKeepAliveThread : public omni_thread
{

public :

    EventConsumerKeepAliveThread(const EventConsumer&);
	EventConsumerKeepAliveThread(KeepAliveThCmd &cmd):shared_cmd(cmd){};
	void start() {start_undetached();}

protected :
	KeepAliveThCmd		&shared_cmd;

private :
	void *run_undetached(void *arg);
	bool reconnect_to_channel(EvChanIte &,EventConsumer *);
	void reconnect_to_event(EvChanIte &,EventConsumer *);
	void re_subscribe_event(EvCbIte &,EvChanIte &);
    void stateless_subscription_failed(vector<EventNotConnected>::iterator &,DevFailed &,time_t &);
};

/********************************************************************************
 * 																				*
 * 						DelayedEventUnsubThread class							*
 * 																				*
 *******************************************************************************/

class DelayedEventUnsubThread: public omni_thread
{
public:
	DelayedEventUnsubThread(EventConsumer *ec,int id,TangoMonitor *m):omni_thread(),event_id(id),ev_cons(ec),the_mon(m) {}

	void run(void *);

private:
	int 			event_id;
	EventConsumer 	*ev_cons;
	TangoMonitor	*the_mon;
};


} // End of namespace


#endif // _EVENTCONSUMER_H

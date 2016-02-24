//====================================================================================================================
//
// file :      		eventconsumer.h
//
// description :  	C++ include file for implementing the TANGO event related client classes - EventConsumer and others.
//             		These classes are used to receive events from the server and from the notification service.
//
// author(s) : 		E.Taurel (taurel@esrf.fr)
//
// Copyright (C) :  2011,2012,2013,2014,2015
//					European Synchrotron Radiation Facility
//                  BP 220, Grenoble 38043
//                  FRANCE
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
//====================================================================================================================

#ifndef _EVENTCONSUMER_H
#define _EVENTCONSUMER_H

#include <attribute.h>
#include <except.h>
#include <tango_const.h>
#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>
#include <omnithread.h>
#include <map>

#include <readers_writers_lock.h>

#include <zmq.hpp>

#ifdef ZMQ_VERSION
	#if ZMQ_VERSION > 30201
		#define ZMQ_HAS_DISCONNECT
	#endif
#endif


namespace Tango
{

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
 * 						ZMQ event unmarshalling related classes					*
 * 																				*
 *******************************************************************************/


/***            TangoCdrMemoryStream                ***/

class TangoCdrMemoryStream : public cdrMemoryStream
{
public:
	enum Unmarshal_type
	{
		UN_ATT,
		UN_PIPE
	};

    TangoCdrMemoryStream(void *buf,size_t si):cdrMemoryStream(buf,si) {}
    TangoCdrMemoryStream():cdrMemoryStream(512,false) {}

    void tango_get_octet_array(int size);
    void *get_end_out_buf() {return pd_outb_end;}
    void *get_mkr_out_buf() {return pd_outb_mkr;}
    void *get_end_in_buf() {return pd_inb_end;}
    void *get_mkr_in_buf() {return pd_inb_mkr;}

    void set_mkr_in_buf(void *_ptr) {pd_inb_mkr=_ptr;}

    void rewind_in(int _nb) {pd_inb_mkr = (char *)pd_inb_mkr - _nb;}
    void set_un_marshal_type(Unmarshal_type _ty) {un_type=_ty;}
    Unmarshal_type get_un_marshal_type() {return un_type;}

    omni::ptr_arith_t align_to(omni::ptr_arith_t p,omni::alignment_t align);

protected:
	Unmarshal_type		un_type;
};

inline omni::ptr_arith_t TangoCdrMemoryStream::align_to(omni::ptr_arith_t p,omni::alignment_t align)
{
    return (p + ((omni::ptr_arith_t) align - 1)) & ~((omni::ptr_arith_t) align - 1);
}

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

/***            ZmqAttributeValue_5               ***/

struct ZmqAttributeValue_5:public AttributeValue_5
{
    ZmqAttrValUnion     zvalue;
    void operator<<= (TangoCdrMemoryStream &);
};

/***            ZmqDevPipeData              	***/

struct ZmqDevPipeData:public DevPipeData
{
    void operator<<= (TangoCdrMemoryStream &);
};

/***            ZmqDevPipeBlob             		***/

struct ZmqDevPipeBlob:public DevPipeBlob
{
    void operator<<= (TangoCdrMemoryStream &);
};

/***            ZmqDevVarPipeDataEltArray     	***/

class ZmqDevVarPipeDataEltArray:public DevVarPipeDataEltArray
{
public:
    void operator<<= (TangoCdrMemoryStream &);
};

/***            ZmqDevPipeDataElt              	***/

struct ZmqDevPipeDataElt:public DevPipeDataElt
{
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

//
// These template specialization allow us to set or get sequences within a AttrValUnion union.
// Union method to get/set sequence value are named short_att_value(), double_att_value(),
// float_att_value()....
//

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
// Create a dummy empty sequence and init union with this dummy sequence (minimun data copy)
// Retrieve a reference to that sequence once it is in the union and replace its data pointer
// by the data in the CdrMemoryStream.
// Use set_seq(), get_seq() specialized template created just above to set/get union sequences
// T is the basic data type (DevShort)
// TA is the sequence data type (DevVarShortArray)
//
// Also manage big and litle endian!!
//

template <typename T,typename TA>
inline void ZmqAttrValUnion::init_seq(char *base_ptr,_CORBA_ULong &length,TangoCdrMemoryStream &_n)
{
    TA dummy_val;
    set_seq<TA>(dummy_val);

    T *ptr;
	if (_n.get_un_marshal_type() == TangoCdrMemoryStream::UN_ATT)
		ptr = (T *)(base_ptr + _n.currentInputPtr());
	else
	{
		char *tmp = (char *)_n.get_mkr_in_buf();
		int delta = tmp - base_ptr;
		ptr = (T *)(base_ptr + delta);
	}

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
	KeepAliveThCmd():cmd_pending(false),cond(this) {};

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
	string							prefix;
} EventNotConnected;

//------------------------ Event Callback related info --------------------------------------

typedef struct event_subscribe
{
	EventQueue						*ev_queue;
	CallBack						*callback;
	int								id;
} EventSubscribeStruct;

typedef struct event_callback_base
{
 	DeviceProxy 					*device;
	string 							obj_name;
	string 							event_name;
	string 							channel_name;
	string                          fully_qualified_event_name;
	time_t 							last_subscribed;
	TangoMonitor					*callback_monitor;
	vector<EventSubscribeStruct>	callback_list;
	bool							alias_used;
} EventCallBackBase;

typedef struct event_callback_zmq
{
    DevLong                         device_idl;
    DevULong                        ctr;
    string							endpoint;
    bool							discarded_event;
    bool							fwd_att;
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

typedef struct event_channel_zmq
{
    string                        	endpoint;
    size_t                          valid_endpoint;
}EventChannelZmq;

typedef struct channel_struct: public EventChannelBase, public EventChannelZmq
{
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var structuredProxyPushSupplier;
	CosNotifyFilter::FilterID 		heartbeat_filter_id;
	string 							notifyd_host;
	bool 							event_system_failed;
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
	void connect(DeviceProxy *,string &,DeviceData &,string &,bool &);

	void shutdown();
	void shutdown_keep_alive_thread();
    ChannelType get_event_system_for_event_id(int);
	virtual void cleanup_EventChannel_map() = 0;
    virtual void get_subscription_command_name(string &) = 0;

	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   CallBack *callback, const vector<string> &filters, bool stateless = false);
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event,
	                   int event_queue_size, const vector<string> &filters, bool stateless = false);
	int subscribe_event(DeviceProxy *device, EventType event,CallBack *callback,bool stateless = false);
	int subscribe_event(DeviceProxy *device, EventType event,int event_queue_size,bool stateless = false);

	void unsubscribe_event(int event_id);

	// methods to access data in event queues

	void get_events (int event_id, EventDataList &event_list);
	void get_events (int event_id, AttrConfEventDataList &event_list);
	void get_events (int event_id, DataReadyEventDataList &event_list);
	void get_events (int event_id, DevIntrChangeEventDataList &event_list);
	void get_events (int event_id, PipeEventDataList &event_list);
	void get_events (int event_id, CallBack *cb);
	int  event_queue_size(int event_id);
	TimeVal get_last_event_date(int event_id);
	bool is_event_queue_empty(int event_id);
    int get_thread_id() {return thread_id;}
    void add_not_connected_event(DevFailed &,EventNotConnected &);
	static ReadersWritersLock &get_map_modification_lock() {return map_modification_lock;};

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
    void attr_to_device(const ZmqAttributeValue_5 *,DeviceAttribute *);
	template <typename T> void base_attr_to_device(const T *,DeviceAttribute *);
    void att_union_to_device(const AttrValUnion *union_ptr,DeviceAttribute *dev_attr);
	void conf_to_info(AttributeConfig_2 &,AttributeInfoEx **);
	void get_cs_tango_host(Database *);

	static map<std::string,std::string> 					device_channel_map;     // key - device_name, value - channel name (full adm name)
	static map<std::string,EventChannelStruct> 				channel_map;            // key - channel_name (full adm name), value - Event Channel info
	static map<std::string,EventCallBackStruct> 			event_callback_map;     // key - callback_key, value - Event CallBack info
	static ReadersWritersLock 								map_modification_lock;

	static vector<EventNotConnected> 						event_not_connected;
	static int 												subscribe_event_id; 	// unique event id
	static vector<string> 									env_var_fqdn_prefix;
	static map<std::string,std::string>						alias_map;				// key - real host name, value - alias

	static omni_mutex										ev_consumer_inst_mutex;

	string													device_name;
	string 													obj_name_lower;
    int                                                     thread_id;

	int add_new_callback(EvCbIte &,CallBack *,EventQueue *,int);
	void get_fire_sync_event(DeviceProxy *,CallBack *,EventQueue *,EventType,string &,const string &,EventCallBackStruct &,string &);

	virtual void connect_event_channel(string &,Database *,bool,DeviceData &) = 0;
    virtual void disconnect_event_channel(TANGO_UNUSED(string &channel_name),TANGO_UNUSED(string &endpoint),TANGO_UNUSED(string &endpoint_event)) {}
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &,size_t) = 0;
    virtual void disconnect_event(string &,string &) {}

    virtual void set_channel_type(EventChannelStruct &) = 0;
    virtual void zmq_specific(DeviceData &,string &,DeviceProxy *,const string &) = 0;
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
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &,size_t);

    virtual void set_channel_type(EventChannelStruct &ecs) {ecs.channel_type = NOTIFD;}
	virtual void zmq_specific(DeviceData &,string &,DeviceProxy *,const string &) {}

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
	    ATT_VALUE,
	    DEV_INTR,
	    PIPE
	};

    enum SocketCmd
    {
        SUBSCRIBE = 0,
        UNSUBSCRIBE
    };

protected :
	ZmqEventConsumer(ApiUtil *ptr);
	virtual void connect_event_channel(string &,Database *,bool,DeviceData &);
    virtual void disconnect_event_channel(string &channel_name,string &endpoint,string &endpoint_event);
    virtual void connect_event_system(string &,string &,string &e,const vector<string> &,EvChanIte &,EventCallBackStruct &,DeviceData &,size_t);
    virtual void disconnect_event(string &,string &);

    virtual void set_channel_type(EventChannelStruct &ecs) {ecs.channel_type = ZMQ;}
	virtual void zmq_specific(DeviceData &,string &,DeviceProxy *,const string &);

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
    ZmqAttributeValue_5						zav5;
    AttributeConfig_2_var                   ac2;
    AttributeConfig_3_var                   ac3;
    AttributeConfig_5_var					ac5;
    AttDataReady_var                        adr;
    DevIntrChange_var						dic;
    ZmqDevPipeData							zdpd;
    DevErrorList_var                        del;

    int                                     old_poll_nb;
    TangoMonitor							subscription_monitor;
    omni_mutex                              sock_bound_mutex;
    bool									ctrl_socket_bound;


	void *run_undetached(void *arg);
	void push_heartbeat_event(string &);
    void push_zmq_event(string &,unsigned char,zmq::message_t &,bool,const DevULong &);
    bool process_ctrl(zmq::message_t &,zmq::pollitem_t *,int &);
    void process_heartbeat(zmq::message_t &,zmq::message_t &,zmq::message_t &);
    void process_event(zmq::message_t &,zmq::message_t &,zmq::message_t &,zmq::message_t &);
    void process_event(zmq_msg_t &,zmq_msg_t &,zmq_msg_t &,zmq_msg_t &);
    void multi_tango_host(zmq::socket_t *,SocketCmd,string &);
	void print_error_message(const char *mess) {ApiUtil *au=ApiUtil::instance();au->print_error_message(mess);}
	void set_ctrl_sock_bound() {sock_bound_mutex.lock();ctrl_socket_bound=true;sock_bound_mutex.unlock();}
	bool is_ctrl_sock_bound() {bool _b;sock_bound_mutex.lock();_b=ctrl_socket_bound;sock_bound_mutex.unlock();return _b;}

    bool check_zmq_endpoint(const string &);

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
    void stateless_subscription_failed(vector<EventNotConnected>::iterator &,DevFailed &,time_t &);
    void fwd_not_conected_event(ZmqEventConsumer *);

protected :
	KeepAliveThCmd		&shared_cmd;

private :
	void *run_undetached(void *arg);
	bool reconnect_to_channel(EvChanIte &,EventConsumer *);
	void reconnect_to_event(EvChanIte &,EventConsumer *);
	void re_subscribe_event(EvCbIte &,EvChanIte &);

    bool reconnect_to_zmq_channel(EvChanIte &,EventConsumer *,DeviceData &);
	void reconnect_to_zmq_event(EvChanIte &,EventConsumer *,DeviceData &);
	void not_conected_event(ZmqEventConsumer *,time_t,NotifdEventConsumer *);
	void confirm_subscription(ZmqEventConsumer *,map<string,EventChannelStruct>::iterator &);
	void main_reconnect(ZmqEventConsumer *,NotifdEventConsumer *,map<string,EventCallBackStruct>::iterator &,map<string,EventChannelStruct>::iterator &);
	void re_subscribe_after_reconnect(ZmqEventConsumer *,NotifdEventConsumer *,map<string,EventCallBackStruct>::iterator &,map<string,EventChannelStruct>::iterator &,string &);
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

/********************************************************************************
 * 																				*
 * 						DelayedEventSubThread class							    *
 * 																				*
 *******************************************************************************/

class DelayedEventSubThread: public omni_thread
{
public:
	DelayedEventSubThread(EventConsumer *ec,DeviceProxy *_device,
				   const string &_attribute,
				   EventType _event,
				   CallBack *_callback,
				   EventQueue *_ev_queue,
				   bool _stateless,
				   const string &_ev_name,
				   int _id):omni_thread(),ev_cons(ec),device(_device),
				   attribute(_attribute),et(_event),callback(_callback),ev_queue(_ev_queue),
				   stateless(_stateless),ev_id(_id),event_name(_ev_name) {}

	void run(void *);

private:
	EventConsumer 	*ev_cons;
	DeviceProxy     *device;
	string          attribute;
	EventType       et;
	CallBack        *callback;
	EventQueue      *ev_queue;
	bool            stateless;
	int             ev_id;
	string          event_name;
};

} // End of namespace


#endif // _EVENTCONSUMER_H

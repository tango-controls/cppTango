////////////////////////////////////////////////////////////////////////////////
///
///  file       event.h
///
/// 	        C++ include file for implementing the TANGO event server and 
///		client singleton classes - EventSupplier and EventConsumer.
///             These classes are used to send events from the server
///             to the notification service and to receive events from
///             the notification service.
///
/// 		author(s) : A.Gotz (goetz@esrf.fr)
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
#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>
#include <omnithread.h>
#include <map>



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

//
// event_callback_list - data structure for managing event consumers subscriptions
//

class EventData
{
public :

	EventData(DeviceProxy *dev,
                  string &nam,
		  string &evt,
		  Tango::DeviceAttribute *attr_value_in,
		  DevErrorList &errors_in) :
                         	device(dev),
                                attr_name(nam),
				event(evt),
				attr_value(attr_value_in),
				errors(errors_in)
        	{if (errors.length()==0) err=false;else err = true;}
		
	~EventData();
	EventData(const EventData &);
	EventData & operator=(const EventData &);
	
	DeviceProxy *device;
	string attr_name;
	string event;
	DeviceAttribute *attr_value;
	bool err;
	DevErrorList errors;
};

class KeepAliveThCmd:public omni_mutex
{
public :
	KeepAliveThCmd():cond(this) {};
	
	bool			cmd_pending;	// The new command flag
	KeepAliveCmdCode	cmd_code;	// The command code
	omni_condition 		cond;
};

typedef struct event_callback
{
	DeviceProxy *device;
	string attr_name;
	string event_name;
	string channel_name;
	string filter_constraint;
	int last_subscribed;
	CallBack *callback;
	CosNotifyFilter::FilterID filter_id;
	bool filter_ok;
	int id;
} EventCallBackStruct;

typedef struct channel_struct
{
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var structuredProxyPushSupplier;
	DeviceProxy *adm_device_proxy;
	string full_adm_name;
	int last_subscribed;
	int last_heartbeat;
	bool heartbeat_skipped;
	CosNotifyFilter::FilterID heartbeat_filter_id;
} EventChannelStruct;
	
	
class EventConsumer : public POA_CosNotifyComm::StructuredPushConsumer ,
  		      public PortableServer::RefCountServantBase,
	      	      public omni_thread
{

	typedef void    (*EventCallbackFunction)(string event_name,string event_type,Tango::DeviceAttribute *attr_value);

public :
    	EventConsumer(const EventConsumer&);
	static EventConsumer *create();
    	void operator=(const EventConsumer&);
	virtual void push_structured_event(
		const CosNotification::StructuredEvent&);
	void connect(DeviceProxy* device_proxy);
	void disconnect_structured_push_consumer();
	void offer_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled);
	int subscribe_event(DeviceProxy *device, const string &attribute, EventType event, CallBack *callback, const vector<string> &filters);
	void unsubscribe_event(int event_id);
	void unsubscribe_device(DeviceProxy *device);
	void cleanup_heartbeat_filters();
	void cleanup_EventChannel_map();
	TANGO_IMP_EXP static void cleanup()
	{if (_instance != NULL){_instance=NULL;}}
	
	CORBA::ORB_var 			orb_;
	KeepAliveThCmd			cmd;
	EventConsumerKeepAliveThread 	*keep_alive_thread;

protected :
		
	friend class EventConsumerKeepAliveThread;
	void attr_to_device(const AttributeValue *,const AttributeValue_3 *,long,DeviceAttribute *);

	EventConsumer(ApiUtil *ptr);

private :

	TANGO_IMP static EventConsumer *_instance;

	void *run_undetached(void *arg);
	void connect_event_channel(string &channel_name,Database *,bool);
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::ConsumerAdmin_var consumerAdmin;
	CosNotifyChannelAdmin::ProxyID proxyId;
	CosNotifyChannelAdmin::ProxySupplier_var proxySupplier;
	CosNotifyChannelAdmin::StructuredProxyPushSupplier_var
            	structuredProxyPushSupplier;
	CosNotifyChannelAdmin::EventChannelFactory_var eventChannelFactory;
	
	map<std::string,std::string> device_channel_map;
	map<std::string,EventCallBackStruct> event_callback_map;
	map<std::string,EventChannelStruct> channel_map;
	
	omni_mutex	cb_map_mut;
	omni_mutex	channel_map_mut;
	
	
};

typedef std::map<std::string,EventChannelStruct>::iterator EvChanIte;
typedef std::map<std::string,EventCallBackStruct>::iterator EvCbIte;

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

};

} // End of namespace


#endif // _EVENTAPI_H

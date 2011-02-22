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

class EventSupplier : public POA_CosNotifyComm::StructuredPushSupplier ,
	              public PortableServer::RefCountServantBase
{
public :

	TANGO_IMP_EXP static EventSupplier *create(CORBA::ORB_var,string,Database*,string &);
	void connect();
	void disconnect_structured_push_supplier();
	void subscription_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled);
	void detect_and_push_events(DeviceImpl *, AttributeValue &,DevFailed *,string &);
	void detect_and_push_change_event(DeviceImpl *, AttributeValue &, Attribute&, string&, DevFailed *);
	void detect_and_push_archive_event(DeviceImpl *, AttributeValue &, Attribute&, string&, DevFailed *);
	void detect_and_push_quality_change_event(DeviceImpl *, AttributeValue &, Attribute&, string&, DevFailed *);
	void detect_and_push_periodic_event(DeviceImpl *, AttributeValue &, Attribute&, string&, DevFailed *);
	void push_event(DeviceImpl *,string, vector<string> &, vector<double> &a, vector<string> &, vector<long> &b,AttributeValue &, string &,DevFailed * ex=NULL);
	void push_heartbeat_event();

protected :

	EventSupplier(CORBA::ORB_var,
		CosNotifyChannelAdmin::SupplierAdmin_var,
		CosNotifyChannelAdmin::ProxyID, 
		CosNotifyChannelAdmin::ProxyConsumer_var, 
		CosNotifyChannelAdmin::StructuredProxyPushConsumer_var, 
		CosNotifyChannelAdmin::EventChannelFactory_var,
		CosNotifyChannelAdmin::EventChannel_var);
		
private :
	static EventSupplier *_instance;
	CosNotifyChannelAdmin::EventChannel_var eventChannel;
	CosNotifyChannelAdmin::SupplierAdmin_var supplierAdmin;
	CosNotifyChannelAdmin::ProxyID proxyId;
	CosNotifyChannelAdmin::ProxyConsumer_var proxyConsumer;
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var
            	structuredProxyPushConsumer;
	CosNotifyChannelAdmin::EventChannelFactory_var eventChannelFactory;
	CORBA::ORB_var orb_;
	
	inline int timeval_diff(TimeVal before, TimeVal after)
	{
		return ((after.tv_sec-before.tv_sec)*1000000 + after.tv_usec - before.tv_usec);
	}
	int heartbeat_period;
	int subscription_timeout;
	void get_attribute_value(AttributeValue attr_value, LastAttrValue &curr_attr_value);
	bool detect_change(Attribute &,AttributeValue &,bool,double &, double &, DevFailed *,bool &);
};
	
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

	DeviceProxy *device;
	string &attr_name;
	string &event;
	DeviceAttribute *attr_value;
	bool err;
	DevErrorList &errors;
};

class KeepAliveThCmd:public omni_mutex
{
public :
	KeepAliveThCmd():cond(this) {};
	
	bool			cmd_pending;	// The new command flag
	KeepAliveCmdCode	cmd_code;	// The command code
	omni_condition 		cond;
};


class EventConsumer : public POA_CosNotifyComm::StructuredPushConsumer ,
  		      public PortableServer::RefCountServantBase,
	      	      public omni_thread
{

	typedef void    (*EventCallbackFunction)(string event_name,string event_type,Tango::DeviceAttribute *attr_value);

	typedef struct event_callback
	{
		DeviceProxy *device;
		string attr_name;
		string event_name;
		string channel_name;
		int last_subscribed;
		CallBack *callback;
		CosNotifyFilter::FilterID filter_id;
		int id;
       	} EventCallBackStruct;

	typedef struct channel_struct
	{
		CosNotifyChannelAdmin::EventChannel_var eventChannel;
		CosNotifyChannelAdmin::StructuredProxyPushSupplier_var structuredProxyPushSupplier;
		DeviceProxy *adm_device_proxy;
		int last_subscribed;
		int last_heartbeat;
		bool heartbeat_skipped;
		CosNotifyFilter::FilterID heartbeat_filter_id;
       	} EventChannelStruct;

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
	void attr_to_device(const AttributeValue *attr_value, DeviceAttribute *dev_attr);

	EventConsumer(ApiUtil *ptr);

private :

	TANGO_IMP static EventConsumer *_instance;

	void *run_undetached(void *arg);
	void connect_event_channel(string channel_name,Database *);
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
};

MAKE_EXCEPT(EventSystemFailed,EventSystemExcept)
MAKE_EXCEPT(EventIdInvalid,EventIdExcept)

} // End of namespace


#endif // _EVENTAPI_H

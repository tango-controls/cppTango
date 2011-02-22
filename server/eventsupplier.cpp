static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
///  file 	eventsupplier.cpp
///
///		C++ classes for implementing the event server and client
///		singleton classes - EventSupplier and EventConsumer.
///		These classes are used to send events from the server
///		to the notification service and to receive events from
///		the notification service. 
///
///		author(s) : E.Taurel (taurel@esrf.fr)
///
///		original : 29 June 2004
///
///		$Revision$
///
///		$Log$
///		Revision 1.1.2.1  2004/10/05 13:55:02  maxchiandone
///		First upload for filedatabase.
///		
///		Revision 1.1  2004/07/07 07:59:20  taurel
///		Added some files
///		
///
///		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>
#include <eventsupplier.h>

#include <COS/CosNotification.hh>
#include <COS/CosNotifyChannelAdmin.hh>
#include <COS/CosNotifyComm.hh>
#include <stdio.h>

#ifdef WIN32
#include <sys/timeb.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace CORBA;

namespace Tango {

EventSupplier *EventSupplier::_instance = NULL;

					   					   
/************************************************************************/
/*		       							*/		
/* 			EventSupplier class 				*/
/*			-------------------				*/
/*		       							*/
/************************************************************************/


EventSupplier::EventSupplier(CORBA::ORB_var _orb,
	CosNotifyChannelAdmin::SupplierAdmin_var _supplierAdmin,
	CosNotifyChannelAdmin::ProxyID _proxId,
	CosNotifyChannelAdmin::ProxyConsumer_var _proxyConsumer,
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var _structuredProxyPushConsumer,
	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory,
	CosNotifyChannelAdmin::EventChannel_var _eventChannel)
{
	orb_ = _orb;
	supplierAdmin = _supplierAdmin;
	proxyId = _proxId;
	proxyConsumer = _proxyConsumer;
	structuredProxyPushConsumer = _structuredProxyPushConsumer;
	eventChannelFactory = _eventChannelFactory;
	eventChannel = _eventChannel;

	_instance = this;
}

EventSupplier *EventSupplier::create(CORBA::ORB_var _orb,
				     string server_name,
				     Database *db,
				     string &host_name) 
{
	cout4 << "calling Tango::EventSupplier::create() \n";

//
// does the EventSupplier singleton exist already ? if so simply return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// Connect process to the notifd service
//

	NotifService ns;	
	connect_to_notifd(ns,_orb,server_name,db,host_name);
	
//
// EventSupplier singleton does not exist, create it
//

	EventSupplier *_event_supplier =
		new EventSupplier(_orb,ns.SupAdm,ns.pID,ns.ProCon,ns.StrProPush,ns.EveChaFac,ns.EveCha);
	
	return _event_supplier;
}

void EventSupplier::connect()
{
//
// Connect to the Proxy Consumer
//
	try
	{
	    structuredProxyPushConsumer -> connect_structured_push_supplier(_this());
	}
	catch(const CosEventChannelAdmin::AlreadyConnected&)
	{
       		cerr << "Tango::EventSupplier::connect() caught AlreadyConnected exception" << endl;
	}

}

void EventSupplier::disconnect_structured_push_supplier()
{
	cout4 << "calling Tango::EventSupplier::disconnect_structured_push_supplier() \n";
}

void EventSupplier::subscription_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled)
{
	cout4 << "calling Tango::EventSupplier::subscription_change() \n";
}

//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::connect_to_notifd()
// 
// description : 	Method to connect the process to the notifd
//
// argument : in :	ns : Ref. to a struct with notifd connection parameters
//
//-----------------------------------------------------------------------------

void EventSupplier::connect_to_notifd(NotifService &ns,CORBA::ORB_var &_orb,
				      string &server_name,
				      Database *db,
				      string &host_name)
{
	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory;
	CosNotifyChannelAdmin::EventChannel_var _eventChannel;

//
// Get a reference to the Notification Service EventChannelFactory from
// the TANGO database
//

	string d_name = "DServer/";
	d_name = d_name + server_name;
		
	string factory_name;
	factory_name = "notifd/factory/" + host_name;

        CORBA::Any send;
	CORBA::Any_var received;

        send <<= factory_name.c_str();
	try 
	{
        	received = db->command_inout("DbImportEvent",send);
	}
	catch (...) 
	{
	
//
// Impossible to connect to notifd. In case there is already an entry in the db
// for this server event channel, clear it.
//

        	send <<= d_name.c_str();
		try
		{
			db->command_inout("DbUnExportEvent",send);
		}
		catch (...) {}
				
//
// There is a cout and a cerr here to have the message displayed on the console
// AND sent to the logging system (cout is redirected to the logging when
// compiled with -D_TANGO_LIB)
//

		cerr << "Failed to import EventChannelFactory " << factory_name << " from the Tango database" << endl;
		cout << "Failed to import EventChannelFactory " << factory_name << " from the Tango database" << endl;
		
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to import the EventChannelFactory from the Tango database",
			(const char*)"EventSupplier::create()");    
	}
 
        const DevVarLongStringArray *dev_import_list;
        received.inout() >>= dev_import_list;
 
	string factory_ior;
	int factory_exported;

        factory_ior = string((dev_import_list->svalue)[1]);
        factory_exported = dev_import_list->lvalue[0];

	try
	{
		CORBA::Object *event_factory_obj;
    		event_factory_obj = _orb -> string_to_object(factory_ior.c_str());
#ifndef WIN32
    		if (event_factory_obj -> _non_existent())
			event_factory_obj = CORBA::Object::_nil();
#endif /* WIN32 */

//
// Narrow the CORBA_Object reference to an EventChannelFactory
// reference so we can invoke its methods
//

		_eventChannelFactory =
	    	CosNotifyChannelAdmin::EventChannelFactory::_narrow(event_factory_obj);

//
// Make sure the CORBA object was really an EventChannelFactory
//

		if(CORBA::is_nil(_eventChannelFactory))
		{
			cerr << factory_name << " is not an EventChannelFactory " << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to import the EventChannelFactory from the Tango database",
				(const char*)"EventSupplier::create()");    
		}
	}
	catch (...)
	{

//
// Impossible to connect to notifd. In case there is already an entry in the db
// for this server event channel, clear it.
//

        	send <<= d_name.c_str();
		try
		{
			db->command_inout("DbUnExportEvent",send);
		}
		catch (...) {}
			
//
// There is a cout and a cerr here to have the message displayed on the console
// AND sent to the logging system (cout is redirected to the logging when
// compiled with -D_TANGO_LIB)
//

		cerr << "Failed to narrow the EventChannelFactory - events will not be generated (hint: start the notifd daemon on this host)" << endl;
		cout << "Failed to narrow the EventChannelFactory - events will not be generated (hint: start the notifd daemon on this host)" << endl;

		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to narrow the EventChannelFactory, make sure the notifd process is running on this host",
			(const char*)"EventSupplier::create()");    
	}

//
// Get a reference to an EventChannel for this device server from the 
// TANGO database
//

	int channel_exported=-1;
	string channel_ior, channel_host;

        send <<= d_name.c_str();
	//cout << d_name.c_str() << endl;
	try 
	{
        	received = db->command_inout("DbImportEvent",send);
	}
	catch (...) 
	{
//
// There is a cout and a cerr here to have the message displayed on the console
// AND sent to the logging system (cout is redirected to the logging when
// compiled with -D_TANGO_LIB)
//

		cerr << d_name << " has no event channel defined in the database - creating it " << endl;
		cout << d_name << " has no event channel defined in the database - creating it " << endl;
		
		channel_exported = 0;
	}
	if (channel_exported != 0)
	{
        	received.inout() >>= dev_import_list;
        	channel_ior = string((dev_import_list->svalue)[1]);
        	channel_exported = dev_import_list->lvalue[0];
		
//
// check if the channel is exported on this host, if not assume it
// is an old channel and we need to recreate it on the local host
//

        	channel_host = string((dev_import_list->svalue)[3]);
		if (channel_host !=  host_name)
			channel_exported = 0;
	}

	if (channel_exported)
	{
        	CORBA::Object *event_channel_obj;
        	event_channel_obj = _orb -> string_to_object(channel_ior.c_str());

		try
		{
        		if (event_channel_obj -> _non_existent())
                		event_channel_obj = CORBA::Object::_nil();
	
			_eventChannel = CosNotifyChannelAdmin::EventChannel::_nil();
 			_eventChannel = CosNotifyChannelAdmin::EventChannel::_narrow(event_channel_obj);

			if(CORBA::is_nil(_eventChannel))
			{
				channel_exported = 0;
			}
		}
		catch (...)
		{
			cout4 << "caught exception while trying to test event_channel object\n";
			channel_exported = 0;
		}
	}
	
//
// The device server event channel does not exist, let's create a new one
//

	if (!channel_exported)
	{
		CosNotification::QoSProperties initialQoS;
		CosNotification::AdminProperties initialAdmin;
		CosNotifyChannelAdmin::ChannelID channelId;

		try
		{
	    		_eventChannel = _eventChannelFactory -> create_channel(initialQoS,
						      initialAdmin,
						      channelId);
			cout4 << "Tango::EventSupplier::create() channel for server " << d_name << " created\n";
			char *_ior = _orb->object_to_string(_eventChannel);
			string ior_string(_ior);

        		Tango::DevVarStringArray *dev_export_list = new Tango::DevVarStringArray;
        		dev_export_list->length(5);
        		(*dev_export_list)[0] = CORBA::string_dup(d_name.c_str());
        		(*dev_export_list)[1] = CORBA::string_dup(ior_string.c_str());
        		(*dev_export_list)[2] = CORBA::string_dup(host_name.c_str());
        		ostringstream ostream;
        		ostream << getpid() << ends;
        		(*dev_export_list)[3] = CORBA::string_dup(ostream.str().c_str());
        		(*dev_export_list)[4] = CORBA::string_dup("1");
        		send <<= dev_export_list;
                	received = db->command_inout("DbExportEvent",send);
                	cout4 << "successfully  exported event channel to Tango database !\n";
			CORBA::string_free(_ior);
		}
		catch(const CosNotification::UnsupportedQoS&)
		{
			cerr << "Failed to create event channel - events will not be generated (hint: start the notifd daemon on this host)" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to create a new EventChannel, make sure the notifd process is running on this host",
				(const char*)"EventSupplier::create()");    
		}
		catch(const CosNotification::UnsupportedAdmin&)
		{
			cerr << "Failed to create event channel - events will not be generated (hint: start the notifd daemon on this host)" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to create a new EventChannel, make sure the notifd process is running on this host",
				(const char*)"EventSupplier::create()");    
		}
	}
	else
	{
		cout4 << "Tango::EventSupplier::create(): _narrow worked, use this event channel\n";
	}

//
// Obtain a Supplier Admin
//

//
// We'll use the channel's default Supplier admin
//

        CosNotifyChannelAdmin::SupplierAdmin_var 
		_supplierAdmin = _eventChannel -> default_supplier_admin();
	if (CORBA::is_nil(_supplierAdmin))
	{
        	cerr << "Could not get CosNotifyChannelAdmin::SupplierAdmin" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to get the default supplier admin from the notification daemon (hint: make sure the notifd process is running on this host)",
			(const char*)"EventSupplier::create()");    
    	}

//
// Obtain a Proxy Consumer
//

//
// We are using the "Push" model and Structured data
//

	CosNotifyChannelAdmin::ProxyID _proxyId;
	CosNotifyChannelAdmin::ProxyConsumer_var _proxyConsumer;
	try
	{
	        _proxyConsumer =
		_supplierAdmin -> obtain_notification_push_consumer(
		    CosNotifyChannelAdmin::STRUCTURED_EVENT, _proxyId);
		if (CORBA::is_nil(_proxyConsumer))
		{
        		cerr << "Could not get CosNotifyChannelAdmin::ProxyConsumer" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to obtain a Notification push consumer, make sure the notifd process is running on this host",
				(const char*)"EventSupplier::create()");    
    		}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		cerr << "Failed to get push consumer from notification daemon - events will not be generated (hint: start the notifd daemon on this host)" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to get push consumer from notification daemon (hint: make sure the notifd process is running on this host)",
			(const char*)"EventSupplier::create()");    
	}

	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var
            _structuredProxyPushConsumer =
		CosNotifyChannelAdmin::StructuredProxyPushConsumer::_narrow(_proxyConsumer);
	if (CORBA::is_nil(_structuredProxyPushConsumer))
	{
       		cerr << "Tango::EventSupplier::create() could not get CosNotifyChannelAdmin::StructuredProxyPushConsumer" << endl;
    	}
	
//
// Init returned value
//

	ns.SupAdm = _supplierAdmin;
	ns.pID = _proxyId;
	ns.ProCon = _proxyConsumer;
	ns.StrProPush = _structuredProxyPushConsumer;
	ns.EveChaFac = _eventChannelFactory;
	ns.EveCha = _eventChannel;
}

//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::push_heartbeat_event()
// 
// description : 	Method to send the hearbeat event
//
// argument : in :
//
//-----------------------------------------------------------------------------

void EventSupplier::push_heartbeat_event()
{
	CosNotification::StructuredEvent struct_event;
	string event, domain_name;
	unsigned int delta_time;
	unsigned int now_time;
	static int heartbeat_counter=0;
	
//
// Heartbeat - check wether a heartbeat event has been sent recently
// if not then send it. A heartbeat contains no data, it is used by the
// consumer to know that the supplier is still alive. 
//

	Tango::Util *tg = Tango::Util::instance();
	DServer *adm_dev = tg->get_dserver_device();
	now_time = time(NULL);
	delta_time = now_time - adm_dev->last_heartbeat;
	cout3 << "EventSupplier::push_heartbeat_event(): delta time since last heartbeat " << delta_time << endl;

//
// We here compare delta_time to 9 and not to 10.
// This is necessary because, sometimes the polling thread is some
// milli second in advance. The computation here is done in seconds
// So, if the polling thread is in advance, delta_time computed in
// seconds will be 9 even if in realirt it is 9,9
//
	
	if (delta_time >= 9)
	{
		domain_name = "dserver/" + adm_dev->get_full_name();
		
		struct_event.header.fixed_header.event_type.domain_name = CORBA::string_dup(domain_name.c_str());
  		struct_event.header.fixed_header.event_type.type_name   = CORBA::string_dup("Tango::Heartbeat");
  		struct_event.header.variable_header.length( 0 );
		
		cout3 << "EventSupplier::push_heartbeat_event(): detected heartbeat event for " << domain_name << endl;
		cout3 << "EventSupplier::push_heartbeat_event(): delta _time " << delta_time << endl;
  		struct_event.header.fixed_header.event_name  = CORBA::string_dup("heartbeat");
  		struct_event.filterable_data.length(1);
  		struct_event.filterable_data[0].name = CORBA::string_dup("heartbeat_counter");
  		struct_event.filterable_data[0].value <<= (CORBA::Long) heartbeat_counter++;
		adm_dev->last_heartbeat = now_time;
		
		struct_event.remainder_of_body <<= (CORBA::Long)adm_dev->last_heartbeat;

//
// Push the event
//

		bool fail = false;
		try
		{
			structuredProxyPushConsumer -> push_structured_event(struct_event);
		}
		catch(const CosEventComm::Disconnected&)
		{
			cout3 << "EventSupplier::push_heartbeat_event() event channel disconnected !\n";
			fail = true;
		}
       		catch(const CORBA::TRANSIENT &)
       		{
       			cout3 << "EventSupplier::push_heartbeat_event() caught a CORBA::TRANSIENT ! " << endl;
			fail = true;
       		}
       		catch(const CORBA::COMM_FAILURE &)
       		{
       			cout3 << "EventSupplier::push_heartbeat_event() caught a CORBA::COMM_FAILURE ! " << endl;
			fail = true;
		}
    		catch(const CORBA::SystemException &)
    		{
       			cout3 << "EventSupplier::push_heartbeat_event() caught a CORBA::SystemException ! " << endl;
			fail = true;
    		}
		
//
// If it was not possible to communicate with notifd,
// try a reconnection
//

		if (fail == true)
		{
			try
			{
				reconnect_notifd();
			}
			catch (...) {}
		}
	}
}


//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::reconnect_notifd()
// 
// description : 	Method to reconnect to the notifd
//
// argument : in :
//
//-----------------------------------------------------------------------------

void EventSupplier::reconnect_notifd()
{

//
// Check notifd but trying to read an attribute of the event channel
// If it works, we immediately return
//

	try
	{
		CosNotifyChannelAdmin::EventChannelFactory_var ecf = eventChannel->MyFactory();
		return;
	}
	catch (...)
	{
		cout3 << "Notifd dead !!!!!!" << endl;
	}
	
//
// Reconnect process to notifd
//

	try
	{
		NotifService ns;
		Tango::Util *tg = Tango::Util::instance();
		
		connect_to_notifd(ns,orb_,
				  tg->get_ds_name(),
				  tg->get_database(),
				  tg->get_host_name());
		
		supplierAdmin = ns.SupAdm;
		proxyId = ns.pID;
		proxyConsumer = ns.ProCon;
		structuredProxyPushConsumer = ns.StrProPush;
		eventChannelFactory = ns.EveChaFac;
		eventChannel = ns.EveCha;
	}
	catch (...)
	{
		cout3 << "Can't reconnect.............." << endl;
	}


	connect();	
	
}
	
} /* End of Tango namespace */


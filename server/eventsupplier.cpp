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
///		Revision 1.20  2008/01/15 12:32:43  taurel
///		- Simply remove some print
///		
///		Revision 1.19  2007/12/19 15:42:43  taurel
///		- Add some cleanup when quitting applis or device server (notifd objects)
///		
///		Revision 1.18  2007/12/12 10:17:18  taurel
///		- Db calls during DS startup has a separate timeout and some retries
///		
///		Revision 1.17  2007/11/08 12:03:44  taurel
///		- Start implementing user interceptors
///		- Fix bug in poll thread pproperty management when removing polling object
///		- Set a database timeout to 6 sec
///		
///		Revision 1.16  2007/10/16 08:23:37  taurel
///		- Add management of the TC connection establishment timeout for DB access
///		- Add DB server cache in DS used during DS startup sequence
///		- Comment out the sleep time during DS startup sequence
///		
///		Revision 1.15  2007/04/20 14:41:33  taurel
///		- Ported to Windows 64 bits x64 architecture
///		
///		Revision 1.14  2007/04/16 14:57:42  taurel
///		- Added 3 new attributes data types (DevULong, DevULong64 and DevState)
///		- Ported to omniORB4.1
///		- Increased the MAX_TRANSFER_SIZE to 256 MBytes
///		- Added a new filterable field in the archive event
///		
///		Revision 1.13  2007/03/29 07:06:55  taurel
///		- Change some data types for 64 bits compatibility
///		
///		Revision 1.12  2007/03/06 08:19:43  taurel
///		- Added 64 bits data types for 64 bits computer...
///		
///		Revision 1.11  2007/03/02 09:47:18  jensmeyer
///		Added quality change to archive event.
///		Changed the forcing of events in the cases of exception or invalid data.
///		Now an event is only send once with the same exception and only once
///		as long as the data quality stays invalid.
///		
///		Revision 1.10  2007/02/16 10:40:57  taurel
///		- Implementing a new types of event on the Attribute configuration
///		
///		Revision 1.9  2006/05/18 14:29:12  jensmeyer
///		Changed the handling of the event period for periodic and archiving events.
///		The event period is now stored as the configured value and no longer
///		multilplied by the factor DELTA_PERIODIC.
///		The calculation of the precision to fire periodic events has moved from
///		the attribbute to the push methods in the event supplier.
///		
///		For event periods < 5000 ms a precision of 2% was kept, but for
///		times < 5000 ms a fixed precision of 100 ms was added.
///		This avoids periodic events with a long event period to be fired to early.
///		
///		Revision 1.8  2006/04/13 06:14:05  jensmeyer
///		Added mutex variables to protect event related methods for accesses
///		from different threads.
///		
///		Revision 1.7  2006/02/17 16:55:21  jensmeyer
///		Corrections when porting to VC7 under windows
///		
///		Revision 1.6  2006/01/27 14:27:10  taurel
///		- Fix a severe incompatibility problem introduced by all the modifs done for
///		PY DS
///		- Duplicate some EventSupplier class methods (instead of using template) in order to be able to generate Tango shared library on Suse 9.3
///		
///		Revision 1.5  2005/06/29 08:31:18  taurel
///		- Last commit before release 5.2 ?
///		
///		Revision 1.4  2005/03/14 12:01:24  taurel
///		- Fix bug in the event system for device server started with the -file option
///		
///		Revision 1.3  2005/03/14 09:49:29  taurel
///		- Fix some bugs in filedatabase (Change in the data transferred between client and
///		  server).
///		- Fix bug in event re-connection
///		- Add event support even for device server started with the -file command line option
///		
///		Revision 1.2  2005/01/13 08:29:06  taurel
///		- Merge trunk with Release_5_0 from brach Release_5_branch
///		
///		Revision 1.1.2.2  2004/10/22 11:25:51  taurel
///		Added warning alarm
///		Change attribute config. It now includes alarm and event parameters
///		Array attribute property now supported
///		subscribe_event throws exception for change event if they are not correctly configured
///		Change in the polling thread: The event heartbeat has its own work in the work list
///		Also add some event_unregister
///		Fix order in which classes are destructed
///		Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
///		Change in the CORBA info call for the device type
///		
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

#ifdef _TG_WINDOWS_
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
	CosNotifyChannelAdmin::EventChannel_var _eventChannel,
	string &event_ior)
{
	orb_ = _orb;
	supplierAdmin = _supplierAdmin;
	proxyId = _proxId;
	proxyConsumer = _proxyConsumer;
	structuredProxyPushConsumer = _structuredProxyPushConsumer;
	eventChannelFactory = _eventChannelFactory;
	eventChannel = _eventChannel;
	event_channel_ior = event_ior;

	_instance = this;
}

EventSupplier *EventSupplier::create(CORBA::ORB_var _orb,
				     string server_name,
				     Database *db,
				     string &host_name,
				     Util *tg) 
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
	connect_to_notifd(ns,_orb,server_name,db,host_name,tg);
	
//
// EventSupplier singleton does not exist, create it
//

	EventSupplier *_event_supplier =
		new EventSupplier(_orb,ns.SupAdm,ns.pID,ns.ProCon,ns.StrProPush,ns.EveChaFac,ns.EveCha,ns.ec_ior);
	
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
				      string &host_name,
				      Util *tg)
{
	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory;
	CosNotifyChannelAdmin::EventChannel_var _eventChannel;

//
// Get a reference to the Notification Service EventChannelFactory from
// the TANGO database or from the server itself in case of server
// started with the -file option
//

	string factory_ior;
	string factory_name;
	factory_name = "notifd/factory/" + host_name;
	CORBA::Any_var received;
	string d_name = "DServer/";
	d_name = d_name + server_name;
	const DevVarLongStringArray *dev_import_list;

	if (Util::_FileDb == false)
	{
		try
		{
			if (tg->get_db_cache() != NULL)
			{
				dev_import_list = tg->get_db_cache()->import_notifd_event();
			}
			else
			{
				received = db->import_event(factory_name);
			}
		}
		catch (...) 
		{
	
//
// Impossible to connect to notifd. In case there is already an entry in the db
// for this server event channel, clear it.
//

			try
			{
				db->unexport_event(d_name);
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

		if (tg->get_db_cache() == NULL)
			received.inout() >>= dev_import_list;
		factory_ior = string((dev_import_list->svalue)[1]);	
	}
	else
	{
		
		Tango::DbDatum na;
		
		string cl_name("notifd");
		try
		{
			na = db->get_device_name(server_name,cl_name);
		}
		catch (Tango::DevFailed &)
		{				
			cerr << "Failed to import EventChannelFactory from the Device Server property file" << endl;
			cerr << "Event will not be generated" << endl;
			cout << "Failed to import EventChannelFactory from the Device Server property file" << endl;
			cout << "Event will not be generated" << endl;

			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to import the EventChannelFactory from the Device Server property file",
				(const char*)"EventSupplier::create()"); 
						
		}				

		factory_ior = na.value_string[0];
	}
 


	try
	{
		CORBA::Object *event_factory_obj;
    	event_factory_obj = _orb -> string_to_object(factory_ior.c_str());
#ifndef _TG_WINDOWS_
    	if (event_factory_obj -> _non_existent())
		event_factory_obj = CORBA::Object::_nil();
#endif /* _TG_WINDOWS_ */

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

		if (Util::_FileDb == false)
		{
			try
			{
				db->unexport_event(d_name);
			}
			catch (...) {}
		}
			
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

	if (Util::_FileDb == false)
	{
		try 
		{
			if (tg->get_db_cache() != NULL)
			{
				dev_import_list = tg->get_db_cache()->DbServerCache::import_adm_event();
			}
			else
			{
				received = db->import_event(d_name);
			}
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
			if (tg->get_db_cache() == NULL)
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
	}
	else
		channel_exported = false;

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

			if (Util::_FileDb == false)
			{
        		Tango::DevVarStringArray *eve_export_list = new Tango::DevVarStringArray;
        		eve_export_list->length(5);
        		(*eve_export_list)[0] = CORBA::string_dup(d_name.c_str());
    			(*eve_export_list)[1] = CORBA::string_dup(ior_string.c_str());
    			(*eve_export_list)[2] = CORBA::string_dup(host_name.c_str());
    			ostringstream ostream;
    			ostream << getpid() << ends;
    			(*eve_export_list)[3] = CORBA::string_dup(ostream.str().c_str());
    			(*eve_export_list)[4] = CORBA::string_dup("1");

				db->export_event(eve_export_list);

        		cout4 << "successfully  exported event channel to Tango database !\n";
			}
			else
			{
			
//
// In case of DS started with -file option, store the
// event channel within the event supplier object
//
				
				ns.ec_ior = ior_string;
			}
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
	    _proxyConsumer = _supplierAdmin -> obtain_notification_push_consumer(CosNotifyChannelAdmin::STRUCTURED_EVENT, _proxyId);
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
	time_t delta_time;
	time_t now_time;
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
// seconds will be 9 even if in reality it is 9,9
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
// Reconnect process to notifd after forcing 
// process to re-read the file database
// in case it is used
//

	try
	{
		NotifService ns;
		Tango::Util *tg = Tango::Util::instance();
		Database *db = tg->get_database();
		
		if (Util::_FileDb == true)
			db->reread_filedatabase();

		connect_to_notifd(ns,orb_,
				  tg->get_ds_name(),
				  db,
				  tg->get_host_name(),tg);
		
		supplierAdmin = ns.SupAdm;
		proxyId = ns.pID;
		proxyConsumer = ns.ProCon;
		structuredProxyPushConsumer = ns.StrProPush;
		eventChannelFactory = ns.EveChaFac;
		eventChannel = ns.EveCha;
		event_channel_ior = ns.ec_ior;
	}
	catch (...)
	{
		cout3 << "Can't reconnect.............." << endl;
	}


	connect();	
	
}


/////////////////////////////////////////////////////////////////////////////
//
//			WARNING
//			-------
//
////////////////////////////////////////////////////////////////////////////

// Unfortunately, with Suse 9.3 with gcc 3.3.4 and binutils 2.15.94,
// it was not possible to generate the shared libs of Tango
// (linker error messages).
// To solve this problem, it was necessary to duplicate these methods
// instead of simply using a template method. This generates a code
// duplicate but...... (all methods finishing with xxxx_3)
//
// Test with gcc 3.4.5 with binutils 2.16.1 did not solve the problems
// We need to check with gcc 4.x.x if we could remove this code duplicate
// The other methods are in eventsupplier.h
//
//
//--------------------------------------------------------------------------

void EventSupplier::detect_and_push_events_3(DeviceImpl *device_impl,
				    long idl_vers,
				    Tango::AttributeValue_3 &attr_value,
				    DevFailed *except,
				    string &attr_name)
	{
		string event, domain_name;
		time_t now, change_subscription, periodic_subscription, archive_subscription;
		cout3 << "EventSupplier::detect_and_push_events(): called for attribute " << attr_name << endl;

		Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

		now = time(NULL);
		change_subscription = now - attr.ext->event_change_subscription;
		periodic_subscription = now - attr.ext->event_periodic_subscription;
		archive_subscription = now - attr.ext->event_archive_subscription;

		cout3 << "EventSupplier::detect_and_push_events(): last subscription for change " << change_subscription << " periodic " << periodic_subscription << " archive " << archive_subscription << endl;
		if (change_subscription < EVENT_RESUBSCRIBE_PERIOD)
		{
			detect_and_push_change_event_3(device_impl,attr_value,attr,attr_name,except);
		}
		if (periodic_subscription < EVENT_RESUBSCRIBE_PERIOD)
		{
			detect_and_push_periodic_event_3(device_impl,attr_value,attr,attr_name,except);
		}
		if (archive_subscription < EVENT_RESUBSCRIBE_PERIOD)
		{
			detect_and_push_archive_event_3(device_impl,attr_value,attr,attr_name,except);
		}
	}
		
bool EventSupplier::detect_change_3(Attribute &attr,
				  AttributeValue_3 &curr_attr_value,
				  bool archive,
				  double &delta_change_rel,
				  double &delta_change_abs,
				  DevFailed *except,
				  bool &force_change,
				  DeviceImpl *dev)
	{
		bool is_change = false;
		
		cout3 << "EventSupplier::detect_change(): called for attribute " << attr.get_name() << endl;

		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(detect_mutex);

//
// Send event, if the read_attribute failed or if it is the first time
// that the read_attribute succeed after a failure.
// Same thing if the attribute quality factor changes to INVALID
//
			
		if (archive == true)
		{
			// force an event only when the last reading was not returning an exception or
			// not returning the same exception
			
			if (except != NULL)
			{
				if ( attr.ext->prev_archive_event.err == true )
				{
					if ( Except::compare_exception (*except, attr.ext->prev_archive_event.except) == true )
					{
						force_change = false;
						return false;
					}
				}
					
				force_change = true;
				return true;
			}
			
			// force an archive event when the last reding was still returning an exception
			if ((except == NULL) && (attr.ext->prev_archive_event.err == true))
			{
				force_change = true;
				return true;
			}
			
			// check wether the quality is invalid
			// Force an event only if the last reading was valid
			
		   if (curr_attr_value.quality == Tango::ATTR_INVALID)
			{
				if ( attr.ext->prev_archive_event.quality == Tango::ATTR_INVALID )
				{
					force_change = false;
					return false;					
				}
					
				force_change = true;
				return true;
			}			
						
			// force an archive event when the last reding was still marked as invalid data
			if ((curr_attr_value.quality != Tango::ATTR_INVALID) && (attr.ext->prev_archive_event.quality == Tango::ATTR_INVALID))
			{
				force_change = true;
				return true;
			}
		}
		else
		{	
			// force an event only when the last reading was not returning an exception or
			// not returning the same exception
			
			if (except != NULL)
			{
				if ( attr.ext->prev_change_event.err == true )
				{
					if ( Except::compare_exception (*except, attr.ext->prev_change_event.except) == true )
					{
						force_change = false;
						return false;
					}
				}
					
				force_change = true;
				return true;
			}

			// force an change event when the last reding was still returning an exception
			if ((except == NULL) && (attr.ext->prev_change_event.err == true))
			{
				force_change = true;
				return true;
			}
						
			// check wether the quality is invalid
			// Force an event only if the last reading was valid
			
		   if (curr_attr_value.quality == Tango::ATTR_INVALID)
			{
				if ( attr.ext->prev_change_event.quality == Tango::ATTR_INVALID )
				{
						force_change = false;
						return false;					
				}
					
				force_change = true;
				return true;
			}			
		
			// force an change event when the last reding was still marked as invalid data
			if ((curr_attr_value.quality != Tango::ATTR_INVALID) && (attr.ext->prev_change_event.quality == Tango::ATTR_INVALID))
			{
				force_change = true;
				return true;
			}
		}

		const DevVarLong64Array *curr_seq_64, *prev_seq_64;
		const DevVarLongArray *curr_seq_lo, *prev_seq_lo;
		const DevVarShortArray *curr_seq_sh, *prev_seq_sh;
		const DevVarDoubleArray *curr_seq_db, *prev_seq_db;
		const DevVarStringArray *curr_seq_str, *prev_seq_str;
		const DevVarFloatArray *curr_seq_fl, *prev_seq_fl;
		const DevVarBooleanArray *curr_seq_bo, *prev_seq_bo;
		const DevVarUShortArray *curr_seq_ush, *prev_seq_ush;
		const DevVarCharArray *curr_seq_uch, *prev_seq_uch;
		const DevVarULongArray *curr_seq_ulo, *prev_seq_ulo;
		const DevVarULong64Array *curr_seq_u64, *prev_seq_u64;
		const DevVarStateArray *curr_seq_state, *prev_seq_state;		
		DevState curr_sta, prev_sta;
				
		double rel_change[2], abs_change[2];
		unsigned int i;
		unsigned int curr_seq_nb,prev_seq_nb;
		bool inited;
		CORBA::TypeCode_var ty = curr_attr_value.value.type();
		delta_change_rel = delta_change_abs = 0;

		bool enable_check = false;
		
		TangoMonitor &mon1 = dev->get_att_conf_monitor();
		mon1.get_monitor();
		if (!archive)
		{
			rel_change[0] = attr.ext->rel_change[0];
			rel_change[1] = attr.ext->rel_change[1];
			abs_change[0] = attr.ext->abs_change[0];
			abs_change[1] = attr.ext->abs_change[1];
			inited = attr.ext->prev_change_event.inited;
			if ((attr.ext->prev_change_event.quality != Tango::ATTR_INVALID) && (curr_attr_value.quality != Tango::ATTR_INVALID))
				    enable_check = true;
		}
		else
		{
			rel_change[0] = attr.ext->archive_rel_change[0];
			rel_change[1] = attr.ext->archive_rel_change[1];
			abs_change[0] = attr.ext->archive_abs_change[0];
			abs_change[1] = attr.ext->archive_abs_change[1];
			inited = attr.ext->prev_archive_event.inited;
			if ((attr.ext->prev_archive_event.quality != Tango::ATTR_INVALID) && (curr_attr_value.quality != Tango::ATTR_INVALID))
				    enable_check = true;		
		}
		mon1.rel_monitor();

		if (inited)
		{	
			if (enable_check == true)
			{
				if (ty->kind() == CORBA::tk_enum)
				{
					curr_attr_value.value >>= curr_sta;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_sta;
					else
						attr.ext->prev_change_event.value >>= prev_sta;
						
					if (curr_sta != prev_sta)
					{
						delta_change_rel = delta_change_abs = 100.;
						is_change = true;
					}
					return is_change;
				}
								
				CORBA::TypeCode_var ty_alias = ty->content_type();
				CORBA::TypeCode_var ty_seq = ty_alias->content_type();					
				switch (ty_seq->kind())
				{
				case CORBA::tk_long:		
					curr_attr_value.value >>= curr_seq_lo;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_lo;
					else
						attr.ext->prev_change_event.value >>= prev_seq_lo;
					curr_seq_nb = curr_seq_lo->length();
					prev_seq_nb = prev_seq_lo->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_lo->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_lo)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_lo)[i] - (*prev_seq_lo)[i])*100/(*prev_seq_lo)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_lo)[i] == (*prev_seq_lo)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_lo)[i] - (*prev_seq_lo)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_longlong:
					curr_attr_value.value >>= curr_seq_64;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_64;
					else
						attr.ext->prev_change_event.value >>= prev_seq_64;
					curr_seq_nb = curr_seq_64->length();
					prev_seq_nb = prev_seq_64->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_64->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_64)[i] != 0)
							{
								delta_change_rel = (double)(((*curr_seq_64)[i] - (*prev_seq_64)[i])*100/(*prev_seq_64)[i]);
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_64)[i] == (*prev_seq_64)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (double)((*curr_seq_64)[i] - (*prev_seq_64)[i]);
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_short:
					curr_attr_value.value >>= curr_seq_sh;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_sh;
					else
						attr.ext->prev_change_event.value >>= prev_seq_sh;
					curr_seq_nb = curr_seq_sh->length();
					prev_seq_nb = prev_seq_sh->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_sh->length(); i++)
					{	
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_sh)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_sh)[i] - (*prev_seq_sh)[i])*100/(*prev_seq_sh)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_sh)[i] == (*prev_seq_sh)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_sh)[i] - (*prev_seq_sh)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_double:
					curr_attr_value.value >>= curr_seq_db;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_db;
					else
						attr.ext->prev_change_event.value >>= prev_seq_db;
					curr_seq_nb = curr_seq_db->length();
					prev_seq_nb = prev_seq_db->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_db->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_db)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_db)[i] - (*prev_seq_db)[i])*100/(*prev_seq_db)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_db)[i] == (*prev_seq_db)[i]) delta_change_rel = 0;
							}
							
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_db)[i] - (*prev_seq_db)[i];
							
							// Correct for rounding errors !
							double max_change = delta_change_abs + (abs_change[1] * 1e-10);
							double min_change = delta_change_abs + (abs_change[0] * 1e-10);
							
							//if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							if (min_change <= abs_change[0] || max_change >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_string:
					curr_attr_value.value >>= curr_seq_str;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_str;
					else
						attr.ext->prev_change_event.value >>= prev_seq_str;
					curr_seq_nb = curr_seq_str->length();
					prev_seq_nb = prev_seq_str->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_str->length(); i++)
					{
						if (strcmp((*curr_seq_str)[i],(*prev_seq_str)[i]) != 0)
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
							return(is_change);
						}
					}
					break;

				case CORBA::tk_float:
					curr_attr_value.value >>= curr_seq_fl;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_fl;
					else
						attr.ext->prev_change_event.value >>= prev_seq_fl;
					curr_seq_nb = curr_seq_fl->length();
					prev_seq_nb = prev_seq_fl->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_fl->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_fl)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_fl)[i] - (*prev_seq_fl)[i])*100/(*prev_seq_fl)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_fl)[i] == (*prev_seq_fl)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_fl)[i] - (*prev_seq_fl)[i];
							
							// Correct for rounding errors !
							double max_change = delta_change_abs + (abs_change[1] * 1e-10);
							double min_change = delta_change_abs + (abs_change[0] * 1e-10);
															
							//if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							if (min_change <= abs_change[0] || max_change >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_ushort:
					curr_attr_value.value >>= curr_seq_ush;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_ush;
					else
						attr.ext->prev_change_event.value >>= prev_seq_ush;
					curr_seq_nb = curr_seq_ush->length();
					prev_seq_nb = prev_seq_ush->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_ush->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_ush)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_ush)[i] - (*prev_seq_ush)[i])*100/(*prev_seq_ush)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_ush)[i] == (*prev_seq_ush)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_ush)[i] - (*prev_seq_ush)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;

				case CORBA::tk_boolean:
					curr_attr_value.value >>= curr_seq_bo;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_bo;
					else
						attr.ext->prev_change_event.value >>= prev_seq_bo;
					curr_seq_nb = curr_seq_bo->length();
					prev_seq_nb = prev_seq_bo->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_bo->length(); i++)
					{
						if ((*curr_seq_bo)[i] != (*prev_seq_bo)[i])
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
							return(is_change);
						}
					}
					break;

				case CORBA::tk_octet:
					curr_attr_value.value >>= curr_seq_uch;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_uch;
					else
						attr.ext->prev_change_event.value >>= prev_seq_uch;
					curr_seq_nb = curr_seq_uch->length();
					prev_seq_nb = prev_seq_uch->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_uch->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_uch)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_uch)[i] - (*prev_seq_uch)[i])*100/(*prev_seq_uch)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_uch)[i] == (*prev_seq_uch)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_uch)[i] - (*prev_seq_uch)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_ulong:		
					curr_attr_value.value >>= curr_seq_ulo;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_ulo;
					else
						attr.ext->prev_change_event.value >>= prev_seq_ulo;
					curr_seq_nb = curr_seq_ulo->length();
					prev_seq_nb = prev_seq_ulo->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_ulo->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_ulo)[i] != 0)
							{
								delta_change_rel = ((*curr_seq_ulo)[i] - (*prev_seq_ulo)[i])*100/(*prev_seq_ulo)[i];
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_ulo)[i] == (*prev_seq_ulo)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (*curr_seq_ulo)[i] - (*prev_seq_ulo)[i];
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_ulonglong:
					curr_attr_value.value >>= curr_seq_u64;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_u64;
					else
						attr.ext->prev_change_event.value >>= prev_seq_u64;
					curr_seq_nb = curr_seq_u64->length();
					prev_seq_nb = prev_seq_u64->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_u64->length(); i++)
					{
						if (rel_change[0] != INT_MAX)
						{
							if ((*prev_seq_u64)[i] != 0)
							{
								delta_change_rel = (double)(((*curr_seq_u64)[i] - (*prev_seq_u64)[i])*100/(*prev_seq_u64)[i]);
							}
							else
							{
								delta_change_rel = 100;
								if ((*curr_seq_u64)[i] == (*prev_seq_u64)[i]) delta_change_rel = 0;
							}
							if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
						if (abs_change[0] != INT_MAX)
						{
							delta_change_abs = (double)((*curr_seq_u64)[i] - (*prev_seq_u64)[i]);
							if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
							{
								is_change = true;
								return(is_change);
							}
						}
					}
					break;
					
				case CORBA::tk_enum:
					curr_attr_value.value >>= curr_seq_state;
					if (archive == true)
						attr.ext->prev_archive_event.value >>= prev_seq_state;
					else
						attr.ext->prev_change_event.value >>= prev_seq_state;
					curr_seq_nb = curr_seq_state->length();
					prev_seq_nb = prev_seq_state->length();
					if (curr_seq_nb != prev_seq_nb)
					{
						force_change = true;
						return true;
					}
					for (i=0; i<curr_seq_state->length(); i++)
					{
						if ((*curr_seq_state)[i] != (*prev_seq_state)[i])
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
							return(is_change);
						}
					}
					break;
					
				default:
					break;
				}
			}
		}	

		cout3 << "EventSupplier::detect_change(): leaving for attribute " << attr.get_name() << endl;
		return(is_change);
	}


void EventSupplier::detect_and_push_change_event_3(DeviceImpl *device_impl,
						 AttributeValue_3 &attr_value,
						 Attribute &attr,
						 string &attr_name,
						 DevFailed *except)
	
{
		string event, domain_name;
		double delta_change_rel = 0.0;
		double delta_change_abs = 0.0;
		bool is_change      = false;
		bool force_change   = false;
		bool quality_change = false;

		cout3 << "EventSupplier::detect_and_push_change_event(): called for attribute " << attr_name << endl;
		
		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(event_mutex);

		//
		// if no attribute of this name is registered with change then
		// insert the current value
		//

		if (!attr.ext->prev_change_event.inited)
		{
			if (except != NULL)
			{
				attr.ext->prev_change_event.err    = true;
				attr.ext->prev_change_event.except = *except;
			}
			else
			{
				attr.ext->prev_change_event.value = attr_value.value;
				attr.ext->prev_change_event.quality = attr_value.quality;
				attr.ext->prev_change_event.err = false;
			}
			attr.ext->prev_change_event.inited = true;
		}
		else
		{
	
//
// determine delta_change in percent compared with previous event sent
// 
			is_change = detect_change_3(attr,attr_value,false,delta_change_rel,delta_change_abs,except,force_change,device_impl);
			cout3 << "EventSupplier::detect_and_push_change_event(): rel_change " << delta_change_rel << " abs_change " << delta_change_abs << " is change = " << is_change << endl;
		}
		
		// check whether the data quality has changed.
		// Fire event on a quality change.
		
		if ( except == NULL &&
		    attr.ext->prev_change_event.quality != attr_value.quality )
		{
			is_change = true;
			quality_change = true;
		}
		
	
		if (is_change)
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
			{
				attr.ext->prev_change_event.err    = true;
				attr.ext->prev_change_event.except = *except;
			}		
			else
			{
				attr.ext->prev_change_event.value   = attr_value.value;
				attr.ext->prev_change_event.quality = attr_value.quality;
				attr.ext->prev_change_event.err     = false;
			}

			domain_name = device_impl->get_name() + "/" + attr_name;
			filterable_names.push_back("delta_change_rel");
			filterable_data.push_back(delta_change_rel);
			filterable_names.push_back("delta_change_abs");
			filterable_data.push_back(delta_change_abs);
			
			filterable_names.push_back("forced_event");
			if (force_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);
			
			filterable_names.push_back("quality");
			if (quality_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);				
			
			push_event_3(device_impl,
				   "change",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}
	cout3 << "EventSupplier::detect_and_push_change_event(): leaving for attribute " << attr_name << endl;		
	}

//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_and_push_archive_event()
// 
// description : 	Method to detect if there it is necessary
//			to push an archive event
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void EventSupplier::detect_and_push_archive_event_3(DeviceImpl *device_impl,
						  AttributeValue_3 &attr_value, 
						  Attribute &attr,
						  string &attr_name,
						  DevFailed *except)
{
						  
		string event, domain_name;
		double delta_change_rel = 0.0;
		double delta_change_abs = 0.0;
		bool is_change = false;
		bool force_change = false;
		bool period_change = false;
		bool quality_change = false;

		cout3 << "EventSupplier::detect_and_push_archive_event(): called for attribute " << attr_name << endl;

		double now_ms, ms_since_last_periodic;
#ifdef _TG_WINDOWS_
        struct _timeb           now_win;
#endif
        struct timeval          now_timeval;

#ifdef _TG_WINDOWS_
		_ftime(&now_win);
		now_timeval.tv_sec = (unsigned long)now_win.time;
		now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&now_timeval,NULL);
#endif

		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(event_mutex);
		

		now_ms = (double)now_timeval.tv_sec * 1000. + (double)now_timeval.tv_usec / 1000.;
		ms_since_last_periodic = now_ms - attr.ext->archive_last_periodic;

		int arch_period;
		TangoMonitor &mon1 = device_impl->get_att_conf_monitor();
		mon1.get_monitor();
		arch_period = attr.ext->archive_period;
		mon1.rel_monitor();
		
		// Specify the precision interval for the archive period testing
		// 2% are used for periods < 5000 ms and
		// 100ms are used for periods > 5000 ms.
	
		if ( arch_period >= 5000 )
		{
			arch_period = arch_period - DELTA_PERIODIC_LONG;
		}
		else
		{
#ifdef _TG_WINDOWS_
			double tmp = (double)arch_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
				eve_round = ceil(tmp);
			else
				eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD))
			double eve_round = rint((double)arch_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint((double)arch_period * DELTA_PERIODIC);
		#else
			double eve_round = round((double)arch_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			arch_period = (int)eve_round;
		}		
		
		
		if ((ms_since_last_periodic > arch_period) && (attr.ext->prev_archive_event.inited == true))
		{
			is_change = true;
			period_change = true;
		}
		
//
// if no attribute of this name is registered with change then
// insert the current value
//


		if (!attr.ext->prev_archive_event.inited)
		{
			if (except != NULL)
			{
				attr.ext->prev_archive_event.err    = true;
				attr.ext->prev_archive_event.except = *except;
			}
			else
			{
				attr.ext->prev_archive_event.value = attr_value.value;
				attr.ext->prev_archive_event.quality = attr_value.quality;
				attr.ext->prev_archive_event.err = false;
			}
			attr.ext->archive_last_periodic = now_ms;
			attr.ext->archive_last_event = now_ms;
			attr.ext->prev_archive_event.inited = true;
		}
		else
		{
	
//
// determine delta_change in percent compared with previous event sent
//

			if (is_change == false)
			{
				is_change = detect_change_3(attr,attr_value,true,
							  delta_change_rel,
							  delta_change_abs,except,
							  force_change,device_impl);
			}
		}

		// check whether the data quality has changed.
		// Fire event on a quality change.
		
		if ( except == NULL &&
		     attr.ext->prev_archive_event.quality != attr_value.quality )
		{
			is_change = true;
			quality_change = true;
		}	
		
		if (is_change)
		{	
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			domain_name = device_impl->get_name() + "/" + attr_name;
			
			if (except != NULL)
			{
				attr.ext->prev_archive_event.err    = true;
				attr.ext->prev_archive_event.except = *except;
			}
			else
			{
				attr.ext->prev_archive_event.value   = attr_value.value;
				attr.ext->prev_archive_event.quality = attr_value.quality;
				attr.ext->prev_archive_event.err     = false;
			}

			filterable_names_lg.push_back("counter");
			if (period_change == true)
			{
				attr.ext->archive_periodic_counter++;
				attr.ext->archive_last_periodic = now_ms;
				filterable_data_lg.push_back(attr.ext->archive_periodic_counter);
			}
			else
			{
				filterable_data_lg.push_back(-1);
			}

			filterable_names.push_back("delta_change_rel");
			filterable_data.push_back(delta_change_rel);
			filterable_names.push_back("delta_change_abs");
			filterable_data.push_back(delta_change_abs);
			filterable_names.push_back("forced_event");
			if (force_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);
			
			filterable_names.push_back("quality");
			if (quality_change == true)
				filterable_data.push_back((double)1.0);
			else
				filterable_data.push_back((double)0.0);
				
			filterable_names.push_back("delta_event");
			filterable_data.push_back(now_ms - attr.ext->archive_last_event);
			attr.ext->archive_last_event = now_ms;
									
			push_event_3(device_impl,
				   "archive",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}
	}


//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_and_push_quality_change_event()
// 
// description : 	Method to detect if there it is necessary
//			to push a quality change event
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void EventSupplier::detect_and_push_quality_change_event_3(DeviceImpl *device_impl,
						  AttributeValue_3 &attr_value,
						  Attribute &attr,
						  string &attr_name,
						  DevFailed *except)
{


		cout3 << "EventSupplier::detect_and_push_quality_change_event(): called for attribute " << attr_name << endl;
		bool is_change = false;
	
//
// if no attribute of this name is registered with change then
// insert the current value
//

		if (!attr.ext->prev_quality_event.inited)
		{
			if (except != NULL)
				attr.ext->prev_quality_event.err = true;
			else
			{
				attr.ext->prev_quality_event.quality = attr_value.quality;
				attr.ext->prev_quality_event.err = false;
			}
			attr.ext->prev_quality_event.inited = true;
		}
		else
		{
			if (except != NULL)
				is_change = true;
			else if ((except == NULL) && (attr.ext->prev_quality_event.err == true))
				is_change = true;
			else if (attr_value.quality != attr.ext->prev_quality_event.quality)
				is_change = true;
			else
				is_change = false;	
		}
	
//
// Send the event if necessary
//
	
		if (is_change)
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			if (except != NULL)
				attr.ext->prev_quality_event.err = true;
			else
			{
				attr.ext->prev_quality_event.quality = attr_value.quality;
				attr.ext->prev_quality_event.err = false;
			}

			push_event_3(device_impl,
				   "quality",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}
	}


//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_and_push_periodic_event()
// 
// description : 	Method to detect if there it is necessary
//			to push a periodic event
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void EventSupplier::detect_and_push_periodic_event_3(DeviceImpl *device_impl,
					    AttributeValue_3 &attr_value,
					    Attribute &attr,
					    string &attr_name,
					    DevFailed *except)
{
					    
		string event, domain_name;
		double now_ms, ms_since_last_periodic;
#ifdef _TG_WINDOWS_
        	struct _timeb           now_win;
#endif
        	struct timeval          now_timeval;

#ifdef _TG_WINDOWS_
		_ftime(&now_win);
		now_timeval.tv_sec = (unsigned long)now_win.time;
		now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&now_timeval,NULL);
#endif
		now_ms = (double)now_timeval.tv_sec * 1000. + (double)now_timeval.tv_usec / 1000.;

		// get the mutex to synchronize the sending of events
		omni_mutex_lock l(event_mutex);
		
		// get the event period
		int eve_period;		
		TangoMonitor &mon1 = device_impl->get_att_conf_monitor();
		mon1.get_monitor();	
		eve_period = attr.ext->event_period;	
		mon1.rel_monitor();

		// Specify the precision interval for the event period testing
		// 2% are used for periods < 5000 ms and
		// 100ms are used for periods > 5000 ms.
	
		if ( eve_period >= 5000 )
			{
			 eve_period = eve_period - DELTA_PERIODIC_LONG;
			}
		else
			{
#ifdef _TG_WINDOWS_
			double tmp = (double)eve_period * DELTA_PERIODIC;
			double int_part,eve_round;
			double frac = modf(tmp,&int_part);
			if (frac >= 0.5)
					eve_round = ceil(tmp);
			else
					eve_round = floor(tmp);					
#else
	#if ((defined __SUNPRO_CC) || (!defined GCC_STD))
			double eve_round = rint((double)eve_period * DELTA_PERIODIC);
	#else
		#if (defined GCC_SOLARIS)
			double eve_round = rint((double)eve_period * DELTA_PERIODIC);
		#else
			double eve_round = round((double)eve_period * DELTA_PERIODIC);
		#endif
	#endif
#endif
			eve_period = (int)eve_round;
			}

		// calculate the time 
		ms_since_last_periodic = now_ms - attr.ext->last_periodic;
		cout3 << "EventSupplier::detect_and_push_is_periodic_event(): delta since last periodic " << ms_since_last_periodic << " event_period " << eve_period << " for " << device_impl->get_name()+"/"+attr_name << endl;
		
		if ( ms_since_last_periodic > eve_period )
		{
			vector<string> filterable_names;
			vector<double> filterable_data;
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;

			attr.ext->periodic_counter++;
			attr.ext->last_periodic = now_ms;
			filterable_names_lg.push_back("counter");
			filterable_data_lg.push_back(attr.ext->periodic_counter);

			cout3 << "EventSupplier::detect_and_push_is_periodic_event(): detected periodic event for " << device_impl->get_name()+"/"+attr_name << endl;
			push_event_3(device_impl,
				   "periodic",
				   filterable_names,
				   filterable_data,
				   filterable_names_lg,
				   filterable_data_lg,
				   attr_value,
				   attr_name,
				   except);
		}

	}

	
//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::push_event()
// 
// description : 	Method to send the event to the event channel
//
// argument : in :	device_impl : The device
//			event_type : The event type (change, periodic....)
//			filterable_names : 
//			filterable_data :
//			attr_value : The attribute value
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void EventSupplier::push_event_3(DeviceImpl *device_impl,
			string event_type,
			vector<string> &filterable_names,
		       	vector<double> &filterable_data,
		       	vector<string> &filterable_names_lg,
		       	vector<long> &filterable_data_lg,
		       	AttributeValue_3 &attr_value,
		       	string &attr_name,
		       	DevFailed *except)
{
	CosNotification::StructuredEvent struct_event;
	string domain_name;

	cout3 << "EventSupplier::push_event(): called for attribute " << attr_name << endl;

	// get the mutex to synchronize the sending of events
	omni_mutex_lock l(push_mutex);
	
	string loc_attr_name = attr_name;	
	transform(loc_attr_name.begin(),loc_attr_name.end(),loc_attr_name.begin(),::tolower);
	domain_name = device_impl->get_name_lower() + "/" + loc_attr_name;

	struct_event.header.fixed_header.event_type.domain_name = CORBA::string_dup(domain_name.c_str());
	struct_event.header.fixed_header.event_type.type_name = CORBA::string_dup("Tango::EventValue");

	struct_event.header.variable_header.length( 0 );

	unsigned long nb_filter = filterable_names.size();
	unsigned long nb_filter_lg = filterable_names_lg.size();

	struct_event.filterable_data.length(nb_filter + nb_filter_lg);

	if (nb_filter != 0)
	{	
		if (nb_filter == filterable_data.size())
		{
			for (unsigned long i = 0; i < nb_filter; i++)
			{
				struct_event.filterable_data[i].name = CORBA::string_dup(filterable_names[i].c_str());
				struct_event.filterable_data[i].value <<= (CORBA::Double) filterable_data[i];
			}
		}
	}

	if (nb_filter_lg != 0)
	{	
		if (nb_filter_lg == filterable_data_lg.size())
		{
			for (unsigned long i = 0; i < nb_filter_lg; i++)
			{
				struct_event.filterable_data[i + nb_filter].name = CORBA::string_dup(filterable_names_lg[i].c_str());
	  			struct_event.filterable_data[i + nb_filter].value <<= (CORBA::Long) filterable_data_lg[i];
			}
		}
	}

	if (except == NULL)	
		struct_event.remainder_of_body <<= attr_value;
	else
		struct_event.remainder_of_body <<= except->errors;
	struct_event.header.fixed_header.event_name = CORBA::string_dup(event_type.c_str());

	cout3 << "EventSupplier::push_event(): push event " << event_type << " for " << device_impl->get_name() + "/" + attr_name << endl;

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
		cout3 << "EventSupplier::push_event() event channel disconnected !\n";
		fail = true;
	}
   	catch(const CORBA::TRANSIENT &)
   	{
   		cout3 << "EventSupplier::push_event() caught a CORBA::TRANSIENT ! " << endl;
		fail = true;
   	}
   	catch(const CORBA::COMM_FAILURE &)
   	{
   		cout3 << "EventSupplier::push_event() caught a CORBA::COMM_FAILURE ! " << endl;
		fail = true;
	}
	catch(const CORBA::SystemException &)
	{
   		cout3 << "EventSupplier::push_event() caught a CORBA::SystemException ! " << endl;
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

//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::disconnect_from_notifd()
// 
// description : 	Method to disconnect the DS from the notifd event channel
//
//-----------------------------------------------------------------------------

void EventSupplier::disconnect_from_notifd()
{
	try
	{
		structuredProxyPushConsumer->disconnect_structured_push_consumer();
	}
	catch(CORBA::Exception &e) {}
}


} /* End of Tango namespace */


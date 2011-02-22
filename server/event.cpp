static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
///  file 	event.cpp
///
///		C++ classes for implementing the event server and client
///		singleton classes - EventSupplier and EventConsumer.
///		These classes are used to send events from the server
///		to the notification service and to receive events from
///		the notification service. 
///
///		author(s) : A.Gotz (goetz@esrf.fr)
///
///		original : 7 April 2003
///
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009
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
///
///		$Revision$
///
///		$Log$
///		Revision 1.59  2009/10/27 08:23:47  taurel
///		- Fully Qualified attribute name passed to the event callback
///		
///		Revision 1.58  2009/10/23 14:36:27  taurel
///		- Tango 7.1.1
///		- Fix bugs 2880372 and 2881841
///		- Now support event in case of Tango system with multi db server
///		- The polling threads start with polling inactive
///		
///		Revision 1.57  2009/10/01 15:13:07  taurel
///		- Fix a bug in case of unsubscribe on event from channel on which several
///		events were registered.(Not proper clean up of the device_channel_map map)
///		
///		Revision 1.56  2009/09/30 06:43:18  taurel
///		- Improve error detection in case of TANGO_HOST not set and not fully
///		qualified device name
///		
///		Revision 1.55  2009/09/29 15:55:15  taurel
///		- Fix bug in case of subscribe_event using fully qualified Tango device
///		name while the TANGO_HOST env. variable is not set
///		
///		Revision 1.54  2009/08/27 07:23:45  taurel
///		- Commit after another merge with Release_7_0_2-bugfixes branch
///		
///		Revision 1.53  2009/06/17 08:52:08  taurel
///		- Commit after a merge with branch Release_7_0_2-bugfixes
///		Revision 1.52.2.9  2009/08/20 07:10:49  taurel
///		- It is now possible to unsubscribe in the event callback!
///		
///		Revision 1.52.2.8  2009/08/17 14:13:33  taurel
///		- Fix SourceForge bug 2821898 for the EventData class.
///		Thank's to Tiago's patch
///		
///		Revision 1.52.2.7  2009/07/16 06:34:49  taurel
///		- Fix bug in AttrConfEventData copy ctor and assignement operator
///		
///		Revision 1.52.2.6  2009/07/15 08:14:34  taurel
///		- Don't forget to set the lock exit installed flag when the EventConsumer object is created !!
///		
///		Revision 1.52.2.5  2009/06/25 11:57:55  taurel
///		- Fix bug in case of multiple subscribe on the same event just before
///		the exception is thrown. A bad lock management
///		
///		Revision 1.52.2.4  2009/06/22 14:48:52  taurel
///		- Fix bug for state change coming from a device 4 server
///		
///		Revision 1.52.2.3  2009/06/17 07:35:44  taurel
///		- Add support for event coming from server with the prefix to build
///		the fqdn contains database host with its full qualified domain
///		name
///		
///		Revision 1.52.2.2  2009/06/15 10:57:09  taurel
///		- Fix a bug in the bug fix !
///		
///		Revision 1.52.2.1  2009/06/12 08:28:51  taurel
///		- Fix bug when using events in multi Tango host environment.
///		The TANGO_HOST is now transferred within the even tin the fixed
///		header event_type field.
///		The DS admin device EventSubscriptionChange command now returns with which Tango lib it is runnig.
///		This allows the client to know if the tango host info will be transmitted within the event
///		
///		Revision 1.52  2009/03/30 15:03:44  taurel
///		- Fix last bugs before Tango 7 ??
///		
///		Revision 1.51  2009/03/20 11:53:28  taurel
///		- Fix some compiler warnings
///		
///		Revision 1.50  2009/03/18 12:18:43  taurel
///		- Fix warnings reported when compiled with the option -Wall
///		
///		Revision 1.49  2009/03/13 09:33:29  taurel
///		- Small changes to fix Windows VC8 warnings in Warning level 3
///		
///		Revision 1.48  2009/01/29 15:25:41  taurel
///		- First implementation of the Data Ready event
///		
///		Revision 1.47  2009/01/21 12:49:03  taurel
///		- Change CopyRights for 2009
///		
///		Revision 1.46  2008/12/17 09:50:59  taurel
///		- First implementation of attributes sent on the wire using IDL Union
///		instead of IDL Any
///		
///		Revision 1.45  2008/10/06 15:01:09  taurel
///		- Changed the licensing info from GPL to LGPL
///		
///		Revision 1.44  2008/10/03 06:52:31  taurel
///		- Add some licensing info in each files
///		
///		Revision 1.43  2008/10/01 12:02:01  jensmeyer
///		Changed method name event_queue_is_empty() to is_event_queue_empty()
///		
///		Revision 1.42  2008/09/23 14:59:33  taurel
///		- Commit after the end of DevEncoded data type implementation
///		- The new test suite is also now running fine
///		
///		Revision 1.41  2008/09/15 13:19:39  jensmeyer
///		Deleted some debugging printouts.
///		
///		Revision 1.40  2008/09/15 12:31:09  jensmeyer
///		Added an eventqueue reading method to call the usual callback method
///		when reading event data from the queue.
///
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>

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


/************************************************************************/
/*		       															*/		
/* 		(client_)leavfunc function 										*/
/*			-----------------											*/
/*		       															*/
/* This function will be executed at process exit or when the main      */
/* returned.  It has to be executed to properly shutdown and destroy    */
/* the ORB used by as a server by the event system. The ORB loop is in  */
/* EventConsumer thread. Therefore, get a reference to it, shutdown the */
/* ORB and wait until the thread exit.				        			*/
/* It also destroys the heartbeat filters								*/
/*																		*/
/************************************************************************/
void client_leavefunc()
{
	leavefunc();
}

void leavefunc()
{
	// flag to avoid calling client_leavefunc twice
	static bool already_executed = false;
	
	Tango::ApiUtil *au = ApiUtil::instance();
	
	if (au->need_reset_already_flag() == true)
		already_executed = false;
	
//
// Kill locking threads (if any)
//
	
	au->clean_locking_threads();

//
// Manage event stuff
//
	
	EventConsumer *ev = au->get_event_consumer();
	
	if (ev != NULL && already_executed == false)
	{
		ev->disconnect_from_notifd();			
		
		//
		// Shut-down the ORB and wait for the thread to exit
		//
		
		int *rv;
		ev->orb_->shutdown(true);
		ev->join((void **)&rv);
		
		already_executed = true;
		au->need_reset_already_flag(false);
	}
		
}



/************************************************************************/
/*		       															*/		
/* 			EventConsumer class 										*/
/*			-------------------											*/
/*		       															*/
/************************************************************************/



EventConsumer *EventConsumer::_instance = NULL;

EventConsumer::EventConsumer(ApiUtil *ptr) : omni_thread((void *)ptr)
{
	cout3 << "calling Tango::EventConsumer::EventConsumer() \n";
	orb_ = ptr->get_orb();

	_instance = this;

//
// Install a function to be executed at exit
// This is the only way I found to properly
// shutdown and destroy the ORB.
// Don't do this for windows DLL.
//
// Is this necessary when events are used within a server ?
//

	ApiUtil *api_ptr =  ApiUtil::instance();
#ifndef _USRDLL
	if (ptr->in_server() == false)
	{
		atexit(leavefunc);
		api_ptr->set_lock_exit_installed(true);
	}
#endif

//
// Build and store the fqdn prefix for devices in the TANGO_HOST
// environment variable
//

	try
	{
		Database *db = (api_ptr->get_db_vect())[api_ptr->get_db_ind()];
		string prefix = "tango://" + db->get_db_host() + ':' + db->get_db_port() + '/' ;
		env_var_fqdn_prefix.push_back(prefix);
		if (db->is_multi_tango_host() == true)
		{
			vector<string> &tango_hosts = db->get_multi_host();
			vector<string> &tango_ports = db->get_multi_port();
			for (unsigned int i = 1;i < tango_hosts.size();i++)
			{
				string prefix = "tango://" + tango_hosts[i] + ':' + tango_ports[i] + '/' ;
				env_var_fqdn_prefix.push_back(prefix);
			}
		}
	}
	catch (Tango::DevFailed)
	{
		env_var_fqdn_prefix.push_back(TangoHostNotSet);
	}

	start_undetached();
}

EventConsumer *EventConsumer::create()
{
//
// check if the EventConsumer singleton exists, if so return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// EventConsumer singleton does not exist, create it
//

	ApiUtil *ptr =  ApiUtil::instance();
	return new EventConsumer(ptr);
}

//
// activate POA and go into endless loop waiting for events to be pushed
// this method should run as a separate thread so as not to block the client
//

void *EventConsumer::run_undetached(void *arg)
{
	// initialise the unique event id for the client;
	subscribe_event_id = 0;
	
	cmd.cmd_pending = false;
	keep_alive_thread = new EventConsumerKeepAliveThread(cmd);
	keep_alive_thread->start();

	ApiUtil *api_util_ptr = (ApiUtil *)arg;
	if (api_util_ptr->in_server() == false)
	{
		CORBA::Object_var obj = orb_->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
       	PortableServer::POAManager_var pman = poa->the_POAManager();
       	pman->activate();

       	api_util_ptr->need_reset_already_flag(true);
		orb_->run();

		orb_->destroy();
		
		CORBA::release(orb_);
	}
	
	return (void *)NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::disconnect_from_notifd()
// 
// description : 	Method to stop the keep alive thread and to
//                disconnect from all used notifd event channels
//
//-----------------------------------------------------------------------------
void EventConsumer::disconnect_from_notifd()
{
	cout3 << "calling Tango::EventConsumer::disconnect_from_notifd() \n";
	
//
// Shut-down the KeepAliveThread and wait for it to exit
//
	
	{
		omni_mutex_lock sync(cmd);
			
		cmd.cmd_pending = true;
		cmd.cmd_code = EXIT_TH;
			
		cmd.cond.signal();
	}
				
	int *rv;
	keep_alive_thread->join((void **)&rv);
			
//
// Destroy DeviceProxy object stored in map container
//

	cleanup_EventChannel_map();			
}


void EventConsumer::disconnect_structured_push_consumer()
{
	cout3 << "calling Tango::EventConsumer::disconnect_structured_push_consumer() \n";
}

void EventConsumer::offer_change(const CosNotification::EventTypeSeq& added,
                                 const CosNotification::EventTypeSeq& deled)
{
	cout3 << "calling Tango::EventConsumer::subscription_change() \n";
}






void EventConsumer::connect(DeviceProxy *device_proxy)
{

//
// Build fully qualified device name
//

	string device_name = device_proxy->dev_name();

	if (device_proxy->get_from_env_var() == false)
	{
		string prot("tango://");
		prot = prot + device_proxy->get_db_host() + ':' + device_proxy->get_db_port() + '/';
		device_name.insert(0,prot);
	}
	else
	{
		device_name.insert(0,env_var_fqdn_prefix[0]);
	}
	
	string adm_name;
	try
	{
		adm_name = device_proxy->adm_name();
	}
	catch(...)
	{
		TangoSys_OMemStream o;
		o << "Can't subscribe to event for device " << device_name << "\n";
		o << "Check that device server is running..." << ends;
		Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        o.str(),
				        (const char *)"EventConsumer::connect()");
	}
	
	string channel_name = adm_name;
	if (device_proxy->get_from_env_var() == true)
	{
		channel_name.insert(0,env_var_fqdn_prefix[0]);
	}

//
// If no connection exists to this channel then connect to it.
// Sometimes, this method is called in order to reconnect
// to the notifd. In such a case, the lock is already
// locked before the method is called
//

	std::map<std::string,EventChannelStruct>::iterator ipos;

	{
		ipos = channel_map.find(channel_name);

		if (ipos == channel_map.end())
		{
			connect_event_channel(channel_name,device_proxy->get_device_db(),false);
		}
		if (channel_map[channel_name].adm_device_proxy != NULL)
			delete channel_map[channel_name].adm_device_proxy;
		try
		{
			if (adm_name.find("tango://",0) == string::npos)
			{
				if (device_proxy->get_from_env_var() == false &&
				   device_proxy->get_db_host() != NOT_USED )
				{
					string added_str = device_proxy->get_db_host();
					added_str = added_str + ':' + device_proxy->get_db_port() + '/';
					adm_name.insert(0,added_str);
				}
			}
			channel_map[channel_name].adm_device_proxy = new DeviceProxy(adm_name);
			if (device_proxy->get_from_env_var() == true)
			{
				adm_name.insert(0,env_var_fqdn_prefix[0]);
			}
		}
		catch (Tango::DevFailed &)
		{
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Cannot contact the DS admin dev for your device" << ends;
			Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        o.str(),
				        (const char *)"EventConsumer::connect()");
		}
		channel_map[channel_name].full_adm_name = adm_name;
	}
	
	device_channel_map[device_name] = channel_name;
}






void EventConsumer::connect_event_channel(string &channel_name,Database *db,bool reconnect)
{
	CORBA::Any_var received;
	const DevVarLongStringArray *dev_import_list;
	
//
// Get a reference to an EventChannel for this device server from the 
// TANGO database or from the DS admin device (for device in a DS
// started with the -file option)
//

	int channel_exported;
	string channel_ior;
	string hostname;

	if (db != NULL)
	{
	
//
// Remove extra info from channel name (protocol,  dbase=xxx)
//

		string local_channel_name(channel_name);
		string::size_type pos;
		if ((pos = local_channel_name.find('#')) != string::npos)
			local_channel_name.erase(pos);
		if ((pos = local_channel_name.find("://")) != string::npos)
		{
			pos = pos + 3;
			if ((pos = local_channel_name.find('/',pos)) != string::npos)
				local_channel_name.erase(0,pos + 1);
		}
		
//
// Import channel event
//

		try 
		{
			received = db->import_event(local_channel_name);
		}
		catch (...) 
		{
			TangoSys_OMemStream o;

			o << channel_name;
			o << " has no event channel defined in the database\n";
			o << "Maybe the server is not running or is not linked with Tango release 4.x (or above)... " << ends;
			Except::throw_exception((const char *)"API_NotificationServiceFailed",
			      			o.str(),
			      			(const char *)"EventConsumer::connect_event_channel");
		}
	
     	received.inout() >>= dev_import_list;
     	channel_ior = string((dev_import_list->svalue)[1]);
     	channel_exported = dev_import_list->lvalue[0];
			
	  	// get the hostname where the notifyd should be running
		hostname = string (dev_import_list->svalue[3]);
	}
	else
	{
		DeviceProxy adm(channel_name);
		
		try
		{
			DeviceData dd;
			dd = adm.command_inout("QueryEventChannelIOR");
			dd >> channel_ior;
			channel_exported = true;
			
			// get the hostname where the notifyd should be running
			DeviceInfo info = adm.info();
			hostname = info.server_host;		
		}
		catch (...)
		{
			TangoSys_OMemStream o;

			o << channel_name;
			o << " has no event channel\n";
			o << "Maybe the server is not running or is not linked with Tango release 4.x (or above)... " << ends;
			Except::throw_exception((const char *)"API_NotificationServiceFailed",
			      			o.str(),
			      			(const char *)"EventConsumer::connect_event_channel");
		}
		
//
// Remove extra info from channel name (protocol,  dbase=xxx)
//

		string::size_type pos;
		if ((pos = channel_name.find('#')) != string::npos)
			channel_name.erase(pos);
		if ((pos = channel_name.find("://")) != string::npos)
		{
			pos = pos + 3;
			if ((pos = channel_name.find('/',pos)) != string::npos)
				channel_name.erase(0,pos + 1);
		}
	}

	if (channel_exported)
	{
		try
		{
        	CORBA::Object *event_channel_obj;
        	event_channel_obj = orb_ -> string_to_object(channel_ior.c_str());

        	if (event_channel_obj -> _non_existent())
                event_channel_obj = CORBA::Object::_nil();
		
			eventChannel = CosNotifyChannelAdmin::EventChannel::_nil();
			eventChannel = CosNotifyChannelAdmin::EventChannel::_narrow(event_channel_obj);
			
			if(CORBA::is_nil(eventChannel))
			{
				channel_exported = 0;
			}
		}
		catch (...)
		{
                        //cerr << "Failed to narrow EventChannel from notification daemon (hint: make sure the notifd process is running on this host)" << endl;
         	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                        	(const char*)"Failed to narrow EventChannel from notification daemon (hint: make sure the notifd process is running on this host)",
                        	(const char*)"EventConsumer::connect_event_channel()");        
		}
	}
	else
	{
    	EventSystemExcept::throw_exception((const char*)"API_EventChannelNotExported",
                        	(const char*)"Failed to narrow EventChannel (hint: make sure a notifd process is running on the server host)",
                        	(const char*)"EventConsumer::connect_event_channel()");  
	}
			
//
// Obtain a Consumer Admin
//

//
// We'll use the channel's default Consumer admin
//

	try
	{
       	consumerAdmin = eventChannel->default_consumer_admin();

		if (CORBA::is_nil(consumerAdmin))
		{
        		//cerr << "Could not get CosNotifyChannelAdmin::ConsumerAdmin" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::connect_event_channel()");        
        	exit((void*)1);
    	}
	}
	catch (...)
	{
     	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::connect_event_channel()");        
	}

//
// Obtain a Proxy Supplier
//

//
// We are using the "Push" model and Structured data
//

	try
	{
	    proxySupplier =
		consumerAdmin -> obtain_notification_push_supplier(CosNotifyChannelAdmin::STRUCTURED_EVENT, proxyId);
		if (CORBA::is_nil(proxySupplier))
		{
        		//cerr << "Could not get CosNotifyChannelAdmin::ProxySupplier" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to obtain a push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::connect_event_channel()");        
    	}

        structuredProxyPushSupplier =
		CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow(
		    		proxySupplier);
		if (CORBA::is_nil(structuredProxyPushSupplier))
		{
       			//cerr << "Tango::EventConsumer::EventConsumer() could not get CosNotifyChannelAdmin::StructuredProxyPushSupplier" << endl;
       		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to narrow the push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::connect_event_channel()");        
    	}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get PushSupplier from notification daemon due to AdminLimitExceeded (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::connect_event_channel()");        
	}

//
// Connect to the Proxy Consumer
//

	try
	{
	    structuredProxyPushSupplier -> connect_structured_push_consumer(_this());
	}
	catch(const CosEventChannelAdmin::AlreadyConnected&)
	{
       	cerr << "Tango::EventConsumer::EventConsumer() caught AlreadyConnected exception" << endl;
	}

	EventChannelStruct new_event_channel_struct;

	if (reconnect == true)
	{
		channel_map[channel_name].eventChannel = eventChannel;
		channel_map[channel_name].structuredProxyPushSupplier = structuredProxyPushSupplier;
		channel_map[channel_name].last_heartbeat = time(NULL);
		channel_map[channel_name].heartbeat_skipped = false;
		channel_map[channel_name].notifyd_host = hostname;
		channel_map[channel_name].notifd_failed = false;
		channel_map[channel_name].has_notifd_closed_the_connection = 0;
	}
	else
	{
		new_event_channel_struct.eventChannel = eventChannel;
		new_event_channel_struct.structuredProxyPushSupplier = structuredProxyPushSupplier;
		new_event_channel_struct.last_heartbeat = time(NULL);
		new_event_channel_struct.heartbeat_skipped = false;
		new_event_channel_struct.adm_device_proxy = NULL;
		// create a channel monitor
		new_event_channel_struct.channel_monitor = new TangoMonitor();
		// set the timeout for the channel monitor to 500ms not to block the event consumer for to long.
		new_event_channel_struct.channel_monitor->timeout(500);

		channel_map[channel_name] = new_event_channel_struct;
		channel_map[channel_name].notifyd_host = hostname;
		channel_map[channel_name].notifd_failed = false;
		channel_map[channel_name].has_notifd_closed_the_connection = 0;
	}

//
// add a filter for heartbeat events
//

    char constraint_expr[256];
    ::sprintf(constraint_expr,"$event_name == \'heartbeat\'");
    CosNotifyFilter::FilterFactory_var ffp;
    CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
    try
	{
		ffp    = channel_map[channel_name].eventChannel->default_filter_factory();
		filter = ffp->create_filter("EXTENDED_TCL");
	}
	catch (...)
	{
                //cerr << "Caught exception obtaining filter object" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating heartbeat filter (check filter)",
                       	(const char*)"EventConsumer::connect_event_channel()");        
	}

//	
// Construct a simple constraint expression; add it to fadmin
//

    CosNotification::EventTypeSeq evs;
    CosNotifyFilter::ConstraintExpSeq exp;
    exp.length(1);
    exp[0].event_types = evs;
    exp[0].constraint_expr = CORBA::string_dup(constraint_expr);
    CORBA::Boolean res = 0; // OK
    try
	{
   		CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);
		channel_map[channel_name].heartbeat_filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
	}
	catch (...)
	{
		res = 1; // error     
	}
	
//
// If error, destroy filter
//
	
	if (res == 1)
	{ 
		try
		{
			filter->destroy();
		}
		catch (...) { }
		
		filter = CosNotifyFilter::Filter::_nil();
		
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while adding constraint for heartbeat (check filter)",
                       	(const char*)"EventConsumer::connect_event_channel()");
	}
}

void EventConsumer::attr_to_device(const AttributeValue *attr_value,
				   const AttributeValue_3 *attr_value_3,
				   long vers,DeviceAttribute *dev_attr)
{
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarLong64Array *tmp_seq_64;
	CORBA::LongLong *tmp_64;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	const DevVarFloatArray *tmp_seq_fl;
	CORBA::Float *tmp_fl;
	const DevVarBooleanArray *tmp_seq_boo;
	CORBA::Boolean *tmp_boo;
	const DevVarUShortArray *tmp_seq_ush;
	CORBA::UShort *tmp_ush;
	const DevVarCharArray *tmp_seq_uch;
	CORBA::Octet *tmp_uch;
	const DevVarULongArray *tmp_seq_ulo;
	CORBA::ULong *tmp_ulo;
	const DevVarULong64Array *tmp_seq_u64;
	CORBA::ULongLong *tmp_ulolo;
	const DevVarStateArray *tmp_seq_state;
	Tango::DevState *tmp_state;
		
	CORBA::ULong max,len;

	if (vers == 3)
	{	
		dev_attr->name = attr_value_3->name;
		dev_attr->quality = attr_value_3->quality;
		dev_attr->time = attr_value_3->time;
		dev_attr->dim_x = attr_value_3->r_dim.dim_x;
		dev_attr->dim_y = attr_value_3->r_dim.dim_y;
		dev_attr->ext->w_dim_x = attr_value_3->w_dim.dim_x;
		dev_attr->ext->w_dim_y = attr_value_3->w_dim.dim_y;
		dev_attr->ext->err_list = new DevErrorList(attr_value_3->err_list);
	}
	else
	{
		dev_attr->name = attr_value->name;
		dev_attr->quality = attr_value->quality;
		dev_attr->time = attr_value->time;
		dev_attr->dim_x = attr_value->dim_x;
		dev_attr->dim_y = attr_value->dim_y;
	}
	
	if (dev_attr->quality != Tango::ATTR_INVALID)
	{	
		CORBA::TypeCode_var ty;
		if (vers == 3)
			ty = attr_value_3->value.type();
		else
			ty = attr_value->value.type();
			
		if (ty->kind() == tk_enum)
		{
			attr_value_3->value >>= dev_attr->d_state;
			dev_attr->d_state_filled = true;
		}
		else
		{		
			CORBA::TypeCode_var ty_alias = ty->content_type();
			CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
			switch (ty_seq->kind())
			{
				case tk_long:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_lo;
					else		
						attr_value->value >>= tmp_seq_lo;
					max = tmp_seq_lo->maximum();
					len = tmp_seq_lo->length();
					if (tmp_seq_lo->release() == true)
					{
						tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
						dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
					}
					else
					{
						tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
						dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
					}
					break;
	
				case tk_longlong:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_64;
					else		
						attr_value->value >>= tmp_seq_64;
					max = tmp_seq_64->maximum();
					len = tmp_seq_64->length();
					if (tmp_seq_64->release() == true)
					{
						tmp_64 = (const_cast<DevVarLong64Array *>(tmp_seq_64))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_64,true);
					}
					else
					{
						tmp_64 = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
						dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_64,false);
					}
					break;
					
				case tk_short:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_sh;
					else
						attr_value->value >>= tmp_seq_sh;
					max = tmp_seq_sh->maximum();
					len = tmp_seq_sh->length();
					if (tmp_seq_sh->release() == true)
					{
						tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
						dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
					}
					else
					{
						tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
						dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
					}
					break;
	
				case tk_double:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_db;
					else
						attr_value->value >>= tmp_seq_db;
					max = tmp_seq_db->maximum();
					len = tmp_seq_db->length();
					if (tmp_seq_db->release() == true)
					{
						tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
						dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
					}
					else
					{
						tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
						dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
					}
					break;
	
				case tk_string:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_str;
					else
						attr_value->value >>= tmp_seq_str;
					max = tmp_seq_str->maximum();
					len = tmp_seq_str->length();
					if (tmp_seq_str->release() == true)
					{
						tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
						dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,true);
					}
					else
					{
						tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
						dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,false);
					}
					break;
	
				case tk_float:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_fl;
					else		
						attr_value->value >>= tmp_seq_fl;
					max = tmp_seq_fl->maximum();
					len = tmp_seq_fl->length();
					if (tmp_seq_fl->release() == true)
					{
						tmp_fl = (const_cast<DevVarFloatArray *>(tmp_seq_fl))->get_buffer((CORBA::Boolean)true);
						dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
					}
					else
					{
						tmp_fl = const_cast<CORBA::Float *>(tmp_seq_fl->get_buffer());
						dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,false);
					}
					break;
	
				case tk_boolean:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_boo;
					else		
						attr_value->value >>= tmp_seq_boo;
					max = tmp_seq_boo->maximum();
					len = tmp_seq_boo->length();
					if (tmp_seq_boo->release() == true)
					{
						tmp_boo = (const_cast<DevVarBooleanArray *>(tmp_seq_boo))->get_buffer((CORBA::Boolean)true);
						dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
					}
					else
					{
						tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq_boo->get_buffer());
						dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,false);
					}
					break;
	
				case tk_ushort:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_ush;
					else		
						attr_value->value >>= tmp_seq_ush;
					max = tmp_seq_ush->maximum();
					len = tmp_seq_ush->length();
					if (tmp_seq_ush->release() == true)
					{
						tmp_ush = (const_cast<DevVarUShortArray *>(tmp_seq_ush))->get_buffer((CORBA::Boolean)true);
						dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
					}
					else
					{
						tmp_ush = const_cast<CORBA::UShort *>(tmp_seq_ush->get_buffer());
						dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,false);
					}
					break;
	
				case tk_octet:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_uch;
					else		
						attr_value->value >>= tmp_seq_uch;
					max = tmp_seq_uch->maximum();
					len = tmp_seq_uch->length();
					if (tmp_seq_uch->release() == true)
					{
						tmp_uch = (const_cast<DevVarCharArray *>(tmp_seq_uch))->get_buffer((CORBA::Boolean)true);
						dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
					}
					else
					{
						tmp_uch = const_cast<CORBA::Octet *>(tmp_seq_uch->get_buffer());
						dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,false);
					}
					break;
					
				case tk_ulong:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_ulo;
					else		
						attr_value->value >>= tmp_seq_ulo;
					max = tmp_seq_ulo->maximum();
					len = tmp_seq_ulo->length();
					if (tmp_seq_ulo->release() == true)
					{
						tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
					}
					else
					{
						tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq_ulo->get_buffer());
						dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
					}
					break;
					
				case tk_ulonglong:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_u64;
					else		
						attr_value->value >>= tmp_seq_u64;
					max = tmp_seq_u64->maximum();
					len = tmp_seq_u64->length();
					if (tmp_seq_u64->release() == true)
					{
						tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_u64))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
					}
					else
					{
						tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq_u64->get_buffer());
						dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
					}
					break;
					
				case tk_enum:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_state;
					else		
						attr_value->value >>= tmp_seq_state;
					max = tmp_seq_state->maximum();
					len = tmp_seq_state->length();
					if (tmp_seq_state->release() == true)
					{
						tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean)true);
						dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
					}
					else
					{
						tmp_state = const_cast<Tango::DevState *>(tmp_seq_state->get_buffer());
						dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
					}
					break;
					
				default:
					break;
				}
		}
	}	
}

void EventConsumer::attr_to_device(const AttributeValue_4 *attr_value_4,long vers,DeviceAttribute *dev_attr)
{

	CORBA::Long *tmp_lo;
	CORBA::Short *tmp_sh;
	CORBA::Double *tmp_db;
	char **tmp_str;
	CORBA::Float *tmp_fl;
	CORBA::Boolean *tmp_boo;
	CORBA::UShort *tmp_ush;
	CORBA::Octet *tmp_uch;
	CORBA::LongLong *tmp_lolo;
	CORBA::ULong *tmp_ulo;
	CORBA::ULongLong *tmp_ulolo;
	Tango::DevState *tmp_state;
	Tango::DevState sta_dev;
	Tango::DevEncoded *tmp_enc;
		
	CORBA::ULong max,len;

	dev_attr->name = attr_value_4->name;
	dev_attr->quality = attr_value_4->quality;
	dev_attr->time = attr_value_4->time;
	dev_attr->dim_x = attr_value_4->r_dim.dim_x;
	dev_attr->dim_y = attr_value_4->r_dim.dim_y;
	dev_attr->ext->w_dim_x = attr_value_4->w_dim.dim_x;
	dev_attr->ext->w_dim_y = attr_value_4->w_dim.dim_y;
	dev_attr->ext->err_list = new DevErrorList(attr_value_4->err_list);		
	
	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		switch (attr_value_4->value._d())
		{
			case ATT_BOOL:
			{
				const DevVarBooleanArray &tmp_seq = attr_value_4->value.bool_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_boo = (const_cast<DevVarBooleanArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
				}
				else
				{
					tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq.get_buffer());
					dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,false);
				}
			}
			break;
			
			case ATT_SHORT:
			{
				const DevVarShortArray &tmp_seq = attr_value_4->value.short_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_sh = (const_cast<DevVarShortArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
				}
				else
				{
					tmp_sh = const_cast<CORBA::Short *>(tmp_seq.get_buffer());
					dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
				}
			}
			break;
			
			case ATT_LONG:
			{
				const DevVarLongArray &tmp_seq = attr_value_4->value.long_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_lo = (const_cast<DevVarLongArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
				}
				else
				{
					tmp_lo = const_cast<CORBA::Long *>(tmp_seq.get_buffer());
					dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
				}
			}
			break;
			
			case ATT_LONG64:
			{
				const DevVarLong64Array &tmp_seq = attr_value_4->value.long64_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_lolo = (const_cast<DevVarLong64Array &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,true);
				}
				else
				{
					tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq.get_buffer());
					dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,false);
				}
			}
			break;
			
			case ATT_FLOAT:
			{
				const DevVarFloatArray &tmp_seq = attr_value_4->value.float_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_fl = (const_cast<DevVarFloatArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
				}
				else
				{
					tmp_fl = const_cast<CORBA::Float *>(tmp_seq.get_buffer());
					dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,false);
				}
			}
			break;
			
			case ATT_DOUBLE:
			{
				const DevVarDoubleArray &tmp_seq = attr_value_4->value.double_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_db = (const_cast<DevVarDoubleArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);	
				}
				else
				{
					tmp_db = const_cast<CORBA::Double *>(tmp_seq.get_buffer());
					dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
				}
			}
			break;
			
			case ATT_UCHAR:
			{
				const DevVarCharArray &tmp_seq = attr_value_4->value.uchar_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_uch = (const_cast<DevVarCharArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
				}
				else
				{
					tmp_uch = const_cast<CORBA::Octet *>(tmp_seq.get_buffer());
					dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,false);
				}
			}
			break;
			
			case ATT_USHORT:
			{
				const DevVarUShortArray &tmp_seq = attr_value_4->value.ushort_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_ush = (const_cast<DevVarUShortArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
				}
				else
				{
					tmp_ush = const_cast<CORBA::UShort *>(tmp_seq.get_buffer());
					dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,false);
				}
			}
			break;
			
			case ATT_ULONG:
			{
				const DevVarULongArray &tmp_seq = attr_value_4->value.ulong_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_ulo = (const_cast<DevVarULongArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
				}
				else
				{
					tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq.get_buffer());
					dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
				}
			}
			break;
			
			case ATT_ULONG64:
			{
				const DevVarULong64Array &tmp_seq = attr_value_4->value.ulong64_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_ulolo = (const_cast<DevVarULong64Array &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
				}
				else
				{
					tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq.get_buffer());
					dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
				}
			}
			break;
			
			case ATT_STRING:
			{
				const DevVarStringArray &tmp_seq = attr_value_4->value.string_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_str = (const_cast<DevVarStringArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,true);
				}
				else
				{
					tmp_str = const_cast<char **>(tmp_seq.get_buffer());
					dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,false);
				}
			}
			break;
			
			case ATT_STATE:
			{
				const DevVarStateArray &tmp_seq = attr_value_4->value.state_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_state = (const_cast<DevVarStateArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
				}
				else
				{
					tmp_state = const_cast<Tango::DevState *>(tmp_seq.get_buffer());
					dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
				}
			}
			break;
			
			case DEVICE_STATE:
				sta_dev = attr_value_4->value.dev_state_att();
				dev_attr->d_state_filled = true;
				dev_attr->d_state = sta_dev;
			break;
			
			case NO_DATA:
			break;
			
			case ATT_ENCODED:
			{
				const DevVarEncodedArray &tmp_seq = attr_value_4->value.encoded_att_value();
				max = tmp_seq.maximum();
				len = tmp_seq.length();
				if (tmp_seq.release() == true)
				{
					tmp_enc = (const_cast<DevVarEncodedArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
					dev_attr->ext->EncodedSeq = new DevVarEncodedArray(max,len,tmp_enc,true);
				}
				else
				{
					tmp_enc = const_cast<Tango::DevEncoded *>(tmp_seq.get_buffer());
					dev_attr->ext->EncodedSeq = new DevVarEncodedArray(max,len,tmp_enc,false);
				}
			}
			break;
		}
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::push_structured_event()
// 
// description : 	Method run when an event is received
//
// argument : in :	event : The event itself...
//
//-----------------------------------------------------------------------------


void EventConsumer::push_structured_event(const CosNotification::StructuredEvent& event)
{
	string domain_name(event.header.fixed_header.event_type.domain_name);
	string event_type(event.header.fixed_header.event_type.type_name);
	string event_name(event.header.fixed_header.event_name);

	bool svr_send_tg_host = false;
	
	if (event_name == "heartbeat")
	{	
		string fq_dev_name = domain_name;
		if (event_type.find("tango://") != string::npos)
		{
			fq_dev_name.insert(0,event_type);
			svr_send_tg_host = true;
		}
		else
			fq_dev_name.insert(0,env_var_fqdn_prefix[0]);
		
		// only reading from the maps
		map_modification_lock.readerIn();
		
		std::map<std::string,EventChannelStruct>::iterator ipos;
		ipos = channel_map.find(fq_dev_name);

//
// Search for entry within the channel_map map using
// 1 - The fully qualified device name
// 2 - The fully qualified device name but with the Tango database host name specifed without fqdn 
// (in case of)
// 3 - The device name (for old servers)
//
		
		if (ipos == channel_map.end())
		{
			string::size_type pos,end;
			if ((pos = event_type.find('.')) != string::npos)
			{
				end = event_type.find(':',pos);
				fq_dev_name.erase(pos,end - pos);

				ipos = channel_map.find(fq_dev_name);
				
				if (ipos == channel_map.end())
					ipos = channel_map.find(domain_name);
			}
			else
			{
				ipos = channel_map.find(domain_name);
			}			
		} 

//
// Special case for Tango system with multiple db server
//
// The event carry info for only one of the multiple db server
// The client also has to know the list of db servers (via the TANGO_HOST)
// Find the event db server in the client list of db server and if found,
// replace in the fqdn the event db server with the first one in the client
// list. The first db server is the one used to create the entry in the map
//

		if (ipos == channel_map.end() && svr_send_tg_host == true)
		{
			string svc_tango_host = event_type.substr(8,event_type.size() - 9);
			unsigned int i = 0;
			for (i = 1;i < env_var_fqdn_prefix.size();i++)
			{
				if (env_var_fqdn_prefix[i].find(svc_tango_host) != string::npos)
				{
					fq_dev_name = domain_name;
					fq_dev_name.insert(0,env_var_fqdn_prefix[0]);
					break;
				}
			}
				
			if (i != env_var_fqdn_prefix.size())
			{
				ipos = channel_map.find(fq_dev_name);
			}
		}
		
		
		if (ipos != channel_map.end())
		{
			try 
			{
				ipos->second.channel_monitor->get_monitor();
				ipos->second.last_heartbeat = time(NULL);
				ipos->second.channel_monitor->rel_monitor();	
			}
			catch (...)
			{
				cerr << "Tango::EventConsumer::push_structured_event() timeout on channel monitor of " << ipos->first << endl;
			}
		}
		
		map_modification_lock.readerOut();
	}	
	else
	{
		string fq_dev_name = domain_name;
		if (event_type.find("tango://") != string::npos)
		{
			fq_dev_name.insert(0,event_type);
			svr_send_tg_host = true;
		}
		else
			fq_dev_name.insert(0,env_var_fqdn_prefix[0]);

		map_modification_lock.readerIn();
		bool map_lock = true;


//
// Search for entry within the event_callback map using
// 1 - The fully qualified attribute event name
// 2 - The fully qualified attribute event name but with the Tango database host name specifed without fqdn 
// (in case of)
// 3 - In case of Tango system with multi db server, replace the db server defined by
// the event with the first one in the client list (like for the heartbeat event)
//
		
		string attr_event_name = fq_dev_name + "." + event_name;
		map<std::string,EventCallBackStruct>::iterator ipos;

		ipos = event_callback_map.find(attr_event_name);
		if (ipos == event_callback_map.end())
		{
			string::size_type pos,end;
			if ((pos = event_type.find('.')) != string::npos)
			{
				end = event_type.find(':',pos);
				attr_event_name.erase(pos,end - pos);

				ipos = event_callback_map.find(attr_event_name);
				
				if (ipos == event_callback_map.end() && svr_send_tg_host == true)
				{
					string svc_tango_host = event_type.substr(8,event_type.size() - 9);
					unsigned int i = 0;
					for (i = 1;i < env_var_fqdn_prefix.size();i++)
					{
						if (env_var_fqdn_prefix[i].find(svc_tango_host) != string::npos)
						{
							fq_dev_name = domain_name;
							fq_dev_name.insert(0,env_var_fqdn_prefix[0]);
							attr_event_name = fq_dev_name + "." + event_name;
							break;
						}
					}
				
					if (i != env_var_fqdn_prefix.size())
						ipos = event_callback_map.find(attr_event_name);
				}
			}
		}
	
		if ((ipos != event_callback_map.end()) && (ipos->second.id > 0))
		{
			try 
			{
				ipos->second.callback_monitor->get_monitor();
				
				CallBack *callback;
				callback = ipos->second.callback;
				EventQueue *ev_queue;
				ev_queue = ipos->second.ev_queue;
				
				map_lock = false;	
				map_modification_lock.readerOut();
				
				AttributeValue *attr_value = NULL;
				AttributeValue_3 *attr_value_3 = NULL;
				AttributeValue_4 *attr_value_4 = NULL;
				AttributeConfig_2 *attr_conf_2 = NULL;
				AttributeConfig_3 *attr_conf_3 = NULL;
				AttributeInfoEx *attr_info_ex = NULL;
				AttDataReady *att_ready = NULL;
				DevErrorList errors;
				bool ev_attr_conf = false;
				bool ev_attr_ready = false;
				const DevErrorList *err_ptr;
				
				
				if (callback != NULL || ev_queue != NULL)
				{

//
// Check if the event transmit error
//

					DeviceAttribute *dev_attr = NULL;
					CORBA::TypeCode_var ty = event.remainder_of_body.type();
					if (ty->kind() == tk_struct)
					{
						CORBA::String_var st_name;
						st_name = ty->name();
						const char *tmp_ptr = st_name.in();
						long vers;
						if (::strcmp(tmp_ptr,"AttributeValue_4") == 0)
						{
							dev_attr = new (DeviceAttribute);
							event.remainder_of_body >>= attr_value_4;
							vers = 4;
							attr_to_device(attr_value_4,vers,dev_attr);
						}
						else if (::strcmp(tmp_ptr,"AttributeValue_3") == 0)
						{
							dev_attr = new (DeviceAttribute);
							event.remainder_of_body >>= attr_value_3;
							vers = 3;
							attr_to_device(attr_value,attr_value_3,vers,dev_attr);
						}
						else if (::strcmp(tmp_ptr,"AttributeValue") == 0)
						{
							dev_attr = new (DeviceAttribute);
							event.remainder_of_body >>= attr_value;
							vers = 2;
							attr_to_device(attr_value,attr_value_3,vers,dev_attr);
						}
						else if (::strcmp(tmp_ptr,"AttributeConfig_2") == 0)
						{
							event.remainder_of_body >>= attr_conf_2;
							vers = 2;
							attr_info_ex = new AttributeInfoEx();
							*attr_info_ex = attr_conf_2;
							ev_attr_conf = true;
						}
						else if (::strcmp(tmp_ptr,"AttributeConfig_3") == 0)
						{
							event.remainder_of_body >>= attr_conf_3;
							vers = 3;
							attr_info_ex = new AttributeInfoEx();
							*attr_info_ex = attr_conf_3;
							ev_attr_conf = true;
						}
						else if (::strcmp(tmp_ptr,"AttDataReady") == 0)
						{
							event.remainder_of_body >>= att_ready;
							ev_attr_conf = false;
							ev_attr_ready = true;
						}
						else
						{
							errors.length(1);

							errors[0].severity = Tango::ERR;
							errors[0].origin = CORBA::string_dup("EventConsumer::push_structured_event()");
							errors[0].reason = CORBA::string_dup("API_IncompatibleAttrDataType");
							errors[0].desc = CORBA::string_dup("Unknown structure used to pass attribute value (Need compilation ?)");
							dev_attr = NULL;						
						}

					}
					else
					{
						event.remainder_of_body >>= err_ptr;
						errors = *err_ptr;

//
// We need to find which type of event we have received
//

						string::size_type pos = attr_event_name.find('.');
						string att_type = attr_event_name.substr(pos + 1);
						if (att_type == CONF_TYPE_EVENT)
							ev_attr_conf = true;
						else if (att_type == DATA_READY_TYPE_EVENT)
							ev_attr_ready = true;
					}			

//
// Fire the user callback
//

					if ((ev_attr_conf == false) && (ev_attr_ready == false))
					{
						EventData *event_data = new EventData (event_callback_map[attr_event_name].device,
															  fq_dev_name,
															  event_name,
															  dev_attr,
															  errors);
					
						// if a callback method was specified, call it!
						if (callback != NULL )
						{
							try
							{
								callback->push_event(event_data);
							}
							catch (...)
							{
								cerr << "Tango::EventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
							}						
						
							delete dev_attr;
							event_data->attr_value = NULL;
							delete event_data;
						}
						
						// no calback method, the event has to be instered
						// into the event queue
						else
						{
							ev_queue->insert_event(event_data);
						}
					}
					else if (ev_attr_ready == false)
					{
						AttrConfEventData *event_data = new AttrConfEventData(event_callback_map[attr_event_name].device,
															  fq_dev_name,
															  event_name,
															  attr_info_ex,
															  errors);
						
						// if a callback method was specified, call it!
						if (callback != NULL )
						{
							try
							{
								callback->push_event(event_data);
							}
							catch (...)
							{
								cerr << "Tango::EventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
							}
						
							delete attr_info_ex;
							event_data->attr_conf = NULL;
							delete event_data;
						}
						
						// no calback method, the event has to be instered
						// into the event queue
						else
						{
							ev_queue->insert_event(event_data);
						}
					}
					else
					{
						DataReadyEventData *event_data = new DataReadyEventData(event_callback_map[attr_event_name].device,
																att_ready,event_name,errors);
						// if a callback method was specified, call it!
						if (callback != NULL )
						{
							try
							{
								callback->push_event(event_data);
							}
							catch (...)
							{
								cerr << "Tango::EventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
							}
							delete event_data;
						}
						
						// no calback method, the event has to be instered
						// into the event queue
						else
						{
							ev_queue->insert_event(event_data);
						}
					}
				}

				ipos->second.callback_monitor->rel_monitor();
				
			}
			catch (...)
			{
				// free the map lock if not already done
				if ( map_lock == true )
				{
					map_modification_lock.readerOut();
				}
				ipos->second.callback_monitor->rel_monitor();
				
				cerr << "Tango::EventConsumer::push_structured_event() timeout on callback monitor of " << ipos->first << endl;
			}				
		}
		else
		{
		// even if nothing was found in the map, free the lock
			map_modification_lock.readerOut();
		}	
	}

}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
// 
// description : 	Method to subscribe to an event with the callback mechanism. 
//                  Can be called in a stateless way, that it even works 
//                  when the attribute is not available.
//
// argument : in :  device    : The device handle
//			        attribute : The name of the attribute
//			        event     : The type of event to subscribe for
//			        callback  : A pointer to the callback object
//			        filters   : Eventual event filter strings
//                  stateless : Flag to enable the stateless connection when set to true
//
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   const vector<string> &filters,
					bool stateless)
{
	if ((device == NULL) || (callback == NULL))
	{		
		EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"Device or callback pointer NULL",
                       	(const char*)"EventConsumer::subscribe_event()");
	}

	return (subscribe_event (device, attribute, event, callback, NULL, filters, stateless));
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
// 
// description : 	Method to subscribe to an event with the event_queue mechanism. 
//                  Can be called in a stateless way, that it even works 
//                  when the attribute is not available.
//
// argument : in :  device    : The device handle
//			        attribute : The name of the attribute
//			        event     : The type of event to subscribe for
//			        event_queue_size:  The size of the circular buffer for incoming
//                                     events
//			        filters   : Eventual event filter strings
//                  stateless : Flag to enable the stateless connection when set to true
//
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   int event_queue_size,
				   const vector<string> &filters,
				   bool stateless)
{
	if ((device == NULL) || (event_queue_size < 0))
	{		
		EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"Device pointer is NULL or the event queue size is invalid",
                       	(const char*)"EventConsumer::subscribe_event()");
	}

	// create an event queue object
	
	EventQueue *ev_queue = new EventQueue(event_queue_size);
	
	return (subscribe_event (device, attribute, event, NULL, ev_queue, filters, stateless));
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
// 
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   EventQueue *ev_queue,
				   const vector<string> &filters,
				   bool stateless)
{

//
// Get device name string and add protocol definition
//

	string device_name = device->dev_name();
	
	if (device->get_from_env_var() == false)
	{
		string prot("tango://");
		prot = prot + device->get_db_host() + ':' + device->get_db_port() + '/';
		device_name.insert(0,prot);
	}
	else
	{
		if (env_var_fqdn_prefix[0] == TangoHostNotSet)
		{
			EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                (const char*)"Nor TANGO_HOST env. variable or fully qualified device name used. Can't use event!",
                (const char*)"EventConsumer::subscribe_event()");
		}
		else
			device_name.insert(0,env_var_fqdn_prefix[0]);
	}
	
	cout3 << "Tango::EventConsumer::subscribe_event(" << device_name << "," << attribute <<"," << event << ")\n";
	
	string event_name;	
	switch (event) 
	{
		case CHANGE_EVENT : event_name = "change";
				    break;

		case QUALITY_EVENT : event_name = "quality";
					  EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"The quality change event does`nt exist any more. A change event is fired on a qaulity change!",
                       	(const char*)"EventConsumer::subscribe_event()");
				    
					  break;

		case PERIODIC_EVENT : event_name = "periodic";
				      break;

		case ARCHIVE_EVENT : event_name = "archive";
				     break;
				     
		case USER_EVENT : event_name = "user_event";
				  break;
				  
		case ATTR_CONF_EVENT : event_name = "attr_conf";
					      break;

		case DATA_READY_EVENT: event_name = "data_ready";
					break;
	}

//
// Build callback map key
//
	
	string att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	string callback_key = device_name + "/" + att_name_lower + "." + event_name;

// only reading from the maps

	{
		ReaderLock r (map_modification_lock);

//
// If a callbck already exist for this event.
// Don't forget the case where a delayed unsubscribe is not yet finished
//

		std::map<std::string,EventCallBackStruct>::iterator epos;
		epos = event_callback_map.find(callback_key);
		if (epos != event_callback_map.end())
		{
			if (epos->second.id > 0)
			{
				TangoSys_OMemStream o;

				o << "Already connected to event " << callback_key << ends;
				EventSystemExcept::throw_exception((const char*)"API_MethodArgument",
                       		o.str(),
                       		(const char*)"EventConsumer::subscribe_event()");
			}
			else
			{
				omni_thread::yield();
				
				if (event_callback_map.find(callback_key) != event_callback_map.end())
				{
					TangoSys_OMemStream o;

					o << "Already connected to event " << callback_key << ends;
					EventSystemExcept::throw_exception((const char*)"API_MethodArgument",
                       			o.str(),
                       		(const char*)"EventConsumer::subscribe_event()");
				}					
			}
		}
	}
				
	try
	{
		int event_id = connect_event (device, attribute, event, callback, ev_queue,
		                             filters, event_name);
		return event_id;		
	}
		
	catch (Tango::DevFailed &)
	{
		// if the stateless flag is not true, rethrow the exception
		if ( stateless == false )
		{
			throw;
		}
		
		// when the subscribe event has not worked, store the connection data in a vector of not
		// yet connected events.
		// Retry to connect in the next heartbeat period.
		
		EventNotConnected conn_params;
		conn_params.device           = device;
		conn_params.attribute        = attribute;
		conn_params.event_type       = event;
		conn_params.event_name       = event_name;
		conn_params.callback         = callback;
		conn_params.ev_queue         = ev_queue;
		conn_params.filters          = filters;
		conn_params.last_heartbeat   = time(NULL);
		
		// protect the vector as the other maps!
		WriterLock w(map_modification_lock);
		
		// create and save the unique event ID
		subscribe_event_id++;	
		conn_params.event_id = subscribe_event_id;
	
		event_not_connected.push_back (conn_params);	
		return subscribe_event_id;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::connect_event()
// 
// description : 	Method to connect to an event and to
//                return an approriate exception if the 
//                connection fails.
//
// argument : in :	device : The device handle
//			        attribute : The name of the attribute
//			        event : The type of event to subscribe for
//			        callback : A pointer to the callback object
//					ev_queue : A pointer to the event queue
//			        filters : Eventual event filter strings
//					event_name : The event name
//                  event_id  : the unique event ID
//
//-----------------------------------------------------------------------------

int EventConsumer::connect_event(DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   EventQueue *ev_queue,
				   const vector<string> &filters,
				   string &event_name,
				   int event_id)
{
	int ret_event_id = event_id;
	string device_name = device->dev_name();
	cout3 << "Tango::EventConsumer::connect_event(" << device_name << "," << attribute <<"," << event << ")\n";

//
// Build callback map key and local device name from fqdn
//

	string local_device_name(device_name);
	if (device->get_from_env_var() == false)
	{
		string prot("tango://");
		prot = prot + device->get_db_host() + ':' + device->get_db_port() + '/';
		device_name.insert(0,prot);
	}
	else
	{
		device_name.insert(0,env_var_fqdn_prefix[0]);
	}
	
		
	string att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	string callback_key = device_name + "/" + att_name_lower + "." + event_name;

//
// Inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is
// down !)
// To do this, we need to build DS adm device proxy. If it is not the first call for this
// DS, we should find it in map. Otherwise, get it.
//

	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(local_device_name);
	subscriber_info.push_back(att_name_lower);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(event_name);
	subscriber_in << subscriber_info;


	DeviceProxy *adm_dev;	
	map<std::string,std::string>::iterator ipos;
	bool allocated = false;
	string channel_name;
	
	map_modification_lock.readerIn();

	ipos = device_channel_map.find(device_name);
	if (ipos == device_channel_map.end())
	{
		string adm_name;
		try
		{
			adm_name = device->adm_name();

			transform(adm_name.begin(),adm_name.end(),adm_name.begin(),::tolower);
			if (adm_name.find("tango://",0) == string::npos)
			{			
				if (device->get_from_env_var() == false && device->get_db_host() != NOT_USED)
				{
					string added_str = device->get_db_host();
					added_str = added_str + ':' + device->get_db_port() + '/';
					adm_name.insert(0,added_str);
				}
			}
			adm_dev = new DeviceProxy(adm_name);
			allocated = true;
		}
		catch(...)
		{
			map_modification_lock.readerOut();
			
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Check that device server is running..." << ends;
			Except::throw_exception((const char *)"API_BadConfigurationProperty",
				       	 	o.str(),
				         	(const char *)"EventConsumer::subscribe_event()");
		}
	}
	else
	{
		channel_name = device_channel_map[device_name];
		
		channel_map[channel_name].channel_monitor->get_monitor();
		adm_dev = channel_map[channel_name].adm_device_proxy;
		channel_map[channel_name].channel_monitor->rel_monitor();
	}

	Tango::DeviceData dd;
	
	try
	{
    	dd = adm_dev->command_inout("EventSubscriptionChange",subscriber_in);
		dd.reset_exceptions(DeviceData::isempty_flag);
		
//
// DS before Tango 7.1 does not send their Tango_host in the event
// Refuse to subsribe to an event from a DS before Tango 7.1 if the device
// is in another CS than the one defined by the TANGO_HOST env. variable
//

		if (dd.is_empty() == true)
		{
			if (device->get_from_env_var() == false)
			{
				string::size_type pos = device_name.find("://");
				pos = pos + 3;
				pos = device_name.find('/',pos);
				string fqdn_prefix = device_name.substr(0,pos + 1);
				transform(fqdn_prefix.begin(),fqdn_prefix.end(),fqdn_prefix.begin(),::tolower);

				if (fqdn_prefix != env_var_fqdn_prefix[0])
				{		
					TangoSys_OMemStream o;
					o << "Device server for device " << device_name;
					o << " is too old to generate event in a multi TANGO_HOST environment. Please, use Tango >= 7.1" << ends;					
					
					EventSystemExcept::throw_exception((const char*)"API_DSFailedRegisteringEvent",o.str(),
                      				(const char*)"EventConsumer::subscribe_event()");
				}
			}
		}
		else
		{
			Tango::DevLong vers;
			dd >> vers;
		}
	}
	catch (Tango::DevFailed &e)
	{
		map_modification_lock.readerOut();
		
		if (allocated == true)
			delete adm_dev;
		EventSystemExcept::re_throw_exception(e,(const char*)"API_DSFailedRegisteringEvent",
                       				(const char*)"Device server send exception while trying to register event",
                      				(const char*)"EventConsumer::subscribe_event()");
	}
	
	if (allocated == true)
		delete adm_dev;
	
//
// Search (or create) entry for channel map
//
	
	ipos = device_channel_map.find(device_name);
	if (ipos == device_channel_map.end())
	{
		// delete the readers lock and take the writer lock now.
		// The channel map will be modified.
		map_modification_lock.readerOut();
		WriterLock w(map_modification_lock);

		cout3 << "device " << device_name << " is not connected, going to connect to the event channel !\n";		
		connect(device);
		
		ipos = device_channel_map.find(device_name);
		if (ipos == device_channel_map.end())
		{
			TangoSys_OMemStream o;
			
			o << "Failed to connect to event channel for device " << device_name << ends;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       				o.str(),
                       				(const char*)"EventConsumer::subscribe_event()");
		}

		channel_name = device_channel_map[device_name];
		{
			channel_map[channel_name].last_subscribed = time(NULL);
		}
	}
	else
	{
		map_modification_lock.readerOut();
	}

//
// Build a filter using the CORBA Notify constraint Language
// (use attribute name in lowercase letters)
//

	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;

	// take the map reader lock again
	map_modification_lock.readerIn();
	
	try
	{
		channel_map[channel_name].channel_monitor->get_monitor();
    	ffp    = channel_map[channel_name].eventChannel->default_filter_factory();  
    	filter = ffp->create_filter("EXTENDED_TCL");
		channel_map[channel_name].channel_monitor->rel_monitor();
  	}
	catch (CORBA::COMM_FAILURE &)
	{
		map_modification_lock.readerOut();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	catch (...)
	{
		map_modification_lock.readerOut();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	
//  	
// Construct a simple constraint expression; add it to fadmin
//
	
	char constraint_expr[512];
	transform(local_device_name.begin(),local_device_name.end(),local_device_name.begin(),::tolower);
	
	::sprintf(constraint_expr,"$domain_name == \'%s/%s\' and $event_name == \'%s\'",
		local_device_name.c_str(),att_name_lower.c_str(),event_name.c_str());
		
	if (filters.size() != 0)
	{
		::strcat(&(constraint_expr[strlen(constraint_expr)])," and ((");
		for (unsigned int i = 0;i < filters.size();i++)
		{
			::strcat(&(constraint_expr[strlen(constraint_expr)]),filters[i].c_str());

			if (i != filters.size() - 1)			
				::strcat(&(constraint_expr[strlen(constraint_expr)])," and ");
		}
		::strcat(&(constraint_expr[strlen(constraint_expr)]),") or $forced_event > 0.5)");
	}

	CosNotification::EventTypeSeq evs;
  	CosNotifyFilter::ConstraintExpSeq exp;
  	exp.length(1);
  	exp[0].event_types = evs;
  	exp[0].constraint_expr = CORBA::string_dup(constraint_expr);
  	CORBA::Boolean res = 0; // OK
	
  	try
	{
		// delete the readers lock and take the writer lock now.
		// The callback map will be modified.
		map_modification_lock.readerOut();
		WriterLock w(map_modification_lock);
		
		
    	CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);
		{		
    		filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
		}

		EventCallBackStruct new_event_callback;
		new_event_callback.device = device;
		new_event_callback.attr_name = attribute;
		new_event_callback.event_name = event_name;
		new_event_callback.channel_name = channel_name;
		new_event_callback.callback = callback;
		new_event_callback.ev_queue = ev_queue;
		new_event_callback.filter_id = filter_id;
		new_event_callback.filter_constraint = constraint_expr;
		
		// if an event ID was passed to the method, reuse it!
		
		if ( ret_event_id <= 0 )
		{
			subscribe_event_id++;
			ret_event_id = subscribe_event_id;
		}
		new_event_callback.id = ret_event_id;
				
		// create a callback monitor
		new_event_callback.callback_monitor = new TangoMonitor();
		// set the timeout for the callback monitor to 500ms not to block the event consumer for to long.
		new_event_callback.callback_monitor->timeout(500);

		event_callback_map[callback_key] = new_event_callback;
  	}
  	catch(CosNotifyFilter::InvalidConstraint &)
	{
    	//cerr << "Exception thrown : Invalid constraint given "
	  	//     << (const char *)constraint_expr << endl;
		res = 1;
  	}
  	catch (...)
	{
    	//cerr << "Exception thrown while adding constraint " 
	 	//     << (const char *)constraint_expr << endl; 
		res = 1;
  	}
	
//
// If error, destroy filter. Else, set the filter_ok flag to true
//
	
  	if (res == 1)
	{ 
    	try
		{
      		filter->destroy();
    	}
		catch (...) { }
		
    	filter = CosNotifyFilter::Filter::_nil();
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	else
	{
		ReaderLock r(map_modification_lock);
		event_callback_map[callback_key].callback_monitor->get_monitor();
		event_callback_map[callback_key].filter_ok = true;
		event_callback_map[callback_key].callback_monitor->rel_monitor();
	}
	
//
// Read the attribute by a simple synchronous call
// This is necessary for the first point in "change" mode
// Force callback execution when it is done
//

	if ((event == CHANGE_EVENT) ||
	    (event == QUALITY_EVENT) || 
	    (event == ARCHIVE_EVENT) ||
	    (event == USER_EVENT))
	{
		ReaderLock r(map_modification_lock);
		
		//DeviceAttribute da;
		DeviceAttribute *da = NULL;
		DevErrorList err;
		err.length(0);
		string domain_name = device_name + "/" + att_name_lower;

		try
		{
			da = new DeviceAttribute();
			*da = device->read_attribute(attribute.c_str());
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}
				
		EventData *event_data = new EventData(device,
						      domain_name,
						      event_name,
						      da,
						      err);
		event_callback_map[callback_key].callback_monitor->get_monitor();
		
		// if a callback method was specified, call it!
		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
			}
				
		//event_data->attr_value = NULL;
		delete event_data;
		}
		
		// no calback method, the event has to be instered
		// into the event queue
		else
		{
			ev_queue->insert_event(event_data);
		}
		
		event_callback_map[callback_key].callback_monitor->rel_monitor();
	}
	
	
	else if (event == ATTR_CONF_EVENT)
	{
		ReaderLock r(map_modification_lock);
		
		DevErrorList err;
		err.length(0);
		string domain_name = device_name + "/" + att_name_lower;
		//AttributeInfoEx aie;
		AttributeInfoEx *aie = NULL;

		try
		{
			aie = new AttributeInfoEx();
			*aie = device->get_attribute_config(const_cast<string &>(attribute));
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}
		
		AttrConfEventData *event_data = new AttrConfEventData(device,
						      domain_name,
						      event_name,
						      aie,
						      err); 
		event_callback_map[callback_key].callback_monitor->get_monitor();					  
		
		// if a callback method was specified, call it!
		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
			}			
		//event_data->attr_conf = NULL;
			delete event_data;
		}
		
		// no calback method, the event has to be instered
		// into the event queue
		else
		{
			ev_queue->insert_event(event_data);
		}		
		
		event_callback_map[callback_key].callback_monitor->rel_monitor();
	}

	return ret_event_id;
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::unsubscribe_event()
// 
// description : 	Method to unsubscribe from an event
//
// argument : in :	event_id : The event identifier
//
//-----------------------------------------------------------------------------

void EventConsumer::unsubscribe_event(int event_id)
{

	if (event_id == 0)
	{
		EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");    
	}
	
	WriterLock w(map_modification_lock);

//
// First remove the callback entry from the callback map
//
	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
//			cout << "Tango::EventConsumer::unsubscribe_event() - found event id " << event_id << " going to remove_filter()\n";

//
// If the unsubscribe is done while the callback is being executed, mark the callback as unusable (event_id < 0)
// and start a thread which will do the unsubscribe when the callback execution will be finished
//
			
			if (epos->second.callback_monitor->get_locking_ctr() != 0)
			{
//				cout << "Unsubscribing for an event while it is in its call callback !!!!!!!!!!" << endl;
				epos->second.id = -event_id;
	
				DelayedEventUnsubThread *th = new DelayedEventUnsubThread(this,event_id,epos->second.callback_monitor);
				th->start();
	
				return;
			}
			
			try
			{
				CosNotifyFilter::Filter_var f = channel_map[epos->second.channel_name].structuredProxyPushSupplier->get_filter(epos->second.filter_id);
				channel_map[epos->second.channel_name].structuredProxyPushSupplier->remove_filter(epos->second.filter_id);
				f->destroy();
			}
			catch (...)
			{
				EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
					(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() (hint: check the Notification daemon is running ",
					(const char*)"EventConsumer::unsubscribe_event()");    
			}
			
			// delete the event queue when used
			if (epos->second.ev_queue != NULL)
				delete epos->second.ev_queue;
			
			// delete the allocated callback monitor
			delete epos->second.callback_monitor;

			string deleted_channel_name = epos->second.channel_name;
			event_callback_map.erase(epos);

//
// Check if there is another callback using the same channel
//
			
			std::map<std::string,EventCallBackStruct>::iterator cb_pos;
			bool channel_used_elsewhere = false;
			for (cb_pos = event_callback_map.begin(); cb_pos != event_callback_map.end(); ++cb_pos)
			{
				if (cb_pos->second.channel_name == deleted_channel_name)
				{
					channel_used_elsewhere = true;
					break;
				}
			}

//
// This channel is not used anymore in the app, remove its entry in the channel maps
//
			
			if (channel_used_elsewhere == false)
			{
				std::map<std::string,EventChannelStruct>::iterator chan_pos;
				for (chan_pos = channel_map.begin(); chan_pos != channel_map.end(); ++chan_pos)
				{
					if (chan_pos->first == deleted_channel_name)
					{
						if (chan_pos->second.adm_device_proxy != NULL)
						{
							try
							{
								CosNotifyFilter::Filter_var f = chan_pos->second.structuredProxyPushSupplier->get_filter(chan_pos->second.heartbeat_filter_id);
								chan_pos->second.structuredProxyPushSupplier->remove_filter(chan_pos->second.heartbeat_filter_id);
								f->destroy();

								chan_pos->second.structuredProxyPushSupplier->disconnect_structured_push_supplier();
							}
							catch (...)
							{
								EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
									(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() on the heartbeat filter (hint: check the Notification daemon is running ",
									(const char*)"EventConsumer::unsubscribe_event()");    
							}
							
							delete chan_pos->second.adm_device_proxy;
							if (chan_pos->second.channel_monitor != NULL)
								delete chan_pos->second.channel_monitor;
						}
			
						channel_map.erase(chan_pos);
						break;
					}
				}
				
				std::map<std::string,std::string>::iterator dev_pos,dev_pos_del;
				for (dev_pos = device_channel_map.begin(); dev_pos != device_channel_map.end();)
				{
					if (dev_pos->second == deleted_channel_name)
					{
						dev_pos_del = dev_pos;
						++dev_pos;
						device_channel_map.erase(dev_pos_del);

//						
// Don't "break" the loop! There may be more than one!
//

					} 
					else
						++dev_pos;
				}

			}
			
			return;
		}
	}
	
	// check also the vector of not yet connected events
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// delete the event queue when used
				if (vpos->ev_queue != NULL)
					delete vpos->ev_queue;
			   
			   // delete element from vector
			   event_not_connected.erase(vpos);
			   return;
			}
		}	
	}
		
	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");    
}


void DelayedEventUnsubThread::run(void *ptr)
{
	try
	{

//
// In case a callback is still in its execution, wait for it to terminate
//

		the_mon->timeout(3000);
		the_mon->get_monitor();
		the_mon->rel_monitor();

//
// Unsubscribe the event
//

		ev_cons->unsubscribe_event(-event_id);
	}
	catch(...) {}
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::cleanup_EventChannel_map()
// 
// description : 	Method to destroy the DeviceProxy objects
//			stored in the EventChannel map
//
//-----------------------------------------------------------------------------

void EventConsumer::cleanup_EventChannel_map()
{
	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); epos++)
	{
		try
		{
			CosNotifyFilter::Filter_var f = channel_map[epos->second.channel_name].structuredProxyPushSupplier->get_filter(epos->second.filter_id);
			channel_map[epos->second.channel_name].structuredProxyPushSupplier->remove_filter(epos->second.filter_id);
		
			f->destroy();
		}
		catch(...)
		{
			cerr << "Could not remove filter from notification daemon for " << epos->second.channel_name << endl;
		}
	}
	
	std::map<std::string,EventChannelStruct>::iterator ipos;
	{
		for (ipos = channel_map.begin(); ipos != channel_map.end(); ipos++)
		{
			if (ipos->second.adm_device_proxy != NULL)
			{
				ipos->second.channel_monitor->get_monitor();
				
				try
				{
					// Destroy the filter created in the
					// notification service for the heartbeat event
				
					CosNotifyFilter::Filter_var f = ipos->second.structuredProxyPushSupplier->get_filter(ipos->second.heartbeat_filter_id);
					ipos->second.structuredProxyPushSupplier->remove_filter(ipos->second.heartbeat_filter_id);
					f->destroy();
				
					// disconnect the pushsupplier to stop receiving events
				
					//cout << "EventConsumer::cleanup_EventChannel_map(): Disconnect push supplier!" << endl;
				
					ipos->second.structuredProxyPushSupplier->disconnect_structured_push_supplier();
				}
				catch(...)
				{
					cerr << "Could not remove heartbeat filter from notification daemon for " << ipos->second.full_adm_name << endl;
				}						
				
				// Release the connection to the device server administration device
				
				delete ipos->second.adm_device_proxy;
				ipos->second.adm_device_proxy = NULL;
				
				ipos->second.channel_monitor->rel_monitor();
			}
		}
	}
}



//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_events()
// 
// description :  Return a vector with all events stored in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied! 
// 
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void EventConsumer::get_events (int event_id, EventDataList &event_list)
{

	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// get the events from the queue
				epos->second.ev_queue->get_events (event_list);
				return;
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");    
}

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_events()
// 
// description :  Return a vector with all attribute configuration events 
//                stored in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied! 
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void EventConsumer::get_events (int event_id, AttrConfEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// get the events from the queue
				epos->second.ev_queue->get_events (event_list);
				return;
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");    
}

void EventConsumer::get_events (int event_id, DataReadyEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// get the events from the queue
				epos->second.ev_queue->get_events (event_list);
				return;
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");    
}

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_events()
// 
// description :  Call the callback method for all events stored 
//                in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied! 
// 
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void EventConsumer::get_events (int event_id, CallBack *cb)
{

	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// get the events from the queue
				epos->second.ev_queue->get_events (cb);
				return;
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (cb);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");    
}





//+----------------------------------------------------------------------------
//
// method :       EventConsumer::event_queue_size()
// 
// description :  Returns the number of events stored in the event queue 
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------
int  EventConsumer::event_queue_size(int event_id)
{
	cout3 << "EventConsumer::event_queue_size() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// get the event queue size
				return (epos->second.ev_queue->size());
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::event_queue_size()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the event queue size
					return (vpos->ev_queue->size());
					
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::event_queue_size()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::event_queue_size()"); 
			
	// Should never reach here. To make compiler happy
	
	int ret = -1;
	return ret;    
}

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::is_event_queue_empty()
// 
// description :  Returns true when the event queue is empty
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------	
bool EventConsumer::is_event_queue_empty(int event_id)
{
	cout3 << "EventConsumer::is_event_queue_empty() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// check whether the event queue is empty
				return (epos->second.ev_queue->is_empty());
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::is_event_queue_empty()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// check whether the event queue is empty
					return (vpos->ev_queue->is_empty());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::is_event_queue_empty()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::is_event_queue_empty()");
			
	// Should never reach here. To make compiler happy
	
	bool ret = true;
	return ret; 
}

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_last_event_date()
// 
// description :  Get the time stamp of the last inserted event
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------	
TimeVal EventConsumer::get_last_event_date(int event_id)
{
	cout3 << "EventConsumer::get_last_event_date() : event_id = " << event_id << endl;
	
	// lock the maps
	ReaderLock l(map_modification_lock);

	//
	// First search the event entry in the callback map
	//	
	std::map<std::string,EventCallBackStruct>::iterator epos;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		if(epos->second.id == event_id)
		{
			// check wether an event queue is used!
			if ( epos->second.callback == NULL )
			{
				// get the last insertion date
				return (epos->second.ev_queue->get_last_event_date());
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No event queue specified during subscribe_event()\n";
				o << "Cannot return any event data" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_last_event_date()");
			}
		}
	}
	
	//
	// check also the vector of not yet connected events
	//
		
	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin(); 
			 vpos != event_not_connected.end(); vpos++)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the last insertion date
					return (vpos->ev_queue->get_last_event_date());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventConsumer::get_last_event_date()");
				}			
			}
		}
	}				

	// nothing was found!
		
	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_last_event_date()");
			
	// Should never reach here. To make compiler happy
	
	struct TimeVal tv;
	tv.tv_sec = tv.tv_usec = tv.tv_nsec = 0;
	return tv; 
}


/************************************************************************/
/*		       															*/		
/* 			EventData class 											*/
/*			---------------												*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
// 	EventData constructor
//
//-----------------------------------------------------------------------

EventData::EventData(DeviceProxy *dev,string &nam,string &evt,
          Tango::DeviceAttribute *attr_value_in, DevErrorList &errors_in) :
          device(dev),attr_name(nam),event(evt),attr_value(attr_value_in),
          errors(errors_in)
{
	if (errors.length()==0) 
		err=false;
	else err = true;
	
	set_time();
}

//+----------------------------------------------------------------------
//
// 	EventData copy constructor
//
//-----------------------------------------------------------------------

EventData::EventData(const EventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	if (sou.attr_value)
		attr_value = new DeviceAttribute(*(sou.attr_value));
	else
		attr_value = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	EventData assignement operator
//
//-----------------------------------------------------------------------

EventData & EventData::operator=(const EventData &ri)
{
	if (&ri == this)
		return *this;
		
	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	if (ri.attr_value)
		attr_value = new DeviceAttribute(*(ri.attr_value));
	else
		attr_value = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;
	
	return *this;
}
	
//+----------------------------------------------------------------------
//
// 	EventData destructor
//
//-----------------------------------------------------------------------

EventData::~EventData()
{
	if (attr_value != NULL)
		delete attr_value;
}


//+-------------------------------------------------------------------------
//
// method : 		EventData::set_time
// 
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void EventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);
		
		reception_date.tv_sec  = (CORBA::Long)t.time;
		reception_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else	
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);
		
		reception_date.tv_sec  = (CORBA::Long)tv.tv_sec;
		reception_date.tv_usec = (CORBA::Long)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}


/************************************************************************/
/*		       															*/		
/* 			AttrConfEventData class 									*/
/*			-----------------											*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  AttrConfEventData constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
                  Tango::AttributeInfoEx *attr_conf_in,DevErrorList &errors_in) :
                  device(dev),attr_name(nam),event(evt),attr_conf(attr_conf_in),
                  errors(errors_in)
{
	if (errors.length()==0) 
		err=false;
	else 
		err = true;
		
	set_time();
}

//+----------------------------------------------------------------------
//
//  AttrConfEventData copy constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(const AttrConfEventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	if (sou.attr_conf != NULL)
	{
		attr_conf = new (AttributeInfoEx);
		*attr_conf = *(sou.attr_conf);
	}
	else
		attr_conf = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	AttrConfEventData assignement operator
//
//-----------------------------------------------------------------------

AttrConfEventData & AttrConfEventData::operator=(const AttrConfEventData &ri)
{
	if (&ri == this)
		return *this;
		
	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	if (ri.attr_conf != NULL)
	{
		attr_conf = new (AttributeInfoEx);
		*attr_conf = *(ri.attr_conf);
	}
	else
		attr_conf = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;
	
	return *this;
}
	
//+----------------------------------------------------------------------
//
// 	AttrConfEventData destructor
//
//-----------------------------------------------------------------------

AttrConfEventData::~AttrConfEventData()
{
	if (attr_conf != NULL)
		delete attr_conf;
}

//+-------------------------------------------------------------------------
//
// method : 		AttrConfEventData::set_time
// 
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void AttrConfEventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);
		
		reception_date.tv_sec  = (CORBA::Long)t.time;
		reception_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else	
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);
		
		reception_date.tv_sec  = (CORBA::Long)tv.tv_sec;
		reception_date.tv_usec = (CORBA::Long)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}


/************************************************************************/
/*		       															*/		
/* 			DataReadyEventData class 									*/
/*			-----------------											*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  DataReadyEventData constructor
//
//-----------------------------------------------------------------------

DataReadyEventData::DataReadyEventData(DeviceProxy *dev,AttDataReady *dr,string &evt,DevErrorList &errors_in)
:event(evt),errors(errors_in)
{
	device = dev;
	if (dr != NULL)
	{
		attr_name = dr->name.in();
		attr_data_type = dr->data_type;
		ctr = dr->ctr;
	}
	else
	{
		attr_name = "Unknown";
		attr_data_type = -1;
		ctr = -1;
	}

	if (errors.length()==0) 
		err = false;
	else 
		err = true;
}

//+----------------------------------------------------------------------
//
//  DataReadyEventData copy constructor
//
//-----------------------------------------------------------------------

DataReadyEventData::DataReadyEventData(const DataReadyEventData &sou)
{
	device = sou.device;
	ctr = sou.ctr;
	attr_name = sou.attr_name;
	event = sou.event;
	attr_data_type = sou.attr_data_type;
	reception_date = sou.reception_date;
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	DataReadyEventData assignement operator
//
//-----------------------------------------------------------------------

DataReadyEventData & DataReadyEventData::operator=(const DataReadyEventData &ri)
{
	if (&ri == this)
		return *this;
		
	device = ri.device;
	ctr = ri.ctr;
	attr_data_type = ri.attr_data_type;
	attr_name = ri.attr_name;
	event = ri.event;

	reception_date = ri.reception_date;

	err = ri.err;
	errors = ri.errors;
	
	return *this;
}
	
//+-------------------------------------------------------------------------
//
// method : 		DataReadyEventData::set_time
// 
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void DataReadyEventData::set_time()
{
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);
		
		reception_date.tv_sec  = (CORBA::Long)t.time;
		reception_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else	
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);
		
		reception_date.tv_sec  = (CORBA::Long)tv.tv_sec;
		reception_date.tv_usec = (CORBA::Long)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}
	
} /* End of Tango namespace */


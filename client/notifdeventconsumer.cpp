static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
///  file 	notifdeventconsumer.cpp
///
///		C++ classes for implementing the event consumer
///		singleton class when used with notifd
///
///		author(s) : E.Taurel
///
///		original : 16 August 2011
///
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
///
///		$Revision$
///
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>
#include <eventconsumer.h>

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


NotifdEventConsumer *NotifdEventConsumer::_instance = NULL;
omni_mutex EventConsumer::ev_consumer_inst_mutex;

void NotifdEventConsumer::disconnect_structured_push_consumer()
{
	cout3 << "calling Tango::NotifdEventConsumer::disconnect_structured_push_consumer() \n";
}

void NotifdEventConsumer::offer_change(TANGO_UNUSED(const CosNotification::EventTypeSeq& added),
                                 TANGO_UNUSED(const CosNotification::EventTypeSeq& deled))
{
	cout3 << "calling Tango::NotifdEventConsumer::subscription_change() \n";
}

/************************************************************************/
/*		       															*/
/* 			NotifdEventConsumer class 									*/
/*			-------------------											*/
/*		       															*/
/************************************************************************/

NotifdEventConsumer::NotifdEventConsumer(ApiUtil *ptr) : EventConsumer(ptr),omni_thread((void *)ptr)
{
	cout3 << "calling Tango::NotifdEventConsumer::NotifdEventConsumer() \n";
	orb_ = ptr->get_orb();

	_instance = this;

	start_undetached();
}

NotifdEventConsumer *NotifdEventConsumer::create()
{
	omni_mutex_lock guard(ev_consumer_inst_mutex);

//
// check if the NotifdEventConsumer singleton exists, if so return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// NotifdEventConsumer singleton does not exist, create it
//

	ApiUtil *ptr =  ApiUtil::instance();
	return new NotifdEventConsumer(ptr);
}

//+----------------------------------------------------------------------------
//
// method : 		NotifdEventConsumer::run_undetached()
//
// description : 	Activate the CORBA POA for the server embedded in client
//                  when it receives events.
//                  Run the CORBA orb.
//
//-----------------------------------------------------------------------------

void *NotifdEventConsumer::run_undetached(void *arg)
{

//
// activate POA and go into endless loop waiting for events to be pushed
// this method should run as a separate thread so as not to block the client
//

	ApiUtil *api_util_ptr = (ApiUtil *)arg;
	if (api_util_ptr->in_server() == false)
	{
		CORBA::Object_var obj = orb_->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
       	PortableServer::POAManager_var pman = poa->the_POAManager();
       	pman->activate();

		orb_->run();

		orb_->destroy();

		CORBA::release(orb_);
	}

	return (void *)NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		NotifdEventConsumer::cleanup_EventChannel_map()
//
// description : 	Method to destroy the DeviceProxy objects
//					stored in the EventChannel map
//
//-----------------------------------------------------------------------------

void NotifdEventConsumer::cleanup_EventChannel_map()
{
	std::map<std::string,EventCallBackStruct>::iterator epos;
	EvChanIte evt_it;
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); epos++)
	{
		EventCallBackStruct &evt_cb = epos->second;
	    evt_it = channel_map.find(evt_cb.channel_name);
	    EventChannelStruct &evt_ch = evt_it->second;
	    if (evt_ch.channel_type == NOTIFD)
	    {
            try
            {
                CosNotifyFilter::Filter_var f = evt_ch.structuredProxyPushSupplier->get_filter(evt_cb.filter_id);
                evt_ch.structuredProxyPushSupplier->remove_filter(evt_cb.filter_id);

                f->destroy();
            }
            catch(...)
            {
                cerr << "Could not remove filter from notification daemon for " << evt_cb.channel_name << endl;
            }
	    }
	}

    for (evt_it = channel_map.begin(); evt_it != channel_map.end(); ++evt_it)
    {
        EventChannelStruct &evt_ch = evt_it->second;
        if (evt_ch.adm_device_proxy != NULL)
        {
            AutoTangoMonitor _mon(evt_ch.channel_monitor);

            if (evt_ch.channel_type == NOTIFD)
            {
                try
                {
                    // Destroy the filter created in the
                    // notification service for the heartbeat event

                    CosNotifyFilter::Filter_var f = evt_ch.structuredProxyPushSupplier->get_filter(evt_ch.heartbeat_filter_id);
                    evt_ch.structuredProxyPushSupplier->remove_filter(evt_ch.heartbeat_filter_id);
                    f->destroy();

                    // disconnect the pushsupplier to stop receiving events

                    //cout << "EventConsumer::cleanup_EventChannel_map(): Disconnect push supplier!" << endl;

                    evt_ch.structuredProxyPushSupplier->disconnect_structured_push_supplier();
                }
                catch(...)
                {
                    cerr << "Could not remove heartbeat filter from notification daemon for " << evt_ch.full_adm_name << endl;
                }

                // Release the connection to the device server administration device

                delete evt_ch.adm_device_proxy;
                evt_ch.adm_device_proxy = NULL;
            }
        }
    }
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::connect_event_system()
//
// description :    Connect to the real event (archive, change, periodic,...)
//
// argument : in :	- device_name : The device fqdn (lower case)
//                  - att_name : The attribute name
//                  - event_name : The event name
//                  - filters : The event filters given by the user
//                  - evt_it : Iterator pointing to the event channel entry
//                             in channel_map map
//                  - new_event_callback : Structure used for the event callback
//                                         entry in the event_callback_map
//                  - dd : The data returned by the DS admin device
//                         xxxSubscriptionChange command
//
//-----------------------------------------------------------------------------

void NotifdEventConsumer::connect_event_system(string &device_name,string &att_name,string &event_name,
                                              const vector<string> &filters,EvChanIte &evt_it,
                                              EventCallBackStruct &new_event_callback,TANGO_UNUSED(DeviceData &dd))
{
//
// Build a filter using the CORBA Notify constraint Language
// (use attribute name in lowercase letters)
//

	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;

	try
	{
		EventChannelStruct &evt_ch = evt_it->second;
		{
			AutoTangoMonitor _mon(evt_ch.channel_monitor);
    		ffp    = evt_ch.eventChannel->default_filter_factory();
    		filter = ffp->create_filter("EXTENDED_TCL");
		}
  	}
	catch (CORBA::COMM_FAILURE &)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"NotifdEventConsumer::connect_event_system()");
  	}
	catch (...)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"NotifdEventConsumer::connect_event_system()");
  	}

//
// Construct a simple constraint expression; add it to fadmin
//
// The device name received by this method is the FQDN, remove the
// protocol prefix
//

    string tmp_dev_name(device_name);
    string::size_type pos;
    pos = tmp_dev_name.find("://");
    pos = pos + 3;
    pos = tmp_dev_name.find('/',pos);
    ++pos;
    string d_name = tmp_dev_name.substr(pos,tmp_dev_name.size() - pos);

	char constraint_expr[512];

	::sprintf(constraint_expr,"$domain_name == \'%s/%s\' and $event_name == \'%s\'",
		d_name.c_str(),att_name.c_str(),event_name.c_str());

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
    	CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);
    	filter_id = evt_it->second.structuredProxyPushSupplier->add_filter(filter);

		new_event_callback.filter_id = filter_id;
		new_event_callback.filter_constraint = constraint_expr;
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
                       	(const char*)"NotifdEventConsumer::connect_event_system()");
  	}
	else
	{
		new_event_callback.filter_ok = true;
	}
}



//+----------------------------------------------------------------------------
//
// method : 		NotifdEventConsumer::connect_event_channel()
//
// description :    Connect to the event channel
//                  This means connect to the heartbeat event
//
// Args in : - channel name : The event channel name (DS admin name)
//           - db : Database object
//           - reconnect: Flag set to true in case this method is called for
//                        event reconnection purpose
//           - dd : The DS admin device command returned data
//                  (ZmqEventSubscriptionChange command)
//
//-----------------------------------------------------------------------------

void NotifdEventConsumer::connect_event_channel(string &channel_name,Database *db,bool reconnect,TANGO_UNUSED(DeviceData &dd))
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
			      			(const char *)"NotifdEventConsumer::connect_event_channel");
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
			      			(const char *)"NotifdEventConsumer::connect_event_channel");
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
                        	(const char*)"NotifdEventConsumer::connect_event_channel()");
		}
	}
	else
	{
    	EventSystemExcept::throw_exception((const char*)"API_EventChannelNotExported",
                        	(const char*)"Failed to narrow EventChannel (hint: make sure a notifd process is running on the server host)",
                        	(const char*)"NotifdEventConsumer::connect_event_channel()");
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
                       		(const char*)"NotifdEventConsumer::connect_event_channel()");
        	exit((void*)1);
    	}
	}
	catch (...)
	{
     	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       	(const char*)"NotifdEventConsumer::connect_event_channel()");
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
                       		(const char*)"NotifdEventConsumer::connect_event_channel()");
    	}

        structuredProxyPushSupplier =
		CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow(
		    		proxySupplier);
		if (CORBA::is_nil(structuredProxyPushSupplier))
		{
       			//cerr << "Tango::NotifdEventConsumer::NotifdEventConsumer() could not get CosNotifyChannelAdmin::StructuredProxyPushSupplier" << endl;
       		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to narrow the push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"NotifdEventConsumer::connect_event_channel()");
    	}

//
// Set a large timeout on this CORBA object
// This is necessary in case of maany threads doing subscribe/unsubscribe as fast as they can
//

		omniORB::setClientCallTimeout(structuredProxyPushSupplier,20000);
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get PushSupplier from notification daemon due to AdminLimitExceeded (hint: make sure the notifd process is running on this host)",
                       	(const char*)"NotifdEventConsumer::connect_event_channel()");
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
       	cerr << "Tango::NotifdEventConsumer::NotifdEventConsumer() caught AlreadyConnected exception" << endl;
	}

	EvChanIte evt_it = channel_map.end();
	if (reconnect == true)
	{
		evt_it = channel_map.find(channel_name);
		EventChannelStruct &evt_ch = evt_it->second;
		evt_ch.eventChannel = eventChannel;
		evt_ch.structuredProxyPushSupplier = structuredProxyPushSupplier;
		evt_ch.last_heartbeat = time(NULL);
		evt_ch.heartbeat_skipped = false;
		evt_ch.notifyd_host = hostname;
		evt_ch.event_system_failed = false;
		evt_ch.has_notifd_closed_the_connection = 0;
	}
	else
	{
		EventChannelStruct new_event_channel_struct;
		new_event_channel_struct.eventChannel = eventChannel;
		new_event_channel_struct.structuredProxyPushSupplier = structuredProxyPushSupplier;
		new_event_channel_struct.last_heartbeat = time(NULL);
		new_event_channel_struct.heartbeat_skipped = false;
		new_event_channel_struct.adm_device_proxy = NULL;
		// create a channel monitor
		new_event_channel_struct.channel_monitor = new TangoMonitor();
		// set the timeout for the channel monitor to 500ms not to block the event consumer for to long.
		new_event_channel_struct.channel_monitor->timeout(500);
		set_channel_type(new_event_channel_struct);

		channel_map[channel_name] = new_event_channel_struct;
		evt_it = channel_map.find(channel_name);
		EventChannelStruct &evt_ch = evt_it->second;
		evt_ch.notifyd_host = hostname;
		evt_ch.event_system_failed = false;
		evt_ch.has_notifd_closed_the_connection = 0;
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
		ffp    = evt_it->second.eventChannel->default_filter_factory();
		filter = ffp->create_filter("EXTENDED_TCL");
	}
	catch (...)
	{
                //cerr << "Caught exception obtaining filter object" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating heartbeat filter (check filter)",
                       	(const char*)"NotifdEventConsumer::connect_event_channel()");
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
		evt_it->second.heartbeat_filter_id = evt_it->second.structuredProxyPushSupplier->add_filter(filter);
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
                       	(const char*)"NotifdEventConsumer::connect_event_channel()");
	}
}


//+----------------------------------------------------------------------------
//
// method : 		NotifdEventConsumer::push_structured_event()
//
// description : 	Method run when an event is received
//
// argument : in :	event : The event itself...
//
//-----------------------------------------------------------------------------


void NotifdEventConsumer::push_structured_event(const CosNotification::StructuredEvent& event)
{
	string domain_name(event.header.fixed_header.event_type.domain_name);
	string event_type(event.header.fixed_header.event_type.type_name);
	string event_name(event.header.fixed_header.event_name);

//	cout << "Received event: domain_name = " << domain_name << ", event_type = " << event_type << ", event_name = " << event_name << endl;

	bool svr_send_tg_host = false;

	if (event_name == "heartbeat")
	{
		string fq_dev_name = domain_name;
		if (event_type.find("tango://") != string::npos)
		{
			if (event_type.find("#") == string::npos)
				fq_dev_name.insert(0,event_type);
			else
			{
				fq_dev_name.insert(0,event_type,0,event_type.size() - 1);
				fq_dev_name = fq_dev_name + MODIFIER_DBASE_NO;
			}
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
			EventChannelStruct &evt_ch = ipos->second;
			try
			{
				AutoTangoMonitor _mon(evt_ch.channel_monitor);
				evt_ch.last_heartbeat = time(NULL);
			}
			catch (...)
			{
				cerr << "Tango::NotifdEventConsumer::push_structured_event() timeout on channel monitor of " << ipos->first << endl;
			}
		}

		map_modification_lock.readerOut();
	}
	else
	{
		string fq_dev_name = domain_name;
		if (event_type.find("tango://") != string::npos)
		{
			if (event_type.find("#") == string::npos)
				fq_dev_name.insert(0,event_type);
			else
			{
				fq_dev_name.insert(0,event_type,0,event_type.size() - 1);
				fq_dev_name = fq_dev_name + MODIFIER_DBASE_NO;
			}
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

		if (ipos != event_callback_map.end())
		{
			EventCallBackStruct &evt_cb = ipos->second;
			AutoTangoMonitor _mon(evt_cb.callback_monitor);
			try
			{
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
				const DevErrorList *err_ptr = NULL;

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
						attr_to_device(attr_value_4,dev_attr);
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
						errors[0].origin = CORBA::string_dup("NotifdEventConsumer::push_structured_event()");
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

				vector<EventSubscribeStruct>::iterator esspos;

				unsigned int cb_nb = ipos->second.callback_list.size();
				unsigned int cb_ctr = 0;

				for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
				{
					cb_ctr++;
					if (esspos->id > 0)
					{
						CallBack *callback;
						callback = esspos->callback;
						EventQueue *ev_queue;
						ev_queue = esspos->ev_queue;

						if (cb_ctr == cb_nb)
						{
							map_lock = false;
							map_modification_lock.readerOut();
						}

						if ((ev_attr_conf == false) && (ev_attr_ready == false))
						{
							EventData *event_data;
							if (cb_ctr != cb_nb)
							{
								DeviceAttribute *dev_attr_copy = NULL;
								if (dev_attr != NULL)
								{
									dev_attr_copy = new DeviceAttribute();
									dev_attr_copy->deep_copy(*dev_attr);
								}

								event_data = new EventData(event_callback_map[attr_event_name].device,
												      				fq_dev_name,
												      				event_name,
												      				dev_attr_copy,
												      				errors);
							}
							else
							{
								event_data = new EventData (event_callback_map[attr_event_name].device,
																  fq_dev_name,
																  event_name,
																  dev_attr,
																  errors);
							}

							// if a callback method was specified, call it!
							if (callback != NULL )
							{
								try
								{
									callback->push_event(event_data);
								}
								catch (...)
								{
									cerr << "Tango::NotifdEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
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
						else if (ev_attr_ready == false)
						{
							AttrConfEventData *event_data;

							if (cb_ctr != cb_nb)
							{
								AttributeInfoEx *attr_info_copy = new AttributeInfoEx();
								*attr_info_copy = *attr_info_ex;
								event_data = new AttrConfEventData(event_callback_map[attr_event_name].device,
																  fq_dev_name,
																  event_name,
																  attr_info_copy,
																  errors);
							}
							else
							{
								event_data = new AttrConfEventData(event_callback_map[attr_event_name].device,
																  fq_dev_name,
																  event_name,
																  attr_info_ex,
																  errors);
							}


							// if callback methods were specified, call them!
							if (callback != NULL )
							{
								try
								{
									callback->push_event(event_data);
								}
								catch (...)
								{
									cerr << "Tango::NotifdEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
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
									cerr << "Tango::NotifdEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
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
					else // id < 0
					{
						if (cb_ctr == cb_nb)
						{
							map_lock = false;
							map_modification_lock.readerOut();
						}

						if ((ev_attr_conf == false) && (ev_attr_ready == false))
							delete dev_attr;
						else if (ev_attr_ready == false)
							delete attr_info_ex;
					}
				} // End of for
			}
			catch (...)
			{
				// free the map lock if not already done
				if ( map_lock == true )
				{
					map_modification_lock.readerOut();
				}

				cerr << "Tango::NotifdEventConsumer::push_structured_event() timeout on callback monitor of " << ipos->first << endl;
			}
		}
		else
		{
		// even if nothing was found in the map, free the lock
			map_modification_lock.readerOut();
		}
	}
}

} /* End of Tango namespace */


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
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
///		Revision 1.33  2009/06/17 08:52:08  taurel
///		- Commit after a merge with branch Release_7_0_2-bugfixes
///		Revision 1.32.2.2  2009/06/26 06:46:06  taurel
///		- No change, simply commit to test the "commit" mailing list
///		
///		Revision 1.32.2.1  2009/06/12 08:28:51  taurel
///		- Fix bug when using events in multi Tango host environment.
///		The TANGO_HOST is now transferred within the even tin the fixed
///		header event_type field.
///		The DS admin device EventSubscriptionChange command now returns with which Tango lib it is runnig.
///		This allows the client to know if the tango host info will be transmitted within the event
///		
///		Revision 1.32  2009/03/13 09:33:29  taurel
///		- Small changes to fix Windows VC8 warnings in Warning level 3
///		
///		Revision 1.31  2009/01/29 16:23:49  taurel
///		- Commit after merge with branch Release_6_1_1-bugfixes
///		
///		Revision 1.30  2009/01/29 15:25:41  taurel
///		- First implementation of the Data Ready event
///		
///		Revision 1.29  2009/01/21 12:49:03  taurel
///		- Change CopyRights for 2009
///		
///		Revision 1.28  2009/01/08 09:52:06  taurel
///		- Fix bugs for server implementing only IDL3
///		
///		Revision 1.27  2009/01/06 09:28:50  taurel
///		- Add compatibility between a server IDL V4 and clients IDL V3
///		
///		Revision 1.26  2008/12/17 09:50:59  taurel
///		- First implementation of attributes sent on the wire using IDL Union
///		instead of IDL Any
///		
///		Revision 1.25  2008/10/06 15:01:09  taurel
///		- Changed the licensing info from GPL to LGPL
///		
///		Revision 1.24  2008/10/03 06:52:31  taurel
///		- Add some licensing info in each files
///		
///		Revision 1.23  2008/09/23 14:59:35  taurel
///		- Commit after the end of DevEncoded data type implementation
///		- The new test suite is also now running fine
///		
///		Revision 1.22  2008/07/01 07:38:40  taurel
///		- Some more code for a proper implementation of the DevEncoded data type with the new IDL release 4
///		Revision 1.21.2.2  2009/01/29 15:17:01  taurel
///		- Add some bug fixes for server used with the -f option (database as
///		file)
///		
///		Revision 1.21.2.1  2008/07/17 12:25:50  taurel
///		- Unexport event channel from db only during device server startup sequence
///		- Add re-try in case of pb during event channel export to db
///		- Clean-up old supplier proxy at DS startup (in case of previous core dump)
///		
///		Revision 1.21  2008/03/11 14:38:25  taurel
///		- Apply patches from Frederic Picca about compilation with gcc 4.2
///		
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
		
	_event_supplier->fqdn_prefix = "tango://";
	_event_supplier->fqdn_prefix = _event_supplier->fqdn_prefix + db->get_db_host() + ':' + db->get_db_port() + '/' ;
	
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

			if (tg->is_svr_starting() == true)
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

		if ((Util::_FileDb == false) && (tg->is_svr_starting() == true))
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

				bool retry = true;
				int ctr = 0;
				int db_to = db->get_timeout_millis();
				
				db->set_timeout_millis(db_to * 2);
				
				while ((retry == true) && (ctr < 4))
				{
					try
					{
						db->export_event(eve_export_list);
						retry = false;
					}
					catch (Tango::CommunicationFailed &) {ctr++;}
				}
				
				db->set_timeout_millis(db_to);
				
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
// If necessary, clean up remaining proxies left by previous run of the DS
// which did a core dump (or similar)
//


	if (tg->is_svr_starting() == true)
	{
		CosNotifyChannelAdmin::ProxyIDSeq_var proxies;

		proxies = _supplierAdmin->push_consumers();
		if (proxies->length() >= 1)
		{
			for (unsigned int loop = 0;loop < proxies->length();loop++)
			{
				CosNotifyChannelAdmin::ProxyConsumer_var _tmp_proxyConsumer;
				_tmp_proxyConsumer = _supplierAdmin->get_proxy_consumer(proxies[loop]);

				CosNotifyChannelAdmin::StructuredProxyPushConsumer_var _tmp_structuredProxyPushConsumer;
				_tmp_structuredProxyPushConsumer = CosNotifyChannelAdmin::StructuredProxyPushConsumer::_narrow(_tmp_proxyConsumer);
				if (CORBA::is_nil(_tmp_structuredProxyPushConsumer))
				{
					continue;
    			}

				try
				{
					_tmp_structuredProxyPushConsumer->disconnect_structured_push_consumer();
				}
				catch(CORBA::Exception &) {}
			}
		}
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
 		struct_event.header.fixed_header.event_type.type_name   = CORBA::string_dup(fqdn_prefix.c_str());
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
				    Tango::AttributeValue_3 *attr_value,
				    Tango::AttributeValue_4 *attr_value_4,
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
		detect_and_push_change_event_3(device_impl,attr_value,attr_value_4,attr,attr_name,except);
	}
	else
		attr.ext->event_change_client_3 = false;
		
	if (periodic_subscription < EVENT_RESUBSCRIBE_PERIOD)
	{
		detect_and_push_periodic_event_3(device_impl,attr_value,attr_value_4,attr,attr_name,except);
	}
	else
		attr.ext->event_periodic_client_3 = false;
		
	if (archive_subscription < EVENT_RESUBSCRIBE_PERIOD)
	{
		detect_and_push_archive_event_3(device_impl,attr_value,attr_value_4,attr,attr_name,except);
	}
	else
		attr.ext->event_archive_client_3 = false;
}
		
bool EventSupplier::detect_change_3(Attribute &attr,
				  AttributeValue_3 *curr_attr_value,
				  AttributeValue_4 *curr_attr_value_4,
				  bool archive,
				  double &delta_change_rel,
				  double &delta_change_abs,
				  DevFailed *except,
				  bool &force_change,
				  DeviceImpl *dev)
	{
		bool is_change = false;
		
		cout3 << "EventSupplier::detect_change(): called for attribute " << attr.get_name() << endl;

		Tango::AttrQuality the_new_quality;
		CORBA::Any *the_new_any = NULL;
		
		if (curr_attr_value != NULL)
		{
			the_new_quality = curr_attr_value->quality;
			the_new_any = &(curr_attr_value->value);
		}
		else
		{
			the_new_quality = curr_attr_value_4->quality;
		}
		
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
			
			// force an archive event when the last reading was still returning an exception
			if ((except == NULL) && (attr.ext->prev_archive_event.err == true))
			{
				force_change = true;
				return true;
			}
			
			// check wether the quality is invalid
			// Force an event only if the last reading was valid
			
		   if (the_new_quality == Tango::ATTR_INVALID)
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
			if ((the_new_quality != Tango::ATTR_INVALID) && (attr.ext->prev_archive_event.quality == Tango::ATTR_INVALID))
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
			
		   if (the_new_quality == Tango::ATTR_INVALID)
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
			if ((the_new_quality != Tango::ATTR_INVALID) && (attr.ext->prev_change_event.quality == Tango::ATTR_INVALID))
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
		CORBA::TypeCode_var ty;
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
			if ((attr.ext->prev_change_event.quality != Tango::ATTR_INVALID) && (the_new_quality != Tango::ATTR_INVALID))
				    enable_check = true;
		}
		else
		{
			rel_change[0] = attr.ext->archive_rel_change[0];
			rel_change[1] = attr.ext->archive_rel_change[1];
			abs_change[0] = attr.ext->archive_abs_change[0];
			abs_change[1] = attr.ext->archive_abs_change[1];
			inited = attr.ext->prev_archive_event.inited;
			if ((attr.ext->prev_archive_event.quality != Tango::ATTR_INVALID) && (the_new_quality != Tango::ATTR_INVALID))
				    enable_check = true;		
		}
		mon1.rel_monitor();

		if (inited)
		{	
			if (enable_check == true)
			{

			if (the_new_any != NULL)
				ty = the_new_any->type();
				
//
// First, analyse the DevEncoded data type
//
				
				if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_ENCODED))
				{
					unsigned int curr_seq_str_nb,prev_seq_str_nb;
					const char *curr_encoded_format,*prev_encoded_format;
					Tango::DevVarUCharArray *curr_data_ptr,*prev_data_ptr;
					
					Tango::DevVarEncodedArray &un_seq = curr_attr_value_4->value.encoded_att_value();
					curr_seq_str_nb = strlen(un_seq[0].encoded_format.in());
					curr_seq_nb = un_seq[0].encoded_data.length();
					curr_encoded_format = un_seq[0].encoded_format.in();
					curr_data_ptr = &un_seq[0].encoded_data;
					
					if (archive == true)
					{
						DevVarEncodedArray &union_seq = attr.ext->prev_archive_event.value_4.encoded_att_value();
						prev_seq_nb = union_seq[0].encoded_data.length();
						prev_seq_str_nb = strlen(union_seq[0].encoded_format.in());
						prev_encoded_format = union_seq[0].encoded_format.in();
						prev_data_ptr = &union_seq[0].encoded_data;
					}
					else
					{
						DevVarEncodedArray &union_seq = attr.ext->prev_change_event.value_4.encoded_att_value();
						prev_seq_nb = union_seq[0].encoded_data.length();
						prev_seq_str_nb = strlen(union_seq[0].encoded_format.in());
						prev_encoded_format = union_seq[0].encoded_format.in();
						prev_data_ptr = &union_seq[0].encoded_data;
					}
					if ((curr_seq_nb != prev_seq_nb) || (curr_seq_str_nb != prev_seq_str_nb))
					{
						force_change = true;
						return true;
					}
					
					if (strcmp(curr_encoded_format,prev_encoded_format) != 0)
					{
						delta_change_rel = delta_change_abs = 100.;
						is_change = true;
						return(is_change);
					}

					if ((rel_change[0] != INT_MAX) || (rel_change[1] != INT_MAX) || (abs_change[0] != INT_MAX) || (abs_change[1] != INT_MAX))
					{
						for (i=0; i< curr_seq_nb; i++)
						{
							if (rel_change[0] != INT_MAX)
							{
								if ((*prev_data_ptr)[i] != 0)
								{
									delta_change_rel = ((*curr_data_ptr)[i] - (*prev_data_ptr)[i])*100/(*prev_data_ptr)[i];
								}
								else
								{
									delta_change_rel = 100;
									if ((*curr_data_ptr)[i] == (*prev_data_ptr)[i])
										delta_change_rel = 0;
								}
								if (delta_change_rel <= rel_change[0] || delta_change_rel >= rel_change[1])
								{
									is_change = true;
									return(is_change);
								}
							}
							if (abs_change[0] != INT_MAX)
							{
								delta_change_abs = (*curr_data_ptr)[i] - (*prev_data_ptr)[i];
								if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
								{
									is_change = true;
									return(is_change);
								}
							}
						}
					}
				}

//
// Now, the DevState data type
//

				else
				{
					bool dev_state_type = false;
					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == DEVICE_STATE))
					{
						dev_state_type = true;
						curr_sta = curr_attr_value_4->value.dev_state_att();
						if (archive == true)
							prev_sta = attr.ext->prev_archive_event.value_4.dev_state_att();
						else
							prev_sta = attr.ext->prev_change_event.value_4.dev_state_att();
					}
					else if ((the_new_any != NULL) && (ty->kind() == CORBA::tk_enum))
					{
						dev_state_type = true;
						*the_new_any >>= curr_sta;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_sta;
						else
							attr.ext->prev_change_event.value >>= prev_sta;

					}

					if (dev_state_type == true)
					{		
						if (curr_sta != prev_sta)
						{
							delta_change_rel = delta_change_abs = 100.;
							is_change = true;
						}
						return is_change;
					}
				
					CORBA::TypeCode_var ty_alias;
					CORBA::TypeCode_var ty_seq;

					if (the_new_any != NULL)
					{
						ty_alias = ty->content_type();
						ty_seq = ty_alias->content_type();
					}

//
// Now, the long data type
//

					bool long_type = false;
					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_LONG))
					{
						long_type = true;
						curr_seq_lo = &curr_attr_value_4->value.long_att_value();
						if (archive == true)
							prev_seq_lo = &(attr.ext->prev_archive_event.value_4.long_att_value());
						else
							prev_seq_lo = &(attr.ext->prev_change_event.value_4.long_att_value());
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_long))
					{
						long_type = true;
						*the_new_any >>= curr_seq_lo;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_lo;
						else
							attr.ext->prev_change_event.value >>= prev_seq_lo;
					}	

					if (long_type == true)
					{
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
						return false;
					}

//
// Now, the long 64 bits data type
//

					bool long_long_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_LONG64))
					{
						long_long_type = true;
						curr_seq_64 = &curr_attr_value_4->value.long64_att_value();
						if (archive == true)
							prev_seq_64 = &attr.ext->prev_archive_event.value_4.long64_att_value();
						else
							prev_seq_64 = &attr.ext->prev_change_event.value_4.long64_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_longlong))
					{
						long_long_type = true;
						*the_new_any >>= curr_seq_64;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_64;
						else
							attr.ext->prev_change_event.value >>= prev_seq_64;
					}

					if (long_long_type == true)
					{
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
						return false;
					}

//
// Now, the short data type
//

					bool short_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_SHORT))
					{
						short_type = true;
						curr_seq_sh = &curr_attr_value_4->value.short_att_value();
						if (archive == true)
							prev_seq_sh = &attr.ext->prev_archive_event.value_4.short_att_value();
						else
							prev_seq_sh = &attr.ext->prev_change_event.value_4.short_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_short))
					{
						short_type = true;
						*the_new_any >>= curr_seq_sh;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_sh;
						else
							attr.ext->prev_change_event.value >>= prev_seq_sh;
					}

					if (short_type == true)
					{
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
						return false;
					}

//
// Now, the double data type
//
	
					bool double_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_DOUBLE))
					{
						double_type = true;
						curr_seq_db = &curr_attr_value_4->value.double_att_value();
						if (archive == true)
							prev_seq_db = &attr.ext->prev_archive_event.value_4.double_att_value();
						else
							prev_seq_db = &attr.ext->prev_change_event.value_4.double_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_double))
					{
						double_type = true;
						*the_new_any >>= curr_seq_db;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_db;
						else
							attr.ext->prev_change_event.value >>= prev_seq_db;
					}

					if (double_type == true)
					{
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
						return false;
					}

//
// Now, the string data type
//
	
					bool string_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_STRING))
					{
						string_type = true;
						curr_seq_str = &curr_attr_value_4->value.string_att_value();
						if (archive == true)
							prev_seq_str = &attr.ext->prev_archive_event.value_4.string_att_value();
						else
							prev_seq_str = &attr.ext->prev_change_event.value_4.string_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_string))
					{					
						string_type = true;
						*the_new_any >>= curr_seq_str;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_str;
						else
							attr.ext->prev_change_event.value >>= prev_seq_str;
					}

					if (string_type == true)
					{
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
						return false;
					}

//
// Now, the float data type
//
	
					bool float_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_FLOAT))
					{
						float_type = true;
						curr_seq_fl = &curr_attr_value_4->value.float_att_value();
						if (archive == true)
							prev_seq_fl = &attr.ext->prev_archive_event.value_4.float_att_value();
						else
							prev_seq_fl = &attr.ext->prev_change_event.value_4.float_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_float))
					{
						float_type = true;
						*the_new_any >>= curr_seq_fl;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_fl;
						else
							attr.ext->prev_change_event.value >>= prev_seq_fl;
					}

					if (float_type == true)
					{
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
						return false;
					}

//
// Now, the unsigned short data type
//
	
					bool unsigned_short_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_USHORT))
					{
						unsigned_short_type = true;
						curr_seq_ush = &curr_attr_value_4->value.ushort_att_value();
						if (archive == true)
							prev_seq_ush = &attr.ext->prev_archive_event.value_4.ushort_att_value();
						else
							prev_seq_ush = &attr.ext->prev_change_event.value_4.ushort_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_ushort))
					{
						unsigned_short_type = true;
						*the_new_any >>= curr_seq_ush;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_ush;
						else
							attr.ext->prev_change_event.value >>= prev_seq_ush;
					}

					if (unsigned_short_type == true)
					{
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
						return false;
					}

//
// Now, the boolean data type
//
	
					bool boolean_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_BOOL))
					{
						boolean_type = true;
						curr_seq_bo = &curr_attr_value_4->value.bool_att_value();
						if (archive == true)
							prev_seq_bo = &attr.ext->prev_archive_event.value_4.bool_att_value();
						else
							prev_seq_bo = &attr.ext->prev_change_event.value_4.bool_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_boolean))
					{	
						boolean_type = true;
						*the_new_any >>= curr_seq_bo;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_bo;
						else
							attr.ext->prev_change_event.value >>= prev_seq_bo;
					}

					if (boolean_type == true)
					{
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
						return false;
					}

//
// Now, the char data type
//
	
					bool char_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_UCHAR))
					{
						char_type = true;
						curr_seq_uch = &curr_attr_value_4->value.uchar_att_value();
						if (archive == true)
							prev_seq_uch = &attr.ext->prev_archive_event.value_4.uchar_att_value();
						else
							prev_seq_uch = &attr.ext->prev_change_event.value_4.uchar_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_octet))
					{
						char_type = true;
						*the_new_any >>= curr_seq_uch;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_uch;
						else
							attr.ext->prev_change_event.value >>= prev_seq_uch;
					}

					if (char_type == true)
					{
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
						return false;
					}

//
// Now, the unsigned long data type
//
	
					bool unsigned_long_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_ULONG))
					{
						unsigned_long_type = true;
						curr_seq_ulo = &curr_attr_value_4->value.ulong_att_value();
						if (archive == true)
							prev_seq_ulo = &attr.ext->prev_archive_event.value_4.ulong_att_value();
						else
							prev_seq_ulo = &attr.ext->prev_change_event.value_4.ulong_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_ulong))
					{
						unsigned_long_type = true;		
						*the_new_any >>= curr_seq_ulo;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_ulo;
						else
							attr.ext->prev_change_event.value >>= prev_seq_ulo;
					}

					if (unsigned_long_type == true)
					{
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
						return false;
					}

//
// Now, the unsigned 64 bits data type
//
	
					bool unsigned_64_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_ULONG64))
					{
						unsigned_64_type = true;
						curr_seq_u64 = &curr_attr_value_4->value.ulong64_att_value();
						if (archive == true)
							prev_seq_u64 = &attr.ext->prev_archive_event.value_4.ulong64_att_value();
						else
							prev_seq_u64 = &attr.ext->prev_change_event.value_4.ulong64_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_ulonglong))
					{
						unsigned_64_type = true;
						*the_new_any >>= curr_seq_u64;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_u64;
						else
							attr.ext->prev_change_event.value >>= prev_seq_u64;

					}
		
					if (unsigned_64_type == true)
					{
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
						return false;
					}

//
// Now, the state data type
//
	
					bool state_type = false;

					if ((curr_attr_value_4 != NULL) && (curr_attr_value_4->value._d() == ATT_STATE))
					{
						state_type = true;
						curr_seq_state = &curr_attr_value_4->value.state_att_value();
						if (archive == true)
							prev_seq_state = &attr.ext->prev_archive_event.value_4.state_att_value();
						else
							prev_seq_state = &attr.ext->prev_change_event.value_4.state_att_value();
					}
					else if ((the_new_any != NULL) && (ty_seq->kind() == CORBA::tk_enum))
					{
						state_type = true;
						*the_new_any >>= curr_seq_state;
						if (archive == true)
							attr.ext->prev_archive_event.value >>= prev_seq_state;
						else
							attr.ext->prev_change_event.value >>= prev_seq_state;
					}

					if (state_type == true)
					{
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
					}

				}
			}
		}

		cout3 << "EventSupplier::detect_change(): leaving for attribute " << attr.get_name() << endl;
		return(is_change);
	}


void EventSupplier::detect_and_push_change_event_3(DeviceImpl *device_impl,
						 AttributeValue_3 *attr_value,
						 AttributeValue_4 *attr_value_4,
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

		Tango::AttrQuality the_quality;

		if (attr_value_4 != NULL)
			the_quality = attr_value_4->quality;
		else
			the_quality = attr_value->quality;
				
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
				if (attr_value_4 != NULL)
					attr.ext->prev_change_event.value_4 = attr_value_4->value;
				else
					attr.ext->prev_change_event.value = attr_value->value;
				attr.ext->prev_change_event.quality = the_quality;
				attr.ext->prev_change_event.err = false;
			}
			attr.ext->prev_change_event.inited = true;
		}
		else
		{
	
//
// determine delta_change in percent compared with previous event sent
// 
			is_change = detect_change_3(attr,attr_value,attr_value_4,false,delta_change_rel,delta_change_abs,except,force_change,device_impl);
			cout3 << "EventSupplier::detect_and_push_change_event(): rel_change " << delta_change_rel << " abs_change " << delta_change_abs << " is change = " << is_change << endl;
		}
		
		// check whether the data quality has changed.
		// Fire event on a quality change.
		
		if ( except == NULL &&
		    attr.ext->prev_change_event.quality != the_quality )
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
				if (attr_value_4 != NULL)
					attr.ext->prev_change_event.value_4 = attr_value_4->value;
				else
					attr.ext->prev_change_event.value   = attr_value->value;
				attr.ext->prev_change_event.quality = the_quality;
				attr.ext->prev_change_event.err     = false;
			}

//
// If one of the subscribed client is still using IDL 3, the attribute value has to be sent
// using an AttributeValue_3 data type
//

			bool need_free = false;
			if ((attr.ext->event_change_client_3 == true) && (attr_value == NULL))
			{
				attr_value = new AttributeValue_3();
				attr.AttributeValue_4_2_AttributeValue_3(attr_value_4,attr_value);
				attr_value_4 = NULL;
				need_free = true;
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
				   attr_value_4,
				   attr_name,
				   except);
				   
			if (need_free == true)
				delete attr_value;
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
//			attr_value_4 : The attribute value for a IDL 4 device
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void EventSupplier::detect_and_push_archive_event_3(DeviceImpl *device_impl,
						  AttributeValue_3 *attr_value, 
						  AttributeValue_4 *attr_value_4,
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

		Tango::AttrQuality the_quality;

		if (attr_value_4 != NULL)
			the_quality = attr_value_4->quality;
		else
			the_quality = attr_value->quality;

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
				if (attr_value_4 != NULL)
					attr.ext->prev_archive_event.value_4 = attr_value_4->value;
				else
					attr.ext->prev_archive_event.value = attr_value->value;
				attr.ext->prev_archive_event.quality = the_quality;
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
				is_change = detect_change_3(attr,attr_value,attr_value_4,true,
							  delta_change_rel,
							  delta_change_abs,except,
							  force_change,device_impl);
			}
		}

		// check whether the data quality has changed.
		// Fire event on a quality change.
		
		if ( except == NULL &&
		     attr.ext->prev_archive_event.quality != the_quality )
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
				if (attr_value_4 != NULL)
					attr.ext->prev_archive_event.value_4 = attr_value_4->value;
				else
					attr.ext->prev_archive_event.value   = attr_value->value;
				attr.ext->prev_archive_event.quality = the_quality;
				attr.ext->prev_archive_event.err     = false;
			}

//
// If one of the subscribed client is still using IDL 3, the attribute value has to be sent
// using an AttributeValue_3 data type
//

			bool need_free = false;
			if ((attr.ext->event_archive_client_3 == true) && (attr_value == NULL))
			{
				attr_value = new AttributeValue_3();
				attr.AttributeValue_4_2_AttributeValue_3(attr_value_4,attr_value);
				attr_value_4 = NULL;
				need_free = true;
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
				   attr_value_4,
				   attr_name,
				   except);
				   
			if (need_free == true)
				delete attr_value;
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
//			attr_value_4 : The attribute value for a IDL 4 device
//			attr : The attribute object
//			attr_name : The attribute name
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void EventSupplier::detect_and_push_periodic_event_3(DeviceImpl *device_impl,
					    AttributeValue_3 *attr_value,
						AttributeValue_4 *attr_value_4,
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
		bool need_free = false;

//
// If one of the subscribed client is still using IDL 3, the attribute value has to be sent
// using an AttributeValue_3 data type
//

		if ((attr.ext->event_periodic_client_3 == true) && (attr_value == NULL))
		{
			attr_value = new AttributeValue_3();
			attr.AttributeValue_4_2_AttributeValue_3(attr_value_4,attr_value);
			attr_value_4 = NULL;
			need_free = true;
		}
				
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
			   attr_value_4,
			   attr_name,
			   except);

		if (need_free == true)
			delete attr_value;
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
//			attr_value_4 : The attribute value for a IDL 4 device
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
		       	AttributeValue_3 *attr_value,
				AttributeValue_4 *attr_value_4,
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
	struct_event.header.fixed_header.event_type.type_name = CORBA::string_dup(fqdn_prefix.c_str());

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
	{
		if (attr_value != NULL)
			struct_event.remainder_of_body <<= (*attr_value);
		else
			struct_event.remainder_of_body <<= (*attr_value_4);
	}
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
// method : 		EventSupplier::push_att_data_ready_event()
// 
// description : 	
//
//-----------------------------------------------------------------------------

void EventSupplier::push_att_data_ready_event(DeviceImpl *device_impl,const string &attr_name,long data_type,DevLong ctr)
{
	cout3 << "EventSupplier::push_att_data_ready_event(): called for attribute " << attr_name << endl;
	
	vector<string> filterable_names;
	vector<double> filterable_data;
	vector<string> filterable_names_lg;
	vector<long> filterable_data_lg;
	
	string ev_type(DATA_READY_TYPE_EVENT);

	AttDataReady dat_ready;
	dat_ready.name = attr_name.c_str();
	dat_ready.data_type = (int)data_type;
	dat_ready.ctr = ctr;

	push_event(device_impl,
		   ev_type,
		   filterable_names,
		   filterable_data,
		   filterable_names_lg,
		   filterable_data_lg,
	       dat_ready,
		   const_cast<string &>(attr_name),
		   NULL);
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
	catch(CORBA::Exception &) {}
}


} /* End of Tango namespace */

static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
//
//  file 	notifdeventsupplier.cpp
//
//		C++ class for implementing the event server
//		singleton class - NotifdEventSupplier.
//		This class is used to send events from the server
//		to the notification service.
//
//  	author(s) : E.Taurel (taurel@esrf.fr)
//
//		original : August 2011
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
//		$Revision: 17240 $
//
//
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

NotifdEventSupplier *NotifdEventSupplier::_instance = NULL;


/************************************************************************/
/*		       															*/
/* 			NotifdEventSupplier class 									*/
/*			-------------------											*/
/*		       															*/
/************************************************************************/


NotifdEventSupplier::NotifdEventSupplier(CORBA::ORB_var _orb,
	CosNotifyChannelAdmin::SupplierAdmin_var _supplierAdmin,
	CosNotifyChannelAdmin::ProxyID _proxId,
	CosNotifyChannelAdmin::ProxyConsumer_var _proxyConsumer,
	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var _structuredProxyPushConsumer,
	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory,
	CosNotifyChannelAdmin::EventChannel_var _eventChannel,
	string &event_ior,
	Util *tg): EventSupplier(tg)
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

NotifdEventSupplier *NotifdEventSupplier::create(CORBA::ORB_var _orb,
				     string server_name,
				     Util *tg)
{
	cout4 << "calling Tango::NotifdEventSupplier::create() \n";

//
// does the NotifdEventSupplier singleton exist already ? if so simply return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// Connect process to the notifd service
//

	NotifService ns;
	connect_to_notifd(ns,_orb,server_name,tg);

//
// NotifdEventSupplier singleton does not exist, create it
//

	NotifdEventSupplier *_event_supplier =
		new NotifdEventSupplier(_orb,ns.SupAdm,ns.pID,ns.ProCon,ns.StrProPush,ns.EveChaFac,ns.EveCha,ns.ec_ior,tg);

	return _event_supplier;
}

void NotifdEventSupplier::connect()
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
		cerr << "Tango::NotifdEventSupplier::connect() caught AlreadyConnected exception" << endl;
	}

}

void NotifdEventSupplier::disconnect_structured_push_supplier()
{
	cout4 << "calling Tango::NotifdEventSupplier::disconnect_structured_push_supplier() \n";
}

void NotifdEventSupplier::subscription_change(TANGO_UNUSED(const CosNotification::EventTypeSeq& added),
                                 TANGO_UNUSED(const CosNotification::EventTypeSeq& deled))
{
	cout4 << "calling Tango::NotifdEventSupplier::subscription_change() \n";
}

//+----------------------------------------------------------------------------
//
// method : 		NotifdEventSupplier::connect_to_notifd()
//
// description : 	Method to connect the process to the notifd
//
// argument : in :	ns : Ref. to a struct with notifd connection parameters
//
//-----------------------------------------------------------------------------

void NotifdEventSupplier::connect_to_notifd(NotifService &ns,CORBA::ORB_var &_orb,
				      string &server_name,
				      Util *tg)
{
	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory;
	CosNotifyChannelAdmin::EventChannel_var _eventChannel;

//
// Get a reference to the Notification Service EventChannelFactory from
// the TANGO database or from the server itself in case of server
// started with the -file option
//

    string &host_name = tg->get_host_name();

	string factory_ior;
	string factory_name;
	factory_name = "notifd/factory/" + host_name;
	CORBA::Any_var received;
	string d_name = "DServer/";
	d_name = d_name + server_name;
	const DevVarLongStringArray *dev_import_list;

	Database *db = tg->get_database();

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
				(const char*)"NotifdEventSupplier::create()");
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
				(const char*)"NotifdEventSupplier::create()");

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

		omniORB::setClientConnectTimeout(NARROW_CLNT_TIMEOUT);
		_eventChannelFactory = CosNotifyChannelAdmin::EventChannelFactory::_narrow(event_factory_obj);
		omniORB::setClientConnectTimeout(0);

//
// Make sure the CORBA object was really an EventChannelFactory
//

		if(CORBA::is_nil(_eventChannelFactory))
		{
			cerr << factory_name << " is not an EventChannelFactory " << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to import the EventChannelFactory from the Tango database",
				(const char*)"NotifdEventSupplier::create()");
		}
	}
	catch (...)
	{
		omniORB::setClientConnectTimeout(0);

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
			(const char*)"NotifdEventSupplier::create()");
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
	{
		try
		{
			Tango::DbDatum na;

			string cl_name(NOTIFD_CHANNEL);
			na = db->get_device_name(server_name,cl_name);
			channel_ior = na.value_string[0];
			channel_exported = 1;
		}
		catch (Tango::DevFailed &)
		{
			channel_exported = 0;
		}
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
			cout4 << "Tango::NotifdEventSupplier::create() channel for server " << d_name << " created\n";
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
// event channel within the event supplier object and in the file
//

				ns.ec_ior = ior_string;

				try
				{
					db->write_event_channel_ior_filedatabase(ior_string);
				}
				catch (Tango::DevFailed &e) {}
			}
			CORBA::string_free(_ior);
		}
		catch(const CosNotification::UnsupportedQoS&)
		{
			cerr << "Failed to create event channel - events will not be generated (hint: start the notifd daemon on this host)" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to create a new EventChannel, make sure the notifd process is running on this host",
				(const char*)"NotifdEventSupplier::create()");
		}
		catch(const CosNotification::UnsupportedAdmin&)
		{
			cerr << "Failed to create event channel - events will not be generated (hint: start the notifd daemon on this host)" << endl;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
				(const char*)"Failed to create a new EventChannel, make sure the notifd process is running on this host",
				(const char*)"NotifdEventSupplier::create()");
		}
	}
	else
	{
		cout4 << "Tango::NotifdEventSupplier::create(): _narrow worked, use this event channel\n";
		if (Util::_FileDb == true)
			ns.ec_ior = channel_ior;
	}

//
// Obtain a Supplier Admin
//

//
// We'll use the channel's default Supplier admin
//

	CosNotifyChannelAdmin::SupplierAdmin_var _supplierAdmin = _eventChannel -> default_supplier_admin();
	if (CORBA::is_nil(_supplierAdmin))
	{
        cerr << "Could not get CosNotifyChannelAdmin::SupplierAdmin" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to get the default supplier admin from the notification daemon (hint: make sure the notifd process is running on this host)",
			(const char*)"NotifdEventSupplier::create()");
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
				(const char*)"NotifdEventSupplier::create()");
    	}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		cerr << "Failed to get push consumer from notification daemon - events will not be generated (hint: start the notifd daemon on this host)" << endl;
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
			(const char*)"Failed to get push consumer from notification daemon (hint: make sure the notifd process is running on this host)",
			(const char*)"NotifdEventSupplier::create()");
	}

	CosNotifyChannelAdmin::StructuredProxyPushConsumer_var
            _structuredProxyPushConsumer =
		CosNotifyChannelAdmin::StructuredProxyPushConsumer::_narrow(_proxyConsumer);
	if (CORBA::is_nil(_structuredProxyPushConsumer))
	{
       	cerr << "Tango::NotifdEventSupplier::create() could not get CosNotifyChannelAdmin::StructuredProxyPushConsumer" << endl;
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
// method : 		NotifdEventSupplier::push_heartbeat_event()
//
// description : 	Method to send the hearbeat event
//
// argument : in :
//
//-----------------------------------------------------------------------------

void NotifdEventSupplier::push_heartbeat_event()
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
	cout3 << "NotifdEventSupplier::push_heartbeat_event(): delta time since last heartbeat " << delta_time << endl;

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

		cout3 << "NotifdEventSupplier::push_heartbeat_event(): detected heartbeat event for " << domain_name << endl;
		cout3 << "NotifdEventSupplier::push_heartbeat_event(): delta _time " << delta_time << endl;
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
			cout3 << "NotifdEventSupplier::push_heartbeat_event() event channel disconnected !\n";
			fail = true;
		}
       	catch(const CORBA::TRANSIENT &)
       	{
			cout3 << "NotifdEventSupplier::push_heartbeat_event() caught a CORBA::TRANSIENT ! " << endl;
			fail = true;
		}
		catch(const CORBA::COMM_FAILURE &)
		{
			cout3 << "NotifdEventSupplier::push_heartbeat_event() caught a CORBA::COMM_FAILURE ! " << endl;
			fail = true;
		}
		catch(const CORBA::SystemException &)
		{
			cout3 << "NotifdEventSupplier::push_heartbeat_event() caught a CORBA::SystemException ! " << endl;
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
// method : 		NotifdEventSupplier::reconnect_notifd()
//
// description : 	Method to reconnect to the notifd
//
// argument : in :
//
//-----------------------------------------------------------------------------

void NotifdEventSupplier::reconnect_notifd()
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

		connect_to_notifd(ns,orb_,tg->get_ds_name(),tg);

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

//+----------------------------------------------------------------------------
//
// method : 		NotifdEventSupplier::disconnect_from_notifd()
//
// description : 	Method to disconnect the DS from the notifd event channel
//
//-----------------------------------------------------------------------------

void NotifdEventSupplier::disconnect_from_notifd()
{
	try
	{
		structuredProxyPushConsumer->disconnect_structured_push_consumer();
	}
	catch(CORBA::Exception &) {}
}

//+----------------------------------------------------------------------------
//
// method : 		NotifdEventSupplier::push_event()
//
// description : 	Method to send the event to the event channel
//
// argument : in :	device_impl : The device
//			        event_type : The event type (change, periodic....)
//			        filterable_names :
//			        filterable_data :
//			        attr_value : The attribute value
//			        except : The exception thrown during the last
//				        attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void NotifdEventSupplier::push_event(DeviceImpl *device_impl,string event_type,
            vector<string> &filterable_names,vector<double> &filterable_data,vector<string> &filterable_names_lg,vector<long> &filterable_data_lg,
            struct AttributeData &attr_value,string &attr_name,DevFailed *except)
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
		if (attr_value.attr_val != NULL)
			struct_event.remainder_of_body <<= (*attr_value.attr_val);
        else if (attr_value.attr_val_3 != NULL)
			struct_event.remainder_of_body <<= (*attr_value.attr_val_3);
		else if (attr_value.attr_val_4 != NULL)
		{
			struct_event.remainder_of_body <<= (*attr_value.attr_val_4);

//
// Insertion in the event structure Any also copy the mutex ptr.
// When this event structure will be deleted (at the end of this method),
// the struct inserted into the Any will also be deleted.
// This will unlock the mutex....
// Clean the mutex ptr in the structure inserted in the Any to prevent this
// bad behavior
//

			const Tango::AttributeValue_4 *tmp_ptr;
			struct_event.remainder_of_body >>= tmp_ptr;
			const_cast<Tango::AttributeValue_4 *>(tmp_ptr)->mut_ptr = NULL;
		}
	    else if (attr_value.attr_conf_2 != NULL)
            struct_event.remainder_of_body <<= (*attr_value.attr_conf_2);
        else if (attr_value.attr_conf_3 != NULL)
            struct_event.remainder_of_body <<= (*attr_value.attr_conf_3);
        else
           struct_event.remainder_of_body <<= (*attr_value.attr_dat_ready);
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
// method : 		NotifdEventSupplier::file_db_svr()
//
// description : 	In case of DS using file as database, set a marker in the
//                  fqdn_prefix
//
//-----------------------------------------------------------------------------

void NotifdEventSupplier::file_db_svr()
{
    fqdn_prefix = fqdn_prefix + '#';
}


} /* End of Tango namespace */

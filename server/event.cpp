 const char *RcsId = "$Id$";

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
///		$Revision$
///
///		$Log$
///		Revision 1.1.4.6  2004/03/02 12:43:08  taurel
///		- Add a cleanup_heartbeat_filters() method to destroy heartbeat filters
///		
///		Revision 1.1.4.5  2004/03/02 07:41:56  taurel
///		- Fix compiler warnings (gcc used with -Wall)
///		- Fix bug in DbDatum insertion operator fro vectors
///		- Now support "modulo" as periodic filter
///		
///		Revision 1.1.4.4  2004/02/25 16:27:44  taurel
///		Minor changes to compile library using Solaris CC compiler
///		
///		Revision 1.1.4.3  2004/02/18 15:06:17  taurel
///		Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
///		restart
///		
///		Revision 1.1.4.2  2004/02/06 11:58:51  taurel
///		- Many changes in the event system
///		
///
///		copyright : European Synchrotron Radiation Facility
///                         BP 220, Grenoble 38043
///                         FRANCE
///
////////////////////////////////////////////////////////////////////////////////

#include <tango.h>
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
// EventSupplier singleton does not exist, create it
//

	CosNotifyChannelAdmin::EventChannelFactory_var _eventChannelFactory;
	CosNotifyChannelAdmin::EventChannel_var _eventChannel;

//
// Get a reference to the Notification Service EventChannelFactory from
// the TANGO database
//

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
	server_name = "DServer/" + server_name;

        send <<= server_name.c_str();
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

		cerr << server_name << " has no event channel defined in the database - creating it " << endl;
		cout << server_name << " has no event channel defined in the database - creating it " << endl;
		
		channel_exported = 0;
	}
	if (channel_exported != 0)
	{
        	received >>= dev_import_list;
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
			cout4 << "Tango::EventSupplier::create() channel for server " << server_name << " created\n";
			char *_ior = _orb->object_to_string(_eventChannel);
			string ior_string(_ior);

        		Tango::DevVarStringArray *dev_export_list = new Tango::DevVarStringArray;
        		dev_export_list->length(5);
        		(*dev_export_list)[0] = CORBA::string_dup(server_name.c_str());
        		(*dev_export_list)[1] = CORBA::string_dup(ior_string.c_str());
        		(*dev_export_list)[2] = CORBA::string_dup(host_name.c_str());
        		ostringstream ostream;
        		ostream << getpid() << ends;
        		(*dev_export_list)[3] = CORBA::string_dup(ostream.str().c_str());
        		(*dev_export_list)[4] = CORBA::string_dup("1");
        		send <<= dev_export_list;
                	received = db->command_inout("DbExportEvent",send);
                	cout4 << "successfully  exported event channel to Tango database !\n";
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
		CosNotifyChannelAdmin::StructuredProxyPushConsumer::_narrow(
		    _proxyConsumer);
	if (CORBA::is_nil(_structuredProxyPushConsumer))
	{
       		cerr << "Tango::EventSupplier::create() could not get CosNotifyChannelAdmin::StructuredProxyPushConsumer" << endl;
    	}

	EventSupplier *_event_supplier =
		new EventSupplier(_orb, _supplierAdmin, _proxyId, _proxyConsumer, _structuredProxyPushConsumer, _eventChannelFactory, _eventChannel);
	
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
// method : 		EventSupplier::detect_and_push__events()
// 
// description : 	Method to detect and eventually push events...
//
// argument : in :	device_impl : The device
//			attr_value : The attribute value
//			except : The exception received when the attribute was read
//				 NULL if no exception was received
//			attr_name : The attribute name
//
//-----------------------------------------------------------------------------


void EventSupplier::detect_and_push_events(DeviceImpl *device_impl,
					   AttributeValue &attr_value,
					   DevFailed *except,
					   string &attr_name)
{
	string event, domain_name;
	int now, change_subscription, quality_subscription, periodic_subscription, archive_subscription;

	cout3 << "EventSupplier::detect_and_push_events(): called for attribute " << attr_name << endl;

	Attribute &attr = device_impl->dev_attr->get_attr_by_name(attr_name.c_str());

	now = time(NULL);
	change_subscription = now - attr.ext->event_change_subscription;
	quality_subscription = now - attr.ext->event_quality_subscription;
	periodic_subscription = now - attr.ext->event_periodic_subscription;
	archive_subscription = now - attr.ext->event_archive_subscription;

	cout3 << "EventSupplier::detect_and_push_events(): last subscription for change " << change_subscription << " quality " << quality_subscription << " periodic " << periodic_subscription << " archive " << archive_subscription << endl;

	if (change_subscription < EVENT_RESUBSCRIBE_PERIOD)
	{
		detect_and_push_change_event(device_impl,attr_value,attr,attr_name,except);
	}
	if (quality_subscription < EVENT_RESUBSCRIBE_PERIOD)
	{
		detect_and_push_quality_change_event(device_impl,attr_value,attr,attr_name,except);
	}
	if (periodic_subscription < EVENT_RESUBSCRIBE_PERIOD)
	{
		detect_and_push_periodic_event(device_impl,attr_value,attr,attr_name,except);
	}
	if (archive_subscription < EVENT_RESUBSCRIBE_PERIOD)
	{
		detect_and_push_archive_event(device_impl,attr_value,attr,attr_name,except);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		EventSupplier::detect_change()
// 
// description : 	Method to detect if there is a change according to the
//			criterions and return a boolean set to true if a change
//			is detected
//
// argument : in :	attr : The attribute object
//			curr_attr_value : The current attribute value
//			archive : 
//			delta_change_rel :
//			delta_change_abs :
//			except : The exception thrown during the last
//				 attribute reading. NULL if no exception
//			force_change : A flag set to true if the change
//				       is due to a non mathematical reason
//				       (array size change, from exception to
//					classic...)
//
//-----------------------------------------------------------------------------

bool EventSupplier::detect_change(Attribute &attr,
				  AttributeValue &curr_attr_value,
				  bool archive,
				  double &delta_change_rel,
				  double &delta_change_abs,
				  DevFailed *except,
				  bool &force_change)
{
	bool is_change = false;

//
// Send event, if the read_attribute failed or if it is the first time
// that the read_attribute succeed after a failure.
// Same thing if the attribute quality factor changes to INVALID
//
	
	if (except != NULL)
	{
		force_change = true;
		return true;
	}
	
	if (curr_attr_value.quality == Tango::ATTR_INVALID)
	{
		force_change = true;
		return true;
	}
	
	if (archive == true)
	{
		if ((except == NULL) && (attr.ext->prev_archive_event.err == true))
		{
			force_change = true;
			return true;
		}

		if ((curr_attr_value.quality != Tango::ATTR_INVALID) && (attr.ext->prev_archive_event.attr.quality == Tango::ATTR_INVALID))
		{
			force_change = true;
			return true;
		}
	}
	else
	{		
		if ((except == NULL) && (attr.ext->prev_change_event.err == true))
		{
			force_change = true;
			return true;
		}

		if ((curr_attr_value.quality != Tango::ATTR_INVALID) && (attr.ext->prev_change_event.attr.quality == Tango::ATTR_INVALID))
		{
			force_change = true;
			return true;
		}
	}
		
	const DevVarLongArray *curr_seq_lo, *prev_seq_lo;
	const DevVarShortArray *curr_seq_sh, *prev_seq_sh;
	const DevVarDoubleArray *curr_seq_db, *prev_seq_db;
	const DevVarStringArray *curr_seq_str, *prev_seq_str;
	CORBA::TypeCode_var ty = curr_attr_value.value.type();
	CORBA::TypeCode_var ty_alias = ty->content_type();
	CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
	double rel_change[2], abs_change[2];
	unsigned int i;
	unsigned int curr_seq_nb,prev_seq_nb;
	bool inited;

	delta_change_rel = delta_change_abs = 0;

	bool enable_check = false;
	if (!archive)
	{
		rel_change[0] = attr.ext->rel_change[0];
		rel_change[1] = attr.ext->rel_change[1];
		abs_change[0] = attr.ext->abs_change[0];
		abs_change[1] = attr.ext->abs_change[1];
		inited = attr.ext->prev_change_event.inited;
		if ((attr.ext->prev_change_event.attr.quality != Tango::ATTR_INVALID) && (curr_attr_value.quality != Tango::ATTR_INVALID))
			    enable_check = true;
	}
	else
	{
		rel_change[0] = attr.ext->archive_rel_change[0];
		rel_change[1] = attr.ext->archive_rel_change[1];
		abs_change[0] = attr.ext->archive_abs_change[0];
		abs_change[1] = attr.ext->archive_abs_change[1];
		inited = attr.ext->prev_archive_event.inited;
		if ((attr.ext->prev_archive_event.attr.quality != Tango::ATTR_INVALID) && (curr_attr_value.quality != Tango::ATTR_INVALID))
			    enable_check = true;		
	}

	if (inited)
	{		
		if (enable_check == true)
		{
			switch (ty_seq->kind())
			{
			case CORBA::tk_long:		
				curr_attr_value.value >>= curr_seq_lo;
				if (archive == true)
					attr.ext->prev_archive_event.attr.value >>= prev_seq_lo;
				else
					attr.ext->prev_change_event.attr.value >>= prev_seq_lo;
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
		
			case CORBA::tk_short:
				curr_attr_value.value >>= curr_seq_sh;
				if (archive == true)
					attr.ext->prev_archive_event.attr.value >>= prev_seq_sh;
				else
					attr.ext->prev_change_event.attr.value >>= prev_seq_sh;
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
					attr.ext->prev_archive_event.attr.value >>= prev_seq_db;
				else
					attr.ext->prev_change_event.attr.value >>= prev_seq_db;
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
						if (delta_change_abs <= abs_change[0] || delta_change_abs >= abs_change[1])
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
					attr.ext->prev_archive_event.attr.value >>= prev_seq_str;
				else
					attr.ext->prev_change_event.attr.value >>= prev_seq_str;
				curr_seq_nb = curr_seq_str->length();
				prev_seq_nb = prev_seq_str->length();
				if (curr_seq_nb != prev_seq_nb)
				{
					force_change = true;
					return true;
				}
				for (i=0; i<curr_seq_str->length(); i++)
				{
					if ((*curr_seq_str)[i] != (*prev_seq_str)[i])
					{
						delta_change_rel = delta_change_abs = 100.;
						is_change = true;
						return(is_change);
					}
				}
				break;
			}
		}
	}	

	return(is_change);
}

void EventSupplier::detect_and_push_change_event(DeviceImpl *device_impl,
						 AttributeValue &attr_value,
						 Attribute &attr,
						 string &attr_name,
						 DevFailed *except)
{
	string event, domain_name;
	double delta_change_rel = 0.0;
	double delta_change_abs = 0.0;
	bool is_change = false;
	bool force_change = false;

	cout3 << "EventSupplier::detect_and_push_change_event(): called for attribute " << attr_name << endl;

//
// if no attribute of this name is registered with change then
// insert the current value
//

	if (!attr.ext->prev_change_event.inited)
	{
		if (except != NULL)
			attr.ext->prev_change_event.err = true;
		else
		{
			attr.ext->prev_change_event.attr = attr_value;
			attr.ext->prev_change_event.err = false;
		}
		attr.ext->prev_change_event.inited = true;
	}
	else
	{
	
//
// determine delta_change in percent compared with previous event sent
// 

		is_change = detect_change(attr,attr_value,false,delta_change_rel,delta_change_abs,except,force_change);
		cout3 << "EventSupplier::detect_and_push_change_event(): rel_change " << delta_change_rel << " abs_change " << delta_change_abs << " is change = " << is_change << endl;
	}
	
	if (is_change)
	{
		vector<string> filterable_names;
		vector<double> filterable_data;
		vector<string> filterable_names_lg;
		vector<long> filterable_data_lg;
		
		if (except != NULL)
			attr.ext->prev_change_event.err = true;
		else
		{
			attr.ext->prev_change_event.attr = attr_value;
			attr.ext->prev_change_event.err = false;
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
		push_event(device_impl,
			   "change",
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

void EventSupplier::detect_and_push_archive_event(DeviceImpl *device_impl,
						  AttributeValue &attr_value, 
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
	bool value_change = false;

	cout3 << "EventSupplier::detect_and_push_archive_event(): called for attribute " << attr_name << endl;

	double now_ms, ms_since_last_periodic;
#ifdef WIN32
        struct _timeb           now_win;
#endif
        struct timeval          now_timeval;

#ifdef WIN32
	_ftime(&now_win);
	now_timeval.tv_sec = (unsigned long)now_win.time;
	now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
	gettimeofday(&now_timeval,NULL);
#endif
	now_ms = (double)now_timeval.tv_sec * 1000. + (double)now_timeval.tv_usec / 1000.;
	ms_since_last_periodic = now_ms - attr.ext->archive_last_periodic;

	if ((ms_since_last_periodic > attr.ext->archive_period) && (attr.ext->prev_archive_event.inited == true))
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
			attr.ext->prev_archive_event.err = true;
		else
		{
			attr.ext->prev_archive_event.attr = attr_value;
			attr.ext->prev_archive_event.err = false;
		}
		attr.ext->archive_last_periodic = now_ms;
		attr.ext->prev_archive_event.inited = true;
	}
	else
	{
	
//
// determine delta_change in percent compared with previous event sent
//

		if (is_change == false)
		{
			is_change = detect_change(attr,attr_value,true,delta_change_rel,delta_change_abs,except,force_change);
			if (is_change == true)
				value_change = true;
		}
	}
	
	if (is_change)
	{
		vector<string> filterable_names;
		vector<double> filterable_data;
		vector<string> filterable_names_lg;
		vector<long> filterable_data_lg;

		domain_name = device_impl->get_name() + "/" + attr_name;
		if (value_change == true)
		{
			if (except != NULL)
				attr.ext->prev_archive_event.err = true;
			else
			{
				attr.ext->prev_archive_event.attr = attr_value;
				attr.ext->prev_archive_event.err = false;
			}
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

		push_event(device_impl,
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

void EventSupplier::detect_and_push_quality_change_event(DeviceImpl *device_impl,
							 AttributeValue &attr_value,
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
			attr.ext->prev_quality_event.attr = attr_value;
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
		else if (attr_value.quality != attr.ext->prev_quality_event.attr.quality)
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
			attr.ext->prev_quality_event.attr = attr_value;
			attr.ext->prev_quality_event.err = false;
		}
		
		push_event(device_impl,
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

void EventSupplier::detect_and_push_periodic_event(DeviceImpl *device_impl,
						   AttributeValue &attr_value,
						   Attribute &attr,
						   string &attr_name,
						   DevFailed *except)
{
	string event, domain_name;
	double now_ms, ms_since_last_periodic;
#ifdef WIN32
        struct _timeb           now_win;
#endif
        struct timeval          now_timeval;

#ifdef WIN32
	_ftime(&now_win);
	now_timeval.tv_sec = (unsigned long)now_win.time;
	now_timeval.tv_usec = (long)now_win.millitm * 1000;
#else
	gettimeofday(&now_timeval,NULL);
#endif
	now_ms = (double)now_timeval.tv_sec * 1000. + (double)now_timeval.tv_usec / 1000.;
	ms_since_last_periodic = now_ms - attr.ext->last_periodic;
	cout3 << "EventSupplier::detect_and_push_is_periodic_event(): delta since last periodic " << ms_since_last_periodic << " event_period " << attr.ext->event_period << " for " << device_impl->get_name()+"/"+attr_name << endl;
	
	if (ms_since_last_periodic > attr.ext->event_period)
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
		push_event(device_impl,
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

void EventSupplier::push_event(DeviceImpl *device_impl,
			       string event_type,
			       vector<string> &filterable_names,
			       vector<double> &filterable_data,
			       vector<string> &filterable_names_lg,
			       vector<long> &filterable_data_lg,
			       AttributeValue &attr_value,
			       string &attr_name,
			       DevFailed *except)
{
	CosNotification::StructuredEvent struct_event;
	string domain_name;

	cout3 << "EventSupplier::push_event(): called for attribute " << attr_name << endl;

	string loc_attr_name = attr_name;	
	transform(loc_attr_name.begin(),loc_attr_name.end(),loc_attr_name.begin(),::tolower);
	domain_name = device_impl->get_name() + "/" + loc_attr_name;
	
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

	try
	{
		structuredProxyPushConsumer -> push_structured_event(struct_event);
	}
	catch(const CosEventComm::Disconnected&)
	{
		cout3 << "EventSupplier::push_event() event channel disconnected !\n";
	}
       	catch(const CORBA::TRANSIENT &)
       	{
       		cout3 << "EventSupplier::push_event() caught a CORBA::TRANSIENT ! " << endl;
       	}
       	catch(const CORBA::COMM_FAILURE &)
       	{
       		cout3 << "EventSupplier::push_event() caught a CORBA::COMM_FAILURE ! " << endl;
	}
    	catch(const CORBA::SystemException &)
    	{
       		cout3 << "EventSupplier::push_event() caught a CORBA::SystemException ! " << endl;
    	}
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
	static int heartbeat_counter=0;
	
//
// Heartbeat - check wether a heartbeat event has been sent recently
// if not then send it. A heartbeat contains no data, it is used by the
// consumer to know that the supplier is still alive. 
//

	Tango::Util *tg = Tango::Util::instance();
	DServer *adm_dev = tg->get_dserver_device();
	delta_time = time(NULL);
	delta_time = delta_time - adm_dev->last_heartbeat;
	cout3 << "EventSupplier::push_heartbeat_event(): delta time since last heartbeat " << delta_time << endl;
	
	if (delta_time >= 10)
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
		adm_dev->last_heartbeat = time(NULL);
		
		struct_event.remainder_of_body <<= (CORBA::Long)adm_dev->last_heartbeat;

//
// Push the event
//

		try
		{
			structuredProxyPushConsumer -> push_structured_event(struct_event);
		}
		catch(const CosEventComm::Disconnected&)
		{
			cout3 << "EventSupplier::push_heartbeat_event() event channel disconnected !\n";
		}
       		catch(const CORBA::TRANSIENT &)
       		{
       			cout3 << "EventSupplier::push_heartbeat_event() caught a CORBA::TRANSIENT ! " << endl;
       		}
       		catch(const CORBA::COMM_FAILURE &)
       		{
       			cout3 << "EventSupplier::push_heartbeat_event() caught a CORBA::COMM_FAILURE ! " << endl;
		}
    		catch(const CORBA::SystemException &)
    		{
       			cout3 << "EventSupplier::push_heartbeat_event() caught a CORBA::SystemException ! " << endl;
    		}
	}
}

/************************************************************************/
/*		       							*/		
/* 			leavfunc function 				*/
/*			-----------------				*/
/*		       							*/
/* This function will be executed at process exit or when the main      */
/* returned.  It has to be executed to properly shutdown and destroy    */
/* the ORB used by as a server by the event system. The ORB loop is in  */
/* EventConsumer thread. Therefore, get a reference to it, shutdown the */
/* ORB and wait until the thread exit.				        */
/* It also destroys the heartbeat filters				*/
/*									*/
/************************************************************************/

extern "C"
{
static void leavefunc()
{
//	cout << "Leaving............" << endl;
	EventConsumer *ev = ApiUtil::instance()->get_event_consumer();
	
	ev->cleanup_heartbeat_filters();

	ev->orb_->shutdown(true);

	int *rv;	
	ev->join((void **)&rv);
	
}
}

/************************************************************************/
/*		       							*/		
/* 			EventConsumer class 				*/
/*			-------------------				*/
/*		       							*/
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
//
// Is this necessary when events are used within a server ?
//

	if (ptr->in_server() == false)	
		atexit(leavefunc);

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

	EventConsumerKeepAliveThread *keep_alive_thread = new EventConsumerKeepAliveThread();
	keep_alive_thread->start();

	ApiUtil *api_util_ptr = (ApiUtil *)arg;
	if (api_util_ptr->in_server() == false)
	{
		CORBA::Object_var obj = orb_->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
       	 	PortableServer::POAManager_var pman = poa->the_POAManager();
        	pman->activate();

		orb_->run();

		orb_->destroy();
	}
	
	return (void *)NULL;
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

	string device_name = device_proxy->dev_name();

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

//
// if no connection exists to this channel then connect to it
//

	std::map<std::string,EventChannelStruct>::iterator ipos;

	{
		omni_mutex_lock lo(channel_map_mut);
		ipos = channel_map.find(channel_name);

		if (ipos == channel_map.end())
		{
			connect_event_channel(channel_name,device_proxy->get_device_db());
		}
		if (channel_map[channel_name].adm_device_proxy != NULL)
			delete channel_map[channel_name].adm_device_proxy;
		channel_map[channel_name].adm_device_proxy = new DeviceProxy(adm_name);
	}
	
	device_channel_map[device_name] = channel_name;
}

void EventConsumer::connect_event_channel(string channel_name,Database *db)
{
        CORBA::Any send;
	CORBA::Any_var received;
        const DevVarLongStringArray *dev_import_list;
	
//
// Get a reference to an EventChannel for this device server from the 
// TANGO database
//

	int channel_exported=-1;
	string channel_ior;

        send <<= channel_name.c_str();
	try 
	{
        	received = db->command_inout("DbImportEvent",send);
	}
	catch (...) 
	{
//		cerr << channel_name << " has no event channel defined in the database - maybe the server is not running ... " << endl;
//		channel_exported = 0;
		TangoSys_OMemStream o;
			
		o << channel_name;
		o << " has no event channel defined in the database\n";
		o << "Maybe the server is not running or is not linked with Tango release 4.x (or above)... " << ends;
		Except::throw_exception((const char *)"API_NotificationServiceFailed",
			      		o.str(),
			      		(const char *)"EventConsumer::connect_event_channel");
	}
	if (channel_exported != 0)
	{
        	received.inout() >>= dev_import_list;
        	channel_ior = string((dev_import_list->svalue)[1]);
        	channel_exported = dev_import_list->lvalue[0];
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
                        	(const char*)"EventConsumer::create()");        
		}
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
                       		(const char*)"EventConsumer::create()");        
        		exit((void*)1);
    		}
	}
	catch (...)
	{
                EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get default Consumer admin from notification daemon (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::create()");        
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
		consumerAdmin -> obtain_notification_push_supplier(
		    CosNotifyChannelAdmin::STRUCTURED_EVENT, proxyId);
		if (CORBA::is_nil(proxySupplier))
		{
        		//cerr << "Could not get CosNotifyChannelAdmin::ProxySupplier" << endl;
                	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to obtain a push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::create()");        
    		}

        	structuredProxyPushSupplier =
			CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow(
		    		proxySupplier);
		if (CORBA::is_nil(structuredProxyPushSupplier))
		{
       			//cerr << "Tango::EventConsumer::EventConsumer() could not get CosNotifyChannelAdmin::StructuredProxyPushSupplier" << endl;
                	EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       		(const char*)"Failed to narrow the push supplier from notification daemon (hint: make sure the notifd process is running on this host)",
                       		(const char*)"EventConsumer::create()");        
    		}
	}
	catch(const CosNotifyChannelAdmin::AdminLimitExceeded&)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Failed to get PushSupplier from notification daemon due to AdminLimitExceeded (hint: make sure the notifd process is running on this host)",
                       	(const char*)"EventConsumer::create()");        
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

	new_event_channel_struct.eventChannel = eventChannel;
	new_event_channel_struct.structuredProxyPushSupplier = structuredProxyPushSupplier;
	new_event_channel_struct.last_heartbeat = time(NULL);
	new_event_channel_struct.heartbeat_skipped = false;
	new_event_channel_struct.adm_device_proxy = NULL;
	channel_map[channel_name] = new_event_channel_struct;

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
                       	(const char*)"EventConsumer::create()");        
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
                       	(const char*)"EventConsumer::create()");
	}
}

void EventConsumer::attr_to_device(const AttributeValue *attr_value, DeviceAttribute *dev_attr)
{

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	CORBA::ULong max,len;
	
	dev_attr->name = attr_value->name;
	dev_attr->quality = attr_value->quality;
	dev_attr->time = attr_value->time;
	dev_attr->dim_x = attr_value->dim_x;
	dev_attr->dim_y = attr_value->dim_y;

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		CORBA::TypeCode_var ty = attr_value->value.type();
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
		switch (ty_seq->kind())
		{
			case tk_long:		
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
		
			case tk_short:
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
		
	if (event_name == "heartbeat")
	{
        	std::map<std::string,EventChannelStruct>::iterator ipos;

		omni_mutex_lock lo(channel_map_mut); 
        	ipos = channel_map.find(domain_name);
 
        	if (ipos != channel_map.end())
        	{
                	channel_map[domain_name].last_heartbeat = time(NULL);
		}
        }
	else
	{
		/*
		 * new interface to Callback does not pass the filterable data
		 * therefore do not extract it
		 */
/*		vector<double> filterable_data;
		for (int i=0; i < event.filterable_data.length(); i++)
		{
			double filterable_value;
			event.filterable_data[i].value >>= filterable_value;
			filterable_data.push_back(filterable_value);
			cout << "Filter = " << event.filterable_data[i].name.in() << ", value = " << filterable_data[i] << endl;
		}*/

		string attr_event_name = domain_name + "." + event_name;
	
		map<std::string,EventCallBackStruct>::iterator ipos;

		cb_map_mut.lock();	
		ipos = event_callback_map.find(attr_event_name);
	
		if (ipos != event_callback_map.end())
		{
			AttributeValue *attr_value;
			CallBack *callback;
			DevErrorList errors;
			const DevErrorList *err_ptr;
			callback = event_callback_map[attr_event_name].callback;
			if (callback != NULL)
			{
			
//
// Check if the event transmit error
//

				DeviceAttribute *dev_attr = new(DeviceAttribute);
				CORBA::TypeCode_var ty = event.remainder_of_body.type();
				if (ty->kind() == tk_struct)
				{
					event.remainder_of_body >>= attr_value;
					attr_to_device(attr_value, dev_attr);
				}
				else
				{
					event.remainder_of_body >>= err_ptr;
					errors = *err_ptr;
				}			

				EventData *event_data = new EventData(event_callback_map[attr_event_name].device,
							domain_name,
							event_name,
							dev_attr,
							errors);
				cb_map_mut.unlock();
				callback->push_event(event_data);
				delete event_data;
				delete dev_attr;
			}
		}
		else
			cb_map_mut.unlock();
	}

}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
// 
// description : 	Method to subscribe to an event
//
// argument : in :	device :
//			attribute :
//			event :
//			callback :
//			filters :
//
//-----------------------------------------------------------------------------

int EventConsumer::subscribe_event(DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   const vector<string> &filters)
{
	if ((device == NULL) || (callback == NULL))
	{		
		Except::throw_exception((const char *)"API_InvalidArgs",
				        (const char *)"Device or callback pointer NULL !!",
				        (const char *)"EventConsumer::subscribe_event()");
	}
	
	static int subscribe_event_id = 0; // unique event id

	string device_name = device->dev_name();
	cout3 << "Tango::EventConsumer::subscribe_event(" << device_name << "," << attribute <<"," << event << ")\n";

	map<std::string,std::string>::iterator ipos;

	ipos = device_channel_map.find(device_name);
	if (ipos == device_channel_map.end())
	{
		cout3 << "device " << device_name << " is not connected, going to connect to the event channel !\n";
		connect(device);
		ipos = device_channel_map.find(device_name);
		if (ipos == device_channel_map.end())
		{
			TangoSys_OMemStream o;
			
			o << "Failed to connect to event channel for device " << device_name << ends;
			Except::throw_exception((const char *)"API_NotificationServiceFailed",
			      			o.str(),
			      			(const char *)"EventConsumer::subscribe_event");

		}
	}

	string channel_name = device_channel_map[device_name];

	string event_name;

	switch (event) 
	{
		case CHANGE_EVENT : event_name = "change";
				    break;

		case QUALITY_EVENT : event_name = "quality";
				     break;

		case PERIODIC_EVENT : event_name = "periodic";
				      break;

		case ARCHIVE_EVENT : event_name = "archive";
				     break;
				     
		case USER_EVENT : event_name = "user_event";
				  break;
	}
	
//
// inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is
// down !)
//

	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(device_name);
	subscriber_info.push_back(attribute);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(event_name);
	subscriber_in << subscriber_info;
	
	{
		omni_mutex_lock lock_cha(channel_map_mut);
    		channel_map[channel_name].adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
		channel_map[channel_name].last_subscribed = time(NULL);
	}

//
// Build a filter using the CORBA Notify constraint Language
// (use attribute name in lowercase letters)
//

	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;
	
	try
	{
		omni_mutex_lock lock_channel(channel_map_mut);
    		ffp    = channel_map[channel_name].eventChannel->default_filter_factory();  
    		filter = ffp->create_filter("EXTENDED_TCL");
  	}
	catch (CORBA::COMM_FAILURE &)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	catch (...)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
  	}
	
//  	
// Construct a simple constraint expression; add it to fadmin
//

	string att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	
	char constraint_expr[512];
	::sprintf(constraint_expr,"$domain_name == \'%s/%s\' and $event_name == \'%s\'",
		device_name.c_str(),att_name_lower.c_str(),event_name.c_str());
		
	if (filters.size() != 0)
	{
		::strcat(&(constraint_expr[strlen(constraint_expr)])," and ((");
		for (unsigned int i = 0;i < filters.size();i++)
		{
			::strcat(&(constraint_expr[strlen(constraint_expr)]),filters[0].c_str());

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

		{		
			omni_mutex_lock lo(channel_map_mut);
    			filter_id = channel_map[channel_name].structuredProxyPushSupplier->add_filter(filter);
		}

		EventCallBackStruct new_event_callback;
		new_event_callback.device = device;
		new_event_callback.attr_name = attribute;
		new_event_callback.event_name = event_name;
		new_event_callback.channel_name = channel_name;
		new_event_callback.callback = callback;
		new_event_callback.filter_id = filter_id;
		subscribe_event_id++;
		new_event_callback.id = subscribe_event_id;
		
		string callback_key = device_name + "/" + att_name_lower + "." + event_name;
		
		omni_mutex_lock lock_cb(cb_map_mut);
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
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumer::subscribe_event()");        
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
		DeviceAttribute da;
		DevErrorList err;
		err.length(0);
		string domain_name = device_name + "/" + att_name_lower;

			
		try
		{
			da = device->read_attribute(attribute.c_str());
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}
				

		EventData *event_data = new EventData(device,
						      domain_name,
						      event_name,
						      &da,
						      err);
						      
		{
			omni_mutex_lock lock_cb(cb_map_mut);
			callback->push_event(event_data);
		}
		delete event_data;
				
	}
	
	
	return subscribe_event_id;
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
	std::map<std::string,EventConsumer::EventCallBackStruct>::iterator epos;
	bool found=false;

	omni_mutex_lock lock_cb(cb_map_mut);
	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); epos++)
	{
		if(epos->second.id == event_id)
		{
			//cout << "Tango::EventConsumer::unsubscribe_event() - found event id " << event_id << " going to remove_filter()\n";
			try
			{
				omni_mutex_lock lock_cha(channel_map_mut); 
				CosNotifyFilter::Filter_var f = channel_map[epos->second.channel_name].structuredProxyPushSupplier->get_filter(epos->second.filter_id);
				channel_map[epos->second.channel_name].structuredProxyPushSupplier->remove_filter(epos->second.filter_id);
				f->destroy();
			}
			catch (...)
			{
				EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
					(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() (hint: check the Notification daemon is running ",
					(const char*)"EventConsumer::unsubscribe_event()");    
			}
			event_callback_map.erase(epos);
			found=true;
			break;
		}
	}
	if (!found)
	{
		EventIdExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");    
	}
	
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::cleanup_heartbeat_filters()
// 
// description : 	Method to destroy the filter created in the
//			notification service for the heartbeat event.
//
//-----------------------------------------------------------------------------

void EventConsumer::cleanup_heartbeat_filters()
{
	std::map<std::string,EventConsumer::EventChannelStruct>::iterator ipos;

	channel_map_mut.lock();
	for (ipos = channel_map.begin(); ipos != channel_map.end(); ipos++)
	{
		CosNotifyFilter::Filter_var f = ipos->second.structuredProxyPushSupplier->get_filter(ipos->second.heartbeat_filter_id);
		ipos->second.structuredProxyPushSupplier->remove_filter(ipos->second.heartbeat_filter_id);
		f->destroy();
	}
	channel_map_mut.unlock();
}



/************************************************************************/
/*		       							*/		
/* 			EventConsumerKeepAlive class 			*/
/*			----------------------------			*/
/*		       							*/
/************************************************************************/


EventConsumerKeepAliveThread::EventConsumerKeepAliveThread() : omni_thread()
{

//	cout << "Tango::EventConsumer::EventConsumer(): start thread which will keep event subscriptions alive and check for server crashes\n";	
//	start();
}

void EventConsumerKeepAliveThread::run(void *arg) 
{	
	int time_to_sleep, now;
	EventConsumer *event_consumer;

//
// first sleep 2 seconds to give the event system time to startup
//

#ifndef WIN32
	unsigned int time_left;
	time_left = ::sleep(2);
	if (time_left != 0)
		::sleep(time_left);
#else
	Sleep(2000L);
#endif /* WIN32 */

	event_consumer = ApiUtil::instance()->get_event_consumer();
	for (;;)
	{
		time_to_sleep = EVENT_HEARTBEAT_PERIOD;
		
//
// goto sleep until next heartbeat 
//

#ifndef WIN32
		time_left = ::sleep(time_to_sleep);
		if (time_left != 0)
			::sleep(time_left);
#else
		Sleep(time_to_sleep*1000);
#endif /* WIN32 */

		std::map<std::string,EventConsumer::EventChannelStruct>::iterator ipos;
		std::map<std::string,EventConsumer::EventCallBackStruct>::iterator epos;
		now = time(NULL);
		
		event_consumer->channel_map_mut.lock();
		for (ipos = event_consumer->channel_map.begin(); ipos != event_consumer->channel_map.end(); ipos++)
		{
			if ((now - ipos->second.last_subscribed) > EVENT_RESUBSCRIBE_PERIOD/3)
			{
				event_consumer->cb_map_mut.lock();
				for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
				{
					if (epos->second.channel_name == ipos->first )
					{
       						DeviceData subscriber_in;
       						vector<string> subscriber_info;
       						subscriber_info.push_back(epos->second.device->dev_name());
       						subscriber_info.push_back(epos->second.attr_name);
       						subscriber_info.push_back("subscribe");
       						subscriber_info.push_back(epos->second.event_name);
       						subscriber_in << subscriber_info;
						try 
						{
       							ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
						}
						catch (...) {};
						
        					ipos->second.last_subscribed = time(NULL);
        					epos->second.last_subscribed = time(NULL);
					}
				}
				event_consumer->cb_map_mut.unlock();
			}
			
//
//  check if a heartbeat have been skipped
//

 			bool heartbeat_skipped;

			heartbeat_skipped = (now - ipos->second.last_heartbeat) > EVENT_HEARTBEAT_PERIOD;
			if (heartbeat_skipped || ipos->second.heartbeat_skipped)
			{
				ipos->second.heartbeat_skipped = true;
				string domain_name("domain_name"), event_name("event_name");
				Tango::DevErrorList errors(1);

				errors.length(1);
				errors[0].severity = Tango::ERR;
				errors[0].origin = CORBA::string_dup("EventConsumer::KeepAliveThread()");
				errors[0].reason = CORBA::string_dup("API_EventTimeout");
				errors[0].desc = CORBA::string_dup("Event channel is not responding anymore, maybe the server or event system is down");
				DeviceAttribute *dev_attr = NULL;

				event_consumer->cb_map_mut.lock();
				for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
				{
					if (epos->second.channel_name == ipos->first && epos->second.callback != NULL)
					{
						EventData *event_data = new EventData(epos->second.device,
										      domain_name,
										      event_name,
										      dev_attr,
										      errors);						CallBack *callback;
						callback = epos->second.callback;
						callback->push_event(event_data);
						delete event_data;
						if (ipos->second.heartbeat_skipped)
						{
       							DeviceData subscriber_in;
       							vector<string> subscriber_info;
       							subscriber_info.push_back(epos->second.device->dev_name());
       							subscriber_info.push_back(epos->second.attr_name);
       							subscriber_info.push_back("subscribe");
       							subscriber_info.push_back(epos->second.event_name);
       							subscriber_in << subscriber_info;
							try 
							{
       								ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
							}
							catch (...) {};
        						ipos->second.last_subscribed = time(NULL);
						}
					}
				}
				event_consumer->cb_map_mut.unlock();
			}
			
//
// only reset the heartbeat_skipped flag if at least one good heartbeat received
//

			if (!heartbeat_skipped && ipos->second.heartbeat_skipped)
			{
				ipos->second.heartbeat_skipped = false;
			}
		}
		event_consumer->channel_map_mut.unlock();
	}
}


	
}

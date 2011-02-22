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
///		$Revision$
///
///		$Log$
///		Revision 1.2.2.8  2004/11/26 13:53:20  taurel
///		- Fix bug if exception thrown during Util class construction
///		- Change attribute label and format default values
///		
///		Revision 1.2.2.7  2004/11/15 12:32:43  taurel
///		- Some changes for omniORB 4.0.5 (Don't use USE_stub_in_nt_dll...)
///		
///		Revision 1.2.2.6  2004/11/04 09:46:39  taurel
///		- Add a tuning method in the polling thread
///		- Some minor fixes to pass test suite
///		
///		Revision 1.2.2.5  2004/10/27 05:59:46  taurel
///		- Some minor changes to compile on all our supported platforms
///		
///		Revision 1.2.2.4  2004/10/22 11:25:50  taurel
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
///		Revision 1.2.2.3  2004/09/15 06:46:28  taurel
///		- Added four new types for attributes (boolean, float, unsigned short and unsigned char)
///		- It is also possible to read state and status as attributes
///		- Fix bug in Database::get_class_property() method (missing ends insertion)
///		- Fix bug in admin device DevRestart command (device name case problem)
///		
///		Revision 1.2.2.2  2004/08/26 07:34:45  taurel
///		- Implement a way to directly fills command or attribute polling buffer
///		
///		Revision 1.2.2.1  2004/07/15 15:04:06  taurel
///		- Added the way to externally filled the polling buffer for attribute
///		  (Command will come soon)
///		
///		Revision 1.2  2004/07/07 07:59:20  taurel
///		Added some files
///		
///		Revision 1.1.4.13  2004/05/24 08:44:05  taurel
///		- Fix bug if device name stored in db used upper case. Add a DeviceImpl::get_name_lower() method
///		
///		Revision 1.1.4.12  2004/05/17 15:35:26  taurel
///		- Added full (server and client) re-connection to notifid in case it crashes
///		  or if the computer where it runs is re-boot
///		
///		Revision 1.1.4.11  2004/04/07 11:22:10  taurel
///		- Add some import/export declaration for Windows DLL
///		- Add test on minor code for the CORBA::IMP_LIMIT exception before
///		  printing it
///		
///		Revision 1.1.4.10  2004/04/05 12:43:34  taurel
///		- A last minor change for HP 11 (thank's Claudio)
///		
///		Revision 1.1.4.9  2004/04/02 14:58:16  taurel
///		Changes for release 4.1
///		- Change the event.h inclusion place in tango.h
///		- Fix bugs in event.cpp file and add a clean way to shutdown event system
///		- Now support attribute min,max,min_alarm and max_alarm defined in scientific notation for long attribute
///		- Added debian30 support in Make.rules
///		
///		Revision 1.1.4.8  2004/03/19 15:25:39  taurel
///		- Changes for the Windows DLL which does not execute the functions   registered by atexit at the same time than static libs or Unix libs !!!
///		- Work to be able to destroy the EventCOnsumer class (via ApiUtil dtor)
///		  but I don't think it is ready as it is commited now
///		- FIx bug in subscribe_event() if several filters in its vector<string>
///		  (thanks Majid)
///		
///		Revision 1.1.4.7  2004/03/09 16:36:36  taurel
///		- Added HP aCC port (thanks to Claudio from Elettra)
///		- Some last small bugs fixes
///		
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


void leavefunc()
{
//	cout << "Leaving............" << endl;
	EventConsumer *ev = ApiUtil::instance()->get_event_consumer();
	
	if (ev != NULL)
	{
	
//
// Clean-up heartbeat filters in notification service
//

		ev->cleanup_heartbeat_filters();

//
// Shut-down the KeepAliveThread and wait for it to exit
//

		{
			omni_mutex_lock sync(ev->cmd);
			
			ev->cmd.cmd_pending = true;
			ev->cmd.cmd_code = EXIT_TH;
			
			ev->cmd.cond.signal();
		}
				
		int *rv;
		ev->keep_alive_thread->join((void **)&rv);
			
//
// Destroy DeviceProxy object stored in map container
//

		ev->cleanup_EventChannel_map();		
			
//
// Shut-down the ORB and wait for its thread to exit
//
		ev->orb_->shutdown(true);

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
// Don't do this for windows DLL.
//
// Is this necessary when events are used within a server ?
//

#ifndef _USRDLL
	if (ptr->in_server() == false)	
		atexit(leavefunc);
#endif

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

		orb_->run();

		orb_->destroy();
		
		CORBA::release(orb_);
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
// If no connection exists to this channel then connect to it.
// Sometimes, this method is called in order to reconnect
// to the notifd. In such a case, the lock is already
// locked before the method is called
//

	std::map<std::string,EventChannelStruct>::iterator ipos;

	{
		omni_mutex_lock lo(channel_map_mut);
		ipos = channel_map.find(channel_name);

		if (ipos == channel_map.end())
		{
			connect_event_channel(channel_name,device_proxy->get_device_db(),false);
		}
		if (channel_map[channel_name].adm_device_proxy != NULL)
			delete channel_map[channel_name].adm_device_proxy;
		channel_map[channel_name].adm_device_proxy = new DeviceProxy(adm_name);
	}
	
	device_channel_map[device_name] = channel_name;
}

void EventConsumer::connect_event_channel(string channel_name,Database *db,bool reconnect)
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
	else
	{
                EventSystemExcept::throw_exception((const char*)"API_EventChannelNotExported",
                        	(const char*)"Failed to narrow EventChannel (hint: make sure a notifd process is running on the server host)",
                        	(const char*)"EventConsumer::create()");  
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

	if (reconnect == true)
	{
		channel_map[channel_name].eventChannel = eventChannel;
		channel_map[channel_name].structuredProxyPushSupplier = structuredProxyPushSupplier;
		channel_map[channel_name].last_heartbeat = time(NULL);
		channel_map[channel_name].heartbeat_skipped = false;
	}
	else
	{
		new_event_channel_struct.eventChannel = eventChannel;
		new_event_channel_struct.structuredProxyPushSupplier = structuredProxyPushSupplier;
		new_event_channel_struct.last_heartbeat = time(NULL);
		new_event_channel_struct.heartbeat_skipped = false;
		new_event_channel_struct.adm_device_proxy = NULL;
		channel_map[channel_name] = new_event_channel_struct;
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

void EventConsumer::attr_to_device(const AttributeValue *attr_value,
				   const AttributeValue_3 *attr_value_3,
				   long vers,DeviceAttribute *dev_attr)
{

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
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
		
	CORBA::ULong max,len;

	if (vers >= 3)
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
		if (vers >= 3)
			ty = attr_value_3->value.type();
		else
			ty = attr_value->value.type();
			
		if (ty->kind() == CORBA::tk_enum)
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
					if (vers >= 3)
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

				case tk_short:
					if (vers >= 3)
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
					if (vers >= 3)
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
					if (vers >= 3)
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
					if (vers >= 3)
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
					if (vers >= 3)
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
					if (vers >= 3)
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
					if (vers >= 3)
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
			}
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
			AttributeValue *attr_value = NULL;
			AttributeValue_3 *attr_value_3 = NULL;
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
					CORBA::String_var st_name;
					st_name = ty->name();
					const char *tmp_ptr = st_name.in();
					long vers;
					if (::strcmp(tmp_ptr,"AttributeValue_3") == 0)
					{
						event.remainder_of_body >>= attr_value_3;
						vers = 3;
						attr_to_device(attr_value,attr_value_3,vers,dev_attr);
					}
					else if (::strcmp(tmp_ptr,"AttributeValue") == 0)
					{
						event.remainder_of_body >>= attr_value;
						vers = 2;
						attr_to_device(attr_value,attr_value_3,vers,dev_attr);
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
				}			

				EventData *event_data = new EventData(event_callback_map[attr_event_name].device,
							domain_name,
							event_name,
							dev_attr,
							errors);
				cb_map_mut.unlock();
				callback->push_event(event_data);
				delete dev_attr;
				event_data->attr_value = NULL;
				delete event_data;
			}
		}
		else
		{
			cb_map_mut.unlock();
		}
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

//
// Build callback map key
//

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
	
	string att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	string callback_key = device_name + "/" + att_name_lower + "." + event_name;

	if (event_callback_map.find(callback_key) != event_callback_map.end())
	{
		TangoSys_OMemStream o;

		o << "Already connected to event " << callback_key << ends;
		Except::throw_exception((const char *)"API_MethodArgument",
			      		o.str(),
			      		(const char *)"EventConsumer::subscribe_event");
	}
		
//
// Search (or create) entry for channel map
//
	
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

//
// inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is
// down !)
//

	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(device_name);
	subscriber_info.push_back(att_name_lower);
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
	
	char constraint_expr[512];
	::sprintf(constraint_expr,"$domain_name == \'%s/%s\' and $event_name == \'%s\'",
		device_name.c_str(),att_name_lower.c_str(),event_name.c_str());
		
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
		new_event_callback.filter_constraint = constraint_expr;
		subscribe_event_id++;
		new_event_callback.id = subscribe_event_id;
		
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
		event_callback_map[callback_key].filter_ok = true;
	
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
		event_data->attr_value = NULL;
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
	std::map<std::string,EventCallBackStruct>::iterator epos;
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
					(const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() (hint: check the Notification daemon is running ",
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
	std::map<std::string,EventChannelStruct>::iterator ipos;

	channel_map_mut.lock();
	for (ipos = channel_map.begin(); ipos != channel_map.end(); ipos++)
	{
		CosNotifyFilter::Filter_var f = ipos->second.structuredProxyPushSupplier->get_filter(ipos->second.heartbeat_filter_id);
		ipos->second.structuredProxyPushSupplier->remove_filter(ipos->second.heartbeat_filter_id);
		f->destroy();
	}
	channel_map_mut.unlock();
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
	std::map<std::string,EventChannelStruct>::iterator ipos;
	{
		omni_mutex_lock lo(channel_map_mut);
		for (ipos = channel_map.begin(); ipos != channel_map.end(); ipos++)
		{
			if (ipos->second.adm_device_proxy != NULL)
			{
				delete ipos->second.adm_device_proxy;
				ipos->second.adm_device_proxy = NULL;
			}
		}
	}
}


/************************************************************************/
/*		       							*/		
/* 			EventData class 				*/
/*			---------------					*/
/*		       							*/
/************************************************************************/


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
	attr_value = new (DeviceAttribute);
	*attr_value = *(sou.attr_value);
	err = sou.err;
	errors = sou.errors;
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
	attr_value = new (DeviceAttribute);
	*attr_value = *(ri.attr_value);
	err = ri.err;
	errors = ri.errors;
	
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
	
/************************************************************************/
/*		       							*/		
/* 			EventConsumerKeepAlive class 			*/
/*			----------------------------			*/
/*		       							*/
/************************************************************************/




//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::reconnect_to_channel()
// 
// description : 	Method to reconnect the process to an event channel
//			in case of reconnection to a notifd
//
// argument : in :	ipos : An iterator to the EventChannel structure to
//			       reconnect to in the Event Channel map
//			event_consumer : Pointer to the EventConsumer
//					 singleton
//
// This method returns true if the reconnection succeeds. Otherwise, returns
// false
//
//-----------------------------------------------------------------------------

bool EventConsumerKeepAliveThread::reconnect_to_channel(EvChanIte &ipos,EventConsumer *event_consumer)
{
	bool ret = true;
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect()" << endl;

	event_consumer->cb_map_mut.lock();
	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
	{
		if ((epos->second.channel_name == ipos->first) && (epos->second.callback != NULL))
		{
			try
			{

				event_consumer->connect_event_channel(epos->second.channel_name,
								      epos->second.device->get_device_db(),
								      true);
		
				if (ipos->second.adm_device_proxy != NULL)
					delete ipos->second.adm_device_proxy;
				ipos->second.adm_device_proxy = new DeviceProxy(epos->second.channel_name);
				cout3 << "Reconnected to event channel" << endl;
			}
			catch(...)
			{
				ret = false;
			}
			
			break;
		}
	}

	event_consumer->cb_map_mut.unlock();
	
	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::reconnect_to_event()
// 
// description : 	Method to reconnect each event associated to a specific
//			event channel to the just reconnected event channel
//
// argument : in :	ipos : An iterator to the EventChannel structure in the 
//			       Event Channel map
//			event_consumer : Pointer to the EventConsumer
//					 singleton
//
//-----------------------------------------------------------------------------

void EventConsumerKeepAliveThread::reconnect_to_event(EvChanIte &ipos,EventConsumer *event_consumer)
{
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect_to_event()" << endl;

	event_consumer->cb_map_mut.lock();
	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
	{
		if ((epos->second.channel_name == ipos->first) && (epos->second.callback != NULL))
		{
			try
			{
				re_subscribe_event(epos,ipos);
				epos->second.filter_ok = true;
				cout3 << "Reconnected to event" << endl;
			}
			catch(...)
			{
				epos->second.filter_ok = false;
			}
		}
	}
	event_consumer->cb_map_mut.unlock();
	
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::re_subscribe_event()
// 
// description : 	Method to reconnect a specific event to an
//			event channel just reconnected
//
// argument : in :	epos : An iterator to the EventCallback structure in the 
//			       Event Callback map
//			ipos : Pointer to the EventChannel structure in the
//			       Event Channel map
//
//-----------------------------------------------------------------------------

void EventConsumerKeepAliveThread::re_subscribe_event(EvCbIte &epos,EvChanIte &ipos)
{
//
// Build a filter using the CORBA Notify constraint Language
// (use attribute name in lowercase letters)
//

	CosNotifyFilter::FilterFactory_var ffp;
  	CosNotifyFilter::Filter_var filter = CosNotifyFilter::Filter::_nil();
	CosNotifyFilter::FilterID filter_id;

	string channel_name = epos->second.channel_name;	
	try
	{
    		ffp    = ipos->second.eventChannel->default_filter_factory();  
    		filter = ffp->create_filter("EXTENDED_TCL");
  	}
	catch (CORBA::COMM_FAILURE &)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");        
  	}
	catch (...)
	{
		EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");        
  	}
	
//  	
// Construct a simple constraint expression; add it to fadmin
//

	string constraint_expr = epos->second.filter_constraint;

	CosNotification::EventTypeSeq evs;
  	CosNotifyFilter::ConstraintExpSeq exp;
  	exp.length(1);
  	exp[0].event_types = evs;
  	exp[0].constraint_expr = CORBA::string_dup(constraint_expr.c_str());
  	CORBA::Boolean res = 0; // OK
  	try
	{
    		CosNotifyFilter::ConstraintInfoSeq_var dummy = filter->add_constraints(exp);

    		filter_id = ipos->second.structuredProxyPushSupplier->add_filter(filter);

		epos->second.filter_id = filter_id;
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
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");        
  	}
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumerKeepAliveThread::run_undetached
// 
// description : 	The main code of the KeepAliveThread
//
//-----------------------------------------------------------------------------

void *EventConsumerKeepAliveThread::run_undetached(void *arg) 
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

	bool exit_th = false;
	event_consumer = ApiUtil::instance()->get_event_consumer();
	while (exit_th == false)
	{
		time_to_sleep = EVENT_HEARTBEAT_PERIOD;
		
//
// go to sleep until next heartbeat
// Wait on a monitor. This allows another thread to wake-up this thread
// before the end of the EVENT_HEARTBEAT_PERIOD time which is 10 seconds
// Only one command can now be send to the thread. It is a stop command
//

		{
			omni_mutex_lock sync(shared_cmd);
			if (shared_cmd.cmd_pending == false)
			{
				unsigned long s,n;

				unsigned long nb_sec,nb_nanos;
				nb_sec = time_to_sleep ;
				nb_nanos = 0;

				omni_thread::get_time(&s,&n,nb_sec,nb_nanos);
				shared_cmd.cond.timedwait(s,n);
			}
			if (shared_cmd.cmd_pending == true)
				exit_th = true;
		}
		
//
// Re-subscribe
//
 
		std::map<std::string,EventChannelStruct>::iterator ipos;
		std::map<std::string,EventCallBackStruct>::iterator epos;
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
// Check if a heartbeat have been skipped
// If a heartbeat is missing, there are two possibilities :
// 1 - The notifd is dead (or the crate is rebooting or has already reboot)
// 2 - The server is dead
//

 			bool heartbeat_skipped;

			heartbeat_skipped = (now - ipos->second.last_heartbeat) > EVENT_HEARTBEAT_PERIOD;
			if (heartbeat_skipped || ipos->second.heartbeat_skipped)
			{
				ipos->second.heartbeat_skipped = true;
				
//
// Check notifd but trying to read an attribute of the event channel
//

				bool notifd_failed = false;
				try
				{
					CosNotifyChannelAdmin::EventChannelFactory_var ecf = ipos->second.eventChannel->MyFactory();
				}
				catch (...)
				{
					notifd_failed = true;
					cout3 << "Notifd is dead !!!" << endl;
				}

//
// Re-build connection to the event channel
// This is a two steps process. First, reconnect
// to the new event channel, then reconnect
// callbacks to this new event channel
//

				bool notifd_reco;
				if (notifd_failed == true)
				{
					notifd_reco = reconnect_to_channel(ipos,event_consumer);

					if (notifd_reco == true)
					{
						reconnect_to_event(ipos,event_consumer);
					}
				}				
				
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
					if ((epos->second.channel_name == ipos->first) && (epos->second.callback != NULL))
					{
						if (epos->second.filter_ok == false)
						{
							try
							{
								re_subscribe_event(epos,ipos);
								epos->second.filter_ok = true;
							}
							catch(...) {}
						}
						
						
						EventData *event_data = new EventData(epos->second.device,
										      domain_name,
										      event_name,
										      dev_attr,
										      errors);						CallBack *callback;
						callback = epos->second.callback;
						callback->push_event(event_data);
						delete event_data;
						
       						DeviceData subscriber_in;
       						vector<string> subscriber_info;
       						subscriber_info.push_back(epos->second.device->dev_name());
       						subscriber_info.push_back(epos->second.attr_name);
       						subscriber_info.push_back("subscribe");
       						subscriber_info.push_back(epos->second.event_name);
       						subscriber_in << subscriber_info;

						bool ds_failed = false;
						try 
						{
       							ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);
							ipos->second.heartbeat_skipped = false;
        						ipos->second.last_subscribed = time(NULL);
						}
						catch (...) {ds_failed = true;}
													
						if (ds_failed == false)
						{							
							if ((epos->second.event_name == "change") ||
	    						    (epos->second.event_name == "quality") || 
	    						    (epos->second.event_name == "archive") ||
	    						    (epos->second.event_name == "user_event"))
							{
								DeviceAttribute da;
								DevErrorList err;
								err.length(0);
								string domain_name = epos->second.device->dev_name() + "/" + epos->second.attr_name;

								bool old_transp = epos->second.device->get_transparency_reconnection();
								epos->second.device->set_transparency_reconnection(true);

								try
								{
									da = epos->second.device->read_attribute(epos->second.attr_name.c_str());
								}
								catch (DevFailed &e)
								{
									err = e.errors;
								}
								epos->second.device->set_transparency_reconnection(old_transp);


								EventData *event_data = new EventData(epos->second.device,
												      domain_name,
												      epos->second.event_name,
												      &da,
												      err);

								callback->push_event(event_data);
								event_data->attr_value = NULL;
								delete event_data;				
							}
						}
					}
				}
				event_consumer->cb_map_mut.unlock();
			}
			
		}
		event_consumer->channel_map_mut.unlock();
	}
	
//
// If we arrive here, this means that we have received the exit thread
// command.
//

	return (void *)NULL;
		
}
	
} /* End of Tango namespace */


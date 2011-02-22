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
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011
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
///
///		$Revision$
///
///		$Log$
///		Revision 1.12  2010/10/06 12:31:15  taurel
///		- Fix bug in re-connection synchronous call for attribute configuration
///		change event
///		
///		Revision 1.11  2010/09/29 12:04:35  taurel
///		- It's now possible to register several callbacks for the same event
///		
///		Revision 1.10  2010/09/09 13:46:00  taurel
///		- Add year 2010 in Copyright notice
///		
///		Revision 1.9  2010/05/26 09:15:36  taurel
///		- Another commit after merge with the bug fixes branch
///		
///		Revision 1.8  2009/12/08 07:55:15  taurel
///		- Get some bug fixes from a merge with the Release_7_1_1-bugfixes branch
///		Revision 1.7.2.2  2010/05/18 08:27:23  taurel
///		- Events from device in a DS started with a file as database are now
///		back into operation
///		
///		Revision 1.7.2.1  2009/12/04 15:17:50  taurel
///		- Fix bug in case of application shutdown when the notifd is dead.
///		If the thread abort is required, do it immediately
///		
///		Revision 1.7  2009/09/22 07:55:05  jensmeyer
///		Changed readers to writers lock in EventConsumerKeepAliveThread::run_undetached() for connection
///		test of not yet connected events.
///		
///		Revision 1.6  2009/03/13 09:33:29  taurel
///		- Small changes to fix Windows VC8 warnings in Warning level 3
///		
///		Revision 1.5  2009/01/29 15:25:41  taurel
///		- First implementation of the Data Ready event
///		
///		Revision 1.4  2009/01/21 12:49:03  taurel
///		- Change CopyRights for 2009
///		
///		Revision 1.3  2008/10/06 15:01:09  taurel
///		- Changed the licensing info from GPL to LGPL
///		
///		Revision 1.2  2008/10/03 06:52:31  taurel
///		- Add some licensing info in each files
///		
///		Revision 1.1  2008/09/15 07:24:08  jensmeyer
///		Added event queues for event reception.
///		To clean-up event.cpp two new files have been added
///		evnetkeepalive.cpp and eventqueue.cpp.
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
/* 			EventConsumerKeepAlive class 								*/
/*			----------------------------								*/
/*		       															*/
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

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
	{
		if (epos->second.channel_name == ipos->first)
		{
			bool need_reconnect = false;
			vector<EventSubscribeStruct>:: iterator esspos;
			for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
			{
				if (esspos->callback != NULL || esspos->ev_queue != NULL)
				{
					need_reconnect = true;
					break;
				}
			}

			if (need_reconnect == true)
			{
				try
				{
					string adm_name = ipos->second.full_adm_name;
					event_consumer->connect_event_channel(adm_name,
									      epos->second.device->get_device_db(),
									      true);
			
					if (ipos->second.adm_device_proxy != NULL)
						delete ipos->second.adm_device_proxy;
					ipos->second.adm_device_proxy = new DeviceProxy(ipos->second.full_adm_name);
					cout3 << "Reconnected to event channel" << endl;
				}
				catch(...)
				{
					ret = false;
				}
			
				break;
			}
		}
	}

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

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
	{
		if (epos->second.channel_name == ipos->first)
		{
			bool need_reconnect = false;
			vector<EventSubscribeStruct>:: iterator esspos;
			for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
			{
				if (esspos->callback != NULL || esspos->ev_queue != NULL)
				{
					need_reconnect = true;
					break;
				}
			}

			if (need_reconnect == true)
			{
				try
				{
					epos->second.callback_monitor->get_monitor();
				
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
				
					epos->second.callback_monitor->rel_monitor();
				}
				catch (...)
				{
					cerr << "EventConsumerKeepAliveThread::reconnect_to_event() cannot get callback monitor for " << epos->first << endl;
				}
			}
		}
	}
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
	int time_to_sleep;
	time_t now;
	EventConsumer *event_consumer;

//
// first sleep 2 seconds to give the event system time to startup
//

#ifndef _TG_WINDOWS_
	unsigned int time_left;
	time_left = ::sleep(2);
	if (time_left != 0)
		::sleep(time_left);
#else
	Sleep(2000L);
#endif /* _TG_WINDOWS_ */

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
			{
				exit_th = true;
				return (void *)NULL;
			}
		}

//
// Re-subscribe
//
 
		// lock the maps only for reading
		event_consumer->map_modification_lock.writerIn();
			
		now = time(NULL);
			
//
// check the list of not yet connected events and try to subscribe
//
			
		if ( !event_consumer->event_not_connected.empty() )
		{
			std::vector<EventNotConnected>::iterator vpos;
			for (vpos = event_consumer->event_not_connected.begin(); 
				 vpos != event_consumer->event_not_connected.end();
				 /*vpos++*/)
			{
				bool inc_vpos = true;
				// check wether it is necessary to try to subscribe again!
				if ( (now - vpos->last_heartbeat) >= (EVENT_HEARTBEAT_PERIOD) )
				{
					try
					{
						// try to subscribe

						event_consumer->connect_event (vpos->device,vpos->attribute,vpos->event_type, 
																					vpos->callback,
																					vpos->ev_queue, 
																					vpos->filters,
																					vpos->event_name,
																					vpos->event_id);	
							
						// delete element from vector when subscribe worked
						vpos = event_consumer->event_not_connected.erase(vpos);
						inc_vpos = false;
					}
		
					catch (Tango::DevFailed &e) 
					{
						// subscribe has not worked, try again in the next hearbeat period
						vpos->last_heartbeat = now;
						
						// The event can still not be connected.
						// Send the return error message as event to the client application.
						//
						// push an event with the error message!
						
						DevErrorList err;
						err.length(0);
						string domain_name = vpos->device->dev_name() + "/" + vpos->attribute;
						err = e.errors;
											
						//
						// For attribute data event
						//
						
						if ((vpos->event_name == "change") ||
	    					(vpos->event_name == "quality") || 
	    					(vpos->event_name == "archive") ||
	    					(vpos->event_name == "user_event"))
						{
							//DeviceAttribute da;
							DeviceAttribute *da = NULL;
							EventData *event_data = new EventData(vpos->device,
													      			domain_name,
													      			vpos->event_name,
													      			da,
													      			err);
							
							// if a callback method was specified, call it!
							if (vpos->callback != NULL )
							{										
								try
								{
									vpos->callback->push_event(event_data);
								}
								catch (...)
								{
									cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << domain_name << endl;
								}
						
								//event_data->attr_value = NULL;							
								delete event_data;
							}

							// no callback method, the event has to be instered
							// into the event queue
							else
							{
								vpos->ev_queue->insert_event(event_data);
							}							
						}
						
//
// For attribute configuration event
//
							
						else if (vpos->event_name == CONF_TYPE_EVENT)
						{
							//AttributeInfoEx aie;
							AttributeInfoEx *aie = NULL;
							AttrConfEventData *event_data = new AttrConfEventData(vpos->device,
													      			domain_name,
													      			vpos->event_name,
													      			aie,
													      			err);
							
							// if a callback method was specified, call it!
							if (vpos->callback != NULL )
							{
								try
								{
									vpos->callback->push_event(event_data);
								}
								catch (...)
								{
									cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << domain_name << endl;
								}
							
								//event_data->attr_conf = NULL;
								delete event_data;
							}
								
							// no calback method, the event has to be inserted
							// into the event queue
							else
							{
								vpos->ev_queue->insert_event(event_data);
							}					
						}
						else if (vpos->event_name == DATA_READY_TYPE_EVENT)
						{
							DataReadyEventData *event_data = new DataReadyEventData(vpos->device,NULL,vpos->event_name,err);

							// if a callback method was specified, call it!
							if (vpos->callback != NULL )
							{
								try
								{
									vpos->callback->push_event(event_data);
								}
								catch (...)
								{
									cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << domain_name << endl;
								}
								delete event_data;
							}
								
							// no calback method, the event has to be inserted
							// into the event queue
							else
								vpos->ev_queue->insert_event(event_data);	
						}
					}
					
					catch (...)
					{						
						// subscribe has not worked, try again in the next hearbeat period
						vpos->last_heartbeat = now;
						
						cout << "During the event subscription an exception was send which is not a Tango::DevFailed exception!" << endl;	
					}						
				}
				if (inc_vpos) 
					++vpos;
			}
		}

		event_consumer->map_modification_lock.writerOut();
		
//		
// Check for all other event reconnections
//	
		
		{
			// lock the maps only for reading
			ReaderLock r (event_consumer->map_modification_lock);	
			
			std::map<std::string,EventChannelStruct>::iterator ipos;
			std::map<std::string,EventCallBackStruct>::iterator epos;

			for (ipos = event_consumer->channel_map.begin(); ipos != event_consumer->channel_map.end(); ipos++)
			{
				try
				{
					// lock the event channel
					ipos->second.channel_monitor->get_monitor();

					if ((now - ipos->second.last_subscribed) > EVENT_RESUBSCRIBE_PERIOD/3)
					{
						for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
						{
							if (epos->second.channel_name == ipos->first )
							{
								try
								{
									// lock the callback 
									epos->second.callback_monitor->get_monitor();

									DeviceData subscriber_in;
									vector<string> subscriber_info;
									subscriber_info.push_back(epos->second.device->dev_name());
									subscriber_info.push_back(epos->second.attr_name);
									subscriber_info.push_back("subscribe");
									subscriber_info.push_back(epos->second.event_name);
									subscriber_in << subscriber_info;
									ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);

									ipos->second.last_subscribed = time(NULL);
        							epos->second.last_subscribed = time(NULL);

									epos->second.callback_monitor->rel_monitor();
								}
								catch (...) 
								{
									epos->second.callback_monitor->rel_monitor();
								};	
							}
						}
					}

//
// Check if a heartbeat have been skipped
// If a heartbeat is missing, there are four possibilities :
// 1 - The notifd is dead (or the crate is rebooting or has already reboot)
// 2 - The server is dead
// 3 - The network was down;
// 4 - The server has been restarted on another host.
//

 					bool heartbeat_skipped;
					heartbeat_skipped = ((now - ipos->second.last_heartbeat) > (EVENT_HEARTBEAT_PERIOD + 1));
										
					if (heartbeat_skipped || ipos->second.heartbeat_skipped || ipos->second.notifd_failed == true )
					{
						ipos->second.heartbeat_skipped = true;

//
// Check notifd by trying to read an attribute of the event channel
//

						try
						{
// 
//  Check if the device server is now running on a different host.
//  In this case we have to reconnect to another notification daemon.
//						 
							DeviceInfo info;
							try
							{
								info = ipos->second.adm_device_proxy->info();
							}
							catch (Tango::DevFailed &)
							{
								// in case of failure, just stay connected to the actual notifd
								info.server_host = ipos->second.notifyd_host;
							}

							if ( ipos->second.notifyd_host != info.server_host )
							{
								ipos->second.notifd_failed = true;
							}
							else
							{
								CosNotifyChannelAdmin::EventChannelFactory_var ecf = ipos->second.eventChannel->MyFactory();
								if (ipos->second.full_adm_name.find(MODIFIER_DBASE_NO) != string::npos)
									ipos->second.notifd_failed = true;
							}
						}
						catch (...)
						{
							ipos->second.notifd_failed = true;
							cout3 << "Notifd is dead !!!" << endl;
						}
		
// if the connection to the notify daemon is marked as ok, the device server is working fine but
// the heartbeat is still not coming back since three periods:
// The notify deamon might have closed the connection, try to reconnect!
 
						if ( ipos->second.notifd_failed == false &&
						     ipos->second.has_notifd_closed_the_connection >= 3 )
						{
							ipos->second.notifd_failed = true;
						}
		
//
// Re-build connection to the event channel
// This is a two steps process. First, reconnect
// to the new event channel, then reconnect
// callbacks to this new event channel
//

						if ( ipos->second.notifd_failed == true )
						{
							bool notifd_reco = reconnect_to_channel(ipos,event_consumer);
							if ( notifd_reco )
								ipos->second.notifd_failed = false;
							else
								ipos->second.notifd_failed = true;

							if ( ipos->second.notifd_failed == false ) 
							{
								reconnect_to_event(ipos,event_consumer);
							}
						}				

						Tango::DevErrorList errors(1);

						errors.length(1);
						errors[0].severity = Tango::ERR;
						errors[0].origin = CORBA::string_dup("EventConsumer::KeepAliveThread()");
						errors[0].reason = CORBA::string_dup("API_EventTimeout");
						errors[0].desc = CORBA::string_dup("Event channel is not responding anymore, maybe the server or event system is down");
						DeviceAttribute *dev_attr = NULL;
						AttributeInfoEx *dev_attr_conf = NULL;

						for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); epos++)
						{
							if (epos->second.channel_name == ipos->first) 
							{

								bool need_reconnect = false;
								vector<EventSubscribeStruct>:: iterator esspos;
								for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
								{
									if (esspos->callback != NULL || esspos->ev_queue != NULL)
									{
										need_reconnect = true;
										break;
									}
								}

								// lock the callback
								try
								{
									epos->second.callback_monitor->get_monitor();

									if (need_reconnect == true)
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
									}

									string domain_name;
									string event_name;

									string::size_type pos = epos->first.rfind('.');
									if (pos == string::npos)
									{
										domain_name = "domain_name";
										event_name = "event_name";
									}
									else
									{
										domain_name = epos->first.substr(0,pos);
										event_name = epos->first.substr(pos + 1);
									}						

									for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
									{
										CallBack   *callback = esspos->callback;
										EventQueue *ev_queue = esspos->ev_queue;
//
// Push an event with error set
//

										if (event_name == CONF_TYPE_EVENT)
										{
											AttrConfEventData *event_data = new AttrConfEventData(epos->second.device,
											      												domain_name,
											      												event_name,
											      												dev_attr_conf,
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
													cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
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
										else if (event_name == DATA_READY_TYPE_EVENT)
										{
											DataReadyEventData *event_data = new DataReadyEventData(epos->second.device,NULL,event_name,errors);
											// if a callback method was specified, call it!
											if (callback != NULL )
											{
												try
												{
													callback->push_event(event_data);
												}
												catch (...)
												{
													cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
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
											EventData *event_data = new EventData(epos->second.device,
											      			domain_name,
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
													cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
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

									if ( ipos->second.notifd_failed == false )
									{
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

//
// Push an event with the value just read from the
// re-connected server
// NOT NEEDED for the Data Ready event
//

											if ((epos->second.event_name == "change") ||
	    							   			 (epos->second.event_name == "quality") || 
	    							   			 (epos->second.event_name == "archive") ||
	    							   			 (epos->second.event_name == "user_event"))
											{

//
// For attribute data event
//

												//DeviceAttribute da;
												DeviceAttribute *da = NULL;
												DevErrorList err;
												err.length(0);
												string domain_name = epos->second.device->dev_name() + "/" + epos->second.attr_name;

												bool old_transp = epos->second.device->get_transparency_reconnection();
												epos->second.device->set_transparency_reconnection(true);

												try
												{
													da = new DeviceAttribute();
													*da = epos->second.device->read_attribute(epos->second.attr_name.c_str());
													
// The reconnection worked fine. The heartbeat should come back now,
// when the notifd has not closed the connection.
// Increase the counter to detect when the heartbeat is not coming back.
													ipos->second.has_notifd_closed_the_connection++;
												}
												catch (DevFailed &e)
												{
													err = e.errors;
												}
												epos->second.device->set_transparency_reconnection(old_transp);
												
												// if callback methods were specified, call them!
												unsigned int cb_nb = epos->second.callback_list.size();
												unsigned int cb_ctr = 0;
												DeviceAttribute *da_copy = NULL;

												for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
												{
													cb_ctr++;
													EventData *event_data;
													if (cb_ctr != cb_nb)
													{
														da_copy = new DeviceAttribute();
														da_copy->deep_copy(*da);

														event_data = new EventData(epos->second.device,
													      				domain_name,
													      				epos->second.event_name,
													      				da_copy,
													      				err);
													}
													else
													{
														event_data = new EventData(epos->second.device,
													      				domain_name,
													      				epos->second.event_name,
													      				da,
													      				err);
													}

													CallBack   *callback = esspos->callback;
													EventQueue *ev_queue = esspos->ev_queue;

													if (callback != NULL )
													{
														try
														{
															callback->push_event(event_data);
														}
														catch (...)
														{
															cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
														}
														
														//event_data->attr_value = NULL;
														delete event_data;
													}
													
													// no calback method, the event has to be inserted
													// into the event queue
													else
													{
														ev_queue->insert_event(event_data);
													}
												}
											}
											
											else if (epos->second.event_name == CONF_TYPE_EVENT)
											{
//
// For attribute configuration event
//
												//AttributeInfoEx aie;
												AttributeInfoEx *aie = NULL;
												DevErrorList err;
												err.length(0);
												string domain_name = epos->second.device->dev_name() + "/" + epos->second.attr_name;

												bool old_transp = epos->second.device->get_transparency_reconnection();
												epos->second.device->set_transparency_reconnection(true);

												try
												{
													aie = new AttributeInfoEx();
													*aie = epos->second.device->get_attribute_config(epos->second.attr_name);
													
// The reconnection worked fine. The heartbeat should come back now,
// when the notifd has not closed the connection.
// Increase the counter to detect when the heartbeat is not coming back.

													ipos->second.has_notifd_closed_the_connection++;		
												}
												catch (DevFailed &e)
												{
													err = e.errors;
												}
												epos->second.device->set_transparency_reconnection(old_transp);

												unsigned int cb_nb = epos->second.callback_list.size();
												unsigned int cb_ctr = 0;
												AttributeInfoEx *aie_copy = NULL;

												for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
												{
													cb_ctr++;
													AttrConfEventData *event_data;
													if (cb_ctr != cb_nb)
													{
														aie_copy = new AttributeInfoEx;
														*aie_copy = *aie;
														event_data = new AttrConfEventData(epos->second.device,
													      				domain_name,
													      				epos->second.event_name,
													      				aie_copy,
													      				err);
													}
													else
													{
														event_data = new AttrConfEventData(epos->second.device,
													      				domain_name,
													      				epos->second.event_name,
													      				aie,
													      				err);
													}

													CallBack   *callback = esspos->callback;
													EventQueue *ev_queue = esspos->ev_queue;

													// if a callback method was specified, call it!
													if (callback != NULL )
													{
														try
														{
															callback->push_event(event_data);
														}
														catch (...)
														{
															cerr << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first << endl;
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
										}
									}
									// release callback monitor
									epos->second.callback_monitor->rel_monitor();
								}
								catch (...)
								{
									cerr << "EventConsumerKeepAliveThread::run_undetached() timeout on callback monitor of " << epos->first << endl;
								}	
							}
						}
					}
					else
					{
						// When the heartbeat has worked, mark the connection to the notifd a OK
						ipos->second.has_notifd_closed_the_connection = 0;
					}

					// release channel monitor
					ipos->second.channel_monitor->rel_monitor();
				}
				catch (...)
				{
					cerr << "EventConsumerKeepAliveThread::run_undetached() timeout on callback monitor of " << epos->first << endl;
				}			
			}
		}
	}
	
//
// If we arrive here, this means that we have received the exit thread
// command.
//

	return (void *)NULL;
		
}
	
} /* End of Tango namespace */


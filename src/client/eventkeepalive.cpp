static const char *RcsId = "$Id$";

//=====================================================================================================================
//
// file :				event.cpp
//
// description :		C++ classes for implementing the event system KeepAliveThread. This class cheks that the
//						heartbeat event are well received. It also manages the stateless subscription because this
//						is this class which will regularely re-try to subsribe to event in case it is needed.
//						Finally, it is also this class which generates the re-connection in case a device server
//						sending events is stopped and re-started
//
// author(s) : 			A.Gotz (goetz@esrf.fr)
//
// original : 			7 April 2003
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//====================================================================================================================

#include <tango.h>
#include <tango/client/eventconsumer.h>

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




//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::reconnect_to_channel()
//
// description :
//		Method to reconnect the process to an event channel in case of reconnection to a notifd
//
// argument :
//		in :
//			- ipos : An iterator to the EventChannel structure to reconnect to in the Event Channel map
//			- event_consumer : Pointer to the EventConsumer singleton
//
// return :
// 		This method returns true if the reconnection succeeds. Otherwise, returns false
//
//--------------------------------------------------------------------------------------------------------------------

bool EventConsumerKeepAliveThread::reconnect_to_channel(EvChanIte &ipos,EventConsumer *event_consumer)
{
	bool ret = true;
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect()" << endl;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); ++epos)
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
				    DeviceData dummy;
					string adm_name = ipos->second.full_adm_name;
					event_consumer->connect_event_channel(adm_name,
									      epos->second.device->get_device_db(),
									      true,dummy);

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


//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::reconnect_to_zmq_channel()
//
// description :
//		Method to reconnect the process to a ZMQ event channel in case of reconnection
//
// argument :
//		in :
//			- ipos : An iterator to the EventChannel structure to reconnect to in the Event Channel map
//			- event_consumer : Pointer to the EventConsumer singleton
//			- dd :
//
// return :
// 		This method returns true if the reconnection succeeds. Otherwise, returns false
//
//---------------------------------------------------------------------------------------------------------------------

bool EventConsumerKeepAliveThread::reconnect_to_zmq_channel(EvChanIte &ipos,EventConsumer *event_consumer,DeviceData &dd)
{
	bool ret = true;
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect_to_zmq_channel()" << endl;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); ++epos)
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
                    DeviceData subscriber_in,subscriber_out;
                    vector<string> subscriber_info;
                    subscriber_info.push_back(epos->second.device->dev_name());
                    subscriber_info.push_back(epos->second.obj_name);
                    subscriber_info.push_back("subscribe");
                    subscriber_info.push_back(epos->second.event_name);
					subscriber_info.push_back("0");
                    subscriber_in << subscriber_info;

                    subscriber_out = ipos->second.adm_device_proxy->command_inout("ZmqEventSubscriptionChange",subscriber_in);

					string adm_name = ipos->second.full_adm_name;

#ifdef ZMQ_HAS_DISCONNECT
//
// Forget exception which could happen during massive restart of device server process running on the same host
//
                    try
                    {
                        event_consumer->disconnect_event_channel(adm_name,ipos->second.endpoint,epos->second.endpoint);
                    }
                    catch (Tango::DevFailed &e) {}
#endif
					event_consumer->connect_event_channel(adm_name,
									      epos->second.device->get_device_db(),
									      true,subscriber_out);

                    dd = subscriber_out;
					if (ipos->second.adm_device_proxy != NULL)
						delete ipos->second.adm_device_proxy;
					ipos->second.adm_device_proxy = new DeviceProxy(ipos->second.full_adm_name);
					cout3 << "Reconnected to zmq event channel" << endl;
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

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::reconnect_to_event()
//
// description :
//		Method to reconnect each event associated to a specific event channel to the just reconnected event channel
//
// argument :
//		in :
//			- ipos : An iterator to the EventChannel structure in the Event Channel map
//			- event_consumer : Pointer to the EventConsumer singleton
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::reconnect_to_event(EvChanIte &ipos,EventConsumer *event_consumer)
{
	EvCbIte epos;

	cout3 << "Entering KeepAliveThread::reconnect_to_event()" << endl;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); ++epos)
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
					ApiUtil *au = ApiUtil::instance();
					stringstream ss;

					ss << "EventConsumerKeepAliveThread::reconnect_to_event() cannot get callback monitor for " << epos->first;
					au->print_error_message(ss.str().c_str());
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::re_subscribe_event()
//
// description :
//		Method to reconnect a specific event to an event channel just reconnected
//
// argument :
//		in :
//			- epos : An iterator to the EventCallback structure in the Event Callback map
//			- ipos : Pointer to the EventChannel structure in the Event Channel map
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::re_subscribe_event(EvCbIte &epos,EvChanIte &ipos)
{

//
// Build a filter using the CORBA Notify constraint Language (use attribute name in lowercase letters)
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
		EventSystemExcept::throw_exception((const char*)API_NotificationServiceFailed,
                       	(const char*)"Caught CORBA::COMM_FAILURE exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");
	}
	catch (...)
	{
		EventSystemExcept::throw_exception((const char*)API_NotificationServiceFailed,
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
	exp[0].constraint_expr = Tango::string_dup(constraint_expr.c_str());
	DevBoolean res = 0; // OK
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
		EventSystemExcept::throw_exception((const char*)API_NotificationServiceFailed,
                       	(const char*)"Caught exception while creating event filter (check filter)",
                       	(const char*)"EventConsumerKeepAliveThread::re_subscribe_event()");
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::reconnect_to_zmq_event()
//
// description :
//		Method to reconnect each event associated to a specific event channel to the just reconnected event channel
//
// argument :
//		in :
//			- ipos : An iterator to the EventChannel structure in the Event Channel map
//			- event_consumer : Pointer to the EventConsumer singleton
//			- dd :
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::reconnect_to_zmq_event(EvChanIte &ipos,EventConsumer *event_consumer,DeviceData &dd)
{
	EvCbIte epos;
#ifdef ZMQ_HAS_DISCONNECT
	bool disconnect_called = false;
#endif

	cout3 << "Entering KeepAliveThread::reconnect_to_zmq_event()" << endl;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); ++epos)
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
						EventCallBackStruct ecbs;
						vector<string> vs;

						vs.push_back(string("reconnect"));

						string d_name = epos->second.device->dev_name();
						string &fqen = epos->second.fully_qualified_event_name;
						string::size_type pos = fqen.find('/');
						pos = pos + 2;
						pos = fqen.find('/',pos);
						string prefix = fqen.substr(0,pos + 1);
						d_name.insert(0,prefix);

#ifdef ZMQ_HAS_DISCONNECT
						if (disconnect_called == false)
						{
							event_consumer->disconnect_event(epos->second.fully_qualified_event_name,epos->second.endpoint);
							disconnect_called = true;
						}
#endif
						event_consumer->connect_event_system(d_name,epos->second.obj_name,epos->second.event_name,
                                                             vs,ipos,ecbs,dd,ipos->second.valid_endpoint);

						const DevVarLongStringArray *dvlsa;
						dd >> dvlsa;
						epos->second.endpoint = dvlsa->svalue[(ipos->second.valid_endpoint << 1) + 1].in();

						cout3 << "Reconnected to ZMQ event" << endl;
					}
					catch(...)
					{
						epos->second.filter_ok = false;
					}

					epos->second.callback_monitor->rel_monitor();
				}
				catch (...)
				{
					ApiUtil *au = ApiUtil::instance();
					stringstream ss;

					ss << "EventConsumerKeepAliveThread::reconnect_to_zmq_event() cannot get callback monitor for " << epos->first;
					au->print_error_message(ss.str().c_str());
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::run_undetached
//
// description :
//		The main code of the KeepAliveThread
//
//-------------------------------------------------------------------------------------------------------------------

void *EventConsumerKeepAliveThread::run_undetached(TANGO_UNUSED(void *arg))
{
	int time_to_sleep;
	time_t now;
	ZmqEventConsumer *event_consumer;

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

	event_consumer = ApiUtil::instance()->get_zmq_event_consumer();

	while (exit_th == false)
	{
		time_to_sleep = EVENT_HEARTBEAT_PERIOD;

//
// Go to sleep until next heartbeat. Wait on a monitor. This allows another thread to wake-up this thread
// before the end of the EVENT_HEARTBEAT_PERIOD time which is 10 seconds. Only one command can now be send to the
// thread. It is a stop command
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

		cout4 << "KeepAliveThread at work" << endl;

//
// Be sure to have valid event consumer object (In case of long startup OS with some notifd event(s) subscribed at the
// end of the process startup. Verified with some ESRF HdbEventHandler process)
//

        if (event_consumer)
            event_consumer = ApiUtil::instance()->get_zmq_event_consumer();

		now = time(NULL);
		if ( event_consumer->event_not_connected.empty() == false)
        {
            DelayEvent de(event_consumer);
            event_consumer->map_modification_lock.writerIn();


//
// Check the list of not yet connected events and try to subscribe
//

            not_conected_event(event_consumer,now);

            event_consumer->map_modification_lock.writerOut();
        }

//
// Check for all other event reconnections
//

		{
			// lock the maps only for reading
			ReaderLock r (event_consumer->map_modification_lock);

			std::map<std::string,EventChannelStruct>::iterator ipos;
			std::map<std::string,EventCallBackStruct>::iterator epos;

			for (ipos = event_consumer->channel_map.begin(); ipos != event_consumer->channel_map.end(); ++ipos)
			{
				try
				{
					// lock the event channel
					ipos->second.channel_monitor->get_monitor();

//
// Check if it is necessary for client to confirm its subscription. Note that starting with Tango 8.1 (and for ZMQ),
// there is a new command in the admin device which allows a better (optimized) confirmation algorithm
//

					if ((now - ipos->second.last_subscribed) > EVENT_RESUBSCRIBE_PERIOD/3)
					{
						confirm_subscription(event_consumer,ipos);
					}

//
// Check if a heartbeat have been skipped. If a heartbeat is missing, there are four possibilities :
// 1 - The notifd is dead (or the crate is rebooting or has already reboot)
// 2 - The server is dead
// 3 - The network was down;
// 4 - The server has been restarted on another host.
//

 					bool heartbeat_skipped;
					heartbeat_skipped = ((now - ipos->second.last_heartbeat) >= EVENT_HEARTBEAT_PERIOD);

					if (heartbeat_skipped || ipos->second.heartbeat_skipped || ipos->second.event_system_failed == true)
					{
						ipos->second.heartbeat_skipped = true;
						main_reconnect(event_consumer,epos,ipos);
					}

					// release channel monitor
					ipos->second.channel_monitor->rel_monitor();
				}
				catch (...)
				{
					ApiUtil *au = ApiUtil::instance();
					stringstream ss;

					ss << "EventConsumerKeepAliveThread::run_undetached() timeout on callback monitor of " << epos->first;
					au->print_error_message(ss.str().c_str());
				}
			}
		}
	}

//
// If we arrive here, this means that we have received the exit thread command.
//

	return (void *)NULL;

}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::not_connected_event
//
// description :
//		Try to connect not yet connected event(s). Try first with ZMQ event consumer.
//		If ZMQ is not used (API_CommandNotFound exception), use notifd.
//
// argument :
//		in :
//			- event_consumer : The ZMQ event consumer object
//			- now : The date
//			- notifd_event_consumer : The notifd event consumer object
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::not_conected_event(ZmqEventConsumer *event_consumer,time_t now)
{
	if ( !event_consumer->event_not_connected.empty() )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos = event_consumer->event_not_connected.begin();
			 vpos != event_consumer->event_not_connected.end();
			 /*vpos++*/)
		{
			bool inc_vpos = true;

//
// check wether it is necessary to try to subscribe again!
//

			if ( (now - vpos->last_heartbeat) >= (EVENT_HEARTBEAT_PERIOD - 1) )
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

//
// delete element from vector when subscribe worked
//

					vpos = event_consumer->event_not_connected.erase(vpos);
					inc_vpos = false;
				}

				catch (Tango::DevFailed &e)
				{
						stateless_subscription_failed(vpos,e,now);
				}
				catch (...)
				{

//
// subscribe has not worked, try again in the next hearbeat period
//

					vpos->last_heartbeat = now;

					ApiUtil *au = ApiUtil::instance();
					au->print_error_message("During the event subscription an exception was sent which is not a Tango::DevFailed exception!");
				}
			}
			if (inc_vpos)
				++vpos;
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::fwd_not_connected_event
//
// description :
//		Try to connect not yet connected event(s). This method is called only in case of forwarded attribute with
//      root attribute inside the same process than the fwd attribute!
//
// argument :
//		in :
//			- event_consumer : The ZMQ event consumer object
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::fwd_not_conected_event(ZmqEventConsumer *event_consumer)
{

//
// lock the maps only for reading
//

    event_consumer->map_modification_lock.writerIn();

	if ( !event_consumer->event_not_connected.empty() )
	{
        time_t now = time(NULL);
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos = event_consumer->event_not_connected.begin();
			 vpos != event_consumer->event_not_connected.end();
			 /*vpos++*/)
		{
			bool inc_vpos = true;

//
// check wether it is necessary to try to subscribe again!
//

            try
            {
                // try to subscribe

                event_consumer->connect_event (vpos->device,vpos->attribute,vpos->event_type,
                                                                            vpos->callback,
                                                                            vpos->ev_queue,
                                                                            vpos->filters,
                                                                            vpos->event_name,
                                                                            vpos->event_id);

//
// delete element from vector when subscribe worked
//

                vpos = event_consumer->event_not_connected.erase(vpos);
                inc_vpos = false;
            }
            catch (Tango::DevFailed &e)
            {
                stateless_subscription_failed(vpos,e,now);
            }
            catch (...)
            {

//
// subscribe has not worked, try again in the next hearbeat period
//

                vpos->last_heartbeat = now;

                ApiUtil *au = ApiUtil::instance();
                au->print_error_message("During the event subscription an exception was sent which is not a Tango::DevFailed exception!");
            }

            if (inc_vpos)
                ++vpos;
		}
	}

    event_consumer->map_modification_lock.writerOut();
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::confirm_subscription
//
// description :
//		Confirm event subscription for all events coming from a specified event channel (Device server)
//
// argument :
//		in :
//			- event_consumer : The ZMQ event consumer object
//			- ipos : Iterator on the EventChannel map
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::confirm_subscription(ZmqEventConsumer *event_consumer,
														map<string,EventChannelStruct>::iterator &ipos)
{
	vector<string> cmd_params;
	vector<map<string,EventCallBackStruct>::difference_type> vd;
	std::map<std::string,EventCallBackStruct>::iterator epos;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); ++epos)
	{
		if (epos->second.channel_name == ipos->first )
		{
			try
			{

//
// lock the callback
//

				epos->second.callback_monitor->get_monitor();

				if (ipos->second.channel_type == ZMQ)
				{
					cmd_params.push_back(epos->second.device->dev_name());
					cmd_params.push_back(epos->second.obj_name);
					cmd_params.push_back(epos->second.event_name);

					vd.push_back(distance(event_consumer->event_callback_map.begin(),epos));
				}
				else
				{
					DeviceData subscriber_in;
					vector<string> subscriber_info;
					subscriber_info.push_back(epos->second.device->dev_name());
					subscriber_info.push_back(epos->second.obj_name);
					subscriber_info.push_back("subscribe");
					subscriber_info.push_back(epos->second.event_name);
					subscriber_in << subscriber_info;

					ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);

					time_t ti = time(NULL);
					ipos->second.last_subscribed = ti;
					epos->second.last_subscribed = ti;
				}
				epos->second.callback_monitor->rel_monitor();
			}
			catch (...)
			{
				epos->second.callback_monitor->rel_monitor();
			}
		}
	}

	if (ipos->second.channel_type == ZMQ && cmd_params.empty() == false)
	{
		try
		{
			DeviceData sub_cmd_in;
			sub_cmd_in << cmd_params;

			ipos->second.adm_device_proxy->command_inout("EventConfirmSubscription",sub_cmd_in);

			time_t ti = time(NULL);
			ipos->second.last_subscribed = ti;
			for (unsigned int loop = 0;loop < vd.size();++loop)
			{
				epos = event_consumer->event_callback_map.begin();
				advance(epos,vd[loop]);

				epos->second.callback_monitor->get_monitor();
				epos->second.last_subscribed = ti;
				epos->second.callback_monitor->rel_monitor();
			}
		}
		catch (Tango::DevFailed &e)
		{
			string reason(e.errors[0].reason.in());
			if (reason == API_CommandNotFound)
			{

//
// We are connected to a Tango 8 server which do not implement the EventConfirmSubscription command
// Send confirmation the old way
//

				time_t ti = time(NULL);
				ipos->second.last_subscribed = ti;

				for (unsigned int loop = 0;loop < vd.size();++loop)
				{
					DeviceData subscriber_in;
					vector<string> subscriber_info;
					subscriber_info.push_back(cmd_params[(loop * 3)]);
					subscriber_info.push_back(cmd_params[(loop * 3) + 1]);
					subscriber_info.push_back("subscribe");
					subscriber_info.push_back(cmd_params[(loop * 3) + 2]);
					subscriber_info.push_back("0");
					subscriber_in << subscriber_info;

					try
					{
						ipos->second.adm_device_proxy->command_inout("ZmqEventSubscriptionChange",subscriber_in);
					}
					catch(...) {}

					epos = event_consumer->event_callback_map.begin();
					advance(epos,vd[loop]);

					epos->second.callback_monitor->get_monitor();
					epos->second.last_subscribed = ti;
					epos->second.callback_monitor->rel_monitor();
				}
			}
		}
		catch (...) {}

		cmd_params.clear();
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::main_reconnect
//
// description :
//		Main method executed to send error to the user callback or to reconnect the event
//
// argument :
//		in :
//			- event_consumer : The ZMQ event consumer object
//			- notifd_event_consumer : The notifd event consumer object
//			- epos : Iterator on the EventCallback map
//			- ipos : Iterator on the EventChannel map
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::main_reconnect(ZmqEventConsumer *event_consumer,
													map<string,EventCallBackStruct>::iterator &epos,
													map<string,EventChannelStruct>::iterator &ipos)
{

//
// First, try to reconnect
//
		DeviceData dd;
		bool zmq_reco = reconnect_to_zmq_channel(ipos,event_consumer,dd);
		if ( zmq_reco )
			ipos->second.event_system_failed = false;
		else
			ipos->second.event_system_failed = true;

		if (ipos->second.event_system_failed == false)
		{
			reconnect_to_zmq_event(ipos,event_consumer,dd);
		}

	Tango::DevErrorList errors(1);

	errors.length(1);
	errors[0].severity = Tango::ERR;
	errors[0].origin = Tango::string_dup("EventConsumer::KeepAliveThread()");
	errors[0].reason = Tango::string_dup("API_EventTimeout");
	errors[0].desc = Tango::string_dup("Event channel is not responding anymore, maybe the server or event system is down");
	DeviceAttribute *dev_attr = NULL;
	AttributeInfoEx *dev_attr_conf = NULL;
	DevicePipe *dev_pipe = NULL;

	for (epos = event_consumer->event_callback_map.begin(); epos != event_consumer->event_callback_map.end(); ++epos)
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

			try
			{
				epos->second.callback_monitor->get_monitor();

				if (need_reconnect == true)
				{
					if ((ipos->second.channel_type == NOTIFD) && (epos->second.filter_ok == false))
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

					string::size_type pos = event_name.find(EVENT_COMPAT);
					if (pos != string::npos)
						event_name.erase(0,EVENT_COMPAT_IDL5_SIZE);
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
						FwdAttrConfEventData *event_data = new FwdAttrConfEventData(epos->second.device,
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
								ApiUtil *au = ApiUtil::instance();
								stringstream ss;

								ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of  " << epos->first;
								au->print_error_message(ss.str().c_str());
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
								ApiUtil *au = ApiUtil::instance();
								stringstream ss;

								ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
								au->print_error_message(ss.str().c_str());
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
					else if (event_name == EventName[INTERFACE_CHANGE_EVENT])
					{
						DevIntrChangeEventData *event_data = new DevIntrChangeEventData(epos->second.device,
																			event_name,domain_name,
																			(CommandInfoList *)NULL,
																			(AttributeInfoListEx *)NULL,
																			false,errors);
						// if a callback method was specified, call it!
						if (callback != NULL )
						{
							try
							{
								callback->push_event(event_data);
							}
							catch (...)
							{
								ApiUtil *au = ApiUtil::instance();
								stringstream ss;

								ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
								au->print_error_message(ss.str().c_str());
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
					else if (event_name == EventName[PIPE_EVENT])
					{
						PipeEventData *event_data = new PipeEventData(epos->second.device,
										domain_name,
										event_name,
										dev_pipe,
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
								ApiUtil *au = ApiUtil::instance();
								stringstream ss;

								ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
								au->print_error_message(ss.str().c_str());
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
						FwdEventData *event_data = new FwdEventData(epos->second.device,
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
								ApiUtil *au = ApiUtil::instance();
								stringstream ss;

								ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
								au->print_error_message(ss.str().c_str());
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

				if ( ipos->second.event_system_failed == false )
				{
					re_subscribe_after_reconnect(event_consumer,epos,ipos,domain_name);
				}
				// release callback monitor
				epos->second.callback_monitor->rel_monitor();
			}
			catch (...)
			{
				ApiUtil *au = ApiUtil::instance();
				stringstream ss;

				ss << "EventConsumerKeepAliveThread::run_undetached() timeout on callback monitor of " << epos->first;
				au->print_error_message(ss.str().c_str());
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::re_subscribe_after_reconnect
//
// description :
//		Re subscribe to the event after a successfull reconnection to the event channel (device server)
//
// argument :
//		in :
//			- event_consumer : The ZMQ event consumer object
//			- notifd_event_consumer : The notifd event consumer object
//			- epos : Iterator on the EventCallback map
//			- ipos : Iterator on the EventChannel map
//			- domain_name :
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::re_subscribe_after_reconnect(ZmqEventConsumer *event_consumer,
												  				map<string,EventCallBackStruct>::iterator &epos,
																map<string,EventChannelStruct>::iterator &ipos,
																string &domain_name)
{
	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(epos->second.device->dev_name());
	subscriber_info.push_back(epos->second.obj_name);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(epos->second.event_name);
	if (ipos->second.channel_type == ZMQ)
		subscriber_info.push_back("0");
	subscriber_in << subscriber_info;

	bool ds_failed = false;

	try
	{
		if (ipos->second.channel_type == ZMQ)
			ipos->second.adm_device_proxy->command_inout("ZmqEventSubscriptionChange",subscriber_in);
		else
			ipos->second.adm_device_proxy->command_inout("EventSubscriptionChange",subscriber_in);

		ipos->second.heartbeat_skipped = false;
		ipos->second.last_subscribed = time(NULL);
	}
	catch (...) {ds_failed = true;}

	if (ds_failed == false)
	{

//
// Push an event with the value just read from the re-connected server
// NOT NEEDED for the Data Ready event
//

		vector<EventSubscribeStruct>:: iterator esspos;

		string ev_name(epos->second.event_name);
		string::size_type pos = ev_name.find(EVENT_COMPAT);
		if (pos != string::npos)
			ev_name.erase(0,EVENT_COMPAT_IDL5_SIZE);

		if ((ev_name == "change") ||
			 (ev_name == "quality") ||
			 (ev_name == "archive") ||
			 (ev_name == "user_event"))
		{
//
// For attribute data event
//

			DeviceAttribute *da = NULL;
			DevErrorList err;
			err.length(0);

			bool old_transp = epos->second.device->get_transparency_reconnection();
			epos->second.device->set_transparency_reconnection(true);

			try
			{
				da = new DeviceAttribute();
				*da = epos->second.device->read_attribute(epos->second.obj_name.c_str());

                if (da->has_failed() == true)
                    err = da->get_err_stack();
//
// The reconnection worked fine. The heartbeat should come back now, when the notifd has not closed the connection.
// Increase the counter to detect when the heartbeat is not coming back.
//

				if (ipos->second.channel_type == NOTIFD)
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
				FwdEventData *event_data;

				if (cb_ctr != cb_nb)
				{
					da_copy = new DeviceAttribute();
					da_copy->deep_copy(*da);

					event_data = new FwdEventData(epos->second.device,
									domain_name,
									epos->second.event_name,
									da_copy,
									err);
				}
				else
				{
					event_data = new FwdEventData(epos->second.device,
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
						ApiUtil *au = ApiUtil::instance();
						stringstream ss;

						ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
						au->print_error_message(ss.str().c_str());
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

		else if (epos->second.event_name.find(CONF_TYPE_EVENT) != string::npos)
		{

//
// For attribute configuration event
//

			AttributeInfoEx *aie = NULL;
			DevErrorList err;
			err.length(0);
			string prefix;
			if (epos->second.device->get_from_env_var() == false)
                {
                    prefix = "tango://";
                    if (epos->second.device->is_dbase_used() == false)
                        prefix = prefix + epos->second.device->get_dev_host() + ':' + epos->second.device->get_dev_port() + '/';
                    else
                        prefix = prefix + epos->second.device->get_db_host() + ':' + epos->second.device->get_db_port() + '/';
                }
                else
                {
                    prefix = event_consumer->env_var_fqdn_prefix[0];
                }

			string dom_name = prefix + epos->second.device->dev_name();
            if (epos->second.device->is_dbase_used() == false)
                dom_name = dom_name + MODIFIER_DBASE_NO;
			dom_name = dom_name + "/" + epos->second.obj_name;

			bool old_transp = epos->second.device->get_transparency_reconnection();
			epos->second.device->set_transparency_reconnection(true);

			try
			{
				aie = new AttributeInfoEx();
				*aie = epos->second.device->get_attribute_config(epos->second.obj_name);

//
// The reconnection worked fine. The heartbeat should come back now, when the notifd has not closed the connection.
// Increase the counter to detect when the heartbeat is not coming back.
//

				if (ipos->second.channel_type == NOTIFD)
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
				FwdAttrConfEventData *event_data;
				string ev_name(epos->second.event_name);
				string::size_type pos = ev_name.find(EVENT_COMPAT);
				if (pos != string::npos)
					ev_name.erase(0,EVENT_COMPAT_IDL5_SIZE);

				if (cb_ctr != cb_nb)
				{
					aie_copy = new AttributeInfoEx;
					*aie_copy = *aie;
					event_data = new FwdAttrConfEventData(epos->second.device,
									dom_name,
									ev_name,
									aie_copy,
									err);
				}
				else
				{
					event_data = new FwdAttrConfEventData(epos->second.device,
									dom_name,
									ev_name,
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
						ApiUtil *au = ApiUtil::instance();
						stringstream ss;

						ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
						au->print_error_message(ss.str().c_str());
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
		else if (epos->second.event_name == EventName[INTERFACE_CHANGE_EVENT])
		{

//
// For device interface change event
//

			AttributeInfoListEx *aie = Tango_nullptr;
			CommandInfoList *cil = Tango_nullptr;
			DevErrorList err;
			err.length(0);
			string prefix = event_consumer->env_var_fqdn_prefix[0];
			string dom_name = prefix + epos->second.device->dev_name();

			bool old_transp = epos->second.device->get_transparency_reconnection();
			epos->second.device->set_transparency_reconnection(true);

			try
			{
				aie = epos->second.device->attribute_list_query_ex();
				cil = epos->second.device->command_list_query();
			}
			catch (DevFailed &e)
			{
				delete aie;
				aie = Tango_nullptr;
				delete cil;
				cil = Tango_nullptr;

				err = e.errors;
			}
			epos->second.device->set_transparency_reconnection(old_transp);

			unsigned int cb_nb = epos->second.callback_list.size();
			unsigned int cb_ctr = 0;
			CommandInfoList *cil_copy = Tango_nullptr;
			AttributeInfoListEx *aie_copy = Tango_nullptr;

			for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
			{
				cb_ctr++;
				DevIntrChangeEventData *event_data;
				string ev_name(epos->second.event_name);

				if (cb_ctr != cb_nb)
				{
					aie_copy = new AttributeInfoListEx;
					*aie_copy = *aie;
					cil_copy = new CommandInfoList;
					*cil_copy = *cil;
					event_data = new DevIntrChangeEventData(epos->second.device,
									ev_name,dom_name,
									cil_copy,aie_copy,true,
									err);
				}
				else
				{
					event_data = new DevIntrChangeEventData(epos->second.device,
									ev_name,dom_name,
									cil,aie,true,
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
						ApiUtil *au = ApiUtil::instance();
						stringstream ss;

						ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
						au->print_error_message(ss.str().c_str());
					}

					if (cb_ctr != cb_nb)
					{
						delete aie_copy;
						delete cil_copy;
					}
					else
					{
						delete aie;
						delete cil;
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
		else if (epos->second.event_name == EventName[PIPE_EVENT])
		{

//
// For pipe event
//

			DevicePipe *dp = Tango_nullptr;
			DevErrorList err;
			err.length(0);

			bool old_transp = epos->second.device->get_transparency_reconnection();
			epos->second.device->set_transparency_reconnection(true);

			try
			{
				dp = new DevicePipe();
				*dp = epos->second.device->read_pipe(epos->second.obj_name);
			}
			catch (DevFailed &e)
			{
				err = e.errors;
			}
			epos->second.device->set_transparency_reconnection(old_transp);

			unsigned int cb_nb = epos->second.callback_list.size();
			unsigned int cb_ctr = 0;

			DevicePipe *dp_copy = NULL;

			for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
			{
				cb_ctr++;
				PipeEventData *event_data;

				if (cb_ctr != cb_nb)
				{
					dp_copy = new DevicePipe();
					*dp_copy = *dp;

					event_data = new PipeEventData(epos->second.device,
									domain_name,
									epos->second.event_name,
									dp_copy,
									err);
				}
				else
				{
					event_data = new PipeEventData(epos->second.device,
									domain_name,
									epos->second.event_name,
									dp,
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
						ApiUtil *au = ApiUtil::instance();
						stringstream ss;

						ss << "EventConsumerKeepAliveThread::run_undetached() exception in callback method of " << epos->first;
						au->print_error_message(ss.str().c_str());
					}

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
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumerKeepAliveThread::stateless_subscription_failed
//
// description :
//
// argument :
//		in :
//			- vpos : An iterator in the vector of non-connected event for the concerned event
//			- e : The received exception
//			- now : When the exception was received
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumerKeepAliveThread::stateless_subscription_failed(vector<EventNotConnected>::iterator &vpos,DevFailed &e,time_t &now)
{

//
// Subscribe has not worked, try again in the next hearbeat period
//

    vpos->last_heartbeat = now;

//
// The event can still not be connected. Send the return error message as event to the client application.
// Push an event with the error message!
//

    DevErrorList err;
    err.length(0);
    string domain_name = vpos->prefix + vpos->device->dev_name();
    if (vpos->event_name != EventName[INTERFACE_CHANGE_EVENT])
		domain_name = domain_name + "/" + vpos->attribute;
    err = e.errors;

//
// For attribute data event
//

    if ((vpos->event_name == "change") ||
        (vpos->event_name == "quality") ||
        (vpos->event_name == "archive") ||
		(vpos->event_name == "periodic") ||
        (vpos->event_name == "user_event"))
    {

        DeviceAttribute *da = NULL;
        FwdEventData *event_data = new FwdEventData(vpos->device,
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
				ApiUtil *au = ApiUtil::instance();
				stringstream ss;

				ss << "EventConsumerKeepAliveThread::stateless_subscription_failed() exception in callback method of " << domain_name;
				au->print_error_message(ss.str().c_str());
            }

            delete event_data;
        }

//
// no callback method, the event has to be instered into the event queue
//
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
        AttributeInfoEx *aie = NULL;
        AttrConfEventData *event_data = new AttrConfEventData(vpos->device,
                                                domain_name,
                                                vpos->event_name,
                                                aie,
                                                err);

//
// If a callback method was specified, call it!
//

        if (vpos->callback != NULL )
        {
            try
            {
                vpos->callback->push_event(event_data);
            }
            catch (...)
            {
				ApiUtil *au = ApiUtil::instance();
				stringstream ss;

				ss << "EventConsumerKeepAliveThread::stateless_subscription_failed() exception in callback method of " << domain_name;
				au->print_error_message(ss.str().c_str());
            }

            //event_data->attr_conf = NULL;
            delete event_data;
        }

//
// No calback method, the event has to be inserted into the event queue
//

        else
        {
            vpos->ev_queue->insert_event(event_data);
        }
    }
    else if (vpos->event_name == DATA_READY_TYPE_EVENT)
    {
        DataReadyEventData *event_data = new DataReadyEventData(vpos->device,NULL,vpos->event_name,err);

//
// If a callback method was specified, call it!
//

        if (vpos->callback != NULL )
        {
            try
            {
                vpos->callback->push_event(event_data);
            }
            catch (...)
            {
				ApiUtil *au = ApiUtil::instance();
				stringstream ss;

				ss << "EventConsumerKeepAliveThread::stateless_subscription_failed() exception in callback method of " << domain_name;
				au->print_error_message(ss.str().c_str());
            }
            delete event_data;
        }

//
// No callback method, the event has to be inserted into the event queue
//
        else
            vpos->ev_queue->insert_event(event_data);
    }
    else if (vpos->event_name == EventName[INTERFACE_CHANGE_EVENT])
    {
        DevIntrChangeEventData *event_data = new DevIntrChangeEventData(vpos->device,vpos->event_name,
																		domain_name,(CommandInfoList *)NULL,
																		(AttributeInfoListEx *)NULL,false,err);

//
// If a callback method was specified, call it!
//

        if (vpos->callback != NULL )
        {
            try
            {
                vpos->callback->push_event(event_data);
            }
            catch (...)
            {
				ApiUtil *au = ApiUtil::instance();
				stringstream ss;

				ss << "EventConsumerKeepAliveThread::stateless_subscription_failed() exception in callback method of " << domain_name;
				au->print_error_message(ss.str().c_str());
            }
            delete event_data;
        }

//
// No callback method, the event has to be inserted into the event queue
//
        else
            vpos->ev_queue->insert_event(event_data);
    }
    else if (vpos->event_name == EventName[PIPE_EVENT])
    {
        PipeEventData *event_data = new PipeEventData(vpos->device,domain_name,vpos->event_name,
																		(DevicePipe *)NULL,err);

//
// If a callback method was specified, call it!
//

        if (vpos->callback != NULL )
        {
            try
            {
                vpos->callback->push_event(event_data);
            }
            catch (...)
            {
				ApiUtil *au = ApiUtil::instance();
				stringstream ss;

				ss << "EventConsumerKeepAliveThread::stateless_subscription_failed() exception in callback method of " << domain_name;
				au->print_error_message(ss.str().c_str());
            }
            delete event_data;
        }

//
// No callback method, the event has to be inserted into the event queue
//
        else
            vpos->ev_queue->insert_event(event_data);
    }
}

} /* End of Tango namespace */


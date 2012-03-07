static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
///
///  file 	zmqeventconsumer.cpp
///
///		C++ classes for implementing the event consumer
///		singleton class when used with zmq
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
#include <assert.h>

#include <omniORB4/internal/giopStream.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace CORBA;

namespace Tango {


ZmqEventConsumer *ZmqEventConsumer::_instance = NULL;
//omni_mutex EventConsumer::ev_consumer_inst_mutex;


/************************************************************************/
/*		       															*/
/* 			ZmqEventConsumer class 					    				*/
/*			----------------											*/
/*		       															*/
/************************************************************************/

ZmqEventConsumer::ZmqEventConsumer(ApiUtil *ptr) : EventConsumer(ptr),omni_thread((void *)ptr),zmq_context(1)
{
	cout3 << "calling Tango::ZmqEventConsumer::ZmqEventConsumer() \n";

	_instance = this;

//
// Initialize the var references
//

    av = new AttributeValue();
    av3 = new AttributeValue_3();
    ac2 = new AttributeConfig_2();
    ac3 = new AttributeConfig_3();
    adr = new AttDataReady();
    del = new DevErrorList();

	start_undetached();
}

ZmqEventConsumer *ZmqEventConsumer::create()
{
	omni_mutex_lock guard(ev_consumer_inst_mutex);

//
// check if the ZmqEventConsumer singleton exists, if so return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// ZmqEventConsumer singleton does not exist, create it
//

	ApiUtil *ptr =  ApiUtil::instance();
	return new ZmqEventConsumer(ptr);
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::run_undetached()
//
// description :    Main method for the ZMQ event system reciving thread
//
//-----------------------------------------------------------------------------

void *ZmqEventConsumer::run_undetached(TANGO_UNUSED(void *arg))
{

    int linger = 0;

//
// Create the subscriber socket used to receive heartbeats coming from different DS
// This socket subscribe to everything because dedicated publishers are used to
// send the heartbeat events. This socket will be connected to all needed publishers
//

    heartbeat_sub_sock = new zmq::socket_t(zmq_context,ZMQ_SUB);
    heartbeat_sub_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

//
// Create the subscriber socket used to receive events coming from different DS
// This socket subscribe to everything because dedicated publishers are used to
// send the heartbeat events. This socket will be connected to all needed publishers
//

    event_sub_sock = new zmq::socket_t(zmq_context,ZMQ_SUB);
    event_sub_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

//
// Create the control socket (REQ/REP pattern) and binds it
//

    control_sock = new zmq::socket_t(zmq_context,ZMQ_REP);
    control_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));
    control_sock->bind(CTRL_SOCK_ENDPOINT);

//
// Initialize poll set
//

    zmq::pollitem_t *items = new zmq::pollitem_t [MAX_SOCKET_SUB];
    int nb_poll_item = 3;

    items[0].socket = *control_sock;
    items[1].socket = *heartbeat_sub_sock;
    items[2].socket = *event_sub_sock;

    for (int loop = 0;loop < nb_poll_item;loop++)
    {
        items[loop].fd = 0;
        items[loop].events = ZMQ_POLLIN;
        items[loop].revents = 0;
    }

//
// Enter the infinite loop
//

    while(1)
    {
        zmq::message_t received_event_name,received_endian;
        zmq::message_t received_call,received_event_data;
        zmq::message_t received_ctrl;

//
// Init messages used by multicast event
//

        zmq_msg_t mcast_received_event_name;
        zmq_msg_t mcast_received_endian;
        zmq_msg_t mcast_received_call;
        zmq_msg_t mcast_received_event_data;

//
// Wait for message
// The try/catch is usefull when the process is running under gdb control
//

        try
        {
            zmq::poll(items,nb_poll_item,-1);
//cout << "Awaken !!!!!!!!" << endl;
        }
        catch(zmq::error_t &e)
        {
            if (e.num() == EINTR)
                continue;
        }


//
// Something received by the heartbeat socket ?
//

        if (items[1].revents & ZMQ_POLLIN)
        {
//cout << "For the heartbeat socket" << endl;
            heartbeat_sub_sock->recv(&received_event_name);
            heartbeat_sub_sock->recv(&received_endian);
            heartbeat_sub_sock->recv(&received_call);

            process_heartbeat(received_event_name,received_endian,received_call);

            items[1].revents = 0;
        }

//
// Something received by the control socket?
//

        if (items[0].revents & ZMQ_POLLIN)
        {
//cout << "For the control socket" << endl;
            control_sock->recv(&received_ctrl);

            string ret_str;
            bool ret = false;

            try
            {
                ret = process_ctrl(received_ctrl,items,nb_poll_item);
                ret_str = "OK";
            }
            catch (zmq::error_t &e)
            {
                ret_str = e.what();
            }
            catch (Tango::DevFailed &e)
            {
                ret_str = e.errors[0].desc;
            }

            zmq::message_t reply(ret_str.size());
            ::memcpy((void *)reply.data(),ret_str.data(),ret_str.size());
            control_sock->send(reply);

            if (ret == true)
            {
                delete heartbeat_sub_sock;
                delete control_sock;

                break;
            }
            items[0].revents = 0;
        }

//
// Something received by the event socket (TCP transport)?
//

        if (items[2].revents & ZMQ_POLLIN)
        {
            event_sub_sock->recv(&received_event_name);
            event_sub_sock->recv(&received_endian);
            event_sub_sock->recv(&received_call);
            event_sub_sock->recv(&received_event_data);

            process_event(received_event_name,received_endian,received_call,received_event_data);

            items[2].revents = 0;
        }

//
// Something received by the event socket (mcast transport)?
// What is stored in the zmq::pollitem_t structure is the real C
// zmq socket, not the C++ zmq::socket_t class instance
// There is no way to create a zmq::socket_t class instance
// from a C zmq socket.
// Only in 11/2012, some C++11 move ctor/assignment operator
// has been added to the socket_t class allowing creation
// of zmq::socket_t class from C zmq socket
// Nevertheless, today (11/2012), it is still not official
//

        for (int loop = 3;loop < nb_poll_item;loop++)
        {
            if (items[loop].revents & ZMQ_POLLIN)
            {
                zmq_msg_init(&mcast_received_event_name);
                zmq_msg_init(&mcast_received_endian);
                zmq_msg_init(&mcast_received_call);
                zmq_msg_init(&mcast_received_event_data);

                zmq_recvmsg(items[loop].socket,&mcast_received_event_name,0);
                zmq_recvmsg(items[loop].socket,&mcast_received_endian,0);
                zmq_recvmsg(items[loop].socket,&mcast_received_call,0);
                zmq_recvmsg(items[loop].socket,&mcast_received_event_data,0);

                process_event(mcast_received_event_name,mcast_received_endian,mcast_received_call,mcast_received_event_data);

                zmq_msg_close(&mcast_received_event_name);
                zmq_msg_close(&mcast_received_endian);
                zmq_msg_close(&mcast_received_call);
                zmq_msg_close(&mcast_received_event_data);

                items[loop].revents = 0;
            }
        }

    }

	return (void *)NULL;
}


//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::process_heartbeat()
//
// description : 	Process execution when a message has been received by the
//                  heartbeat socket
//
// args: in : - received_event_name : The full event name
//            - received_endian : The sender endianess
//            - received_call : The call informations (oid - method name...)
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::process_heartbeat(zmq::message_t &received_event_name,zmq::message_t &received_endian,zmq::message_t &received_call)
{
//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A heartbeat message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Event name" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_event_name.data(),received_event_name.size());

        {
            omniORB::logger log;
            log << "ZMQ: Endianess" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_endian.data(),received_endian.size());

        {
            omniORB::logger log;
            log << "ZMQ: Call info" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_call.data(),received_call.size());
    }

//
// Extract data from messages
//

    unsigned char endian = ((char *)received_endian.data())[0];
    string event_name((char *)received_event_name.data(),(size_t)received_event_name.size());

    cdrMemoryStream call_info((char *)received_call.data(),(size_t)received_call.size());
    call_info.setByteSwapFlag(endian);

    ZmqCallInfo_var c_info_var = new ZmqCallInfo;
    (ZmqCallInfo &)c_info_var <<= call_info;

//
// Call the heartbeat method
//

    push_heartbeat_event(event_name);

}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::process_event()
//
// description : 	Process execution when a message has been received by the
//                  event socket
//
// args: in : - received_event_name : The full event name
//            - received_endian : The sender endianess
//            - received_call : The call informations (oid - method name...)
//            - event_data : The event data !
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::process_event(zmq::message_t &received_event_name,zmq::message_t &received_endian,zmq::message_t &received_call,zmq::message_t &event_data)
{
//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A event message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Event name" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_event_name.data(),received_event_name.size());

        {
            omniORB::logger log;
            log << "ZMQ: Endianess" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_endian.data(),received_endian.size());

        {
            omniORB::logger log;
            log << "ZMQ: Call info" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_call.data(),received_call.size());

        {
            omniORB::logger log;
            log << "ZMQ: Event data" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)event_data.data(),event_data.size());
    }

//
// Extract data from messages
//

    const ZmqCallInfo *receiv_call;

    unsigned char endian = ((char *)received_endian.data())[0];
    string event_name((char *)received_event_name.data(),(size_t)received_event_name.size());

    cdrMemoryStream call_info((char *)received_call.data(),(size_t)received_call.size());
    call_info.setByteSwapFlag(endian);

    ZmqCallInfo_var c_info_var = new ZmqCallInfo;
    (ZmqCallInfo &)c_info_var <<= call_info;
    receiv_call = &c_info_var.in();

//
// Call the event method
//

    push_zmq_event(event_name,endian,event_data,receiv_call->call_is_except,receiv_call->ctr);

}


void ZmqEventConsumer::process_event(zmq_msg_t &received_event_name,zmq_msg_t &received_endian,zmq_msg_t &received_call,zmq_msg_t &event_data)
{
//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A event message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Event name" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)zmq_msg_data(&received_event_name),zmq_msg_size(&received_event_name));

        {
            omniORB::logger log;
            log << "ZMQ: Endianess" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)zmq_msg_data(&received_endian),zmq_msg_size(&received_endian));

        {
            omniORB::logger log;
            log << "ZMQ: Call info" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)zmq_msg_data(&received_call),zmq_msg_size(&received_call));

        {
            omniORB::logger log;
            log << "ZMQ: Event data" << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)zmq_msg_data(&event_data),zmq_msg_size(&event_data));
    }

//
// Extract data from messages
//

    const ZmqCallInfo *receiv_call;

    unsigned char endian = ((char *)zmq_msg_data(&received_endian))[0];
    string event_name((char *)zmq_msg_data(&received_event_name),zmq_msg_size(&received_event_name));

    cdrMemoryStream call_info((char *)zmq_msg_data(&received_call),zmq_msg_size(&received_call));
    call_info.setByteSwapFlag(endian);

    ZmqCallInfo_var c_info_var = new ZmqCallInfo;
    (ZmqCallInfo &)c_info_var <<= call_info;
    receiv_call = &c_info_var.in();

//
// Call the event method
//

    zmq::message_t cpp_ev_data;
    cpp_ev_data.rebuild(zmq_msg_data(&event_data),zmq_msg_size(&event_data),NULL);

    push_zmq_event(event_name,endian,cpp_ev_data,receiv_call->call_is_except,receiv_call->ctr);

}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::process_ctrl()
//
// description : 	Process task when something has been received by the control
//                  socket
//
// args: - received_ctrl :  The received data
//       - poll_list :      The ZMQ poll ed item list
//       - poll_nb :        The item number in previous list
//
// This method returns true if the calling thread has to exit (because a
// ZMQ_END command has been received). Otherwise, it returns false
//
//-----------------------------------------------------------------------------

bool ZmqEventConsumer::process_ctrl(zmq::message_t &received_ctrl,zmq::pollitem_t *poll_list,int &poll_nb)
{
    bool ret = false;

//
// For debug and logging purposes
//

    if (omniORB::trace(20))
    {
        omniORB::logger log;
        log << "ZMQ: A control message has been received" << '\n';
    }
    if (omniORB::trace(30))
    {
        {
            omniORB::logger log;
            log << "ZMQ: Control data " << '\n';
        }
        omni::giopStream::dumpbuf((unsigned char *)received_ctrl.data(),received_ctrl.size());
    }

//
// Extract cmd code from messages
//

    const char *tmp_ptr = (const char *)received_ctrl.data();
    char cmd_code = tmp_ptr[0];

//
// Process each command
//

    switch (cmd_code)
    {
        case ZMQ_END:
        {
            ret = true;
        }
        break;

        case ZMQ_CONNECT_HEARTBEAT:
        {
//
// First extract the endpoint and the event name from received buffer
//

            const char *endpoint = &(tmp_ptr[1]);
            int start = ::strlen(endpoint) + 2;
            const char *event_name = &(tmp_ptr[start]);

//
// Connect the heartbeat socket to the new publisher
//

            bool connect_heart = false;

            if (connected_heartbeat.empty() == false)
            {
                vector<string>::iterator pos;
                pos = find(connected_heartbeat.begin(),connected_heartbeat.end(),endpoint);
                if (pos == connected_heartbeat.end())
                    connect_heart = true;
            }
            else
                connect_heart = true;

            if (connect_heart == true)
            {
                heartbeat_sub_sock->connect(endpoint);
                connected_heartbeat.push_back(endpoint);
            }


//
// Subscribe to the new heartbeat event
//

            heartbeat_sub_sock->setsockopt(ZMQ_SUBSCRIBE,event_name,::strlen(event_name));
        }
        break;

        case ZMQ_DISCONNECT_HEARTBEAT:
        {
//
// Get event name
//

            const char *event_name = &(tmp_ptr[1]);

//
// Unsubscribe this event from the heartbeat socket
//

            heartbeat_sub_sock->setsockopt(ZMQ_UNSUBSCRIBE,event_name,::strlen(event_name));
        }
        break;

        case ZMQ_CONNECT_EVENT:
        {
//
// First extract the endpoint and the event name from received buffer
//

            const char *endpoint = &(tmp_ptr[1]);
            int start = ::strlen(endpoint) + 2;
            const char *event_name = &(tmp_ptr[start]);
            start = start + ::strlen(event_name) + 1;
            Tango::DevLong sub_hwm;
            ::memcpy(&sub_hwm,&(tmp_ptr[start]),sizeof(Tango::DevLong));

//
// Connect the socket to the publisher
//

            bool connect_pub = false;

            if (connected_pub.empty() == false)
            {
                vector<string>::iterator pos;
                pos = find(connected_pub.begin(),connected_pub.end(),endpoint);
                if (pos == connected_pub.end())
                    connect_pub = true;
            }
            else
                connect_pub = true;

            if (connect_pub == true)
            {
                event_sub_sock->setsockopt(ZMQ_RCVHWM,&sub_hwm,sizeof(sub_hwm));

                event_sub_sock->connect(endpoint);
                connected_pub.push_back(endpoint);
            }

//
// Subscribe to the new event
//

            event_sub_sock->setsockopt(ZMQ_SUBSCRIBE,event_name,::strlen(event_name));
        }
        break;

        case ZMQ_DISCONNECT_EVENT:
        {

//
// Get event name
//

            const char *event_name = &(tmp_ptr[1]);
            string ev_name(event_name);

//
// Check if it is a multicast event
//

            bool mcast = false;

            map<string,zmq::socket_t *>::iterator pos;
            if (event_mcast.empty() != true)
            {
                pos = event_mcast.find(ev_name);
                if (pos != event_mcast.end())
                    mcast = true;
            }

//
// Unsubscribe this event from the socket
//

            if (mcast == false)
                event_sub_sock->setsockopt(ZMQ_UNSUBSCRIBE,event_name,::strlen(event_name));
            else
            {
                delete pos->second;
                event_mcast.erase(pos);
            }
        }
        break;

        case ZMQ_CONNECT_MCAST_EVENT:
        {
//
// First extract the endpoint and the event name from received buffer
//

            const char *endpoint = &(tmp_ptr[1]);
            int start = ::strlen(endpoint) + 2;
            const char *event_name = &(tmp_ptr[start]);
            start = start + ::strlen(event_name) + 1;
            Tango::DevLong sub_hwm,rate,ivl;
            ::memcpy(&sub_hwm,&(tmp_ptr[start]),sizeof(Tango::DevLong));
            start = start + sizeof(Tango::DevLong);
            ::memcpy(&rate,&(tmp_ptr[start]),sizeof(Tango::DevLong));
            start = start + sizeof(Tango::DevLong);
            ::memcpy(&ivl,&(tmp_ptr[start]),sizeof(Tango::DevLong));

//
// Connect the socket to the publisher
//

            bool created_sub = false;
            string ev_name(event_name);
            map<string,zmq::socket_t *>::iterator pos;

            if (event_mcast.empty() == false)
            {
                pos = event_mcast.find(ev_name);
                if (pos != event_mcast.end())
                    created_sub = true;
            }

            if (created_sub == false)
            {

//
// Check that we are not at the socket high limit
//

                if (poll_nb == MAX_SOCKET_SUB)
                {
                    Except::throw_exception((const char *)"DServer_Events",
                                            (const char *)"Array to store sockets for zmq poll() call is already full",
                                            (const char *)"ZmqEventConsumer::process_control");
                }

//
// Create the socket
//

                zmq::socket_t *tmp_sock = new zmq::socket_t(zmq_context,ZMQ_SUB);

//
// Set socket rate, ivl linger and hwm
//

                int local_rate = rate;
                tmp_sock->setsockopt(ZMQ_RATE,&local_rate,sizeof(local_rate));

                int local_ivl = ivl;
                tmp_sock->setsockopt(ZMQ_RECOVERY_IVL,&local_ivl,sizeof(local_ivl));

                int linger = 0;
                tmp_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

                tmp_sock->setsockopt(ZMQ_RCVHWM,&sub_hwm,sizeof(sub_hwm));

//
// Connect the socket
//

                tmp_sock->connect(endpoint);

//
// Subscribe to the new event
//

                tmp_sock->setsockopt(ZMQ_SUBSCRIBE,event_name,::strlen(event_name));

//
// Store socket in map
//

                if (event_mcast.insert(make_pair(ev_name,tmp_sock)).second == false)
                {
                    delete tmp_sock;
                    cerr << "Error while inserting pair<event name,mcast socket> in map!" << endl;

                    Except::throw_exception((const char *)"DServer_Events",
                                            (const char *)"Error while inserting pair<event name,multicast socket> in map",
                                            (const char *)"ZmqEventConsumer::process_control");
                }

//
// Update poll item list
//

                poll_list[poll_nb].socket = *tmp_sock;
                poll_list[poll_nb].fd = 0;
                poll_list[poll_nb].events = ZMQ_POLLIN;
                poll_list[poll_nb].revents = 0;

                poll_nb++;
            }
        }
        break;

        case ZMQ_DELAY_EVENT:
        {
            old_poll_nb = poll_nb;
            poll_nb = 1;
        }
        break;

        case ZMQ_RELEASE_EVENT:
        {
            poll_nb = old_poll_nb;
        }
        break;

        default:
            cerr << "ZMQ main thread: Received an unknown command code from control socket!" << endl;
        break;
    }

    return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::cleanup_EventChannel_map()
//
// description : 	Method to destroy the DeviceProxy objects
//					stored in the EventChannel map
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::cleanup_EventChannel_map()
{
	EvChanIte evt_it;

    for (evt_it = channel_map.begin(); evt_it != channel_map.end(); ++evt_it)
    {
        EventChannelStruct &evt_ch = evt_it->second;
        if ((evt_ch.channel_type == ZMQ) && (evt_ch.adm_device_proxy != NULL))
        {
            AutoTangoMonitor _mon(evt_ch.channel_monitor);

//
// Release the connection to the device server administration device
//

            delete evt_ch.adm_device_proxy;
            evt_ch.adm_device_proxy = NULL;
        }
    }

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;

    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is only a command code
//

        char buffer[10];
        int length = 0;

        buffer[length] = ZMQ_END;
        length++;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);
        sender.send(send_data);

        sender.recv(&reply);
    }
    catch(zmq::error_t) {}
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::connect_event_channel()
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

void ZmqEventConsumer::connect_event_channel(string &channel_name,TANGO_UNUSED(Database *db),bool reconnect,DeviceData &dd)
{

//
// Extract server command result
//

    const DevVarLongStringArray *ev_svr_data;
    dd >> ev_svr_data;

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;
    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);

//
// In case this thread runs before the main ZMQ thread, it is possible
// to call connect before the main ZMQ thread has binded its socket.
// In such a case, error code is set to ECONNREFUSED.
// If this happens, give the main ZMQ thread a chance to run and
// retry the connect call
// I have tried with a yield call but it still failed in some cases
// (when running the DS with a file as database  for instance)
// Replace the yield with a 10 mS sleep !!!
//

        try
        {
            sender.connect(CTRL_SOCK_ENDPOINT);
        }
        catch (zmq::error_t &e)
        {
            if (e.num() == ECONNREFUSED)
            {
#ifndef _TG_WINDOWS_
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 10000000;

                nanosleep(&ts,NULL);
#else
                Sleep(10);
#endif
                sender.connect(CTRL_SOCK_ENDPOINT);
            }
            else
                throw;
        }

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        buffer[length] = ZMQ_CONNECT_HEARTBEAT;
        length++;

        ::strcpy(&(buffer[length]),ev_svr_data->svalue[0].in());
        length = length + ::strlen(ev_svr_data->svalue[0].in()) + 1;

        string sub(channel_name);
        sub = sub + '.' + HEARTBEAT_EVENT_NAME;

        ::strcpy(&(buffer[length]),sub.c_str());
        length = length + sub.size() + 1;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);

        sender.send(send_data);

        sender.recv(&reply);
    }
    catch(zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ error code = " << e.num() << "\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_channel");
    }

//
// Any error during ZMQ main thread socket operations?
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to create connection to event channel!\n";
        o << "Error while trying to connect or subscribe the heartbeat ZMQ socket to the new publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_channel");
    }


//
// Init (or create) EventChannelStruct
//

	EvChanIte evt_it = channel_map.end();
	if (reconnect == true)
	{
		evt_it = channel_map.find(channel_name);
		EventChannelStruct &evt_ch = evt_it->second;
		evt_ch.last_heartbeat = time(NULL);
		evt_ch.heartbeat_skipped = false;
		evt_ch.notifd_failed = false;
	}
	else
	{
		EventChannelStruct new_event_channel_struct;

		new_event_channel_struct.last_heartbeat = time(NULL);
		new_event_channel_struct.heartbeat_skipped = false;
		new_event_channel_struct.adm_device_proxy = NULL;
		// create a channel monitor
		new_event_channel_struct.channel_monitor = new TangoMonitor();
		// set the timeout for the channel monitor to 500ms not to block the event consumer for to long.
		new_event_channel_struct.channel_monitor->timeout(500);

		new_event_channel_struct.notifd_failed = false;
		set_channel_type(new_event_channel_struct);

		channel_map[channel_name] = new_event_channel_struct;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::disconnect_event_channel()
//
// description :    Disconnect to the event channel
//                  This means that the process should not receive
//                  the heartbeat event for this channel
//                  It will be filtered out by ZMQ
//
// Args in : - channel name : The event channel name (DS admin name)
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::disconnect_event_channel(string &channel_name)
{
    string unsub(channel_name);
    unsub = unsub + '.' + HEARTBEAT_EVENT_NAME;

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;

    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        buffer[length] = ZMQ_DISCONNECT_HEARTBEAT;
        length++;

        ::strcpy(&(buffer[length]),unsub.c_str());
        length = length + unsub.size() + 1;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);
        sender.send(send_data);

        sender.recv(&reply);
    }
    catch (zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to disconnect from the event channel!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event_channel");
    }

//
// In case of error returned by the main ZMQ thread
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to disconnect from event channel!\n";
        o << "Error while trying to unsubscribe the heartbeat ZMQ socket from the channel heartbeat publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event_channel");
    }

}


//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::disconnect_event()
//
// description :    Disconnect to the event
//                  This means that the process should not receive
//                  the event any more
//                  It will be filtered out by ZMQ
//
// Args in : - event_name : The event name
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::disconnect_event(string &event_name)
{

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;

    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint
// and the event name
//

        char buffer[1024];
        int length = 0;

        buffer[length] = ZMQ_DISCONNECT_EVENT;
        length++;

        ::strcpy(&(buffer[length]),event_name.c_str());
        length = length + event_name.size() + 1;

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);
        sender.send(send_data);

        sender.recv(&reply);
    }
    catch (zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to disconnect from event!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event");
    }

//
// In case of error returned by the main ZMQ thread
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to disconnect from event!\n";
        o << "Error while trying to unsubscribe the heartbeat ZMQ socket from the channel heartbeat publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::disconnect_event");
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::connect_event_system()
//
// description :    Connect to the real event (change, archive,...)
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

void ZmqEventConsumer::connect_event_system(string &device_name,string &att_name,string &event_name,TANGO_UNUSED(const vector<string> &filters),
                                            TANGO_UNUSED(EvChanIte &eve_it),TANGO_UNUSED(EventCallBackStruct &new_event_callback),
                                            DeviceData &dd)
{
//
// Build full event name
// Don't forget case of device in a DS using file as database
//

    string full_event_name;
    string::size_type pos;

    if ((pos = device_name.find(MODIFIER_DBASE_NO)) != string::npos)
    {
        full_event_name = device_name;
        string tmp = '/' + att_name;
        full_event_name.insert(pos,tmp);
        full_event_name = full_event_name + '.' + event_name;
    }
    else
        full_event_name = device_name + '/' + att_name + '.' + event_name;

//
// Extract server command result
//

    const DevVarLongStringArray *ev_svr_data;
    dd >> ev_svr_data;

//
// Create and connect the REQ socket used to send message to the
// ZMQ main thread
//

    zmq::message_t reply;
    try
    {
        zmq::socket_t sender(zmq_context,ZMQ_REQ);
        sender.connect(CTRL_SOCK_ENDPOINT);

//
// If the transport is multicast, add main IP interface address in endpoint
//

        bool mcast_transport = false;
        ApiUtil *au = ApiUtil::instance();

        string endpoint(ev_svr_data->svalue[1].in());
        if (endpoint.find(MCAST_PROT) != string::npos)
        {
            mcast_transport = true;

            vector<string> adrs;
            au->get_ip_from_if(adrs);

            for (unsigned int i = 0;i < adrs.size();++i)
            {
                if (adrs[i].find("127.") == 0)
                    continue;
                adrs[i] = adrs[i] + ';';
                string::size_type pos = endpoint.find('/');
                pos = pos + 2;
                endpoint.insert(pos,adrs[i]);
                break;
            }
        }

//
// Build message sent to ZMQ main thread
// In this case, this is the command code, the publisher endpoint,
// the event name and the sub hwm
//

        char buffer[1024];
        int length = 0;

        if (mcast_transport == true)
            buffer[length] = ZMQ_CONNECT_MCAST_EVENT;
        else
            buffer[length] = ZMQ_CONNECT_EVENT;
        length++;

        ::strcpy(&(buffer[length]),endpoint.c_str());
        length = length + endpoint.size() + 1;

        ::strcpy(&(buffer[length]),full_event_name.c_str());
        length = length + full_event_name.size() + 1;

        DevLong user_hwm = au->get_user_sub_hwm();
        if (user_hwm != -1)
            ::memcpy(&(buffer[length]),&(user_hwm),sizeof(Tango::DevLong));
        else
            ::memcpy(&(buffer[length]),&(ev_svr_data->lvalue[2]),sizeof(Tango::DevLong));
        length = length + sizeof(Tango::DevLong);

//
// In case of multicasting, add rate and ivl parameters
//

        if (mcast_transport == true)
        {
            ::memcpy(&(buffer[length]),&(ev_svr_data->lvalue[3]),sizeof(Tango::DevLong));
            length = length + sizeof(Tango::DevLong);

            ::memcpy(&(buffer[length]),&(ev_svr_data->lvalue[4]),sizeof(Tango::DevLong));
            length = length + sizeof(Tango::DevLong);
        }

//
// Send command to main ZMQ thread
//

        zmq::message_t send_data(length);
        ::memcpy(send_data.data(),buffer,length);

        sender.send(send_data);

        sender.recv(&reply);
    }
    catch(zmq::error_t &e)
    {
        TangoSys_OMemStream o;

        o << "Failed to create connection to event!\n";
        o << "Error while communicating with the ZMQ main thread\n";
        o << "ZMQ message: " << e.what() << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_system");
    }

//
// Any error during ZMQ main thread socket operations?
//

    if (reply.size() != 2)
    {
        char err_mess[512];
        ::memcpy(err_mess,reply.data(),reply.size());
        err_mess[reply.size()] = '\0';

        TangoSys_OMemStream o;

        o << "Failed to create connection to event!\n";
        o << "Error while trying to connect or subscribe the event ZMQ socket to the new publisher\n";
        o << "ZMQ message: " << err_mess << ends;

        Except::throw_exception((const char *)"API_ZmqFailed",
                        o.str(),
                        (const char *)"ZmqEventConsumer::connect_event_system");
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::push_heartbeat_event()
//
// description :    Method called when the heartbeat event is received
//                  This method retrieve the channel entry in the channel_map
//                  and update the last heartbeat date.
//
// argument(s) : in : - ev_name : The fully qualifed event name
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::push_heartbeat_event(string &ev_name)
{
//
// Remove ".heartbeat" at the end of event name
//

    string::size_type pos = ev_name.find(".heartbeat");
    if (pos == string::npos)
    {
        return;
    }
    ev_name.erase(pos);

//
// Only reading from the maps
//

    map_modification_lock.readerIn();

    std::map<std::string,EventChannelStruct>::iterator ipos;
    ipos = channel_map.find(ev_name);

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
            cerr << "Tango::ZmqEventConsumer::push_heartbeat_event() timeout on channel monitor of " << ipos->first << endl;
        }
    }
    else
    {
        cerr << "No entry in channel map for heartbeat " << ev_name << "!" << endl;
    }

    map_modification_lock.readerOut();
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventConsumer::push_zmq_event()
//
// description :    Method called when the event is received
//                  This method retrieve the channel entry in the channel_map
//                  and update the last heartbeat date.
//
// argument(s) : in : - ev_name : The fully qualifed event name
//                    - endian : The sender host endianess
//                    - event_data : The event data still in a ZMQ message
//                    - error : Flag set to true if the event data is an error
//                              stack
//                    - ctr : Event counter as received from server
//
//-----------------------------------------------------------------------------

void ZmqEventConsumer::push_zmq_event(string &ev_name,unsigned char endian,zmq::message_t &event_data,bool error,const DevULong &ds_ctr)
{

    map_modification_lock.readerIn();
    bool map_lock = true;

//
// Search for entry within the event_callback map using the event
// name received in the event
//

    map<std::string,EventCallBackStruct>::iterator ipos;

    ipos = event_callback_map.find(ev_name);
    if (ipos != event_callback_map.end())
    {
        const AttributeValue *attr_value = NULL;
        const AttributeValue_3 *attr_value_3 = NULL;
        const ZmqAttributeValue_4 *z_attr_value_4 = NULL;
        const AttributeConfig_2 *attr_conf_2 = NULL;
        const AttributeConfig_3 *attr_conf_3 = NULL;
        const AttDataReady *att_ready = NULL;
        const DevErrorList *err_ptr;
        DevErrorList errors;
        AttributeInfoEx *attr_info_ex = NULL;

        bool ev_attr_conf = false;
        bool ev_attr_ready = false;

        EventCallBackStruct &evt_cb = ipos->second;

//
// Miss some events?
// Due to LIBZMQ Bug 283, the first event after a process startup is sent two times
// with the same ctr value. Do not call the user callback for the second times.
//

        bool err_missed_event = false;
        DevLong missed_event = ds_ctr - evt_cb.ctr;

        if (missed_event >= 2)
        {
            err_missed_event = true;
        }
        else if (missed_event == 0)
        {
            map_modification_lock.readerOut();
            return;
        }


        evt_cb.ctr = ds_ctr;

//
// Get which type of event data has been received (from the event type)
//

        string::size_type pos = ev_name.rfind('.');
        string event_name = ev_name.substr(pos + 1);
        string att_name = ev_name.substr(0,pos);

        UserDataEventType data_type;

        if (event_name == CONF_TYPE_EVENT)
            data_type = ATT_CONF;
        else if (event_name == DATA_READY_TYPE_EVENT)
            data_type = ATT_READY;
        else
            data_type = ATT_VALUE;

//
// Unmarshal the event data
//

        long vers = 0;
        DeviceAttribute *dev_attr = NULL;

//
// For 64 bits data (double, long64 and ulong64), omniORB unmarshalling
// methods required that the 64 bits data are aligned on a 8 bytes
// memory address.
// ZMQ returned memory which is sometimes aligned on a 8 bytes boundary but
// not always (seems to depend on the host architecture)
// The attribute data transfert starts with the union discriminator
// (4 bytes), the elt nb (4 bytes) and the element themselves.
// This means 8 bytes before the real data.
// There is a trick here.
// The buffer is always transferred with an extra 4 bytes added at the beginning
// If the alignememnt is not correct (buffer aligned on a 8 bytes boundary
// and 64 bits data type), shift the whole buffer by 4 bytes erasing the
// additional 4 bytes sent.
//
// Note: The buffer is not correctly aligned if it is retruned on a
// 8 bytes boundary because we have the 4 extra bytes + 8 bytes for
// union discriminator + elt nb. This means 64 bits data not on a
// 8 bytes boundary
//

        char *data_ptr = (char *)event_data.data();
        size_t data_size = (size_t)event_data.size();

        bool data64 = false;
        if (data_type == ATT_VALUE && error == false)
        {
            int disc = ((int *)data_ptr)[1];
            if (disc == ATT_DOUBLE || disc == ATT_LONG64 || disc == ATT_ULONG64)
                data64 = true;
        }

        bool buffer_aligned64 = false;
        if (data64 == true)
        {
            if (((unsigned long)data_ptr & 0x7) == 0)
                buffer_aligned64 = true;
        }

//
// Shift buffer if required
//

        if (data64 == true && buffer_aligned64 == true)
        {
            int nb_loop = (data_size >> 2) - 1;
            int remaining = data_size & 0x3;

            if (omniORB::trace(30))
            {
                {
                    omniORB::logger log;
                    log << "ZMQ: Shifting received buffer!!!" << '\n';
                }
            }

            int *src,*dest;
            dest = (int *)data_ptr;
            src = dest + 1;
            for (int loop = 0;loop < nb_loop;++loop)
            {
                *dest = *src;
                ++dest;
                ++src;
            }

            for (int loop = 0;loop < remaining;++loop)
            {
                *dest = *src;
                ++dest;
                ++src;
            }

            data_size = data_size - 4;
        }
        else
        {
            data_ptr = data_ptr + sizeof(CORBA::Long);
            data_size = data_size - sizeof(CORBA::Long);
        }

        TangoCdrMemoryStream event_data_cdr(data_ptr,data_size);
        event_data_cdr.setByteSwapFlag(endian);

//
// Unmarshall the data
//

        if (error == true)
        {
            (DevErrorList &)del <<= event_data_cdr;
            err_ptr = &del.in();
            errors = *err_ptr;
        }
        else
        {
            switch (data_type)
            {
                case ATT_CONF:
                if (evt_cb.device_idl > 2)
                {
                    (AttributeConfig_3 &)ac3 <<= event_data_cdr;
                    attr_conf_3 = &ac3.in();
                    vers = 3;
                    attr_info_ex = new AttributeInfoEx();
                    *attr_info_ex = const_cast<AttributeConfig_3 *>(attr_conf_3);
                    ev_attr_conf = true;
                }
                else if (evt_cb.device_idl == 2)
                {
                    (AttributeConfig_2 &)ac2 <<= event_data_cdr;
                    attr_conf_2 = &ac2.in();
                    vers = 2;
                    attr_info_ex = new AttributeInfoEx();
                    *attr_info_ex = const_cast<AttributeConfig_2 *>(attr_conf_2);
                    ev_attr_conf = true;
                }
                break;

                case ATT_READY:
                (AttDataReady &)adr <<= event_data_cdr;
                att_ready = &adr.in();
                ev_attr_ready = true;
                break;

                case ATT_VALUE:
                if (evt_cb.device_idl > 3)
                {
                    zav4.operator<<=(event_data_cdr);
                    z_attr_value_4 = &zav4;
                    vers = 4;
                    dev_attr = new (DeviceAttribute);
                    attr_to_device(z_attr_value_4,dev_attr);
                }
                else if (evt_cb.device_idl == 3)
                {
                    (AttributeValue_3 &)av3 <<= event_data_cdr;
                    attr_value_3 = &av3.in();
                    vers = 3;
                    dev_attr = new (DeviceAttribute);
                    attr_to_device(attr_value,attr_value_3,vers,dev_attr);
                }
                else if (evt_cb.device_idl < 3)
                {
                    (AttributeValue &)av <<= event_data_cdr;
                    attr_value = &av.in();
                    vers = 2;
                    dev_attr = new (DeviceAttribute);
                    attr_to_device(attr_value,attr_value_3,vers,dev_attr);
                }
                break;
            }
        }

        EventData *missed_event_data = NULL;
        AttrConfEventData *missed_conf_event_data = NULL;
        DataReadyEventData *missed_ready_event_data = NULL;

        AutoTangoMonitor _mon(evt_cb.callback_monitor);

        try
        {

//
// In case we have missed some event, prepare srtucture to send to callback
// to inform user of this bad behavior
//

            if (err_missed_event == true)
            {
                DevErrorList missed_errors;
                missed_errors.length(1);
                missed_errors[0].reason = "API_MissedEvents";
                missed_errors[0].origin = "ZmqEventConsumer::push_zmq_event()";
                missed_errors[0].desc = "Missed some events! Zmq queue has reached HWM?";
                missed_errors[0].severity = ERR;

                if ((ev_attr_conf == false) && (ev_attr_ready == false))
                    missed_event_data = new EventData (event_callback_map[ev_name].device,
                                                    att_name,event_name,NULL,missed_errors);
                else if (ev_attr_ready == false)
                    missed_conf_event_data = new AttrConfEventData(event_callback_map[ev_name].device,
                                                                att_name,event_name,
                                                                NULL,missed_errors);
                else
                    missed_ready_event_data = new DataReadyEventData(event_callback_map[ev_name].device,
                                                                NULL,event_name,missed_errors);
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

//
// In case we have several callbacks on the same event
// or if the event has to be stored in a queue, copy
// the event data (Event data are in the ZMQ message)
//

                        if (cb_ctr != cb_nb)
                        {
                            DeviceAttribute *dev_attr_copy = NULL;
                            if (dev_attr != NULL || (callback == NULL && vers == 4))
                            {
                                dev_attr_copy = new DeviceAttribute();
                                dev_attr_copy->deep_copy(*dev_attr);
                            }

                            event_data = new EventData(event_callback_map[ev_name].device,
                                                                att_name,
                                                                event_name,
                                                                dev_attr_copy,
                                                                errors);
                        }
                        else
                        {
                            if (callback == NULL && vers == 4)
                            {
                                DeviceAttribute *dev_attr_copy = NULL;
                                if (dev_attr != NULL)
                                {
                                    dev_attr_copy = new DeviceAttribute();
                                    dev_attr_copy->deep_copy(*dev_attr);
                                }

                                event_data = new EventData(event_callback_map[ev_name].device,
                                                                att_name,
                                                                event_name,
                                                                dev_attr_copy,
                                                                errors);

                            }
                            else
                                event_data = new EventData (event_callback_map[ev_name].device,
                                                              att_name,
                                                              event_name,
                                                              dev_attr,
                                                              errors);
                        }

//
// If a callback method was specified, call it!
//

                        if (callback != NULL )
                        {
                            try
                            {
                                if (err_missed_event == true)
                                    callback->push_event(missed_event_data);
                                callback->push_event(event_data);
                            }
                            catch (...)
                            {
                                cerr << "Tango::ZmqEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
                            }

                            delete event_data;
                        }

//
// No calback method, the event has to be inserted into the event queue
//

                        else
                        {
                            if (err_missed_event == true)
                                ev_queue->insert_event(missed_event_data);
                            ev_queue->insert_event(event_data);
                            if (vers == 4 && cb_ctr == cb_nb)
                                delete dev_attr;
                        }
                    }
                    else if (ev_attr_ready == false)
                    {
                        AttrConfEventData *event_data;

                        if (cb_ctr != cb_nb)
                        {
                            AttributeInfoEx *attr_info_copy = new AttributeInfoEx();
                            *attr_info_copy = *attr_info_ex;
                            event_data = new AttrConfEventData(event_callback_map[ev_name].device,
                                                              att_name,
                                                              event_name,
                                                              attr_info_copy,
                                                              errors);
                        }
                        else
                        {
                            event_data = new AttrConfEventData(event_callback_map[ev_name].device,
                                                              att_name,
                                                              event_name,
                                                              attr_info_ex,
                                                              errors);
                        }


                        // if callback methods were specified, call them!
                        if (callback != NULL )
                        {
                            try
                            {
                                if (err_missed_event == true)
                                    callback->push_event(missed_conf_event_data);
                                callback->push_event(event_data);
                            }
                            catch (...)
                            {
                                cerr << "Tango::ZmqEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
                            }

                            delete event_data;
                        }

                        // no calback method, the event has to be instered
                        // into the event queue
                        else
                        {
                           if (err_missed_event == true)
                                ev_queue->insert_event(missed_conf_event_data);
                            ev_queue->insert_event(event_data);
                        }
                    }
                    else
                    {
                        DataReadyEventData *event_data = new DataReadyEventData(event_callback_map[ev_name].device,
                                                                const_cast<AttDataReady *>(att_ready),event_name,errors);
                        // if a callback method was specified, call it!
                        if (callback != NULL )
                        {
                            try
                            {
                                if (err_missed_event == true)
                                    callback->push_event(missed_ready_event_data);
                                callback->push_event(event_data);
                            }
                            catch (...)
                            {
                                cerr << "Tango::ZmqEventConsumer::push_structured_event() exception in callback method of " << ipos->first << endl;
                            }
                            delete event_data;
                        }

                        // no calback method, the event has to be instered
                        // into the event queue
                        else
                        {
                            if (err_missed_event == true)
                                ev_queue->insert_event(missed_ready_event_data);
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

            delete missed_event_data;
            delete missed_conf_event_data;
            delete missed_ready_event_data;
        }
        catch (...)
        {
            delete missed_event_data;
            delete missed_conf_event_data;
            delete missed_ready_event_data;

            // free the map lock if not already done
            if ( map_lock == true )
            {
                map_modification_lock.readerOut();
            }

            cerr << "Tango::ZmqEventConsumer::push_structured_event() timeout on callback monitor of " << ipos->first << endl;
        }
    }
    else
    {
        cerr << "Event " << ev_name << " not found in event callback map !!!" << endl;
		// even if nothing was found in the map, free the lock
        map_modification_lock.readerOut();
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqAttrValUnion::operator<<=()
//
// description :    Write our own unmarshalling method. The omniORB one
//                  allocate memory and copy data. We already have memory
//                  allocated in the ZMQ message. No need to allocate once
//                  more and to copy data.
//                  We are doinng this only for attribute data.
//                  For the remaining, keep using omniORB stuff
//
// argument(s) : in :
//
//-----------------------------------------------------------------------------

void ZmqAttrValUnion::operator<<= (TangoCdrMemoryStream& _n)
{
    char *data_ptr = (char *)_n.bufPtr();

//
// Get union discriminator from cdr and if data type is string or device_state
// let omniORB do its stuff. Don't forget to rewind memory
// ptr before returning to omniORB
//

    AttributeDataType _pd__d = ATT_BOOL;
    (AttributeDataType&)_pd__d <<= _n;

    if (_pd__d == ATT_STRING || _pd__d == DEVICE_STATE)
    {
        _n.rewindPtrs();
        AttrValUnion::operator<<=(_n);
    }
    else
    {

//
// Get data length from cdr
//

        _CORBA_ULong length;
        length <<= _n;

        if (length == 0)
            return;

//
// Get att data depending on type
//

        switch (_pd__d)
        {
            case ATT_SHORT:
            {
                init_seq<DevShort,DevVarShortArray>(data_ptr,length,_n);
            }
            break;

            case ATT_DOUBLE:
            {
                init_seq<DevDouble,DevVarDoubleArray>(data_ptr,length,_n);
            }
            break;

            case ATT_FLOAT:
            {
                init_seq<DevFloat,DevVarFloatArray>(data_ptr,length,_n);
            }
            break;

            case ATT_USHORT:
            {
                init_seq<DevUShort,DevVarUShortArray>(data_ptr,length,_n);
            }
            break;

            case ATT_BOOL:
            {
                init_seq<DevBoolean,DevVarBooleanArray>(data_ptr,length,_n);
            }
            break;

            case ATT_LONG:
            {
                init_seq<DevLong,DevVarLongArray>(data_ptr,length,_n);
            }
            break;

            case ATT_LONG64:
            {
                init_seq<DevLong64,DevVarLong64Array>(data_ptr,length,_n);
            }
            break;

            case ATT_ULONG:
            {
                init_seq<DevULong,DevVarULongArray>(data_ptr,length,_n);
            }
            break;

            case ATT_ULONG64:
            {
                init_seq<DevULong64,DevVarULong64Array>(data_ptr,length,_n);
            }
            break;

            case ATT_UCHAR:
            {
                init_seq<DevUChar,DevVarUCharArray>(data_ptr,length,_n);
            }
            break;

            case ATT_STATE:
            {
                init_seq<DevState,DevVarStateArray>(data_ptr,length,_n);
            }
            break;

//
// We have special cases for DevEncoded (a structure)
// and NO_DATA
//

            case ATT_ENCODED:
            {
                DevVarEncodedArray dummy_seq;
                encoded_att_value(dummy_seq);

                DevVarEncodedArray &dvea = encoded_att_value();
                dvea.length(length);

                for (_CORBA_ULong i = 0;i < length;i++)
                {
                    dvea[i].encoded_format = _n.unmarshalString(0);
                    _CORBA_ULong seq_length;
                    seq_length <<= _n;
                    _CORBA_Octet *ptr = (_CORBA_Octet *)(data_ptr + _n.currentInputPtr());
                    dvea[i].encoded_data.replace(seq_length,seq_length,ptr,false);
                    _n.tango_get_octet_array((seq_length * sizeof(_CORBA_Octet)));
                }
            }
            break;

            case NO_DATA:
            {
                DevBoolean bo;
                bo = _n.unmarshalBoolean();

                union_no_data(bo);
               _n.tango_get_octet_array((length * sizeof(DevBoolean)));
            }
            break;

            default:
            assert(false);
        }
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqAttributeValue_4::operator<<=()
//
// description :
//
// argument(s) : in :
//
//-----------------------------------------------------------------------------

void Tango::ZmqAttributeValue_4::operator<<= (TangoCdrMemoryStream &_n)
{
  (ZmqAttrValUnion&)zvalue <<= _n;
  (AttrQuality&)quality <<= _n;
  (AttrDataFormat&)data_format <<= _n;
  (TimeVal&)time <<= _n;
  name = _n.unmarshalString(0);
  (AttributeDim&)r_dim <<= _n;
  (AttributeDim&)w_dim <<= _n;
  (DevErrorList&)err_list <<= _n;
}

//+----------------------------------------------------------------------------
//
// method : 		DelayEvent::DelayEvent
//
// description : A class to ask the ZMQ main thread to stop receiving
//               external event. This is necessary to prevent a possible
//               deadlock which could happen if an event is received while
//               a user is calling subscribe or unsubscribe event
//
// argument(s) : in : ec : Event consumer pointer
//
//-----------------------------------------------------------------------------

DelayEvent::DelayEvent(EventConsumer *ec):released(false),eve_con(NULL)
{
    string str;
    ec->get_subscription_command_name(str);

    if (str[0] == 'Z')
    {
        eve_con = static_cast<ZmqEventConsumer *>(ec);
        zmq::message_t reply;

        try
        {
            zmq::socket_t sender(eve_con->zmq_context,ZMQ_REQ);

//
// In case this thread runs before the main ZMQ thread, it is possible
// to call connect before the main ZMQ thread has binded its socket.
// In such a case, error code is set to ECONNREFUSED.
// If this happens, give the main ZMQ thread a chance to run and
// retry the connect call
// I have tried with a yield call but it still failed in some cases
// (when running the DS with a file as database  for instance)
// Replace the yield with a 15 mS sleep !!!
//

            try
            {
                sender.connect(CTRL_SOCK_ENDPOINT);
            }
            catch (zmq::error_t &e)
            {
                if (e.num() == ECONNREFUSED)
                {
#ifndef _TG_WINDOWS_
                    struct timespec ts;
                    ts.tv_sec = 0;
                    ts.tv_nsec = 15000000;

                    nanosleep(&ts,NULL);
#else
                    Sleep(20);
#endif
                    sender.connect(CTRL_SOCK_ENDPOINT);
                }
                else
                    throw;
            }

//
// Build message sent to ZMQ main thread
// In this case, this is only a command code
//

            char buffer[10];
            int length = 0;

            buffer[length] = ZMQ_DELAY_EVENT;
            length++;

//
// Send command to main ZMQ thread
//

            zmq::message_t send_data(length);
            ::memcpy(send_data.data(),buffer,length);
            sender.send(send_data);

            sender.recv(&reply);
        }
        catch (zmq::error_t &e)
        {
            TangoSys_OMemStream o;

            o << "Failed to delay event!\n";
            o << "Error while communicating with the ZMQ main thread\n";
            o << "ZMQ message: " << e.what() << ends;

            Except::throw_exception((const char *)"API_ZmqFailed",
                            o.str(),
                            (const char *)"DelayEvent::DelayEvent");
        }

//
// In case of error returned by the main ZMQ thread
//

        if (reply.size() != 2)
        {
            char err_mess[512];
            ::memcpy(err_mess,reply.data(),reply.size());
            err_mess[reply.size()] = '\0';

            TangoSys_OMemStream o;

            o << "Failed to delay events!\n";
            o << "Error while asking the ZMQ thread to delay events\n";
            o << "ZMQ message: " << err_mess << ends;

            Except::throw_exception((const char *)"API_ZmqFailed",
                            o.str(),
                            (const char *)"DelayEvent::DelayEvent");
        }
    }
}


DelayEvent::~DelayEvent()
{
    if (released == false)
        release();
}

void DelayEvent::release()
{
    if (eve_con != NULL)
    {
        zmq::message_t reply;

        try
        {
            zmq::socket_t sender(eve_con->zmq_context,ZMQ_REQ);
            sender.connect(CTRL_SOCK_ENDPOINT);

//
// Build message sent to ZMQ main thread
// In this case, this is only a command code
//

            char buffer[10];
            int length = 0;

            buffer[length] = ZMQ_RELEASE_EVENT;
            length++;

//
// Send command to main ZMQ thread
//

            zmq::message_t send_data(length);
            ::memcpy(send_data.data(),buffer,length);
            sender.send(send_data);

            sender.recv(&reply);
            released = true;
        }
        catch (zmq::error_t &e)
        {
            TangoSys_OMemStream o;

            o << "Failed to delay event!\n";
            o << "Error while communicating with the ZMQ main thread\n";
            o << "ZMQ message: " << e.what() << ends;

            Except::throw_exception((const char *)"API_ZmqFailed",
                            o.str(),
                            (const char *)"DelayEvent::release");
        }

//
// In case of error returned by the main ZMQ thread
//

        if (reply.size() != 2)
        {
            char err_mess[512];
            ::memcpy(err_mess,reply.data(),reply.size());
            err_mess[reply.size()] = '\0';

            TangoSys_OMemStream o;

            o << "Failed to release event!\n";
            o << "Error while trying to ask the ZMQ thread to release events\n";
            o << "ZMQ message: " << err_mess << ends;

            Except::throw_exception((const char *)"API_ZmqFailed",
                            o.str(),
                            (const char *)"DelayEvent::release");
        }
    }
}

} /* End of Tango namespace */


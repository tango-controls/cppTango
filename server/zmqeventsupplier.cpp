static const char *RcsId = "$Id$";

////////////////////////////////////////////////////////////////////////////////
//
//  file 	zmqeventsupplier.cpp
//
//		C++ classes for implementing the event server and client
//		singleton classes - ZmqEventSupplier.
//		This class is used to send events from the server
//		to the client(s) when zmq is used to transport the events
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

#include <omniORB4/internal/giopStream.h>

#include <iterator>

using namespace CORBA;

namespace Tango {

ZmqEventSupplier *ZmqEventSupplier::_instance = NULL;


/************************************************************************/
/*		       															*/
/* 			ZmqEventSupplier class 					    				*/
/*			----------------											*/
/*		       															*/
/************************************************************************/


ZmqEventSupplier::ZmqEventSupplier(Util *tg):EventSupplier(tg),zmq_context(1),event_pub_sock(NULL),double_send(false),double_send_heartbeat(false)
{
	_instance = this;

//
// Create the Publisher socket for heartbeat event and bind it
// If the user has specified one IP address on the command line,
// re-use it in the endpoint
//

    heartbeat_pub_sock = new zmq::socket_t(zmq_context,ZMQ_PUB);

    int linger = 0;
    heartbeat_pub_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

    heartbeat_endpoint = "tcp://";

    string &specified_ip = tg->get_specified_ip();

    if (specified_ip.empty() == false)
    {
        heartbeat_endpoint = heartbeat_endpoint + specified_ip + ':';
        ip_specified = true;
        user_ip = specified_ip;
    }
    else
    {
        heartbeat_endpoint = heartbeat_endpoint + "*:";
        ip_specified = false;
    }

//
// Bind the publisher socket to one ephemeral port
//

    tango_bind(heartbeat_pub_sock,heartbeat_endpoint);

//
// If needed, replace * by host IP address in endpoint string
//

    if (specified_ip.empty() == true)
    {
        ApiUtil *au = ApiUtil::instance();
        vector<string> adrs;

        au->get_ip_from_if(adrs);

        string::size_type pos = heartbeat_endpoint.find('*');
        if (adrs.size() > 1)
        {
            for (unsigned int i = 0;i < adrs.size();++i)
            {
                string::size_type start;
                if ((start = adrs[i].find("127.")) == 0)
                    continue;
                heartbeat_endpoint.replace(pos,1,adrs[i]);
                host_ip = adrs[i];
                break;
            }
        }
        else
        {
           heartbeat_endpoint.replace(pos,1,adrs[0]);
           host_ip = adrs[0];
        }
    }

//
// Find out the host endianness and
// create the zmq message used to pass it
//

    host_endian = test_endian();

    endian_mess.rebuild(1);
    memcpy(endian_mess.data(),&host_endian,1);

    endian_mess_2.copy(&endian_mess);

//
// Init heartbeat call info
// Leave the OID and method name un-initialized
// Marshall the structure into CORBA CDR
//

    heartbeat_call.version = ZMQ_EVENT_PROT_VERSION;
    heartbeat_call.call_is_except = false;

    heartbeat_call >>= heartbeat_call_cdr;

//
// Create some ZMQ messages from the already created memory buffer in CDR
//

    heartbeat_call_mess.rebuild(heartbeat_call_cdr.bufSize());
    memcpy(heartbeat_call_mess.data(),heartbeat_call_cdr.bufPtr(),heartbeat_call_cdr.bufSize());

    heartbeat_call_mess_2.copy(&heartbeat_call_mess);

//
// Start to init the event name used for the DS heartbeat event
//

    heartbeat_event_name = fqdn_prefix;
    heartbeat_event_name = heartbeat_event_name + "dserver/";
    heartbeat_name_init = false;
}


ZmqEventSupplier *ZmqEventSupplier::create(Util *tg)
{
	cout4 << "calling Tango::ZmqEventSupplier::create() \n";

//
// Does the ZmqEventSupplier singleton exist already ? if so simply return it
//

	if (_instance != NULL)
	{
		return _instance;
	}

//
// ZmqEventSupplier singleton does not exist, create it
//

	ZmqEventSupplier *_event_supplier = new ZmqEventSupplier(tg);

	return _event_supplier;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::tango_bind()
//
// description : 	Choose a free port to bind ZMQ socket
//
// argument : in : - sock : The ZMQ socket
//                 - endpoint : The beginning of the ZMQ endpoint
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::tango_bind(zmq::socket_t *sock,string &endpoint)
{
    stringstream ss;
    string base_endpoint(endpoint);
    string tmp_endpoint;

    int port;
    for (port = EPHEMERAL_PORT_BEGIN; port < EPHEMERAL_PORT_END; port++)
    {
        ss << port;
        tmp_endpoint = base_endpoint + ss.str();

        if (zmq_bind(*sock, tmp_endpoint.c_str()) == 0)
        {
            break;
        }
        ss.str("");
    }

    if (port == EPHEMERAL_PORT_END)
    {
        EventSystemExcept::throw_exception((const char*)"API_ZmqInitFailed",
                        (const char*)"Can't bind the ZMQ socket. All port used!",
                        (const char*)"ZmqEventSupplier::tango_bind()");
    }
    endpoint = tmp_endpoint;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::test_endian()
//
// description : 	Get the host endianness
//
// This method returns the host endianness
//      0 -> Big endian
//      1 -> Little endian
//
//-----------------------------------------------------------------------------

unsigned char ZmqEventSupplier::test_endian()
{
    int test_var = 1;
	unsigned char *cptr = (unsigned char*)&test_var;
    return (!(cptr[0] == 0));
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::create_event_socket()
//
// description : 	Create and bind the publisher socket used to publish the
//                  real events
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::create_event_socket()
{

    if (event_pub_sock == NULL)
    {

//
// Create the Publisher socket for real events and bind it
// If the user has specified one IP address on the command line,
// re-use it in the endpoint
//

        event_pub_sock = new zmq::socket_t(zmq_context,ZMQ_PUB);
        int linger = 0;
        event_pub_sock->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

        event_endpoint = "tcp://";

        if (ip_specified == true)
        {
            event_endpoint = event_endpoint + user_ip + ':';
        }
        else
        {
            event_endpoint = event_endpoint + "*:";
        }

//
// Set a publisher HWM
//

        Tango::Util *tg = Tango::Util::instance();
        DServer *admin_dev = tg->get_dserver_device();

        int hwm = tg->get_user_pub_hwm();
        if (hwm == -1)
            hwm = admin_dev->zmq_pub_event_hwm;

        event_pub_sock->setsockopt(ZMQ_SNDHWM,&hwm,sizeof(hwm));

//
// Bind the publisher socket to one ephemeral port
//

        tango_bind(event_pub_sock,event_endpoint);

//
// If needed, replace * by host IP address in enpoint string
//

        if (ip_specified == false)
        {
            event_endpoint.replace(6,1,host_ip);
        }
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::create_mcast_event_socket()
//
// description : 	Create and bind the publisher socket used to publish the
//                  real events when multicast transport is required
//
// argument : in :	mcast_data : The multicast addr and port (mcast_adr:port)
//                  ev_name : The event name (dev_name/attr_name.event_type)
//                  rate: The user defined PGM rate (O if undefined)
//                  local_call: True if the caller is on the same host
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::create_mcast_event_socket(string &mcast_data,string &ev_name,int rate,bool local_call)
{

    map<string,McastSocketPub>::iterator ite;

//
// If the event is already in the mcast event map, check if it is
// already used by local clients
//

    if ((ite = event_mcast.find(ev_name)) != event_mcast.end())
    {
        if (local_call == true)
        {
            if (ite->second.local_client == false)
            {
                create_event_socket();

                ite->second.local_client = true;
            }
        }
        else
        {
            if (ite->second.local_client == true)
            {
                create_mcast_socket(mcast_data,rate,ite->second);
            }
        }
    }
    else
    {

//
// New mcast event
//

        McastSocketPub ms;

        if (local_call == true)
        {
            create_event_socket();

            ms.pub_socket = NULL;
            ms.local_client = true;
        }
        else
        {

            create_mcast_socket(mcast_data,rate,ms);

            ms.local_client = false;
        }

//
// Insert element in map
//

        if (event_mcast.insert(make_pair(ev_name,ms)).second == false)
        {
            TangoSys_OMemStream o;
            o << "Can't insert multicast transport parameter for event ";
            o << ev_name << " in EventSupplier instance" << ends;

            Except::throw_exception((const char *)"DServer_Events",
                                o.str(),
                               (const char *)"ZmqEventSupplier::create_mcast_event_socket");
        }
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::create_mcast_socket()
//
// description : 	Create and bind the publisher socket used to publish the
//                  real events when multicast transport is required
//
// argument : in :	mcast_data : The multicast addr and port (mcast_adr:port)
//                  rate: The user defined PGM rate (O if undefined)
//                  ms: Reference to the structure to be stored in the macst map
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::create_mcast_socket(string &mcast_data,int rate,McastSocketPub &ms)
{

//
// Create the Publisher socket for real events and bind it
// If the user has specified one IP address on the command line,
// re-use it in the endpoint
//

    ms.pub_socket = new zmq::socket_t(zmq_context,ZMQ_PUB);

    ms.endpoint = MCAST_PROT;
    if (ip_specified == true)
    {
        ms.endpoint = ms.endpoint + user_ip + ';';
    }
    else
    {
        ApiUtil *au = ApiUtil::instance();
        vector<string> adrs;

        au->get_ip_from_if(adrs);

        for (unsigned int i = 0;i < adrs.size();++i)
        {
            if (adrs[i].find("127.") == 0)
                continue;
            ms.endpoint = ms.endpoint + adrs[i] + ';';
            break;
        }
    }
    ms.endpoint = ms.endpoint + mcast_data;

    int linger = 0;
    ms.pub_socket->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));

//
// Change multicast hops
//

    Tango::Util *tg = Tango::Util::instance();
    DServer *admin_dev = tg->get_dserver_device();

    int nb_hops = admin_dev->mcast_hops;
    ms.pub_socket->setsockopt(ZMQ_MULTICAST_HOPS,&nb_hops,sizeof(nb_hops));

//
// Change PGM rate to default value (80 Mbits/sec) or to user defined value
//

    int local_rate = rate;

    ms.pub_socket->setsockopt(ZMQ_RATE,&local_rate,sizeof(local_rate));

//
// Bind the publisher socket to the specified port
//

    if (zmq_bind(*(ms.pub_socket),ms.endpoint.c_str()) != 0)
    {
        TangoSys_OMemStream o;
        o << "Can't bind ZMQ socket with endpoint ";
        o << ms.endpoint;
        o << "\nZmq error: " << zmq_strerror(zmq_errno()) << ends;

        Except::throw_exception((const char *)"DServer_Events",
                                    o.str(),
                                   (const char *)"ZmqEventSupplier::create_mcast_event_socket");
    }

//
// The connection string returned to client does not need the host IP at all
//

    ms.endpoint = MCAST_PROT + mcast_data;

}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::is_event_mcast()
//
// description : 	This method checks if the event is already defined
//                  in the map of multicast event.
//
// argument : in :	ev_name : The event name (device/attr.event_type)
//
// This method returns true if the event is in the map and false otherwise
//-----------------------------------------------------------------------------

bool ZmqEventSupplier::is_event_mcast(string &ev_name)
{
    bool ret = false;

    if (event_mcast.find(ev_name) != event_mcast.end())
        ret = true;

    return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::get_mcast_event_endpoint()
//
// description : 	This method returns the multicast socket endpoint for the
//                  event passed as parameter
//
// argument : in :	event_name : The event name (device/attr.event_type)
//
// This method returns a reference to the enpoint string
//-----------------------------------------------------------------------------

string &ZmqEventSupplier::get_mcast_event_endpoint(string &ev_name)
{
    return event_mcast.find(ev_name)->second.endpoint;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::init_event_cptr()
//
// description : 	Method to initialize event counter for a specific event
//
// argument : in :	event_name : The event name (device/attr.event_type)
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::init_event_cptr(string &event_name)
{
    map<string,unsigned int>::iterator pos;

    pos = event_cptr.find(event_name);
    if (pos == event_cptr.end())
    {
        if (event_cptr.insert(make_pair(event_name,0)).second == false)
        {
            TangoSys_OMemStream o;
            o << "Can't insert event counter for event ";
            o << event_name << " in EventSupplier instance" << ends;

            Except::throw_exception((const char *)"DServer_Events",
                                    o.str(),
                                   (const char *)"ZmqEventSupplier::init_event_cptr");
        }
    }
    else
    {
        push_mutex.lock();
        pos->second = 0;
        push_mutex.unlock();
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::push_heartbeat_event()
//
// description : 	Method to push the hearbeat event
//
//-----------------------------------------------------------------------------

void ZmqEventSupplier::push_heartbeat_event()
{
	time_t delta_time;
	time_t now_time;

//
// Heartbeat - check wether a heartbeat event has been sent recently
// if not then send it. A heartbeat contains no data, it is used by the
// consumer to know that the supplier is still alive.
//

	Tango::Util *tg = Tango::Util::instance();
	DServer *adm_dev = tg->get_dserver_device();
	now_time = time(NULL);
	delta_time = now_time - adm_dev->last_heartbeat;
	cout3 << "ZmqEventSupplier::push_heartbeat_event(): delta time since last heartbeat " << delta_time << endl;

	if (heartbeat_name_init == false)
	{

//
// Build heartbeat name
// This is something like
//   tango://host:port/dserver/exec_name/inst_name.heartbeat when using DB
//   tango://host:port/dserver/exec_name/inst_name#dbase=no.heartbeat when using file as database
//

        heartbeat_event_name = heartbeat_event_name + adm_dev->get_full_name();
        if (Util::_FileDb == true)
            heartbeat_event_name = heartbeat_event_name + MODIFIER_DBASE_NO;
        heartbeat_event_name = heartbeat_event_name + ".heartbeat";
	    heartbeat_name_init = true;
	}

//
// We here compare delta_time to 9 and not to 10.
// This is necessary because, sometimes the polling thread is some
// milli second in advance. The computation here is done in seconds
// So, if the polling thread is in advance, delta_time computed in
// seconds will be 9 even if in reality it is 9,9
//

    int nb_event = 1;

	if (delta_time >= 9)
	{
		cout3 << "ZmqEventSupplier::push_heartbeat_event(): detected heartbeat event for " << heartbeat_event_name << endl;
		cout3 << "ZmqEventSupplier::push_heartbeat_event(): delta _time " << delta_time << endl;

        if (double_send_heartbeat == true)
        {
            nb_event = 2;
            double_send_heartbeat = false;
        }

        while (nb_event != 0)
        {

//
// Create zmq message
//

            zmq::message_t name_mess(heartbeat_event_name.size());
            memcpy(name_mess.data(),(void *)heartbeat_event_name.data(),heartbeat_event_name.size());

            bool endian_mess_sent = false;
            bool call_mess_sent = false;

            try
            {
//
// For debug and logging purposes
//

                if (nb_event == 1)
                {
                    if (omniORB::trace(20))
                    {
                        omniORB::logger log;
                        log << "ZMQ: Pushing some data" << '\n';
                    }
                    if (omniORB::trace(30))
                    {
                        {
                            omniORB::logger log;
                            log << "ZMQ: Event name" << '\n';
                        }
                        omni::giopStream::dumpbuf((unsigned char *)name_mess.data(),name_mess.size());

                        {
                            omniORB::logger log;
                            log << "ZMQ: Endianess" << '\n';
                        }
                        omni::giopStream::dumpbuf((unsigned char *)endian_mess.data(),endian_mess.size());

                        {
                            omniORB::logger log;
                            log << "ZMQ: Call info" << '\n';
                        }
                        omni::giopStream::dumpbuf((unsigned char *)heartbeat_call_mess.data(),heartbeat_call_mess.size());
                    }
                }

//
// Push the event
//

                heartbeat_pub_sock->send(name_mess,ZMQ_SNDMORE);
                heartbeat_pub_sock->send(endian_mess,ZMQ_SNDMORE);
                endian_mess_sent = true;
                heartbeat_pub_sock->send(heartbeat_call_mess,0);
                call_mess_sent = true;

//
// For reference counting on zmq messages which do not have a local scope
//

                endian_mess.copy(&endian_mess_2);
                heartbeat_call_mess.copy(&heartbeat_call_mess_2);

                nb_event--;
            }
            catch(...)
            {
                cout3 << "ZmqEventSupplier::push_heartbeat_event() failed !\n";
                if (endian_mess_sent == true)
                    endian_mess.copy(&endian_mess_2);
                if (call_mess_sent == true)
                    heartbeat_call_mess.copy(&heartbeat_call_mess_2);

                TangoSys_OMemStream o;
                o << "Can't push ZMQ heartbeat event for event ";
                o << heartbeat_event_name;
                if (zmq_errno() != 0)
                    o << "\nZmq error: " << zmq_strerror(zmq_errno()) << ends;
                else
                    o << ends;

                Except::throw_exception((const char *)"DServer_Events",
                                        o.str(),
                                       (const char *)"ZmqEventSupplier::push_heartbeat_event");
            }
        }
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::push_event()
//
// description : 	Method to send the event to the event channel
//
// argument : in :	device_impl : The device
//			        event_type : The event type (change, periodic....)
//			        filterable_names :
//			        filterable_data :
//			        attr_value : The attribute value
//			        except : The exception thrown during the last
//				             attribute reading. NULL if no exception
//
//-----------------------------------------------------------------------------

void tg_unlock(TANGO_UNUSED(void *data),void *hint)
{
    EventSupplier *ev = (EventSupplier *)hint;
    omni_mutex &the_mutex = ev->get_push_mutex();
    the_mutex.unlock();
}

void ZmqEventSupplier::push_event(DeviceImpl *device_impl,string event_type,
            TANGO_UNUSED(vector<string> &filterable_names),TANGO_UNUSED(vector<double> &filterable_data),
            TANGO_UNUSED(vector<string> &filterable_names_lg),TANGO_UNUSED(vector<long> &filterable_data_lg),
            struct AttributeData &attr_value,string &attr_name,DevFailed *except)
{
	cout3 << "ZmqEventSupplier::push_event(): called for attribute " << attr_name << endl;

//
// Get the mutex to synchronize the sending of events
// This method may be called by several threads in case they are several
// user threads doing dev.push_xxxx_event() on several devices.
// On top of that, zmq socket can be used by several threads
// only if they are memory barriers between their use in these different
// threads. The mutex used here is also a memory barrier
//

    push_mutex.lock();

//
// Create full event name
// Don't forget case where we have notifd client (thus with a fqdn_prefix modified)
//

	string loc_attr_name(attr_name);
	transform(loc_attr_name.begin(),loc_attr_name.end(),loc_attr_name.begin(),::tolower);

	event_name = fqdn_prefix;

	int size = event_name.size();
	if (event_name[size - 1] == '#')
        event_name.erase(size - 1);

	event_name = event_name + device_impl->get_name_lower() + '/' + loc_attr_name;
	if (Util::_FileDb == true)
        event_name = event_name + MODIFIER_DBASE_NO;
    event_name = event_name + '.' + event_type;

//
// Create zmq messages
// Use memcpy here. Don't use message with no-copy option because
// it does not give any performance improvement in this case
// (too small amount of data)
//

    zmq::message_t name_mess(event_name.size());
    memcpy(name_mess.data(),event_name.data(),event_name.size());

//
// Get event cptr and create the event call zmq message
//

    map<string,unsigned int>::iterator ev_cptr_ite;
    unsigned int ev_ctr = 0;

    ev_cptr_ite = event_cptr.find(event_name);
    if (ev_cptr_ite != event_cptr.end())
        ev_ctr = ev_cptr_ite->second;
    else
    {
        Attribute &att = device_impl->get_device_attr()->get_attr_by_name(attr_name.c_str());
        bool print = false;

        if (event_type == "data_ready")
        {
            if (att.ext->event_data_ready_subscription != 0)
                print = true;
        }
        else if (event_type == "attr_conf")
        {
            if (att.ext->event_attr_conf_subscription != 0)
                print = true;
        }
        else if (event_type == "user_event")
        {
            if (att.ext->event_user_subscription != 0)
                print = true;
        }
        else if (event_type == "change")
        {
            if (att.ext->event_change_subscription != 0)
                print = true;
        }
        else if (event_type == "periodic")
        {
            if (att.ext->event_periodic_subscription != 0)
                print = true;
        }
        else if (event_type == "archive")
        {
            if (att.ext->event_archive_subscription != 0)
                print = true;
        }

        if (print == true)
            cerr << "Can't find event counter for event " << event_name << " in map!" << endl;
    }


    ZmqCallInfo event_call;
    event_call.version = ZMQ_EVENT_PROT_VERSION;
    if (except == NULL)
        event_call.call_is_except = false;
    else
        event_call.call_is_except = true;
    event_call.ctr = ev_ctr;

    cdrMemoryStream event_call_cdr;
    event_call >>= event_call_cdr;

    zmq::message_t event_call_mess(event_call_cdr.bufSize());
    memcpy(event_call_mess.data(),event_call_cdr.bufPtr(),event_call_cdr.bufSize());

//
// Marshall the event data
//

    size_t mess_size;
    void *mess_ptr;

	CORBA::Long padding = 0XDEC0DEC0;
	data_call_cdr.rewindPtrs();
	padding >>= data_call_cdr;
	padding >>= data_call_cdr;
	bool large_data = false;
	bool large_message_created = false;

    if (except == NULL)
    {
        if (attr_value.attr_val != NULL)
        {
            *(attr_value.attr_val) >>= data_call_cdr;
        }
        else if (attr_value.attr_val_3 != NULL)
        {
            *(attr_value.attr_val_3) >>= data_call_cdr;
        }
        else if (attr_value.attr_val_4 != NULL)
        {

//
// Get number of data exchanged by this event
// If this value is greater than a threashold, set a flag
// In such a case, we will use ZMQ no-copy message call
//

            *(attr_value.attr_val_4) >>= data_call_cdr;
            mess_ptr = data_call_cdr.bufPtr();
            mess_ptr = (char *)mess_ptr + (sizeof(CORBA::Long) << 1);

            int nb_data;
            int data_discr = ((int *)mess_ptr)[0];

            if (data_discr == ATT_ENCODED)
            {
                const DevVarEncodedArray &dvea = attr_value.attr_val_4->value.encoded_att_value();
                nb_data = dvea.length();
                if (nb_data > LARGE_DATA_THRESHOLD_ENCODED)
                    large_data = true;
            }
            else
            {
                nb_data = ((int *)mess_ptr)[1];
                if (nb_data >= LARGE_DATA_THRESHOLD)
                    large_data = true;
            }
        }
        else if (attr_value.attr_conf_2 != NULL)
        {
            *(attr_value.attr_conf_2) >>= data_call_cdr;
        }
        else if (attr_value.attr_conf_3 != NULL)
        {
            *(attr_value.attr_conf_3) >>= data_call_cdr;
        }
        else
        {
            *(attr_value.attr_dat_ready) >>= data_call_cdr;
        }
    }
    else
    {
        except->errors >>= data_call_cdr;
    }

    mess_size = data_call_cdr.bufSize() - sizeof(CORBA::Long);
    mess_ptr = (char *)data_call_cdr.bufPtr() + sizeof(CORBA::Long);

//
// For event with samll amount of data, use memcpy to initialize
// the zmq message. For large amount of data, use zmq message
// with no-copy option
//

    zmq::message_t data_mess;

    if (large_data == true)
    {
        data_mess.rebuild(mess_ptr,mess_size,tg_unlock,(void *)this);
        large_message_created = true;
    }
    else
    {
        data_mess.rebuild(mess_size);
        memcpy(data_mess.data(),mess_ptr,mess_size);
    }

//
// Send the data
//

    bool endian_mess_sent = false;

    try
    {

//
// For debug and logging purposes
//

        if (omniORB::trace(20))
        {
            omniORB::logger log;
            log << "ZMQ: Pushing some data" << '\n';
        }
        if (omniORB::trace(30))
        {
            {
                omniORB::logger log;
                log << "ZMQ: Event name" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)name_mess.data(),name_mess.size());

            {
                omniORB::logger log;
                log << "ZMQ: Endianess" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)endian_mess.data(),endian_mess.size());

            {
                omniORB::logger log;
                log << "ZMQ: Call info" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)event_call_mess.data(),event_call_mess.size());

            {
                omniORB::logger log;
                log << "ZMQ: Event data" << '\n';
            }
            omni::giopStream::dumpbuf((unsigned char *)data_mess.data(),data_mess.size());
        }

//
// Get publisher socket (multicast case)
//

        int send_nb = 1;
        zmq::socket_t *pub;
        pub = event_pub_sock;

        zmq::message_t *name_mess_ptr = &name_mess;
        zmq::message_t *endian_mess_ptr = &endian_mess;
        zmq::message_t *event_call_mess_ptr = &event_call_mess;
        zmq::message_t *data_mess_ptr = &data_mess;

        if (event_mcast.empty() == false)
        {
            map<string,McastSocketPub>::iterator ite;

            if ((ite = event_mcast.find(event_name)) != event_mcast.end())
            {
                if (ite->second.local_client == false)
                {
                   pub = ite->second.pub_socket;
                }
                else
                {
                    if (ite->second.pub_socket != NULL)
                    {
                        send_nb = 2;
                        pub = ite->second.pub_socket;
                    }
                }

            }
        }

        if (double_send == true)
        {
            send_nb = 2;
            double_send = false;
        }

//
// If we have a multicast socket with also a local client
// we are obliged to send to times the messages.
// ZMQ does not support local client with PGM socket
//

        zmq::message_t name_mess_2;
        zmq::message_t event_call_mess_2;
        zmq::message_t data_mess_2;

        if (send_nb == 2)
        {
            name_mess_2.copy(&name_mess);
            event_call_mess_2.copy(&event_call_mess);
            data_mess_2.copy(&data_mess);
        }

        while(send_nb > 0)
        {

//
// Push the event
//

            bool ret;

            ret = pub->send(*name_mess_ptr,ZMQ_SNDMORE);
if (ret == false)
{
    cout << "Name message returned false" << endl;
    assert(false);
}
            ret = pub->send(*endian_mess_ptr,ZMQ_SNDMORE);
if (ret == false)
{
    cout << "Endian message returned false" << endl;
    assert(false);
}
            endian_mess_sent = true;
            pub->send(*event_call_mess_ptr,ZMQ_SNDMORE);
            ret = pub->send(*data_mess_ptr,0);
if (ret == false)
{
    cout << "Data message returned false" << endl;
    assert(false);
}

            send_nb--;
            if (send_nb == 1)
            {

//
// Case of multicast socket with a local client
//

                pub = event_pub_sock;

                name_mess_ptr = &name_mess_2;
                endian_mess.copy(&endian_mess_2);
                event_call_mess_ptr = &event_call_mess_2;
                data_mess_ptr = &data_mess_2;
            }

        }

//
// Increment event counter
//

        if (ev_cptr_ite != event_cptr.end())
            ev_cptr_ite->second++;

//
// release mutex if we haven't use ZMQ no copy mode
//

        if (large_data == false)
            push_mutex.unlock();

//
// For reference counting on zmq messages which do not have a local scope
//

        endian_mess.copy(&endian_mess_2);
    }
    catch(...)
    {
        cout3 << "ZmqEventSupplier::push_event() failed !!!!!!!!!!!\n";
        if (endian_mess_sent == true)
            endian_mess.copy(&endian_mess_2);

        if (large_message_created == false)
            push_mutex.unlock();

        TangoSys_OMemStream o;
        o << "Can't push ZMQ event for event ";
        o << event_name;
        if (zmq_errno() != 0)
            o << "\nZmq error: " << zmq_strerror(zmq_errno()) << ends;
        else
            o << ends;

        Except::throw_exception((const char *)"DServer_Events",
                                    o.str(),
                                   (const char *)"ZmqEventSupplier::push_event");
    }
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSupplier::update_connected_client
//
// description :
//
//-----------------------------------------------------------------------------


bool ZmqEventSupplier::update_connected_client(client_addr *cl)
{
    bool ret = false;

//
// Immediately return if client identification not possible
// (Very old client....)
//

    if (cl == NULL)
        return ret;

//
// First try to find the client in list
//

    struct timeval now;

#ifdef _TG_WINDOWS_
    struct _timeb after_win;

    _ftime(&after_win);
    now.tv_sec = (time_t)after_win.time;
#else
    gettimeofday(&now,NULL);
#endif

    list<ConnectedClient>::iterator pos;

#ifdef HAS_LAMBDA_FUNC
    pos = find_if(con_client.begin(),con_client.end(),
                  [&] (ConnectedClient &cc) -> bool
                  {
                      return (cc.clnt == *cl);
                  });
#else
    pos = find_if(con_client.begin(),con_client.end(),
            bind2nd(WantedClient<ZmqEventSupplier::ConnectedClient,client_addr,bool>(),*cl));
#endif

//
// Update date if client in list. Otherwise add client to list
//

    if (pos != con_client.end())
    {
        pos->date = now.tv_sec;
    }
    else
    {
        ConnectedClient new_cc;
        new_cc.clnt = *cl;
        new_cc.date = now.tv_sec;

        con_client.push_back(new_cc);
        ret = true;
    }

//
// Remove presumly dead client
//

#ifdef HAS_LAMBDA_FUNC
    con_client.remove_if([&] (ConnectedClient &cc) -> bool
                        {
                            if (now.tv_sec > (cc.date + 500))
                                return true;
                            else
                                return false;
                        });
#else
   con_client.remove_if(bind2nd(OldClient<ZmqEventSupplier::ConnectedClient,time_t,bool>(),now.tv_sec));
#endif

    return ret;
}

} /* End of Tango namespace */

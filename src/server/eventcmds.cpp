//+===================================================================================================================
//
// file :        eventcmds.cpp
//
// description : C++ source for the event commands which are part of the DServer class.
//
// project :     TANGO events
//
// $Author$
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
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
//-=================================================================================================================

#include <tango.h>
#include <tango/server/eventsupplier.h>
#include <common/admin/commands/zmq_event_subscription_change_response.h>

namespace Tango
{


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::event_subscription_change()
//
// description :
//		method to execute the command EventSubscriptionChange command.
//
// args :
//		in :
// 			- argin : The command input argument
//
// returns :
//		The command output data (Tango lib release number)
//
//------------------------------------------------------------------------------------------------------------------
DevLong DServer::event_subscription_change(TANGO_UNUSED(const Tango::DevVarStringArray *argin))
{
    Except::throw_exception((const char *) API_UnsupportedFeature,
                            "Notifd is not supported. Please update!!!",
                            (const char *) "DServer::event_subscription_change");

}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::event_subscription()
//
// description :
//		method to do all the necessary checks on attribute config to generate events
//
// args :
// 		in :
//			- dev_name : The device name
//      	- obj_name : The attribute/pipe name
//      	- action : What the user want to do
//      	- event : The event type
//      	- obj_name_lower : The attribute/pipe name in lower case letters
//      	- ct : The channel type (notifd or zmq)
//      	- mcast_data : The multicast transport data
//      	- rate : PGM rate parameter
//      	- ivl : PGM ivl paramteter
//      	- dev : The device pointer
//			- client_lib : Tango release number used by client
//
//--------------------------------------------------------------------------------------------------------------------
//TODO looks like action is always == "subscribe"
//TODO pass event_type instead of event_name
//TODO return object with rate, ivl etc
void
DServer::event_subscription(string &dev_name,
                            string &obj_name,
                            string &action,
                            string &event_name,
                            string &ev_full_name,
                            string &obj_name_lower,
                            ChannelType ct,
                            string &mcast_data,
                            int &rate,
                            int &ivl,
                            DeviceImpl *dev,
                            int client_lib)
{
    Tango::Util *tg = Tango::Util::instance();

//
// Get device reference
//

    DeviceImpl *dev_impl = dev;
//TODO move to caller
    if (dev_impl == NULL)
    {
        try
        {
            dev_impl = tg->get_device_by_name(dev_name);
        }
        catch (Tango::DevFailed &e)
        {
            TangoSys_OMemStream o;
            o << "Device " << dev_name << " not found" << ends;
            Except::re_throw_exception(e, (const char *) API_DeviceNotFound, o.str(),
                                       (const char *) "DServer::event_subscription");
        }
    }

    if (event_name != EventName[INTERFACE_CHANGE_EVENT] && event_name != EventName[PIPE_EVENT])
    {

        MultiAttribute *m_attr = dev_impl->get_device_attr();
        int attr_ind = m_attr->get_attr_ind_by_name(obj_name.c_str());
        Attribute &attribute = m_attr->get_attr_by_ind(attr_ind);

        if (attribute.is_fwd_att() == true && client_lib < 5)
        {
            stringstream ss;
            ss << "The attribute " << obj_name << " is a forwarded attribute.";
            ss
                << "\nIt is not supported to subscribe events from forwarded attribute using Tango < 9. Please update!!";

            Except::throw_exception(API_NotSupportedFeature,
                                    ss.str(), "DServer::event_subscription");
        }

        if (action == "subscribe")
        {
            if (event_name == "user_event")
            {
                update_user_event_subscription_time(client_lib, attribute);
            }
            else if (event_name.find(CONF_TYPE_EVENT) != string::npos)
            {
                update_attr_conf_subscription_time(client_lib, attribute);
            }
            else if (event_name == "data_ready")
            {
                update_data_ready_subscription_time(obj_name, attribute);
            }
            else
            {

//
// If the polling is necessary to send events, check whether the polling is started for the requested attribute.
//

                check_polling(obj_name, event_name, attribute);


                update_attribute_subscription_time(obj_name, event_name, obj_name_lower, client_lib, attribute);
            }

//
// Set channel type in attribute object
//

            if (ct == ZMQ)
                attribute.set_use_zmq_event();
            else
                assert(false);

            //set zmq topic
            if (ct == ZMQ)
                attribute.set_zmq_subscription_topic(ev_full_name);
            else
                assert(false);

            if (ct == ZMQ)
            {
                setup_mcast(event_name, mcast_data, rate, ivl, tg, attribute);
            }
            else
            {
                rate = 0;
                ivl = 0;
            }
        }

//
// Memorize client lib release. Protect this setting in case of user thread pushing event when the subscription
// command is received
//

        if (client_lib != 0)
        {
            omni_mutex_lock oml(EventSupplier::get_event_mutex());
            attribute.set_client_lib(client_lib, event_name);
        }
    }
    else if (event_name == EventName[PIPE_EVENT])
    {
        if (action == "subscribe")
        {
            DeviceClass *cl = dev_impl->get_device_class();
            Pipe &pipe = cl->get_pipe_by_name(obj_name, dev_impl->get_name_lower());

            cout4 << "DServer::event_subscription(): update pipe subscription\n";

            omni_mutex_lock oml(EventSupplier::get_event_mutex());
            pipe.set_event_subscription(time(NULL));

// TODO: Pipe: Do we support multicast for pipe event

            rate = 0;
            ivl = 0;

            pipe.set_zmq_subscription_topic(ev_full_name);
        }
    }
    else
    {
        if (action == "subscribe")
        {
            cout4 << "DServer::event_subscription(): update device interface_change subscription\n";

            omni_mutex_lock oml(EventSupplier::get_event_mutex());
            dev_impl->set_event_intr_change_subscription(time(NULL));

// TODO: Do we support multicast for interface change event

            rate = 0;
            ivl = 0;

            if (client_lib != 0)
                dev_impl->set_client_lib(client_lib);

            dev_impl->set_zmq_subscription_topic(ev_full_name);
        }
    }

//
// Ask polling thread in charge of heartbeat to send them (if not already done)
//

    if (action == "subscribe")
    {
        try
        {
            if (get_heartbeat_started() == false)
            {
                add_event_heartbeat();
                set_heartbeat_started(true);
            }
        }
        catch (...)
        {
        }
    }
}
void DServer::check_polling(const string &obj_name, const string &event_name, Attribute &attribute) const
{
    if (not(attribute.is_polled()))
    {
        TangoSys_OMemStream o;
        o << "The polling (necessary to send events) for the attribute ";
        o << obj_name;
        o << " is not started" << ends;

        if (event_name == "change")
        {
            if (attribute.is_fwd_att() == false && attribute.is_change_event() == false)
            {
                Except::throw_exception(API_AttributePollingNotStarted, o.str(),
                                        "DServer::event_subscription");
            }
        }
        else
        {
            if (event_name == "archive")
            {
                if (attribute.is_fwd_att() == false && attribute.is_archive_event() == false)
                {
                    Except::throw_exception(API_AttributePollingNotStarted, o.str(),
                                            "DServer::event_subscription");
                }
            }
            else
            {
                if (attribute.is_fwd_att() == false)
                    Except::throw_exception(API_AttributePollingNotStarted,
                                            o.str(),
                                            "DServer::event_subscription");
            }
        }
    }
}
void DServer::update_user_event_subscription_time(int client_lib, Attribute &attribute) const
{
    cout4 << "DServer::event_subscription(): update user_event subscription\n";

    omni_mutex_lock oml(EventSupplier::get_event_mutex());
    switch (client_lib)
    {
        //TODO extract class hierarchy based on version!!!
        case 6:
        case 5:
            attribute.event_user5_subscription = time(NULL);
            break;

        case 4:
            attribute.event_user4_subscription = time(NULL);
            break;

        default:
            attribute.event_user3_subscription = time(NULL);
            break;
    }
}
void DServer::update_attr_conf_subscription_time(int client_lib, Attribute &attribute) const
{
    cout4 << "DServer::event_subscription(): update attr_conf subscription\n";

    omni_mutex_lock oml(EventSupplier::get_event_mutex());
    //TODO extract class hierarchy based on versions
    if (client_lib >= 5)
        attribute.event_attr_conf5_subscription = time(NULL);
    else
        attribute.event_attr_conf_subscription = time(NULL);
}
void DServer::update_data_ready_subscription_time(const string &obj_name, Attribute &attribute) const
{
    if (attribute.is_fwd_att() == false && attribute.is_data_ready_event() == false)
    {
        TangoSys_OMemStream o;
        o << "The attribute ";
        o << obj_name;
        o << " is not data ready event enabled" << ends;

        Except::throw_exception(API_AttributeNotDataReadyEnabled,
                                o.str(),
                                "DServer::event_subscription");
    }
    cout4 << "DServer::event_subscription(): update data_ready subscription\n";

    omni_mutex_lock oml(EventSupplier::get_event_mutex());
    attribute.event_data_ready_subscription = time(NULL);
}
void DServer::update_attribute_subscription_time(const string &obj_name,
                                                 const string &event_name,
                                                 const string &obj_name_lower,
                                                 int client_lib,
                                                 Attribute &attribute) const
{
    if (event_name == "change")
    {
        update_change_event_subscription_time(obj_name, obj_name_lower, client_lib, attribute);
    }
    else if (event_name == "quality")
    {
        cout4 << "DServer::event_subscription(): update quality_change subscription\n";
        attribute.event_quality_subscription = time(NULL);
    }
    else if (event_name == "periodic")
    {
        update_periodic_event_subscription_time(client_lib, attribute);

    }
    else if (event_name == "archive")
    {
        update_archive_event_subscription_time(obj_name, obj_name_lower, client_lib, attribute);
    }
}
void DServer::update_periodic_event_subscription_time(int client_lib, Attribute &attribute) const
{
    cout4 << "DServer::event_subscription(): update periodic subscription\n";

    omni_mutex_lock oml(EventSupplier::get_event_mutex());
    switch (client_lib)
    {
        //TODO extract class hierarchy based on version!!!
        case 6:
        case 5:
            attribute.event_periodic5_subscription = time(NULL);
            break;

        case 4:
            attribute.event_periodic4_subscription = time(NULL);
            break;

        default:
            attribute.event_periodic3_subscription = time(NULL);
            break;
    }
}
void
DServer::update_change_event_subscription_time(const string &obj_name,
                                               const string &obj_name_lower,
                                               int client_lib,
                                               Attribute &attribute) const
{
    cout4 << "DServer::event_subscription(): update change subscription\n";

//
// Check if the attribute has some of the change properties defined
//

    if (obj_name_lower != "state")
    {
        if ((attribute.get_data_type() != DEV_STRING) &&
            (attribute.get_data_type() != DEV_BOOLEAN) &&
            (attribute.get_data_type() != DEV_ENCODED) &&
            (attribute.get_data_type() != DEV_STATE) &&
            (attribute.get_data_type() != DEV_ENUM))
        {
            if (attribute.is_check_change_criteria() == true)
            {
                if ((attribute.rel_change[0] == INT_MAX) &&
                    (attribute.rel_change[1] == INT_MAX) &&
                    (attribute.abs_change[0] == INT_MAX) &&
                    (attribute.abs_change[1] == INT_MAX))
                {
                    TangoSys_OMemStream o;
                    o << "Event properties (abs_change or rel_change) for attribute ";
                    o << obj_name;
                    o << " are not set" << ends;

                    Except::throw_exception(API_EventPropertiesNotSet,
                                            o.str(), "DServer::event_subscription");
                }
            }
        }
    }

    omni_mutex_lock oml(EventSupplier::get_event_mutex());

    switch (client_lib)
    {
        //TODO extract class hierarchy based on version!!!
        case 6:
        case 5:
            attribute.event_change5_subscription = time(NULL);
            break;

        case 4:
            attribute.event_change4_subscription = time(NULL);
            break;

        default:
            attribute.event_change3_subscription = time(NULL);
            break;
    }
}

void DServer::update_archive_event_subscription_time(const string &obj_name,
                                                     const string &obj_name_lower,
                                                     int client_lib,
                                                     Attribute &attribute) const
{
    if (obj_name_lower != "state")
    {
        if ((attribute.get_data_type() != DEV_STRING) &&
            (attribute.get_data_type() != DEV_BOOLEAN) &&
            (attribute.get_data_type() != DEV_ENCODED) &&
            (attribute.get_data_type() != DEV_STATE) &&
            (attribute.get_data_type() != DEV_ENUM))
        {
            if (attribute.is_check_archive_criteria() == true)
            {
                if ((attribute.archive_abs_change[0] == INT_MAX) &&
                    (attribute.archive_abs_change[1] == INT_MAX) &&
                    (attribute.archive_rel_change[0] == INT_MAX) &&
                    (attribute.archive_rel_change[1] == INT_MAX) &&
                    (attribute.archive_period == INT_MAX))
                {
                    TangoSys_OMemStream o;
                    o
                        << "Archive event properties (archive_abs_change or archive_rel_change or archive_period) for attribute ";
                    o << obj_name;
                    o << " are not set" << ends;

                    Except::throw_exception(API_EventPropertiesNotSet,
                                            o.str(), "DServer::event_subscription");
                }
            }
        }
    }

    cout4 << "DServer::event_subscription(): update archive subscription\n";

    omni_mutex_lock oml(EventSupplier::get_event_mutex());
    switch (client_lib)
    {
        //TODO extract class hierarchy based on version!!!
        case 6:
        case 5:
            attribute.event_archive5_subscription = time(NULL);
            break;

        case 4:
            attribute.event_archive4_subscription = time(NULL);
            break;

        default:
            attribute.event_archive3_subscription = time(NULL);
            break;
    }
}
void DServer::setup_mcast(const string &event_name,
                          string &mcast_data,
                          int &rate,
                          int &ivl,
                          Util *tg,
                          const Attribute &attribute) const
{
    bool found = false;

    ZmqEventSupplier *ev;
    ev = tg->get_zmq_event_supplier();
    int zmq_release = ev->get_zmq_release();

    for (size_t i = 0; i != attribute.mcast_event.size(); ++i)
    {
        if (attribute.mcast_event[i].find(event_name) == 0)
        {
            if (zmq_release < 320)
            {
                int zmq_major, zmq_minor, zmq_patch;
                zmq_version(&zmq_major, &zmq_minor, &zmq_patch);

                TangoSys_OMemStream o;
                o << "Device server process is using zmq release ";
                o << zmq_major << "." << zmq_minor << "." << zmq_patch;
                o << "\nMulticast event(s) not available with this ZMQ release" << ends;

                Except::throw_exception(API_UnsupportedFeature,
                                        o.str(), "DServer::event_subscription");
            }

            string::size_type start, end;
            start = attribute.mcast_event[i].find(':');
            start++;
            end = attribute.mcast_event[i].find(':', start);

            if ((end = attribute.mcast_event[i].find(':', end + 1)) == string::npos)
            {
                mcast_data = attribute.mcast_event[i].substr(start);
                rate = 0;
                ivl = 0;
                found = true;
                break;
            }
            else
            {
                mcast_data = attribute.mcast_event[i].substr(start, end - start);

//
// Get rate because one is defined
//

                string::size_type start_rate = end + 1;
                if ((end = attribute.mcast_event[i].find(':', start_rate)) == string::npos)
                {
                    istringstream iss(attribute.mcast_event[i].substr(start_rate));
                    iss >> rate;
                    rate = rate * 1024;
                    ivl = 0;
                    found = true;
                    break;
                }
                else
                {
                    istringstream iss(attribute.mcast_event[i].substr(start_rate, end - start_rate));
                    iss >> rate;
                    rate = rate * 1024;

//
// Get ivl because one is defined
//

                    istringstream iss_ivl(attribute.mcast_event[i].substr(end + 1));
                    iss_ivl >> ivl;
                    ivl = ivl * 1000;
                    found = true;
                    break;
                }
            }
        }
    }

    if (not(found))
    {
        rate = 0;
        ivl = 0;
    }

//
// If one of the 2 parameters are not defined, get the default value
//

    if (rate == 0)
        rate = mcast_rate;
    if (ivl == 0)
        ivl = mcast_ivl;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::zmq_event_subscription_change()
//
// description :
//		method to execute the command ZmqEventSubscriptionChange command.
//
// args :
// 		in :
//			- argin : The command input argument
//
// returns :
//		The command output data (Tango lib release number)
//
//-------------------------------------------------------------------------------------------------------------------
DevVarLongStringArray *DServer::zmq_event_subscription_change(const Tango::DevVarStringArray *argin)
{
    if (argin->length() > 1 && argin->length() < 4)
    {
        TangoSys_OMemStream o;
        o
            << "Not enough input arguments, needs at least 4 i.e. device name, attribute/pipe name, action, event name, <Tango lib release>"
            << ends;

        Except::throw_exception((const char *) API_WrongNumberOfArgs,
                                o.str(),
                                (const char *) "DServer::zmq_event_subscription_change");
    }

    if (argin->length() == 1 && strcmp((*argin)[0], "info"))
    {
        return zmq_event_subscription_info();
    }
    else
    {
        Tango::Util *tg = Tango::Util::instance();

        string dev_name, obj_name, action, event_name, obj_name_lower;
        dev_name = (*argin)[0];
        obj_name = (*argin)[1];
        action = (*argin)[2];
        event_name = (*argin)[3];

        check_event_name_exists(event_name);

        bool intr_change = false;
        if (event_name == EventName[INTERFACE_CHANGE_EVENT])
            intr_change = true;

        bool pipe_event = false;
        if (event_name == EventName[PIPE_EVENT])
            pipe_event = true;

        obj_name_lower = obj_name;
        transform(obj_name_lower.begin(), obj_name_lower.end(), obj_name_lower.begin(), ::tolower);

        int client_release = guess_client_lib_version(argin, event_name);

        cout4 << "ZmqEventSubscriptionChangeCmd: subscription for device " << dev_name << " attribute/pipe " << obj_name
              << " action " << action << " event " << event_name << " client lib = " << client_release << endl;

//
// If we receive this command while the DS is in its shuting down sequence, do nothing
//

        if (tg->get_heartbeat_thread_object() == NULL)
        {
            TangoSys_OMemStream o;
            o << "The device server is shutting down! You can no longer subscribe for events" << ends;

            Except::throw_exception((const char *) API_ShutdownInProgress,
                                    o.str(),
                                    (const char *) "DServer::zmq_event_subscription_change");
        }

//
// If the EventSupplier object is not created, create it right now
//

        ZmqEventSupplier *ev;
        if ((ev = tg->get_zmq_event_supplier()) == NULL)
        {
            tg->create_zmq_event_supplier();
            ev = tg->get_zmq_event_supplier();
        }

//
// Get device pointer and check which IDL release it implements. If it is less than IDL 4, refuse to use ZMQ event.
// To do so, simulate a Tango 7 DS (throw command not exist exception)
// Also change event name if both device and client supports IDL5 and lib 9 (For attribute conf. change event)
//

        DeviceImpl *dev = NULL;

        try
        {
            dev = tg->get_device_by_name(dev_name);
        }
        catch (Tango::DevFailed &e)
        {
            TangoSys_OMemStream o;
            o << "Device " << dev_name << " not found" << ends;
            Except::re_throw_exception(e, (const char *) API_DeviceNotFound, o.str(),
                                       (const char *) "DServer::event_subscription");
        }

        long idl_vers = dev->get_dev_idl_version();
        if (idl_vers < 4)
        {
            TangoSys_OMemStream o;

            o << "Device " << dev_name << " too old to use ZMQ event (it does not implement IDL 4)";
            o << "\nSimulate a CommandNotFound exception to move to notifd event system" << ends;
            Except::throw_exception((const char *) API_CommandNotFound,
                                    o.str(),
                                    (const char *) "DServer::zmq_event_subscription_change");
        }

        if (client_release > idl_vers)
            client_release = idl_vers;

//
// Call common method (common between old and new command)
//

        string mcast;
        int rate, ivl;

        string
            event_full_name = get_event_full_name(event_name, obj_name_lower, intr_change, dev, ev->get_fqdn_prefix());

        //TODO avoid this - introduce a new object with corresponding methods
        string::size_type pos = event_name.find(EVENT_COMPAT);
        if (pos != string::npos)
            event_name.erase(0, EVENT_COMPAT_IDL5_SIZE);

        event_subscription(dev_name,
                           obj_name,
                           action,
                           event_name,
                           event_full_name,
                           obj_name_lower,
                           ZMQ,
                           mcast,
                           rate,
                           ivl,
                           dev,
                           client_release);

//
// Check if the client is a new one
//

        bool new_client = ev->update_connected_client(get_client_ident());
        if (new_client == true)
            ev->set_double_send();

//
// Create the event publisher socket (if not already done). Take care for case where the device is running with db
// in a file
//



        //
// If the event is defined as using mcast transport, get caller host
//

        bool local_call = false;
        if (mcast.empty() == false)
        {
            client_addr *c_addr = get_client_ident();
            if ((c_addr->client_ip[5] == 'u') ||
                ((c_addr->client_ip[9] == '1') && (c_addr->client_ip[10] == '2') && (c_addr->client_ip[11] == '7')))
            {
                local_call = true;
            }

        }

//
// Create ZMQ event socket
//

        if (mcast.empty() == false)
            ev->create_mcast_event_socket(mcast, event_full_name, rate, local_call);
        else
            ev->create_event_socket();

//
// Init event counter in Event Supplier
//

        ev->init_event_cptr(event_full_name);

//
// Init one subscription command flag in Eventsupplier
//

        if (ev->get_one_subscription_cmd() == false)
            ev->set_one_subscription_cmd(true);

//
// For forwarded attribute, eventually subscribe to events coming from root attribute
//

        if (intr_change == false && pipe_event == false)
        {
            Attribute &attribute = dev->get_device_attr()->get_attr_by_name(obj_name.c_str());
            EventType et;
            tg->event_name_2_event_type(event_name, et);

            if (attribute.is_fwd_att() == true && et != ATTR_CONF_EVENT)
            {
                FwdAttribute &fwd_att = static_cast<FwdAttribute &>(attribute);
                string root_name = fwd_att.get_fwd_dev_name() + "/" + fwd_att.get_fwd_att_name();
                RootAttRegistry &rar = tg->get_root_att_reg();
                bool already_there = rar.is_event_subscribed(root_name, et);

//
// We unsubscribe and subscribe. This is mandatory for following case: The appli is killed and re-started
// but in the meantime, polling for the root attribute has been stopped. The error that the polling is not started
// for the root attribute is sent at subscription time
//

                if (already_there == true)
                    rar.unsubscribe_user_event(fwd_att.get_fwd_dev_name(), fwd_att.get_fwd_att_name(), et);
                rar.subscribe_user_event(fwd_att.get_fwd_dev_name(), fwd_att.get_fwd_att_name(), et);
            }
        }

//
// Init data returned by command
//
        using ZmqChangeSubscriptionResponse = org::tango::common::admin::commands::ZmqChangeSubscriptionResponse;


        vector<pair<string, string>> alternative_endpoints;
        size_t nb_alt = ev->get_alternate_heartbeat_endpoint().size();
        bool has_alternative_endpoints = nb_alt > 0;
        if (has_alternative_endpoints)
        {
            alternative_endpoints.reserve(nb_alt);
            for (size_t i = 0; i < nb_alt; i++)
            {
                alternative_endpoints
                    .emplace_back(ev->get_alternate_heartbeat_endpoint()[i], ev->get_alternate_event_endpoint()[i]);
            }
        }

        return ZmqChangeSubscriptionResponse{
            tg->get_tango_lib_release(),
            dev->get_dev_idl_version(),
            zmq_sub_event_hwm,
            rate,
            ivl,
            ev->get_zmq_release(),
            ev->get_heartbeat_endpoint(),
            mcast.empty() ? ev->get_event_endpoint() : ev->get_mcast_event_endpoint(event_full_name),
            event_full_name,
            alternative_endpoints
        }.to_DevVarLongStringArray();
    }
}

Tango::DevVarLongStringArray *DServer::zmq_event_subscription_info() const
{
    Tango::Util *tg = Tango::Util::instance();
    auto ret_data = new Tango::DevVarLongStringArray();

    ret_data->svalue.length(2);

    ret_data->lvalue.length(1);
    ret_data->lvalue[0] = (DevLong) tg->get_tango_lib_release();;

    ZmqEventSupplier *ev;
    if ((ev = tg->get_zmq_event_supplier()) != NULL)
        {
            string tmp_str("Heartbeat: ");
            tmp_str = tmp_str + ev->get_heartbeat_endpoint();
            ret_data->svalue[0] = string_dup(tmp_str.c_str());

            tmp_str = "Event: ";
            string ev_end = ev->get_event_endpoint();
            if (ev_end.size() != 0)
                tmp_str = "Event: " + ev_end;
            size_t nb_mcast = ev->get_mcast_event_nb();
            if (nb_mcast != 0)
            {
                if (ev_end.size() != 0)
                    tmp_str = tmp_str + "\n";
                tmp_str = tmp_str + "Some event(s) sent using multicast protocol";
            }
            ret_data->svalue[1] = string_dup(tmp_str.c_str());

            size_t nb_alt = ev->get_alternate_heartbeat_endpoint().size();
            if (nb_alt != 0)
            {
                ret_data->svalue.length((nb_alt + 1) << 1);

                for (size_t loop = 0; loop < nb_alt; loop++)
                {
                    string tmp_str("Alternate heartbeat: ");
                    tmp_str = tmp_str + ev->get_alternate_heartbeat_endpoint()[loop];
                    ret_data->svalue[(loop + 1) << 1] = string_dup(tmp_str.c_str());

                    tmp_str = "Alternate event: ";
                    if (ev->get_alternate_event_endpoint().size() != 0)
                    {
                        string ev_end = ev->get_alternate_event_endpoint()[loop];
                        if (ev_end.empty() == false)
                            tmp_str = "Alternate event: " + ev_end;
                    }
                    ret_data->svalue[((loop + 1) << 1) + 1] = string_dup(tmp_str.c_str());
                }
            }
        }
    else
    {
        ret_data->svalue[0] = string_dup("No ZMQ event yet!");
    }

    return ret_data;
}

string DServer::get_event_full_name(const string &event,
                                    const string &obj_name_lower,
                                    bool intr_change,
                                    DeviceImpl *dev,
                                    string &fqdn_prefix) const
{
    string ev_name = fqdn_prefix;
    if (Util::_FileDb == true)
    {
        int size = ev_name.size();
        if (ev_name[size - 1] == '#')
            ev_name.erase(size - 1);
    }

    ev_name = ev_name + dev->get_name_lower();
    if (intr_change == false)
        ev_name = ev_name + '/' + obj_name_lower;
    if (Util::_FileDb == true)
        ev_name = ev_name + MODIFIER_DBASE_NO;
    ev_name = ev_name + '.' + event;
    return ev_name;
}

int DServer::guess_client_lib_version(const DevVarStringArray *argin, string &event)
{
    int client_release = 4;
    if (event == EventName[ATTR_CONF_EVENT])
        client_release = 3;

    if (argin->length() == 5)
    {
        stringstream ss;
        ss << (*argin)[4];
        ss >> client_release;

        if (client_release == 0)
        {
            string::size_type pos = event.find(EVENT_COMPAT);
            if (pos != string::npos)
            {
                string client_lib_str = event.substr(pos + 3, 1);
                stringstream ss;
                ss << client_lib_str;
                ss >> client_release;
                event.erase(0, EVENT_COMPAT_IDL5_SIZE);
            }
            else
            {
                if (event == EventName[ATTR_CONF_EVENT])
                    client_release = 3;
                else
                {

//
// Check if the request comes from a Tango 6 client (without client identification)
// If true, the event has to be sent using AttributeValue_3 data structure
// If cl is NULL, this means that the call is local (Two tango classes within the same process and with events between
// device from class 1 and device from classs 2)
//

                    client_addr *cl = get_client_ident();

                    if (cl == NULL)
                        client_release = 4;
                    else
                    {
                        if (cl->client_ident == true)
                            client_release = 4;
                        else
                            client_release = 3;
                    }
                }
            }
        }
    }
    return client_release;
}

//
// Check event type validity
//
void DServer::check_event_name_exists(const string &event) const
{
    string check_event = event;
    transform(check_event.begin(), check_event.end(), check_event.begin(), ::tolower);

    string::size_type pos_check = check_event.find(EVENT_COMPAT);
    if (pos_check != string::npos)
        check_event.erase(0, EVENT_COMPAT_IDL5_SIZE);

    size_t nb_event_type = sizeof(EventName) / sizeof(char *);

    for (size_t i = 0; i < nb_event_type; i++)
    {
        if (strcmp(check_event.c_str(), EventName[i]) == 0)
        {
            return;
        }
    }

    Except::throw_exception(
        API_WrongNumberOfArgs,
        "The event type you sent (" + event + ") is not a valid event type", //TODO name valid types
        "DServer::zmq_event_subscription_change");
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::event_confirm_subscription()
//
// description :
//		method to execute the command EventConfirmSubscription command.
//
// args :
// 		in :
//			- argin : The command input argument
//
//------------------------------------------------------------------------------------------------------------------
void DServer::event_confirm_subscription(const Tango::DevVarStringArray *argin)
{

//
// Some check on argument
//

    if ((argin->length() == 0) || (argin->length() % 3) != 0)
    {
        TangoSys_OMemStream o;
        o << "Wrong number of input arguments: 3 needed per event: device name, attribute/pipe name and event name"
          << endl;

        Except::throw_exception((const char *) API_WrongNumberOfArgs, o.str(),
                                (const char *) "DServer::event_confirm_subscription");
    }

//
// If we receive this command while the DS is in its shuting down sequence, do nothing
//

    Tango::Util *tg = Tango::Util::instance();
    if (tg->get_heartbeat_thread_object() == NULL)
    {
        TangoSys_OMemStream o;
        o << "The device server is shutting down! You can no longer subscribe for events" << ends;

        Except::throw_exception((const char *) API_ShutdownInProgress, o.str(),
                                (const char *) "DServer::event_confirm_subscription");
    }

//
// A loop for each event
//

    unsigned int nb_event = argin->length() / 3;

    string old_dev;
    DeviceImpl *dev = NULL;

    for (unsigned int loop = 0; loop < nb_event; loop++)
    {
        string dev_name, obj_name, event, obj_name_lower;
        int base = loop * 3;
        dev_name = (*argin)[base];
        obj_name = (*argin)[base + 1];
        event = (*argin)[base + 2];

        obj_name_lower = obj_name;
        transform(obj_name_lower.begin(), obj_name_lower.end(), obj_name_lower.begin(), ::tolower);

        cout4 << "EventConfirmSubscriptionCmd: confirm subscription for device " << dev_name << " attribute/pipe "
              << obj_name << " event " << event << endl;

//
// Find device
//

        if (old_dev != dev_name)
        {
            try
            {
                dev = tg->get_device_by_name(dev_name);
            }
            catch (Tango::DevFailed &e)
            {
                TangoSys_OMemStream o;
                o << "Device " << dev_name << " not found" << ends;
                Except::re_throw_exception(e, (const char *) API_DeviceNotFound, o.str(),
                                           (const char *) "DServer::event_confirm_subscription");
            }

            old_dev = dev_name;
        }

//
// Call common method (common between old and new command)
//

        string mcast;
        int rate, ivl, client_lib;

        string action("subscribe");
        string client_lib_str;

        string ev_full_name = get_event_full_name(event,
                                                  obj_name_lower,
                                                  event == EventName[INTERFACE_CHANGE_EVENT],
                                                  dev,
                                                  tg->get_zmq_event_supplier()->get_fqdn_prefix());//TODO may throw NPE

        string::size_type pos = event.find(EVENT_COMPAT);
        if (pos != string::npos)
        {
            client_lib_str = event.substr(pos + 3, 1);
            stringstream ss;
            ss << client_lib_str;
            ss >> client_lib;
            event.erase(0, EVENT_COMPAT_IDL5_SIZE);
        }
        else
        {
            if (event == EventName[ATTR_CONF_EVENT])
                client_lib = 3;
            else
                client_lib = 4;        // Command implemented only with Tango 8 -> IDL 4 for event data
        }


        event_subscription(dev_name,
                           obj_name,
                           action,
                           event,
                           ev_full_name,
                           obj_name_lower,
                           ZMQ,
                           mcast,
                           rate,
                           ivl,
                           dev,
                           client_lib);
    }

}

}    // namespace

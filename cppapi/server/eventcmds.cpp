//+===================================================================================================================
//
// file :        eventcmds.cpp
//
// description : C++ source for the event commands which are part of the DServer class.
//
// project :     TANGO events
//
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
//
//-=================================================================================================================

#include <tango.h>
#include <eventsupplier.h>

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
DevLong DServer::event_subscription_change(const Tango::DevVarStringArray *argin)
{
    if (argin->length() < 4)
    {
		TangoSys_OMemStream o;
		o << "Not enough input arguments, needs 4 i.e. device name, attribute name, action, event name" << std::ends;

		Except::throw_exception((const char *)API_WrongNumberOfArgs,
								o.str(),
								(const char *)"DServer::event_subscription_change");
	}

	std::string dev_name, attr_name, action, event, attr_name_lower;
	dev_name = (*argin)[0];
	attr_name = (*argin)[1];
	action = (*argin)[2];
	event = (*argin)[3];

	attr_name_lower = attr_name;
	std::transform(attr_name_lower.begin(),attr_name_lower.end(),attr_name_lower.begin(),::tolower);

	cout4 << "EventSubscriptionChangeCmd: subscription for device " << dev_name << " attribute " << attr_name << " action " << action << " event " << event << std::endl;

	Tango::Util *tg = Tango::Util::instance();

//
// If we receive this command while the DS is in its shuting down sequence, do nothing
//

	if (tg->get_heartbeat_thread_object() == NULL)
	{
     	TangoSys_OMemStream o;
		o << "The device server is shutting down! You can no longer subscribe for events" << std::ends;

		Except::throw_exception((const char *)API_ShutdownInProgress,
									    o.str(),
									   (const char *)"DServer::event_subscription_change");
	}

//
// If the EventSupplier object is not created, create it right now
//

	NotifdEventSupplier *ev;
	if ((ev = tg->get_notifd_event_supplier()) == NULL)
	{
		tg->create_notifd_event_supplier();
		ev = tg->get_notifd_event_supplier();
	}

//
// If we are using a file as database, gives port number to event supplier
//

	if (Util::_FileDb == true && ev != NULL)
	{
		ev->file_db_svr();
	}

    std::string mcast;
    int rate,ivl;

//
// Check if the request comes from a Tango 6 client (without client identification)
// If true, the event has to be sent using AttributeValue_3 data structure
// If cl is NULL, this means that the call is local (Two tango classes within the same process and with events between
// device from class 1 and device from classs 2)
//

	int client_release = 0;
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

	event_subscription(dev_name,attr_name,action,event,attr_name_lower,NOTIFD,mcast,rate,ivl,NULL,client_release);

//
// Init one subscription command flag in Eventsupplier
//

	if (ev != NULL && ev->get_one_subscription_cmd() == false)
		ev->set_one_subscription_cmd(true);

//
// Return to caller
//

	Tango::DevLong ret_val = (Tango::DevLong)tg->get_tango_lib_release();
	return ret_val;
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

void DServer::event_subscription(std::string &dev_name,std::string &obj_name,std::string &action,std::string &event,std::string &obj_name_lower,
								 ChannelType ct,std::string &mcast_data,int &rate,int &ivl,DeviceImpl *dev,int client_lib)
{
    Tango::Util *tg = Tango::Util::instance();

//
// Get device reference
//

	DeviceImpl *dev_impl = dev;

	if (dev_impl == NULL)
	{
        try
        {
            dev_impl = tg->get_device_by_name(dev_name);
        }
        catch (Tango::DevFailed &e)
        {
            TangoSys_OMemStream o;
            o << "Device " << dev_name << " not found" << std::ends;
            Except::re_throw_exception(e,(const char *)API_DeviceNotFound,o.str(),
                                       (const char *)"DServer::event_subscription");
        }
	}

	if (event != EventName[INTERFACE_CHANGE_EVENT] && event != EventName[PIPE_EVENT])
	{

		MultiAttribute *m_attr = dev_impl->get_device_attr();
		int attr_ind = m_attr->get_attr_ind_by_name(obj_name.c_str());
		AttributePrivate &attribute = m_attr->get_attr_by_ind(attr_ind);

//
// Refuse subscription on forwarded attribute and notifd
//

		if (ct == NOTIFD)
		{
			if (attribute.is_fwd_att() == true)
			{
				std::stringstream ss;
				ss << "The attribute " << obj_name << " is a forwarded attribute.";
				ss << "\nIt is not supported to subscribe events from forwarded attribute using Tango < 9. Please update!!";

				Except::throw_exception(API_NotSupportedFeature,
										ss.str(),"DServer::event_subscription");
			}
		}
		else
		{
			if (attribute.is_fwd_att() == true && client_lib < 5)
			{
				std::stringstream ss;
				ss << "The attribute " << obj_name << " is a forwarded attribute.";
				ss << "\nIt is not supported to subscribe events from forwarded attribute using Tango < 9. Please update!!";

				Except::throw_exception(API_NotSupportedFeature,
										ss.str(),"DServer::event_subscription");
			}
		}

		if (action == "subscribe")
		{
			if (event == "user_event")
			{
				cout4 << "DServer::event_subscription(): update user_event subscription\n";

				omni_mutex_lock oml(EventSupplier::get_event_mutex());
				switch (client_lib)
				{
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
			else if (event.find(CONF_TYPE_EVENT) != std::string::npos)
			{
				cout4 << "DServer::event_subscription(): update attr_conf subscription\n";

				omni_mutex_lock oml(EventSupplier::get_event_mutex());
				if (client_lib == 5)
					attribute.event_attr_conf5_subscription = time(NULL);
				else
					attribute.event_attr_conf_subscription = time(NULL);
			}
			else if (event == "data_ready")
			{
				if (attribute.is_fwd_att() == false && attribute.is_data_ready_event() == false)
				{
					TangoSys_OMemStream o;
					o << "The attribute ";
					o << obj_name;
					o << " is not data ready event enabled" << std::ends;

					Except::throw_exception(API_AttributeNotDataReadyEnabled,
											o.str(),
											"DServer::event_subscription");
				}
				cout4 << "DServer::event_subscription(): update data_ready subscription\n";

				omni_mutex_lock oml(EventSupplier::get_event_mutex());
				attribute.event_data_ready_subscription = time(NULL);
			}
			else
			{

//
// If the polling is necessary to send events, check whether the polling is started for the requested attribute.
//

				if (attribute.is_polled() == false )
				{
					TangoSys_OMemStream o;
					o << "The polling (necessary to send events) for the attribute ";
					o << obj_name;
					o << " is not started" << std::ends;

					if ( event == "change")
					{
						if (attribute.is_fwd_att() == false && attribute.is_change_event() == false)
						{
							Except::throw_exception(API_AttributePollingNotStarted,o.str(),
													"DServer::event_subscription");
						}
					}
					else
					{
						if ( event == "archive")
						{
							if (attribute.is_fwd_att() == false && attribute.is_archive_event() == false)
							{
								Except::throw_exception(API_AttributePollingNotStarted,o.str(),
														"DServer::event_subscription");
							}
						}
						else
						{
							if (attribute.is_fwd_att() == false)
								Except::throw_exception(API_AttributePollingNotStarted,o.str(),"DServer::event_subscription");
						}
					}
				}


				if (event == "change")
				{
					cout4 << "DServer::event_subscription(): update change subscription\n";

//
// Check if the attribute has some of the change properties defined
//

					if (obj_name_lower != "state")
					{
						if ((attribute.get_data_type() != Tango::DEV_STRING) &&
							(attribute.get_data_type() != Tango::DEV_BOOLEAN) &&
							(attribute.get_data_type() != Tango::DEV_ENCODED) &&
							(attribute.get_data_type() != Tango::DEV_STATE) &&
							(attribute.get_data_type() != Tango::DEV_ENUM))
						{
							if ( attribute.is_check_change_criteria() == true )
							{
								if ((attribute.rel_change[0] == INT_MAX) &&
									(attribute.rel_change[1] == INT_MAX) &&
									(attribute.abs_change[0] == INT_MAX) &&
									(attribute.abs_change[1] == INT_MAX))
								{
									TangoSys_OMemStream o;
									o << "Event properties (abs_change or rel_change) for attribute ";
									o << obj_name;
									o << " are not set" << std::ends;

									Except::throw_exception(API_EventPropertiesNotSet,
															o.str(),"DServer::event_subscription");
								}
							}
						}
					}

					omni_mutex_lock oml(EventSupplier::get_event_mutex());

					switch (client_lib)
					{
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
				else if (event == "quality")
				{
					cout4 << "DServer::event_subscription(): update quality_change subscription\n";
					attribute.event_quality_subscription = time(NULL);
				}
				else if (event == "periodic")
				{
					cout4 << "DServer::event_subscription(): update periodic subscription\n";

					omni_mutex_lock oml(EventSupplier::get_event_mutex());
					switch (client_lib)
					{
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
				else if (event == "archive")
				{

//
// Check if the attribute has some of the archive properties defined
//

					if (obj_name_lower != "state")
					{
						if ((attribute.get_data_type() != Tango::DEV_STRING) &&
							(attribute.get_data_type() != Tango::DEV_BOOLEAN) &&
							(attribute.get_data_type() != Tango::DEV_ENCODED) &&
							(attribute.get_data_type() != Tango::DEV_STATE) &&
							(attribute.get_data_type() != Tango::DEV_ENUM))
						{
							if ( attribute.is_check_archive_criteria() == true )
							{
								if ((attribute.archive_abs_change[0] == INT_MAX) &&
									(attribute.archive_abs_change[1] == INT_MAX) &&
									(attribute.archive_rel_change[0] == INT_MAX) &&
									(attribute.archive_rel_change[1] == INT_MAX) &&
									(attribute.archive_period        == INT_MAX))
								{
									TangoSys_OMemStream o;
									o << "Archive event properties (archive_abs_change or archive_rel_change or archive_period) for attribute ";
									o << obj_name;
									o << " are not set" << std::ends;

									Except::throw_exception(API_EventPropertiesNotSet,
															o.str(),"DServer::event_subscription");
								}
							}
						}
					}

					cout4 << "DServer::event_subscription(): update archive subscription\n";

					omni_mutex_lock oml(EventSupplier::get_event_mutex());
					switch (client_lib)
					{
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
			}

//
// Set channel type in attribute object
//

			if (ct == ZMQ)
				attribute.set_use_zmq_event();
			else
				attribute.set_use_notifd_event();

//
// Check if multicast has to be used for event transport (only for ZMQ event)
// Don't forget syntax in attribute mcast_event string:
// 			event_name:ip_address:port:rate:ivl
// The last two are not optionals
//

			if (ct == ZMQ)
			{
				bool found = false;

				ZmqEventSupplier *ev;
				ev = tg->get_zmq_event_supplier();
				int zmq_release = ev->get_zmq_release();

				for(unsigned int i = 0;i != attribute.mcast_event.size();++i)
				{
					if (attribute.mcast_event[i].find(event) == 0)
					{
						if (zmq_release < 320)
						{
							int zmq_major,zmq_minor,zmq_patch;
							zmq_version(&zmq_major,&zmq_minor,&zmq_patch);

							TangoSys_OMemStream o;
							o << "Device server process is using zmq release ";
							o << zmq_major << "." << zmq_minor << "." << zmq_patch;
							o << "\nMulticast event(s) not available with this ZMQ release" << std::ends;

							Except::throw_exception(API_UnsupportedFeature,
													o.str(),"DServer::event_subscription");
						}

						std::string::size_type start,end;
						start = attribute.mcast_event[i].find(':');
						start++;
						end = attribute.mcast_event[i].find(':',start);

						if ((end = attribute.mcast_event[i].find(':',end + 1)) == std::string::npos)
						{
							mcast_data = attribute.mcast_event[i].substr(start);
							rate = 0;
							ivl = 0;
							found = true;
							break;
						}
						else
						{
							mcast_data = attribute.mcast_event[i].substr(start,end - start);

//
// Get rate because one is defined
//

							std::string::size_type start_rate = end + 1;
							if ((end = attribute.mcast_event[i].find(':',start_rate)) == std::string::npos)
							{
								std::istringstream iss(attribute.mcast_event[i].substr(start_rate));
								iss >> rate;
								rate = rate * 1024;
								ivl = 0;
								found = true;
								break;
							}
							else
							{
								std::istringstream iss(attribute.mcast_event[i].substr(start_rate,end - start_rate));
								iss >> rate;
								rate = rate * 1024;

//
// Get ivl because one is defined
//

								std::istringstream iss_ivl(attribute.mcast_event[i].substr(end + 1));
								iss_ivl >> ivl;
								ivl = ivl * 1000;
								found = true;
								break;
							}
						}
					}
				}

				if (found == false)
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
			EventType event_type = CHANGE_EVENT;
			tg->event_name_2_event_type(event, event_type);

			omni_mutex_lock oml(EventSupplier::get_event_mutex());
			attribute.set_client_lib(client_lib, event_type);
		}
	}
	else if (event == EventName[PIPE_EVENT])
	{
		if (action == "subscribe")
		{
			DeviceClass *cl = dev_impl->get_device_class();
			Pipe &pi = cl->get_pipe_by_name(obj_name,dev_impl->get_name_lower());

			cout4 << "DServer::event_subscription(): update pipe subscription\n";

			omni_mutex_lock oml(EventSupplier::get_event_mutex());
			pi.set_event_subscription(time(NULL));

// TODO: Pipe: Do we support multicast for pipe event

			rate = 0;
			ivl = 0;
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
		o << "Not enough input arguments, needs at least 4 i.e. device name, attribute/pipe name, action, event name, <Tango lib release>" << std::ends;

		Except::throw_exception((const char *)API_WrongNumberOfArgs,
								o.str(),
								(const char *)"DServer::zmq_event_subscription_change");
	}

    Tango::Util *tg = Tango::Util::instance();
	Tango::DevVarLongStringArray *ret_data = nullptr;

    if (argin->length() == 1)
    {
        std::string arg((*argin)[0]);
        std::transform(arg.begin(),arg.end(),arg.begin(),::tolower);
        if (arg != "info")
        {
            TangoSys_OMemStream o;
            o << "Not enough input arguments, needs 4 i.e. device name, attribute/pipe name, action, event name" << std::ends;

            Except::throw_exception((const char *)API_WrongNumberOfArgs,
                                    o.str(),
                                    (const char *)"DServer::zmq_event_subscription_change");
        }

//
// It's just the call to help debugging. Returns event configuration
//

		ret_data = new Tango::DevVarLongStringArray();
        ret_data->svalue.length(2);

        ret_data->lvalue.length(1);
        ret_data->lvalue[0] = (Tango::DevLong)tg->get_tango_lib_release();

        ZmqEventSupplier *ev;
        if ((ev = tg->get_zmq_event_supplier()) != NULL)
        {
            std::string tmp_str("Heartbeat: ");
            tmp_str = tmp_str + ev->get_heartbeat_endpoint();
            ret_data->svalue[0] = Tango::string_dup(tmp_str.c_str());

            tmp_str = "Event: ";
            std::string ev_end = ev->get_event_endpoint();
            if (ev_end.size() != 0)
				tmp_str = "Event: " + ev_end;
			size_t nb_mcast = ev->get_mcast_event_nb();
			if (nb_mcast != 0)
			{
				if (ev_end.size() != 0)
					tmp_str = tmp_str + "\n";
				tmp_str = tmp_str + "Some event(s) sent using multicast protocol";
			}
            ret_data->svalue[1] = Tango::string_dup(tmp_str.c_str());

            size_t nb_alt = ev->get_alternate_heartbeat_endpoint().size();
            if (nb_alt != 0)
            {
                ret_data->svalue.length((nb_alt + 1) << 1);

                for (size_t loop = 0;loop < nb_alt;loop++)
                {
                    std::string tmp_str("Alternate heartbeat: ");
                    tmp_str = tmp_str + ev->get_alternate_heartbeat_endpoint()[loop];
                    ret_data->svalue[(loop + 1) << 1] = Tango::string_dup(tmp_str.c_str());

                    tmp_str = "Alternate event: ";
                    if (ev->get_alternate_event_endpoint().size() != 0)
                    {
                         std::string ev_end = ev->get_alternate_event_endpoint()[loop];
                         if (ev_end.empty() == false)
                            tmp_str = "Alternate event: " + ev_end;
                    }
                    ret_data->svalue[((loop + 1) << 1) + 1] = Tango::string_dup(tmp_str.c_str());
                }
            }
        }
        else
        {
            ret_data->svalue[0] = Tango::string_dup("No ZMQ event yet!");
        }
    }
    else
    {
        std::string dev_name, obj_name, action, event, obj_name_lower;
        dev_name = (*argin)[0];
        obj_name = (*argin)[1];
        action = (*argin)[2];
        event = (*argin)[3];

//
// Check event type validity
//

        std::string check_event = event;
        std::transform(check_event.begin(),check_event.end(),check_event.begin(),::tolower);

        std::string::size_type pos_check = check_event.find(EVENT_COMPAT);
        if (pos_check != std::string::npos)
            check_event.erase(0,EVENT_COMPAT_IDL5_SIZE);

        size_t nb_event_type = sizeof(EventName)/sizeof(char *);
        bool found = false;

        for (size_t loop = 0;loop < nb_event_type;loop++)
        {
            if (strcmp(check_event.c_str(),EventName[loop]) == 0)
            {
                found = true;
                break;
            }
        }

        if (found == false)
        {
            std::stringstream ss;
            ss << "The event type you sent (" << event << ") is not  valid event type";

            Except::throw_exception(API_WrongNumberOfArgs,ss.str(),"DServer::zmq_event_subscription_change");
        }

        bool intr_change = false;
        if (event == EventName[INTERFACE_CHANGE_EVENT])
			intr_change = true;

        bool pipe_event = false;
        if (event == EventName[PIPE_EVENT])
			pipe_event = true;

        obj_name_lower = obj_name;
        std::transform(obj_name_lower.begin(),obj_name_lower.end(),obj_name_lower.begin(),::tolower);

        int client_release = 4;
		if (event == EventName[ATTR_CONF_EVENT])
			client_release = 3;

        if (argin->length() == 5)
        {
			std::stringstream ss;
			ss << (*argin)[4];
			ss >> client_release;

			if (client_release == 0)
			{
				std::string::size_type pos = event.find(EVENT_COMPAT);
				if (pos != std::string::npos)
				{
					std::string client_lib_str = event.substr(pos + 3,1);
					std::stringstream ss;
					ss << client_lib_str;
					ss >> client_release;
					event.erase(0,EVENT_COMPAT_IDL5_SIZE);
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

        cout4 << "ZmqEventSubscriptionChangeCmd: subscription for device " << dev_name << " attribute/pipe " << obj_name << " action " << action << " event " << event << " client lib = " << client_release << std::endl;

//
// If we receive this command while the DS is in its shuting down sequence, do nothing
//

        if (tg->get_heartbeat_thread_object() == NULL)
        {
            TangoSys_OMemStream o;
            o << "The device server is shutting down! You can no longer subscribe for events" << std::ends;

            Except::throw_exception((const char *)API_ShutdownInProgress,
                                            o.str(),
                                           (const char *)"DServer::zmq_event_subscription_change");
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
            o << "Device " << dev_name << " not found" << std::ends;
            Except::re_throw_exception(e,(const char *)API_DeviceNotFound,o.str(),
                                       (const char *)"DServer::event_subscription");
        }

		long idl_vers = dev->get_dev_idl_version();
        if (idl_vers < 4)
        {
            TangoSys_OMemStream o;

            o << "Device " << dev_name << " too old to use ZMQ event (it does not implement IDL 4)";
            o << "\nSimulate a CommandNotFound exception to move to notifd event system" << std::ends;
            Except::throw_exception((const char *)API_CommandNotFound,
				      o.str(),
				      (const char *)"DServer::zmq_event_subscription_change");
        }

        if (client_release > idl_vers)
			client_release = idl_vers;

//
// Call common method (common between old and new command)
//

        std::string mcast;
        int rate,ivl;

		std::string::size_type pos = event.find(EVENT_COMPAT);
		if (pos != std::string::npos)
			event.erase(0,EVENT_COMPAT_IDL5_SIZE);

        event_subscription(dev_name,obj_name,action,event,obj_name_lower,ZMQ,mcast,rate,ivl,dev,client_release);

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

        std::string ev_name = ev->get_fqdn_prefix();
        if (Util::_FileDb == true)
        {
            int size = ev_name.size();
            if (ev_name[size - 1] == '#')
                ev_name.erase(size - 1);
        }

        ev_name = ev_name + dev->get_name_lower();
        if (intr_change == false)
			ev_name = ev_name + '/' + obj_name_lower;
        if (Util::_FileDb == true && ev != NULL)
            ev_name = ev_name + MODIFIER_DBASE_NO;
        ev_name = ev_name + '.' +  event;

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
            ev->create_mcast_event_socket(mcast,ev_name,rate,local_call);
        else
            ev->create_event_socket();

//
// Init event counter in Event Supplier
//

		ev->init_event_cptr(ev_name);

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
			AttributePrivate &attribute = dev->get_device_attr()->get_attr_by_name(obj_name.c_str());
			EventType et;
			tg->event_name_2_event_type(event,et);

			if (attribute.is_fwd_att() == true && et != ATTR_CONF_EVENT)
			{
				FwdAttribute &fwd_att = static_cast<FwdAttribute &>(attribute);
				std::string root_name = fwd_att.get_fwd_dev_name() + "/" + fwd_att.get_fwd_att_name();
				RootAttRegistry &rar = tg->get_root_att_reg();
				bool already_there = rar.is_event_subscribed(root_name,et);

//
// We unsubscribe and subscribe. This is mandatory for following case: The appli is killed and re-started
// but in the meantime, polling for the root attribute has been stopped. The error that the polling is not started
// for the root attribute is sent at subscription time
//

				if (already_there == true)
					rar.unsubscribe_user_event(fwd_att.get_fwd_dev_name(),fwd_att.get_fwd_att_name(),et);
				rar.subscribe_user_event(fwd_att.get_fwd_dev_name(),fwd_att.get_fwd_att_name(),et);
			}
		}

//
// Init data returned by command
//

		ret_data = new Tango::DevVarLongStringArray();
        ret_data->lvalue.length(6);
        ret_data->svalue.length(2);

        ret_data->lvalue[0] = (Tango::DevLong)tg->get_tango_lib_release();
        ret_data->lvalue[1] = dev->get_dev_idl_version();
        ret_data->lvalue[2] = zmq_sub_event_hwm;
        ret_data->lvalue[3] = rate;
        ret_data->lvalue[4] = ivl;
        ret_data->lvalue[5] = ev->get_zmq_release();

        std::string &heartbeat_endpoint = ev->get_heartbeat_endpoint();
        ret_data->svalue[0] = Tango::string_dup(heartbeat_endpoint.c_str());
        if (mcast.empty() == true)
        {
            std::string &event_endpoint = ev->get_event_endpoint();
            ret_data->svalue[1] = Tango::string_dup(event_endpoint.c_str());
        }
        else
        {
            if (local_call == true)
            {
                std::string &event_endpoint = ev->get_event_endpoint();
                ret_data->svalue[1] = Tango::string_dup(event_endpoint.c_str());
            }
            else
            {
                std::string &event_endpoint = ev->get_mcast_event_endpoint(ev_name);
                ret_data->svalue[1] = Tango::string_dup(event_endpoint.c_str());
            }
        }

        size_t nb_alt = ev->get_alternate_heartbeat_endpoint().size();
        if (nb_alt != 0)
        {
            ret_data->svalue.length((nb_alt + 1) << 1);

            for (size_t loop = 0;loop < nb_alt;loop++)
            {
                std::string tmp_str = ev->get_alternate_heartbeat_endpoint()[loop];
                ret_data->svalue[(loop + 1) << 1] = Tango::string_dup(tmp_str.c_str());

                tmp_str = ev->get_alternate_event_endpoint()[loop];
                ret_data->svalue[((loop + 1) << 1) + 1] = Tango::string_dup(tmp_str.c_str());
            }
        }

        //add two more
        size_t size = ret_data->svalue.length();
        ret_data->svalue.length(size + 2);

        std::string event_topic = "";
        bool add_compat_info = false;
        if ((event != EventName[PIPE_EVENT]) &&
            (event != EventName[INTERFACE_CHANGE_EVENT]) &&
            (event != EventName[DATA_READY_EVENT]))
        {
            add_compat_info = true;
        }
        if(client_release >= 5 && add_compat_info) // client_release here is the minimum of the client release and dev IDL version
        {
            event_topic = ev->create_full_event_name(dev, EVENT_COMPAT_IDL5 + event, obj_name_lower, intr_change);
        }
        else
        {
            event_topic = ev->create_full_event_name(dev, event, obj_name_lower, intr_change);
        }
        assert(!(event_topic.empty()));
        cout4 << "Sending event_topic = " << event_topic << std::endl;
        ret_data->svalue[size] = Tango::string_dup(event_topic.c_str());

        std::string channel_name = ev->get_fqdn_prefix();
        char * adm_name = dev->adm_name();
        channel_name += adm_name;
        Tango::string_free(adm_name);
        std::transform(channel_name.begin(), channel_name.end(), channel_name.begin(), ::tolower);
        assert(!(channel_name.empty()));
        cout4 << "Sending channel_name = " << channel_name << std::endl;
        ret_data->svalue[size + 1] = Tango::string_dup(channel_name.c_str());
    }

	return ret_data;
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


	Tango::Util *tg = Tango::Util::instance();

//
// A loop for each event
//

    unsigned int nb_event = argin->length() / 3;

	std::string old_dev;
	DeviceImpl *dev = NULL;

	for (unsigned int loop = 0;loop < nb_event;loop++)
	{
		std::string dev_name, obj_name, event, obj_name_lower;
		int base = loop * 3;
		dev_name = (*argin)[base];
		obj_name = (*argin)[base + 1];
		event = (*argin)[base + 2];

		obj_name_lower = obj_name;
		std::transform(obj_name_lower.begin(),obj_name_lower.end(),obj_name_lower.begin(),::tolower);

		cout4 << "EventConfirmSubscriptionCmd: confirm subscription for device " << dev_name << " attribute/pipe " << obj_name << " event " << event << std::endl;

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
            	o << "Device " << dev_name << " not found" << std::ends;
            	Except::re_throw_exception(e,(const char *)API_DeviceNotFound,o.str(),
             	                          (const char *)"DServer::event_confirm_subscription");
        	}

			old_dev = dev_name;
		}

//
// Call common method (common between old and new command)
//

		std::string mcast;
		int rate,ivl,client_lib;

		std::string action("subscribe");
		std::string client_lib_str;

		std::string::size_type pos = event.find(EVENT_COMPAT);
		if (pos != std::string::npos)
		{
			client_lib_str = event.substr(pos + 3,1);
			std::stringstream ss;
			ss << client_lib_str;
			ss >> client_lib;
			event.erase(0,EVENT_COMPAT_IDL5_SIZE);
		}
		else
		{
			if (event == EventName[ATTR_CONF_EVENT])
				client_lib = 3;
			else
				client_lib = 4;		// Command implemented only with Tango 8 -> IDL 4 for event data
		}

		event_subscription(dev_name,obj_name,action,event,obj_name_lower,ZMQ,mcast,rate,ivl,dev,client_lib);
	}

}


}	// namespace

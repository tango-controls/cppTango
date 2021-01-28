//====================================================================================================================
//
//  file :  			event.cpp
//
//	description : 		C++ classes for implementing the event server and client singleton classes - EventConsumer.
//
//	author(s) : 		A.Gotz (goetz@esrf.fr)
//
//	original : 			7 April 2003
//
//  Copyright (C) :     2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision: 30155 $
//
//
//====================================================================================================================

#include <tango.h>
#include <eventconsumer.h>
#include <event.tpp>
#include <tango_clock.h>

#include <stdio.h>

#ifdef _TG_WINDOWS_
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace CORBA;

namespace Tango {

EventConsumerKeepAliveThread *EventConsumer::keep_alive_thread = NULL;
std::map<std::string,std::string> EventConsumer::device_channel_map;
std::map<std::string,EventChannelStruct> EventConsumer::channel_map;
std::map<std::string,EventCallBackStruct> EventConsumer::event_callback_map;
ReadersWritersLock 	EventConsumer::map_modification_lock;

std::vector<EventNotConnected> EventConsumer::event_not_connected;
int EventConsumer::subscribe_event_id = 0;
std::vector<std::string> EventConsumer::env_var_fqdn_prefix;
std::map<std::string,std::string> EventConsumer::alias_map;

KeepAliveThCmd EventConsumer::cmd;

//+--------------------------------------------------------------------------------------------------------------------
//
// function :
//		leavefunc
//
// description :
//		This function will be executed at process exit or when the main returned.  It has to be executed to properly
//		shutdown and destroy the ORB used by as a server by the event system. The ORB loop is in EventConsumer thread.
//		Therefore, get a reference to it, shutdown the ORB and wait until the thread exit. It also destroys the
//		heartbeat filters
//
//--------------------------------------------------------------------------------------------------------------------

void leavefunc()
{
	// flag to avoid calling leavefunc twice
	static bool already_executed = false;

	Tango::ApiUtil *au = ApiUtil::instance();

	if (au->need_reset_already_flag() == true)
		already_executed = false;

//
// Kill locking threads (if any)
//

	if (already_executed == false)
		au->clean_locking_threads();

//
// Manage event stuff
//

	NotifdEventConsumer *notifd_ec = au->get_notifd_event_consumer();

	if (notifd_ec != NULL && already_executed == false)
	{
		notifd_ec->shutdown();

//
// Shut-down the notifd ORB and wait for the thread to exit
//

		int *rv;
		notifd_ec->orb_->shutdown(true);
		notifd_ec->join((void **)&rv);
	}

	ZmqEventConsumer *zmq_ec = au->get_zmq_event_consumer();

	if (zmq_ec != NULL && already_executed == false)
	{
		zmq_ec->shutdown();
	}

//
// Shutdown and destroy the ORB
//

	if (already_executed == false)
	{
		if (notifd_ec == NULL)
		{
			CORBA::ORB_ptr orb = au->get_orb();
			orb->shutdown(true);
			orb->destroy();
		}
		already_executed = true;
		au->need_reset_already_flag(false);
	}
}


//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::EventConsumer
//
// description :
//		Constructor for the EventConsumer class
//
//--------------------------------------------------------------------------------------------------------------------

EventConsumer::EventConsumer(ApiUtil *api_ptr)
{

//
// Build and store the fqdn prefix for devices in the TANGO_HOST environment variable (in lower case letters)
//

    if (env_var_fqdn_prefix.empty() == true)
    {
        try
        {
            Database *db = (api_ptr->get_db_vect())[api_ptr->get_db_ind()];
            std::string prefix = "tango://" + db->get_db_host() + ':' + db->get_db_port() + '/' ;
            env_var_fqdn_prefix.push_back(prefix);

            if (db->is_multi_tango_host() == true)
            {
                std::vector<std::string> &tango_hosts = db->get_multi_host();
                std::vector<std::string> &tango_ports = db->get_multi_port();
                for (unsigned int i = 1;i < tango_hosts.size();i++)
                {
                    std::string prefix = "tango://" + tango_hosts[i] + ':' + tango_ports[i] + '/' ;
                    env_var_fqdn_prefix.push_back(prefix);
                }
            }

            for (size_t loop = 0;loop < env_var_fqdn_prefix.size();++loop)
                std::transform(env_var_fqdn_prefix[loop].begin(),env_var_fqdn_prefix[loop].end(),env_var_fqdn_prefix[loop].begin(),::tolower);

//
// Also get Db server defined in DB but not in the user TANGO_HOST env. variable
//

			get_cs_tango_host(db);
		}
		catch (Tango::DevFailed &)
		{
			env_var_fqdn_prefix.push_back(TangoHostNotSet);
		}
    }

//
// initialise the unique event id for the client;
//

    if (keep_alive_thread == NULL)
        subscribe_event_id = 0;

//
// Install a function to be executed at exit. This is the only way I found to properly shutdown and destroy the ORB.
// Don't do this for windows DLL.
//
// Is this necessary when events are used within a server ?
//

#ifndef _USRDLL
	if ((api_ptr->in_server() == false) && (api_ptr->is_lock_exit_installed() == false))
	{
		atexit(leavefunc);
		api_ptr->set_sig_handler();
		api_ptr->set_lock_exit_installed(true);
	}
#endif

//
// Miscellaneous init
//

	cmd.cmd_pending = false;
    if (keep_alive_thread == NULL)
        api_ptr->need_reset_already_flag(true);

    thread_id = 0;

//
// Create and start the EventConsumerKeepAliveThread. Do this at the method's end because the keep_alive_thread
// ptr is also used as a "init done" flag.
//

    if (keep_alive_thread == NULL)
    {
        keep_alive_thread = new EventConsumerKeepAliveThread(cmd);
        keep_alive_thread->start();
    }

}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_cs_tango_host()
//
// description :
//		Get from a Tango control system which TANGO_HOST are available. This info comes from the Tango CS database
//
// argument :
//		in :
//			- db : Control system database pointer
//
//--------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_cs_tango_host(Database *db)
{
	try
	{
		DeviceData dd;
		dd = db->command_inout("DbGetCSDbServerList");
		std::vector<std::string> vs;
		dd >> vs;

//
// Do we have a CS with a host alias used as TANGO_HOST?
// It true and if we don't know this alias, store its definition in the alias map
//

		if (vs.size() == 1)
		{
			std::string lower_vs(vs[0]);
			std::transform(lower_vs.begin(),lower_vs.end(),lower_vs.begin(),::tolower);
			std::string::size_type pos = lower_vs.find(':');
			if (pos != std::string::npos)
				lower_vs.erase(pos);

			std::string tg_host(db->get_orig_tango_host());
			std::transform(tg_host.begin(),tg_host.end(),tg_host.begin(),::tolower);

			if (tg_host.empty() == false && lower_vs != tg_host)
			{
				if (alias_map.find(tg_host) == alias_map.end())
				{
					alias_map.insert({lower_vs,tg_host});
				}
			}
		}

//
// Serveral Db servers for one TANGO_HOST case
//

		std::vector<std::string>::iterator pos;

		for (unsigned int i = 0;i < vs.size();i++)
		{
			std::transform(vs[i].begin(),vs[i].end(),vs[i].begin(),::tolower);
			pos = find_if(env_var_fqdn_prefix.begin(),env_var_fqdn_prefix.end(),
					[&] (std::string str) -> bool
					{
						if (str.find(vs[i]) != std::string::npos)
							return true;
						else
							return false;
					});

			if (pos == env_var_fqdn_prefix.end())
			{
				std::string prefix = "tango://" + vs[i] + '/' ;
				env_var_fqdn_prefix.push_back(prefix);
			}
		}
	}
	catch(...) {}
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::shutdown()
//
// description :
//		Method to stop the keep alive thread and to disconnect from all used event channels
//
//--------------------------------------------------------------------------------------------------------------------
void EventConsumer::shutdown()
{
	cout3 << "calling Tango::EventConsumer::shutdown() \n";

//
// First, shutdown the keep alive thread
//

    shutdown_keep_alive_thread();

//
// cleanup map containers
//

	cleanup_EventChannel_map();
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::shutdown_keep_alive_thread()
//
// description : 	Ask the KeepAliveThread to suicide and wait for this to
//                  happen
//
//-----------------------------------------------------------------------------

void EventConsumer::shutdown_keep_alive_thread()
{
//
// Shut-down the KeepAliveThread and wait for it to exit
//

    if (keep_alive_thread != NULL)
    {
        {
            omni_mutex_lock sync(cmd);

            cmd.cmd_pending = true;
            cmd.cmd_code = EXIT_TH;

            cmd.cond.signal();
        }

        int *rv;
        keep_alive_thread->join((void **)&rv);

        keep_alive_thread = NULL;
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::connect()
//
// description :
//		This method is a wrapper around the connection to the event channel (heartbeat event)
//
// argument :
//		in :
//			- device_proxy : The device handle
//          - d_name : The FQDN (lower case)
//          - dd : The server command result (Used by ZMQ event system only)
//          - adm_name : The admin device name
//			- necm :
//
//------------------------------------------------------------------------------------------------------------------

void EventConsumer::connect(DeviceProxy *device_proxy,std::string &d_name,DeviceData &dd,std::string &adm_name,bool &necm)
{
	std::string channel_name = adm_name;


//
// If no connection exists to this channel then connect to it. Sometimes, this method is called in order to reconnect
// to the notifd. In such a case, the lock is already locked before the method is called
//

	std::map<std::string,EventChannelStruct>::iterator ipos = channel_map.find(channel_name);
	if (ipos == channel_map.end())
	{
		connect_event_channel(channel_name,device_proxy->get_device_db(),false,dd);
	}

//
// Init adm device name in channel map entry
//

	if (ipos == channel_map.end())
	{
		channel_map[channel_name].full_adm_name = adm_name;
		necm = true;
	}

//
// Add entry in device_channel_map map
//

	device_channel_map[d_name] = channel_name;
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::attr_to_device()
//
// description :
//		Method to initialize in the DeviceAttribute instance given to the user the attribute value which are received
//      in a AttrValUnion (or in a class inheriting from)
//
// argument :
//		in :
//			- attr_value :
//			- attr_value_3 :
//			- vers :
//		out :
//			- dev_attr :
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::attr_to_device(const AttributeValue *attr_value,
				   const AttributeValue_3 *attr_value_3,
				   long vers,DeviceAttribute *dev_attr)
{
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarLong64Array *tmp_seq_64;
	CORBA::LongLong *tmp_64;
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
	const DevVarULongArray *tmp_seq_ulo;
	CORBA::ULong *tmp_ulo;
	const DevVarULong64Array *tmp_seq_u64;
	CORBA::ULongLong *tmp_ulolo;
	const DevVarStateArray *tmp_seq_state;
	Tango::DevState *tmp_state;

	CORBA::ULong max,len;

	if (vers == 3)
	{
		dev_attr->name = attr_value_3->name;
		dev_attr->quality = attr_value_3->quality;
		dev_attr->time = attr_value_3->time;
		dev_attr->dim_x = attr_value_3->r_dim.dim_x;
		dev_attr->dim_y = attr_value_3->r_dim.dim_y;
		dev_attr->set_w_dim_x(attr_value_3->w_dim.dim_x);
		dev_attr->set_w_dim_y(attr_value_3->w_dim.dim_y);
		dev_attr->err_list = new DevErrorList(attr_value_3->err_list);
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
		if (vers == 3)
			ty = attr_value_3->value.type();
		else
			ty = attr_value->value.type();

		if (ty->kind() == tk_enum)
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
					if (vers == 3)
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

				case tk_longlong:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_64;
					else
						attr_value->value >>= tmp_seq_64;
					max = tmp_seq_64->maximum();
					len = tmp_seq_64->length();
					if (tmp_seq_64->release() == true)
					{
						tmp_64 = (const_cast<DevVarLong64Array *>(tmp_seq_64))->get_buffer((CORBA::Boolean)true);
						dev_attr->Long64Seq = new DevVarLong64Array(max,len,tmp_64,true);
					}
					else
					{
						tmp_64 = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
						dev_attr->Long64Seq = new DevVarLong64Array(max,len,tmp_64,false);
					}
					break;

				case tk_short:
					if (vers == 3)
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
					if (vers == 3)
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
					if (vers == 3)
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
					if (vers == 3)
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
					if (vers == 3)
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
					if (vers == 3)
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
					if (vers == 3)
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

				case tk_ulong:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_ulo;
					else
						attr_value->value >>= tmp_seq_ulo;
					max = tmp_seq_ulo->maximum();
					len = tmp_seq_ulo->length();
					if (tmp_seq_ulo->release() == true)
					{
						tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean)true);
						dev_attr->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
					}
					else
					{
						tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq_ulo->get_buffer());
						dev_attr->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
					}
					break;

				case tk_ulonglong:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_u64;
					else
						attr_value->value >>= tmp_seq_u64;
					max = tmp_seq_u64->maximum();
					len = tmp_seq_u64->length();
					if (tmp_seq_u64->release() == true)
					{
						tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_u64))->get_buffer((CORBA::Boolean)true);
						dev_attr->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
					}
					else
					{
						tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq_u64->get_buffer());
						dev_attr->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
					}
					break;

				case tk_enum:
					if (vers == 3)
						attr_value_3->value >>= tmp_seq_state;
					else
						attr_value->value >>= tmp_seq_state;
					max = tmp_seq_state->maximum();
					len = tmp_seq_state->length();
					if (tmp_seq_state->release() == true)
					{
						tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean)true);
						dev_attr->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
					}
					else
					{
						tmp_state = const_cast<Tango::DevState *>(tmp_seq_state->get_buffer());
						dev_attr->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
					}
					break;

				default:
					break;
				}
		}
	}
}

void EventConsumer::attr_to_device(const AttributeValue_4 *attr_value_4,DeviceAttribute *dev_attr)
{
	dev_attr->name = attr_value_4->name;
	dev_attr->quality = attr_value_4->quality;
	dev_attr->time = attr_value_4->time;
	dev_attr->dim_x = attr_value_4->r_dim.dim_x;
	dev_attr->dim_y = attr_value_4->r_dim.dim_y;
	dev_attr->set_w_dim_x(attr_value_4->w_dim.dim_x);
	dev_attr->set_w_dim_y(attr_value_4->w_dim.dim_y);
	dev_attr->err_list = new DevErrorList(attr_value_4->err_list);

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
	    att_union_to_device(&attr_value_4->value,dev_attr);
	}
	//
	// Warning: Since Tango 9, data type SHORT is used for both short attribute and enumeration attribute!
	// Therefore, we need to store somewhere which exact type it is. With IDL 5, it is easy, because the data type is
	// transferred on the network (modified IDL), For previous release, we do not have enumerated data type and therefore
	// the data type could be used only for SHORT.
	//
	dev_attr->data_type = Tango::DEV_SHORT;
	dev_attr->data_format = attr_value_4->data_format;
}


void EventConsumer::attr_to_device(const ZmqAttributeValue_4 *attr_value_4,DeviceAttribute *dev_attr)
{
	base_attr_to_device(attr_value_4,dev_attr);

	//
	// Warning: Since Tango 9, data type SHORT is used for both short attribute and enumeration attribute!
	// Therefore, we need to store somewhere which exact type it is. With IDL 5, it is easy, because the data type is
	// transferred on the network (modified IDL), For previous release, we do not have enumerated data type and therefore
	// the data type could be used only for SHORT.
	//
	dev_attr->data_type = Tango::DEV_SHORT;
}

void EventConsumer::attr_to_device(const ZmqAttributeValue_5 *attr_value_5,DeviceAttribute *dev_attr)
{
	base_attr_to_device(attr_value_5,dev_attr);
	dev_attr->data_type = attr_value_5->data_type;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::att_union_to_device()
//
// description :
//		Method to initialize in the DeviceAttribute instance given to the user the attribute value which are received
//      in a AttrValUnion (or in a class inheriting from)
//
// argument :
//		in :
//			- union_ptr : Pointer to the received union
//		out :
//			- dev_attr  : Pointer to the DeviceAttribute which will be given to the user
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::att_union_to_device(const AttrValUnion *union_ptr,DeviceAttribute *dev_attr)
{
	CORBA::Long *tmp_lo;
	CORBA::Short *tmp_sh;
	CORBA::Double *tmp_db;
	char **tmp_str;
	CORBA::Float *tmp_fl;
	CORBA::Boolean *tmp_boo;
	CORBA::UShort *tmp_ush;
	CORBA::Octet *tmp_uch;
	CORBA::LongLong *tmp_lolo;
	CORBA::ULong *tmp_ulo;
	CORBA::ULongLong *tmp_ulolo;
	Tango::DevState *tmp_state;
	Tango::DevState sta_dev;
	Tango::DevEncoded *tmp_enc;

	CORBA::ULong max,len;

    switch (union_ptr->_d())
    {
        case ATT_BOOL:
        {
            const DevVarBooleanArray &tmp_seq = union_ptr->bool_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_boo = (const_cast<DevVarBooleanArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
            }
            else
            {
                tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq.get_buffer());
                dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,false);
            }
        }
        break;

        case ATT_SHORT:
        {
            const DevVarShortArray &tmp_seq = union_ptr->short_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_sh = (const_cast<DevVarShortArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
            }
            else
            {
                tmp_sh = const_cast<CORBA::Short *>(tmp_seq.get_buffer());
                dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
            }
        }
        break;

        case ATT_LONG:
        {
            const DevVarLongArray &tmp_seq = union_ptr->long_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_lo = (const_cast<DevVarLongArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
            }
            else
            {
                tmp_lo = const_cast<CORBA::Long *>(tmp_seq.get_buffer());
                dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
            }
        }
        break;

        case ATT_LONG64:
        {
            const DevVarLong64Array &tmp_seq = union_ptr->long64_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_lolo = (const_cast<DevVarLong64Array &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,true);
            }
            else
            {
                tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq.get_buffer());
                dev_attr->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,false);
            }
        }
        break;

        case ATT_FLOAT:
        {
            const DevVarFloatArray &tmp_seq = union_ptr->float_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_fl = (const_cast<DevVarFloatArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
            }
            else
            {
                tmp_fl = const_cast<CORBA::Float *>(tmp_seq.get_buffer());
                dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,false);
            }
        }
        break;

        case ATT_DOUBLE:
        {
            const DevVarDoubleArray &tmp_seq = union_ptr->double_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_db = (const_cast<DevVarDoubleArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
            }
            else
            {
                tmp_db = const_cast<CORBA::Double *>(tmp_seq.get_buffer());
                dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
            }
        }
        break;

        case ATT_UCHAR:
        {
            const DevVarCharArray &tmp_seq = union_ptr->uchar_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_uch = (const_cast<DevVarCharArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
            }
            else
            {
                tmp_uch = const_cast<CORBA::Octet *>(tmp_seq.get_buffer());
                dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,false);
            }
        }
        break;

        case ATT_USHORT:
        {
            const DevVarUShortArray &tmp_seq = union_ptr->ushort_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_ush = (const_cast<DevVarUShortArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
            }
            else
            {
                tmp_ush = const_cast<CORBA::UShort *>(tmp_seq.get_buffer());
                dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,false);
            }
        }
        break;

        case ATT_ULONG:
        {
            const DevVarULongArray &tmp_seq = union_ptr->ulong_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_ulo = (const_cast<DevVarULongArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
            }
            else
            {
                tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq.get_buffer());
                dev_attr->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
            }
        }
        break;

        case ATT_ULONG64:
        {
            const DevVarULong64Array &tmp_seq = union_ptr->ulong64_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_ulolo = (const_cast<DevVarULong64Array &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
            }
            else
            {
                tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq.get_buffer());
                dev_attr->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
            }
        }
        break;

        case ATT_STRING:
        {
            const DevVarStringArray &tmp_seq = union_ptr->string_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_str = (const_cast<DevVarStringArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,true);
            }
            else
            {
                tmp_str = const_cast<char **>(tmp_seq.get_buffer());
                dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,false);
            }
        }
        break;

        case ATT_STATE:
        {
            const DevVarStateArray &tmp_seq = union_ptr->state_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_state = (const_cast<DevVarStateArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
            }
            else
            {
                tmp_state = const_cast<Tango::DevState *>(tmp_seq.get_buffer());
                dev_attr->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
            }
        }
        break;

        case DEVICE_STATE:
            sta_dev = union_ptr->dev_state_att();
            dev_attr->d_state_filled = true;
            dev_attr->d_state = sta_dev;
        break;

        case ATT_NO_DATA:
        break;

        case ATT_ENCODED:
        {
            const DevVarEncodedArray &tmp_seq = union_ptr->encoded_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_enc = (const_cast<DevVarEncodedArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->EncodedSeq = new DevVarEncodedArray(max,len,tmp_enc,true);
            }
            else
            {
                tmp_enc = const_cast<Tango::DevEncoded *>(tmp_seq.get_buffer());
                dev_attr->EncodedSeq = new DevVarEncodedArray(max,len,tmp_enc,false);
            }
        }
        break;
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::subscribe_event()
//
// description :
//		Method to subscribe to an event with the callback mechanism. Can be called in a stateless way, that it even
//		works when the attribute is not available.
//
// argument :
//		in :
//			- device    : The device handle
//			- attribute : The name of the attribute
//			- event     : The type of event to subscribe for
//			- callback  : A pointer to the callback object
//			- filters   : Eventual event filter strings
//          - stateless : Flag to enable the stateless connection when set to true
//
//-------------------------------------------------------------------------------------------------------------------

int EventConsumer::subscribe_event (DeviceProxy *device,
				   const std::string &attribute,
				   EventType event,
				   CallBack *callback,
				   const std::vector<std::string> &filters,
					bool stateless)
{
	if ((device == NULL) || (callback == NULL))
	{
		EventSystemExcept::throw_exception(API_InvalidArgs,
                       	"Device or callback pointer NULL","EventConsumer::subscribe_event()");
	}

	return (subscribe_event (device, attribute, event, callback, NULL, filters, stateless));
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::subscribe_event()
//
// description :
//		Method to subscribe to an event with the event_queue mechanism. Can be called in a stateless way, that it even
//		works when the attribute is not available.
//
// argument :
//		in :
//			- device    : The device handle
//			- attribute : The name of the attribute
//			- event     : The type of event to subscribe for
//			- event_queue_size:  The size of the circular buffer for incoming events
//			- filters   : Eventual event filter strings
//          - stateless : Flag to enable the stateless connection when set to true
//
//-------------------------------------------------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const std::string &attribute,
				   EventType event,
				   int event_queue_size,
				   const std::vector<std::string> &filters,
				   bool stateless)
{
	if ((device == NULL) || (event_queue_size < 0))
	{
		EventSystemExcept::throw_exception(API_InvalidArgs,
                       	"Device pointer is NULL or the event queue size is invalid",
                       	"EventConsumer::subscribe_event()");
	}

	// create an event queue object

	EventQueue *ev_queue = new EventQueue(event_queue_size);

	return (subscribe_event (device, attribute, event, NULL, ev_queue, filters, stateless));
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::subscribe_event()
//
// description :
//
// argument :
//		in :
//			- device : Pointer to the DeviceProxy instance
//			- attribute : The attribute name
//			- event : The event type
//			- callback : Pointer to the callback object
//			- ev_queue : Pointer to the eveent queue
//			- filters : Event filter (unused starting with Tango 8)
//			- stateless : Stateless subsription flag
//
//--------------------------------------------------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const std::string &attribute,
				   EventType event,
				   CallBack *callback,
				   EventQueue *ev_queue,
				   const std::vector<std::string> &filters,
				   bool stateless)
{
	std::string event_name;
	if (event == QUALITY_EVENT)
	{
        EventSystemExcept::throw_exception(API_InvalidArgs,
            "The quality change event does not exist any more. A change event is fired on a quality change!",
            "EventConsumer::subscribe_event()");
	}
    else
        event_name = EventName[event];

//
// Following code is for the case of event subscription in one event callback
// In such a case, we have to do  the subscription in a thread  otherwise we have a deadlock due to the
// event consumer thread which can not at the same time execute the call back and register the new event
//

    if (thread_id != 0)
    {
        omni_thread::ensure_self se;
        if (omni_thread::self()->id() == thread_id)
        {
            if (stateless == false)
            {
                EventSystemExcept::throw_exception(API_InvalidArgs,
                "When subscribing to event within a event callback, only stateless subscription is allowed",
                "EventConsumer::subscribe_event()");
            }

            subscribe_event_id++;
            int ret_event_id = subscribe_event_id;

            DelayedEventSubThread *th = new DelayedEventSubThread(this,device,attribute,event,callback,ev_queue,event_name,ret_event_id);
            th->start();

            return ret_event_id;
        }
    }

//
// Take a writer lock right now and not in the connect_event method. In case of stateless subscription and if the
// device is not there, the lock will still be valid when the data will be inserted into the vector of non-connected
// events. Also ask the main ZMQ thread to delay all incoming event until this method exit.
// A dead lock could happen if we don't do this (really experienced!)
//

    DelayEvent de(this);
	WriterLock w(map_modification_lock);

	try
	{
		int event_id = connect_event (device,attribute,event,callback,ev_queue,filters,event_name);
		return event_id;
	}
	catch (Tango::DevFailed &e)
	{
	    std::string reason(e.errors[0].reason.in());
		// if the stateless flag is not true, rethrow the exception
		if ((stateless == false) || (reason == API_CommandNotFound))
		{
			throw;
		}

		// when the subscribe event has not worked, store the connection data in a vector of not
		// yet connected events.
		// Retry to connect in the next heartbeat period.

		EventNotConnected conn_params;
		conn_params.device           = device;
		conn_params.attribute        = attribute;
		conn_params.event_type       = event;
		conn_params.event_name       = event_name;
		conn_params.callback         = callback;
		conn_params.ev_queue         = ev_queue;
		conn_params.filters          = filters;
		conn_params.last_heartbeat   = time(NULL);
		if (env_var_fqdn_prefix.empty() == false)
			conn_params.prefix			 = env_var_fqdn_prefix[0];
		// protect the vector as the other maps!

		// create and save the unique event ID
		subscribe_event_id++;
		conn_params.event_id = subscribe_event_id;

		event_not_connected.push_back (conn_params);

		std::vector<EventNotConnected>::iterator vpos = event_not_connected.end() - 1;
		time_t now = time(NULL);
		keep_alive_thread->stateless_subscription_failed(vpos,e,now);
		return subscribe_event_id;
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::subscribe_event()
//
// description :
//		Method to subscribe to an event with the callback mechanism. Can be called in a stateless way, that it even
//		works when the attribute is not available.
//
// argument :
//		in :
//			- device    : The device handle
//			- event     : The type of event to subscribe for
//			- callback  : A pointer to the callback object
//          - stateless : Flag to enable the stateless connection when set to true
//
//-------------------------------------------------------------------------------------------------------------------

int EventConsumer::subscribe_event (DeviceProxy *device,
				   EventType event,
				   CallBack *callback,
				   bool stateless)
{
	if ((device == NULL) || (callback == NULL) || (event != INTERFACE_CHANGE_EVENT))
	{
		EventSystemExcept::throw_exception(API_InvalidArgs,
                       	"Device,callback pointer NULL or unsupported event type","EventConsumer::subscribe_event()");
	}

	std::vector<std::string> filters;

	return (subscribe_event(device,"dummy",event,callback,NULL,filters,stateless));
}

int EventConsumer::subscribe_event (DeviceProxy *device,
				   EventType event,
				   int event_queue_size,
				   bool stateless)
{
	if ((device == NULL) || (event != INTERFACE_CHANGE_EVENT))
	{
		EventSystemExcept::throw_exception(API_InvalidArgs,
                       	"Device NULL or unsupported event type","EventConsumer::subscribe_event()");
	}

	std::vector<std::string> filters;

// create an event queue object

	EventQueue *ev_queue = new EventQueue(event_queue_size);
	return (subscribe_event(device,"dummy",event,NULL,ev_queue,filters,stateless));
}
//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::connect_event()
//
// description :
//		Main nethod called by the subsccribe_event call
//
// argument :
//		in :
//			- device : The device handle
//			- obj_name : The attribute or pipe name
//			- event : The type of event to subscribe for
//			- callback : A pointer to the callback object
//			- ev_queue : A pointer to the event queue
//			- filters : Eventual event filter strings
//			- event_name : The event name
//          - event_id  : the unique event ID
//
//--------------------------------------------------------------------------------------------------------------------

int EventConsumer::connect_event(DeviceProxy *device,
				   const std::string &obj_name,
				   EventType event,
				   CallBack *callback,
				   EventQueue *ev_queue,
				   const std::vector<std::string> &filters,
				   std::string &event_name,
				   int event_id)
{
	int ret_event_id = event_id;
	device_name = device->dev_name();//TODO convert to local
	cout3 << "Tango::EventConsumer::connect_event(" << device_name << "," << obj_name <<"," << event << ")\n";

	bool inter_event = false;
	if (event == INTERFACE_CHANGE_EVENT)
		inter_event = true;

	bool pipe_event = false;
	if (event == PIPE_EVENT)
		pipe_event = true;

//
// Build callback map key and local device name from fqdn
//

	std::string local_device_name(device_name);
	if (device->get_from_env_var() == false)
	{
		std::string prot("tango://");
		if (device->is_dbase_used() == false)
        {
            std::string &ho = device->get_dev_host();
            if (ho.find('.') == std::string::npos)
                Connection::get_fqdn(ho);
 			prot = prot + ho + ':' + device->get_dev_port() + '/';
        }
		else
			prot = prot + device->get_db_host() + ':' + device->get_db_port() + '/';
		device_name.insert(0,prot);
		if (device->is_dbase_used() == false)
			device_name = device_name + MODIFIER_DBASE_NO;
	}
	else
	{
		device_name.insert(0,env_var_fqdn_prefix[0]);
	}

	std::transform(device_name.begin(),device_name.end(),device_name.begin(),::tolower);

	obj_name_lower = obj_name;
	std::transform(obj_name_lower.begin(),obj_name_lower.end(),obj_name_lower.begin(),::tolower);
	std::string local_callback_key(device_name);

	std::string::size_type pos;
	if ((pos = local_callback_key.find('#')) == std::string::npos)
	{
		if (inter_event == true)
			local_callback_key = local_callback_key + "." + event_name;
		else
			local_callback_key = local_callback_key + "/" +obj_name_lower + "." + event_name;
	}
	else
	{
		local_callback_key.erase(pos);
		if (inter_event == true)
			local_callback_key = local_callback_key + MODIFIER_DBASE_NO + '.' + event_name;
		else
			local_callback_key = local_callback_key + "/" + obj_name_lower + MODIFIER_DBASE_NO + '.' + event_name;
	}

//
// Do we have to support event compatibility ?
//

	bool add_compat_info = false;
	if (event == ATTR_CONF_EVENT ||
		event == CHANGE_EVENT ||
		event == PERIODIC_EVENT ||
		event == ARCHIVE_EVENT ||
		event == USER_EVENT)
		add_compat_info = true;

//
// Inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is down !)
// To do this, we need to build DS adm device proxy. If it is not the first call for this
// DS, we should find it in map. Otherwise, get it.
//

	DeviceData subscriber_in;
	std::vector<std::string> subscriber_info;
	subscriber_info.push_back(local_device_name);
	subscriber_info.push_back(obj_name_lower);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(event_name);

	DeviceProxy *adm_dev = NULL;
	bool allocated = false;

        std::map<std::string,std::string>::iterator ipos = device_channel_map.find(device_name);
	EvChanIte evt_it = channel_map.end();

    std::string adm_name;

	if (ipos == device_channel_map.end())
	{
		try
		{
			adm_name = device->adm_name();
			adm_dev = new DeviceProxy(adm_name);
			allocated = true;
		}
		catch(...)
		{
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Check that device server is running..." << std::ends;
			Except::throw_exception(API_CantConnectToDevice,o.str(),"EventConsumer::connect_event()");
		}
	}
	else
	{
		evt_it = channel_map.find(ipos->second);
		if (evt_it == channel_map.end())
		{
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Corrupted internal map. Please report bug" << std::ends;
			Except::throw_exception(API_BadConfigurationProperty,o.str(),"EventConsumer::connect_event()");
		}
		const EventChannelStruct &evt_ch = evt_it->second;
		{
			AutoTangoMonitor _mon(evt_ch.channel_monitor);
			adm_dev = evt_ch.adm_device_proxy;
			adm_name = evt_ch.full_adm_name;
		}
	}

	Tango::DeviceData dd;
	bool zmq_used = false;

	try
	{
	    std::string cmd_name;
	    get_subscription_command_name(cmd_name);

	    if (cmd_name.find("Zmq") != std::string::npos)
	    {
			zmq_used = true;
			std::stringstream ss;
			ss << DevVersion;
			subscriber_info.push_back(ss.str());
		}

		subscriber_in << subscriber_info;
    	dd = adm_dev->command_inout(cmd_name,subscriber_in);

		dd.reset_exceptions(DeviceData::isempty_flag);

//
// DS before Tango 7.1 does not send their Tango_host in the event
// Refuse to subscribe to an event from a DS before Tango 7.1 if the device is in another CS than the one defined by
// the TANGO_HOST env. variable
//

		if (dd.is_empty() == true)
		{
			if (device->get_from_env_var() == false)
			{
				std::string::size_type pos = device_name.find("://");
				pos = pos + 3;
				pos = device_name.find('/',pos);
				std::string fqdn_prefix = device_name.substr(0,pos + 1);
				std::transform(fqdn_prefix.begin(),fqdn_prefix.end(),fqdn_prefix.begin(),::tolower);

				if (fqdn_prefix != env_var_fqdn_prefix[0])
				{
					TangoSys_OMemStream o;
					o << "Device server for device " << device_name;
					o << " is too old to generate event in a multi TANGO_HOST environment. Please, use Tango >= 7.1" << std::ends;

					EventSystemExcept::throw_exception(API_DSFailedRegisteringEvent,o.str(),
                      				"EventConsumer::connect_event()");
				}
			}
		}
	}
	catch (Tango::DevFailed &e)
	{
		if (allocated == true)
			delete adm_dev;
        std::string reason(e.errors[0].reason.in());
        if (reason == API_CommandNotFound)
            throw;
        else
            EventSystemExcept::re_throw_exception(e,API_DSFailedRegisteringEvent,
                       				"Device server send exception while trying to register event",
                      				"EventConsumer::connect_event()");
	}

//
// Change event name if it is IDL 5 compatible:
// This code is Tango 9 or more. If the remote device is IDL 5 (or more), insert tango IDL release number
// at the beginning of event name.
//

	const DevVarLongStringArray *dvlsa;
	bool dd_extract_ok = true;

	if ((dd >> dvlsa) == false)
		dd_extract_ok = false;

	if (dd_extract_ok == true && add_compat_info == true && dvlsa->lvalue[1] >= MIN_IDL_CONF5)
	{
		event_name = EVENT_COMPAT_IDL5 + event_name;
		std::string::size_type pos = local_callback_key.rfind('.');
		local_callback_key.insert(pos + 1,EVENT_COMPAT_IDL5);
	}

	ReceivedFromAdmin received_from_admin = initialize_received_from_admin(dvlsa, local_callback_key, adm_name, device->get_from_env_var());

	//
	// Do we already have this event in the callback map? If yes, simply add this new callback to the event callback list
	// If it's a ATTR_CONF_EVENT, don't forget to look for the two different event kinds
	//

	EvCbIte iter = event_callback_map.find(received_from_admin.event_name);

	if (iter == event_callback_map.end() && add_compat_info == true)
	{
		for (int i = 0; i < ATT_CONF_REL_NB; i++)
		{
			std::string mod_local_callback_key(received_from_admin.event_name);
			std::string::size_type pos = mod_local_callback_key.rfind('.');
			mod_local_callback_key.insert(pos + 1, EVENT_COMPAT_IDL5);
			iter = event_callback_map.find(mod_local_callback_key);
			if (iter != event_callback_map.end())
			{
				break;
			}
		}
	}

	if (iter != event_callback_map.end())
	{
		int new_event_id = add_new_callback(device, iter, callback, ev_queue, event_id);
		get_fire_sync_event(device,
							callback,
							ev_queue,
							event,
							event_name,
							obj_name,
							iter->second,
							received_from_admin.event_name);
		return new_event_id;
	}

	//
	// Some Zmq specific code (Check release compatibility,....)
	//
	zmq_specific(dd, adm_name, device, obj_name);

//
// Search (or create) entry for channel map
//

    int valid_endpoint_nb = 0;

	if (ipos == device_channel_map.end())
	{
		cout3 << "device " << device_name << " is not connected, going to connect to the event channel !\n";
		bool new_entry_in_channel_map = false;

		try
		{
			connect(device, device_name, dd, received_from_admin.channel_name, new_entry_in_channel_map);
		}
		catch (Tango::DevFailed &e)
		{
		    if (allocated == true)
                delete adm_dev;
		    throw;
		}

		ipos = device_channel_map.find(device_name);
		if (ipos == device_channel_map.end())
		{
		    if (allocated == true)
                delete adm_dev;

			TangoSys_OMemStream o;

			o << "Failed to connect to event channel for device " << device_name << std::ends;
			EventSystemExcept::throw_exception((const char*)API_NotificationServiceFailed,
                       				o.str(),
                       				(const char*)"EventConsumer::connect_event()");
		}

		if (evt_it == channel_map.end())
		{
			evt_it = channel_map.find(ipos->second);
			evt_it->second.last_subscribed = time(NULL);
			valid_endpoint_nb = evt_it->second.valid_endpoint;

			if (new_entry_in_channel_map == true)
			{
				AutoTangoMonitor _mon(evt_it->second.channel_monitor);
				evt_it->second.adm_device_proxy = adm_dev;
			}
			else
				delete adm_dev;
		}
	}
	else
	{
		evt_it = channel_map.find(ipos->second);
		valid_endpoint_nb = evt_it->second.valid_endpoint;
	}

//
// Init device proxy in channel event map
//

/*	{
		AutoTangoMonitor _mon(evt_it->second.channel_monitor);
		evt_it->second.adm_device_proxy = adm_dev;
	}*/

//
// Now, connect to the event system
//

    EventCallBackStruct new_event_callback;
    EventSubscribeStruct new_ess{};

    new_event_callback.received_from_admin = received_from_admin;
    new_event_callback.obj_name = obj_name_lower;
    new_event_callback.event_name = event_name;
    new_event_callback.channel_name = evt_it->first;
    new_event_callback.alias_used = false;
    new_event_callback.client_attribute_name = get_client_attribute_name(local_callback_key, filters);

    if (inter_event == true)
		new_event_callback.fully_qualified_event_name = device_name + '.' + event_name;
	else
		new_event_callback.fully_qualified_event_name = device_name + '/' + obj_name_lower + '.' + event_name;

    if (dd_extract_ok == false)
        new_event_callback.device_idl = 0;
    else
    {
        if (dvlsa->lvalue.length() >= 2)
            new_event_callback.device_idl = dvlsa->lvalue[1];
        else
            new_event_callback.device_idl = 0;
    }
    new_event_callback.ctr = 0;
    new_event_callback.discarded_event = false;
    if (zmq_used == true)
		new_event_callback.endpoint = dvlsa->svalue[(valid_endpoint_nb << 1) + 1].in();

    new_ess.callback = callback;
    new_ess.ev_queue = ev_queue;
    new_ess.device = device;

	connect_event_system(device_name,obj_name_lower,event_name,filters,evt_it,new_event_callback,dd,valid_endpoint_nb);

//
// Check if this subscription is for a fwd attribute root attribute (when relevant)
//

	new_event_callback.fwd_att = false;
	if (inter_event == false && pipe_event == false)
	{
		ApiUtil *au = ApiUtil::instance();
		if (au->in_server() == true)
		{
			RootAttRegistry &rar = Util::instance()->get_root_att_reg();

			std::string root_att_name = device_name;
			root_att_name = root_att_name + '/' + obj_name_lower;
			if (rar.is_root_attribute(root_att_name) == true)
				new_event_callback.fwd_att = true;
		}
		else
			new_event_callback.fwd_att = false;
	}
	else
		new_event_callback.fwd_att = false;

//
// if an event ID was passed to the method, reuse it!
//

    if (ret_event_id <= 0)
    {
        subscribe_event_id++;
        ret_event_id = subscribe_event_id;
    }
    new_ess.id = ret_event_id;

    new_event_callback.callback_list.push_back(new_ess);

//
// Create a callback monitor and set its timeout to 1000ms not to block the event consumer for too long.
//

    new_event_callback.callback_monitor = new TangoMonitor();
    new_event_callback.callback_monitor->timeout(1000);

//
// If we have a CS for which TANGO_HOST is one alias (host name in alias map), set flag in map
//

	pos = local_callback_key.find(':',6);
	std::string tg_host = local_callback_key.substr(8,pos - 8);
	std::map<std::string,std::string>::iterator ite = alias_map.find(tg_host);
	if (ite != alias_map.end())
	{
		new_event_callback.alias_used = true;
	}

//
// Insert new entry in map
//

    std::pair<EvCbIte, bool> ret = event_callback_map
		.insert(std::pair<std::string, EventCallBackStruct>(received_from_admin.event_name, new_event_callback));
    if (!ret.second)
    {
        TangoSys_OMemStream o;
        o << "Failed to connect to event channel for device " << device_name
          << "\nCorrupted internal map: event callback already exists. Please report bug!" << std::ends;
        EventSystemExcept::throw_exception(API_NotificationServiceFailed,
                                o.str(),
								"EventConsumer::connect_event()");
    }
    iter = ret.first;

//
// Read the attribute/pipe by a simple synchronous call.This is necessary for the first point in "change" mode
// Force callback execution when it is done
//

	get_fire_sync_event(device,callback,ev_queue,event,event_name,obj_name,iter->second,local_callback_key);

//
// Sleep for some mS in order to give to ZMQ some times to propagate the subscription to the publisher
//

#ifndef _TG_WINDOWS_
	struct timespec ts;
	ts.tv_nsec = 1000000; //20000000;
	ts.tv_sec = 0;
	nanosleep(&ts,NULL);
#else
	Sleep(20);
#endif

	return ret_event_id;
}

std::string EventConsumer::get_client_attribute_name(const std::string &local_callback_key, const std::vector<std::string> &filters)
{
  if(filters.size() == 1)
    return filters[0];// here filters[0] is expected to be user defined attribute name


	size_t pos = local_callback_key.rfind('.');//remove event_type e.g. .idl5_change
	return local_callback_key.substr(0, pos);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::unsubscribe_event()
//
// description :
//		Method to unsubscribe from an event
//
// argument :
//		in :
//			- event_id : The event identifier
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::unsubscribe_event(int event_id)
{
	if (event_id == 0)
	{
		EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");
	}

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

//
// First, we need to check if the unsubscribe is not done within a callback
// Do not take a WriterLock because the push_structured_event method already holds a Reader lock
// In such a case, the real unsubscribe will be done later via a thread
//

	{
		ReaderLock r(map_modification_lock);
		for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
		{
			for (esspos = epos->second.callback_list.begin(); esspos != epos->second.callback_list.end(); ++esspos)
			{
				if(esspos->id == event_id)
				{

//
// If the unsubscribe is done while the callback is being executed, mark the callback as unusable (event_id < 0)
// and start a thread which will do the unsubscribe when the callback execution will be finished
//

                    if (thread_id != 0)
                    {
                        omni_thread::ensure_self se;
                        if (omni_thread::self()->id() == thread_id)
                        {
 //                           cout << event_id << ": Unsubscribing for an event while it is in its callback !!!!!!!!!!" << endl;
                            esspos->id = -event_id;

                            DelayedEventUnsubThread *th = new DelayedEventUnsubThread(this,event_id,epos->second.callback_monitor);
                            th->start();

                            return;
                        }
					}
				}
			}
		}
	}

//
// Ask the main ZMQ thread to delay all incoming event until this method exit. A dead lock could happen if we don't
// do this (really experienced!)
//

    DelayEvent de(this);
	WriterLock w(map_modification_lock);

//
// First remove the callback entry from the callback map
//

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{

//
// delete the event queue when used
//

                delete esspos->ev_queue;

//
// Remove callback entry in vector
//

				evt_cb.callback_list.erase(esspos);

//
// If the callback list is empty
//

				if (evt_cb.callback_list.empty() == true)
				{
                    EvChanIte evt_it = channel_map.find(evt_cb.channel_name);
                    EventChannelStruct &evt_ch = evt_it->second;
                    if (evt_ch.channel_type == NOTIFD)
                    {
                        try
                        {
                            CosNotifyFilter::Filter_var f = evt_ch.structuredProxyPushSupplier->get_filter(evt_cb.filter_id);
                            evt_ch.structuredProxyPushSupplier->remove_filter(evt_cb.filter_id);
                            f->destroy();
					    }
                        catch (...)
                        {
                            EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
                                (const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() (hint: check the Notification daemon is running ",
                                (const char*)"EventConsumer::unsubscribe_event()");
                        }
					}
					else
					{
					    disconnect_event(evt_cb.fully_qualified_event_name,evt_cb.endpoint);
					}

					// delete the allocated callback monitor
					delete epos->second.callback_monitor;

					std::string deleted_channel_name = epos->second.channel_name;
					std::string deleted_event_endpoint = evt_cb.endpoint;
					event_callback_map.erase(epos);

//
// Check if there is another callback using the same channel
//

					std::map<std::string,EventCallBackStruct>::iterator cb_pos;
					bool channel_used_elsewhere = false;
					for (cb_pos = event_callback_map.begin(); cb_pos != event_callback_map.end(); ++cb_pos)
					{
						if (cb_pos->second.channel_name == deleted_channel_name)
						{
							channel_used_elsewhere = true;
							break;
						}
					}

//
// This channel is not used anymore in the app, remove its entry in the channel maps
//

					if (channel_used_elsewhere == false)
					{
						std::map<std::string,EventChannelStruct>::iterator chan_pos;
						for (chan_pos = channel_map.begin(); chan_pos != channel_map.end(); ++chan_pos)
						{
							EventChannelStruct &evt_ch = chan_pos->second;
							if (chan_pos->first == deleted_channel_name)
							{
								if (evt_ch.adm_device_proxy != NULL)
								{
								    if (evt_ch.channel_type == NOTIFD)
								    {
                                        try
                                        {
                                            CosNotifyFilter::Filter_var f = evt_ch.structuredProxyPushSupplier->get_filter(evt_ch.heartbeat_filter_id);
                                            evt_ch.structuredProxyPushSupplier->remove_filter(evt_ch.heartbeat_filter_id);
                                            f->destroy();
                                            try
                                            {
                                                omniORB::setClientCallTimeout(evt_ch.structuredProxyPushSupplier,1000);
                                                evt_ch.structuredProxyPushSupplier->disconnect_structured_push_supplier();
                                            }
                                            catch(CORBA::TRANSIENT &c_t)
                                            {
                                                if (c_t.minor() != omni::TRANSIENT_CallTimedout)
                                                    throw;
                                            }
                                        }
                                        catch (...)
                                        {
                                            EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
                                                (const char*)"Failed to unsubscribe event, caught exception while calling remove_filter() or destroy() on the heartbeat filter (hint: check the Notification daemon is running ",
                                                (const char*)"EventConsumer::unsubscribe_event()");
                                        }
								    }
								    else
								    {
                                        disconnect_event_channel(deleted_channel_name,evt_ch.endpoint,deleted_event_endpoint);
								    }

									delete evt_ch.adm_device_proxy;
                                    delete evt_ch.channel_monitor;
								}

								channel_map.erase(chan_pos);
								break;
							}
						}

						std::map<std::string,std::string>::iterator dev_pos,dev_pos_del;
						for (dev_pos = device_channel_map.begin(); dev_pos != device_channel_map.end();)
						{
							if (dev_pos->second == deleted_channel_name)
							{
								dev_pos_del = dev_pos;
								++dev_pos;
								device_channel_map.erase(dev_pos_del);

//
// Don't "break" the loop! There may be more than one!
//

							}
							else
								++dev_pos;
						}

					}
				}
				return;
			}
		}
	}

	// check also the vector of not yet connected events

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// delete the event queue when used
                delete vpos->ev_queue;

			   // delete element from vector
			   event_not_connected.erase(vpos);
			   return;
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::unsubscribe_event()");
}


void DelayedEventUnsubThread::run(TANGO_UNUSED(void *ptr))
{
	try
	{

//
// In case a callback is still in its execution, wait for it to terminate
//

		the_mon->timeout(3000);
		the_mon->get_monitor();
		the_mon->rel_monitor();

//
// Unsubscribe the event
//

		ev_cons->unsubscribe_event(-event_id);
	}
	catch(...) {}
}


void DelayedEventSubThread::run(TANGO_UNUSED(void *ptr))
{

//
// Subscribe to the event
//

    std::vector<std::string> v_s;
	try
	{
        DelayEvent de(ev_cons);
        WriterLock w(EventConsumer::get_map_modification_lock());

		ev_cons->connect_event(device,attribute,et,callback,ev_queue,v_s,event_name,ev_id);
	}
	catch (Tango::DevFailed &e)
	{

//
// when the subscribe event has not worked, store the connection data in a vector of not
// yet connected events.
// Retry to connect in the next heartbeat period.
//

		EventNotConnected conn_params;
		conn_params.device           = device;
		conn_params.attribute        = attribute;
		conn_params.event_type       = et;
		conn_params.event_name       = event_name;
		conn_params.callback         = callback;
		conn_params.ev_queue         = ev_queue;
		conn_params.filters          = v_s;
		conn_params.last_heartbeat   = time(NULL);
		conn_params.event_id = ev_id;

		ev_cons->add_not_connected_event(e,conn_params);
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::add_not_connected_event()
//
// description :
//		Add a not connected event to the list of not connected event managed in the event consumer in order  to
//      periodically retry the subscription. This method is supposed to be called when event are subscribed
//      within a event callback. In this case the subsciption is done in a thread (DelayedEventSubThread class)
//
// argument :
//		in  :
//			- e : The thrown exception
//			- not_con : A structure describing the not connected event
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::add_not_connected_event(DevFailed &e,EventNotConnected &not_con)
{
    if (env_var_fqdn_prefix.empty() == false)
        not_con.prefix = env_var_fqdn_prefix[0];

    event_not_connected.push_back (not_con);

    std::vector<EventNotConnected>::iterator vpos = event_not_connected.end() - 1;
    time_t now = time(NULL);
    keep_alive_thread->stateless_subscription_failed(vpos,e,now);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_events()
//
// description :
//		Return a vector with all events stored in the event queue. Events are kept in the buffer since the last
//		extraction with get_events().
//      After returning the event data, the event queue gets emptied!
//
// argument :
//		in  :
//			- event_id   : The event identifier
// 		out :
//			- event_list : A reference to an event data list to be filled
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_events (int event_id, EventDataList &event_list)
{

	cout3 << "EventConsumer::get_events() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the events from the queue
					esspos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_events()
//
// description :
//		Return a vector with all attribute configuration events stored in the event queue.
//      Events are kept in the buffer since the last extraction with get_events().
//      After returning the event data, the event queue gets emptied!
//
// argument :
//		in  :
//			- event_id   : The event identifier
// 		out :
//			- event_list : A reference to an event data list to be filled
//
//------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_events (int event_id, AttrConfEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the events from the queue
					esspos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_events()
//
// description :
//		Return a vector with all data ready events stored in the event queue.
//      Events are kept in the buffer since the last extraction with get_events().
//      After returning the event data, the event queue gets emptied!
//
// argument :
//		in  :
//			- event_id   : The event identifier
// 		out :
//			- event_list : A reference to an event data list to be filled
//
//------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_events (int event_id, DataReadyEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the events from the queue
					esspos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_events()
//
// description :
//		Return a vector with all device interface change events stored in the event queue.
//      Events are kept in the buffer since the last extraction with get_events().
//      After returning the event data, the event queue gets emptied!
//
// argument :
//		in  :
//			- event_id   : The event identifier
// 		out :
//			- event_list : A reference to an event data list to be filled
//
//------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_events (int event_id, DevIntrChangeEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the events from the queue
					esspos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception(API_EventQueues,
					        o.str(),"EventConsumer::get_events()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception(API_EventQueues,
				        o.str(),"EventConsumer::get_events()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception("API_EventNotFound",
			"Failed to get event, the event id specified does not correspond with any known one",
			"EventConsumer::get_events()");
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_events()
//
// description :
//		Return a vector with all pipe events stored in the event queue.
//      Events are kept in the buffer since the last extraction with get_events().
//      After returning the event data, the event queue gets emptied!
//
// argument :
//		in  :
//			- event_id   : The event identifier
// 		out :
//			- event_list : A reference to an event data list to be filled
//
//------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_events (int event_id, PipeEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the events from the queue
					esspos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception(API_EventQueues,
					        o.str(),"EventConsumer::get_events()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (event_list);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception(API_EventQueues,
				        o.str(),"EventConsumer::get_events()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception("API_EventNotFound",
			"Failed to get event, the event id specified does not correspond with any known one",
			"EventConsumer::get_events()");
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_events()
//
// description :
//		Call the callback method for all events stored in the event queue.
//      Events are kept in the buffer since the last extraction with get_events().
//      After returning the event data, the event queue gets emptied!
//
// argument :
//		in  :
//			- event_id   : The event identifier
// 		out :
//			- event_list : A reference to an event data list to be filled
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_events (int event_id, CallBack *cb)
{

	cout3 << "EventConsumer::get_events() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the events from the queue
					esspos->ev_queue->get_events (cb);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the events from the queue
					vpos->ev_queue->get_events (cb);
					return;
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_events()");
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::event_queue_size()
//
// description :
//		Returns the number of events stored in the event queue
//
// argument :
//		in :
//			- event_id   : The event identifier
//
//-------------------------------------------------------------------------------------------------------------------

int  EventConsumer::event_queue_size(int event_id)
{
	cout3 << "EventConsumer::event_queue_size() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the event queue size
					return (esspos->ev_queue->size());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
					        o.str(),
					        (const char *)"EventConsumer::event_queue_size()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the event queue size
					return (vpos->ev_queue->size());

				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::event_queue_size()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::event_queue_size()");

	// Should never reach here. To make compiler happy

	int ret = -1;
	return ret;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::is_event_queue_empty()
//
// description :
//		Returns true when the event queue is empty
//
// argument :
//		in :
//			- event_id   : The event identifier
//
//-------------------------------------------------------------------------------------------------------------------

bool EventConsumer::is_event_queue_empty(int event_id)
{
	cout3 << "EventConsumer::is_event_queue_empty() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// check whether the event queue is empty
					return (esspos->ev_queue->is_empty());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
					        o.str(),
					        (const char *)"EventConsumer::is_event_queue_empty()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// check whether the event queue is empty
					return (vpos->ev_queue->is_empty());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::is_event_queue_empty()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::is_event_queue_empty()");

	// Should never reach here. To make compiler happy

	bool ret = true;
	return ret;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_last_event_date()
//
// description :
//		Get the time stamp of the last inserted event
//
// argument :
//		in :
//			- event_id   : The event identifier
//
//--------------------------------------------------------------------------------------------------------------------

TimeVal EventConsumer::get_last_event_date(int event_id)
{
	cout3 << "EventConsumer::get_last_event_date() : event_id = " << event_id << std::endl;

	// lock the maps
	ReaderLock l(map_modification_lock);

//
// First search the event entry in the callback map
//

	std::map<std::string,EventCallBackStruct>::iterator epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
	{
		EventCallBackStruct &evt_cb = epos->second;
		for (esspos = evt_cb.callback_list.begin(); esspos != evt_cb.callback_list.end(); ++esspos)
		{
			if(esspos->id == event_id)
			{
				// check wether an event queue is used!
				if ( esspos->callback == NULL )
				{
					// get the last insertion date
					return (esspos->ev_queue->get_last_event_date());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        	o.str(),
				        	(const char *)"EventConsumer::get_last_event_date()");
				}
			}
		}
	}

//
// check also the vector of not yet connected events
//

	if ( event_not_connected.empty() == false )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); ++vpos)
		{
			if ( vpos->event_id == event_id)
			{
				// check wether an event queue is used!
				if ( vpos->callback == NULL )
				{
					// get the last insertion date
					return (vpos->ev_queue->get_last_event_date());
				}
				else
				{
					TangoSys_OMemStream o;
					o << "No event queue specified during subscribe_event()\n";
					o << "Cannot return any event data" << std::ends;
					EventSystemExcept::throw_exception((const char *)API_EventQueues,
				        o.str(),
				        (const char *)"EventConsumer::get_last_event_date()");
				}
			}
		}
	}

	// nothing was found!

	EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to get event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_last_event_date()");

	// Should never reach here. To make compiler happy

	struct TimeVal tv;
	tv.tv_sec = tv.tv_usec = tv.tv_nsec = 0;
	return tv;
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::add_new_callback()
//
// description :
//		Add a new callback to an already existing event entry in the callback map
//
// argument :
//		in :
//			- device : Device proxy used for subscription
//			- iter : Iterator in the callback map
//			- callback : Pointer to the Callback object
//			- ev_queue : Pointer to the event queue
//			- event_id : The event identifier
//
//--------------------------------------------------------------------------------------------------------------------

int EventConsumer::add_new_callback(DeviceProxy *device, EvCbIte &iter,CallBack *callback,EventQueue *ev_queue,int event_id)
{
	EventSubscribeStruct ess{};
	int ret_event_id = event_id;

	if (ret_event_id <= 0)
	{
		subscribe_event_id++;
		ret_event_id = subscribe_event_id;
	}

	ess.device = device;
	ess.id = ret_event_id;
	ess.callback = callback;
	ess.ev_queue = ev_queue;

	iter->second.callback_list.push_back(ess);

	return ret_event_id;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_fire_sync_event()
//
// description :
//		Get event data and fire a synchronous event
//
// argument :
//		in :
//			- device : The device pointer
//			- callback : The callback pointer
//			- ev_queue : The event queue
//			- event : The event type
//			- event_name : The event name
//			- obj_name : The attribute/pipe name
//			- cb :
//			- callback_key :
//
//-------------------------------------------------------------------------------------------------------------------

void EventConsumer::get_fire_sync_event(DeviceProxy *device,CallBack *callback,EventQueue *ev_queue,EventType event,
										std::string &event_name,const std::string &obj_name,EventCallBackStruct &cb,
										std::string &callback_key)
{

//
// A small mS sleep here! This is required in case there is a push_event in the read_attribute (or pipe)
// method on the device side. This sleep gives time to ZMQ to send its subscription message
//

#ifndef _TG_WINDOWS_
	struct timespec to_wait,inter;
	to_wait.tv_sec = 0;
	to_wait.tv_nsec = 500000; //10000000;
	nanosleep(&to_wait,&inter);
#else
	Sleep(25);
#endif

	if ((event == CHANGE_EVENT) ||
	    (event == QUALITY_EVENT) ||
	    (event == ARCHIVE_EVENT) ||
	    (event == USER_EVENT) ||
        (event == PERIODIC_EVENT))
	{
		DevErrorList err;
		err.length(0);

		std::string domain_name;
		std::string::size_type pos;

		if ((pos = device_name.find(MODIFIER_DBASE_NO)) != std::string::npos)
		{
            domain_name = device_name;
            std::string tmp = '/' + obj_name_lower;
            domain_name.insert(pos,tmp);
		}
		else
            domain_name = device_name + '/' + obj_name_lower;

		AttributeValue_5 *av_5 = nullptr;
		DeviceAttribute *da = nullptr;
		FwdEventData *event_data;

		try
		{
			if (cb.fwd_att == true)
			{
				device->read_attribute(obj_name.c_str(),av_5);
				if (av_5->err_list.length() != 0)
				{
					err = av_5->err_list;
					err.length(err.length() - 1);
				}
			}
			else
			{
				da = new DeviceAttribute();
				*da = device->read_attribute(obj_name.c_str());
				if (da->has_failed() == true)
				{
					err = da->get_err_stack();
					err.length(err.length() - 1);
				}
			}
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}

		std::string local_event_name = event_name;
		pos = local_event_name.find(EVENT_COMPAT);
		if (pos != std::string::npos)
			local_event_name.erase(0,EVENT_COMPAT_IDL5_SIZE);
		std::string local_domain_name = cb.get_client_attribute_name();

		if (cb.fwd_att == true)
		{
			da = new DeviceAttribute();
			event_data = new FwdEventData(device,local_domain_name,local_event_name,da,err);
			event_data->set_av_5(av_5);
		}
		else
		{
			event_data = new FwdEventData(device,local_domain_name,local_event_name,da,err);
		}

		AutoTangoMonitor _mon(cb.callback_monitor);

//
// If a callback method was specified, call it!
//

		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				std::cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << std::endl;
			}

			delete event_data;
			if (cb.fwd_att == true)
				delete [] av_5;
		}

//
// No calback method, the event has to be inserted into the event queue
//

		else
		{
			ev_queue->insert_event(event_data);
		}
	}
	else if (event == ATTR_CONF_EVENT)
	{
		DevErrorList err;
		err.length(0);
		std::string local_domain_name = cb.get_client_attribute_name();
		AttributeInfoEx *aie = NULL;

		std::string local_event_name = event_name;
		std::string::size_type pos = local_event_name.find(EVENT_COMPAT);
		if (pos != std::string::npos)
			local_event_name.erase(0,EVENT_COMPAT_IDL5_SIZE);

		try
		{
			aie = new AttributeInfoEx();
			*aie = device->get_attribute_config(const_cast<std::string &>(obj_name));
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}

		FwdAttrConfEventData *event_data = new FwdAttrConfEventData(device,
						      local_domain_name,
						      local_event_name,
						      aie,
						      err);
		AutoTangoMonitor _mon(cb.callback_monitor);

//
// If a callback method was specified, call it!
//

		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				std::cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << std::endl;
			}
			delete event_data;
		}

//
// No calback method, the event has to be inserted into the event queue
//

		else
		{
			ev_queue->insert_event(event_data);
		}
	}
	else if (event == INTERFACE_CHANGE_EVENT)
	{
		DevErrorList err;
		err.length(0);
		CommandInfoList *c_list = nullptr;
		AttributeInfoListEx *a_list = nullptr;
		std::string ev_name(EventName[INTERFACE_CHANGE_EVENT]);

		try
		{
			c_list = device->command_list_query();
			a_list = device->attribute_list_query_ex();
		}
		catch (DevFailed &e)
		{
			delete c_list;
			c_list = nullptr;
			delete a_list;
			a_list = nullptr;

			err = e.errors;
		}

		DevIntrChangeEventData *event_data = new DevIntrChangeEventData(device,
						      ev_name,device_name,
						      c_list,a_list,true,
						      err);

		AutoTangoMonitor _mon(cb.callback_monitor);

//
// if a callback method was specified, call it!
//

		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				std::cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << std::endl;
			}

			delete event_data;
			delete c_list;
			delete a_list;
		}

//
// No calback method, the event has to be instered into the event queue
//

		else
		{
			ev_queue->insert_event(event_data);
		}
	}
	else if (event == PIPE_EVENT)
	{
		DevErrorList err;
		err.length(0);

		std::string domain_name;
		std::string::size_type pos;

		if ((pos = device_name.find(MODIFIER_DBASE_NO)) != std::string::npos)
		{
            domain_name = device_name;
            std::string tmp = '/' + obj_name_lower;
            domain_name.insert(pos,tmp);
		}
		else
            domain_name = device_name + '/' + obj_name_lower;

		DevicePipe *da = nullptr;
		PipeEventData *event_data;

		try
		{
			da = new DevicePipe();
			*da = device->read_pipe(obj_name);
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}

		event_data = new PipeEventData(device,domain_name,event_name,da,err);

		AutoTangoMonitor _mon(cb.callback_monitor);

//
// If a callback method was specified, call it!
//

		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				std::cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << std::endl;
			}

			delete event_data;
		}

//
// No calback method, the event has to be inserted into the event queue
//

		else
		{
			ev_queue->insert_event(event_data);
		}
	}
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		EventConsumer::get_event_system_for_event_id()
//
// description :
//		Get which event system is used by one event from its id
//
// argument :
//		in :
//			- event_id : The event id
//
// returns :
//		Event system type used by the event with the specified event id
//
//--------------------------------------------------------------------------------------------------------------------

ChannelType EventConsumer::get_event_system_for_event_id(int event_id)
{
    ChannelType ret = Tango::ZMQ;
    EvCbIte epos;
	std::vector<EventSubscribeStruct>::iterator esspos;

	if (event_id == 0)
	{
		EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_event_system_for_event_id()");
	}

    bool found = false;
    ReaderLock r(map_modification_lock);
    for (epos = event_callback_map.begin(); epos != event_callback_map.end(); ++epos)
    {
        EventCallBackStruct &ecs = epos->second;
        for (esspos = ecs.callback_list.begin(); esspos != ecs.callback_list.end(); ++esspos)
        {
            if(esspos->id == event_id)
            {
                found = true;
                EvChanIte evt_it = channel_map.find(ecs.channel_name);
                if (evt_it == channel_map.end())
                {
                    TangoSys_OMemStream o;
                    o << "Can't unsubscribe to event with id " << event_id << "\n";
                    o << "Corrupted internal map. Please report bug" << std::ends;
                    Except::throw_exception((const char *)API_BadConfigurationProperty,
									o.str(),
									(const char *)"EventConsumer::get_event_system_for_event_id()");
                }
                EventChannelStruct &evt_ch = evt_it->second;
                ret = evt_ch.channel_type;
                break;
            }
            if (found == true)
                break;
        }
    }

//
// Also search in the not connected event vector. The returned value in this case is not relevant
//

    if (found == false && event_not_connected.empty() == false)
    {
        std::vector<EventNotConnected>::iterator vpos;
        for (vpos = event_not_connected.begin();vpos != event_not_connected.end();++vpos)
        {
            if (vpos->event_id == event_id)
            {
                found = true;
                break;
            }
        }
    }

//
// Throw exception if the event_id has not been found in maps
//

    if (found == false)
    {
        EventSystemExcept::throw_exception((const char*)"API_EventNotFound",
			(const char*)"Failed to unsubscribe event, the event id specified does not correspond with any known one",
			(const char*)"EventConsumer::get_event_system_for_event_id()");
    }

    return ret;
}


/************************************************************************/
/*		       															*/
/* 			EventData class 											*/
/*			---------------												*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
// 	EventData constructor
//
//-----------------------------------------------------------------------

EventData::EventData(DeviceProxy *dev,std::string &nam,std::string &evt,
          Tango::DeviceAttribute *attr_value_in, DevErrorList &errors_in) :
          device(dev),attr_name(nam),event(evt),attr_value(attr_value_in),
          errors(errors_in)
{
	if (errors.length()==0)
		err=false;
	else err = true;

	set_time();
}

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
	if (sou.attr_value)
		attr_value = new DeviceAttribute(*(sou.attr_value));
	else
		attr_value = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
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
	if (ri.attr_value)
		attr_value = new DeviceAttribute(*(ri.attr_value));
	else
		attr_value = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;

	return *this;
}

//+----------------------------------------------------------------------
//
// 	EventData destructor
//
//-----------------------------------------------------------------------

EventData::~EventData()
{
    delete attr_value;
}


//+-------------------------------------------------------------------------
//
// method : 		EventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void EventData::set_time()
{
	reception_date = make_TimeVal(std::chrono::system_clock::now());
}

FwdEventData::FwdEventData():EventData(),av_5(nullptr),event_data(nullptr)
{
}

FwdEventData::FwdEventData(DeviceProxy *dev,std::string &_s1,std::string &_s2,Tango::DeviceAttribute *_da,DevErrorList &_del) :
                  EventData(dev,_s1,_s2,_da,_del),av_5(nullptr),event_data(nullptr)
{
}

FwdEventData::FwdEventData(DeviceProxy *dev,std::string &_s1,std::string &_s2,Tango::DeviceAttribute *_da,DevErrorList &_del,zmq::message_t *_m) :
                  EventData(dev,_s1,_s2,_da,_del),av_5(nullptr),event_data(_m)
{
}
/************************************************************************/
/*		       															*/
/* 			AttrConfEventData class 									*/
/*			-----------------											*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  AttrConfEventData constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(DeviceProxy *dev,std::string &nam,std::string &evt,
                  Tango::AttributeInfoEx *attr_conf_in,DevErrorList &errors_in) :
                  device(dev),attr_name(nam),event(evt),attr_conf(attr_conf_in),
                  errors(errors_in)
{
	if (errors.length()==0)
		err=false;
	else
		err = true;

	set_time();
}

//+----------------------------------------------------------------------
//
//  AttrConfEventData copy constructor
//
//-----------------------------------------------------------------------

AttrConfEventData::AttrConfEventData(const AttrConfEventData &sou)
{
	device = sou.device;
	attr_name = sou.attr_name;
	event = sou.event;
	if (sou.attr_conf != NULL)
	{
		attr_conf = new (AttributeInfoEx);
		*attr_conf = *(sou.attr_conf);
	}
	else
		attr_conf = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	AttrConfEventData assignement operator
//
//-----------------------------------------------------------------------

AttrConfEventData & AttrConfEventData::operator=(const AttrConfEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	attr_name = ri.attr_name;
	event = ri.event;
	if (ri.attr_conf != NULL)
	{
		attr_conf = new (AttributeInfoEx);
		*attr_conf = *(ri.attr_conf);
	}
	else
		attr_conf = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;

	return *this;
}

//+----------------------------------------------------------------------
//
// 	AttrConfEventData destructor
//
//-----------------------------------------------------------------------

AttrConfEventData::~AttrConfEventData()
{
    delete attr_conf;
}

//+-------------------------------------------------------------------------
//
// method : 		AttrConfEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void AttrConfEventData::set_time()
{
	reception_date = make_TimeVal(std::chrono::system_clock::now());
}

FwdAttrConfEventData::FwdAttrConfEventData():AttrConfEventData(),fwd_attr_conf(nullptr)
{
}


FwdAttrConfEventData::FwdAttrConfEventData(DeviceProxy *dev,std::string &nam,std::string &evt,
                  Tango::AttributeInfoEx *attr_conf_in,DevErrorList &errors_in) :
                  AttrConfEventData(dev,nam,evt,attr_conf_in,errors_in),fwd_attr_conf(nullptr)
{
}

/************************************************************************/
/*		       															*/
/* 			DataReadyEventData class 									*/
/*			------------------											*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  DataReadyEventData constructor
//
//-----------------------------------------------------------------------

DataReadyEventData::DataReadyEventData(DeviceProxy *dev,AttDataReady *dr,std::string &evt,DevErrorList &errors_in)
:event(evt),errors(errors_in)
{
	device = dev;
	if (dr != NULL)
	{
		attr_name = dr->name.in();
		attr_data_type = dr->data_type;
		ctr = dr->ctr;
	}
	else
	{
		attr_name = "Unknown";
		attr_data_type = -1;
		ctr = -1;
	}

	if (errors.length()==0)
		err = false;
	else
		err = true;
}

//+----------------------------------------------------------------------
//
//  DataReadyEventData copy constructor
//
//-----------------------------------------------------------------------

DataReadyEventData::DataReadyEventData(const DataReadyEventData &sou)
{
	device = sou.device;
	ctr = sou.ctr;
	attr_name = sou.attr_name;
	event = sou.event;
	attr_data_type = sou.attr_data_type;
	reception_date = sou.reception_date;
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	DataReadyEventData assignement operator
//
//-----------------------------------------------------------------------

DataReadyEventData & DataReadyEventData::operator=(const DataReadyEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	ctr = ri.ctr;
	attr_data_type = ri.attr_data_type;
	attr_name = ri.attr_name;
	event = ri.event;

	reception_date = ri.reception_date;

	err = ri.err;
	errors = ri.errors;

	return *this;
}

//+-------------------------------------------------------------------------
//
// method : 		DataReadyEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void DataReadyEventData::set_time()
{
	reception_date = make_TimeVal(std::chrono::system_clock::now());
}

/************************************************************************/
/*		       															*/
/* 			DevIntrChangeEventData class 								*/
/*			----------------------										*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
//  DevIntrChangeEventData constructor
//
//-----------------------------------------------------------------------

DevIntrChangeEventData::DevIntrChangeEventData(DeviceProxy *dev,std::string &evt,std::string &d_name,
								DevCmdInfoList_2 *c_list,AttributeConfigList_5 *a_list,
								bool d_s,DevErrorList &errors_in)
:event(evt),device_name(d_name),dev_started(d_s),errors(errors_in)
{
	device = dev;

	if (errors.length()==0)
		err = false;
	else
		err = true;

	if (err == false)
	{

//
// Convert first command list and then attribute list
//

		cmd_list.resize(c_list->length());
		for (size_t i=0; i < c_list->length(); i++)
		{
			cmd_list[i].cmd_name = (*c_list)[i].cmd_name;
			cmd_list[i].cmd_tag = (*c_list)[i].cmd_tag;
			cmd_list[i].in_type = (*c_list)[i].in_type;
			cmd_list[i].out_type = (*c_list)[i].out_type;
			cmd_list[i].in_type_desc = (*c_list)[i].in_type_desc.in();
			cmd_list[i].out_type_desc = (*c_list)[i].out_type_desc.in();
			cmd_list[i].disp_level = (*c_list)[i].level;
		}

		att_list.resize(a_list->length());
		AttributeConfigList_5_var a_list_var(a_list);

		for (size_t i=0; i<a_list->length(); i++)
		{
			COPY_BASE_CONFIG(att_list,a_list_var)

			for (size_t j=0; j<(*a_list)[i].sys_extensions.length(); j++)
			{
				att_list[i].sys_extensions[j] = (*a_list)[i].sys_extensions[j];
			}
			att_list[i].disp_level = (*a_list)[i].level;
			att_list[i].min_alarm = (*a_list)[i].att_alarm.min_alarm;
			att_list[i].max_alarm = (*a_list)[i].att_alarm.max_alarm;
			att_list[i].root_attr_name = (*a_list)[i].root_attr_name;
			if ((*a_list)[i].memorized == false)
				att_list[i].memorized	= NONE;
			else
			{
				if ((*a_list)[i].mem_init == false)
					att_list[i].memorized	= MEMORIZED;
				else
					att_list[i].memorized	= MEMORIZED_WRITE_INIT;
			}

			COPY_ALARM_CONFIG(att_list,a_list_var)

			COPY_EVENT_CONFIG(att_list,a_list_var)
		}

		a_list_var._retn();
	}
}

DevIntrChangeEventData::DevIntrChangeEventData(DeviceProxy *dev,std::string &evt,std::string &d_name,
								CommandInfoList *c_list,AttributeInfoListEx *a_list,
								bool d_s,DevErrorList &errors_in)
:event(evt),device_name(d_name),dev_started(d_s),errors(errors_in)
{
	device = dev;

	if (errors.length()==0)
		err = false;
	else
		err = true;

	if (err == false)
	{
		cmd_list = *c_list;
		att_list = *a_list;
	}
}

//+----------------------------------------------------------------------
//
//  DevIntrChangeEventData copy constructor
//
//-----------------------------------------------------------------------

DevIntrChangeEventData::DevIntrChangeEventData(const DevIntrChangeEventData &sou)
{
	device = sou.device;
	event = sou.event;
	device_name = sou.device_name;
	cmd_list = sou.cmd_list;
	att_list = sou.att_list;
	dev_started = sou.dev_started;
	reception_date = sou.reception_date;
	err = sou.err;
	errors = sou.errors;
}

//+----------------------------------------------------------------------
//
// 	DevIntrChangeEventData assignement operator
//
//-----------------------------------------------------------------------

DevIntrChangeEventData & DevIntrChangeEventData::operator=(const DevIntrChangeEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	event = ri.event;
	device_name = ri.device_name;
	cmd_list = ri.cmd_list;
	att_list = ri.att_list;
	dev_started = ri.dev_started;
	reception_date = ri.reception_date;

	err = ri.err;
	errors = ri.errors;

	return *this;
}

//+-------------------------------------------------------------------------
//
// method : 		DevIntrChangeEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void DevIntrChangeEventData::set_time()
{
	reception_date = make_TimeVal(std::chrono::system_clock::now());
}

/************************************************************************/
/*		       															*/
/* 			PipepEventData class 										*/
/*			---------------												*/
/*		       															*/
/************************************************************************/

//+----------------------------------------------------------------------
//
// 	PipeEventData constructor
//
//-----------------------------------------------------------------------

PipeEventData::PipeEventData(DeviceProxy *dev,std::string &nam,std::string &evt,
          Tango::DevicePipe *pipe_value_in, DevErrorList &errors_in) :
          device(dev),pipe_name(nam),event(evt),pipe_value(pipe_value_in),
          errors(errors_in)
{
	if (errors.length()==0)
		err=false;
	else err = true;

	set_time();
}

//+----------------------------------------------------------------------
//
// 	PipeEventData copy constructor
//
//-----------------------------------------------------------------------

PipeEventData::PipeEventData(const PipeEventData &sou)
{
	device = sou.device;
	pipe_name = sou.pipe_name;
	event = sou.event;
	if (sou.pipe_value)
		pipe_value = new DevicePipe(*(sou.pipe_value));
	else
		pipe_value = NULL;
	err = sou.err;
	errors = sou.errors;
	reception_date = sou.reception_date;
}

//+----------------------------------------------------------------------
//
// 	PipeEventData assignement operator
//
//-----------------------------------------------------------------------

PipeEventData & PipeEventData::operator=(const PipeEventData &ri)
{
	if (&ri == this)
		return *this;

	device = ri.device;
	pipe_name = ri.pipe_name;
	event = ri.event;
	if (ri.pipe_value)
		pipe_value = new DevicePipe(*(ri.pipe_value));
	else
		pipe_value = NULL;
	err = ri.err;
	errors = ri.errors;
	reception_date = ri.reception_date;

	return *this;
}

//+----------------------------------------------------------------------
//
// 	PipeEventData destructor
//
//-----------------------------------------------------------------------

PipeEventData::~PipeEventData()
{
    delete pipe_value;
}


//+-------------------------------------------------------------------------
//
// method : 		PipeEventData::set_time
//
// description : 	Set the event reception data
//
//--------------------------------------------------------------------------

void PipeEventData::set_time()
{
	reception_date = make_TimeVal(std::chrono::system_clock::now());
}

DeviceProxy& EventCallBackBase::get_device_proxy()
{
    assert(!callback_list.empty());
    return *callback_list[0].device;
}

} /* End of Tango namespace */

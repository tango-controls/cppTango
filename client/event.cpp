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
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

using namespace CORBA;

namespace Tango {

EventConsumerKeepAliveThread *EventConsumer::keep_alive_thread = NULL;
map<std::string,std::string> EventConsumer::device_channel_map;
map<std::string,EventChannelStruct> EventConsumer::channel_map;
map<std::string,EventCallBackStruct> EventConsumer::event_callback_map;
ReadersWritersLock 	EventConsumer::map_modification_lock;

vector<EventNotConnected> EventConsumer::event_not_connected;
int EventConsumer::subscribe_event_id = 0;
vector<string> EventConsumer::env_var_fqdn_prefix;

KeepAliveThCmd EventConsumer::cmd;

/************************************************************************/
/*		       															*/
/* 		(client_)leavefunc function 									*/
/*		------------------								    			*/
/*		       															*/
/* This function will be executed at process exit or when the main      */
/* returned.  It has to be executed to properly shutdown and destroy    */
/* the ORB used by as a server by the event system. The ORB loop is in  */
/* EventConsumer thread. Therefore, get a reference to it, shutdown the */
/* ORB and wait until the thread exit.				        			*/
/* It also destroys the heartbeat filters								*/
/*																		*/
/************************************************************************/
void client_leavefunc()
{
	leavefunc();
}

void leavefunc()
{
	// flag to avoid calling client_leavefunc twice
	static bool already_executed = false;

	Tango::ApiUtil *au = ApiUtil::instance();

	if (au->need_reset_already_flag() == true)
		already_executed = false;

//
// Kill locking threads (if any)
//

	au->clean_locking_threads();

//
// Manage event stuff
//

	NotifdEventConsumer *notifd_ec = au->get_notifd_event_consumer();

	if (notifd_ec != NULL && already_executed == false)
	{
		notifd_ec->shutdown();

		//
		// Shut-down the ORB and wait for the thread to exit
		//

		int *rv;
		notifd_ec->orb_->shutdown(true);
		notifd_ec->join((void **)&rv);
	}

	ZmqEventConsumer *zmq_ec = au->get_zmq_event_consumer();

	if (zmq_ec != NULL && already_executed == false)
	{
		zmq_ec->shutdown();

		already_executed = true;
		au->need_reset_already_flag(false);
	}

}



/************************************************************************/
/*		       															*/
/* 			EventConsumer class 										*/
/*			-------------------											*/
/*		       															*/
/************************************************************************/

EventConsumer::EventConsumer(ApiUtil *api_ptr)
{
//
// Build and store the fqdn prefix for devices in the TANGO_HOST
// environment variable (in lower case letters)
//

    if (env_var_fqdn_prefix.empty() == true)
    {
        try
        {
            Database *db = (api_ptr->get_db_vect())[api_ptr->get_db_ind()];
            string prefix = "tango://" + db->get_db_host() + ':' + db->get_db_port() + '/' ;
            env_var_fqdn_prefix.push_back(prefix);
            if (db->is_multi_tango_host() == true)
            {
                vector<string> &tango_hosts = db->get_multi_host();
                vector<string> &tango_ports = db->get_multi_port();
                for (unsigned int i = 1;i < tango_hosts.size();i++)
                {
                    string prefix = "tango://" + tango_hosts[i] + ':' + tango_ports[i] + '/' ;
                    env_var_fqdn_prefix.push_back(prefix);
                }
            }

            for (size_t loop = 0;loop < env_var_fqdn_prefix.size();++loop)
                transform(env_var_fqdn_prefix[loop].begin(),env_var_fqdn_prefix[loop].end(),env_var_fqdn_prefix[loop].begin(),::tolower);
        }
        catch (Tango::DevFailed)
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
// Install a function to be executed at exit
// This is the only way I found to properly
// shutdown and destroy the ORB.
// Don't do this for windows DLL.
//
// Is this necessary when events are used within a server ?
//

#ifndef _USRDLL

	if ((api_ptr->in_server() == false) && (api_ptr->is_lock_exit_installed() == false))
	{
		atexit(leavefunc);
		api_ptr->set_lock_exit_installed(true);
	}
#endif

//
// Miscellaneous init
//

	cmd.cmd_pending = false;
    if (keep_alive_thread == NULL)
        api_ptr->need_reset_already_flag(true);

//
// Create and start the EventConsumerKeepAliveThread
// Do this at the method's end because the keep_alive_thread
// ptr is also used as a "init done" flag.
//

    if (keep_alive_thread == NULL)
    {
        keep_alive_thread = new EventConsumerKeepAliveThread(cmd);
        keep_alive_thread->start();
    }
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::shutdown()
//
// description : 	Method to stop the keep alive thread and to
//                  disconnect from all used event channels
//
//-----------------------------------------------------------------------------
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

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::connect()
//
// description :    This method is a wrapper around the connection
//                  to the event channel (heartbeat event)
//
// argument : in :  device_proxy : The device handle
//                  d_name : The FQDN (lower case)
//                  dd : The server command result (Used by ZMQ event system only)
//                  adm_name : The admin device name
//
//-----------------------------------------------------------------------------

void EventConsumer::connect(DeviceProxy *device_proxy,string &d_name,DeviceData &dd,string &adm_name)
{
	string channel_name = adm_name;
	if (device_proxy->get_from_env_var() == true)
	{
		channel_name.insert(0,env_var_fqdn_prefix[0]);
	}

//
// If no connection exists to this channel then connect to it.
// Sometimes, this method is called in order to reconnect
// to the notifd. In such a case, the lock is already
// locked before the method is called
//

	std::map<std::string,EventChannelStruct>::iterator ipos = channel_map.find(channel_name);
	if (ipos == channel_map.end())
	{
		connect_event_channel(channel_name,device_proxy->get_device_db(),false,dd);
		ipos = channel_map.find(channel_name);
	}


    if (device_proxy->get_from_env_var() == true)
    {
        adm_name.insert(0,env_var_fqdn_prefix[0]);
    }

//
// Init adm device name in channel map entry
//

	channel_map[channel_name].full_adm_name = adm_name;

//
// Add entry in device_channel_map map
//

	device_channel_map[d_name] = channel_name;
}



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
		dev_attr->set_err_list(new DevErrorList(attr_value_3->err_list));
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
						dev_attr->set_Long64_data(new DevVarLong64Array(max,len,tmp_64,true));
					}
					else
					{
						tmp_64 = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
						dev_attr->set_Long64_data(new DevVarLong64Array(max,len,tmp_64,false));
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
						dev_attr->set_ULong_data(new DevVarULongArray(max,len,tmp_ulo,true));
					}
					else
					{
						tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq_ulo->get_buffer());
						dev_attr->set_ULong_data(new DevVarULongArray(max,len,tmp_ulo,false));
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
						dev_attr->set_ULong64_data(new DevVarULong64Array(max,len,tmp_ulolo,true));
					}
					else
					{
						tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq_u64->get_buffer());
						dev_attr->set_ULong64_data(new DevVarULong64Array(max,len,tmp_ulolo,false));
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
						dev_attr->set_State_data(new DevVarStateArray(max,len,tmp_state,true));
					}
					else
					{
						tmp_state = const_cast<Tango::DevState *>(tmp_seq_state->get_buffer());
						dev_attr->set_State_data(new DevVarStateArray(max,len,tmp_state,false));
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
	dev_attr->set_err_list(new DevErrorList(attr_value_4->err_list));

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
	    att_union_to_device(&attr_value_4->value,dev_attr);
	}
}


void EventConsumer::attr_to_device(const ZmqAttributeValue_4 *attr_value_4,DeviceAttribute *dev_attr)
{
	dev_attr->name = attr_value_4->name;
	dev_attr->quality = attr_value_4->quality;
	dev_attr->time = attr_value_4->time;
	dev_attr->dim_x = attr_value_4->r_dim.dim_x;
	dev_attr->dim_y = attr_value_4->r_dim.dim_y;
	dev_attr->set_w_dim_x(attr_value_4->w_dim.dim_x);
	dev_attr->set_w_dim_y(attr_value_4->w_dim.dim_y);
	dev_attr->set_err_list(new DevErrorList(attr_value_4->err_list));

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
	    att_union_to_device(&attr_value_4->zvalue,dev_attr);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::att_union_to_device()
//
// description : 	Method to initialize in the DeviceAttribute instance
//                  given to the user the attribute value which are received
//                  in a AttrValUnion (or in a class inheriting from)
//
// argument : in :  union_ptr : Pointer to the received union
//			        dev_attr  : Pointer to the DeviceAttribute which will be given
//                              to the user
//
//-----------------------------------------------------------------------------

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
                dev_attr->set_Long64_data(new DevVarLong64Array(max,len,tmp_lolo,true));
            }
            else
            {
                tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq.get_buffer());
                dev_attr->set_Long64_data(new DevVarLong64Array(max,len,tmp_lolo,false));
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
                dev_attr->set_ULong_data(new DevVarULongArray(max,len,tmp_ulo,true));
            }
            else
            {
                tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq.get_buffer());
                dev_attr->set_ULong_data(new DevVarULongArray(max,len,tmp_ulo,false));
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
                dev_attr->set_ULong64_data(new DevVarULong64Array(max,len,tmp_ulolo,true));
            }
            else
            {
                tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq.get_buffer());
                dev_attr->set_ULong64_data(new DevVarULong64Array(max,len,tmp_ulolo,false));
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
                dev_attr->set_State_data(new DevVarStateArray(max,len,tmp_state,true));
            }
            else
            {
                tmp_state = const_cast<Tango::DevState *>(tmp_seq.get_buffer());
                dev_attr->set_State_data(new DevVarStateArray(max,len,tmp_state,false));
            }
        }
        break;

        case DEVICE_STATE:
            sta_dev = union_ptr->dev_state_att();
            dev_attr->d_state_filled = true;
            dev_attr->d_state = sta_dev;
        break;

        case NO_DATA:
        break;

        case ATT_ENCODED:
        {
            const DevVarEncodedArray &tmp_seq = union_ptr->encoded_att_value();
            max = tmp_seq.maximum();
            len = tmp_seq.length();
            if (tmp_seq.release() == true)
            {
                tmp_enc = (const_cast<DevVarEncodedArray &>(tmp_seq)).get_buffer((CORBA::Boolean)true);
                dev_attr->set_Encoded_data(new DevVarEncodedArray(max,len,tmp_enc,true));
            }
            else
            {
                tmp_enc = const_cast<Tango::DevEncoded *>(tmp_seq.get_buffer());
                dev_attr->set_Encoded_data(new DevVarEncodedArray(max,len,tmp_enc,false));
            }
        }
        break;
    }
}

//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
//
// description : 	Method to subscribe to an event with the callback mechanism.
//                  Can be called in a stateless way, that it even works
//                  when the attribute is not available.
//
// argument : in :  device    : The device handle
//			        attribute : The name of the attribute
//			        event     : The type of event to subscribe for
//			        callback  : A pointer to the callback object
//			        filters   : Eventual event filter strings
//                  stateless : Flag to enable the stateless connection when set to true
//
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   const vector<string> &filters,
					bool stateless)
{
	if ((device == NULL) || (callback == NULL))
	{
		EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"Device or callback pointer NULL",
                       	(const char*)"EventConsumer::subscribe_event()");
	}

	return (subscribe_event (device, attribute, event, callback, NULL, filters, stateless));
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
//
// description : 	Method to subscribe to an event with the event_queue mechanism.
//                  Can be called in a stateless way, that it even works
//                  when the attribute is not available.
//
// argument : in :  device    : The device handle
//			        attribute : The name of the attribute
//			        event     : The type of event to subscribe for
//			        event_queue_size:  The size of the circular buffer for incoming
//                                     events
//			        filters   : Eventual event filter strings
//                  stateless : Flag to enable the stateless connection when set to true
//
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   int event_queue_size,
				   const vector<string> &filters,
				   bool stateless)
{
	if ((device == NULL) || (event_queue_size < 0))
	{
		EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
                       	(const char*)"Device pointer is NULL or the event queue size is invalid",
                       	(const char*)"EventConsumer::subscribe_event()");
	}

	// create an event queue object

	EventQueue *ev_queue = new EventQueue(event_queue_size);

	return (subscribe_event (device, attribute, event, NULL, ev_queue, filters, stateless));
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::subscribe_event()
//
//-----------------------------------------------------------------------------
int EventConsumer::subscribe_event (DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   EventQueue *ev_queue,
				   const vector<string> &filters,
				   bool stateless)
{
	string event_name;
	if (event == QUALITY_EVENT)
	{
        EventSystemExcept::throw_exception((const char*)"API_InvalidArgs",
            (const char*)"The quality change event does`nt exist any more. A change event is fired on a qaulity change!",
            (const char*)"EventConsumer::subscribe_event()");
	}
    else
        event_name = EventName[event];

//
// Take a writer lock right now and not in the connect_event method
// In case of stateless subscription and if the device is not there,
// the lock will still be valid when the data will be inserted into the
// vector of non-connected events
// Also ask the main ZMQ thread to delay all incoming event until this meethod
// exit. A dead lock could happen if we don't do this (really experienced!)
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
	    string reason(e.errors[0].reason.in());
		// if the stateless flag is not true, rethrow the exception
		if ((stateless == false) || (reason == "API_CommandNotFound"))
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

		// protect the vector as the other maps!

		// create and save the unique event ID
		subscribe_event_id++;
		conn_params.event_id = subscribe_event_id;

		event_not_connected.push_back (conn_params);
		return subscribe_event_id;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		EventConsumer::connect_event()
//
// description : 	Main nethod called by the subsccribe_event call
//
// argument : in :	device : The device handle
//			        attribute : The name of the attribute
//			        event : The type of event to subscribe for
//			        callback : A pointer to the callback object
//					ev_queue : A pointer to the event queue
//			        filters : Eventual event filter strings
//					event_name : The event name
//                  event_id  : the unique event ID
//
//-----------------------------------------------------------------------------

int EventConsumer::connect_event(DeviceProxy *device,
				   const string &attribute,
				   EventType event,
				   CallBack *callback,
				   EventQueue *ev_queue,
				   const vector<string> &filters,
				   string &event_name,
				   int event_id)
{
	int ret_event_id = event_id;
	device_name = device->dev_name();
	cout3 << "Tango::EventConsumer::connect_event(" << device_name << "," << attribute <<"," << event << ")\n";

//
// Build callback map key and local device name from fqdn
//

	string local_device_name(device_name);
	if (device->get_from_env_var() == false)
	{
		string prot("tango://");
		if (device->is_dbase_used() == false)
			prot = prot + device->get_dev_host() + ':' + device->get_dev_port() + '/';
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

	transform(device_name.begin(),device_name.end(),device_name.begin(),::tolower);

	att_name_lower = attribute;
	transform(att_name_lower.begin(),att_name_lower.end(),att_name_lower.begin(),::tolower);
	string local_callback_key(device_name);

	string::size_type pos;
	if ((pos = local_callback_key.find('#')) == string::npos)
		local_callback_key = local_callback_key + "/" + att_name_lower + "." + event_name;
	else
	{
		local_callback_key.erase(pos);
		local_callback_key = local_callback_key + "/" + att_name_lower + MODIFIER_DBASE_NO + '.' + event_name;
	}
	callback_key = local_callback_key;

//
// Do we already have this event in the callback map?
// If yes, simply add this new callback to the event callback list
//

	EvCbIte iter = event_callback_map.find(local_callback_key);
	if (iter != event_callback_map.end())
	{
		int new_event_id = add_new_callback(iter,callback,ev_queue,event_id);
		get_fire_sync_event(device,callback,ev_queue,event,event_name,attribute,iter->second);
		return new_event_id;
	}

//
// Inform server that we want to subscribe (we cannot use the asynchronous fire-and-forget
// request so as not to block the client because it does not reconnect if the device is
// down !)
// To do this, we need to build DS adm device proxy. If it is not the first call for this
// DS, we should find it in map. Otherwise, get it.
//

	DeviceData subscriber_in;
	vector<string> subscriber_info;
	subscriber_info.push_back(local_device_name);
	subscriber_info.push_back(att_name_lower);
	subscriber_info.push_back("subscribe");
	subscriber_info.push_back(event_name);
	subscriber_in << subscriber_info;

	DeviceProxy *adm_dev = NULL;
	bool allocated = false;
	map<std::string,std::string>::iterator ipos = device_channel_map.find(device_name);
	EvChanIte evt_it = channel_map.end();

    string adm_name;

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
			o << "Check that device server is running..." << ends;
			Except::throw_exception((const char *)"API_BadConfigurationProperty",
				       	 	o.str(),
				         	(const char *)"EventConsumer::connect_event()");
		}
	}
	else
	{
		evt_it = channel_map.find(ipos->second);
		if (evt_it == channel_map.end())
		{
			TangoSys_OMemStream o;
			o << "Can't subscribe to event for device " << device_name << "\n";
			o << "Corrupted internal map. Please report bug" << ends;
			Except::throw_exception((const char *)"API_BadConfigurationProperty",
									o.str(),
									(const char *)"EventConsumer::connect_event()");
		}
		const EventChannelStruct &evt_ch = evt_it->second;
		{
			AutoTangoMonitor _mon(evt_ch.channel_monitor);
			adm_dev = evt_ch.adm_device_proxy;
		}
	}

	Tango::DeviceData dd;

	try
	{
	    string cmd_name;
	    get_subscription_command_name(cmd_name);

    	dd = adm_dev->command_inout(cmd_name,subscriber_in);
		dd.reset_exceptions(DeviceData::isempty_flag);

//
// DS before Tango 7.1 does not send their Tango_host in the event
// Refuse to subsribe to an event from a DS before Tango 7.1 if the device
// is in another CS than the one defined by the TANGO_HOST env. variable
//

		if (dd.is_empty() == true)
		{
			if (device->get_from_env_var() == false)
			{
				string::size_type pos = device_name.find("://");
				pos = pos + 3;
				pos = device_name.find('/',pos);
				string fqdn_prefix = device_name.substr(0,pos + 1);
				transform(fqdn_prefix.begin(),fqdn_prefix.end(),fqdn_prefix.begin(),::tolower);

				if (fqdn_prefix != env_var_fqdn_prefix[0])
				{
					TangoSys_OMemStream o;
					o << "Device server for device " << device_name;
					o << " is too old to generate event in a multi TANGO_HOST environment. Please, use Tango >= 7.1" << ends;

					EventSystemExcept::throw_exception((const char*)"API_DSFailedRegisteringEvent",o.str(),
                      				(const char*)"EventConsumer::connect_event()");
				}
			}
		}
	}
	catch (Tango::DevFailed &e)
	{
		if (allocated == true)
			delete adm_dev;
        string reason(e.errors[0].reason.in());
        if (reason == "API_CommandNotFound")
            throw;
        else
            EventSystemExcept::re_throw_exception(e,(const char*)"API_DSFailedRegisteringEvent",
                       				(const char*)"Device server send exception while trying to register event",
                      				(const char*)"EventConsumer::connect_event()");
	}

//	if (allocated == true)
//		delete adm_dev;

//
// Search (or create) entry for channel map
//

	if (ipos == device_channel_map.end())
	{
		cout3 << "device " << device_name << " is not connected, going to connect to the event channel !\n";
		try
		{
            connect(device,device_name,dd,adm_name);
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

			o << "Failed to connect to event channel for device " << device_name << ends;
			EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                       				o.str(),
                       				(const char*)"EventConsumer::connect_event()");
		}

		if (evt_it == channel_map.end())
		{
			evt_it = channel_map.find(ipos->second);
			evt_it->second.last_subscribed = time(NULL);
		}
	}
	else
	{
		evt_it = channel_map.find(ipos->second);
	}

//
// Init device proxy in channel event map
//

    {
        AutoTangoMonitor _mon(evt_it->second.channel_monitor);
        evt_it->second.adm_device_proxy = adm_dev;
    }

//
// Now, connect to the event system
//

    EventCallBackStruct new_event_callback;
    EventSubscribeStruct new_ess;

    new_event_callback.device = device;
    new_event_callback.attr_name = attribute;
    new_event_callback.event_name = event_name;
    new_event_callback.channel_name = evt_it->first;
    new_event_callback.fully_qualified_event_name = device_name + '/' + att_name_lower + '.' + event_name;

    const DevVarLongStringArray *dvlsa;

    if ((dd >> dvlsa) == false)
        new_event_callback.device_idl = 0;
    else
    {
        if (dvlsa->lvalue.length() >= 2)
            new_event_callback.device_idl = dvlsa->lvalue[1];
        else
            new_event_callback.device_idl = 0;
    }
    new_event_callback.ctr = 1;

    new_ess.callback = callback;
    new_ess.ev_queue = ev_queue;

	connect_event_system(device_name,att_name_lower,event_name,filters,evt_it,new_event_callback,dd);

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
// Create a callback monitor and set its timeout to 500ms not to block the event consumer for too long.
//

    new_event_callback.callback_monitor = new TangoMonitor();
    new_event_callback.callback_monitor->timeout(500);

    pair<EvCbIte, bool> ret = event_callback_map.insert(pair<string, EventCallBackStruct>(local_callback_key, new_event_callback));
    if (!ret.second)
    {
        TangoSys_OMemStream o;
        o << "Failed to connect to event channel for device " << device_name
          << "\nCorrupted internal map: event callback already exists. Please report bug!" << ends;
        EventSystemExcept::throw_exception((const char*)"API_NotificationServiceFailed",
                                o.str(),
                                (const char*)"EventConsumer::connect_event()");
    }
    iter = ret.first;

//
// Read the attribute by a simple synchronous call
// This is necessary for the first point in "change" mode
// Force callback execution when it is done
//

	get_fire_sync_event(device,callback,ev_queue,event,event_name,attribute,iter->second);

//
// Sleep for some mS (20) in order to give to ZMQ some times to propagate the subscription
// to the publisher
//

#ifndef _TG_WINDOWS_
    struct timespec ts;
    ts.tv_nsec = 20000000;
    ts.tv_sec = 0;

    nanosleep(&ts,NULL);
#else
	Sleep(20);
#endif

	return ret_event_id;
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
// Do not take a WriterLock because the push_structured_event method already holds
// a Reader lock
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

					if (epos->second.callback_monitor->get_locking_ctr() != 0)
					{
//						cout << event_id << ": Unsubscribing for an event while it is in its callback !!!!!!!!!!" << endl;
						esspos->id = -event_id;

						DelayedEventUnsubThread *th = new DelayedEventUnsubThread(this,event_id,epos->second.callback_monitor);
						th->start();

						return;
					}
				}
			}
		}
	}

//
// Ask the main ZMQ thread to delay all incoming event until this meethod
// exit. A dead lock could happen if we don't do this (really experienced!)
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
//				cout << "Tango::EventConsumer::unsubscribe_event() - found event id " << event_id << " going to remove_filter()\n";

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
					    disconnect_event(evt_cb.fully_qualified_event_name);
					}

					// delete the allocated callback monitor
					delete epos->second.callback_monitor;

					string deleted_channel_name = epos->second.channel_name;
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
                                        disconnect_event_channel(deleted_channel_name);
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

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_events()
//
// description :  Return a vector with all events stored in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied!
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void EventConsumer::get_events (int event_id, EventDataList &event_list)
{

	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_events()
//
// description :  Return a vector with all attribute configuration events
//                stored in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied!
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void EventConsumer::get_events (int event_id, AttrConfEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

void EventConsumer::get_events (int event_id, DataReadyEventDataList &event_list)
{
	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_events()
//
// description :  Call the callback method for all events stored
//                in the event queue.
//                Events are kept in the buffer since the last extraction
//                with get_events().
//                After returning the event data, the event queue gets
//                emptied!
//
// argument : in  : event_id   : The event identifier
// argument : out : event_list : A reference to an event data list to be filled
//-----------------------------------------------------------------------------
void EventConsumer::get_events (int event_id, CallBack *cb)
{

	cout3 << "EventConsumer::get_events() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
					        o.str(),
					        (const char *)"EventConsumer::get_events()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::event_queue_size()
//
// description :  Returns the number of events stored in the event queue
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------
int  EventConsumer::event_queue_size(int event_id)
{
	cout3 << "EventConsumer::event_queue_size() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
					        o.str(),
					        (const char *)"EventConsumer::event_queue_size()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::is_event_queue_empty()
//
// description :  Returns true when the event queue is empty
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------
bool EventConsumer::is_event_queue_empty(int event_id)
{
	cout3 << "EventConsumer::is_event_queue_empty() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
					        o.str(),
					        (const char *)"EventConsumer::is_event_queue_empty()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_last_event_date()
//
// description :  Get the time stamp of the last inserted event
//
// argument : in : event_id   : The event identifier
//
//-----------------------------------------------------------------------------
TimeVal EventConsumer::get_last_event_date(int event_id)
{
	cout3 << "EventConsumer::get_last_event_date() : event_id = " << event_id << endl;

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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        	o.str(),
				        	(const char *)"EventConsumer::get_last_event_date()");
				}
			}
		}
	}

	//
	// check also the vector of not yet connected events
	//

	if ( event_not_connected.size() > 0 )
	{
		std::vector<EventNotConnected>::iterator vpos;
		for (vpos =  event_not_connected.begin();
			 vpos != event_not_connected.end(); vpos++)
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
					o << "Cannot return any event data" << ends;
					EventSystemExcept::throw_exception((const char *)"API_EventQueues",
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

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::add_new_callback()
//
// description :  Add a new callback to an already existing event entry
//				  in the callback map
//
// argument : in : iter : Iterator in the callback map
//				   callback : Pointer to the Callback object
//				   ev_queue : Pointer to the event queue
//				   event_id : The event identifier
//
//-----------------------------------------------------------------------------

int EventConsumer::add_new_callback(EvCbIte &iter,CallBack *callback,EventQueue *ev_queue,int event_id)
{
	EventSubscribeStruct ess;
	int ret_event_id = event_id;

	if (ret_event_id <= 0)
	{
		subscribe_event_id++;
		ret_event_id = subscribe_event_id;
	}

	ess.id = ret_event_id;
	ess.callback = callback;
	ess.ev_queue = ev_queue;

	iter->second.callback_list.push_back(ess);

	return ret_event_id;
}

//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_fire_sync_event()
//
// description :  Get event data and fire a synchronous event
//
// argument : in : - device : The device pointer
//				   - callback : The callback pointer
//				   - ev_queue : The event queue
//				   - event : The event type
//				   - event_name : The event name
//				   - attribute : The attribute name
//
//-----------------------------------------------------------------------------

void EventConsumer::get_fire_sync_event(DeviceProxy *device,CallBack *callback,EventQueue *ev_queue,EventType event,string &event_name,const string &attribute,EventCallBackStruct &cb)
{
	if ((event == CHANGE_EVENT) ||
	    (event == QUALITY_EVENT) ||
	    (event == ARCHIVE_EVENT) ||
	    (event == USER_EVENT))
	{
		//DeviceAttribute da;
		DeviceAttribute *da = NULL;
		DevErrorList err;
		err.length(0);

		string domain_name;
		string::size_type pos;

		if ((pos = device_name.find(MODIFIER_DBASE_NO)) != string::npos)
		{
            domain_name = device_name;
            string tmp = '/' + att_name_lower;
            domain_name.insert(pos,tmp);
		}
		else
            domain_name = device_name + '/' + att_name_lower;

		try
		{
			da = new DeviceAttribute();
			*da = device->read_attribute(attribute.c_str());
			if (da->has_failed() == true)
			{
				err = da->get_err_stack();
				err.length(err.length() - 1);
			}
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}

		EventData *event_data = new EventData(device,
						      domain_name,
						      event_name,
						      da,
						      err);
		AutoTangoMonitor _mon(cb.callback_monitor);

		// if a callback method was specified, call it!
		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
			}

			//event_data->attr_value = NULL;
			delete event_data;
		}

		// no calback method, the event has to be instered
		// into the event queue
		else
		{
			ev_queue->insert_event(event_data);
		}
	}


	else if (event == ATTR_CONF_EVENT)
	{
		DevErrorList err;
		err.length(0);
		string domain_name = device_name + "/" + att_name_lower;
		//AttributeInfoEx aie;
		AttributeInfoEx *aie = NULL;

		try
		{
			aie = new AttributeInfoEx();
			*aie = device->get_attribute_config(const_cast<string &>(attribute));
		}
		catch (DevFailed &e)
		{
			err = e.errors;
		}

		AttrConfEventData *event_data = new AttrConfEventData(device,
						      domain_name,
						      event_name,
						      aie,
						      err);
		AutoTangoMonitor _mon(cb.callback_monitor);

		// if a callback method was specified, call it!
		if (callback != NULL )
		{
			try
			{
				callback->push_event(event_data);
			}
			catch (...)
			{
				cerr << "EventConsumer::subscribe_event() exception in callback method of " << callback_key << endl;
			}
		//event_data->attr_conf = NULL;
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


//+----------------------------------------------------------------------------
//
// method :       EventConsumer::get_event_system_for_event_id()
//
// description :  Get which event system is used by one event from its id
//
// argument : in : - event_id : The event id
//
// This methods returns which is the event system type used by the
// event with the specified event id
//
//-----------------------------------------------------------------------------

ChannelType EventConsumer::get_event_system_for_event_id(int event_id)
{
    ChannelType ret = Tango::ZMQ;
    EvCbIte epos;
	vector<EventSubscribeStruct>::iterator esspos;

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
                    o << "Corrupted internal map. Please report bug" << ends;
                    Except::throw_exception((const char *)"API_BadConfigurationProperty",
									o.str(),
									(const char *)"EventConsumer::get_event_system_for_event_id()");
                }
                EventChannelStruct &evt_ch = evt_it->second;
                ret = evt_ch.channel_type;
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

EventData::EventData(DeviceProxy *dev,string &nam,string &evt,
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
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (CORBA::Long)t.time;
		reception_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (CORBA::Long)tv.tv_sec;
		reception_date.tv_usec = (CORBA::Long)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
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

AttrConfEventData::AttrConfEventData(DeviceProxy *dev,string &nam,string &evt,
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
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (CORBA::Long)t.time;
		reception_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (CORBA::Long)tv.tv_sec;
		reception_date.tv_usec = (CORBA::Long)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
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

DataReadyEventData::DataReadyEventData(DeviceProxy *dev,AttDataReady *dr,string &evt,DevErrorList &errors_in)
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
#ifdef _TG_WINDOWS_
		struct _timeb t;
		_ftime(&t);

		reception_date.tv_sec  = (CORBA::Long)t.time;
		reception_date.tv_usec = (CORBA::Long)(t.millitm * 1000);
		reception_date.tv_nsec = 0;
#else
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);

		reception_date.tv_sec  = (CORBA::Long)tv.tv_sec;
		reception_date.tv_usec = (CORBA::Long)tv.tv_usec;
		reception_date.tv_nsec = 0;
#endif
}

} /* End of Tango namespace */


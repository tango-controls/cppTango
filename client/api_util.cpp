static const char *RcsId = "$Id$\n$Name$";

//-----------------------------------------------------------------------------
//
// cpp 	- C++ source code file for TANGO api class ApiUtil
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// Copyright (C) :      2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// GNU General Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// original 	- May 2002
//
// $Revision$
//
//-----------------------------------------------------------------------------

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventconsumer.h>

#ifndef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>

// There is a NO_DATA defined in netdb.h

#ifdef NO_DATA
#undef NO_DATA
#endif

#else
#include <ws2tcpip.h>
#include <process.h>
#endif


namespace Tango
{

ApiUtil *ApiUtil::_instance = NULL;
omni_mutex ApiUtil::inst_mutex;

//+----------------------------------------------------------------------------
//
// method : 		ApiUtil::ApiUtil()
//
// description : 	Constructor of the ApiUtil class.
//
//-----------------------------------------------------------------------------

ApiUtil::ApiUtil():exit_lock_installed(false),reset_already_executed_flag(false),ext(new ApiUtilExt)
{
	_orb = CORBA::ORB::_nil();

//
// Check if it is created from a device server
//

	if (Util::_constructed == true)
		in_serv = true;
	else
		in_serv = false;

//
// Create the Asynchronous polling request Id generator
//

	UniqIdent *ui = new UniqIdent();

//
// Create the table used to store asynchronous polling requests
//

	asyn_p_table = new AsynReq(ui);

//
// Set the asynchronous callback mode to "ON_CALL"
//

	auto_cb = PULL_CALLBACK;
	cb_thread_ptr = NULL;

//
// Get the process PID
//

#ifdef _TG_WINDOWS_
	ext->cl_pid = _getpid();
#else
	ext->cl_pid = getpid();
#endif

//
// Check if the user has defined his own connection timeout
//

	string var;
	if (get_env_var("TANGOconnectTimeout",var) == 0)
	{
		int user_to = -1;
		istringstream iss(var);
		iss >> user_to;
		if (iss)
			ext->user_connect_timeout = user_to;
	}

//
// Check if the user has defined his own subscriber hwm (fpr zmq event tuning)
//

	var.clear();
	if (get_env_var("TANGO_EVENT_BUFFER_HWM",var) == 0)
	{
		int sub_hwm = -1;
		istringstream iss(var);
		iss >> sub_hwm;
		if (iss)
			ext->user_sub_hwm = sub_hwm;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ApiUtil::~ApiUtil()
//
// description : 	Destructor of the ApiUtil class.
//
//-----------------------------------------------------------------------------

ApiUtil::~ApiUtil()
{
//
// Release Asyn stuff
//

	delete asyn_p_table;

	if (cb_thread_ptr != NULL)
	{
		cb_thread_cmd.stop_thread();
		cb_thread_ptr->join(0);
	}

//
// Kill any remaining locking threads
//

	clean_locking_threads();

//
// Release event stuff (in case it is necessary)
//

	bool event_was_used = false;

#ifdef HAS_UNIQUE_PTR
	if (ext.get() != NULL)
#else
	if (ext != NULL)
#endif
	{
		if ((ext->notifd_event_consumer != NULL) || (ext->zmq_event_consumer != NULL))
		{
			event_was_used = true;
			leavefunc();
			NotifdEventConsumer::cleanup();
			ZmqEventConsumer::cleanup();
		}
#ifndef HAS_UNIQUE_PTR
		delete ext;
#endif
	}

//
// Delete the database object
//

	for (unsigned int i = 0;i < db_vect.size();i++)
	{
		delete db_vect[i];
	}

//
// Properly shutdown the ORB
//

	if ((in_serv == false) && (CORBA::is_nil(_orb) == false))
	{
		if (event_was_used == false)
		{
			try
			{
				_orb->destroy();
			}
			catch (...) {cout << "In the exception handler"<<endl;}
		}
		CORBA::release(_orb);
	}

}

//-----------------------------------------------------------------------------
//
// ApiUtil::create_orb() - create a CORBA orb object
//
//-----------------------------------------------------------------------------

void ApiUtil::create_orb()
{
	int _argc;
	char **_argv;

//
// pass dummy arguments to init() because we don't have access to
// argc and argv
//

	_argc = 1;
	_argv = (char**)malloc(sizeof(char*));
	_argv[0] = (char*)"dummy";

//
// Init the ORB
//

	const char *options[][2] = {
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT_STR},
			{"verifyObjectExistsAndType","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};

	_orb = CORBA::ORB_init(_argc,_argv,"omniORB4",options);

	free(_argv);
}

//-----------------------------------------------------------------------------
//
// ApiUtil::get_db_ind() - Retrieve a Tango database object created from the
//			   TANGO_HOST environment variable
//
//-----------------------------------------------------------------------------

int ApiUtil::get_db_ind()
{
	omni_mutex_lock lo(the_mutex);

	for (unsigned int i = 0;i < db_vect.size();i++)
	{
		if (db_vect[i]->get_from_env_var() == true)
			return i;
	}

//
// The database object has not been found, create it
//

	db_vect.push_back(new Database());

	return (db_vect.size() - 1);

}

int ApiUtil::get_db_ind(string &host,int port)
{
	omni_mutex_lock lo(the_mutex);

	for (unsigned int i = 0;i < db_vect.size();i++)
	{
		if (db_vect[i]->get_db_port_num() == port)
		{
			if (db_vect[i]->get_db_host() == host)
				return i;
		}
	}

//
// The database object has not been found, create it
//

	db_vect.push_back(new Database(host,port));

	return (db_vect.size() - 1);

}


//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::get_asynch_replies()
//
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. Fire callback for replies alaredy
//			arrived
//
//-----------------------------------------------------------------------------

void ApiUtil::get_asynch_replies()
{

//
// First get all replies from ORB buffers
//

	try
	{
		while (_orb->poll_next_response() == true)
		{
			CORBA::Request_ptr req;
			_orb->get_next_response(req);

//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

			TgRequest &tg_req = asyn_p_table->get_request(req);

			tg_req.arrived = true;
			asyn_p_table->mark_as_arrived(req);

//
// Process request
//

			switch (tg_req.req_type)
			{
			case TgRequest::CMD_INOUT :
				tg_req.dev->Cb_Cmd_Request(req,tg_req.cb_ptr);
				break;

			case TgRequest::READ_ATTR :
				tg_req.dev->Cb_ReadAttr_Request(req,tg_req.cb_ptr);
				break;

			case TgRequest::WRITE_ATTR :
			case TgRequest::WRITE_ATTR_SINGLE :
				tg_req.dev->Cb_WriteAttr_Request(req,tg_req.cb_ptr);
				break;
			}

			tg_req.dev->dec_asynch_counter(CALL_BACK);
			asyn_p_table->remove_request(tg_req.dev,req);
		}
	}
	catch (CORBA::BAD_INV_ORDER &e)
	{
		if (e.minor() != omni::BAD_INV_ORDER_RequestNotSentYet)
			throw;
	}

//
// For all replies already there
//

	multimap<Connection *,TgRequest>::iterator pos;
	multimap<Connection *,TgRequest> &req_table = asyn_p_table->get_cb_dev_table();

	for (pos = req_table.begin();pos != req_table.end();++pos)
	{
		if (pos->second.arrived == true)
		{
			switch (pos->second.req_type)
			{
			case TgRequest::CMD_INOUT :
				pos->first->Cb_Cmd_Request(pos->second.request,pos->second.cb_ptr);
				break;

			case TgRequest::READ_ATTR :
				pos->first->Cb_ReadAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;

			case TgRequest::WRITE_ATTR :
			case TgRequest::WRITE_ATTR_SINGLE :
				pos->first->Cb_WriteAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;
			}
			pos->first->dec_asynch_counter(CALL_BACK);
			asyn_p_table->remove_request(pos->first,pos->second.request);
		}

	}

}


//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::get_asynch_replies()
//
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. Fire callback for replies alaredy
//			arrived
//
// argin(s) :		call_timeout : The timeout value in mS
//
//-----------------------------------------------------------------------------

void ApiUtil::get_asynch_replies(long call_timeout)
{
//
// For all replies already there
//

	multimap<Connection *,TgRequest>::iterator pos;
	multimap<Connection *,TgRequest> &req_table = asyn_p_table->get_cb_dev_table();

	for (pos = req_table.begin();pos != req_table.end();++pos)
	{
		if (pos->second.arrived == true)
		{
			switch (pos->second.req_type)
			{
			case TgRequest::CMD_INOUT :
				pos->first->Cb_Cmd_Request(pos->second.request,pos->second.cb_ptr);
				break;

			case TgRequest::READ_ATTR :
				pos->first->Cb_ReadAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;

			case TgRequest::WRITE_ATTR :
			case TgRequest::WRITE_ATTR_SINGLE :
				pos->first->Cb_WriteAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;
			}
			pos->first->dec_asynch_counter(CALL_BACK);
			asyn_p_table->remove_request(pos->first,pos->second.request);
		}

	}

//
// If they are requests already sent but without being replied yet
//

	if (asyn_p_table->get_cb_request_nb() != 0)
	{
		CORBA::Request_ptr req;

		if (call_timeout != 0)
		{
//
// A timeout has been specified. Wait if there are still request without
// replies but not more than the specified timeout. Leave method if the
// timeout is not arrived but there is no more request without reply
//

			long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
			struct timespec to_wait,inter;
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 20000000;
#endif

			while ((nb > 0) && (asyn_p_table->get_cb_request_nb() != 0))
			{
#ifdef _TG_WINDOWS_
				Sleep(20);
#else
				nanosleep(&to_wait,&inter);
#endif
				nb--;

				try
				{
					if (_orb->poll_next_response() == true)
					{
						_orb->get_next_response(req);

//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

						TgRequest &tg_req = asyn_p_table->get_request(req);

						tg_req.arrived = true;
						asyn_p_table->mark_as_arrived(req);

//
// Is it a request for our device, process it ?
//

						switch (tg_req.req_type)
						{
						case TgRequest::CMD_INOUT :
							tg_req.dev->Cb_Cmd_Request(req,tg_req.cb_ptr);
							break;

						case TgRequest::READ_ATTR :
							tg_req.dev->Cb_ReadAttr_Request(req,tg_req.cb_ptr);
							break;

						case TgRequest::WRITE_ATTR :
						case TgRequest::WRITE_ATTR_SINGLE :
							tg_req.dev->Cb_WriteAttr_Request(req,tg_req.cb_ptr);
							break;
						}

						tg_req.dev->dec_asynch_counter(CALL_BACK);
						asyn_p_table->remove_request(tg_req.dev,req);
					}
				}
				catch (CORBA::BAD_INV_ORDER &e)
				{
					if (e.minor() != omni::BAD_INV_ORDER_RequestNotSentYet)
						throw;
				}
			}

//
// Throw exception if the timeout has expired but there are still request
// without replies
//

			if ((nb == 0) && (asyn_p_table->get_cb_request_nb() != 0))
			{
				TangoSys_OMemStream desc;
				desc << "Still some reply(ies) for asynchronous callback call(s) to be received" << ends;
				ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"ApiUtil::get_asynch_replies");
			}
		}
		else
		{
//
// If timeout is set to 0, this means wait until all the requests sent to this
// device has sent their replies
//

			while (asyn_p_table->get_cb_request_nb() != 0)
			{
				try
				{
					_orb->get_next_response(req);

//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

					TgRequest &tg_req = asyn_p_table->get_request(req);

					tg_req.arrived = true;
					asyn_p_table->mark_as_arrived(req);

//
// Process the reply
//

					switch (tg_req.req_type)
					{
					case TgRequest::CMD_INOUT :
						tg_req.dev->Cb_Cmd_Request(req,tg_req.cb_ptr);
						break;

					case TgRequest::READ_ATTR :
						tg_req.dev->Cb_ReadAttr_Request(req,tg_req.cb_ptr);
						break;

					case TgRequest::WRITE_ATTR :
					case TgRequest::WRITE_ATTR_SINGLE :
						tg_req.dev->Cb_WriteAttr_Request(req,tg_req.cb_ptr);
						break;
					}

					tg_req.dev->dec_asynch_counter(CALL_BACK);
					asyn_p_table->remove_request(tg_req.dev,req);
				}
				catch (CORBA::BAD_INV_ORDER &e)
				{
					if (e.minor() != omni::BAD_INV_ORDER_RequestNotSentYet)
						throw;
				}
			}
		}
	}

}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::set_asynch_cb_sub_model()
//
// description : 	Set the callback automatic mode (Fired by dedicated call
//			or automatically fired by a separate thread)
//
// argin(s) :		mode : The new callback mode
//
//-----------------------------------------------------------------------------

void ApiUtil::set_asynch_cb_sub_model(cb_sub_model mode)
{

	if (auto_cb == PULL_CALLBACK)
	{
		if (mode == PUSH_CALLBACK)
		{

//
// In this case, delete the old object in case it is needed, create a new
// thread and start it
//

            delete cb_thread_ptr;
            cb_thread_ptr = NULL;

			cb_thread_cmd.start_thread();

			cb_thread_ptr = new CallBackThread(cb_thread_cmd,asyn_p_table);
			cb_thread_ptr->start();
			auto_cb = PUSH_CALLBACK;
		}
	}
	else
	{
		if (mode == PULL_CALLBACK)
		{

//
// Ask the thread to stop and to exit
//

			cb_thread_cmd.stop_thread();
			auto_cb = PULL_CALLBACK;
			asyn_p_table->signal();
		}
	}

}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::create_xxx_event_consumer()
//
// description : 	Create the event consumer. This will automatically
//			        start a new thread which is waiting in a CORBA::run()
//			        indefintely for events. It will then trigger the events.
//
// argin(s) :		none
//
//-----------------------------------------------------------------------------

void ApiUtil::create_notifd_event_consumer()
{
    ext->notifd_event_consumer = NotifdEventConsumer::create();
}

void ApiUtil::create_zmq_event_consumer()
{
    ext->zmq_event_consumer = ZmqEventConsumer::create();
}

NotifdEventConsumer *ApiUtil::get_notifd_event_consumer()
{
	return ext->notifd_event_consumer;
}

ZmqEventConsumer *ApiUtil::get_zmq_event_consumer()
{
	return ext->zmq_event_consumer;
}

//+----------------------------------------------------------------------------
//
// method : 		ApiUtil::clean_locking_threads()
//
// description : 	Ask all remaining locking threads to exit
//
//-----------------------------------------------------------------------------

void ApiUtil::clean_locking_threads(bool clean)
{
	omni_mutex_lock oml(lock_th_map);

	if (lock_threads.empty() == false)
	{
		map<string,LockingThread>::iterator pos;
		for (pos = lock_threads.begin();pos != lock_threads.end();++pos)
		{
			if (pos->second.shared->suicide == true)
			{
				delete pos->second.shared;
				delete pos->second.mon;
			}
			else
			{
				{
					omni_mutex_lock sync(*(pos->second.mon));

					pos->second.shared->cmd_pending = true;
					if (clean == true)
						pos->second.shared->cmd_code = LOCK_UNLOCK_ALL_EXIT;
					else
						pos->second.shared->cmd_code = LOCK_EXIT;

					pos->second.mon->signal();

					cout4 << "Cmd sent to locking thread" << endl;

					if (pos->second.shared->cmd_pending == true)
						pos->second.mon->wait(DEFAULT_TIMEOUT);
				}
				delete pos->second.shared;
				delete pos->second.mon;
			}
		}

		if (clean == false)
			lock_threads.clear();
	}
}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::attr_to_device()
//
// description : 	Create the event consumer. This will automatically
//			start a new thread which is waiting in a CORBA::run()
//			indefintely for events. It will then trigger the events.
//
// argin(s) :		none
//
//-----------------------------------------------------------------------------

void ApiUtil::attr_to_device(const AttributeValue *attr_value,
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
	const DevVarLong64Array *tmp_seq_64;
	CORBA::LongLong *tmp_lolo;
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

		if (ty->kind() == CORBA::tk_enum)
		{
			attr_value_3->value >>= dev_attr->d_state;
			dev_attr->d_state_filled = true;
			return;
		}

		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();
		switch (ty_seq->kind())
		{
			case CORBA::tk_long:
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

			case CORBA::tk_longlong:
				if (vers == 3)
					attr_value_3->value >>= tmp_seq_64;
				else
					attr_value->value >>= tmp_seq_64;
				max = tmp_seq_64->maximum();
				len = tmp_seq_64->length();
				if (tmp_seq_64->release() == true)
				{
					tmp_lolo = (const_cast<DevVarLong64Array *>(tmp_seq_64))->get_buffer((CORBA::Boolean)true);
					dev_attr->set_Long64_data(new DevVarLong64Array(max,len,tmp_lolo,true));
				}
				else
				{
					tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
					dev_attr->set_Long64_data(new DevVarLong64Array(max,len,tmp_lolo,false));
				}
				break;

			case CORBA::tk_short:
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

			case CORBA::tk_double:
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

			case CORBA::tk_string:
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

			case CORBA::tk_float:
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

			case CORBA::tk_boolean:
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

			case CORBA::tk_ushort:
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

			case CORBA::tk_octet:
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

			case CORBA::tk_ulong:
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

			case CORBA::tk_ulonglong:
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

			case CORBA::tk_enum:
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

void ApiUtil::attr_to_device(const AttributeValue_4 *attr_value_4,TANGO_UNUSED(long vers),DeviceAttribute *dev_attr)
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
	Tango::DevEncoded *tmp_enc;

	CORBA::ULong max,len;

	dev_attr->name = attr_value_4->name;
	dev_attr->quality = attr_value_4->quality;
	dev_attr->data_format = attr_value_4->data_format;
	dev_attr->time = attr_value_4->time;
	dev_attr->dim_x = attr_value_4->r_dim.dim_x;
	dev_attr->dim_y = attr_value_4->r_dim.dim_y;
	dev_attr->set_w_dim_x(attr_value_4->w_dim.dim_x);
	dev_attr->set_w_dim_y(attr_value_4->w_dim.dim_y);
	dev_attr->set_err_list(new DevErrorList(attr_value_4->err_list));

	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		switch (attr_value_4->value._d())
		{
			case ATT_BOOL:
			{
				const DevVarBooleanArray &tmp_seq = attr_value_4->value.bool_att_value();
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
				const DevVarShortArray &tmp_seq = attr_value_4->value.short_att_value();
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
				const DevVarLongArray &tmp_seq = attr_value_4->value.long_att_value();
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
				const DevVarLong64Array &tmp_seq = attr_value_4->value.long64_att_value();
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
				const DevVarFloatArray &tmp_seq = attr_value_4->value.float_att_value();
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
				const DevVarDoubleArray &tmp_seq = attr_value_4->value.double_att_value();
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
				const DevVarCharArray &tmp_seq = attr_value_4->value.uchar_att_value();
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
				const DevVarUShortArray &tmp_seq = attr_value_4->value.ushort_att_value();
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
				const DevVarULongArray &tmp_seq = attr_value_4->value.ulong_att_value();
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
				const DevVarULong64Array &tmp_seq = attr_value_4->value.ulong64_att_value();
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
				const DevVarStringArray &tmp_seq = attr_value_4->value.string_att_value();
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
				const DevVarStateArray &tmp_seq = attr_value_4->value.state_att_value();
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
			{
				dev_attr->d_state = attr_value_4->value.dev_state_att();
				dev_attr->d_state_filled = true;
			}
			break;

			case ATT_ENCODED:
			{
				const DevVarEncodedArray &tmp_seq = attr_value_4->value.encoded_att_value();
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

			case NO_DATA:
			break;
		}
	}
}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::device_to_attr()
//
// description :
//
// argin(s) :		none
//
//-----------------------------------------------------------------------------

void ApiUtil::device_to_attr(const DeviceAttribute &dev_attr,AttributeValue_4 &att)
{
	att.name = dev_attr.name.c_str();
	att.quality = dev_attr.quality;
	att.time = dev_attr.time;
	att.w_dim.dim_x = dev_attr.dim_x;
	att.w_dim.dim_y = dev_attr.dim_y;
	att.data_format = Tango::FMT_UNKNOWN;

	if (dev_attr.LongSeq.operator->() != NULL)
		att.value.long_att_value(dev_attr.LongSeq.in());
	else if (dev_attr.ShortSeq.operator->() != NULL)
		att.value.short_att_value(dev_attr.ShortSeq.in());
	else if (dev_attr.DoubleSeq.operator->() != NULL)
		att.value.double_att_value(dev_attr.DoubleSeq.in());
	else if (dev_attr.StringSeq.operator->() != NULL)
		att.value.string_att_value(dev_attr.StringSeq.in());
	else if (dev_attr.FloatSeq.operator->() != NULL)
		att.value.float_att_value(dev_attr.FloatSeq.in());
	else if (dev_attr.BooleanSeq.operator->() != NULL)
		att.value.bool_att_value(dev_attr.BooleanSeq.in());
	else if (dev_attr.UShortSeq.operator->() != NULL)
		att.value.ushort_att_value(dev_attr.UShortSeq.in());
	else if (dev_attr.UCharSeq.operator->() != NULL)
		att.value.uchar_att_value(dev_attr.UCharSeq.in());
	else if (dev_attr.get_Long64_data().operator->() != NULL)
		att.value.long64_att_value(dev_attr.get_Long64_data().in());
	else if (dev_attr.get_ULong_data().operator->() != NULL)
		att.value.ulong_att_value(dev_attr.get_ULong_data().in());
	else if (dev_attr.get_ULong64_data().operator->() != NULL)
		att.value.ulong64_att_value(dev_attr.get_ULong64_data().in());
	else if (dev_attr.get_State_data().operator->() != NULL)
		att.value.state_att_value(dev_attr.get_State_data().in());
	else if (dev_attr.get_Encoded_data().operator->() != NULL)
		att.value.encoded_att_value(dev_attr.get_Encoded_data().in());
}

void ApiUtil::device_to_attr(const DeviceAttribute &dev_attr,AttributeValue &att,string &d_name)
{

	att.name = dev_attr.name.c_str();
	att.quality = dev_attr.quality;
	att.time = dev_attr.time;
	att.dim_x = dev_attr.dim_x;
	att.dim_y = dev_attr.dim_y;

	if (dev_attr.LongSeq.operator->() != NULL)
		att.value <<= dev_attr.LongSeq.in();
	else if (dev_attr.ShortSeq.operator->() != NULL)
		att.value <<= dev_attr.ShortSeq.in();
	else if (dev_attr.DoubleSeq.operator->() != NULL)
		att.value <<= dev_attr.DoubleSeq.in();
	else if (dev_attr.StringSeq.operator->() != NULL)
		att.value  <<= dev_attr.StringSeq.in();
	else if (dev_attr.FloatSeq.operator->() != NULL)
		att.value <<= dev_attr.FloatSeq.in();
	else if (dev_attr.BooleanSeq.operator->() != NULL)
		att.value <<= dev_attr.BooleanSeq.in();
	else if (dev_attr.UShortSeq.operator->() != NULL)
		att.value <<= dev_attr.UShortSeq.in();
	else if (dev_attr.UCharSeq.operator->() != NULL)
		att.value  <<= dev_attr.UCharSeq.in();
	else if (dev_attr.get_Long64_data().operator->() != NULL)
		att.value  <<= dev_attr.get_Long64_data().in();
	else if (dev_attr.get_ULong_data().operator->() != NULL)
		att.value  <<= dev_attr.get_ULong_data().in();
	else if (dev_attr.get_ULong64_data().operator->() != NULL)
		att.value  <<= dev_attr.get_ULong64_data().in();
	else if (dev_attr.get_State_data().operator->() != NULL)
		att.value  <<= dev_attr.get_State_data().in();
	else if (dev_attr.get_Encoded_data().operator->() != NULL)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << d_name;
		desc << " does not support DevEncoded data type" << ends;
		ApiNonSuppExcept::throw_exception((const char *)"API_UnsupportedFeature",
						  	desc.str(),
						  	(const char *)"DeviceProxy::device_to_attr");
	}
}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::get_env_var()
//
// description :
//
// argin(s) :		- env_var_name : The environment variable name
//					- env_var : Reference to the string initialised with
//								the env. variable value (if found)
//
// This method returns 0 if the env. variable is found and -1 otherwise
//
//-----------------------------------------------------------------------------

int ApiUtil::get_env_var(const char *env_var_name,string &env_var)
{
	DummyDeviceProxy d;
	return d.get_env_var(env_var_name,env_var);
}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::get_ip_from_if()
//
// description : Get host IP address from its network interface
//
// argin(s) : out : - ip_adr_list : Host IP address
//
//-----------------------------------------------------------------------------

void ApiUtil::get_ip_from_if(vector<string> &ip_adr_list)
{
    if (ext->host_ip_adrs.empty() == true)
    {
#ifndef _TG_WINDOWS_
		int sock = socket(AF_INET,SOCK_STREAM,0);

		int lastlen = 0;
		int len = 100 * sizeof(struct ifreq);
		struct ifconf ifc;

//
// There is no way to know for sure the buffer is big enough to get
// the info for all the interfaces. We work around this by calling
// the ioctl 2 times and increases the buffer size in the 2nd call.
// If both calls return the info with the same size, we know we have
// got all the interfaces.
//

		while (true)
		{
			ifc.ifc_buf = new char[len];
			ifc.ifc_len = len;

			if (ioctl(sock,SIOCGIFCONF,&ifc) < 0)
			{
				if (errno != EINVAL || lastlen != 0)
				{
					delete[] ifc.ifc_buf;
					close(sock);

					cerr << "Warning: ioctl SIOCGICONF failed" << endl;
					cerr << "Unable to obtain the list of all interface addresses." << endl;

					Tango::Except::throw_exception((const char *)"API_SystemCallFailed",
												   (const char *)"Can't retrieve list of all interfaces addresses (ioctl - SIOCGICONF)!",
												   (const char *)"ApiUtil::get_ip_from_if()");
				}
			}
			else
			{
				if (ifc.ifc_len == lastlen)
					break; // Success, len has not changed.
				lastlen = ifc.ifc_len;
			}
			len += 10 * sizeof(struct ifreq);
			delete[] ifc.ifc_buf;
		}

		close(sock);

//
// Convert IP addresses to string
//

		int total = ifc.ifc_len / sizeof(struct ifreq);
		struct ifreq *ifr = ifc.ifc_req;

		for (int i = 0; i < total; i++)
		{
			if ( ifr[i].ifr_addr.sa_family == AF_INET || ifr[i].ifr_addr.sa_family == AF_INET6 )
			{
				struct sockaddr_in *iaddr = (struct sockaddr_in *)&ifr[i].ifr_addr;
				struct sockaddr *addr = (struct sockaddr *)&ifr[i].ifr_addr;

				if ( iaddr->sin_addr.s_addr != 0 )
				{
					char dest[NI_MAXHOST];
					socklen_t addrlen = sizeof(sockaddr);

					int result = getnameinfo(addr, addrlen, dest, sizeof(dest),0,0,NI_NUMERICHOST);
					if (result != 0)
					{
						delete[] ifc.ifc_buf;

						cerr << "Warning: getnameinfo failed" << endl;
						cerr << "Unable to convert IP address to string (getnameinfo)." << endl;

						Tango::Except::throw_exception((const char *)"API_SystemCallFailed",
												   (const char *)"Can't convert IP address to string (getnameinfo)!",
												   (const char *)"ApiUtil::get_ip_from_if()");
					}
					string tmp_str(dest);
					ext->host_ip_adrs.push_back(tmp_str);
				}
			}
		}

		delete[] ifc.ifc_buf;
#else

//
// Get address from intrerfaces
//

		int sock = socket(AF_INET,SOCK_STREAM,0);

		INTERFACE_INFO info[64];  // Assume max 64 interfaces
		DWORD retlen;

		if ( WSAIoctl(sock, SIO_GET_INTERFACE_LIST, NULL,0,(LPVOID)&info, sizeof(info), (LPDWORD)&retlen,NULL,NULL) == SOCKET_ERROR )
		{
			closesocket(sock);

			int err = WSAGetLastError();
			cerr << "Warning: WSAIoctl failed" << endl;
			cerr << "Unable to obtain the list of all interface addresses. Error = " << err << endl;

			TangoSys_OMemStream desc;
			desc << "Can't retrieve list of all interfaces addresses (WSAIoctl)! Error = " << err << ends;

			Tango::Except::throw_exception((const char*)"API_SystemCallFailed",
										   (const char *)desc.str().c_str(),
										   (const char *)"ApiUtil::get_ip_from_if()");
		}
		closesocket(sock);

//
// Converts addresses to string
// Only for running interfaces
//

		int numAddresses = retlen / sizeof(INTERFACE_INFO);
		for (int i = 0; i < numAddresses; i++)
		{
			if (info[i].iiFlags & IFF_UP)
			{
				if (info[i].iiAddress.Address.sa_family == AF_INET || info[i].iiAddress.Address.sa_family == AF_INET6)
				{
					struct sockaddr* addr = (struct sockaddr*)&info[i].iiAddress.AddressIn;
					char dest[NI_MAXHOST];
					socklen_t addrlen = sizeof(sockaddr);

					int result = getnameinfo(addr, addrlen, dest, sizeof(dest),0,0,NI_NUMERICHOST);
					if (result != 0)
					{
						cerr << "Warning: getnameinfo failed" << endl;
						cerr << "Unable to convert IP address to string (getnameinfo)." << endl;

						Tango::Except::throw_exception((const char *)"API_SystemCallFailed",
												   (const char *)"Can't convert IP address to string (getnameinfo)!",
												   (const char *)"ApiUtil::get_ip_from_if()");
					}
					string tmp_str(dest);
					if (tmp_str != "0.0.0.0" && tmp_str != "0:0:0:0:0:0:0:0" &&tmp_str != "::")
						ext->host_ip_adrs.push_back(tmp_str);
				}
			}
		}
#endif
    }

//
// Copy host IP addresses into caller vector
//

    ip_adr_list.clear();
    copy(ext->host_ip_adrs.begin(),ext->host_ip_adrs.end(),back_inserter(ip_adr_list));
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing instance of the
//			AttributeInfo class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,AttributeInfo &p)
{


//
// Print all these properties
//

	o_str << "Attribute name = " << p.name << endl;
	o_str << "Attribute data_type = ";
	switch (p.data_type)
	{
	case Tango::DEV_SHORT :
		o_str << "Tango::DevShort" << endl;
		break;

	case Tango::DEV_LONG :
		o_str << "Tango::DevLong" << endl;
		break;

	case Tango::DEV_DOUBLE :
		o_str << "Tango::DevDouble" << endl;
		break;

	case Tango::DEV_STRING :
		o_str << "Tango::DevString" << endl;
		break;

	case Tango::DEV_FLOAT :
		o_str << "Tango::DevFloat" << endl;
		break;

	case Tango::DEV_USHORT :
		o_str << "Tango::DevUShort" << endl;
		break;

	case Tango::DEV_UCHAR :
		o_str << "Tango::DevUChar" << endl;
		break;

	case Tango::DEV_BOOLEAN :
		o_str << "Tango::DevBoolean" << endl;
		break;

	case Tango::DEV_STATE :
		o_str << "Tango::DevState" << endl;
		break;
	}

	o_str << "Attribute data_format = ";
	switch (p.data_format)
	{
	case Tango::FMT_UNKNOWN:
		break;

	case Tango::SCALAR :
		o_str << "scalar" << endl;
		break;

	case Tango::SPECTRUM :
		o_str << "spectrum, max_dim_x = " << p.max_dim_x << endl;
		break;

	case Tango::IMAGE :
		o_str << "image, max_dim_x = " << p.max_dim_x << ", max_dim_y = " << p.max_dim_y << endl;
		break;
	}

	if ((p.writable == Tango::WRITE) || (p.writable == Tango::READ_WRITE))
		o_str << "Attribute is writable" << endl;
	else
		o_str << "Attribute is not writable" << endl;
	o_str << "Attribute label = " << p.label << endl;
	o_str << "Attribute description = " << p.description << endl;
	o_str << "Attribute unit = " << p.unit;
	o_str << ", standard unit = " << p.standard_unit;
	o_str << ", display unit = " << p.display_unit << endl;
	o_str << "Attribute format = " << p.format << endl;
	o_str << "Attribute min alarm = " << p.min_alarm << endl;
	o_str << "Attribute max alarm = " << p.max_alarm << endl;
	o_str << "Attribute min value = " << p.min_value << endl;
	o_str << "Attribute max value = " << p.max_value << endl;
	o_str << "Attribute writable_attr_name = " << p.writable_attr_name;

	return o_str;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	=
//
// description : 	Assignement operator for the AttributeInfoEx class
//					from a AttributeConfig_2 pointer
//
//--------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(AttributeConfig_2 *att_2)
{
	name = att_2->name;
	writable = att_2->writable;
	data_format = att_2->data_format;
	data_type = att_2->data_type;
	max_dim_x = att_2->max_dim_x;
	max_dim_y = att_2->max_dim_y;
	description = att_2->description;
	label = att_2->label;
	unit = att_2->unit;
	standard_unit = att_2->standard_unit;
	display_unit = att_2->display_unit;
	format = att_2->format;
	min_value = att_2->min_value;
	max_value = att_2->max_value;
	min_alarm = att_2->min_alarm;
	max_alarm = att_2->max_alarm;
	writable_attr_name = att_2->writable_attr_name;
	extensions.resize(att_2->extensions.length());
	for (unsigned int j=0; j<att_2->extensions.length(); j++)
	{
		extensions[j] = att_2->extensions[j];
	}
	disp_level = att_2->level;

	return *this;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	=
//
// description : 	Assignement operator for the AttributeInfoEx class
//					from a AttributeConfig_3 pointer
//
//--------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(AttributeConfig_3 *att_3)
{
	name = att_3->name;
	writable = att_3->writable;
	data_format = att_3->data_format;
	data_type = att_3->data_type;
	max_dim_x = att_3->max_dim_x;
	max_dim_y = att_3->max_dim_y;
	description = att_3->description;
	label = att_3->label;
	unit = att_3->unit;
	standard_unit = att_3->standard_unit;
	display_unit = att_3->display_unit;
	format = att_3->format;
	min_value = att_3->min_value;
	max_value = att_3->max_value;
	min_alarm = att_3->att_alarm.min_alarm;
	max_alarm = att_3->att_alarm.max_alarm;
	writable_attr_name = att_3->writable_attr_name;
	extensions.resize(att_3->sys_extensions.length());
	for (unsigned int j=0; j<att_3->sys_extensions.length(); j++)
	{
		extensions[j] = att_3->sys_extensions[j];
	}
	disp_level = att_3->level;

	alarms.min_alarm = att_3->att_alarm.min_alarm;
	alarms.max_alarm = att_3->att_alarm.max_alarm;
	alarms.min_warning = att_3->att_alarm.min_warning;
	alarms.max_warning = att_3->att_alarm.max_warning;
	alarms.delta_t = att_3->att_alarm.delta_t;
	alarms.delta_val = att_3->att_alarm.delta_val;
	alarms.extensions.resize(att_3->att_alarm.extensions.length());
	for (unsigned int j=0; j<att_3->att_alarm.extensions.length(); j++)
	{
		alarms.extensions[j] = att_3->att_alarm.extensions[j];
	}

	events.ch_event.abs_change = att_3->event_prop.ch_event.abs_change;
	events.ch_event.rel_change = att_3->event_prop.ch_event.rel_change;
	events.ch_event.extensions.resize(att_3->event_prop.ch_event.extensions.length());
	for (unsigned int j=0; j<att_3->event_prop.ch_event.extensions.length(); j++)
	{
		events.ch_event.extensions[j] = att_3->event_prop.ch_event.extensions[j];
	}

	events.per_event.period = att_3->event_prop.per_event.period;
	events.per_event.extensions.resize(att_3->event_prop.per_event.extensions.length());
	for (unsigned int j=0; j<att_3->event_prop.per_event.extensions.length(); j++)
	{
		events.per_event.extensions[j] = att_3->event_prop.per_event.extensions[j];
	}

	events.arch_event.archive_abs_change = att_3->event_prop.arch_event.abs_change;
	events.arch_event.archive_rel_change = att_3->event_prop.arch_event.rel_change;
	events.arch_event.archive_period = att_3->event_prop.arch_event.period;
	events.arch_event.extensions.resize(att_3->event_prop.arch_event.extensions.length());
	for (unsigned int j=0; j<att_3->event_prop.arch_event.extensions.length(); j++)
	{
		events.arch_event.extensions[j] = att_3->event_prop.arch_event.extensions[j];
	}

	return *this;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing instance of the
//			AttributeInfo class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,AttributeInfoEx &p)
{


//
// Print all these properties
//

	o_str << "Attribute name = " << p.name << endl;
	o_str << "Attribute data_type = ";
	switch (p.data_type)
	{
	case Tango::DEV_SHORT :
		o_str << "Tango::DevShort" << endl;
		break;

	case Tango::DEV_LONG :
		o_str << "Tango::DevLong" << endl;
		break;

	case Tango::DEV_DOUBLE :
		o_str << "Tango::DevDouble" << endl;
		break;

	case Tango::DEV_STRING :
		o_str << "Tango::DevString" << endl;
		break;

	case Tango::DEV_FLOAT :
		o_str << "Tango::DevFloat" << endl;
		break;

	case Tango::DEV_USHORT :
		o_str << "Tango::DevUShort" << endl;
		break;

	case Tango::DEV_UCHAR :
		o_str << "Tango::DevUChar" << endl;
		break;

	case Tango::DEV_BOOLEAN :
		o_str << "Tango::DevBoolean" << endl;
		break;

	case Tango::DEV_STATE :
		o_str << "Tango::DevState" << endl;
		break;
	}

	o_str << "Attribute data_format = ";
	switch (p.data_format)
	{
	case Tango::FMT_UNKNOWN:
		break;

	case Tango::SCALAR :
		o_str << "scalar" << endl;
		break;

	case Tango::SPECTRUM :
		o_str << "spectrum, max_dim_x = " << p.max_dim_x << endl;
		break;

	case Tango::IMAGE :
		o_str << "image, max_dim_x = " << p.max_dim_x << ", max_dim_y = " << p.max_dim_y << endl;
		break;
	}

	if ((p.writable == Tango::WRITE) || (p.writable == Tango::READ_WRITE))
		o_str << "Attribute is writable" << endl;
	else
		o_str << "Attribute is not writable" << endl;
	o_str << "Attribute label = " << p.label << endl;
	o_str << "Attribute description = " << p.description << endl;
	o_str << "Attribute unit = " << p.unit;
	o_str << ", standard unit = " << p.standard_unit;
	o_str << ", display unit = " << p.display_unit << endl;
	o_str << "Attribute format = " << p.format << endl;
	o_str << "Attribute min value = " << p.min_value << endl;
	o_str << "Attribute max value = " << p.max_value << endl;
	o_str << "Attribute writable_attr_name = " << p.writable_attr_name << endl;

	unsigned int i;
	for (i = 0;i < p.extensions.size();i++)
		o_str << "Attribute extensions " << i + 1 << " = " << p.extensions[i] << endl;

	o_str << "Attribute alarm : min alarm = ";
	p.alarms.min_alarm.empty() == true ? o_str << "Not specified" : o_str << p.alarms.min_alarm;
	o_str << endl;

	o_str << "Attribute alarm : max alarm = ";
	p.alarms.max_alarm.empty() == true ? o_str << "Not specified" : o_str << p.alarms.max_alarm;
	o_str << endl;

	o_str << "Attribute warning alarm : min warning = ";
	p.alarms.min_warning.empty() == true ? o_str << "Not specified" : o_str << p.alarms.min_warning;
	o_str << endl;

	o_str << "Attribute warning alarm : max warning = ";
	p.alarms.max_warning.empty() == true ? o_str << "Not specified" : o_str << p.alarms.max_warning;
	o_str << endl;

	o_str << "Attribute rds alarm : delta time = ";
	p.alarms.delta_t.empty() == true ? o_str << "Not specified" : o_str << p.alarms.delta_t;
	o_str << endl;

	o_str << "Attribute rds alarm : delta value = ";
	p.alarms.delta_val.empty() == true ? o_str << "Not specified" : o_str << p.alarms.delta_val;
	o_str << endl;

	for (i = 0;i < p.alarms.extensions.size();i++)
		o_str << "Attribute alarm extensions " << i + 1 << " = " << p.alarms.extensions[i] << endl;

	o_str << "Attribute event : change event absolute change = ";
	p.events.ch_event.abs_change.empty() == true ? o_str << "Not specified" : o_str << p.events.ch_event.abs_change;
	o_str << endl;

	o_str << "Attribute event : change event relative change = ";
	p.events.ch_event.rel_change.empty() == true ? o_str << "Not specified" : o_str << p.events.ch_event.rel_change;
	o_str << endl;

	for (i = 0;i < p.events.ch_event.extensions.size();i++)
		o_str << "Attribute alarm : change event extensions " << i + 1 << " = " << p.events.ch_event.extensions[i] << endl;

	o_str << "Attribute event : periodic event period = ";
	p.events.per_event.period.empty() == true ? o_str << "Not specified" : o_str << p.events.per_event.period;
	o_str << endl;

	for (i = 0;i < p.events.per_event.extensions.size();i++)
		o_str << "Attribute alarm : periodic event extensions " << i + 1 << " = " << p.events.per_event.extensions[i] << endl;

	o_str << "Attribute event : archive event absolute change = ";
	p.events.arch_event.archive_abs_change.empty() == true ? o_str << "Not specified" : o_str << p.events.arch_event.archive_abs_change;
	o_str << endl;

	o_str << "Attribute event : archive event relative change = ";
	p.events.arch_event.archive_rel_change.empty() == true ? o_str << "Not specified" : o_str << p.events.arch_event.archive_rel_change;
	o_str << endl;

	o_str << "Attribute event : archive event period = ";
	p.events.arch_event.archive_period.empty() == true ? o_str << "Not specified" : o_str << p.events.arch_event.archive_period;
	o_str << endl;

	for (i = 0;i < p.events.arch_event.extensions.size();i++)
	{
		if (i == 0)
			o_str << endl;
		o_str << "Attribute alarm : archive event extensions " << i + 1 << " = " << p.events.arch_event.extensions[i];
		if (i != p.events.arch_event.extensions.size() - 1)
			o_str << endl;
	}

	return o_str;
}

} // End of tango namespace

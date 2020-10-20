//+==================================================================================================================
//
// C++ source code file for TANGO api class ApiUtil
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// Copyright (C) :      2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Lesser Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// original 	- May 2002
//
// $Revision: 30271 $
//
//+==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventconsumer.h>
#include <api_util.tpp>
#include <thread>

#ifndef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <signal.h>
#include <ifaddrs.h>
#include <netinet/in.h>    // FreeBSD
#else
#include <ws2tcpip.h>
#include <process.h>
#endif


namespace Tango
{

ApiUtil *ApiUtil::_instance = NULL;

omni_mutex ApiUtil::inst_mutex;

void _killproc_()
{
    ::exit(-1);
}

void _t_handler(TANGO_UNUSED(int signum))
{
    std::thread t(_killproc_);
    t.detach();
    Tango_sleep(3);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::ApiUtil()
//
// description :
//		Constructor of the ApiUtil class.
//
//------------------------------------------------------------------------------------------------------------------

ApiUtil::ApiUtil()
    : exit_lock_installed(false), reset_already_executed_flag(false), ext(new ApiUtilExt),
      notifd_event_consumer(NULL), cl_pid(0), user_connect_timeout(-1), zmq_event_consumer(NULL), user_sub_hwm(-1)
{
    _orb = CORBA::ORB::_nil();

//
// Check if it is created from a device server
//

    if (Util::_constructed == true)
    {
        in_serv = true;
    }
    else
    {
        in_serv = false;
    }

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
    cl_pid = _getpid();
#else
    cl_pid = getpid();
#endif

//
// Check if the user has defined his own connection timeout
//

    std::string var;
    if (get_env_var("TANGOconnectTimeout", var) == 0)
    {
        int user_to = -1;
        std::istringstream iss(var);
        iss >> user_to;
        if (iss)
        {
            user_connect_timeout = user_to;
        }
    }

//
// Check if the user has defined his own subscriber hwm (fpr zmq event tuning)
//

    var.clear();
    if (get_env_var("TANGO_EVENT_BUFFER_HWM", var) == 0)
    {
        int sub_hwm = -1;
        std::istringstream iss(var);
        iss >> sub_hwm;
        if (iss)
        {
            user_sub_hwm = sub_hwm;
        }
    }
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::~ApiUtil()
//
// description :
//		Destructor of the ApiUtil class.
//
//------------------------------------------------------------------------------------------------------------------

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

    if (ext.get() != NULL)
    {
        if ((notifd_event_consumer != NULL) || (zmq_event_consumer != NULL))
        {
            event_was_used = true;
            leavefunc();
            NotifdEventConsumer::cleanup();
            ZmqEventConsumer::cleanup();
        }
    }

//
// Delete the database object
//

    for (unsigned int i = 0; i < db_vect.size(); i++)
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
            catch (...)
            {}
        }
        CORBA::release(_orb);
    }

}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::set_sig_handler()
//
// description :
//		Install a signal handler for SIGINT and SIGTERM but only if nothing is already installed
//
//-------------------------------------------------------------------------------------------------------------------

void ApiUtil::set_sig_handler()
{
#ifndef _TG_WINDOWS_
    if (in_serv == false)
    {
        struct sigaction sa, old_action;

        sa.sa_handler = _t_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;

        if (sigaction(SIGTERM, NULL, &old_action) != -1)
        {
            if (old_action.sa_handler == NULL)
            {
                sigaction(SIGTERM, &sa, NULL);
            }
        }

        if (sigaction(SIGINT, NULL, &old_action) != -1)
        {
            if (old_action.sa_handler == NULL)
            {
                sigaction(SIGINT, &sa, NULL);
            }
        }
    }
#endif
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::create_orb()
//
// description :
//		Create the CORBA orb object
//
//-------------------------------------------------------------------------------------------------------------------

void ApiUtil::create_orb()
{
    int _argc;
    char **_argv;

//
// pass dummy arguments to init() because we don't have access to argc and argv
//

    _argc = 1;
    _argv = (char **) malloc(sizeof(char *));
    _argv[0] = (char *) "dummy";

//
// Get user signal handler for SIGPIPE (ORB_init call install a SIG_IGN for SIGPIPE. This could be annoying in case
// the user uses SIGPIPE)
//

#ifndef _TG_WINDOWS_
    struct sigaction sa;
    sa.sa_handler = NULL;

    if (sigaction(SIGPIPE, NULL, &sa) == -1)
    {
        sa.sa_handler = NULL;
    }
#endif

//
// Init the ORB
// Starting with omniORB 4.2, we need to add the throwTransientOnTimeout option for compatibility
//

    bool omni_42_compat = false;
    CORBA::ULong omni_vers_hex = omniORB::versionHex();
    if (omni_vers_hex > 0x04020000)
    {
        omni_42_compat = true;
    }

    const char *options[][2] = {
        {"clientCallTimeOutPeriod", CLNT_TIMEOUT_STR},
        {"verifyObjectExistsAndType", "0"},
        {"maxGIOPConnectionPerServer", MAX_GIOP_PER_SERVER},
        {"giopMaxMsgSize", MAX_TRANSFER_SIZE},
        {"throwTransientOnTimeOut", "1"},
        {0, 0}};

    if (omni_42_compat == false)
    {
        int nb_opt = sizeof(options) / sizeof(char *[2]);
        options[nb_opt - 2][0] = NULL;
        options[nb_opt - 2][1] = NULL;
    }

    _orb = CORBA::ORB_init(_argc, _argv, "omniORB4", options);

    free(_argv);

//
// Restore SIGPIPE handler
//

#ifndef _TG_WINDOWS_
    if (sa.sa_handler != NULL)
    {
        struct sigaction sb;

        sb = sa;

        if (sigaction(SIGPIPE, &sb, NULL) == -1)
        {
            std::cerr << "Can re-install user signal handler for SIGPIPE!" << std::endl;
        }
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
//
// method :
// 		ApiUtil::get_db_ind()
//
// description :
//		Retrieve a Tango database object created from the TANGO_HOST environment variable
//
//--------------------------------------------------------------------------------------------------------------------

int ApiUtil::get_db_ind()
{
    omni_mutex_lock lo(the_mutex);

    for (unsigned int i = 0; i < db_vect.size(); i++)
    {
        if (db_vect[i]->get_from_env_var() == true)
        {
            return i;
        }
    }

//
// The database object has not been found, create it
//

    db_vect.push_back(new Database());

    return (db_vect.size() - 1);

}

int ApiUtil::get_db_ind(std::string &host, int port)
{
    omni_mutex_lock lo(the_mutex);

    for (unsigned int i = 0; i < db_vect.size(); i++)
    {
        if (db_vect[i]->get_db_port_num() == port)
        {
            if (db_vect[i]->get_db_host() == host)
            {
                return i;
            }
        }
    }

//
// The database object has not been found, create it
//

    db_vect.push_back(new Database(host, port));

    return (db_vect.size() - 1);

}


//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::get_asynch_replies()
//
// description :
//		Try to obtain data returned by a command asynchronously requested. This method does not block if the reply is
//		not yet arrived. Fire callback for replies already arrived
//
//------------------------------------------------------------------------------------------------------------------

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
                    tg_req.dev->Cb_Cmd_Request(req, tg_req.cb_ptr);
                    break;

                case TgRequest::READ_ATTR :
                    tg_req.dev->Cb_ReadAttr_Request(req, tg_req.cb_ptr);
                    break;

                case TgRequest::WRITE_ATTR :
                case TgRequest::WRITE_ATTR_SINGLE :
                    tg_req.dev->Cb_WriteAttr_Request(req, tg_req.cb_ptr);
                    break;
            }

            tg_req.dev->dec_asynch_counter(CALL_BACK);
            asyn_p_table->remove_request(tg_req.dev, req);
        }
    }
    catch (CORBA::BAD_INV_ORDER &e)
    {
        if ((e.minor)() != omni::BAD_INV_ORDER_RequestNotSentYet)
        {
            throw;
        }
    }

//
// For all replies already there
//

    std::multimap<Connection *, TgRequest>::iterator pos;
    std::multimap<Connection *, TgRequest> &req_table = asyn_p_table->get_cb_dev_table();

    for (pos = req_table.begin(); pos != req_table.end(); ++pos)
    {
        if (pos->second.arrived == true)
        {
            switch (pos->second.req_type)
            {
                case TgRequest::CMD_INOUT :
                    pos->first->Cb_Cmd_Request(pos->second.request, pos->second.cb_ptr);
                    break;

                case TgRequest::READ_ATTR :
                    pos->first->Cb_ReadAttr_Request(pos->second.request, pos->second.cb_ptr);
                    break;

                case TgRequest::WRITE_ATTR :
                case TgRequest::WRITE_ATTR_SINGLE :
                    pos->first->Cb_WriteAttr_Request(pos->second.request, pos->second.cb_ptr);
                    break;
            }
            pos->first->dec_asynch_counter(CALL_BACK);
            asyn_p_table->remove_request(pos->first, pos->second.request);
        }

    }

}


//------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::get_asynch_replies()
//
// description :
//		Try to obtain data returned by a command asynchronously requested. This method does not block if the reply is
//		not yet arrived. Fire callback for replies already arrived
//
// arg(s) :
//		in :
//			- call_timeout : The timeout value in mS
//
//-----------------------------------------------------------------------------

void ApiUtil::get_asynch_replies(long call_timeout)
{
//
// For all replies already there
//

    std::multimap<Connection *, TgRequest>::iterator pos;
    std::multimap<Connection *, TgRequest> &req_table = asyn_p_table->get_cb_dev_table();

    for (pos = req_table.begin(); pos != req_table.end(); ++pos)
    {
        if (pos->second.arrived == true)
        {
            switch (pos->second.req_type)
            {
                case TgRequest::CMD_INOUT :
                    pos->first->Cb_Cmd_Request(pos->second.request, pos->second.cb_ptr);
                    break;

                case TgRequest::READ_ATTR :
                    pos->first->Cb_ReadAttr_Request(pos->second.request, pos->second.cb_ptr);
                    break;

                case TgRequest::WRITE_ATTR :
                case TgRequest::WRITE_ATTR_SINGLE :
                    pos->first->Cb_WriteAttr_Request(pos->second.request, pos->second.cb_ptr);
                    break;
            }
            pos->first->dec_asynch_counter(CALL_BACK);
            asyn_p_table->remove_request(pos->first, pos->second.request);
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
// A timeout has been specified. Wait if there are still request without replies but not more than the specified
// timeout. Leave method if the timeout is not arrived but there is no more request without reply
//

            long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
            struct timespec to_wait, inter;
            to_wait.tv_sec = 0;
            to_wait.tv_nsec = 20000000;
#endif

            while ((nb > 0) && (asyn_p_table->get_cb_request_nb() != 0))
            {
#ifdef _TG_WINDOWS_
                Sleep(20);
#else
                nanosleep(&to_wait, &inter);
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
                                tg_req.dev->Cb_Cmd_Request(req, tg_req.cb_ptr);
                                break;

                            case TgRequest::READ_ATTR :
                                tg_req.dev->Cb_ReadAttr_Request(req, tg_req.cb_ptr);
                                break;

                            case TgRequest::WRITE_ATTR :
                            case TgRequest::WRITE_ATTR_SINGLE :
                                tg_req.dev->Cb_WriteAttr_Request(req, tg_req.cb_ptr);
                                break;
                        }

                        tg_req.dev->dec_asynch_counter(CALL_BACK);
                        asyn_p_table->remove_request(tg_req.dev, req);
                    }
                }
                catch (CORBA::BAD_INV_ORDER &e)
                {
                    if ((e.minor)() != omni::BAD_INV_ORDER_RequestNotSentYet)
                    {
                        throw;
                    }
                }
            }

//
// Throw exception if the timeout has expired but there are still request without replies
//

            if ((nb == 0) && (asyn_p_table->get_cb_request_nb() != 0))
            {
                TangoSys_OMemStream desc;
                desc << "Still some reply(ies) for asynchronous callback call(s) to be received" << std::ends;
                ApiAsynNotThereExcept::throw_exception((const char *) API_AsynReplyNotArrived,
                                                       desc.str(),
                                                       (const char *) "ApiUtil::get_asynch_replies");
            }
        }
        else
        {
//
// If timeout is set to 0, this means wait until all the requests sent to this device has sent their replies
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
                            tg_req.dev->Cb_Cmd_Request(req, tg_req.cb_ptr);
                            break;

                        case TgRequest::READ_ATTR :
                            tg_req.dev->Cb_ReadAttr_Request(req, tg_req.cb_ptr);
                            break;

                        case TgRequest::WRITE_ATTR :
                        case TgRequest::WRITE_ATTR_SINGLE :
                            tg_req.dev->Cb_WriteAttr_Request(req, tg_req.cb_ptr);
                            break;
                    }

                    tg_req.dev->dec_asynch_counter(CALL_BACK);
                    asyn_p_table->remove_request(tg_req.dev, req);
                }
                catch (CORBA::BAD_INV_ORDER &e)
                {
                    if ((e.minor)() != omni::BAD_INV_ORDER_RequestNotSentYet)
                    {
                        throw;
                    }
                }
            }
        }
    }

}

//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::set_asynch_cb_sub_model()
//
// description :
//		Set the callback automatic mode (Fired by dedicated call or automatically fired by a separate thread)
//
// arg(s) :
//		in :
//			- mode : The new callback mode
//
//--------------------------------------------------------------------------------------------------------------------

void ApiUtil::set_asynch_cb_sub_model(cb_sub_model mode)
{

    if (auto_cb == PULL_CALLBACK)
    {
        if (mode == PUSH_CALLBACK)
        {

//
// In this case, delete the old object in case it is needed, create a new thread and start it
//

            delete cb_thread_ptr;
            cb_thread_ptr = NULL;

            cb_thread_cmd.start_thread();

            cb_thread_ptr = new CallBackThread(cb_thread_cmd, asyn_p_table);
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

//-----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::create_xxx_event_consumer()
//
// description :
//		Create the event consumer. This will automatically start a new thread which is waiting in a CORBA::run()
//		indefintely for events. It will then trigger the events.
//
//----------------------------------------------------------------------------------------------------------------

void ApiUtil::create_notifd_event_consumer()
{
    notifd_event_consumer = NotifdEventConsumer::create();
}

void ApiUtil::create_zmq_event_consumer()
{
    zmq_event_consumer = ZmqEventConsumer::create();
}

NotifdEventConsumer *ApiUtil::get_notifd_event_consumer()
{
    return notifd_event_consumer;
}

ZmqEventConsumer *ApiUtil::get_zmq_event_consumer()
{
    return zmq_event_consumer;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::clean_locking_threads()
//
// description :
//		Ask all remaining locking threads to exit
//
// args :
//		in :
//			- clean :
//
//------------------------------------------------------------------------------------------------------------------

void ApiUtil::clean_locking_threads(bool clean)
{
    omni_mutex_lock oml(lock_th_map);

    if (lock_threads.empty() == false)
    {
        std::map<std::string, LockingThread>::iterator pos;
        for (pos = lock_threads.begin(); pos != lock_threads.end(); ++pos)
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
                    {
                        pos->second.shared->cmd_code = LOCK_UNLOCK_ALL_EXIT;
                    }
                    else
                    {
                        pos->second.shared->cmd_code = LOCK_EXIT;
                    }

                    pos->second.mon->signal();

                    cout4 << "Cmd sent to locking thread" << std::endl;

                    if (pos->second.shared->cmd_pending == true)
                    {
                        pos->second.mon->wait(DEFAULT_TIMEOUT);
                    }
                }
                delete pos->second.shared;
                delete pos->second.mon;
            }
        }

        if (clean == false)
        {
            lock_threads.clear();
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::attr_to_device()
//
// description :
//
//
//-------------------------------------------------------------------------------------------------------------------

void ApiUtil::attr_to_device(const AttributeValue *attr_value, const AttributeValue_3 *attr_value_3,
                             long vers, DeviceAttribute *dev_attr)
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

    CORBA::ULong max, len;

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
        {
            ty = attr_value_3->value.type();
        }
        else
        {
            ty = attr_value->value.type();
        }

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
                {
                    attr_value_3->value >>= tmp_seq_lo;
                }
                else
                {
                    attr_value->value >>= tmp_seq_lo;
                }
                max = tmp_seq_lo->maximum();
                len = tmp_seq_lo->length();
                if (tmp_seq_lo->release() == true)
                {
                    tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean) true);
                    dev_attr->LongSeq = new DevVarLongArray(max, len, tmp_lo, true);
                }
                else
                {
                    tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
                    dev_attr->LongSeq = new DevVarLongArray(max, len, tmp_lo, false);
                }
                break;

            case CORBA::tk_longlong:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_64;
                }
                else
                {
                    attr_value->value >>= tmp_seq_64;
                }
                max = tmp_seq_64->maximum();
                len = tmp_seq_64->length();
                if (tmp_seq_64->release() == true)
                {
                    tmp_lolo = (const_cast<DevVarLong64Array *>(tmp_seq_64))->get_buffer((CORBA::Boolean) true);
                    dev_attr->Long64Seq = new DevVarLong64Array(max, len, tmp_lolo, true);
                }
                else
                {
                    tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
                    dev_attr->Long64Seq = new DevVarLong64Array(max, len, tmp_lolo, false);
                }
                break;

            case CORBA::tk_short:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_sh;
                }
                else
                {
                    attr_value->value >>= tmp_seq_sh;
                }
                max = tmp_seq_sh->maximum();
                len = tmp_seq_sh->length();
                if (tmp_seq_sh->release() == true)
                {
                    tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean) true);
                    dev_attr->ShortSeq = new DevVarShortArray(max, len, tmp_sh, true);
                }
                else
                {
                    tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
                    dev_attr->ShortSeq = new DevVarShortArray(max, len, tmp_sh, false);
                }
                break;

            case CORBA::tk_double:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_db;
                }
                else
                {
                    attr_value->value >>= tmp_seq_db;
                }
                max = tmp_seq_db->maximum();
                len = tmp_seq_db->length();
                if (tmp_seq_db->release() == true)
                {
                    tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean) true);
                    dev_attr->DoubleSeq = new DevVarDoubleArray(max, len, tmp_db, true);
                }
                else
                {
                    tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
                    dev_attr->DoubleSeq = new DevVarDoubleArray(max, len, tmp_db, false);
                }
                break;

            case CORBA::tk_string:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_str;
                }
                else
                {
                    attr_value->value >>= tmp_seq_str;
                }
                max = tmp_seq_str->maximum();
                len = tmp_seq_str->length();
                if (tmp_seq_str->release() == true)
                {
                    tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean) true);
                    dev_attr->StringSeq = new DevVarStringArray(max, len, tmp_str, true);
                }
                else
                {
                    tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
                    dev_attr->StringSeq = new DevVarStringArray(max, len, tmp_str, false);
                }
                break;

            case CORBA::tk_float:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_fl;
                }
                else
                {
                    attr_value->value >>= tmp_seq_fl;
                }
                max = tmp_seq_fl->maximum();
                len = tmp_seq_fl->length();
                if (tmp_seq_fl->release() == true)
                {
                    tmp_fl = (const_cast<DevVarFloatArray *>(tmp_seq_fl))->get_buffer((CORBA::Boolean) true);
                    dev_attr->FloatSeq = new DevVarFloatArray(max, len, tmp_fl, true);
                }
                else
                {
                    tmp_fl = const_cast<CORBA::Float *>(tmp_seq_fl->get_buffer());
                    dev_attr->FloatSeq = new DevVarFloatArray(max, len, tmp_fl, false);
                }
                break;

            case CORBA::tk_boolean:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_boo;
                }
                else
                {
                    attr_value->value >>= tmp_seq_boo;
                }
                max = tmp_seq_boo->maximum();
                len = tmp_seq_boo->length();
                if (tmp_seq_boo->release() == true)
                {
                    tmp_boo = (const_cast<DevVarBooleanArray *>(tmp_seq_boo))->get_buffer((CORBA::Boolean) true);
                    dev_attr->BooleanSeq = new DevVarBooleanArray(max, len, tmp_boo, true);
                }
                else
                {
                    tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq_boo->get_buffer());
                    dev_attr->BooleanSeq = new DevVarBooleanArray(max, len, tmp_boo, false);
                }
                break;

            case CORBA::tk_ushort:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_ush;
                }
                else
                {
                    attr_value->value >>= tmp_seq_ush;
                }
                max = tmp_seq_ush->maximum();
                len = tmp_seq_ush->length();
                if (tmp_seq_ush->release() == true)
                {
                    tmp_ush = (const_cast<DevVarUShortArray *>(tmp_seq_ush))->get_buffer((CORBA::Boolean) true);
                    dev_attr->UShortSeq = new DevVarUShortArray(max, len, tmp_ush, true);
                }
                else
                {
                    tmp_ush = const_cast<CORBA::UShort *>(tmp_seq_ush->get_buffer());
                    dev_attr->UShortSeq = new DevVarUShortArray(max, len, tmp_ush, false);
                }
                break;

            case CORBA::tk_octet:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_uch;
                }
                else
                {
                    attr_value->value >>= tmp_seq_uch;
                }
                max = tmp_seq_uch->maximum();
                len = tmp_seq_uch->length();
                if (tmp_seq_uch->release() == true)
                {
                    tmp_uch = (const_cast<DevVarCharArray *>(tmp_seq_uch))->get_buffer((CORBA::Boolean) true);
                    dev_attr->UCharSeq = new DevVarCharArray(max, len, tmp_uch, true);
                }
                else
                {
                    tmp_uch = const_cast<CORBA::Octet *>(tmp_seq_uch->get_buffer());
                    dev_attr->UCharSeq = new DevVarCharArray(max, len, tmp_uch, false);
                }
                break;

            case CORBA::tk_ulong:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_ulo;
                }
                else
                {
                    attr_value->value >>= tmp_seq_ulo;
                }
                max = tmp_seq_ulo->maximum();
                len = tmp_seq_ulo->length();
                if (tmp_seq_ulo->release() == true)
                {
                    tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean) true);
                    dev_attr->ULongSeq = new DevVarULongArray(max, len, tmp_ulo, true);
                }
                else
                {
                    tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq_ulo->get_buffer());
                    dev_attr->ULongSeq = new DevVarULongArray(max, len, tmp_ulo, false);
                }
                break;

            case CORBA::tk_ulonglong:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_u64;
                }
                else
                {
                    attr_value->value >>= tmp_seq_u64;
                }
                max = tmp_seq_u64->maximum();
                len = tmp_seq_u64->length();
                if (tmp_seq_u64->release() == true)
                {
                    tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_u64))->get_buffer((CORBA::Boolean) true);
                    dev_attr->ULong64Seq = new DevVarULong64Array(max, len, tmp_ulolo, true);
                }
                else
                {
                    tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq_u64->get_buffer());
                    dev_attr->ULong64Seq = new DevVarULong64Array(max, len, tmp_ulolo, false);
                }
                break;

            case CORBA::tk_enum:
                if (vers == 3)
                {
                    attr_value_3->value >>= tmp_seq_state;
                }
                else
                {
                    attr_value->value >>= tmp_seq_state;
                }
                max = tmp_seq_state->maximum();
                len = tmp_seq_state->length();
                if (tmp_seq_state->release() == true)
                {
                    tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean) true);
                    dev_attr->StateSeq = new DevVarStateArray(max, len, tmp_state, true);
                }
                else
                {
                    tmp_state = const_cast<Tango::DevState *>(tmp_seq_state->get_buffer());
                    dev_attr->StateSeq = new DevVarStateArray(max, len, tmp_state, false);
                }
                break;

            default:
                break;
        }
        dev_attr->data_type = Tango::DEV_SHORT;
    }
}

void ApiUtil::attr_to_device(const AttributeValue_4 *attr_value_4, TANGO_UNUSED(long vers), DeviceAttribute *dev_attr)
{
    attr_to_device_base(attr_value_4, dev_attr);

//
// Warning: Since Tango 9, data type SHORT is used for both short attribute and enumeration attribute!
// Therefore, we need to store somewhere which exact type it is. With IDL 5, it is easy, because the data type is
// transferred on the network (modified IDL), For previous release, we do not have enumerated data type and therefore
// the data type could be used only for SHORT.
//
    dev_attr->data_type = Tango::DEV_SHORT;
}

void ApiUtil::attr_to_device(const AttributeValue_5 *attr_value_5, TANGO_UNUSED(long vers), DeviceAttribute *dev_attr)
{
    attr_to_device_base(attr_value_5, dev_attr);
    dev_attr->data_type = attr_value_5->data_type;
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::device_to_attr()
//
// description :
//		initialize one AttributeValue instance from a DeviceAttribute one
//
// arg(s) :
//		in :
//			- dev_attr : The DeviceAttribute instance taken as source
//		out :
//			- att : The AttributeValue used as destination
//
//---------------------------------------------------------------------------------------------------------------

void ApiUtil::device_to_attr(const DeviceAttribute &dev_attr, AttributeValue_4 &att)
{
    att.name = dev_attr.name.c_str();
    att.quality = dev_attr.quality;
    att.time = dev_attr.time;
    att.w_dim.dim_x = dev_attr.dim_x;
    att.w_dim.dim_y = dev_attr.dim_y;
    att.data_format = Tango::FMT_UNKNOWN;

    if (dev_attr.LongSeq.operator->() != NULL)
    {
        att.value.long_att_value(dev_attr.LongSeq.in());
    }
    else if (dev_attr.ShortSeq.operator->() != NULL)
    {
        att.value.short_att_value(dev_attr.ShortSeq.in());
    }
    else if (dev_attr.DoubleSeq.operator->() != NULL)
    {
        att.value.double_att_value(dev_attr.DoubleSeq.in());
    }
    else if (dev_attr.StringSeq.operator->() != NULL)
    {
        att.value.string_att_value(dev_attr.StringSeq.in());
    }
    else if (dev_attr.FloatSeq.operator->() != NULL)
    {
        att.value.float_att_value(dev_attr.FloatSeq.in());
    }
    else if (dev_attr.BooleanSeq.operator->() != NULL)
    {
        att.value.bool_att_value(dev_attr.BooleanSeq.in());
    }
    else if (dev_attr.UShortSeq.operator->() != NULL)
    {
        att.value.ushort_att_value(dev_attr.UShortSeq.in());
    }
    else if (dev_attr.UCharSeq.operator->() != NULL)
    {
        att.value.uchar_att_value(dev_attr.UCharSeq.in());
    }
    else if (dev_attr.Long64Seq.operator->() != NULL)
    {
        att.value.long64_att_value(dev_attr.Long64Seq.in());
    }
    else if (dev_attr.ULongSeq.operator->() != NULL)
    {
        att.value.ulong_att_value(dev_attr.ULongSeq.in());
    }
    else if (dev_attr.ULong64Seq.operator->() != NULL)
    {
        att.value.ulong64_att_value(dev_attr.ULong64Seq.in());
    }
    else if (dev_attr.StateSeq.operator->() != NULL)
    {
        att.value.state_att_value(dev_attr.StateSeq.in());
    }
    else if (dev_attr.EncodedSeq.operator->() != NULL)
    {
        att.value.encoded_att_value(dev_attr.EncodedSeq.in());
    }
}

void ApiUtil::device_to_attr(const DeviceAttribute &dev_attr, AttributeValue &att, std::string &d_name)
{

    att.name = dev_attr.name.c_str();
    att.quality = dev_attr.quality;
    att.time = dev_attr.time;
    att.dim_x = dev_attr.dim_x;
    att.dim_y = dev_attr.dim_y;

    if (dev_attr.LongSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.LongSeq.in();
    }
    else if (dev_attr.ShortSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.ShortSeq.in();
    }
    else if (dev_attr.DoubleSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.DoubleSeq.in();
    }
    else if (dev_attr.StringSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.StringSeq.in();
    }
    else if (dev_attr.FloatSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.FloatSeq.in();
    }
    else if (dev_attr.BooleanSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.BooleanSeq.in();
    }
    else if (dev_attr.UShortSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.UShortSeq.in();
    }
    else if (dev_attr.UCharSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.UCharSeq.in();
    }
    else if (dev_attr.Long64Seq.operator->() != NULL)
    {
        att.value <<= dev_attr.Long64Seq.in();
    }
    else if (dev_attr.ULongSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.ULongSeq.in();
    }
    else if (dev_attr.ULong64Seq.operator->() != NULL)
    {
        att.value <<= dev_attr.ULong64Seq.in();
    }
    else if (dev_attr.StateSeq.operator->() != NULL)
    {
        att.value <<= dev_attr.StateSeq.in();
    }
    else if (dev_attr.EncodedSeq.operator->() != NULL)
    {
        TangoSys_OMemStream desc;
        desc << "Device " << d_name;
        desc << " does not support DevEncoded data type" << std::ends;
        ApiNonSuppExcept::throw_exception((const char *) API_UnsupportedFeature,
                                          desc.str(),
                                          (const char *) "DeviceProxy::device_to_attr");
    }
}

//------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::AttributeInfoEx_to_AttributeConfig()
//
// description :
//		Initialize one AttributeConfig instance from a AttributeInfoEx one
//
// arg(s) :
//		in :
//			- aie : The AttributeInfoEx instance taken as source
//		out :
//			- att_conf_5 : The AttributeConfig used as destination
//
//------------------------------------------------------------------------------------------------------------------

void ApiUtil::AttributeInfoEx_to_AttributeConfig(const AttributeInfoEx *aie, AttributeConfig_5 *att_conf_5)
{
    att_conf_5->name = aie->name.c_str();
    att_conf_5->writable = aie->writable;
    att_conf_5->data_format = aie->data_format;
    att_conf_5->data_type = aie->data_type;
    att_conf_5->max_dim_x = aie->max_dim_x;
    att_conf_5->max_dim_y = aie->max_dim_y;
    att_conf_5->description = aie->description.c_str();
    att_conf_5->label = aie->label.c_str();
    att_conf_5->unit = aie->unit.c_str();
    att_conf_5->standard_unit = aie->standard_unit.c_str();
    att_conf_5->display_unit = aie->display_unit.c_str();
    att_conf_5->format = aie->format.c_str();
    att_conf_5->min_value = aie->min_value.c_str();
    att_conf_5->max_value = aie->max_value.c_str();
    att_conf_5->writable_attr_name = aie->writable_attr_name.c_str();
    att_conf_5->level = aie->disp_level;
    att_conf_5->root_attr_name = aie->root_attr_name.c_str();
    switch (aie->memorized)
    {
        case NOT_KNOWN:
        case NONE:
            att_conf_5->memorized = false;
            att_conf_5->mem_init = false;
            break;

        case MEMORIZED:
            att_conf_5->memorized = true;
            att_conf_5->mem_init = false;
            break;

        case MEMORIZED_WRITE_INIT:
            att_conf_5->memorized = true;
            att_conf_5->mem_init = true;
            break;

        default:
            break;
    }
    att_conf_5->enum_labels.length(aie->enum_labels.size());
    for (size_t j = 0; j < aie->enum_labels.size(); j++)
    {
        att_conf_5->enum_labels[j] = string_dup(aie->enum_labels[j].c_str());
    }
    att_conf_5->extensions.length(aie->extensions.size());
    for (size_t j = 0; j < aie->extensions.size(); j++)
    {
        att_conf_5->extensions[j] = string_dup(aie->extensions[j].c_str());
    }
    for (size_t j = 0; j < aie->sys_extensions.size(); j++)
    {
        att_conf_5->sys_extensions[j] = string_dup(aie->sys_extensions[j].c_str());
    }

    att_conf_5->att_alarm.min_alarm = aie->alarms.min_alarm.c_str();
    att_conf_5->att_alarm.max_alarm = aie->alarms.max_alarm.c_str();
    att_conf_5->att_alarm.min_warning = aie->alarms.min_warning.c_str();
    att_conf_5->att_alarm.max_warning = aie->alarms.max_warning.c_str();
    att_conf_5->att_alarm.delta_t = aie->alarms.delta_t.c_str();
    att_conf_5->att_alarm.delta_val = aie->alarms.delta_val.c_str();
    for (size_t j = 0; j < aie->alarms.extensions.size(); j++)
    {
        att_conf_5->att_alarm.extensions[j] = string_dup(aie->alarms.extensions[j].c_str());
    }

    att_conf_5->event_prop.ch_event.rel_change = aie->events.ch_event.rel_change.c_str();
    att_conf_5->event_prop.ch_event.abs_change = aie->events.ch_event.abs_change.c_str();
    for (size_t j = 0; j < aie->events.ch_event.extensions.size(); j++)
    {
        att_conf_5->event_prop.ch_event.extensions[j] = string_dup(aie->events.ch_event.extensions[j].c_str());
    }

    att_conf_5->event_prop.per_event.period = aie->events.per_event.period.c_str();
    for (size_t j = 0; j < aie->events.per_event.extensions.size(); j++)
    {
        att_conf_5->event_prop.per_event.extensions[j] = string_dup(aie->events.per_event.extensions[j].c_str());
    }

    att_conf_5->event_prop.arch_event.rel_change = aie->events.arch_event.archive_rel_change.c_str();
    att_conf_5->event_prop.arch_event.abs_change = aie->events.arch_event.archive_abs_change.c_str();
    att_conf_5->event_prop.arch_event.period = aie->events.arch_event.archive_period.c_str();
    for (size_t j = 0; j < aie->events.ch_event.extensions.size(); j++)
    {
        att_conf_5->event_prop.arch_event.extensions[j] = string_dup(aie->events.arch_event.extensions[j].c_str());
    }
}


//-------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::get_env_var()
//
// description :
//		Get environment variable
//
// arg(s) :
//		in :
//			- env_var_name : The environment variable name
//		out :
//			- env_var : Reference to the string initialised with the env. variable value (if found)
//
// return :
//		0 if the env. variable is found and -1 otherwise
//
//----------------------------------------------------------------------------------------------------------------

int ApiUtil::get_env_var(const char *env_var_name, std::string &env_var)
{
    DummyDeviceProxy d;
    return d.get_env_var(env_var_name, env_var);
}

//---------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::get_ip_from_if()
//
// description :
//		Get host IP address from its network interface
//
// arg(s) :
//		out :
//			- ip_adr_list : Host IP address
//
//----------------------------------------------------------------------------------------------------------------

void ApiUtil::get_ip_from_if(std::vector<std::string> &ip_adr_list)
{
    omni_mutex_lock oml(lock_th_map);

    if (host_ip_adrs.empty() == true)
    {
#ifndef _TG_WINDOWS_
        struct ifaddrs *ifaddr, *ifa;
        int family, s;
        char host[NI_MAXHOST];

        if (getifaddrs(&ifaddr) == -1)
        {
            std::cerr << "ApiUtil::get_ip_from_if: getifaddrs() failed: " << strerror(errno) << std::endl;

            Tango::Except::throw_exception((const char *) API_SystemCallFailed,
                                           (const char *) strerror(errno),
                                           (const char *) "ApiUtil::get_ip_from_if()");
        }

//
// Walk through linked list, maintaining head pointer so we can free list later. The ifa_addr field points to a
// structure containing the interface address. (The sa_family subfield should be consulted to determine the format
// of the address structure.)
//

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr != NULL)
            {
                family = ifa->ifa_addr->sa_family;

//
// Only get IP V4 addresses
//

/*              if(family == AF_INET || family == AF_INET6)
                {
                    s = getnameinfo(ifa->ifa_addr,(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);*/

                if (family == AF_INET)
                {
                    s = getnameinfo(ifa->ifa_addr,
                                    sizeof(struct sockaddr_in),
                                    host,
                                    NI_MAXHOST,
                                    NULL,
                                    0,
                                    NI_NUMERICHOST);

                    if (s != 0)
                    {
                        std::cerr << "ApiUtil::get_ip_from_if: getnameinfo() failed: " << gai_strerror(s);
                        std::cerr << "ApiUtil::get_ip_from_if: not getting info from remaining ifaddrs";

                        freeifaddrs(ifaddr);

                        Tango::Except::throw_exception((const char *) API_SystemCallFailed,
                                                       (const char *) gai_strerror(s),
                                                       (const char *) "ApiUtil::get_ip_from_if()");
                    }
                    else
                    {
                        host_ip_adrs.push_back(std::string(host));
                    }
                }
            }
        }

        freeifaddrs(ifaddr);
#else

        //
        // Get address from interfaces
        //

        int sock = socket(AF_INET, SOCK_STREAM, 0);

        INTERFACE_INFO info[64];  // Assume max 64 interfaces
        DWORD retlen;

        if (WSAIoctl(sock,
                     SIO_GET_INTERFACE_LIST,
                     NULL,
                     0,
                     (LPVOID) & info,
                     sizeof(info),
                     (LPDWORD) & retlen,
                     NULL,
                     NULL) == SOCKET_ERROR)
        {
            closesocket(sock);

            int err = WSAGetLastError();
            std::cerr << "Warning: WSAIoctl failed" << std::endl;
            std::cerr << "Unable to obtain the list of all interface addresses. Error = " << err << std::endl;

            TangoSys_OMemStream desc;
            desc << "Can't retrieve list of all interfaces addresses (WSAIoctl)! Error = " << err << std::ends;

            Tango::Except::throw_exception((const char *) API_SystemCallFailed,
                                           (const char *) desc.str().c_str(),
                                           (const char *) "ApiUtil::get_ip_from_if()");
        }
        closesocket(sock);

        //
        // Converts addresses to string. Only for running interfaces
        //

        int numAddresses = retlen / sizeof(INTERFACE_INFO);
        for (int i = 0; i < numAddresses; i++)
        {
            if (info[i].iiFlags & IFF_UP)
            {
                if (info[i].iiAddress.Address.sa_family == AF_INET || info[i].iiAddress.Address.sa_family == AF_INET6)
                {
                    struct sockaddr *addr = (struct sockaddr *) &info[i].iiAddress.AddressIn;
                    char dest[NI_MAXHOST];
                    socklen_t addrlen = sizeof(sockaddr);

                    int result = getnameinfo(addr, addrlen, dest, sizeof(dest), 0, 0, NI_NUMERICHOST);
                    if (result != 0)
                    {
                        std::cerr << "Warning: getnameinfo failed" << std::endl;
                        std::cerr << "Unable to convert IP address to string (getnameinfo)." << std::endl;

                        Tango::Except::throw_exception((const char *) API_SystemCallFailed,
                                                       (const char *) "Can't convert IP address to string (getnameinfo)!",
                                                       (const char *) "ApiUtil::get_ip_from_if()");
                    }
                    std::string tmp_str(dest);
                    if (tmp_str != "0.0.0.0" && tmp_str != "0:0:0:0:0:0:0:0" && tmp_str != "::")
                    {
                        host_ip_adrs.push_back(tmp_str);
                    }
                }
            }
        }
#endif
    }

//
// Copy host IP addresses into caller vector
//

    ip_adr_list.clear();
    copy(host_ip_adrs.begin(), host_ip_adrs.end(), back_inserter(ip_adr_list));
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		ApiUtil::print_error_message
//
// description :
//		Print error message on stderr but first print date
//
// argument :
//		in :
//			- mess : The user error message
//
//---------------------------------------------------------------------------------------------------------------------

void ApiUtil::print_error_message(const char *mess)
{
    time_t tmp_val = time(NULL);

    char tmp_date[128];
#ifdef _TG_WINDOWS_
    ctime_s(tmp_date,128,&tmp_val);
#else
    ctime_r(&tmp_val, tmp_date);
#endif
    tmp_date[strlen(tmp_date) - 1] = '\0';
    std::cerr << tmp_date << ": " << mess << std::endl;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// function
// 		operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the AttributeInfo class
//
//-----------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &o_str, const AttributeInfo &p)
{


//
// Print all these properties
//

    o_str << "Attribute name = " << p.name << std::endl;
    o_str << "Attribute data_type = ";
    switch (p.data_type)
    {
        case Tango::DEV_SHORT :
            o_str << "Tango::DevShort" << std::endl;
            break;

        case Tango::DEV_LONG :
            o_str << "Tango::DevLong" << std::endl;
            break;

        case Tango::DEV_DOUBLE :
            o_str << "Tango::DevDouble" << std::endl;
            break;

        case Tango::DEV_STRING :
            o_str << "Tango::DevString" << std::endl;
            break;

        case Tango::DEV_FLOAT :
            o_str << "Tango::DevFloat" << std::endl;
            break;

        case Tango::DEV_USHORT :
            o_str << "Tango::DevUShort" << std::endl;
            break;

        case Tango::DEV_UCHAR :
            o_str << "Tango::DevUChar" << std::endl;
            break;

        case Tango::DEV_BOOLEAN :
            o_str << "Tango::DevBoolean" << std::endl;
            break;

        case Tango::DEV_STATE :
            o_str << "Tango::DevState" << std::endl;
            break;
    }

    o_str << "Attribute data_format = ";
    switch (p.data_format)
    {
        case Tango::FMT_UNKNOWN:
            break;

        case Tango::SCALAR :
            o_str << "scalar" << std::endl;
            break;

        case Tango::SPECTRUM :
            o_str << "spectrum, max_dim_x = " << p.max_dim_x << std::endl;
            break;

        case Tango::IMAGE :
            o_str << "image, max_dim_x = " << p.max_dim_x << ", max_dim_y = " << p.max_dim_y << std::endl;
            break;
    }

    if ((p.writable == Tango::WRITE) || (p.writable == Tango::READ_WRITE))
    {
        o_str << "Attribute is writable" << std::endl;
    }
    else
    {
        o_str << "Attribute is not writable" << std::endl;
    }
    o_str << "Attribute label = " << p.label << std::endl;
    o_str << "Attribute description = " << p.description << std::endl;
    o_str << "Attribute unit = " << p.unit;
    o_str << ", standard unit = " << p.standard_unit;
    o_str << ", display unit = " << p.display_unit << std::endl;
    o_str << "Attribute format = " << p.format << std::endl;
    o_str << "Attribute min alarm = " << p.min_alarm << std::endl;
    o_str << "Attribute max alarm = " << p.max_alarm << std::endl;
    o_str << "Attribute min value = " << p.min_value << std::endl;
    o_str << "Attribute max value = " << p.max_value << std::endl;
    o_str << "Attribute writable_attr_name = " << p.writable_attr_name;

    return o_str;
}

//+----------------------------------------------------------------------------------------------------------------
//
// function :
// 		operator overloading : 	=
//
// description :
//		Assignement operator for the AttributeInfoEx class from a AttributeConfig_2 pointer
//
//----------------------------------------------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(const AttributeConfig_2 *att_2)
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
    for (unsigned int j = 0; j < att_2->extensions.length(); j++)
    {
        extensions[j] = att_2->extensions[j];
    }
    disp_level = att_2->level;

    return *this;
}

//+---------------------------------------------------------------------------------------------------------------
//
// function :
// 		operator overloading : 	=
//
// description :
//		Assignement operator for the AttributeInfoEx class from a AttributeConfig_3 pointer
//
//-----------------------------------------------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(const AttributeConfig_3 *att_3)
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
    for (unsigned int j = 0; j < att_3->sys_extensions.length(); j++)
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
    for (unsigned int j = 0; j < att_3->att_alarm.extensions.length(); j++)
    {
        alarms.extensions[j] = att_3->att_alarm.extensions[j];
    }

    events.ch_event.abs_change = att_3->event_prop.ch_event.abs_change;
    events.ch_event.rel_change = att_3->event_prop.ch_event.rel_change;
    events.ch_event.extensions.resize(att_3->event_prop.ch_event.extensions.length());
    for (unsigned int j = 0; j < att_3->event_prop.ch_event.extensions.length(); j++)
    {
        events.ch_event.extensions[j] = att_3->event_prop.ch_event.extensions[j];
    }

    events.per_event.period = att_3->event_prop.per_event.period;
    events.per_event.extensions.resize(att_3->event_prop.per_event.extensions.length());
    for (unsigned int j = 0; j < att_3->event_prop.per_event.extensions.length(); j++)
    {
        events.per_event.extensions[j] = att_3->event_prop.per_event.extensions[j];
    }

    events.arch_event.archive_abs_change = att_3->event_prop.arch_event.abs_change;
    events.arch_event.archive_rel_change = att_3->event_prop.arch_event.rel_change;
    events.arch_event.archive_period = att_3->event_prop.arch_event.period;
    events.arch_event.extensions.resize(att_3->event_prop.arch_event.extensions.length());
    for (unsigned int j = 0; j < att_3->event_prop.arch_event.extensions.length(); j++)
    {
        events.arch_event.extensions[j] = att_3->event_prop.arch_event.extensions[j];
    }

    return *this;
}

//+---------------------------------------------------------------------------------------------------------------
//
// function :
// 		operator overloading : 	=
//
// description :
//		Assignement operator for the AttributeInfoEx class from a AttributeConfig_5 pointer
//
//-----------------------------------------------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(const AttributeConfig_5 *att_5)
{
    name = att_5->name;
    writable = att_5->writable;
    data_format = att_5->data_format;
    data_type = att_5->data_type;
    max_dim_x = att_5->max_dim_x;
    max_dim_y = att_5->max_dim_y;
    description = att_5->description;
    label = att_5->label;
    unit = att_5->unit;
    standard_unit = att_5->standard_unit;
    display_unit = att_5->display_unit;
    format = att_5->format;
    min_value = att_5->min_value;
    max_value = att_5->max_value;
    min_alarm = att_5->att_alarm.min_alarm;
    max_alarm = att_5->att_alarm.max_alarm;
    writable_attr_name = att_5->writable_attr_name;
    extensions.resize(att_5->sys_extensions.length());
    for (unsigned int j = 0; j < att_5->sys_extensions.length(); j++)
    {
        extensions[j] = att_5->sys_extensions[j];
    }
    disp_level = att_5->level;
    root_attr_name = att_5->root_attr_name;
    if (att_5->memorized == false)
    {
        memorized = NONE;
    }
    else
    {
        if (att_5->mem_init == false)
        {
            memorized = MEMORIZED;
        }
        else
        {
            memorized = MEMORIZED_WRITE_INIT;
        }
    }
    enum_labels.clear();
    for (unsigned int j = 0; j < att_5->enum_labels.length(); j++)
    {
        enum_labels.push_back(att_5->enum_labels[j].in());
    }

    alarms.min_alarm = att_5->att_alarm.min_alarm;
    alarms.max_alarm = att_5->att_alarm.max_alarm;
    alarms.min_warning = att_5->att_alarm.min_warning;
    alarms.max_warning = att_5->att_alarm.max_warning;
    alarms.delta_t = att_5->att_alarm.delta_t;
    alarms.delta_val = att_5->att_alarm.delta_val;
    alarms.extensions.resize(att_5->att_alarm.extensions.length());
    for (unsigned int j = 0; j < att_5->att_alarm.extensions.length(); j++)
    {
        alarms.extensions[j] = att_5->att_alarm.extensions[j];
    }

    events.ch_event.abs_change = att_5->event_prop.ch_event.abs_change;
    events.ch_event.rel_change = att_5->event_prop.ch_event.rel_change;
    events.ch_event.extensions.resize(att_5->event_prop.ch_event.extensions.length());
    for (unsigned int j = 0; j < att_5->event_prop.ch_event.extensions.length(); j++)
    {
        events.ch_event.extensions[j] = att_5->event_prop.ch_event.extensions[j];
    }

    events.per_event.period = att_5->event_prop.per_event.period;
    events.per_event.extensions.resize(att_5->event_prop.per_event.extensions.length());
    for (unsigned int j = 0; j < att_5->event_prop.per_event.extensions.length(); j++)
    {
        events.per_event.extensions[j] = att_5->event_prop.per_event.extensions[j];
    }

    events.arch_event.archive_abs_change = att_5->event_prop.arch_event.abs_change;
    events.arch_event.archive_rel_change = att_5->event_prop.arch_event.rel_change;
    events.arch_event.archive_period = att_5->event_prop.arch_event.period;
    events.arch_event.extensions.resize(att_5->event_prop.arch_event.extensions.length());
    for (unsigned int j = 0; j < att_5->event_prop.arch_event.extensions.length(); j++)
    {
        events.arch_event.extensions[j] = att_5->event_prop.arch_event.extensions[j];
    }

    return *this;
}

//+----------------------------------------------------------------------------------------------------------------
//
// function :
// 		operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the AttributeInfo class
//
//-----------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &o_str, const AttributeInfoEx &p)
{


//
// Print all these properties
//

    o_str << "Attribute name = " << p.name << std::endl;
    o_str << "Attribute data_type = ";
    switch (p.data_type)
    {
        case Tango::DEV_SHORT :
            o_str << "Tango::DevShort" << std::endl;
            break;

        case Tango::DEV_LONG :
            o_str << "Tango::DevLong" << std::endl;
            break;

        case Tango::DEV_DOUBLE :
            o_str << "Tango::DevDouble" << std::endl;
            break;

        case Tango::DEV_STRING :
            o_str << "Tango::DevString" << std::endl;
            break;

        case Tango::DEV_FLOAT :
            o_str << "Tango::DevFloat" << std::endl;
            break;

        case Tango::DEV_USHORT :
            o_str << "Tango::DevUShort" << std::endl;
            break;

        case Tango::DEV_UCHAR :
            o_str << "Tango::DevUChar" << std::endl;
            break;

        case Tango::DEV_BOOLEAN :
            o_str << "Tango::DevBoolean" << std::endl;
            break;

        case Tango::DEV_STATE :
            o_str << "Tango::DevState" << std::endl;
            break;

        case Tango::DEV_ULONG :
            o_str << "Tango::DevULong" << std::endl;
            break;

        case Tango::DEV_ULONG64 :
            o_str << "Tango::DevULong64" << std::endl;
            break;

        case Tango::DEV_ENCODED :
            o_str << "Tango::DevEncoded" << std::endl;
            break;

        case Tango::DEV_ENUM :
            o_str << "Tango::DevEnum" << std::endl;
            for (size_t loop = 0; loop < p.enum_labels.size(); loop++)
            {
                o_str << "\tEnumeration label = " << p.enum_labels[loop] << std::endl;
            }
            break;

        case Tango::DATA_TYPE_UNKNOWN :
            o_str << "Unknown" << std::endl;
            break;
    }

    o_str << "Attribute data_format = ";
    switch (p.data_format)
    {
        case Tango::FMT_UNKNOWN:
            o_str << " Unknown" << std::endl;
            break;

        case Tango::SCALAR :
            o_str << "scalar" << std::endl;
            break;

        case Tango::SPECTRUM :
            o_str << "spectrum, max_dim_x = " << p.max_dim_x << std::endl;
            break;

        case Tango::IMAGE :
            o_str << "image, max_dim_x = " << p.max_dim_x << ", max_dim_y = " << p.max_dim_y << std::endl;
            break;
    }

    o_str << "Attribute writable type = ";
    switch (p.writable)
    {
        case Tango::WRITE:
            o_str << "Write" << std::endl;
            break;

        case Tango::READ:
            o_str << "Read" << std::endl;
            break;

        case Tango::READ_WRITE:
            o_str << "Read/Write" << std::endl;
            break;

        case Tango::READ_WITH_WRITE:
            o_str << "Read with write" << std::endl;
            break;

        default:
            break;
    }

    if ((p.writable == Tango::WRITE) || (p.writable == Tango::READ_WRITE))
    {
        switch (p.memorized)
        {
            case NOT_KNOWN:
                o_str << "Device/Appli too old to send/receive attribute memorisation data" << std::endl;
                break;

            case NONE:
                o_str << "Attribute is not memorized" << std::endl;
                break;

            case MEMORIZED:
                o_str << "Attribute is memorized" << std::endl;
                break;

            case MEMORIZED_WRITE_INIT:
                o_str << "Attribute is memorized and the memorized value is written at initialisation" << std::endl;
                break;

            default:
                break;
        }
    }

    o_str << "Attribute display level = ";
    switch (p.disp_level)
    {
        case DL_UNKNOWN :
            o_str << "Unknown" << std::endl;
            break;

        case OPERATOR:
            o_str << "Operator" << std::endl;
            break;

        case EXPERT:
            o_str << "Expert" << std::endl;
            break;

        default:
            break;
    }

    o_str << "Attribute writable_attr_name = " << p.writable_attr_name << std::endl;
    if (p.root_attr_name.empty() == false)
    {
        o_str << "Root attribute name = " << p.root_attr_name << std::endl;
    }
    o_str << "Attribute label = " << p.label << std::endl;
    o_str << "Attribute description = " << p.description << std::endl;
    o_str << "Attribute unit = " << p.unit;
    o_str << ", standard unit = " << p.standard_unit;
    o_str << ", display unit = " << p.display_unit << std::endl;
    o_str << "Attribute format = " << p.format << std::endl;
    o_str << "Attribute min value = " << p.min_value << std::endl;
    o_str << "Attribute max value = " << p.max_value << std::endl;

    unsigned int i;
    for (i = 0; i < p.extensions.size(); i++)
    {
        o_str << "Attribute extensions " << i + 1 << " = " << p.extensions[i] << std::endl;
    }

    o_str << "Attribute alarm : min alarm = ";
    p.alarms.min_alarm.empty() == true ? o_str << "Not specified" : o_str << p.alarms.min_alarm;
    o_str << std::endl;

    o_str << "Attribute alarm : max alarm = ";
    p.alarms.max_alarm.empty() == true ? o_str << "Not specified" : o_str << p.alarms.max_alarm;
    o_str << std::endl;

    o_str << "Attribute warning alarm : min warning = ";
    p.alarms.min_warning.empty() == true ? o_str << "Not specified" : o_str << p.alarms.min_warning;
    o_str << std::endl;

    o_str << "Attribute warning alarm : max warning = ";
    p.alarms.max_warning.empty() == true ? o_str << "Not specified" : o_str << p.alarms.max_warning;
    o_str << std::endl;

    o_str << "Attribute rds alarm : delta time = ";
    p.alarms.delta_t.empty() == true ? o_str << "Not specified" : o_str << p.alarms.delta_t;
    o_str << std::endl;

    o_str << "Attribute rds alarm : delta value = ";
    p.alarms.delta_val.empty() == true ? o_str << "Not specified" : o_str << p.alarms.delta_val;
    o_str << std::endl;

    for (i = 0; i < p.alarms.extensions.size(); i++)
    {
        o_str << "Attribute alarm extensions " << i + 1 << " = " << p.alarms.extensions[i] << std::endl;
    }

    o_str << "Attribute event : change event absolute change = ";
    p.events.ch_event.abs_change.empty() == true ? o_str << "Not specified" : o_str << p.events.ch_event.abs_change;
    o_str << std::endl;

    o_str << "Attribute event : change event relative change = ";
    p.events.ch_event.rel_change.empty() == true ? o_str << "Not specified" : o_str << p.events.ch_event.rel_change;
    o_str << std::endl;

    for (i = 0; i < p.events.ch_event.extensions.size(); i++)
    {
        o_str << "Attribute alarm : change event extensions " << i + 1 << " = " << p.events.ch_event.extensions[i]
              << std::endl;
    }

    o_str << "Attribute event : periodic event period = ";
    p.events.per_event.period.empty() == true ? o_str << "Not specified" : o_str << p.events.per_event.period;
    o_str << std::endl;

    for (i = 0; i < p.events.per_event.extensions.size(); i++)
    {
        o_str << "Attribute alarm : periodic event extensions " << i + 1 << " = " << p.events.per_event.extensions[i]
              << std::endl;
    }

    o_str << "Attribute event : archive event absolute change = ";
    p.events.arch_event.archive_abs_change.empty() == true ? o_str << "Not specified" : o_str
        << p.events.arch_event.archive_abs_change;
    o_str << std::endl;

    o_str << "Attribute event : archive event relative change = ";
    p.events.arch_event.archive_rel_change.empty() == true ? o_str << "Not specified" : o_str
        << p.events.arch_event.archive_rel_change;
    o_str << std::endl;

    o_str << "Attribute event : archive event period = ";
    p.events.arch_event.archive_period.empty() == true ? o_str << "Not specified" : o_str
        << p.events.arch_event.archive_period;
    o_str << std::endl;

    for (i = 0; i < p.events.arch_event.extensions.size(); i++)
    {
        if (i == 0)
        {
            o_str << std::endl;
        }
        o_str << "Attribute alarm : archive event extensions " << i + 1 << " = " << p.events.arch_event.extensions[i];
        if (i != p.events.arch_event.extensions.size() - 1)
        {
            o_str << std::endl;
        }
    }

    return o_str;
}

//+----------------------------------------------------------------------------------------------------------------
//
// function :
// 		operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the PipeInfo class
//
//-----------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &o_str, const PipeInfo &p)
{

//
// Print all these properties
//

    o_str << "Pipe name = " << p.name << std::endl;
    o_str << "Pipe label = " << p.label << std::endl;
    o_str << "Pipe description = " << p.description << std::endl;

    o_str << "Pipe writable type = ";
    if (p.writable == PIPE_READ)
    {
        o_str << "READ" << std::endl;
    }
    else
    {
        o_str << "READ_WRITE" << std::endl;
    }

    o_str << "Pipe display level = ";
    switch (p.disp_level)
    {
        case DL_UNKNOWN :
            o_str << "Unknown";
            break;

        case OPERATOR:
            o_str << "Operator";
            break;

        case EXPERT:
            o_str << "Expert";
            break;

        default:
            break;
    }

    unsigned int i;
    for (i = 0; i < p.extensions.size(); i++)
    {
        if (i == 0)
        {
            o_str << std::endl;
        }
        o_str << "Pipe extensions " << i + 1 << " = " << p.extensions[i] << std::endl;
    }

    return o_str;
}

} // End of tango namespace

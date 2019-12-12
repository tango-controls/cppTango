//+==================================================================================================================
//
// file :		Device.cpp
//
// description :	C++ source code for the DeviceImpl class. This class is the root class for all derived Device
//					classes. It is an abstract class. The DeviceImpl class is the CORBA servant which is "exported"
//					onto the network and accessed by the client.
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <new>

#include <basiccommand.h>
#include <blackbox.h>
#include <dserversignal.h>
#include <classattribute.h>
#include <eventsupplier.h>
#include <apiexcept.h>

#ifdef TANGO_HAS_LOG4TANGO
#include <logging.h>
#endif

namespace Tango
{

//
// The per thread data storage key (The client identification is stored in thread specific storage) defined in utils.cpp
//

extern omni_thread::key_t key;

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::DeviceImpl
//
// description :	constructors for the device_impl class from the class object
//			pointer, the device name, the description field,
//			the =aqmz and the status.
//
// argument : in :	- cl_ptr : The class object pointer
//			- d_name : The device name
//			- de : The device description (default to "A TANGO device")
//			- st : The device state (default to UNKNOWN)
//			- sta : The device status (default to "Not initialised")
//
//--------------------------------------------------------------------------

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr, const char *d_name,
                       const char *de, Tango::DevState st, const char *sta)
    : device_name(d_name), desc(de), device_status(sta),
      device_state(st), device_class(cl_ptr), ext(new DeviceImplExt),
#ifdef TANGO_HAS_LOG4TANGO
      logger(NULL), saved_log_level(log4tango::Level::WARN), rft(Tango::kDefaultRollingThreshold), poll_old_factor(0),
      idl_version(1),
#endif
      exported(false), polled(false), poll_ring_depth(0), only_one(d_name),
      store_in_bb(true), poll_mon("cache"), att_conf_mon("att_config"),
      state_from_read(false), py_device(false), device_locked(false),
      locker_client(NULL), old_locker_client(NULL), lock_ctr(0),
      min_poll_period(0), run_att_conf_loop(true), force_alarm_state(false), with_fwd_att(false),
      event_intr_change_subscription(0), intr_change_ev(false), devintr_thread(Tango_nullptr)
{
    real_ctor();
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr, string &d_name, string &de,
                       Tango::DevState st, string &sta)
    : device_name(d_name), desc(de), device_status(sta),
      device_state(st), device_class(cl_ptr), ext(new DeviceImplExt),
#ifdef TANGO_HAS_LOG4TANGO
      logger(NULL), saved_log_level(log4tango::Level::WARN), rft(Tango::kDefaultRollingThreshold), poll_old_factor(0),
      idl_version(1),
#endif
      exported(false), polled(false), poll_ring_depth(0), only_one(d_name.c_str()),
      store_in_bb(true), poll_mon("cache"), att_conf_mon("att_config"),
      state_from_read(false), py_device(false), device_locked(false),
      locker_client(NULL), old_locker_client(NULL), lock_ctr(0),
      min_poll_period(0), run_att_conf_loop(true), force_alarm_state(false), with_fwd_att(false),
      event_intr_change_subscription(0), intr_change_ev(false), devintr_thread(Tango_nullptr)
{
    real_ctor();
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr, string &d_name)
    : device_name(d_name), device_class(cl_ptr), ext(new DeviceImplExt),
#ifdef TANGO_HAS_LOG4TANGO
      logger(NULL), saved_log_level(log4tango::Level::WARN), rft(Tango::kDefaultRollingThreshold), poll_old_factor(0),
      idl_version(1),
#endif
      exported(false), polled(false), poll_ring_depth(0), only_one(d_name.c_str()),
      store_in_bb(true), poll_mon("cache"), att_conf_mon("att_config"),
      state_from_read(false), py_device(false), device_locked(false),
      locker_client(NULL), old_locker_client(NULL), lock_ctr(0),
      min_poll_period(0), run_att_conf_loop(true), force_alarm_state(false), with_fwd_att(false),
      event_intr_change_subscription(0), intr_change_ev(false), devintr_thread(Tango_nullptr)
{
    desc = "A Tango device";
    device_state = Tango::UNKNOWN;
    device_status = StatusNotSet;

    real_ctor();
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr, string &d_name, string &description)
    : device_name(d_name), device_class(cl_ptr), ext(new DeviceImplExt),
#ifdef TANGO_HAS_LOG4TANGO
      logger(NULL), saved_log_level(log4tango::Level::WARN), rft(Tango::kDefaultRollingThreshold), poll_old_factor(0),
      idl_version(1),
#endif
      exported(false), polled(false), poll_ring_depth(0), only_one(d_name.c_str()),
      store_in_bb(true), poll_mon("cache"), att_conf_mon("att_config"),
      state_from_read(false), py_device(false), device_locked(false),
      locker_client(NULL), old_locker_client(NULL), lock_ctr(0),
      min_poll_period(0), run_att_conf_loop(true), force_alarm_state(false), with_fwd_att(false),
      event_intr_change_subscription(0), intr_change_ev(false), devintr_thread(Tango_nullptr)
{
    desc = description;
    device_state = Tango::UNKNOWN;
    device_status = StatusNotSet;

    real_ctor();
}

void DeviceImpl::real_ctor()
{
    version = DevVersion;
    blackbox_depth = 0;

    device_prev_state = device_state;

//
// Init lower case device name
//

    device_name_lower = device_name;
    transform(device_name_lower.begin(), device_name_lower.end(),
              device_name_lower.begin(), ::tolower);

//
//  Write the device name into the per thread data for sub device diagnostics
//

    Tango::Util *tg = Tango::Util::instance();
    tg->get_sub_dev_diag().set_associated_device(device_name_lower);

//
// Create the DbDevice object
//

    try
    {
        db_dev = new DbDevice(device_name, Tango::Util::instance()->get_database());
    }
    catch (Tango::DevFailed &)
    {
        throw;
    }

    get_dev_system_resource();

    black_box_create();

    idl_version = 1;
    devintr_shared.th_running = false;

//
// Create the multi attribute object
//

    dev_attr = new MultiAttribute(device_name, device_class, this);

//
// Create device pipe and finish the pipe config init since we now have device name
//

    device_class->create_device_pipe(device_class, this);
    end_pipe_config();

//
// Build adm device name
//

    adm_device_name = "dserver/";
    adm_device_name = adm_device_name + Util::instance()->get_ds_name();

//
// Init logging
//

#ifdef TANGO_HAS_LOG4TANGO
    init_logger();
#endif

}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::stop_polling
//
// description :	Stop all polling for a device. if the device is
//			polled, call this method before deleting it.
//
// argin(s) : - with_db_upd : Is it necessary to update db ?
//
//--------------------------------------------------------------------------

void DeviceImpl::stop_polling(bool with_db_upd)
{
    Tango::Util *tg = Tango::Util::instance();

//
// If the vector of polling info is empty, no need to do anything (polling
// already stopped for all devices)
//

    vector<PollingThreadInfo *> &v_th_info = tg->get_polling_threads_info();
    if (v_th_info.empty() == true)
    {
        return;
    }

//
// Find out which thread is in charge of the device.
//

    PollingThreadInfo *th_info;

    int poll_th_id = tg->get_polling_thread_id_by_name(device_name.c_str());
    if (poll_th_id == 0)
    {
        TangoSys_OMemStream o;
        o << "Can't find a polling thread for device " << device_name << ends;
        Except::throw_exception((const char *) API_PollingThreadNotFound, o.str(),
                                (const char *) "DeviImpl::stop_polling");
    }

    th_info = tg->get_polling_thread_info_by_id(poll_th_id);

    TangoMonitor &mon = th_info->poll_mon;
    PollThCmd &shared_cmd = th_info->shared_data;

    {
        omni_mutex_lock sync(mon);
        if (shared_cmd.cmd_pending == true)
        {
            mon.wait();
        }
        shared_cmd.cmd_pending = true;
        shared_cmd.cmd_code = POLL_REM_DEV;
        shared_cmd.dev = this;

        mon.signal();

//
// Wait for thread to execute command
//

        while (shared_cmd.cmd_pending == true)
        {
            int interupted = mon.wait(DEFAULT_TIMEOUT);

            if ((shared_cmd.cmd_pending == true) && (interupted == false))
            {
                cout4 << "TIME OUT" << endl;
                Except::throw_exception((const char *) API_CommandTimedOut,
                                        (const char *) "Polling thread blocked !!",
                                        (const char *) "DeviceImpl::stop_polling");

            }
        }
    }

    is_polled(false);

//
// Update the pool conf first locally.
// Also update the map<device name,thread id>
// If this device was the only one for a polling thread, kill the thread
// Then in Db if possible
//

    bool kill_thread = false;
    int ind;

    if ((ind = tg->get_dev_entry_in_pool_conf(device_name_lower)) == -1)
    {
        TangoSys_OMemStream o;
        o << "Can't find entry for device " << device_name << " in polling threads pool configuration !" << ends;
        Except::throw_exception((const char *) API_PolledDeviceNotInPoolConf, o.str(),
                                (const char *) "DeviceImpl::stop_polling");
    }

    vector<string> &pool_conf = tg->get_poll_pool_conf();
    string &conf_entry = pool_conf[ind];
    string::size_type pos;
    if ((pos = conf_entry.find(',')) != string::npos)
    {
        pos = conf_entry.find(device_name_lower);
        if ((pos + device_name_lower.size()) != conf_entry.size())
        {
            conf_entry.erase(pos, device_name_lower.size() + 1);
        }
        else
        {
            conf_entry.erase(pos - 1);
        }
    }
    else
    {
        vector<string>::iterator iter = pool_conf.begin() + ind;
        pool_conf.erase(iter);
        kill_thread = true;
    }

    tg->remove_dev_from_polling_map(device_name_lower);

//
// Kill the thread if needed and join
//

    if (kill_thread == true)
    {
        TangoMonitor &mon = th_info->poll_mon;
        PollThCmd &shared_cmd = th_info->shared_data;

        {
            omni_mutex_lock sync(mon);

            shared_cmd.cmd_pending = true;
            shared_cmd.cmd_code = POLL_EXIT;

            mon.signal();
        }

        void *dummy_ptr;
        cout4 << "POLLING: Joining with one polling thread" << endl;
        th_info->poll_th->join(&dummy_ptr);

        tg->remove_polling_thread_info_by_id(poll_th_id);
    }

//
// Update db
//

    if ((with_db_upd == true) && (Tango::Util::_UseDb == true))
    {
        DbData send_data;
        send_data.push_back(DbDatum("polling_threads_pool_conf"));
        send_data[0] << tg->get_poll_pool_conf();

        tg->get_dserver_device()->get_db_device()->put_property(send_data);
    }
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::~DeviceImpl
//
// description :	Destructor for the device class. It simply frees
//			the memory allocated for the black box object
//
//--------------------------------------------------------------------------

DeviceImpl::~DeviceImpl()
{
    cout4 << "Entering DeviceImpl destructor for device " << device_name << endl;

//
// Call user delete_device method
//

    delete_device();

//
// Delete the black box
//

    delete blackbox_ptr;

//
// Delete the DbDevice object
//

    delete db_dev;

//
// Unregister the signal from signal handler
//

    DServerSignal::instance()->unregister_dev_signal(this);

//
// Delete the multi attribute object
//

    delete dev_attr;

//
// Clean up previously executed in extension destructor
// Deletes memory for ring buffer used for polling
//

    for (unsigned long i = 0; i < poll_obj_list.size(); i++)
    {
        delete (poll_obj_list[i]);
    }

#ifdef TANGO_HAS_LOG4TANGO
    if (logger && logger != Logging::get_core_logger())
    {
        logger->remove_all_appenders();
        delete logger;
        logger = 0;
    }
#endif

    delete locker_client;
    delete old_locker_client;

//
// Delete the extension class instance
//

#ifndef HAS_UNIQUE_PTR
    delete ext;
#endif

//
// Clear our ptr in the device class vector
//

    vector<DeviceImpl *> &dev_vect = get_device_class()->get_device_list();
    vector<DeviceImpl *>::iterator ite = find(dev_vect.begin(), dev_vect.end(), this);
    if (ite != dev_vect.end())
    {
        *ite = NULL;
    }

    cout4 << "Leaving DeviceImpl destructor for device " << device_name << endl;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::black_box_create
//
// description :	Private method to create the device black box.
//			The black box depth is a resource with a default
//			value if the resource is not defined
//
//--------------------------------------------------------------------------

void DeviceImpl::black_box_create()
{

//
// If the black box depth object attribute is null, create one with the
// default depth
//

    try
    {
        if (blackbox_depth == 0)
        {
            blackbox_ptr = new BlackBox();
        }
        else
        {
            blackbox_ptr = new BlackBox(blackbox_depth);
        }
    }
    catch (bad_alloc &)
    {
        throw;
    }
}

//+----------------------------------------------------------------------------
//
// method :		DeviceImpl::get_dev_system_resource()
//
// description :	Method to retrieve some basic device resources
//			The resources to be retrived are :
//				- The black box depth
//				- The device description
//				- The polling ring buffer depth
//				- The polled command(s)
//				- The polled attribute(s)
//				- The non automatic polled command list
//				- The non automatic polled attribute list
//				- The polling too old factor
//				- The command polling ring depth (if any)
//				- The attribute polling ring depth (if any)
//
//-----------------------------------------------------------------------------

void DeviceImpl::get_dev_system_resource()
{

//
// Try to retrieve resources for device black box depth and device
// description
//

    Tango::Util *tg = Tango::Util::instance();
    if (tg->_UseDb == true)
    {
        DbData db_data;

        db_data.push_back(DbDatum("blackbox_depth"));
        db_data.push_back(DbDatum("description"));
        db_data.push_back(DbDatum("poll_ring_depth"));
        db_data.push_back(DbDatum("polled_cmd"));
        db_data.push_back(DbDatum("polled_attr"));
        db_data.push_back(DbDatum("non_auto_polled_cmd"));
        db_data.push_back(DbDatum("non_auto_polled_attr"));
        db_data.push_back(DbDatum("poll_old_factor"));
        db_data.push_back(DbDatum("cmd_poll_ring_depth"));
        db_data.push_back(DbDatum("attr_poll_ring_depth"));
        db_data.push_back(DbDatum("min_poll_period"));
        db_data.push_back(DbDatum("cmd_min_poll_period"));
        db_data.push_back(DbDatum("attr_min_poll_period"));

        try
        {
            db_dev->get_property(db_data);
        }
        catch (Tango::DevFailed &)
        {
            TangoSys_OMemStream o;
            o << "Database error while trying to retrieve device prperties for device " << device_name.c_str() << ends;

            Except::throw_exception((const char *) API_DatabaseAccess,
                                    o.str(),
                                    (const char *) "DeviceImpl::get_dev_system_resource");
        }

        if (db_data[0].is_empty() == false)
        {
            db_data[0] >> blackbox_depth;
        }
        if (db_data[1].is_empty() == false)
        {
            db_data[1] >> desc;
        }
        if (db_data[2].is_empty() == false)
        {
            long tmp_depth;
            db_data[2] >> tmp_depth;
            set_poll_ring_depth(tmp_depth);
        }
        if (db_data[3].is_empty() == false)
        {
            db_data[3] >> get_polled_cmd();
        }
        if (db_data[4].is_empty() == false)
        {
            db_data[4] >> get_polled_attr();
        }
        if (db_data[5].is_empty() == false)
        {
            db_data[5] >> get_non_auto_polled_cmd();
        }
        if (db_data[6].is_empty() == false)
        {
            db_data[6] >> get_non_auto_polled_attr();
        }
        if (db_data[7].is_empty() == false)
        {
            long tmp_poll;
            db_data[7] >> tmp_poll;
            set_poll_old_factor(tmp_poll);
        }
        else
        {
            set_poll_old_factor(DEFAULT_POLL_OLD_FACTOR);
        }
        if (db_data[8].is_empty() == false)
        {
            db_data[8] >> cmd_poll_ring_depth;
            unsigned long nb_prop = cmd_poll_ring_depth.size();
            if ((nb_prop % 2) == 1)
            {
                cmd_poll_ring_depth.clear();
                TangoSys_OMemStream o;
                o << "System property cmd_poll_ring_depth for device " << device_name << " has wrong syntax" << ends;
                Except::throw_exception((const char *) API_BadConfigurationProperty,
                                        o.str(),
                                        (const char *) "DeviceImpl::get_dev_system_resource()");
            }
            for (unsigned int i = 0; i < nb_prop; i = i + 2)
            {
                transform(cmd_poll_ring_depth[i].begin(),
                          cmd_poll_ring_depth[i].end(),
                          cmd_poll_ring_depth[i].begin(),
                          ::tolower);
            }
        }
        if (db_data[9].is_empty() == false)
        {
            db_data[9] >> attr_poll_ring_depth;
            unsigned long nb_prop = attr_poll_ring_depth.size();
            if ((attr_poll_ring_depth.size() % 2) == 1)
            {
                attr_poll_ring_depth.clear();
                TangoSys_OMemStream o;
                o << "System property attr_poll_ring_depth for device " << device_name << " has wrong syntax" << ends;
                Except::throw_exception((const char *) API_BadConfigurationProperty,
                                        o.str(),
                                        (const char *) "DeviceImpl::get_dev_system_resource()");
            }
            for (unsigned int i = 0; i < nb_prop; i = i + 2)
            {
                transform(attr_poll_ring_depth[i].begin(),
                          attr_poll_ring_depth[i].end(),
                          attr_poll_ring_depth[i].begin(),
                          ::tolower);
            }
        }

//
// The min. period related properties
//

        if (db_data[10].is_empty() == false)
        {
            db_data[10] >> min_poll_period;
        }

        if (db_data[11].is_empty() == false)
        {
            db_data[11] >> cmd_min_poll_period;
            unsigned long nb_prop = cmd_min_poll_period.size();
            if ((cmd_min_poll_period.size() % 2) == 1)
            {
                cmd_min_poll_period.clear();
                TangoSys_OMemStream o;
                o << "System property cmd_min_poll_period for device " << device_name << " has wrong syntax" << ends;
                Except::throw_exception((const char *) API_BadConfigurationProperty,
                                        o.str(),
                                        (const char *) "DeviceImpl::get_dev_system_resource()");
            }
            for (unsigned int i = 0; i < nb_prop; i = i + 2)
            {
                transform(cmd_min_poll_period[i].begin(),
                          cmd_min_poll_period[i].end(),
                          cmd_min_poll_period[i].begin(),
                          ::tolower);
            }
        }

        if (db_data[12].is_empty() == false)
        {
            db_data[12] >> attr_min_poll_period;
            unsigned long nb_prop = attr_min_poll_period.size();
            if ((attr_min_poll_period.size() % 2) == 1)
            {
                attr_min_poll_period.clear();
                TangoSys_OMemStream o;
                o << "System property attr_min_poll_period for device " << device_name << " has wrong syntax" << ends;
                Except::throw_exception((const char *) API_BadConfigurationProperty,
                                        o.str(),
                                        (const char *) "DeviceImpl::get_dev_system_resource()");
            }
            for (unsigned int i = 0; i < nb_prop; i = i + 2)
            {
                transform(attr_min_poll_period[i].begin(),
                          attr_min_poll_period[i].end(),
                          attr_min_poll_period[i].begin(),
                          ::tolower);
            }
        }

//
// Since Tango V5 (IDL V3), State and Status are now polled as attributes
// Change properties if necessary
//

        if ((get_polled_cmd()).size() != 0)
        {
            poll_lists_2_v5();
        }
    }
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::_default_POA
//
// description :	Return a pointer to the POA on which the device should
//			be activated. This method is required by CORBA to
//			create a POA with the IMPLICIT_ACTIVATION policy
//
//--------------------------------------------------------------------------

PortableServer::POA_ptr DeviceImpl::_default_POA()
{
    return Util::instance()->get_poa();
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::register_signal
//
// description :	Method to register a device on a signal. When the
//			signal is sent to the process, the signal_handler
//			method of this class will be executed
//
// in : 		signo : The signal number
//
//--------------------------------------------------------------------------

#ifndef  _TG_WINDOWS_
void DeviceImpl::register_signal(long signo, bool hand)
{
    cout4 << "DeviceImpl::register_signal() arrived for signal " << signo << endl;

    DServerSignal::instance()->register_dev_signal(signo, hand, this);

    cout4 << "Leaving DeviceImpl::register_signal method()" << endl;
}
#else
                                                                                                                        void DeviceImpl::register_signal(long signo)
{
	cout4 << "DeviceImpl::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_dev_signal(signo,this);

	cout4 << "Leaving DeviceImpl::register_signal method()" << endl;
}
#endif

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::unregister_signal
//
// description :	Method to unregister a device on a signal.
//
// in : 		signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceImpl::unregister_signal(long signo)
{
    cout4 << "DeviceImpl::unregister_signal() arrived for signal " << signo << endl;

    DServerSignal::instance()->unregister_dev_signal(signo, this);

    cout4 << "Leaving DeviceImpl::unregister_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::signal_handler
//
// description :	This is the signal handler for the device. This method
//			is defined as virtual and therefore, can be redefined
//			by DS programmers in their own classes derived from
//			DeviceImpl
//
// in : 		signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceImpl::signal_handler(long signo)
{
    cout4 << "DeviceImpl::signal_handler() arrived for signal " << signo << endl;

    cout4 << "Leaving DeviceImpl::signal_handler method()" << endl;
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::check_command_exist
//
// description :	This method check that a comamnd is supported by
//			the device and does not need input value.
//			The method throws an exception if the
//			command is not defined or needs an input value
//
// in : 		cmd_name : The command name
//
//--------------------------------------------------------------------------

void DeviceImpl::check_command_exists(const string &cmd_name)
{
    vector<Command *> &cmd_list = device_class->get_command_list();
    unsigned long i;
    for (i = 0; i < cmd_list.size(); i++)
    {
        if (cmd_list[i]->get_lower_name() == cmd_name)
        {
            if (cmd_list[i]->get_in_type() != Tango::DEV_VOID)
            {
                TangoSys_OMemStream o;
                o << "Command " << cmd_name << " cannot be polled because it needs input value" << ends;
                Except::throw_exception((const char *) API_IncompatibleCmdArgumentType,
                                        o.str(), (const char *) "DeviceImpl::check_command_exists");
            }
            return;
        }
    }

    TangoSys_OMemStream o;
    o << "Command " << cmd_name << " not found" << ends;
    Except::throw_exception((const char *) API_CommandNotFound, o.str(),
                            (const char *) "DeviceImpl::check_command_exists");
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::get_command
//
// description :	This method returns a pointer to command object.
//			The method throws an exception if the
//			command is not defined
//
// in : 		cmd_name : The command name
//
//--------------------------------------------------------------------------

Command *DeviceImpl::get_command(const string &cmd_name)
{
    vector<Command *> cmd_list = device_class->get_command_list();
    unsigned long i;
    for (i = 0; i < cmd_list.size(); i++)
    {
        if (cmd_list[i]->get_lower_name() == cmd_name)
        {
            return cmd_list[i];
        }
    }

    TangoSys_OMemStream o;
    o << "Command " << cmd_name << " not found" << ends;
    Except::throw_exception((const char *) API_CommandNotFound, o.str(),
                            (const char *) "DeviceImpl::get_command");

//
// This piece of code is added for VC++ compiler. As they advice, I have try to
// use the __decspec(noreturn) for the throw_exception method, but it seems
// that it does not work !
//

    return NULL;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::get_polled_obj_by_type_name
//
// description :	This method check that a comamnd is supported by
//			the device and does not need input value.
//			The method throws an exception if the
//			command is not defined or needs an input value
//
// in : 		cmd_name : The command name
//
//--------------------------------------------------------------------------

vector<PollObj *>::iterator DeviceImpl::get_polled_obj_by_type_name(
    Tango::PollObjType obj_type,
    const string &obj_name)
{
    vector<PollObj *> &po_list = get_poll_obj_list();
    vector<PollObj *>::iterator ite;
    for (ite = po_list.begin(); ite < po_list.end(); ++ite)
    {
        omni_mutex_lock sync(**ite);
        {
            if ((*ite)->get_type_i() == obj_type)
            {
                if ((*ite)->get_name_i() == obj_name)
                {
                    return ite;
                }
            }
        }
    }

    TangoSys_OMemStream o;
    o << obj_name << " not found in list of polled object" << ends;
    Except::throw_exception((const char *) API_PollObjNotFound, o.str(),
                            (const char *) "DeviceImpl::get_polled_obj_by_type_name");

//
// Only to make compiler quiet. Should never pass here
//

// exclude the return value for VC8+
#if not defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER < 1400)
    return (vector<PollObj *>::iterator) NULL;
#endif
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::get_cmd_poll_ring_depth
//
// description :
//		This method returns the polling buffer depth. Most of the times, this is defined at device level
//		via the device "poll_ring_depth" property. Nevertheless, in some cases, this value can be overwritten via the
//		device "cmd_poll_ring_depth" property.
//
// args :
// 		in :
//			- cmd_name : The command name
//
// return :
// 		This method returns the polling buffer depth
//
//-------------------------------------------------------------------------------------------------------------------

long DeviceImpl::get_cmd_poll_ring_depth(string &cmd_name)
{
    long ret;

    if (cmd_poll_ring_depth.size() == 0)
    {
//
// No specific depth defined
//
        if (poll_ring_depth == 0)
        {
            ret = DefaultPollRingDepth;
        }
        else
        {
            ret = poll_ring_depth;
        }
    }
    else
    {
        unsigned long k;
//
// Try to find command in list of specific polling buffer depth
//

        for (k = 0; k < cmd_poll_ring_depth.size(); k = k + 2)
        {
            if (cmd_poll_ring_depth[k] == cmd_name)
            {
                TangoSys_MemStream s;
                s << cmd_poll_ring_depth[k + 1];
                if (!(s >> ret))
                {
                    TangoSys_OMemStream o;
                    o << "System property cmd_poll_ring_depth for device " << device_name << " has wrong syntax"
                      << ends;
                    Except::throw_exception((const char *) API_BadConfigurationProperty,
                                            o.str(),
                                            (const char *) "DeviceImpl::get_poll_ring_depth()");
                }
                break;
            }
        }
        if (k >= cmd_poll_ring_depth.size())
        {
//
// Not found
//

            if (poll_ring_depth == 0)
            {
                ret = DefaultPollRingDepth;
            }
            else
            {
                ret = poll_ring_depth;
            }
        }
    }

    return ret;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::get_attr_poll_ring_depth
//
// description :
//		This method returns the polling buffer depth. Most of the times, this is defined at device level
//		via the device "poll_ring_depth" property. Nevertheless, in some cases, this value can be overwritten via the
//		device "attr_poll_ring_depth" property.
//
// args :
// 		in :
//			- attr_name : The attribute name
//
// return :
// 		This method returns the polling buffer depth
//
//--------------------------------------------------------------------------

long DeviceImpl::get_attr_poll_ring_depth(string &attr_name)
{
    long ret;

    if (attr_poll_ring_depth.size() == 0)
    {
        if ((attr_name == "state") || (attr_name == "status"))
        {
            ret = get_cmd_poll_ring_depth(attr_name);
        }
        else
        {

//
// No specific depth defined
//

            if (poll_ring_depth == 0)
            {
                ret = DefaultPollRingDepth;
            }
            else
            {
                ret = poll_ring_depth;
            }
        }
    }
    else
    {
        unsigned long k;
//
// Try to find command in list of specific polling buffer depth
//

        for (k = 0; k < attr_poll_ring_depth.size(); k = k + 2)
        {
            if (attr_poll_ring_depth[k] == attr_name)
            {
                TangoSys_MemStream s;
                s << attr_poll_ring_depth[k + 1];
                if (!(s >> ret))
                {
                    TangoSys_OMemStream o;
                    o << "System property attr_poll_ring_depth for device " << device_name << " has wrong syntax"
                      << ends;
                    Except::throw_exception((const char *) API_BadConfigurationProperty,
                                            o.str(),
                                            (const char *) "DeviceImpl::get_poll_ring_depth()");
                }
                break;
            }
        }
        if (k >= attr_poll_ring_depth.size())
        {
            if ((attr_name == "state") || (attr_name == "status"))
            {
                ret = get_cmd_poll_ring_depth(attr_name);
            }
            else
            {
//
// Not found
//

                if (poll_ring_depth == 0)
                {
                    ret = DefaultPollRingDepth;
                }
                else
                {
                    ret = poll_ring_depth;
                }
            }
        }
    }

    return ret;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::dev_state
//
// description :
//		The default method called by the DevState command. If the device is ON, this method checks attribute
//		with a defined alarm and set the state to ALARM if one of these attribute is in alarm. Otherwise, simply
//		returns device state
//
//--------------------------------------------------------------------------------------------------------------------

Tango::DevState DeviceImpl::dev_state()
{
    NoSyncModelTangoMonitor mon(this);

//
// If we need to run att. conf loop, do it.
// If the flag to force state is true, do not call state computation method, simply set it to ALARM
//

    if (run_att_conf_loop == true)
    {
        att_conf_loop();
    }

    if (device_state != Tango::FAULT && force_alarm_state == true)
    {
        return Tango::ALARM;
    }
    else
    {
        if ((device_state == Tango::ON) ||
            (device_state == Tango::ALARM))
        {

//
// Build attribute lists
//

            long vers = get_dev_idl_version();
            bool set_alrm = false;

            vector<long> attr_list = dev_attr->get_alarm_list();
            vector<long> attr_list_2 = get_alarmed_not_read();
            vector<long> attr_polled_list;
            long nb_wanted_attr;

            if (vers >= 3)
            {
                if (state_from_read == true)
                {
                    vector<long>::iterator ite = attr_list_2.begin();
                    while (ite != attr_list_2.end())
                    {
                        Attribute &att = dev_attr->get_attr_by_ind(*ite);
                        if (att.is_polled() == true)
                        {
                            ite = attr_list_2.erase(ite);
                        }
                        else
                        {
                            ++ite;
                        }
                    }
                    nb_wanted_attr = attr_list_2.size();
                }
                else
                {
                    vector<long>::iterator ite = attr_list.begin();
                    while (ite != attr_list.end())
                    {
                        Attribute &att = dev_attr->get_attr_by_ind(*ite);
                        if (att.is_polled() == true)
                        {
                            ite = attr_list.erase(ite);
                        }
                        else
                        {
                            ++ite;
                        }
                    }
                    nb_wanted_attr = attr_list.size();
                }
            }
            else
            {
                nb_wanted_attr = attr_list.size();
            }

            cout4 << "State: Number of attribute(s) to read: " << nb_wanted_attr << endl;

            if (nb_wanted_attr != 0)
            {

//
// Read the hardware
//

                if (state_from_read == false)
                {
                    read_attr_hardware(attr_list);
                }

//
// Set attr value
//

                long i, j;
                vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();

                for (i = 0; i < nb_wanted_attr; i++)
                {

//
// Starting with IDL 3, it is possible that some of the alarmed attribute have already been read.
//

                    long idx;
                    if ((vers >= 3) && (state_from_read == true))
                    {
                        idx = attr_list_2[i];
                    }
                    else
                    {
                        idx = attr_list[i];
                    }

                    Attribute &att = dev_attr->get_attr_by_ind(idx);
                    att.save_alarm_quality();

                    try
                    {
                        if (vers < 3)
                        {
                            read_attr(att);
                        }
                        else
                        {

//
// Otherwise, get it from device
//

                            att.wanted_date(false);
                            att.set_value_flag(false);

                            if (attr_vect[att.get_attr_idx()]->is_allowed(this, Tango::READ_REQ) == false)
                            {
                                att.wanted_date(true);
                                continue;
                            }
                            attr_vect[att.get_attr_idx()]->read(this, att);
                            Tango::AttrQuality qua = att.get_quality();
                            if ((qua != Tango::ATTR_INVALID) && (att.get_value_flag() == false))
                            {
                                TangoSys_OMemStream o;

                                o << "Read value for attribute ";
                                o << att.get_name();
                                o << " has not been updated";
                                o << "Hint: Did the server follow Tango V5 attribute reading framework ?" << ends;

                                Except::throw_exception((const char *) API_AttrValueNotSet, o.str(),
                                                        (const char *) "DeviceImpl::dev_state");
                            }
                        }
                    }
                    catch (Tango::DevFailed &)
                    {
                        if (!att.get_value_flag())
                        {
                            WARN_STREAM
                                << "Attribute has no value, forcing INVALID quality for: "
                                << att.get_name() << std::endl;
                            att.set_quality(Tango::ATTR_INVALID);
                        }

                        for (j = 0; j < i; j++)
                        {
                            long idx;
                            if ((vers >= 3) && (state_from_read == true))
                            {
                                idx = attr_list_2[j];
                            }
                            else
                            {
                                idx = attr_list[j];
                            }
                            Tango::Attribute &tmp_att = dev_attr->get_attr_by_ind(idx);
                            if (att.get_wanted_date() == false)
                            {
                                if (tmp_att.get_quality() != Tango::ATTR_INVALID)
                                {
                                    tmp_att.delete_seq();
                                }
                                tmp_att.wanted_date(true);
                            }
                        }
                        att.wanted_date(true);
                        //                       throw;
                    }
                }

//
// Check alarm level
//

                if (dev_attr->check_alarm() == true)
                {
                    set_alrm = true;
                    if (device_state != Tango::ALARM)
                    {
                        device_state = Tango::ALARM;
                        ext->alarm_state_kernel = time(NULL);
                    }
                }
                else
                {
                    if (ext->alarm_state_kernel > ext->alarm_state_user)
                    {
                        device_state = Tango::ON;
                    }
                }


//
// Free the sequence created to store the attribute value
//

                for (long i = 0; i < nb_wanted_attr; i++)
                {
                    long idx;
                    if ((vers >= 3) && (state_from_read == true))
                    {
                        idx = attr_list_2[i];
                    }
                    else
                    {
                        idx = attr_list[i];
                    }
                    Tango::Attribute &att = dev_attr->get_attr_by_ind(idx);
                    if (att.get_wanted_date() == false)
                    {
                        if (att.get_quality() != Tango::ATTR_INVALID)
                        {
                            att.delete_seq();
                        }
                        att.wanted_date(true);
                    }
                }
            }
            else
            {
                if (ext->alarm_state_kernel > ext->alarm_state_user)
                {
                    device_state = Tango::ON;
                }
            }



//
// Check if one of the remaining attributes has its quality factor set to ALARM or WARNING. It is not necessary to do
// this if we have already detected that the state must switch to ALARM
//

            if ((set_alrm == false) && (device_state != Tango::ALARM))
            {
                if (dev_attr->is_att_quality_alarmed() == true)
                {
                    if (device_state != Tango::ALARM)
                    {
                        device_state = Tango::ALARM;
                        ext->alarm_state_kernel = time(NULL);
                    }
                }
                else
                {
                    device_state = Tango::ON;
                }
            }
        }
    }

    return device_state;
}

//----------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::dev_status
//
// description :
//		The default method called by the DevStatus command. If the device is ON, this method add Attribute status
//		for all device attribute in alarm state.
//
//---------------------------------------------------------------------------------------------------------------------

Tango::ConstDevString DeviceImpl::dev_status()
{
    NoSyncModelTangoMonitor mon(this);
    const char *returned_str;

    if (run_att_conf_loop == true)
    {
        att_conf_loop();
    }

    if (device_state != Tango::FAULT && force_alarm_state == true)
    {
        alarm_status = "The device is in ALARM state.";

//
// First add message for attribute with wrong conf. in db
//

        size_t nb_wrong_att = att_wrong_db_conf.size();
        if (nb_wrong_att != 0)
        {
            alarm_status = alarm_status + "\nAttribute";
            build_att_list_in_status_mess(nb_wrong_att, DeviceImpl::CONF);
            alarm_status = alarm_status + "wrong configuration";
            alarm_status = alarm_status + "\nTry accessing the faulty attribute(s) to get more information";
        }

//
// Add message for memorized attributes which failed during device startup
//

        nb_wrong_att = att_mem_failed.size();
        if (nb_wrong_att != 0)
        {
            alarm_status = alarm_status + "\nMemorized attribute";
            build_att_list_in_status_mess(nb_wrong_att, DeviceImpl::MEM);
            alarm_status = alarm_status + "failed during device startup sequence";
        }

//
// Add message for forwarded attributes wrongly configured
//

        nb_wrong_att = fwd_att_wrong_conf.size();
        if (nb_wrong_att != 0)
        {
            build_att_list_in_status_mess(nb_wrong_att, DeviceImpl::FWD);
        }

        returned_str = alarm_status.c_str();
    }
    else
    {
        if (device_status == StatusNotSet)
        {
            alarm_status = "The device is in ";
            alarm_status = alarm_status + DevStateName[device_state] + " state.";
            if (device_state == Tango::ALARM)
            {
                dev_attr->read_alarm(alarm_status);
                dev_attr->add_alarmed_quality_factor(alarm_status);
            }
            returned_str = alarm_status.c_str();
        }
        else
        {
            if (device_state == Tango::ALARM)
            {
                alarm_status = device_status;
                dev_attr->read_alarm(alarm_status);
                dev_attr->add_alarmed_quality_factor(alarm_status);
                returned_str = alarm_status.c_str();
            }
            else
            {
                returned_str = device_status.c_str();
            }
        }
    }

    return returned_str;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::command_inout
//
// description :	Method called for each command_inout operation executed
//			from any client
//			The call to this method is in a try bloc for the
//			Tango::DevFailed exception (generated by the idl
//			compiler in the tango_skel.cpp file). Therefore, it
//			is not necessary to propagate by re-throw the exceptions
//			thrown by the underlying functions/methods.
//
//--------------------------------------------------------------------------

CORBA::Any *DeviceImpl::command_inout(const char *in_cmd,
                                      const CORBA::Any &in_any)
{
    AutoTangoMonitor sync(this);

    string command(in_cmd);
    CORBA::Any *out_any;

    cout4 << "DeviceImpl::command_inout(): command received : " << command << endl;

//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//
    SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
    string last_associated_device = sub.get_associated_device();
    sub.set_associated_device(get_name());

//
// Catch all exceptions to set back the associated device after execution
//

    try
    {

//
// Record operation request in black box
//

        if (store_in_bb == true)
        {
            blackbox_ptr->insert_cmd(in_cmd);
        }
        store_in_bb = true;

//
// Execute command
//

        out_any = device_class->command_handler(this, command, in_any);
    }
    catch (...)
    {
        // set back the device attribution for the thread
        // and rethrow the exception.
        sub.set_associated_device(last_associated_device);
        throw;
    }

    // set back the device attribution for the thread
    sub.set_associated_device(last_associated_device);

//
// Return value to the caller
//

    cout4 << "DeviceImpl::command_inout(): leaving method for command " << in_cmd << endl;
    return (out_any);
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::name
//
// description :	Method called when a client request the name attribute
//			This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::name()
{
    try
    {
        cout4 << "DeviceImpl::name arrived" << endl;

//
// Record attribute request in black box
//

        blackbox_ptr->insert_corba_attr(Attr_Name);
    }
    catch (Tango::DevFailed &e)
    {
        CORBA::IMP_LIMIT lim;
        if (strcmp(e.errors[0].reason, API_CommandTimedOut) == 0)
        {
            lim.minor(TG_IMP_MINOR_TO);
        }
        else
        {
            lim.minor(TG_IMP_MINOR_DEVFAILED);
        }
        cout4 << "Leaving DeviceImpl::name throwing IMP_LIMIT" << endl;
        throw lim;
    }
    catch (...)
    {
        CORBA::IMP_LIMIT lim;
        lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
        cout4 << "Leaving DeviceImpl::name throwing IMP_LIMIT" << endl;
        throw lim;
    }

//
// Return data to caller
//

    cout4 << "Leaving DeviceImpl::name" << endl;
    return CORBA::string_dup(device_name.c_str());
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::adm_name
//
// description :	Method called when a client request the adm_name attribute
//			This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::adm_name()
{
    try
    {
        cout4 << "DeviceImpl::adm_name arrived" << endl;

//
// Record attribute request in black box
//

        blackbox_ptr->insert_corba_attr(Attr_AdmName);
    }
    catch (Tango::DevFailed &e)
    {
        CORBA::IMP_LIMIT lim;
        if (strcmp(e.errors[0].reason, API_CommandTimedOut) == 0)
        {
            lim.minor(TG_IMP_MINOR_TO);
        }
        else
        {
            lim.minor(TG_IMP_MINOR_DEVFAILED);
        }
        cout4 << "Leaving DeviceImpl::adm_name throwing IMP_LIMIT" << endl;
        throw lim;
    }
    catch (...)
    {
        CORBA::IMP_LIMIT lim;
        lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
        cout4 << "Leaving DeviceImpl::adm_name throwing IMP_LIMIT" << endl;
        throw lim;
    }

//
// Return data to caller
//

    cout4 << "Leaving DeviceImpl::adm_name" << endl;
    return CORBA::string_dup(adm_device_name.c_str());
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::description
//
// description :	Method called when a client request the description
//			attribute
//			This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::description()
{
    try
    {
        cout4 << "DeviceImpl::description arrived" << endl;

//
// Record attribute request in black box
//

        blackbox_ptr->insert_corba_attr(Attr_Description);
    }
    catch (Tango::DevFailed &e)
    {
        CORBA::IMP_LIMIT lim;
        if (strcmp(e.errors[0].reason, API_CommandTimedOut) == 0)
        {
            lim.minor(TG_IMP_MINOR_TO);
        }
        else
        {
            lim.minor(TG_IMP_MINOR_DEVFAILED);
        }
        cout4 << "Leaving DeviceImpl::description throwing IMP_LIMIT" << endl;
        throw lim;
    }
    catch (...)
    {
        CORBA::IMP_LIMIT lim;
        lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
        cout4 << "Leaving DeviceImpl::description throwing IMP_LIMIT" << endl;
        throw lim;
    }

//
// Return data to caller
//

    cout4 << "Leaving DeviceImpl::description" << endl;
    return CORBA::string_dup(desc.c_str());
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::state
//
// description :	Method called when a client request the state
//			attribute
//
//--------------------------------------------------------------------------

Tango::DevState DeviceImpl::state()
{
    Tango::DevState tmp;
    string last_associated_device;

    try
    {
        AutoTangoMonitor sync(this);
        cout4 << "DeviceImpl::state (attribute) arrived" << endl;

//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//

        SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
        last_associated_device = sub.get_associated_device();
        sub.set_associated_device(get_name());

//
// Record attribute request in black box
//

        blackbox_ptr->insert_corba_attr(Attr_State);

        always_executed_hook();

//
// Return data to caller. If the state_cmd throw an exception, catch it and do
// not re-throw it because we are in a CORBA attribute implementation

        tmp = dev_state();
    }
    catch (Tango::DevFailed &e)
    {
        if (last_associated_device.size() > 0)
        {
            // set back the device attribution for the thread
            (Tango::Util::instance())->get_sub_dev_diag().set_associated_device(last_associated_device);
        }

        CORBA::IMP_LIMIT lim;
        if (strcmp(e.errors[0].reason, API_CommandTimedOut) == 0)
        {
            lim.minor(TG_IMP_MINOR_TO);
        }
        else
        {
            lim.minor(TG_IMP_MINOR_DEVFAILED);
        }
        cout4 << "Leaving DeviceImpl::state (attribute) throwing IMP_LIMIT" << endl;
        throw lim;
    }
    catch (...)
    {
        if (last_associated_device.size() > 0)
        {
            // set back the device attribution for the thread
            (Tango::Util::instance())->get_sub_dev_diag().set_associated_device(last_associated_device);
        }

        CORBA::IMP_LIMIT lim;
        lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
        cout4 << "Leaving DeviceImpl::state (attribute) throwing IMP_LIMIT" << endl;
        throw lim;
    }

    if (last_associated_device.size() > 0)
    {
        // set back the device attribution for the thread
        (Tango::Util::instance())->get_sub_dev_diag().set_associated_device(last_associated_device);
    }

    cout4 << "Leaving DeviceImpl::state (attribute)" << endl;
    return tmp;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::status
//
// description :	Method called when a client request the description
//			status
//			This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::status()
{
    char *tmp;
    string last_associated_device;

    try
    {
        AutoTangoMonitor sync(this);
        cout4 << "DeviceImpl::status (attribute) arrived" << endl;

//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//

        SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
        last_associated_device = sub.get_associated_device();
        sub.set_associated_device(get_name());

//
// Record attribute request in black box
//

        blackbox_ptr->insert_corba_attr(Attr_Status);

//
// Return data to caller. If the status_cmd method throw exception, catch it
// and forget it because we are in a CORBA attribute implementation
//

        always_executed_hook();
        tmp = CORBA::string_dup(dev_status());
    }
    catch (Tango::DevFailed &e)
    {
        if (last_associated_device.size() > 0)
        {
            // set back the device attribution for the thread
            (Tango::Util::instance())->get_sub_dev_diag().set_associated_device(last_associated_device);
        }

        if (strcmp(e.errors[0].reason, API_CommandTimedOut) == 0)
        {
            tmp = CORBA::string_dup("Not able to acquire device monitor");
        }
        else
        {
            tmp = CORBA::string_dup("Got exception	when trying to build device status");
        }
    }
    catch (...)
    {
        if (last_associated_device.size() > 0)
        {
            // set back the device attribution for the thread
            (Tango::Util::instance())->get_sub_dev_diag().set_associated_device(last_associated_device);
        }

        CORBA::IMP_LIMIT lim;
        lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
        throw lim;
    }

    if (last_associated_device.size() > 0)
    {
        // set back the device attribution for the thread
        (Tango::Util::instance())->get_sub_dev_diag().set_associated_device(last_associated_device);
    }

    cout4 << "Leaving DeviceImpl::status (attribute)" << endl;
    return tmp;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::black_box
//
// description :	CORBA operation to read n element(s) of the black-box.
//			This method returns black box element as strings
//
// argument: in :	- n : Number of elt to read
//
//--------------------------------------------------------------------------


Tango::DevVarStringArray *DeviceImpl::black_box(CORBA::Long n)
{
    cout4 << "DeviceImpl::black_box arrived" << endl;

    Tango::DevVarStringArray *ret = blackbox_ptr->read((long) n);

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_BlackBox);

    cout4 << "Leaving DeviceImpl::black_box" << endl;
    return ret;
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::command_list_query
//
// description :	CORBA operation to read the device command list.
//			This method returns command info in a sequence of
//			DevCmdInfo
//
//--------------------------------------------------------------------------


Tango::DevCmdInfoList *DeviceImpl::command_list_query()
{
    cout4 << "DeviceImpl::command_list_query arrived" << endl;

//
// Retrieve number of command and allocate memory to send back info
//

    long nb_cmd = device_class->get_command_list().size();
    cout4 << nb_cmd << " command(s) for device" << endl;
    Tango::DevCmdInfoList *back = NULL;

    try
    {
        back = new Tango::DevCmdInfoList(nb_cmd);
        back->length(nb_cmd);

//
// Populate the vector
//

        for (long i = 0; i < nb_cmd; i++)
        {
            Tango::DevCmdInfo tmp;
            tmp.cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
            tmp.cmd_tag = (long) ((device_class->get_command_list())[i]->get_disp_level());
            tmp.in_type = (long) ((device_class->get_command_list())[i]->get_in_type());
            tmp.out_type = (long) ((device_class->get_command_list())[i]->get_out_type());
            string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
            if (str_in.size() != 0)
            {
                tmp.in_type_desc = CORBA::string_dup(str_in.c_str());
            }
            else
            {
                tmp.in_type_desc = CORBA::string_dup(NotSet);
            }
            string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
            if (str_out.size() != 0)
            {
                tmp.out_type_desc = CORBA::string_dup(str_out.c_str());
            }
            else
            {
                tmp.out_type_desc = CORBA::string_dup(NotSet);
            }

            (*back)[i] = tmp;
        }
    }
    catch (bad_alloc &)
    {
        Except::throw_exception((const char *) API_MemoryAllocation,
                                (const char *) "Can't allocate memory in server",
                                (const char *) "DeviceImpl::command_list_query");
    }

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_Command_list);

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::command_list_query" << endl;
    return back;
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::command_query
//
// description :	CORBA operation to read a device command info.
//			This method returns command info for a specific
//			command.
//
//
//--------------------------------------------------------------------------

Tango::DevCmdInfo *DeviceImpl::command_query(const char *command)
{
    cout4 << "DeviceImpl::command_query arrived" << endl;

    Tango::DevCmdInfo *back = NULL;
    string cmd(command);
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

    try
    {
        back = new Tango::DevCmdInfo();
    }
    catch (bad_alloc &)
    {
        Except::throw_exception((const char *) API_MemoryAllocation,
                                (const char *) "Can't allocate memory in server",
                                (const char *) "DeviceImpl::command_query");
    }

//
// Try to retrieve the command in the command list
//

    long i;
    long nb_cmd = device_class->get_command_list().size();
    for (i = 0; i < nb_cmd; i++)
    {
        if (device_class->get_command_list()[i]->get_lower_name() == cmd)
        {
            back->cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
            back->cmd_tag = (long) ((device_class->get_command_list())[i]->get_disp_level());
            back->in_type = (long) ((device_class->get_command_list())[i]->get_in_type());
            back->out_type = (long) ((device_class->get_command_list())[i]->get_out_type());
            string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
            if (str_in.size() != 0)
            {
                back->in_type_desc = CORBA::string_dup(str_in.c_str());
            }
            else
            {
                back->in_type_desc = CORBA::string_dup(NotSet);
            }
            string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
            if (str_out.size() != 0)
            {
                back->out_type_desc = CORBA::string_dup(str_out.c_str());
            }
            else
            {
                back->out_type_desc = CORBA::string_dup(NotSet);
            }
            break;
        }
    }

    if (i == nb_cmd)
    {
        delete back;
        cout3 << "DeviceImpl::command_query(): command " << command << " not found" << endl;

//
// throw an exception to client
//

        TangoSys_OMemStream o;

        o << "Command " << command << " not found" << ends;
        Except::throw_exception((const char *) API_CommandNotFound,
                                o.str(),
                                (const char *) "DeviceImpl::command_query");
    }

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_Command);

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::command_query" << endl;
    return back;
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::info
//
// description :	CORBA operation to get device info
//
//--------------------------------------------------------------------------


Tango::DevInfo *DeviceImpl::info()
{
    cout4 << "DeviceImpl::info arrived" << endl;

    Tango::DevInfo *back = NULL;

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

    try
    {
        back = new Tango::DevInfo();
    }
    catch (bad_alloc &)
    {
        Except::throw_exception((const char *) API_MemoryAllocation,
                                (const char *) "Can't allocate memory in server",
                                (const char *) "DeviceImpl::info");
    }

//
// Retrieve server host
//

    Tango::Util *tango_ptr = Tango::Util::instance();
    back->server_host = CORBA::string_dup(tango_ptr->get_host_name().c_str());

//
// Fill-in remaining structure fields
//

    back->dev_class = CORBA::string_dup(device_class->get_name().c_str());
    back->server_id = CORBA::string_dup(tango_ptr->get_ds_name().c_str());
    back->server_version = DevVersion;

//
// Build the complete info sent in the doc_url string
//

    string doc_url("Doc URL = ");
    doc_url = doc_url + device_class->get_doc_url();

//
// Add TAG if it exist
//

    string &svn_tag = device_class->get_svn_tag();
    if (svn_tag.size() != 0)
    {
        doc_url = doc_url + "\nSVN Tag = ";
        doc_url = doc_url + svn_tag;
    }
    else
    {
        string &cvs_tag = device_class->get_cvs_tag();
        if (cvs_tag.size() != 0)
        {
            doc_url = doc_url + "\nCVS Tag = ";
            doc_url = doc_url + cvs_tag;
        }
    }

//
// Add SCM location if defined
//

    string &svn_location = device_class->get_svn_location();
    if (svn_location.size() != 0)
    {
        doc_url = doc_url + "\nSVN Location = ";
        doc_url = doc_url + svn_location;
    }
    else
    {
        string &cvs_location = device_class->get_cvs_location();
        if (cvs_location.size() != 0)
        {
            doc_url = doc_url + "\nCVS Location = ";
            doc_url = doc_url + cvs_location;
        }
    }

    back->doc_url = CORBA::string_dup(doc_url.c_str());

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_Info);

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::info" << endl;
    return back;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::ping
//
// description :	CORBA operation to ping if a device to see it is alive
//
//--------------------------------------------------------------------------


void DeviceImpl::ping()
{
    cout4 << "DeviceImpl::ping arrived" << endl;

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_Ping);

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::ping" << endl;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::get_attribute_config
//
// description :	CORBA operation to get attribute configuration.
//
// argument: in :	- names: name of attribute(s)
//
// This method returns a pointer to a AttributeConfigList with one AttributeConfig
// structure for each atribute
//
//--------------------------------------------------------------------------

Tango::AttributeConfigList *DeviceImpl::get_attribute_config(const Tango::DevVarStringArray &names)
{
    cout4 << "DeviceImpl::get_attribute_config arrived" << endl;

    TangoMonitor &mon = get_att_conf_monitor();
    AutoTangoMonitor sync(&mon);

    long nb_attr = names.length();
    Tango::AttributeConfigList *back = NULL;
    bool all_attr = false;

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_Get_Attr_Config);

//
// Get attribute number and device version
//

    long nb_dev_attr = dev_attr->get_attr_nb();
    long vers = get_dev_idl_version();

//
// Check if the caller want to get config for all attribute
// If the device implements IDL 3 (State and status as attributes)
// and the client is an old one (not able to read state/status as
// attribute), decrement attribute number
//

    string in_name(names[0]);
    if (nb_attr == 1)
    {
        if (in_name == AllAttr)
        {
            all_attr = true;
            if (vers < 3)
            {
                nb_attr = nb_dev_attr;
            }
            else
            {
                nb_attr = nb_dev_attr - 2;
            }
        }
        else if (in_name == AllAttr_3)
        {
            all_attr = true;
            nb_attr = nb_dev_attr;
        }
    }

//
// Allocate memory for the AttributeConfig structures
//

    try
    {
        back = new Tango::AttributeConfigList(nb_attr);
        back->length(nb_attr);
    }
    catch (bad_alloc &)
    {
        Except::throw_exception((const char *) API_MemoryAllocation,
                                (const char *) "Can't allocate memory in server",
                                (const char *) "DeviceImpl::get_attribute_config");
    }

//
// Fill in these structures
//

    for (long i = 0; i < nb_attr; i++)
    {
        try
        {
            if (all_attr == true)
            {
                Attribute &attr = dev_attr->get_attr_by_ind(i);
                attr.get_properties((*back)[i]);
            }
            else
            {
                Attribute &attr = dev_attr->get_attr_by_name(names[i]);
                attr.get_properties((*back)[i]);
            }
        }
        catch (Tango::DevFailed &e)
        {
            delete back;
            throw;
        }
    }

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::get_attribute_config" << endl;

    return back;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::set_attribute_config
//
// description :	CORBA operation to set attribute configuration locally
//			and in the Tango database
//
// argument: in :	- new_conf: The new attribute(s) configuration. One
//				    AttributeConfig structure is needed for each
//				    attribute to update
//
//--------------------------------------------------------------------------

void DeviceImpl::set_attribute_config(const Tango::AttributeConfigList &new_conf)
{
    AutoTangoMonitor sync(this, true);
    cout4 << "DeviceImpl::set_attribute_config arrived" << endl;

//
// The attribute conf. is protected by two monitors. One protects access between
// get and set attribute conf. The second one protects access between set and
// usage. This is the classical device monitor
//

    TangoMonitor &mon1 = get_att_conf_monitor();
    AutoTangoMonitor sync1(&mon1);

//
// Record operation request in black box
//

    blackbox_ptr->insert_op(Op_Set_Attr_Config);

//
// Check if device is locked
//

    check_lock("set_attribute_config");

//
// Return exception if the device does not have any attribute
//

    long nb_dev_attr = dev_attr->get_attr_nb();
    if (nb_dev_attr == 0)
    {
        Except::throw_exception((const char *) API_AttrNotFound,
                                (const char *) "The device does not have any attribute",
                                (const char *) "DeviceImpl::set_attribute_config");
    }

//
// Get some event related data
//

    Tango::Util *tg = Tango::Util::instance();

    EventSupplier *event_supplier_nd = NULL;
    EventSupplier *event_supplier_zmq = NULL;

//
// Update attribute config first in database, then locally
// Finally send attr conf. event
//

    long nb_attr = new_conf.length();
    long i;

    try
    {
        for (i = 0; i < nb_attr; i++)
        {
            string tmp_name(new_conf[i].name);
            transform(tmp_name.begin(), tmp_name.end(), tmp_name.begin(), ::tolower);
            if ((tmp_name == "state") || (tmp_name == "status"))
            {
                Except::throw_exception((const char *) API_AttrNotFound,
                                        (const char *) "Cannot set config for attribute state or status",
                                        (const char *) "DeviceImpl::set_attribute_config");
            }

            Attribute &attr = dev_attr->get_attr_by_name(new_conf[i].name);
            bool old_alarm = attr.is_alarmed().any();
            vector<Attribute::AttPropDb> v_db;
            attr.set_properties(new_conf[i], device_name, false, v_db);
            if (Tango::Util::_UseDb == true)
            {
                attr.upd_database(v_db);
            }

//
// In case the attribute quality factor was set to ALARM, reset it to VALID
//

            if ((attr.get_quality() == Tango::ATTR_ALARM) &&
                (old_alarm == true) &&
                (attr.is_alarmed().any() == false))
            {
                attr.set_quality(Tango::ATTR_VALID);
            }

//
// Send the event
//

            if (attr.use_notifd_event() == true)
            {
                event_supplier_nd = tg->get_notifd_event_supplier();
            }
            else
            {
                event_supplier_nd = NULL;
            }

            if (attr.use_zmq_event() == true)
            {
                event_supplier_zmq = tg->get_zmq_event_supplier();
            }
            else
            {
                event_supplier_zmq = NULL;
            }

            if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL))
            {
                string tmp_name(new_conf[i].name);

                EventSupplier::SuppliedEventData ad;
                ::memset(&ad, 0, sizeof(ad));

                long vers = get_dev_idl_version();
                if (vers <= 2)
                {
                    Tango::AttributeConfig_2 attr_conf_2;
                    attr.get_properties(attr_conf_2);
                    ad.attr_conf_2 = &attr_conf_2;
                    if (event_supplier_nd != NULL)
                    {
                        event_supplier_nd->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, tmp_name);
                    }
                    if (event_supplier_zmq != NULL)
                    {
                        event_supplier_zmq->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, tmp_name);
                    }
                }
                else if (vers <= 4)
                {
                    Tango::AttributeConfig_3 attr_conf_3;
                    attr.get_properties(attr_conf_3);
                    ad.attr_conf_3 = &attr_conf_3;
                    if (event_supplier_nd != NULL)
                    {
                        event_supplier_nd->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, tmp_name);
                    }
                    if (event_supplier_zmq != NULL)
                    {
                        event_supplier_zmq->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, tmp_name);
                    }
                }
                else
                {
                    Tango::AttributeConfig_5 attr_conf_5;
                    attr.get_properties(attr_conf_5);
                    ad.attr_conf_5 = &attr_conf_5;
                    if (event_supplier_nd != NULL)
                    {
                        event_supplier_nd->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, tmp_name);
                    }
                    if (event_supplier_zmq != NULL)
                    {
                        event_supplier_zmq->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, tmp_name);
                    }
                }
            }
        }

    }
    catch (Tango::DevFailed &e)
    {

//
// Re build the list of "alarmable" attribute
//

        dev_attr->get_alarm_list().clear();
        for (long j = 0; j < nb_dev_attr; j++)
        {
            Attribute &att = dev_attr->get_attr_by_ind(j);
            if (att.is_alarmed().any() == true)
            {
                if (att.get_writable() != Tango::WRITE)
                {
                    dev_attr->get_alarm_list().push_back(j);
                }
            }
        }

//
// Change the exception reason flag
//

        TangoSys_OMemStream o;

        o << e.errors[0].reason;
        if (i != 0)
        {
            o << "\nAll previous attribute(s) have been successfully updated";
        }
        if (i != (nb_attr - 1))
        {
            o << "\nAll remaining attribute(s) have not been updated";
        }
        o << ends;

        string s = o.str();
        e.errors[0].reason = CORBA::string_dup(s.c_str());

        throw;
    }

//
// Re build the list of "alarmable" attribute
//

    dev_attr->get_alarm_list().clear();
    for (i = 0; i < nb_dev_attr; i++)
    {
        Tango::Attribute &attr = dev_attr->get_attr_by_ind(i);
        Tango::AttrWriteType w_type = attr.get_writable();
        if (attr.is_alarmed().any() == true)
        {
            if (w_type != Tango::WRITE)
            {
                dev_attr->get_alarm_list().push_back(i);
            }
        }
    }

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::set_attribute_config" << endl;
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::read_attributes
//
// description :	CORBA operation to read attribute(s) value.
//
// argument: in :	- names: name of attribute(s) to be read
//
// This method returns a pointer to a AttributeConfigList with one AttributeValue
// structure for each atribute. This structure contains the attribute value, the
// value quality and the date.
//
//--------------------------------------------------------------------------

Tango::AttributeValueList *DeviceImpl::read_attributes(const Tango::DevVarStringArray &names)
{
    AutoTangoMonitor sync(this, true);

    Tango::AttributeValueList *back = NULL;

    cout4 << "DeviceImpl::read_attributes arrived" << endl;

//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//
    SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
    string last_associated_device = sub.get_associated_device();
    sub.set_associated_device(get_name());

// Catch all execeptions to set back the associated device after
// execution
    try
    {

//
// Record operation request in black box
//

        if (store_in_bb == true)
        {
            blackbox_ptr->insert_attr(names);
        }
        store_in_bb = true;

//
// Return exception if the device does not have any attribute
// For device implementing IDL 3, substract 2 to the attributes
// number for state and status which could be read only by
// a "new" client
//

        long vers = get_dev_idl_version();
        long nb_dev_attr = dev_attr->get_attr_nb();

        if (nb_dev_attr == 0)
        {
            Except::throw_exception((const char *) API_AttrNotFound,
                                    (const char *) "The device does not have any attribute",
                                    (const char *) "DeviceImpl::read_attributes");
        }
        if (vers >= 3)
        {
            nb_dev_attr = nb_dev_attr - 2;
        }

//
// Build a sequence with the names of the attribute to be read.
// This is necessary in case of the "AllAttr" shortcut is used
// If all attributes are wanted, build this list
//

        long i;
        Tango::DevVarStringArray real_names(nb_dev_attr);
        long nb_names = names.length();
        if (nb_names == 1)
        {
            string att_name(names[0]);
            if (att_name == AllAttr)
            {
                real_names.length(nb_dev_attr);
                for (i = 0; i < nb_dev_attr; i++)
                {
                    real_names[i] = dev_attr->get_attr_by_ind(i).get_name().c_str();
                }
            }
            else
            {
                real_names = names;
            }
        }
        else
        {
            real_names = names;
        }


//
// Retrieve index of wanted attributes in the device attribute list and clear
// their value set flag
//
// In IDL release 3, possibility to write spectrum and
// images attributes have been added. This implies some
// changes in the struture returned for a read_attributes
// Throw exception if users want to use these new features
// through an old interface
//
//

        nb_names = real_names.length();
        vector<long> wanted_attr;
        vector<long> wanted_w_attr;

        for (i = 0; i < nb_names; i++)
        {
            long j = dev_attr->get_attr_ind_by_name(real_names[i]);
            if ((dev_attr->get_attr_by_ind(j).get_writable() == Tango::READ_WRITE) ||
                (dev_attr->get_attr_by_ind(j).get_writable() == Tango::READ_WITH_WRITE))
            {
                wanted_w_attr.push_back(j);
                wanted_attr.push_back(j);
                Attribute &att = dev_attr->get_attr_by_ind(wanted_attr.back());
                Tango::AttrDataFormat format_type = att.get_data_format();
                if ((format_type == Tango::SPECTRUM) || (format_type == Tango::IMAGE))
                {
                    TangoSys_OMemStream o;
                    o << "Client too old to get data for attribute " << real_names[i].in();
                    o << ".\nPlease, use a client linked with Tango V5";
                    o << " and a device inheriting from Device_3Impl" << ends;
                    Except::throw_exception((const char *) API_NotSupportedFeature,
                                            o.str(),
                                            (const char *) "DeviceImpl::read_attributes");
                }
                att.set_value_flag(false);
                att.get_when().tv_sec = 0;
            }
            else
            {
                if (dev_attr->get_attr_by_ind(j).get_writable() == Tango::WRITE)
                {
                    wanted_w_attr.push_back(j);
                    Attribute &att = dev_attr->get_attr_by_ind(wanted_w_attr.back());
                    Tango::AttrDataFormat format_type = att.get_data_format();
                    if ((format_type == Tango::SPECTRUM) || (format_type == Tango::IMAGE))
                    {
                        TangoSys_OMemStream o;
                        o << "Client too old to get data for attribute " << real_names[i].in();
                        o << ".\nPlease, use a client linked with Tango V5";
                        o << " and a device inheriting from Device_3Impl" << ends;
                        Except::throw_exception((const char *) API_NotSupportedFeature,
                                                o.str(),
                                                (const char *) "DeviceImpl::read_attributes");
                    }
                }
                else
                {
                    wanted_attr.push_back(j);
                    Attribute &att = dev_attr->get_attr_by_ind(wanted_attr.back());
                    att.set_value_flag(false);
                    att.get_when().tv_sec = 0;
                }
            }
        }
        long nb_wanted_attr = wanted_attr.size();
        long nb_wanted_w_attr = wanted_w_attr.size();

//
// Call the always_executed_hook
//

        always_executed_hook();

//
// Read the hardware for readable attribute
//

        if (nb_wanted_attr != 0)
        {
            read_attr_hardware(wanted_attr);
        }

//
// Set attr value (for readable attribute)
//

        vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();

        for (i = 0; i < nb_wanted_attr; i++)
        {
            if (vers < 3)
            {
                read_attr(dev_attr->get_attr_by_ind(wanted_attr[i]));
            }
            else
            {
                Attribute &att = dev_attr->get_attr_by_ind(wanted_attr[i]);
                long idx = att.get_attr_idx();
                if (idx == -1)
                {
                    TangoSys_OMemStream o;

                    o << "It is not possible to read state/status as attributes with your\n";
                    o << "Tango software release. Please, re-link with Tango V5." << ends;

                    Except::throw_exception((const char *) API_NotSupportedFeature,
                                            o.str(),
                                            (const char *) "Device_Impl::read_attributes");
                }

                if (attr_vect[idx]->is_allowed(this, Tango::READ_REQ) == false)
                {
                    TangoSys_OMemStream o;

                    o << "It is currently not allowed to read attribute ";
                    o << att.get_name() << ends;

                    Except::throw_exception((const char *) API_AttrNotAllowed,
                                            o.str(),
                                            (const char *) "Device_Impl::read_attributes");
                }
                attr_vect[idx]->read(this, att);
            }
        }

//
// Set attr value for writable attribute
//

        for (i = 0; i < nb_wanted_w_attr; i++)
        {
            Tango::AttrWriteType w_type = dev_attr->get_attr_by_ind(wanted_w_attr[i]).get_writable();
            if ((w_type == Tango::READ_WITH_WRITE) ||
                (w_type == Tango::WRITE))
            {
                dev_attr->get_attr_by_ind(wanted_w_attr[i]).set_rvalue();
            }
        }

//
// Allocate memory for the AttributeValue structures
//

        try
        {
            back = new Tango::AttributeValueList(nb_names);
            back->length(nb_names);
        }
        catch (bad_alloc &)
        {
            Except::throw_exception((const char *) API_MemoryAllocation,
                                    (const char *) "Can't allocate memory in server",
                                    (const char *) "DeviceImpl::read_attributes");
        }

//
// Build the sequence returned to caller for readable attributes and check
// that all the wanted attributes set value has been updated
//

        for (i = 0; i < nb_names; i++)
        {
            Attribute &att = dev_attr->get_attr_by_name(real_names[i]);
            Tango::AttrQuality qual = att.get_quality();
            if (qual != Tango::ATTR_INVALID)
            {
                if (att.get_value_flag() == false)
                {
                    TangoSys_OMemStream o;
                    delete back;
                    for (long j = 0; j < i; j++)
                    {
                        att = dev_attr->get_attr_by_name(real_names[j]);
                        att.delete_seq();
                    }

                    try
                    {
                        string att_name(real_names[i]);
                        transform(att_name.begin(), att_name.end(), att_name.begin(), ::tolower);

                        vector<PollObj *>::iterator ite = get_polled_obj_by_type_name(Tango::POLL_ATTR, att_name);
                        long upd = (*ite)->get_upd();
                        if (upd == 0)
                        {
                            o << "Attribute ";
                            o << att.get_name();
                            o << " value is available only by CACHE.\n";
                            o << "Attribute values are set by external polling buffer filling" << ends;
                        }
                        else
                        {
                            o << "Read value for attribute ";
                            o << att.get_name();
                            o << " has not been updated" << ends;
                        }
                    }
                    catch (Tango::DevFailed &)
                    {
                        o << "Read value for attribute ";
                        o << att.get_name();
                        o << " has not been updated" << ends;
                    }

                    Except::throw_exception((const char *) API_AttrValueNotSet,
                                            o.str(),
                                            (const char *) "DeviceImpl::read_attributes");
                }
                else
                {
                    Tango::AttrWriteType w_type = att.get_writable();
                    if ((w_type == Tango::READ) ||
                        (w_type == Tango::READ_WRITE) ||
                        (w_type == Tango::READ_WITH_WRITE))
                    {
                        if ((w_type == Tango::READ_WRITE) ||
                            (w_type == Tango::READ_WITH_WRITE))
                        {
                            dev_attr->add_write_value(att);
                        }

                        if ((att.is_alarmed().any() == true) && (qual != Tango::ATTR_INVALID))
                        {
                            att.check_alarm();
                        }
                    }

                    CORBA::Any &a = (*back)[i].value;
                    switch (att.get_data_type())
                    {
                        case Tango::DEV_SHORT :
                        case Tango::DEV_ENUM :
                        {
                            Tango::DevVarShortArray *ptr = att.get_short_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_LONG :
                        {
                            Tango::DevVarLongArray *ptr = att.get_long_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_LONG64 :
                        {
                            Tango::DevVarLong64Array *ptr = att.get_long64_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_DOUBLE :
                        {
                            Tango::DevVarDoubleArray *ptr = att.get_double_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_STRING :
                        {
                            Tango::DevVarStringArray *ptr = att.get_string_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_FLOAT :
                        {
                            Tango::DevVarFloatArray *ptr = att.get_float_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_BOOLEAN :
                        {
                            Tango::DevVarBooleanArray *ptr = att.get_boolean_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_USHORT :
                        {
                            Tango::DevVarUShortArray *ptr = att.get_ushort_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_UCHAR :
                        {
                            Tango::DevVarUCharArray *ptr = att.get_uchar_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_ULONG :
                        {
                            Tango::DevVarULongArray *ptr = att.get_ulong_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_ULONG64 :
                        {
                            Tango::DevVarULong64Array *ptr = att.get_ulong64_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }

                        case Tango::DEV_STATE :
                        {
                            Tango::DevVarStateArray *ptr = att.get_state_value();
                            a <<= *ptr;
                            delete ptr;
                            break;
                        }
                    }
                }
            }

            if (att.get_when().tv_sec == 0)
            {
                att.set_time();
            }

            (*back)[i].time = att.get_when();
            (*back)[i].quality = att.get_quality();
            (*back)[i].name = CORBA::string_dup(att.get_name().c_str());
            (*back)[i].dim_x = att.get_x();
            (*back)[i].dim_y = att.get_y();
        }
    }
    catch (...)
    {
        // set back the device attribution for the thread
        // and rethrow the exception.
        sub.set_associated_device(last_associated_device);
        throw;
    }

    // set back the device attribution for the thread
    sub.set_associated_device(last_associated_device);

//
// Return to caller
//

    cout4 << "Leaving DeviceImpl::read_attributes" << endl;
    return back;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::write_attributes
//
// description :	CORBA operation to write attribute(s) value
//
// argument: in :	- values: The new attribute(s) value to be set.
//
//--------------------------------------------------------------------------

void DeviceImpl::write_attributes(const Tango::AttributeValueList &values)
{
    AutoTangoMonitor sync(this, true);
    cout4 << "DeviceImpl::write_attributes arrived" << endl;


//
//  Write the device name into the per thread data for
//  sub device diagnostics.
//  Keep the old name, to put it back at the end!
//  During device access inside the same server,
//  the thread stays the same!
//
    SubDevDiag &sub = (Tango::Util::instance())->get_sub_dev_diag();
    string last_associated_device = sub.get_associated_device();
    sub.set_associated_device(get_name());

// Catch all execeptions to set back the associated device after
// execution
    try
    {

//
// Record operation request in black box
//

        blackbox_ptr->insert_attr(values);

//
// Check if the device is locked
//

        check_lock("write_attributes");

//
// Return exception if the device does not have any attribute
//

        long nb_dev_attr = dev_attr->get_attr_nb();
        if (nb_dev_attr == 0)
        {
            Except::throw_exception((const char *) API_AttrNotFound,
                                    (const char *) "The device does not have any attribute",
                                    (const char *) "DeviceImpl::write_attributes");
        }

//
// Retrieve index of wanted attributes in the device attribute list
//

        long nb_updated_attr = values.length();
        vector<long> updated_attr;

        long i;
        for (i = 0; i < nb_updated_attr; i++)
        {
            updated_attr.push_back(dev_attr->get_attr_ind_by_name(values[i].name));
        }

//
// Check that these attributes are writable
//

        for (i = 0; i < nb_updated_attr; i++)
        {
            if ((dev_attr->get_attr_by_ind(updated_attr[i]).get_writable() == Tango::READ) ||
                (dev_attr->get_attr_by_ind(updated_attr[i]).get_writable() == Tango::READ_WITH_WRITE))
            {
                TangoSys_OMemStream o;

                o << "Attribute ";
                o << dev_attr->get_attr_by_ind(updated_attr[i]).get_name();
                o << " is not writable" << ends;

                Except::throw_exception((const char *) API_AttrNotWritable,
                                        o.str(),
                                        (const char *) "DeviceImpl::write_attributes");
            }
        }

//
// Call the always_executed_hook
//

        always_executed_hook();

//
// Set attribute internal value
//

        for (i = 0; i < nb_updated_attr; i++)
        {
            try
            {
                dev_attr->get_w_attr_by_ind(updated_attr[i])
                    .check_written_value(values[i].value, (unsigned long) 1, (unsigned long) 0);
            }
            catch (Tango::DevFailed &)
            {
                for (long j = 0; j < i; j++)
                {
                    dev_attr->get_w_attr_by_ind(updated_attr[j]).rollback();
                }

                throw;
            }
        }

//
// Write the hardware
//

        long vers = get_dev_idl_version();
        if (vers < 3)
        {
            write_attr_hardware(updated_attr);
            for (i = 0; i < nb_updated_attr; i++)
            {
                WAttribute &att = dev_attr->get_w_attr_by_ind(updated_attr[i]);
                att.copy_data(values[i].value);
            }
        }
        else
        {
            vector<long> att_in_db;

            for (i = 0; i < nb_updated_attr; i++)
            {
                WAttribute &att = dev_attr->get_w_attr_by_ind(updated_attr[i]);
                vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();
                if (attr_vect[att.get_attr_idx()]->is_allowed(this, Tango::WRITE_REQ) == false)
                {
                    TangoSys_OMemStream o;

                    o << "It is currently not allowed to write attribute ";
                    o << att.get_name();
                    o << ". The device state is " << Tango::DevStateName[get_state()] << ends;


                    Except::throw_exception((const char *) API_AttrNotAllowed,
                                            o.str(),
                                            (const char *) "Device_Impl::write_attributes");
                }
                attr_vect[att.get_attr_idx()]->write(this, att);
                att.copy_data(values[i].value);
                if (att.is_memorized() == true)
                {
                    att_in_db.push_back(i);
                }
                if (att.is_alarmed().test(Attribute::rds) == true)
                {
                    att.set_written_date();
                }
            }

            if ((Tango::Util::_UseDb == true) && (att_in_db.empty() == false))
            {
                try
                {
                    static_cast<Device_3Impl *>(this)->write_attributes_in_db(att_in_db, updated_attr);
                }
                catch (Tango::DevFailed &e)
                {
                    Except::re_throw_exception(e, (const char *) API_AttrNotAllowed,
                                               (const char *) "Failed to store memorized attribute value in db",
                                               (const char *) "Device_Impl::write_attributes");
                }
            }
        }
    }
    catch (...)
    {
        // set back the device attribution for the thread
        // and rethrow the exception.
        sub.set_associated_device(last_associated_device);
        throw;
    }

    // set back the device attribution for the thread
    sub.set_associated_device(last_associated_device);

//
// Return to caller.
//

    cout4 << "Leaving DeviceImpl::write_attributes" << endl;
}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::add_attribute
//
// description :
//		Add attribute to the device attribute(s) list
//
// argument:
//		in :
//			- new_attr: The new attribute to be added.
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::add_attribute(Tango::Attr *new_attr)
{
//
// Take the device monitor in order to protect the attribute list
//

    AutoTangoMonitor sync(this, true);

    vector<Tango::Attr *> &attr_list = device_class->get_class_attr()->get_attr_list();
    long old_attr_nb = attr_list.size();

//
// Check that this attribute is not already defined for this device. If it is already there, immediately returns.
// Trick : If you add an attribute to a device, this attribute will be inserted in the device class attribute list.
// Therefore, all devices created after this attribute addition will also have this attribute.
//

    string &attr_name = new_attr->get_name();
    bool already_there = true;
    bool throw_ex = false;
    try
    {
        Tango::Attribute &al_attr = dev_attr->get_attr_by_name(attr_name.c_str());
        if ((al_attr.get_data_type() != new_attr->get_type()) ||
            (al_attr.get_data_format() != new_attr->get_format()) ||
            (al_attr.get_writable() != new_attr->get_writable()))
        {
            throw_ex = true;
        }
    }
    catch (Tango::DevFailed &)
    {
        already_there = false;
    }

//
// Throw exception if the device already have an attribute with the same name but with a different definition
//

    if (throw_ex == true)
    {
        TangoSys_OMemStream o;

        o << "Device " << get_name() << " -> Attribute " << attr_name
          << " already exists for your device but with other definition";
        o << "\n(data type, data format or data write type)" << ends;

        Except::throw_exception((const char *) API_AttrNotFound,
                                o.str(),
                                (const char *) "DeviceImpl::add_attribute");
    }

    if (already_there == true)
    {
        delete new_attr;
        return;
    }

//
// If device is IDL 5 or more and if enabled, and if there is some client(s) listening on the device interface
// change event, get device interface.
//

    ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();
    bool ev_client = event_supplier_zmq->any_dev_intr_client(this);

    if (idl_version >= MIN_IDL_DEV_INTR && is_intr_change_ev_enable() == true)
    {
        if (ev_client == true)
        {
            bool th_running;
            {
                omni_mutex_lock lo(devintr_mon);
                th_running = devintr_shared.th_running;
            }

            if (th_running == false)
            {
                devintr_shared.interface.get_interface(this);
            }
        }
    }

//
// Add this attribute in the MultiClassAttribute attr_list vector if it does not already exist
//

    bool need_free = false;
    long i;

    for (i = 0; i < old_attr_nb; i++)
    {
        if ((attr_list[i]->get_name() == attr_name) && (attr_list[i]->get_cl_name() == new_attr->get_cl_name()))
        {
            need_free = true;
            break;
        }
    }

    if (i == old_attr_nb)
    {
        attr_list.push_back(new_attr);

//
// Get all the properties defined for this attribute at class level
//

        device_class->get_class_attr()->init_class_attribute(
            device_class->get_name(),
            old_attr_nb);
    }
    else
    {

//
// An attribute with the same name is already defined within the class. Check if the data type, data format and
// write type are the same
//

        if ((attr_list[i]->get_type() != new_attr->get_type()) ||
            (attr_list[i]->get_format() != new_attr->get_format()) ||
            (attr_list[i]->get_writable() != new_attr->get_writable()))
        {
            TangoSys_OMemStream o;

            o << "Device " << get_name() << " -> Attribute " << attr_name
              << " already exists for your device class but with other definition";
            o << "\n(data type, data format or data write type)" << ends;

            Except::throw_exception((const char *) API_AttrNotFound,
                                    o.str(),
                                    (const char *) "DeviceImpl::add_attribute");
        }
    }

//
// Add the attribute to the MultiAttribute object
//

    if (new_attr->is_fwd() == true)
    {
        dev_attr->add_fwd_attribute(device_name, device_class, i, new_attr);
    }
    else
    {
        dev_attr->add_attribute(device_name, device_class, i);
    }

//
// Eventually start or update device interface change event thread
//

    push_dev_intr(ev_client);

//
// If attribute has to be polled (set by Pogo), start polling now
//

    long per = new_attr->get_polling_period();
    if ((!is_attribute_polled(attr_name)) && (per != 0))
    {
        poll_attribute(attr_name, per);
    }

//
// Free memory if needed
//

    if (need_free == true)
    {
        delete new_attr;
    }

}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::remove_attribute
//
// description :
//		Remove attribute to the device attribute(s) list
//
// argument:
//		in :
//			- rem_attr: The attribute to be deleted.
//          - free_it : Free Attr object flag
//          - clean_db : Clean attribute related info in db
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::remove_attribute(Tango::Attr *rem_attr, bool free_it, bool clean_db)
{

//
// Take the device monitor in order to protect the attribute list
//

    AutoTangoMonitor sync(this, true);

//
// Check that the class support this attribute
//

    string &attr_name = rem_attr->get_name();

    try
    {
        dev_attr->get_attr_by_name(attr_name.c_str());
    }
    catch (Tango::DevFailed &)
    {
        TangoSys_OMemStream o;

        o << "Attribute " << attr_name << " is not defined as attribute for your device.";
        o << "\nCan't remove it" << ends;

        Except::throw_exception((const char *) API_AttrNotFound,
                                o.str(),
                                (const char *) "DeviceImpl::remove_attribute");
    }

//
// If device is IDL 5 or more and if enabled, and if there is some client(s) listening on the device interface
// change event, get device interface.
//

    ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();
    bool ev_client = event_supplier_zmq->any_dev_intr_client(this);

    if (idl_version >= MIN_IDL_DEV_INTR && is_intr_change_ev_enable() == true)
    {
        if (ev_client == true)
        {
            bool th_running;
            {
                omni_mutex_lock lo(devintr_mon);
                th_running = devintr_shared.th_running;
            }

            if (th_running == false)
            {
                devintr_shared.interface.get_interface(this);
            }
        }
    }

//
// stop any configured polling for this attribute first!
//

    vector<string> &poll_attr = get_polled_attr();
    vector<string>::iterator ite_attr;

    string attr_name_low(attr_name);
    transform(attr_name_low.begin(), attr_name_low.end(), attr_name_low.begin(), ::tolower);

//
// Try to find the attribute in the list of polled attributes
//

    Tango::Util *tg = Tango::Util::instance();
    ite_attr = find(poll_attr.begin(), poll_attr.end(), attr_name_low);
    if (ite_attr != poll_attr.end())
    {
        // stop the polling and clean-up the database

        DServer *adm_dev = tg->get_dserver_device();

        DevVarStringArray send;
        send.length(3);

        send[0] = CORBA::string_dup(device_name.c_str());
        send[1] = CORBA::string_dup("attribute");
        send[2] = CORBA::string_dup(attr_name.c_str());

        if (tg->is_svr_shutting_down() == true)
        {

//
// There is no need to stop the polling because we are in the server shutdown sequence and the polling is
// already stopped.
//

            if (clean_db == true && Tango::Util::_UseDb == true)
            {

//
// Memorize the fact that the dynamic polling properties has to be removed from db.
// The classical attribute properties as well
//

                tg->get_polled_dyn_attr_names().push_back(attr_name_low);
                if (tg->get_full_polled_att_list().size() == 0)
                {
                    tg->get_full_polled_att_list() = poll_attr;
                    tg->get_dyn_att_dev_name() = device_name;
                }
            }
        }
        else
        {
            if (tg->is_device_restarting(get_name()) == false)
            {
                adm_dev->rem_obj_polling(&send, clean_db);
            }
        }
    }

//
// Now remove all configured attribute properties from the database. Do it in one go if the Db server support this
//

    if (clean_db == true)
    {
        if ((tg->is_svr_shutting_down() == false) || (tg->get_database()->get_server_release() < 400))
        {
            Tango::Attribute &att_obj = dev_attr->get_attr_by_name(attr_name.c_str());
            att_obj.remove_configuration();
        }
        else
        {
            tg->get_all_dyn_attr_names().push_back(attr_name);
            if (tg->get_dyn_att_dev_name().size() == 0)
            {
                tg->get_dyn_att_dev_name() = device_name;
            }

        }
    }

//
// Remove attribute in MultiClassAttribute in case there is only one device in the class or it is the last device
// in this class with this attribute
//

    bool update_idx = false;
    unsigned long nb_dev = device_class->get_device_list().size();

    if (nb_dev <= 1)
    {
        device_class->get_class_attr()->remove_attr(attr_name, rem_attr->get_cl_name());
        update_idx = true;
    }
    else
    {
        vector<Tango::DeviceImpl *> dev_list = device_class->get_device_list();
        unsigned long nb_except = 0;
        for (unsigned long i = 0; i < nb_dev; i++)
        {
            try
            {
                Attribute &att = dev_list[i]->get_device_attr()->get_attr_by_name(attr_name.c_str());
                vector<Tango::Attr *> &attr_list = device_class->get_class_attr()->get_attr_list();
                if (attr_list[att.get_attr_idx()]->get_cl_name() != rem_attr->get_cl_name())
                {
                    nb_except++;
                }
            }
            catch (Tango::DevFailed &)
            {
                nb_except++;
            }
        }
        if (nb_except == (nb_dev - 1))
        {
            device_class->get_class_attr()->remove_attr(attr_name, rem_attr->get_cl_name());
            update_idx = true;
        }
    }

//
// Now, remove the attribute from the MultiAttribute object
//

    dev_attr->remove_attribute(attr_name, update_idx);

//
// Delete Attr object if wanted
//

    if ((free_it == true) && (update_idx == true))
    {
        delete rem_attr;
    }

//
// Eventually start or update device interface change event thread
//

    push_dev_intr(ev_client);

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::remove_attribute
//
// description :
//		Remove attribute to the device attribute(s) list
//
// argument:
//		in :
//			- rem_attr: The name of the attribute to be deleted.
//          - free_it : Free Attr object flag
//          - clean_db : Clean attribute related info in db
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::remove_attribute(string &rem_attr_name, bool free_it, bool clean_db)
{

    try
    {
        Attr &att = device_class->get_class_attr()->get_attr(rem_attr_name);
        remove_attribute(&att, free_it, clean_db);
    }
    catch (Tango::DevFailed &e)
    {
        TangoSys_OMemStream o;

        o << "Attribute " << rem_attr_name << " is not defined as attribute for your device.";
        o << "\nCan't remove it" << ends;

        Except::re_throw_exception(e, (const char *) API_AttrNotFound,
                                   o.str(),
                                   (const char *) "DeviceImpl::remove_attribute");
    }

}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::add_command
//
// description :
//		Add command to the device command(s) list
//
// argument:
//		in :
//			- new_cmd: The new command to be added.
//			- device_level : flag set to true if the command must be added at the device level (instead of class level)
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::add_command(Tango::Command *new_cmd, bool device_level)
{
//
// Take the device monitor in order to protect the command list
//

    AutoTangoMonitor sync(this, true);

//
// Check that this command is not already defined for this device. If it is already there, immediately returns.
//

    string &cmd_name = new_cmd->get_name();
    bool already_there = true;
    bool throw_ex = false;
    try
    {
        Tango::Command &al_cmd = device_class->get_cmd_by_name(cmd_name);
        if ((al_cmd.get_in_type() != new_cmd->get_in_type()) ||
            (al_cmd.get_out_type() != new_cmd->get_out_type()))
        {
            throw_ex = true;
        }
    }
    catch (Tango::DevFailed &)
    {
        already_there = false;
    }

    if (already_there == false)
    {
        already_there = true;
        try
        {
            Tango::Command &al_cmd_dev = get_local_cmd_by_name(cmd_name);
            if ((al_cmd_dev.get_in_type() != new_cmd->get_in_type()) ||
                (al_cmd_dev.get_out_type() != new_cmd->get_out_type()))
            {
                throw_ex = true;
            }
        }
        catch (Tango::DevFailed &)
        {
            already_there = false;
        }
    }

//
// Throw exception if the device already have a command with the same name but with a different definition
//

    if (throw_ex == true)
    {
        TangoSys_OMemStream o;

        o << "Device " << get_name() << " -> Command " << cmd_name
          << " already exists for your device but with other definition";
        o << "\n(command input data type or command output data type)" << ends;

        Except::throw_exception(API_CommandNotFound, o.str(), "DeviceImpl::add_command");
    }

    if (already_there == true)
    {
        delete new_cmd;
        return;
    }

//
// If device is IDL 5 or more and if enabled, and if there is some client(s) listening on the device interface
// change event, get device interface.
//

    ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();
    bool ev_client = event_supplier_zmq->any_dev_intr_client(this);

    if (idl_version >= MIN_IDL_DEV_INTR && is_intr_change_ev_enable() == true)
    {
        if (ev_client == true)
        {
            bool th_running;
            {
                omni_mutex_lock lo(devintr_mon);
                th_running = devintr_shared.th_running;
            }

            if (th_running == false)
            {
                devintr_shared.interface.get_interface(this);
            }
        }
    }

//
// Add this command to the command list
//

    if (device_level == false)
    {
        vector<Tango::Command *> &cmd_list = device_class->get_command_list();
        cmd_list.push_back(new_cmd);
    }
    else
    {
        vector<Tango::Command *> &dev_cmd_list = get_local_command_list();
        dev_cmd_list.push_back(new_cmd);
    }

//
// Eventually start or update device interface change event thread
//

    push_dev_intr(ev_client);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::remove_command
//
// description :
//		Remove command to the device command(s) list
//
// argument:
//		in :
//			- rem_cmd: The command to be deleted.
//          - free_it : Free Command object flag
//          - clean_db : Clean command related info in db
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::remove_command(Tango::Command *rem_cmd, bool free_it, bool clean_db)
{

//
// Take the device monitor in order to protect the command list
//

    AutoTangoMonitor sync(this, true);

//
// Check that the class or the device support this command
//

    string &cmd_name = rem_cmd->get_name();
    bool device_cmd = false;

    try
    {
        device_class->get_cmd_by_name(cmd_name);
    }
    catch (Tango::DevFailed &)
    {
        try
        {
            get_local_cmd_by_name(cmd_name);
            device_cmd = true;
        }
        catch (Tango::DevFailed &)
        {
            TangoSys_OMemStream o;

            o << "Command " << cmd_name << " is not defined as command for your device.";
            o << "\nCan't remove it" << ends;

            Except::throw_exception(API_CommandNotFound, o.str(), "DeviceImpl::remove_command");
        }
    }

//
// If device is IDL 5 or more and if enabled, and if there is some client(s) listening on the device interface
// change event, get device interface.
//

    ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();
    bool ev_client = event_supplier_zmq->any_dev_intr_client(this);

    if (idl_version >= MIN_IDL_DEV_INTR && is_intr_change_ev_enable() == true)
    {
        if (ev_client == true)
        {
            bool th_running;
            {
                omni_mutex_lock lo(devintr_mon);
                th_running = devintr_shared.th_running;
            }

            if (th_running == false)
            {
                devintr_shared.interface.get_interface(this);
            }
        }
    }

//
// stop any configured polling for this command first!
//

    vector<string> &poll_cmd = get_polled_cmd();
    vector<string>::iterator ite_cmd;

    string cmd_name_low(cmd_name);
    transform(cmd_name_low.begin(), cmd_name_low.end(), cmd_name_low.begin(), ::tolower);

//
// Try to find the command in the list of polled commands
//

    Tango::Util *tg = Tango::Util::instance();
    ite_cmd = find(poll_cmd.begin(), poll_cmd.end(), cmd_name_low);
    if (ite_cmd != poll_cmd.end())
    {
        // stop the polling and clean-up the database

        DServer *adm_dev = tg->get_dserver_device();

        DevVarStringArray send;
        send.length(3);

        send[0] = CORBA::string_dup(device_name.c_str());
        send[1] = CORBA::string_dup("command");
        send[2] = CORBA::string_dup(cmd_name.c_str());

        if (tg->is_svr_shutting_down() == true)
        {

//
// There is no need to stop the polling because we are in the server shutdown sequence and the polling is
// already stopped.
//

            if (clean_db == true && Tango::Util::_UseDb == true)
            {

//
// Memorize the fact that the dynamic polling properties has to be removed from db.
//

                tg->get_polled_dyn_cmd_names().push_back(cmd_name_low);
                if (tg->get_full_polled_cmd_list().size() == 0)
                {
                    tg->get_full_polled_cmd_list() = poll_cmd;
                    tg->get_dyn_cmd_dev_name() = device_name;
                }
            }
        }
        else
        {
            if (tg->is_device_restarting(get_name()) == false)
            {
                adm_dev->rem_obj_polling(&send, clean_db);
            }
        }
    }

//
// Now, remove the command from the command list
//

    if (device_cmd == false)
    {
        device_class->remove_command(cmd_name_low);
    }
    else
    {
        remove_local_command(cmd_name_low);
    }

//
// Delete Command object if wanted
//

    if (free_it == true)
    {
        delete rem_cmd;
    }

//
// Eventually start or update device interface change event thread
//

    push_dev_intr(ev_client);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::remove_command
//
// description :
//		Remove command to the device command(s) list
//
// argument:
//		in :
//			- rem_cmd_name: The name of the command to be deleted.
//          - free_it : Free Command object flag
//          - clean_db : Clean command related info in db
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::remove_command(const string &rem_cmd_name, bool free_it, bool clean_db)
{

//
// Search for command first at class level and then at device level (for dynamic cmd)
//

    try
    {
        Command &cmd = device_class->get_cmd_by_name(rem_cmd_name);
        remove_command(&cmd, free_it, clean_db);
    }
    catch (Tango::DevFailed &e)
    {
        try
        {
            Command &cmd = get_local_cmd_by_name(rem_cmd_name);
            remove_command(&cmd, free_it, clean_db);
        }
        catch (Tango::DevFailed &e)
        {
            TangoSys_OMemStream o;

            o << "Command " << rem_cmd_name << " is not defined as a command for your device.";
            o << "\nCan't remove it" << ends;

            Except::re_throw_exception(e, API_CommandNotFound, o.str(), "DeviceImpl::remove_command");
        }
    }

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::poll_lists_2_v5
//
// description :
//		Started from Tango V5, state and status are polled as attributes. Previously, they were polled as commands.
//		If state or status are polled as commands, move them to the list of polled attributes
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::poll_lists_2_v5()
{
    bool db_update = false;

    vector<string> &poll_cmd = get_polled_cmd();
    vector<string> &poll_attr = get_polled_attr();

    vector<string>::iterator ite_state;
    vector<string>::iterator ite_status;

//
// Try to find state in list of polled command(s). If found, remove it from poll cmd and move it to poll attr
//

    ite_state = find(poll_cmd.begin(), poll_cmd.end(), "state");
    if (ite_state != poll_cmd.end())
    {
        poll_attr.push_back(*ite_state);
        poll_attr.push_back(*(ite_state + 1));
        poll_cmd.erase(ite_state, ite_state + 2);
        db_update = true;
    }

//
// The same for status
//

    ite_status = find(poll_cmd.begin(), poll_cmd.end(), "status");
    if (ite_status != poll_cmd.end())
    {
        poll_attr.push_back(*ite_status);
        poll_attr.push_back(*(ite_status + 1));
        poll_cmd.erase(ite_status, ite_status + 2);
        db_update = true;
    }

//
// Now update database if needed
//

    if (db_update == true)
    {
        DbDatum p_cmd("polled_cmd");
        DbDatum p_attr("polled_attr");

        p_cmd << poll_cmd;
        p_attr << poll_attr;

        DbData db_data;
        db_data.push_back(p_cmd);
        db_data.push_back(p_attr);

        get_db_device()->put_property(db_data);
    }

}


//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_cmd_poll_ext_trig
//
// description :
//		Write the command name to the list of polled commands in the database. The polling period is set to 0 to
//		indicate that the polling buffer is filled externally from the device server code.
//
// args :
//		in :
//			- cmd_name : The command name
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::init_cmd_poll_ext_trig(string cmd_name)
{
    string cmd_lowercase(cmd_name);
    transform(cmd_lowercase.begin(), cmd_lowercase.end(), cmd_lowercase.begin(), ::tolower);

//
// never do the for the state or status commands, they are handled as attributes!
//

    if (cmd_name == "state" || cmd_name == "status")
    {
        TangoSys_OMemStream o;

        o << "State and status are handled as attributes for the polling" << ends;
        Except::throw_exception((const char *) API_CommandNotFound,
                                o.str(),
                                (const char *) "DeviceImpl::init_poll_ext_trig");
    }

//
// check whether the command exists for the device and can be polled
//

    check_command_exists(cmd_lowercase);

//
// check wether the database is used
//

    Tango::Util *tg = Tango::Util::instance();
    if (tg->_UseDb == true)
    {
        vector<string> &poll_list = get_polled_cmd();
        Tango::DbData poll_data;
        bool found = false;

        poll_data.push_back(Tango::DbDatum("polled_cmd"));

        if (poll_list.empty() == false)
        {

//
// search the attribute in the list of polled attributes
//

            for (unsigned int i = 0; i < poll_list.size(); i = i + 2)
            {
                string name_lowercase(poll_list[i]);
                transform(name_lowercase.begin(), name_lowercase.end(), name_lowercase.begin(), ::tolower);

                if (name_lowercase == cmd_lowercase)
                {
                    poll_list[i + 1] = "0";
                    found = true;
                }
            }
        }

        if (found == false)
        {
            poll_list.push_back(cmd_lowercase);
            poll_list.push_back("0");
        }

        poll_data[0] << poll_list;
        tg->get_database()->put_device_property(device_name, poll_data);
    }
}

//+--------------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_cmd_poll_period
//
// description :
//		Checks the specified polling period for all commands of the device. If a polling period is specified for a
//		command the command name and the period are written to the list of polled commands in the database.
//		This happens only if the command is not yet in the list of polled commands.
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::init_cmd_poll_period()
{

//
// check wether the database is used
//

    Tango::Util *tg = Tango::Util::instance();
    if (tg->_UseDb == true)
    {
        vector<string> &poll_list = get_polled_cmd();
        Tango::DbData poll_data;

        poll_data.push_back(Tango::DbDatum("polled_cmd"));

//
// get the command list
//

        vector<Command *> &cmd_list = device_class->get_command_list();

//
// loop over the command list
//

        unsigned long added_cmd = 0;
        unsigned long i;
        for (i = 0; i < cmd_list.size(); i++)
        {
            long poll_period;
            poll_period = cmd_list[i]->get_polling_period();

//
// check the validity of the polling period. must be longer than min polling period
//

            if (poll_period < MIN_POLL_PERIOD)
            {
                continue;
            }

//
// never do the for the state or status commands, they are handled as attributes!
//

            string cmd_name = cmd_list[i]->get_lower_name();
            if (cmd_name == "state" || cmd_name == "status")
            {
                continue;
            }

//
// Can only handle commands without input argument
//

            if (cmd_list[i]->get_in_type() != Tango::DEV_VOID)
            {
                continue;
            }

//
// search the command in the list of polled commands
//

            bool found = false;
            for (unsigned int i = 0; i < poll_list.size(); i = i + 2)
            {
                string name_lowercase(poll_list[i]);
                transform(name_lowercase.begin(), name_lowercase.end(), name_lowercase.begin(), ::tolower);

                if (name_lowercase == cmd_name)
                {
                    found = true;
                    break;
                }
            }

            if (found == false)
            {
                string period_str;
                stringstream str;
                str << poll_period << ends;
                str >> period_str;

                poll_list.push_back(cmd_name);
                poll_list.push_back(period_str);
                added_cmd++;
            }
        }

//
// only write to the database when a polling need to be added
//

        if (added_cmd > 0)
        {
            poll_data[0] << poll_list;
            tg->get_database()->put_device_property(device_name, poll_data);
        }
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		Attribute::init_attr_poll_ext_trig
//
// description :
//		Write the attribute name to the list of polled attributes in the database. The polling period is set to 0
//		to indicate that the polling buffer is filled externally from the device server code.
//
// args :
//		in :
//			- attr_name : The attribute name
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::init_attr_poll_ext_trig(string attr_name)
{
    string attr_lowercase(attr_name);
    transform(attr_lowercase.begin(), attr_lowercase.end(), attr_lowercase.begin(), ::tolower);

//
// check whether the attribute exists for the device and can be polled
//

    dev_attr->get_attr_by_name(attr_lowercase.c_str());

//
// check wether the database is used
//

    Tango::Util *tg = Tango::Util::instance();
    if (tg->_UseDb == true)
    {
        vector<string> &poll_list = get_polled_attr();
        Tango::DbData poll_data;
        bool found = false;

        poll_data.push_back(Tango::DbDatum("polled_attr"));

//
// read the polling configuration from the database
//

        if (poll_list.empty() == false)
        {

//
// search the attribute in the list of polled attributes
//

            for (unsigned int i = 0; i < poll_list.size(); i = i + 2)
            {

//
// Convert to lower case before comparison
//

                string name_lowercase(poll_list[i]);
                transform(name_lowercase.begin(), name_lowercase.end(), name_lowercase.begin(), ::tolower);

                if (name_lowercase == attr_lowercase)
                {
                    if (poll_list[i + 1] == "0")
                    {

//
// The configuration is already correct, no need for further action
//

                        return;
                    }
                    else
                    {
                        poll_list[i + 1] = "0";
                        found = true;
                    }
                }
            }
        }

        if (found == false)
        {
            poll_list.push_back(attr_lowercase);
            poll_list.push_back("0");
        }

        poll_data[0] << poll_list;
        tg->get_database()->put_device_property(device_name, poll_data);
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::init_attr_poll_period
//
// description :
//		Checks the specified polling period for all attributes of the device. If a polling period is specified for an
//		attribute the attribute name and the period are written to the list of polled attributes in the database.
//		This happens only if the attribute is not yet in the list of polled attributes.
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::init_attr_poll_period()
{

//
// check wether the database is used
//

    Tango::Util *tg = Tango::Util::instance();
    if (tg->_UseDb == true)
    {
        vector<string> &poll_list = get_polled_attr();
        Tango::DbData poll_data;

        poll_data.push_back(Tango::DbDatum("polled_attr"));

//
// get the multi attribute object
//

        vector<Attribute *> &attr_list = dev_attr->get_attribute_list();

//
// loop over the attribute list
//

        unsigned long added_attr = 0;
        unsigned long i;
        for (i = 0; i < attr_list.size(); i++)
        {
            string &attr_name = attr_list[i]->get_name_lower();

//
// Special case for state and status attributes. They are polled as attribute but they are managed by Pogo as
// commands (historical reasons). If the polling is set in the state or status defined as command, report this info
// when they are defined as attributes
//

            if (attr_name == "state")
            {
                Command &state_cmd = device_class->get_cmd_by_name("state");
                long state_poll_period = state_cmd.get_polling_period();
                if (state_poll_period != 0)
                {
                    attr_list[i]->set_polling_period(state_poll_period);
                }
            }

            if (attr_name == "status")
            {
                Command &status_cmd = device_class->get_cmd_by_name("status");
                long status_poll_period = status_cmd.get_polling_period();
                if (status_poll_period != 0)
                {
                    attr_list[i]->set_polling_period(status_poll_period);
                }
            }

            long poll_period;
            poll_period = attr_list[i]->get_polling_period();

//
// check the validity of the polling period. must be longer than 20ms
//

            if (poll_period < MIN_POLL_PERIOD)
            {
                continue;
            }

//
// search the attribute in the list of polled attributes
//

            bool found = false;
            for (unsigned int i = 0; i < poll_list.size(); i = i + 2)
            {

//
// Convert to lower case before comparison
//

                string name_lowercase(poll_list[i]);
                transform(name_lowercase.begin(), name_lowercase.end(), name_lowercase.begin(), ::tolower);

                if (name_lowercase == attr_name)
                {
                    found = true;
                    break;
                }
            }

            if (found == false)
            {
                string period_str;
                stringstream str;
                str << poll_period << ends;
                str >> period_str;

                poll_list.push_back(attr_name);
                poll_list.push_back(period_str);
                added_attr++;
            }
        }

//
// only write to the database when a polling need to be added
//

        if (added_attr > 0)
        {
            poll_data[0] << poll_list;
            tg->get_database()->put_device_property(device_name, poll_data);
        }

//
// Another loop to correctly initialize polling period data in Attribute instance
//

        for (unsigned int i = 0; i < poll_list.size(); i = i + 2)
        {
            try
            {
                Attribute &att = dev_attr->get_attr_by_name(poll_list[i].c_str());
                stringstream ss;
                long per;
                ss << poll_list[i + 1];
                ss >> per;
                if (ss)
                {
                    att.set_polling_period(per);
                }
            }
            catch (Tango::DevFailed &)
            {}
        }
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_att_conf_event
//
// description :
//		Push an attribute configuration event
//
// args :
//		in :
//			- attr : The attribute
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_att_conf_event(Attribute *attr)
{
    EventSupplier *event_supplier_nd = NULL;
    EventSupplier *event_supplier_zmq = NULL;

    Tango::Util *tg = Tango::Util::instance();

    if (attr->use_notifd_event() == true)
    {
        event_supplier_nd = tg->get_notifd_event_supplier();
    }
    if (attr->use_zmq_event() == true)
    {
        event_supplier_zmq = tg->get_zmq_event_supplier();
    }

    if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL))
    {
        EventSupplier::SuppliedEventData ad;
        ::memset(&ad, 0, sizeof(ad));

        long vers = get_dev_idl_version();
        if (vers <= 2)
        {
            Tango::AttributeConfig_2 attr_conf_2;
            attr->get_properties(attr_conf_2);
            ad.attr_conf_2 = &attr_conf_2;
            if (event_supplier_nd != NULL)
            {
                event_supplier_nd->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, attr->get_name());
            }
            if (event_supplier_zmq != NULL)
            {
                event_supplier_zmq->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, attr->get_name());
            }
        }
        else if (vers <= 4)
        {
            Tango::AttributeConfig_3 attr_conf_3;
            attr->get_properties(attr_conf_3);
            ad.attr_conf_3 = &attr_conf_3;
            if (event_supplier_nd != NULL)
            {
                event_supplier_nd->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, attr->get_name());
            }
            if (event_supplier_zmq != NULL)
            {
                event_supplier_zmq->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, attr->get_name());
            }
        }
        else
        {
            Tango::AttributeConfig_5 attr_conf_5;
            attr->get_properties(attr_conf_5);
            ad.attr_conf_5 = &attr_conf_5;
            if (event_supplier_nd != NULL)
            {
                event_supplier_nd->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, attr->get_name());
            }
            if (event_supplier_zmq != NULL)
            {
                event_supplier_zmq->push_att_conf_events(this, ad, (Tango::DevFailed *) NULL, attr->get_name());
            }
        }
    }
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::get_client_ident()
//
// description :
//		Get client identification. This method returns a pointer to the client identification
//
//-----------------------------------------------------------------------------------------------------------------

Tango::client_addr *DeviceImpl::get_client_ident()
{
    omni_thread::value_t *ip = omni_thread::self()->get_value(key);
    return (client_addr *) ip;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::lock
//
// description :
//		Lock the device
//
// args :
//		in :
//			- cl : The client identification
//			- validity : The lock validity
//
//-----------------------------------------------------------------------------------------------------------------

void DeviceImpl::lock(client_addr *cl, int validity)
{

//
// Check if the device is already locked and if it is a valid lock. If the lock is not valid any more, clear it
//

    if (device_locked == true)
    {
        if (valid_lock() == true)
        {
            if (*cl != *(locker_client))
            {
                TangoSys_OMemStream o;
                o << "Device " << get_name() << " is already locked by another client" << ends;
                Except::throw_exception((const char *) API_DeviceLocked, o.str(),
                                        (const char *) "Device_Impl::lock");
            }
        }
        else
        {
            basic_unlock();
        }
    }

//
// Lock the device
//

    device_locked = true;
    if (locker_client == NULL)
    {
        locker_client = new client_addr(*cl);
    }

    locking_date = time(NULL);
    lock_validity = validity;
    lock_ctr++;

//
// Also lock root device(s) in case it is needed (due to forwarded attributes)
//

    if (get_with_fwd_att() == true)
    {
        lock_root_devices(validity, true);
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::relock
//
// description :
//		ReLock the device
//
// args :
//		in :
//			- cl : The client identification
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::relock(client_addr *cl)
{

//
// Check if the device is already locked and if it is a valid lock. A ReLock is valid only if the device is already
// locked by the same client and if this lock is valid
//

    if (device_locked == true)
    {
        if (valid_lock() == true)
        {
            if (*cl != *(locker_client))
            {
                TangoSys_OMemStream o;
                o << get_name() << ": ";
                o << "Device " << get_name() << " is already locked by another client" << ends;
                Except::throw_exception((const char *) API_DeviceLocked, o.str(),
                                        (const char *) "Device_Impl::relock");
            }

            device_locked = true;
            locking_date = time(NULL);
        }
        else
        {
            TangoSys_OMemStream o;
            o << get_name() << ": ";
            o << "Device " << get_name() << " is not locked. Can't re-lock it" << ends;
            Except::throw_exception((const char *) API_DeviceNotLocked, o.str(),
                                    (const char *) "Device_Impl::relock");
        }
    }
    else
    {
        TangoSys_OMemStream o;
        o << get_name() << ": ";
        o << "Device " << get_name() << " is not locked. Can't re-lock it" << ends;
        Except::throw_exception((const char *) API_DeviceNotLocked, o.str(),
                                (const char *) "Device_Impl::relock");
    }

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::unlock
//
// description :
//		Unlock the device
//
// args :
//		in :
//			- forced : Flag set to true if the unlock is forced
//
//-----------------------------------------------------------------------------------------------------------------

Tango::DevLong DeviceImpl::unlock(bool forced)
{

//
// Check if the device is already locked and if it is a valid lock. If the lock is not valid any more, clear it
//

    if (device_locked == true)
    {
        if (valid_lock() == true)
        {
            client_addr *cl = get_client_ident();

            if (forced == false)
            {
                if (*cl != *(locker_client))
                {
                    TangoSys_OMemStream o;
                    o << "Device " << get_name() << " is locked by another client, can't unlock it" << ends;
                    Except::throw_exception((const char *) API_DeviceLocked, o.str(),
                                            (const char *) "Device_Impl::unlock");
                }
            }
        }
    }

    if (lock_ctr > 0)
    {
        lock_ctr--;
    }
    if ((lock_ctr <= 0) || (forced == true))
    {
        basic_unlock(forced);
    }

    return lock_ctr;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::basic_unlock
//
// description :
//		Mark the device as unlocked
//
// args :
//		in :
//			- forced : Flag set to true if the unlock is forced
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::basic_unlock(bool forced)
{
    device_locked = false;
    if (forced == true)
    {
        old_locker_client = locker_client;
    }
    else
    {
        delete locker_client;
    }
    locker_client = NULL;
    lock_ctr = 0;

//
// Also unlock root device(s) in case it is needed (due to forwarded attributes)
//

    if (get_with_fwd_att() == true)
    {
        lock_root_devices(0, false);
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::valid_lock
//
// description :
//		Check lock validity (according to lock validity time). This method returns true if the lock is still valid.
//		Otherwise, returns false
//
//-------------------------------------------------------------------------------------------------------------------

bool DeviceImpl::valid_lock()
{
    time_t now = time(NULL);
    if (now > (locking_date + lock_validity))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::lock_status
//
// description :
//		Build a device locking status
//		This method returns a sequence with longs and strings. The strings contain:
//			1 - The locker process hostname
//			2 - The java main class (in case of Java locker)
//			3 - A string which summarizes the locking status
// 		The longs contain:
//			1 - A locked flag (0 means not locked, 1 means locked)
//			2 - The locker process PID (C++ client)
//			3 - The locker UUID (Java client) which needs 4 longs
//
//------------------------------------------------------------------------------------------------------------------

Tango::DevVarLongStringArray *DeviceImpl::lock_status()
{
    Tango::DevVarLongStringArray *dvlsa = new Tango::DevVarLongStringArray();
    dvlsa->lvalue.length(6);
    dvlsa->svalue.length(3);

//
// Check if the device is already locked and if it is a valid lock. If the lock is not valid any more, clear it
//

    if (device_locked == true)
    {
        if (valid_lock() == true)
        {
            lock_stat = "Device " + device_name + " is locked by ";
            ostringstream ostr;
            ostr << *(locker_client) << ends;
            lock_stat = lock_stat + ostr.str();

            dvlsa->lvalue[0] = 1;
            dvlsa->lvalue[1] = locker_client->client_pid;
            const char *tmp = locker_client->client_ip;
            dvlsa->svalue[1] = CORBA::string_dup(tmp);
            if (locker_client->client_lang == Tango::JAVA)
            {
                dvlsa->svalue[2] = CORBA::string_dup(locker_client->java_main_class.c_str());

                Tango::DevULong64 tmp_data = locker_client->java_ident[0];
                dvlsa->lvalue[2] = (DevLong) ((tmp_data & 0xFFFFFFFF00000000LL) >> 32);
                dvlsa->lvalue[3] = (DevLong) (tmp_data & 0xFFFFFFFF);

                tmp_data = locker_client->java_ident[1];
                dvlsa->lvalue[4] = (DevLong) ((tmp_data & 0xFFFFFFFF00000000LL) >> 32);
                dvlsa->lvalue[5] = (DevLong) (tmp_data & 0xFFFFFFFF);
            }
            else
            {
                dvlsa->svalue[2] = CORBA::string_dup("Not defined");
                for (long loop = 2; loop < 6; loop++)
                {
                    dvlsa->lvalue[loop] = 0;
                }
            }
        }
        else
        {
            basic_unlock();
            lock_stat = "Device " + device_name + " is not locked";
            dvlsa->svalue[1] = CORBA::string_dup("Not defined");
            dvlsa->svalue[2] = CORBA::string_dup("Not defined");
            for (long loop = 0; loop < 6; loop++)
            {
                dvlsa->lvalue[loop] = 0;
            }
        }
    }
    else
    {
        lock_stat = "Device " + device_name + " is not locked";
        dvlsa->svalue[1] = CORBA::string_dup("Not defined");
        dvlsa->svalue[2] = CORBA::string_dup("Not defined");
        for (long loop = 0; loop < 6; loop++)
        {
            dvlsa->lvalue[loop] = 0;
        }
    }

    dvlsa->svalue[0] = CORBA::string_dup(lock_stat.c_str());

    return dvlsa;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::set_locking_param
//
// description :
//		Restore device locking parameter
//
// args :
//		in :
//			- cl : Locker
//		  	- old_cl : Previous locker
//		  	- date : Locking date
//		  	- ctr : Locking counter
//		  	- valid : Locking validity
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::set_locking_param(client_addr *cl, client_addr *old_cl, time_t date, DevLong ctr, DevLong valid)
{
    locker_client = cl;
    old_locker_client = old_cl;
    locking_date = date;
    lock_ctr = ctr;
    device_locked = true;
    lock_validity = valid;
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::check_lock
//
// description :
//		Method called for each command_inout operation executed from any client on a Tango device.
//
// argument:
//		in :
//			- meth : Method name (for error message)
//			- cmd : Command name
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::check_lock(const char *meth, const char *cmd)
{
    if (device_locked == true)
    {
        if (valid_lock() == true)
        {
            client_addr *cl = get_client_ident();
            if (cl->client_ident == false)
            {

//
// Old client, before throwing the exception, in case the CORBA operation is a command_inout, checks if the command
// is an "allowed" one
//

                if (cmd != NULL)
                {
                    if (device_class->is_command_allowed(cmd) == false)
                    {
                        throw_locked_exception(meth);
                    }
                }
                else
                {
                    throw_locked_exception(meth);
                }
            }

            if (*cl != *(locker_client))
            {

//
// Wrong client, before throwing the exception, in case the CORBA operation is a command_inout, checks if the command
// is an "allowed" one
//

                if (cmd != NULL)
                {
                    if (device_class->is_command_allowed(cmd) == false)
                    {
                        throw_locked_exception(meth);
                    }
                }
                else
                {
                    throw_locked_exception(meth);
                }
            }
        }
        else
        {
            basic_unlock();
        }
    }
    else
    {
        client_addr *cl = get_client_ident();
        if (old_locker_client != NULL)
        {
            if (*cl == (*old_locker_client))
            {
                TangoSys_OMemStream o;
                TangoSys_OMemStream o2;
                o << "Device " << get_name() << " has been unlocked by an administrative client!!!" << ends;
                o2 << "Device_Impl::" << meth << ends;
                Except::throw_exception((const char *) DEVICE_UNLOCKED_REASON, o.str(), o2.str());
            }
            delete old_locker_client;
            old_locker_client = NULL;
        }
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::throw_locked_exception()
//
// description :
//		Throw a DeviceLocked exception
//
// argument:
//		in :
//			- meth : Method name
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::throw_locked_exception(const char *meth)
{
    TangoSys_OMemStream o;
    TangoSys_OMemStream o2;
    o << "Device " << get_name() << " is locked by another client" << ends;
    o2 << "Device_Impl::" << meth << ends;
    Except::throw_exception((const char *) API_DeviceLocked, o.str(), o2.str());
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::data_into_net_obj
//
// description :
//		Put the attribute data within the object used on the wire to transfer the attribute. For IDL release <= 3,
//		it's an Any object. Then, it is an IDL union
//
// argument:
//		in :
//			- att :
//			- aid :
//			- index :
//			- w_type :
//			- del_seq :
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::data_into_net_object(Attribute &att, AttributeIdlData &aid,
                                      long index, AttrWriteType w_type, bool del_seq)
{

//
// A big switch according to attribute data type
//

    switch (att.get_data_type())
    {
        case Tango::DEV_SHORT :
        case Tango::DEV_ENUM :
        {
            DATA_IN_OBJECT(DevVarShortArray, get_short_value, dummy_short_att_value, short_att_value);
            break;
        }

        case Tango::DEV_LONG :
        {
            DATA_IN_OBJECT(DevVarLongArray, get_long_value, dummy_long_att_value, long_att_value);
            break;
        }

        case Tango::DEV_LONG64 :
        {
            DATA_IN_OBJECT(DevVarLong64Array, get_long64_value, dummy_long64_att_value, long64_att_value);
            break;
        }

        case Tango::DEV_DOUBLE :
        {
            DATA_IN_OBJECT(DevVarDoubleArray, get_double_value, dummy_double_att_value, double_att_value);
            break;
        }

        case Tango::DEV_STRING :
        {
            DATA_IN_OBJECT(DevVarStringArray, get_string_value, dummy_string_att_value, string_att_value);
            break;
        }

        case Tango::DEV_FLOAT :
        {
            DATA_IN_OBJECT(DevVarFloatArray, get_float_value, dummy_float_att_value, float_att_value);
            break;
        }

        case Tango::DEV_BOOLEAN :
        {
            DATA_IN_OBJECT(DevVarBooleanArray, get_boolean_value, dummy_boolean_att_value, bool_att_value);
            break;
        }

        case Tango::DEV_USHORT :
        {
            DATA_IN_OBJECT(DevVarUShortArray, get_ushort_value, dummy_ushort_att_value, ushort_att_value);
            break;
        }

        case Tango::DEV_UCHAR :
        {
            DATA_IN_OBJECT(DevVarCharArray, get_uchar_value, dummy_uchar_att_value, uchar_att_value);
            break;
        }

        case Tango::DEV_ULONG :
        {
            DATA_IN_OBJECT(DevVarULongArray, get_ulong_value, dummy_ulong_att_value, ulong_att_value);
            break;
        }

        case Tango::DEV_ULONG64 :
        {
            DATA_IN_OBJECT(DevVarULong64Array, get_ulong64_value, dummy_ulong64_att_value, ulong64_att_value);
            break;
        }

        case Tango::DEV_STATE :
        {
            DATA_IN_OBJECT(DevVarStateArray, get_state_value, dummy_state_att_value, state_att_value);
            break;
        }

        case Tango::DEV_ENCODED :
        {
            if (aid.data_3 != Tango_nullptr)
            {
                (*aid.data_3)[index].err_list.length(1);
                (*aid.data_3)[index].err_list[0].severity = Tango::ERR;
                (*aid.data_3)[index].err_list[0].reason = CORBA::string_dup(API_NotSupportedFeature);
                (*aid.data_3)[index].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_no_except");
                (*aid.data_3)[index].err_list[0].desc = CORBA::string_dup(
                    "The DevEncoded data type is available only for device implementing IDL 4 and above");
                (*aid.data_3)[index].quality = Tango::ATTR_INVALID;
                (*aid.data_3)[index].name = CORBA::string_dup(att.get_name().c_str());
                clear_att_dim((*aid.data_3)[index]);
            }
            else
            {
                Tango::DevVarEncodedArray *ptr = att.get_encoded_value();
                if (aid.data_5 != Tango_nullptr)
                {
                    (*aid.data_5)[index].value.encoded_att_value(dummy_encoded_att_value);
                    DevVarEncodedArray &the_seq = (*aid.data_5)[index].value.encoded_att_value();

                    if ((w_type == Tango::READ) || (w_type == Tango::WRITE))
                    {
                        the_seq.length(1);
                    }
                    else
                    {
                        the_seq.length(2);
                    }

                    the_seq[0].encoded_format = CORBA::string_dup((*ptr)[0].encoded_format);

                    if (ptr->release() == true)
                    {
                        unsigned long nb_data = (*ptr)[0].encoded_data.length();
                        the_seq[0].encoded_data
                            .replace(nb_data, nb_data, (*ptr)[0].encoded_data.get_buffer(true), true);
                        (*ptr)[0].encoded_data.replace(0, 0, NULL, false);
                    }
                    else
                    {
                        the_seq[0].encoded_data.replace((*ptr)[0].encoded_data.length(),
                                                        (*ptr)[0].encoded_data.length(),
                                                        (*ptr)[0].encoded_data.get_buffer());
                    }

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        the_seq[1].encoded_format = CORBA::string_dup((*ptr)[1].encoded_format);
                        the_seq[1].encoded_data.replace((*ptr)[1].encoded_data.length(),
                                                        (*ptr)[1].encoded_data.length(),
                                                        (*ptr)[1].encoded_data.get_buffer());
                    }
                }
                else
                {
                    (*aid.data_4)[index].value.encoded_att_value(dummy_encoded_att_value);
                    DevVarEncodedArray &the_seq = (*aid.data_4)[index].value.encoded_att_value();

                    if ((w_type == Tango::READ) || (w_type == Tango::WRITE))
                    {
                        the_seq.length(1);
                    }
                    else
                    {
                        the_seq.length(2);
                    }

                    the_seq[0].encoded_format = CORBA::string_dup((*ptr)[0].encoded_format);

                    if (ptr->release() == true)
                    {
                        unsigned long nb_data = (*ptr)[0].encoded_data.length();
                        the_seq[0].encoded_data
                            .replace(nb_data, nb_data, (*ptr)[0].encoded_data.get_buffer(true), true);
                        (*ptr)[0].encoded_data.replace(0, 0, NULL, false);
                    }
                    else
                    {
                        the_seq[0].encoded_data.replace((*ptr)[0].encoded_data.length(),
                                                        (*ptr)[0].encoded_data.length(),
                                                        (*ptr)[0].encoded_data.get_buffer());
                    }

                    if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
                    {
                        the_seq[1].encoded_format = CORBA::string_dup((*ptr)[1].encoded_format);
                        the_seq[1].encoded_data.replace((*ptr)[1].encoded_data.length(),
                                                        (*ptr)[1].encoded_data.length(),
                                                        (*ptr)[1].encoded_data.get_buffer());
                    }
                }
                if (del_seq == true)
                {
                    delete ptr;
                }
            }
            break;
        }
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::polled_data_into_net_obj
//
// description :
//		Put the attribute data within the object used on the wire to transfer the attribute. For IDL release <= 3,
//		it's an Any object. Then, it is an IDL union
//
// argument:
//		in :
//			- aid :
//			- index :
//			- type :
//			- vers : Device IDl version
//			- polled_att :
//			- names :
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::polled_data_into_net_object(AttributeIdlData &aid,
                                             long index, long type, long vers, PollObj *polled_att,
                                             const DevVarStringArray &names)
{
    const Tango::DevVarDoubleArray *tmp_db;
    Tango::DevVarDoubleArray *new_tmp_db;
    const Tango::DevVarShortArray *tmp_sh;
    Tango::DevVarShortArray *new_tmp_sh;
    const Tango::DevVarLongArray *tmp_lg;
    Tango::DevVarLongArray *new_tmp_lg;
    const Tango::DevVarLong64Array *tmp_lg64;
    Tango::DevVarLong64Array *new_tmp_lg64;
    const Tango::DevVarStringArray *tmp_str;
    Tango::DevVarStringArray *new_tmp_str;
    const Tango::DevVarFloatArray *tmp_fl;
    Tango::DevVarFloatArray *new_tmp_fl;
    const Tango::DevVarBooleanArray *tmp_boo;
    Tango::DevVarBooleanArray *new_tmp_boo;
    const Tango::DevVarUShortArray *tmp_ush;
    Tango::DevVarUShortArray *new_tmp_ush;
    const Tango::DevVarCharArray *tmp_uch;
    Tango::DevVarCharArray *new_tmp_uch;
    const Tango::DevVarULongArray *tmp_ulg;
    Tango::DevVarULongArray *new_tmp_ulg;
    const Tango::DevVarULong64Array *tmp_ulg64;
    Tango::DevVarULong64Array *new_tmp_ulg64;
    const Tango::DevVarStateArray *tmp_state;
    Tango::DevVarStateArray *new_tmp_state;
    Tango::DevState sta;

    switch (type)
    {
        case Tango::DEV_SHORT :
        case Tango::DEV_ENUM :
            DATA_IN_NET_OBJECT(short_att_value, DevVarShortArray, short, new_tmp_sh, tmp_sh);
            break;

        case Tango::DEV_DOUBLE :
            DATA_IN_NET_OBJECT(double_att_value, DevVarDoubleArray, double, new_tmp_db, tmp_db);
            break;

        case Tango::DEV_LONG :
            DATA_IN_NET_OBJECT(long_att_value, DevVarLongArray, DevLong, new_tmp_lg, tmp_lg);
            break;

        case Tango::DEV_LONG64 :
            DATA_IN_NET_OBJECT(long64_att_value, DevVarLong64Array, DevLong64, new_tmp_lg64, tmp_lg64);
            break;

        case Tango::DEV_STRING :
            DATA_IN_NET_OBJECT(string_att_value, DevVarStringArray, char *, new_tmp_str, tmp_str);
            break;

        case Tango::DEV_FLOAT :
            DATA_IN_NET_OBJECT(float_att_value, DevVarFloatArray, float, new_tmp_fl, tmp_fl);
            break;

        case Tango::DEV_BOOLEAN :
            DATA_IN_NET_OBJECT(bool_att_value, DevVarBooleanArray, DevBoolean, new_tmp_boo, tmp_boo);
            break;

        case Tango::DEV_USHORT :
            DATA_IN_NET_OBJECT(ushort_att_value, DevVarUShortArray, DevUShort, new_tmp_ush, tmp_ush);
            break;

        case Tango::DEV_UCHAR :
            DATA_IN_NET_OBJECT(uchar_att_value, DevVarUCharArray, DevUChar, new_tmp_uch, tmp_uch);
            break;

        case Tango::DEV_ULONG :
            DATA_IN_NET_OBJECT(ulong_att_value, DevVarULongArray, DevULong, new_tmp_ulg, tmp_ulg);
            break;

        case Tango::DEV_ULONG64 :
            DATA_IN_NET_OBJECT(ulong64_att_value, DevVarULong64Array, DevULong64, new_tmp_ulg64, tmp_ulg64);
            break;

        case Tango::DEV_STATE :
            if (aid.data_5 != Tango_nullptr)
            {
                AttributeValue_5 &att_val = polled_att->get_last_attr_value_5(false);
                if (att_val.value._d() == DEVICE_STATE)
                {
                    sta = att_val.value.dev_state_att();
                    (*aid.data_5)[index].value.dev_state_att(sta);
                }
                else if (att_val.value._d() == ATT_STATE)
                {
                    DevVarStateArray &union_seq = att_val.value.state_att_value();
                    (*aid.data_5)[index].value.state_att_value(union_seq);
                }
            }
            else if (aid.data_4 != Tango_nullptr)
            {
                if (vers >= 5)
                {
                    AttributeValue_5 &att_val = polled_att->get_last_attr_value_5(false);
                    if (att_val.value._d() == DEVICE_STATE)
                    {
                        sta = att_val.value.dev_state_att();
                        (*aid.data_4)[index].value.dev_state_att(sta);
                    }
                    else if (att_val.value._d() == ATT_STATE)
                    {
                        DevVarStateArray &union_seq = att_val.value.state_att_value();
                        (*aid.data_4)[index].value.state_att_value(union_seq);
                    }
                }
                else
                {
                    AttributeValue_4 &att_val = polled_att->get_last_attr_value_4(false);
                    if (att_val.value._d() == DEVICE_STATE)
                    {
                        sta = att_val.value.dev_state_att();
                        (*aid.data_4)[index].value.dev_state_att(sta);
                    }
                    else if (att_val.value._d() == ATT_STATE)
                    {
                        DevVarStateArray &union_seq = att_val.value.state_att_value();
                        (*aid.data_4)[index].value.state_att_value(union_seq);
                    }
                }
            }
            else
            {
                if (vers >= 5)
                {
                    AttributeValue_5 &att_val = polled_att->get_last_attr_value_5(false);
                    if (att_val.value._d() == DEVICE_STATE)
                    {
                        sta = att_val.value.dev_state_att();
                        (*aid.data_3)[index].value <<= sta;
                    }
                    else if (att_val.value._d() == ATT_STATE)
                    {
                        DevVarStateArray &union_seq = att_val.value.state_att_value();
                        new_tmp_state = new DevVarStateArray(union_seq.length(),
                                                             union_seq.length(),
                                                             const_cast<DevState *>(union_seq.get_buffer()),
                                                             false);
                        (*aid.data_3)[index].value <<= new_tmp_state;
                    }
                }
                else if (vers == 4)
                {
                    AttributeValue_4 &att_val = polled_att->get_last_attr_value_4(false);
                    if (att_val.value._d() == DEVICE_STATE)
                    {
                        sta = att_val.value.dev_state_att();
                        (*aid.data_3)[index].value <<= sta;
                    }
                    else if (att_val.value._d() == ATT_STATE)
                    {
                        DevVarStateArray &union_seq = att_val.value.state_att_value();
                        new_tmp_state = new DevVarStateArray(union_seq.length(),
                                                             union_seq.length(),
                                                             const_cast<DevState *>(union_seq.get_buffer()),
                                                             false);
                        (*aid.data_3)[index].value <<= new_tmp_state;
                    }
                }
                else
                {
                    AttributeValue_3 &att_val = polled_att->get_last_attr_value_3(false);
                    CORBA::TypeCode_var ty;
                    ty = att_val.value.type();

                    if (ty->kind() == CORBA::tk_enum)
                    {
                        att_val.value >>= sta;
                        (*aid.data_3)[index].value <<= sta;
                    }
                    else
                    {
                        att_val.value >>= tmp_state;
                        new_tmp_state = new DevVarStateArray(tmp_state->length(), tmp_state->length(),
                                                             const_cast<DevState *>(tmp_state->get_buffer()), false);
                        (*aid.data_3)[index].value <<= new_tmp_state;
                    }
                }
            }
            break;

        case Tango::DEV_ENCODED:
            if (aid.data_5 != Tango_nullptr)
            {
                AttributeValue_5 &att_val = polled_att->get_last_attr_value_5(false);
                DevVarEncodedArray &polled_seq = att_val.value.encoded_att_value();

                unsigned int nb_encoded = polled_seq.length();

                (*aid.data_5)[index].value.encoded_att_value(dummy_encoded_att_value);
                DevVarEncodedArray &the_seq = (*aid.data_5)[index].value.encoded_att_value();

                the_seq.length(nb_encoded);
                for (unsigned int loop = 0; loop < nb_encoded; loop++)
                {
                    the_seq[loop].encoded_format = CORBA::string_dup(polled_seq[loop].encoded_format);
                    unsigned char *tmp_enc = polled_seq[loop].encoded_data.get_buffer();
                    unsigned int nb_data = polled_seq[loop].encoded_data.length();
                    the_seq[loop].encoded_data.replace(nb_data, nb_data, tmp_enc);
                }
            }
            else if (aid.data_4 != Tango_nullptr)
            {
                if (vers == 5)
                {
                    AttributeValue_5 &att_val = polled_att->get_last_attr_value_5(false);
                    DevVarEncodedArray &polled_seq = att_val.value.encoded_att_value();

                    unsigned int nb_encoded = polled_seq.length();

                    (*aid.data_4)[index].value.encoded_att_value(dummy_encoded_att_value);
                    DevVarEncodedArray &the_seq = (*aid.data_4)[index].value.encoded_att_value();

                    the_seq.length(nb_encoded);
                    for (unsigned int loop = 0; loop < nb_encoded; loop++)
                    {
                        the_seq[loop].encoded_format = CORBA::string_dup(polled_seq[loop].encoded_format);
                        unsigned char *tmp_enc = polled_seq[loop].encoded_data.get_buffer();
                        unsigned int nb_data = polled_seq[loop].encoded_data.length();
                        the_seq[loop].encoded_data.replace(nb_data, nb_data, tmp_enc);
                    }
                }
                else
                {
                    AttributeValue_4 &att_val = polled_att->get_last_attr_value_4(false);
                    DevVarEncodedArray &polled_seq = att_val.value.encoded_att_value();

                    unsigned int nb_encoded = polled_seq.length();

                    (*aid.data_4)[index].value.encoded_att_value(dummy_encoded_att_value);
                    DevVarEncodedArray &the_seq = (*aid.data_4)[index].value.encoded_att_value();

                    the_seq.length(nb_encoded);
                    for (unsigned int loop = 0; loop < nb_encoded; loop++)
                    {
                        the_seq[loop].encoded_format = CORBA::string_dup(polled_seq[loop].encoded_format);
                        unsigned char *tmp_enc = polled_seq[loop].encoded_data.get_buffer();
                        unsigned int nb_data = polled_seq[loop].encoded_data.length();
                        the_seq[loop].encoded_data.replace(nb_data, nb_data, tmp_enc);
                    }
                }
            }
            else
            {
                TangoSys_OMemStream o;
                o << "Data type for attribute " << names[index] << " is DEV_ENCODED.";
                o << " It's not possible to retrieve this data type through the interface you are using (IDL V3)"
                  << ends;

                (*aid.data_3)[index].err_list.length(1);
                (*aid.data_3)[index].err_list[0].severity = Tango::ERR;
                (*aid.data_3)[index].err_list[0].reason = CORBA::string_dup(API_NotSupportedFeature);
                (*aid.data_3)[index].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");

                string s = o.str();
                (*aid.data_3)[index].err_list[0].desc = CORBA::string_dup(s.c_str());
                (*aid.data_3)[index].quality = Tango::ATTR_INVALID;
                (*aid.data_3)[index].name = CORBA::string_dup(names[index]);
                clear_att_dim((*aid.data_3)[index]);
            }
            break;
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//			DeviceImpl::att_conf_loop
//
// description :
//			Set flags in DeviceImpl if any of the device attributes has some wrong configuration in DB generating
//			startup exception when the server started.
//			In DeviceImpl class, this method set the force_alarm_state flag and fills in the
//          att_wrong_db_conf vector with attribute name(s) (for device status)
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::att_conf_loop()
{
    vector<Attribute *> &att_list = get_device_attr()->get_attribute_list();

//
// Reset data before the new loop
//

    vector<string> &wrong_conf_att_list = get_att_wrong_db_conf();
    wrong_conf_att_list.clear();

    vector<string> &mem_att_list = get_att_mem_failed();
    mem_att_list.clear();

    force_alarm_state = false;

//
// Run the loop for wrong attribute conf. or memorized att which failed at startup
//

    for (size_t i = 0; i < att_list.size(); ++i)
    {
        if (att_list[i]->is_startup_exception() == true ||
            att_list[i]->is_mem_exception() == true)
        {
            force_alarm_state = true;
            if (att_list[i]->is_startup_exception() == true)
            {
                wrong_conf_att_list.push_back(att_list[i]->get_name());
            }
            else
            {
                mem_att_list.push_back(att_list[i]->get_name());
            }
        }
    }

    if (force_alarm_state == false && fwd_att_wrong_conf.empty() == false)
    {
        force_alarm_state = true;
    }

    run_att_conf_loop = false;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::check_att_conf
//
// description :
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::check_att_conf()
{
    dev_attr->check_idl_release(this);

    if (run_att_conf_loop == true)
    {
        att_conf_loop();
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::build_att_list_in_status_mess
//
// description :
//		Build device status message in case of device with some
//			- att with wrong conf. in db
//			- some memorized attribute failed during the startup phase
//			- some wrongly configured forwarded attributes
//
// argument:
//		in :
//			- nb_att : Number of attributes in error
//			- att_type : Type of attribute error (conf or mem)
//
//---------------------------------------------------------------------------------------------------------------------

void DeviceImpl::build_att_list_in_status_mess(size_t nb_att, AttErrorType att_type)
{

//
// First, the wrongly configured forwarded attributes case
//

    if (att_type == Tango::DeviceImpl::FWD)
    {
        for (size_t i = 0; i < nb_att; ++i)
        {
            alarm_status = alarm_status + "\nForwarded attribute " + fwd_att_wrong_conf[i].att_name;
            if (fwd_att_wrong_conf[i].fae != FWD_ROOT_DEV_NOT_STARTED)
            {
                alarm_status = alarm_status + " is not correctly configured! ";
            }
            else
            {
                alarm_status = alarm_status + " not reachable! ";
            }

            alarm_status = alarm_status + "Root attribute name = ";
            alarm_status = alarm_status + fwd_att_wrong_conf[i].full_root_att_name;
            if (fwd_att_wrong_conf[i].fae != FWD_ROOT_DEV_NOT_STARTED)
            {
                alarm_status = alarm_status + "\nYou can update it using the Jive tool";
            }
            alarm_status = alarm_status + "\nError: ";
            switch (fwd_att_wrong_conf[i].fae)
            {
                case FWD_WRONG_ATTR:
                    alarm_status = alarm_status + "Attribute not found in root device";
                    break;

                case FWD_CONF_LOOP:
                    alarm_status = alarm_status + "Loop found in root attributes configuration";
                    break;

                case FWD_WRONG_DEV:
                    alarm_status = alarm_status + "Wrong root device";
                    break;

                case FWD_MISSING_ROOT:
                    alarm_status = alarm_status + "Missing or wrong root attribute definition";
                    break;

                case FWD_ROOT_DEV_LOCAL_DEV:
                    alarm_status = alarm_status + "Root device is local device";
                    break;

                case FWD_WRONG_SYNTAX:
                    alarm_status = alarm_status + "Wrong syntax in root attribute definition";
                    break;

                case FWD_ROOT_DEV_NOT_STARTED:
                    alarm_status =
                        alarm_status + "Root device not started yet. Polling (if any) for this attribute not available";
                    break;

                case FWD_TOO_OLD_LOCAL_DEVICE:
                    alarm_status = alarm_status + "Local device too old (< IDL 5)";
                    break;

                case FWD_TOO_OLD_ROOT_DEVICE:
                    alarm_status = alarm_status + "Root device too old (< IDL 5)";
                    break;

                case FWD_DOUBLE_USED:
                {
                    alarm_status =
                        alarm_status + "Root attribute already used in this device server process for attribute ";
                    Util *tg = Util::instance();
                    string root_name(fwd_att_wrong_conf[i].full_root_att_name);
                    transform(root_name.begin(), root_name.end(), root_name.begin(), ::tolower);
                    string local_att_name = tg->get_root_att_reg().get_local_att_name(root_name);
                    alarm_status = alarm_status + local_att_name;
                    break;
                }

                default:
                    break;
            }
        }
    }
    else
    {

//
// For wrong conf. in db or memorized attributes which failed at startup
//

        if (nb_att > 1)
        {
            alarm_status = alarm_status + "s";
        }
        alarm_status = alarm_status + " ";
        for (size_t i = 0; i < nb_att; ++i)
        {
            if (att_type == Tango::DeviceImpl::CONF)
            {
                alarm_status = alarm_status + att_wrong_db_conf[i];
            }
            else
            {
                alarm_status = alarm_status + att_mem_failed[i];
            }

            if ((nb_att > 1) && (i <= nb_att - 2))
            {
                alarm_status = alarm_status + ", ";
            }
        }

        if (nb_att == 1)
        {
            alarm_status = alarm_status + " has ";
        }
        else
        {
            alarm_status = alarm_status + " have ";
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::is_there_subscriber
//
// description :
//		Returns true if there is some subscriber(s) listening on the event
//
// argument:
//		in :
//			- att_name : The attribute name
//			- event_type : The event type
//
//---------------------------------------------------------------------------------------------------------------------

bool DeviceImpl::is_there_subscriber(const string &att_name, EventType event_type)
{
    Attribute &att = dev_attr->get_attr_by_name(att_name.c_str());

    bool ret = false;

    switch (event_type)
    {
        case CHANGE_EVENT:
            ret = att.change_event_subscribed();
            break;

        case QUALITY_EVENT:
            ret = att.quality_event_subscribed();
            break;

        case PERIODIC_EVENT:
            ret = att.periodic_event_subscribed();
            break;

        case ARCHIVE_EVENT:
            ret = att.archive_event_subscribed();
            break;

        case USER_EVENT:
            ret = att.user_event_subscribed();
            break;

        case ATTR_CONF_EVENT:
            ret = att.attr_conf_event_subscribed();
            break;

        case DATA_READY_EVENT:
            ret = att.data_ready_event_subscribed();
            break;

        default:
            Except::throw_exception(API_UnsupportedFeature, "Unsupported event type", "Device::get_cmd_by_name");
            break;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::rem_wrong_fwd_att
//
// description :
//		Remove one forwarded attribute from the list of errored forwarded attribute
//
// argument:
//		in :
//			- root_att_name : The root attribute name to be removed
//
//---------------------------------------------------------------------------------------------------------------------

void DeviceImpl::rem_wrong_fwd_att(const string &root_att_name)
{
    vector<FwdWrongConf>::iterator ite;
    for (ite = fwd_att_wrong_conf.begin(); ite != fwd_att_wrong_conf.end(); ++ite)
    {
        string local_name(ite->full_root_att_name);
        transform(local_name.begin(), local_name.end(), local_name.begin(), ::tolower);
        if (local_name == root_att_name)
        {
            fwd_att_wrong_conf.erase(ite);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::update_wrong_fwd_att
//
// description :
//		Update error code for one forwarded attribute in the list of errored forwarded attribute
//
// argument:
//		in :
//			- root_att_name : The root attribute name to be removed
//			- err : The new error code
//
//---------------------------------------------------------------------------------------------------------------------

void DeviceImpl::update_wrong_conf_att(const string &root_att_name, FwdAttError err)
{
    vector<FwdWrongConf>::iterator ite;
    for (ite = fwd_att_wrong_conf.begin(); ite != fwd_att_wrong_conf.end(); ++ite)
    {
        string local_name(ite->full_root_att_name);
        transform(local_name.begin(), local_name.end(), local_name.begin(), ::tolower);
        if (local_name == root_att_name)
        {
            ite->fae = err;
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::lock_root_devices
//
// description :
//		Lock/Unlock all root devices for all the forwarded attributes defined for this device
//
// argument:
//		in :
//			- validity : The lock validity interval (used only in case of locking)
//			- lock_action : Flag set to true if root device(s) must be locked. If false, root devices will be unlocked
//
//---------------------------------------------------------------------------------------------------------------------

void DeviceImpl::lock_root_devices(int validity, bool lock_action)
{
//
// Get list of root device(s)
//

    vector<string> root_devs;
    vector<string>::iterator ite;
    vector<Attribute *> att_list = dev_attr->get_attribute_list();
    for (size_t j = 0; j < att_list.size(); j++)
    {
        if (att_list[j]->is_fwd_att() == true)
        {
            FwdAttribute *fwd_att = static_cast<FwdAttribute *>(att_list[j]);
            string &dev_name = fwd_att->get_fwd_dev_name();
            ite = find(root_devs.begin(), root_devs.end(), dev_name);
            if (ite == root_devs.end())
            {
                root_devs.push_back(dev_name);
            }
        }
    }

//
// Lock/Unlock all these devices
//

    RootAttRegistry &rar = Util::instance()->get_root_att_reg();
    for (size_t loop = 0; loop < root_devs.size(); loop++)
    {
        DeviceProxy *dp = rar.get_root_att_dp(root_devs[loop]);

        if (lock_action == true)
        {
            dp->lock(validity);
        }
        else
        {
            dp->unlock();
        }
    }
}

//+----------------------------------------------------------------------------
//
// method :		DeviceImpl::get_local_cmd_by_name
//
// description :	Get a reference to a local Command object
//
// in : 	cmd_name : The command name
//
//-----------------------------------------------------------------------------

Command &DeviceImpl::get_local_cmd_by_name(const string &cmd_name)
{
    vector<Command *>::iterator pos;

#ifdef HAS_LAMBDA_FUNC
    pos = find_if(command_list.begin(), command_list.end(),
                  [&](Command *cmd) -> bool
                  {
                      if (cmd_name.size() != cmd->get_lower_name().size())
                      {
                          return false;
                      }
                      string tmp_name(cmd_name);
                      transform(tmp_name.begin(), tmp_name.end(), tmp_name.begin(), ::tolower);
                      return cmd->get_lower_name() == tmp_name;
                  });
#else
                                                                                                                            pos = find_if(command_list.begin(),command_list.end(),
				bind2nd(WantedCmd<Command *,const char *,bool>(),cmd_name.c_str()));
#endif

    if (pos == command_list.end())
    {
        cout3 << "DeviceImpl::get_cmd_by_name throwing exception" << endl;
        TangoSys_OMemStream o;

        o << cmd_name << " command not found" << ends;
        Except::throw_exception(API_CommandNotFound, o.str(), "Device::get_cmd_by_name");
    }

    return *(*pos);
}

//+----------------------------------------------------------------------------
//
// method :		DeviceImpl::remove_local_command
//
// description :	Delete a command from the local command list
//
// in : 	cmd_name : The command name (in lower case letter)
//
//-----------------------------------------------------------------------------

void DeviceImpl::remove_local_command(const string &cmd_name)
{
    vector<Command *>::iterator pos;

#ifdef HAS_LAMBDA_FUNC
    pos = find_if(command_list.begin(), command_list.end(),
                  [&](Command *cmd) -> bool
                  {
                      if (cmd_name.size() != cmd->get_lower_name().size())
                      {
                          return false;
                      }
                      return cmd->get_lower_name() == cmd_name;
                  });
#else
                                                                                                                            pos = find_if(command_list.begin(),command_list.end(),
				bind2nd(WantedCmd<Command *,const char *,bool>(),cmd_name.c_str()));
#endif

    if (pos == command_list.end())
    {
        cout3 << "DeviceImpl::remove_local_command throwing exception" << endl;
        TangoSys_OMemStream o;

        o << cmd_name << " command not found" << ends;
        Except::throw_exception((const char *) API_CommandNotFound,
                                o.str(),
                                (const char *) "DeviceImpl::remove_local_command");
    }

    command_list.erase(pos);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::get_event_param
//
// description :
//		Return event info for the device with events subscribed
//
// argument :
// 		out :
//			- eve : One structure in this vector for each device event subsribed
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::get_event_param(vector<EventPar> &eve)
{
    ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

    if (event_supplier_zmq->any_dev_intr_client(this) == true)
    {
        EventPar ep;

        ep.notifd = false;
        ep.zmq = true;
        ep.attr_id = -1;
        ep.quality = false;
        ep.data_ready = false;
        ep.dev_intr_change = true;

        eve.push_back(ep);
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::set_event_param
//
// description :
//      Set device interface change event subscription time
//
// argument :
// 		in :
//			- eve : One structure in this vector for each device event subscribed
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::set_event_param(vector<EventPar> &eve)
{
    for (size_t loop = 0; loop < eve.size(); loop++)
    {
        if (eve[loop].attr_id == -1)
        {
            if (eve[loop].dev_intr_change == true)
            {
                set_event_intr_change_subscription(time(NULL));
            }
            break;
        }
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::push_dev_intr
//
// description :
//		Start or update device interface change event thread
//
// argument :
// 		in :
//			- ev_client : Flag set to true if some clients are listening on the event
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::push_dev_intr(bool ev_client)
{

//
// If device is IDL 5 or more and if enabled, push a device interface change event but only if there is some
// client(s) listening on the event.
// This is done by starting a dedicated thread (if not already started). The rule of this thread is to delayed
// the event in case of attributes/commands added/removed in a loop in order to minimize the event number.
//

    if (idl_version >= MIN_IDL_DEV_INTR && is_intr_change_ev_enable() == true && ev_client == true)
    {
        bool th_running;
        {
            omni_mutex_lock lo(devintr_mon);
            th_running = devintr_shared.th_running;
        }

        if (th_running == false)
        {
            devintr_shared.cmd_pending = false;
            devintr_thread = new DevIntrThread(devintr_shared, devintr_mon, this);
            devintr_shared.th_running = true;

            devintr_thread->start();
        }
        else
        {
            int interupted;

            omni_mutex_lock sync(devintr_mon);

            devintr_shared.cmd_pending = true;
            devintr_shared.cmd_code = DEV_INTR_SLEEP;

            devintr_mon.signal();

            cout4 << "Cmd sent to device interface change thread" << endl;

            while (devintr_shared.cmd_pending == true)
            {
                interupted = devintr_mon.wait(DEFAULT_TIMEOUT);

                if ((devintr_shared.cmd_pending == true) && (interupted == 0))
                {
                    cout4 << "TIME OUT" << endl;
                    Except::throw_exception(API_CommandTimedOut, "Device interface change event thread blocked !!!",
                                            "DeviceImpl::push_dev_intr");
                }
            }
        }
    }
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::end_pipe_config
//
// description :
//		Get all pipe properties defined at device level and aggregate all the pipe properties defined at different
//		level (device, user default, class default
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::end_pipe_config()
{
    cout4 << "Entering end_pipe_config for device " << device_name << endl;

    vector<Pipe *> &pipe_list = device_class->get_pipe_list(device_name_lower);
    size_t nb_pipe = pipe_list.size();

//
// First get device pipe configuration from db
//

    cout4 << nb_pipe << " pipe(s)" << endl;

    if (nb_pipe != 0)
    {
        Tango::Util *tg = Tango::Util::instance();
        Tango::DbData db_list;

        if (tg->_UseDb == true)
        {
            for (size_t i = 0; i < nb_pipe; i++)
            {
                db_list.push_back(DbDatum(pipe_list[i]->get_name()));
            }

//
// On some small and old computers, this request could take time if at the same time some other processes also access
// the device pipe properties table. This has been experimented at ESRF. Increase timeout to cover this case
//


            int old_db_timeout = 0;
            if (Util::_FileDb == false)
            {
                old_db_timeout = tg->get_database()->get_timeout_millis();
            }
            try
            {
                if (old_db_timeout != 0)
                {
                    tg->get_database()->set_timeout_millis(6000);
                }
                tg->get_database()->get_device_pipe_property(device_name, db_list, tg->get_db_cache());
                if (old_db_timeout != 0)
                {
                    tg->get_database()->set_timeout_millis(old_db_timeout);
                }
            }
            catch (Tango::DevFailed &)
            {
                cout4 << "Exception while accessing database" << endl;

                tg->get_database()->set_timeout_millis(old_db_timeout);
                stringstream ss;
                ss << "Can't get device pipe properties for device " << device_name << ends;

                Except::throw_exception(API_DatabaseAccess, ss.str(), "DeviceImpl::end_pipe_config");
            }

//
// A loop for each pipe
//

            long ind = 0;
            for (size_t i = 0; i < nb_pipe; i++)
            {
//
// If pipe has some properties defined at device level, build a vector of PipeProperty with them
//

                long nb_prop = 0;
                vector<PipeProperty> dev_prop;

                db_list[ind] >> nb_prop;
                ind++;

                for (long j = 0; j < nb_prop; j++)
                {
                    if (db_list[ind].size() > 1)
                    {
                        string tmp(db_list[ind].value_string[0]);
                        long nb = db_list[ind].size();
                        for (int k = 1; k < nb; k++)
                        {
                            tmp = tmp + ",";
                            tmp = tmp + db_list[ind].value_string[k];
                        }
                        dev_prop.push_back(PipeProperty(db_list[ind].name, tmp));
                    }
                    else
                    {
                        dev_prop.push_back(PipeProperty(db_list[ind].name, db_list[ind].value_string[0]));
                    }
                    ind++;
                }

                Pipe *pi_ptr = pipe_list[i];

//
// Call method which will aggregate prop definition retrieved at different levels
//

                set_pipe_prop(dev_prop, pi_ptr, LABEL);
                set_pipe_prop(dev_prop, pi_ptr, DESCRIPTION);
            }
        }
    }

    cout4 << "Leaving end_pipe_config for device " << device_name << endl;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::set_pipe_prop
//
// description :
//		Set a pipe property. A pipe property can be defined at device level in DB, by a user default or at
//		class level in DB. Properties defined in DB at device level are given to the method as the first
//		parameter. If the user has defined some user default, the pipe object already has them. The properties
//		defined at class level are available in the MultiClassPipe object available in the DeviceClass instance
//
// argument :
//		in:
//			- dev_prop : Pipe properties defined at device level
//			- pi_ptr : Pipe instance pointer
//			- ppt : Property type (label or description)
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::set_pipe_prop(vector<PipeProperty> &dev_prop, Pipe *pi_ptr, PipePropType ppt)
{
    cout4 << "Entering set_pipe_prop() method" << endl;
//
// Final init of pipe prop with following priorities:
// - Device pipe
// - User default
// - Class pipe
// The pipe instance we have here already has config set to user default (if any)
//

    bool found = false;
    string req_p_name;
    if (ppt == LABEL)
    {
        req_p_name = "label";
    }
    else
    {
        req_p_name = "description";
    }

    vector<PipeProperty>::iterator dev_ite;
    for (dev_ite = dev_prop.begin(); dev_ite != dev_prop.end(); ++dev_ite)
    {
        string p_name = dev_ite->get_name();
        transform(p_name.begin(), p_name.end(), p_name.begin(), ::tolower);

        if (p_name == req_p_name)
        {
            found = true;
            break;
        }
    }

    if (found == true)
    {
        if (ppt == LABEL)
        {
            pi_ptr->set_label(dev_ite->get_value());
        }
        else
        {
            pi_ptr->set_desc(dev_ite->get_value());
        }
    }
    else
    {

//
// Prop not defined at device level. If the prop is still the lib default one, search if it is defined at class
// level
//

        bool still_default;
        if (ppt == LABEL)
        {
            still_default = pi_ptr->is_label_lib_default();
        }
        else
        {
            still_default = pi_ptr->is_desc_lib_default();
        }

        if (still_default == true)
        {
            try
            {
                vector<PipeProperty> &cl_pi_prop = device_class->get_class_pipe()->get_prop_list(pi_ptr->get_name());

                bool found = false;
                vector<PipeProperty>::iterator class_ite;
                for (class_ite = cl_pi_prop.begin(); class_ite != cl_pi_prop.end(); ++class_ite)
                {
                    string p_name = class_ite->get_name();
                    transform(p_name.begin(), p_name.end(), p_name.begin(), ::tolower);

                    if (p_name == req_p_name)
                    {
                        found = true;
                        break;
                    }
                }

                if (found == true)
                {
                    if (ppt == LABEL)
                    {
                        pi_ptr->set_label(class_ite->get_value());
                    }
                    else
                    {
                        pi_ptr->set_desc(class_ite->get_value());
                    }
                }
            }
            catch (Tango::DevFailed &)
            {}
        }
    }

    cout4 << "Leaving set_pipe_prop() method" << endl;
}

} // End of Tango namespace

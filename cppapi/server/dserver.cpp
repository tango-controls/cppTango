//+==================================================================================================================
//
// file :               DServer.cpp
//
// description :        C++ source for the DServer class and its commands. The class is derived from Device.
//						It represents the CORBA servant object which will be accessed from the network. All commands
//						which can be executed on a DServer object are implemented in this file.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventsupplier.h>
#include <devintr.h>

#include <new>
#include <algorithm>
#include <iterator>
#include <sstream>

#ifndef _TG_WINDOWS_
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#endif /* _TG_WINDOWS_ */

#include <stdlib.h>

extern omni_thread::key_t key_py_data;
namespace Tango
{

ClassFactoryFuncPtr DServer::class_factory_func_ptr = NULL;

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::DServer(std::string &s)
//
// description :
//		constructor for DServer object
//
// args :
// 		in :
//			- cp_ptr : The class object pointer
//			- n : The device name
//			- d : The device description (default to "not initialised")
//			- s : The device state (default to UNKNOWN)
//			- st : The device status (default to "Not initialised")
//
//------------------------------------------------------------------------------------------------------------------

DServer::DServer(DeviceClass *cl_ptr,const char *n,const char *d,Tango::DevState s,const char *st)
:TANGO_BASE_CLASS(cl_ptr,n,d,s,st)
{
	process_name = Tango::Util::instance()->get_ds_exec_name();
	instance_name = Tango::Util::instance()->get_ds_inst_name();

	full_name = process_name;
	full_name.append(1,'/');
	full_name.append(instance_name);

	fqdn = "tango://";
	Tango::Util *tg = Tango::Util::instance();
	Database *db = tg->get_database();
	if (db != NULL)
		fqdn = fqdn + db->get_db_host() + ':' + db->get_db_port() + "/dserver/" + full_name;
	else
		fqdn = "dserver/" + full_name;

	last_heartbeat = time(NULL);
	last_heartbeat_zmq = last_heartbeat;
	heartbeat_started = false;

	polling_th_pool_size = DEFAULT_POLLING_THREADS_POOL_SIZE;
	optimize_pool_usage = true;
}

bool less_than (Command *a,Command *b)
{
	if (a->get_name() < b->get_name())
		return true;
	else
		return false;
}

void DServer::init_device()
{
//
// In case of database in file
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->_FileDb)
	{
		tg->reset_filedatabase();
		db_dev->set_dbase(tg->get_database());
	}

//
// Get device properties
//

	get_dev_prop(tg);

//
// Destroy already registered classes
//

	if (class_list.empty() == false)
	{
		delete_devices();
	}

	cout3 << "DServer::DSserver() create dserver " << device_name << std::endl;

	bool class_factory_done = false;
	unsigned long i = 0;
	try
	{

//
// Activate the POA manager
//

		PortableServer::POA_var poa = Util::instance()->get_poa();
		PortableServer::POAManager_var manager = poa->the_POAManager();

		PortableServer::POAManager::State man_state = manager->get_state();
		if ((man_state == PortableServer::POAManager::HOLDING) || (man_state == PortableServer::POAManager::DISCARDING))
			manager->activate();

//
// Create user TDSOM implementation
//

		if (class_factory_func_ptr == NULL)
			class_factory();
		else
			class_factory_func_ptr(this);
		class_factory_done = true;

		if (class_list.empty() == false)
		{

//
// Set the class list pointer in the Util class and add the DServer object class
//

			tg->set_class_list(&class_list);
			tg->add_class_to_list(this->get_device_class());

//
// Retrieve event related properties (multicast and other)
//

			get_event_misc_prop(tg);

//
// In case nodb is used, validate class name used in DS command line
//

			if (tg->_UseDb == false)
				tg->validate_cmd_line_classes();

//
// A loop for each class
//

			for (i = 0;i < class_list.size();i++)
			{

//
// Build class commands
//

				class_list[i]->command_factory();

//
// Sort the Command list array
//

				sort(class_list[i]->get_command_list().begin(),class_list[i]->get_command_list().end(),less_than);

//
// Build class attributes
//

				MultiClassAttribute *c_attr = class_list[i]->get_class_attr();
				class_list[i]->attribute_factory(c_attr->get_attr_list());
				c_attr->init_class_attribute(class_list[i]->get_name());

//
// Retrieve device(s) name list from the database. No need to implement a retry here (in case of db server restart)
// because the db reconnection is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

				if (tg->_UseDb == true)
				{
					Tango::Database *db = tg->get_database();
					Tango::DbDatum na;
					try
					{
						na = db->get_device_name(tg->get_ds_name(),class_list[i]->get_name(),tg->get_db_cache());
					}
					catch (Tango::DevFailed &)
					{
						TangoSys_OMemStream o;
						o << "Database error while trying to retrieve device list for class " << class_list[i]->get_name().c_str() << std::ends;

						Except::throw_exception((const char *)API_DatabaseAccess,
				                			o.str(),
				                			(const char *)"Dserver::init_device");
					}

					long nb_dev = na.size();
					Tango::DevVarStringArray dev_list(nb_dev);
					dev_list.length(nb_dev);

					for (int l = 0;l < nb_dev;l++)
						dev_list[l] = na.value_string[l].c_str();

					cout4 << dev_list.length() << " device(s) defined" << std::endl;

//
// Create all device(s) - Device creation creates device pipe(s)
//

                    class_list[i]->set_device_factory_done(false);
					{
						AutoTangoMonitor sync(class_list[i]);
						class_list[i]->device_factory(&dev_list);
					}
                    class_list[i]->set_device_factory_done(true);

//
// Set value for each device with memorized writable attr. This is necessary only if db is used
// For Python device server, writing the attribute will tak the Python lock. If we already have it --> dead lock.
// Release the python lock if we already have it before calling the set_memorized_values method
//

					PyLock *lock_ptr = NULL;
					omni_thread *th;

					if (tg->is_py_ds() == true)
					{
						th = omni_thread::self();

						omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
						lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
						lock_ptr->Release();
					}

					class_list[i]->set_memorized_values(true);

					if (tg->is_py_ds() == true)
					{
						lock_ptr->Get();
					}

//
// Check attribute configuration
//

					class_list[i]->check_att_conf();

//
// Get mcast event parameters (in case of)
//

					class_list[i]->get_mcast_event(this);

//
// Release device(s) monitor
//

					class_list[i]->release_devices_mon();
				}
				else
				{

//
// Retrieve devices name list from either the command line or from the device_name_factory method
//

					std::vector<std::string> &list = class_list[i]->get_nodb_name_list();
					Tango::DevVarStringArray *dev_list_nodb = new Tango::DevVarStringArray();

					tg->get_cmd_line_name_list(class_list[i]->get_name(),list);
					if (i == class_list.size() - 1)
					{
						tg->get_cmd_line_name_list(NoClass,list);
					}

					if (list.empty() == true)
						class_list[i]->device_name_factory(list);

					if (list.empty() == true)
					{
						dev_list_nodb->length(1);
						(*dev_list_nodb)[0] = Tango::string_dup("NoName");
					}
					else
					{
						(*dev_list_nodb) << list;
					}

//
// Create all device(s)
//

					class_list[i]->set_device_factory_done(false);
					{
						AutoTangoMonitor sync(class_list[i]);
						class_list[i]->device_factory(dev_list_nodb);
					}
					class_list[i]->set_device_factory_done(true);

					delete dev_list_nodb;
				}
			}
		}

		man_state = manager->get_state();
		if (man_state == PortableServer::POAManager::DISCARDING)
			manager->activate();

	}
	catch (std::bad_alloc &)
	{
//
// If the class_factory method have not been successfully executed, erase all classes already built. If the error
// occurs during the command or device factories, erase only the following classes
//

		TangoSys_OMemStream o;

		if (class_factory_done == false)
		{
			long class_err = class_list.size() + 1;
			o << "Can't allocate memory in server while creating class number " << class_err << std::ends;
			if (class_list.empty() == false)
			{
				for (unsigned long j = 0;j < class_list.size();j++)
				{
					class_list[j]->release_devices_mon();

					if (class_list[j]->is_py_class() == false)
						delete class_list[j];
					else
						class_list[j]->delete_class();
				}
				class_list.clear();
			}
		}
		else
		{
			o << "Can't allocate memory in server while building command(s) or device(s) for class number " << i + 1 << std::ends;
			for (unsigned long j = i;j < class_list.size();j++)
			{
				class_list[j]->release_devices_mon();

				if (class_list[j]->is_py_class() == false)
					delete class_list[j];
				else
					class_list[j]->delete_class();
			}
			class_list.erase(class_list.begin() + i,class_list.end());
		}

		Tango::Util::instance()->set_svr_shutting_down(true);
		Except::throw_exception((const char *)API_MemoryAllocation,
						o.str(),
						(const char *)"DServer::init_device");
	}
	catch (Tango::NamedDevFailedList &)
	{

//
// If the class_factory method have not been successfully executed, erase all classes already built. If the error
// occurs during the command or device factories, erase only the following classes
//

		if (class_factory_done == false)
		{
			if (class_list.empty() == false)
			{
				for (unsigned long j = 0;j < class_list.size();j++)
				{
					class_list[j]->release_devices_mon();

					if (class_list[j]->is_py_class() == false)
						delete class_list[j];
					else
						class_list[j]->delete_class();
				}
				class_list.clear();
			}
		}
		else
		{
			for (unsigned long j = i;j < class_list.size();j++)
			{
				class_list[j]->release_devices_mon();

				if (class_list[j]->is_py_class() == false)
					delete class_list[j];
				else
					class_list[j]->delete_class();
			}
			class_list.erase(class_list.begin() + i,class_list.end());
		}

		Tango::Util::instance()->set_svr_shutting_down(true);
		throw;
	}
	catch (Tango::DevFailed &)
	{
//
// If the class_factory method have not been successfully executed, erase all classes already built. If the error
// occurs during the command or device factories, erase only the following classes
//

		if (class_factory_done == false)
		{
			if (class_list.empty() == false)
			{
				for (unsigned long j = 0;j < class_list.size();j++)
				{
					if (class_list[j]->is_py_class() == false)
						delete class_list[j];
					else
					{
						class_list[j]->delete_class();
						break;
					}
				}
				class_list.clear();
			}
		}
		else
		{
			for (unsigned long j = i;j < class_list.size();j++)
			{
				if (class_list[j]->is_py_class() == false)
					delete class_list[j];
			}
			class_list.erase(class_list.begin() + i,class_list.end());
		}

		Tango::Util::instance()->set_svr_shutting_down(true);
		throw;
	}
}

void DServer::server_init_hook()
{
    for (DeviceClass *dclass : this->get_class_list())
    {
        for (DeviceImpl *device : dclass->get_device_list())
        {
            cout4 << "Device " << device->get_name_lower() << " executes init_server_hook" << std::endl;
            try
            {
                device->server_init_hook();
            }
            catch (const DevFailed &devFailed)
            {
                device->set_state(FAULT);

                std::ostringstream ss;
                ss << "Device[" << device->get_name_lower() << "] server_init_hook has failed due to DevFailed:"
                   << std::endl;
                ss << devFailed;

                device->set_status(ss.str());
            }
        }
    }
}

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::~DServer
//
// description :
//		destructor for DServer object
//
//-----------------------------------------------------------------------------------------------------------------

DServer::~DServer()
{

//
// Destroy already registered classes
//

	if (class_list.empty() == false)
	{
		for (long i = class_list.size() - 1;i >= 0;i--)
		{
			if (class_list[i]->is_py_class() == false)
				delete class_list[i];
			else
			{
				class_list[i]->delete_class();
				break;
			}
		}
		class_list.clear();
	}
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::delete_devices
//
// description :
//		Call destructor for all objects registered in the server
//
//------------------------------------------------------------------------------------------------------------------

void DServer::delete_devices()
{
	if (class_list.empty() == false)
	{
		for (long i = class_list.size() - 1;i >= 0;i--)
		{
			if (class_list[i]->is_py_class() == false)
			{
				Tango::Util *tg = Tango::Util::instance();
				PortableServer::POA_ptr r_poa = tg->get_poa();
				unsigned long loop;

				std::vector<DeviceImpl *> &devs = class_list[i]->get_device_list();
				unsigned long nb_dev = devs.size();
				for (loop = 0;loop < nb_dev;loop++)
				{

//
// Clear vectors used to memorize info used to clean db in case of devices with dyn attr removed during device
// destruction
//

					tg->get_polled_dyn_attr_names().clear();
					tg->get_full_polled_att_list().clear();
					tg->get_all_dyn_attr_names().clear();
					tg->get_dyn_att_dev_name().clear();

//
// Delete device
//

					class_list[i]->delete_dev(0,tg,r_poa);

//
// Clean-up db (dyn attribute and dyn command)
//

					if (tg->get_polled_dyn_attr_names().size() != 0)
						tg->clean_attr_polled_prop();
					if (tg->get_all_dyn_attr_names().size() != 0)
						tg->clean_dyn_attr_prop();
					if (tg->get_polled_dyn_cmd_names().size() != 0)
						tg->clean_cmd_polled_prop();

//
// Wait for POA to destroy the object before going to the next one. Limit this waiting time to 200 mS
//

					std::vector<DeviceImpl *>::iterator it = devs.begin();
					devs.erase(it);
				}
				devs.clear();
				CORBA::release(r_poa);

				delete class_list[i];

				class_list.pop_back();
			}
			else
			{
				class_list[i]->delete_class();
				break;
			}
		}
		class_list.clear();
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::add_class()
//
// description :
//		To add a new class to the class list vector
//
// args :
// 		out :
//			- class_ptr : pointer to DeviceClass object
//
//------------------------------------------------------------------------------------------------------------------

void DServer::add_class(DeviceClass *class_ptr)
{
	class_list.push_back(class_ptr);
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::query_class()
//
// description :
//		command to read all the classes used in a device server process
//
// returns :
//		The class name list in a strings sequence
//
//-----------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_class()
{
	NoSyncModelTangoMonitor sync(this);

	cout4 << "In query_class command" << std::endl;

	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret = NULL;

	try
	{
		ret = new Tango::DevVarStringArray(nb_class);
                ret->length(nb_class);

		for (int i = 0;i < nb_class;i++)
		{
			(*ret)[i] = class_list[i]->get_name().c_str();
		}
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DServer::query_class");
	}
	return(ret);

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::query_device()
//
// description :
//		command to read all the devices implemented by a device server process
//
// returns :
//		The device name list in a strings sequence
//
//------------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_device()
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In query_device command" << std::endl;

	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret = NULL;
	std::vector<std::string> vs;

	try
	{
		ret = new Tango::DevVarStringArray(DefaultMaxSeq);

		for (int i = 0;i < nb_class;i++)
		{
			long nb_dev = class_list[i]->get_device_list().size();
			std::string &class_name = class_list[i]->get_name();
			for (long j = 0;j < nb_dev;j++)
			{
				vs.push_back(class_name + "::" + (class_list[i]->get_device_list())[j]->get_name());
			}
		}
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				        (const char *)"Can't allocate memory in server",
				        (const char *)"DServer::query_device");
	}

	int nb_dev = vs.size();
	ret->length(nb_dev);
	for (int k = 0;k < nb_dev;k++)
		(*ret)[k] = Tango::string_dup(vs[k].c_str());

	return(ret);
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::query_sub_device()
//
// description :
//		command to read all the sub devices used by a device server process
//
// returns :
//		The sub device name list in a sequence of strings
//
//------------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_sub_device()
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In query_sub_device command" << std::endl;

	Tango::DevVarStringArray *ret;

	Tango::Util *tg = Tango::Util::instance();
	ret = tg->get_sub_dev_diag().get_sub_devices();

	return(ret);
}



//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::restart()
//
// description :
//		command to restart a device
//
// args :
//		in :
// 			- d_name : The device name to be re-started
//
//-----------------------------------------------------------------------------------------------------------------

void DServer::restart(std::string &d_name)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In restart method" << std::endl;

//
// Change device name to lower case letters and remove extra field in name in case of (#dbase=xxx or protocol specif)
// Check if the wanted device exists in each class
//

	bool found = false;
	unsigned int nb_class = class_list.size();
	unsigned long i;

	std::string lower_d_name(d_name);
	std::transform(lower_d_name.begin(),lower_d_name.end(),lower_d_name.begin(),::tolower);
	std::string::size_type pos;
	if ((pos = lower_d_name.find('#')) != std::string::npos)
		lower_d_name.erase(pos);
	if ((pos = lower_d_name.find("://")) != std::string::npos)
	{
		pos = pos + 3;
		if ((pos = lower_d_name.find('/',pos)) != std::string::npos)
			lower_d_name.erase(0,pos + 1);
	}

	std::vector<DeviceImpl *>::iterator ite,ite_end;
	for (i = 0;i < nb_class;i++)
	{
		std::vector<DeviceImpl *> &dev_list = class_list[i]->get_device_list();
		ite_end = dev_list.end();
		for (ite = dev_list.begin();ite != dev_list.end();++ite)
		{
			if ((*ite)->get_name_lower() == lower_d_name)
			{
				found = true;
				break;
			}
		}
		if (found == true)
			break;
	}

//
// Throw exception if the device is not found
//

	if ((i == nb_class) && (ite == ite_end))
	{
		cout3 << "Device " << d_name << " not found in server !" << std::endl;
		TangoSys_OMemStream o;
		o << "Device " << d_name << " not found" << std::ends;
		Except::throw_exception((const char *)API_DeviceNotFound,
				        o.str(),
				        (const char *)"Dserver::restart()");
	}

	DeviceImpl *dev_to_del = *ite;
	unsigned long found_class_index = i;

//
// Memorize device interface if some client(s) are listening on device interface change event
//

	DevIntr di;
	bool ev_client = false;

	if (dev_to_del->get_dev_idl_version() >= MIN_IDL_DEV_INTR)
	{
		ZmqEventSupplier *event_supplier_zmq = nullptr;
		event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

		if (event_supplier_zmq != nullptr)
			ev_client = event_supplier_zmq->any_dev_intr_client(dev_to_del);

		if (ev_client == true)
			di.get_interface(dev_to_del);
	}

//
// If the device is locked and if the client is not the lock owner, refuse to do the job
//

	check_lock_owner(dev_to_del,"restart",d_name.c_str());

//
// clean the sub-device list for this device
//

	Tango::Util *tg = Tango::Util::instance();
	tg->get_sub_dev_diag().remove_sub_devices (dev_to_del->get_name());
	tg->get_sub_dev_diag().set_associated_device(dev_to_del->get_name());

//
// Get device name, class pointer, polled object list and event parameters
//

	DeviceClass *dev_cl = dev_to_del->get_device_class();
	Tango::DevVarStringArray name(1);
	name.length(1);

	name[0] = lower_d_name.c_str();

	std::vector<PollObj *> &p_obj = dev_to_del->get_poll_obj_list();
	std::vector<Pol> dev_pol;
	EventSubscriptionStates eve;

	for (i = 0;i < p_obj.size();i++)
	{
		Pol tmp;
		tmp.type = p_obj[i]->get_type();
		tmp.upd = p_obj[i]->get_upd();
		tmp.name = p_obj[i]->get_name();
		dev_pol.push_back(tmp);
	}

	dev_to_del->get_device_attr()->get_event_param(eve);
	dev_to_del->get_event_param(eve);

//
// Also get device locker parameters if device locked
//

	client_addr *cl_addr = NULL;
	client_addr *old_cl_addr = NULL;
	time_t l_date = 0;
	DevLong l_ctr = 0,l_valid = 0;

	if (dev_to_del->is_device_locked() == true)
	{
		cl_addr = dev_to_del->get_locker();
		old_cl_addr = dev_to_del->get_old_locker();
		l_date = dev_to_del->get_locking_date();
		l_ctr = dev_to_del->get_locking_ctr();
		l_valid = dev_to_del->get_lock_validity();
		dev_to_del->clean_locker_ptrs();
	}

//
// If the device was polled, stop polling
//

	if (dev_pol.empty() == false)
	{
		dev_to_del->stop_polling(false);
	}

//
// Delete the device (deactivate it and remove it)
//

    try
    {
        tg->add_restarting_device(lower_d_name);
        PortableServer::POA_ptr r_poa = tg->get_poa();
        bool py_device = dev_to_del->is_py_device();
        if (py_device == true)
        {
            AutoPyLock PyLo;

            Device_3Impl *dev_to_del_3 = static_cast<Device_3Impl *>(dev_to_del);
            dev_to_del_3->delete_dev();
        }
        if (dev_to_del->get_exported_flag() == true)
            r_poa->deactivate_object(dev_to_del->get_obj_id().in());
        CORBA::release(r_poa);

//
// Remove ourself from device list
//

		class_list[found_class_index]->get_device_list().erase(ite);

//
// Re-create device. Take the monitor in case of class or process serialisation model
//

        dev_cl->set_device_factory_done(false);
        {
            AutoTangoMonitor sync(dev_cl);
            AutoPyLock PyLo;

            dev_cl->device_factory(&name);
        }
        dev_cl->set_device_factory_done(true);
        tg->delete_restarting_device(lower_d_name);
    }
    catch (Tango::DevFailed &e)
    {
		tg->delete_restarting_device(lower_d_name);

		std::stringstream ss;
		ss << "init_device() method for device " << d_name << " throws DevFailed exception.";
		ss << "\nDevice not available any more. Please fix exception reason";

		Tango::Except::re_throw_exception(e,API_InitThrowsException,ss.str(),"Dserver::restart()");
    }
    catch (...)
    {
        tg->delete_restarting_device(lower_d_name);

		std::stringstream ss;
		ss << "init_device() method for device " << d_name << " throws an unknown exception.";
		ss << "\nDevice not available any more. Please fix exception reason";

		Tango::Except::throw_exception(API_InitThrowsException,ss.str(),"Dserver::restart()");
    }

//
// Apply memorized values for memorized attributes (if any)
//

	dev_cl->set_memorized_values(false,dev_cl->get_device_list().size() - 1);

//
// Unlock the device (locked by export_device for memorized attribute)
//

	std::vector<DeviceImpl *> &dev_list = dev_cl->get_device_list();
	DeviceImpl *last_dev = dev_list.back();
	last_dev->get_dev_monitor().rel_monitor();

//
// Re-start device polling (if any)
//

	DevVarLongStringArray *send = new DevVarLongStringArray();
	send->lvalue.length(1);
	send->svalue.length(3);

	for (i = 0;i < dev_pol.size();i++)
	{

//
// Send command to the polling thread
//

		send->lvalue[0] = dev_pol[i].upd;
		send->svalue[0] = Tango::string_dup(name[0]);
		if (dev_pol[i].type == Tango::POLL_CMD)
			send->svalue[1] = Tango::string_dup("command");
		else
			send->svalue[1] = Tango::string_dup("attribute");
		send->svalue[2] = Tango::string_dup(dev_pol[i].name.c_str());

		try
		{
			add_obj_polling(send,false);
		}
		catch (Tango::DevFailed &e)
		{
			if (Tango::Util::_tracelevel >= 4)
				Except::print_exception(e);
		}
	}

	delete send;

//
// Find the new device
//

	DeviceImpl *new_dev = NULL;

	std::vector<Tango::DeviceImpl *> &d_list = dev_cl->get_device_list();

	for (i = 0;i < d_list.size();i++)
	{
		if (d_list[i]->get_name() == lower_d_name)
		{
			new_dev = d_list[i];
			break;
		}
	}
	if (i == d_list.size())
	{
		cout3 << "Not able to find the new device" << std::endl;
		TangoSys_OMemStream o;
		o << "Not able to find the new device" << std::ends;
		Except::throw_exception((const char *)API_DeviceNotFound,
			        	o.str(),
			        	(const char *)"Dserver::restart()");
	}

//
// Re-set classical event parameters (if needed)
//

	Tango::MultiAttribute *m_attr = new_dev->get_device_attr();
	m_attr->set_event_param(eve);
	new_dev->set_event_param(eve);

//
// Re-set multicast event parameters
//

	std::vector<std::string> m_cast;
	std::vector<Attribute *> &att_list = new_dev->get_device_attr()->get_attribute_list();

	for (unsigned int j = 0;j < att_list.size();++j)
	{
		mcast_event_for_att(new_dev->get_name_lower(),att_list[j]->get_name_lower(),m_cast);
		if (m_cast.empty() == false)
			att_list[j]->set_mcast_event(m_cast);
	}

//
// Re-set device pointer in the RootAttRegistry (if needed)
//

	RootAttRegistry &rar = tg->get_root_att_reg();
	rar.update_device_impl(lower_d_name,new_dev);

//
// Re-lock device if necessary
//

	if (cl_addr != NULL)
		new_dev->set_locking_param(cl_addr,old_cl_addr,l_date,l_ctr,l_valid);

//
// Fire device interface change event if needed
//

	if (ev_client == true)
	{
		if (di.has_changed(new_dev) == true)
		{
			cout4 << "Device interface has changed !!!!!!!!!!!!!!!!!!!" << std::endl;

			Device_5Impl *dev_5 = static_cast<Device_5Impl *>(new_dev);
			DevCmdInfoList_2 *cmds_list = dev_5->command_list_query_2();

			DevVarStringArray dvsa(1);
			dvsa.length(1);
			dvsa[0] = Tango::string_dup(AllAttr_3);
			AttributeConfigList_5 *atts_list = dev_5->get_attribute_config_5(dvsa);

			ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();
			event_supplier_zmq->push_dev_intr_change_event(new_dev,false,cmds_list,atts_list);
		}
	}

// Attribute properties may have changed after the restart.
// Push an attribute configuration event to all registered subscribers.

	for (Attribute* attr : new_dev->get_device_attr()->get_attribute_list())
	{
		new_dev->push_att_conf_event(attr);
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::restart_server()
//
// description :
//		command to restart a server (all devices embedded within the server)
//
//------------------------------------------------------------------------------------------------------------------

void DServer::restart_server()
{
	NoSyncModelTangoMonitor mon(this);

//
// Create the thread and start it
//

	ServRestartThread *t = new ServRestartThread(this);

	t->start();

}


void ServRestartThread::run(void *ptr)
{
	PyData *py_data_ptr = new PyData();
	omni_thread::self()->set_value(key_py_data,py_data_ptr);

//
// The arg. passed to this method is a pointer to the DServer device
//

	DServer *dev = (DServer *)ptr;

//
// clean the sub-device list for the server
//

	Tango::Util *tg = Tango::Util::instance();
	tg->get_sub_dev_diag().remove_sub_devices();

//
// Change the POA manager to discarding state. This is necessary to discard all request arriving while the server
// restart.
//

	PortableServer::POA_var poa = Util::instance()->get_poa();
	PortableServer::POAManager_var manager = poa->the_POAManager();

	manager->discard_requests(true);

//
// Setup logging
//

#ifdef TANGO_HAS_LOG4TANGO
  	dev->init_logger();
#endif

//
// Reset initial state and status
//

	dev->set_state(Tango::ON);
	dev->set_status("The device is ON");

//
// Memorize event parameters and devices interface
//

	ServerEventSubscriptionState map_events;
	std::map<std::string,DevIntr> map_dev_inter;

	dev->mem_event_par(map_events);
	dev->mem_devices_interface(map_dev_inter);

//
// Destroy and recreate the multi attribute object
//

	MultiAttribute *tmp_ptr;
	try
	{
		tmp_ptr = new MultiAttribute(dev->get_name(),dev->get_device_class(),dev);
	}
	catch (Tango::DevFailed &)
	{
		throw;
	}
	delete dev->get_device_attr();
	dev->set_device_attr(tmp_ptr);
	dev->add_state_status_attrs();

//
// Restart device(s)
// Before set 2 values to retrieve correct polling threads pool size
//

    tg->set_polling_threads_pool_size(ULONG_MAX);
	dev->set_poll_th_pool_size(DEFAULT_POLLING_THREADS_POOL_SIZE);

    tg->set_svr_starting(true);

	std::vector<DeviceClass *> empty_class;
	tg->set_class_list(&empty_class);

	{
		AutoPyLock PyLo;
		dev->init_device();
	}

//
// Set the class list pointer in the Util class and add the DServer object class
//

	tg->set_class_list(&(dev->get_class_list()));
	tg->add_class_to_list(dev->get_device_class());

	tg->set_svr_starting(false);

//
// Restart polling (if any)
//

	tg->polling_configure();

//
// Reset event params and send event(s) if some device interface has changed
//

	dev->apply_event_par(map_events);
	dev->changed_devices_interface(map_dev_inter);

//
// Exit thread
//

	omni_thread::self()->remove_value(key_py_data);
	delete py_data_ptr;
	omni_thread::exit();
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::query_class_prop()
//
// description :
//		command to return the list of property device at class level for the specified class
//
//-------------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_class_prop(std::string &class_name)
{
	NoSyncModelTangoMonitor sync(this);

	cout4 << "In query_class_prop command" << std::endl;

	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret = NULL;

//
// Find the wanted class in server and throw exception if not found
//

	std::transform(class_name.begin(),class_name.end(),class_name.begin(),::tolower);
	int k;
	for (k = 0;k < nb_class;k++)
	{
		std::string tmp_name(class_list[k]->get_name());
		std::transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		if (tmp_name == class_name)
			break;
	}


	if (k == nb_class)
	{
		TangoSys_OMemStream o;
		o << "Class " << class_name << " not found in device server" << std::ends;
		Except::throw_exception((const char *)API_ClassNotFound,o.str(),
				        (const char *)"DServer::query_class_prop");
	}


//
// Retrieve class prop vector and returns its content in a DevVarStringArray
//

	std::vector<std::string> &wiz = class_list[k]->get_wiz_class_prop();
	long nb_prop = wiz.size();

	try
	{
		ret = new Tango::DevVarStringArray(nb_prop);
		ret->length(nb_prop);

		for (int i = 0;i < nb_prop;i++)
		{
			(*ret)[i] = Tango::string_dup(wiz[i].c_str());
		}
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DServer::query_class_prop");
	}

	return(ret);
}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::query_dev_prop()
//
// description :
//		command to return the list of property device at device level for the specified class
//
//------------------------------------------------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_dev_prop(std::string &class_name)
{
	NoSyncModelTangoMonitor sync(this);

	cout4 << "In query_dev_prop command" << std::endl;

	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret = NULL;

//
// Find the wanted class in server and throw exception if not found
//

	std::transform(class_name.begin(),class_name.end(),class_name.begin(),::tolower);
	int k;
	for (k = 0;k < nb_class;k++)
	{
		std::string tmp_name(class_list[k]->get_name());
		std::transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		if (tmp_name == class_name)
			break;
	}


	if (k == nb_class)
	{
		TangoSys_OMemStream o;
		o << "Class " << class_name << " not found in device server" << std::ends;
		Except::throw_exception((const char *)API_ClassNotFound,o.str(),
				        (const char *)"DServer::query_dev_prop");
	}


//
// Retrieve device prop vector and returns its content in a DevVarStringArray
//

	std::vector<std::string> &wiz = class_list[k]->get_wiz_dev_prop();
	long nb_prop = wiz.size();

	try
	{
		ret = new Tango::DevVarStringArray(nb_prop);
		ret->length(nb_prop);

		for (int i = 0;i < nb_prop;i++)
		{
			(*ret)[i] = Tango::string_dup(wiz[i].c_str());
		}
	}
	catch (std::bad_alloc &)
	{
		Except::throw_exception((const char *)API_MemoryAllocation,
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DServer::query_dev_prop");
	}

	return(ret);
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::kill()
//
// description :
//		command to kill the device server process. This is done by starting a thread which will kill the process.
//		Starting a thread allows the client to receive something from the server before it is killed
//
//-----------------------------------------------------------------------------------------------------------------

void DServer::kill()
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In kill command" << std::endl;

//
// Create the thread and start it
//

	KillThread *t = new KillThread;

	t->start();

}

void *KillThread::run_undetached(TANGO_UNUSED(void *ptr))
{
	cout4 << "In the killer thread !!!" << std::endl;

	omni_thread::self()->set_value(key_py_data,new PyData());

//
// Shutdown the server
//

	Tango::Util *tg = Tango::Util::instance();
	tg->shutdown_ds();

	return NULL;
}


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::create_cpp_class()
//
// description :
//		Create a Cpp Tango class from its name
//
//------------------------------------------------------------------------------------------------------------------


void DServer::create_cpp_class(const char *cl_name,const char *par_name)
{
	cout4 << "In DServer::create_cpp_class for " << cl_name <<  ", " << par_name << std::endl;
	std::string class_name(cl_name);
	std::string lib_name = class_name;

#ifdef _TG_WINDOWS_
	HMODULE mod;

	if ((mod = LoadLibrary(lib_name.c_str())) == NULL)
	{
   		char *str = 0;

		DWORD l_err = GetLastError();
    		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			      l_err,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(char *)&str,0,NULL);

		std::cerr << "Error: " << str << std::endl;

		TangoSys_OMemStream o;
		o << "Trying to load shared library " << lib_name << " failed. It returns error: " << str << std::ends;
		::LocalFree((HLOCAL)str);

		Except::throw_exception((const char *)API_ClassNotFound,o.str(),
				        (const char *)"DServer::create_cpp_class");
	}

	cout4 << "GetModuleHandle is a success" << std::endl;

	std::string sym_name("_create_");
	sym_name = sym_name + class_name;
	sym_name = sym_name + "_class";

	cout4 << "Symbol name = " << sym_name << std::endl;
	FARPROC proc;

	if ((proc = GetProcAddress(mod,sym_name.c_str())) == NULL)
	{
		TangoSys_OMemStream o;
		o << "Class " << cl_name << " does not have the C creator function (_create_<Class name>_class)" << std::ends;

		Except::throw_exception((const char *)API_ClassNotFound,o.str(),
				        (const char *)"DServer::create_cpp_class");
	}
	cout4 << "GetProcAddress is a success" << std::endl;

	Cpp_creator_ptr mt = (Cpp_creator_ptr)proc;
#else
	void *lib_ptr;
	lib_name = lib_name + ".so";

	lib_ptr = dlopen(lib_name.c_str(),RTLD_NOW);
	if (lib_ptr == NULL)
	{
		TangoSys_OMemStream o;
		o << "Trying to load shared library " << lib_name << " failed. It returns error: " << dlerror() << std::ends;

		Except::throw_exception((const char *)API_ClassNotFound,o.str(),
				        (const char *)"DServer::create_cpp_class");
	}

	cout4 << "dlopen is a success" << std::endl;

	void *sym;

	std::string sym_name("_create_");
	sym_name = sym_name + class_name;
	sym_name = sym_name + "_class";

	cout4 << "Symbol name = " << sym_name << std::endl;

	sym = dlsym(lib_ptr,sym_name.c_str());
	if (sym == NULL)
	{
		TangoSys_OMemStream o;
		o << "Class " << cl_name << " does not have the C creator function (_create_<Class name>_class)" << std::ends;

		Except::throw_exception((const char *)API_ClassNotFound,o.str(),
				        (const char *)"DServer::create_cpp_class");
	}

	cout4 << "dlsym is a success" << std::endl;

	Cpp_creator_ptr mt = (Cpp_creator_ptr)sym;
#endif /* _TG_WINDOWS_ */
	Tango::DeviceClass *dc = (*mt)(par_name);
	add_class(dc);
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::get_dev_prop()
//
// description :
//		Retrieve device properties
//
// args :
//		in :
// 			- tg : Tango Util object ptr
//
//------------------------------------------------------------------------------------------------------------------

void DServer::get_dev_prop(Tango::Util *tg)
{
	polling_bef_9_def = false;
//
// Try to retrieve device properties (Polling threads pool conf.)
//

	if (tg->_UseDb == true)
	{
		DbData db_data;

		db_data.push_back(DbDatum("polling_threads_pool_size"));
		db_data.push_back(DbDatum("polling_threads_pool_conf"));
		db_data.push_back(DbDatum("polling_before_9"));

		try
		{
			db_dev->get_property(db_data);
		}
		catch (Tango::DevFailed &)
		{
			TangoSys_OMemStream o;
			o << "Database error while trying to retrieve device properties for device " << device_name.c_str() << std::ends;

			Except::throw_exception((const char *)API_DatabaseAccess,
					o.str(),
					(const char *)"DServer::get_dev_prop");
		}

//
// If the prop is not defined in db and if the user has defined it in the Util class, takes the user definition
//

		if (db_data[0].is_empty() == false)
			db_data[0] >> polling_th_pool_size;
		else
		{
			unsigned long p_size = tg->get_polling_threads_pool_size();
			if (p_size != ULONG_MAX)
				polling_th_pool_size = p_size;
		}

		if (db_data[1].is_empty() == false)
		{
			std::vector<std::string> tmp_vect;
			db_data[1] >> tmp_vect;

//
// If the polling threads pool conf. has been splitted due to the max device property length of 255 chars,
// rebuild a real pool conf
//

			std::string rebuilt_str;
			bool ended = true;
			std::vector<std::string>::iterator iter;
			polling_th_pool_conf.clear();

			for (iter = tmp_vect.begin();iter != tmp_vect.end();++iter)
			{
				std::string tmp_str = (*iter);
				if (tmp_str[tmp_str.size() - 1] == '\\')
				{
					tmp_str.resize(tmp_str.size() - 1);
					ended = false;
				}
				else
					ended = true;

				rebuilt_str = rebuilt_str + tmp_str;

				if (ended == true)
				{
					polling_th_pool_conf.push_back(rebuilt_str);
					rebuilt_str.clear();
				}
			}
		}
		else
			polling_th_pool_conf.clear();

//
// Polling before 9 algorithm property
//

        if (db_data[2].is_empty() == false)
        {
            polling_bef_9_def = true;
            db_data[2] >> polling_bef_9;
        }
        else
            polling_bef_9_def = false;
	}

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::check_lock_owner()
//
// description :
//		Check in case the device is locked if the client is the lock owner
//
// args :
//		in :
// 			- dev : The device
//		  	- cmd_name : The DServer device command name
//		  	- dev_name : The device name
//
//------------------------------------------------------------------------------------------------------------------

void DServer::check_lock_owner(DeviceImpl *dev,const char *cmd_name,const char *dev_name)
{
	if (dev->is_device_locked() == true)
	{
		if (dev->valid_lock() == true)
		{
			client_addr *cl = get_client_ident();
			if (cl->client_ident == true)
			{
				if (*cl != *(dev->get_locker()))
				{
					TangoSys_OMemStream o,v;
					o << "Device " << dev_name << " is locked by another client.";
					o << " Your request is not allowed while a device is locked." << std::ends;
					v << "DServer::" << cmd_name << std::ends;
					Except::throw_exception((const char *)API_DeviceLocked,o.str(),v.str());
				}
			}
			else
			{
				TangoSys_OMemStream o,v;
				o << "Device " << dev_name << " is locked by another client.";
				o << " Your request is not allowed while a device is locked." << std::ends;
				v << "DServer::" << cmd_name << std::ends;
				Except::throw_exception((const char *)API_DeviceLocked,o.str(),v.str());
			}
		}
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::get_event_misc_prop()
//
// description :
//		Get the properties defining which event has to be transported using multicast protocol. Also retrieve
//		property for Nan allowed in writing attribute
//
// args :
// 		in :
// 			- tg : Tango Util instance pointer
//
//------------------------------------------------------------------------------------------------------------------

void DServer::get_event_misc_prop(Tango::Util *tg)
{

	if (tg->_UseDb == true)
	{

//
// Get property
//

		DbData db_data;

		db_data.push_back(DbDatum("MulticastEvent"));
		db_data.push_back(DbDatum("MulticastHops"));
		db_data.push_back(DbDatum("MulticastRate"));
		db_data.push_back(DbDatum("MulticastIvl"));
		db_data.push_back(DbDatum("DSEventBufferHwm"));
		db_data.push_back(DbDatum("EventBufferHwm"));
		db_data.push_back(DbDatum("WAttrNaNAllowed"));

		try
		{
			tg->get_database()->get_property(CONTROL_SYSTEM,db_data,tg->get_db_cache());

			mcast_event_prop.clear();
			db_data[0] >> mcast_event_prop;

//
// Check property coherency
//

			std::vector<std::string>::size_type nb_elt = mcast_event_prop.size();
			bool uncoherent  = false;

			for (unsigned int i = 0;i < nb_elt;++i)
			{
				if (is_ip_address(mcast_event_prop[i]) == true)
				{

//
// Check multicast address validity
//

                    std::string start_ip_mcast = mcast_event_prop[i].substr(0,mcast_event_prop[i].find('.'));

                    std::istringstream ss(start_ip_mcast);
                    int ip_start;
                    ss >> ip_start;

                    if ((ip_start < 224) || (ip_start > 239))
                        uncoherent = true;
                    else
                    {

//
// Check property definition
//

                        if (nb_elt < i + 3)
                            uncoherent = true;
                        else
                        {
                            if (nb_elt > i + 4)
                            {
                                if ((is_event_name(mcast_event_prop[i + 2]) == false) &&
                                    (is_event_name(mcast_event_prop[i + 3]) == false) &&
                                    (is_event_name(mcast_event_prop[i + 4]) == false))
                                    uncoherent = true;
                            }
                            else if (nb_elt > i + 3)
                            {
                                if ((is_event_name(mcast_event_prop[i + 2]) == false) &&
                                    (is_event_name(mcast_event_prop[i + 3]) == false))
                                    uncoherent = true;
                            }
                            else
                            {
                                if (is_event_name(mcast_event_prop[i + 2]) == false)
                                    uncoherent = true;
                            }
                        }
                    }

//
// If the property is uncoherent, clear it but inform user. No event will use multicasting in this case
//

					if (uncoherent == true)
					{
						std::cerr << "Database CtrlSystem/MulticastEvent property is uncoherent" << std::endl;
						std::cerr << "Prop syntax = mcast ip adr (must be between 224.x.y.z and 239.x.y.z) - port - [rate] - [ivl] - event name" << std::endl;
						std::cerr << "All events will use unicast communication" << std::endl;

						mcast_event_prop.clear();
						break;
					}
					else
						i = i + 1;
				}
			}

//
// All values in lower case letters
//

			for (unsigned int i = 0;i < nb_elt;i++)
			{
				std::transform(mcast_event_prop[i].begin(),mcast_event_prop[i].end(),mcast_event_prop[i].begin(),::tolower);
			}

//
// Multicast Hops
//

			mcast_hops = MCAST_HOPS;
			if (db_data[1].is_empty() == false)
				db_data[1] >> mcast_hops;

//
// Multicast PGM rate
//

			mcast_rate = PGM_RATE;
			if (db_data[2].is_empty() == false)
			{
				db_data[2] >> mcast_rate;
				mcast_rate = mcast_rate * 1024;
			}

//
// Multicast IVL
//

			mcast_ivl = PGM_IVL;
			if (db_data[3].is_empty() == false)
			{
				db_data[3] >> mcast_ivl;
				mcast_ivl = mcast_ivl * 1000;
			}

//
// Publisher Hwm
//

			zmq_pub_event_hwm = PUB_HWM;
			if (db_data[4].is_empty() == false)
				db_data[4] >> zmq_pub_event_hwm;

//
// Subscriber Hwm
//

			zmq_sub_event_hwm = SUB_HWM;
			if (db_data[5].is_empty() == false)
				db_data[5] >> zmq_sub_event_hwm;

//
// Nan allowed in writing attribute
//

			if (db_data[6].is_empty() == false)
			{
				bool new_val;
				db_data[6] >> new_val;
				tg->set_wattr_nan_allowed(new_val);
			}

		}
		catch (Tango::DevFailed &)
		{
			std::cerr << "Database error while trying to retrieve multicast event property" << std::endl;
			std::cerr << "All events will use unicast communication" << std::endl;
		}
	}
	else
    {
		mcast_event_prop.clear();
        zmq_pub_event_hwm = PUB_HWM;
        zmq_sub_event_hwm = SUB_HWM;
    }

}


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::mcast_event_for_att()
//
// description :
//		Return in m_event vector, list of mcast event for the specified device/attribute.
//		For each event, the returned string in the vector follows the syntax
//						event_name:ip_adr:port:rate:ivl
//		The last two are optionals. Please note that the same dev/att may have several event using multicasting
//
// args :
//		in :
// 			- dev_name : The device name
//		  	- att_name : The attribute name
//		out :
// 			- m_event : The multicast event definition
//
//------------------------------------------------------------------------------------------------------------------

void DServer::mcast_event_for_att(std::string &dev_name,std::string &att_name,std::vector<std::string> &m_event)
{

	m_event.clear();

	std::string full_att_name = dev_name + '/' + att_name;

	std::vector<std::string>::size_type nb_elt = mcast_event_prop.size();
	unsigned int ip_adr_ind = 0;

	for (unsigned int i = 0;i < nb_elt;++i)
	{
		if (is_ip_address(mcast_event_prop[i]) == true)
		{
			ip_adr_ind = i;
			continue;
		}

		if (is_event_name(mcast_event_prop[i]) == true)
		{
			if (mcast_event_prop[i].find(full_att_name) == 0)
			{
				std::string::size_type pos = mcast_event_prop[i].rfind('.');
				std::string tmp = mcast_event_prop[i].substr(pos + 1);
				tmp = tmp + ':' + mcast_event_prop[ip_adr_ind] + ':' + mcast_event_prop[ip_adr_ind + 1];
				if (is_event_name(mcast_event_prop[ip_adr_ind + 2]) == false)
				{
					tmp = tmp + ':' + mcast_event_prop[ip_adr_ind + 2];
					if (is_event_name(mcast_event_prop[ip_adr_ind + 3]) == false)
						tmp = tmp + ':' + mcast_event_prop[ip_adr_ind + 3];
				}
				m_event.push_back(tmp);
			}
		}
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::mem_event_par()
//
// description :
//		Store event parameters for all class/devices in a DS. This is necessary in case of command RestartServer
//
// args :
//		out :
// 			- _map : The map where these info are stored
//
//------------------------------------------------------------------------------------------------------------------

void DServer::mem_event_par(ServerEventSubscriptionState& _map)
{
	for (size_t i = 0;i < class_list.size();i++)
	{
		std::vector<DeviceImpl *> &dev_list = class_list[i]->get_device_list();
		for (size_t j = 0;j < dev_list.size();j++)
		{
			EventSubscriptionStates eve;
			dev_list[j]->get_device_attr()->get_event_param(eve);
			dev_list[j]->get_event_param(eve);

			if (eve.size() != 0)
			{
				_map.insert(make_pair(dev_list[j]->get_name(),eve));
			}
		}
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::apply_event_par()
//
// description :
//		Apply event parameters for all class/devices in a DS. This is necessary in case of command RestartServer
//
// args :
//		in :
// 			- _map : The map where these info are stored
//
//------------------------------------------------------------------------------------------------------------------

void DServer::apply_event_par(const ServerEventSubscriptionState& _map)
{
	for (size_t i = 0;i < class_list.size();i++)
	{
		std::vector<DeviceImpl *> &dev_list = class_list[i]->get_device_list();
		for (size_t j = 0;j < dev_list.size();j++)
		{
			std::string &dev_name = dev_list[j]->get_name();

			const auto ite = _map.find(dev_name);

			if (ite != _map.end())
			{
				dev_list[j]->get_device_attr()->set_event_param(ite->second);
				dev_list[j]->set_event_param(ite->second);
			}
		}
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::mem_devices_interface()
//
// description :
//		Memorize device interface for devices where some client(s) are listening on device interface change
//		event
//
// arguments :
//		out :
//			- _map : reference to the map (name,device interface) where the interface must be stored
//
//------------------------------------------------------------------------------------------------------------------

void DServer::mem_devices_interface(std::map<std::string,DevIntr> &_map)
{
	ZmqEventSupplier *event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

	if (class_list.empty() == false)
	{
		for (long i = class_list.size() - 1;i >= 0;i--)
		{
			if (class_list[i]->is_py_class() == false)
			{
				std::vector<DeviceImpl *> &devs = class_list[i]->get_device_list();
				size_t nb_dev = devs.size();
				for (size_t loop = 0;loop < nb_dev;loop++)
				{
					if (event_supplier_zmq->any_dev_intr_client(devs[loop]) == true &&
						devs[loop]->get_dev_idl_version() >= MIN_IDL_DEV_INTR)
					{
						cout4 << "Memorize dev interface for device " << devs[loop]->get_name() << std::endl;

						DevIntr di;
						di.get_interface(devs[loop]);
						_map.insert(make_pair(devs[loop]->get_name(),di));
					}
				}
			}
		}
	}

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DServer::changed_devices_interface()
//
// description :
//		Check for each device with client(s) listening on device interface change event if the device interface
//		has changed and if true, send event
//
// arguments :
//		in :
//			- _map : reference to the map (name,device interface) where devices interface are stored
//
//------------------------------------------------------------------------------------------------------------------

void DServer::changed_devices_interface(std::map<std::string,DevIntr> &_map)
{
	Tango::Util *tg = Util::instance();
	ZmqEventSupplier *event_supplier_zmq = tg->get_zmq_event_supplier();

	std::map<std::string,DevIntr>::iterator pos;
	for (pos = _map.begin();pos != _map.end();++pos)
	{
		DeviceImpl *dev = tg->get_device_by_name(pos->first);
		if (pos->second.has_changed(dev) == true)
		{
			cout4 << "Device interface for device " << dev->get_name() << " has changed !!!!!!!!!!!!!!!!!!!" << std::endl;

			Device_5Impl *dev_5 = static_cast<Device_5Impl *>(dev);
			DevCmdInfoList_2 *cmds_list = dev_5->command_list_query_2();

			DevVarStringArray dvsa(1);
			dvsa.length(1);
			dvsa[0] = Tango::string_dup(AllAttr_3);
			AttributeConfigList_5 *atts_list = dev_5->get_attribute_config_5(dvsa);

			event_supplier_zmq->push_dev_intr_change_event(dev,false,cmds_list,atts_list);
		}
	}
}

}// End of Tango namespace


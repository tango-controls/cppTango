static const char *RcsId = "$Id$";

//+============================================================================
//
// file :		Device.cpp
//
// description :	C++ source code for the DeviceImpl
//			class. This class
//			is the root class for all derived Device classes.
//			It is an abstract class. The DeviceImpl class is the
//			CORBA servant which is "exported" onto the network and
//			accessed by the client.
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
// $Revision$
//
//-============================================================================

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
// The per thread data storage key (The client identification is stored in thread specific storage)
// defined in utils.cpp
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

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,const char *d_name,
		       const char *de,Tango::DevState st,const char *sta)
:device_name(d_name),desc(de),device_status(sta),
 device_state(st),device_class(cl_ptr)
{
	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt(d_name);
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);
		  
//
//  Write the device name into the per thread data for 
//  sub device diagnostics
//

	(Tango::Util::instance())->get_sub_dev_diag().set_associated_device(ext->device_name_lower);
  
//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		throw;
	}

	get_dev_system_resource();
		
	black_box_create();
	
	ext->idl_version = 1;
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
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

// 
// write the polling
//
	init_cmd_poll_period();
	init_attr_poll_period();
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name)
:device_name(d_name),device_class(cl_ptr)
{
	desc = "A Tango device";
	device_state = Tango::UNKNOWN;
	device_status = StatusNotSet;

	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt(d_name.c_str());
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);

//
//  Write the device name into the per thread data for 
//  sub device diagnostics
//
  
	(Tango::Util::instance())->get_sub_dev_diag().set_associated_device(ext->device_name_lower);
	  
//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
	ext->idl_version = 1;
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
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

// 
// write the polling
//
	init_cmd_poll_period();	
	init_attr_poll_period();
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &description)
:device_name(d_name),device_class(cl_ptr)
{
	desc = description;
	device_state = Tango::UNKNOWN;
	device_status = StatusNotSet;

	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt(d_name.c_str());
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);

//
//  Write the device name into the per thread data for 
//  sub device diagnostics
//
  
	(Tango::Util::instance())->get_sub_dev_diag().set_associated_device(ext->device_name_lower);
	  
//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
	ext->idl_version = 1;
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
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

// 
// write the polling
//
	init_cmd_poll_period();
	init_attr_poll_period();
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &de,
		       Tango::DevState st,string &sta)
:device_name(d_name),desc(de),device_status(sta),
 device_state(st),device_class(cl_ptr)
{
	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt(d_name.c_str());
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);
	
//
//  Write the device name into the per thread data for 
//  sub device diagnostics
//
  
	(Tango::Util::instance())->get_sub_dev_diag().set_associated_device(ext->device_name_lower);
	  
//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
	ext->idl_version = 1;
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	
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

// 
// write the polling
//
	init_cmd_poll_period();
	init_attr_poll_period();	
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
	int interupted;
	Tango::Util *tg = Tango::Util::instance();
	
//
// If the vector of polling info is empty, no need to do anything (polling
// already stopped for all devices)
//

	vector<PollingThreadInfo *> &v_th_info = tg->get_polling_threads_info();
	if (v_th_info.size() == 0)
		return;
	
//
// Find out which thread is in charge of the device.
//

	PollingThreadInfo *th_info;
			
	int poll_th_id = tg->get_polling_thread_id_by_name(device_name.c_str());
	if (poll_th_id == 0)
	{
		TangoSys_OMemStream o;
		o << "Can't find a polling thread for device " << device_name << ends;
		Except::throw_exception((const char *)"API_PollingThreadNotFound",o.str(),
						(const char *)"DeviImpl::stop_polling");
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
			interupted = mon.wait(DEFAULT_TIMEOUT);

			if ((shared_cmd.cmd_pending == true) && (interupted == false))
			{
				cout4 << "TIME OUT" << endl;
				Except::throw_exception((const char *)"API_CommandTimedOut",
							(const char *)"Polling thread blocked !!",
							(const char *)"DeviceImpl::stop_polling");
				
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
	
	if ((ind = tg->get_dev_entry_in_pool_conf(ext->device_name_lower)) ==  -1)
	{
		TangoSys_OMemStream o;
		o << "Can't find entry for device " << device_name << " in polling threads pool configuration !"<< ends;
		Except::throw_exception((const char *)"API_PolledDeviceNotInPoolConf",o.str(),
								(const char *)"DeviceImpl::stop_polling");				
	}

	vector<string> &pool_conf = tg->get_poll_pool_conf();
	string &conf_entry = pool_conf[ind];
	string::size_type pos;
	if ((pos = conf_entry.find(',')) != string::npos)
	{
		pos = conf_entry.find(ext->device_name_lower);
		if ((pos + ext->device_name_lower.size()) != conf_entry.size())
			conf_entry.erase(pos,ext->device_name_lower.size() + 1);
		else
			conf_entry.erase(pos - 1);
	}
	else
	{
		vector<string>::iterator iter = pool_conf.begin() + ind;
		pool_conf.erase(iter);
		kill_thread = true;
	}

	tg->remove_dev_from_polling_map(ext->device_name_lower);
			
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
// Delete the extension class instance
//

	delete ext;

//
// Clear our ptr in the device class vector
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->is_svr_shutting_down() != true)
	{
		vector<DeviceImpl *> &dev_vect = get_device_class()->get_device_list();
		vector<DeviceImpl *>::iterator ite = find(dev_vect.begin(),dev_vect.end(),this);
		if (ite != dev_vect.end())
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
			blackbox_ptr = new BlackBox();
		else
			blackbox_ptr = new BlackBox(blackbox_depth);
	}
	catch (bad_alloc)
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

			Except::throw_exception((const char *)"API_DatabaseAccess",
					o.str(),
					(const char *)"DeviceImpl::get_dev_system_resource");
		}
		
		if (db_data[0].is_empty() == false)
			db_data[0] >> blackbox_depth;
		if (db_data[1].is_empty() == false)
			db_data[1] >> desc;
		if (db_data[2].is_empty() == false)
		{
			long tmp_depth;
			db_data[2] >> tmp_depth;
			set_poll_ring_depth(tmp_depth);
		}
		if (db_data[3].is_empty() == false)
			db_data[3] >> get_polled_cmd();
		if (db_data[4].is_empty() == false)
			db_data[4] >> get_polled_attr();
		if (db_data[5].is_empty() == false)
			db_data[5] >> get_non_auto_polled_cmd();
		if (db_data[6].is_empty() == false)
			db_data[6] >> get_non_auto_polled_attr();
		if (db_data[7].is_empty() == false)
		{
			long tmp_poll;
			db_data[7] >> tmp_poll;
			set_poll_old_factor(tmp_poll);
		}
		else
			set_poll_old_factor(DEFAULT_POLL_OLD_FACTOR);
		if (db_data[8].is_empty() == false)
		{
			db_data[8] >> ext->cmd_poll_ring_depth;
			unsigned long nb_prop = ext->cmd_poll_ring_depth.size();
			if ((nb_prop % 2) == 1)
			{
				ext->cmd_poll_ring_depth.clear();
				TangoSys_OMemStream o;
				o << "System property cmd_poll_ring_depth for device " << device_name << " has wrong syntax" << ends;
				Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        		o.str(),
				        		(const char *)"DeviceImpl::get_dev_system_resource()");
			}
			for (unsigned int i = 0;i < nb_prop;i = i + 2)
				transform(ext->cmd_poll_ring_depth[i].begin(),
					  ext->cmd_poll_ring_depth[i].end(),
					  ext->cmd_poll_ring_depth[i].begin(),
					  ::tolower);
		}
		if (db_data[9].is_empty() == false)
		{
			db_data[9] >> ext->attr_poll_ring_depth;
			unsigned long nb_prop = ext->attr_poll_ring_depth.size();
			if ((ext->attr_poll_ring_depth.size() % 2) == 1)
			{
				ext->attr_poll_ring_depth.clear();
				TangoSys_OMemStream o;
				o << "System property attr_poll_ring_depth for device " << device_name << " has wrong syntax" << ends;
				Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        		o.str(),
				        		(const char *)"DeviceImpl::get_dev_system_resource()");
			}
			for (unsigned int i = 0;i < nb_prop;i = i + 2)
				transform(ext->attr_poll_ring_depth[i].begin(),
					  ext->attr_poll_ring_depth[i].end(),
					  ext->attr_poll_ring_depth[i].begin(),
					  ::tolower);
		}

//
// The min. period related properties
//

		if (db_data[10].is_empty() == false)
			db_data[10] >> ext->min_poll_period;

		if (db_data[11].is_empty() == false)
		{
			db_data[11] >> ext->cmd_min_poll_period;
			unsigned long nb_prop = ext->cmd_min_poll_period.size();
			if ((ext->cmd_min_poll_period.size() % 2) == 1)
			{
				ext->cmd_min_poll_period.clear();
				TangoSys_OMemStream o;
				o << "System property cmd_min_poll_period for device " << device_name << " has wrong syntax" << ends;
				Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        		o.str(),
				        		(const char *)"DeviceImpl::get_dev_system_resource()");
			}
			for (unsigned int i = 0;i < nb_prop;i = i + 2)
				transform(ext->cmd_min_poll_period[i].begin(),
					  ext->cmd_min_poll_period[i].end(),
					  ext->cmd_min_poll_period[i].begin(),
					  ::tolower);
		}

		if (db_data[12].is_empty() == false)
		{
			db_data[12] >> ext->attr_min_poll_period;
			unsigned long nb_prop = ext->attr_min_poll_period.size();
			if ((ext->attr_min_poll_period.size() % 2) == 1)
			{
				ext->attr_min_poll_period.clear();
				TangoSys_OMemStream o;
				o << "System property attr_min_poll_period for device " << device_name << " has wrong syntax" << ends;
				Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        		o.str(),
				        		(const char *)"DeviceImpl::get_dev_system_resource()");
			}
			for (unsigned int i = 0;i < nb_prop;i = i + 2)
				transform(ext->attr_min_poll_period[i].begin(),
					  ext->attr_min_poll_period[i].end(),
					  ext->attr_min_poll_period[i].begin(),
					  ::tolower);
		}
	
//
// Since Tango V5 (IDL V3), State and Status are now polled as attributes
// Change properties if necessary
//

		if ((get_polled_cmd()).size() != 0)
			poll_lists_2_v5();		
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

#if (defined __linux)
void DeviceImpl::register_signal(long signo,bool hand)
{
	cout4 << "DeviceImpl::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_dev_signal(signo,hand,this);
		
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
	
	DServerSignal::instance()->unregister_dev_signal(signo,this);
	
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
	for (i = 0;i < cmd_list.size();i++)
	{
		if (cmd_list[i]->get_lower_name() == cmd_name)
		{
			if (cmd_list[i]->get_in_type() != Tango::DEV_VOID)
			{
				TangoSys_OMemStream o;
				o << "Command " << cmd_name << " cannot be polled because it needs input value" << ends;
				Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
							o.str(),(const char *)"DeviceImpl::check_command_exists");
			}
			return;
		}
	}

	TangoSys_OMemStream o;
	o << "Command " << cmd_name << " not found" << ends;	
	Except::throw_exception((const char *)"API_CommandNotFound",o.str(),
				(const char *)"DeviceImpl::check_command_exists");
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
	for (i = 0;i < cmd_list.size();i++)
	{
		if (cmd_list[i]->get_lower_name() == cmd_name)
		{
			return cmd_list[i];
		}
	}

	TangoSys_OMemStream o;
	o << "Command " << cmd_name << " not found" << ends;	
	Except::throw_exception((const char *)"API_CommandNotFound",o.str(),
				(const char *)"DeviceImpl::get_command");

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
	for (ite = po_list.begin();ite < po_list.end();++ite)
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
	Except::throw_exception((const char *)"API_PollObjNotFound",o.str(),
				(const char *)"DeviceImpl::get_polled_obj_by_type_name");

//
// Only to make compiler quiet. Should never pass here
//

// exclude the return value for VC8+
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER < 1400)
	return (vector<PollObj *>::iterator)NULL;
#endif
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::get_cmd_poll_ring_depth
// 
// description :	This method returns the polling buffer depth.
//			Most of the times, this is defined at device level
//			via the device "poll_ring_depth" property.
//			nevertheless, in some cases, this value cab be
//			overwritten via the device "cmd_poll_ring_depth"
//			property.
//
// in : 		cmd_name : The command name
//
// This method returns the polling buffer depth
//
//--------------------------------------------------------------------------

long DeviceImpl::get_cmd_poll_ring_depth(string &cmd_name)
{
	long ret;
	
	if (ext->cmd_poll_ring_depth.size() == 0)
	{
//
// No specific depth defined
//	
		if (ext->poll_ring_depth == 0)
			ret = DefaultPollRingDepth;
		else
			ret = ext->poll_ring_depth;
	}
	else
	{
		unsigned long k;		
//
// Try to find command in list of specific polling buffer depth
//

		for (k = 0; k < ext->cmd_poll_ring_depth.size();k = k + 2)
		{
			if (ext->cmd_poll_ring_depth[k] == cmd_name)
			{
				TangoSys_MemStream s;
				s << ext->cmd_poll_ring_depth[k + 1];
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				if ((s >> ret) == false)
#else
				s >> ret;
				if (!s)
#endif
				{
					TangoSys_OMemStream o;
					o << "System property cmd_poll_ring_depth for device " << device_name << " has wrong syntax" << ends;
					Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        			o.str(),
				        			(const char *)"DeviceImpl::get_poll_ring_depth()");
				}
				break;
			}
		}
		if (k >= ext->cmd_poll_ring_depth.size())
		{		
//
// Not found
//

			if (ext->poll_ring_depth == 0)
				ret = DefaultPollRingDepth;
			else
				ret = ext->poll_ring_depth;		
		}
	}
	
	return ret;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::get_attr_poll_ring_depth
// 
// description :	This method returns the polling buffer depth.
//			Most of the times, this is defined at device level
//			via the device "poll_ring_depth" property.
//			Nevertheless, in some cases, this value can be
//			overwritten via the device "attr_poll_ring_depth"
//			property.
//
// in : 		attr_name : The attribute name
//
// This method returns the polling buffer depth
//
//--------------------------------------------------------------------------

long DeviceImpl::get_attr_poll_ring_depth(string &attr_name)
{
	long ret;

	if (ext->attr_poll_ring_depth.size() == 0)
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

			if (ext->poll_ring_depth == 0)
				ret = DefaultPollRingDepth;
			else
				ret = ext->poll_ring_depth;
		}
	}
	else
	{
		unsigned long k;		
//
// Try to find command in list of specific polling buffer depth
//

		for (k = 0; k < ext->attr_poll_ring_depth.size();k = k + 2)
		{
			if (ext->attr_poll_ring_depth[k] == attr_name)
			{
				TangoSys_MemStream s;
				s << ext->attr_poll_ring_depth[k + 1];
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				if ((s >> ret) == false)
#else
				s >> ret;
				if (!s)
#endif
				{
					TangoSys_OMemStream o;
					o << "System property attr_poll_ring_depth for device " << device_name << " has wrong syntax" << ends;
					Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        			o.str(),
				        			(const char *)"DeviceImpl::get_poll_ring_depth()");
				}
				break;
			}
		}
		if (k >= ext->attr_poll_ring_depth.size())
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

				if (ext->poll_ring_depth == 0)
					ret = DefaultPollRingDepth;
				else
					ret = ext->poll_ring_depth;
			}		
		}
	}

	return ret;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::dev_state
// 
// description :	The default method called by the DevState command.
//			If the device is ON, this method checks attribute 
//			with a defined alarm and set the state to ALARM if one
//			of these attribute is in alarm. Otherwise, simply
//			returns device state
//
//--------------------------------------------------------------------------

Tango::DevState DeviceImpl::dev_state()
{
	NoSyncModelTangoMonitor mon(this);

	if ((device_state == Tango::ON) ||
	    (device_state == Tango::ALARM))
	{
		
//
// Read the hardware
//

		long vers = get_dev_idl_version();
		bool set_alrm = false;

		vector<long> &attr_list = dev_attr->get_alarm_list();
		vector<long> &attr_list_2 = get_alarmed_not_read();
		long nb_wanted_attr;
		
		if ((vers >= 3) && (ext->state_from_read == true))			
			nb_wanted_attr = attr_list_2.size();
		else
			nb_wanted_attr = attr_list.size();
		
		if (nb_wanted_attr != 0)
		{
			if (ext->state_from_read == false)
				read_attr_hardware(attr_list);
	
//
// Set attr value
//

			long i,j;
			vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();

			for (i = 0;i < nb_wanted_attr;i++)
			{

//
// Starting with IDl 3, it is possible that some of the alarmed attribute have
// already been read.
//

				long idx;
				if ((vers >= 3) && (ext->state_from_read == true))			
					idx = attr_list_2[i];
				else
					idx = attr_list[i];
									
				Attribute &att = dev_attr->get_attr_by_ind(idx);
				att.wanted_date(false);
				att.set_value_flag(false);

				try
				{
					if (vers < 3)
						read_attr(att);
					else
					{
						if (attr_vect[att.get_attr_idx()]->is_allowed(this,Tango::READ_REQ) == false)
						{
							att.wanted_date(true);
							continue;
						}
						attr_vect[att.get_attr_idx()]->read(this,att);
						Tango::AttrQuality qua = att.get_quality();
						if ((qua != Tango::ATTR_INVALID) && (att.get_value_flag() == false))
						{
							TangoSys_OMemStream o;
		
							o << "Read value for attribute ";
							o << att.get_name();
							o << " has not been updated";
							o << "Hint: Did the server follow Tango V5 attribute reading framework ?" << ends;

							Except::throw_exception((const char *)"API_AttrValueNotSet",o.str(),
				        			   (const char *)"DeviceImpl::dev_state");
						}
					}
				}
				catch (Tango::DevFailed)
				{
					for (j = 0;j < i;j++)
					{
						long idx;
						if ((vers >= 3) && (ext->state_from_read == true))			
							idx = attr_list_2[j];
						else
							idx = attr_list[j];
						Tango::Attribute &tmp_att = dev_attr->get_attr_by_ind(idx);
						tmp_att.wanted_date(true);
						if (tmp_att.get_quality() != Tango::ATTR_INVALID)					
							tmp_att.delete_seq();
					}
					att.wanted_date(true);
					throw;
				}
			}
		}
		
//
// Check alarm level
//

		if (dev_attr->check_alarm() == true)
		{
			set_alrm = true;
			device_state = Tango::ALARM;
		}
		else
			device_state = Tango::ON;
			
//
// Free the sequence created to store the attribute value
//

		if (nb_wanted_attr != 0)
		{
			for (long i = 0;i < nb_wanted_attr;i++)
			{
				long idx;
				if ((vers >= 3) && (ext->state_from_read == true))			
					idx = attr_list_2[i];
				else
					idx = attr_list[i];
				Tango::Attribute &att = dev_attr->get_attr_by_ind(idx);
				att.wanted_date(true);
				if (att.get_quality() != Tango::ATTR_INVALID)
					att.delete_seq();
			}
			
		}
		
//
// Check if one of the remaining attributes has its quality factor
// set to ALARM or WARNING. It is not necessary to do this if we have already detected
// that the state must switch to ALARM
//

		if ((set_alrm == false) && (device_state != Tango::ALARM))
		{
			if (dev_attr->is_att_quality_alarmed(false) == true)
				device_state = Tango::ALARM;
			else
				device_state = Tango::ON;
		}
	}

	return device_state;
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::dev_status
// 
// description :	The default method called by the DevStatus command.
//			If the device is ON, this method add Attribute status
//			for all device attribute in alarm state.
//
//--------------------------------------------------------------------------

Tango::ConstDevString DeviceImpl::dev_status()
{
	NoSyncModelTangoMonitor mon(this);
	const char *returned_str;

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
			returned_str = device_status.c_str();
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
throw (Tango::DevFailed, CORBA::SystemException)
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

// Catch all execeptions to set back the associated device after
// execution
	try
	{

//
// Record operation request in black box
//

		if (ext->store_in_bb == true)
			blackbox_ptr->insert_cmd(in_cmd);
		ext->store_in_bb = true;

//
// Execute command
//

		out_any = device_class->command_handler(this,command,in_any);
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
	return(out_any);
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
throw (CORBA::SystemException)
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
		if (strcmp(e.errors[0].reason,"API_CommandTimedOut") == 0)
			lim.minor(TG_IMP_MINOR_TO);
		else
			lim.minor(TG_IMP_MINOR_DEVFAILED);
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
throw (CORBA::SystemException)
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
		if (strcmp(e.errors[0].reason,"API_CommandTimedOut") == 0)
			lim.minor(TG_IMP_MINOR_TO);
		else
			lim.minor(TG_IMP_MINOR_DEVFAILED);
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
throw (CORBA::SystemException)
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
		if (strcmp(e.errors[0].reason,"API_CommandTimedOut") == 0)
			lim.minor(TG_IMP_MINOR_TO);
		else
			lim.minor(TG_IMP_MINOR_DEVFAILED);
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
throw (CORBA::SystemException)
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
		

//
// Return data to caller. If the state_cmd throw an exception, catch it and do 
// not re-throw it because we are in a CORBA attribute implementation
//

		always_executed_hook(); 
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
		if (strcmp(e.errors[0].reason,"API_CommandTimedOut") == 0)
			lim.minor(TG_IMP_MINOR_TO);
		else
			lim.minor(TG_IMP_MINOR_DEVFAILED);
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
throw (CORBA::SystemException)
{
	char *tmp;
	string last_associated_device;
	
	try
	{
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::status (attibute) arrived" << endl;

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
		
		if (strcmp(e.errors[0].reason,"API_CommandTimedOut") == 0)
			tmp = CORBA::string_dup("Not able to acquire device monitor");
		else
			tmp = CORBA::string_dup("Got exception	when trying to build device status");
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


Tango::DevVarStringArray* DeviceImpl::black_box(CORBA::Long n)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::black_box arrived" << endl;
	
	Tango::DevVarStringArray *ret = blackbox_ptr->read((long)n);
	
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


Tango::DevCmdInfoList* DeviceImpl::command_list_query()
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::command_list_query arrived" << endl;

//
// Retrieve number of command and allocate memory to send back info
//

	long nb_cmd = device_class->get_command_list().size();
	cout4 << nb_cmd << " command(s) for device" << endl;
	Tango::DevCmdInfoList *back;

	try
	{
		back = new Tango::DevCmdInfoList(nb_cmd);
		back->length(nb_cmd);
		
//
// Populate the vector
//

		for (long i = 0;i < nb_cmd;i++)
		{
			Tango::DevCmdInfo tmp;
			tmp.cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
			tmp.cmd_tag = (long)((device_class->get_command_list())[i]->get_disp_level());
			tmp.in_type = (long)((device_class->get_command_list())[i]->get_in_type());
			tmp.out_type = (long)((device_class->get_command_list())[i]->get_out_type());
			string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
			if (str_in.size() != 0)
				tmp.in_type_desc = CORBA::string_dup(str_in.c_str());
			else
				tmp.in_type_desc = CORBA::string_dup(DescNotSet);
			string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
			if (str_out.size() != 0)
				tmp.out_type_desc = CORBA::string_dup(str_out.c_str());
			else
				tmp.out_type_desc = CORBA::string_dup(DescNotSet);
			
			(*back)[i] = tmp;
		}
	}	
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::command_list_query");
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
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::command_query arrived" << endl;

	Tango::DevCmdInfo *back;
	string cmd(command);
	transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

	try
	{
		back = new Tango::DevCmdInfo();
	}	
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
					(const char *)"Can't allocate memory in server",
					(const char *)"DeviceImpl::command_query");
	}
	
//
// Try to retrieve the command in the command list
//

	long i; 
	long nb_cmd = device_class->get_command_list().size();
	for (i = 0;i < nb_cmd;i++)
	{
		if (device_class->get_command_list()[i]->get_lower_name() == cmd)
		{
			back->cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
			back->cmd_tag = (long)((device_class->get_command_list())[i]->get_disp_level());
			back->in_type = (long)((device_class->get_command_list())[i]->get_in_type());
			back->out_type = (long)((device_class->get_command_list())[i]->get_out_type());
			string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
			if (str_in.size() != 0)
				back->in_type_desc = CORBA::string_dup(str_in.c_str());
			else
				back->in_type_desc = CORBA::string_dup(DescNotSet);
			string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
			if (str_out.size() != 0)
				back->out_type_desc = CORBA::string_dup(str_out.c_str());
			else
				back->out_type_desc = CORBA::string_dup(DescNotSet);
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
		Except::throw_exception((const char *)"API_CommandNotFound",
				      o.str(),
				      (const char *)"DeviceImpl::command_query");
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
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::info arrived" << endl;

	Tango::DevInfo *back;

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

	try
	{
		back = new Tango::DevInfo();
	}	
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::info");
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
	string &cvs_tag = device_class->get_cvs_tag();
	if (cvs_tag.size() != 0)
	{
		doc_url = doc_url + "\nCVS Tag = ";
		doc_url = doc_url + cvs_tag;
	}
	string &cvs_location = device_class->get_cvs_location();
	if (cvs_location.size() != 0)
	{
		doc_url = doc_url + "\nCVS Location = ";
		doc_url = doc_url + cvs_location;
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
throw (Tango::DevFailed, CORBA::SystemException)
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

Tango::AttributeConfigList *DeviceImpl::get_attribute_config(const Tango::DevVarStringArray& names)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::get_attribute_config arrived" << endl;
	
	TangoMonitor &mon = get_att_conf_monitor();
	AutoTangoMonitor sync(&mon);

	long nb_attr = names.length();
	Tango::AttributeConfigList *back;
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
				nb_attr = nb_dev_attr;
			else
				nb_attr = nb_dev_attr - 2;
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
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"DeviceImpl::get_attribute_config");
	}

//
// Fill in these structures
//
		
	for (long i = 0;i < nb_attr;i++)
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
		catch (Tango::DevFailed e)
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

void DeviceImpl::set_attribute_config(const Tango::AttributeConfigList& new_conf)
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this,true);
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
		Except::throw_exception((const char *)"API_AttrNotFound",
				        (const char *)"The device does not have any attribute",
				        (const char *)"DeviceImpl::set_attribute_config");
	}

//
// Get some event related data
//
	
	Tango::Util *tg = Tango::Util::instance();
	EventSupplier *ev_supply = tg->get_event_supplier();

//
// Update attribute config first in database, then locally
// Finally send attr conf. event
//
	
	long nb_attr = new_conf.length();
	long i;

	try
	{	
		for (i = 0;i < nb_attr;i++)
		{
			string tmp_name(new_conf[i].name);
			transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
			if ((tmp_name == "state") || (tmp_name == "status"))
			{
				Except::throw_exception((const char *)"API_AttrNotFound",
				        		(const char *)"Cannot set config for attribute state or status",
				        		(const char *)"DeviceImpl::set_attribute_config");
			}

			Attribute &attr = dev_attr->get_attr_by_name(new_conf[i].name);
			bool old_alarm = attr.is_alarmed().any();
			if (Tango::Util::_UseDb == true)
				attr.upd_database(new_conf[i],device_name);
			attr.set_properties(new_conf[i],device_name);
			
//
// In case the attribute quality factor was set to ALARM, reset it to VALID
//

			if ((attr.get_quality() == Tango::ATTR_ALARM) &&
			    (old_alarm == true) &&
			    (attr.is_alarmed().any() == false))
				attr.set_quality(Tango::ATTR_VALID);

//
// Send the event
//
				
			if (ev_supply != NULL)
			{
				string tmp_name(new_conf[i].name);
				
				if (get_dev_idl_version() <= 2)
				{
					Tango::AttributeConfig_2 attr_conf_2;
					attr.get_properties_2(attr_conf_2);
					ev_supply->push_att_conf_events(this,attr_conf_2,(Tango::DevFailed *)NULL,tmp_name);
				}
				else
				{
					Tango::AttributeConfig_3 attr_conf_3;
					attr.get_properties_3(attr_conf_3);
					ev_supply->push_att_conf_events(this,attr_conf_3,(Tango::DevFailed *)NULL,tmp_name);
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
		for (long j = 0;j < nb_dev_attr;j++)
		{
			Attribute &att = dev_attr->get_attr_by_ind(j);
			if (att.is_alarmed().any() == true)
			{
				if (att.get_writable() != Tango::WRITE)
					dev_attr->get_alarm_list().push_back(j);
			}
		}		
	
//
// Change the exception reason flag
//

		TangoSys_OMemStream o;
		
		o << e.errors[0].reason;
		if (i != 0)
			o << "\nAll previous attribute(s) have been successfully updated";
		if (i != (nb_attr - 1))
			o << "\nAll remaining attribute(s) have not been updated";
		o << ends;

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
		string s = o.str();
		e.errors[0].reason = CORBA::string_dup(s.c_str());
#else		
		char *mess = o.str();			
		e.errors[0].reason = CORBA::string_dup(mess);
		delete [] mess;
#endif
		throw;
	}

//
// Re build the list of "alarmable" attribute
//

	dev_attr->get_alarm_list().clear();
	for (i = 0;i < nb_dev_attr;i++)
	{
		Tango::Attribute &attr = dev_attr->get_attr_by_ind(i);
		Tango::AttrWriteType w_type = attr.get_writable();
		if (attr.is_alarmed().any() == true)
		{
			if (w_type != Tango::WRITE)
				dev_attr->get_alarm_list().push_back(i);
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

Tango::AttributeValueList *DeviceImpl::read_attributes(const Tango::DevVarStringArray& names)
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this,true);
	
	Tango::AttributeValueList *back;

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

		if (ext->store_in_bb == true)
			blackbox_ptr->insert_attr(names);
		ext->store_in_bb = true;

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
			Except::throw_exception((const char *)"API_AttrNotFound",
						(const char *)"The device does not have any attribute",
						(const char *)"DeviceImpl::read_attributes");
		}
		if (vers >= 3)
			nb_dev_attr = nb_dev_attr - 2;

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
				for (i = 0;i < nb_dev_attr;i++)
					real_names[i] = dev_attr->get_attr_by_ind(i).get_name().c_str();		
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

		for (i = 0;i < nb_names;i++)
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
					Except::throw_exception((const char *)"API_NotSupportedFeature",
								o.str(),
								(const char *)"DeviceImpl::read_attributes");
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
						Except::throw_exception((const char *)"API_NotSupportedFeature",
								o.str(),
								(const char *)"DeviceImpl::read_attributes");
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
			read_attr_hardware(wanted_attr);

//
// Set attr value (for readable attribute)
//

		vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();

		for (i = 0;i < nb_wanted_attr;i++)
		{
			if (vers < 3)
				read_attr(dev_attr->get_attr_by_ind(wanted_attr[i]));
			else
			{
				Attribute &att = dev_attr->get_attr_by_ind(wanted_attr[i]);
				long idx = att.get_attr_idx();
				if (idx == -1)
				{
					TangoSys_OMemStream o;

					o << "It is not possible to read state/status as attributes with your\n";
					o << "Tango software release. Please, re-link with Tango V5." << ends;

					Except::throw_exception((const char *)"API_NotSupportedFeature",
					        		o.str(),
					        		(const char *)"Device_Impl::read_attributes");
				}

				if (attr_vect[idx]->is_allowed(this,Tango::READ_REQ) == false)
				{
					TangoSys_OMemStream o;

					o << "It is currently not allowed to read attribute ";
					o << att.get_name() << ends;

					Except::throw_exception((const char *)"API_AttrNotAllowed",
					        		o.str(),
					        		(const char *)"Device_Impl::read_attributes");
				}
				attr_vect[idx]->read(this,att);
			}		
		}

//
// Set attr value for writable attribute
//

		for (i = 0;i < nb_wanted_w_attr;i++)
		{
			Tango::AttrWriteType w_type = dev_attr->get_attr_by_ind(wanted_w_attr[i]).get_writable();
			if ((w_type == Tango::READ_WITH_WRITE) ||
		    	(w_type == Tango::WRITE)) 
				dev_attr->get_attr_by_ind(wanted_w_attr[i]).set_rvalue();
		}

//
// Allocate memory for the AttributeValue structures
//

		try
		{
			back = new Tango::AttributeValueList(nb_names);
			back->length(nb_names);
		}
		catch (bad_alloc)
		{
			Except::throw_exception((const char *)"API_MemoryAllocation",
						(const char *)"Can't allocate memory in server",
						(const char *)"DeviceImpl::read_attributes");
		}

//
// Build the sequence returned to caller for readable attributes and check 
// that all the wanted attributes set value has been updated
//

		for (i = 0;i < nb_names;i++)
		{
			Attribute &att = dev_attr->get_attr_by_name(real_names[i]);
			Tango::AttrQuality qual = att.get_quality();
			if (qual != Tango::ATTR_INVALID)
			{
				if (att.get_value_flag() == false)
				{
					TangoSys_OMemStream o;
					delete back;
					for (long j = 0;j < i;j++)
					{
						att = dev_attr->get_attr_by_name(real_names[j]);
						att.delete_seq();
					}

					try
					{
						string att_name(real_names[i]);
						transform(att_name.begin(),att_name.end(),att_name.begin(),::tolower);

						vector<PollObj *>::iterator ite = get_polled_obj_by_type_name(Tango::POLL_ATTR,att_name);
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

					Except::throw_exception((const char *)"API_AttrValueNotSet",
								 o.str(),
								(const char *)"DeviceImpl::read_attributes");
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
							dev_attr->add_write_value(att);

						if ((att.is_alarmed().any() == true) && (qual != Tango::ATTR_INVALID))
							att.check_alarm();
					}

					CORBA::Any &a = (*back)[i].value;
					switch (att.get_data_type())
					{
					case Tango::DEV_SHORT :
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
				att.set_time();					

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

void DeviceImpl::write_attributes(const Tango::AttributeValueList& values)
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this,true);
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
			Except::throw_exception((const char *)"API_AttrNotFound",
						(const char *)"The device does not have any attribute",
						(const char *)"DeviceImpl::write_attributes");
		}

//
// Retrieve index of wanted attributes in the device attribute list
//

		long nb_updated_attr = values.length();
		vector<long> updated_attr;

		long i;
		for (i = 0;i < nb_updated_attr;i++)
		{
			updated_attr.push_back(dev_attr->get_attr_ind_by_name(values[i].name));
		}

//
// Check that these attributes are writable
//

		for (i = 0;i < nb_updated_attr;i++)
		{
			if ((dev_attr->get_attr_by_ind(updated_attr[i]).get_writable() == Tango::READ) ||
		    	(dev_attr->get_attr_by_ind(updated_attr[i]).get_writable() == Tango::READ_WITH_WRITE))
			{
				TangoSys_OMemStream o;

				o << "Attribute ";
				o << dev_attr->get_attr_by_ind(updated_attr[i]).get_name();
				o << " is not writable" << ends;

				Except::throw_exception((const char *)"API_AttrNotWritable",
							o.str(),
							(const char *)"DeviceImpl::write_attributes");
			}
		}

//
// Call the always_executed_hook
//

		always_executed_hook();

//
// Set attribute internal value
//

		for (i = 0;i < nb_updated_attr;i++)
		{
			try
			{
				dev_attr->get_w_attr_by_ind(updated_attr[i]).check_written_value(values[i].value,(unsigned long)1,(unsigned long)0);
			}
			catch (Tango::DevFailed)
			{
				for (long j = 0;j < i;j++)
					dev_attr->get_w_attr_by_ind(updated_attr[j]).rollback();

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
			for (i = 0;i < nb_updated_attr;i++)
			{
				WAttribute &att = dev_attr->get_w_attr_by_ind(updated_attr[i]);
				att.copy_data(values[i].value);
			}
		}
		else
		{
			vector<long> att_in_db;

			for (i = 0;i < nb_updated_attr;i++)
			{
				WAttribute &att = dev_attr->get_w_attr_by_ind(updated_attr[i]);
				vector<Tango::Attr *> &attr_vect = device_class->get_class_attr()->get_attr_list();
				if (attr_vect[att.get_attr_idx()]->is_allowed(this,Tango::WRITE_REQ) == false)
				{
					TangoSys_OMemStream o;

					o << "It is currently not allowed to write attribute ";
					o << att.get_name();
					o << ". The device state is " << Tango::DevStateName[get_state()] << ends;


					Except::throw_exception((const char *)"API_AttrNotAllowed",
					        		o.str(),
					        		(const char *)"Device_Impl::write_attributes");
				}
				attr_vect[att.get_attr_idx()]->write(this,att);
				att.copy_data(values[i].value);
				if (att.is_memorized() == true)
					att_in_db.push_back(i);
				if (att.is_alarmed().test(Attribute::rds) == true)
					att.set_written_date();
			}

			if ((Tango::Util::_UseDb == true) && (att_in_db.size() != 0))
			{
				try
				{
					 static_cast<Device_3Impl *>(this)->write_attributes_in_db(att_in_db,updated_attr);
				}
				catch (Tango::DevFailed &e)
				{
					Except::re_throw_exception(e,(const char *)"API_AttrNotAllowed",
					        	    	 (const char *)"Failed to store memorized attribute value in db",
					        	    	 (const char *)"Device_Impl::write_attributes");
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


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::add_attribute
// 
// description :	Add attribute to the device attribute(s) list
//
// argument: in :	- new_attr: The new attribute to be added.
//
//--------------------------------------------------------------------------

void DeviceImpl::add_attribute(Tango::Attr *new_attr)
{
//
// Take the device monitor in order to protect the attribute list
//

	AutoTangoMonitor sync(this,true);
	
	vector<Tango::Attr *> &attr_list = device_class->get_class_attr()->get_attr_list();
	long old_attr_nb = attr_list.size();

//
// Check that this attribute is not already defined for this device.
// If it is already there, immediately returns.
// Trick : If you add an attribute to a device, this attribute will be 
// inserted in the device class attribute list. Therefore, all devices
// created after this attribute addition will also have this attribute.
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
	catch (Tango::DevFailed)
	{
		already_there = false;
	}

//
// Throw exception if the device already have an attribute with the same name
// but with a different definition
//
	
	if (throw_ex == true)
	{
		TangoSys_OMemStream o;

		o << "Device " << get_name() << " -> Attribute " << attr_name << " already exists for your device but with other definition";
		o << "\n(data type, data format or data write type)" << ends;

		Except::throw_exception((const char *)"API_AttrNotFound",
				o.str(),
				(const char *)"Device_Impl::add_attribute");
	}

	if (already_there == true)
	{
		delete new_attr;
		return;
	}
				
//
// Add this attribute in the MultiClassAttribute attr_list vector if it does not
// already exist
//

	bool need_free = false;
	long i;
	for (i = 0;i < old_attr_nb;i++)
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
// An attribute with the same name is already defined within the class
// Check if the data type, data format and write type are the same
//

		if ((attr_list[i]->get_type() != new_attr->get_type()) ||
		    (attr_list[i]->get_format() != new_attr->get_format()) ||
		    (attr_list[i]->get_writable() != new_attr->get_writable()))
		{
			TangoSys_OMemStream o;

			o << "Device " << get_name() << " -> Attribute " << attr_name << " already exists for your device class but with other definition";
			o << "\n(data type, data format or data write type)" << ends;

			Except::throw_exception((const char *)"API_AttrNotFound",
					o.str(),
					(const char *)"Device_Impl::add_attribute");		
		}
	}
						
//
// Add the attribute to the MultiAttribute object
//

	dev_attr->add_attribute(device_name,device_class,i);
	
//
// Free memory if needed
//

	if (need_free == true)
		delete new_attr;
		
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::remove_attribute
// 
// description :	Remove attribute to the device attribute(s) list
//
// argument: in :	- rem_attr: The attribute to be deleted.
//
//--------------------------------------------------------------------------

void DeviceImpl::remove_attribute(Tango::Attr *rem_attr, bool free_it)
{
//
// Take the device monitor in order to protect the attribute list
//

	AutoTangoMonitor sync(this,true);

//
// Check that the class support this attribute
//

	string &attr_name = rem_attr->get_name();

	try
	{
		dev_attr->get_attr_by_name(attr_name.c_str());
	}
	catch (Tango::DevFailed)
	{
		TangoSys_OMemStream o;

		o << "Attribute " << attr_name << " is not defined as attribute for your device.";
		o << "\nCan't remove it" << ends;

		Except::throw_exception((const char *)"API_AttrNotFound",
					o.str(),
					(const char *)"Device_Impl::remove_attribute");		
	}

//
// stop any configured polling for this attribute first!
//
	
	vector<string> &poll_attr = get_polled_attr();
	vector<string>::iterator ite_attr;

//	
// convert the attribute name to lowercase
//

	string  attr_name_low(attr_name);
	transform(attr_name_low.begin(),attr_name_low.end(),attr_name_low.begin(),::tolower);				

//
// try to find the attribute in the list of polled attributes
//	
	
	Tango::Util *tg = Tango::Util::instance();
	ite_attr = find(poll_attr.begin(),poll_attr.end(), attr_name_low);	
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
			adm_dev->rem_obj_polling(&send,false);
			tg->get_polled_dyn_attr_names().push_back(attr_name_low);
			if (tg->get_full_polled_att_list().size() == 0)
			{
				tg->get_full_polled_att_list() = poll_attr;
				tg->get_dyn_att_dev_name() = device_name;
			}
		}
		else							
			adm_dev->rem_obj_polling(&send, true);
	}

//
// Get  Db server version to know if it support the new command to remove
// all attribute properties in one go
//

	if (tg->get_db_svr_version() == 0)
	{
		tg->set_db_svr_version();
	}
	
//		
// Now remove all configured attribute properties from the database
//	

	if ((tg->is_svr_shutting_down() == false) || (tg->get_db_svr_version() < 400))
	{	
		Tango::Attribute &att_obj = dev_attr->get_attr_by_name(attr_name.c_str());
		att_obj.remove_configuration();
	}
	else
	{
		tg->get_all_dyn_attr_names().push_back(attr_name);
		if (tg->get_dyn_att_dev_name().size() == 0)
			tg->get_dyn_att_dev_name() = device_name;

	}
	
//
// Remove attribute in MultiClassAttribute in case there is
// only one device in the class or it is the last device
// in this class with this attribute
//

	bool update_idx = false;
	unsigned long nb_dev = device_class->get_device_list().size();

	if (nb_dev <= 1)
	{
		device_class->get_class_attr()->remove_attr(attr_name,rem_attr->get_cl_name());
		update_idx = true;
	}
	else
	{
		vector<Tango::DeviceImpl *> dev_list = device_class->get_device_list();
		unsigned long nb_except = 0;
		for (unsigned long i = 0;i < nb_dev;i++)
		{
			try
			{
				Attribute &att = dev_list[i]->get_device_attr()->get_attr_by_name(attr_name.c_str());
				vector<Tango::Attr *> &attr_list = device_class->get_class_attr()->get_attr_list();
				if (attr_list[att.get_attr_idx()]->get_cl_name() != rem_attr->get_cl_name())
					nb_except++;
			}
			catch (Tango::DevFailed)
			{
				nb_except++;
			}
		}
		if (nb_except == (nb_dev - 1))
		{
			device_class->get_class_attr()->remove_attr(attr_name,rem_attr->get_cl_name());
			update_idx = true;
		}
	}
		
//
// Now, remove the attribute from the MultiAttribute object
//

	dev_attr->remove_attribute(attr_name,update_idx);
	
//
// Delete Attr object if wanted
//

	if ((free_it == true) && (update_idx == true))
		delete rem_attr;
		
}



//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::remove_attribute
// 
// description :	Remove attribute to the device attribute(s) list
//
// argument: in :	- rem_attr: The name of the attribute to be deleted.
//
//--------------------------------------------------------------------------

void DeviceImpl::remove_attribute(string &rem_attr_name, bool free_it)
{

	try
	{
		Attr &att = device_class->get_class_attr()->get_attr(rem_attr_name);
		remove_attribute(&att,free_it);
	}
	catch (Tango::DevFailed)
	{
		TangoSys_OMemStream o;

		o << "Attribute " << rem_attr_name << " is not defined as attribute for your device.";
		o << "\nCan't remove it" << ends;

		Except::throw_exception((const char *)"API_AttrNotFound",
					o.str(),
					(const char *)"Device_Impl::remove_attribute");		
	}
	
}


//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::poll_lists_2_v5
// 
// description :	Started from Tango V5, state and status are polled
//			as attributes. Previously, they were polled as commands.
//			If state or status is polled as commands, move them
//			to the list of polled attributes
//
//--------------------------------------------------------------------------

void DeviceImpl::poll_lists_2_v5()		
{
	bool db_update = false;
	
	vector<string> &poll_cmd = get_polled_cmd();
	vector<string> &poll_attr = get_polled_attr();
	
	vector<string>::iterator ite_state;
	vector<string>::iterator ite_status;

//
// Try to find state in list of polled command(s). If found,
// remove it from poll cmd and move it to poll attr
//

	ite_state = find(poll_cmd.begin(),poll_cmd.end(),"state");	
	if (ite_state != poll_cmd.end())
	{
		poll_attr.push_back(*ite_state);
		poll_attr.push_back(*(ite_state + 1));
		poll_cmd.erase(ite_state,ite_state + 2);
		db_update = true;
	}

//
// The same for status
//
	
	ite_status = find(poll_cmd.begin(),poll_cmd.end(),"status");
	if (ite_status != poll_cmd.end())
	{
		poll_attr.push_back(*ite_status);
		poll_attr.push_back(*(ite_status + 1));
		poll_cmd.erase(ite_status,ite_status + 2);
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

//+----------------------------------------------------------------------------
// method : DeviceImplExt::DeviceImplExt
//-----------------------------------------------------------------------------

#ifdef TANGO_HAS_LOG4TANGO
DeviceImplExt::DeviceImplExt(const char *d_name)
  : exported(false),
    polled(false),
    poll_ring_depth(0),only_one(d_name),
    logger(0),saved_log_level(log4tango::Level::WARN),
    rft(Tango::kDefaultRollingThreshold),
    store_in_bb(true),poll_mon("cache"),
    att_conf_mon("att_config"),state_from_read(false),
    py_device(false),
    device_locked(false),locker_client(NULL),old_locker_client(NULL),
	lock_ctr(0),min_poll_period(0)
{
}
#endif


//+----------------------------------------------------------------------------
//
// method :		DeviceImplExt::~DeviceImplExt
// 
// description :	DeviceImpl extension class destructor. This destructor
//			delete memory for ring buffer used for polling.
//
//-----------------------------------------------------------------------------

DeviceImplExt::~DeviceImplExt()
{
	for (unsigned long i = 0;i < poll_obj_list.size();i++)
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
	
	if (locker_client != NULL)
		delete locker_client;
	if (old_locker_client != NULL)
		delete old_locker_client;
	
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::init_cmd_poll_ext_trig
// 
// description : 	Write the command name to the list of polled commands in the database.
//			The polling period is set to 0 to indicate that the polling buffer
//			is filled externally from the device server code.
//
//--------------------------------------------------------------------------

void DeviceImpl::init_cmd_poll_ext_trig(string cmd_name)
{
	string  cmd_lowercase(cmd_name);
   	transform(cmd_lowercase.begin(),cmd_lowercase.end(),cmd_lowercase.begin(),::tolower);				

	// never do the for the state or status commands, they are
	// handled as attributes!
	if ( cmd_name == "state" || cmd_name == "status" )
	{
		TangoSys_OMemStream o;
		
		o << "State and status are handled as attributes for the polling" << ends;
		Except::throw_exception((const char *)"API_CommandNotFound",
				      o.str(),
				      (const char *)"DeviceImpl::init_poll_ext_trig");		
	}
	
	// check whether the command exists for the device and can be polled
	check_command_exists (cmd_lowercase);
	
	
	// check wether the database is used
	Tango::Util *tg = Tango::Util::instance();	
	if ( tg->_UseDb == true )
	{
		vector <string> &poll_list = get_polled_cmd();
		Tango::DbData poll_data;
		bool found = false;
		
		poll_data.push_back(Tango::DbDatum("polled_cmd"));
	
		// read the polling configuration from the database
		//		tg->get_database()->get_device_property(device_name, poll_data);
		
		//		if (poll_data[0].is_empty()==false) 
		if (poll_list.empty() == false)
		{			
			//poll_data[0] >> poll_list;
			
			// search the attribute in the list of polled attributes
			for (unsigned int i = 0;i < poll_list.size();i = i+2)
			{
            //      Convert to lower case before comparison
          			string  name_lowercase(poll_list[i]);
            			transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
				
				if ( name_lowercase ==  cmd_lowercase)
				{		
					poll_list[i+1] = "0";
					found = true;				
				}
			}
		}
			
		if ( found == false )
		{	
			poll_list.push_back (cmd_lowercase);
			poll_list.push_back ("0");		
		}
	
		poll_data[0] << poll_list;	
		tg->get_database()->put_device_property(device_name, poll_data);
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::init_cmd_poll_period
// 
// description : 	Checks the specified polling period for all commands of the device.
//			If a polling period is specified for a command the
//			command name and the period are written to the list of polled 
//			commands in the database.
//			This happens only if the command is not yet in the list of polled commands.
//
//--------------------------------------------------------------------------

void DeviceImpl::init_cmd_poll_period()
{
	// check wether the database is used
	Tango::Util *tg = Tango::Util::instance();	
	if ( tg->_UseDb == true )
	{
		vector <string> &poll_list = get_polled_cmd();
		Tango::DbData poll_data;
		
		poll_data.push_back(Tango::DbDatum("polled_cmd"));
	
		// read the polling configuration from the database
		//		tg->get_database()->get_device_property(device_name, poll_data);
		//		if (poll_data[0].is_empty()==false) 
		//		{
		//			poll_data[0] >> poll_list;
		//		}
			
		// get the command list
		vector<Command *> &cmd_list = device_class->get_command_list(); 

		// loop over the command list
		unsigned long added_cmd = 0;
		unsigned long i;
		for (i = 0;i < cmd_list.size();i++)
		{
			long poll_period;
			poll_period = cmd_list[i]->get_polling_period();

			//check the validity of the polling period.
			// must be longer than 20ms
			if ( poll_period < MIN_POLL_PERIOD )
			{
				continue;
			}

			// never do the for the state or status commands, they are
			// handled as attributes!
			string cmd_name = cmd_list[i]->get_lower_name();
			if ( cmd_name == "state" || cmd_name == "status" )
			{
				continue;		
			}

			// Can only handle commands without input argument
			if (cmd_list[i]->get_in_type() != Tango::DEV_VOID)
			{	
				continue;
			}

			// search the command in the list of polled commands
			bool found = false;
			for (unsigned int i = 0;i < poll_list.size();i = i+2)
			{
            //      Convert to lower case before comparison
           		string  name_lowercase(poll_list[i]);
            	transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);

				if ( name_lowercase == cmd_name)
				{		
					found = true;
					break;				
				}
			}

			if ( found == false )
			{	
				string period_str;
				stringstream str;
				str << poll_period << ends;
				str >> period_str;

				poll_list.push_back (cmd_name);
				poll_list.push_back (period_str);
				added_cmd++;
			}
		}
		
		// only write to the database when a polling need to be added
		if ( added_cmd > 0 )
		{
			poll_data[0] << poll_list;	
			tg->get_database()->put_device_property(device_name, poll_data);
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::init_attr_poll_ext_trig
// 
// description : 	Write the attribute name to the list of polled attributes in the database.
//			The polling period is set to 0 to indicate that the polling buffer
//			is filled externally from the device server code.
//
//--------------------------------------------------------------------------

void DeviceImpl::init_attr_poll_ext_trig(string attr_name)
{
	string  attr_lowercase(attr_name);
   	transform(attr_lowercase.begin(),attr_lowercase.end(),attr_lowercase.begin(),::tolower);				
	
	// check whether the attribute exists for the device and can be polled
	dev_attr->get_attr_by_name (attr_lowercase.c_str());
	
	// check wether the database is used
	Tango::Util *tg = Tango::Util::instance();	
	if ( tg->_UseDb == true )
	{
		vector <string> &poll_list = get_polled_attr();
		Tango::DbData poll_data;
		bool found = false;
		
		poll_data.push_back(Tango::DbDatum("polled_attr"));
	
		// read the polling configuration from the database
		//		tg->get_database()->get_device_property(device_name, poll_data);
		
		//		if (poll_data[0].is_empty()==false) 
		if (poll_list.empty() == false)
		{			
			//poll_data[0] >> poll_list;
			
			// search the attribute in the list of polled attributes
			for (unsigned int i = 0;i < poll_list.size();i = i+2)
			{
            //      Convert to lower case before comparison
          			string  name_lowercase(poll_list[i]);
            			transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
				
				if ( name_lowercase ==  attr_lowercase)
				{	
					if ( poll_list[i+1] == "0" )
						{
						// The configuration is already correct, no nned for further action
						return;
						}
					else
						{				
						poll_list[i+1] = "0";
						found = true;
						}				
				}
			}
		}
			
		if ( found == false )
		{	
			poll_list.push_back (attr_lowercase);
			poll_list.push_back ("0");	
		}
					
		poll_data[0] << poll_list;	
		tg->get_database()->put_device_property(device_name, poll_data);	
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::init_attr_poll_period
// 
// description : 	Checks the specified polling period for all attributes of the device.
//			If a polling period is specified for an attribute the
//			command name and the period are written to the list of polled 
//			attributes in the database.
//			This happens only if the attribute is not yet in the list of polled attributes.
//
//--------------------------------------------------------------------------

void DeviceImpl::init_attr_poll_period()
{
	// check wether the database is used
	Tango::Util *tg = Tango::Util::instance();	
	if ( tg->_UseDb == true )
	{
		vector <string> &poll_list = get_polled_attr();
		Tango::DbData poll_data;
		
		poll_data.push_back(Tango::DbDatum("polled_attr"));
	
		// read the polling configuration from the database
		//		tg->get_database()->get_device_property(device_name, poll_data);
		//		if (poll_data[0].is_empty()==false) 
		//		{
		//			poll_data[0] >> poll_list;
		//		}
			
		// get the multi attribute object
		vector<Attribute *> &attr_list = dev_attr->get_attribute_list(); 

		// loop over the command list
		unsigned long added_attr = 0;
		unsigned long i;
		for (i = 0;i < attr_list.size();i++)
		{
			long poll_period;
			poll_period = attr_list[i]->get_polling_period();

			//check the validity of the polling period.
			// must be longer than 20ms
			if ( poll_period < MIN_POLL_PERIOD )
			{
				continue;
			}

			string attr_name = attr_list[i]->get_name_lower();

			// search the attribute in the list of polled attributes
			bool found = false;
			for (unsigned int i = 0;i < poll_list.size();i = i+2)
			{
            //      Convert to lower case before comparison
           			string  name_lowercase(poll_list[i]);
            			transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);

				if ( name_lowercase == attr_name)
				{		
					found = true;
					break;				
				}
			}

			if ( found == false )
			{	
				string period_str;
				stringstream str;
				str << poll_period << ends;
				str >> period_str;

				poll_list.push_back (attr_name);
				poll_list.push_back (period_str);
				added_attr++;
			}
		}
		
		// only write to the database when a polling need to be added
		if ( added_attr > 0 )
		{			
			poll_data[0] << poll_list;	
			tg->get_database()->put_device_property(device_name, poll_data);
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::push_att_conf_event
// 
// description : 	Push an attribute configuration event
//
// args : in : - attr : The attribute
//
//--------------------------------------------------------------------------

void DeviceImpl::push_att_conf_event(Attribute *attr)
{
	Tango::Util *tg = Tango::Util::instance();
	EventSupplier *ev_supply = tg->get_event_supplier();
	
	if (ev_supply != NULL)
	{	
		if (get_dev_idl_version() <= 2)
		{
			Tango::AttributeConfig_2 attr_conf_2;
			attr->get_properties_2(attr_conf_2);
			ev_supply->push_att_conf_events(this,attr_conf_2,(Tango::DevFailed *)NULL,attr->get_name());
		}
		else
		{
			Tango::AttributeConfig_3 attr_conf_3;
			attr->get_properties_3(attr_conf_3);
			ev_supply->push_att_conf_events(this,attr_conf_3,(Tango::DevFailed *)NULL,attr->get_name());
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::get_client_ident()
// 
// description : 	Get client identification
//
// This method returns a pointer to the client identification
//
//--------------------------------------------------------------------------

Tango::client_addr *DeviceImpl::get_client_ident()
{
	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	return (client_addr *)ip;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::lock
// 
// description : 	Lock the device
//
// args : in : - cl : The client identification
//			   - validity : The lock validity
//
//--------------------------------------------------------------------------

void DeviceImpl::lock(client_addr *cl,int validity)
{

//
// Check if the device is already locked and if it is a valid lock
// If the lock is not valid any more, clear it
//

	if (ext->device_locked == true)
	{
		if (valid_lock() == true)
		{
			if (*cl != *(ext->locker_client))
			{
				TangoSys_OMemStream o;
				o << "Device " << get_name() << " is already locked by another client" << ends;
				Except::throw_exception((const char *)"API_DeviceLocked",o.str(),
								(const char *)"Device_Impl::lock");
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

	ext->device_locked = true;
	if (ext->locker_client == NULL)
	{
		ext->locker_client = new client_addr(*cl);
	}
	
	ext->locking_date = time(NULL);
	ext->lock_validity = validity;
	ext->lock_ctr++;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::relock
// 
// description : 	ReLock the device
//
// args : in : - cl : The client identification
//			   - th_period : The lock validity time
//
//--------------------------------------------------------------------------

void DeviceImpl::relock(client_addr *cl)
{
	
//
// Check if the device is already locked and if it is a valid lock
// A ReLock is valid only if the device is already locked by the same client
// and if this lock is valid
//

	if (ext->device_locked == true)
	{
		if (valid_lock() == true)
		{	
			if (*cl != *(ext->locker_client))
			{
				TangoSys_OMemStream o;
				o << get_name() << ": ";
				o << "Device " << get_name() << " is already locked by another client" << ends;
				Except::throw_exception((const char *)"API_DeviceLocked",o.str(),
								(const char *)"Device_Impl::relock");
			}
			
			ext->device_locked = true;
			ext->locking_date = time(NULL);
		}
		else
		{
			TangoSys_OMemStream o;
			o << get_name() << ": ";
			o << "Device " << get_name() << " is not locked. Can't re-lock it" << ends;
			Except::throw_exception((const char *)"API_DeviceNotLocked",o.str(),
							(const char *)"Device_Impl::relock");			
		}
	}
	else
	{
		TangoSys_OMemStream o;
		o << get_name() << ": ";
		o << "Device " << get_name() << " is not locked. Can't re-lock it" << ends;
		Except::throw_exception((const char *)"API_DeviceNotLocked",o.str(),
						(const char *)"Device_Impl::relock");		
	}

}
//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::unlock
// 
// description : 	Unlock the device
//
//--------------------------------------------------------------------------

Tango::DevLong DeviceImpl::unlock(bool forced)
{

//
// Check if the device is already locked and if it is a valid lock
// If the lock is not valid any more, clear it
//

	if (ext->device_locked == true)
	{
		if (valid_lock() == true)
		{
			client_addr *cl = get_client_ident();

			if (forced == false)
			{
				if (*cl != *(ext->locker_client))
				{
					TangoSys_OMemStream o;
					o << "Device " << get_name() << " is locked by another client, can't unlock it" << ends;
					Except::throw_exception((const char *)"API_DeviceLocked",o.str(),
								(const char *)"Device_Impl::unlock");
				}
			}
		}
	}

	if (ext->lock_ctr > 0)
		ext->lock_ctr--;
	if ((ext->lock_ctr <= 0) || (forced == true))
		basic_unlock(forced);
	
	return ext->lock_ctr;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::basic_unlock
// 
// description : 	Mark the device as unlocked
//
//--------------------------------------------------------------------------

void DeviceImpl::basic_unlock(bool forced)
{
	ext->device_locked = false;
	if (forced == true)
		ext->old_locker_client = ext->locker_client;
	else
		delete ext->locker_client;
	ext->locker_client = NULL;
	ext->lock_ctr = 0;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::valid_lock
// 
// description : 	Check lock validity (according to lock validity time)
//
// This method returns true if the lock is still valid. Otherwise, returns
// false
//
//--------------------------------------------------------------------------

bool DeviceImpl::valid_lock()
{
	time_t now = time(NULL);
	if (now > (ext->locking_date + ext->lock_validity))
		return false;
	else
		return true;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::lock_status
// 
// description : 	Build a device locking status
//
// This method returns a sequence with longs and strings
//
// The strings contain:
//	1 - The locker process hostname
//	2 - The java main class (in case of Java locker)
//	3 - A string which summarizes the locking status
// The longs contain:
//	1 - A locked flag (0 means not locked, 1 means locked)
//	2 - The locker process PID (C++ client)
//	3 - The locker UUID (Java client) which needs 4 longs
//
//--------------------------------------------------------------------------

Tango::DevVarLongStringArray *DeviceImpl::lock_status()
{
	Tango::DevVarLongStringArray *dvlsa = new Tango::DevVarLongStringArray();
	dvlsa->lvalue.length(6);
	dvlsa->svalue.length(3);
	
//
// Check if the device is already locked and if it is a valid lock
// If the lock is not valid any more, clear it
//

	if (ext->device_locked == true)
	{
		if (valid_lock() == true)
		{
			ext->lock_status = "Device " + device_name + " is locked by ";
			ostringstream ostr;
			ostr << *(ext->locker_client) << ends;
			ext->lock_status = ext->lock_status + ostr.str();
			
			dvlsa->lvalue[0] = 1;
			dvlsa->lvalue[1] = ext->locker_client->client_pid;
			const char *tmp = ext->locker_client->client_ip;
			dvlsa->svalue[1] = CORBA::string_dup(tmp);
			if (ext->locker_client->client_lang == Tango::JAVA)
			{
				dvlsa->svalue[2] = CORBA::string_dup(ext->locker_client->java_main_class.c_str());
				
				Tango::DevULong64 tmp_data = ext->locker_client->java_ident[0];				
				dvlsa->lvalue[2] = (DevLong)((tmp_data & 0xFFFFFFFF00000000LL) >> 32);
				dvlsa->lvalue[3] = (DevLong)(tmp_data & 0xFFFFFFFF);
				
				tmp_data = ext->locker_client->java_ident[1];
				dvlsa->lvalue[4] = (DevLong)((tmp_data & 0xFFFFFFFF00000000LL) >> 32);
				dvlsa->lvalue[5] = (DevLong)(tmp_data & 0xFFFFFFFF);
			}
			else
			{
				dvlsa->svalue[2] = CORBA::string_dup("Not defined");
				for (long loop = 2;loop < 6;loop++)
					dvlsa->lvalue[loop] = 0;
			}
		}
		else
		{
			basic_unlock();
			ext->lock_status = "Device " + device_name + " is not locked";
			dvlsa->svalue[1] = CORBA::string_dup("Not defined");
			dvlsa->svalue[2] = CORBA::string_dup("Not defined");
			for (long loop = 0;loop < 6;loop++)
				dvlsa->lvalue[loop] = 0;
		}
	}
	else
	{
		ext->lock_status = "Device " + device_name + " is not locked";
		dvlsa->svalue[1] = CORBA::string_dup("Not defined");
		dvlsa->svalue[2] = CORBA::string_dup("Not defined");
		for (long loop = 0;loop < 6;loop++)
			dvlsa->lvalue[loop] = 0;
	}

	dvlsa->svalue[0] = CORBA::string_dup(ext->lock_status.c_str());

	return dvlsa;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::set_locking_param
// 
// description : 	Restore device locking parameter
//
// args : - cl : Locker
//		  - old_cl : Previous locker
//		  - date : Locking date
//		  - ctr : Locking counter
//		  - valid : Locking validity
//
//--------------------------------------------------------------------------

void DeviceImpl::set_locking_param(client_addr *cl,client_addr *old_cl,time_t date,DevLong ctr,DevLong valid)
{
	ext->locker_client = cl;
	ext->old_locker_client = old_cl;
	ext->locking_date = date;
	ext->lock_ctr = ctr;
	ext->device_locked = true;
	ext->lock_validity = valid;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::check_lock
// 
// description : 	Method called for each command_inout operation executed
//			from any client on a Tango device.
//
//--------------------------------------------------------------------------

void DeviceImpl::check_lock(const char *meth,const char *cmd)
{
	if (ext->device_locked == true)
	{
		if (valid_lock() == true)
		{
			client_addr *cl = get_client_ident();
			if (cl->client_ident == false)
			{
				
//
// Old client, before throwing the exception, in case the CORBA operation is
// a command_inout, checks if the command is an "allowed" one
//
				
				if (cmd != NULL)
				{
					if (device_class->is_command_allowed(cmd) == false)
					{
						throw_locked_exception(meth);
					}
				}
				else
					throw_locked_exception(meth);
			}

			if (*cl != *(ext->locker_client))
			{
				
//
// Wrong client, before throwing the exception, in case the CORBA operation is
// a command_inout, checks if the command is an "allowed" one
//
				
				if (cmd != NULL)
				{
					if (device_class->is_command_allowed(cmd) == false)
					{
						throw_locked_exception(meth);
					}
				}
				else
					throw_locked_exception(meth);
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
		if (ext->old_locker_client != NULL)
		{
			if (*cl == (*ext->old_locker_client))
			{
				TangoSys_OMemStream o;
				TangoSys_OMemStream o2;
				o << "Device " << get_name() << " has been unlocked by an administrative client!!!" << ends;
				o2 << "Device_Impl::" << meth << ends;
				Except::throw_exception((const char *)DEVICE_UNLOCKED_REASON,o.str(),o2.str());
			}
			delete ext->old_locker_client;
			ext->old_locker_client = NULL;
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::throw_locked_exception()
// 
// description : 	Throw a DeviceLocked exception
//
//--------------------------------------------------------------------------

void DeviceImpl::throw_locked_exception(const char *meth)
{
	TangoSys_OMemStream o;
	TangoSys_OMemStream o2;
	o << "Device " << get_name() << " is locked by another client" << ends;
	o2 << "Device_Impl::" << meth << ends;
	Except::throw_exception((const char *)"API_DeviceLocked",o.str(),o2.str());	
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::data_into_net_obj
// 
// description : 	Put the attribute data within the object used on the
//					wire to transfer the attribute. For IDL release <= 3,
//					it's an Any object. Then, it is an IDL union
//
// argument: in :
//
//--------------------------------------------------------------------------

void DeviceImpl::data_into_net_object(Attribute &att,AttributeValueList_3 *back,
										AttributeValueList_4 *back4,
										long index,AttrWriteType w_type,bool del_seq)
{

//
// A big switch according to attribute data type
//
						
	switch (att.get_data_type())
	{
		case Tango::DEV_SHORT :
		{
			Tango::DevVarShortArray *ptr = att.get_short_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.short_att_value(dummy_short_att_value);

				DevVarShortArray &the_seq = (*back4)[index].value.short_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());

				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}

		case Tango::DEV_LONG :
		{
			Tango::DevVarLongArray *ptr = att.get_long_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.long_att_value(dummy_long_att_value);

				DevVarLongArray &the_seq = (*back4)[index].value.long_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
							
		case Tango::DEV_LONG64 :
		{
			Tango::DevVarLong64Array *ptr = att.get_long64_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.long64_att_value(dummy_long64_att_value);

				DevVarLong64Array &the_seq = (*back4)[index].value.long64_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());

				if (ptr->release() == true)			
					ptr->get_buffer(true);				
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
				
		case Tango::DEV_DOUBLE :
		{
			Tango::DevVarDoubleArray *ptr = att.get_double_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.double_att_value(dummy_double_att_value);

				DevVarDoubleArray &the_seq = (*back4)[index].value.double_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
				
		case Tango::DEV_STRING :
		{
			Tango::DevVarStringArray *ptr = att.get_string_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.string_att_value(dummy_string_att_value);

				DevVarStringArray &the_seq = (*back4)[index].value.string_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
			
		case Tango::DEV_FLOAT :
		{
			Tango::DevVarFloatArray *ptr = att.get_float_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.float_att_value(dummy_float_att_value);

				DevVarFloatArray &the_seq = (*back4)[index].value.float_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}

		case Tango::DEV_BOOLEAN :
		{
			Tango::DevVarBooleanArray *ptr = att.get_boolean_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.bool_att_value(dummy_boolean_att_value);

				DevVarBooleanArray &the_seq = (*back4)[index].value.bool_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}

		case Tango::DEV_USHORT :
		{
			Tango::DevVarUShortArray *ptr = att.get_ushort_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.ushort_att_value(dummy_ushort_att_value);

				DevVarUShortArray &the_seq = (*back4)[index].value.ushort_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}

		case Tango::DEV_UCHAR :
		{
			Tango::DevVarCharArray *ptr = att.get_uchar_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.uchar_att_value(dummy_uchar_att_value);

				DevVarCharArray &the_seq = (*back4)[index].value.uchar_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
			
		case Tango::DEV_ULONG :
		{
			Tango::DevVarULongArray *ptr = att.get_ulong_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.ulong_att_value(dummy_ulong_att_value);

				DevVarULongArray &the_seq = (*back4)[index].value.ulong_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());

				if (ptr->release() == true)			
					ptr->get_buffer(true);				
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
			
		case Tango::DEV_ULONG64 :
		{
			Tango::DevVarULong64Array *ptr = att.get_ulong64_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.ulong64_att_value(dummy_ulong64_att_value);

				DevVarULong64Array &the_seq = (*back4)[index].value.ulong64_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}
			
		case Tango::DEV_STATE :
		{
			Tango::DevVarStateArray *ptr = att.get_state_value();
			if (back != NULL)
			{
				(*back)[index].value <<= *ptr;
			}
			else
			{
				(*back4)[index].value.state_att_value(dummy_state_att_value);

				DevVarStateArray &the_seq = (*back4)[index].value.state_att_value();
				the_seq.replace(ptr->length(),ptr->length(),ptr->get_buffer(),ptr->release());
				
				if (ptr->release() == true)			
					ptr->get_buffer(true);
			}
			if (del_seq == true)
				delete ptr;
			break;
		}

		case Tango::DEV_ENCODED :
		{
			if (back4 == NULL)
			{							
				(*back)[index].err_list.length(1);
				(*back)[index].err_list[0].severity = Tango::ERR;
				(*back)[index].err_list[0].reason = CORBA::string_dup("API_NotSupportedFeature");
				(*back)[index].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_no_except");
				(*back)[index].err_list[0].desc = CORBA::string_dup("The DevEncoded data type is available only for device implementing IDL 4 and above");
				(*back)[index].quality = Tango::ATTR_INVALID;
				(*back)[index].name = CORBA::string_dup(att.get_name().c_str());
				clear_att_dim((*back)[index]);								
			}
			else
			{
				(*back4)[index].value.encoded_att_value(dummy_encoded_att_value);
				DevVarEncodedArray &the_seq = (*back4)[index].value.encoded_att_value();
				
				Tango::DevVarEncodedArray *ptr = att.get_encoded_value();
				if ((w_type == Tango::READ) || (w_type == Tango::WRITE))
					the_seq.length(1);
				else
					the_seq.length(2);
		
				the_seq[0].encoded_format = CORBA::string_dup((*ptr)[0].encoded_format);

				if (ptr->release() == true)
				{
					unsigned long nb_data = (*ptr)[0].encoded_data.length();
					the_seq[0].encoded_data.replace(nb_data,nb_data,(*ptr)[0].encoded_data.get_buffer(true),true);
					(*ptr)[0].encoded_data.replace(0,0,NULL,false);
				}
				else
					the_seq[0].encoded_data.replace((*ptr)[0].encoded_data.length(),(*ptr)[0].encoded_data.length(),(*ptr)[0].encoded_data.get_buffer());
				
				if ((w_type == Tango::READ_WRITE) || (w_type == Tango::READ_WITH_WRITE))
				{
					the_seq[1].encoded_format = CORBA::string_dup((*ptr)[1].encoded_format);
					the_seq[1].encoded_data.replace((*ptr)[1].encoded_data.length(),(*ptr)[1].encoded_data.length(),(*ptr)[1].encoded_data.get_buffer());
				}
				if (del_seq == true)
					delete ptr;
			}
			break;
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::polled_data_into_net_obj
// 
// description : 	Put the attribute data within the object used on the
//					wire to transfer the attribute. For IDL release <= 3,
//					it's an Any object. Then, it is an IDL union
//
// argument: in :
//
//--------------------------------------------------------------------------

void DeviceImpl::polled_data_into_net_object(AttributeValueList_3 *back,
										AttributeValueList_4 *back4,
										long index,long type,long vers,PollObj *polled_attr,
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
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarShortArray &union_seq = att_val.value.short_att_value();
				new_tmp_sh = new DevVarShortArray(union_seq.length(),
									union_seq.length(),
									const_cast<short *>(union_seq.get_buffer()),
									false);
			}
			else
			{
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);
				att_val.value >>= tmp_sh;
				new_tmp_sh = new DevVarShortArray(tmp_sh->length(),
								tmp_sh->length(),
								const_cast<short *>(tmp_sh->get_buffer()),
								false);
			}
			(*back)[index].value <<= new_tmp_sh;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.short_att_value(att_val.value.short_att_value());
		}
		break;

    case Tango::DEV_DOUBLE :
		if (back != NULL)
		{	
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarDoubleArray &union_seq = att_val.value.double_att_value();
				new_tmp_db = new DevVarDoubleArray(union_seq.length(),
									union_seq.length(),
									const_cast<double *>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);		
				att_val.value >>= tmp_db;		
				new_tmp_db = new DevVarDoubleArray(tmp_db->length(),
							   tmp_db->length(),
							   const_cast<double *>(tmp_db->get_buffer()),
							   false);
			}
			(*back)[index].value <<= new_tmp_db;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.double_att_value(att_val.value.double_att_value());
		}						
		break;

	case Tango::DEV_LONG :
		if (back != NULL)
		{	
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarLongArray &union_seq = att_val.value.long_att_value();
				new_tmp_lg = new DevVarLongArray(union_seq.length(),
									union_seq.length(),
									const_cast<DevLong *>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);		
				att_val.value >>= tmp_lg;		
				new_tmp_lg = new DevVarLongArray(tmp_lg->length(),
							   tmp_lg->length(),
							   const_cast<DevLong *>(tmp_lg->get_buffer()),
							   false);
			}
			(*back)[index].value <<= new_tmp_lg;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.long_att_value(att_val.value.long_att_value());
		}			
		break;

	case Tango::DEV_LONG64 :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarLong64Array &union_seq = att_val.value.long64_att_value();
				new_tmp_lg64 = new DevVarLong64Array(union_seq.length(),
									union_seq.length(),
									const_cast<DevLong64 *>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);		
				att_val.value >>= tmp_lg64;
				new_tmp_lg64 = new DevVarLong64Array(tmp_lg64->length(),tmp_lg64->length(),
						 				const_cast<DevLong64 *>(tmp_lg64->get_buffer()),false);
			}
			(*back)[index].value <<= new_tmp_lg64;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.long64_att_value(att_val.value.long64_att_value());
		}		
		break;

	case Tango::DEV_STRING :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarStringArray &union_seq = att_val.value.string_att_value();
				new_tmp_str = new DevVarStringArray(union_seq.length(),
									union_seq.length(),
									const_cast<char **>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);				
				att_val.value >>= tmp_str;
				new_tmp_str = new DevVarStringArray(tmp_str->length(),
								tmp_str->length(),
								const_cast<char **>(tmp_str->get_buffer()),
								false);
			}
			(*back)[index].value <<= new_tmp_str;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.string_att_value(att_val.value.string_att_value());
		}			
		break;

	case Tango::DEV_FLOAT :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarFloatArray &union_seq = att_val.value.float_att_value();
				new_tmp_fl = new DevVarFloatArray(union_seq.length(),
									union_seq.length(),
									const_cast<float *>(union_seq.get_buffer()),
									false);
			}
			else
			{
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);					
				att_val.value >>= tmp_fl;
				new_tmp_fl = new DevVarFloatArray(tmp_fl->length(),
						  	tmp_fl->length(),
						  	const_cast<float *>(tmp_fl->get_buffer()),
						  	false);
			}
			(*back)[index].value <<= new_tmp_fl;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.float_att_value(att_val.value.float_att_value());
		}	
		break;

	case Tango::DEV_BOOLEAN :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarBooleanArray &union_seq = att_val.value.bool_att_value();
				new_tmp_boo = new DevVarBooleanArray(union_seq.length(),
									union_seq.length(),
									const_cast<DevBoolean *>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);							
				att_val.value >>= tmp_boo;		
				new_tmp_boo = new DevVarBooleanArray(tmp_boo->length(),
						   tmp_boo->length(),
						   const_cast<DevBoolean *>(tmp_boo->get_buffer()),
						   false);
			}
			(*back)[index].value <<= new_tmp_boo;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.bool_att_value(att_val.value.bool_att_value());
		}
		break;

	case Tango::DEV_USHORT :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarUShortArray &union_seq = att_val.value.ushort_att_value();
				new_tmp_ush = new DevVarUShortArray(union_seq.length(),
									union_seq.length(),
									const_cast<DevUShort *>(union_seq.get_buffer()),
									false);
			}
			else
			{
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);				
				att_val.value >>= tmp_ush;
				new_tmp_ush = new DevVarUShortArray(tmp_ush->length(),
						 			tmp_ush->length(),
						 			const_cast<DevUShort *>(tmp_ush->get_buffer()),
						 			false);
			}
			(*back)[index].value <<= new_tmp_ush;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.ushort_att_value(att_val.value.ushort_att_value());
		}
		break;

	case Tango::DEV_UCHAR :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarCharArray &union_seq = att_val.value.uchar_att_value();
				new_tmp_uch = new DevVarUCharArray(union_seq.length(),
									union_seq.length(),
									const_cast<DevUChar *>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);		
				att_val.value >>= tmp_uch;
				new_tmp_uch = new DevVarCharArray(tmp_uch->length(),
							tmp_uch->length(),
							const_cast<DevUChar *>(tmp_uch->get_buffer()),
							false);
			}
			(*back)[index].value <<= new_tmp_uch;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.uchar_att_value(att_val.value.uchar_att_value());
		}
		break;

	case Tango::DEV_ULONG :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarULongArray &union_seq = att_val.value.ulong_att_value();
				new_tmp_ulg = new DevVarULongArray(union_seq.length(),
									union_seq.length(),
									const_cast<DevULong *>(union_seq.get_buffer()),
									false);
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);				
				att_val.value >>= tmp_ulg;
				new_tmp_ulg = new DevVarULongArray(tmp_ulg->length(),tmp_ulg->length(),
						 		const_cast<DevULong *>(tmp_ulg->get_buffer()),false);
			}
			(*back)[index].value <<= new_tmp_ulg;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.ulong_att_value(att_val.value.ulong_att_value());
		}
		break;

	case Tango::DEV_ULONG64 :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				DevVarULong64Array &union_seq = att_val.value.ulong64_att_value();
				new_tmp_ulg64 = new DevVarULong64Array(union_seq.length(),
									union_seq.length(),
									const_cast<DevULong64 *>(union_seq.get_buffer()),
									false);
			}
			else
			{
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);				
				att_val.value >>= tmp_ulg64;
				new_tmp_ulg64 = new DevVarULong64Array(tmp_ulg64->length(),tmp_ulg64->length(),
						 		const_cast<DevULong64 *>(tmp_ulg64->get_buffer()),false);
			}
			(*back)[index].value <<= new_tmp_ulg64;
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			(*back4)[index].value.ulong64_att_value(att_val.value.ulong64_att_value());
		}		
		break;

	case Tango::DEV_STATE :
		if (back != NULL)
		{
			if (vers >= 4)
			{
				AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
				if (att_val.value._d() == DEVICE_STATE)
				{
					sta = att_val.value.dev_state_att();
					(*back)[index].value <<= sta;
				}
				else if (att_val.value._d() == ATT_STATE)
				{
					DevVarStateArray &union_seq = att_val.value.state_att_value();
					new_tmp_state = new DevVarStateArray(union_seq.length(),
										union_seq.length(),
										const_cast<DevState *>(union_seq.get_buffer()),
										false);
					(*back)[index].value <<= new_tmp_state;
				}
			}
			else
			{	
				AttributeValue_3 &att_val = polled_attr->get_last_attr_value_3(false);		
				CORBA::TypeCode_var ty;
				ty = att_val.value.type();

				if (ty->kind() == CORBA::tk_enum)
				{
					att_val.value >>= sta;
					(*back)[index].value <<= sta;
				}
				else
				{
					att_val.value >>= tmp_state;
					new_tmp_state = new DevVarStateArray(tmp_state->length(),tmp_state->length(),
									 const_cast<DevState *>(tmp_state->get_buffer()),false);
					(*back)[index].value <<= new_tmp_state;
				}
			}
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			if (att_val.value._d() == DEVICE_STATE)
			{
				sta = att_val.value.dev_state_att();
				(*back4)[index].value.dev_state_att(sta);
			}
			else if (att_val.value._d() == ATT_STATE)
			{
				DevVarStateArray &union_seq = att_val.value.state_att_value();
				(*back4)[index].value.state_att_value(union_seq);
			}
		}
		break;
		
	case Tango::DEV_ENCODED:
		if (back != NULL)
		{
			TangoSys_OMemStream o;
			o << "Data type for attribute " << names[index] << " is DEV_ENCODED.";
			o << " It's not possible to retrieve this data type through the interface you are using (IDL V3)" << ends;

			(*back)[index].err_list.length(1);
			(*back)[index].err_list[0].severity = Tango::ERR;
			(*back)[index].err_list[0].reason = CORBA::string_dup("API_NotSupportedFeature");
			(*back)[index].err_list[0].origin = CORBA::string_dup("Device_3Impl::read_attributes_from_cache");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string s = o.str();
			(*back)[index].err_list[0].desc = CORBA::string_dup(s.c_str());
#else
			char *tmp_str = o.str();
			(*back)[index].err_list[0].desc = CORBA::string_dup(tmp_str);
			delete[]tmp_str;
#endif
			(*back)[index].quality = Tango::ATTR_INVALID;
			(*back)[index].name = CORBA::string_dup(names[index]);
			clear_att_dim((*back)[index]);
		}
		else
		{
			AttributeValue_4 &att_val = polled_attr->get_last_attr_value_4(false);
			DevVarEncodedArray &polled_seq = att_val.value.encoded_att_value();
			 
			unsigned int nb_encoded = polled_seq.length();
			
			(*back4)[index].value.encoded_att_value(dummy_encoded_att_value);
			DevVarEncodedArray &the_seq = (*back4)[index].value.encoded_att_value();
						
			the_seq.length(nb_encoded);
			for (unsigned int loop = 0;loop < nb_encoded;loop++)
			{
				the_seq[loop].encoded_format = CORBA::string_dup(polled_seq[loop].encoded_format);
				unsigned char *tmp_enc = polled_seq[loop].encoded_data.get_buffer();
				unsigned int nb_data = polled_seq[loop].encoded_data.length();
				the_seq[loop].encoded_data.replace(nb_data,nb_data,tmp_enc);
			}		
		}
	}	
}

} // End of Tango namespace

static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               DServer.cpp
//
// description :        C++ source for the DServer class and its commands. 
//			The class is derived from Device. It represents the
//			CORBA servant object which will be accessed from the
//			network. All commands which can be executed on a
//			DServer object are implemented in this file.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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
// $Log$
// Revision 3.60  2011/01/10 13:09:02  taurel
// - No retry on command to get data for cache during DS startup
// - Only three reties during DbDevExport
// - Device are deleted by omniORB even if not exported into Tango database
//
// Revision 3.59  2010/12/08 15:42:14  taurel
// - During device shutdown, do not wait for POA to effectively destroying
// the device. Rely on the ORB
//
// Revision 3.58  2010/09/30 08:12:59  taurel
// - Add a new way to write class_factory as a function instead as
// DServer classs method
//
// Revision 3.57  2010/09/24 14:06:15  taurel
// - For Python DS, do not give full device ownership to the POA.
// Otherwise, a python DS crashes at exit.
//
// Revision 3.56  2010/09/09 13:45:22  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.55  2010/08/30 12:28:01  taurel
// - Commit just to make tkcvs happy. No real difference with previous release. Just a space adding/missing somewhere
//
// Revision 3.54  2010/08/25 11:41:27  taurel
// - Fix some bugs preventing dynamic attributes management (in some cases)
//
// Revision 3.53  2010/06/23 09:14:50  taurel
// - Increment sleeping time (up to 400mS) in case we have to wait for
// request end before the POA destroys servant
//
// Revision 3.52  2010/06/23 09:03:09  taurel
// - The admin device Restart a device command also need to be modified due to the new servant reference counting use (fast DS shutdown)
//
// Revision 3.51  2010/06/21 12:38:23  taurel
// - Implement a much faster server shutdown sequence
//
// Revision 3.50  2010/06/18 13:57:09  taurel
// - Add a way (using properties) to define a minimum polling period
//
// Revision 3.49  2010/06/18 07:45:47  taurel
// - In case of locked device, polling and logging related commands are
// allowed only for the locker process
//
// Revision 3.48  2010/05/26 09:15:35  taurel
// - Another commit after merge with the bug fixes branch
//
// Revision 3.47  2010/04/13 08:32:29  taurel
// - The previous fix about memory leaking was a bit too strong!
//
// Revision 3.46  2010/04/12 12:57:38  taurel
// - Fix a memory leak in case of server started without DB
//
// Revision 3.45  2010/01/20 07:53:02  taurel
// - Commit after merge with the Release_7_1_1-bugfixes branch
// Revision 3.44.2.2  2010/05/18 08:27:22  taurel
// - Events from device in a DS started with a file as database are now
// back into operation
//
// Revision 3.44.2.1  2010/01/19 10:04:06  taurel
// - Fix bug when DS with polled devices moved from Tango 6 to Tango 7.1.1
// (polling pool size = 0!!)
//
// Revision 3.44  2009/10/23 14:36:27  taurel
// - Tango 7.1.1
// - Fix bugs 2880372 and 2881841
// - Now support event in case of Tango system with multi db server
// - The polling threads start with polling inactive
//
// Revision 3.43  2009/09/18 09:18:06  taurel
// - End of attribute serialization implementation?
//
// Revision 3.42  2009/09/01 07:41:31  taurel
// - Commit after test suite for Tango 7.1
//
// Revision 3.41  2009/08/27 07:23:45  taurel
// - Commit after another merge with Release_7_0_2-bugfixes branch
//
// Revision 3.40  2009/06/17 08:52:08  taurel
// - Commit after a merge with branch Release_7_0_2-bugfixes
// Revision 3.39.2.2  2009/06/22 06:58:09  taurel
// - Fix bug introduced by previous bug fix. It makes the db server crashes.
//
// Revision 3.39.2.1  2009/06/12 08:28:51  taurel
// - Fix bug when using events in multi Tango host environment.
// The TANGO_HOST is now transferred within the even tin the fixed
// header event_type field.
// The DS admin device EventSubscriptionChange command now returns with which Tango lib it is runnig.
// This allows the client to know if the tango host info will be transmitted within the event
//
// Revision 3.39  2009/03/23 15:21:40  taurel
// - Forgot to remove some commented code when debugging  on Windows Vista
//
// Revision 3.38  2009/03/13 09:33:29  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.37  2009/02/03 15:15:08  jensmeyer
// Added a QuerySubDevice command to read the list of opened sub device
// connections.
//
// Revision 3.36  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.35  2008/11/18 09:28:56  taurel
// - Ported to gcc 4.3
// - Removed some cout messages
//
// Revision 3.34  2008/10/06 15:01:09  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.33  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.32  2008/10/02 12:24:43  taurel
// - The user now has the possibility to install its own event loop
//
// Revision 3.31  2008/10/02 09:09:47  taurel
// - First implementation of multiple polling thread(s)
//
// Revision 3.30  2008/05/20 12:44:11  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.29  2008/01/25 15:44:51  taurel
// - Some changes in the Db cache
// - A lighter system to shutdown DS in case of dynamic attribute
//
// Revision 3.28  2008/01/18 07:58:12  taurel
// - Protect the unregister_server call against any kind of exception
//
// Revision 3.27  2008/01/09 13:03:47  jensmeyer
// Added a correct clean-up when exiting a device server which
// is also used as an event client.
// Added a EventConsumer::disconnect_from_notifd method as for
// the EventSupplier.
// Revision 3.25.2.4  2008/01/03 16:05:52  taurel
// - Some changes in locking feature implementation
//
// Revision 3.26  2007/12/19 15:42:43  taurel
// - Add some cleanup when quitting applis or device server (notifd objects)
// Revision 3.25.2.3  2007/12/20 14:29:01  taurel
// - Some more work on locking
//
// Revision 3.25.2.2  2007/12/19 15:54:47  taurel
// - Still some work going on for the locking feature
//
// Revision 3.25.2.1  2007/11/22 12:33:10  taurel
// - First part of the device locking implementation
//
// Revision 3.25  2007/10/26 11:31:52  taurel
// - Simply remove some log message sin source code
//
// Revision 3.24  2007/10/16 08:22:51  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
// Revision 3.23  2007/04/20 14:40:32  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.22  2007/01/19 10:24:04  taurel
// - Fix bug in the delete_devices() method which makes the Util::get_device_list_by_class() method generating core in some cases
//
// Revision 3.21  2006/06/28 09:21:28  taurel
// - Remove a 1 second sleep at the beginning of the thread executing the Kill command
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventsupplier.h>

#include <new>
#include <algorithm>
#include <math.h>

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

//+----------------------------------------------------------------------------
//
// method : 		DServer::DServer(string &s)
// 
// description : 	constructor for DServer object
//
// in :			- cp_ptr : The class object pointer
//			- n : The device name 
//			- d : The device description (default to "not initialised")
//			- s : The device state (default to UNKNOWN)
//			- st : The device status (default to "Not initialised")
//
//-----------------------------------------------------------------------------

DServer::DServer(DeviceClass *cl_ptr,const char *n,const char *d,Tango::DevState s,const char *st)
:Device_4Impl(cl_ptr,n,d,s,st)
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
	heartbeat_started = false;
	
	polling_th_pool_size = DEFAULT_POLLING_THREADS_POOL_SIZE;
	optimize_pool_usage = true;

	from_constructor = true;	
	init_device();
	from_constructor = false;
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
		
	cout3 << "DServer::DSserver() create dserver " << device_name << endl;

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
// Retrieve device(s) name list from the database. No need to implement
// a retry here (in case of db server restart) because the db reconnection
// is forced by the get_property call executed during xxxClass construction
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
						o << "Database error while trying to retrieve device list for class " << class_list[i]->get_name().c_str() << ends;
					
						Except::throw_exception((const char *)"API_DatabaseAccess",
				                			o.str(),
				                			(const char *)"Dserver::init_device");			
					}				
					
					long nb_dev = na.size();
					Tango::DevVarStringArray dev_list(nb_dev);
					dev_list.length(nb_dev);
					
					for (int l = 0;l < nb_dev;l++)
						dev_list[l] = na.value_string[l].c_str();
				
					cout4 << dev_list.length() << " device(s) defined" << endl;

//
// Create all device(s)
//
				
					{
						AutoTangoMonitor sync(class_list[i]);
						class_list[i]->device_factory(&dev_list);
					}

//
// Set value for each device with memorized writable attr
// This is necessary only if db is used
// For Python device server, writing the attribute will tak the Python lock.
// If we already have it --> dead lock.
// Release the python lock if we already have it before calling the set_memorized_values
// method
//
					
					PyLock *lock_ptr;
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
				}
				else
				{
				
					vector<string> &list = class_list[i]->get_nodb_name_list();
					Tango::DevVarStringArray *dev_list_nodb = new Tango::DevVarStringArray();
					if (i != class_list.size() - 1)
						class_list[i]->device_name_factory(list);
					else
					{
						if (tg->get_cmd_line_name_list().size() == 0)
							class_list[i]->device_name_factory(list);
						else
							list = tg->get_cmd_line_name_list();
					}					

					if (list.empty() == true)
					{				
						dev_list_nodb->length(1);
						(*dev_list_nodb)[0] = CORBA::string_dup("NoName");
					}
					else
					{
						(*dev_list_nodb) << list;
					}
						
//
// Create all device(s)
//
		
					{
						AutoTangoMonitor sync(class_list[i]);
						class_list[i]->device_factory(dev_list_nodb);
					}

					delete dev_list_nodb;
				}
			}
		}
		
		man_state = manager->get_state();
		if (man_state == PortableServer::POAManager::DISCARDING)	
			manager->activate();
								
	}
	catch (bad_alloc)
	{
//
// If the class_factory method have not been successfully executed, erase
// all classes already built. If the error occurs during the command or device
// factories, erase only the following classes
//

		TangoSys_OMemStream o;
		
		if (class_factory_done == false)
		{
			long class_err = class_list.size() + 1;
			o << "Can't allocate memory in server while creating class number " << class_err << ends;
			if (class_list.empty() == false)
			{

				for (unsigned long j = 0;j < class_list.size();j++)
				{
					if (class_list[i]->is_py_class() == false)
						delete class_list[i];
					else
						class_list[i]->delete_class();			
				}
				class_list.clear();
			}
		}
		else
		{
			o << "Can't allocate memory in server while building command(s) or device(s) for class number " << i + 1 << ends;
			for (unsigned long j = i;j < class_list.size();j++)
			{
				if (class_list[i]->is_py_class() == false)
					delete class_list[i];
				else
					class_list[i]->delete_class();	
			}
			class_list.erase(class_list.begin() + i,class_list.end());
		}
		
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      o.str(),
				      (const char *)"DServer::init_device");
	}
	catch (Tango::NamedDevFailedList &)
	{	
	
//
// If the class_factory method have not been successfully executed, erase
// all classes already built. If the error occurs during the command or device
// factories, erase only the following classes
//

		if (class_factory_done == false)
		{
			if (class_list.empty() == false)
			{
				for (unsigned long j = 0;j < class_list.size();j++)
				{
					if (class_list[i]->is_py_class() == false)
						delete class_list[i];
					else
						class_list[i]->delete_class();					
				}
				class_list.clear();
			}
		}
		else
		{
			for (unsigned long j = i;j < class_list.size();j++)
			{
				if (class_list[i]->is_py_class() == false)
					delete class_list[i];
				else
					class_list[i]->delete_class();	
			}
			class_list.erase(class_list.begin() + i,class_list.end());
		}
		throw;
	}
	catch (Tango::DevFailed)
	{

//
// If the class_factory method have not been successfully executed, erase
// all classes already built. If the error occurs during the command or device
// factories, erase only the following classes
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
		throw;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::~DServer
// 
// description : 	destructor for DServer object
//
//-----------------------------------------------------------------------------

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


//+----------------------------------------------------------------------------
//
// method : 		DServer::delete_devices
// 
// description : 	Call destructor for all objects registered in the server
//
//-----------------------------------------------------------------------------

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

				vector<DeviceImpl *> &devs = class_list[i]->get_device_list();
				unsigned long nb_dev = devs.size();
				for (loop = 0;loop < nb_dev;loop++)
				{	
				
//
// Clear vectors used to memorize info used to clean db
// in case of devices with dyn attr removed during device
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
// Clean-up db (dyn attribute)
//
					

					if (tg->get_polled_dyn_attr_names().size() != 0)
						tg->clean_attr_polled_prop();			
					if (tg->get_all_dyn_attr_names().size() != 0)
						tg->clean_dyn_attr_prop();

//
// Wait for POA to destroy the object before going to the next one
// Limit this waiting time to 200 mS
//

					vector<DeviceImpl *>::iterator it = devs.begin();
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

//+----------------------------------------------------------------------------
//
// method : 		DServer::add_class()
// 
// description : 	To add a new class to the class list vector
//
// out :		class_ptr : pointer to DeviceClass object
//
//-----------------------------------------------------------------------------

void DServer::add_class(DeviceClass *class_ptr)
{
	class_list.push_back(class_ptr);
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::query_class()
// 
// description : 	command to read all the classes used in a device server
//			process
////
// out :		The class name list in a strings sequence
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_class()
{
	NoSyncModelTangoMonitor sync(this);
	
	cout4 << "In query_class command" << endl;
	
	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret;
	
	try
	{
		ret = new Tango::DevVarStringArray(nb_class);
                ret->length(nb_class);
	
		for (int i = 0;i < nb_class;i++)
		{
			(*ret)[i] = class_list[i]->get_name().c_str();
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DServer::query_class");
	}		
	return(ret);
	
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::query_device()
// 
// description : 	command to read all the devices implemented by a device
//			server process
////
// out :		The device name list in a strings sequence
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_device()
{
	NoSyncModelTangoMonitor mon(this);
	
	cout4 << "In query_device command" << endl;
	
	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret;
	vector<string> vs;
		
	try
	{
		ret = new Tango::DevVarStringArray(DefaultMaxSeq);
			
		for (int i = 0;i < nb_class;i++)
		{
			long nb_dev = class_list[i]->get_device_list().size();
			string &class_name = class_list[i]->get_name();
			for (long j = 0;j < nb_dev;j++)
			{
				vs.push_back(class_name + "::" + (class_list[i]->get_device_list())[j]->get_name());
			}
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"DServer::query_device");
	}

	int nb_dev = vs.size();
	ret->length(nb_dev);
	for (int k = 0;k < nb_dev;k++)
		(*ret)[k] = CORBA::string_dup(vs[k].c_str());
		
	return(ret);
}


//+----------------------------------------------------------------------------
//
// method :         DServer::query_sub_device()
// 
// description :    command to read all the sub devices used by a device
//                  server process
////
// out :            The sub device name list in a sequence of strings
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_sub_device()
{
	NoSyncModelTangoMonitor mon(this);
	
	cout4 << "In query_sub_device command" << endl;
	
	Tango::DevVarStringArray *ret;
	
	Tango::Util *tg = Tango::Util::instance();
	ret = tg->get_sub_dev_diag().get_sub_devices();
	
	return(ret);
}



//+----------------------------------------------------------------------------
//
// method : 		DServer::restart()
// 
// description : 	command to restart a device
////
// out :		The device name to be re-started
//
//-----------------------------------------------------------------------------

void DServer::restart(string &d_name)
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In restart method" << endl;

//
// Change device name to lower case letters
// and remove extra field in name in case of
// (#dbase=xxx or protocol specif)
//
// Check if the wanted device exists in each class
//

	bool found = false;
	unsigned int nb_class = class_list.size();
	unsigned long i;
	
	string lower_d_name(d_name);
	transform(lower_d_name.begin(),lower_d_name.end(),lower_d_name.begin(),::tolower);
	string::size_type pos;
	if ((pos = lower_d_name.find('#')) != string::npos)
		lower_d_name.erase(pos);
	if ((pos = lower_d_name.find("://")) != string::npos)
	{
		pos = pos + 3;
		if ((pos = lower_d_name.find('/',pos)) != string::npos)
			lower_d_name.erase(0,pos + 1);
	}
				
	vector<DeviceImpl *>::iterator ite,ite_end;
	for (i = 0;i < nb_class;i++)
	{
		vector<DeviceImpl *> &dev_list = class_list[i]->get_device_list();
		ite_end = dev_list.end();
		for (ite = dev_list.begin();ite != dev_list.end();ite++)
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
		cout3 << "Device " << d_name << " not found in server !" << endl;
		TangoSys_OMemStream o;
		o << "Device " << d_name << " not found" << ends;
		Except::throw_exception((const char *)"API_DeviceNotFound",
				        o.str(),
				        (const char *)"Dserver::restart()");
	}
	
	DeviceImpl *dev_to_del = *ite;
	
//
// If the device is locked and if the client is not the lock owner,
// refuse to do the job
//

	check_lock_owner(dev_to_del,"restart",d_name.c_str());
	
//
// clean the sub-device list for this device
//
	Tango::Util *tg = Tango::Util::instance();
	tg->get_sub_dev_diag().remove_sub_devices (dev_to_del->get_name());
	tg->get_sub_dev_diag().set_associated_device(dev_to_del->get_name());

//
// Remove ourself from device list
//

	class_list[i]->get_device_list().erase(ite);
	
//
// Get device name, class pointer, polled object list and event parameter
//

	DeviceClass *dev_cl = dev_to_del->get_device_class();	
	Tango::DevVarStringArray name(1);
	name.length(1);
	
	name[0] = lower_d_name.c_str();
	
	vector<PollObj *> &p_obj = dev_to_del->get_poll_obj_list();
	vector<Pol> dev_pol;
	vector<EventPar> eve;
	
	for (i = 0;i < p_obj.size();i++)
	{
		Pol tmp;
		tmp.type = p_obj[i]->get_type();
		tmp.upd = p_obj[i]->get_upd();
		tmp.name = p_obj[i]->get_name();
		dev_pol.push_back(tmp);
	}
	
	dev_to_del->get_device_attr()->get_event_param(eve);

//
// Also get device locker parameters if device locked
//
	
	client_addr *cl_addr = NULL;
	client_addr *old_cl_addr;
	time_t l_date;
	DevLong l_ctr,l_valid;
	
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
// Re-create device. Take the monitor in case of class or process serialisation
// model
//

	{
		AutoTangoMonitor sync(dev_cl);
		AutoPyLock PyLo;		
	
		dev_cl->device_factory(&name);
	}

//
// Apply memorized values for memorized attributes (if any)
//

	dev_cl->set_memorized_values(false,dev_cl->get_device_list().size() - 1);
				
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
		send->svalue[0] = CORBA::string_dup(name[0]);
		if (dev_pol[i].type == Tango::POLL_CMD)
			send->svalue[1] = CORBA::string_dup("command");
		else
			send->svalue[1] = CORBA::string_dup("attribute");
		send->svalue[2] = CORBA::string_dup(dev_pol[i].name.c_str());
					
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

	vector<Tango::DeviceImpl *> &d_list = dev_cl->get_device_list();
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
		cout3 << "Not able to find the new device" << endl;
		TangoSys_OMemStream o;
		o << "Not able to find the new device" << ends;
		Except::throw_exception((const char *)"API_DeviceNotFound",
			        	o.str(),
			        	(const char *)"Dserver::restart()");			
	}
	
//
// Re-set event parameters (if needed)
//
	
	for (i = 0;i < eve.size();i++)
	{
		Tango::MultiAttribute *m_attr = new_dev->get_device_attr();
		Tango::Attribute &att = m_attr->get_attr_by_ind(eve[i].attr_id);
		if (eve[i].change == true)
			att.set_change_event_sub();
		if (eve[i].periodic == true)
			att.set_periodic_event_sub();
		if (eve[i].quality == true)
			att.set_quality_event_sub();
		if (eve[i].archive == true)
			att.set_archive_event_sub();
		if (eve[i].user == true)
			att.set_user_event_sub();
	}

//
// Re-lock device if necessary
//
	
	if (cl_addr != NULL)
		new_dev->set_locking_param(cl_addr,old_cl_addr,l_date,l_ctr,l_valid);
		
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::restart_server()
// 
// description : 	command to restart a server (all devices embedded
//			within the server)
//
//-----------------------------------------------------------------------------

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
// Change the POA manager to discarding state. This is necessary to discard all
// request arriving while the server restart.
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
// Destroy and recreate the multi attribute object
//

	MultiAttribute *tmp_ptr;
	try
	{
		tmp_ptr = new MultiAttribute(dev->get_name(),dev->get_device_class());		
	}
	catch (Tango::DevFailed)
	{
		throw;
	}
	delete dev->get_device_attr();
	dev->set_device_attr(tmp_ptr);
	dev->add_state_status_attrs();
				
//	
// Restart device(s)
//

	{
		AutoPyLock PyLo;
		dev->init_device();
	}
	
//
// Restart polling (if any)
//

	tg->polling_configure();

//
// Exit thread
//

	omni_thread::self()->remove_value(key_py_data);
	delete py_data_ptr;
	omni_thread::exit();
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::query_class_prop()
// 
// description : 	command to return the list of property device at class
//			level for the specified class
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_class_prop(string &class_name)
{
	NoSyncModelTangoMonitor sync(this);
	
	cout4 << "In query_class_prop command" << endl;
	
	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret;

//
// Find the wanted class in server and throw exception if not found
//

	transform(class_name.begin(),class_name.end(),class_name.begin(),::tolower);
	int k;
	for (k = 0;k < nb_class;k++)
	{
		string tmp_name(class_list[k]->get_name());
		transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		if (tmp_name == class_name)
			break;
	}
	

	if (k == nb_class)
	{
		TangoSys_OMemStream o;
		o << "Class " << class_name << " not found in device server" << ends;
		Except::throw_exception((const char *)"API_ClassNotFound",o.str(),
				        (const char *)"DServer::query_class_prop");
	}
	

//
// Retrieve class prop vector and returns its content in a DevVarStringArray
//

	vector<string> &wiz = class_list[k]->get_wiz_class_prop();
	long nb_prop = wiz.size();
				
	try
	{
		ret = new Tango::DevVarStringArray(nb_prop);
		ret->length(nb_prop);
	
		for (int i = 0;i < nb_prop;i++)
		{
			(*ret)[i] = CORBA::string_dup(wiz[i].c_str());
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DServer::query_class_prop");
	}
			
	return(ret);
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::query_dev_prop()
// 
// description : 	command to return the list of property device at device
//			level for the specified class
//
//-----------------------------------------------------------------------------

Tango::DevVarStringArray *DServer::query_dev_prop(string &class_name)
{
	NoSyncModelTangoMonitor sync(this);
	
	cout4 << "In query_dev_prop command" << endl;
	
	long nb_class = class_list.size();
	Tango::DevVarStringArray *ret;

//
// Find the wanted class in server and throw exception if not found
//

	transform(class_name.begin(),class_name.end(),class_name.begin(),::tolower);
	int k;
	for (k = 0;k < nb_class;k++)
	{
		string tmp_name(class_list[k]->get_name());
		transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
		if (tmp_name == class_name)
			break;
	}
	

	if (k == nb_class)
	{
		TangoSys_OMemStream o;
		o << "Class " << class_name << " not found in device server" << ends;
		Except::throw_exception((const char *)"API_ClassNotFound",o.str(),
				        (const char *)"DServer::query_dev_prop");
	}
	

//
// Retrieve device prop vector and returns its content in a DevVarStringArray
//

	vector<string> &wiz = class_list[k]->get_wiz_dev_prop();
	long nb_prop = wiz.size();
				
	try
	{
		ret = new Tango::DevVarStringArray(nb_prop);
                ret->length(nb_prop);
	
		for (int i = 0;i < nb_prop;i++)
		{
			(*ret)[i] = CORBA::string_dup(wiz[i].c_str());
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DServer::query_dev_prop");
	}
			
	return(ret);
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::kill()
// 
// description : 	command to kill the device server process. This is done
//			by starting a thread which will kill the process.
//			Starting a thread allows the client to receive
//			something from the server before it is killed
//
//-----------------------------------------------------------------------------

void DServer::kill()
{
	NoSyncModelTangoMonitor mon(this);

	cout4 << "In kill command" << endl;

//
// Create the thread and start it
//

	KillThread *t = new KillThread;
	
	t->start();	
	
}


void *KillThread::run_undetached(void *ptr)
{
	cout4 << "In the killer thread !!!" << endl;
	
	omni_thread::self()->set_value(key_py_data,new PyData());

//
// Shutdown the server
//

	Tango::Util *tg = Tango::Util::instance();
	tg->shutdown_server();
	
	return NULL;
}


//+----------------------------------------------------------------------------
//
// method : 		DServer::create_cpp_class()
// 
// description : 	Create a Cpp Tango class from its name
//
//-----------------------------------------------------------------------------


void DServer::create_cpp_class(const char *cl_name,const char *par_name)
{
	cout4 << "In DServer::create_cpp_class for " << cl_name <<  ", " << par_name << endl;
	string class_name(cl_name);
	string lib_name = class_name;
	
#ifdef _TG_WINDOWS_
	HMODULE mod;

	if ((mod = LoadLibrary(lib_name.c_str())) == NULL)
	{
   		char *str = 0;
		
		DWORD l_err = GetLastError();
    		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			      l_err,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(char *)&str,0,NULL);

		cerr << "Error: " << str << endl;

		TangoSys_OMemStream o;
		o << "Trying to load shared library " << lib_name << " failed. It returns error: " << str << ends;
		::LocalFree((HLOCAL)str);

		Except::throw_exception((const char *)"API_ClassNotFound",o.str(),
				        (const char *)"DServer::create_cpp_class");
	}
	
	cout4 << "GetModuleHandle is a success" << endl;
	
	string sym_name("_create_");
	sym_name = sym_name + class_name;
	sym_name = sym_name + "_class";
	
	cout4 << "Symbol name = " << sym_name << endl;
	FARPROC proc;
	
	if ((proc = GetProcAddress(mod,sym_name.c_str())) == NULL)
	{
		TangoSys_OMemStream o;
		o << "Class " << cl_name << " does not have the C creator function (_create_<Class name>_class)" << ends;

		Except::throw_exception((const char *)"API_ClassNotFound",o.str(),
				        (const char *)"DServer::create_cpp_class");
	}
	cout4 << "GetProcAddress is a success" << endl;
	
	Cpp_creator_ptr mt = (Cpp_creator_ptr)proc;
#else
	void *lib_ptr;
	lib_name = lib_name + ".so";
	
	lib_ptr = dlopen(lib_name.c_str(),RTLD_NOW);
	if (lib_ptr == NULL)
	{
		TangoSys_OMemStream o;
		o << "Trying to load shared library " << lib_name << " failed. It returns error: " << dlerror() << ends;

		Except::throw_exception((const char *)"API_ClassNotFound",o.str(),
				        (const char *)"DServer::create_cpp_class");
	}
	
	cout4 << "dlopen is a success" << endl;
	
	void *sym;

	string sym_name("_create_");
	sym_name = sym_name + class_name;
	sym_name = sym_name + "_class";
	
	cout4 << "Symbol name = " << sym_name << endl;
		
	sym = dlsym(lib_ptr,sym_name.c_str());
	if (sym == NULL)
	{
		TangoSys_OMemStream o;
		o << "Class " << cl_name << " does not have the C creator function (_create_<Class name>_class)" << ends;

		Except::throw_exception((const char *)"API_ClassNotFound",o.str(),
				        (const char *)"DServer::create_cpp_class");
	}
	
	cout4 << "dlsym is a success" << endl;
	
	Cpp_creator_ptr mt = (Cpp_creator_ptr)sym;
#endif /* _TG_WINDOWS_ */
	Tango::DeviceClass *dc = (*mt)(par_name);
	add_class(dc);
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::get_dev_prop()
// 
// description : 	Retrieve device properties
//
// argin: tg : Tango Util object ptr
//
//-----------------------------------------------------------------------------

void DServer::get_dev_prop(Tango::Util *tg)
{

//
// Try to retrieve device properties (Polling threads pool conf.)
//

	if (tg->_UseDb == true)
	{
		DbData db_data;
		
		db_data.push_back(DbDatum("polling_threads_pool_size"));
		db_data.push_back(DbDatum("polling_threads_pool_conf"));

		try
		{		
			db_dev->get_property(db_data);
		}
		catch (Tango::DevFailed &)
		{
			TangoSys_OMemStream o;
			o << "Database error while trying to retrieve device properties for device " << device_name.c_str() << ends;

			Except::throw_exception((const char *)"API_DatabaseAccess",
					o.str(),
					(const char *)"DServer::get_dev_prop");
		}

//
// If the prop is not defined in db and if the user has defined it in the Util class,
// takes the user definition
//

		if (db_data[0].is_empty() == false)
			db_data[0] >> polling_th_pool_size;
		else
		{
			int p_size = tg->get_polling_threads_pool_size();
			if (p_size != 0)
				polling_th_pool_size = p_size;
		}
		if (db_data[1].is_empty() == false)
			db_data[1] >> polling_th_pool_conf;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServer::check_lock_owner()
// 
// description : 	Check in case the device is locked if the client is the 
//					lock owner
//
// argin: dev : The device
//		  cmd_name : The DServer device command name
//		  dev_name : The device name
//
//-----------------------------------------------------------------------------

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
					o << " Your request is not allowed while a device is locked." << ends;
					v << "DServer::" << cmd_name << ends;
					Except::throw_exception((const char *)"API_DeviceLocked",o.str(),v.str());
				}
			}
			else
			{
				TangoSys_OMemStream o,v;
				o << "Device " << dev_name << " is locked by another client.";
				o << " Your request is not allowed while a device is locked." << ends;
				v << "DServer::" << cmd_name << ends;
				Except::throw_exception((const char *)"API_DeviceLocked",o.str(),v.str());
			}
		}
	}
}

} // End of Tango namespace


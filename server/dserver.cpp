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
// $Revision$
//
// $Log$
// Revision 3.28  2008/01/18 07:58:12  taurel
// - Protect the unregister_server call against any kind of exception
//
// Revision 3.27  2008/01/09 13:03:47  jensmeyer
// Added a correct clean-up when exiting a device server which
// is also used as an event client.
// Added a EventConsumer::disconnect_from_notifd method as for
// the EventSupplier.
//
// Revision 3.26  2007/12/19 15:42:43  taurel
// - Add some cleanup when quitting applis or device server (notifd objects)
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
// Revision 3.20  2006/06/15 07:08:00  taurel
// - Fix bug in the order devices/classes are destroyed in the admin device Init command
//
// Revision 3.19  2006/05/18 08:52:37  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.18  2006/04/06 09:57:59  taurel
// - Minor changes in error message in the create_cpp_class method
//
// Revision 3.17  2006/03/09 08:18:08  taurel
// - Change the order of device server destruction. First devices then class.
// It was the contrary....
//
// Revision 3.16  2006/02/17 16:51:28  jensmeyer
// Corrections when porting to VC7 under windows
//
// Revision 3.15  2006/01/27 14:27:10  taurel
// - Fix a severe incompatibility problem introduced by all the modifs done for
// PY DS
// - Duplicate some EventSupplier class methods (instead of using template) in order to be able to generate Tango shared library on Suse 9.3
//
// Revision 3.14  2006/01/20 08:22:29  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 3.13  2005/06/29 08:31:17  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.12  2005/03/14 09:49:29  taurel
// - Fix some bugs in filedatabase (Change in the data transferred between client and
//   server).
// - Fix bug in event re-connection
// - Add event support even for device server started with the -file command line option
//
// Revision 3.11  2005/03/03 15:38:00  taurel
// - Change in attribute quality factor change event. It is now fired by the Attribute
// set_quailty() and set_value_date_quality() methods. For scannable device.
//
// Revision 3.10  2005/01/13 08:29:05  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.9.2.8  2004/11/09 09:57:49  taurel
// - Fix one memory leak and some minor changes
// - Force database file writing when server exit
// - Some minor changes for the -file option for WIN32
//
// Revision 3.9.2.7  2004/11/04 09:46:38  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.9.2.6  2004/10/27 05:59:46  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.9.2.5  2004/10/22 11:25:50  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.9.2.4  2004/10/05 13:55:02  maxchiandone
// First upload for filedatabase.
//
// Revision 3.9.2.3  2004/09/27 09:09:07  taurel
// - Changes to allow reading state and/or status as attributes
//
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
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

DServer::DServer(DeviceClass *cl_ptr,const char *n,const char *d,Tango::DevState s,
		 const char *st)
:Device_3Impl(cl_ptr,n,d,s,st)
{
	process_name = Tango::Util::instance()->get_ds_exec_name();
	instance_name = Tango::Util::instance()->get_ds_inst_name();
	
	full_name = process_name;
	full_name.append(1,'/');
	full_name.append(instance_name);
	
	last_heartbeat = time(NULL);
	heartbeat_started = false;

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
// In case of database 
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->_FileDb)
		tg->reset_filedatabase();
	
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

		class_factory();
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

	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->_FileDb)
			tg->instance()->get_database()->write_filedatabase();
	}

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
// in case of devices with dyn attr removed durind device
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
// If the device was polled, stop polling
//

	if (dev_pol.empty() == false)
	{
		dev_to_del->stop_polling();
	}
		
//
// Delete the device (deactivate it and remove it)
//
	
	Tango::Util *tg = Tango::Util::instance();
	PortableServer::POA_ptr r_poa = tg->get_poa();
	if (dev_to_del->get_exported_flag() == true)
		r_poa->deactivate_object(dev_to_del->get_obj_id().in());
	if (dev_to_del->is_py_device() == false)
		dev_to_del->_remove_ref();
	else
	{
		AutoPyLock PyLo;
		
		Device_3Impl *dev_to_del_3 = static_cast<Device_3Impl *>(dev_to_del);		
		dev_to_del_3->delete_dev();
	}	
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
// Re-set event parameters (if needed)
//

	if (eve.size() != 0)
	{
		vector<Tango::DeviceImpl *> &d_list = dev_cl->get_device_list();
		DeviceImpl *new_dev = NULL;
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
			cout3 << "Not able to restart event rapidly" << endl;
			TangoSys_OMemStream o;
			o << "Not able to restart event rapidly" << ends;
			Except::throw_exception((const char *)"API_DeviceNotFound",
				        	o.str(),
				        	(const char *)"Dserver::restart()");			
		}
		
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
	}

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

	Tango::Util *tg = Tango::Util::instance();
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
// Mark the server as shutting down
//

	Tango::Util *tg = Tango::Util::instance();
	tg->set_svr_shutting_down(true);

//
//  Send kill command to the polling thread
// 

	TangoMonitor &mon = tg->get_poll_monitor();
	PollThCmd &shared_cmd = tg->get_poll_shared_cmd();

	{	
		omni_mutex_lock sync(mon);

		shared_cmd.cmd_pending = true;
		shared_cmd.cmd_code = POLL_EXIT;

		mon.signal();

	}
	tg->get_polling_thread_object()->join(0);
	tg->clr_poll_th_ptr();
					
//
// Unregister server device(s) from database
//

	try
	{
		tg->unregister_server();
	}
	catch(...) {}

//
// Delete devices (except the admin one)
//


	{
		AutoPyLock PyLo;
		tg->get_dserver_device()->delete_devices();
	}

//
// Write file database for server started with the -file option
//

	if (tg->_FileDb == true)
	{
		Database *db_ptr = tg->get_database();
		delete db_ptr;
	}

// 	Stop the KeepAliveThread and the EventConsumer Thread when 
//  	they have been started to receive events.

	EventConsumer *ec = ApiUtil::instance()->get_event_consumer();
	if (ec != NULL)
		ec->disconnect_from_notifd();

//
// If the server has used event, disconnect from the notifd
// structured push consumer
//

	EventSupplier *ev = tg->get_event_supplier();
	if (ev != NULL)
		ev->disconnect_from_notifd();
						
//
// Shutdown the ORB
//

	CORBA::ORB_ptr orb = Tango::Util::instance()->get_orb();
	orb->shutdown(true);

//
// Cleanup logging stuffs
//

#ifdef TANGO_HAS_LOG4TANGO
  	Logging::cleanup();
#endif

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


} // End of Tango namespace


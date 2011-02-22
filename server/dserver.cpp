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
// Revision 3.4  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.3  2003/04/24 10:18:09  nleclercq
// Minor modifications to make some compilers happy
//
// Revision 3.2  2003/04/08 15:06:17  taurel
// - Names of read/written attributes are now stored in blackbox
// - Admin device RestartServer command is now done in its own thread with
//   changing CORBA POA manager state
//
// Revision 3.1  2003/04/02 12:26:04  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
//
// Revision 3.0  2003/03/25 16:43:01  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.11  2003/03/11 17:55:52  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:40  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:18  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:21  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:06  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:19  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:54  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:41  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
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

#include <new>
#include <algorithm>
#include <math.h>

#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#endif /* WIN32 */

#include <stdlib.h>

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
:Device_2Impl(cl_ptr,n,d,s,st)
{
	process_name = Tango::Util::instance()->get_ds_exec_name();
	instance_name = Tango::Util::instance()->get_ds_inst_name();
	
	full_name = process_name;
	full_name.append(1,'/');
	full_name.append(instance_name);

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
// Destroy already registered classes
//

	if (class_list.empty() == false)
	{
		for (unsigned long i = 0;i < class_list.size();i++)
			delete class_list[i];
		class_list.clear();
	}
		
	cout3 << "DServer::DSserver() create dserver " << device_name << endl;

	bool class_factory_done;
	unsigned long i;
	try
	{

//
// Activate the POA manager
//

		PortableServer::POA_var poa = Util::instance()->get_poa();
		PortableServer::POAManager_var manager = poa->the_POAManager();

		PortableServer::POAManager::State man_state = manager->get_state();
		if (man_state == PortableServer::POAManager::HOLDING)	
			manager->activate();
						
//
// Create user TDSOM implementation
//

		class_factory_done = false;
		class_factory();
		class_factory_done = true;
		
		if (class_list.empty() == false)
		{
		
//
// Set the class list pointer in the Util class and add the DServer object class
//

			Tango::Util *tg = Tango::Util::instance();
			tg->set_class_list(&class_list);
			tg->add_class_to_list(this->get_device_class());
			const char *ds_name_ptr = tg->get_ds_name().c_str();

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

				const Tango::DevVarStringArray *dev_list;
				if (tg->_UseDb == true)
				{
					CORBA::Any send;
					Tango::DevVarStringArray *ds = new Tango::DevVarStringArray(2);
					ds->length(2);
					(*ds)[0] = CORBA::string_dup(ds_name_ptr);
					(*ds)[1] = CORBA::string_dup(class_list[i]->get_name().c_str());

					send <<= ds;								
					CORBA::Any_var received = tg->get_database()->get_dbase()->command_inout("DbGetDeviceList",send);
		
					if ((received >>= dev_list) == false)
					{
						TangoSys_OMemStream o;
						o << "Database error while trying to retrieve device list for class " << class_list[i]->get_name().c_str() << ends;
					
						Except::throw_exception((const char *)"API_DatabaseAccess",
				                			o.str(),
				                			(const char *)"Dserver::init_device");
					}
				
					cout4 << dev_list->length() << " device(s) defined" << endl;

//
// Create all device(s)
//
				
					class_list[i]->device_factory(dev_list);
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
		
					class_list[i]->device_factory(dev_list_nodb);
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
					delete class_list[j];
				class_list.clear();
			}
		}
		else
		{
			o << "Can't allocate memory in server while building command(s) or device(s) for class number " << i + 1 << ends;
			for (unsigned long j = i;j < class_list.size();j++)
				delete class_list[j];
			class_list.erase(class_list.begin() + i,class_list.end());
		}
		
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      o.str(),
				      (const char *)"DServer::init_device");
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
				for (long j = 0;j < class_list.size();j++)
					delete class_list[j];
				class_list.clear();
			}
		}
		else
		{
			for (unsigned long j = i;j < class_list.size();j++)
				delete class_list[j];
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
		for (unsigned long i = 0;i < class_list.size();i++)
			delete class_list[i];
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
	
	try
	{
		ret = new Tango::DevVarStringArray(DefaultMaxSeq);
	
		for (int i = 0;i < nb_class;i++)
		{
			long nb_dev = class_list[i]->get_device_list().size();
			int base = ret->length();
                        ret->length(nb_dev + base);
			for (long j = 0;j < nb_dev;j++)
			{
				(*ret)[base + j] = (class_list[i]->get_device_list())[j]->get_name().c_str();
			}
		}
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				        (const char *)"Can't allocate memory in server",
				        (const char *)"DServer::query_device");
	}		
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
// Check if the wanted device exists in each class
//

	bool found = false;
	int nb_class = class_list.size();
	unsigned int i;	
	vector<DeviceImpl *>::iterator ite,ite_end;
	for (i = 0;i < nb_class;i++)
	{
		vector<DeviceImpl *> &dev_list = class_list[i]->get_device_list();
		ite_end = dev_list.end();
		for (ite = dev_list.begin();ite != dev_list.end();ite++)
		{
			if ((*ite)->get_name() == d_name)
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
// Get device name, class pointer and polled object list
//

	DeviceClass *dev_cl = dev_to_del->get_device_class();	
	Tango::DevVarStringArray name(1);
	name.length(1);
	
	name[0] = d_name.c_str();
	
	vector<PollObj *> &p_obj = dev_to_del->get_poll_obj_list();
	vector<Pol> dev_pol;
	
	for (i = 0;i < p_obj.size();i++)
	{
		Pol tmp;
		tmp.type = p_obj[i]->get_type();
		tmp.upd = p_obj[i]->get_upd();
		tmp.name = p_obj[i]->get_name();
		dev_pol.push_back(tmp);
	}

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
	dev_to_del->_remove_ref();
	CORBA::release(r_poa);
		
//
// Re-create device. Take the monitor in case of class or process serialisation
// model
//

	{
		AutoTangoMonitor sync(dev_cl);	
		dev_cl->device_factory(&name);
	}
			
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

void *ServRestartThread::run_undetached(void *ptr)
{

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
				
//	
// Restart the device
//

	dev->init_device();
	
//
// Restart polling (if any)
//

	Tango::Util *tg = Tango::Util::instance();

	tg->polling_configure();

//
// Just make some compiler happy
//

  	return 0;
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
	
	sleep(1);

//
// Unregister server device(s) from database
//

	Tango::Util *tg = Tango::Util::instance();
	tg->unregister_server();

//
// Shutdown the ORB
//

	CORBA::ORB_ptr orb = Tango::Util::instance()->get_orb();
	orb->shutdown(true);
	CORBA::release(orb);

//
// Cleanup logging stuffs
//

#ifdef TANGO_HAS_LOG4TANGO
  	Logging::cleanup();
#endif
		
//
// Now, the suicide
//

#ifdef __linux
	raise(SIGKILL);
//	::exit(-1);
#else	
	::exit(-1);
#endif

	return NULL;
}

} // End of Tango namespace


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
// Revision 3.9.2.2  2004/09/15 06:46:28  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.9.2.1  2004/08/19 07:44:04  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.9  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.8  2004/06/15 12:38:22  taurel
// - Last commit before merge with release 4 branch
//
// Revision 3.7  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.6  2003/07/03 07:40:51  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.5.2.7  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.5.2.6  2004/03/02 07:41:56  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.5.2.5  2004/02/18 15:06:17  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 3.5.2.4  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.5.2.3  2003/11/28 17:04:13  taurel
// Changes to handle tango lib build as a Windows DLL and used in a device
// server
// - Modify how the device server is killed with the Dserver kill command
//
// Revision 3.5.2.1  2003/09/30 11:50:43  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.5  2003/05/28 14:55:09  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
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
#include <dlfcn.h>
#endif /* WIN32 */

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

// In case of database 

	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->_FileDb)
			tg->reset_filedatabase();
	}
	
//	
// Destroy already registered classes
//

	if (class_list.empty() == false)
	{
		for (unsigned long i = 0;i < class_list.size();i++)
		{
			if (class_list[i]->is_py_class() == false)
				delete class_list[i];
			else
				class_list[i]->delete_class();
		}
		class_list.clear();
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

			Tango::Util *tg = Tango::Util::instance();
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
						na = db->get_device_name(tg->get_ds_name(),class_list[i]->get_name());
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

					omni_thread *th = omni_thread::self();
					omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
					PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
					lock_ptr->Release();
					
					class_list[i]->set_memorized_values(true);
					
					lock_ptr->Get();
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
				for (loop = 0;loop < devs.size();loop++)
				{	
					class_list[i]->delete_dev(loop,tg,r_poa);		
				}
				devs.clear();
				CORBA::release(r_poa);
				
				delete class_list[i];
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
	sleep(1);

//
//  Send kill command to the polling thread
// 

	Tango::Util *tg = Tango::Util::instance();
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

	tg->unregister_server();

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

//
// Shutdown the ORB
//

	CORBA::ORB_ptr orb = Tango::Util::instance()->get_orb();
	orb->shutdown(true);
//	CORBA::release(orb);

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
#ifndef WIN32
	cout4 << "In DServer::create_cpp_class for " << cl_name <<  ", " << par_name << endl;

	void *lib_ptr;
	string class_name(cl_name);
	
	string lib_name = class_name;
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
	Tango::DeviceClass *dc = (*mt)(par_name);
	add_class(dc);
#endif
}


} // End of Tango namespace


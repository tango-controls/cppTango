static const char *RcsId = "$Header$";

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
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <tango.h>

#include <new>
#include <algorithm>

#ifndef WIN32
#include <unistd.h>
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
:DeviceImpl(cl_ptr,n,d,s,st)
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
	
	cout3 << "DServer::DSserver() create dserver " << device_name << endl;

//
// Now, creates all user classes
//

	bool class_factory_done;
	long i;
	try
	{
		class_factory_done = false;
		class_factory();
		class_factory_done = true;
		
		if (class_list.empty() == false)
		{
		
//
// A loop for each class
//

			Tango::Util *tg = Tango::Util::instance();
			const char *ds_name_ptr = tg->get_ds_name().c_str();
						
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
// Retrieve device(s) name list from the database
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
					
					if (dev_list->length() == 0)
					{
						TangoSys_OMemStream o;

						o << "No device defined in database for class " << class_list[i]->get_name().c_str() << ends;
						Except::throw_exception((const char *)"API_DatabaseAccess",
								      o.str(),
								      (const char *)"DServer::init_device");
					}
				
					cout4 << dev_list->length() << " device(s) defined" << endl;

//
// Create all device(s)
//
				
					class_list[i]->device_factory(dev_list);
				}
				else
				{
					Tango::DevVarStringArray *dev_list_nodb = new Tango::DevVarStringArray(1);
					dev_list_nodb->length(1);
					(*dev_list_nodb)[0] = CORBA::string_dup("Not defined");

//
// Create all device(s)
//
		
					class_list[i]->device_factory(dev_list_nodb);
				}
			}
		}
		
//
// Activate the POA manager
//

	PortableServer::POA_var poa = Util::instance()->get_root_poa();
	PortableServer::POAManager_var manager = poa->the_POAManager();
	
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
				for (long j = 0;j < class_list.size();j++)
					delete class_list[j];
				class_list.clear();
			}
		}
		else
		{
			o << "Can't allocate memory in server while building command(s) or device(s) for class number " << i + 1 << ends;
			for (long j = i;j < class_list.size();j++)
				delete class_list[j];
			class_list.erase(class_list.begin() + i,class_list.end());
		}
		
//
// If this method is used from the object constructor, the next throw will
// destroy the object. This object has now a reference count = 1 and the object
// destruction will failed due tot his value of 1. Force the reference count
// to 0 and the call to the destructor will suceed.

		if (from_constructor == true)
			this->_OB_setRef(0);
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
			for (long j = i;j < class_list.size();j++)
				delete class_list[j];
			class_list.erase(class_list.begin() + i,class_list.end());
		}
		if (from_constructor == true)
			this->_OB_setRef(0);
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
		for (long i = 0;i < class_list.size();i++)
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
	cout4 << "In kill command" << endl;

//
// Create the thread and start it
//

	JTCThread *t = new KillThread;
	
	t->start();	
	
}


void KillThread::run()
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
	orb->shutdown(false);
	CORBA::release(orb);
		
//
// Now, the suicide
//

#ifdef __linux
	pthread_exit((void *)-1);
#else	
	exit(-1);
#endif
}

} // End of Tango namespace


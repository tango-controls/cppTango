static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               Device.cpp
//
// description :        C++ source code for the DeviceImpl and DeviceClass
//			classes. These classes
//			are the root class for all derived Device classes.
//			They are abstract classes. The DeviceImpl class is the
//			CORBA servant which is "exported" onto the network and
//			accessed by the client.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.2  2003/05/16 08:46:15  taurel
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
// Revision 3.1  2003/04/08 15:06:16  taurel
// - Names of read/written attributes are now stored in blackbox
// - Admin device RestartServer command is now done in its own thread with
//   changing CORBA POA manager state
//
// Revision 3.0  2003/03/25 16:41:53  taurel
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
// Revision 2.11  2003/03/11 17:55:50  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:39  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:06:22  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:15:36  taurel
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
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:12  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:10  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:13  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:45  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:15  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:21  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
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

#ifdef TANGO_HAS_LOG4TANGO
#include <logging.h>
#endif

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::DeviceImpl 
// 
// description : 	constructors for the device_impl class from the class object
//			pointer, the device name, the description field, 
//			the =aqmz and the status.
//
// argument : in : 	- cl_ptr : The class object pointer
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
	ext = new DeviceImplExt();

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
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name)
:device_name(d_name),device_class(cl_ptr)
{
	desc = "A Tango device";
	device_state = Tango::UNKNOWN;
	device_status = "Not Initialised";

	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt();

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
	
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &description)
:device_name(d_name),device_class(cl_ptr)
{
	desc = description;
	device_state = Tango::UNKNOWN;
	device_status = "Not Initialised";

	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt();

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
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &de,
	               Tango::DevState st,string &sta)
:device_name(d_name),desc(de),device_status(sta),
 device_state(st),device_class(cl_ptr)
{
	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt();

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
	
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::stop_polling 
// 
// description : 	Stop all polling for a device. if the device is 
//			polled, call this method before deleting it.
//
//--------------------------------------------------------------------------

void DeviceImpl::stop_polling()
{
	int interupted;
	Tango::Util *tg = Tango::Util::instance();
	TangoMonitor &mon = tg->get_poll_monitor();
	PollThCmd &shared_cmd = tg->get_poll_shared_cmd();
	
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
				Except::throw_exception((const char *)"API_CommandTimedout",
							(const char *)"Polling thread blocked !!",
							(const char *)"DeviceImpl::stop_polling");
				
			}
		}
	}
	
	is_polled(false);
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::~DeviceImpl 
// 
// description : 	Destructor for the device class. It simply frees
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

	cout4 << "Leaving DeviceImpl destructor for device " << device_name << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::black_box_create 
// 
// description : 	Private method to create the device black box.
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
// method : 		DeviceImpl::get_dev_system_resource()
// 
// description : 	Method to retrieve some basic device resources
//			The resources to be retrived are :
//				- The black box depth
//				- The device description
//				- The polling ring buffer depth
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

		bool retry = true;
		while (retry == true)
		{
			try
			{		
				db_dev->get_property(db_data);
				retry = false;
			}
			catch (Tango::CommunicationFailed &) {}
			catch (Tango::DevFailed &)
			{
				TangoSys_OMemStream o;
				o << "Database error while trying to retrieve device prperties for device " << device_name.c_str() << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"DeviceImpl::get_dev_system_resource");
			}
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
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::_default_POA
// 
// description : 	Return a pointer to the POA on which the device should
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
// method : 		DeviceImpl::register_signal
// 
// description : 	Method to register a device on a signal. When the
//			signal is sent to the process, the signal_handler
//			method of this class will be executed
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

#ifdef __linux
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
// method : 		DeviceImpl::unregister_signal
// 
// description : 	Method to unregister a device on a signal.
//
// in :			signo : The signal number
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
// method : 		DeviceImpl::signal_handler
// 
// description : 	This is the signal handler for the device. This method
//			is defined as virtual and therefore, can be redefined
//			by DS programmers in their own classes derived from
//			DeviceImpl
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceImpl::signal_handler(long signo)
{
	cout4 << "DeviceImpl::signal_handler() arrived for signal " << signo << endl;
	
	cout4 << "Leaving DeviceImpl::signal_handler method()" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::check_command_exist
// 
// description : 	This method check that a comamnd is supported by 
//			the device and does not need input value.
//			The method throws an exception if the
//			command is not defined or needs an input value
//
// in :			cmd_name : The command name
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
// method : 		DeviceImpl::get_command
// 
// description : 	This method returns a pointer to command object.
//			The method throws an exception if the
//			command is not defined
//
// in :			cmd_name : The command name
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
// method : 		DeviceImpl::get_polled_obj_by_type_name
// 
// description : 	This method check that a comamnd is supported by 
//			the device and does not need input value.
//			The method throws an exception if the
//			command is not defined or needs an input value
//
// in :			cmd_name : The command name
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

#ifdef WIN32				
	return NULL;
#endif
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::dev_state
// 
// description : 	The default method called by the DevState command.
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

		vector<long> &attr_list = dev_attr->get_alarm_list();
		long nb_wanted_attr = attr_list.size();
		if (nb_wanted_attr != 0)
		{
			read_attr_hardware(attr_list);
	
//
// Set attr value
//

			long i;
			for (i = 0;i < nb_wanted_attr;i++)
			{
				Attribute &att = dev_attr->get_attr_by_ind(attr_list[i]);
				att.wanted_date(false);
				try
				{
					read_attr(att);
				}
				catch (Tango::DevFailed)
				{
					att.wanted_date(true);
					throw;
				}
				att.wanted_date(true);
			}
		
//
// Check alarm level
//

			if (dev_attr->check_alarm() == true)
				device_state = Tango::ALARM;
			else
				device_state = Tango::ON;
			
//
// Free the sequence created to store the attribute value
//

			for (i = 0;i < nb_wanted_attr;i++)
			{
				dev_attr->get_attr_by_ind(attr_list[i]).delete_seq();
			}
		}
	}
	return device_state;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::dev_status
// 
// description : 	The default method called by the DevStatus command.
//			If the device is ON, this method add Attribute status
//			for all device attribute in alarm state.
//
//--------------------------------------------------------------------------

Tango::ConstDevString DeviceImpl::dev_status()
{
	NoSyncModelTangoMonitor mon(this);
	
	if ((device_state == Tango::ALARM) ||
	    (device_state == Tango::ON))
	{
		alarm_status = device_status;
		dev_attr->read_alarm(alarm_status);
		return alarm_status.c_str();
	}
	return device_status.c_str();
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::command_inout 
// 
// description : 	Method called for each command_inout operation executed
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

	cout4 << "DeviceImpl::command_inout(): command received : " << command << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_cmd(in_cmd);

//	
// Execute command
//

	CORBA::Any *out_any;
	transform(command.begin(),command.end(),command.begin(),::tolower);
		
	out_any = device_class->command_handler(this,command,in_any);
		
//
// Return value to the caller 
//

	cout4 << "DeviceImpl::command_inout(): leaving method for command " << in_cmd << endl;	
	return(out_any);
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::name 
// 
// description : 	Method called when a client request the name attribute
//                      This method is called for a IDL attribute which can
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
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::name arrived" << endl;

//
// Record attribute request in black box
//

		blackbox_ptr->insert_corba_attr(Attr_Name);
	}
	catch (Tango::DevFailed &e)
	{
		CORBA::IMP_LIMIT lim;
		if (strcmp(e.errors[0].reason,"API_CommanTimedOut") == 0)
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
// method : 		DeviceImpl::adm_name 
// 
// description : 	Method called when a client request the adm_name attribute
//                      This method is called for a IDL attribute which can
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
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::adm_name arrived" << endl;

//
// Record attribute request in black box
//

		blackbox_ptr->insert_corba_attr(Attr_AdmName);
	}
	catch (Tango::DevFailed &e)
	{
		CORBA::IMP_LIMIT lim;
		if (strcmp(e.errors[0].reason,"API_CommanTimedOut") == 0)
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
// method : 		DeviceImpl::description 
// 
// description : 	Method called when a client request the description
//			attribute
//                      This method is called for a IDL attribute which can
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
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::description arrived" << endl;

//
// Record attribute request in black box
//

		blackbox_ptr->insert_corba_attr(Attr_Description);
	}
	catch (Tango::DevFailed &e)
	{
		CORBA::IMP_LIMIT lim;
		if (strcmp(e.errors[0].reason,"API_CommanTimedOut") == 0)
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
// method : 		DeviceImpl::state
// 
// description : 	Method called when a client request the state
//			attribute
//
//--------------------------------------------------------------------------

Tango::DevState DeviceImpl::state()
throw (CORBA::SystemException)
{
	Tango::DevState tmp;
	
	try
	{
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::state (attibute) arrived" << endl;

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
		CORBA::IMP_LIMIT lim;
		if (strcmp(e.errors[0].reason,"API_CommanTimedOut") == 0)
			lim.minor(TG_IMP_MINOR_TO);
		else
			lim.minor(TG_IMP_MINOR_DEVFAILED);
		cout4 << "Leaving DeviceImpl::state (attribute) throwing IMP_LIMIT" << endl;			
		throw lim;
	}
	catch (...)
	{
		CORBA::IMP_LIMIT lim;
		lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
		cout4 << "Leaving DeviceImpl::state (attribute) throwing IMP_LIMIT" << endl;			
		throw lim;
	}	
	
	
	cout4 << "Leaving DeviceImpl::state (attribute)" << endl;			
	return tmp;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::status
// 
// description : 	Method called when a client request the description
//			status
//                      This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::status()
throw (CORBA::SystemException)
{
	char *tmp;
	try
	{
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::status (attibute) arrived" << endl;

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
		if (strcmp(e.errors[0].reason,"API_CommandTimedOut") == 0)
			tmp = CORBA::string_dup("Not able to acquire device monitor");
		else
			tmp = CORBA::string_dup("Got exception  when trying to build device status");
	}
	catch (...)
	{
		CORBA::IMP_LIMIT lim;
		lim.minor(TG_IMP_MINOR_NON_DEVFAILED);
		throw lim;
	}
	
	cout4 << "Leaving DeviceImpl::status (attribute)" << endl;		
	return tmp;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::black_box 
// 
// description : 	CORBA operation to read n element(s) of the black-box.
//			This method returns black box element as strings
//
// argument: in :	- n : Number of elt to read
//
//--------------------------------------------------------------------------


Tango::DevVarStringArray* DeviceImpl::black_box(CORBA::Long n)
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this);

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
// method : 		DeviceImpl::command_list_query
// 
// description : 	CORBA operation to read the device command list.
//			This method returns command info in a sequence of
//			DevCmdInfo
//
//--------------------------------------------------------------------------


Tango::DevCmdInfoList* DeviceImpl::command_list_query()
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this);

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
// method : 		DeviceImpl::command_query
// 
// description : 	CORBA operation to read a device command info.
//			This method returns command info for a specific
//			command.
//
//
//--------------------------------------------------------------------------

Tango::DevCmdInfo *DeviceImpl::command_query(const char *command)
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this);
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
// method : 		DeviceImpl::info
// 
// description : 	CORBA operation to get device info
//
//--------------------------------------------------------------------------


Tango::DevInfo *DeviceImpl::info()
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this);

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
	back->doc_url = CORBA::string_dup(device_class->get_doc_url().c_str());
		
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
// method : 		DeviceImpl::ping
// 
// description : 	CORBA operation to ping if a device to see it is alive
//
//--------------------------------------------------------------------------


void DeviceImpl::ping()
throw (Tango::DevFailed, CORBA::SystemException)
{
	AutoTangoMonitor sync(this);
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
// method : 		DeviceImpl::get_attribute_config
// 
// description : 	CORBA operation to get attribute configuration.
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
	AutoTangoMonitor sync(this,true);
	cout4 << "DeviceImpl::get_attribute_config arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList *back;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config);

//
// Get attribute number
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	
//
// Check if the caller want to get config for all attribute
//

	string in_name(names[0]);
	if ((nb_attr == 1) && (in_name == AllAttr))
	{
		nb_attr = nb_dev_attr;
		all_attr = true;
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
// method : 		DeviceImpl::set_attribute_config
// 
// description : 	CORBA operation to set attribute configuration locally
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
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Set_Attr_Config);

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
// Update attribute config first in database, then locally
//
	
	long nb_attr = new_conf.length();
	long i;

	try
	{	
		for (i = 0;i < nb_attr;i++)
		{
			Attribute &attr = dev_attr->get_attr_by_name(new_conf[i].name);
			if (Tango::Util::_UseDb == true)
				attr.upd_database(new_conf[i],device_name);
			attr.set_properties(new_conf[i],device_name);
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
			if (att.is_alarmed() == true)
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

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
		e.errors[0].reason = CORBA::string_dup(o.str().c_str());
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
		if (attr.is_alarmed() == true)
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
// method : 		DeviceImpl::read_attributes
// 
// description : 	CORBA operation to read attribute(s) value.
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

	cout4 << "DeviceImpl::read_attributes arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_attr(names);
	
//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				        (const char *)"The device does not have any attribute",
				        (const char *)"DeviceImpl::read_attributes");
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

	nb_names = real_names.length();
	vector<long> wanted_attr;
	vector<long> wanted_w_attr;

	for (i = 0;i < nb_names;i++)
	{
		long j = dev_attr->get_attr_ind_by_name(real_names[i]);
		if (dev_attr->get_attr_by_ind(j).get_writable() == Tango::READ_WRITE)
		{		
			wanted_w_attr.push_back(j);
			wanted_attr.push_back(j);
			dev_attr->get_attr_by_ind(wanted_attr.back()).set_value_flag(false);
		}
		else
		{
			if (dev_attr->get_attr_by_ind(j).get_writable() == Tango::WRITE)
			{					
				wanted_w_attr.push_back(j);
			}
			else
			{
				wanted_attr.push_back(j);
				dev_attr->get_attr_by_ind(wanted_attr.back()).set_value_flag(false);
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

	for (i = 0;i < nb_wanted_attr;i++)
	{
		read_attr(dev_attr->get_attr_by_ind(wanted_attr[i]));
	}

//
// Set attr value for writable attribute
//

	for (i = 0;i < nb_wanted_w_attr;i++)
	{
		Tango::AttrWriteType w_type = dev_attr->get_attr_by_ind(wanted_w_attr[i]).get_writable();
		if ((w_type == Tango::READ_WITH_WRITE) ||
		    (w_type == Tango::WRITE)) 
			dev_attr->get_attr_by_ind(wanted_w_attr[i]).set_value();
	}
	
//
// Allocate memory for the AttributeValue structures
//

	Tango::AttributeValueList *back;
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
		if ((qual == Tango::ATTR_VALID) || (qual == Tango::ATTR_ALARM))
		{
			if (att.get_value_flag() == false)
			{
				delete back;
				for (long j = 0;j < i;j++)
				{
					att.delete_seq();
				}
				
				TangoSys_OMemStream o;
		
				o << "Read value for attribute ";
				o << att.get_name();
				o << " has not been updated" << ends;
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
				
					if ((att.is_alarmed() == true) && (qual != Tango::ATTR_INVALID))
						att.check_alarm();
				}
				
				CORBA::Any &a = (*back)[i].value;
				switch (att.get_data_type())
				{
				case Tango::DEV_SHORT :			
					a <<= att.get_short_value();
					break;
				
				case Tango::DEV_LONG :
					a <<= att.get_long_value();
					break;
				
				case Tango::DEV_DOUBLE :
					a <<= att.get_double_value();
					break;
				
				case Tango::DEV_STRING :
					a <<= att.get_string_value();
					break;
				}
			}
		}
					
		(*back)[i].time = att.get_when();
		(*back)[i].quality = att.get_quality();
		(*back)[i].name = CORBA::string_dup(att.get_name().c_str());
		(*back)[i].dim_x = att.get_x();
		(*back)[i].dim_y = att.get_y();
	}
					
//
// Return to caller
//

	cout4 << "Leaving DeviceImpl::read_attributes" << endl;
	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::write_attributes
// 
// description : 	CORBA operation to write attribute(s) value
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
// Record operation request in black box
//

	blackbox_ptr->insert_attr(values);

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
			dev_attr->get_w_attr_by_ind(updated_attr[i]).set_write_value(values[i].value);
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

	write_attr_hardware(updated_attr);
	
//
// Return to caller.
//
	
	cout4 << "Leaving DeviceImpl::write_attributes" << endl;						
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::add_attribute
// 
// description : 	Add attribute to the device attribute(s) list
//
// argument: in :	- new_attr: The new attribute to be added.
//
//--------------------------------------------------------------------------

void DeviceImpl::add_attribute(Tango::Attr *new_attr)
{
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
	try
	{
		dev_attr->get_attr_by_name(attr_name.c_str());
	}
	catch (Tango::DevFailed)
	{
		already_there = false;
	}

	if (already_there == true)
	{
		return;
	}
				
//
// Add this attribute in the MultiClassAttribute attr_list vector if it does not
// already exist
//

	long i;
	for (i = 0;i < old_attr_nb;i++)
	{
		if (attr_list[i]->get_name() == attr_name)
			break;
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
						
//
// Add the attribute to the MultiAttribute object
//

	dev_attr->add_attribute(device_name,device_class,i);
}

//+----------------------------------------------------------------------------
// method : DeviceImplExt::DeviceImplExt
//-----------------------------------------------------------------------------

#ifdef TANGO_HAS_LOG4TANGO
DeviceImplExt::DeviceImplExt()
  : exported(false),
    polled(false),
    poll_ring_depth(0),logger(0),
    saved_log_level(log4tango::Level::WARN),
    rft(Tango::kDefaultRollingThreshold)
{
}
#endif


//+----------------------------------------------------------------------------
//
// method : 		DeviceImplExt::~DeviceImplExt
// 
// description : 	DeviceImpl extension class destructor. This destructor
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
  	if (logger)
	{
    		logger->remove_all_appenders();
		delete logger;
    		logger = 0;
  	}
#endif
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::DeviceClass(string &s)
// 
// description : 	DeviceClass constructor. Protected method which will 
//			be called automatically by the compiler.
//
//-----------------------------------------------------------------------------

DeviceClass::DeviceClass(string &s):name(s)
{

//
// Create the DeviceClassExt instance
//

	ext = new DeviceClassExt;
	
//
// Create the associated DbClass object
//

	db_class = new DbClass(name,Tango::Util::instance()->get_database());
		
//
// initialise command_list with DevState and DevStatus 
//

	try
	{
		command_list.push_back(new DevStatusCmd("Status",Tango::DEV_VOID,Tango::DEV_STRING));
		command_list[0]->set_out_type_desc("Device status");
		command_list.push_back(new DevStateCmd("State",Tango::DEV_VOID,Tango::DEV_STATE));
		command_list[1]->set_out_type_desc("Device state");
		command_list.push_back(new DevInitCmd("Init",Tango::DEV_VOID,Tango::DEV_VOID));
	}
	catch (bad_alloc)
	{
		if (command_list.empty() == false)
		{
			for(unsigned long i = 0;i < command_list.size();i++)
				delete command_list[i];
			command_list.clear();
		}
		throw;
	}

//
// Retrieve basic class resource
//
	
	get_class_system_resource();
	
//
// Create the multi class attribute object
//

	class_attr = new MultiClassAttribute();
		
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::get_class_system_resource(string &s)
// 
// description : 	Method to retrieve some basic class resource(s)
//			The resource to be retrived are :
//				- The class doc URL
//
//-----------------------------------------------------------------------------

void DeviceClass::get_class_system_resource()
{

//
// Try to retrieve the class resource doc_url
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->_UseDb == true)
	{
		DbData db_data;
		
		db_data.push_back(DbDatum("doc_url"));

		bool retry = true;
		while (retry == true)
		{		
			try
			{
				db_class->get_property(db_data);
				retry = false;
			}
			catch (Tango::CommunicationFailed &) {}
			catch (Tango::DevFailed &)
			{
				TangoSys_OMemStream o;
				o << "Database error while trying to retrieve properties for class " << name.c_str() << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"DeviceClass::get_class_system_resource");
			}
		}
		
		if (db_data[0].is_empty() == true)
		{
			cout4 << "doc_url property for class " << name << " is not defined in database" << endl;
			Database *db = tg->get_database();
			try
			{
				db->get_class_property("Default",db_data);
			}
			catch (Tango::DevFailed &)
			{			
				TangoSys_OMemStream o;
				o << "Database error while trying to retrieve properties for class " << name.c_str() << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
				                	o.str(),
				                	(const char *)"DeviceClass::get_class_system_resource");
			}			
			
			if (db_data[0].is_empty() == true)
			{
				doc_url = DefaultDocUrl;
			}
			else
				db_data[0] >> doc_url;
		}
		else
			db_data[0] >> doc_url;		
	}
	else
	{
		doc_url = DefaultDocUrl;
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::~DeviceClass(string &s)
// 
// description : 	DeviceClass destructor.
//
//-----------------------------------------------------------------------------

DeviceClass::~DeviceClass()
{
	cout4 << "Entering DeviceClass destructor for class " << name << endl;

//
// Destroy the DbClass object
//

	delete db_class;
		
//
// Destroy the device list
//

	Tango::Util *tg = Tango::Util::instance();
	PortableServer::POA_ptr r_poa = tg->get_poa();
	unsigned long i;
	
	for (i = 0;i < device_list.size();i++)
	{
	
//
// If the device is polled, ask polling thread to stop polling
//

		if (device_list[i]->is_polled() == true)
			device_list[i]->stop_polling();
	
//
// Deactivate the CORBA object
//

		if (device_list[i]->get_exported_flag() == true)
			r_poa->deactivate_object(device_list[i]->get_obj_id().in());
	
//
// Remove the C++ servant
//

		device_list[i]->_remove_ref();
		
	}
	device_list.clear();
	CORBA::release(r_poa);

//
// Destroy the command list
//

	for (i = 0;i < command_list.size();i++)
		delete command_list[i];
	command_list.clear();

//
// Destroy the MultiClassAttribute object
//

	delete class_attr;
		
//
// Unregister the class from signal handler
//

	DServerSignal::instance()->unregister_class_signal(this);
	
//
// Delete the class extension object
//

	delete ext;
	
	cout4 << "Leaving DeviceClass destructor for class " << name << endl;
	
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::register_signal
// 
// description : 	Method to register a class on a signal. When the
//			signal is sent to the process, the signal_handler
//			method of this class will be executed
//
// in :			signo : The signal number
//			delayed : The boolean delayed flag
//
//--------------------------------------------------------------------------

#ifndef __linux
void DeviceClass::register_signal(long signo)
{
	cout4 << "DeviceClass::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_class_signal(signo,this);
		
	cout4 << "Leaving DeviceClass::register_signal method()" << endl;
}
#else
void DeviceClass::register_signal(long signo,bool handler)
{
	cout4 << "DeviceClass::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_class_signal(signo,handler,this);
		
	cout4 << "Leaving DeviceClass::register_signal method()" << endl;
}
#endif

//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::unregister_signal
// 
// description : 	Method to unregister a class on a signal.
//
// in :			signo : The signal number
//			delayed : The boolean delayed flag
//
//--------------------------------------------------------------------------

void DeviceClass::unregister_signal(long signo)
{
	cout4 << "DeviceClass::unregister_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->unregister_class_signal(signo,this);
		
	cout4 << "Leaving DeviceClass::unregister_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::signal_handler
// 
// description : 	This is the signal handler for the class. This method
//			is defined as virtual and therefore, can be redefined
//			by DS programmers in their own classes derived from
//			DeviceClass class
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceClass::signal_handler(long signo)
{
	cout4 << "DeviceClass::signal_handler() arrived for signal " << signo << endl;
	
	cout4 << "Leaving DeviceClass::signal_handler method()" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::export_device()
// 
// description : 	This method exports a device to the outside world. 
//			This is done by sending its CORBA network parameter
//			(mainly the IOR) to the Tango database
//
// in :			dev : The device to be exported
//			corba_obj_name : The CORBA object name associated
//					 with the device. A default value
//					 is provided. This field is mainly
//					 use for specific device server
//					 like the database device server.
//
//--------------------------------------------------------------------------

void DeviceClass::export_device(DeviceImpl *dev,const char *corba_obj_name)
{
	cout4 << "DeviceClass::export_device() arrived" << endl;

	Device_var d;
	
//
// Get the real device IDL release
//

	DeviceVers vers;
	
	if (dev->_is_a("IDL:Tango/Device_2:1.0") == true)
		vers = REL_2;
	else
		vers = REL_1;
		
	if (Tango::Util::_UseDb == true)
	{
			
// 
// Activate the CORBA object incarnated by the dev C++ object
//

		if (vers == Tango::REL_2)
			d = static_cast<Device_2Impl *>(dev)->_this();
		else
			d = dev->_this();
		dev->set_d_var(Tango::Device::_duplicate(d));

//
// Store the ObjectId (The ObjectId_var type is a typedef of a string_var
// type)
//

		PortableServer::ObjectId_var oid;
		try
		{
			PortableServer::POA_ptr r_poa = Util::instance()->get_poa();
			oid = r_poa->reference_to_id(d);
			CORBA::release(r_poa);
		}
		catch (...)
		{
			TangoSys_OMemStream o;
			o << "Cant get CORBA reference Id for device " << dev->get_name() << ends;
			Except::throw_exception((const char *)"API_CantGetDevObjectId",
					      o.str(),
					      (const char *)"DeviceClass::export_device");
		}
		dev->set_obj_id(oid);
	}
	else
	{
	
//
// For server started without db usage (Mostly the database server). In this case,
// it is necessary to create our own CORBA object id and to bind it into the
// OOC Boot Manager for access through a stringified object reference 
// constructed using the corbaloc style
//

		PortableServer::ObjectId_var id = PortableServer::string_to_ObjectId(corba_obj_name);
		try
		{
			PortableServer::POA_ptr r_poa = Util::instance()->get_poa();
			r_poa->activate_object_with_id(id.in(),dev);
			CORBA::release(r_poa);
		}
		catch (...)
		{
			TangoSys_OMemStream o;
			o << "Can't get CORBA reference Id for device " << dev->get_name() << ends;
			Except::throw_exception((const char *)"API_CantgetDevObjId",
						o.str(),
						(const char *)"DeviceClass::export_device");
		}
		
		if (vers == Tango::REL_2)
			d = static_cast<Device_2Impl *>(dev)->_this();
		else
			d = dev->_this();
		dev->set_obj_id(id);		
		dev->set_d_var(Tango::Device::_duplicate(d));		
	}
			
//
// Prepare sent parameters and allocate mem for them
//

	if (Tango::Util::_UseDb == true)
	{
		Tango::Util *tg = Tango::Util::instance();
		CORBA::ORB_ptr orb_ptr = tg->get_orb();	
		
		char *s = orb_ptr->object_to_string(d);
		string ior_string(s);

		Tango::DbDevExportInfo exp;
			
		exp.name = dev->get_name();
		exp.ior = ior_string;
		exp.host = tg->get_host_name();
		exp.pid = tg->get_pid();
		exp.version = tg->get_version_str();

//
// Call db server
//

		bool retry = true;
		while (retry == true)
		{
			try
			{
				tg->get_database()->export_device(exp);
				retry = false;
			}
			catch (Tango::CommunicationFailed &) {}
		}
		
		CORBA::release(orb_ptr);
		CORBA::string_free(s);
	}
	
//
// Set the DeviceImpl exported flag to true
//

	dev->set_exported_flag(true);
			
	cout4 << "Leaving DeviceClass::export_device method()" << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::command_handler(string &s)
// 
// description : 	Command handler which is called by Device
//			when a command is received. It will check
//			to see if the command is implemented. If
//			so it will test to see if it is allowed
//			in this state. Finally it will execute the
//			command by calling its execute method.
//			If an error occurs it will throw a DevFailed
//			exception.
//
//-----------------------------------------------------------------------------

CORBA::Any *DeviceClass::command_handler(DeviceImpl *device,string &command,const CORBA::Any &in_any)
{	
	CORBA::Any *ret;
	vector<Command *>::iterator i_cmd;

	cout4 << "Entering DeviceClass::command_handler() method" << endl;
	
	for (i_cmd = command_list.begin();i_cmd < command_list.end();++i_cmd)
	{
		if ((*i_cmd)->get_lower_name() == command)
		{
		
//
// Call the always executed method
//

			device->always_executed_hook();

//
// Chaeck if command is allowed
//
			
			if ((*i_cmd)->is_allowed(device,in_any) == false)
			{
				TangoSys_OMemStream o;
				o << "Command " << command << " not allowed when the device is in " << Tango::DevStateName[device->get_state()] << " state"  << ends;
				Except::throw_exception((const char *)"API_CommandNotAllowed",
						      o.str(),
						      (const char *)"DeviceClass::command_handler");
			}

//
// Execute command
//

			ret = (*i_cmd)->execute(device,in_any);
			break;
		}
	}
		
	if (i_cmd == command_list.end())
	{
		cout3 << "DeviceClass::command_handler(): command " << command << " not found" << endl;
		
//		
// throw an exception to client
//

		TangoSys_OMemStream o;
		
		o << "Command " << command << " not found" << ends;
		Except::throw_exception((const char *)"API_CommandNotFound",
				      o.str(),
				      (const char *)"DeviceClass::command_handler");
	}
	
	cout4 << "Leaving DeviceClass::command_handler() method" << endl;	
	return ret;
}

} // End of Tango namespace

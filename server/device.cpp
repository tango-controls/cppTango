static const char *RcsId = "$Id$\n$Name$";

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
// $Revision$
//
// $Log$
// Revision 3.13  2006/01/20 08:22:29  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 3.12  2005/06/29 08:30:53  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.11  2005/03/03 15:36:16  taurel
// - Change in attribute quality factor change event. It is now fired by the Attribute
// set_quailty() and set_value_date_quality() methods. For scannable device.
//
// Revision 3.10  2005/01/13 09:27:52  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 3.8.2.10  2004/11/22 14:14:45  taurel
// - Added methods for the AttrDesc class for the new attribute config
//   parameters.
// - Some minor changes
//
// Revision 3.8.2.9  2004/11/04 09:46:38  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.8.2.8  2004/10/27 05:59:45  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.8.2.7  2004/10/22 11:25:00  taurel
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
// Revision 3.8.2.6  2004/10/05 13:55:02  maxchiandone
// First upload for filedatabase.
//
// Revision 3.8.2.5  2004/09/27 09:09:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.8.2.4  2004/09/15 06:45:44  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.8.2.3  2004/08/19 07:44:03  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.8.2.2  2004/07/15 15:04:06  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.8.2.1  2004/07/08 11:02:12  taurel
// - Add possibility to have command/attribute polling buffer depth specified
//   individually
//
// Revision 3.8  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.7  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.6  2003/08/22 12:52:54  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.5  2003/08/21 07:23:46  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.4  2003/07/03 07:40:51  taurel
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
// Revision 3.3.2.12  2004/05/24 08:44:01  taurel
// - Fix bug if device name stored in db used upper case. Add a DeviceImpl::get_name_lower() method
//
// Revision 3.3.2.11  2004/05/19 09:25:59  taurel
// - Change one local variable name for Solaris compilers (device.h)
//
// Revision 3.3.2.10  2004/03/13 08:01:05  nleclercq
// Fixed a bug inDeviceImplExt dtor (multiple delete of Tango::core_logger)
//
// Revision 3.3.2.9  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.3.2.8  2004/03/02 07:41:56  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.3.2.7  2004/02/27 16:47:00  taurel
// - Updated DeviceImpl and Util classes sizes for compatibility
//
// Revision 3.3.2.6  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.3.2.5  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.3.2.4  2003/12/10 16:08:56  taurel
// Last commit before merging with the event branch.
//
// Revision 3.3.2.3  2003/10/03 13:34:26  taurel
// - Fix bug for device server started without database, with device name specified on command line using mix of upper and lower cases
// - It's now possible to send a command to the polling thread from itself
// - Add a release lock if the insert into the polling buffer failed
//
// Revision 3.3.2.2  2003/10/02 07:36:35  taurel
// CHnage the Attribute::set_date() method from private to public
//
// Revision 3.3.2.1  2003/09/30 11:50:42  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.3  2003/05/28 14:55:08  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
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
// Revision 3.0.2.1  2003/11/16 22:10:43  andy_gotz
// New version which defines 4 types of events - change, quality, periodic and
// archive. Code has been factorised to reduce redundancy. Minimum and maximum
// changes are supported. Event period is taken into account. Relative and
// absolute changes are detected. Whole sequence is taken into account when
// determining change.
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
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
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
	ext = new DeviceImplExt();
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);
		  
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
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name)
:device_name(d_name),device_class(cl_ptr)
{
	desc = "A Tango device";
	device_state = Tango::UNKNOWN;
	device_status = StatusNotSet;

	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt();
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);
		  
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
	
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &description)
:device_name(d_name),device_class(cl_ptr)
{
	desc = description;
	device_state = Tango::UNKNOWN;
	device_status = StatusNotSet;

	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt();
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);
		  
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
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &de,
		       Tango::DevState st,string &sta)
:device_name(d_name),desc(de),device_status(sta),
 device_state(st),device_class(cl_ptr)
{
	version = DevVersion;
	blackbox_depth = 0;
	ext = new DeviceImplExt();
	ext->device_prev_state = device_state;

//
// Init lower case device name
//

	ext->device_name_lower = device_name;
	transform(ext->device_name_lower.begin(),ext->device_name_lower.end(),
		  ext->device_name_lower.begin(),::tolower);
		  
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
	
}

//+-------------------------------------------------------------------------
//
// method :		DeviceImpl::stop_polling 
// 
// description :	Stop all polling for a device. if the device is 
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
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER < 1400)
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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
						if (att.get_value_flag() == false)
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
						dev_attr->get_attr_by_ind(idx).delete_seq();
					}
					att.wanted_date(true);
					throw;
				}
				att.wanted_date(true);
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

			for (i = 0;i < nb_wanted_attr;i++)
			{
				long idx;
				if ((vers >= 3) && (ext->state_from_read == true))			
					idx = attr_list_2[i];
				else
					idx = attr_list[i];	
				dev_attr->get_attr_by_ind(idx).delete_seq();
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

	cout4 << "DeviceImpl::command_inout(): command received : " << command << endl;

//
// Record operation request in black box
//

	if (ext->store_in_bb == true)
		blackbox_ptr->insert_cmd(in_cmd);
	ext->store_in_bb = true;

//	
// Execute command
//

	CORBA::Any *out_any;
		
	out_any = device_class->command_handler(this,command,in_any);
		
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
	
	try
	{
		AutoTangoMonitor sync(this);
		cout4 << "DeviceImpl::state (attribute) arrived" << endl;

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
			tmp = CORBA::string_dup("Got exception	when trying to build device status");
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

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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

	cout4 << "DeviceImpl::read_attributes arrived" << endl;

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
		
				o << "It is actually not allowed to read attribute ";
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
					
				case Tango::DEV_FLOAT : 		
					a <<= att.get_float_value();
					break;
				
				case Tango::DEV_BOOLEAN :
					a <<= att.get_boolean_value();
					break;
				
				case Tango::DEV_USHORT :
					a <<= att.get_ushort_value();
					break;
				
				case Tango::DEV_UCHAR :
					a <<= att.get_uchar_value();
					break;
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
			dev_attr->get_w_attr_by_ind(updated_attr[i]).set_write_value(values[i].value,
										     (unsigned long)1,
										     (unsigned long)0);
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

				o << "It is actually not allowed to write attribute ";
				o << att.get_name() << ends;


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
// Remove attribute in MultiClassAttribute in case there is
// only one device in the class or it is the last device
// in this class with this attribute
//

	unsigned long nb_dev = device_class->get_device_list().size();
	vector<Tango::Attr *> &attr_list = device_class->get_class_attr()->get_attr_list();

	if (nb_dev == 1)
	{
		device_class->get_class_attr()->remove_attr(attr_name);
	}
	else
	{
		vector<Tango::DeviceImpl *> dev_list = device_class->get_device_list();
		int nb_except = 0; 
		for (unsigned long i = 0;i < nb_dev;i++)
		{
			try
			{
				dev_list[i]->get_device_attr()->get_attr_by_name(attr_name.c_str());
			}
			catch (Tango::DevFailed)
			{
				nb_except++;
			}
		}
		if (nb_except == (nb_dev - 1))
			device_class->get_class_attr()->remove_attr(attr_name);
	}
		
//
// Now, remove the attribute from the MultiAttribute object
//

	dev_attr->remove_attribute(attr_name);
	
//
// Delete Attr object if wanted
//

	if (free_it == true)
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
// method :		DeviceImpl::push_event
// 
// description :	Push a user event to the Notification service
//
// argument: in :	- new_attr: The new attribute to be added.
//
//--------------------------------------------------------------------------

void DeviceImpl::push_event(vector<string> &filterable_names,
			    vector<double> &filterable_value,
			    Tango::Attribute &att,
			    Tango::DevErrorList *err)
{
	EventSupplier *event_supplier;
	event_supplier = Util::instance()->get_event_supplier();
	if (event_supplier != NULL)
	{
		Tango::DevFailed *ex;

//
// Build a pseudo exception
//
		if (err != NULL)
			ex = new Tango::DevFailed(*err);
		else
			ex = NULL;

//
// Build the attribute value object
//

		Tango::AttributeValue att_val;
		Tango::AttrQuality qual = att.get_quality();
		if (qual != Tango::ATTR_INVALID)
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

			CORBA::Any &a = att_val.value;
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
				
			case Tango::DEV_FLOAT : 		
				a <<= att.get_float_value();
				break;

			case Tango::DEV_BOOLEAN :
				a <<= att.get_boolean_value();
				break;

			case Tango::DEV_USHORT :
				a <<= att.get_ushort_value();
				break;

			case Tango::DEV_UCHAR :
				a <<= att.get_uchar_value();
				break;
			}
		}

		att_val.time = att.get_when();
		att_val.quality = qual;
		att_val.name = CORBA::string_dup(att.get_name().c_str());
		att_val.dim_x = att.get_x();
		att_val.dim_y = att.get_y();
		
//
// Push the event
//

		try
		{
			vector<string> filterable_names_lg;
			vector<long> filterable_data_lg;
					
			event_supplier->push_event(this,
					   "user_event",
					   filterable_names,
					   filterable_value,
					   filterable_names_lg,
					   filterable_data_lg,
					   att_val,
					   att.get_name(),
					   ex);
		}
		catch (...)
		{
			delete ex;
			throw;
		}
					   
		delete ex;
							       
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
DeviceImplExt::DeviceImplExt()
  : exported(false),
    polled(false),
    poll_ring_depth(0),only_one("dev"),
    logger(0),saved_log_level(log4tango::Level::WARN),
    rft(Tango::kDefaultRollingThreshold),
    store_in_bb(true),poll_mon("cache"),
    att_conf_mon("att_config"),state_from_read(false),
    py_device(false)
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
}


} // End of Tango namespace

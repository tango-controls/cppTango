//=============================================================================	
//
// file :		Device.h
//
// description :	Include for the Device root classes called DeviceImpl 
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.25  2007/05/17 07:59:07  taurel
// - The polling is not configured via a separate thread any more. The polling thread add_obj_polling method has been modified to support a parameter telling to the polling thread when it has to polled the object.
// Add device name in monitor print message
// Add device_destroyer method in DeviceClass class
//
// Revision 3.24  2007/04/20 14:40:28  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.23  2007/04/16 14:56:36  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.22  2007/03/06 08:18:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.21  2007/02/16 10:40:09  taurel
// - Implementing a new types of event on the Attribute configuration
//
// Revision 3.20  2006/06/29 11:49:45  jensmeyer
// Added init_attr_poll_ext_trig() and init_attr_poll_period() to initialise the attribute
// polling from the code.
//
// Revision 3.19  2006/06/27 17:33:42  jensmeyer
// Added init_cmd_poll_ext_trig() and init_cmd_poll_period() to write the polling configuration for commands to the database.
// When specifying a polling of a command with Pogo now the configuration
// get written into the database at startup time.
//
// Revision 3.18  2006/06/06 10:44:05  taurel
// - Modified the DeviceImpl::push_event() method used to send user event.
// It is now coherent with the other events type (change and archhive)
//
// Revision 3.17  2006/05/30 11:17:56  jensmeyer
// Modifications on push_event functionality for MS Windows.
// Mainly modifications for timing and time stamps.
//
// Revision 3.16  2006/05/23 14:51:02  jensmeyer
// Corrected typing error.
//
// Revision 3.15  2006/05/23 12:05:56  jensmeyer
// Added methods to push change and archive events.
//
// Revision 3.14  2006/05/18 07:57:19  taurel
// - Updated to gcc 4 (minor changes)
// - With the change done by SourceForge on their CVS servers, previous release
// of these files has disappeared from CVS !!
//
// Revision 3.14  2006/05/08 07:16:07  taurel
// - Small changes for GCC 4
//
// Revision 3.13  2006/04/27 13:47:11  taurel
// - Merge the two DeviceImpl::push_event() methods in only one with only one method supporting also state or status attributes
//
// Revision 3.12  2006/04/13 13:31:44  taurel
// Several changes:
// - A new DeviceImpl::push_event() method to push USER event with State as data
// - Fix bug in polling thread POLL_IUPD_PERIOD. The new polling period was applied only after the following poll still done with the old polling period
// - It is now possible to update polling period of polled objects from a polled attribute or command
//
// Revision 3.11  2006/01/27 14:27:09  taurel
// - Fix a severe incompatibility problem introduced by all the modifs done for
// PY DS
// - Duplicate some EventSupplier class methods (instead of using template) in order to be able to generate Tango shared library on Suse 9.3
//
// Revision 3.10  2006/01/20 08:22:29  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 3.9  2005/07/28 07:34:41  taurel
// - Fix some incompatibility between 5.1 and 5.2
//
// Revision 3.8  2005/06/29 08:31:16  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.7  2005/02/25 13:28:51  nleclercq
// Added logging support in 'const' methods
//
// Revision 3.6  2005/01/21 19:58:30  taurel
// - Some changes in include files for gcc 3.4.2
//
// Revision 3.5  2005/01/13 08:29:05  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.4.2.6  2004/10/27 05:59:46  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.4.2.5  2004/10/22 11:25:00  taurel
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
// Revision 3.4.2.4  2004/09/27 09:09:07  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.4.2.3  2004/09/15 06:45:44  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.4.2.2  2004/08/19 07:44:03  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.4.2.1  2004/07/08 11:02:12  taurel
// - Add possibility to have command/attribute polling buffer depth specified
//   individually
//
// Revision 3.4  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.3  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.2  2003/08/21 07:23:46  taurel
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
// Revision 3.1.2.12  2004/06/01 15:15:25  taurel
// - Forget a f...  ";" before the last commit....
//
// Revision 3.1.2.11  2004/06/01 14:45:28  taurel
// - Change the string called device_name_lower to be the last data member
//   of the DeviceImplExt class. It was the reason of incompatibility between
//   release 4.1 and 4.2
//
// Revision 3.1.2.10  2004/05/24 08:44:05  taurel
// - Fix bug if device name stored in db used upper case. Add a DeviceImpl::get_name_lower() method
//
// Revision 3.1.2.9  2004/05/19 09:23:48  taurel
// - Change one local variable name for Solaris compilers
//
// Revision 3.1.2.8  2004/05/18 10:55:12  taurel
// - Modified DeviceImpl::push_event() doc
//
// Revision 3.1.2.7  2004/05/17 15:31:50  taurel
// - DeviceImpl::get_db_device() now throws exception if used in a Non Database DS
//
// Revision 3.1.2.6  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.1.2.5  2004/02/27 16:47:00  taurel
// - Updated DeviceImpl and Util classes sizes for compatibility
//
// Revision 3.1.2.4  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.1.2.3  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.1.2.2  2003/12/10 16:08:56  taurel
// Last commit before merging with the event branch.
// Revision 3.0.2.3  2003/07/17 03:46:46  andy_gotz
// Added new events "on_valid" and "on_state_change"; factorized EventSupplier::detect_and_push_events() method;
// Catching more exceptions when creating EventSupplier, device server will start without Notification service;
// Added AttributeProxy and DbAttribute classes to api; created test programs in test directory
//
// Revision 3.1.2.1  2003/09/30 11:50:43  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.1  2003/05/16 08:46:15  taurel
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
// Revision 3.0.2.2  2003/04/13 22:12:14  andy_gotz
// added heartbeat; polling starts automatically on subscription
//
// Revision 3.0.2.1  2003/04/10 11:07:44  andy_gotz
// added event commands to DServer admin object
//
// Revision 3.0  2003/03/25 16:41:55  taurel
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
//=============================================================================

#ifndef _DEVICE_H
#define _DEVICE_H

#include <tango.h>
#include <blackbox.h>
#include <classattribute.h>
#include <attrdesc.h>
#include <attribute.h>
#include <w_attribute.h>
#include <multiattribute.h>
#include <pollobj.h>
#include <deviceclass.h>

namespace Tango
{

class Command;
class DeviceClass;
class AutoTangoMonitor;
class NoSyncModelTangoMonitor;
class EventSupplier;
class EventSubscriptionChangeCmd;
class Util;


//=============================================================================
//
//			The DeviceImpl class
//
//
// description :	This class is derived directly from the Tango::Device_skel
//			class generated by CORBA. It represents the CORBA 
//			servant which will be accessed by the client. 
//			It implements all the methods
//			and attributes defined in the IDL interface for Device.
//
//=============================================================================

class DeviceImplExt
{
public:
#ifdef TANGO_HAS_LOG4TANGO
	DeviceImplExt(const char *);
#else

//
// 				WARNING : 
// 				---------
//
// If you add a new data member, don't forget to also add it in the
// ctor used when TANGO_HAS_LOG$TANGO is ON (which is true 90% of time)
// This ctor is in device.cpp file
//
	DeviceImplExt(const char *d_name):exported(false),polled(false),poll_ring_depth(0)
			only_one(d_name),store_in_bb(true),poll_mon("cache"),
			att_conf_mon("att_config"),state_from_read(false),
			py_device(false) {};
#endif
	~DeviceImplExt();
	
	bool				exported;
	bool				polled;
	long				poll_ring_depth;
	long				poll_old_factor;
	vector<string>		polled_cmd;
	vector<string>		polled_attr;
	vector<string>		non_auto_polled_cmd;
	vector<string>		non_auto_polled_attr;
	vector<PollObj *>	poll_obj_list;
	
	TangoMonitor		only_one;		// Device monitor
	Tango::DevState		device_prev_state;	// Device previous state

#ifdef TANGO_HAS_LOG4TANGO
  	log4tango::Logger* 	logger;
  	log4tango::Level::Value saved_log_level;
  	size_t rft;
#endif
	string				device_name_lower;
	long				idl_version;
	
	vector<string>		cmd_poll_ring_depth;
	vector<string>		attr_poll_ring_depth;
	
	bool				store_in_bb;
	TangoMonitor		poll_mon;		// Polling list monitor
	TangoMonitor		att_conf_mon;		// Attribute config monitor
	bool				state_from_read;
	vector<long>		alarmed_not_read;
	
	bool				py_device;
	string				alias_name_lower;	// Alias name (if any)
};

/**
 * Base class for all TANGO device.
 *
 * This class inherits from CORBA classes where all the network layer is
 * implemented.
 *
 * $Author$
 * $Revision$
 */

class DeviceImpl : public virtual POA_Tango::Device
{
public:
	friend class Tango::AutoTangoMonitor;
	friend class Tango::NoSyncModelTangoMonitor;
	friend class Tango::EventSupplier;
	friend class Tango::EventSubscriptionChangeCmd;

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated DeviceImpl object from its name.
 * 
 * The device description field is set to <i>A Tango device</i>. The device
 * state is set to unknown and the device status is set to 
 * <b>Not Initialised</b>
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 *
 */
	DeviceImpl(DeviceClass *device_class,string &dev_name);
	
/**
 * Constructs a newly allocated DeviceImpl object from its name and its description.
 *
 * The device
 * state is set to unknown and the device status is set to 
 * <i>Not Initialised</i>
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 * @param	desc	The device description
 *
 */
	DeviceImpl(DeviceClass *device_class,string &dev_name,string &desc);
/**
 * Constructs a newly allocated DeviceImpl object from all its creation 
 * parameters.
 *
 * The device is constructed from its name, its description, an original state
 * and status
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 * @param	desc 	The device description
 * @param	dev_state 	The device initial state
 * @param	dev_status	The device initial status
 *
 */
	DeviceImpl(DeviceClass *device_class,
	           string &dev_name,string &desc,
	           Tango::DevState dev_state,string &dev_status);
/**
 * Constructs a newly allocated DeviceImpl object from all its creation 
 * parameters with some default values.
 *
 * The device is constructed from its name, its description, an original state
 * and status. This constructor defined default values for the description,
 * state and status parameters. The default device description is <i>A TANGO device</i>.
 * The default device state is <i>UNKNOWN</i> and the default device status 
 * is <i>Not initialised</i>.
 *
 * @param 	device_class	Pointer to the device class object
 * @param	dev_name	The device name
 * @param	desc	The device desc
 * @param	dev_state 	The device initial state
 * @param	dev_status	The device initial status
 *
 */
 
	DeviceImpl(DeviceClass *device_class,
	           const char *dev_name,const char *desc = "A TANGO device",
	           Tango::DevState dev_state = Tango::UNKNOWN,
	           const char *dev_status = StatusNotSet);
		   
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The device desctructor.
 */
	virtual ~DeviceImpl();
//@}

/**@name Get/Set object members.
 * These methods allows the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Get device status.
 *
 * Return the device dev_status field. This method does the same thing than the
 * default status_cmd method.
 *
 * @return Device status
 */	
	string &get_status() {return device_status;}
	
/**
 * Set device status.
 *
 * @param new_status The new device status
 */
	void set_status(const string &new_status) {device_status = new_status;}
		
/**
 * Appends a string to the device status.
 *
 * @param stat The string to be appened to the device status
 * @param new_line If true, appends a new line character before the string
 */
	void append_status(const string &stat,bool new_line=false) {if(new_line==true)device_status.append(1,'\n');device_status.append(stat);}
	
/**
 * Get device state.
 *
 * Return the device dev_state field. This method does the same thing than the
 * default state_cmd method.
 *
 * @return Device state
 */	
	Tango::DevState &get_state() {return device_state;}
	
/**
 * Get device's previous state.
 *
 * Return the device dev_prev_state field. This method is used for the on_state_change event
 *
 * @return Device previous state
 */	
	Tango::DevState &get_prev_state() {return ext->device_prev_state;}
/**
 * Set device state.
 *
 * @param new_state The new device state
 */
	void set_state (const Tango::DevState &new_state) {ext->device_prev_state = device_state; device_state = new_state;}

/**
 * Get device name.
 *
 * Return the device name (dev_name field)
 *
 * @return Device name
 */
 	
	string &get_name() {return device_name;}
	
/**
 * Get device class singleton.
 *
 * Return the device class singleton (device_class field)
 *
 * @return Pointer to the device class singleton
 */
 	
	DeviceClass *get_device_class() {return device_class;}
	
/**
 * Get device multi attribute object.
 *
 * Return a pointer to the device multi attribute object
 *
 * @return Pointer to the device multi attribute object
 */
 	
	MultiAttribute *get_device_attr() {return dev_attr;}
	
/**
 * Set device multi attribute object.
 *
 * Set the pointer to the device multi attribute object
 *
 * @return Pointer to the device multi attribute object
 */
 	
	void set_device_attr(MultiAttribute *ptr) {dev_attr = ptr;}

/**
 * Get a pointer to the associated DbDevice object.
 *
 * Return a pointer to DbDevice object associated with the device
 *
 * @return Pointer to the DbDevice object
 */
 	
	DbDevice *get_db_device();
	
/**
 * Set the associated CORBA object reference.
 *
 * Set the associated CORBA object reference. Tango supports only a one to 
 * one servant-CORBA object link.
 *
 * @param 	d	The CORBA object reference
 */
 	
	void set_d_var(Tango::Device_ptr d) {d_var = d;}
	
/**
 * Get the associated CORBA object reference.
 *
 * Get the associated CORBA object reference. Tango supports only a one to 
 * one servant-CORBA object link.
 *
 * @return The CORBA object reference
 */
 	
	Tango::Device_var get_d_var() {return d_var;}
	
/**
 * Set the associated CORBA object identifier.
 *
 * Set the associated CORBA object identifier.
 *
 * @param 	o	The CORBA object identifier
 */
 
	void set_obj_id(PortableServer::ObjectId_var o) {obj_id = o;}
	
/**
 * Get the associated CORBA object identifier.
 *
 * Return the CORBA object identifier as a _var type variable
 *
 * @return The CORBA object identifier
 */
 
	PortableServer::ObjectId_var &get_obj_id() {return obj_id;}
	
/**
 * Return device POA.
 *
 * Return a pointer to the device POA. This method is necessary for the
 * CORBA object implicit activation by the _this() method.
 *
 * @return Pointer to the device POA
 */	
	virtual PortableServer::POA_ptr _default_POA();
	
//@}	

/**@name CORBA attribute methods
 * Method defined to implement TANGO device CORBA attribute */
//@{
/**
 * Get device name.
 *
 * It's the master method executed when the device name is requested
 * via a CORBA attribute. It updates the device black-box and return the
 * device name
 *
 * @return The device name
 */
	virtual char *name() throw (CORBA::SystemException);

/**
 * Get administrator device name.
 *
 * It's the master method executed when the administrator device name is requested
 * via a CORBA attribute. It updates the device black-box and return the
 * administrator device name
 *
 * @return The device name
 */
	virtual char *adm_name() throw (CORBA::SystemException);
		
/**
 * Get device description.
 *
 * It's the master method executed when the device description is requested
 * via a CORBA attribute. It updates the device black-box and return the
 * device description field
 *
 * @return The device description
 */
	virtual char *description() throw (CORBA::SystemException);
	
/**
 * Get device status.
 *
 * It's the master method executed when the device status is requested
 * via a CORBA attribute. It updates the device black-box and return the
 * device state. This method calls the <i>status_cmd</i> device method but 
 * catch all the execption and does not re-throw them because exception can't
 * be thrown to a client for CORBA attribute
 *
 * @return The device status
 */
	virtual char *status() throw (CORBA::SystemException);
	
/**
 * Get device state.
 *
 * It's the master method executed when the device state is requested
 * via a CORBA attribute. It updates the device black-box and return the
 * device state. This method calls the <i>state_cmd</i> device method but 
 * catch all the execption and does not re-throw them because exception can't
 * be thrown to a client for CORBA attribute
 *
 * @return The device state
 */
	virtual Tango::DevState state() throw (CORBA::SystemException);
//@}


/**@name CORBA operation methods
 * Method defined to implement TANGO device CORBA operation */
//@{
/**
 * Execute a command.
 *
 * It's the master method executed when a "command_inout" CORBA operation is
 * requested by a client. It updates the device black-box, call the 
 * TANGO command handler and returned the output Any
 *
 * @param in_cmd The command name
 * @param in_any The command input data packed in a CORBA Any
 * @return The command output data packed in a CORBA Any object
 * @exception DevFailed Re-throw of the exception thrown by the command_handler
 * method.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */			
	virtual CORBA::Any *command_inout(const char *in_cmd,
					  const CORBA::Any &in_data)
	throw (Tango::DevFailed, CORBA::SystemException);
	
/**
 * Get device black box.
 *
 * It's the master method executed when the device black box is requested.
 * It reads the device black box, update it and return black-box data to the
 * client
 *
 * @param n The number of actions description which must be returned to the
 * client. The number of returned element is limited to the number of elements
 * stored in the black-box or to the complete black-box depth if it is full.
 * @return The device black box with one String for each action requested on 
 * the device
 * @exception DevFailed If it is not possible to read the device black box.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual Tango::DevVarStringArray *black_box(CORBA::Long n)
	throw (Tango::DevFailed, CORBA::SystemException);
	
/**
 * Get device command list.
 *
 * Invoked when the client request the command_list_query CORBA operation.
 * It updates the device black box and returns an array of DevCmdInfo object
 * with one object for each command.
 *
 * @return The device command list. One DevCmdInfo is initialised for each
 * device command
 */
	virtual Tango::DevCmdInfoList *command_list_query()
	throw (Tango::DevFailed, CORBA::SystemException);
	
/**
 * Get command info.
 *
 * Invoked when the client request the command_query CORBA operation.
 * It updates the device black box and returns a DevCmdInfo object for the 
 * command with name passed
 * to the method as parameter.
 *
 * @param command The command name
 * @return A DevCmdInfo initialised for the wanted command
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual Tango::DevCmdInfo *command_query(const char *command)
	throw (Tango::DevFailed, CORBA::SystemException);
	
/**
 * Get device info.
 *
 * Invoked when the client request the info CORBA operation.
 * It updates the black box and returns a DevInfo object
 * with miscellaneous device info
 *
 * @return A DevInfo object
 */
	virtual Tango::DevInfo *info() throw (Tango::DevFailed, CORBA::SystemException);
	
/**
 * Ping the device to check if it is still alive.
 *
 * Invoked when the client request the ping CORBA operation.
 * It updates the device black box and simply returns
 *
 */
	virtual void ping() throw (Tango::DevFailed, CORBA::SystemException);
	
/**
 * Get attribute(s) configuration.
 *
 * Invoked when the client request the get_attribute_config CORBA operation.
 * It returns to the client one AttributeConfig structure for each wanted
 * attribute. All the attribute properties value are returned in this 
 * AttributeConfig structure.
 *
 * @param names The attribute(s) name list
 * @return A sequence of AttributeConfig structure. One structure is initialised
 * for each wanted attribute. Click <a href="../../../tango_idl/idl_html/Tango.html#AttributeConfig">here</a>
 * to read <b>AttributeConfig</b> structure specification.
 *
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual Tango::AttributeConfigList* get_attribute_config(const Tango::DevVarStringArray& names)
	throw (Tango::DevFailed, CORBA::SystemException);

/**
 * Set attribute(s) configuration.
 *
 * Invoked when the client request the set_attribute_config CORBA operation.
 * It updates the device attribute configuration actually used by the device but
 * this method also updates the Tango database. One structure of the
 * AttributeConfig type is needed for each attribute to update configuration.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#AttributeConfig">here</a>
 * to read <b>AttributeConfig</b> structure specification.
 *
 * @param new_conf The attribute(s) new configuration structure sequence
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual void set_attribute_config(const Tango::AttributeConfigList& new_conf)
    	throw (Tango::DevFailed, CORBA::SystemException);

/**
 * Read attribute(s) value.
 *
 * Invoked when the client request the read_attributes CORBA operation.
 * It returns to the client one AttributeValue structure for each wanted
 * attribute.
 *
 * @param names The attribute(s) name list
 * @return A sequence of AttributeValue structure. One structure is initialised
 * for each wanted attribute with the attribute value, the date and the attribute 
 * value quality. Click <a href="../../../tango_idl/idl_html/Tango.html#AttributeValue">here</a>
 * to read <b>AttributeValue</b> structure definition.
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual Tango::AttributeValueList* read_attributes(const Tango::DevVarStringArray& names)
	throw (Tango::DevFailed, CORBA::SystemException);

/**
 * Write attribute(s) value.
 *
 * Invoked when the client request the write_attributes CORBA operation.
 * It sets the attribute(s) with the new value(s) passed as parameter.
 *
 * @param values The attribute(s) new value(s). One structure is initialised
 * for each wanted attribute with the attribute value. The attribute quality and
 * date are not used by this method.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#AttributeValue">here</a>
 * to read <b>AttributeValue</b> structure definition.
 * @exception DevFailed Thrown if the command does not exist.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
    	virtual void write_attributes(const Tango::AttributeValueList& values)
	throw (Tango::DevFailed, CORBA::SystemException);

//@}

/**@name Methods to build Tango array types.
 * These methods are helper methods to build Tango array types from an already
 * existing buffer (Tango array types are CORBA sequences)
 */
//@{
/**
 * Create a DevVarCharArray type.
 *
 * Create a DevVarCharArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarCharArray type data
 */	
	inline Tango::DevVarCharArray *create_DevVarCharArray(unsigned char *ptr,long length)
	{
		return new Tango::DevVarCharArray(length,length,ptr,false);
	}
	
/**
 * Create a DevVarShortArray type.
 *
 * Create a DevVarShortArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarShortArray type data
 */	
	inline Tango::DevVarShortArray *create_DevVarShortArray(short *ptr,long length)
	{
		return new Tango::DevVarShortArray(length,length,ptr,false);
	}
	
/**
 * Create a DevVarLongArray type.
 *
 * Create a DevVarLongArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in the previous buffer
 *
 * @return Pointer to the created DevVarLongArray type data
 */	

	inline Tango::DevVarLongArray *create_DevVarLongArray(DevLong *ptr,long length)
	{
		return new Tango::DevVarLongArray(length,length,ptr,false);
	}

/**
 * Create a DevVarLong64Array type.
 *
 * Create a DevVarLong64Array type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in the previous buffer
 *
 * @return Pointer to the created DevVarLong64Array type data
 */	

	inline Tango::DevVarLong64Array *create_DevVarLong64Array(DevLong64 *ptr,long length)
	{
		return new Tango::DevVarLong64Array(length,length,ptr,false);
	}
	
/**
 * Create a DevVarFloatArray type.
 *
 * Create a DevVarFloatArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarFloatArray type data
 */	
	inline Tango::DevVarFloatArray *create_DevVarFloatArray(float *ptr,long length)
	{
		return new Tango::DevVarFloatArray(length,length,ptr,false);
	}
	
/**
 * Create a DevVarDoubleArray type.
 *
 * Create a DevVarDoubleArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarDoubleArray type data
 */	
	inline Tango::DevVarDoubleArray *create_DevVarDoubleArray(double *ptr,long length)
	{
		return new Tango::DevVarDoubleArray(length,length,ptr,false);
	}
	
/**
 * Create a DevVarUShortArray type.
 *
 * Create a DevVarUShortArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarUShortArray type data
 */	
	inline Tango::DevVarUShortArray *create_DevVarUShortArray(unsigned short *ptr,long length)
	{
		return new Tango::DevVarUShortArray(length,length,ptr,false);
	}
	
/**
 * Create a DevVarULongArray type.
 *
 * Create a DevVarULongArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarULongArray type data
 */	

	inline Tango::DevVarULongArray *create_DevVarULongArray(DevULong *ptr,long length)
	{
		return new Tango::DevVarULongArray(length,length,ptr,false);
	}

/**
 * Create a DevVarULong64Array type.
 *
 * Create a DevVarULong64Array type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in the previous buffer
 *
 * @return Pointer to the created DevVarULong64Array type data
 */	


	inline Tango::DevVarULong64Array *create_DevVarULong64Array(DevULong64 *ptr,long length)
	{
		return new Tango::DevVarULong64Array(length,length,ptr,false);
	}
	
/**
 * Create a DevVarStringArray type.
 *
 * Create a DevVarStringArray type data and return a pointer to it. The array is
 * build using the input pointer with the given length
 *
 * @param ptr	Pointer to the basic type data buffer
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarStringArray type data
 */	
	inline Tango::DevVarStringArray *create_DevVarStringArray(char **ptr,long length)
	{
		return new Tango::DevVarStringArray(length,length,ptr,false);
	}
//@}


/**@name Miscellaneous methods */
//@{
/**
 * Intialise a device.
 *
 * In the DeviceImpl class, this method is pure abstract and must be defined
 * in sub-class. Its rule is to initialise a device. This method is called
 * during device creation by the device constructor.
 *
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	virtual void init_device() = 0;
/**
 * Delete a device.
 *
 * In the DeviceImpl class, this method is virtual and can be defined
 * in sub-class. Its rule is to delete memory allocated in the init_device
 * method. This method is called by the device destructor and by the
 * device Init command.
 *
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	virtual void delete_device() {};	
/**
 * Hook method.
 *
 * Default method to implement an action necessary on a device before any
 * command is executed. This method can be redefined in 
 * sub-classes in case of the default behaviour does not fullfill the needs
 *
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual void always_executed_hook(void) {};

/**
 * Read the hardware to return attribute value(s).
 *
 * Default method to implement an action necessary on a device to read the
 * hardware involved in a a read attribute CORBA call.
 * This method must be redefined in sub-classes in order to support attribute
 * reading
 *
 * @param attr_list Reference to a vector with Integer object. Each element in
 * this vector
 * is the index in the device object attribute vector of an attribute to be read.
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual void read_attr_hardware(vector<long> &attr_list) {};
/**
 * Set the attribute read value.
 *
 * Default method to set an attribute read value.
 * This method must be redefined in sub-classes when attributes are needed
 *
 * @param attr The attribute object
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual void read_attr(Attribute &attr) {};
/**
 * Write the hardware for attributes.
 *
 * Default method to implement an action necessary on a device to write the
 * hardware involved in a a write attribute.
 * This method must be redefined in sub-classes in order to support writable
 * attribute
 *
 * @param attr_list Reference to a vector of Integer objects. Each element in
 * this vector
 * is the index in the main attribute vector of an attribute to be written.
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual void write_attr_hardware(vector<long> &attr_list) {};			
/**
 * Get device state.
 *
 * Default method to get device state. The behaviour of this method depends
 * on the device state. If the device state is ON or ALARM, it reads
 * the attribute(s) with an alarm level defined, check if the read value is
 * above/below the alarm and eventually change the state to ALARM, return the
 * device state. For all th eother device state, ti smethod simply returns
 * the state 
 * This method can be redefined in 
 * sub-classes in case of the default behaviour does not fullfill the needs
 *
 * @return The device state
 * @exception DevFailed If it is necessary to read attribute(s) and a problem
 * occurs during the reading.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual Tango::DevState dev_state();
	
/**
 * Get device status.
 *
 * Default method to get device status. It returns the contents of the device
 * dev_status field. If the device state is ALARM, alarm messages are
 * added to the device status. This method can be redefined in 
 * sub-classes in case of the default behaviour does not fullfill the needs
 *
 * @return The device status
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual Tango::ConstDevString dev_status();
	
/**
 * Add a new attribute to the device attribute list.
 *
 * Attributes are normally 
 * constructed in the DeviceClass::attribute_factory() method. Nevertheless, it
 * is still possible to add a new attribute to a device with this method.
 * Please, note that if you add an attribute to a device at device creation
 * time, this attribute will
 * be added to the device class attribute list. Therefore, all devices
 * belonging to the same class created after this attribute addition
 * will also have this attribute.
 *
 * @param new_attr Pointer to the new attribute to be added to the list. This pointer
 * must point to "heap" allocated memory (or to static memory) and not to "stack"
 * allocated memory
 * @exception DevFailed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	void add_attribute(Attr *new_attr);

/**
 * Remove one attribute from the device attribute list.
 *
 * Attributes are normally 
 * constructed in the DeviceClass::attribute_factory() method. Nevertheless, it
 * is still possible to add a new attribute to a device with the DeviceImpl::add_attribute method.
 * This remove_Attribute method delete the attribute from the
 * device attribute list.
 *
 * @param rem_attr Pointer to the attribute to be removed
 * @param free_it  Boolean set to true if the object passed as first argument
 *		   must be freed. Default value is false.
 * @exception DevFailed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	void remove_attribute(Attr *rem_attr,bool free_it = false);
	
/**
 * Remove one attribute from the device attribute list.
 *
 * Attributes are normally 
 * constructed in the DeviceClass::attribute_factory() method. Nevertheless, it
 * is still possible to add a new attribute to a device with the DeviceImpl::add_attribute method.
 * This remove_Attribute method delete the attribute from the
 * device attribute list.
 *
 * @param rem_attr_name The name of the attribute to be removed
 * @param free_it  Boolean set to true if the object passed as first argument
 *		   must be freed. Default value is false.
 * @exception DevFailed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	void remove_attribute(string &rem_attr_name,bool free_it = false);
		
/**
 * Retrieve index in polled object list.
 *
 * Retrieve in the device polled object list, the index of a specific polled object
 * (command or attribute).
 *
 * @param obj_type The object type (command or attribute)
 * @param obj_name The object name
 * @return An iterator pointing to the polled object in the polled object list
 * @exception DevFailed Thrown if the object is not found.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	vector<PollObj *>::iterator get_polled_obj_by_type_name(Tango::PollObjType obj_type,const string &obj_name);

/**
 * Write the command name to the list of polled commands in the database.
 * The polling period is set to 0 to indicate that the polling buffer
 * is filled externally from the device server code.
 * An already specified polling for this command gets replaced! 
 *
 * @exception DevFailed in the case of database access problems.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */			
	void init_cmd_poll_ext_trig (string cmd_name);	

/**
 * Checks the specified polling period for all commands of the device.
 * If a polling period is specified for a command the
 * command name and the period are written to the list of polled commands in the database.
 * This happens only if the command is not yet in the list of polled commands.
 *
 * @exception DevFailed in the case of database access problems.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */			
	void init_cmd_poll_period();
	
/**
 * Write the attribute name to the list of polled attributes in the database.
 * The polling period is set to 0 to indicate that the polling buffer
 * is filled externally from the device server code.
 * An already specified polling for this attribute gets replaced! 
 *
 * @exception DevFailed in the case of database access problems.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */			
	void init_attr_poll_ext_trig (string cmd_name);	

/**
 * Checks the specified polling period for all attributes of the device.
 * If a polling period is specified for an attribute the
 * attribute name and the period are written to the list of polled attributes in the database.
 * This happens only if the attribute is not yet in the list of polled attributes.
 *
 * @exception DevFailed in the case of database access problems.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */			
	void init_attr_poll_period();				
//@}




/**@name Push change event methods.
 * These methods allow to fire change events for attributes manually,
 *  without the polling to be started.
 */

//@{ 
/**
 * Set an implemented flag for the attribute to indicate that the server fires change events manually, 
  * without the polling to be started.
 * If the detect parameter is set to true, the criteria specified for the change 
 * event are verified and the event is only pushed if they are fullfilled.
 * If detect is set to false the event is fired without any value checking!
 * 
 * @param attr_name The name of the attribute
 * @param implemented True when the server fires change events manually. 
 * @param detect Triggers the verification of the change event properties when set to true. Default value is true.
 */
	void set_change_event  (string attr_name, bool implemented, bool detect = true); 

/**
 * Push a change event for a state or status attribute or return an exception as change
 * event for any attribute. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name as input.
 * For the state and status attributes the actual state and status values are pushed.
 * In case of an exception, the exception is pushed as a change event for the attribute.
 *
 * @param attr_name The name of the attribute
 * @param except Pointer to a Tango::DevFailed exception. Default value is NULL.
 */	
	void push_change_event (string attr_name, DevFailed *except = NULL);		

/**
 * Push a change event for an attribute with Tango::DevShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void push_change_event (string attr_name, Tango::DevShort *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevLong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_change_event (string attr_name, Tango::DevLong  *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a change event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_change_event (string attr_name, Tango::DevLong64  *p_data, long x = 1,long y = 0,bool release = false);
		
/**
 * Push a change event for an attribute with Tango::DevFloat attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_change_event (string attr_name, Tango::DevFloat *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevDouble attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	void push_change_event (string attr_name, Tango::DevDouble *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevString attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */			
	void push_change_event (string attr_name, Tango::DevString *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevBoolean attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	void push_change_event (string attr_name, Tango::DevBoolean *p_data, long x = 1,long y = 0,bool release = false);
	
/**void push_change_event (string attr_name, Tango::DevBoolea
 * Push a change event for an attribute with Tango::DevUShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	void push_change_event (string attr_name, Tango::DevUShort *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevUChar attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	void push_change_event (string attr_name, Tango::DevUChar *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a change event for an attribute with Tango::DevULong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_change_event (string attr_name, Tango::DevULong  *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevULong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_change_event (string attr_name, Tango::DevULong64  *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a change event for an attribute with Tango::DevState attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_change_event (string attr_name, Tango::DevState  *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a change event for an attribute with Tango::DevShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevShort *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 
	void push_change_event (string attr_name, Tango::DevShort *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a change event for an attribute with Tango::DevLong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevLong *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_change_event (string attr_name, Tango::DevLong *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a change event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevLong64 *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_change_event (string attr_name, Tango::DevLong64 *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
	
/**
 * Push a change event for an attribute with Tango::DevFloat attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevFloat *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_change_event (string attr_name, Tango::DevFloat *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a change event for an attribute with Tango::DevDouble attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevDouble *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_change_event (string attr_name, Tango::DevDouble *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a change event for an attribute with Tango::DevString attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevString *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_change_event (string attr_name, Tango::DevString *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a change event for an attribute with Tango::DevBoolean attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevBoolean *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_change_event (string attr_name, Tango::DevBoolean *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a change event for an attribute with Tango::DevUShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevUShort *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_change_event (string attr_name, Tango::DevUShort *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a change event for an attribute with Tango::DevUChar attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevUChar *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_change_event (string attr_name, Tango::DevUChar *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif

/**
 * Push a change event for an attribute with Tango::DevULong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevULong *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_change_event (string attr_name, Tango::DevULong *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a change event for an attribute with Tango::DevULong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevULong64 *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_change_event (string attr_name, Tango::DevULong64 *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a change event for an attribute with Tango::DevState attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the change event criteria depending
 * on the configuration choosen with set_change_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
#ifdef _TG_WINDOWS_
	void push_change_event (string attr_name, Tango::DevState *p_data, struct _timeb &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_change_event (string attr_name, Tango::DevState *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
//@}
	
	
	
	
/**@name Push archive event methods.
 * These methods allow to fire archive events for attributes manually,
 *  without the polling to be started.
 */

//@{ 
/**
 * Set an implemented flag for the attribute to indicate that the server fires archive events manually, 
  * without the polling to be started.
 * If the detect parameter is set to true, the criteria specified for the archive 
 * event are verified and the event is only pushed if they are fullfilled.
 * If detect is set to false the event is fired without any value checking!
 * 
 * @param attr_name The name of the attribute
 * @param implemented True when the server fires archive events manually. 
 * @param detect Triggers the verification of the archive event properties when set to true. Default value is true.
 */	
	void set_archive_event  (string attr_name, bool implemented, bool detect = true); 
	
/**
 * Push an archive event for a state or status attribute or return an exception as archive
 * event for any attribute. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name as input.
 * For the state and status attributes the actual state and status values are pushed.
 * In case of an exception, the exception is pushed as an archive event for the attribute.
 *
 * @param attr_name The name of the attribute
 * @param except Pointer to a Tango::DevFailed exception. Default value is NULL.
 */	
	void push_archive_event (string attr_name, DevFailed *except = NULL);

/**
 * Push an archive event for an attribute with Tango::DevShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	void push_archive_event (string attr_name, Tango::DevShort *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevLong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevLong  *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevLong64  *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevFloat attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevFloat *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevDouble attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevDouble *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevString attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevString *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevBoolean attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevBoolean *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevUShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevUShort *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevUChar attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevUChar *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevULong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevULong *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevULong64  *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push an archive event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_archive_event (string attr_name, Tango::DevState *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push an archive event for an attribute with Tango::DevShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevShort *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_archive_event (string attr_name, Tango::DevShort *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push an archive event for an attribute with Tango::DevLong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevLong *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_archive_event (string attr_name, Tango::DevLong *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif
/**
 * Push an archive event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevLong64 *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_archive_event (string attr_name, Tango::DevLong64 *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif		
/**
 * Push an archive event for an attribute with Tango::DevFloat attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevFloat *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_archive_event (string attr_name, Tango::DevFloat *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push an archive event for an attribute with Tango::DevDouble attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevDouble *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_archive_event (string attr_name, Tango::DevDouble *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push an archive event for an attribute with Tango::DevString attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevString *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_archive_event (string attr_name, Tango::DevString *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push an archive event for an attribute with Tango::DevBoolean attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevBoolean *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_archive_event (string attr_name, Tango::DevBoolean *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push an archive event for an attribute with Tango::DevUShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevUShort *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_archive_event (string attr_name, Tango::DevUShort *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push an archive event for an attribute with Tango::DevUChar attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevUChar *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_archive_event (string attr_name, Tango::DevUChar *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif
/**
 * Push an archive event for an attribute with Tango::DevULong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevULong *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_archive_event (string attr_name, Tango::DevULong *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif		
/**
 * Push an archive event for an attribute with Tango::DevULong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevULong64 *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_archive_event (string attr_name, Tango::DevULong64 *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif		
/**
 * Push an archive event for an attribute with Tango::DevState attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The event is triggered with or without checking of the archive event criteria depending
 * on the configuration choosen with set_archive_event().
 *
 * @param attr_name The name of the attribute
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_archive_event (string attr_name, Tango::DevState *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_archive_event (string attr_name, Tango::DevState *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif		
//@}




/**@name Push user event methods.
 * These methods allow to fire user events for attributes manually,
 *  without the polling to be started.
 */

//@{ 
/**
 * Push a user event for a state or status attribute or return an exception as user
 * event for any attribute. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name as input.
 * For the state and status attributes the actual state and status values are pushed.
 * In case of an exception, the exception is pushed as a user event for the attribute.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param except Pointer to a Tango::DevFailed exception. Default value is NULL.
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,DevFailed *except = NULL);

/**
 * Push a user event for an attribute with Tango::DevShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevShort *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevLong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevLong  *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a user event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevLong64  *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevFloat attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevFloat *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevDouble attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevDouble *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevString attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevString *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevBoolean attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevBoolean *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevUShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevUShort *p_data, long x = 1,long y = 0,bool release = false);
	
/**
 * Push a user event for an attribute with Tango::DevUChar attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevUChar *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a user event for an attribute with Tango::DevULong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevULong  *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a user event for an attribute with Tango::DevULong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevULong64  *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a user event for an attribute with Tango::DevState attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name and a pointer to the data to be pushed as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 * The time stamp of the event is set to the actual time and the attribute quality
 * is set to valid.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevState  *p_data, long x = 1,long y = 0,bool release = false);

/**
 * Push a user event for an attribute with Tango::DevShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevShort *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevShort *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a user event for an attribute with Tango::DevLong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevLong *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevLong *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a user event for an attribute with Tango::DevLong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevLong64 *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevLong64 *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a user event for an attribute with Tango::DevFloat attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevFloat *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevFloat *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a user event for an attribute with Tango::DevDouble attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevDouble *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevDouble *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a user event for an attribute with Tango::DevString attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevString *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevString *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a user event for an attribute with Tango::DevBoolean attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevBoolean *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevBoolean *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a user event for an attribute with Tango::DevUShort attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevUShort *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevUShort *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
/**
 * Push a user event for an attribute with Tango::DevUChar attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevUChar *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else 	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevUChar *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif

/**
 * Push a user event for an attribute with Tango::DevULong attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevULong *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevULong *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a user event for an attribute with Tango::DevULong64 attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevULong64 *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevULong64 *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	

/**
 * Push a user event for an attribute with Tango::DevState attribute data type. 
 * The event is pushed to the notification daemon.
 *
 * The method needs the attribue name, a pointer to the data to be pushed, the time stamp
 * for the data and the attribute quality factor as input.
 * Depending on the attribute type the dimensions x and why need to be given.
 *
 * @param attr_name The name of the attribute
 * @param filt_names The filterable fields name
 * @param filt_vals The filterable fields value (as double)
 * @param p_data Pointer to the data to be pushed
 * @param t The time stamp
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
#ifdef _TG_WINDOWS_
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevState *p_data, struct _timeb  &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#else  	
	void push_event (string attr_name,vector<string> &filt_names,vector<double> &filt_vals,Tango::DevState *p_data, struct timeval &t, Tango::AttrQuality qual, long x = 1,long y = 0,bool release = false);
#endif	
//@}


/**@name Signal related methods
 * These methods allow a signal management at device level */
//@{
#if (defined __linux)
/**
 * Register a signal to be executed in a signal handler.
 *
 * Register this device as device to be informed when signal signo is sent to
 * to the device server process. This method is available only under Linux.
 *
 * @param signo The signal number
 * @param own_handler A boolean set to true if you want the device signal handler
 * to be executed in its own handler instead of being executed by the signal
 * thread. If this parameter is set to true, care should be taken on how the
 * handler is written. A default false value is provided
 * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to register a signal for the process.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void register_signal(long signo,bool own_handler = false);
#else
/**
 * Register a signal.
 *
 * Register this device as device to be informed when signal signo is sent to
 * to the device server process
 *
 * @param signo The signal number
 * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to register a signal for the process.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void register_signal(long signo);
#endif
	
/**
 * Unregister a signal.
 *
 * Unregister this device as device to be informed when signal signo is sent to
 * to the device server process
 *
 * @param signo The signal number
 * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to unregister a signal for the process. Unregister
 * a device for a signal number for a device not previously registered is not
 * an error. This simply will do nothing.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void unregister_signal(long signo);
	
/**
 * Signal handler.
 *
 * The method executed when the signal arrived in the device server process.
 * This method is defined as virtual and then, can be redefined following
 * device needs.
 *
 * @param signo The signal number
 */
	virtual void signal_handler(long signo);
	
//@}			
	
	void set_exported_flag(bool exp) {ext->exported = exp;}
	bool get_exported_flag() {return ext->exported;}

	void set_poll_ring_depth(long depth) {ext->poll_ring_depth = depth;}
	long get_poll_ring_depth() {return ext->poll_ring_depth;}

	void set_poll_old_factor(long fact) {ext->poll_old_factor = fact;}
	long get_poll_old_factor() {return ext->poll_old_factor;}
			
	void is_polled(bool poll) {ext->polled = poll;}
	bool is_polled() {return ext->polled;}

	vector<string> &get_polled_cmd() {return ext->polled_cmd;}
	vector<string> &get_polled_attr() {return ext->polled_attr;}
	vector<string> &get_non_auto_polled_cmd() {return ext->non_auto_polled_cmd;}
	vector<string> &get_non_auto_polled_attr() {return ext->non_auto_polled_attr;}			
	vector<PollObj *> &get_poll_obj_list() {return ext->poll_obj_list;}
	void stop_polling();
	
	void check_command_exists(const string &);
	Command *get_command(const string &);
	
	string &get_name_lower() {return ext->device_name_lower;}
	
	TangoMonitor &get_dev_monitor() {return ext->only_one;}
	TangoMonitor &get_poll_monitor() {return ext->poll_mon;}
	TangoMonitor &get_att_conf_monitor() {return ext->att_conf_mon;}
	
	long get_dev_idl_version() {return ext->idl_version;}
	long get_cmd_poll_ring_depth(string &);
	long get_attr_poll_ring_depth(string &);
	vector<long> &get_alarmed_not_read() {return ext->alarmed_not_read;}
	void poll_lists_2_v5();
	
	bool is_py_device() {return ext->py_device;}	
	void set_py_device(bool py) {ext->py_device=py;}
	
	void set_alias_name_lower(string &al) {ext->alias_name_lower = al;}
	string &get_alias_name_lower() {return ext->alias_name_lower;}
	
	void push_att_conf_event(Attribute *);
			
#ifdef TANGO_HAS_LOG4TANGO
 	inline log4tango::Logger *get_logger(void)
	{return ext->logger ? ext->logger : get_logger_i();}
	
  	void init_logger(void);
  	void start_logging(void);
  	void stop_logging(void);
#endif // TANGO_HAS_LOG4TANGO
		
private:
//
// Some private methods and variables
//

	void get_dev_system_resource();
	void black_box_create();

#ifdef TANGO_HAS_LOG4TANGO
  	log4tango::Logger *get_logger_i (void);
#endif

	string							alarm_status;
	Tango::Device_var				d_var;
	PortableServer::ObjectId_var	obj_id;
	
			
//
// Device class data members
//
	
protected:

/**@name Class data members */
//@{
/** 
 * The device black box pointer
 */
	BlackBox		*blackbox_ptr;	
/** 
 * The device black box depth
 */
	long			blackbox_depth;
/** 
 * The device name
 */	
	string			device_name;
/** 
 * The device description
 */
	string			desc;
/** 
 * The device status
 */
	string			device_status;
/** 
 * The device state
 */
	Tango::DevState		device_state;
/** 
 * The device version
 */
	long			version;
/** 
 * Pointer to the device-class object associated with the device
 */
	DeviceClass		*device_class;
/**
 * Pointer to the multi attribute object
 */
 	MultiAttribute		*dev_attr;
/**
 * Pointer to the associated DbDevice object
 */
 	DbDevice		*db_dev;
/**
 * The administration device name
 */
 	string			adm_device_name;
//@}

	DeviceImplExt		*ext;
};




} // End of Tango namespace

#endif // _DEVICE_H

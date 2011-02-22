//=============================================================================	
//
// file :		Device.h
//
// description :	Include for the Device root classes. 
//			Three classes are declared in this file :
//				The Device class
//				The DeviceClass class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
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
// Revision 2.10  2003/03/11 17:55:51  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.9  2003/02/17 14:57:39  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
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
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
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
#include <multiattribute.h>
#include <pollobj.h>

namespace Tango
{

class Command;
class DeviceClass;
class AutoTangoMonitor;
class NoSyncModelTangoMonitor;


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
	DeviceImplExt();
#else
	DeviceImplExt():exported(false),polled(false),poll_ring_depth(0) {};
#endif
	~DeviceImplExt();
	
	bool			exported;
	bool			polled;
	long			poll_ring_depth;
	long			poll_old_factor;
	vector<string>		polled_cmd;
	vector<string>		polled_attr;
	vector<string>		non_auto_polled_cmd;
	vector<string>		non_auto_polled_attr;
	vector<PollObj *>	poll_obj_list;
	
	TangoMonitor		only_one;		// Polling monitor

#ifdef TANGO_HAS_LOG4TANGO
  	log4tango::Logger* logger;
  	log4tango::Level::Value saved_log_level;
  	size_t rft;
#endif
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

class DeviceImpl : public virtual POA_Tango::Device,
		   public PortableServer::RefCountServantBase
{
public:
	friend class Tango::AutoTangoMonitor;
	friend class Tango::NoSyncModelTangoMonitor;

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
	           const char *dev_status = "Not Initialised");
		   
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
 * Get device state.
 *
 * Return the device dev_state field. This method does the same thing than the
 * default state_cmd method.
 *
 * @return Device state
 */	
	Tango::DevState &get_state() {return device_state;}
	
/**
 * Set device state.
 *
 * @param new_state The new device state
 */
	void set_state (const Tango::DevState &new_state) {device_state = new_state;}

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
 	
	DbDevice *get_db_device() {return db_dev;}
	
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
 * @param length Number of element in  the previous buffer
 *
 * @return Pointer to the created DevVarLongArray type data
 */	
	inline Tango::DevVarLongArray *create_DevVarLongArray(long *ptr,long length)
	{
		return new Tango::DevVarLongArray(length,length,ptr,false);
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
	inline Tango::DevVarULongArray *create_DevVarULongArray(unsigned long *ptr,long length)
	{
		return new Tango::DevVarULongArray(length,length,ptr,false);
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

//@}

/**@name Signal related methods
 * These methods allow a signal management at device level */
//@{
#ifdef __linux
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
	
	TangoMonitor &get_dev_monitor() {return ext->only_one;}
	
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

	string				alarm_status;
	Tango::Device_var		d_var;
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



//=============================================================================
//
//			The DeviceClass class
//
//
// description : 	This class will act as root class for all other 
//			DeviceClass classes. Its job is to define all Device 
//			class related properties and methods which exist only 
//			once e.g. the command list.	
//
//=============================================================================

class DeviceClassExt
{
public:
	vector<string>		nodb_name_list;
	
	TangoMonitor		only_one;
};

/**
 * Base class for all TANGO device-class class. A TANGO device-class class is
 * a class where is stored all data/method common to all devices of a TANGO
 * device class
 *
 * $Author$
 * $Revision$
 */
 
//class DeviceClass: public JTCMonitor
class DeviceClass
{
	friend class Tango::AutoTangoMonitor;
public:
	
/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The device desctructor.
 */
	virtual ~DeviceClass();
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Execute a command.
 *
 * It looks for the correct command object in the command object vector.
 * If the command is found, it invoke the <i>always_executed_hook</i> method.
 * Check if the command is allowed by invoking the <i>is_allowed</i> method
 * If the command is allowed, invokes the <i>execute</i> method.
 *
 * @param 	device	The device on which the command must be executed
 * @param	command	The command name
 * @param	in_any 	The command input data still packed in a CORBA Any object
 * @return	A CORBA Any object with the output data packed in
 * @exception DevFailed If the command is not found, if the command is not allowed
 * in the actual device state and re-throws of all the exception thrown by the
 * <i>always_executed_hook</i>, <i>is_alloed</i> and <i>execute</i> methods.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	CORBA::Any *command_handler(DeviceImpl *device,
				    string &command,const CORBA::Any &in_any);
/**
 * Create command objects for all command supported by this class of device.
 *
 * In the DeviceClass class, this method is pure abstract and must be defined
 * in sub-class. Its rule is to create the command object and to store them
 * in a vector of command objects
 *
 */
	virtual void command_factory() = 0;
/**
 * Create all the attributes name supported by this class of device.
 *
 * In the DeviceClass class, this method does nothing and must be re-defined
 * in sub-class if the sub-class supports attributes. Its rule is to
 * store the supported attributes in a vector.
 *
 */
	virtual void attribute_factory(vector<Attr *> &att_list) {};
		
/**
 * Create device(s).
 *
 * In the DeviceClass class, this method is pure abstract and must be defined
 * in sub-class. Its rule is to create all the calss devices and to store them
 * in a vector of device
 *
 * @param dev_list The device name list
 * @exception DevFailed This method does not throw exception but a 
 * redefined method can.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual void device_factory(const Tango::DevVarStringArray *) = 0;
	
/**
 * Create device(s) name list (for no database device server).
 *
 * This method can be re-defined in DeviceClass sub-class for device server
 * started without database. Its rule is to initialise class device name.
 * The default method does nothing.
 *
 * @param list Reference to the device name list
 */	
	
	virtual void device_name_factory(vector<string> &list) {};
	
//@}
	
/**@name Get/Set object members.
 * These methods allows the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Get the TANGO device class name.
 *
 * @return The TANGO device class name
 */
	string &get_name() {return name;}
/**
 * Get the TANGO device class documentation URL.
 *
 * @return The TANGO device class documentation
 */
	string &get_doc_url() {return doc_url;}
/**
 * Get the device object vector.
 *
 * @return A reference to the device vector
 */	
	vector<DeviceImpl *> &get_device_list() {return device_list;}
/**
 * Get the command object vector.
 *
 * @return A reference to the command vector
 */
	vector<Command *> &get_command_list() {return command_list;}
/**
 * Get a pointer to the associated DbClass object.
 *
 * @return Pointer to the DbClas object
 */
 	
	DbClass *get_db_class() {return db_class;}
/**
 * Get a pointer to the class attributes object
 *
 * @return A pointer to the instance of the MultiClassAttribute
 */
 	MultiClassAttribute *get_class_attr() {return class_attr;}
//@}
	

/**@name Signal related methods
 * These methods allow a signal management at device level */
//@{
#ifndef __linux
/**
 * Register a signal.
 *
 * Register this class as class to be informed when signal signo is sent to
 * to the device server process
 *
 * @param signo The signal number
 * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to register a signal for the process.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void register_signal(long signo);
#else
/**
 * Register a signal.
 *
 * Register this class as class to be informed when signal signo is sent to
 * to the device server process. This method is available only under Linux.
 *
 * @param signo The signal number
 * @param own_handler A boolean set to true if you want the device signal handler
 * to be executed in its own handler instead of being executed by the signal
 * thread. If this parameter is set to true, care should be taken on how the
 * handler is written. A default false value is provided * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to register a signal for the process.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void register_signal(long signo,bool own_handler = false);
#endif
	
/**
 * Unregister a signal.
 *
 * Unregister this class as class to be informed when signal signo is sent to
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
 * device class needs.
 *
 * @param signo The signal number
 */
	virtual void signal_handler(long signo);	

//@}

	vector<string> &get_nodb_name_list() {return ext->nodb_name_list;}
				
protected:
/**@name Constructor
 Only one constructot for this class which is a singleton */
//@{
/**
 * Construct a newly allocated DeviceClass object.
 *
 * @param 	s	The Tango device class name
 *
 */
	DeviceClass(string &s);
//@}

/**@name Miscellaneous protected methods */
//@{	
/**
 * Export a device.
 *
 * Associate the servant to a CORBA object and send device network parameter
 * to TANGO database. The main parameter sent to database is the CORBA 
 * object stringified device IOR.
 *
 * @param dev The device to be exported (CORBA servant)
 * @param corba_dev_name The name to be used in the CORBA object key. This
 * parameter does not need to be set in most of cases and has a default value.
 * It is used for special device server like the database device server.
 * @exception DevFailed If the command sent to the database failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void export_device(DeviceImpl *dev,const char* corba_dev_name = "Unused");
//@}


/**@name Class data members */
//@{
/**
 * The TANGO device class name
 */
	string 				name;
/**
 * The TANGO device class documentation URL
 */
	string				doc_url;
/**
 * The command(s) list
 */
	vector<Command *> 		command_list;
/**
 * The device(s) list
 */
	vector<DeviceImpl *>		device_list;
/**
 * The associated DbClass object
 */
	DbClass				*db_class;
/**
 * Pointer to the class multi attribute object
 */
 	MultiClassAttribute		*class_attr;
//@}
	
private:	
	void get_class_system_resource();
	
	DeviceClassExt			*ext;
};


//=============================================================================
//
//			The AutoTangoMonitor class
//
//
// description :	This class is only a helper class used to get a
//			TangoMonitor object during its construction and to 
//			it during its destruction. It makes developper life
//			easier.
//
//=============================================================================

class AutoTangoMonitor
{
public:
	AutoTangoMonitor(Tango::DeviceImpl *dev, bool force = false)
	{
		SerialModel ser = Util::instance()->get_serial_model();

		switch(ser)
		{
		case NO_SYNC:
			if (force == true)
			{
				mon = &(dev->ext->only_one);
			}
			else
				mon = NULL;
			break;			

		case BY_DEVICE:
			mon = &(dev->ext->only_one);
			break;

		case BY_CLASS:
			mon = &(dev->device_class->ext->only_one);
			break;

		case BY_PROCESS:
			mon = &(Util::instance()->ext->only_one);
			break;
		}
		if (mon)
			mon->get_monitor();

	}

	AutoTangoMonitor(Tango::DeviceClass *dev_cl)
	{
		SerialModel ser = Util::instance()->get_serial_model();

		switch(ser)
		{
		case NO_SYNC:
		case BY_DEVICE:
			mon = NULL;
			break;
					
		case BY_CLASS:
			mon = &(dev_cl->ext->only_one);
			mon->get_monitor();
			break;
		
		case BY_PROCESS:
			mon = &(Util::instance()->ext->only_one);
			mon->get_monitor();
			break;
		}
	}
	
	~AutoTangoMonitor() {if (mon)mon->rel_monitor();}

private:
	TangoMonitor *mon;
};


class NoSyncModelTangoMonitor
{
public:
	NoSyncModelTangoMonitor(Tango::DeviceImpl *dev)
	{
		SerialModel ser = Util::instance()->get_serial_model();
		if (ser == NO_SYNC)
		{
			mon = &(dev->ext->only_one);
			mon->get_monitor();
		}
		else
			mon = NULL;
	}
	
	~NoSyncModelTangoMonitor() {if (mon)mon->rel_monitor();}

private:
	TangoMonitor *mon;
};

} // End of Tango namespace

#endif // _DEVICE_H

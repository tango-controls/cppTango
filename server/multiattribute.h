//=============================================================================	
//
// file :		MultiAttribute.h
//
// description :	Include file for the MultiAttribute class. 
//			Each device has one object of this class. All device
//			attribute objects are stored in this class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:16:22  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
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
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:13  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:11  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:14  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:45  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:16  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//=============================================================================

#ifndef _MULTIATTRIBUTE_H
#define _MULTIATTRIBUTE_H

#include <tango.h>

namespace Tango
{

class AttrProperty;
class DeviceClass;

class MultiAttributeExt
{
};

//=============================================================================
//
//			The MultiAttribute class
//
//
// description :	There is one instance of this class for each device.
//			This is mainly a helper class. It maintains a vector
//			of all the attribute for the device
//
//=============================================================================

/**
 * There is one instance of this class for each device. This class is mainly
 * an aggregate of Attribute or WAttribute objects. It eases management of 
 * multiple attributes
 *
 * $Author$
 * $Revision$
 */
 
class MultiAttribute
{
public:
/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Create a new MultiAttribute object.
 *
 * This constructor will in-turn call the constructor of the Attribute or
 * WAttribute class of all the device class attributes.
 *
 * @param dev_name The device name
 * @param dev_class Reference to the device DeviceClass object
 * @exception DevFailed If the command sent to the database failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	MultiAttribute(string &dev_name,DeviceClass *dev_class);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The MultiAttribute desctructor.
 */
	~MultiAttribute();
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Get Attribute object from its name.
 *
 * This method returns a reference to the Attribute object with a name passed
 * as parameter. The equality on attribute name is case independant.
 *
 * @param attr_name The attribute name
 * @return A reference to the Attribute object
 * @exception DevFailed If the attribute is not defined.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	Attribute &get_attr_by_name(const char *attr_name);
/**
 * Get Attribute object from its index.
 *
 * This method returns a reference to the Attribute object from the index in the
 * main attribute vector
 *
 * @param ind The attribute index
 * @return A reference to the Attribute object
 */
	Attribute &get_attr_by_ind(const long ind) {return *(attr_list[ind]);}
/**
 * Get Writable Attribute object from its name.
 *
 * This method returns a reference to the WAttribute object with a name passed
 * as parameter. The equality on attribute name is case independant.
 *
 * @param attr_name The attribute name
 * @return A reference to the writable attribute object
 * @exception DevFailed If the attribute is not defined.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	WAttribute &get_w_attr_by_name(const char *attr_name);
/**
 * Get Writable Attribute object from its index.
 *
 * This method returns a reference to the Writable Attribute object from the
 * index in the main attribute vector
 *
 * @param ind The attribute index
 * @return A reference to the WAttribute object
 */
	WAttribute &get_w_attr_by_ind(const long ind) {return static_cast<WAttribute &>(*(attr_list[ind]));}
/**
 * Get Attribute index into the main attribute vector from its name.
 *
 * This method returns the index in the Attribute vector (stored in the
 * MultiAttribute object) of an attribute with a given name. The name equality
 * is case independant
 *
 * @param attr_name The attribute name
 * @retrun The index in the main attributes vector
 * @exception DevFailed If the attribute is not found in the vector.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	long get_attr_ind_by_name(const char *attr_name);
/**
 * Get list of attribute with an alarm level defined.
 *
 * @return  A vector of long data. Each object is the index in the main
 * attribute vector of attribute with alarm level defined
 */	
	vector<long> &get_alarm_list() {return alarm_attr_list;}
/**
 * Get attribute number.
 *
 * @return  The attribute number
 */
	long get_attr_nb() {return attr_list.size();}

/**
 * Check alarm for one attribute with a given name.
 *
 * This method returns a boolean set to true if the attribute with the given
 * name is in alarm condition
 *
 * @param  attr_name  The attribute name
 * @return A boolean set to true if the attribute is in alarm
 * @exception DevFailed If the attribute does not have any alarm level defined.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */		
	bool check_alarm(const char *attr_name) {return get_attr_by_name(attr_name).check_alarm();}
/**
 * Check alarm for one attribute from its index in the main attributes vector.
 *
 * This method returns a boolean set to true if the attribute with the given
 * index in the attrobite object vector is in alarm condition
 *
 * @param  ind  The attribute index
 * @return A boolean set to true if the attribute is in alarm
 * @exception DevFailed If the attribute does not have any alarm level defined.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm(const long ind) {return get_attr_by_ind(ind).check_alarm();}
/**
 * Check alarm on all attribute(s) with an alarm defined.
 *
 * This method returns a boolean set to true if one of the attribute with an 
 * alarm level defined is in alarm condition.
 *
 * @return A boolean set to true if one attribute is in alarm
 * @exception DevFailed If the alarm level are not defined for one of the
 * attribute in the list of alarmable one
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	bool check_alarm();
/**
 * Add alarm message to device status.
 *
 * This method add alarm mesage to the string passed as parameter. A message
 * is added for each attribute which is in alarm condition
 *
 * @param  status  The string (should be the device status)
 */
	void read_alarm(string &status);
//@}
	
	void add_write_value(Attribute &);
	void add_attribute(string &,DeviceClass *,long);

private:
	void concat(vector<AttrProperty> &,vector<AttrProperty> &,vector<AttrProperty> &);
	void add_default(vector<AttrProperty> &);
	void add_user_default(vector<AttrProperty> &,vector<AttrProperty> &);
	void check_associated(long,string &);
	
	MultiAttributeExt		*ext;
	
protected:
/**@name Class data members */
//@{
/**
 * The Attribute objects vector.
 *
 * This vector is often referred as the main attributes vector
 */	
	vector<Attribute *>	attr_list;
/**
 * The list of writable attribute.
 *
 * It is a vector of index in the main attribute vector
 */
	vector<long>		writable_attr_list;
/**
 * The list of attribute with an alarm level defined.
 *
 * It is a vector of index in the main attribute vector
 */
	vector<long>		alarm_attr_list;
//@}
};

} // End of Tango namespace

#endif // _MULTIATTRIBUTE_H

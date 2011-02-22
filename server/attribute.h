//=============================================================================	
//
// file :		Attribute.h
//
// description :	Include file for the Attribute classes. 
//			Two classes are declared in this file :
//				The Attribute class
//				The WAttribute class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.6.2.5  2004/11/10 13:00:54  taurel
// - Some minor changes for the WIN32 port
//
// Revision 3.6.2.4  2004/10/22 11:25:00  taurel
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
// Revision 3.6.2.3  2004/09/27 09:09:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.6.2.2  2004/09/15 06:45:44  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.6.2.1  2004/07/15 15:04:06  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.6  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.5  2003/09/02 13:08:14  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.4  2003/08/22 12:52:54  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.3  2003/08/21 07:23:46  taurel
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
// Revision 3.2  2003/07/03 07:40:51  taurel
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
// Revision 3.1.2.8  2004/03/02 07:41:56  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.1.2.7  2004/02/25 16:27:44  taurel
// Minor changes to compile library using Solaris CC compiler
//
// Revision 3.1.2.6  2004/02/18 15:06:17  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 3.1.2.5  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.1.2.4  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.1.2.3  2003/12/10 16:08:56  taurel
// Last commit before merging with the event branch.
// Revision 3.0.2.4  2003/11/16 22:10:43  andy_gotz
// New version which defines 4 types of events - change, quality, periodic and
// archive. Code has been factorised to reduce redundancy. Minimum and maximum
// changes are supported. Event period is taken into account. Relative and
// absolute changes are detected. Whole sequence is taken into account when
// determining change.
//
// Revision 3.0.2.3  2003/07/17 03:46:46  andy_gotz
// Added new events "on_valid" and "on_state_change"; factorized EventSupplier::detect_and_push_events() method;
// Catching more exceptions when creating EventSupplier, device server will start without Notification service;
// Added AttributeProxy and DbAttribute classes to api; created test programs in test directory
//
// Revision 3.1.2.2  2003/10/02 07:36:34  taurel
// CHnage the Attribute::set_date() method from private to public
// Revision 3.0.2.2  2003/04/13 22:12:14  andy_gotz
// added heartbeat; polling starts automatically on subscription
//
// Revision 3.1.2.1  2003/09/30 11:49:56  taurel
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
// Revision 3.0.2.1  2003/04/08 13:12:45  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:41:50  taurel
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
// Revision 2.10  2003/03/11 17:55:47  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.9  2003/02/17 14:57:38  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.8  2002/12/16 12:06:21  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:15:35  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:05  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:18  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:53  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:23  taurel
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
// Revision 2.2  2002/04/30 10:50:40  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:02  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:08  taurel
// See Tango WEB pages for list of changes
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//=============================================================================

#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include <tango.h>
#include <attrdesc.h>
#include <functional>
#include <time.h>

namespace Tango
{

//
// Binary function objects to be used by the find_if algorithm.
// The find_if algo. want to have a predicate, this means that the return value
// must be a boolean (R is its name).
// The test is done between a AttrProperty object (name A1) and a string (name A2)
// The find_if algo. needs a unary predicate. This function object is a binary
// function object. It must be used with the bind2nd function adapter
//

template <class A1, class A2, class R>
struct WantedProp : public  binary_function<A1,A2,R>
{
	R operator() (A1 att,A2 name_str) const
	{
		return att.get_name() == name_str;
	}
};

template <class A1, class A2, class R>
struct WantedAttr : public binary_function<A1,A2,R>
{
	R operator() (A1 attr_ptr, A2 name) const
	{
		string st(name);
		if (st.size() != attr_ptr->get_name_size())
			return false;
		transform(st.begin(),st.end(),st.begin(),::tolower);
		return attr_ptr->get_name_lower() == st;
	}
};


class AttrProperty;

typedef union _Attr_CheckVal
{
	short		sh;
	long		lg;
	double		db;
	float 		fl;
	unsigned short	ush;
	unsigned char	uch;
}Attr_CheckVal;

typedef union _Attr_Value
{
	DevVarShortArray  	*sh_seq;
	DevVarLongArray   	*lg_seq;
	DevVarFloatArray  	*fl_seq;
	DevVarDoubleArray 	*db_seq;
	DevVarStringArray 	*str_seq;
	DevVarUShortArray 	*ush_seq;
	DevVarBooleanArray	*boo_seq;
	DevVarCharArray		*cha_seq;
}Attr_Value;


typedef struct last_attr_value {
	bool 			inited;
	Tango::AttrQuality 	quality;
	CORBA::Any 		value;
	bool 			err;
} LastAttrValue;

class EventSupplier;

//=============================================================================
//
//			The Attribute class
//
//
// description :	There is one instance of this class for each attribute
//			for each device. This class stores the attribute
//			properties and the attribute value.
//
//=============================================================================

class AttributeExt
{
public:
	AttributeExt() {}
	
 	Tango::DispLevel 	disp_level;			// Display level
 	long			poll_period;			// Polling period
	double			rel_change[2];			// Delta for relative change events in %
	double			abs_change[2];			// Delta for absolute change events
	double			archive_rel_change[2];		// Delta for relative archive change events in %
	double			archive_abs_change[2];		// Delta for absolute change events
	int			event_period;			// Delta for periodic events in ms
	int			archive_period;			// Delta for archive periodic events in ms
	double			last_periodic;			// Last time a periodic event was detected
	double			archive_last_periodic;		// Last time an archive periodic event was detected
	long			periodic_counter;		// Number of periodic events sent so far
	long			archive_periodic_counter;	// Number of periodic events sent so far
	LastAttrValue		prev_change_event;		// Last change attribute
	LastAttrValue		prev_quality_event;		// Last quality attribute
	LastAttrValue		prev_archive_event;		// Last archive attribute
	int			event_change_subscription;	// Last time() a subscription was made
	int			event_quality_subscription;	// Last time() a subscription was made
	int			event_periodic_subscription;	// Last time() a subscription was made
	int			event_archive_subscription; 	// Last time() a subscription was made
	int			event_user_subscription; 	// Last time() a subscription was made
	long			idx_in_attr;			// Index in MultiClassAttribute vector
};

/**
 * This class represents a Tango attribute.
 *
 * $Author$
 * $Revision$
 */
 
class Attribute
{
public:

	enum alarm_flags
	{
		min_level,
		max_level,
		rds,
		min_warn,
		max_warn,
		numFlags
	};
	
/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Create a new Attribute object.
 *
 * @param prop_list The attribute properties list. Each property is an object
 * of the AttrProperty class
 * @param tmp_attr Temporary attribute object built from user parameters
 * @param dev_name The device name
 * @param idx The index of the related Attr object in the MultiClassAttribute
 *            vector of Attr object
 */
	Attribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name,long idx);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class
 */
//@{
/**
 * The attribute desctructor.
 */
	virtual ~Attribute() {delete ext;}
//@}

/**@name Check attribute methods
 * Miscellaneous method returning boolean flag accorrding to attribute state
 */
//@{
/**
 * Check if the attribute has an associated writable attribute.
 *
 * This method returns a boolean set to true if the attribute has a writable
 * attribute associated to it.
 *
 * @return A boolean set to true if there is an associated writable attribute
 */
	bool is_writ_associated();
/**
 * Check if the attribute is in minimum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value below the min. alarm).
 */	
	bool is_min_alarm() {return alarm.test(min_level);}
/**
 * Check if the attribute is in maximum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value above the max. alarm).
 */
	bool is_max_alarm() {return alarm.test(max_level);}
/**
 * Check if the attribute is in minimum warning condition .
 *
 * @return A boolean set to true if the attribute is in warning condition (read
 * value below the min. warning).
 */	
	bool is_min_warning() {return alarm.test(min_warn);}
/**
 * Check if the attribute is in maximum warning condition .
 *
 * @return A boolean set to true if the attribute is in warning condition (read
 * value above the max. warning).
 */
	bool is_max_warning() {return alarm.test(max_warn);}
/**
 * Check if the attribute is in RDS alarm condition .
 *
 * @return A boolean set to true if the attribute is in RDS condition (Read
 * Different than Set).
 */
	bool is_rds_alarm() {return alarm.test(rds);}
/**
 * Check if the attribute has an alarm defined.
 *
 * This method returns a set of bits. Each alarm type is defined by one
 * bit.
 *
 * @return A bitset. Each bit is set if the coresponding alarm is on
 */
	bitset<numFlags> &is_alarmed() {return alarm_conf;}
/**
 * Check if the attribute read value is below/above the alarm level.
 *
 * @return A boolean set to true if the attribute is in alarm condition.
 * @exception DevFailed If no alarm level is defined.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */		
	bool check_alarm();
//@}
	
/**@name Get/Set object members.
 * These methods allows the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Get the attribute writable type (RO/WO/RW).
 *
 * @return The attribute write type.
 */	
	Tango::AttrWriteType get_writable() {return writable;}
/**
 * Get attribute name
 *
 * @return The attribute name
 */	
	string &get_name() {return name;}
/**
 * Get attribute data type
 *
 * @return The attribute data type
 */
	long get_data_type() {return data_type;}
/**
 * Get attribute data format
 *
 * @return The attribute data format
 */
	Tango::AttrDataFormat get_data_format() {return data_format;}
/**
 * Get name of the associated writable attribute
 *
 * @return The associated writable attribute name
 */
	string &get_assoc_name() {return writable_attr_name;}
/**
 * Get index of the associated writable attribute
 *
 * @return The index in the main attribute vector of the associated writable
 * attribute
 */
	long get_assoc_ind() {return assoc_ind;}
/**
 * Set index of the associated writable attribute
 *
 * @param The new index in the main attribute vector of the associated writable
 * attribute
 */
	void set_assoc_ind(long val) {assoc_ind = val;}
/**
 * Get attribute date
 *
 * @return The attribute date
 */
	Tango::TimeVal &get_date() {return when;}
/**
 * Set attribute date
 *
 * @param The attribute date
 */
	void set_date(Tango::TimeVal &new_date) {when = new_date;}
#ifdef WIN32
/**
 * Set attribute date
 *
 * @param The attribute date
 */
	void set_date(struct _timeb &t) {when.tv_sec=t.time;when.tv_usec=(t.millitm*1000);when.tv_nsec=0;}
#endif
/**
 * Set attribute date
 *
 * @param The attribute date
 */
	void set_date(struct timeval &t) {when.tv_sec=t.tv_sec;when.tv_usec=t.tv_usec;when.tv_nsec=0;}
/**
 * Set attribute date
 *
 * @param The attribute date
 */
	void set_date(time_t new_date) {when.tv_sec=new_date;when.tv_usec=0;when.tv_nsec=0;}
/**
 * Get attribute label property
 *
 * @return The attribute label
 */
 	string &get_label() {return label;}
/**
 * Get attribute data quality
 *
 * @return The attribute data quality
 */
	Tango::AttrQuality &get_quality() {return quality;}
/**
 * Set attribute data quality
 *
 * @param qua	The new attribute data quality
 */
	void set_quality(Tango::AttrQuality qua) {quality = qua;}
/**
 * Get attribute data size
 *
 * @return The attribute data size
 */
	long get_data_size() {return data_size;}
/**
 * Get attribute data size in x dimension
 *
 * @return The attribute data size in x dimension. Set to 1 for scalar attribute
 */
	long get_x() {return dim_x;}
/**
 * Get attribute maximum data size in x dimension
 *
 * @return The attribute maximum data size in x dimension. Set to 1 for scalar attribute
 */
	long get_max_dim_x() {return max_x;}
/**
 * Get attribute data size in y dimension
 *
 * @return The attribute data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_y() {return dim_y;}
/**
 * Get attribute maximum data size in y dimension
 *
 * @return The attribute maximum data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_max_dim_y() {return max_y;}
/**
 * Get attribute polling period
 *
 * @return The attribute polling period in mS. Set to 0 when the attribute is
 * not polled
 */
	long get_polling_period() {return ext->poll_period;}
/**
 * Get attribute properties.
 *
 * This method initialise the fields of a AttributeConfig object with the 
 * attribute properties value
 *
 * @param conf A AttributeConfig object.
 */	
	void get_properties(Tango::AttributeConfig &);
/**
 * Get attribute properties version 2.
 *
 * This method initialise the fields of a AttributeConfig_2 object with the 
 * attribute properties value
 *
 * @param conf A AttributeConfig_2 object.
 */	
	void get_properties_2(Tango::AttributeConfig_2 &);
/**
 * Get attribute properties version 3.
 *
 * This method initialise the fields of a AttributeConfig_3 object with the 
 * attribute properties value
 *
 * @param conf A AttributeConfig_3 object.
 */	
	void get_properties_3(Tango::AttributeConfig_3 &);
//@}


/**@name Set attribute value methods.
 * These methods allows the external world to set attribute object internal
 * value
 */
//@{
/**
 * Set internal attribute value (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void set_value(Tango::DevShort *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevLong *p_data,long x = 1, long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevFloat *p_data,long x = 1,long y = 0,bool release = false);

/**
 * Set internal attribute value (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevDouble *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevString *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevBoolean *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevUShort *p_data,long x = 1,long y = 0,bool release = false);
/**
 * Set internal attribute value (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value inside the object. This data will be
 * returned to the caller. This method also stores the date when it is called
 * and initialise the attribute quality factor.
 *
 * @param p_data The attribute read value
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value(Tango::DevUChar *p_data,long x = 1,long y = 0,bool release = false);

//---------------------------------------------------------------------------


/**
 * Set internal attribute value, date and quality factor (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void set_value_date_quality(Tango::DevShort *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);

#ifdef WIN32				    
	void set_value_date_quality(Tango::DevShort *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevShort *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void set_value_date_quality(Tango::DevLong *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevLong *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevLong attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevLong *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	void set_value_date_quality(Tango::DevFloat *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevFloat *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevFloat attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevFloat *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif


//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	void set_value_date_quality(Tango::DevDouble *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevDouble *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevDouble attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevDouble *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevString *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevString attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevString *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevBoolean attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevBoolean *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	void set_value_date_quality(Tango::DevUShort *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevUShort *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevUShort attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevUShort *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//-----------------------------------------------------------------------

/**
 * Set internal attribute value, date and quality factor (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	void set_value_date_quality(Tango::DevUChar *p_data,
				    time_t t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#ifdef WIN32				    
	void set_value_date_quality(Tango::DevUChar *p_data,
				    struct _timeb &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#else
/**
 * Set internal attribute value, date and quality factor (for Tango::DevUChar attribute data type).
 *
 * This method stores the attribute read value, the date and the attribute
 * quality factor inside the object. This data will be
 * returned to the caller.
 *
 * Please note that for Win32 user, the same method is defined using a 
 * "_timeb" structure instead of a "timeval" structure to set date. 
 *
 * @param p_data The attribute read value
 * @param t The date
 * @param qual The attribute quality factor
 * @param x The attribute x length. Default value is 1
 * @param y The attribute y length. Default value is 0
 * @param release The release flag. If true, memory pointed to by p_data will be 
 * 		  freed after being send to the client. Default value is false.
 * @exception DevFailed If the attribute data type is not coherent.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void set_value_date_quality(Tango::DevUChar *p_data,
				    struct timeval &t,
				    Tango::AttrQuality qual,
				    long x = 1,long y = 0,
				    bool release = false);
#endif

//@}

//
// methods not usable for the external world
//

	virtual void set_value() {};
	void delete_seq();
	void wanted_date(bool flag) {date = flag;}
	Tango::TimeVal &get_when() {return when;}
	void set_time();
		
	Tango::DevVarShortArray *get_short_value() {return value.sh_seq;}
	Tango::DevVarLongArray *get_long_value() {return value.lg_seq;}
	Tango::DevVarDoubleArray *get_double_value() {return value.db_seq;}
	Tango::DevVarStringArray *get_string_value() {return value.str_seq;}
	Tango::DevVarFloatArray *get_float_value() {return value.fl_seq;}
	Tango::DevVarBooleanArray *get_boolean_value() {return value.boo_seq;}
	Tango::DevVarUShortArray *get_ushort_value() {return value.ush_seq;}
	Tango::DevVarCharArray *get_uchar_value() {return value.cha_seq;}
	
	void add_write_value(Tango::DevVarShortArray *);
	void add_write_value(Tango::DevVarLongArray *);
	void add_write_value(Tango::DevVarDoubleArray *);
	void add_write_value(Tango::DevVarStringArray *);
	void add_write_value(Tango::DevVarFloatArray *);
	void add_write_value(Tango::DevVarBooleanArray *);
	void add_write_value(Tango::DevVarUShortArray *);
	void add_write_value(Tango::DevVarCharArray *);
	
	unsigned long get_name_size() {return name_size;}
	string &get_name_lower() {return name_lower;}
	void set_value_flag(bool val) {value_flag = val;}
	bool get_value_flag() {return value_flag;}
	
	void set_properties(const Tango::AttributeConfig &,string &);
	void set_properties(const Tango::AttributeConfig_3 &,string &);
	void upd_database(const Tango::AttributeConfig &,string &);
	void upd_database(const Tango::AttributeConfig_3 &,string &);
	
	bool change_event_subscribed() {if (ext->event_change_subscription != 0)return true;else return false;}
	bool periodic_event_subscribed() {if (ext->event_periodic_subscription != 0)return true;else return false;}
	bool archive_event_subscribed() {if (ext->event_archive_subscription != 0)return true;else return false;}
	bool quality_event_subscribed() {if (ext->event_quality_subscription != 0)return true;else return false;}
	bool user_event_subscribed() {if (ext->event_user_subscription != 0)return true;else return false;}

	void set_change_event_sub() {ext->event_change_subscription=time(NULL);}
	void set_periodic_event_sub() {ext->event_periodic_subscription=time(NULL);}
	void set_archive_event_sub() {ext->event_archive_subscription=time(NULL);}
	void set_quality_event_sub() {ext->event_quality_subscription=time(NULL);}
	void set_user_event_sub() {ext->event_user_subscription=time(NULL);}		

	long get_attr_idx() {return ext->idx_in_attr;}
		
#ifndef TANGO_HAS_LOG4TANGO							
	friend ostream &operator<<(ostream &,Attribute &);
#endif // TANGO_HAS_LOG4TANGO
	friend class EventSupplier;
	friend class EventSubscriptionChangeCmd;

private:
	void set_data_size();
	void throw_err_format(const char *,string &);
	void check_str_prop(const AttributeConfig &,DbData &,long &,DbData &,long &);	

	unsigned long 		name_size;
	string 			name_lower;
		
	AttributeExt		*ext;
			
protected:
	virtual void init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name);
	virtual void init_event_prop(vector<AttrProperty> &prop_list);
	string &get_attr_value(vector<AttrProperty> &prop_list,const char *name);
	long get_lg_attr_value(vector<AttrProperty> &prop_list,const char *name);
	virtual bool check_rds_alarm() {return false;}
	bool check_level_alarm();
	bool check_warn_alarm();

/**@name Class data members */
//@{
/**
 * A flag set to true if the attribute value has been updated
 */		
	bool 			value_flag;
/**
 * The date when attribute was read
 */
	Tango::TimeVal		when;
/**
 * Flag set to true if the date must be set
 */
	bool			date;
/**
 * The attribute quality factor
 */
	Tango::AttrQuality	quality;
	
/**
 * The attribute name
 */	
	string 			name;
/**
 * The attribute writable flag
 */
	Tango::AttrWriteType	writable;
/**
 * The attribute data type.
 *
 * Eight types are suported. They are Tango::DevShort, Tango::DevLong,
 * Tango::DevDouble, Tango::DevString, Tango::DevUShort, Tango::DevUChar,
 * Tango::DevFloat and Tango::DevBoolean
 */
	long			data_type;
/**
 * The attribute data format.
 *
 * Three data formats are supported. They are SCALAR, SPECTRUM and IMAGE
 */
	Tango::AttrDataFormat	data_format;
/**
 * The attribute maximum x dimension.
 *
 * It is needed for SPECTRUM or IMAGE data format
 */
	long			max_x;
/**
 * The attribute maximum y dimension.
 *
 * It is necessary only for IMAGE data format
 */
	long			max_y;
/**
 * The attribute label
 */	
	string			label;
/**
 * The attribute description
 */
	string			description;
/**
 * The attribute unit
 */
	string			unit;
/**
 * The attribute standard unit
 */
	string			standard_unit;
/**
 * The attribute display unit
 */
	string 			display_unit;
/**
 * The attribute format.
 *
 * This string specifies how an attribute value must be printed
 */
	string			format;
/**
 * The name of the associated writable attribute
 */
	string			writable_attr_name;
/**
 * The attribute minimum alarm level
 */
	string			min_alarm_str;
/**
 * The attribute maximun alarm level
 */
	string			max_alarm_str;
/**
 * The attribute minimum value
 */
	string			min_value_str;
/**
 * The attribute maximum value
 */
	string			max_value_str;
/**
 * The attribute minimun  warning
 */
	string			min_warning_str;
/**
 * The attribute maximum warning
 */
	string			max_warning_str;
/**
 * The attribute delta value RDS alarm
 */
	string			delta_val_str;
/**
 * The attribute delta time RDS alarm
 */
	string			delta_t_str;
/**
 * Index in the main attribute vector of the associated writable attribute (if any)
 */
	long			assoc_ind;
/**
 * The attribute minimum alarm in binary format
 */				
	Tango::Attr_CheckVal	min_alarm;
/**
 * The attribute maximum alarm in binary format
 */	
	Tango::Attr_CheckVal	max_alarm;
/**
 * The attribute minimum warning in binary format
 */				
	Tango::Attr_CheckVal	min_warning;
/**
 * The attribute maximum warning in binary format
 */	
	Tango::Attr_CheckVal	max_warning;
/**
 * The attribute minimum value in binary format
 */	
	Tango::Attr_CheckVal	min_value;
/**
 * The attribute maximum value in binary format
 */	
	Tango::Attr_CheckVal	max_value;
/**
 * The attribute value
 */	
	Tango::Attr_Value	value;
/**
 * The attribute data size
 */
	long			data_size;
/**
 * Flag set to true if a minimum value is defined
 */
	bool			check_min_value;
/**
 * Flag set to true if a maximum alarm is defined
 */
	bool			check_max_value;
/**
 * Authorized delta between the last written value and the
 * actual read. Used if the attribute has an alarm on
 * Read Different Than Set (RDS)
 */
	Tango::Attr_CheckVal	delta_val;
/**
 * Delta time after which the read value must be checked again the
 * last written value if the attribute has an alarm on
 * Read Different Than Set (RDS)
 */
 	long 			delta_t;
//@}

	bitset<numFlags>	alarm_conf;
	bitset<numFlags>	alarm;	

	long			dim_x;
	long			dim_y;
	
	Tango::DevShort		tmp_sh[2];
	Tango::DevLong		tmp_lo[2];
	Tango::DevFloat		tmp_fl[2];
	Tango::DevDouble	tmp_db[2];
	Tango::DevString	tmp_str[2];
	Tango::DevUShort	tmp_ush[2];
	Tango::DevBoolean	tmp_boo[2];
	Tango::DevUChar		tmp_cha[2];		
	vector<AttrProperty>::iterator pos_end;	
};

//
// Macro to help coding
//

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
#define MEM_STREAM_2_CORBA(A,B) NEW_MEM_STREAM_2_CORBA(A,B)
#else
#define MEM_STREAM_2_CORBA(A,B) OLD_MEM_STREAM_2_CORBA(A,B)
#endif

#define NEW_MEM_STREAM_2_CORBA(A,B) \
	string s = B.str(); \
	A = CORBA::string_dup(s.c_str()); \
	B.str("");
	
#define OLD_MEM_STREAM_2_CORBA(A,B) \
	char *tmp_str = B.str(); \
	A = CORBA::string_dup(tmp_str); \
	delete[]tmp_str;
	
//
// Define one macro to make code more readable
// Arg list : 	A : property as a string
//		B : stream
//		C : device name
//		D : DbDatum for db update
//		E : DbDatum for db delete
//		F : Number of prop to update
//		G : Number of prop to delete
//		H : Property name
//
// Too many parameters ?
//

#define CHECK_PROP(A,B,C,D,E,F,G,H) \
	if ((strcmp(A,AlrmValueNotSpec) != 0) && \
	    (strcmp(A,NotANumber) != 0)) \
	{ \
		if ((data_type != Tango::DEV_STRING) && \
		    (data_type != Tango::DEV_BOOLEAN)) \
		{ \
			short sh; \
			long lg; \
			double db; \
			float fl; \
			unsigned short ush; \
			unsigned char uch; \
\
			B.seekp(0); \
			B.seekg(0); \
			B.clear(); \
			B << A << ends; \
			switch (data_type) \
			{ \
			case Tango::DEV_SHORT: \
				if (!(B >> sh)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_LONG: \
				if (!(B >> lg)) \
					throw_err_format(H,C); \
				break;\
\
			case Tango::DEV_DOUBLE: \
				if (!(B >> db)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_FLOAT: \
				if (!(B >> fl)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_USHORT: \
				if (!(B >> ush)) \
					throw_err_format(H,C); \
				break; \
\
			case Tango::DEV_UCHAR: \
				if (!(B >> uch)) \
					throw_err_format(H,C); \
				break; \
			} \
		} \
		DbDatum max_val(H); \
		const char *tmp = A.in(); \
		max_val << tmp; \
		D.push_back(max_val); \
		F++; \
	} \
\
	if (strcmp(A,NotANumber) == 0) \
	{ \
		DbDatum max_val(H); \
		E.push_back(max_val); \
		G++; \
	} 


//
// Define another macro to make code more readable !!
// Arg list : 	A : property as a string
//		B : stream
//		C : device name
//		D : DbDatum for db update
//		E : DbDatum for db delete
//		F : Number of prop to update
//		G : Number of prop to delete
//		H : Property name
//
// Too many parameters ?
//
	
#define CHECK_CH_PROP(A,B,C,D,E,F,G,H) \
	if ((strcmp(A,AlrmValueNotSpec) != 0) && \
	    (strcmp(A,NotANumber) != 0)) \
	{ \
		B.seekp(0); \
		B.seekg(0); \
		B.clear(); \
		string st(A); \
		string::size_type pos = st.find(','); \
		if (pos != string::npos) \
			replace(st.begin(),st.end(),',',' '); \
		B << st << ends; \
		double db1,db2; \
		if (!(B >> db1)) \
			throw_err_format(H,C); \
		if (pos != string::npos) \
		{ \
			if (!(B >> db2)) \
				throw_err_format(H,C); \
		}\
		else \
			db2 = db1; \
		db1 = fabs(db1); \
		db2 = fabs(db2); \
		DbDatum max_val(H); \
		if (db1 == db2) \
			max_val << db1; \
		else \
		{ \
			vector<double> vd(2); \
			vd[0] = db1; \
			vd[1] = db2; \
			max_val << vd; \
		} \
		D.push_back(max_val); \
		F++; \
	} \
\
	if (strcmp(A,NotANumber) == 0) \
	{ \
		DbDatum max_val(H); \
		E.push_back(max_val); \
		G++; \
	} 	


//
// Oh, a new macro !!
// Arg list : 	A : property as a string
//		B : stream
//		C : storage place
//
	
#define SET_EV_PROP(A,B,C) \
	if (strcmp(A,NotANumber) == 0)\
	{ \
		ext->C[0] = INT_MAX; \
		ext->C[1] = INT_MAX; \
	} \
	else \
	{ \
		if (strcmp(A,AlrmValueNotSpec) != 0) \
		{ \
			double rel_change_min=INT_MAX, rel_change_max=INT_MAX; \
 			B.seekp(0); \
			B.seekg(0); \
			B.clear(); \
			string st(A); \
			string::size_type pos = st.find(','); \
			if (pos != string::npos) \
				replace(st.begin(),st.end(),',',' '); \
			B << st << ends; \
			B >> rel_change_min; \
			if (pos != string::npos) \
				B >> rel_change_max; \
        		if (fabs(rel_change_min) > 0 && rel_change_min != INT_MAX) \
			{ \
				ext->C[0] = -fabs(rel_change_min); \
				ext->C[1] = fabs(rel_change_min); \
        		} \
        		if (rel_change_max > 0 && rel_change_max != INT_MAX) \
			{ \
				ext->C[1] = fabs(rel_change_max); \
        		} \
		} \
	} 


} // End of Tango namespace

#endif // _ATTRIBUTE_H

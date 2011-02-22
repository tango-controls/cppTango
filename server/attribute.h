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
// Revision 1.6  2001/10/08 09:03:10  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:09  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:12  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:44  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:14  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
//
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
		return attr_ptr->get_name() == st;
	}
};



class AttrProperty;

typedef union _Attr_CheckVal
{
	short	sh;
	long	lg;
	double	db;
}Attr_CheckVal;

typedef union _Attr_Value
{
	DevVarShortArray  *sh_seq;
	DevVarLongArray   *lg_seq;
	DevVarDoubleArray *db_seq;
	DevVarStringArray *str_seq;
}Attr_Value;

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
	
 	Tango::DispLevel	disp_level;		// Display level
 	long			poll_period;		// Polling period
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
 */
	Attribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name);
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
	bool is_min_alarm() {return min_alarm_on;}
/**
 * Check if the attribute is in maximum alarm condition .
 *
 * @return A boolean set to true if the attribute is in alarm condition (read
 * value above the max. alarm).
 */
	bool is_max_alarm() {return max_alarm_on;}
/**
 * Check if the attribute has an alarm value defined.
 *
 * This method returns a boolean set to true if the attribute has an alarm
 * value defined.
 *
 * @return A boolean set to true if there is an alarm defined.
 */
	bool is_alarmed();
/**
 * Check if the attribute read value is below/above the larm level.
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
	Tango::TimeVal &get_when() {return when;}
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
	void set_quality(Tango::AttrQuality val) {quality = val;}
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
 * Get attribute data size in y dimension
 *
 * @return The attribute data size in y dimension. Set to 0 for scalar and
 * spectrum attribute
 */
	long get_y() {return dim_y;}
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
//@}

//
// methods not usable for the external world
//

	virtual void set_value() {};
	void delete_seq();
	void wanted_date(bool flag) {date = flag;}
		
	Tango::DevVarShortArray *get_short_value() {return value.sh_seq;}
	Tango::DevVarLongArray *get_long_value() {return value.lg_seq;}
	Tango::DevVarDoubleArray *get_double_value() {return value.db_seq;}
	Tango::DevVarStringArray *get_string_value() {return value.str_seq;}
	
	void add_write_value(Tango::DevShort &);
	void add_write_value(Tango::DevLong &);
	void add_write_value(Tango::DevDouble &);
	void add_write_value(Tango::DevString &);

	void set_value_flag(bool val) {value_flag = val;}
	bool get_value_flag() {return value_flag;}
	
	void set_properties(const Tango::AttributeConfig &,string &);
	void upd_database(const Tango::AttributeConfig &,string &);
							
	friend ostream &operator<<(ostream &,Attribute &);

private:
	void set_time();
	void set_data_size();
	void throw_err_format(const char *,string &);
		
	AttributeExt		*ext;
			
protected:
	virtual void init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name);
	string &get_attr_value(vector<AttrProperty> &prop_list,const char *name);

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
 * Only four types are suported. They are Tango::DevShort, Tango::DevLong,
 * Tango::DevDouble and Tango::DevString
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
 * Index in the main attribute vector of the associated writable attribute (if any)
 */
	long			assoc_ind;
/**
 * The attribute minimum alarm in binary format
 */				
	Tango::Attr_CheckVal	min_alarm;
/**
 * The attribute miaaximum alarm in binary format
 */	
	Tango::Attr_CheckVal	max_alarm;
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
 * Flag set to true if a minimum alarm is defined
 */
	bool			check_min_alarm;
/**
 * Flag set to true if a maximum alarm is defined
 */
	bool			check_max_alarm;
/**
 * Flag set to true if a minimum value is defined
 */
	bool			check_min_value;
/**
 * Flag set to true if a maximum alarm is defined
 */
	bool			check_max_value;
/**
 * Flag set to true if the read value is below the alarm level
 */
	bool			min_alarm_on;
/**
 * Flag set to true if the read value is above the alarm level
 */
	bool			max_alarm_on;
//@}


	long			dim_x;
	long			dim_y;
	
	Tango::DevShort		tmp_sh[2];
	Tango::DevLong		tmp_lo[2];
	Tango::DevDouble	tmp_db[2];
	Tango::DevString	tmp_str[2];		
	vector<AttrProperty>::iterator pos_end;	
};

//=============================================================================
//
//			The WAttribute class
//
//
// description :	This class inherits the Attribute class. There is one
//			instance of this class for each writable attribute
//
//=============================================================================

class WAttributeExt
{
};

/**
 * This class represents a writable attribute. It inherits from the Attribute
 * class and only add what is specific to writable attribute.
 *
 * $Author$
 * $Revision$
 */
 
class WAttribute:public Attribute
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Create a new Writable Attribute object.
 *
 * @param prop_list The attribute properties list. Each property is an object
 * of the AttrProperty class
 * @param tmp_attr The temporary attribute object built from user parameters
 * @param dev_name The device name
 */
	WAttribute(vector<AttrProperty> &prop_list,Attr &tmp_attr,string &dev_name);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class
 */
//@{
/**
 * The WAttribute desctructor.
 */
	~WAttribute() {delete ext;}
//@}

/**@name Get new value for attribute
 * Miscellaneous method to retieve from the WAttribute object the new value
 * for the attribute.
 */
 //@{
/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevShort.
 *
 * @param val A reference to a Tango::DevShort data which will be initialised
 * with the new value
 */	
	void get_write_value(Tango::DevShort &val) {val = short_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevLong.
 *
 * @param val A reference to a Tango::DevLong data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevLong &val) {val = long_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevDouble.
 *
 * @param val A reference to a Tango::DevDouble data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevDouble &val) {val = double_val;}

/**
 * Retrieve the new value for writable attribute when attribute data type is 
 * Tango::DevString.
 *
 * @param val A reference to a Tango::DevString data which will be initialised
 * with the new value
 */
	void get_write_value(Tango::DevString &val) {val = str_val;}
//@}
	
	virtual void set_value();
	void rollback();
	
	void set_write_value(const CORBA::Any &);

private:
	Tango::DevShort 	short_val;
	Tango::DevShort		old_short_val;
	
	Tango::DevLong		long_val;
	Tango::DevLong		old_long_val;
	
	Tango::DevDouble	double_val;
	Tango::DevDouble	old_double_val;
	
	Tango::DevString	str_val;
	Tango::DevString	old_str_val;
	
	bool			string_allocated;
	WAttributeExt		*ext;
};

} // End of Tango namespace

#endif // _ATTRIBUTE_H

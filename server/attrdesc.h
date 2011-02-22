//=============================================================================	
//
// file :		attrdesc.h
//
// description :	Include file for the Attr classes hierarchy. 
//			Three classes are declared in this file :
//				The Attr class
//				The SpectrumAttr class
//				The ImageAttr class
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
// Revision 1.3  2001/03/30 08:03:43  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:13  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//=============================================================================

#ifndef _ATTRDESC_H
#define _ATTRDESC_H

#include <tango.h>

namespace Tango
{

class AttrProperty;

class AttrExt
{
public:
	AttrExt():poll_period(0) {disp_level = Tango::OPERATOR;}
	AttrExt(DispLevel level) {disp_level = level;}
	
	Tango::DispLevel	disp_level;		// Display  level
	long			poll_period;		// Polling period
};

class UserDefaultAttrPropExt
{
};

/**
 * User class to set attribute default properties.
 *
 * This class is used to set attribute default properties. Three levels of 
 * attributes properties setting are implemented within Tango. The highest
 * property setting priority is the database. Then the user default (set using
 * this UserDefaultAttrProp class) and finally a Tango library default
 * value
 *
 * $Author$
 * $Revision$
 */
 
class UserDefaultAttrProp
{
public:

/**@name Constructor
 * Only one constructor is defined for this class */
//@{
/**
 * Constructs a newly allocated UserDefaultAttrProp object.
 */
	UserDefaultAttrProp() {ext = NULL;}
//@}
	
	~UserDefaultAttrProp() {delete ext;}

/**@name Set default property methods */
//@{
/**
 * Set default label property
 *
 * @param	def_label	The user default label property
 */
	void set_label(const char *def_label)
	{
		label = def_label;
	}
	
/**
 * Set default description property
 *
 * @param	def_desc	The user default description property
 */	
	void set_description(const char *def_desc)
	{
		description = def_desc;
	}
	
/**
 * Set default unit property
 *
 * @param	def_unit	The user default unit property
 */
	void set_unit(const char *def_unit)
	{
		unit = def_unit;
	}
	
/**
 * Set default standard unit property
 *
 * @param	def_std_unit	The user default standard unit property
 */
	void set_standard_unit(const char *def_std_unit)
	{
		standard_unit = def_std_unit;
	}
	
/**
 * Set default display unit property
 *
 * @param	def_disp_unit	The user default display unit property
 */
	void set_display_unit(const char *def_disp_unit)
	{
		display_unit = def_disp_unit;
	}
	
/**
 * Set default format property
 *
 * @param	def_format	The user default format property
 */
	void set_format(const char *def_format)
	{
		format = def_format;
	}
	
/**
 * Set default min_value property
 *
 * @param	def_min_value	The user default min_value property
 */
	void set_min_value(const char *def_min_value)
	{
		min_value = def_min_value;
	}
	
/**
 * Set default max_value property
 *
 * @param	def_max_value	The user default max_value property
 */
	void set_max_value(const char *def_max_value)
	{
		max_value = def_max_value;
	}
	
/**
 * Set default min_alarm property
 *
 * @param	def_min_alarm	The user default min_alarm property
 */
	void set_min_alarm(const char *def_min_alarm)
	{
		min_alarm = def_min_alarm;
	}
	
/**
 * Set default max_alarm property
 *
 * @param	def_max_alarm	The user default max_alarm property
 */
	void set_max_alarm(const char *def_max_alarm)
	{
		max_alarm = def_max_alarm;
	}
//@}
					
	string 			label;
	string 			description;
	string 			unit;
	string 			standard_unit;
	string 			display_unit;
	string 			format;
	string 			min_value;
	string			max_value;
	string			min_alarm;
	string			max_alarm;
	
	UserDefaultAttrPropExt	*ext;	
};

/**
 * User class to create a no dimension attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 * $Author$
 * $Revision$
 */
 
class Attr
{
public:

/**@name Constructors
 * Two constructor are defined for this class */
//@{
/**
 * Constructs a newly allocated Attr object.
 * The attribute display level is set to OPERATOR.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	w_type	The attribute type (read, write, read with write ...)
 * @param	assoc	Name of the associated writable attribute. This is used
 * only the read with write attribute
 *
 */
	Attr(const char *name,long data_type,
	     Tango::AttrWriteType w_type = Tango::READ,
	     const char *assoc = AssocWritNotSpec);

/**
 * Constructs a newly allocated Attr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	disp	The attribute display level
 * @param	w_type	The attribute type (read, write, read with write ...)
 * @param	assoc	Name of the associated writable attribute. This is used
 * only the read with write attribute
 *
 */	     
	Attr(const char *name,long data_type,Tango::DispLevel disp,
	     Tango::AttrWriteType w_type = Tango::READ,
	     const char *assoc = AssocWritNotSpec);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
	virtual ~Attr();
//@}

/**@name Miscellaneous methods*/
//@{
/**
 * Set default attribute properties
 *
 * @param	prop	The user default property class
 */
 	void set_default_properties(UserDefaultAttrProp &prop);
/**
 * Set the attribute display level
 *
 * @param	level	The attribute display level
 */
 	void set_disp_level(Tango::DispLevel level) {ext->disp_level = level;}
/**
 * Set the attribute polling update period
 *
 * @param	update	The attribute polling period (in mS)
 */
 	void set_polling_period(long update) {ext->poll_period = update;}
//@}

	string 			&get_name() {return name;}
	Tango::AttrDataFormat 	get_format() {return format;}
	Tango::AttrWriteType 	get_writable() {return writable;}
	long 			get_type() {return type;}
	Tango::DispLevel	get_disp_level() {return ext->disp_level;}
	long			get_polling_period() {return ext->poll_period;}
	string			&get_assoc() {return assoc_name;}
	bool			is_assoc()
				{
					if (assoc_name != AssocWritNotSpec)
						return true;
					else
						return false;
				}
	
	vector<AttrProperty>	&get_class_properties() {return class_properties;}
	vector<AttrProperty>	&get_user_default_properties() {return user_default_properties;}
	void 			set_class_properties(vector<AttrProperty> &in_prop)
				{
					class_properties = in_prop;
				}
	void			check_type();
	friend ostream &operator<<(ostream &,const Attr &);
		
protected:
	string			name;
	Tango::AttrDataFormat	format;
	Tango::AttrWriteType	writable;
	long			type;
	string			assoc_name;
	
	vector<AttrProperty>	class_properties;
	vector<AttrProperty>	user_default_properties;

private:	
	AttrExt			*ext;
};

class SpectrumAttrExt
{
};

/**
 * User class to create a one dimension attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 * $Author$
 * $Revision$
 */
 
class SpectrumAttr: public Attr
{
public:

/**@name Constructors
 * Two constructors are defined for this class */
//@{
/**
 * Constructs a newly allocated SpectrumAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 *
 */
	SpectrumAttr(const char *name,long data_type,long max_x);

/**
 * Constructs a newly allocated SpectrumAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	disp 	The attribuute display type
 *
 */
	SpectrumAttr(const char *name,long data_type,long max_x,DispLevel level);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
	~SpectrumAttr() {delete ext;}
//@}
	
	long 			get_max_x() {return max_x;}
	
protected:
	long			max_x;
	
private:
	SpectrumAttrExt		*ext;
};

class ImageAttrExt
{
};

/**
 * User class to create a two dimensions attribute object.
 *
 * Information from this class and information fetched out from the Tango
 * database allows the Tango core software to create the Attribute object
 * for the attribute created by the user.
 *
 * $Author$
 * $Revision$
 */
 
class ImageAttr: public SpectrumAttr
{
public:

/**@name Constructors
 * Two constructors are defined for this class */
//@{
/**
 * Constructs a newly allocated ImageAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 *
 */
	ImageAttr(const char *name,long data_type,long max_x, long max_y);
	
/**
 * Constructs a newly allocated ImageAttr object.
 *
 * @param 	name	The attribute name
 * @param	data_type	The attribute data type
 * @param	max_x	The attribute maximum x dimension
 * @param	max_y	The attribute maximum y dimension
 * @param	disp	The attribute display type
 *
 */
	ImageAttr(const char *name,long data_type,long max_x,
		  long max_y, Tango::DispLevel level);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */
	~ImageAttr() {delete ext;}
//@}
	
	long 			get_max_y() {return max_y;}
	
protected:
	long			max_y;
	
private:
	ImageAttrExt		*ext;
};

} // End of Tango namespace

#endif /* _ATTRDESC_H */

static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               Attribute.cpp
//
// description :        C++ source code for the Attribute and WAttribute classes.
//			These classes are used to manage attribute.
//			A Tango Device object instance has one
//			MultiAttribute object which is an aggregate of
//			Attribute or WAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
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
// Revision 3.0  2003/03/25 16:41:51  taurel
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
// Revision 2.11  2003/03/11 17:55:46  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:38  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:15  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
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
// Revision 1.2  2001/03/09 08:20:13  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
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
#include <attribute.h>
#include <classattribute.h>

#include <functional>
#include <algorithm>

#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */

namespace Tango
{

//
// A classical function which will be used as unary predicate for the find_if
// algo. It must be used with the bind2nd adapter to be transform as unary and
// with the ptr_fun adapter to be transform as a function object
//

static bool WantedProp_f(AttrProperty a,const char *n)
{
	return (a.get_name() == n);
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::Attribute
// 
// description : 	constructor for the Attribute class from the 
//			attribute property vector, its type and the device
//			name
//
// argument : in : 	- prop_list : The attribute property list
//			- type : The attrubute data type
//			- dev_name : The device name
//
//--------------------------------------------------------------------------

Attribute::Attribute(vector<AttrProperty> &prop_list,
		     Attr &tmp_attr,string &dev_name)
:date(true),check_min_alarm(false),check_max_alarm(false),
 check_min_value(false),check_max_value(false),
 min_alarm_on(false),max_alarm_on(false)
{

//
// Create the extension class
//

	ext = new AttributeExt();
		
//
// Init the attribute name
//

	name = tmp_attr.get_name();
	name_size = name.size();
	name_lower = name;
	transform(name_lower.begin(),name_lower.end(),name_lower.begin(),::tolower);

//
// Init the remaining attribute main characteristic
//

	data_type = tmp_attr.get_type();
	writable = tmp_attr.get_writable();
	data_format = tmp_attr.get_format();
	ext->disp_level = tmp_attr.get_disp_level();
	ext->poll_period = tmp_attr.get_polling_period();
	writable_attr_name = tmp_attr.get_assoc();

	switch(data_format)
	{
	case Tango::SPECTRUM:
		max_x = dynamic_cast<SpectrumAttr &>(tmp_attr).get_max_x();
		max_y = 0;
		dim_y = 0;
		break;
				
	case Tango::IMAGE:
		max_x = dynamic_cast<ImageAttr &>(tmp_attr).get_max_x();
		max_y = dynamic_cast<ImageAttr &>(tmp_attr).get_max_y();
		break;
				
	default :
		max_x = 1;
		max_y = 0;
		dim_x = 1;
		dim_y = 0;
	}
	
//
// Initialise optional properties
//

	init_opt_prop(prop_list,dev_name);
	
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::init_opt_prop
// 
// description : 	Init the optional properties
//
// in :			prop_list : The property vector
//			dev_name : The device name (usefull for error)
//
//--------------------------------------------------------------------------

void Attribute::init_opt_prop(vector<AttrProperty> &prop_list,string &dev_name)
{
	
//
// Init the label property
//

	label = get_attr_value(prop_list,"label");
	
//
// Init the description property
//

	description = get_attr_value(prop_list,"description");
	
//
// Init the unit property
//

	unit = get_attr_value(prop_list,"unit");
	
//
// Init the standard unit property
//

	standard_unit = get_attr_value(prop_list,"standard_unit");
	
//
// Init the display unit property
//
	
	display_unit = get_attr_value(prop_list,"display_unit");
	
//
// Init the format property
//

	format = get_attr_value(prop_list,"format");
	
	
	TangoSys_MemStream str;
	bool empty = true;
	
//
// Init the min alarm property
//

	min_alarm_str = get_attr_value(prop_list,"min_alarm");
	if ((min_alarm_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
	{
		str << min_alarm_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> min_alarm.sh))
				throw_err_format("min_alarm",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> min_alarm.lg))
				throw_err_format("min_alarm",dev_name);
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> min_alarm.db))
				throw_err_format("min_alarm",dev_name);
			break;
		}
		check_min_alarm = true;
	}
	
//
// Init the max alarm property
//

	max_alarm_str = get_attr_value(prop_list,"max_alarm");
	if ((max_alarm_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << max_alarm_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> max_alarm.sh))
				throw_err_format("max_alarm",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> max_alarm.lg))
				throw_err_format("max_alarm",dev_name);
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> max_alarm.db))
				throw_err_format("max_alarm",dev_name);
			break;
		}
		check_max_alarm = true;
	}
		
//
// Init the min value property
//

	min_value_str = get_attr_value(prop_list,"min_value");
	if ((min_value_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << min_value_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> min_value.sh))
				throw_err_format("min_value",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> min_value.lg))
				throw_err_format("min_value",dev_name);
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> min_value.db))
				throw_err_format("min_value",dev_name);
			break;
		}
		check_min_value = true;
	}
		
//
// Init the max value property
//

	max_value_str = get_attr_value(prop_list,"max_value");
	if ((max_value_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
	{
		if (empty == false)
		{
			str.seekp(0);
			str.seekg(0);
		}

		str << max_value_str << ends;
		empty = false;
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			if (!(str >> max_value.sh))
				throw_err_format("max_value",dev_name);
			break;
			
		case Tango::DEV_LONG:
			if (!(str >> max_value.lg))
				throw_err_format("max_value",dev_name);
			break;
			
		case Tango::DEV_DOUBLE:
			if (!(str >> max_value.db))
				throw_err_format("max_value",dev_name);
			break;
		}
		check_max_value = true;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::throw_err_format
// 
// description : 	Throw a Tango DevFailed exception when an error format
//			is detected in the string which should be converted
//			to a number
//
// in :			prop_name : The property name
//			dev_name : The device name
//
//--------------------------------------------------------------------------

void Attribute::throw_err_format(const char *prop_name,string &dev_name)
{
	TangoSys_OMemStream o;
			
	o << "Device " << dev_name << "-> Attribute : " << name;
	o << "\nThe property " << prop_name << " is defined in a unsupported format" << ends;
	Except::throw_exception((const char *)"API_AttrOptProp",
			      o.str(),
			      (const char *)"Attribute::throw_err_format");
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::is_writ_associated
// 
// description : 	Check if the attribute has an associated writable
//			attribute
//
// This method returns a boolean set to true if the atribute has an associatied
// writable attribute
//
//--------------------------------------------------------------------------

bool Attribute::is_writ_associated()
{
	if (writable_attr_name != AssocWritNotSpec)
		return true;
	else
		return false;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_attr_value
// 
// description : 	Retrieve a property value as a string from the vector
//			of properties
//
// in :			prop_list : The property vector
//			prop_name : the property name
//
//--------------------------------------------------------------------------


string &Attribute::get_attr_value(vector <AttrProperty> &prop_list,const char *prop_name)
{
	vector<AttrProperty>::iterator pos;

//
// Get the property value
//

	pos = find_if(prop_list.begin(),prop_list.end(),bind2nd(ptr_fun(WantedProp_f),prop_name));
	if (pos == prop_list.end())
	{
		TangoSys_OMemStream o;
		
		o << "Property " << prop_name << " is missing for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::get_attr_value");
	}

	return pos->get_value();
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::is_alarmed
// 
// description : 	Check if the attribute has an an alarm value set
//
// This method returns a boolean set to true if the atribute has an alarm
// value defined
//
//--------------------------------------------------------------------------

bool Attribute::is_alarmed()
{
	if ((check_min_alarm == true) || 
	    (check_max_alarm == true))
		return true;
	else
		return false;
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::get_properties
// 
// description : 	Init the Tango::AttributeConfig with all the attribute
//			properties value
//
//--------------------------------------------------------------------------

void Attribute::get_properties(Tango::AttributeConfig &conf)
{

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = CORBA::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = CORBA::string_dup(label.c_str());
	conf.description = CORBA::string_dup(description.c_str());
	conf.unit = CORBA::string_dup(unit.c_str());
	conf.standard_unit = CORBA::string_dup(standard_unit.c_str());
	conf.display_unit = CORBA::string_dup(display_unit.c_str());
	conf.format = CORBA::string_dup(format.c_str());
	conf.writable_attr_name = CORBA::string_dup(writable_attr_name.c_str());
	conf.min_alarm = CORBA::string_dup(min_alarm_str.c_str());
	conf.max_alarm = CORBA::string_dup(max_alarm_str.c_str());
	conf.min_value = CORBA::string_dup(min_value_str.c_str());
	conf.max_value = CORBA::string_dup(max_value_str.c_str());
}

void Attribute::get_properties_2(Tango::AttributeConfig_2 &conf)
{

//
// Copy mandatory properties
//

	conf.writable = writable;
	conf.data_format = data_format;
	conf.max_dim_x = max_x;
	conf.max_dim_y = max_y;
	conf.data_type = data_type;
	conf.name = CORBA::string_dup(name.c_str());

//
// Copy optional properties
//

	conf.label = CORBA::string_dup(label.c_str());
	conf.description = CORBA::string_dup(description.c_str());
	conf.unit = CORBA::string_dup(unit.c_str());
	conf.standard_unit = CORBA::string_dup(standard_unit.c_str());
	conf.display_unit = CORBA::string_dup(display_unit.c_str());
	conf.format = CORBA::string_dup(format.c_str());
	conf.writable_attr_name = CORBA::string_dup(writable_attr_name.c_str());
	conf.min_alarm = CORBA::string_dup(min_alarm_str.c_str());
	conf.max_alarm = CORBA::string_dup(max_alarm_str.c_str());
	conf.min_value = CORBA::string_dup(min_value_str.c_str());
	conf.max_value = CORBA::string_dup(max_value_str.c_str());
	
	conf.level = ext->disp_level;
}
//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_properties
// 
// description : 	Init the Tango::AttributeConfig with all the attribute
//			properties value
//
//--------------------------------------------------------------------------

void Attribute::set_properties(const Tango::AttributeConfig &conf,string &dev_name)
{

//
// Copy only a sub-set of the new properties
//

	description = conf.description;
	label = conf.label;
	unit = conf.unit;
	standard_unit = conf.standard_unit;
	display_unit = conf.display_unit;
	format = conf.format;

//
// For the last four properties, convert their value to the right type
//

	TangoSys_MemStream str;
	min_value_str = conf.min_value;
	if (min_value_str == NotANumber)
	{
		min_value_str = AlrmValueNotSpec;
		check_min_value = false;
	}
	else
	{
		if ((min_value_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
		{
			str << min_value_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_value.sh))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> min_value.lg))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> min_value.db))
					throw_err_format("min_value",dev_name);
				break;
			}
			check_min_value = true;
		}
		else
			check_min_value = false;
	}

	max_value_str = conf.max_value;
	if (max_value_str == NotANumber)
	{
		max_value_str = AlrmValueNotSpec;
		check_max_value = false;
	}
	else
	{	
		if ((max_value_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
		{
			str.seekp(0);
			str.seekg(0);

			str << max_value_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_value.sh))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> max_value.lg))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> max_value.db))
					throw_err_format("max_value",dev_name);
				break;
			}
			check_max_value = true;
		}
		else
			check_max_value = false;
	}

	min_alarm_str = conf.min_alarm;
	if (min_alarm_str == NotANumber)
	{
		min_alarm_str = AlrmValueNotSpec;
		check_min_alarm = false;
	}
	else
	{		
		if ((min_alarm_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
		{
			str.seekp(0);
			str.seekg(0);

			str << min_alarm_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> min_alarm.sh))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> min_alarm.lg))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> min_alarm.db))
					throw_err_format("min_alarm",dev_name);
				break;
			}
			check_min_alarm = true;
		}
		else
			check_min_alarm = false;
	}

	max_alarm_str = conf.max_alarm;
	if (max_alarm_str == NotANumber)
	{
		max_alarm_str = AlrmValueNotSpec;
		check_max_alarm = false;
	}
	else
	{		
		if ((max_alarm_str != AlrmValueNotSpec) && (data_type != Tango::DEV_STRING))
		{
			str.seekp(0);
			str.seekg(0);

			str << max_alarm_str << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> max_alarm.sh))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> max_alarm.lg))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> max_alarm.db))
					throw_err_format("max_alarm",dev_name);
				break;
			}
			check_max_alarm = true;
		}
		else
			check_max_alarm = false;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::upd_database
// 
// description : 	Update the tango database with the new attribute
//			values
//
//--------------------------------------------------------------------------

void Attribute::upd_database(const Tango::AttributeConfig &conf,string &dev_name)
{
	cout4 << "Entering upd_database method for attribute " << name << endl;

//
// Build the array of strings sent to database
//

	Tango::DevVarStringArray *db_data = new Tango::DevVarStringArray(24);
	db_data->length(4);
	
	(*db_data)[0] = CORBA::string_dup(dev_name.c_str());
	(*db_data)[1] = CORBA::string_dup("1");
	(*db_data)[2] = CORBA::string_dup(name.c_str());
	(*db_data)[3] = CORBA::string_dup("10");
	
	long len = 4;
	long ind = 4;
	long prop_to_update = 0;
	
	long prop_to_delete = 0;
	Tango::DevVarStringArray *db_del_data;
	long len_del = 3;

//
// Check if each attribute property is not the default value. Only properties
// not having the default value will be sent to the database
//
	
	if (strcmp(conf.description,DescNotSpec) != 0)
	{
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("description");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.description);
		prop_to_update++;
		ind = ind + 2;
	}
	
	if (strcmp(conf.label,LabelNotSpec) != 0)
	{
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("label");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.label);
		prop_to_update++;
		ind = ind + 2;
	}
	
	if (strcmp(conf.unit,UnitNotSpec) != 0)
	{
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("unit");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.unit);
		prop_to_update++;
		ind = ind + 2;
	}
	
	if (strcmp(conf.standard_unit,StdUnitNotSpec) != 0)
	{
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("standard_unit");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.standard_unit);
		prop_to_update++;
		ind = ind + 2;
	}

	if (strcmp(conf.display_unit,DispUnitNotSpec) != 0)
	{
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("display_unit");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.display_unit);
		prop_to_update++;
		ind = ind + 2;
	}

	if (strcmp(conf.format,FormatNotSpec) != 0)
	{
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("format");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.format);
		prop_to_update++;
		ind = ind + 2;
	}

//
// For the last four, if the data type is not string, checks that the input
// strings are really number
//

	TangoSys_MemStream str;
	short sh;
	long lg;
	double db;
		
	if ((strcmp(conf.min_value,AlrmValueNotSpec) != 0) &&
	    (strcmp(conf.min_value,NotANumber) != 0))
	{
		if (data_type != Tango::DEV_STRING)
		{
			str << conf.min_value << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> sh))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> lg))
					throw_err_format("min_value",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> db))
					throw_err_format("min_value",dev_name);
				break;
			}
		}
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("min_value");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.min_value);
		prop_to_update++;
		ind = ind + 2;
	}
	
	if (strcmp(conf.min_value,NotANumber) == 0)
	{
		prop_to_delete++;

		db_del_data = new Tango::DevVarStringArray(10);
		db_del_data->length(3);
	
		(*db_del_data)[0] = CORBA::string_dup(dev_name.c_str());
		(*db_del_data)[1] = CORBA::string_dup(name.c_str());
		(*db_del_data)[2] = CORBA::string_dup("min_value");
	}

	if ((strcmp(conf.max_value,AlrmValueNotSpec) != 0) &&
	    (strcmp(conf.max_value,NotANumber) != 0))
	{
		if (data_type != Tango::DEV_STRING)
		{
			str.seekp(0);
			str.seekg(0);
			
			str << conf.max_value << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> sh))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> lg))
					throw_err_format("max_value",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> db))
					throw_err_format("max_value",dev_name);
				break;
			}
		}
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("max_value");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.max_value);
		prop_to_update++;
		ind = ind + 2;
	}

	if (strcmp(conf.max_value,NotANumber) == 0)
	{
		prop_to_delete++;
		
		if (prop_to_delete == 1)
		{
			db_del_data = new Tango::DevVarStringArray(10);
			db_del_data->length(3);
	
			(*db_del_data)[0] = CORBA::string_dup(dev_name.c_str());
			(*db_del_data)[1] = CORBA::string_dup(name.c_str());
			(*db_del_data)[2] = CORBA::string_dup("max_value");
		}
		else
		{
			len_del = len_del++;
			db_del_data->length(len_del);
			(*db_del_data)[len_del - 1] = CORBA::string_dup("max_value");
		}
	}
	
	if ((strcmp(conf.min_alarm,AlrmValueNotSpec) != 0) &&
	    (strcmp(conf.min_alarm,NotANumber) != 0))
	{
		if (data_type != Tango::DEV_STRING)
		{
			str.seekp(0);
			str.seekg(0);
			
			str << conf.min_alarm << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> sh))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> lg))
					throw_err_format("min_alarm",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> db))
					throw_err_format("min_alarm",dev_name);
				break;
			}
		}
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("min_alarm");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.min_alarm);
		prop_to_update++;
		ind = ind + 2;
	}
	
	if (strcmp(conf.min_alarm,NotANumber) == 0)
	{
		prop_to_delete++;

		if (prop_to_delete == 1)
		{
			db_del_data = new Tango::DevVarStringArray(10);
			db_del_data->length(3);
	
			(*db_del_data)[0] = CORBA::string_dup(dev_name.c_str());
			(*db_del_data)[1] = CORBA::string_dup(name.c_str());
			(*db_del_data)[2] = CORBA::string_dup("min_alarm");
		}
		else
		{
			len_del = len_del++;
			db_del_data->length(len_del);
			(*db_del_data)[len_del - 1] = CORBA::string_dup("min_alarm");
		}
	}
	
	if ((strcmp(conf.max_alarm,AlrmValueNotSpec) != 0) &&
	    (strcmp(conf.max_alarm,NotANumber) != 0))
	{
		if (data_type != Tango::DEV_STRING)
		{
			str.seekp(0);
			str.seekg(0);
			
			str << conf.max_alarm << ends;
			switch (data_type)
			{
			case Tango::DEV_SHORT:
				if (!(str >> sh))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_LONG:
				if (!(str >> lg))
					throw_err_format("max_alarm",dev_name);
				break;
			
			case Tango::DEV_DOUBLE:
				if (!(str >> db))
					throw_err_format("max_alarm",dev_name);
				break;
			}
		}
		len = len + 2;
		db_data->length(len);
		(*db_data)[ind] = CORBA::string_dup("max_alarm");
		(*db_data)[ind + 1] = CORBA::string_dup(conf.max_alarm);
		prop_to_update++;
		ind = ind + 2;
	}

	if (strcmp(conf.max_alarm,NotANumber) == 0)
	{
		prop_to_delete++;
		
		if (prop_to_delete == 1)
		{
			db_del_data = new Tango::DevVarStringArray(10);
			db_del_data->length(3);
	
			(*db_del_data)[0] = CORBA::string_dup(dev_name.c_str());
			(*db_del_data)[1] = CORBA::string_dup(name.c_str());
			(*db_del_data)[2] = CORBA::string_dup("max_alarm");
		}
		else
		{
			len_del = len_del++;
			db_del_data->length(len_del);
			(*db_del_data)[len_del - 1] = CORBA::string_dup("max_alarm");
		}
	}
										
//
// Update db only if needed
//

	if (prop_to_update != 0)
	{
		cout4 << prop_to_update << " properties to update in db" << endl;
		TangoSys_MemStream o;
		const char *tmp_ptr;
		
		o << prop_to_update << ends;
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
		tmp_ptr = o.str().c_str();
#else
		tmp_ptr = o.str();
#endif		
		(*db_data)[3] = CORBA::string_dup(tmp_ptr);	
		CORBA::Any send;
		send <<= db_data;
	
		Tango::Util *tg = Tango::Util::instance();

//
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
//

		bool retry = true;
		while (retry == true)
		{
			try
			{
				CORBA::Any_var received = tg->get_database()->get_dbase()->command_inout("DbPutDeviceAttributeProperty",send);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
#ifndef WIN32
		delete [] tmp_ptr;
#endif
	}
	
	if (prop_to_delete != 0)
	{
		cout4 << prop_to_delete << " properties to delete in db" << endl;
		CORBA::Any send;
		send <<= db_del_data;
	
		Tango::Util *tg = Tango::Util::instance();
//
// Implement a reconnection schema. The first exception received if the db
// server is down is a COMM_FAILURE exception. Following exception received
// from following calls are TRANSIENT exception
//

		bool retry = true;
		while (retry == true)
		{
			try
			{
				CORBA::Any_var received = tg->get_database()->get_dbase()->command_inout("DbDeleteDeviceAttributeProperty",send);
				retry = false;
			}
			catch (CORBA::COMM_FAILURE)
			{
				tg->get_database()->reconnect(true);
			}
		}
	}
	
	cout4 << "Leaving upd_database method" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_value
// 
// description : 	Set the attribute read value and quality. This method
//			automatically set the date when it has been called
//			This method is overloaded several times for all the
//			supported attribute data type
//
// in :			p_data : The attribute read value
//			qua : The attribute read value quality
//
//--------------------------------------------------------------------------

void Attribute::set_value(Tango::DevShort *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_SHORT)
	{
		TangoSys_OMemStream o;
		
		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		TangoSys_OMemStream o;
		
		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}
	
//
// Compute data size and set default quality to valid
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;
	
//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			tmp_sh[0] = *p_data;
			if (release == true)
				delete p_data;
		}
		else
			value.sh_seq = new Tango::DevVarShortArray(data_size,data_size,p_data,release);
	}
	value_flag = true;
	
//
// Reset alarm flags
//

	min_alarm_on = false;
	max_alarm_on = false;

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevLong *p_data,long x,long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_LONG)
	{
		TangoSys_OMemStream o;
		
		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",
				      o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		TangoSys_OMemStream o;
		
		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}
	
//
// Compute data size and set default quality to valid
//

	dim_x = x;
	dim_y = y;	
	set_data_size();
	quality = Tango::ATTR_VALID;
	
//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			tmp_lo[0] = *p_data;
			if (release == true)
				delete p_data;
		}
		else
			value.lg_seq = new Tango::DevVarLongArray(data_size,data_size,p_data,release);
	}
	value_flag = true;
	
//
// Reset alarm flags
//

	min_alarm_on = false;
	max_alarm_on = false;

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevDouble *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_DOUBLE)
	{
		TangoSys_OMemStream o;
		
		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		TangoSys_OMemStream o;
		
		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}
	
//
// Compute data size and set default quality to valid
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;
	
//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			tmp_db[0] = *p_data;
			if (release == true)
				delete p_data;
		}
		else
			value.db_seq = new Tango::DevVarDoubleArray(data_size,data_size,p_data,release);
	}
	value_flag = true;
	
//
// Reset alarm flags
//

	min_alarm_on = false;
	max_alarm_on = false;

//
// Get time
//

	set_time();
}

void Attribute::set_value(Tango::DevString *p_data,long x, long y,bool release)
{

//
// Throw exception if type is not correct
//

	if (data_type != Tango::DEV_STRING)
	{
		TangoSys_OMemStream o;
		
		o << "Invalid data type for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}

//
// Check that data size is less than the given max
//

	if ((x > max_x) || (y > max_y))
	{
		TangoSys_OMemStream o;
		
		o << "Data size for attribute " << name << " exceeds given limit" << ends;
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"Attribute::set_value");
	}
		
//
// Compute data size and set default quality to valid
//

	dim_x = x;
	dim_y = y;
	set_data_size();
	quality = Tango::ATTR_VALID;

//
// If the data is wanted from the DevState command, store it in a sequence.
// If the attribute  has an associated writable attribute, store data in a
// temporary buffer (the write value must be added before the data is sent
// back to the caller)
//

	if (date == false)
	{
		if (release == true)
		{
			char **strvec = Tango::DevVarStringArray::allocbuf(data_size);
			for (int i = 0;i < data_size;i++)
				strvec[i] = p_data[i];
			value.str_seq = new Tango::DevVarStringArray(data_size,data_size,strvec,release);
		}
		else
			value.str_seq = new Tango::DevVarStringArray(data_size,data_size,p_data,release);			
	}
	else
	{
		if ((is_writ_associated() == true))
		{
			tmp_str[0] = *p_data;
			if (release == true)
				delete p_data;
		}
		else
		{
			if (release == true)
			{
				char **strvec = Tango::DevVarStringArray::allocbuf(data_size);
				for (int i = 0;i < data_size;i++)
					strvec[i] = p_data[i];
				value.str_seq = new Tango::DevVarStringArray(data_size,data_size,strvec,release);
			}
			else
				value.str_seq = new Tango::DevVarStringArray(data_size,data_size,p_data,release);
		}
	}
	value_flag = true;

//
// Get time
//

	set_time();
}


//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_data_size
// 
// description : 	Compute the attribute amount of data
//
//--------------------------------------------------------------------------

void Attribute::set_data_size()
{
	switch(data_format)
	{
	case Tango::SCALAR :
		data_size = 1;
		break;
		
	case Tango::SPECTRUM :
		data_size = dim_x;
		break;
		
	case Tango::IMAGE :
		data_size = dim_x * dim_y;
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::set_time
// 
// description : 	Set the date if the date flag is true
//
//--------------------------------------------------------------------------

void Attribute::set_time()
{
	if (date == true)
	{	
#ifdef WIN32
		struct _timeb t;
		_ftime(&t);
	
		when.tv_sec = (CORBA::Long)t.time;
		when.tv_usec = (CORBA::Long)(t.millitm * 1000);
		when.tv_nsec = 0;
#else	
		struct timezone tz;
		struct timeval tv;
		gettimeofday(&tv,&tz);
	
		when.tv_sec = (CORBA::Long)tv.tv_sec;
		when.tv_usec = (CORBA::Long)tv.tv_usec;
		when.tv_nsec = 0;
#endif
	}
}

//+-------------------------------------------------------------------------
//
// method :		Attribute::check_alarm
//
// description :	Check if the attribute is in alarm
//
// This method returns a boolean set to true if the atribute is in alarm. In
// this case, it also set the attribute quality factor to ALARM
//
//--------------------------------------------------------------------------

bool Attribute::check_alarm()
{
	bool returned = false;
//
// Throw exception if no alarm is defined for this attribute
//

	if (is_alarmed() == false)
	{
		TangoSys_OMemStream o;
		
		o << "No alarm defined for attribute " << name << ends;
		Except::throw_exception((const char *)"API_AttrNoAlarm",o.str(),
				      (const char *)"Attribute::check_alarm");
	}

//
// If the attribute quality factor is set to INVALID, don't check anything
//

	if (quality == Tango::ATTR_INVALID)
	{
		return returned;
	}
		
//
// Check the min alarm if defined
//

	long i;
	if (check_min_alarm == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.sh_seq)[i] <= min_alarm.sh)
				{
					quality = Tango::ATTR_ALARM;
					min_alarm_on = true;
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_LONG:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.lg_seq)[i] <= min_alarm.lg)
				{
					quality = Tango::ATTR_ALARM;
					min_alarm_on = true;
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_DOUBLE:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.db_seq)[i] <= min_alarm.db)
				{
					quality = Tango::ATTR_ALARM;
					min_alarm_on = true;
					returned = true;
				}
			}
			break;
		}
	}
	
//
// Check the max alarm if defined
//

	if (check_max_alarm == true)
	{
		switch (data_type)
		{
		case Tango::DEV_SHORT:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.sh_seq)[i] >= max_alarm.sh)
				{
					quality = Tango::ATTR_ALARM;
					max_alarm_on = true;
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_LONG:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.lg_seq)[i] >= max_alarm.lg)
				{
					quality = Tango::ATTR_ALARM;
					max_alarm_on = true;
					returned = true;
				}
			}
			break;
			
		case Tango::DEV_DOUBLE:
			for (i = 0;i < data_size;i++)
			{
				if ((*value.db_seq)[i] >= max_alarm.db)
				{
					quality = Tango::ATTR_ALARM;
					max_alarm_on = true;
					returned = true;
				}
			}
			break;
		}
	}
	
	return returned;
}

//+-------------------------------------------------------------------------
//
// method :		Attribute::delete_seq
//
// description :	Delete the sequence created to store attribute
//			value.
//			In case of a read_attributes CORBA operation,
//			this delete will be done automatically because the
//			sequence is inserted in an CORBA::Any object which will
//			delete the sequence when it is destroyed.
//			This method is usefull only to delete the sequence
//			created in case of the attribute is read during a
//			DevState command to evaluate  device state
//
//--------------------------------------------------------------------------

void Attribute::delete_seq()
{
	switch (data_type)
	{
	case Tango::DEV_SHORT:
		delete value.sh_seq;
		break;
		
	case Tango::DEV_LONG:
		delete value.lg_seq;
		break;
		
	case Tango::DEV_DOUBLE:
		delete value.db_seq;
		break;
		
	case Tango::DEV_STRING:
		delete value.str_seq;
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		Attribute::add_write_value
// 
// description : 	These methods add the associated writable attribute
//			value to the read attribute buffer and create a
//			sequence from the attribute internal buffer
//
// in :			val : The associated write attribute value
//
//--------------------------------------------------------------------------

void Attribute::add_write_value(Tango::DevShort &val)
{
	tmp_sh[1] = val;
	
	value.sh_seq = new Tango::DevVarShortArray(data_size + 1,data_size + 1,tmp_sh,false);	 
}

void Attribute::add_write_value(Tango::DevLong &val)
{
	tmp_lo[1] = val;
	
	value.lg_seq = new Tango::DevVarLongArray(data_size + 1,data_size + 1,tmp_lo,false);	 
}

void Attribute::add_write_value(Tango::DevDouble &val)
{
	tmp_db[1] = val;
	
	value.db_seq = new Tango::DevVarDoubleArray(data_size + 1,data_size + 1,tmp_db,false);	 
}

void Attribute::add_write_value(Tango::DevString &val)
{
	tmp_str[1] = val;
	
	value.str_seq = new Tango::DevVarStringArray(data_size + 1,data_size + 1,tmp_str,false);	 
}


//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			Attribute class
//
//--------------------------------------------------------------------------

#ifndef TANGO_HAS_LOG4TANGO
ostream &operator<<(ostream &o_str,Attribute &p)
{
	Tango::AttributeConfig conf;

//
// Get attribute properties
//
	
	p.get_properties(conf);

//
// Print all these properties
//
	
	cout << "Attribute name = " << conf.name.in() << endl;
	cout << "Attribute data_type = ";
	switch (conf.data_type)
	{
	case Tango::DEV_SHORT :
		cout << "Tango::DevShort" << endl;
		break;
		
	case Tango::DEV_LONG :
		cout << "Tango::DevLong" << endl;
		break;
		
	case Tango::DEV_DOUBLE :
		cout << "Tango::DevDouble" << endl;
		break;
		
	case Tango::DEV_STRING :
		cout << "Tango::DevString" << endl;
		break;
	}
	
	cout << "Attribute data_format = ";
	switch (conf.data_format)
	{
	case Tango::SCALAR :
		cout << "scalar" << endl;
		break;
		
	case Tango::SPECTRUM :
		cout << "spectrum, max_dim_x = " << conf.max_dim_x << endl;
		break;
		
	case Tango::IMAGE :
		cout << "image, max_dim_x = " << conf.max_dim_x << ", max_dim_y = " << conf.max_dim_y << endl;
		break;
	}

	if (conf.writable == static_cast<unsigned char>(true))
		cout << "Attribute is writable" << endl;
	else
		cout << "Attribute is not writable" << endl;
	cout << "Attribute label = " << conf.label.in() << endl;
	cout << "Attribute description = " << conf.description.in() << endl;
	cout << "Attribute unit = " << conf.unit.in();
	cout << ", standard unit = " << conf.standard_unit.in();
	cout << ", display unit = " << conf.display_unit.in() << endl;
	cout << "Attribute format = " << conf.format.in() << endl;
	cout << "Attribute min alarm = " << conf.min_alarm.in() << endl;
	cout << "Attribute max alarm = " << conf.max_alarm.in() << endl;
	cout << "Attribute min value = " << conf.min_value.in() << endl;
	cout << "Attribute max value = " << conf.max_value.in() << endl;	
	cout << "Attribute writable_attr_name = " << conf.writable_attr_name.in() << endl;
	
	return o_str;
}
#endif // TANGO_HAS_LOG4TANGO

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::WAttribute
// 
// description : 	constructor for the WAttribute class from the 
//			attribute property vector, its type and the device
//			name
//
// argument : in : 	- prop_list : The attribute property list
//			- type : The attrubute data type
//			- dev_name : The device name
//
//--------------------------------------------------------------------------


WAttribute::WAttribute(vector<AttrProperty> &prop_list,
		       Attr &tmp_attr,string &dev_name)
:Attribute(prop_list,tmp_attr,dev_name),ext(NULL)
{
	short_val = old_short_val = 0;
	long_val = old_long_val = 0;
	double_val = old_double_val = 0.0;
	str_val = (char *)"Not initialised";
	
	string_allocated = false;
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_value
// 
// description : 	This method is used when a Writable attribute is
//			set to set the value in the Attribute class. This
//			is necessary for the read_attribute CORBA operation
//			which takes its data from this internal Attribute
//			class data.
//			It is used in the read_attributes code in the
//			device class
//
//--------------------------------------------------------------------------

void WAttribute::set_value()
{
	switch(data_type)
	{
	case Tango::DEV_SHORT:
		Attribute::set_value(&short_val);
		break;
		
	case Tango::DEV_LONG:
		Attribute::set_value(&long_val);
		break;
				
	case Tango::DEV_DOUBLE:
		Attribute::set_value(&double_val);
		break;
		
	case Tango::DEV_STRING:
		Attribute::set_value(&str_val);
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::set_write_value
// 
// description : 	Set the value inside the Wattribute object from
//			the Any object received from the network. Some special
//			treatement for string due to memory allocation.
//
// in :			any : Reference to the CORBA Any object
//
//--------------------------------------------------------------------------

void WAttribute::set_write_value(const CORBA::Any &any)
{
	switch (data_type)
	{
	case Tango::DEV_SHORT :
	
//
// Check data type inside the any
//

		const Tango::DevVarShortArray *sh_ptr;
		if ((any >>= sh_ptr) == false)
		{
			TangoSys_OMemStream o;
	
			o << "Incompatible attribute type, expected type is : Tango::DevVarShortArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::set_write_value()");
		}
		short sh;
		sh = (*sh_ptr)[0];
		
//
// Check the incoming value
//

		if (check_min_value == true)
		{
			if (sh < min_value.sh)
			{
				TangoSys_OMemStream o;
	
				o << "Set value for attribute " << name;
				o << " is below the minimum authorized" << ends;
				Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
			}
		}
		if (check_max_value == true)
		{
			if (sh > max_value.sh)
			{
				TangoSys_OMemStream o;
	
				o << "Set value for attribute " << name;
				o << " is above the maximum authorized" << ends;
				Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
			}
		}

		old_short_val = short_val;		
		short_val = sh;
		break;
		
	case Tango::DEV_LONG :
	
//
// Check data type inside the any
//

		const Tango::DevVarLongArray *lg_ptr;
		if ((any >>= lg_ptr) == false)
		{
			TangoSys_OMemStream o;
	
			o << "Incompatible attribute type, expected type is : Tango::DevVarLongArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::set_write_value()");
		}
		long lg;
		lg = (*lg_ptr)[0];

//
// Check the incoming value
//

		if (check_min_value == true)
		{
			if (lg < min_value.lg)
			{
				TangoSys_OMemStream o;
	
				o << "Set value for attribute " << name;
				o << " is below the minimum authorized" << ends;
				Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
			}
		}
		if (check_max_value == true)
		{
			if (lg > max_value.lg)
			{
				TangoSys_OMemStream o;
	
				o << "Set value for attribute " << name;
				o << " is above the maximum authorized" << ends;
				Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
			}
		}

		old_long_val = long_val;				
		long_val = lg;
		break;
		
	case Tango::DEV_DOUBLE :
	
//
// Check data type inside the any
//

		const Tango::DevVarDoubleArray *db_ptr;
		if ((any >>= db_ptr) == false)
		{
			TangoSys_OMemStream o;
	
			o << "Incompatible attribute type, expected type is : Tango::DevVarDoubleArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::set_write_value()");
		}
		double db;
		db = (*db_ptr)[0];
		
//
// Check the incoming value
//

		if (check_min_value == true)
		{
			if (db < min_value.db)
			{
				TangoSys_OMemStream o;
	
				o << "Set value for attribute " << name;
				o << " is below the minimum authorized" << ends;
				Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
			}
		}
		if (check_max_value == true)
		{
			if (db > max_value.db)
			{
				TangoSys_OMemStream o;
	
				o << "Set value for attribute " << name;
				o << " is above the maximum authorized" << ends;
				Except::throw_exception((const char *)"API_WAttrOutsideLimit",
						      o.str(),
						      (const char *)"WAttribute::set_write_value()");
			}
		}		

		old_double_val = double_val;
		double_val = db;
		break;
		
	case Tango::DEV_STRING :
		const Tango::DevVarStringArray *str_ptr;
		if ((any >>= str_ptr) == false)
		{
			TangoSys_OMemStream o;
	
			o << "Incompatible attribute type, expected type is : Tango::DevVarStringArray (even for single value)" << ends;
			Except::throw_exception((const char *)"API_IncompatibleAttrDataType",
					      o.str(),
					      (const char *)"WAttribute::set_write_value()");
		}
		if (string_allocated == true)
			CORBA::string_free(old_str_val);
		old_str_val = CORBA::string_dup(str_val);
		
		if (string_allocated == true)
			CORBA::string_free(str_val);
		else
			string_allocated = true;
			
		str_val = CORBA::string_dup((*str_ptr)[0]);
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		WAttribute::rollback
// 
// description : 	Reset the internal data to its value before the
//			set_write_value method was applied (Useful in case of
//			error in the set_write_value method)
//
//--------------------------------------------------------------------------

void WAttribute::rollback()
{
	switch (data_type)
	{
	case Tango::DEV_SHORT :
		short_val = old_short_val;
		break;
		
	case Tango::DEV_LONG :
		long_val = old_long_val;
		break;
		
	case Tango::DEV_DOUBLE :
		double_val = old_double_val;
		break;
		
	case Tango::DEV_STRING :
		CORBA::string_free(str_val);
		str_val = CORBA::string_dup(old_str_val);
		break;
	}
}

} // End of Tango namespace

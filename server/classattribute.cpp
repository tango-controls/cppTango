static const char *RcsId = "$Header$";

//+============================================================================
//
// file :               ClassAttribute.cpp
//
// description :        C++ source code for the 
//				AttrProperty
//				ClassAttribute and 
//				MultiClassAttribute
//			classes. These classes
//			are used to manage attribute properties defined at the
//			class level. A Tango DeviceClass class instance has one
//			MultiClassAttribute object which is an aggregate of
//			ClassAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:15:36  taurel
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
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:11  taurel
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
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <classattribute.h>
#include <algorithm>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		AttrProperty::AttrProperty 
// 
// description : 	Constructors for the AttrProperty class.
//			These constructor change the property name to
//			lowercase letters and also change the property
//			value to lowercase letters for the data_format and
//			data_type property
//
// argument : in : 	- name : The property name
//			- value : The property value
//
//--------------------------------------------------------------------------

AttrProperty::AttrProperty(string &name,string &value):attr_name(name),
						       attr_value(value)
{
	attr_lg = 0;
	
//
// Property name in lower case letters
//

	transform(attr_name.begin(),attr_name.end(),attr_name.begin(),Tango_tolower);
	
//
// For data_type or data_format properties, also change property value to
// lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
	{
		transform(attr_value.begin(),attr_value.end(),
			  attr_value.begin(),Tango_tolower);
	}
}

AttrProperty::AttrProperty(const char *name,const char *value):attr_name(name),
							       attr_value(value)
{
	attr_lg = 0;
	
//
// Property name in lower case letters
//

	transform(attr_name.begin(),attr_name.end(),attr_name.begin(),Tango_tolower);
	
//
// For data_type or data_format properties, also change property value to
// lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
	{
		transform(attr_value.begin(),attr_value.end(),
			  attr_value.begin(),Tango_tolower);
	}
}

AttrProperty::AttrProperty(const char *name,long value):attr_name(name),
							attr_lg(value)
{
}

AttrProperty::AttrProperty(const char *name,string &value):attr_name(name),
							   attr_value(value)
{
}

//+-------------------------------------------------------------------------
//
// method : 		AttrProperty::convert 
// 
// description : 	Convert the property value into a long. The long data
//			is also stored in the AttrProperty class
//
//--------------------------------------------------------------------------

void AttrProperty::convert()
{
	TangoSys_MemStream o;
	o << attr_value << ends;
	if (!(o >> attr_lg))
	{
		Except::throw_exception((const char *)"API_AttrOptProp",
				      (const char *)"Can't convert property value",
				      (const char *)"AttrProperty::convert");
	}
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			AttrProperty class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,const AttrProperty &p)
{
	o_str << "Property name = " << p.attr_name << ", Property value = " << p.attr_value;
	return o_str;
}


//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::MultiClassAttribute
// 
// description : 	constructor for the MultiClassAttribute class from the 
//			device class name
//
//--------------------------------------------------------------------------

MultiClassAttribute::~MultiClassAttribute()
{
	long nb_attr = attr_list.size();
	for (int i = 0;i < nb_attr;i++)
		delete attr_list[i];	
}


//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::MultiClassAttribute
// 
// description : 	constructor for the MultiClassAttribute class from the 
//			device class name
//
//--------------------------------------------------------------------------

MultiClassAttribute::MultiClassAttribute()
{
	cout4 << "Entering MultiClassAttribute constructor" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::init_class_attribute
// 
// description : 	Ask the database for prperties defined at class
//			level and build the ClassAttribute object for
//			each attribute with defined properties
//
// argument : in : 	- class_name : The device class name
//
//--------------------------------------------------------------------------

void MultiClassAttribute::init_class_attribute(string &class_name,long base)
{
	cout4 << "Entering MultiClassAttribute::init_class_attribute" << endl;
	long i;


	Tango::Util *tg = Tango::Util::instance();
	CORBA::Any send;

	long nb_attr;
	if (base == 0)	
		nb_attr = attr_list.size();
	else
		nb_attr = 1;
	
//
// Get class attribute(s) properties stored in DB
// No need to implement
// a retry here (in case of db server restart) because the db reconnection
// is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

	if ((nb_attr != 0) && (Tango::Util::_UseDb == true))
	{
		Tango::DevVarStringArray *send_list = new Tango::DevVarStringArray(nb_attr + 1);
		send_list->length(nb_attr + 1);
	
		(*send_list)[0] = CORBA::string_dup(class_name.c_str());
		for (i = 0;i < nb_attr;i++)
			(*send_list)[i + 1] = CORBA::string_dup(attr_list[i + base]->get_name().c_str());
			
		send <<= send_list;
		CORBA::Any_var received_prop = tg->get_database()->get_dbase()->command_inout("DbGetClassAttributeProperty",send);
		const Tango::DevVarStringArray *db_attr_prop_list;
		if ((received_prop >>= db_attr_prop_list) == false)
		{
			TangoSys_OMemStream o;
			o << "Can't get class attribute properties for class " << class_name << ends;
					
			Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"MultiClassAttribute::init_class_attribute");
		}
	
//
// Sort property for each attribute and create a ClassAttribute object for each
// of them
//
	
		long ind = 2;
		TangoSys_MemStream o;
	
		for (i = 0;i < nb_attr;i++)
		{
			vector<AttrProperty> prop_list;

			string attr_name = (*db_attr_prop_list)[ind].in();		
			ind++;
		
			long nb_prop;
			o << (*db_attr_prop_list)[ind].in() << ends;
			o >> nb_prop;

			o.seekp(0);
			o.seekg(0);
		
			ind++;
			for (long j = 0;j < nb_prop;j++)
			{
				prop_list.push_back(AttrProperty((*db_attr_prop_list)[ind],(*db_attr_prop_list)[ind + 1]));
				ind = ind + 2;
			}

			if (nb_prop != 0)
			{
//
// Find this attribute in the attribute list
//

				int k;
				for (k = 0;k < attr_list.size();k++)
				{
					if (attr_name == attr_list[k]->get_name())
						break;
				}
				if (k == attr_list.size())
				{
					TangoSys_OMemStream o;
					o << "Attribute " << attr_name << " not found in class attribute(s)" << ends;
		
					Except::throw_exception((const char *)"API_AttrNotFound",
							        o.str(),
							        (const char *)"MultiClassAttribute::init_class_attribute");
				}
				
//
// Add its class property list
//

				attr_list[k]->set_class_properties(prop_list);
			}		
		}
	}

	for (i = 0;i < nb_attr;i++)
		cout4 << *(attr_list[i + base]) << endl;
			
	cout4 << "Leaving MultiClassAttribute::init_class_attribute" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		MultiClassAttribute::get_attr
// 
// description : 	Get the Attr object for the attribute with
//			name passed as parameter
//
// in :			attr_name : The attribute name
//
// This method returns a reference to the ClassAttribute object or throw
// an exceptionif the attribute is not found
//
//--------------------------------------------------------------------------


Attr &MultiClassAttribute::get_attr(string &attr_name)
{

//
// Search for the wanted attribute in the attr_list vector from its name
//

	vector<Attr *>::iterator pos;
	
	pos = find_if(attr_list.begin(),attr_list.end(),
		      bind2nd(WantedClassAttr<Attr *,string,bool>(),attr_name));
	
	if (pos == attr_list.end())
	{
		TangoSys_OMemStream o;
		o << "Attribute " << attr_name << " not found in class attribute(s)" << ends;
		
		Except::throw_exception((const char *)"API_AttrOptProp",o.str(),
				      (const char *)"MultiClassAttribute::get_attr");
	}
	
	return *(*pos);
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			Attr class. It prints all the attribute
//			property(ies) name and value defined in DB
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,const Attr &c)
{
	long nb_prop = c.class_properties.size();
	for (long i = 0;i < nb_prop;i++)
	{
		o_str << c.class_properties[i];
		if (i <= (nb_prop - 2))
			o_str << endl;
	}
	return o_str;
}

} // End of Tango namespace

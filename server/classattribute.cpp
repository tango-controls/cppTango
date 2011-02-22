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

	transform(attr_name.begin(),attr_name.end(),attr_name.begin(),tolower);
	
//
// For data_type or data_format properties, also change property value to
// lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
	{
		transform(attr_value.begin(),attr_value.end(),
			  attr_value.begin(),tolower);
	}
}

AttrProperty::AttrProperty(const char *name,const char *value):attr_name(name),
							       attr_value(value)
{
	attr_lg = 0;
	
//
// Property name in lower case letters
//

	transform(attr_name.begin(),attr_name.end(),attr_name.begin(),tolower);
	
//
// For data_type or data_format properties, also change property value to
// lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
	{
		transform(attr_value.begin(),attr_value.end(),
			  attr_value.begin(),tolower);
	}
}

AttrProperty::AttrProperty(const char *name,long value):attr_name(name),
							attr_lg(value)
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

void MultiClassAttribute::init_class_attribute(string &class_name)
{
	cout4 << "Entering MultiClassAttribute::init_class_attribute" << endl;
	long i;


	Tango::Util *tg = Tango::Util::instance();
	CORBA::Any send;
	
	long nb_attr = attr_list.size();
	
//
// Get class attribute(s) properties stored in DB
//

	if (nb_attr != 0)
	{
		Tango::DevVarStringArray *send_list = new Tango::DevVarStringArray(nb_attr + 1);
		send_list->length(nb_attr + 1);
	
		(*send_list)[0] = CORBA::string_dup(class_name.c_str());
		for (i = 0;i < nb_attr;i++)
			(*send_list)[i + 1] = CORBA::string_dup(attr_list[i]->get_name().c_str());
			
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

			string attr_name = (*db_attr_prop_list)[ind];		
			ind++;
		
			long nb_prop;
			o << (*db_attr_prop_list)[ind] << ends;
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
				for (k = 0;k < nb_attr;k++)
				{
					if (attr_name == attr_list[i]->get_name())
						break;
				}
				if (k == nb_attr)
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

	for (i = 0;i < attr_list.size();i++)
		cout4 << *(attr_list[i]) << endl;
			
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

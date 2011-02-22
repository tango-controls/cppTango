static const char *RcsId = "$Header$";

//+============================================================================
//
// file :               MultiAttribute.cpp
//
// description :        C++ source code for the MultiAttribute class. This class
//			is used to manage attribute.
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
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <multiattribute.h>
#include <classattribute.h>

#include <functional>
#include <algorithm>

namespace Tango
{

//
// Define the optional attribute name and default value
//

static OptAttrProp Tango_OptAttrProp[] = {
	{"label","No label"},
	{"description","No description"},
	{"unit","No unit"},
	{"standard_unit","No standard unit"},
	{"display_unit","No display unit"},
	{"format","No format"},
	{"min_value",AlrmValueNotSpec},
	{"max_value",AlrmValueNotSpec},
	{"min_alarm",AlrmValueNotSpec},
	{"max_alarm",AlrmValueNotSpec},
	{"writable_attr_name",AssocWritNotSpec}
};

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
// method : 		MultiAttribute::MultiAttribute
// 
// description : 	constructor for the MultiAttribute class from the 
//			device device name and a pointer to the DeviceClass
//			object
//
// argument : in : 	- dev_name : The device name
//			- dev_class_ptr : Pointer to the DeviceClass object
//
//--------------------------------------------------------------------------

MultiAttribute::MultiAttribute(string &dev_name,DeviceClass *dev_class_ptr)
{
	long i;
	cout4 << "Entering MultiAttribute class constructor for device " << dev_name << endl;
	
//
// Retrieve attr name list
//

	vector<Attr *> &tmp_attr_list = dev_class_ptr->get_class_attr()->get_attr_list();
	long nb_attr = tmp_attr_list.size();
	
//
// Get device attribute properties
//

	if (nb_attr != 0)
	{
		Tango::Util *tg = Tango::Util::instance();
		CORBA::Any send;
		Tango::DevVarStringArray *send_list = new Tango::DevVarStringArray(nb_attr + 1);
		send_list->length(nb_attr + 1);
	
		(*send_list)[0] = CORBA::string_dup(dev_name.c_str());
		for (i = 0;i < nb_attr;i++)
			(*send_list)[i + 1] = CORBA::string_dup(tmp_attr_list[i]->get_name().c_str());
			
		send <<= send_list;
		CORBA::Any_var received_prop = tg->get_database()->get_dbase()->command_inout("DbGetDeviceAttributeProperty",send);
		const Tango::DevVarStringArray *db_attr_prop_list;
		if ((received_prop >>= db_attr_prop_list) == false)
		{
			TangoSys_OMemStream o;
			o << "Can't get device attribute properties for device " << dev_name << ends;
					
			Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"MultiAttribute::MultiAttribute");
		}
		
//
// Create a vector of strings with the values returned from db
//

		vector<string> db_dev_attr_prop;
		long nb_db_data = db_attr_prop_list->length();
		nb_db_data = nb_db_data - 2;
		for (i = 0;i < nb_db_data;i++)
			db_dev_attr_prop.push_back(string((*db_attr_prop_list)[i + 2]));
				
//
// Build property list for each attribute
//

		TangoSys_MemStream o;
		long ind = 0;
	
		for (i = 0;i < nb_attr;i++)
		{
	
//
// Get attribute class properties
//

			Attr &attr = dev_class_ptr->get_class_attr()->get_attr(tmp_attr_list[i]->get_name());
			vector<AttrProperty> &class_prop = attr.get_class_properties();

//
// If the attribute has some properties defined at device level, build a vector
// of these properties
//
		
			vector<AttrProperty> dev_prop;

			ind++;
			long nb_prop;
			o << db_dev_attr_prop[ind] << ends;
			o >> nb_prop;
			ind++;

			o.seekp(0);
			o.seekg(0);

			for (long j = 0;j < nb_prop;j++)
			{
				dev_prop.push_back(AttrProperty(db_dev_attr_prop[ind],
								db_dev_attr_prop[ind + 1]));
				ind = ind + 2;
			}

//
// Concatenate these two attribute properties levels
//
	
			vector<AttrProperty> prop_list;
			concat(dev_prop,class_prop,prop_list);
			add_default(prop_list);
		
//
// Check and get attribute data type
//

			long data_type = attr.get_type();
		
//
// Create an Attribute instance
//

			if ((attr.get_writable() == Tango::WRITE) ||
			    (attr.get_writable() == Tango::READ_WRITE))
				attr_list.push_back(new WAttribute(prop_list,attr,dev_name));
			else
				attr_list.push_back(new Attribute(prop_list,attr,dev_name));
		
//
// If it is writable, add it to the writable attribute list
//

			if ((attr_list[i]->get_writable() == Tango::WRITE) ||
			    (attr_list[i]->get_writable() == Tango::READ_WRITE))
			{
				writable_attr_list.push_back(i);
			}
		
//
// If one of the alarm properties is defined, add it to the alarmed attribute
// list
//

			if (attr_list[i]->is_alarmed() == true)
			{
				alarm_attr_list.push_back(i);
			}
			
			cout4 << *(attr_list[i]) << endl;
		}
	}
	
//
// For each attribute, check if the writable_attr_name is set and in this
// case, check if the associated attribute exists and is writable
//

	for (i = 0;i < nb_attr;i++)
	{
		if ((attr_list[i]->get_writable() == Tango::READ_WITH_WRITE) ||
		    (attr_list[i]->get_writable() == Tango::READ_WRITE))
		{
		
			if (attr_list[i]->get_data_format() != Tango::SCALAR)
			{
				TangoSys_OMemStream o;
				
				o << "Device --> " << dev_name;
				o << "\nProperty writable_attr_name for attribute " << attr_list[i]->get_name();
				o << " is defined but this attribute data format is not SCALAR" << ends;
				Except::throw_exception((const char *)"API_AttrOptProp",
						      o.str(),
						      (const char *)"MultiAttribute::MultiAttribute");
			}
			
			long j;
			string &assoc_name = attr_list[i]->get_assoc_name();
			transform(assoc_name.begin(),assoc_name.end(),assoc_name.begin(),toupper);
			for (j = 0;j < writable_attr_list.size();j++)
			{
				if (attr_list[writable_attr_list[j]]->get_upper_name() == assoc_name)
					break;
			}
			if (j == writable_attr_list.size())
			{
				TangoSys_OMemStream o;

				o << "Device --> " << dev_name;		
				o << "\nProperty writable_attr_name for attribute " << attr_list[i]->get_name(); 
				o << " is set to " << assoc_name;
				o << ", but this attribute does not exists or is not writable" << ends;
				Except::throw_exception((const char *)"API_AttrOptProp",
						      o.str(),
						      (const char *)"MultiAttribute::MultiAttribute");
			}

//
// Also check if the associated write attribute is a scalar one
//
			
			if (attr_list[writable_attr_list[j]]->get_data_format() != Tango::SCALAR)
			{
				TangoSys_OMemStream o;
				
				o << "Device --> " << dev_name;		
				o << "\nProperty writable_attr_name for attribute " << attr_list[i]->get_name();
				o << " is set to " << assoc_name;
				o << ", but this attribute is not of the SCALAR data format" << ends;
				Except::throw_exception((const char *)"API_AttrOptProp",
						      o.str(),
						      (const char *)"MultiAttribute::MultiAttribute");
			}

//
// Check that the two associated attributes have the same data type
//
			
			if (attr_list[writable_attr_list[j]]->get_data_type() != attr_list[i]->get_data_type())
			{
				TangoSys_OMemStream o;
				
				o << "Device --> " << dev_name;		
				o << "\nProperty writable_attr_name for attribute " << attr_list[i]->get_name();
				o << " is set to " << assoc_name;
				o << ", but these two attributes do not support the same data type" << ends;
				Except::throw_exception((const char *)"API_AttrOptProp",
						      o.str(),
						      (const char *)"MultiAttribute::MultiAttribute");
			}
			
			attr_list[i]->set_assoc_ind(writable_attr_list[j]);
		}

	}
	
	cout4 << "Leaving MultiAttribute class constructor" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::~MultiAttribute
// 
// description : 	destructor for the MultiAttribute class. It simply 
//			delete all the Attribute object stored in its
//			attr_list data member
//
//--------------------------------------------------------------------------

MultiAttribute::~MultiAttribute()
{
	for(long i = 0;i < attr_list.size();i++)
		delete attr_list[i];
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::concat
// 
// description : 	Concatenate porperties defined at the class level and
//			at the device level. Prperties defined at the device
//			level have the highest priority
//
// in :			dev_prop : The device properties
//			class_prop : The class properties
//
// out :		result : The resulting vector
//
//--------------------------------------------------------------------------


void MultiAttribute::concat(vector<AttrProperty> &dev_prop,
			    vector<AttrProperty> &class_prop,
			    vector<AttrProperty> &result)
{
	
//
// Copy all device properties
//

	long i;
	for (i = 0;i < dev_prop.size();i++)
		result.push_back(dev_prop[i]);
		
//
// Add class properties if they have not been redefined at the device level
//

	vector<AttrProperty> tmp_result = result;
	long nb_class_check = class_prop.size() - 1;
	for (i = 0;i < nb_class_check;i++)
	{
		vector<AttrProperty>::iterator pos;

		pos = find_if(tmp_result.begin(),tmp_result.end(),
			      bind2nd(WantedProp<AttrProperty,string,bool>(),class_prop[i + 1].get_name()));
		
		if (pos != tmp_result.end())
			continue;
		else
			result.push_back(class_prop[i + 1]);
	}
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::add_default
// 
// description : 	Add default value for optional property if they
//			are not defined
//
// in :			prop_list : The already defined property vector
//
//--------------------------------------------------------------------------

void MultiAttribute::add_default(vector<AttrProperty> &prop_list)
{
	long nb_opt_prop = sizeof(Tango_OptAttrProp)/sizeof(OptAttrProp);

//
// For all the optional attribute properties, search in the already built
// vector of attributes if they are defined. If yes, continue. Otherwise,
// add a new property with the default value
//
	
	for (long i = 0;i < nb_opt_prop;i++)
	{
		vector<AttrProperty>::iterator pos;
		string opt_prop_name(Tango_OptAttrProp[i].name);
		
		pos = find_if(prop_list.begin(),prop_list.end(),
			      bind2nd(WantedProp<AttrProperty,string,bool>(),opt_prop_name));
			      
		if (pos == prop_list.end())
			prop_list.push_back(AttrProperty(Tango_OptAttrProp[i].name,Tango_OptAttrProp[i].default_value));
	}
}


//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::get_attr_by_name
// 
// description : 	Return a reference to the the Attribute object for
//			the wanted attribue
//
// in :			attr_name : The attribute name
//
// This method returns a reference to the wanted attribute or throws an
// exception id the attribute is not found
//
//--------------------------------------------------------------------------

Attribute &MultiAttribute::get_attr_by_name(const char *attr_name)
{
	vector<Attribute *>::iterator pos;
	
	pos = find_if(attr_list.begin(),attr_list.end(),
		      bind2nd(WantedAttr<Attribute *,const char *,bool>(),attr_name));
		      
	if (pos == attr_list.end())
	{
		cout3 << "MultiAttribute::get_attr_by_name throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << attr_name << " attribute not found" << ends;
		Except::throw_exception((const char *)"API_AttrNotFound",
				      o.str(),
				      (const char *)"MultiAttribute::get_attr_by_name");
	}
	
	return *(*pos);
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::get_w_attr_by_name
// 
// description : 	Return a reference to the the Attribute object for
//			the wanted attribue
//
// in :			attr_name : The attribute name
//
// This method returns a reference to the wanted attribute or throws an
// exception id the attribute is not found
//
//--------------------------------------------------------------------------

WAttribute &MultiAttribute::get_w_attr_by_name(const char *attr_name)
{
	vector<Attribute *>::iterator pos;
	
	pos = find_if(attr_list.begin(),attr_list.end(),
		      bind2nd(WantedAttr<Attribute *,const char *,bool>(),attr_name));
		      
	if (pos == attr_list.end())
	{
		cout3 << "MultiAttribute::get_w_attr_by_name throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << attr_name << " attribute not found" << ends;
		Except::throw_exception((const char *)"API_AttrNotFound",
				      o.str(),
				      (const char *)"MultiAttribute::get_w_attr_by_name");
	}

		
	return static_cast<WAttribute &>(*(*pos));
}


//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::get_attr_ind_by_name
// 
// description : 	Return the index in the Attribute object vector of
//			a specified attribute
//
// in :			attr_name : The attribute name
//
// This method returns the index of the wanted attribute or throws an
// exception id the attribute is not found
//
//--------------------------------------------------------------------------

long MultiAttribute::get_attr_ind_by_name(const char *attr_name)
{
	long i;

	long nb_attr = attr_list.size();
	string st(attr_name);	
	for (i = 0;i < nb_attr;i++)
	{
		if (attr_list[i]->get_name_size() != st.size())
			continue;
		transform(st.begin(),st.end(),st.begin(),toupper);
		if (attr_list[i]->get_upper_name() == st)
			break;
	}
	
	if (i == nb_attr)
	{
		cout3 << "MultiAttribute::get_attr_ind_by_name throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << attr_name << " attribute not found" << ends;
		Except::throw_exception((const char *)"API_AttrNotFound",
				      o.str(),
				      (const char *)"MultiAttribute::get_attr_ind_by_name");
	}
	
	return i;
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::check_alarm
// 
// description : 	check alarm on all the attribute where one alarm is
//			defined
//
// This method returns a boolen set to true if one of the attribute with
// an alarm defined is in alarm state
//
//--------------------------------------------------------------------------

bool MultiAttribute::check_alarm()
{
	long i;
	bool ret,tmp_ret;

	ret = false;	
	for (i = 0;i < alarm_attr_list.size();i++)
	{
		tmp_ret = check_alarm(alarm_attr_list[i]);
		if (tmp_ret == true)
		{
			ret = true;
			break;
		}
	}
	
	return ret;
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::read_alarm
// 
// description : 	Add a message in the device status string if one of
//			the device attribute is in the alarm state
//
// in :			status : The device status
//
//--------------------------------------------------------------------------

void MultiAttribute::read_alarm(string &status)
{
	long i;
	
	for (i = 0;i < alarm_attr_list.size();i++)
	{
		Attribute &att = get_attr_by_ind(alarm_attr_list[i]);

//
// Add a message for low level alarm
//

		if (att.is_min_alarm() == true)
		{
			string str = "\nAlarm : Value too low for attribute ";
			str = str + att.get_name();
				
			status = status + str;
		}

//
// Add a message for high level alarm
//
			
		if (att.is_max_alarm() == true)
		{
			string str = "\nAlarm : Value too high for attribute ";
			str = str + att.get_name();
				
			status = status + str;
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::add_write_value
// 
// description : 	For scalar attribute with an associated write 
//			attribute, the read_attributes CORBA operation also
//			returns the write value. This method gets the associated
//			write attribute value and adds it to the read
//			attribute
//
// in :			att : Reference to the attribute which must be read
//
//--------------------------------------------------------------------------

void MultiAttribute::add_write_value(Attribute &att)
{
	WAttribute &assoc_att = get_w_attr_by_ind(att.get_assoc_ind());
	
	switch (att.get_data_type())
	{
	case Tango::DEV_SHORT :
		Tango::DevShort sh_write_val;
		assoc_att.get_write_value(sh_write_val);
		
		att.add_write_value(sh_write_val);
		break;
		
	case Tango::DEV_LONG :
		Tango::DevLong lg_write_val;
		assoc_att.get_write_value(lg_write_val);
		
		att.add_write_value(lg_write_val);
		break;
		
	case Tango::DEV_DOUBLE :
		Tango::DevDouble db_write_val;
		assoc_att.get_write_value(db_write_val);
		
		att.add_write_value(db_write_val);
		break;
		
	case Tango::DEV_STRING :
		Tango::DevString str_write_val;
		assoc_att.get_write_value(str_write_val);
		
		att.add_write_value(str_write_val);
		break;
	}
}

} // End of Tango namespace

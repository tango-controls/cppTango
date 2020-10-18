//+=================================================================================================================
//
// file :               ClassAttribute.cpp
//
// description :        C++ source code for the
//				AttrProperty
//				ClassAttribute and
//				MultiClassAttribute
//			classes. These classes are used to manage attribute properties defined at the class level. A Tango
//			DeviceClass class instance has one MultiClassAttribute object which is an aggregate of
//			ClassAttribute objects
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <classattribute.h>
#include <algorithm>

namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		AttrProperty::AttrProperty
//
// description :
//		Constructors for the AttrProperty class. These constructor change the property name to lowercase letters and
//		also change the property value to lowercase letters for the data_format and data_type property
//
// argument :
//		in :
//			- name : The property name
//			- value : The property value
//
//------------------------------------------------------------------------------------------------------------------

AttrProperty::AttrProperty(std::string &name,std::string &value):attr_name(name),attr_value(value)
{
	attr_lg = 0;

//
// Property name in lower case letters
//

	std::transform(attr_name.begin(),attr_name.end(),attr_name.begin(),::tolower);

//
// For data_type or data_format properties, also change property value to lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
		std::transform(attr_value.begin(),attr_value.end(),attr_value.begin(),::tolower);
}

AttrProperty::AttrProperty(const char *name,const char *value):attr_name(name),attr_value(value)
{
	attr_lg = 0;

//
// Property name in lower case letters
//

	std::transform(attr_name.begin(),attr_name.end(),attr_name.begin(),::tolower);

//
// For data_type or data_format properties, also change property value to lowercase letters
//

	if ((attr_name == "data_type") || (attr_name == "data_format"))
		std::transform(attr_value.begin(),attr_value.end(),attr_value.begin(),::tolower);
}

AttrProperty::AttrProperty(const char *name,long value):attr_name(name),attr_lg(value)
{
}

AttrProperty::AttrProperty(const char *name,std::string &value):attr_name(name),attr_value(value),attr_lg(0)
{
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		AttrProperty::convert
//
// description :
//		Convert the property value into a long. The long data is also stored in the AttrProperty class
//
//------------------------------------------------------------------------------------------------------------------

void AttrProperty::convert(const char *prop_name)
{
	TangoSys_MemStream o;
	o << attr_value;
	if (!(o >> attr_lg && o.eof()))
	{
	    std::stringstream ss;
	    ss << "Can't convert property value for property " << prop_name;
		Except::throw_exception(API_AttrOptProp,
				      ss.str(),
				      "AttrProperty::convert");
	}
}

//+-----------------------------------------------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the AttrProperty class
//
//-------------------------------------------------------------------------------------------------------------------

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassAttribute::MultiClassAttribute
//
// description :
//		constructor for the MultiClassAttribute class from the device class name
//
//-------------------------------------------------------------------------------------------------------------------

MultiClassAttribute::~MultiClassAttribute()
{
	long nb_attr = attr_list.size();
	for (int i = 0;i < nb_attr;i++)
		delete attr_list[i];
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassAttribute::MultiClassAttribute
//
// description :
//		Constructor for the MultiClassAttribute class from the device class name
//
//--------------------------------------------------------------------------------------------------------------------

MultiClassAttribute::MultiClassAttribute()
{
	cout4 << "Entering MultiClassAttribute constructor" << std::endl;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassAttribute::init_class_attribute
//
// description :
//		Ask the database for properties defined at class level and build the ClassAttribute object for each attribute
//		with defined properties
//
// argument :
//		in :
//			- class_name : The device class name
//
//-------------------------------------------------------------------------------------------------------------------

void MultiClassAttribute::init_class_attribute(std::string &class_name,long base)
{
	cout4 << "Entering MultiClassAttribute::init_class_attribute" << std::endl;
	long i;

	Tango::Util *tg = Tango::Util::instance();
	CORBA::Any send;

	long nb_attr;
	if (base == 0)
		nb_attr = attr_list.size();
	else
		nb_attr = 1;

//
// Get class attribute(s) properties stored in DB. No need to implement a retry here (in case of db server restart)
// because the db reconnection is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

	if ((nb_attr != 0) && (Tango::Util::_UseDb == true))
	{
		Tango::DbData db_list;

		for(i = 0;i < nb_attr;i++)
			db_list.push_back(DbDatum(attr_list[i + base]->get_name()));

		try
		{
			tg->get_database()->get_class_attribute_property(class_name,db_list,tg->get_db_cache());
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream o;
			o << "Can't get class attribute properties for class " << class_name << std::ends;

			Except::re_throw_exception(e,(const char *)API_DatabaseAccess,
				                o.str(),
				                (const char *)"MultiClassAttribute::init_class_attribute");
		}

//
// Sort property for each attribute and create a ClassAttribute object for each of them
//

		long ind = 0;
		for (i = 0;i < nb_attr;i++)
		{
			std::vector<AttrProperty> prop_list;

			std::string attr_name = db_list[ind].name;
			long nb_prop = 0;
			db_list[ind] >> nb_prop;

			ind++;
			for (long j = 0;j < nb_prop;j++)
			{
				if (db_list[ind].size() > 1)
				{
					std::string tmp(db_list[ind].value_string[0]);
					long nb = db_list[ind].size();
					for (int k = 1;k < nb;k++)
					{
						tmp = tmp + " ";
						tmp = tmp + db_list[ind].value_string[k];
					}
					prop_list.push_back(AttrProperty(db_list[ind].name,tmp));
				}
				else
					prop_list.push_back(AttrProperty(db_list[ind].name,db_list[ind].value_string[0]));
				ind++;
			}

			if (nb_prop != 0)
			{
//
// Find this attribute in the attribute list
//

				unsigned int k;
				for (k = 0;k < attr_list.size();k++)
				{
					if (attr_name == attr_list[k]->get_name())
						break;
				}
				if (k == attr_list.size())
				{
					TangoSys_OMemStream o;
					o << "Attribute " << attr_name << " not found in class attribute(s)" << std::ends;

					Except::throw_exception((const char *)API_AttrNotFound,
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
	{
		cout4 << *(attr_list[i + base]) << std::endl;
  	}

	cout4 << "Leaving MultiClassAttribute::init_class_attribute" << std::endl;
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassAttribute::get_attr
//
// description :
//		Get the Attr object for the attribute with name passed as parameter
//
// argument :
// 		in :
//			- attr_name : The attribute name
//
// return :
//		Reference to the ClassAttribute object or throw an exceptionif the attribute is not found
//
//--------------------------------------------------------------------------------------------------------------------


Attr &MultiClassAttribute::get_attr(std::string &attr_name)
{
//
// Search for the wanted attribute in the attr_list vector from its name
//

	std::vector<Attr *>::iterator pos;

	pos = find_if(attr_list.begin(),attr_list.end(),
		      std::bind2nd(WantedClassAttr<Attr *,std::string,bool>(),attr_name));

	if (pos == attr_list.end())
	{
		TangoSys_OMemStream o;
		o << "Attribute " << attr_name << " not found in class attribute(s)" << std::ends;

		Except::throw_exception((const char *)API_AttrOptProp,o.str(),
				      (const char *)"MultiClassAttribute::get_attr");
	}

	return *(*pos);
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassAttribute::remove_attr
//
// description :
//		Remove the Attr object for the attribute with name passed as parameter
//
// argument :
// 		in :
//			- attr_name : The attribute name
//			- cl_name : The attribute class name
//
//--------------------------------------------------------------------------------------------------------------------


void MultiClassAttribute::remove_attr(const std::string &attr_name,const std::string &cl_name)
{
	std::vector<Tango::Attr *>::iterator ite;
	for (ite = attr_list.begin();ite != attr_list.end();++ite)
	{
		if (((*ite)->get_name() == attr_name) && ((*ite)->get_cl_name() == cl_name))
		{
			attr_list.erase(ite);
			break;
		}
	}
}


//+------------------------------------------------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description :
//		Friend function to ease printing instance of the Attr class. It prints all the attribute property(ies) name
// 		and value defined in DB
//
//-------------------------------------------------------------------------------------------------------------------


} // End of Tango namespace

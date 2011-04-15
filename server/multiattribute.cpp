static const char *RcsId = "$Id$\n$Name$";

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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
// $Log$
// Revision 3.28  2010/09/17 08:21:12  taurel
// - Add optimization in MultiAttribute::get_attr_ind_by_name()
//
// Revision 3.27  2010/09/09 13:46:00  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.26  2010/08/25 11:41:27  taurel
// - Fix some bugs preventing dynamic attributes management (in some cases)
//
// Revision 3.25  2010/07/16 10:51:53  taurel
// - Now it's possible to fill the polling buffer externally for R/W attribute
// specifying the attribute written part
//
// Revision 3.24  2010/06/21 14:01:15  taurel
// - Yet another merge with the Release_7_1_1-bugfixes branch
//
// Revision 3.23.2.1  2010/06/16 07:21:07  taurel
// - Fix bug in state command when attribute(s) has alarm level defined
// but their attribute quality is INVALID
//
// Revision 3.23  2009/10/27 16:33:44  taurel
// - Fix a bug in attribute mutex management in case the attribute
// is_allowed() method returns false
//
// Revision 3.22  2009/10/23 14:36:27  taurel
// - Tango 7.1.1
// - Fix bugs 2880372 and 2881841
// - Now support event in case of Tango system with multi db server
// - The polling threads start with polling inactive
//
// Revision 3.21  2009/09/18 09:18:06  taurel
// - End of attribute serialization implementation?
//
// Revision 3.20  2009/01/21 12:47:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.19  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.18  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.17  2008/06/10 07:52:15  taurel
// - Add code for the DevEncoded attribute data type
//
// Revision 3.16  2008/03/11 14:38:25  taurel
// - Apply patches from Frederic Picca about compilation with gcc 4.2
//
// Revision 3.15  2008/01/30 10:28:47  jensmeyer
// Corrected bug in get_w_attr_by_name method.
// The method returned without exception also for non writabe attributes.
//
// Revision 3.14  2007/10/26 11:36:35  taurel
// - Clarify some exception messages
//
// Revision 3.13  2007/10/16 08:23:37  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
// Revision 3.12  2007/04/16 14:57:42  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.11  2007/03/06 08:19:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.10  2007/02/08 16:21:19  taurel
// - Fix a bug in index management in the remove_attribute() method
//
// Revision 3.9  2006/11/09 16:29:44  taurel
// - Fix bug in some index management of the remove_attribute() feature
//
// Revision 3.8  2005/06/29 08:31:18  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.7  2005/01/13 09:27:53  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 3.5.2.6  2004/11/26 13:53:21  taurel
// - Fix bug if exception thrown during Util class construction
// - Change attribute label and format default values
//
// Revision 3.5.2.5  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.5.2.4  2004/10/22 11:26:32  taurel
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
// Revision 3.5.2.3  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.5.2.2  2004/09/15 06:47:16  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.5.2.1  2004/08/19 07:44:59  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.5  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.4  2003/08/22 12:52:55  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.3  2003/08/21 07:24:37  taurel
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
// Revision 3.2.2.4  2004/03/09 16:36:37  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.2.2.3  2004/03/02 07:41:56  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.2.2.2  2004/02/18 15:06:18  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 3.2.2.1  2003/09/30 11:50:43  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.2  2003/05/28 14:55:10  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.1  2003/05/16 08:46:16  taurel
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
// Revision 3.0  2003/03/25 16:44:11  taurel
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
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

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
	{"label",LabelNotSpec},
	{"description",DescNotSpec},
	{"unit",UnitNotSpec},
	{"standard_unit",StdUnitNotSpec},
	{"display_unit",DispUnitNotSpec},
	{"format",FormatNotSpec},
	{"min_value",AlrmValueNotSpec},
	{"max_value",AlrmValueNotSpec},
	{"min_alarm",AlrmValueNotSpec},
	{"max_alarm",AlrmValueNotSpec},
	{"writable_attr_name",AssocWritNotSpec},
	{"min_warning",AlrmValueNotSpec},
	{"max_warning",AlrmValueNotSpec},
	{"delta_t","0"},
	{"delta_val",AlrmValueNotSpec}
};


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
:ext(NULL)
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
// No need to implement
// a retry here (in case of db server restart) because the db reconnection
// is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

	if (nb_attr != 0)
	{
		Tango::Util *tg = Tango::Util::instance();
		Tango::DbData db_list;
		
		if (tg->_UseDb == true)
		{
			for (i = 0;i < nb_attr;i++)
				db_list.push_back(DbDatum(tmp_attr_list[i]->get_name()));
		
			try
			{
				tg->get_database()->get_device_attribute_property(dev_name,db_list,tg->get_db_cache());
			}
			catch (Tango::DevFailed &)
			{		
				TangoSys_OMemStream o;
				o << "Can't get device attribute properties for device " << dev_name << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
				                	o.str(),
				                	(const char *)"MultiAttribute::MultiAttribute");
			}
		}
						
//
// Build property list for each attribute
//

		long ind = 0;
	
		for (i = 0;i < nb_attr;i++)
		{
	
//
// Get attribute class properties
//

			Attr &attr = dev_class_ptr->get_class_attr()->get_attr(tmp_attr_list[i]->get_name());
			vector<AttrProperty> &class_prop = attr.get_class_properties();
			vector<AttrProperty> &def_user_prop = attr.get_user_default_properties();

//
// If the attribute has some properties defined at device level, build a vector
// of these properties
//
		
			vector<AttrProperty> dev_prop;

			if (tg->_UseDb == true)
			{
				long nb_prop;
				db_list[ind] >> nb_prop;
				ind++;

				for (long j = 0;j < nb_prop;j++)
				{
					if (db_list[ind].size() > 1)
					{
						string tmp(db_list[ind].value_string[0]);
						long nb = db_list[ind].size();
						for (int k = 1;k < nb;k++)
						{
							tmp = tmp + " ";
							tmp = tmp + db_list[ind].value_string[k];
						}
						dev_prop.push_back(AttrProperty(db_list[ind].name,tmp));					
					}
					else
						dev_prop.push_back(AttrProperty(db_list[ind].name,
										db_list[ind].value_string[0]));
					ind++;
				}
			}

//
// Concatenate these two attribute properties levels
//
	
			vector<AttrProperty> prop_list;
			concat(dev_prop,class_prop,prop_list);
			add_user_default(prop_list,def_user_prop);
			add_default(prop_list,dev_name,attr.get_name());
		
//
// Create an Attribute instance
//

			if ((attr.get_writable() == Tango::WRITE) ||
			    (attr.get_writable() == Tango::READ_WRITE))
				attr_list.push_back(new WAttribute(prop_list,attr,dev_name,i));
			else
				attr_list.push_back(new Attribute(prop_list,attr,dev_name,i));
		
//
// If it is writable, add it to the writable attribute list
//

			Tango::AttrWriteType w_type = attr_list[i]->get_writable();
			if ((w_type == Tango::WRITE) ||
			    (w_type == Tango::READ_WRITE))
			{
				writable_attr_list.push_back(i);
			}
		
//
// If one of the alarm properties is defined, add it to the alarmed attribute
// list
//

			if (attr_list[i]->is_alarmed().any() == true)
			{
				if (w_type != Tango::WRITE)
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
		check_associated(i,dev_name);
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
	for(unsigned long i = 0;i < attr_list.size();i++)
		delete attr_list[i];

	delete ext;
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::concat
// 
// description : 	Concatenate porperties defined at the class level and
//			at the device level. Prperties defined at the device
//			level have the highest level
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

	unsigned long i;
	for (i = 0;i < dev_prop.size();i++)
		result.push_back(dev_prop[i]);
		
//
// Add class properties if they have not been redefined at the device level
//

	vector<AttrProperty> tmp_result = result;
	unsigned long nb_class_check = class_prop.size();
	for (i = 0;i < nb_class_check;i++)
	{
		vector<AttrProperty>::iterator pos;

		pos = find_if(tmp_result.begin(),tmp_result.end(),
			      bind2nd(WantedProp<AttrProperty,string,bool>(),class_prop[i].get_name()));
		
		if (pos != tmp_result.end())
			continue;
		else
			result.push_back(class_prop[i]);
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
//			dev_name : The device name
//			att_name : The attribute name
//
//--------------------------------------------------------------------------

void MultiAttribute::add_default(vector<AttrProperty> &prop_list,
				 TANGO_UNUSED(string &dev_name),
				 string &att_name)
{

//
// Overwrite the label attribute property in order to be set to the
// attribute name
//

	Tango_OptAttrProp[0].default_value = att_name.c_str();

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
// method : 		MultiAttribute::add_user_default
// 
// description : 	Add default value for optional property if they
//			are not defined
//
// in :			prop_list : The already defined property vector
//			user_default : The user defined default property values
//
//--------------------------------------------------------------------------

void MultiAttribute::add_user_default(vector<AttrProperty> &prop_list,
				      vector<AttrProperty> &user_default)
{

//
// Add default user properties if they have not been defined in the database
//

	long nb_user = user_default.size();
	for (int i = 0;i < nb_user;i++)
	{
		vector<AttrProperty>::iterator pos;

		pos = find_if(prop_list.begin(),prop_list.end(),
			      bind2nd(WantedProp<AttrProperty,string,bool>(),user_default[i].get_name()));
		
		if (pos != prop_list.end())
			continue;
		else
			prop_list.push_back(user_default[i]);
	}
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::check_associated
// 
// description :	Check if the writable_attr_name property is set and
//			in this case, check if the associated attribute exists
//			and is writable. This is necessary only for attribute
//			of the READ_WITH_WRITE or READ_WRITE types 
//
// argument : in : 	- index : The index of the attribute to checked in the
//				  attr vector
//			- dev_name : The device name
//
//--------------------------------------------------------------------------

void MultiAttribute::check_associated(long index,string &dev_name)
{
	if ((attr_list[index]->get_writable() == Tango::READ_WITH_WRITE) ||
	    (attr_list[index]->get_writable() == Tango::READ_WRITE))
	{
		
/*		if (attr_list[index]->get_data_format() != Tango::SCALAR)
		{
			TangoSys_OMemStream o;
				
			o << "Device --> " << dev_name;
			o << "\nProperty writable_attr_name for attribute " << attr_list[index]->get_name();
			o << " is defined but this attribute data format is not SCALAR" << ends;
			Except::throw_exception((const char *)"API_AttrOptProp",
						o.str(),
						(const char *)"MultiAttribute::MultiAttribute");
		}*/
			
		unsigned long j;
		string &assoc_name = attr_list[index]->get_assoc_name();
		transform(assoc_name.begin(),assoc_name.end(),assoc_name.begin(),::tolower);
		for (j = 0;j < writable_attr_list.size();j++)
		{
			if (attr_list[writable_attr_list[j]]->get_name_lower() == assoc_name)
				break;
		}
		if (j == writable_attr_list.size())
		{
			TangoSys_OMemStream o;

			o << "Device --> " << dev_name;		
			o << "\nProperty writable_attr_name for attribute " << attr_list[index]->get_name(); 
			o << " is set to " << assoc_name;
			o << ", but this attribute does not exists or is not writable" << ends;
			Except::throw_exception((const char *)"API_AttrOptProp",
						o.str(),
						(const char *)"MultiAttribute::MultiAttribute");
		}

//
// Also check if the associated write attribute is a scalar one
//
			
/*		if (attr_list[writable_attr_list[j]]->get_data_format() != Tango::SCALAR)
		{
			TangoSys_OMemStream o;
				
			o << "Device --> " << dev_name;		
			o << "\nProperty writable_attr_name for attribute " << attr_list[index]->get_name();
			o << " is set to " << assoc_name;
			o << ", but this attribute is not of the SCALAR data format" << ends;
			Except::throw_exception((const char *)"API_AttrOptProp",
						o.str(),
						(const char *)"MultiAttribute::MultiAttribute");
		}*/

//
// Check that the two associated attributes have the same data type
//
			
		if (attr_list[writable_attr_list[j]]->get_data_type() != attr_list[index]->get_data_type())
		{
			TangoSys_OMemStream o;
				
			o << "Device --> " << dev_name;		
			o << "\nProperty writable_attr_name for attribute " << attr_list[index]->get_name();
			o << " is set to " << assoc_name;
			o << ", but these two attributes do not support the same data type" << ends;
			Except::throw_exception((const char *)"API_AttrOptProp",
						o.str(),
						(const char *)"MultiAttribute::MultiAttribute");
		}
			
		attr_list[index]->set_assoc_ind(writable_attr_list[j]);
	}

}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::add_attribute
// 
// description : 	Construct a new attribute object and add it to the
//			device attribute list
//
// argument : in : 	- dev_name : The device name
//			- dev_class_ptr : Pointer to the DeviceClass object
//			- index : Index in class attribute list of the new
//				  device attribute
//
//--------------------------------------------------------------------------

void MultiAttribute::add_attribute(string &dev_name,
				   DeviceClass *dev_class_ptr,
				   long index)
{
	cout4 << "Entering MultiAttribute::add_attribute" << endl;
	
//
// Retrieve device class attribute list
//

	vector<Attr *> &tmp_attr_list = dev_class_ptr->get_class_attr()->get_attr_list();
	
//
// Get device attribute properties
// No need to implement
// a retry here (in case of db server restart) because the db reconnection
// is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

	Tango::Util *tg = Tango::Util::instance();
	Tango::DbData db_list;
	
	if (tg->_UseDb == true)
	{
		db_list.push_back(DbDatum(tmp_attr_list[index]->get_name()));
		
		try
		{
			tg->get_database()->get_device_attribute_property(dev_name,db_list,tg->get_db_cache());
		}
		catch (Tango::DevFailed &e)
		{
			TangoSys_OMemStream o;
			o << "Can't get device attribute properties for device " << dev_name << ends;
					
			Except::re_throw_exception(e,(const char *)"API_DatabaseAccess",
				       		 o.str(),
				        	(const char *)"MultiAttribute::add_attribute");
		}
	}

	long ind = 0;
	
//
// Get attribute class properties
//

	Attr &attr = dev_class_ptr->get_class_attr()->get_attr(tmp_attr_list[index]->get_name());
	vector<AttrProperty> &class_prop = attr.get_class_properties();
	vector<AttrProperty> &def_user_prop = attr.get_user_default_properties();

//
// If the attribute has some properties defined at device level, build a vector
// of these properties
//
		
	vector<AttrProperty> dev_prop;

	if (tg->_UseDb == true)
	{
		long nb_prop;
		db_list[ind] >> nb_prop;
		ind++;

		for (long j = 0;j < nb_prop;j++)
		{
			if (db_list[ind].size() > 1)
			{
				string tmp(db_list[ind].value_string[0]);
				long nb = db_list[ind].size();
				for (int k = 1;k < nb;k++)
				{
					tmp = tmp + " ";
					tmp = tmp + db_list[ind].value_string[k];
				}
				dev_prop.push_back(AttrProperty(db_list[ind].name,tmp));
			}
			else
				dev_prop.push_back(AttrProperty(db_list[ind].name,
								db_list[ind].value_string[0]));
			ind++;
		}
	}

//
// Concatenate these two attribute properties levels
//
	
	vector<AttrProperty> prop_list;
	concat(dev_prop,class_prop,prop_list);
	add_user_default(prop_list,def_user_prop);
	add_default(prop_list,dev_name,attr.get_name());
		
//
// Create an Attribute instance and insert it in the attribute list
// If the device implement IDL 3 (with state and status as attributes),
// add it at the end of the list but before state and status.
//

	bool idl_3 = false;
	vector<Attribute *>::iterator ite;
	if ((attr_list.back())->get_name() == "Status")
	{
		idl_3 = true;
		ite = attr_list.end();
		ite = ite - 2;
	}
		
	if ((attr.get_writable() == Tango::WRITE) ||
	    (attr.get_writable() == Tango::READ_WRITE))
	{
		if (idl_3 == false)
		{
			attr_list.push_back(new WAttribute(prop_list,attr,dev_name,index));
			index = attr_list.size() - 1;
		}
		else
		{
			attr_list.insert(ite,new WAttribute(prop_list,attr,dev_name,index));
			index = attr_list.size() - 3;
		}
	}
	else
	{
		if (idl_3 == false)
		{
			attr_list.push_back(new Attribute(prop_list,attr,dev_name,index));
			index = attr_list.size() - 1;
		}
		else
		{
			attr_list.insert(ite,new Attribute(prop_list,attr,dev_name,index));
			index = attr_list.size() - 3;
		}	
	}
		
//
// If it is writable, add it to the writable attribute list
//

	Tango::AttrWriteType w_type = attr_list[index]->get_writable();
	if ((w_type == Tango::WRITE) ||
	    (w_type == Tango::READ_WRITE))
	{
		writable_attr_list.push_back(index);
	}
		
//
// If one of the alarm properties is defined, add it to the alarmed attribute
// list
//

	if (attr_list[index]->is_alarmed().any() == true)
	{
		if (w_type != Tango::WRITE)
			alarm_attr_list.push_back(index);
	}

//
// Check if the writable_attr_name property is set and in this
// case, check if the associated attribute exists and is writable
//

	check_associated(index,dev_name);
	
	cout4 << "Leaving MultiAttribute::add_attribute" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::remove_attribute
// 
// description : 	Remove one  attribute object from the
//			device attribute list
//
// argument : in : 	- attr_name : The attribute name
//					- update_idx : Flag set to true if the attributes object index
//								   used to retrieve the corresponding Attr object
//								   has to be updated (because one Attr object
//								   has been removed)
//
//--------------------------------------------------------------------------

void MultiAttribute::remove_attribute(string &attr_name,bool update_idx)
{
	cout4 << "Entering MultiAttribute::remove_attribute" << endl;

//
// Get attribute index in vector
//

	long att_index = get_attr_ind_by_name(attr_name.c_str());
	
//
// Remove the attribute from the main vector
//

	Attribute *att = attr_list[att_index];

	int old_idx = att->get_attr_idx();
	DeviceImpl *the_dev = att->get_att_device();
	string &dev_class_name = the_dev->get_device_class()->get_name();

	delete att;
	vector<Tango::Attribute *>::iterator pos = attr_list.begin();
	advance(pos,att_index);
	pos = attr_list.erase(pos);

//
// Update all the index for attribute following the one which has been deleted
// This is a 2 steps process:
// 1 - Update index in local device
// 2 - Update indexes in remaining device(s) belonging to the same class
//

	if (update_idx == true)
	{	
		for (;pos != attr_list.end();++pos)
			(*pos)->set_attr_idx((*pos)->get_attr_idx() - 1);

		Tango::Util *tg = Tango::Util::instance();
		vector<DeviceImpl *> &dev_list = tg->get_device_list_by_class(dev_class_name);
		
		vector<DeviceImpl *>::iterator dev_ite;
		for (dev_ite = dev_list.begin();dev_ite != dev_list.end();++dev_ite)
		{
			if (*dev_ite == the_dev)
				continue;

			vector<Attribute *> &dev_att_list = (*dev_ite)->get_device_attr()->get_attribute_list();
			for (unsigned int loop = 0;loop < dev_att_list.size();++loop)
			{
				int idx = dev_att_list[loop]->get_attr_idx();
				if (idx > old_idx)
					dev_att_list[loop]->set_attr_idx(idx - 1);							
			}
		}
	}

//
// Clear the writable attribute index vector and rebuild it
// This is necessary because this vector containd index in
// the main attribute list which has been modified
//

	writable_attr_list.clear();
	for (unsigned long i = 0;i < attr_list.size();i++)
	{
		Tango::AttrWriteType w_type = attr_list[i]->get_writable();
		if ((w_type == Tango::WRITE) || (w_type == Tango::READ_WRITE))
		{
			writable_attr_list.push_back(i);
		}
	}
	
//
// Do the same for the alarmed attribute for the same reason
//

	alarm_attr_list.clear();
	for (unsigned long i = 0;i < attr_list.size();i++)
	{
		if (attr_list[i]->is_alarmed().any() == true)
		{
			Tango::AttrWriteType w_type = attr_list[i]->get_writable();
			if (w_type != Tango::WRITE)
				alarm_attr_list.push_back(i);
		}
	}
			
//
// Check the associated attributes
//
			
	string default_dev_name("a/b/c");
	for (unsigned long i = 0;i < attr_list.size();i++)
	{
		check_associated(i,default_dev_name);
	}
	
		
	cout4 << "Leaving MultiAttribute::remove_attribute" << endl;
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
		      
	if ( (    pos == attr_list.end() ) ||
		  ( ((*pos)->get_writable() != Tango::WRITE) &&
		    ((*pos)->get_writable() != Tango::READ_WRITE) ) )
	{
		cout3 << "MultiAttribute::get_w_attr_by_name throwing exception" << endl;
		TangoSys_OMemStream o;
		
		o << attr_name << " writable attribute not found" << ends;
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
	transform(st.begin(),st.end(),st.begin(),::tolower);

	for (i = 0;i < nb_attr;i++)
	{
		if (attr_list[i]->get_name_size() != st.size())
			continue;
		if (attr_list[i]->get_name_lower() == st)
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
	unsigned long i;
	bool ret,tmp_ret;

	tmp_ret = false;
	ret = false;	
	for (i = 0;i < alarm_attr_list.size();i++)
	{
		Tango::AttrQuality qua = (get_attr_by_ind(alarm_attr_list[i])).get_quality();
		if (ret == false)
		{
			if (qua == Tango::ATTR_ALARM)
				ret = true;
		}
		if (qua != Tango::ATTR_INVALID)
		{
			tmp_ret = check_alarm(alarm_attr_list[i]);
			if (tmp_ret == true)
				ret = true;
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
	unsigned long i;

	for (i = 0;i < alarm_attr_list.size();i++)
	{
		Attribute &att = get_attr_by_ind(alarm_attr_list[i]);

//
// Add a message for low level alarm
//

		if (att.is_min_alarm() == true)
		{
			string &attr_label = att.get_label();
			string str;
			if (attr_label == LabelNotSpec)
			{
				str = "\nAlarm : Value too low for attribute ";
				str = str + att.get_name();
			}
			else
			{
				str = "\nAlarm : Value too low for ";
				str = str + attr_label;
			}				
			status = status + str;
		}

//
// Add a message for high level alarm
//
			
		else if (att.is_max_alarm() == true)
		{
			string &attr_label = att.get_label();
			string str;
			if (attr_label == LabelNotSpec)
			{
				str = "\nAlarm : Value too high for attribute ";
				str = str + att.get_name();
			}
			else
			{
				str = "\nAlarm : Value too high for ";
				str = str + attr_label;
			}				
			status = status + str;
		}
		
//
// Add a message for rds alarm
//

		if (att.is_rds_alarm() == true)
		{
			string &attr_label = att.get_label();
			string str;
			if (attr_label == LabelNotSpec)
			{
				str = "\nAlarm : Read too Different than Set (RDS) for attribute ";
				str = str + att.get_name();
			}
			else
			{
				str = "\nAlarm : Read too Different than Set (RDS) for ";
				str = str + attr_label;
			}				
			status = status + str;
		}
	
//
// Add a message for min warning
//

		if (att.is_min_warning() == true)
		{
			string &attr_label = att.get_label();
			string str;
			if (attr_label == LabelNotSpec)
			{
				str = "\nWarning : Value too low for attribute ";
				str = str + att.get_name();
			}
			else
			{
				str = "\nWarning : Value too low for ";
				str = str + attr_label;
			}				
			status = status + str;
		}
	
//
// Add a message for max warning
//

		else if (att.is_max_warning() == true)
		{
			string &attr_label = att.get_label();
			string str;
			if (attr_label == LabelNotSpec)
			{
				str = "\nWarning : Value too high for attribute ";
				str = str + att.get_name();
			}
			else
			{
				str = "\nWarning : Value too high for ";
				str = str + attr_label;
			}				
			status = status + str;
		}

	}
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::get_event_param
// 
// description : 	Return event info for each attribute with events
//			subscribed
//
// in :			eve : One structure in this vector for each attribute
//			with events subscribed
//
//--------------------------------------------------------------------------

void MultiAttribute::get_event_param(vector<EventPar> &eve)
{
	unsigned int i;
	
	for (i = 0;i < attr_list.size();i++)
	{
		bool once_more = false;
		bool ch = false;
		bool ar = false;
		bool qu = false;
		bool pe = false;
		bool us = false;
		
		if (attr_list[i]->change_event_subscribed() == true)
		{
			once_more = true;
			ch = true;
		}
		
		if (attr_list[i]->quality_event_subscribed() == true)
		{
			once_more = true;
			qu = true;
		}
		
		if (attr_list[i]->periodic_event_subscribed() == true)
		{
			once_more = true;
			pe = true;
		}
		
		if (attr_list[i]->archive_event_subscribed() == true)
		{
			once_more = true;
			ar = true;
		}
		
		if (attr_list[i]->user_event_subscribed() == true)
		{
			once_more = true;
			us = true;
		}
		
		if (once_more == true)
		{
			EventPar ep;
			ep.attr_id = i;
			ep.change = ch;
			ep.quality = qu;
			ep.archive = ar;
			ep.periodic = pe;
			ep.user = us;
			
			eve.push_back(ep);
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

	Tango::DevVarShortArray *sh_write_val;
	Tango::DevVarLongArray *lg_write_val;
	Tango::DevVarLong64Array *lg64_write_val;
	Tango::DevVarDoubleArray *db_write_val;
	Tango::DevVarStringArray *str_write_val;
	Tango::DevVarFloatArray *fl_write_val;
	Tango::DevVarBooleanArray *boo_write_val;
	Tango::DevVarUShortArray *ush_write_val;
	Tango::DevVarCharArray *uch_write_val;
	Tango::DevVarULongArray *ulg_write_val;
	Tango::DevVarULong64Array *ulg64_write_val;
	Tango::DevVarStateArray *state_write_val;
		
	switch (att.get_data_type())
	{
	case Tango::DEV_SHORT :
		sh_write_val = assoc_att.get_last_written_sh();		
		att.add_write_value(sh_write_val);
		break;
		
	case Tango::DEV_LONG :
		lg_write_val = assoc_att.get_last_written_lg();
		att.add_write_value(lg_write_val);
		break;
		
	case Tango::DEV_LONG64 :
		lg64_write_val = assoc_att.get_last_written_lg64();
		att.add_write_value(lg64_write_val);
		break;
		
	case Tango::DEV_DOUBLE :
		db_write_val = assoc_att.get_last_written_db();		
		att.add_write_value(db_write_val);
		break;
		
	case Tango::DEV_STRING :
		str_write_val = assoc_att.get_last_written_str();
		att.add_write_value(str_write_val);
		break;
		
	case Tango::DEV_FLOAT :
		fl_write_val = assoc_att.get_last_written_fl();		
		att.add_write_value(fl_write_val);
		break;
		
	case Tango::DEV_BOOLEAN :
		boo_write_val = assoc_att.get_last_written_boo();
		att.add_write_value(boo_write_val);
		break;
		
	case Tango::DEV_USHORT :
		ush_write_val = assoc_att.get_last_written_ush();		
		att.add_write_value(ush_write_val);
		break;
		
	case Tango::DEV_UCHAR :
		uch_write_val = assoc_att.get_last_written_uch();		
		att.add_write_value(uch_write_val);
		break;
		
	case Tango::DEV_ULONG :
		ulg_write_val = assoc_att.get_last_written_ulg();
		att.add_write_value(ulg_write_val);
		break;
		
	case Tango::DEV_ULONG64 :
		ulg64_write_val = assoc_att.get_last_written_ulg64();
		att.add_write_value(ulg64_write_val);
		break;
		
	case Tango::DEV_STATE :
		state_write_val = assoc_att.get_last_written_state();
		att.add_write_value(state_write_val);
		break;
		
	case Tango::DEV_ENCODED :
		{
			DevEncoded &enc_write_val = assoc_att.get_last_written_encoded();
			att.add_write_value(enc_write_val);
			break;
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::is_att_quality_alarmed()
// 
// description : 	Check for all attribute if one of them has its
//			quality factor set to ALARM.
//			Returns true in this case. Otherwise, returns false
//
// in :			all_att : Flag set to false if the search must exclude
//				  all attributes with alarm levels set
//
//--------------------------------------------------------------------------

bool MultiAttribute::is_att_quality_alarmed(bool all_att)
{
	unsigned long i,j;
	bool ret;

	ret = false;
		
	for (i = 0;i < attr_list.size();i++)
	{
		if (all_att == false)
		{
			bool found = false;
			for (j = 0;j < alarm_attr_list.size();j++)
			{
				if (alarm_attr_list[j] == (long)i)
				{
					found = true;
					break;
				}
			}
			if (found == true)
				continue;
		}
		
		if ((attr_list[i]->get_quality() == Tango::ATTR_ALARM) ||
		    (attr_list[i]->get_quality() == Tango::ATTR_WARNING))
		{
			ret = true;
			break;
		}
	}
	
	return ret;
}


//+-------------------------------------------------------------------------
//
// method : 		MultiAttribute::add_alarmed_quality_factor()
// 
// description : 	Add to the status string name of attributes with
//			a quality factor set to alarm
//
//--------------------------------------------------------------------------

void MultiAttribute::add_alarmed_quality_factor(string &status)
{
	unsigned long i,j;
	bool ret;

	ret = false;
		
	for (i = 0;i < attr_list.size();i++)
	{
		bool found = false;
		for (j = 0;j < alarm_attr_list.size();j++)
		{
			if (alarm_attr_list[j] == (long)i)
			{
				found = true;
				break;
			}
		}
		if (found == true)
			continue;
		
		if (attr_list[i]->get_quality() == Tango::ATTR_ALARM)
		{
			string &attr_label = attr_list[i]->get_label();
			string str("\nAlarm : Quality factor set to ALARM for attribute ");

			if (attr_label == LabelNotSpec)
			{
				str = str + attr_list[i]->get_name();
			}
			else
			{
				str = str + attr_label;
			}				
			status = status + str;
		}
		else if (attr_list[i]->get_quality() == Tango::ATTR_WARNING)
		{
			string &attr_label = attr_list[i]->get_label();
			string str("\nWarning : Quality factor set to WARNING for attribute ");

			if (attr_label == LabelNotSpec)
			{
				str = str + attr_list[i]->get_name();
			}
			else
			{
				str = str + attr_label;
			}				
			status = status + str;
		}
	}
	
}

} // End of Tango namespace

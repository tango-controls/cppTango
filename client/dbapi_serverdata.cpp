static const char *RcsId = "$Id$";

//+===================================================================================================================
//
// file :			dbapi_serverdata.cpp
//
// description :	C++ source code for the DbServerData class
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-===================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

using namespace CORBA;

namespace Tango
{

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::DbServerData
//
// description:
//		constructor for the DbServerData class. This class data stotred in the database related to a device server
//		process. It allows easy insert/remove of one device server process from one database to another
//
// argument:
// 		in :
//			exec_name : The device server process exec name
//			inst_name : The device server process instance name
//
//--------------------------------------------------------------------------------------------------------------------

DbServerData::DbServerData(const string &exec_name,const string &inst_name)
{
	full_server_name = exec_name + '/' + inst_name;

//
//  Check if server defined in database
//

	string adm_name("dserver/");
	adm_name = adm_name + full_server_name;
	Database *db_ptr = NULL;

	try
	{
		DeviceProxy adm(adm_name);
		ApiUtil *au = ApiUtil::instance();
		int ind = au->get_db_ind(adm.get_db_host(),adm.get_db_port_num());
		db_ptr = au->get_db_vect()[ind];
	}
	catch (Tango::DevFailed &e)
	{
		stringstream ss;
		ss << "Device server process " << full_server_name << " is not defined in database";
		Tango::Except::throw_exception("DBServerNotDefinedInDb",ss.str(),"DbServerData::DbServerData");
	}

//
//  Build Server classes
//

	TangoClass tc("DServer",full_server_name,db_ptr);
	classes.push_back(tc);

	DbDatum svr_class_list = db_ptr->get_server_class_list(full_server_name);
	vector<string> vs;
	svr_class_list >> vs;

	for (size_t loop = 0;loop < vs.size();loop++)
	{
		TangoClass tc(vs[loop],full_server_name,db_ptr);
		classes.push_back(tc);
	}

}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::put_in_database
//
// description:
//		Store all the data related to the device server process in the database specified by the input arg.
//
// argument:
// 		in :
//			tg_host : The tango host for the new database
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::put_in_database(const string &tg_host)
{
//
// Check tango host syntax
//

	string::size_type pos;
	if ((pos = tg_host.find(':')) == string::npos)
	{
		stringstream ss;
		ss << tg_host << " is not a valid synatx for Tango host (host:port)";
		Tango::Except::throw_exception("DBWrongTangoHostSyntax",ss.str(),"DbServerData::put_in_database");
	}

//
// Create db object
//

	string db_host = tg_host.substr(0,pos);
	string db_port_str = tg_host.substr(pos + 1);
	stringstream ss;
	ss << db_port_str;
	int db_port;
	ss >> db_port;

	Database *db_ptr = new Database(db_host,db_port);

//
// Create server in DB and put associated properties
//

	create_server(db_ptr);
	put_properties(db_ptr);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::already_exist
//
// description:
//		Check if any of the device server process device(s) is already defined in the database specified by the
//		tango host given as input arg
//
// argument:
// 		in :
//			tg_host : The tango host for the database
//
// return:
//		True in case any of the device is already known. False otherwise
//
//--------------------------------------------------------------------------------------------------------------------


bool DbServerData::already_exist(const string &tg_host)
{

//
// Check tango host syntax
//

	if (tg_host.find(':') == string::npos)
	{
		stringstream ss;
		ss << tg_host << " is not a valid synatx for Tango host (host:port)";
		Tango::Except::throw_exception("DBWrongTangoHostSyntax",ss.str(),"DbServerData::already_exist");
	}

	string header("tango://");
	header = header + tg_host + '/';

//
// Check if server is defined and immediately returned if it is defined
//

	try
	{
		string adm_name(header);
		adm_name = adm_name + "dserver/" + full_server_name;
		DeviceProxy adm(adm_name);

		return true;
	}
	catch (Tango::DevFailed &) {}

//
//  Check if at least one device already defined
//

	for (size_t loop = 0;loop < classes.size();loop++)
	{
		for (size_t ctr = 0;ctr < classes[loop].size();ctr++)
		{
			string dev_name = header + classes[loop][ctr].name;

			try
			{
				DeviceProxy dev(dev_name);
				return true;
			}
			catch (Tango::DevFailed &e)
			{
				string desc(e.errors[0].desc.in());
				if (desc.find("not defined in the database") == string::npos)
				{
					stringstream ss;
					ss << "Failed to check " << dev_name << " in Tango host " << tg_host << endl;
					Tango::Except::re_throw_exception(e,"DBFailedToCheck",ss.str(),"DbServerData::already_exist");
				}
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::create_server
//
// description:
//		Create the device server process with all its devices in the database given as input arg
//
// argument:
// 		in :
//			db_ptr : Ptr to the database object
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::create_server(Database *db_ptr)
{
	DbDevInfos db_infs;
	for (size_t loop = 0;loop < classes.size();loop++)
	{
		for (size_t lo = 0;lo < classes[loop].size();lo++)
		{
			DbDevInfo ddi;
			ddi.name = classes[loop][lo].name;
			ddi._class = classes[loop].name;
			ddi.server = full_server_name;
			db_infs.push_back(ddi);
		}
	}

	db_ptr->add_server(full_server_name,db_infs);
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::put_properties
//
// description:
//		Store all properties (for classes and devices) in the database specified by the input arg
//
// argument:
// 		in :
//			db_ptr : Ptr to the database object
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::put_properties(Database *db_ptr)
{
	for (size_t loop = 0;loop < classes.size();loop++)
	{
		classes[loop].put_properties(db_ptr);
		classes[loop].put_attribute_properties(db_ptr);
		for (size_t ctr = 0;ctr < classes[loop].size();ctr++)
		{
			classes[loop][ctr].put_properties(db_ptr);
			classes[loop][ctr].put_attribute_properties(db_ptr);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::remove
//
// description:
//		Remove device server process from a database
//
// argument:
// 		in :
//			tg_host : The tango host for the new database
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::remove(const string &tg_host)
{

//
// Check tango host syntax
//

	string::size_type pos;
	if ((pos = tg_host.find(':')) == string::npos)
	{
		stringstream ss;
		ss << tg_host << " is not a valid synatx for Tango host (host:port)";
		Tango::Except::throw_exception("DBWrongTangoHostSyntax",ss.str(),"DbServerData::remove");
	}

//
// Create db object
//

	string db_host = tg_host.substr(0,pos);
	string db_port_str = tg_host.substr(pos + 1);
	stringstream ss;
	ss << db_port_str;
	int db_port;
	ss >> db_port;

	Database *db_ptr = new Database(db_host,db_port);

	try
	{

//
// Delete devices
//

		for (size_t loop = classes.size();loop != 0;loop--)
		{
			for (size_t ctr = 0;ctr < classes[loop - 1].size();ctr++)
			{
				db_ptr->delete_device(classes[loop - 1][ctr].name);
			}
		}

//
// Also delete class properties if this is the last class instance in DB
// Forget DServer class
//

		for (size_t loop = 1;loop < classes.size();loop++)
		{
			string all("*");
			DbDatum dev_list = db_ptr->get_device_name(all,classes[loop].name);
			if (dev_list.value_string.empty() == true)
			{
				classes[loop].remove_properties(db_ptr);
			}
		}
	}
	catch(Tango::DevFailed &)
	{
		delete db_ptr;
		throw;
	}

	delete db_ptr;
}

void DbServerData::remove()
{

//
// Get from one device the tango host
//

	string &db_host = classes[0][0].get_db_host();
	string &db_port = classes[0][0].get_db_port();

	string tg_host = db_host + ":" + db_port;

	remove(tg_host);
}



//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoDevice::TangoDevice
//
// description:
//		Constructor for the DbServerData inner structure TangoDevice. There is one instance of this struc for
//		every device defined in the device server process
//
// argument:
// 		in :
//			na : The device name
//
//--------------------------------------------------------------------------------------------------------------------

DbServerData::TangoDevice::TangoDevice(string &na):DeviceProxy(na),name(na)
{
//
//  First read device properties
//

	vector<string> prop_list;
	get_property_list("*",prop_list);

	if (prop_list.empty() == false)
	{
		DbData prop_val;
		get_property(prop_list,prop_val);

		for (size_t loop = 0; loop < prop_list.size();loop++)
		{
			if (prop_val[loop].is_empty() == false)
			{
				TangoProperty tp(prop_list[loop],prop_val[loop].value_string);
				properties.push_back(tp);
			}
		}
	}

//
// Then, get attribute list from db
//

	ApiUtil *au = ApiUtil::instance();
	int ind = au->get_db_ind(get_db_host(),get_db_port_num());
	Database *db = au->get_db_vect()[ind];

	vector<string> att_list;
	db->get_device_attribute_list(name,att_list);

	for (size_t loop = 0; loop < att_list.size();loop++)
	{
		DbData db_data;
		db->get_device_attribute_property(att_list[loop],db_data);
		for (size_t lo = 0;lo < db_data.size();lo++)
		{
			if (db_data[lo].is_empty() == false)
			{
				TangoAttribute ta(att_list[loop]);
				ta.push_back(TangoProperty(db_data[loop].name,db_data[loop].value_string));
				attributes.push_back(ta);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoDevice::put_properties
//
// description:
//		Store in database all the properties belonging to the device
//
// argument:
// 		in :
//			db_ptr : Pointer to the database instance
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::TangoDevice::put_properties(Database *db_ptr)
{
	if (properties.empty() == false)
	{
		DbData db_data;
		for (size_t ctr = 0;ctr < properties.size();ctr++)
		{
			DbDatum prop(properties[ctr].name);
			prop.value_string = properties[ctr].values;
			db_data.push_back(prop);
		}
		db_ptr->put_device_property(name,db_data);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoDevice::put_attribute_properties
//
// description:
//		Store in database all the properties belonging to the device attribute(s)
//
// argument:
// 		in :
//			db_ptr : Pointer to the database instance
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::TangoDevice::put_attribute_properties(Database *db_ptr)
{
	if (attributes.empty() == false)
	{
		DbData db_data;
		for (size_t loop = 0;loop < attributes.size();loop++)
		{
			if (attributes[loop].empty() == false)
			{
				DbDatum db_d(attributes[loop].name);
				db_d << (DevLong)attributes[loop].size();
				db_data.push_back(db_d);
				for (size_t ctr = 0;ctr < attributes[loop].size();ctr++)
				{
					DbDatum db(attributes[loop][ctr].name);
					db.value_string = attributes[loop][ctr].values;
					db_data.push_back(db);
				}
			}
		}

		db_ptr->put_device_attribute_property(name,db_data);
	}
}


//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoClass::TangoClass
//
// description:
//		Constructor for the DbServerData inner structure TangoClass. There is one instance of this struc for
//		every tango class defined in the device server process
//
// argument:
// 		in :
//			- na : The Tango class name
//			- full_ds_name : The device server process name
//			- db : Pointer to the database instance
//
//--------------------------------------------------------------------------------------------------------------------

DbServerData::TangoClass::TangoClass(const string &na,const string &full_ds_name,Database *db):name(na)
{

//
// First, read class properties
//

	DbDatum cl_prop = db->get_class_property_list(name);
	if (cl_prop.is_empty() == false)
	{
		vector<string> vs;
		cl_prop >> vs;

		DbData db_data;
		for (size_t loop = 0;loop < vs.size();loop++)
			db_data.push_back(DbDatum(vs[loop]));
		db->get_class_property(name,db_data);

		for (size_t loop = 0;loop < vs.size();loop++)
		{
			TangoProperty tp(db_data[loop].name,db_data[loop].value_string);
			properties.push_back(tp);
		}
	}

//
//  Get class attribute list
//

	string all("*");
	DbDatum cl_att_list = db->get_class_attribute_list(name,all);
	if (cl_att_list.is_empty() == false)
	{
		vector<string> vs;
		cl_att_list >> vs;

		for (size_t loop = 0;loop < vs.size();loop++)
		{
			DbData db_data;
			db->get_class_attribute_property(vs[loop],db_data);
			for (size_t lo = 0;lo < db_data.size();lo++)
			{
				TangoAttribute ta(vs[loop]);
				ta.push_back(TangoProperty(db_data[lo].name,db_data[lo].value_string));
				attributes.push_back(ta);
			}
		}
	}

//
// Get all devices for this class
//

	DbDatum dev_names = db->get_device_name(const_cast<string &>(full_ds_name),name);

	for (size_t loop = 0;loop < dev_names.value_string.size();loop++)
	{
		TangoDevice td(dev_names.value_string[loop]);
		push_back(td);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoClass::put_properties
//
// description:
//		Store in database all the class properties
//
// argument:
// 		in :
//			- db_ptr : Pointer to the database instance
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::TangoClass::put_properties(Database *db_ptr)
{
	if (properties.empty() == false)
	{
		DbData db_data;
		for (size_t ctr = 0;ctr < properties.size();ctr++)
		{
			DbDatum prop(properties[ctr].name);
			prop.value_string = properties[ctr].values;
			db_data.push_back(prop);
		}
		db_ptr->put_class_property(name,db_data);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoClass::put_attribute_properties
//
// description:
//		Store in database all the class attribute properties
//
// argument:
// 		in :
//			- db_ptr : Pointer to the database instance
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::TangoClass::put_attribute_properties(Database *db_ptr)
{
	if (attributes.empty() == false)
	{
		DbData db_data;
		for (size_t loop = 0;loop < attributes.size();loop++)
		{
			if (attributes[loop].empty() == false)
			{
				DbDatum db_d(attributes[loop].name);
				db_d << (DevLong)attributes[loop].size();
				db_data.push_back(db_d);
				for (size_t ctr = 0;ctr < attributes[loop].size();ctr++)
				{
					DbDatum db(attributes[loop][ctr].name);
					db.value_string = attributes[loop][ctr].values;
					db_data.push_back(db);
				}
			}
		}

		db_ptr->put_class_attribute_property(name,db_data);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbServerData::TangoClass::remove_properties
//
// description:
//		Remove from the database all the class properties
//
// argument:
// 		in :
//			- db_ptr : Pointer to the database instance
//
//--------------------------------------------------------------------------------------------------------------------

void DbServerData::TangoClass::remove_properties(Database *db_ptr)
{
//
// Delete class properties if any
//

	if (properties.empty() == false)
	{
		DbData props;
		for (size_t loop = 0;loop < properties.size();loop++)
			props.push_back(DbDatum(properties[loop].name));

		db_ptr->delete_class_property(name,props);
	}

//
// Delete class attribute properties if any
//

	if (attributes.empty() == false)
	{
		for (size_t loop = 0;loop < attributes.size();loop++)
		{
			if (attributes[loop].empty() == false)
			{
				DbData db_data;
				db_data.push_back(DbDatum(attributes[loop].name));
				for (size_t ctr = 0;ctr < attributes[loop].size();ctr++)
					db_data.push_back(DbDatum(attributes[loop][ctr].name));
				db_ptr->delete_class_attribute_property(name,db_data);
			}
		}
	}
}

} // End of Tango namespace

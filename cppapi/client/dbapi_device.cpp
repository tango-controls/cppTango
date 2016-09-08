static const char *RcsId = "$Id$\n$Name$";

//===================================================================================================================
//
// dbdevice.cpp - C++ source code file for TANGO dbapi class DbDevice
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- October 2000
//
// Copyright (C) :      2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//===================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>


using namespace CORBA;

namespace Tango
{

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::DbDevice()
//
//	description :
//		Constructor to create a DbDevice object for accessing a device of this name in the specified TANGO database
//		(import/export info and properties)
//
//------------------------------------------------------------------------------------------------------------------

DbDevice::DbDevice(string &dev_name, Database *dev_dbase):ext(Tango_nullptr)
{
	name = dev_name;
	dbase = dev_dbase;
	ext_dbase = true;
}

//------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::DbDevice()
//
//	description :
//		Constructor to create a DbDevice object for accessing a device of this name without specifying the
//		TANGO database.
//
//-----------------------------------------------------------------------------------------------------------------

DbDevice::DbDevice(string &dev_name):ext(Tango_nullptr)
{
	name = dev_name;
	db_ind = ApiUtil::instance()->get_db_ind();
	ext_dbase = false;
}

//-----------------------------------------------------------------------------------------------------------------
//
//	method:
//		DbDevice::DbDevice()
//
//	description :
//		Constructor to create a DbDevice object for accessing a device of this name with specifying the TANGO
//		database via its server host and port
//
//------------------------------------------------------------------------------------------------------------------

DbDevice::DbDevice(string &dev_name,string &host,string &port_str):ext(Tango_nullptr)
{
	name = dev_name;

	TangoSys_MemStream s;
	int port_num;
	s << port_str << ends;
	s >> port_num;

	db_ind = ApiUtil::instance()->get_db_ind(host,port_num);
	ext_dbase = false;
}

//------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::~DbDevice()
//
//	description :
//		Destructor to destroy a DbDevice object
//
//------------------------------------------------------------------------------------------------------------------

DbDevice::~DbDevice()
{
}


//------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::get_dbase()
//
//	description :
//		Public method to return the database used by this db device.
//
//------------------------------------------------------------------------------------------------------------------

Database *DbDevice::get_dbase()
{
	if (ext_dbase == true)
		return dbase;
	else
	{
		ApiUtil *au = ApiUtil::instance();
		return (au->get_db_vect())[db_ind];
	}
}


//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::import()
//
//	description :
//		Public method to return import info for a device
//
//------------------------------------------------------------------------------------------------------------------

DbDevImportInfo DbDevice::import_device()
{
	if (ext_dbase == true)
		return dbase->import_device(name);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		return (au->get_db_vect())[db_ind]->import_device(name);
	}
}

//-------------------------------------------------------------------------------------------------------------------
//
// 	method :
// 		DbDevice::export_device()
//
//	description :
//		Public method to export device to the database
//
//-------------------------------------------------------------------------------------------------------------------

void DbDevice::export_device(DbDevExportInfo &dev_export)
{
	if (ext_dbase == true)
		dbase->export_device(dev_export);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->export_device(dev_export);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::get_property()
//
// 	description :
//		Public method to get device properties from the database
//
//------------------------------------------------------------------------------------------------------------------

void DbDevice::get_property(DbData &db_data)
{
	ApiUtil *au = ApiUtil::instance();
	DbServerCache *dsc;
	if (au->in_server() == true)
	{
		Tango::Util *tg = Tango::Util::instance();
		dsc = tg->get_db_cache();
	}
	else
		dsc = NULL;

	if (ext_dbase == true)
		dbase->get_device_property(name, db_data, dsc);
	else
	{
		(au->get_db_vect())[db_ind]->get_device_property(name,db_data);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::put_property()
//
//	description :
//		Public method to put device properties from the database
//
//-------------------------------------------------------------------------------------------------------------------

void DbDevice::put_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_device_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_device_property(name,db_data);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::delete_property()
//
//	description :
//		Public method to delete device properties from the database
//
//-------------------------------------------------------------------------------------------------------------------

void DbDevice::delete_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_device_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_device_property(name,db_data);
	}
	return;
}

//------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::get_attribute_property()
//
//	description:
//		Public method to get device attribute properties from the database
//
//------------------------------------------------------------------------------------------------------------------

void DbDevice::get_attribute_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->get_device_attribute_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->get_device_attribute_property(name,db_data);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::put_attribute_property()
//
//	description :
//		Public method to put device attribute properties from the database
//
//-------------------------------------------------------------------------------------------------------------------

void DbDevice::put_attribute_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_device_attribute_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_device_attribute_property(name,db_data);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::delete_attribute_property()
//
//	description :
//		Public method to delete device attribute properties from the database
//
//------------------------------------------------------------------------------------------------------------------

void DbDevice::delete_attribute_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_device_attribute_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_device_attribute_property(name,db_data);
	}
	return;
}

//------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::get_pipe_property()
//
//	description:
//		Public method to get device pipe properties from the database
//
//------------------------------------------------------------------------------------------------------------------

void DbDevice::get_pipe_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->get_device_pipe_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->get_device_pipe_property(name,db_data);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::put_pipe_property()
//
//	description :
//		Public method to put device pipe properties from the database
//
//-------------------------------------------------------------------------------------------------------------------

void DbDevice::put_pipe_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_device_pipe_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_device_pipe_property(name,db_data);
	}
	return;
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::delete_pipe_property()
//
//	description :
//		Public method to delete device pipe properties from the database
//
//------------------------------------------------------------------------------------------------------------------

void DbDevice::delete_pipe_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_device_pipe_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_device_pipe_property(name,db_data);
	}
	return;
}

//--------------------------------------------------------------------------------------------------------------------
//
//	method :
//	 	DbDevice::check_access_control()
//
//	description :
//		Public method to return device access right
//
//-------------------------------------------------------------------------------------------------------------------

AccessControlType DbDevice::check_access_control()
{
	if (ext_dbase == true)
		return dbase->check_access_control(name);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		Database *db = au->get_db_vect()[db_ind];
		AccessControlType acc = db->check_access_control(name);
		return acc;
	}
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::clear_access_except_errors()
//
//	description :
//		Public method to clear memorized access control exception
//
//------------------------------------------------------------------------------------------------------------------

void DbDevice::clear_access_except_errors()
{
	if (ext_dbase == true)
		dbase->clear_access_except_errors();
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->clear_access_except_errors();
	}
}

//-------------------------------------------------------------------------------------------------------------------
//
//	method :
// 		DbDevice::get_property_list()
//
//	description :
//		Public method to get device properties list from the database
//
//-------------------------------------------------------------------------------------------------------------------

void DbDevice::get_property_list(const string &wildcard,vector<string> &prop_list)
{
	ApiUtil *au = ApiUtil::instance();
	DbServerCache *dsc;
	if (au->in_server() == true)
	{
		Tango::Util *tg = Tango::Util::instance();
		dsc = tg->get_db_cache();
	}
	else
		dsc = NULL;

	if (ext_dbase == true)
	{
		dbase->get_device_property_list(name, wildcard, prop_list, dsc);
	}
	else
	{
		(au->get_db_vect())[db_ind]->get_device_property_list(name, wildcard, prop_list);
	}
	return;
}


} // End of Tango namespace

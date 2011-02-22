static const char *RcsId = "$Id$\n$Name$";
//
// dbdevice.cpp - C++ source code file for TANGO dbapi class DbDevice
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- October 2000
//
// Copyright (C) :      2000,2001,2002,2003,2004,2005,2006,2007,2008,2009
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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

                                                   
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DbDevice::DbDevice() - constructor to create a DbDevice object for
//			  accessing a device of this name in the specified
//			  TANGO database (import/export info and properties) 
//
//-----------------------------------------------------------------------------

DbDevice::DbDevice(string &dev_name, Database *dev_dbase)
{
	name = dev_name;
	dbase = dev_dbase;
	ext_dbase = true;
}

//-----------------------------------------------------------------------------
//
// DbDevice::DbDevice() - constructor to create a DbDevice object for
//			  accessing a device of this name without specifying
//			  the TANGO database.
//
//-----------------------------------------------------------------------------

DbDevice::DbDevice(string &dev_name)
{
	name = dev_name;
	db_ind = ApiUtil::instance()->get_db_ind();
	ext_dbase = false;
}

//-----------------------------------------------------------------------------
//
// DbDevice::DbDevice() - constructor to create a DbDevice object for
//			  accessing a device of this name with specifying
//			  the TANGO database via its server host and port
//
//-----------------------------------------------------------------------------

DbDevice::DbDevice(string &dev_name,string &host,string &port_str)
{
	name = dev_name;
	
	TangoSys_MemStream s;
	int port_num;
	s << port_str << ends;
	s >> port_num;
	
	db_ind = ApiUtil::instance()->get_db_ind(host,port_num);
	ext_dbase = false;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// DbDevice::~DbDevice() - destructor to destroy a DbDevice object
//
//-----------------------------------------------------------------------------

DbDevice::~DbDevice()
{
}


//-----------------------------------------------------------------------------
//
// DbDevice::get_dbase() - public method to return the database used by this
//			   db device.
//
//-----------------------------------------------------------------------------

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


//-----------------------------------------------------------------------------
//
// DbDevice::import() - public method to return import info for a device
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::export_device() - public method to export device to the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::get_property() - public method to get device properties from the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::put_property() - public method to put device properties from the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::delete_property() - public method to delete device properties from the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::get_attribute_property() - public method to get device attribute 
//           properties from the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::put_attribute_property() - public method to put device attribute
//           properties from the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::delete_attribute_property() - public method to delete device attribute
//           properties from the database
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::check_access_control() - public method to return device access right
//
//-----------------------------------------------------------------------------

AccessControlType DbDevice::check_access_control()
{
	if (ext_dbase == true)
		return dbase->check_access_control(name);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		AccessControlType acc = (au->get_db_vect()[db_ind])->check_access_control(name);
		(au->get_db_vect()[db_ind])->set_access_checked(true);
		return acc;
	}
}

//-----------------------------------------------------------------------------
//
// DbDevice::clear_access_except_errors() - public method to clear memorized
// access control exception
//
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//
// DbDevice::get_property_list() - public method to get device properties list
// from the database
//
//-----------------------------------------------------------------------------

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

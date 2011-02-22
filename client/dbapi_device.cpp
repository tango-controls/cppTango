static const char *RcsId = "$Id$\n$Name$";
//
// dbdevice.cpp - C++ source code file for TANGO dbapi class DbDevice
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- October 2000
//
// last changed	- 17/10/2000 
//
// version 	- 1.0
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
	if (ext_dbase == true)
		dbase->get_device_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
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

} // End of Tango namespace

static const char *RcsId = "$Header$";
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

#ifdef _HPUX_SOURCE
#include <iostream.h>
#else
#include <iostream>
#endif /* _HPUX_SOURCE */
#ifdef STRSTREAM
#ifdef _HPUX_SOURCE
#include <strstream.h>
#else
#include <strstream>
#endif /* _HPUX_SOURCE */
#else
#include <sstream>
#endif /* STRSTREAM */
#include <tango.h>
#include <dbapi.h>
#ifdef _HPUX_SOURCE
namespace std{}
#endif /* _HPUX_SOURCE */                                                       
using namespace std;
using namespace CORBA;
using namespace Tango;

//-----------------------------------------------------------------------------
//
// DbDevice::DbDevice() - constructor to create a DbDevice object for
//			  accessing a device of this name in the specified
//			  TANGO database (import/export info and properties) 
//
//-----------------------------------------------------------------------------

DbDevice::DbDevice(string dev_name, Database *dev_dbase)
{
	name = string(dev_name);
	dbase = dev_dbase;
	dbase_create = 0;
}

//-----------------------------------------------------------------------------
//
// DbDevice::DbDevice() - constructor to create a DbDevice object for
//			  accessing a device of this name without specifying
//			  the TANGO database.
//
//-----------------------------------------------------------------------------

DbDevice::DbDevice(string dev_name)
{
	name = string(dev_name);
//
// TODO - replace this with a singleton
//
	dbase = new Database();
	dbase_create = 1;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// DbDevice::~DbDevice() - destructor to destroy a DbDevice object
//
//-----------------------------------------------------------------------------

DbDevice::~DbDevice()
{
	if (dbase_create)
	{
		delete dbase;
	}
	
}

//-----------------------------------------------------------------------------
//
// DbDevice::import() - public method to return import info for a device
//
//-----------------------------------------------------------------------------

DbDevImportInfo DbDevice::import_device()
{
	return dbase->import_device(name);
}

//-----------------------------------------------------------------------------
//
// DbDevice::export_device() - public method to export device to the database
//
//-----------------------------------------------------------------------------

void DbDevice::export_device(DbDevExportInfo &dev_export)
{
	dbase->export_device(dev_export);

	return;
}

//-----------------------------------------------------------------------------
//
// DbDevice::get_property() - public method to get device properties from the database
//
//-----------------------------------------------------------------------------

void DbDevice::get_property(DbData &db_data)
{
	dbase->get_device_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbDevice::put_property() - public method to put device properties from the database
//
//-----------------------------------------------------------------------------

void DbDevice::put_property(DbData &db_data)
{
	dbase->put_device_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbDevice::delete_property() - public method to delete device properties from the database
//
//-----------------------------------------------------------------------------

void DbDevice::delete_property(DbData &db_data)
{
	dbase->delete_device_property(name, db_data);
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
	dbase->get_device_attribute_property(name, db_data);
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
	dbase->put_device_attribute_property(name, db_data);
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
	dbase->delete_device_attribute_property(name, db_data);
	return;
}

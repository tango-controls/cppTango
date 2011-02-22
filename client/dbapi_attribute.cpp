static const char *RcsId = "$Id$\n$Name$";
//
// dbdevice.cpp - C++ source code file for TANGO dbapi class DbAttribute
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- July 2003
//
// last changed	- 7/7/20003
//
// version 	- 1.0
//

#include <tango.h>

                                                   
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// DbAttribute::DbAttribute() - constructor to create a DbAttribute object for
//			  accessing an attribute of this name in the specified
//			  TANGO database (import/export info and properties) 
//
//-----------------------------------------------------------------------------

DbAttribute::DbAttribute(string &att_name, string &dev_name, Database *att_dbase)
{
	name = att_name;
	device_name = dev_name;
	dbase = att_dbase;
	ext_dbase = true;
}

//-----------------------------------------------------------------------------
//
// DbAttribute::DbAttribute() - constructor to create a DbAttribute object for
//			  accessing an attribute of this name without specifying
//			  the TANGO database.
//
//-----------------------------------------------------------------------------

DbAttribute::DbAttribute(string &att_name, string &dev_name)
{
	name = att_name;
	device_name = dev_name;
	db_ind = ApiUtil::instance()->get_db_ind();
	ext_dbase = false;
}

//-----------------------------------------------------------------------------
//
// DbAttribute::DbAttribute() - constructor to create a DbAttribute object for
//			  accessing an attribute of this name with specifying
//			  the TANGO database via its server host and port
//
//-----------------------------------------------------------------------------

DbAttribute::DbAttribute(string &att_name, string &dev_name, string &host,string &port_str)
{
	name = att_name;
	device_name = dev_name;
	
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
// DbAttribute::~DbAttribute() - destructor to destroy a DbAttribute object
//
//-----------------------------------------------------------------------------

DbAttribute::~DbAttribute()
{
}

//-----------------------------------------------------------------------------
//
// DbAttribute::get_property() - public method to get attribute properties from the database
//
//-----------------------------------------------------------------------------

void DbAttribute::get_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->get_device_attribute_property(device_name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->get_device_attribute_property(device_name,db_data);
	}
	return;
}

//-----------------------------------------------------------------------------
//
// DbAttribute::put_property() - public method to put attribute properties from the database
//
//-----------------------------------------------------------------------------

void DbAttribute::put_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_device_attribute_property(device_name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_device_attribute_property(device_name,db_data);
	}
	return;
}

//-----------------------------------------------------------------------------
//
// DbAttribute::delete_property() - public method to delete attribute properties from the database
//
//-----------------------------------------------------------------------------

void DbAttribute::delete_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_device_attribute_property(device_name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_device_attribute_property(device_name,db_data);
	}
	return;
}

} // End of Tango namespace

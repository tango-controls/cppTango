static const char *RcsId = "$Header$";
//
// dbapi_class.cpp - C++ source code file for TANGO dbapi class DbClass
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
// DbClass::DbClass() - constructor to create a DbClass object for
//			accessing a class of this name in the specified
//			  TANGO database (import/export info and properties) 
//
//-----------------------------------------------------------------------------

DbClass::DbClass(string class_name, Database *class_dbase)
{
	name = string(class_name);
	dbase = class_dbase;
	dbase_create = 0;
}

//-----------------------------------------------------------------------------
//
// DbClass::DbClass() - constructor to create a DbClass object for
//			accessing a class of this name without specifying
//			the TANGO database.
//
//-----------------------------------------------------------------------------

DbClass::DbClass(string class_name)
{
	name = string(class_name);
//
// TODO - replace this with a singleton
//
	dbase = new Database();
	dbase_create = 1;
}

//-----------------------------------------------------------------------------
//
// DbClass::~DbClass() - destructor to destroy a DbClass object
//
//-----------------------------------------------------------------------------

DbClass::~DbClass()
{
	if (dbase_create)
	{
		delete dbase;
	}
	
}

//-----------------------------------------------------------------------------
//
// DbClass::get_property() - public method to get class properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::get_property(DbData &db_data)
{
	dbase->get_class_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbClass::put_property() - public method to put class properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::put_property(DbData &db_data)
{
	dbase->put_class_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbClass::delete_property() - public method to delete class properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::delete_property(DbData &db_data)
{
	dbase->delete_class_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbClass::get_attribute_property() - public method to get class attribute 
//           properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::get_attribute_property(DbData &db_data)
{
	dbase->get_class_attribute_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbClass::put_attribute_property() - public method to put class attribute
//           properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::put_attribute_property(DbData &db_data)
{
	dbase->put_class_attribute_property(name, db_data);
	return;
}

//-----------------------------------------------------------------------------
//
// DbClass::delete_attribute_property() - public method to delete class attribute
//           properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::delete_attribute_property(DbData &db_data)
{
	dbase->delete_class_attribute_property(name, db_data);
	return;
}

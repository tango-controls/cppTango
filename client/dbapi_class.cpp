static const char *RcsId = "$Id$\n$Name$";
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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

using namespace CORBA;

namespace Tango
{

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
	ext_dbase = true;
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
	db_ind = ApiUtil::instance()->get_db_ind();
	ext_dbase = false;
}

//-----------------------------------------------------------------------------
//
// DbClass::~DbClass() - destructor to destroy a DbClass object
//
//-----------------------------------------------------------------------------

DbClass::~DbClass()
{
}

//-----------------------------------------------------------------------------
//
// DbClass::get_property() - public method to get class properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::get_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->get_class_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->get_class_property(name, db_data);
	}
}

//-----------------------------------------------------------------------------
//
// DbClass::put_property() - public method to put class properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::put_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_class_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_class_property(name, db_data);
	}
}

//-----------------------------------------------------------------------------
//
// DbClass::delete_property() - public method to delete class properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::delete_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_class_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_class_property(name, db_data);
	}
}

//-----------------------------------------------------------------------------
//
// DbClass::get_attribute_property() - public method to get class attribute 
//           properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::get_attribute_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->get_class_attribute_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->get_class_attribute_property(name, db_data);
	}
}

//-----------------------------------------------------------------------------
//
// DbClass::put_attribute_property() - public method to put class attribute
//           properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::put_attribute_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_class_attribute_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_class_attribute_property(name, db_data);
	}
}

//-----------------------------------------------------------------------------
//
// DbClass::delete_attribute_property() - public method to delete class attribute
//           properties from the database
//
//-----------------------------------------------------------------------------

void DbClass::delete_attribute_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_class_attribute_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_class_attribute_property(name, db_data);
	}
}

} // End of Tango namespace

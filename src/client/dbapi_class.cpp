static const char *RcsId = "$Id$\n$Name$";

//==================================================================================================================
//
// dbapi_class.cpp - C++ source code file for TANGO dbapi class DbClass
//
// programmer 	- Andy Gotz (goetz@esrf.fr) - E. Taurel
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
//====================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

using namespace CORBA;

namespace Tango
{

//-------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::DbClass()
//
// description:
//		Constructor to create a DbClass object for accessing a class of this name in the specified
//		TANGO database (import/export info and properties)
//
// argument:
//		in :
//			- class_name : The class name
//			- class_dbase : The database object ptr
//
//------------------------------------------------------------------------------------------------------------------

DbClass::DbClass(string class_name, Database *class_dbase):ext(Tango_nullptr)
{
	name = string(class_name);
	dbase = class_dbase;
	ext_dbase = true;
}

DbClass::DbClass(string class_name):ext(Tango_nullptr)
{
	name = string(class_name);
	db_ind = ApiUtil::instance()->get_db_ind();
	ext_dbase = false;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::~DbClass()
//
// description:
//		Destructor to destroy a DbClass object
//
//--------------------------------------------------------------------------------------------------------------------

DbClass::~DbClass()
{
}

//---------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::get_property()
//
// description:
//		Public method to get class properties from the database
//
//---------------------------------------------------------------------------------------------------------------------

void DbClass::get_property(DbData &db_data)
{
//
// Try to get db server cache in case we are called during a DS startup sequence
//

	ApiUtil *au = ApiUtil::instance();
	DbServerCache *dsc;
	if (au->in_server() == true)
	{
		Tango::Util *tg = Tango::Util::instance();
		dsc = tg->get_db_cache();
	}
	else
		dsc = NULL;

//
// Call DB (or cache)
//

	if (ext_dbase == true)
		dbase->get_class_property(name, db_data, dsc);
	else
	{
		(au->get_db_vect())[db_ind]->get_class_property(name, db_data);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::put_property()
//
// description:
//		Public method to put class properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

void DbClass::put_property(DbData &db_data)
{

//
// Protect DS code againt a DB exception while the server is in its starting phase
//

	ApiUtil *au = ApiUtil::instance();
	bool forget_except = false;
	if (au->in_server() == true)
	{
		Tango::Util *tg = Tango::Util::instance();
		if (tg->is_svr_starting() == true)
		{
			if (db_data.size() >= 2)
			{
				if ((db_data[0].name == POGO_TITLE) && (db_data[1].name == POGO_DESC))
					forget_except = true;
			}
		}
	}

//
// Call DB
//

	try
	{
		if (ext_dbase == true)
			dbase->put_class_property(name, db_data);
		else
		{
			(au->get_db_vect())[db_ind]->put_class_property(name, db_data);
		}
	}
	catch (Tango::DevFailed &)
	{
		if (forget_except == false)
			throw;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::delete_property()
//
// description:
//		Public method to delete class properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------
//
// method:
//		DbClass::get_attribute_property()
//
// description:
//		Public method to get class attribute properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::put_attribute_property()
//
// description:
//		Public method to put class attribute properties from the database
//
//-------------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::delete_attribute_property()
//
// descriptio:
//		Public method to delete class attribute properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::get_pipe_property()
//
// description:
//		Public method to get class pipe properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

void DbClass::get_pipe_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->get_class_pipe_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->get_class_pipe_property(name, db_data);
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::put_pipe_property()
//
// description:
//		Public method to put class pipe properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

void DbClass::put_pipe_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->put_class_pipe_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->put_class_pipe_property(name, db_data);
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
// method:
// 		DbClass::delete_pipe_property()
//
// description:
//		Public method to delete class pipe properties from the database
//
//--------------------------------------------------------------------------------------------------------------------

void DbClass::delete_pipe_property(DbData &db_data)
{
	if (ext_dbase == true)
		dbase->delete_class_pipe_property(name, db_data);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_class_pipe_property(name, db_data);
	}
}

} // End of Tango namespace

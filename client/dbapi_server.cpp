static const char *RcsId = "$Id$\n$Name$";
//
// dbdevice.cpp - C++ source code file for TANGO dbapi class DbServer
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- November 2000
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
// DbServer::DbServer() - constructor to create a DbServer object for
//			  accessing a server of this name in the specified
//			  TANGO database (import/export info and properties) 
//
//-----------------------------------------------------------------------------

DbServer::DbServer(string server_name, Database *server_dbase)
{
	name = string(server_name);
	dbase = server_dbase;
	ext_dbase = true;
}

//-----------------------------------------------------------------------------
//
// DbServer::DbServer() - constructor to create a DbServer object for
//			  accessing a server of this name without specifying
//			  the TANGO database.
//
//-----------------------------------------------------------------------------

DbServer::DbServer(string server_name)
{
	name = string(server_name);
	db_ind = ApiUtil::instance()->get_db_ind();
	ext_dbase = false;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// DbServer::~DbServer() - destructor to destroy a DbServer object
//
//-----------------------------------------------------------------------------

DbServer::~DbServer()
{
}

//-----------------------------------------------------------------------------
//
// DbServer::add_server() - public method to add a server and its devices
//                          to the database
//
//-----------------------------------------------------------------------------

void DbServer::add_server(DbDevInfos &dev_infos)
{
	if (ext_dbase == true)
		dbase->add_server(name, dev_infos);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->add_server(name, dev_infos);
	}
}

//-----------------------------------------------------------------------------
//
// DbServer::export_server() - public method to export devices belonging to
//                             this server to the database
//
//-----------------------------------------------------------------------------

void DbServer::export_server(DbDevExportInfos &dev_export)
{
	if (ext_dbase == true)
		dbase->export_server(dev_export);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->export_server(dev_export);
	}
}

//-----------------------------------------------------------------------------
//
// DbServer::unexport_server() - public method to unexport all devices belonging
//                               to this server in the database
//
//-----------------------------------------------------------------------------

void DbServer::unexport_server()
{
	if (ext_dbase == true)
		dbase->unexport_server(name);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->unexport_server(name);
	}
}

//-----------------------------------------------------------------------------
//
// DbServer::delete_server() - public method to delete all devices belonging
//                             to this server from the database
//
//-----------------------------------------------------------------------------

void DbServer::delete_server()
{
	if (ext_dbase == true)
		dbase->delete_server(name);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		(au->get_db_vect())[db_ind]->delete_server(name);
	}
}

//-----------------------------------------------------------------------------
//
// DbServer::get_server_info() - public method to get server info
//
//-----------------------------------------------------------------------------

DbServerInfo DbServer::get_server_info()
{
	if (ext_dbase == true)
		return dbase->get_server_info(name);
	else
	{
		ApiUtil *au = ApiUtil::instance();
		return (au->get_db_vect())[db_ind]->get_server_info(name);
	}
}

} // End of Tango namespace

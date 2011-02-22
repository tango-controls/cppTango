static const char *RcsId = "$Header$";
//
// dbdevice.cpp - C++ source code file for TANGO dbapi class DbServer
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- November 2000
//
// last changed	- 14/11/2000 
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
// DbServer::DbServer() - constructor to create a DbServer object for
//			  accessing a server of this name in the specified
//			  TANGO database (import/export info and properties) 
//
//-----------------------------------------------------------------------------

DbServer::DbServer(string server_name, Database *server_dbase)
{
	name = string(server_name);
	dbase = server_dbase;
	dbase_create = 0;
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
//
// TODO - maybe replace this with a singleton
//
	dbase = new Database();
	dbase_create = 1;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// DbServer::~DbServer() - destructor to destroy a DbServer object
//
//-----------------------------------------------------------------------------

DbServer::~DbServer()
{
	if (dbase_create)
	{
		delete dbase;
	}
	
}

//-----------------------------------------------------------------------------
//
// DbServer::add_server() - public method to add a server and its devices
//                          to the database
//
//-----------------------------------------------------------------------------

void DbServer::add_server(DbDevInfos &dev_infos)
{
	dbase->add_server(name, dev_infos);
	return;
}

//-----------------------------------------------------------------------------
//
// DbServer::export_server() - public method to export devices belonging to
//                             this server to the database
//
//-----------------------------------------------------------------------------

void DbServer::export_server(DbDevExportInfos &dev_export)
{
	dbase->export_server(name, dev_export);
	return;
}

//-----------------------------------------------------------------------------
//
// DbServer::unexport_server() - public method to unexport all devices belonging
//                               to this server in the database
//
//-----------------------------------------------------------------------------

void DbServer::unexport_server()
{
	dbase->unexport_server(name);
	return;
}

//-----------------------------------------------------------------------------
//
// DbServer::delete_server() - public method to delete all devices belonging
//                             to this server from the database
//
//-----------------------------------------------------------------------------

void DbServer::delete_server()
{
	dbase->delete_server(name);
	return;
}

//-----------------------------------------------------------------------------
//
// DbServer::get_server_info() - public method to get server info
//
//-----------------------------------------------------------------------------

DbServerInfo DbServer::get_server_info()
{
	return(dbase->get_server_info(name));
}


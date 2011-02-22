static const char *RcsId = "$Header$";
//
// cpp 	- C++ source code file for TANGO dbapi class Database
//
// programmer 	- Andy Gotz (goetz@esrf.fr)
//
// original 	- September 2000
//
// last changed	- 5/9/2000 
//
// version 	- 1.0
//
// $Log$
// Revision 2.2  2002/10/14 09:32:43  taurel
// Fix bugs in devapi_base.cpp file :
// - In read_attribute and read_attributes method of the DeviceProxy class
//   Do not create sequence the same way if the call is local or remote.
// - Add reconnection in the Connection::set_timeout_millis method
// - Add flags to the Connection::set_timeout_millis method
// - Fix bug in the DeviceProxy constructor when device is not marked as exported
//   in the database. The constructor was not stateless in this case.
//
// Revision 2.1  2002/08/12 12:43:23  taurel
// Fix bug in DeviceProxy::write_attributes method when writing several
// attributes in one call. (File devapi_base.cpp)
//
// Revision 2.0  2002/06/28 13:43:07  taurel
// Lot of changes since last releases :
// 	- Database object managed as a singleton per control system
// 	- Support all tango device naming syntax (using TANGO_HOST env.
//  	  variable, without env variable and non database device)
// 	- No more copy during read_attribute and command_inout
// 	- Added some missing methods
// 	- Build an exception class hierarchy
// 	- Added correct management of device time-out
// 	- Support all Tango device interface release 2 features
// 	  (data/attribute comming from polling buffer, polling related methods,
// 	   command/attribute result history)
//
// Revision 1.13  2002/04/29 12:11:25  goetz
// New change in Database::delete_device_attribute_property. The fix done in the previous release was not enough
//
// Revision 1.12  2002/04/29 05:43:09  goetz
// Check in by ET. Fix bug (i=i+n_props+1) in Database:delete_device_attribute_property, delete_class_attribute_property and put_class_attribute_property
//
// Revision 1.11  2002/03/18 07:20:55  goetz
// new DeviceProxy() stateless now; added inline method name()
//
// Revision 1.10  2002/03/01 15:48:05  goetz
// added get_attribute_list() method
//
// Revision 1.9  2002/02/28 17:00:52  goetz
// intermediate checkin
//
// Revision 1.8  2002/02/18 20:42:59  goetz
// supports attributes, added lots of new methods, changed some old ones
//
// Revision 1.7  2002/02/11 20:16:58  goetz
// added inline methods for Tango CORBA sequence types
//
// Revision 1.6  2001/12/04 21:34:46  goetz
// fixed bug in Database::put_device_attribute_property
//
// Revision 1.5  2001/09/27 13:04:46  goetz
// added (const char*) to all throw_exceptions; ported to dll on Windows
//
// Revision 1.4  2001/08/13 13:40:00  goetz
// improved exception handling; ported to Windows
//
// Revision 1.3  2001/08/03 14:24:38  goetz
// implemented reconnect; catching CORBA::Exception's and rethrowing them
//
// Revision 1.2  2001/08/02 14:50:11  goetz
// added support for bool, DevState and DevVarCharArray types
//
// Revision 1.1.1.1  2001/07/18 08:42:24  goetz
// Imported sources
//
// Revision 1.12  2001/06/08 12:55:06  goetz
// fixed bug with extracting vectors under Windows
//
//
//

#include <tango.h>
                                                  
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// Database::Database() - constructor to create connection to TANGO Database
//                        without arguments, host and port specified by
//                        environment variable TANGO_HOST
//
//-----------------------------------------------------------------------------

Database::Database(ORB *orb_in) : Connection(orb_in)
{
//
// get host and port from environment variable TANGO_HOST
//
	char *tango_host_env_c_str;
	
       	tango_host_env_c_str = getenv("TANGO_HOST");

       	if (tango_host_env_c_str == NULL)
       	{
       		cerr << "TANGO_HOST environment variable not set, set it and retry (e.g. TANGO_HOST=tango:20000)" << endl;
		
		TangoSys_MemStream desc;
		desc << "TANGO_HOST environment variable not set, set it and retry (e.g. TANGO_HOST=tango:20000)" << ends;
		ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       desc.str(),
					       (const char *)"Database::Database");
	}
	string tango_host_env(tango_host_env_c_str);
	string::size_type separator;
	
       	separator = tango_host_env.find(":");
       	if (separator != string::npos)
       	{
		db_port = tango_host_env.substr(separator+1);
		db_port_num = atoi(db_port.c_str());
	}
	db_host = tango_host_env.substr(0,separator);
	
	host = db_host;
	port = db_port;
	port_num = db_port_num;
	
	dbase_used = true;
	from_env_var = true;
	
	cout4 <<"Database::Database(): TANGO host " << host << " port " << port << endl;

	build_connection();		
}

//-----------------------------------------------------------------------------
//
// Database::Database(string host, int port, ORB *orb) - constructor to 
//           create connection to TANGO Database with host, port, orb specified.
//
//-----------------------------------------------------------------------------

Database::Database(string &in_host, int in_port, ORB *orb_in) : Connection(orb_in)
{
	host = in_host;
	db_host = host;
	
	TangoSys_MemStream o;
	o << in_port << ends;
#ifdef STRSTREAM
	char *ptr = o.str();
	port = ptr;
	delete [] ptr;
#else
	port = o.str().c_str();
#endif

	db_port = port;
	
	db_port_num = in_port;
	port_num = in_port;
	from_env_var = false;
	dbase_used = true;
		
	build_connection();	
}

//-----------------------------------------------------------------------------
//
// Database::~Database() - destructor to destroy connection to TANGO Database
//
//-----------------------------------------------------------------------------

Database::~Database()
{
}

//-----------------------------------------------------------------------------
//
// Database::connect() - method to create connection to TANGO Database
//                       specifying host and port
//
//-----------------------------------------------------------------------------

void Database::build_connection()
{
	string corba_name = get_corba_name();
	connect(corba_name);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::state() - private method to return corba name for 
//
//-----------------------------------------------------------------------------

string Database::get_corba_name()
{
        string db_corbaloc = "corbaloc:iiop:";
        db_corbaloc = db_corbaloc+host+":"+port;
	db_corbaloc = db_corbaloc+"/"+DbObjName;

	return db_corbaloc;
}

//-----------------------------------------------------------------------------
//
// Database::get_info() - public method to return info about the Database
//
//-----------------------------------------------------------------------------

string Database::get_info()
{
	Any send;

	Any_var received = command_inout("DbInfo",send);
	const DevVarStringArray *db_info_list;
	received >>= db_info_list;

#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	for (int i=0; i<db_info_list->length(); i++)
	{
		ostream << (*db_info_list)[i] << endl;
	}
	ostream << ends;
	
	string ret_str = ostream.str();
#ifdef STRSTREAM
	ostream.rdbuf()->freeze(false);
#endif /* STRSTREAM */
	
	return(ret_str);
}

//-----------------------------------------------------------------------------
//
// Database::import_device() - public method to return import info for a device
//
//-----------------------------------------------------------------------------

DbDevImportInfo Database::import_device(string &dev)
{
	Any send;
	send <<= dev.c_str();
	Any_var received = command_inout("DbImportDevice",send);

	const DevVarLongStringArray *dev_import_list;
	received >>= dev_import_list;

	DbDevImportInfo dev_import;
	dev_import.name = dev;
	dev_import.ior = string((dev_import_list->svalue)[1]);
	dev_import.version = string((dev_import_list->svalue)[2]);
	dev_import.exported = dev_import_list->lvalue[0];

	return(dev_import);
}

//-----------------------------------------------------------------------------
//
// Database::export_device() - public method to export device to the Database
//
//-----------------------------------------------------------------------------

void Database::export_device(DbDevExportInfo& dev_export)
{
	Any send;
	DevVarStringArray *dev_export_list = new DevVarStringArray;
	dev_export_list->length(5);
	(*dev_export_list)[0] = string_dup(dev_export.name.c_str());
	(*dev_export_list)[1] = string_dup(dev_export.ior.c_str());
	(*dev_export_list)[2] = string_dup(dev_export.host.c_str());
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	ostream << dev_export.pid << ends;
#ifdef STRSTREAM
	(*dev_export_list)[3] = string_dup(ostream.str());
	delete [] (ostream.str());
#else
	(*dev_export_list)[3] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	(*dev_export_list)[4] = string_dup(dev_export.version.c_str());
	send <<= dev_export_list;

	Any_var received = command_inout("DbExportDevice",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::unexport_device() - public method to unexport a device from the Database
//
//-----------------------------------------------------------------------------

void Database::unexport_device(string dev)
{
	Any send;
	send <<= string_dup(dev.c_str());
	Any_var received = command_inout("DbUnExportDevice",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::add_device() - public method to add a device to the Database
//
//-----------------------------------------------------------------------------

void Database::add_device(DbDevInfo &dev_info)
{
	Any send;
	DevVarStringArray *dev_info_list = new DevVarStringArray;
	dev_info_list->length(3);
	(*dev_info_list)[0] = string_dup(dev_info.server.c_str());
	(*dev_info_list)[1] = string_dup(dev_info.name.c_str());
	(*dev_info_list)[2] = string_dup(dev_info._class.c_str());
	send <<= dev_info_list;
	Any_var received = command_inout("DbAddDevice",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_device() - public method to delete a device from the Database
//
//-----------------------------------------------------------------------------

void Database::delete_device(string dev)
{
	Any send;
	send <<= string_dup(dev.c_str());
	Any_var received = command_inout("DbDeleteDevice",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::add_server() - public method to add a server to the Database
//
//-----------------------------------------------------------------------------

void Database::add_server(string &server, DbDevInfos &dev_infos)
{
	Any send;
	DevVarStringArray *dev_info_list = new DevVarStringArray;
	dev_info_list->length(2*dev_infos.size()+1);
	(*dev_info_list)[0] = string_dup(server.c_str());
	for (int i=0; i<dev_infos.size(); i++)
	{
		(*dev_info_list)[i*2+1] = string_dup(dev_infos[i].name.c_str());
		(*dev_info_list)[i*2+2] = string_dup(dev_infos[i]._class.c_str());
	}
	send <<= dev_info_list;
	Any_var received = command_inout("DbAddServer",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_server() - public method to delete a server from the Database
//
//-----------------------------------------------------------------------------

void Database::delete_server(string &server)
{
	Any send;
	send <<= string_dup(server.c_str());
	Any_var received = command_inout("DbDeleteServer",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::export_server() - public method to export a server to the Database
//
//-----------------------------------------------------------------------------

void Database::export_server(string &server, DbDevExportInfos& dev_export)
{
	Any send;
	DevVarStringArray *dev_export_list = new DevVarStringArray;
	dev_export_list->length(5*dev_export.size());
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	for (int i=0; i<dev_export.size(); i++)
	{
		(*dev_export_list)[i*5] = string_dup(dev_export[i].name.c_str());
		(*dev_export_list)[i*5+1] = string_dup(dev_export[i].ior.c_str());
		(*dev_export_list)[i*5+2] = string_dup(dev_export[i].host.c_str());
		ostream << dev_export[i].pid << ends;
#ifdef STRSTREAM
		(*dev_export_list)[i*5+3] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*dev_export_list)[i*5+3] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		(*dev_export_list)[i*5+4] = string_dup(dev_export[i].version.c_str());
	}
	send <<= dev_export_list;

	Any_var received = command_inout("DbExportServer",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::unexport_server() - public method to unexport a server from the Database
//
//-----------------------------------------------------------------------------

void Database::unexport_server(string &server)
{
	Any send;
	send <<= server.c_str();
	Any_var received = command_inout("DbUnExportServer",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_server_info() - public method to return info for a server
//
//-----------------------------------------------------------------------------

DbServerInfo Database::get_server_info(string &server)
{
	Any send;
	send <<= server.c_str();
	Any_var received = command_inout("DbGetServerInfo",send);

	const DevVarStringArray *server_info_list;
	received >>= server_info_list;

	DbServerInfo server_info;
	server_info.name = server;
	server_info.host = string((*server_info_list)[0]);
	server_info.mode = atoi((*server_info_list)[1]);
	server_info.level = atoi((*server_info_list)[2]);

	return(server_info);
}
//-----------------------------------------------------------------------------
//
// Database::get_device_property() - public method to get a device property from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_device_property(string dev, DbData &db_data)
{
	int i;
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(dev.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbGetDeviceProperty",send);
	const DevVarStringArray *property_values;
	received >>= property_values;

	int n_props, index;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in();
	iostream >> n_props;
	index = 2;
	for (i=0; i<n_props; i++)
	{
		int n_values;
		db_data[i].name = (*property_values)[index].in(); index++;	
		iostream.clear(); iostream.seekp (0);  iostream.seekg (0);
		iostream << (*property_values)[index].in() << ends; 
		iostream >> n_values;
		index++;
	
		db_data[i].value_string.resize(n_values);
		if (n_values == 0)
		{
			index++; // skip dummy returned value " "
		}
		else
		{
			for (int j=0; j<n_values; j++)
			{
				db_data[i].value_string[j] = (*property_values)[index].in(); 
				index++;
			}
		}
	}
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::put_device_property() - public method to put a device property from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::put_device_property(string dev, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(dev.c_str());
	ostream << db_data.size();
#ifdef STRSTREAM
	ostream << ends;
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	for (int i=0; i<db_data.size(); i++)
	{
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		ostream.clear(); ostream.seekp (0);
		ostream << db_data[i].size() << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		ostream << ends;
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<db_data[i].size(); j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i].value_string[j].c_str());
		}
	}
	send <<= property_values;

	Any_var received = command_inout("DbPutDeviceProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_device_property() - public method to delete device properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::delete_device_property(string dev, DbData &db_data)
{
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(dev.c_str());
	for (int i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbDeleteDeviceProperty",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_attribute_property() - public method to get device attribute properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_device_attribute_property(string dev, DbData &db_data)
{
	int i;
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(dev.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbGetDeviceAttributeProperty",send);
	const DevVarStringArray *property_values;
	received >>= property_values;

	int n_attribs, index;
	int i_total_props, n_total_props;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in();
	iostream >> n_attribs;
	index = 2;
	n_total_props = 0;
	i_total_props = 0;
	for (i=0; i<n_attribs; i++)
	{
		short n_props;
		db_data[i_total_props].name = (*property_values)[index]; index++;	
		iostream.clear(); iostream.seekp(0); iostream.seekg(0);
		iostream << (*property_values)[index].in() << ends; 
		iostream >> n_props;
		index++;
		db_data[i_total_props] << n_props; i_total_props++;
		n_total_props += n_props + 1;
		db_data.resize(n_total_props);
		for (int j=0; j<n_props; j++)
		{
			db_data[i_total_props].name = (*property_values)[index]; 
			index++;
			db_data[i_total_props].value_string.resize(1);
			db_data[i_total_props].value_string[0] = (*property_values)[index]; 
			index++; i_total_props++;
		}
	}
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::put_device_attribute_property() - public method to put device 
//	     attribute properties into the Database
//
//-----------------------------------------------------------------------------

void Database::put_device_attribute_property(string dev, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index, n_attribs ;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(dev.c_str());
	n_attribs = 0;
	for (int i=0; i<db_data.size();)
	{
		short n_props;
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		db_data[i] >> n_props;
		ostream.clear(); ostream.seekp(0);
		ostream << n_props << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<n_props; j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i+j+1].name.c_str());
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i+j+1].value_string[0].c_str());
		}
		i = i+n_props+1;
		n_attribs++;
	}
	ostream << n_attribs << ends;
#ifdef STRSTREAM
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	send <<= property_values;

	Any_var received = command_inout("DbPutDeviceAttributeProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_device_attribute_property() - public method to delete device
//	     attribute properties from the Database
//
//-----------------------------------------------------------------------------

void Database::delete_device_attribute_property(string dev, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index, n_attribs;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2);
// index = 2;
 	index = 1;
	(*property_values)[0] = string_dup(dev.c_str());
	n_attribs = 0;
	for (int i=0; i<db_data.size();)
	{
		short n_props;
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		db_data[i] >> n_props;
		ostream.clear(); ostream.seekp(0);
		ostream << n_props << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<n_props; j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i+j+1].name.c_str());
		}
		i = i+n_props+1;
		n_attribs++;
	}
	ostream << n_attribs << ends;
#ifdef STRSTREAM
//	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
//	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	send <<= property_values;

	Any_var received = command_inout("DbDeleteDeviceAttributeProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_class_property() - public method to get class properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_class_property(string device_class, DbData &db_data)
{
	int i;
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(device_class.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbGetClassProperty",send);
	const DevVarStringArray *property_values;
	received >>= property_values;

	int n_props, index;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in();
	iostream >> n_props;
	index = 2;
	for (i=0; i<n_props; i++)
	{
		int n_values;
		db_data[i].name = (*property_values)[index]; index++;	
		iostream.clear(); iostream.seekp(0); iostream.seekg(0);
		iostream << (*property_values)[index].in(); index++;
		iostream >> n_values;
	
		db_data[i].value_string.resize(n_values);
		for (int j=0; j<n_values; j++)
		{

			db_data[i].value_string[j] = (*property_values)[index]; 
			index++;
		}
	}
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::put_class_property() - public method to put class properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::put_class_property(string device_class, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(device_class.c_str());
	ostream << db_data.size() << ends;
#ifdef STRSTREAM
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	for (int i=0; i<db_data.size(); i++)
	{
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		ostream.clear(); ostream.seekp (0);
		ostream << db_data[i].size() << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<db_data[i].size(); j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i].value_string[j].c_str());
		}
	}
	send <<= property_values;

	Any_var received = command_inout("DbPutClassProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_class_property() - public method to delete class properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::delete_class_property(string device_class, DbData &db_data)
{
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(device_class.c_str());
	for (int i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbDeleteClassProperty",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_class_attribute_property() - public method to get class attribute properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_class_attribute_property(string device_class, DbData &db_data)
{
	int i;
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(device_class.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbGetClassAttributeProperty",send);
	const DevVarStringArray *property_values;
	received >>= property_values;

	int n_attribs, index;
	int i_total_props, n_total_props;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in();
	iostream >> n_attribs;
	index = 2;
	n_total_props = 0;
	i_total_props = 0;
	for (i=0; i<n_attribs; i++)
	{
		short n_props;
		db_data[i_total_props].name = (*property_values)[index]; index++;	
		iostream.clear(); iostream.seekp(0); iostream.seekg(0);
		iostream << (*property_values)[index].in() << ends; index++;
		iostream >> n_props;
		db_data[i_total_props] << n_props; i_total_props++;
		n_total_props += n_props + 1;
		db_data.resize(n_total_props);
		for (int j=0; j<n_props; j++)
		{
			db_data[i_total_props].name = (*property_values)[index]; 
			index++;
			db_data[i_total_props].value_string.resize(1);
			db_data[i_total_props].value_string[0] = (*property_values)[index]; 
			index++; i_total_props++;
		}
	}
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::put_class_attribute_property() - public method to put class 
//	     attribute properties into the Database
//
//-----------------------------------------------------------------------------

void Database::put_class_attribute_property(string device_class, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index, n_attribs;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(device_class.c_str());
	n_attribs = 0;
	for (int i=0; i<db_data.size(); )
	{
		short n_props;
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		db_data[i] >> n_props;
		ostream.clear(); ostream.seekp(0);
		ostream << n_props << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<n_props; j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i+j+1].name.c_str());
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i+j+1].value_string[0].c_str());
		}
		i = i+n_props+1;
		n_attribs++;
	}
	ostream << n_attribs << ends;
#ifdef STRSTREAM
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	send <<= property_values;

	Any_var received = command_inout("DbPutClassAttributeProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_class_attribute_property() - public method to delete class
//	     attribute properties from the Database
//
//-----------------------------------------------------------------------------

void Database::delete_class_attribute_property(string device_class, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index, n_attribs;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(device_class.c_str());
	n_attribs = 0;
	for (int i=0; i<db_data.size();)
	{
		short n_props;
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		db_data[i] >> n_props;
		ostream.clear(); ostream.seekp(0);
		ostream << n_props << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<n_props; j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i+j+1].name.c_str());
		}
		i = i+n_props+1;
		n_attribs++;
	}
	ostream << n_attribs << ends;
#ifdef STRSTREAM
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	send <<= property_values;

	Any_var received = command_inout("DbDeleteClassAttributeProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_name() - public method to get device names from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_name(string &device_server, string &device_class)
{
	Any send;
	DevVarStringArray *device_server_class = new DevVarStringArray;
	device_server_class->length(2);
	(*device_server_class)[0] = string_dup(device_server.c_str());
	(*device_server_class)[1] = string_dup(device_class.c_str());
	send <<= device_server_class;

	Any_var received = command_inout("DbGetDeviceList",send);
	const DevVarStringArray *device_names;
	received >>= device_names;

	DbDatum db_datum;
	int n_devices;
	n_devices = device_names->length();
	db_datum.name = device_server;
	db_datum.value_string.resize(n_devices);
	for (int i=0; i<n_devices; i++)
	{
		db_datum.value_string[i] = (*device_names)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_alias() - public method to get device aliases from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_alias(string  &dev)
{
	Any send;
	send <<= dev.c_str();

	Any_var received = command_inout("DbGetDeviceAliasList",send);
	const DevVarStringArray *device_alias;
	received >>= device_alias;

	DbDatum db_datum;
	int n_aliases;
	n_aliases = device_alias->length();
	db_datum.name = dev;
	db_datum.value_string.resize(n_aliases);
	for (int i=0; i<n_aliases; i++)
	{
		db_datum.value_string[i] = (*device_alias)[i]; 
	}
		
	return db_datum;
}


//-----------------------------------------------------------------------------
//
// Database::get_device_exported() - public method to get exported device names from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_exported(string &filter)
{
	Any send;
	DevString filter_tmp;
	filter_tmp = string_dup(filter.c_str());
	send <<= filter_tmp;

	Any_var received = command_inout("DbGetDeviceExportedList",send);
	const DevVarStringArray *device_names;
	received >>= device_names;

	DbDatum db_datum;
	int n_devices;
	n_devices = device_names->length();
	db_datum.name = filter;
	db_datum.value_string.resize(n_devices);
	for (int i=0; i<n_devices; i++)
	{
		db_datum.value_string[i] = (*device_names)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_member() - public method to get device members from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_member(string &wildcard)
{
	Any send;
	send <<= wildcard.c_str();

	Any_var received = command_inout("DbGetDeviceMemberList",send);
	const DevVarStringArray *device_member;
	received >>= device_member;

	DbDatum db_datum;
	int n_members;
	n_members = device_member->length();
	db_datum.name = wildcard;
	db_datum.value_string.resize(n_members);
	for (int i=0; i<n_members; i++)
	{
		db_datum.value_string[i] = (*device_member)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_family() - public method to get device familys from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_family(string &wildcard)
{
	Any send;
	send <<= wildcard.c_str();

	Any_var received = command_inout("DbGetDeviceFamilyList",send);
	const DevVarStringArray *device_family;
	received >>= device_family;

	DbDatum db_datum;
	int n_familys;
	n_familys = device_family->length();
	db_datum.name = wildcard;
	db_datum.value_string.resize(n_familys);
	for (int i=0; i<n_familys; i++)
	{
		db_datum.value_string[i] = (*device_family)[i]; 
	}
		
	return db_datum;
}


//-----------------------------------------------------------------------------
//
// Database::get_device_domain() - public method to get device domains from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_domain(string &wildcard)
{
	Any send;
	send <<= wildcard.c_str();

	Any_var received = command_inout("DbGetDeviceFamilyList",send);
	const DevVarStringArray *device_domain;
	received >>= device_domain;

	DbDatum db_datum;
	int n_domains;
	n_domains = device_domain->length();
	db_datum.name = wildcard;
	db_datum.value_string.resize(n_domains);
	for (int i=0; i<n_domains; i++)
	{
		db_datum.value_string[i] = (*device_domain)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::get_property() - public method to get object property from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_property(string obj, DbData &db_data)
{
	int i;
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(obj.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbGetProperty",send);
	const DevVarStringArray *property_values;
	received >>= property_values;

	int n_props, index;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in();
	iostream >> n_props;
	index = 2;
	for (i=0; i<n_props; i++)
	{
		int n_values;
		db_data[i].name = (*property_values)[index]; index++;	
		iostream.clear(); iostream.seekp (0);  iostream.seekg (0);
		iostream << (*property_values)[index].in() << ends; 
		iostream >> n_values;
		index++;
	
		db_data[i].value_string.resize(n_values);
		if (n_values == 0)
		{
			index++; // skip dummy returned value " "
		}
		else
		{
			for (int j=0; j<n_values; j++)
			{
				db_data[i].value_string[j] = (*property_values)[index]; 
				index++;
			}
		}
	}
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::put_property() - public method to put object property from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::put_property(string obj, DbData &db_data)
{
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	int index;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(obj.c_str());
	ostream << db_data.size();
#ifdef STRSTREAM
	ostream << ends;
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	(*property_values)[1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
	for (int i=0; i<db_data.size(); i++)
	{
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		ostream.clear(); ostream.seekp (0);
		ostream << db_data[i].size() << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		ostream << ends;
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		(*property_values)[index-1] = string_dup(ostream.str().c_str());
#endif /* STRSTREAM */
		for (int j=0; j<db_data[i].size(); j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i].value_string[j].c_str());
		}
	}
	send <<= property_values;

	Any_var received = command_inout("DbPutProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_property() - public method to delete object properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::delete_property(string obj, DbData &db_data)
{
	Any send;
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(obj.c_str());
	for (int i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	Any_var received = command_inout("DbDeleteProperty",send);

	return;
}

} // End of Tango namespace

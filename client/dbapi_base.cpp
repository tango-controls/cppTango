static const char *RcsId = "$Id$\n$Name$";

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
// Revision 3.25  2008/03/11 14:36:44  taurel
// - Apply patches from Frederic Picca about compilation with gcc 4.2
//
// Revision 3.24  2008/01/25 15:45:58  taurel
// - Some changes in the Db cache
// - A lighter system to shutdown DS in case of dynamic attribute
//
// Revision 3.23  2008/01/08 14:32:39  taurel
// - Fix bug in the unexport_event() method
//
// Revision 3.22  2008/01/07 07:13:25  taurel
// - Some tab spacing in services related calls
//
// Revision 3.21  2007/12/12 10:15:46  taurel
// - Db calls during DS startup has a separate timeout and some retries
//
// Revision 3.20  2007/11/08 12:00:20  taurel
// - Add miscellaneous getter methods in the DbServerCache class
// - The DbDatum::Size() method is now inline
// - Fix bug in database re-connection timeout management
//
// Revision 3.19  2007/10/16 08:25:47  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
// Revision 3.18  2007/04/30 14:05:47  jlpons
// Added new commands to the database client interface.
//
// Revision 3.17  2007/04/20 14:38:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.16  2007/03/28 07:48:51  taurel
// - Remove some unused 64 bits management
//
// Revision 3.15  2006/08/11 13:51:57  taurel
// - Added the Database::get_device_property_list() method
//
// Revision 3.14  2006/06/28 14:38:06  taurel
// - Fix memory leak in Database::delete_device method
//
// Revision 3.13  2005/06/29 08:32:21  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.12  2005/05/20 15:19:23  taurel
// - Clarify the use of delete_device_attribute_property to be coherent with
//   Java
//
// Revision 3.11  2005/05/10 16:09:13  taurel
// - Fix windows VC7 warnings
//
// Revision 3.10  2005/04/15 11:35:05  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.9  2005/03/14 09:48:12  taurel
// - Fix some bugs in filedatabase (Change in the data transferred between client and
//   server).
// - Fix bug in event re-connection
// - Add event support even for device server started with the -file command line option
//
// Revision 3.8  2005/01/13 09:29:26  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 3.6.2.6  2004/11/10 13:06:51  taurel
// - Fix bug for WIN32 in the get_info() method
// - Change how the Database string ctor calls the Connection ctor
//
// Revision 3.6.2.5  2004/11/04 09:51:29  taurel
// - Add compatibility with old database server for the DbGetDeviceAttributeProperty
//   and associate commands
// - Some minor fixes to pass test suite
//
// Revision 3.6.2.4  2004/10/22 11:23:16  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.6.2.3  2004/10/19 16:43:18  maxchiandone
// implemented the re-read method.
//
// Revision 3.6.2.2  2004/10/05 13:58:10  maxchiandone
// First upload for filedatabase.
//
// Revision 3.6.2.1  2004/09/15 06:44:43  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.6  2004/07/07 08:39:55  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.5  2003/10/10 10:57:59  ounsy
// minor bug corrections
//
// Revision 3.4  2003/09/02 13:09:09  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.3  2003/08/21 07:22:01  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.2.2.6  2004/03/19 15:19:50  taurel
// - Comment out the connect(this) call in DeviceProxy::unsubscribe_event() method
// - Fix bug in db interface with stream usage (once more)
//
// Revision 3.2.2.5  2004/03/09 16:34:07  taurel
// - Added port to HP aCC (Thanks to Claudio from Elettra)
// - Some last small bug fixes
//
// Revision 3.2.2.4  2004/03/02 07:40:23  taurel
// - Fix compiler warnings (gcc used with -Wall)
// - Fix bug in DbDatum insertion operator fro vectors
// - Now support "modulo" as periodic filter
//
// Revision 3.2.2.3  2004/01/20 08:30:07  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.2.2.2  2003/10/16 11:58:17  taurel
// - Fix some memory leaks in DeviceProxy::is_polled() and in
// Database::get_device_exported() methods
// - Add some bug fixes in Database::get_device_attribute_property(),
// Database::put_class_attribute_property()
//
// Revision 3.2.2.1  2003/09/18 14:07:41  taurel
// Fixes some bugs:
//  - Bug fix in DeviceProxy copy constructor and assignement operator
//  - Change the way how DeviceProxy::write_attribute() is coded
//  - Added DeviceAttribute ctors from "const char *"
//  - Split "str().c_str()" in two lines of code. It was the reason of some
//    problems using Windows VC6
//
// Revision 3.4  2003/09/02 13:09:09  taurel
// Add memorized attribute feature (only for SCALAR and WRITE/READ_WRITE attribute)
//
// Revision 3.3  2003/08/21 07:22:01  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.2  2003/05/28 14:42:55  taurel
// Add (conditionaly) autoconf generated include file
//
// Revision 3.1  2003/04/02 12:22:07  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
// Revision 3.0.2.1  2003/11/16 22:10:43  andy_gotz
// New version which defines 4 types of events - change, quality, periodic and
// archive. Code has been factorised to reduce redundancy. Minimum and maximum
// changes are supported. Event period is taken into account. Relative and
// absolute changes are detected. Whole sequence is taken into account when
// determining change.
//
// Revision 3.0  2003/03/25 16:30:34  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 2.5  2003/03/20 08:54:53  taurel
// Updated to support asynchronous calls
//
// Revision 2.4  2003/01/09 12:00:33  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.3  2002/12/16 11:58:36  taurel
// - Change the underlying ORB fom ORBacus to omniORB
// - New method get_device_list() in Util class
// - Util::get_class_list() takes DServer device into account
// - Util::get_device_by_name() takes DSErver device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
//
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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
                                                  
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// Database::Database() - constructor to create connection to TANGO Database
//                        without arguments, host and port specified by
//                        environment variable TANGO_HOST
//			  Rem : For Windows, when the server is used as a
//				service, the TANGO_HOST environment
//				variable must be retrieved from the registry
//				from the device server exec name and instance
//				name
//
//-----------------------------------------------------------------------------

Database::Database(ORB *orb_in) : Connection(orb_in)
{
//
// get host and port from environment variable TANGO_HOST
//

	char *tango_host_env_c_str;
	filedb = 0;
	serv_version = 0;
	
	ext = new DatabaseExt();

#ifdef _TG_WINDOWS_
	tango_host_env_c_str = getenv(EnvVariable);
#else	
	tango_host_env_c_str = getenv(EnvVariable);
#endif

	if (tango_host_env_c_str == NULL)
	{		
		TangoSys_MemStream desc;
		desc << "TANGO_HOST env. variable not set, set it and retry (e.g. TANGO_HOST=<host>:<port>)" << ends;
		ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       desc.str(),
					       (const char *)"Database::Database");
	}
	
	check_tango_host(tango_host_env_c_str);
	
	cout4 <<"Database::Database(): TANGO host " << host << " port " << port << endl;

	build_connection();		
}

#ifdef _TG_WINDOWS_
Database::Database(ORB *orb_in,string &ds_exec_name,string &ds_inst_name) : Connection(orb_in)
{
//
// get host and port from environment variable TANGO_HOST
//
	char *tango_host_env_c_str;
	filedb = 0;
	serv_version = 0;
	
	ext = new DatabaseExt();

	if (get_tango_host_from_reg(&tango_host_env_c_str,ds_exec_name,ds_inst_name) == -1)
		tango_host_env_c_str = NULL;

	if (tango_host_env_c_str == NULL)
	{		
		TangoSys_MemStream desc;
		desc << "TANGO_HOST env. variable not set, set it and retry (e.g. TANGO_HOST=<host>:<port>)" << ends;
		ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       desc.str(),
					       (const char *)"Database::Database");
	}
	
	check_tango_host(tango_host_env_c_str);
	
	delete [] tango_host_env_c_str;

	cout4 <<"Database::Database(): TANGO host " << host << " port " << port << endl;

	build_connection();
}
#endif

//+----------------------------------------------------------------------------
//
// method : 		Database::check_tango_host()
// 
// description : 	Check the TANGO_HOST environment variable syntax and
//			extract database server host(s) and port(s) from
//			it
//
// in :	tango_host_env_c_str : The TANGO_HOST env. variable value		
//
//-----------------------------------------------------------------------------

void Database::check_tango_host(char *tango_host_env_c_str)
{
	filedb = 0;
	string tango_host_env(tango_host_env_c_str);
	string::size_type separator;

	db_multi_svc = false;
	separator = tango_host_env.find(',');
	if (separator != string::npos)
	{
	
//
// It is a multi db server system, extract each host and port and check 
// syntax validity (<host>:<port>,<host>:<port>)
//

		db_multi_svc = true;
		separator = 0;
		string::size_type old_sep = separator;
		string::size_type host_sep;
		while((separator = tango_host_env.find(',',separator + 1)) != string::npos)
		{
			string sub = tango_host_env.substr(old_sep,separator - old_sep);
			old_sep = separator + 1;
			host_sep = sub.find(":");
			if (host_sep != string::npos)
			{
				if ((host_sep == sub.size() - 1) || (host_sep == 0))
				{		
					TangoSys_MemStream desc;
					desc << "TANGO_HOST env. variable syntax incorrect (e.g. TANGO_HOST=<host>:<port>,<host>:<port>)" << ends;
					ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       				desc.str(),
					       				(const char *)"Database::Database");
				}
				multi_db_port.push_back(sub.substr(host_sep + 1));
			}
			else
			{		
				TangoSys_MemStream desc;
				desc << "TANGO_HOST env. variable syntax incorrect (e.g. TANGO_HOST=<host>:<port>,<host>:<port>)" << ends;
				ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       		desc.str(),
					       		(const char *)"Database::Database");
			}
			multi_db_host.push_back(sub.substr(0,host_sep));
		}
		string last = tango_host_env.substr(old_sep);
		host_sep = last.find(":");
		if (host_sep != string::npos)
		{
			if ((host_sep == last.size() - 1) || (host_sep == 0))
			{		
				TangoSys_MemStream desc;
				desc << "TANGO_HOST env. variable syntax incorrect (e.g. TANGO_HOST=<host>:<port>,<host>:<port>)" << ends;
				ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       				desc.str(),
					       				(const char *)"Database::Database");
			}
			multi_db_port.push_back(last.substr(host_sep + 1));
		}
		else
		{		
			TangoSys_MemStream desc;
			desc << "TANGO_HOST env. variable syntax incorrect (e.g. TANGO_HOST=<host>:<port>,<host>:<port>)" << ends;
			ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       		desc.str(),
					       		(const char *)"Database::Database");
		}
		multi_db_host.push_back(last.substr(0,host_sep));
		
		db_port = multi_db_port[0];
		db_host = multi_db_host[0];
		db_port_num = atoi(db_port.c_str());
	}
	else
	{

//
// Single database server case
//
			
		separator = tango_host_env.find(":");
		if (separator != string::npos)
		{
			if ((separator == tango_host_env.size() - 1) || (separator == 0))
			{		
				TangoSys_MemStream desc;
				desc << "TANGO_HOST env. variable syntax incorrect (e.g. TANGO_HOST=<host>:<port>)" << ends;
				ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       				desc.str(),
					       				(const char *)"Database::Database");
			}
			db_port = tango_host_env.substr(separator+1);
			db_port_num = atoi(db_port.c_str());
		}
		else
		{		
			TangoSys_MemStream desc;
			desc << "TANGO_HOST env. variable syntax incorrect (e.g. TANGO_HOST=<host>:<port>)" << ends;
			ApiConnExcept::throw_exception((const char *)"API_TangoHostNotSet",
					       		desc.str(),
					       		(const char *)"Database::Database");
		}
		db_host = tango_host_env.substr(0,separator);
	}
	
	host = db_host;
	port = db_port;
	port_num = db_port_num;
	
	dbase_used = true;
	from_env_var = true;

}


//-----------------------------------------------------------------------------
//
// Database::Database(string host, int port, ORB *orb) - constructor to 
//           create connection to TANGO Database with host, port, orb specified.
//
//-----------------------------------------------------------------------------

Database::Database(string &in_host, int in_port, ORB *orb_in) : Connection(orb_in)
{
	filedb = 0;
	serv_version = 0;
	db_multi_svc = false;
	
	ext = new DatabaseExt();
	
	host = in_host;
	db_host = host;
	
	TangoSys_MemStream o;
	o << in_port << ends;
#ifdef STRSTREAM
	char *ptr = o.str();
	port = ptr;
	delete [] ptr;
#else
	string st = o.str();
	port = st.c_str();
#endif

	db_port = port;
	
	db_port_num = in_port;
	port_num = in_port;
	from_env_var = false;
	dbase_used = true;
		
	build_connection();	
}

Database::Database(string &name) : Connection(true)
{	
	file_name = name;
	filedb = new FileDatabase(file_name);
	serv_version = 0;
	
	ext = new DatabaseExt();
}

//-----------------------------------------------------------------------------
//
// Database::~Database() - destructor to destroy connection to TANGO Database
//
//-----------------------------------------------------------------------------

Database::~Database()
{
	if (filedb != 0)
	{
		write_filedatabase();
		delete filedb;
	}
	
	if (ext != NULL)
		delete ext;
}

#ifdef _TG_WINDOWS_
//+----------------------------------------------------------------------------
//
// method : 		Util::get_tango_host_from_reg()
// 
// description : 	When the server is a Win32 service, it is not
//			possible to get the env. variable TANGO_HOST
//			value. In this case, the TANGO_HOST value is 
//			stored in the WIN32 registry when the service is
//			installed and retrieved by this method.
//
//-----------------------------------------------------------------------------


long Database::get_tango_host_from_reg(char **buffer,
				       string &ds_exec_name,
				       string &ds_instance_name)
{

//
// Build the key name
//

	HKEY regHandle = HKEY_LOCAL_MACHINE;
	string keyName("SYSTEM\\CurrentControlSet\\Services\\");
	keyName = keyName + ds_exec_name + '_' + ds_instance_name;
	keyName = keyName + "\\Server";

//
// Open the key.
//

	HKEY keyHandle;
	if(::RegOpenKeyEx(regHandle,keyName.c_str(),0,KEY_ALL_ACCESS,
			  &keyHandle) != ERROR_SUCCESS)
	{
		return -1;
	}

//
// Read the value
//

	DWORD type,size;
	char tmp_buf[128];
	size = 127;
	if (::RegQueryValueEx(keyHandle,"TangoHost",NULL,&type,
			      (unsigned char *)tmp_buf,
			      &size) != ERROR_SUCCESS)
	{
		return -1;
	}

//
// Store answer in caller parameter
//

	*buffer = new char[strlen(tmp_buf) + 1];
	strcpy(*buffer,tmp_buf);

	return 0;
}
#endif

//-----------------------------------------------------------------------------
//
// Database::connect() - method to create connection to TANGO Database
//                       specifying host and port
//
//-----------------------------------------------------------------------------

void Database::write_filedatabase()
{
	if (filedb != 0)
		filedb->write_file();
}

void Database::reread_filedatabase()
{
	if (filedb != 0)
		delete filedb;
	filedb = new FileDatabase(file_name);
}

void Database::build_connection()
{
	string corba_name = get_corba_name();
	omniORB::setClientConnectTimeout(CORBA::ULong(DB_CONNECT_TIMEOUT));
	try
	{
		connect(corba_name);
		omniORB::setClientConnectTimeout(0);
	}
	catch (Tango::DevFailed &e)
	{
		omniORB::setClientConnectTimeout(0);
		throw;
	}

	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_corba_name() - private method to return corba name
//
//-----------------------------------------------------------------------------

string Database::get_corba_name()
{
	string db_corbaloc;
	if (db_multi_svc == true)
	{
        db_corbaloc = "corbaloc:iiop:1.2@";
		int nb_host = multi_db_host.size();
		for (int i = 0;i <  nb_host;i++)
		{
			db_corbaloc = db_corbaloc + multi_db_host[i];
			db_corbaloc = db_corbaloc + ":" + multi_db_port[i];
			if (i != nb_host - 1)
				db_corbaloc = db_corbaloc + ",iiop:1.2@";
		}
		db_corbaloc = db_corbaloc + "/" + DbObjName;
	}
	else
	{
		db_corbaloc = "corbaloc:iiop:";
		db_corbaloc = db_corbaloc+host+":"+port;
		db_corbaloc = db_corbaloc+"/"+DbObjName;
	}

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
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	if (filedb != 0)
		received = filedb->DbInfo(send);
	else 
		CALL_DB_SERVER("DbInfo",send,received);
	const DevVarStringArray *db_info_list;
	received.inout() >>= db_info_list;

#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	for (unsigned int i=0; i<db_info_list->length(); i++)
	{
		ostream << (*db_info_list)[i].in() << endl;
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
	DeviceData received_cmd;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	const DevVarLongStringArray *dev_import_list;
	
	send <<= dev.c_str();
	if (filedb != 0)
	{
		received = filedb->DbImportDevice(send);
		received.inout() >>= dev_import_list;
	}
	else
	{
		DeviceData send_name;
		send_name << dev;
		CALL_DB_SERVER("DbImportDevice",send_name,received_cmd);
		received_cmd >> dev_import_list;
	}

	DbDevImportInfo dev_import;
	dev_import.exported = 0;
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
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
	string st = ostream.str();
	(*dev_export_list)[3] = string_dup(st.c_str());
#endif /* STRSTREAM */
	(*dev_export_list)[4] = string_dup(dev_export.version.c_str());
	send <<= dev_export_list;

	if (filedb != 0)
		filedb->DbExportDevice(send);
	else
		CALL_DB_SERVER_NO_RET("DbExportDevice",send);

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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= string_dup(dev.c_str());
	if (filedb != 0)
		filedb->DbUnExportDevice(send);
	else
		CALL_DB_SERVER_NO_RET("DbUnExportDevice",send);

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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);

	DevVarStringArray *dev_info_list = new DevVarStringArray;
	dev_info_list->length(3);
	(*dev_info_list)[0] = string_dup(dev_info.server.c_str());
	(*dev_info_list)[1] = string_dup(dev_info.name.c_str());
	(*dev_info_list)[2] = string_dup(dev_info._class.c_str());
	send <<= dev_info_list;
	if (filedb != 0)
		filedb->DbAddDevice(send);
	else
		CALL_DB_SERVER_NO_RET("DbAddDevice",send);

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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= dev.c_str();
	if (filedb != 0)
		filedb->DbDeleteDevice(send);
	else
		CALL_DB_SERVER_NO_RET("DbDeleteDevice",send);

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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	DevVarStringArray *dev_info_list = new DevVarStringArray;
	dev_info_list->length(2*dev_infos.size()+1);
	(*dev_info_list)[0] = string_dup(server.c_str());
	for (unsigned int i=0; i<dev_infos.size(); i++)
	{
		(*dev_info_list)[i*2+1] = string_dup(dev_infos[i].name.c_str());
		(*dev_info_list)[i*2+2] = string_dup(dev_infos[i]._class.c_str());
	}
	send <<= dev_info_list;
	if (filedb != 0)
		filedb->DbAddServer(send);
	else
		CALL_DB_SERVER_NO_RET("DbAddServer",send);

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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= string_dup(server.c_str());
	if (filedb != 0)
		filedb->DbDeleteServer(send);
	else
		CALL_DB_SERVER_NO_RET("DbDeleteServer",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::export_server() - public method to export a server to the Database
//
//-----------------------------------------------------------------------------

void Database::export_server(DbDevExportInfos& dev_export)
{
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *dev_export_list = new DevVarStringArray;
	dev_export_list->length(5*dev_export.size());
#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	for (unsigned int i=0; i<dev_export.size(); i++)
	{
		(*dev_export_list)[i*5] = string_dup(dev_export[i].name.c_str());
		(*dev_export_list)[i*5+1] = string_dup(dev_export[i].ior.c_str());
		(*dev_export_list)[i*5+2] = string_dup(dev_export[i].host.c_str());
		ostream << dev_export[i].pid << ends;
#ifdef STRSTREAM
		(*dev_export_list)[i*5+3] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		string st = ostream.str();
		(*dev_export_list)[i*5+3] = string_dup(st.c_str());
#endif /* STRSTREAM */
		(*dev_export_list)[i*5+4] = string_dup(dev_export[i].version.c_str());
	}
	send <<= dev_export_list;
	
	if (filedb != 0)
		filedb->DbExportServer(send);
	else
		CALL_DB_SERVER_NO_RET("DbExportServer",send);

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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= server.c_str();
	if (filedb != 0)
		filedb->DbUnExportServer(send);
	else
		CALL_DB_SERVER_NO_RET("DbUnExportServer",send);

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
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= server.c_str();
	if (filedb != 0)
		received = filedb->DbGetServerInfo(send);
	else 
		CALL_DB_SERVER("DbGetServerInfo",send,received);

	const DevVarStringArray *server_info_list;
	received.inout() >>= server_info_list;

	DbServerInfo server_info;
	server_info.name = string((*server_info_list)[0]);
	server_info.host = string((*server_info_list)[1]);
	server_info.mode = atoi((*server_info_list)[2]);
	server_info.level = atoi((*server_info_list)[3]);

	return(server_info);
}
//-----------------------------------------------------------------------------
//
// Database::get_device_property() - public method to get a device property from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_device_property(string dev, DbData &db_data,DbServerCache *db_cache)
{
	unsigned int i;
	Any_var received;
	const DevVarStringArray *property_values;
	
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(dev.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	
	if (db_cache == NULL)
	{

//
// Get property(ies) from DB server
//

		AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
		Any send;
		
		send <<= property_names;
	
		if (filedb != 0)
			received = filedb->DbGetDeviceProperty(send);
		else
			CALL_DB_SERVER("DbGetDeviceProperty",send,received);
		received.inout() >>= property_values;
	}
	else
	{

//
// Try to get property(ies) from cache
//

		try
		{
			property_values = db_cache->get_dev_property(property_names);
			delete property_names;
		}
		catch(Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"DB_DeviceNotFoundInCache") == 0)
			{

//
// The device is not defined in the cache, call DB server
//

				AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
				Any send;
		
				send <<= property_names;
	
				if (filedb != 0)
					received = filedb->DbGetDeviceProperty(send);
				else
					CALL_DB_SERVER("DbGetDeviceProperty",send,received);
				received.inout() >>= property_values;
			}
			else
			{
				delete property_names;
				throw;
			}
		}
	}

	unsigned int n_props, index;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in() << ends;
	iostream >> n_props;
	index = 2;
	for (i=0; i<n_props; i++)
	{
		int n_values;
		db_data[i].name = (*property_values)[index].in(); index++;	
		iostream.seekp (0);  iostream.seekg (0); iostream.clear();
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
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
	string st = ostream.str();
	(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		ostream.seekp (0); ostream.clear();
		ostream << db_data[i].size() << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		ostream << ends;
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		string st = ostream.str();
		(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		for (int j=0; j<db_data[i].size(); j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i].value_string[j].c_str());
		}
	}
	send <<= property_values;

	if (filedb != 0)
		filedb->DbPutDeviceProperty(send);
	else
		CALL_DB_SERVER_NO_RET("DbPutDeviceProperty",send);
		
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(dev.c_str());
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	if (filedb != 0)
		filedb->DbDeleteDeviceProperty(send);
	else 
		CALL_DB_SERVER_NO_RET("DbDeleteDeviceProperty",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_attribute_property() - public method to get device 
//					       attribute properties from
//                                             the Database
//
//-----------------------------------------------------------------------------

void Database::get_device_attribute_property(string dev, DbData &db_data, DbServerCache *db_cache)
{
	unsigned int i,j;
	Any_var received;
	const DevVarStringArray *property_values;
	
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(dev.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	
	if (db_cache == NULL)
	{

//
// Get propery(ies) from DB server
//

		AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
		Any send;
		
		send <<= property_names;

		if (filedb != 0)
		{
			received = filedb->DbGetDeviceAttributeProperty(send);
			serv_version = 230;
		}
		else
		{
			try
			{
				if ((serv_version == 0) || (serv_version >= 230))
				{
					CALL_DB_SERVER("DbGetDeviceAttributeProperty2",send,received);
					serv_version = 230;
				}
				else
					CALL_DB_SERVER("DbGetDeviceAttributeProperty",send,received);
			}
			catch (Tango::DevFailed &e)
			{
				if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") == 0)
				{
					CALL_DB_SERVER("DbGetDeviceAttributeProperty",send,received);
					serv_version = 210;
				}
				else
					throw;
			}
		}
		received.inout() >>= property_values;
	}
	else
	{

//
// Try  to get property(ies) from cache
//

		serv_version = 230;
		try
		{
			property_values = db_cache->get_dev_att_property(property_names);
			delete property_names;
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"DB_DeviceNotFoundInCache") == 0)
			{

//
// The device is not in cache, ask the db server
//

				AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
				Any send;
		
				send <<= property_names;

				if (filedb != 0)
				{
					received = filedb->DbGetDeviceAttributeProperty(send);
					serv_version = 230;
				}
				else
				{
					try
					{
						if ((serv_version == 0) || (serv_version >= 230))
						{
							CALL_DB_SERVER("DbGetDeviceAttributeProperty2",send,received);
							serv_version = 230;
						}
						else
							CALL_DB_SERVER("DbGetDeviceAttributeProperty",send,received);
					}
					catch (Tango::DevFailed &e)
					{
						if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") == 0)
						{
							CALL_DB_SERVER("DbGetDeviceAttributeProperty",send,received);
							serv_version = 210;
						}
						else
							throw;
					}
				}
				received.inout() >>= property_values;
			}
			else
			{
				delete property_names;
				throw;
			}
		}
	}
	
	unsigned int n_attribs, index;
	int i_total_props;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in() << ends;
	iostream >> n_attribs;
	index = 2;
	if (serv_version < 230)
		db_data.resize((property_values->length())/2 -1);

	i_total_props = 0;
	
	if (serv_version < 230)
	{
		for (i=0; i<n_attribs; i++)
		{
			unsigned short n_props;
			db_data[i_total_props].name = (*property_values)[index]; index++;	
			iostream.seekp(0); iostream.seekg(0); iostream.clear();
			iostream << (*property_values)[index].in() << ends; 
			iostream >> n_props;
			db_data[i_total_props] << n_props; i_total_props++;
			index++;
			for (j=0; j<n_props; j++)
			{
				db_data[i_total_props].name = (*property_values)[index]; 
				index++;
				db_data[i_total_props].value_string.resize(1);
				db_data[i_total_props].value_string[0] = (*property_values)[index]; 
				index++; i_total_props++;
			}
		}
	}
	else
	{
		long old_size = 0;
		for (i=0; i<n_attribs; i++)
		{
			old_size++;
			db_data.resize(old_size);
			unsigned short n_props;
			db_data[i_total_props].name = (*property_values)[index]; index++;	
			iostream.seekp(0); iostream.seekg(0); iostream.clear();
			iostream << (*property_values)[index].in() << ends; 
			iostream >> n_props;
			db_data[i_total_props] << n_props;
			db_data.resize(old_size + n_props);
			old_size = old_size + n_props;
			i_total_props++;
			index++;
			for (j=0; j<n_props; j++)
			{
				db_data[i_total_props].name = (*property_values)[index]; 
				index++;
				int n_values;
				iostream.seekp (0);  iostream.seekg (0); iostream.clear();
				iostream << (*property_values)[index].in() << ends; 
				iostream >> n_values;
				index++;

				db_data[i_total_props].value_string.resize(n_values);
				if (n_values == 0)
				{
					index++; // skip dummy returned value " "
				}
				else
				{
					for (int k=0; k<n_values; k++)
					{
						db_data[i_total_props].value_string[k] = (*property_values)[index].in(); 
						index++;
					}
				}
				i_total_props++;
			}
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	int index, n_attribs ;
	bool retry = true;

	while (retry == true)
	{	
		DevVarStringArray *property_values = new DevVarStringArray;
		property_values->length(2); index = 2;
		(*property_values)[0] = string_dup(dev.c_str());
		n_attribs = 0;

		if ((serv_version == 0) || (serv_version >= 230))
		{
			for (unsigned int i=0; i<db_data.size();)
			{
				short n_props;
				index++; property_values->length(index);
				(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
				db_data[i] >> n_props;
				ostream.seekp(0); ostream.clear();
				ostream << n_props << ends;
				index++; property_values->length(index);
#ifdef STRSTREAM
				(*property_values)[index-1] = string_dup(ostream.str());
				ostream.rdbuf()->freeze(false);
#else
				string st = ostream.str();
				(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
				for (int j=0; j<n_props; j++)
				{
					index++; property_values->length(index);
					(*property_values)[index-1] = string_dup(db_data[i+j+1].name.c_str());
					index++; property_values->length(index);
					int prop_size = db_data[i+j+1].size();
					ostream.seekp(0); ostream.clear();
					ostream << prop_size << ends;
#ifdef STRSTREAM
					(*property_values)[index-1] = string_dup(ostream.str());
					ostream.rdbuf()->freeze(false);
#else
					string st = ostream.str();
					(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
					property_values->length(index + prop_size);
					for (int q=0; q<prop_size; q++)
					{
						(*property_values)[index+q] = string_dup(db_data[i+j+1].value_string[q].c_str());
					}
					index = index + prop_size;
				}
				i = i+n_props+1;
				n_attribs++;
			}

			ostream.seekp(0); ostream.clear();
			ostream << n_attribs << ends;
#ifdef STRSTREAM
			(*property_values)[1] = string_dup(ostream.str());
			ostream.rdbuf()->freeze(false);
#else
			string st = ostream.str();
			(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		}
		else
		{
			for (unsigned int i=0; i<db_data.size();)
			{
				short n_props;
				index++; property_values->length(index);
				(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
				db_data[i] >> n_props;
				ostream.seekp(0); ostream.clear();
				ostream << n_props << ends;
				index++; property_values->length(index);
#ifdef STRSTREAM
				(*property_values)[index-1] = string_dup(ostream.str());
				ostream.rdbuf()->freeze(false);
#else
				string st = ostream.str();
				(*property_values)[index-1] = string_dup(st.c_str());
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
			ostream.seekp(0); ostream.clear();
			ostream << n_attribs << ends;
#ifdef STRSTREAM
			(*property_values)[1] = string_dup(ostream.str());
			ostream.rdbuf()->freeze(false);
#else
			string st = ostream.str();
			(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		}
		send <<= property_values;


		if (filedb != 0)
		{
			filedb->DbPutDeviceAttributeProperty(send);
			serv_version = 230;
			retry = false;
		}
		else
		{
			try
			{
				if ((serv_version == 0) || (serv_version >= 230))
				{
					CALL_DB_SERVER_NO_RET("DbPutDeviceAttributeProperty2",send);
					serv_version = 230;
				}
				else
					CALL_DB_SERVER_NO_RET("DbPutDeviceAttributeProperty",send);
				retry = false;
			}
			catch (Tango::DevFailed &e)
			{
				if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") != 0)
					throw;
				else
					serv_version = 210;
			}
		}
	}
		
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
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *property_values = new DevVarStringArray;
	unsigned int nb_prop = db_data.size() - 1;
	property_values->length(nb_prop + 2);
	(*property_values)[0] = string_dup(dev.c_str());
	(*property_values)[1] = string_dup(db_data[0].name.c_str());
	for (unsigned int i=0; i < nb_prop;i++)
		(*property_values)[i + 2] = string_dup(db_data[i + 1].name.c_str());

	send <<= property_values;

	if (filedb != 0)
		filedb->DbDeleteDeviceAttributeProperty(send);
	else 
		CALL_DB_SERVER_NO_RET("DbDeleteDeviceAttributeProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_class_property() - public method to get class properties from
//                                   the Database
//
//-----------------------------------------------------------------------------

void Database::get_class_property(string device_class, DbData &db_data, DbServerCache *db_cache)
{
	unsigned int i;
	const DevVarStringArray *property_values;
	Any_var received;

//
// Parameters for db server call
//
	
	DevVarStringArray *property_names = new DevVarStringArray;
	
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(device_class.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}

//
// Call db server or get data from cache
//
	
	if (db_cache != NULL)
	{

//
// Try to get property(ies) from cache
//

		try
		{
			property_values = db_cache->get_class_property(property_names);
			delete property_names;
		}
		catch(Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"DB_ClassNotFoundInCache") == 0)
			{

//
// The class is not defined in cache, try in DB server
//

				Any send;
				AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
				send <<= property_names;

				if (filedb != 0)
					received = filedb->DbGetClassProperty(send);
				else 
					CALL_DB_SERVER("DbGetClassProperty",send,received);

				received.inout() >>= property_values;
			}
			else
			{
				delete property_names;
				throw;
			}
		}
	}
	else
	{

//
// Call DB server
//

		Any send;
		AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
		send <<= property_names;

		if (filedb != 0)
			received = filedb->DbGetClassProperty(send);
		else 
			CALL_DB_SERVER("DbGetClassProperty",send,received);

		received.inout() >>= property_values;
	}

//
// Build data returned to caaller from those received from db server or from cache
//
	
	unsigned int n_props, index;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in() << ends;
	iostream >> n_props;
	index = 2;
	for (i=0; i<n_props; i++)
	{
		int n_values;
		db_data[i].name = (*property_values)[index]; index++;	
		iostream.seekp(0); iostream.seekg(0); iostream.clear();
		iostream << (*property_values)[index].in() << ends; index++;
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	int index;
	
	DevVarStringArray *property_values = new DevVarStringArray;
	property_values->length(2); index = 2;
	(*property_values)[0] = string_dup(device_class.c_str());
	ostream << db_data.size() << ends;
#ifdef STRSTREAM
	(*property_values)[1] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	string st = ostream.str();
	(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		ostream.seekp (0); ostream.clear();
		ostream << db_data[i].size() << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		string st = ostream.str();
		(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		for (int j=0; j<db_data[i].size(); j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i].value_string[j].c_str());
		}
	}
	send <<= property_values;

	if (filedb != 0)
		filedb->DbPutClassProperty(send);
	else
		CALL_DB_SERVER_NO_RET("DbPutClassProperty",send);
		
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(device_class.c_str());
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	if (filedb != 0)
		filedb->DbDeleteClassProperty(send);
	else
		CALL_DB_SERVER_NO_RET("DbDeleteClassProperty",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_class_attribute_property() - public method to get class attribute
//	     properties from the Database
//
//-----------------------------------------------------------------------------

void Database::get_class_attribute_property(string device_class, DbData &db_data, DbServerCache *db_cache)
{
	unsigned int i;
	Any_var received;
	const DevVarStringArray *property_values;
	
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(device_class.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	
	if (db_cache == NULL)
	{

//
// Get property(ies) from DB server
//

		Any send;
		AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
		
		send <<= property_names;

		if (filedb != 0)
		{
			received = filedb->DbGetClassAttributeProperty(send);
			serv_version = 230;
		}
		else
		{
			try
			{
				if ((serv_version == 0) || (serv_version >= 230))
				{
					CALL_DB_SERVER("DbGetClassAttributeProperty2",send,received);
					serv_version = 230;
				}
				else
					CALL_DB_SERVER("DbGetClassAttributeProperty",send,received);
			}
			catch (Tango::DevFailed &e)
			{
				if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") == 0)
				{
					CALL_DB_SERVER("DbGetClassAttributeProperty",send,received);
					serv_version = 210;
				}
				else
					throw;
			}
		}
		received.inout() >>= property_values;
	}
	else
	{

//
// Try to get property(ies) from cache
//

		serv_version = 230;
		try
		{
			property_values = db_cache->get_class_att_property(property_names);
			delete property_names;
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"DB_ClassNotFoundInCache") == 0)
			{

//
// The class is not defined in cache, get property(ies) from DB server
//

				Any send;
				AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
		
				send <<= property_names;

				if (filedb != 0)
				{
					received = filedb->DbGetClassAttributeProperty(send);
					serv_version = 230;
				}
				else
				{
					try
					{
						if ((serv_version == 0) || (serv_version >= 230))
						{
							CALL_DB_SERVER("DbGetClassAttributeProperty2",send,received);
							serv_version = 230;
						}
						else
							CALL_DB_SERVER("DbGetClassAttributeProperty",send,received);
					}
					catch (Tango::DevFailed &e)
					{
						if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") == 0)
						{
							CALL_DB_SERVER("DbGetClassAttributeProperty",send,received);
							serv_version = 210;
						}
						else
							throw;
					}
				}
				received.inout() >>= property_values;
			}
			else
			{
				delete property_names;
				throw;
			}
		}
	}
	

	unsigned int n_attribs, index;
	int i_total_props;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */

	iostream << (*property_values)[1].in() << ends;
	iostream >> n_attribs;
	index = 2;
	if (serv_version < 230)
		db_data.resize((property_values->length())/2 - 1);
	i_total_props = 0;
	
	if (serv_version < 230)
	{
		for (i=0; i<n_attribs; i++)
		{
			short n_props;
			db_data[i_total_props].name = (*property_values)[index]; index++;	
			iostream.seekp(0); iostream.seekg(0); iostream.clear();
			iostream << (*property_values)[index].in() << ends; index++;
			iostream >> n_props;
			db_data[i_total_props] << n_props; i_total_props++;
			for (int j=0; j<n_props; j++)
			{
				db_data[i_total_props].name = (*property_values)[index]; 
				index++;
				db_data[i_total_props].value_string.resize(1);
				db_data[i_total_props].value_string[0] = (*property_values)[index]; 
				index++; i_total_props++;
			}
		}
	}
	else
	{
		long old_size = 0;
		for (i=0; i<n_attribs; i++)
		{
			old_size++;
			db_data.resize(old_size);
			short n_props;
			db_data[i_total_props].name = (*property_values)[index]; index++;	
			iostream.seekp(0); iostream.seekg(0); iostream.clear();
			iostream << (*property_values)[index].in() << ends;
			iostream >> n_props;
			db_data[i_total_props] << n_props;
			db_data.resize(old_size + n_props);
			old_size = old_size + n_props;
			i_total_props++;
			index++;
			for (int j=0; j<n_props; j++)
			{
				db_data[i_total_props].name = (*property_values)[index]; 
				index++;
				int n_values;
				iostream.seekp(0); iostream.seekg(0); iostream.clear();
				iostream << (*property_values)[index].in() << ends;
				iostream >> n_values;
				index++;

				db_data[i_total_props].value_string.resize(n_values);
				if (n_values == 0)
				{
					index++; // skip dummy returned value ""
				}
				else
				{
					for (int k=0; k<n_values; k++)
					{
						db_data[i_total_props].value_string[k] = (*property_values)[index].in(); 
						index++;
					}
				}
				i_total_props++;
			}
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	int index, n_attribs;
	bool retry = true;
	
	while (retry == true)
	{
		DevVarStringArray *property_values = new DevVarStringArray;
		property_values->length(2); index = 2;
		(*property_values)[0] = string_dup(device_class.c_str());
		n_attribs = 0;
		
		if ((serv_version == 0) || (serv_version >= 230))
		{
			for (unsigned int i=0; i<db_data.size(); )
			{
				short n_props;
				index++; property_values->length(index);
				(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
				db_data[i] >> n_props;
				ostream.seekp(0); ostream.clear();
				ostream << n_props << ends;
				index++; property_values->length(index);
#ifdef STRSTREAM
				(*property_values)[index-1] = string_dup(ostream.str());
				ostream.rdbuf()->freeze(false);
#else
				string st = ostream.str();
				(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
				for (int j=0; j<n_props; j++)
				{
					index++; property_values->length(index);
					(*property_values)[index-1] = string_dup(db_data[i+j+1].name.c_str());
					index++; property_values->length(index);
					int prop_size = db_data[i+j+1].size();
					ostream.seekp(0); ostream.clear();
					ostream << prop_size << ends;
#ifdef STRSTREAM
					(*property_values)[index-1] = string_dup(ostream.str());
					ostream.rdbuf()->freeze(false);
#else
					string st = ostream.str();
					(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
					property_values->length(index + prop_size);
					for (int q=0; q<prop_size; q++)
					{
						(*property_values)[index+q] = string_dup(db_data[i+j+1].value_string[q].c_str());
					}
					index = index + prop_size;
				}
				i = i+n_props+1;
				n_attribs++;
			}
	
			ostream.seekp(0); ostream.clear();
			ostream << n_attribs << ends;
#ifdef STRSTREAM
			(*property_values)[1] = string_dup(ostream.str());
			ostream.rdbuf()->freeze(false);
#else
			string st = ostream.str();
			(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		}
		else
		{
			for (unsigned int i=0; i<db_data.size(); )
			{
				short n_props;
				index++; property_values->length(index);
				(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
				db_data[i] >> n_props;
				ostream.seekp(0); ostream.clear();
				ostream << n_props << ends;
				index++; property_values->length(index);
#ifdef STRSTREAM
				(*property_values)[index-1] = string_dup(ostream.str());
				ostream.rdbuf()->freeze(false);
#else
				string st = ostream.str();
				(*property_values)[index-1] = string_dup(st.c_str());
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
			ostream.seekp(0); ostream.clear();
			ostream << n_attribs << ends;
#ifdef STRSTREAM
			(*property_values)[1] = string_dup(ostream.str());
			ostream.rdbuf()->freeze(false);
#else
			string st = ostream.str();
			(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		}
		send <<= property_values;

		if (filedb != 0)
		{
			filedb->DbPutClassAttributeProperty(send);
			serv_version = 230;
			retry = false;
		}
		else
		{
			try
			{
				if ((serv_version == 0) || (serv_version >= 230))
				{
					CALL_DB_SERVER_NO_RET("DbPutClassAttributeProperty2",send);
					serv_version = 230;
				}
				else
					CALL_DB_SERVER_NO_RET("DbPutClassAttributeProperty",send);
				retry = false;
			}
			catch (Tango::DevFailed &e)
			{
				if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") != 0)
					throw;
				else
					serv_version = 210;
			}
		}
	}
		
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
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *property_values = new DevVarStringArray;
	unsigned int nb_prop = db_data.size() - 1;
	property_values->length(nb_prop + 2);
	(*property_values)[0] = string_dup(device_class.c_str());
	(*property_values)[1] = string_dup(db_data[0].name.c_str());
	for (unsigned int i=0; i < nb_prop;i++)
		(*property_values)[i + 2] = string_dup(db_data[i + 1].name.c_str());

	send <<= property_values;

	if (filedb != 0)
		filedb->DbDeleteClassAttributeProperty(send);
	else 
		CALL_DB_SERVER_NO_RET("DbDeleteClassAttributeProperty",send);
		
	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_name() - public method to get device names from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_name(string &device_server, string &device_class, DbServerCache *db_cache)
{
	Any_var received;
	const DevVarStringArray *device_names;
	
	DevVarStringArray *device_server_class = new DevVarStringArray;
	device_server_class->length(2);
	(*device_server_class)[0] = string_dup(device_server.c_str());
	(*device_server_class)[1] = string_dup(device_class.c_str());
	
	if (db_cache == NULL)
	{
		Any send;
		AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
		
		send <<= device_server_class;

		if (filedb != 0)
			received = filedb->DbGetDeviceList(send);
		else 
			CALL_DB_SERVER("DbGetDeviceList",send,received);
		received.inout() >>= device_names;
	}
	else
	{
		device_names = db_cache->get_dev_list(device_server_class);
		delete device_server_class;
	}

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
// Database::get_device_exported() - public method to get exported device names from
//                               the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_exported(string &filter)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= filter.c_str();

	if (filedb != 0)
		received = filedb->DbGetDeviceExportedList(send);
	else 
		CALL_DB_SERVER("DbGetDeviceExportedList",send,received);
	const DevVarStringArray *device_names;
	received.inout() >>= device_names;

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
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= wildcard.c_str();

	if (filedb != 0)
		received = filedb->DbGetDeviceMemberList(send);
	else
		CALL_DB_SERVER("DbGetDeviceMemberList",send,received);
	const DevVarStringArray *device_member;
	received.inout() >>= device_member;

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
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= wildcard.c_str();

	if (filedb != 0)
		received = filedb->DbGetDeviceFamilyList(send);
	else 
		CALL_DB_SERVER("DbGetDeviceFamilyList",send,received);
	const DevVarStringArray *device_family;
	received.inout() >>= device_family;

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
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= wildcard.c_str();

	if (filedb != 0)
		received = filedb->DbGetDeviceDomainList(send);
	else 
		CALL_DB_SERVER("DbGetDeviceDomainList",send,received);
	const DevVarStringArray *device_domain;
	received.inout() >>= device_domain;

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

void Database::get_property(string obj, DbData &db_data, DbServerCache *db_cache)
{
	unsigned int i;
	Any_var received;
	const DevVarStringArray *property_values;
	
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(obj.c_str());
	for (i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}

	if (db_cache == NULL)
	{	
		AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
		Any send;
		
		send <<= property_names;

		if (filedb != 0)
			received = filedb->DbGetProperty(send);
		else 
			CALL_DB_SERVER("DbGetProperty",send,received);
		received.inout() >>= property_values;
	}
	else
	{
	
//
// Try to get property(ies) from cache
//

		try
		{
			property_values = db_cache->get_obj_property(property_names);
			delete property_names;
		}
		catch(Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"DB_DeviceNotFoundInCache") == 0)
			{

//
// The object is not defined in the cache, call DB server
//

				AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
				Any send;
		
				send <<= property_names;

				if (filedb != 0)
					received = filedb->DbGetDeviceProperty(send);
				else
					CALL_DB_SERVER("DbGetDeviceProperty",send,received);
				received.inout() >>= property_values;
			}
			else
			{
				delete property_names;
				throw;
			}
		}
	}

	unsigned int n_props, index;
#ifdef STRSTREAM
	strstream iostream; 
#else
	stringstream iostream;
#endif /* STRSTREAM */
	iostream << (*property_values)[1].in() << ends;
	iostream >> n_props;
	index = 2;
	for (i=0; i<n_props; i++)
	{
		int n_values;
		db_data[i].name = (*property_values)[index]; index++;	
		iostream.seekp (0);  iostream.seekg (0); iostream.clear();
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
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
	string st = ostream.str();
	(*property_values)[1] = string_dup(st.c_str());
#endif /* STRSTREAM */
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		index++; property_values->length(index);
		(*property_values)[index-1] = string_dup(db_data[i].name.c_str());
		ostream.seekp (0); ostream.clear();
		ostream << db_data[i].size() << ends;
		index++; property_values->length(index);
#ifdef STRSTREAM
		ostream << ends;
		(*property_values)[index-1] = string_dup(ostream.str());
		ostream.rdbuf()->freeze(false);
#else
		string st = ostream.str();
		(*property_values)[index-1] = string_dup(st.c_str());
#endif /* STRSTREAM */
		for (int j=0; j<db_data[i].size(); j++)
		{
			index++; property_values->length(index);
			(*property_values)[index-1] = string_dup(db_data[i].value_string[j].c_str());
		}
	}
	send <<= property_values;

	if (filedb != 0)
		filedb->DbPutProperty(send);
	else 
		CALL_DB_SERVER_NO_RET("DbPutProperty",send);
		
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
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *property_names = new DevVarStringArray;
	property_names->length(db_data.size()+1);
	(*property_names)[0] = string_dup(obj.c_str());
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		(*property_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= property_names;

	if (filedb != 0)
		filedb->DbDeleteProperty(send);
	else 
		CALL_DB_SERVER_NO_RET("DbDeleteProperty",send);

	return;
}


//-----------------------------------------------------------------------------
//
// Database::get_device_alias() - public method to get device name from
//                               its alias
//
//-----------------------------------------------------------------------------

void Database::get_device_alias(string alias,string &dev_name)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= alias.c_str();

	if (filedb != 0)
		received = filedb->DbGetAliasDevice(send);
	else 
		CALL_DB_SERVER("DbGetAliasDevice",send,received);
	const char *dev_name_tmp;
	received.inout() >>= dev_name_tmp;
	dev_name = dev_name_tmp;
}
 
//-----------------------------------------------------------------------------
//
// Database::get_alias() - public method to get alias name from
//                         device name
//
//-----------------------------------------------------------------------------

void Database::get_alias(string dev_name,string &alias_name)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= dev_name.c_str();

	if (filedb != 0)
		received = filedb->DbGetDeviceAlias(send);
	else 
		CALL_DB_SERVER("DbGetDeviceAlias",send,received);
	const char *dev_name_tmp;
	received.inout() >>= dev_name_tmp;
	alias_name = dev_name_tmp;
}
 
//-----------------------------------------------------------------------------
//
// Database::get_attribute_alias() - public method to get attribute name from
//                                   its alias
//
//-----------------------------------------------------------------------------

void Database::get_attribute_alias(string  attr_alias, string &attr_name)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= attr_alias.c_str();

	if (filedb != 0)
		received = filedb->DbGetAttributeAlias(send);
	else 
		CALL_DB_SERVER("DbGetAttributeAlias",send,received);
	const char* attr_name_tmp;
	received.inout() >>= attr_name_tmp;
	attr_name = attr_name_tmp;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_alias_list() - public method to get device alias list.
//				       Wildcard (*) is suported
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_alias_list(string &alias)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= alias.c_str();

	if (filedb != 0)
		received = filedb->DbGetDeviceAliasList(send);
	else 
		CALL_DB_SERVER("DbGetDeviceAliasList",send,received);
	const DevVarStringArray *alias_array;
	received.inout() >>= alias_array;

	DbDatum db_datum;
	int n_aliases;
	n_aliases = alias_array->length();
	db_datum.name = alias;
	db_datum.value_string.resize(n_aliases);
	for (int i=0; i<n_aliases; i++)
	{
		db_datum.value_string[i] = (*alias_array)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::get_attribute_alias_list() - public method to get attribute alias list
//				          Wildcard (*) is suported
//
//-----------------------------------------------------------------------------

DbDatum Database::get_attribute_alias_list(string &alias)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= alias.c_str();

	if (filedb != 0)
		received = filedb->DbGetAttributeAliasList(send);
	else 
		CALL_DB_SERVER("DbGetAttributeAliasList",send,received);
	const DevVarStringArray *alias_array;
	received.inout() >>= alias_array;

	DbDatum db_datum;
	int n_aliases;
	n_aliases = alias_array->length();
	db_datum.name = alias;
	db_datum.value_string.resize(n_aliases);
	for (int i=0; i<n_aliases; i++)
	{
		db_datum.value_string[i] = (*alias_array)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::make_string_array() - Build a string array DbDatum from received
//
//-----------------------------------------------------------------------------

DbDatum Database::make_string_array(string name,Any_var &received) {

	const DevVarStringArray *prop_list;
	DbDatum db_datum;
	int n_props;
	
	received.inout() >>= prop_list;
	n_props = prop_list->length();
	db_datum.name = name;
	db_datum.value_string.resize(n_props);
	for (int i=0; i<n_props; i++)
	{
		db_datum.value_string[i] = (*prop_list)[i]; 
	}
		
	return db_datum;

}

//-----------------------------------------------------------------------------
//
// Database::get_device_property_list() - public method to get a device property 
//					  list from the Database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_property_list(string &dev, string &wildcard)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *sent_names = new DevVarStringArray;
	sent_names->length(2);
	(*sent_names)[0] = string_dup(dev.c_str());
	(*sent_names)[1] = string_dup(wildcard.c_str());
	
	send <<= sent_names;

	CALL_DB_SERVER("DbGetDevicePropertyList",send,received);
	
	return make_string_array(dev,received);
}

//-----------------------------------------------------------------------------
//
// Database::get_host_list() - Query the database for a list of host registered.
//
//-----------------------------------------------------------------------------

DbDatum Database::get_host_list()
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= "*";

	CALL_DB_SERVER("DbGetHostList",send,received);
	
	return make_string_array(string("host"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_host_list() - Query the database for a list of host registred.
//
//-----------------------------------------------------------------------------

DbDatum Database::get_host_list(string &wildcard)
{
	Any send;
	Any_var received;
	send <<= wildcard.c_str();

	CALL_DB_SERVER("DbGetHostList",send,received);
	
	return make_string_array(string("host"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_server_class_list() - Query the database for a list of classes 
//                                     instancied for a server
//
//-----------------------------------------------------------------------------

DbDatum Database::get_server_class_list(string &servname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= servname.c_str();

	CALL_DB_SERVER("DbGetDeviceServerClassList",send,received);
	
	const DevVarStringArray *prop_list;
	received.inout() >>= prop_list;
	
	// Extract the DServer class

	DbDatum db_datum;
	int n_props;
	int nb_classes;
	n_props = prop_list->length();
	if(n_props == 0 )
	  nb_classes = 0;
	else
	  nb_classes = n_props - 1;
		
	db_datum.name = servname;
	db_datum.value_string.resize(nb_classes);
	for (int i=0,j=0; i<n_props; i++)
	{
		if( strcmp( (*prop_list)[i] , "DServer" ) !=0 )
			db_datum.value_string[j++] = (*prop_list)[i]; 
	}
		
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::get_server_name_list() - Query the database for a list of server 
//                                    names registred in the database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_server_name_list()
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= "*";

	CALL_DB_SERVER("DbGetServerNameList",send,received);
	
	return make_string_array(string("server"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_instance_name_list() - Query the database for a list of instance 
//                                      names registred for specified server name
//
//-----------------------------------------------------------------------------

DbDatum Database::get_instance_name_list(string &servname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= servname.c_str();

	CALL_DB_SERVER("DbGetInstanceNameList",send,received);
	
	return make_string_array(servname,received);
}

//-----------------------------------------------------------------------------
//
// Database::get_server_list() - Query the database for a list of servers registred 
//                               in the database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_server_list()
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= "*";

	CALL_DB_SERVER("DbGetServerList",send,received);
	
	return make_string_array(string("server"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_server_list() - Query the database for a list of servers registred 
//                               in the database
//
//-----------------------------------------------------------------------------

DbDatum Database::get_server_list(string &wildcard)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= wildcard.c_str();

	CALL_DB_SERVER("DbGetServerList",send,received);
	
	return make_string_array(string("server"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_host_server_list() - Query the database for a list of servers 
//                                    registred on the specified host
//
//-----------------------------------------------------------------------------

DbDatum Database::get_host_server_list(string &hostname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= hostname.c_str();

	CALL_DB_SERVER("DbGetHostServerList",send,received);
	
	return make_string_array(string("server"),received);
}

//-----------------------------------------------------------------------------
//
// Database::put_server_info() - Add/update server information in databse
//
//-----------------------------------------------------------------------------

void Database::put_server_info(DbServerInfo &info)
{

#ifdef STRSTREAM
	ostrstream ostream;
#else
	ostringstream ostream;
#endif /* STRSTREAM */
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	DevVarStringArray *serv_info = new DevVarStringArray;
	
	serv_info->length(4);
	(*serv_info)[0] = string_dup(info.name.c_str());
	(*serv_info)[1] = string_dup(info.host.c_str());

	ostream.seekp(0); ostream.clear();
	ostream << info.mode << ends;
#ifdef STRSTREAM
	(*serv_info)[2] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	string st = ostream.str();
	(*serv_info)[2] = string_dup(st.c_str());
#endif /* STRSTREAM */

	ostream.seekp(0); ostream.clear();
	ostream << info.level << ends;
#ifdef STRSTREAM
	(*serv_info)[3] = string_dup(ostream.str());
	ostream.rdbuf()->freeze(false);
#else
	st = ostream.str();
	(*serv_info)[3] = string_dup(st.c_str());
#endif /* STRSTREAM */
	
	send <<= serv_info;

	CALL_DB_SERVER_NO_RET("DbPutServerInfo",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_server_info() - Delete server information in databse
//
//-----------------------------------------------------------------------------

void Database::delete_server_info(string &servname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= servname.c_str();

	CALL_DB_SERVER_NO_RET("DbDeleteServerInfo",send);
	
	return;
}

//-----------------------------------------------------------------------------
//
// Database::get_device_class_list() - Query the database for server devices 
//                                     and classes.
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_class_list(string &servname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= servname.c_str();

	CALL_DB_SERVER("DbGetDeviceClassList",send,received);
	
	return make_string_array(string("server"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_object_list() - Query the database for a list of object 
//                               (ie non-device) for which properties are defined
//
//-----------------------------------------------------------------------------

DbDatum Database::get_object_list(string &wildcard)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= wildcard.c_str();

	CALL_DB_SERVER("DbGetObjectList",send,received);
	
	return make_string_array(string("object"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_object_property_list() - Query the database for a list of
//                                        properties defined for the specified
//                                        object.
//
//-----------------------------------------------------------------------------

DbDatum Database::get_object_property_list(string &objname,string &wildcard)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *obj_info = new DevVarStringArray;
	
	obj_info->length(2);
	(*obj_info)[0] = string_dup(objname.c_str());
	(*obj_info)[1] = string_dup(wildcard.c_str());
	
	send <<= obj_info;

	CALL_DB_SERVER("DbGetPropertyList",send,received);
	
	return make_string_array(string("object"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_class_property_list() - Query the database for a list of
//                                       properties defined for the specified
//                                       class.
//
//-----------------------------------------------------------------------------

DbDatum Database::get_class_property_list(string &classname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= classname.c_str();

	CALL_DB_SERVER("DbGetClassPropertyList",send,received);
	
	return make_string_array(string("class"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_class_for_device() - Return the class of the specified device
//
//-----------------------------------------------------------------------------

string Database::get_class_for_device(string &devname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= devname.c_str();

	CALL_DB_SERVER("DbGetClassforDevice",send,received);
	
	const char *classname;
	string ret_str;
	received.inout() >>= classname;
	ret_str = classname;
	return ret_str;	
}

//-----------------------------------------------------------------------------
//
// Database::get_class_inheritance_for_device() - Return the class inheritance
//                                                scheme of the specified device
//
//-----------------------------------------------------------------------------

DbDatum Database::get_class_inheritance_for_device(string &devname)
{
	Any send;
	Any_var received;	
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= devname.c_str();

	try {
	
	  CALL_DB_SERVER("DbGetClassInheritanceForDevice",send,received);
	  
	} catch (DevFailed &e) {
	
	  // Check if an old API else re-throw
	  if (strcmp(e.errors[0].reason.in(),"API_CommandNotFound") != 0) {
	    throw e;
	  } else {
	    DbDatum db_datum;
	    db_datum.name = "class";
	    db_datum.value_string.resize(1);
            db_datum.value_string[0] = "Device_3Impl"; 
	    return db_datum;	    
	  }
	  
	}
	
	return make_string_array(string("class"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_class_list() - Query the database for a list of classes
//
//-----------------------------------------------------------------------------

DbDatum Database::get_class_list(string &wildcard)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= wildcard.c_str();

	CALL_DB_SERVER("DbGetClassList",send,received);
	
	return make_string_array(string("class"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_class_attribute_list() - Query the database for a list of
//                                        attributes defined for the specified
//                                        class.
//
//-----------------------------------------------------------------------------

DbDatum Database::get_class_attribute_list(string &classname,string &wildcard)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *class_info = new DevVarStringArray;
	
	class_info->length(2);
	(*class_info)[0] = string_dup(classname.c_str());
	(*class_info)[1] = string_dup(wildcard.c_str());
	
	send <<= class_info;

	CALL_DB_SERVER("DbGetClassAttributeList",send,received);
	
	return make_string_array(string("class"),received);
}

//-----------------------------------------------------------------------------
//
// Database::get_device_exported_for_class() - Query database for list of exported
//                                             devices for the specified class
//
//-----------------------------------------------------------------------------

DbDatum Database::get_device_exported_for_class(string &classname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= classname.c_str();

	CALL_DB_SERVER("DbGetExportedDeviceListForClass",send,received);
	
	return make_string_array(string("device"),received);
}

//-----------------------------------------------------------------------------
//
// Database::put_device_alias() - Set an alias for a device
//
//-----------------------------------------------------------------------------

void Database::put_device_alias(string &devname,string &aliasname)
{
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *alias_info = new DevVarStringArray;
	
	alias_info->length(2);
	(*alias_info)[0] = string_dup(devname.c_str());
	(*alias_info)[1] = string_dup(aliasname.c_str());
	
	send <<= alias_info;

	CALL_DB_SERVER_NO_RET("DbPutDeviceAlias",send);
	
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_device_alias() - Delete a device alias from the database
//
//-----------------------------------------------------------------------------

void Database::delete_device_alias(string &aliasname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	send <<= aliasname.c_str();

	CALL_DB_SERVER_NO_RET("DbDeleteDeviceAlias",send);
	
	return;
}

//-----------------------------------------------------------------------------
//
// Database::put_attribute_alias() - Set an alias for an attribute
//
//-----------------------------------------------------------------------------

void Database::put_attribute_alias(string &attname,string &aliasname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *alias_info = new DevVarStringArray;
	
	alias_info->length(2);
	(*alias_info)[0] = string_dup(attname.c_str());
	(*alias_info)[1] = string_dup(aliasname.c_str());
	
	send <<= alias_info;

	CALL_DB_SERVER_NO_RET("DbPutAttributeAlias",send);
	
	return;
}

//-----------------------------------------------------------------------------
//
// Database::delete_attribute_alias() - Delete an attribute alias from the database
//
//-----------------------------------------------------------------------------

void Database::delete_attribute_alias(string &aliasname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	send <<= aliasname.c_str();

	CALL_DB_SERVER("DbDeleteAttributeAlias",send,received);
	
	return;
}

//-----------------------------------------------------------------------------
//
// Database::make_history_array() - Convert the result of a DbGet...PropertyHist 
//                                  command.
//
//-----------------------------------------------------------------------------

vector<DbHistory> Database::make_history_array(bool is_attribute, Any_var &received) {

	const DevVarStringArray *ret;	
	received.inout() >>= ret;
	
	vector<DbHistory> v;
	unsigned int	i=0;
	int		  count=0;
	int		  offset;
	string	aName = "";
	string	pName;
	string	pDate;
	string	pCount;

	while(i<ret->length())
	{
		if(is_attribute)
		{
			aName = (*ret)[i];
			pName = (*ret)[i+1];
			pDate = (*ret)[i+2];
			pCount = (*ret)[i+3];
			offset = 4;
		}
		else
		{
			pName = (*ret)[i];
			pDate = (*ret)[i+1];
			pCount = (*ret)[i+2];
			offset = 3;
		}

#ifdef STRSTREAM
		istrstream istream(pCount.c_str());
#else
		istringstream istream(pCount);
#endif /* STRSTREAM */
		istream >> count;
		if (!istream)
		{
			Except::throw_exception( (const char *)"API_HistoryInvalid",
					         (const char *)"History format is invalid", 
					         (const char *)"Database::make_history_array()");
		}
		vector<string> value;
		for(int j=0;j<count;j++)
		 value.push_back( string((*ret)[i+offset+j]) );

		if (is_attribute)
			v.push_back(DbHistory(aName,pName,pDate,value));
		else
			v.push_back(DbHistory(pName,pDate,value));

		i += (count+offset);
	}

	return v;
}

//-----------------------------------------------------------------------------
//
// Database::get_property_history() - Returns the history of the specified object
//                                    property
//
//-----------------------------------------------------------------------------

vector<DbHistory> Database::get_property_history(string &objname,string &propname) {

	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *obj_info = new DevVarStringArray;
	
	obj_info->length(2);
	(*obj_info)[0] = string_dup(objname.c_str());
	(*obj_info)[1] = string_dup(propname.c_str());
	
	send <<= obj_info;

	CALL_DB_SERVER("DbGetPropertyHist",send,received);
	
	return make_history_array(false,received);
  
}

//-----------------------------------------------------------------------------
//
// Database::get_device_property_history() - Returns the history of the specified
//                                           device property
//
//-----------------------------------------------------------------------------

vector<DbHistory> Database::get_device_property_history(string &devname,string &propname) {

	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *obj_info = new DevVarStringArray;
	
	obj_info->length(2);
	(*obj_info)[0] = string_dup(devname.c_str());
	(*obj_info)[1] = string_dup(propname.c_str());
	
	send <<= obj_info;

	CALL_DB_SERVER("DbGetDevicePropertyHist",send,received);
	
	return make_history_array(false,received);
  
}

//-----------------------------------------------------------------------------
//
// Database::get_device_attribute_property_history() - Returns the history of 
//                                                     the specified device
//                                                     attribute property
//
//-----------------------------------------------------------------------------

vector<DbHistory> Database::get_device_attribute_property_history(string &devname,string &attname,string &propname) {

	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *obj_info = new DevVarStringArray;
	
	obj_info->length(3);
	(*obj_info)[0] = string_dup(devname.c_str());
	(*obj_info)[1] = string_dup(attname.c_str());
	(*obj_info)[2] = string_dup(propname.c_str());
	
	send <<= obj_info;

	CALL_DB_SERVER("DbGetDeviceAttributePropertyHist",send,received);
	
	return make_history_array(true,received);
  
}

//-----------------------------------------------------------------------------
//
// Database::get_class_property_history() - Returns the history of the specified
//                                          class property
//
//-----------------------------------------------------------------------------

vector<DbHistory> Database::get_class_property_history(string &classname,string &propname) {

	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	DevVarStringArray *obj_info = new DevVarStringArray;
	
	obj_info->length(2);
	(*obj_info)[0] = string_dup(classname.c_str());
	(*obj_info)[1] = string_dup(propname.c_str());
	
	send <<= obj_info;

	CALL_DB_SERVER("DbGetClassPropertyHist",send,received);
	
	return make_history_array(false,received);
  
}

//-----------------------------------------------------------------------------
//
// Database::get_class_attribute_property_history() - Returns the history of 
//                                                    the specified class
//                                                    attribute property
//
//-----------------------------------------------------------------------------

vector<DbHistory> Database::get_class_attribute_property_history(string &classname,string &attname,string &propname)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	DevVarStringArray *obj_info = new DevVarStringArray;
	
	obj_info->length(3);
	(*obj_info)[0] = string_dup(classname.c_str());
	(*obj_info)[1] = string_dup(attname.c_str());
	(*obj_info)[2] = string_dup(propname.c_str());
	
	send <<= obj_info;

	CALL_DB_SERVER("DbGetClassAttributePropertyHist",send,received);
	
	return make_history_array(true,received);
  
}

//-----------------------------------------------------------------------------
//
// Database::get_services() - Query database for specified services
//
//-----------------------------------------------------------------------------

DbDatum Database::get_services(string &servname,string &instname)
{
	DbData data;
	DbDatum db_datum;
	vector<string> services;
	vector<string> filter_services;
  
// Get list of services

	data.push_back( DbDatum(SERVICE_PROP_NAME) );
	get_property(CONTROL_SYSTEM, data );
	data[0] >> services;
  
// Filter 

	string filter = servname+"/";
	if( strcmp( instname.c_str() , "*")!=0 )
		filter += instname + ":";
    
	transform(filter.begin(),filter.end(),filter.begin(),::tolower);
    
	for(unsigned int i=0;i<services.size();i++)
	{ 
		transform(services[i].begin(),services[i].end(),services[i].begin(),::tolower);
		if( strncmp( services[i].c_str() , filter.c_str() , filter.length() )==0 )
		{
			string::size_type pos;
			pos = services[i].find(':');
			if( pos != string::npos )
			{
				filter_services.push_back( services[i].substr(pos+1) );
			}
		}  
	}

// Build return value 
 
	db_datum.name = "services";
	db_datum.value_string.resize(filter_services.size());
	for (unsigned int i=0; i<filter_services.size(); i++)
		db_datum.value_string[i] = filter_services[i]; 
  
	return db_datum;
}

//-----------------------------------------------------------------------------
//
// Database::register_service() - Register a new service
//
//-----------------------------------------------------------------------------

void Database::register_service(string &servname,string &instname,string &devname)
{
	DbData data;
	vector<string> services;
	vector<string> new_services;
	bool service_exists = false;
  
// Get list of services

	data.push_back( DbDatum(SERVICE_PROP_NAME) );
	get_property(CONTROL_SYSTEM,data );
	data[0] >> services;
  
	string full_name = servname+"/"+instname+":";
	transform(full_name.begin(),full_name.end(),full_name.begin(),::tolower);

// Update service list 
  
	for(unsigned int i=0;i<services.size();i++)
	{
		string service_lower = services[i];
		transform(service_lower.begin(),service_lower.end(),service_lower.begin(),::tolower);
		if( strncmp(service_lower.c_str(),full_name.c_str(),full_name.length())==0 )
		{

// The service already exists, update the device name

			string::size_type pos;
			pos = services[i].find(':');
			if( pos != string::npos )
			{
				new_services.push_back( services[i].substr(0,pos) + ":" + devname );
				service_exists = true;
			}
		}
		else
		{ 
			new_services.push_back( services[i] );      
		}   
	}
  
	if( !service_exists )
	{
// Add the new service
		new_services.push_back( servname + "/" + instname + ":" + devname );
	}

// Update the property

	data[0] << new_services;
	put_property(CONTROL_SYSTEM,data);
  
	return;
}

//-----------------------------------------------------------------------------
//
// Database::unregister_service() - Unregister a service
//
//-----------------------------------------------------------------------------

void Database::unregister_service(string &servname,string &instname)
{
	DbData data;
	vector<string> services;
	vector<string> new_services;
	bool service_deleted = false;
  
// Get list of services

	data.push_back( DbDatum(SERVICE_PROP_NAME) );
	get_property(CONTROL_SYSTEM, data );
	data[0] >> services;
  
	string full_name = servname+"/"+instname+":";
	transform(full_name.begin(),full_name.end(),full_name.begin(),::tolower);

// Update service list 
   
	for(unsigned int i=0;i<services.size();i++)
	{
		string service_lower = services[i];
		transform(service_lower.begin(),service_lower.end(),service_lower.begin(),::tolower);
		if( strncmp(service_lower.c_str(),full_name.c_str(),full_name.length())!=0 )
		{
			new_services.push_back( services[i] );
		}
		else
		{
			service_deleted = true;    
		}   
	}
  
	if( service_deleted )
	{
// Update the property

		data[0] << new_services;
		put_property(CONTROL_SYSTEM,data);
	}
  
	return;
}

//-----------------------------------------------------------------------------
//
// Database::export_event() - public method to export event to the Database
//
//-----------------------------------------------------------------------------

void Database::export_event(DevVarStringArray *eve_export)
{
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= eve_export;

	CALL_DB_SERVER_NO_RET("DbExportEvent",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::unexport_event() - public method to unexport an event from the Database
//
//-----------------------------------------------------------------------------

void Database::unexport_event(string &event)
{
	Any send;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= string_dup(event.c_str());

	CALL_DB_SERVER_NO_RET("DbUnExportEvent",send);

	return;
}

//-----------------------------------------------------------------------------
//
// Database::import_event() - public method to return import info for an event
//
//-----------------------------------------------------------------------------

CORBA::Any *Database::import_event(string &event)
{
	Any send;
	Any_var received;
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
	send <<= event.c_str();

	CALL_DB_SERVER("DbImportEvent",send,received);

	return received._retn();
}

//-----------------------------------------------------------------------------
//
// Database::fill_server_cache() - Call Db server to get DS process cache data
//
//-----------------------------------------------------------------------------

CORBA::Any *Database::fill_server_cache(string &ds_name,string &host)
{
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	
//
// Some check on args.
//
	
	string::size_type pos;
	pos = ds_name.find('/');
	if (pos == string::npos)
	{
		Tango::Except::throw_exception((const char *)"API_MethodArgument",
				       		(const char *)"The device server name parameter is incorrect. Should be: <ds_exec_name>/<inst_name>",
				       		(const char *)"Database::fill_server_cache");		
	}
	
//
// Call the db server
//
	
	Any send;
	Any_var received;
	DevVarStringArray *sent_info = new DevVarStringArray;
	
	sent_info->length(2);
	(*sent_info)[0] = string_dup(ds_name.c_str());
	(*sent_info)[1] = string_dup(host.c_str());	
	send <<= sent_info;
	
	CALL_DB_SERVER("DbGetDataForServerCache",send,received);
	return received._retn();
}

//-----------------------------------------------------------------------------
//
// Database::delete_all_device_attribute_property() - Call Db server to
// delete all the property(ies) belonging to the specified device
// attribute(s)
//
//-----------------------------------------------------------------------------

void Database::delete_all_device_attribute_property(string dev_name,DbData &db_data)
{
	AutoConnectTimeout act(DB_RECONNECT_TIMEOUT);
	Any send;

	DevVarStringArray *att_names = new DevVarStringArray;
	att_names->length(db_data.size()+1);
	(*att_names)[0] = string_dup(dev_name.c_str());
	for (unsigned int i=0; i<db_data.size(); i++)
	{
		(*att_names)[i+1] = string_dup(db_data[i].name.c_str());
	}
	send <<= att_names;

	if (filedb != 0)
	{
		Tango::Except::throw_exception((const char *)"API_NotSupportedFeature",
				       		(const char *)"The underlying database command is not implemented when the database is a file",
				       		(const char *)"Database::delete_all_device_attribute_property");	
	}
	else
		CALL_DB_SERVER_NO_RET("DbDeleteAllDeviceAttributeProperty",send);	
}


} // End of Tango namespace

static const char *RcsId = "$Header$";
//
// devapi_base.cpp 	- C++ source code file for TANGO device api 
//
// programmer(s)	- Andy Gotz (goetz@esrf.fr)
//
// original 		- March 2001
//
// log			- $Log$
// log			- Revision 2.2  2002/10/14 09:32:43  taurel
// log			- Fix bugs in devapi_base.cpp file :
// log			- - In read_attribute and read_attributes method of the DeviceProxy class
// log			-   Do not create sequence the same way if the call is local or remote.
// log			- - Add reconnection in the Connection::set_timeout_millis method
// log			- - Add flags to the Connection::set_timeout_millis method
// log			- - Fix bug in the DeviceProxy constructor when device is not marked as exported
// log			-   in the database. The constructor was not stateless in this case.
// log			-
// log			- Revision 2.1  2002/08/12 12:43:24  taurel
// log			- Fix bug in DeviceProxy::write_attributes method when writing several
// log			- attributes in one call. (File devapi_base.cpp)
// log			-
// log			- Revision 2.0  2002/06/28 13:43:08  taurel
// log			- Lot of changes since last releases :
// log			- 	- Database object managed as a singleton per control system
// log			- 	- Support all tango device naming syntax (using TANGO_HOST env.
// log			-  	  variable, without env variable and non database device)
// log			- 	- No more copy during read_attribute and command_inout
// log			- 	- Added some missing methods
// log			- 	- Build an exception class hierarchy
// log			- 	- Added correct management of device time-out
// log			- 	- Support all Tango device interface release 2 features
// log			- 	  (data/attribute comming from polling buffer, polling related methods,
// log			- 	   command/attribute result history)
// log			-
// log			- Revision 1.13  2002/04/29 12:11:25  goetz
// log			- New change in Database::delete_device_attribute_property. The fix done in the previous release was not enough
// log			-
// log			- Revision 1.12  2002/04/29 05:43:09  goetz
// log			- Check in by ET. Fix bug (i=i+n_props+1) in Database:delete_device_attribute_property, delete_class_attribute_property and put_class_attribute_property
// log			-
// log			- Revision 1.11  2002/03/18 07:20:56  goetz
// log			- new DeviceProxy() stateless now; added inline method name()
// log			-
// log			- Revision 1.10  2002/03/01 15:48:06  goetz
// log			- added get_attribute_list() method
// log			-
// log			- Revision 1.9  2002/02/28 17:00:52  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.8  2002/02/18 20:42:59  goetz
// log			- supports attributes, added lots of new methods, changed some old ones
// log			-
// log			- Revision 1.7  2002/02/11 20:16:59  goetz
// log			- added inline methods for Tango CORBA sequence types
// log			-
// log			- Revision 1.6  2001/12/04 21:34:46  goetz
// log			- fixed bug in Database::put_device_attribute_property
// log			-
// log			- Revision 1.5  2001/09/27 13:04:47  goetz
// log			- added (const char*) to all throw_exceptions; ported to dll on Windows
// log			-
// log			- Revision 1.4  2001/08/13 13:40:01  goetz
// log			- improved exception handling; ported to Windows
// log			-
// log			- Revision 1.3  2001/08/03 14:24:38  goetz
// log			- implemented reconnect; catching CORBA::Exception's and rethrowing them
// log			-
// log			- Revision 1.2  2001/08/02 14:50:12  goetz
// log			- added support for bool, DevState and DevVarCharArray types
// log			-
// log			- Revision 1.1.1.1  2001/07/18 08:42:24  goetz
// log			- Imported sources
// log			-
// log			- Revision 1.2  2001/05/28 11:50:07  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.1.1.1  2001/04/02 12:33:35  goetz
// log			- Imported sources
// log			-
//
// version 		- $Version$
//

#include <tango.h>

#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif /* WIN32 */

#include <time.h>
#include <signal.h>

#include <algorithm>
                                                    
using namespace CORBA;

namespace Tango
{

//-----------------------------------------------------------------------------
//
// Connection::Connection() - constructor to manage a connection to a device
//
//-----------------------------------------------------------------------------

Connection::Connection(ORB *orb_in):version(0),source(Tango::CACHE_DEV),
				    timeout(DEFAULT_API_TIMEOUT),
				    timeout_changed(0)
{

//
// If the proxy is created from inside a device server, use the server orb
//

	ApiUtil *au = ApiUtil::instance();		
	if ((orb_in == NULL) && (au->get_orb() == NULL)) 
	{
		if (au->in_server() == true)
			ApiUtil::instance()->set_orb(Util::instance()->get_orb());
		else
			ApiUtil::instance()->create_orb();
	}
	else
	{
		if (orb_in != NULL)
			au->set_orb(orb_in);
	}
}

//-----------------------------------------------------------------------------
//
// Connection::~Connection() - destructor to destroy connection to TANGO device
//
//-----------------------------------------------------------------------------

Connection::~Connection()
{
}

//-----------------------------------------------------------------------------
//
// Connection::connect() - method to create connection to a TANGO device 
//		using it's stringified CORBA reference i.e. IOR or corbaloc
//
//-----------------------------------------------------------------------------

void Connection::connect(string &corba_name)
{
	try
	{
//
// narrow CORBA string name to CORBA object
//
        	Object_var obj;
		obj = ApiUtil::instance()->get_orb()->string_to_object(corba_name.c_str());

		version = 1;
		device = Device::_narrow(obj.in());

//
// Also initialiase device_2 if it's support IDL release 2
//

		if (obj->_is_a("IDL:Tango/Device_2:1.0") == true)
		{
			version = 2;
			device_2 = Device_2::_narrow(obj);
		}

        	if (CORBA::is_nil(device))
        	{
        		cerr << "Can't build connection to object " << corba_name << endl;
			
			connection_state = CONNECTION_NOTOK;
			
			TangoSys_OMemStream desc;
			desc << "Failed to connect to device " << dev_name();
			desc << " (device nil after _narrowing)" << ends;
                	ApiConnExcept::throw_exception((const char*)"API_CantConnectToDevice",
                        			       desc.str(),
						       (const char*)"Connection::connect()");
        	}
		
// Mark the connection as OK and set timeout to its value
// (The default is 3 seconds)

		connection_state = CONNECTION_OK;
		
		set_timeout_millis(timeout);
#ifdef WIN32
		timeout_changed = 0;
#endif

	}
        catch (CORBA::SystemException &ce)
        {			
		TangoSys_OMemStream desc;
		TangoSys_MemStream reason;
		desc << "Failed to connect to ";
			
		string::size_type pos = corba_name.find(':');
		if (pos == string::npos)
		{
			desc << "device " << dev_name() << ends;
			reason << "API_CantConnectToDevice" << ends;
		}
		else
		{
			string prot = corba_name.substr(0,pos);
			if (prot == "corbaloc")
			{
				string::size_type tmp = corba_name.find('/');
				if (tmp != string::npos)
				{
					string dev = corba_name.substr(tmp + 1);
					if (dev == "database")
					{
						desc << "database on host ";
						desc << db_host << " with port ";
						desc << db_port << ends;
						reason << "API_CantConnectToDatabase" << ends;
					}
					else
					{
						CORBA::SystemException *se;
						desc << "device " << dev_name() << ends;
						if ((se = CORBA::OBJECT_NOT_EXIST::_downcast(&ce)) != 0)
							reason << "API_DeviceNotDefined" << ends;
						else if ((se = CORBA::TRANSIENT::_downcast(&ce)) != 0)
							reason << "API_ServerNotRunning" << ends;
						else
							reason << "API_CantConnectToDevice" << ends;
					}
				}
				else
				{
					desc << "device " << dev_name() << ends;
					reason << "API_CantConnectToDevice" << ends;
				}
			}
			else
			{
				desc << "device " << dev_name() << ends;
				reason << "API_CantConnectToDevice" << ends;
			}
		}
						
		ApiConnExcept::re_throw_exception(ce,reason.str(),desc.str(),
					          (const char *)"Connection::connect");
	}
	
}


//-----------------------------------------------------------------------------
//
// Connection::reconnect() - reconnect to a CORBA object
//
//-----------------------------------------------------------------------------

void Connection::reconnect(bool db_used)
{
	if (connection_state != CONNECTION_OK)
	{
		string corba_name;
		if (db_used == true)
			corba_name = get_corba_name();
		else
			corba_name = build_corba_name();

		connect(corba_name);
	}

//
// Try to ping the device. With omniORB, it is possible that the first
// real access to the device is done when a call to one of the interface
// operation is done. Do it now.
//
	
	try
	{
		device->ping();
	}
	catch (CORBA::SystemException &ce)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to connect to device " << dev_name() << ends;
								
		ApiConnExcept::re_throw_exception(ce,
						  (const char *)"API_CantConnectToDevice",
						  desc.str(),
					          (const char *)"Connection::reconnect");
	}
}

//-----------------------------------------------------------------------------
//
// Connection::get_timeout_millis() - public method to get timeout on a TANGO device
//
//-----------------------------------------------------------------------------

int Connection::get_timeout_millis()
{
	return(timeout);
}


//-----------------------------------------------------------------------------
//
// Connection::set_timeout_millis() - public method to set timeout on a TANGO device
//
//-----------------------------------------------------------------------------

void Connection::set_timeout_millis(int millisecs,bitset<numFlags> fl)
{

	if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
/*	CORBA::PolicyList policy;
	CORBA::Any any;
	CORBA::Object_var policy_obj;
	CORBA::ULong ul_timeout;

	policy.length(1);

	ul_timeout = millisecs;

	any <<= ul_timeout;

	policy[0] = ApiUtil::instance()->get_orb()->create_policy(OB::TIMEOUT_POLICY_ID,any);
       	if ((fl[device_flag] == true) ||
	    (fl[all_devices_flag] == true))
	{
		policy_obj = device->_set_policy_overrides(policy,CORBA::ADD_OVERRIDE);
		device = Device::_narrow(policy_obj);
	}
	
	if (version == 2)
	{
		if ((fl[device_2_flag] == true) ||
		    (fl[all_devices_flag] == true))
		{
			policy_obj = device_2->_set_policy_overrides(policy,CORBA::ADD_OVERRIDE);
			device_2 = Device_2::_narrow(policy_obj);
		}
	}

	timeout = millisecs;
#ifdef WIN32
	timeout_changed++;
#endif*/

	return;
}


//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//
//-----------------------------------------------------------------------------


DeviceData Connection::command_inout(string &command)
{
	DeviceData data_in;

	return(command_inout(command,data_in));
}

//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//
//-----------------------------------------------------------------------------


DeviceData Connection::command_inout(string &command, DeviceData &data_in)
{

//
// We are using a pointer to an Any as the return value of the command_inout
// call. This is because the assignament to the Any_var any in the
// DeviceData object in faster in this case (no copy).
// Don't forget that the any_var in the DeviceData takes ownership of the
// memory allocated 
//

	DeviceData data_out;
	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		CORBA::Any *received;
		if (version == 2)
			received = device_2->command_inout_2(command.c_str(),data_in.any,source);
		else
			received = device->command_inout(command.c_str(),data_in.any);

		data_out.any = received;
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"Connection::command_inout()");
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
                Except::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"Connection::command_inout()");
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT_CMD(trans);		
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
				
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
		ApiCommExcept::re_throw_exception(ce,
					   (const char*)"API_CommunicationFailed",
                        		   desc.str(),
					   (const char*)"Connection::command_inout()");
	}
	
	return data_out;
}

//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//				 using low level CORBA types
//
//-----------------------------------------------------------------------------


CORBA::Any_var Connection::command_inout(string &command, CORBA::Any &any)
{
	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		if (version == 2)
			return (device_2->command_inout_2(command.c_str(),any,source));
		else
			return (device->command_inout(command.c_str(),any));
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"Connection::command_inout()");
	}
	catch (Tango::DevFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
                Except::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"Connection::command_inout()");
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT_CMD(trans);
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
			
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
		ApiCommExcept::re_throw_exception(ce,
					   (const char*)"API_CommunicationFailed",
                        		   desc.str(),
					   (const char*)"Connection::command_inout()");
	}
//
// Just to make VC++ quiet (will never reach this code !)
//

#ifdef WIN32
	CORBA::Any_var tmp;
	return tmp;
#endif
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - constructor for device proxy object 
//
//-----------------------------------------------------------------------------

DeviceProxy::DeviceProxy (string &name, CORBA::ORB *orb) : Connection(orb),
							   adm_device(NULL)
{
	real_constructor(name);
}

DeviceProxy::DeviceProxy (const char *na, CORBA::ORB *orb) : Connection(orb),
							     adm_device(NULL)
{
	string name(na);
	real_constructor(name);
}

void DeviceProxy::real_constructor (string &name)
{

//
// parse device name
//

	parse_name(name);
	string corba_name;
	bool exported = true;
		
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
				db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
			else
				db_dev = new DbDevice(device_name);
			int ind = ui->get_db_ind();
			db_host = (ui->get_db_vect())[ind]->get_db_host();
			db_port = (ui->get_db_vect())[ind]->get_db_port();
			db_port_num = (ui->get_db_vect())[ind]->get_db_port_num();
		}
		else
		{
			db_dev = new DbDevice(device_name,db_host,db_port);
		}

		try
		{		
			corba_name = get_corba_name();
		}
		catch (Tango::DevFailed &dfe)
		{
			if (strcmp(dfe.errors[0].reason,"DB_DeviceNotDefined") == 0)
			{
				TangoSys_OMemStream desc;
				desc << "Can't connect to device " << device_name << ends;
				ApiConnExcept::re_throw_exception(dfe,
						(const char *)"API_DeviceNotDefined",
					 	desc.str(),
						(const char *)"DeviceProxy::DeviceProxy");
			}
			else if (strcmp(dfe.errors[0].reason,"API_DeviceNotExported") == 0)
				exported = false;
		}
	}
	else
	{
		corba_name = build_corba_name();
	}

//
// implement stateless new() i.e. even if connect fails continue
//
	try
	{
		if (exported == true)
			connect(corba_name);
	}
        catch (Tango::ConnectionFailed &dfe)
	{
		connection_state = CONNECTION_NOTOK;
		if (dbase_used == false)
		{
			if (strcmp(dfe.errors[1].reason,"API_DeviceNotDefined") == 0)
				throw;
		}
	}
        catch (CORBA::SystemException &) 
	{
		connection_state = CONNECTION_NOTOK;
		if (dbase_used == false)
			throw;
	}
	
//
// For non-database device , try to ping them. It's the only way to know that 
// the device is not defined
//

	if (dbase_used == false)
	{
		try
		{
			ping();
		}
		catch (Tango::ConnectionFailed &dfe)
		{
			if (strcmp(dfe.errors[1].reason,"API_DeviceNotDefined") == 0)
				throw;
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - copy constructor
//
//-----------------------------------------------------------------------------

DeviceProxy::DeviceProxy(const DeviceProxy &sou)
{

//
// First Connection call members
//
	dbase_used = sou.dbase_used;
	from_env_var = sou.from_env_var;
	host = sou.host;
	port = sou.port;
	port_num = sou.port_num;
	db_host = sou.db_host;
	db_port = sou.db_port;
	db_port_num = sou.db_port_num;
	ior = sou.ior;
	device = sou.device;
	device_2 = sou.device_2;
	timeout = sou.timeout;
	connection_state = sou.connection_state;
	version = sou.version;
	source = sou.source;
	
//
// Now DeviceProxy members
//
	
	device_name = sou.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
				db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
			else
				db_dev = new DbDevice(device_name);
		}
		else
		{
			db_dev = new DbDevice(device_name,db_host,db_port);
		}
	}
		
	adm_device = NULL;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - assignement operator 
//
//-----------------------------------------------------------------------------

DeviceProxy &DeviceProxy::operator=(const DeviceProxy &rval)
{

//
// First Connection call members
//
	if (dbase_used == true)
		delete db_dev;
	dbase_used = rval.dbase_used;
	from_env_var = rval.from_env_var;
	host = rval.host;
	port = rval.port;
	port_num = rval.port_num;
	db_host = rval.db_host;
	db_port = rval.db_port;
	db_port_num = rval.db_port_num;
	ior = rval.ior;
	device = rval.device;
	device_2 = rval.device_2;
	timeout = rval.timeout;
	connection_state = rval.connection_state;
	version = rval.version;
	source = rval.source;
	
//
// Now DeviceProxy members
//
	
	device_name = rval.device_name;
			
	if (dbase_used == true)
	{
		if (from_env_var == true)
		{	
			ApiUtil *ui = ApiUtil::instance();
			if (ui->in_server() == true)
				db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
			else
				db_dev = new DbDevice(device_name);
		}
		else
		{
			db_dev = new DbDevice(device_name,db_host,db_port);
		}
	}
	
	if (adm_device != NULL)
		delete adm_device;
		
	adm_device = NULL;
	
	return *this;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::parse_name() - Parse device name according to Tango device
//			       name syntax
//
// in :	- full_name : The device name
//
//-----------------------------------------------------------------------------

void DeviceProxy::parse_name(string &full_name)
{
	string name_wo_prot;
	string name_wo_db_mod;
	string dev_name;

//
// Device name in lower case letters
//
	
	transform(full_name.begin(),full_name.end(),full_name.begin(),Tango_tolower);
		
//
// Try to find protocol specification in device name and analyse it
//

	string::size_type pos = full_name.find(PROT_SEP);
	if (pos == string::npos)
	{
		name_wo_prot = full_name;
	}
	else
	{
		string protocol = full_name.substr(0,pos);

		if (protocol == TANGO_PROTOCOL)
		{
			name_wo_prot = full_name.substr(pos + 3);
		}		
		else if (protocol == TACO_PROTOCOL)
		{		
			TangoSys_OMemStream desc;
			desc << "Taco protocol is not supported" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");			exit(0);
		}
		else
		{		
			TangoSys_OMemStream desc;
			desc << protocol;
			desc << " protocol is an unsupported protocol" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedProtocol",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
		}			
	}

//
// Try to find database database modifier and analyse it
//

	pos = name_wo_prot.find(MODIFIER);
	if (pos != string::npos)
	{
		string mod = name_wo_prot.substr(pos + 1);
		
		if (mod == DBASE_YES)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos)); 
			dbase_used = true;
		}
		else if (mod == DBASE_NO)
		{
			string::size_type len = name_wo_prot.size();
			name_wo_db_mod = name_wo_prot.substr(0,len - (len - pos));
			dbase_used = false;
		}
		else
		{
			cerr << mod << " is a non supported database modifier!" << endl;

			TangoSys_OMemStream desc;
			desc << mod;
			desc << " modifier is an unsupported db momdifier" << ends;
			ApiWrongNameExcept::throw_exception((const char*)"API_UnsupportedDBaseModifier",
						desc.str(),
						(const char*)"DeviceProxy::parse_name()");
		}
	}
	else
	{
		name_wo_db_mod = name_wo_prot;
		dbase_used = true;
	}

	if (dbase_used == false)
	{
	
//
// Extract host name and port number
//

		pos = name_wo_db_mod.find(HOST_SEP);
		if (pos == string::npos)
		{
			cerr << "Host and port not correctly defined in device name " << full_name << endl;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongDeviceNameSyntax",
						(const char*)"Host and port not correctly defined in device name",
						(const char*)"DeviceProxy::parse_name()");
		}
		
		host = name_wo_db_mod.substr(0,pos);
		string::size_type tmp = name_wo_db_mod.find(PORT_SEP);
		if (tmp == string::npos)
		{
			cerr << "Host and port not correctly defined in device name " << full_name << endl;

			ApiWrongNameExcept::throw_exception((const char*)"API_WrongDeviceNameSyntax",
						(const char*)"Host and port not correctly defined in device name",
						(const char*)"DeviceProxy::parse_name()");
		}
		port = name_wo_db_mod.substr(pos + 1,tmp - pos - 1);
		TangoSys_MemStream s;
		s << port << ends;
		s >> port_num;
		device_name = name_wo_db_mod.substr(tmp + 1);
		
		db_host = db_port = NOT_USED;
		db_port_num = 0;
				 
	}
	else
	{
	
//
// Search if host and port are specified
//

		pos = name_wo_db_mod.find(PORT_SEP);
		if (pos == string::npos)
		{
			device_name = name_wo_db_mod;
			from_env_var = true;
			port_num = 0;
			host = FROM_IOR;
			port = FROM_IOR;
		}
		else
		{
			string bef_sep = name_wo_db_mod.substr(0,pos);
			string::size_type tmp = bef_sep.find(HOST_SEP);
			if (tmp == string::npos)
			{
				device_name = name_wo_db_mod;
				from_env_var = true;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
			else
			{
				db_host = bef_sep.substr(0,tmp);
				db_port = bef_sep.substr(tmp + 1);
				TangoSys_MemStream s;
				s << db_port << ends;
				s >> db_port_num;
				device_name = name_wo_db_mod.substr(pos + 1);
				from_env_var = false;
				port_num = 0;
				port = FROM_IOR;
				host = FROM_IOR;
			}
		}

	}
		
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_corba_name() - return IOR for device from database
//
//-----------------------------------------------------------------------------

string DeviceProxy::get_corba_name()
{
	DbDevImportInfo import_info;

	import_info = db_dev->import_device();

	if (import_info.exported != 1)
	{
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;		
		desc << "Device " << device_name << " is not exported (hint: try starting the device server)" << ends;
		ApiConnExcept::throw_exception((const char*)"API_DeviceNotExported",
					       desc.str(),
					       (const char*)"DeviceProxy::get_corba_name()");
	}

	return(import_info.ior);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::build_corba_name() - build corba name for non database device
//				     server. In this case, corba name uses
//				     the "corbaloc" naming schema
//
//-----------------------------------------------------------------------------

string DeviceProxy::build_corba_name()
{

        string db_corbaloc = "corbaloc:iiop:";
        db_corbaloc = db_corbaloc + host + ":" + port;
	db_corbaloc = db_corbaloc + "/" + device_name;

	return db_corbaloc;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::import_info() - return import info for device from database
//
//-----------------------------------------------------------------------------

DbDevImportInfo DeviceProxy::import_info()
{
	DbDevImportInfo import_info;
	
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					        desc.str(),
					        (const char *)"DeviceProxy::import_info");
	}
	else
	{
		import_info = db_dev->import_device();
	}

	return(import_info);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::~DeviceProxy() - destructor to destroy proxy to TANGO device
//
//-----------------------------------------------------------------------------

DeviceProxy::~DeviceProxy()
{
	if (dbase_used == true)
		delete db_dev;
		
	if (adm_device != NULL)
		delete adm_device;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::ping() - ping TANGO device and return time elapsed in microseconds
//
//-----------------------------------------------------------------------------

int DeviceProxy::ping()
{
	int elapsed;

#ifndef WIN32
        struct timeval before, after;

	gettimeofday(&before, NULL);
#else
	struct _timeb before, after;

	_ftime(&before);
#endif /* WIN32 */
	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		device->ping();
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","ping");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;			
		desc << "Failed to execute ping on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					   (const char*)"API_CommunicationFailed",
                        		   desc.str(),
					   (const char*)"DeviceProxy::ping()");
	}
#ifndef WIN32
	gettimeofday(&after, NULL);
	elapsed = (after.tv_sec-before.tv_sec)*1000000;
	elapsed = (after.tv_usec-before.tv_usec) + elapsed;
#else
	_ftime(&after);
	elapsed = (after.time-before.time)*1000000;
	elapsed = (after.millitm-before.millitm)*1000 + elapsed;
#endif /* WIN32 */

	return(elapsed);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::name() - return TANGO device name as string
//
//-----------------------------------------------------------------------------

string DeviceProxy::name()
{
	string na;

	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		CORBA::String_var n = device->name();
		na = n;
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","name");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute name() on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::name()");
	}

	return(na);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::state() - return TANGO state of device
//
//-----------------------------------------------------------------------------

DevState DeviceProxy::state()
{
	DevState sta;
	
	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		sta = device->state();
	}
	catch (CORBA::TRANSIENT &transp)
	{
		TRANSIENT_EXCEPT(transp,"DeviceProxy","state");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute state() on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                    			      desc.str(),
					      (const char*)"DeviceProxy::state()");
	}
	
	return sta;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::status() - return TANGO status of device
//
//-----------------------------------------------------------------------------

string DeviceProxy::status()
{
	string status_str;

	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		CORBA::String_var st = device->status();
		status_str = st;
	}
	catch (CORBA::TRANSIENT &transp)
	{
		TRANSIENT_EXCEPT(transp,"DeviceProxy","status");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute status() on device (CORBA exception)" << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::status()");
	}

	return(status_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::adm_name() - return TANGO admin name of device
//
//-----------------------------------------------------------------------------

string DeviceProxy::adm_name()
{
	string adm_name_str;

	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		CORBA::String_var st = device->adm_name();
		adm_name_str = st;
	}
	catch (CORBA::TRANSIENT &transp)
	{
		TRANSIENT_EXCEPT(transp,"DeviceProxy","adm_name");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute adm_name() on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::adm_name()");
	}

	return(adm_name_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::description() - return TANGO device description as string
//
//-----------------------------------------------------------------------------

string DeviceProxy::description()
{
	string description_str;

	try
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		CORBA::String_var st = device->description();
		description_str = st;
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","description");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute description() on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::description()");
	}

	return(description_str);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::black_box() - return the list of the last n commands exectued on
//		this TANGO device 
//
//-----------------------------------------------------------------------------
 
vector<string> *DeviceProxy::black_box(int last_n_commands) 
{
	DevVarStringArray_var last_commands;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
		last_commands = device->black_box(last_n_commands);

	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","black_box");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute black_box on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                      			      desc.str(),
					      (const char*)"DeviceProxy::black_box()");
	}

	vector<string> *last_commands_vector = new(vector<string>);
	last_commands_vector->resize(last_commands->length());

	for (int i=0; i<last_commands->length(); i++)
	{
		(*last_commands_vector)[i] = last_commands[i];
	}

	return(last_commands_vector);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::info() - return information about this device
//
//-----------------------------------------------------------------------------
 
DeviceInfo DeviceProxy::info() 
{
	DevInfo_var dev_info;
	DeviceInfo device_info;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
		dev_info = device->info();

	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","info");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute info() on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					   (const char*)"API_CommunicationFailed",
                        		   desc.str(),
					   (const char*)"DeviceProxy::info()");
	}

	device_info.dev_class = dev_info->dev_class;
	device_info.server_id = dev_info->server_id;
	device_info.server_host = dev_info->server_host;
	device_info.server_version = dev_info->server_version;
	device_info.doc_url = dev_info->doc_url;
	device_info.dev_type = 0;

	return(device_info);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::command_query() - return the description for the specified 
//		command implemented for this TANGO device 
//
//-----------------------------------------------------------------------------
 
CommandInfo DeviceProxy::command_query(string cmd) 
{
	CommandInfo command_info;
	DevCmdInfo_var cmd_info;
	DevCmdInfo_2_var cmd_info_2;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		if (version == 1)
		{
			cmd_info = device->command_query(cmd.c_str());
						
			command_info.cmd_name = cmd_info->cmd_name;
			command_info.cmd_tag = cmd_info->cmd_tag;
			command_info.in_type = cmd_info->in_type;
			command_info.out_type = cmd_info->out_type;
			command_info.in_type_desc = cmd_info->in_type_desc;
			command_info.out_type_desc = cmd_info->out_type_desc;
			command_info.disp_level = Tango::OPERATOR;
		}
		else
		{
			cmd_info_2 = device_2->command_query_2(cmd.c_str());
						
			command_info.cmd_name = cmd_info_2->cmd_name;
			command_info.cmd_tag = cmd_info_2->cmd_tag;
			command_info.in_type = cmd_info_2->in_type;
			command_info.out_type = cmd_info_2->out_type;
			command_info.in_type_desc = cmd_info_2->in_type_desc;
			command_info.out_type_desc = cmd_info_2->out_type_desc;
			command_info.disp_level = cmd_info_2->level;
		}
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","command_query");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_query on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					   (const char*)"API_CommunicationFailed",
                        		   desc.str(),
					   (const char*)"DeviceProxy::command_query()");
	}


	
	return(command_info);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::command_list_query() - return the list of commands implemented for this TANGO device 
//
//-----------------------------------------------------------------------------
 
CommandInfoList *DeviceProxy::command_list_query() 
{
	CommandInfoList *command_info_list = new CommandInfoList();
	DevCmdInfoList_var cmd_info_list;
	DevCmdInfoList_2_var cmd_info_list_2;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		if (version == 1)
		{	
			cmd_info_list = device->command_list_query();
			
			command_info_list->resize(cmd_info_list->length());

			for (int i=0; i < cmd_info_list->length(); i++)
			{
				(*command_info_list)[i].cmd_name = cmd_info_list[i].cmd_name;
				(*command_info_list)[i].cmd_tag = cmd_info_list[i].cmd_tag;
				(*command_info_list)[i].in_type = cmd_info_list[i].in_type;
				(*command_info_list)[i].out_type = cmd_info_list[i].out_type;
				(*command_info_list)[i].in_type_desc = cmd_info_list[i].in_type_desc;
				(*command_info_list)[i].out_type_desc = cmd_info_list[i].out_type_desc;
				(*command_info_list)[i].disp_level = Tango::OPERATOR;
			}
		}
		else
		{
			cmd_info_list_2 = device_2->command_list_query_2();
			
			command_info_list->resize(cmd_info_list_2->length());

			for (int i=0; i < cmd_info_list_2->length(); i++)
			{
				(*command_info_list)[i].cmd_name = cmd_info_list_2[i].cmd_name;
				(*command_info_list)[i].cmd_tag = cmd_info_list_2[i].cmd_tag;
				(*command_info_list)[i].in_type = cmd_info_list_2[i].in_type;
				(*command_info_list)[i].out_type = cmd_info_list_2[i].out_type;
				(*command_info_list)[i].in_type_desc = cmd_info_list_2[i].in_type_desc;
				(*command_info_list)[i].out_type_desc = cmd_info_list_2[i].out_type_desc;
				(*command_info_list)[i].disp_level = cmd_info_list_2[i].level;
			}		
		}
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","command_list_query");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_list_query on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::command_list_query()");
	}



	return(command_info_list);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property(string &property_name, DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property");
	}
	else
	{
		db_data.resize(1);
		db_data[0] = DbDatum(property_name);

		db_dev->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property(vector<string> &property_names, DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property");
	}
	else
	{
		db_data.resize(property_names.size());
		for (int i=0; i<property_names.size(); i++)
		{
			db_data[i] = DbDatum(property_names[i]);
		}

		db_dev->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_property() - get a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::get_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::get_property");
	}
	else
	{
		db_dev->get_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::put_property() - put a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::put_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::put_property");
	}
	else
	{
		db_dev->put_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::delete_property() - deleteget a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::delete_property(string &property_name) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::delete_property");
	}
	else
	{
		DbData db_data;

		db_data.push_back(DbDatum(property_name));

		db_dev->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::delete_property(vector<string> &property_names) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::delete_property");
	}
	else
	{
		DbData db_data;

		for (int i=0; i<property_names.size(); i++)
		{
			db_data.push_back(DbDatum(property_names[i]));
		}

		db_dev->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::delete_property() - delete a property from the database
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::delete_property(DbData &db_data) 
{
	if (dbase_used == false)
	{
		TangoSys_OMemStream desc;
		desc << "Method not available for device ";
		desc << device_name;
		desc << " which is a non database device";
		
		ApiNonDbExcept::throw_exception((const char *)"API_NonDatabaseDevice",
					desc.str(),
					(const char *)"DeviceProxy::delete_property");
	}
	else
	{
		db_dev->delete_property(db_data);
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_config() - return a list of attributes
//
//-----------------------------------------------------------------------------
 
AttributeInfoList *DeviceProxy::get_attribute_config(vector<string>& attr_string_list) 
{
	AttributeConfigList_var attr_config_list;
	AttributeConfigList_2_var attr_config_list_2;
	AttributeInfoList *dev_attr_config = new AttributeInfoList();
	DevVarStringArray attr_list;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
		attr_list.length(attr_string_list.size());
		for (int i=0; i<attr_string_list.size(); i++)
		{
			attr_list[i] = string_dup(attr_string_list[i].c_str());
		}
		
		if (version == 1)
		{
			attr_config_list = device->get_attribute_config(attr_list);

			dev_attr_config->resize(attr_config_list->length());

			for (int i=0; i<attr_config_list->length(); i++)
			{
				(*dev_attr_config)[i].name = attr_config_list[i].name;
				(*dev_attr_config)[i].writable = attr_config_list[i].writable;
				(*dev_attr_config)[i].data_format = attr_config_list[i].data_format;
				(*dev_attr_config)[i].data_type = attr_config_list[i].data_type;
				(*dev_attr_config)[i].max_dim_x = attr_config_list[i].max_dim_x;
				(*dev_attr_config)[i].max_dim_y = attr_config_list[i].max_dim_y;
				(*dev_attr_config)[i].description = attr_config_list[i].description;
				(*dev_attr_config)[i].label = attr_config_list[i].label;
				(*dev_attr_config)[i].unit = attr_config_list[i].unit;
				(*dev_attr_config)[i].standard_unit = attr_config_list[i].standard_unit;
				(*dev_attr_config)[i].display_unit = attr_config_list[i].display_unit;
				(*dev_attr_config)[i].format = attr_config_list[i].format;
				(*dev_attr_config)[i].min_value = attr_config_list[i].min_value;
				(*dev_attr_config)[i].max_value = attr_config_list[i].max_value;
				(*dev_attr_config)[i].min_alarm = attr_config_list[i].min_alarm;
				(*dev_attr_config)[i].max_alarm = attr_config_list[i].max_alarm;
				(*dev_attr_config)[i].writable_attr_name = attr_config_list[i].writable_attr_name;
				(*dev_attr_config)[i].extensions.resize(attr_config_list[i].extensions.length());
				for (int j=0; j<attr_config_list[i].extensions.length(); j++)
				{
					(*dev_attr_config)[i].extensions[j] = attr_config_list[i].extensions[j];
				}
				(*dev_attr_config)[i].disp_level = Tango::OPERATOR;
			}
		}
		else
		{
			attr_config_list_2 = device_2->get_attribute_config_2(attr_list);

			dev_attr_config->resize(attr_config_list_2->length());

			for (int i=0; i<attr_config_list_2->length(); i++)
			{
				(*dev_attr_config)[i].name = attr_config_list_2[i].name;
				(*dev_attr_config)[i].writable = attr_config_list_2[i].writable;
				(*dev_attr_config)[i].data_format = attr_config_list_2[i].data_format;
				(*dev_attr_config)[i].data_type = attr_config_list_2[i].data_type;
				(*dev_attr_config)[i].max_dim_x = attr_config_list_2[i].max_dim_x;
				(*dev_attr_config)[i].max_dim_y = attr_config_list_2[i].max_dim_y;
				(*dev_attr_config)[i].description = attr_config_list_2[i].description;
				(*dev_attr_config)[i].label = attr_config_list_2[i].label;
				(*dev_attr_config)[i].unit = attr_config_list_2[i].unit;
				(*dev_attr_config)[i].standard_unit = attr_config_list_2[i].standard_unit;
				(*dev_attr_config)[i].display_unit = attr_config_list_2[i].display_unit;
				(*dev_attr_config)[i].format = attr_config_list_2[i].format;
				(*dev_attr_config)[i].min_value = attr_config_list_2[i].min_value;
				(*dev_attr_config)[i].max_value = attr_config_list_2[i].max_value;
				(*dev_attr_config)[i].min_alarm = attr_config_list_2[i].min_alarm;
				(*dev_attr_config)[i].max_alarm = attr_config_list_2[i].max_alarm;
				(*dev_attr_config)[i].writable_attr_name = attr_config_list_2[i].writable_attr_name;
				(*dev_attr_config)[i].extensions.resize(attr_config_list_2[i].extensions.length());
				for (int j=0; j<attr_config_list_2[i].extensions.length(); j++)
				{
					(*dev_attr_config)[i].extensions[j] = attr_config_list_2[i].extensions[j];
				}
				(*dev_attr_config)[i].disp_level = attr_config_list_2[i].level;
			}
		}
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","get_attribute_config");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute get_attribute_config on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					   (const char*)"API_CommunicationFailed",
                       			   desc.str(),
					   (const char*)"DeviceProxy::get_attribute_config()");
	}

	return(dev_attr_config);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_config() - return a single attribute config
//
//-----------------------------------------------------------------------------
 
AttributeInfo DeviceProxy::get_attribute_config(string& attr_string) 
{
	vector<string> attr_string_list;
	AttributeInfoList *dev_attr_config_list;
	AttributeInfo dev_attr_config;

	attr_string_list.push_back(attr_string);
	dev_attr_config_list = get_attribute_config(attr_string_list);

	dev_attr_config = (*dev_attr_config_list)[0];
	delete(dev_attr_config_list);
	
	return(dev_attr_config);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::set_attribute_config() - set config for a list of attributes
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::set_attribute_config(AttributeInfoList &dev_attr_list) 
{
	AttributeConfigList attr_config_list;
	DevVarStringArray attr_list;

	attr_config_list.length(dev_attr_list.size());

	for (int i=0; i<attr_config_list.length(); i++)
	{
		attr_config_list[i].name = dev_attr_list[i].name.c_str();
		attr_config_list[i].writable = dev_attr_list[i].writable;
		attr_config_list[i].data_format = dev_attr_list[i].data_format;
		attr_config_list[i].data_type = dev_attr_list[i].data_type;
		attr_config_list[i].max_dim_x = dev_attr_list[i].max_dim_x;
		attr_config_list[i].max_dim_y = dev_attr_list[i].max_dim_y;
		attr_config_list[i].description = dev_attr_list[i].description.c_str();
		attr_config_list[i].label = dev_attr_list[i].label.c_str();
		attr_config_list[i].unit = dev_attr_list[i].unit.c_str();
		attr_config_list[i].standard_unit = dev_attr_list[i].standard_unit.c_str();
		attr_config_list[i].display_unit = dev_attr_list[i].display_unit.c_str();
		attr_config_list[i].format = dev_attr_list[i].format.c_str();
		attr_config_list[i].min_value = dev_attr_list[i].min_value.c_str();
		attr_config_list[i].max_value = dev_attr_list[i].max_value.c_str();
		attr_config_list[i].min_alarm = dev_attr_list[i].min_alarm.c_str();
		attr_config_list[i].max_alarm = dev_attr_list[i].max_alarm.c_str();
		attr_config_list[i].writable_attr_name = dev_attr_list[i].writable_attr_name.c_str();
		attr_config_list[i].extensions.length(dev_attr_list[i].extensions.size());
		for (int j=0; j<dev_attr_list[i].extensions.size(); j++)
		{
			attr_config_list[i].extensions[j] = string_dup(dev_attr_list[i].extensions[j].c_str());
		}
	}
	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
		device->set_attribute_config(attr_config_list);

	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","set_attribute_config");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute set_attribute_config on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::set_attribute_config()");
	}


	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::read_attributes() - return a list of attributes
//
//-----------------------------------------------------------------------------
 
vector<DeviceAttribute> *DeviceProxy::read_attributes(vector<string>& attr_string_list) 
{
	AttributeValueList_var attr_value_list;
	vector<DeviceAttribute> *dev_attr = new(vector<DeviceAttribute>);
	DevVarStringArray attr_list;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
		attr_list.length(attr_string_list.size());
		for (int i=0; i<attr_string_list.size(); i++)
		{
			attr_list[i] = string_dup(attr_string_list[i].c_str());
		}
		
		if (version == 2)
			attr_value_list = device_2->read_attributes_2(attr_list,source);
		else
			attr_value_list = device->read_attributes(attr_list);

	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to read_attributes on device " << device_name;
		desc << ", attributes ";
		int nb_attr = attr_string_list.size();
		for (int i = 0;i < nb_attr;i++)
		{
			desc << attr_string_list[i];
			if (i != nb_attr - 1)
				desc << ", ";
		}
		desc << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        desc.str(), (const char*)"DeviceProxy::read_attributes()");
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","read_attributes");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute read_attributes on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::read_attributes()");
	}

	dev_attr->resize(attr_value_list->length());

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	CORBA::ULong max,len;
	
	for (int i=0; i<attr_value_list->length(); i++)
	{
		(*dev_attr)[i].name = attr_value_list[i].name;
		(*dev_attr)[i].quality = attr_value_list[i].quality;
		(*dev_attr)[i].time = attr_value_list[i].time;
		(*dev_attr)[i].dim_x = attr_value_list[i].dim_x;
		(*dev_attr)[i].dim_y = attr_value_list[i].dim_y;

		if ((*dev_attr)[i].quality != Tango::ATTR_INVALID)
		{
			TypeCode_var ty = attr_value_list[i].value.type();
			TypeCode_var ty_alias = ty->content_type();
			TypeCode_var ty_seq = ty_alias->content_type();			
			switch (ty_seq->kind())
			{
			case tk_long:		
				attr_value_list[i].value >>= tmp_seq_lo;
				max = tmp_seq_lo->maximum();
				len = tmp_seq_lo->length();
				if (tmp_seq_lo->release() == true)
				{
					tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
				}
				else
				{
					tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
					(*dev_attr)[i].LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
				}
				break;
		
			case tk_short:
				attr_value_list[i].value >>= tmp_seq_sh;
				max = tmp_seq_sh->maximum();
				len = tmp_seq_sh->length();
				if (tmp_seq_sh->release() == true)
				{
					tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
				}
				else
				{
					tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
					(*dev_attr)[i].ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
				}
				break;
		
			case tk_double:
				attr_value_list[i].value >>= tmp_seq_db;
				max = tmp_seq_db->maximum();
				len = tmp_seq_db->length();
				if (tmp_seq_db->release() == true)
				{
					tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
				}
				else
				{
					tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
					(*dev_attr)[i].DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
				}
				break;
		
			case tk_string:
				attr_value_list[i].value >>= tmp_seq_str;
				max = tmp_seq_str->maximum();
				len = tmp_seq_str->length();
				if (tmp_seq_str->release() == true)
				{
					tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].StringSeq = new DevVarStringArray(max,len,tmp_str,true);
				}
				else
				{
					tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
					(*dev_attr)[i].StringSeq = new DevVarStringArray(max,len,tmp_str,false);
				}
				break;
			}
		}	
	}

	return(dev_attr);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::read_attribute() - return a single attribute 
//
//-----------------------------------------------------------------------------
 

DeviceAttribute DeviceProxy::read_attribute(string& attr_string) 
{
	AttributeValueList_var attr_value_list;
	DeviceAttribute dev_attr;
	DevVarStringArray attr_list;

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		attr_list.length(1);
		attr_list[0] = CORBA::string_dup(attr_string.c_str());

		if (version == 2)
			attr_value_list = device_2->read_attributes_2(attr_list,source);
		else
			attr_value_list = device->read_attributes(attr_list);
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to read_attribute on device " << device_name;
		desc << ", attribute " << attr_string << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_AttributeFailed",
                        desc.str(), (const char*)"DeviceProxy::read_attribute()");
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","read_attribute");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to read_attribute on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::read_attribute()");
	}

	dev_attr.name = attr_value_list[0].name;
	dev_attr.quality = attr_value_list[0].quality;
	dev_attr.time = attr_value_list[0].time;
	dev_attr.dim_x = attr_value_list[0].dim_x;
	dev_attr.dim_y = attr_value_list[0].dim_y;

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	CORBA::ULong max,len;

	if (dev_attr.quality != Tango::ATTR_INVALID)
	{
		CORBA::TypeCode_var ty = attr_value_list[0].value.type();
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();		
		switch (ty_seq->kind())
		{
		case tk_long:		
			attr_value_list[0].value >>= tmp_seq_lo;
			max = tmp_seq_lo->maximum();
			len = tmp_seq_lo->length();
			if (tmp_seq_lo->release() == true)
			{
				tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
				dev_attr.LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
			}
			else
			{
				tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
				dev_attr.LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
			}
			break;
		
		case tk_short:
			attr_value_list[0].value >>= tmp_seq_sh;
			max = tmp_seq_sh->maximum();
			len = tmp_seq_sh->length();
			if (tmp_seq_sh->release() == true)
			{
				tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
				dev_attr.ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
			}
			else
			{
				tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
				dev_attr.ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
			}
			break;
		
		case tk_double:
			attr_value_list[0].value >>= tmp_seq_db;
			max = tmp_seq_db->maximum();
			len = tmp_seq_db->length();
			if (tmp_seq_db->release() == true)
			{
				tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
				dev_attr.DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
			}
			else
			{
				tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
				dev_attr.DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
			}
			break;
		
		case tk_string:
			attr_value_list[0].value >>= tmp_seq_str;
			max = tmp_seq_str->maximum();
			len = tmp_seq_str->length();
			if (tmp_seq_str->release() == true)
			{
				tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
				dev_attr.StringSeq = new DevVarStringArray(max,len,tmp_str,true);
			}
			else
			{
				tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
				dev_attr.StringSeq = new DevVarStringArray(max,len,tmp_str,false);
			}
			break;
		}
	}	

	return(dev_attr);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::write_attributes() - write a list of attributes
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::write_attributes(vector<DeviceAttribute>& attr_list) 
{
	AttributeValueList attr_value_list;

	attr_value_list.length(attr_list.size());
	for (int i=0; i<attr_list.size(); i++)
	{
		attr_value_list[i].name = attr_list[i].name.c_str();
		attr_value_list[i].quality = attr_list[i].quality;
		attr_value_list[i].time = attr_list[i].time;
		attr_value_list[i].dim_x = attr_list[i].dim_x;
		attr_value_list[i].dim_y = attr_list[i].dim_y;
		
		if (attr_list[i].LongSeq.operator->() != NULL)
		{
			attr_value_list[i].value <<= attr_list[i].LongSeq.in();	
			continue;
		}
		if (attr_list[i].ShortSeq.operator->() != NULL)
		{	
			attr_value_list[i].value <<= attr_list[i].ShortSeq.in();
			continue;
		}	
		if (attr_list[i].DoubleSeq.operator->() != NULL)
		{	
			attr_value_list[i].value <<= attr_list[i].DoubleSeq.in();
			continue;
		}
		if (attr_list[i].StringSeq.operator->() != NULL)
		{	
			attr_value_list[i].value  <<= attr_list[i].StringSeq.in();
			continue;
		}
	}

	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);
	
		device->write_attributes(attr_value_list);

	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","write_attributes");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::write_attributes()");
	}

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::write_attribute() - write a single attribute 
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::write_attribute(DeviceAttribute &dev_attr) 
{
	vector<DeviceAttribute> dev_attr_list;

	dev_attr_list.push_back(dev_attr);
	write_attributes(dev_attr_list);

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_list() - get list of attributes
//
//-----------------------------------------------------------------------------
 
vector<string> *DeviceProxy::get_attribute_list() 
{
	vector<string> all_attr;
	AttributeInfoList * all_attr_config;
	vector<string> *attr_list = new vector<string>;

	all_attr.push_back(AllAttr);
	all_attr_config = get_attribute_config(all_attr);

	attr_list->resize(all_attr_config->size());
	for (int i=0; i<all_attr_config->size(); i++)
	{
		(*attr_list)[i] = (*all_attr_config)[i].name;

	}
	delete all_attr_config;

	return attr_list;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::attribute_list_query() - get list of attributes
//
//-----------------------------------------------------------------------------
 
AttributeInfoList *DeviceProxy::attribute_list_query() 
{
	vector<string> all_attr;
	AttributeInfoList *all_attr_config;

	all_attr.push_back(AllAttr);
	all_attr_config = get_attribute_config(all_attr);

	return all_attr_config;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::command_history() - get command history (only for polled command)
//
//-----------------------------------------------------------------------------
 
vector<DeviceDataHistory> *DeviceProxy::command_history(string &cmd_name,int depth) 
{
	if (version == 1)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << device_name;
		desc << " does not support command_history feature" << ends;
		ApiNonSuppExcept::throw_exception((const char *)"API_UnsupportedFeature",
						  desc.str(),
						  (const char *)"DeviceProxy::command_history");
	}

	DevCmdHistoryList *hist;
		
	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		hist = device_2->command_inout_history_2(cmd_name.c_str(),depth);
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","command_history");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Command_history failed on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::command_history()");
	}

	int *ctr_ptr = new int;
	*ctr_ptr = 0;
		
	vector<DeviceDataHistory> *ddh = new vector<DeviceDataHistory>;
	ddh->reserve(hist->length());
	
	for (int i = 0;i < hist->length();i++)
	{
		ddh->push_back(DeviceDataHistory(i,ctr_ptr,hist));
	}

	return ddh;

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::attribute_history() - get attribute history
//				      (only for polled attribute)
//
//-----------------------------------------------------------------------------
 
vector<DeviceAttributeHistory> *DeviceProxy::attribute_history(string &cmd_name,int depth) 
{
	if (version == 1)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << device_name;
		desc << " does not support attribute_history feature" << ends;
		ApiNonSuppExcept::throw_exception((const char *)"API_UnsupportedFeature",
						  desc.str(),
						  (const char *)"DeviceProxy::attribute_history");
	}

	DevAttrHistoryList_var hist;
		
	try 
	{
		if (connection_state != CONNECTION_OK) reconnect(dbase_used);

		hist = device_2->read_attribute_history_2(cmd_name.c_str(),depth);
	}
	catch (CORBA::TRANSIENT &trans)
	{
		TRANSIENT_EXCEPT(trans,"DeviceProxy","attribute_history");
	}
        catch (CORBA::SystemException &ce)
        {
		connection_state = CONNECTION_NOTOK;
		
		TangoSys_OMemStream desc;
		desc << "Attribute_history failed on device " << device_name << ends;
		ApiCommExcept::re_throw_exception(ce,
					      (const char*)"API_CommunicationFailed",
                        		      desc.str(),
					      (const char*)"DeviceProxy::attribute_history()");
	}
		
	vector<DeviceAttributeHistory> *ddh = new vector<DeviceAttributeHistory>;
	ddh->reserve(hist->length());
	
	for (int i = 0;i < hist->length();i++)
	{
		ddh->push_back(DeviceAttributeHistory(i,hist));
	}

	return ddh;

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::connect_to_adm_device() - get device polling status
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::connect_to_adm_device() 
{
	string adm_dev_name = adm_name();
	
	adm_device = new DeviceProxy(adm_dev_name);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::polling_cmd_except() - Throw polling command exception
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::polling_cmd_except(Tango::DevFailed *ex,string &command,const char *Source) 
{
	TangoSys_OMemStream desc;
	desc << "Administration device for DS hosting device " << device_name;
	desc << " failed when executing " << command << " command" << ends;

	Tango::ConnectionFailed *con_ptr;
	Tango::CommunicationFailed *comm_ptr;
	Tango::DevFailed *fail_ptr;
				
	if ((con_ptr = dynamic_cast<Tango::ConnectionFailed *>(ex)) != NULL)
	{
		ApiConnExcept::re_throw_exception(*con_ptr,
						  (const char *)"API_",
						  desc.str(),Source);
	}
	else if ((comm_ptr = dynamic_cast<Tango::CommunicationFailed *>(ex)) != NULL)
	{
		ApiCommExcept::re_throw_exception(*comm_ptr,
						  (const char *)"API_",
						  desc.str(),Source);
	}
	else if ((fail_ptr = dynamic_cast<Tango::DevFailed *>(ex)) != NULL)
	{
		Except::re_throw_exception(*fail_ptr,
						  (const char *)"API_",
						  desc.str(),Source);
	}
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::polling_status() - get device polling status
//
//-----------------------------------------------------------------------------
 
vector<string> *DeviceProxy::polling_status() 
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DeviceData dout,din;
	string cmd("DevPollStatus");
	din.any <<= device_name.c_str();
	
//
// In case of connection failed error, do a re-try
//

	try
	{
		dout = adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		dout = adm_device->command_inout(cmd,din);
	}

	const DevVarStringArray *out_str;
	dout >> out_str;

	vector<string> *poll_stat = new vector<string>;
	poll_stat->reserve(out_str->length());
	
	for (int i = 0;i < out_str->length();i++)
	{
		string str = (*out_str)[i].in();
		poll_stat->push_back(str);
	}	
	return poll_stat;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_polled() - return true if the object "obj_name" is polled.
//			      In this case, the upd string is initialised with
//			      the polling period.
//
//-----------------------------------------------------------------------------
 
bool DeviceProxy::is_polled(polled_object obj, string &obj_name,string &upd) 
{
	bool ret = false;
	vector<string> *poll_str;
	
	poll_str = polling_status();
	if (poll_str->size() == 0)
		return ret;

	for (int i = 0;i < poll_str->size();i++)
	{
		string &tmp_str = (*poll_str)[i];
		string::size_type pos,end;
		pos = tmp_str.find(' ');
		pos++;
		end = tmp_str.find(' ',pos + 1);
		string obj_type = tmp_str.substr(pos, end - pos);
		if (obj_type == "command")
		{
			if (obj == Attr)
				continue;
		}
		else if (obj_type == "attribute")
		{
			if (obj == Cmd)
				continue;
		}
		
		pos = tmp_str.find('=');
		pos = pos + 2;
		end = tmp_str.find('\n',pos + 1);
		string name = tmp_str.substr(pos, end - pos);
		if (name == obj_name)
		{
		
//
// Now that it's found, serach for its polling period
//
			pos = tmp_str.find('=',end);
			pos = pos + 2;
			end = tmp_str.find('\n',pos + 1);
			string per = tmp_str.substr(pos, end - pos);
			upd = per;
			ret = true;
			break;
		}
	}
			
	delete poll_str;
	
	return  ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_command_poll_period() - Return command polling period
//					    (in mS)
//
//-----------------------------------------------------------------------------
 
int DeviceProxy::get_command_poll_period(string &cmd_name) 
{
	string poll_per;
	bool poll = is_polled(Cmd,cmd_name,poll_per);

	int ret;	
	if (poll == true)
	{
		TangoSys_MemStream stream;
		
		stream << poll_per << ends;
		stream >> ret;
	}
	else
		ret = 0;
		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::get_attribute_poll_period() - Return attribute polling period
//					    (in mS)
//
//-----------------------------------------------------------------------------
 
int DeviceProxy::get_attribute_poll_period(string &attr_name) 
{
	string poll_per;
	bool poll = is_polled(Attr,attr_name,poll_per);

	int ret;	
	if (poll == true)
	{
		TangoSys_MemStream stream;
		
		stream << poll_per << ends;
		stream >> ret;
	}
	else
		ret = 0;
		
	return ret;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::poll_command() - If object is already polled, just update its
//				 polling period. If object is not polled, add
//				 it to the list of polled objects
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::poll_command(string &cmd_name, int period) 
{
	string poll_per;
	bool poll = is_polled(Cmd,cmd_name,poll_per);
	
	DevVarLongStringArray in;
	in.lvalue.length(1);
	in.svalue.length(3);
	
	in.svalue[0] = CORBA::string_dup(device_name.c_str());
	in.svalue[1] = CORBA::string_dup("command");
	in.svalue[2] = CORBA::string_dup(cmd_name.c_str());
	in.lvalue[0] = period;
	
	if (poll == true)
	{
	
//
// If object is polled and the polling period is the same, simply retruns
//		
		TangoSys_MemStream stream;
		int per;
		
		stream << poll_per << ends;
		stream >> per;
		
		if (per == period)
			return;
		else
		{
	
//
// If object is polled, this is an update of the polling period
//

			DeviceData din;
			string cmd("UpdObjPollingPeriod");
			din.any <<= in;

			try
			{			
				adm_device->command_inout(cmd,din);
			}
			catch (Tango::CommunicationFailed &)
			{
				adm_device->command_inout(cmd,din);
			}

		}
	}
	else
	{
	
//
// This a AddObjPolling command
//

		DeviceData din;
		string cmd("AddObjPolling");
		din.any <<= in;

		try
		{			
			adm_device->command_inout(cmd,din);
		}
		catch (Tango::CommunicationFailed &)
		{
			adm_device->command_inout(cmd,din);
		}

	}
	
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::poll_attribute() - If object is already polled, just update its
//				 polling period. If object is not polled, add
//				 it to the list of polled objects
//
//-----------------------------------------------------------------------------
 
void DeviceProxy::poll_attribute(string &attr_name, int period) 
{
	string poll_per;
	bool poll = is_polled(Attr,attr_name,poll_per);
	
	DevVarLongStringArray in;
	in.lvalue.length(1);
	in.svalue.length(3);
	
	in.svalue[0] = CORBA::string_dup(device_name.c_str());
	in.svalue[1] = CORBA::string_dup("attribute");
	in.svalue[2] = CORBA::string_dup(attr_name.c_str());
	in.lvalue[0] = period;
	
	if (poll == true)
	{
	
//
// If object is polled and the polling period is the same, simply retruns
//		
		TangoSys_MemStream stream;
		int per;
		
		stream << poll_per << ends;
		stream >> per;
		
		if (per == period)
			return;
		else
		{
	
//
// If object is polled, this is an update of the polling period
//

			DeviceData din;
			string cmd("UpdObjPollingPeriod");
			din.any <<= in;

			try
			{			
				adm_device->command_inout(cmd,din);
			}
			catch (Tango::CommunicationFailed &)
			{
				adm_device->command_inout(cmd,din);
			}

		}
	}
	else
	{
	
//
// This a AddObjPolling command
//

		DeviceData din;
		string cmd("AddObjPolling");
		din.any <<= in;
		
		try
		{	
			adm_device->command_inout(cmd,din);
		}
		catch (Tango::CommunicationFailed &)
		{
			adm_device->command_inout(cmd,din);
		}

	}
	
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_cmd_polled() - return true if the command is polled
//
//-----------------------------------------------------------------------------

bool DeviceProxy::is_command_polled(string &cmd_name)
{
	string upd;
	return is_polled(Cmd,cmd_name,upd);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::is_attribute_polled() - return true if the attribute is polled
//
//-----------------------------------------------------------------------------

bool DeviceProxy::is_attribute_polled(string &attr_name)
{
	string upd;
	return is_polled(Attr,attr_name,upd);
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::stop_poll_command() - Stop polling a command
//
//-----------------------------------------------------------------------------

void DeviceProxy::stop_poll_command(string &cmd_name)
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DevVarStringArray in;
	in.length(3);
	
	in[0] = CORBA::string_dup(device_name.c_str());
	in[1] = CORBA::string_dup("command");
	in[2] = CORBA::string_dup(cmd_name.c_str());
	
	DeviceData din;
	string cmd("RemObjPolling");
	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd,din);
	}

}

//-----------------------------------------------------------------------------
//
// DeviceProxy::stop_poll_attribute() - Stop polling attribute
//
//-----------------------------------------------------------------------------

void DeviceProxy::stop_poll_attribute(string &attr_name)
{
	if (adm_device == NULL)
		connect_to_adm_device();
		
	DevVarStringArray in;
	in.length(3);
	
	in[0] = CORBA::string_dup(device_name.c_str());
	in[1] = CORBA::string_dup("attribute");
	in[2] = CORBA::string_dup(attr_name.c_str());
	
	DeviceData din;
	string cmd("RemObjPolling");
	din.any <<= in;

	try
	{			
		adm_device->command_inout(cmd,din);
	}
	catch (Tango::CommunicationFailed &)
	{
		adm_device->command_inout(cmd,din);
	}

}

} // End of Tango namespace

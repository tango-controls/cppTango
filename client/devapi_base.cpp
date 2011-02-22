static const char *RcsId = "$Header$";
//
// devapi_base.cpp 	- C++ source code file for TANGO device api 
//
// programmer(s)	- Andy Gotz (goetz@esrf.fr)
//
// original 		- March 2001
//
// log			- $Log$
// log			- Revision 1.2  2001/05/28 11:50:07  goetz
// log			- intermediate checkin
// log			-
// log			- Revision 1.1.1.1  2001/04/02 12:33:35  goetz
// log			- Imported sources
// log			-
//
// version 		- $Version$
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
#endif
#else
#include <sstream>
#endif /* STRSTREAM */
#include <tango.h>
#include <devapi.h>
#include <time.h>
#include <signal.h>
#include <OB/OBPolicies.h>
#ifdef _HPUX_SOURCE
namespace std{}
#endif /* _HPUX_SOURCE */                                                       
using namespace std;
using namespace CORBA;
using namespace Tango;

static CORBA::ORB *_orb=NULL;
static int _orb_created=0;

//-----------------------------------------------------------------------------
//
// Connection::Connection() - constructor to manage a connection to a device
//
//-----------------------------------------------------------------------------

Connection::Connection(ORB *orb_in)
{

	if ((orb_in == NULL) && (::_orb == NULL)) 
	{
		create_orb();
	}
	else
	{
		if (orb_in != NULL) ::_orb = orb_in;
		orb = ::_orb;
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

void Connection::connect(string corba_name)
{
//
// narrow CORBA string name to CORBA object
//
        Object_var obj;
	obj = ::_orb->string_to_object(corba_name.c_str());

        device = Device::_narrow(obj);

        if (CORBA::is_nil(device))
        {
        	cerr << "Can't build connection to object " << corba_name << endl;
        }

// set timeout to 3 seconds (default)

	set_timeout(3000);
}

//-----------------------------------------------------------------------------
//
// Connection::create_orb() - create a CORBA orb object
//
//-----------------------------------------------------------------------------

void Connection::create_orb()
{
	int _argc;
	char **_argv;
//
// pass dummy arguments to init() because we don't have access to
// argc and argv
//
       	_argc = 1;
       	_argv = (char**)malloc(sizeof(char*));
       	_argv[0] = (char*)"dummy";

	::_orb = ORB_init(_argc, _argv);
	::_orb_created = 1;
	orb = ::_orb;
}

//-----------------------------------------------------------------------------
//
// Connection::get_timeout() - public method to get timeout on a TANGO device
//
//-----------------------------------------------------------------------------

int Connection::get_timeout()
{
	return(timeout);
}


//-----------------------------------------------------------------------------
//
// Connection::set_timeout() - public method to set timeout on a TANGO device
//
//-----------------------------------------------------------------------------

void Connection::set_timeout(int millisecs)
{
	CORBA::PolicyList policy;
	CORBA::Any any;
	CORBA::Object_var policy_obj;
	CORBA::ULong ul_timeout;

	policy.length(1);

	ul_timeout = millisecs;

	any <<= ul_timeout;

	policy[0] = _orb->create_policy(OB::TIMEOUT_POLICY_ID,any);
       	policy_obj = device->_set_policy_overrides(policy,CORBA::ADD_OVERRIDE);
	device = Device::_narrow(policy_obj);

	timeout = millisecs;

	return;
}

//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//
//-----------------------------------------------------------------------------

DeviceData Connection::command_inout(string command)
{
	DeviceData data_in;

	return(command_inout(command.c_str(),data_in));

}
//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//
//-----------------------------------------------------------------------------

DeviceData Connection::command_inout(string command, DeviceData &data_in)
{
	CORBA::Any_var received = device->command_inout(command.c_str(),data_in.any);

	DeviceData data_out;
	data_out.any = received;

	return(data_out);
}

//-----------------------------------------------------------------------------
//
// Connection::command_inout() - public method to execute a command on a TANGO device
//				 using low level CORBA types
//
//-----------------------------------------------------------------------------

CORBA::Any_var Connection::command_inout(string command, CORBA::Any &any)
{
	return(device->command_inout(command.c_str(),any));

}
//-----------------------------------------------------------------------------
//
// DeviceProxy::DeviceProxy() - constructor for device proxy object 
//
//-----------------------------------------------------------------------------

DeviceProxy::DeviceProxy (string name, CORBA::ORB *orb) : Connection(orb)
{
	db_dev = new DbDevice(name);

	DbDevImportInfo import_info;

	import_info = db_dev->import_device();

	connect(import_info.ior);

	device_name = name;

	return;
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::~DeviceProxy() - destructor to destroy proxy to TANGO device
//
//-----------------------------------------------------------------------------

DeviceProxy::~DeviceProxy()
{
}

//-----------------------------------------------------------------------------
//
// DeviceProxy::ping() - ping TANGO device and return time elapsed in microseconds
//
//-----------------------------------------------------------------------------

int DeviceProxy::ping()
{
        struct timeval before, after;
	int elapsed;

	gettimeofday(&before, NULL);
	try
	{
		device->ping();
	}
	catch (CORBA::Exception &e)
	{
		Except::throw_exception("DeviceProxy failed",
		        "failed to ping device ",
		        "DeviceProxy::ping()");

	}
	gettimeofday(&after, NULL);
	elapsed = (after.tv_sec-before.tv_sec)*1000000;
	elapsed = (after.tv_usec-before.tv_usec) + elapsed;

	return(elapsed);
}


//-----------------------------------------------------------------------------
//
// DeviceProxy::command_list_query() - return the list of commands implemented for this TANGO device 
//
//-----------------------------------------------------------------------------
 
DevCommandInfoList DeviceProxy::command_list_query() 
{
	DevCommandInfoList command_info_list;
	DevCmdInfoList_var cmd_info_list;

	try 
	{
	
		cmd_info_list = device->command_list_query();

	}
        catch (CORBA::Exception &e)
        {
                Except::throw_exception("DeviceProxy failed",
                        "failed to query device for list of commands device ",
                        "DeviceProxy::command_list_query()");
        }                                                                       

	command_info_list.resize(cmd_info_list->length());

	for (int i=0; i<cmd_info_list->length(); i++)
	{
		command_info_list[i].cmd_name = (*cmd_info_list)[i].cmd_name;
		command_info_list[i].cmd_tag = (*cmd_info_list)[i].cmd_tag;
		command_info_list[i].in_type = (*cmd_info_list)[i].in_type;
		command_info_list[i].out_type = (*cmd_info_list)[i].out_type;
		command_info_list[i].in_type_desc = (*cmd_info_list)[i].in_type_desc;
		command_info_list[i].out_type_desc = (*cmd_info_list)[i].out_type_desc;
	}

	return(command_info_list);
}

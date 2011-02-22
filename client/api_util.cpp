static const char *RcsId = "$Header$";

//
// cpp 	- C++ source code file for TANGO api class ApiUtil
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- May 2002
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
//
//

#include <tango.h>
                                                    
namespace Tango
{

ApiUtil *ApiUtil::_instance = NULL;


//+----------------------------------------------------------------------------
//
// method : 		ApiUtil::ApiUtil()
// 
// description : 	Constructor of the ApiUtil class.
//
//-----------------------------------------------------------------------------

ApiUtil::ApiUtil()
{
	_orb = NULL;
	
//
// Check if it is created from a device server
//

	Tango::Util *tg;
	try
	{
		tg = Tango::Util::instance(false);
		in_serv = true;
	}
	catch (Tango::DevFailed &)
	{
		in_serv = false;
	}
}

//-----------------------------------------------------------------------------
//
// ApiUtil::create_orb() - create a CORBA orb object
//
//-----------------------------------------------------------------------------

void ApiUtil::create_orb()
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

//
// Init the ORB
//		

	const char *options[][2] = {
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};
				
	_orb = CORBA::ORB_init(_argc,_argv,"omniORB4",options);
	
	free(_argv);
}

//-----------------------------------------------------------------------------
//
// ApiUtil::get_db_obj() - Retrieve a Tango database object created from the
//			   TANGO_HOST environment variable
//
//-----------------------------------------------------------------------------

int ApiUtil::get_db_ind()
{
	for (int i = 0;i < db_vect.size();i++)
	{
		if (db_vect[i]->get_from_env_var() == true)
			return i;
	}
	
//
// The database object has not been founded, create it
//

	db_vect.push_back(new Database());
	
	return (db_vect.size() - 1);
	
}

int ApiUtil::get_db_ind(string &host,int port)
{
	for (int i = 0;i < db_vect.size();i++)
	{
		if (db_vect[i]->get_db_port_num() == port)
		{
			if (db_vect[i]->get_db_host() == host)
				return i;
		}
	}
	
//
// The database object has not been founded, create it
//

	db_vect.push_back(new Database(host,port));
	
	return (db_vect.size() - 1);
	
}

} // End of tango namespace

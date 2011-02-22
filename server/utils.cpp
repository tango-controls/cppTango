static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               Tango_utils.cpp
//
// description :        C++ source for all the utilities used by Tango device
//			server and mainly for the Tango class
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
// $Revision$
//
// $Log$
// Revision 3.59  2009/06/17 08:44:45  taurel
// - Unix servers are now listenning also on a UNIX domain socket for
// looback communication
//
// Revision 3.58  2009/03/18 10:23:12  taurel
// - Fix bug for Windows in GUI mode
//
// Revision 3.57  2009/03/17 08:10:42  taurel
// - Remove the endPoint specification before the ORB_init() method call
// - Still some Windows warnings fixes
//
// Revision 3.56  2009/03/04 08:26:55  taurel
// - Windows DS: Add a console handler for DS to survive a Log-off
//
// Revision 3.55  2009/02/26 07:48:21  taurel
// - The attribute data format is now transferred within the AttributeValue_4 structure
//
// Revision 3.54  2009/02/16 12:07:19  taurel
// - Fix bug in Util::get_device_by_name() for admin device
//
// Revision 3.53  2009/02/03 15:18:51  jensmeyer
// Extract sub device properties from the db_cache when the cache
// is available.
//
// Revision 3.52  2009/01/29 16:23:49  taurel
// - Commit after merge with branch Release_6_1_1-bugfixes
//
// Revision 3.51  2009/01/23 13:58:18  taurel
// - Print out wrong option from the command line before aborting
//
// Revision 3.50  2009/01/21 12:49:03  taurel
// - Change CopyRights for 2009
//
// Revision 3.49  2009/01/15 13:57:20  taurel
// - Fix bugs found by Jens
//
// Revision 3.48  2008/10/13 14:59:38  taurel
// - Set the ORB verifyObjectExistsAndType option to false when creating the ORB
//
// Revision 3.47  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.46  2008/10/03 06:53:09  taurel
// - Add some licensing info in each files
//
// Revision 3.45  2008/10/02 12:24:43  taurel
// - The user now has the possibility to install its own event loop
//
// Revision 3.44  2008/10/02 09:09:47  taurel
// - First implementation of multiple polling thread(s)
//
// Revision 3.43  2008/09/23 14:59:35  taurel
// - Commit after the end of DevEncoded data type implementation
// - The new test suite is also now running fine
//
// Revision 3.42  2008/06/10 07:52:15  taurel
// - Add code for the DevEncoded attribute data type
// Revision 3.40.2.2  2009/01/29 15:17:01  taurel
// - Add some bug fixes for server used with the -f option (database as
// file)
//
// Revision 3.41  2008/05/20 12:44:20  taurel
// - Commit after merge with release 7 branch
// Revision 3.40.2.1  2008/05/22 12:10:53  taurel
// - Fix bug for server started without database (the db server itself for instance)
//
// Revision 3.40  2008/03/26 16:51:12  taurel
// - Change the printed message when a DS starts (on cout instead of cerr)
// Revision 3.32.2.3  2008/05/20 06:17:46  taurel
// - Last commit before merge with trunk
// (start the implementation of the new DevEncoded data type)
//
// Revision 3.39  2008/03/25 14:59:01  taurel
// - Fix bug in the previous change
// Revision 3.32.2.2  2008/02/07 15:58:15  taurel
// - First implementation of the Controlled Access done
//
// Revision 3.38  2008/03/25 13:32:25  taurel
// - Change when the RootPOA CORBA object is created. This allows DS to
// start on windows in daemon mode even if the PC is not connected to the
// network at boot time
//
// Revision 3.37  2008/03/14 11:56:03  taurel
// - Fix some gcc 4.2 warnings
//
// Revision 3.36  2008/02/29 12:53:08  taurel
// - Increase timeout for DB get data cache command
// - Add a method to unvalidate the cache
//
// Revision 3.35  2008/01/25 15:44:51  taurel
// - Some changes in the Db cache
// - A lighter system to shutdown DS in case of dynamic attribute
//
// Revision 3.34  2008/01/15 12:29:55  taurel
// - Change db timeout when getting data for the db cache
// Revision 3.32.2.1  2007/12/19 15:54:48  taurel
// - Still some work going on for the locking feature
//
// Revision 3.33  2007/12/12 10:17:18  taurel
// - Db calls during DS startup has a separate timeout and some retries
//
// Revision 3.32  2007/11/08 12:03:45  taurel
// - Start implementing user interceptors
// - Fix bug in poll thread pproperty management when removing polling object
// - Set a database timeout to 6 sec
//
// Revision 3.31  2007/10/26 11:27:43  taurel
// - Add some print-out for test purposes
//
// Revision 3.30  2007/10/16 08:24:14  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#include <stdlib.h>
#include <dserversignal.h>
#include <dserverclass.h>
#include <eventsupplier.h>

#ifndef _TG_WINDOWS_
	#include <unistd.h>
	#include <assert.h>
	#include <sys/time.h>
	#include <netdb.h>
#else
	#include <sys/timeb.h>
	#include <process.h>
        #include <coutbuf.h>
	#include <ntservice.h>
#endif /* _TG_WINDOWS_ */

#include <omniORB4/omniInterceptors.h>

omni_thread::key_t key_py_data;

namespace Tango
{

Util *Util::_instance = NULL;
int Util::_tracelevel = 0;
bool Util::_UseDb = true;
bool Util::_FileDb = false;
bool Util::_daemon = false;
long Util::_sleep_between_connect = 60;
bool Util::_constructed = false;
#ifdef _TG_WINDOWS_
bool Util::_win = false;
bool Util::_service = false;
#endif

//
// A global key used for per thread specific storage. This is used to retrieve
// client host and stores it in the device blackbox. This global is referenced
// in blackbox.cpp
//


omni_thread::key_t key;


//+----------------------------------------------------------------------------
//
// method : 		Util::init()
// 
// description : 	static method to create/retrieve the Tango object.
//			This method is the only one which enables a user to
//			create the object
//
// in :			- argc : The command line argument number
//			- argv : The command line argument
//
//-----------------------------------------------------------------------------

Util *Util::init(int argc,char *argv[])
{
	if (_instance == NULL)
	{
		_instance = new Util(argc,argv);
	}		
	return _instance;
}

#ifdef _TG_WINDOWS_
Util *Util::init(HINSTANCE hi,int nCmd)
{
	if (_instance == NULL)
	{
		_instance = new Util(hi,nCmd);
	}		
	return _instance;
}
#endif

//+----------------------------------------------------------------------------
//
// method : 		Util::instance()
// 
// description : 	static method to retrieve the Tango object once it has
//			been initialised
//
//-----------------------------------------------------------------------------

Util *Util::instance(bool exit)
{
	if (_instance == NULL)
	{
		if (exit == true) 
                	Util::print_err_message("Tango is not initialised !!!\nExiting");
		else
		{
			Except::throw_exception((const char*)"API_UtilSingletonNotCreated",
                        		   	(const char*)"Util singleton not created",
					   	(const char*)"Util::instance");
		}
	}		
	return _instance;
}

//+----------------------------------------------------------------------------
//
// method : 		Util::Util()
// 
// description : 	Constructor of the Tango class.
//
// in :			- argc : The command line argument number
//			- argv : The command line argument
//
//-----------------------------------------------------------------------------

#if ((defined __SUNPRO_CC) || (defined GCC_STD))
Util::Util(int argc,char *argv[]):cl_list_ptr(NULL)
# ifndef TANGO_HAS_LOG4TANGO
    ,cout_tmp(cout.rdbuf())
# endif
#elif (defined _TG_WINDOWS_)
Util::Util(int argc,char *argv[]):cl_list_ptr(NULL),mon("Windows startup")
# ifndef TANGO_HAS_LOG4TANGO
    ,cout_tmp(cout.rdbuf())
# endif
#else
Util::Util(int argc,char *argv[]):cl_list_ptr(NULL)
#endif
{
//
// Create the UtilExt instance
//

	ext = new UtilExt;

//
// Do the job
//

	effective_job(argc,argv);
	_constructed = true;

//
// For Windows, install a console handler to ignore LOGOFF event
//

#ifdef _TG_WINDOWS_
	install_cons_handler();
#endif
}

void Util::effective_job(int argc,char *argv[])
{
	try
	{

//
// Check server option
//

		if (argc < 2)
		{
			print_usage(argv[0]);
		}
	   
//
// Manage command line option (personal name and -v option)
//

		check_args(argc,argv);

//
// Create the signal object
// It is necessary to create this object before the ORB is initialised.
// Otherwise, threads created by thread started by the ORB_init will not have
// the correct signal mask (set by the DServerSignal object) and the device
// server signal feature will not work
//

		DServerSignal::instance();

//
// Initialise CORBA ORB
//

#ifdef _TG_WINDOWS_
		WORD rel = 0x0202;
		WSADATA dat;
		WSAStartup(rel,&dat);
#endif

		char h_name[80];
		int res = gethostname(h_name,80);
		if (res == 0)
		{
			const char *options[][2] = {
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT_STR},
			{"serverCallTimeOutPeriod","5000"},
			{"maxServerThreadPoolSize","100"},
			{"threadPerConnectionUpperLimit","55"},
			{"threadPerConnectionLowerLimit","50"},
			{"supportCurrent","0"},
			{"verifyObjectExistsAndType","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
#ifndef _TG_WINDOWS_
			{"endPoint","giop:tcp::"},
			{"endPoint","giop:unix:"},
#endif
			{0,0}};
			
			orb = CORBA::ORB_init(argc,argv,"omniORB4",options);
		}
		else
		{
			const char *options[][2] = {
			{"endPointPublish","all(addr)"},
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT_STR},
			{"serverCallTimeOutPeriod","5000"},
			{"maxServerThreadPoolSize","100"},
			{"threadPerConnectionUpperLimit","55"},
			{"threadPerConnectionLowerLimit","50"},
			{"supportCurrent","0"},
			{"verifyObjectExistsAndType","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
#ifndef _TG_WINDOWS_
			{"endPoint","giop:tcp::"},
			{"endPoint","giop:unix:"},
#endif
			{0,0}};
			
			orb = CORBA::ORB_init(argc,argv,"omniORB4",options);
		}
		

#ifndef TANGO_HAS_LOG4TANGO

//
// Initialize trace output (For Windows, the stdc++ lib also implements the
// new iostreams where the xx_with_assign classes are not defined. Therefore,
// to copy streams, I have used the advices in the C++ report of June 1997 
//

  		trace_output = InitialOutput;
		file_stream = NULL;

# if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
		cout_tmp.copyfmt(cout);
		cout_tmp.clear(cout.rdstate());
# else
		cout_tmp = cout;
# endif 
#endif // TANGO_HAS_LOG4TANGO

//
// Init host name
//
		
		init_host_name();
		
//
// Connect to the database
//

		if (_UseDb == true)
		{
			connect_db();

//
// Display help message if requested by user. This is done after the process is 
// connected to the database becaue a call to the database server is done in the
// display_help_message() method
//

			if (display_help == true)
			{
				display_help_message();
			}
		}
		else
			db = NULL;

//
// Create the server CORBA objects
//

		create_CORBA_objects();
		
#ifdef TANGO_HAS_LOG4TANGO	
//
// Initialize logging stuffs
//
		Logging::init(ds_name, (int)_tracelevel,  ((!_FileDb) && _UseDb), *db, this);
#endif

		cout4 << "Connected to database" << endl;
		if (get_db_cache() == NULL)
			cout4 << "DbServerCache unavailable, will call db..." << endl;

//
// Check if the server is not already running somewhere else
//

		if ((_UseDb == true) && (_FileDb == false))
			server_already_running();

//
// Get process PID and Tango version
//

		misc_init();
						
//
// Automatically create the EventSupplier object
//
// In the future this could be created only when the 
// first event is fired ...
//

		create_event_supplier();

//
// Create the heartbeat thread and start it
//

		ext->heartbeat_th = new PollThread(ext->shared_data,ext->poll_mon,true);
		ext->heartbeat_th->start();
		ext->heartbeat_th_id = ext->heartbeat_th->id();
		cout4 << "Heartbeat thread Id = " << ext->heartbeat_th_id;
								
		cout4 << "Tango object singleton constructed" << endl;
		
	}
	catch (CORBA::Exception &)
	{
		throw;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		Util::create_CORBA_objects()
// 
// description : 	Create some CORBA objects needed later-on
//
//-----------------------------------------------------------------------------

void Util::create_CORBA_objects()
{

//
// Install an omniORB interceptors to store client name in blackbox
// and allocate a key for per thread specific storage
//

	omni::omniInterceptors *intercep = omniORB::getInterceptors();
	intercep->serverReceiveRequest.add(get_client_addr);
	intercep->createThread.add(create_PyPerThData);

	key = omni_thread::allocate_key();
	key_py_data = omni_thread::allocate_key();
				
//
// Get some CORBA object references
//

	CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA");
	PortableServer::POA_var root_poa = PortableServer::POA::_narrow(poaObj);

//
// If the database is not used, we must used the omniINSPOA poa
// in both cases of database on file or nodb
// remember that when you have database on file you have
//  _UseDb == true and  _FileDb == true
//

	PortableServer::POA_var nodb_poa;
	if ((_UseDb == false) || (_FileDb == true))
	{
		CORBA::Object_var poaInsObj = orb->resolve_initial_references("omniINSPOA");
		nodb_poa = PortableServer::POA::_narrow(poaInsObj);	
	}
		
//
// Store POA
// this is the same test but inverted
//

	if ((_UseDb == true) && (_FileDb == false))
		_poa = PortableServer::POA::_duplicate(root_poa);
	else if ((_UseDb == false) || (_FileDb == true))
		_poa = PortableServer::POA::_duplicate(nodb_poa);
}



#ifdef _TG_WINDOWS_
//+----------------------------------------------------------------------------
//
// method : 		Util::Util()
// 
// description : 	Constructor of the Tango class when used in a non-console
//                      Windows device server. On top of the UNIX way of building
//                      a Util singleton, for Windows non-console mode, it is 
//                      necessary to :
//                      - Build a UNIX like argc,argv from the command line
//                      - Initialise the OB reactor
//                      - Create a debug output window if verbose mode is requested
//                        and change cout so that it prints into this window
//                        
// in :			- hInst : The application instance
//			- nCmdShow : The display window flag
//
//-----------------------------------------------------------------------------

Util::Util(HINSTANCE hInst,int nCmdShow):cl_list_ptr(NULL),mon("Windows startup")
#ifndef TANGO_HAS_LOG4TANGO
  ,cout_tmp(cout.rdbuf())
#endif
{
//
// Create the UtilExt instance
//

	ext = new UtilExt;
	
//
// This method should be called from a Windows graphic program
//

	_win = true;

//
// Build UNIX like command argument(s)
//

	build_argc_argv();

//
// Really do the job now
//

	effective_job(argc,argv);

//
// Store the nCmdShow parameter an mark the object has being completely
// constructed. Usefull, in case one of the method previously called
// failed, the orb variable (type ORB_var) is alaready destroyed therefore
// releasing the orb pointer.
//

	nCmd = nCmdShow;
	_constructed = true;
	
}
#endif

//+----------------------------------------------------------------------------
//
// method : 		Util::ckeck_args()
// 
// description : 	Check the command line arguments. The first one is
//			mandatory and is the server personal name. A -v option
//			is authorized with an optional argument. The other
//			option should be ORBacus option
//
// in :			- argc : The command line argument number
//			- argv : The command line argument
//
//-----------------------------------------------------------------------------

void Util::check_args(int argc,char *argv[])
{

//
// Check command line argument
//

	string first_arg(argv[1]);
	display_help = false;
	
	if ((argc == 2) && (_UseDb == true))
	{
		if ((first_arg == "-?") || (first_arg == "-help") || (first_arg == "-h"))
		{
			display_help = true;
		}
	}
		
	if ((display_help == false) && (argv[1][0] == '-'))
	{
		print_usage(argv[0]);
	}
	ds_instance_name = argv[1];
	char *tmp;
#ifdef _TG_WINDOWS_
	if ((tmp = strrchr(argv[0],'\\')) == 0)
	{
		if ((tmp = strrchr(argv[0],'/')) == 0)
		{
			ds_exec_name = argv[0];
		}
		else
		{
			tmp++;
			ds_exec_name = tmp;
		}
	}
	else
	{
		tmp++;
		ds_exec_name = tmp;
	}
#else
	if ((tmp = strrchr(argv[0],'/')) == 0)
		ds_exec_name = argv[0];
	else
	{
		tmp++;
		ds_exec_name = tmp;
	}
#endif
	
//
// For Windows only. Remove the .exe after the executable name
//

#ifdef _TG_WINDOWS_
	string::size_type pos;
	if ((pos = ds_exec_name.find('.')) != string::npos)
	{
		ds_exec_name.erase(pos,ds_exec_name.size());
	}
#endif /* _TG_WINDOWS_ */

	if (argc > 2)
	{
		long ind = 2;
		string dlist;	
		while (ind < argc)
		{
			if (argv[ind][0] == '-')
			{
				switch (argv[ind][1])
				{
				
//
// The verbose option
//

				case 'v':
					if (strlen(argv[ind]) == 2)
					{
						if ((argc - 1)> ind)
						{
							if (argv[ind + 1][0] == '-')
								set_trace_level(4);
							else
							{
								cerr << "Unknown option " << argv[ind] << endl;
								print_usage(argv[0]);
							}
						}
						else
							set_trace_level(4);
						ind++;
					}
					else
					{
						long level = atol(&(argv[ind][2]));
						if (level == 0)
						{
							cerr << "Unknown option " << argv[ind] << endl;
							print_usage(argv[0]);
						}
						else
						{
							set_trace_level(level);
							ind++;
						}
					}
					break;

//
// Device server without database
//
					
				case 'n':
					if (strcmp(argv[ind],"-nodb") != 0)
					{
						cerr << "Unknown option " << argv[ind] << endl;
						print_usage(argv[0]);
					}
					else
					{
						if ( _FileDb )
							print_usage(argv[0]);
						_UseDb = false;
						ind++;
					}
					break;

				case 'f':
					if (strncmp(argv[ind],"-file=",6) != 0)
					{
						cerr << "Unknown option " << argv[ind] << endl;
						print_usage(argv[0]);
					}
					else
					{
						if ( !_UseDb )
							print_usage(argv[0]);
						Tango::Util::_FileDb = true;
						database_file_name = argv[ind]; 
						database_file_name.erase(0,6);
#ifdef _TG_WINDOWS_
						replace(database_file_name.begin(),
							database_file_name.end(),
							'\\','/');
#endif
						cout4 << "File name = <" << database_file_name << ">" << endl;
						ind++;
					}
					break;

//
// Device list (for device server without database)
//
					
				case 'd':
					if (strcmp(argv[ind],"-dbg")==0)
					{
						ind++;
						break;
					}
					
					if (strcmp(argv[ind],"-dlist") != 0)
					{
						cerr << "Unknown option " << argv[ind] << endl;
						print_usage(argv[0]);
					}
					else
					{
						if (_UseDb == true)
							print_usage(argv[0]);
							
						ind++;
						if (ind == argc)
							print_usage(argv[0]);
						else
						{
							dlist = argv[ind];

//
// Extract each device name
//

							string::size_type start = 0;
							string str;
							string::size_type pos;
							vector<string> &list = get_cmd_line_name_list();	
	
							while ((pos = dlist.find(',',start)) != string::npos)
							{
								str = dlist.substr(start,pos - start);
								start = pos + 1;
								list.push_back(str);
							}
							if (start != dlist.size())
							{
								str = dlist.substr(start);
								list.push_back(str);
							}

//
// Check that the same device name is not used twice
//
			
							unsigned long i,j;
							for (i = 0;i < list.size();i++)
							{
								for (j = 0;j < list.size();j++)
								{
									if (i == j)
										continue;
									else
									{
										if (list[i] == list[j])
										{
        										print_err_message("Each device must have different name",Tango::INFO);
										}
									}
								}
							} 
						}
					}
				
				default: 
					ind++;
					break;
				}
			}
			else
			{
				if (strncmp(argv[ind - 1],"-v",2) == 0)
				{
					print_usage(argv[0]);
				}
				ind++;
			}
		}
	}
	
//
// Build server name
//

//	long ctr;
//	for (ctr = 0;ctr < ds_exec_name.size();ctr++)
//		ds_exec_name[ctr] = tolower(ds_exec_name[ctr]);
//	for (ctr = 0;ctr < ds_instance_name.size();ctr++)
//		ds_instance_name[ctr] = tolower(ds_instance_name[ctr]);
	ds_name = ds_exec_name;
	ds_name.append("/");
	ds_name.append(ds_instance_name);
	
//
// Check that the server name is not too long
//

	if (ds_name.size() > MaxServerNameLength)
	{
                TangoSys_OMemStream o;
		
	        o << "The device server name is too long! Max length is " << MaxServerNameLength << " characters" << ends;
		print_err_message(o.str(),Tango::INFO);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		Util::display_help_message()
// 
// description : 	Check the command line arguments. The first one is
//			mandatory and is the server personal name. A -v option
//			is authorized with an optional argument. The other
//			option should be ORBacus option
//
// in :			- argc : The command line argument number
//			- argv : The command line argument
//
//-----------------------------------------------------------------------------

void Util::display_help_message()
{

//
// Display device server usage string
//

	TangoSys_OMemStream o;
		
	o << "usage :  " << ds_exec_name << " instance_name [-v[trace level]]";
	o << " [-nodb [-dlist <device name list>]]";
	
//
// Try to get instance name from db
//

	string str("dserver/");
	str.append(ds_exec_name);
	str.append("/*");

	vector<string> db_inst;	
		
	try
	{
		DbDatum received = db->get_device_member(str);
		received >> db_inst;
	}
	catch (Tango::DevFailed &)
	{
		o << ends;
		print_err_message(o.str(),Tango::INFO);
	}
	
//
// Add instance name list to message
//

	o << "\nInstance name defined in database for server " << ds_exec_name << " :";
	for (unsigned long i = 0;i < db_inst.size();i++)
	{
		o << "\n\t" << db_inst[i];
	}
	o << ends;
	
//
// Display message
//

	print_err_message(o.str(),Tango::INFO);

}

//+----------------------------------------------------------------------------
//
// method : 		Util::print_usage()
// 
// description : 	Print device server command line syntax
//
// in :			- serv_name : The server name
//
//-----------------------------------------------------------------------------

void Util::print_usage(char *serv_name)
{
	TangoSys_OMemStream o;
		
	o << "usage :  " << serv_name << " instance_name [-v[trace level]]";
	o << " [-file=<file_name> | -nodb [-dlist <device name list>] ]" << ends;
	
	print_err_message(o.str(),Tango::INFO);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::connect_db()
// 
// description : 	This method builds a connection to the Tango database
//			servant. It uses the db_host and db_port object 
//			variables. The Tango database server implements its
//			CORBA object as named servant.
//
//-----------------------------------------------------------------------------

void Util::connect_db()
{

//
// Try to create the Database object
//

	if (_daemon == true)
	{	
		int connected = false;
		while (connected == false)
		{
			try
			{
#ifdef _TG_WINDOWS_
				if (_service == true)
					db = new Database(orb.in(),
							  ds_exec_name,
							  ds_instance_name);
				else
				{
					if (_FileDb == false)
						db = new Database(orb.in());
					else
						db = new Database(database_file_name);
				}
#else
				if (_FileDb == false)
					db = new Database(orb.in());
				else
					db = new Database(database_file_name);
#endif
				db->set_tango_utils(this);
				connected = true;
			}
			catch (Tango::DevFailed &e)
			{
				if (strcmp(e.errors[0].reason.in(),"API_TangoHostNotSet") == 0)
				{
					print_err_message(e.errors[0].desc.in());
				}
				else
				{
					cout4 << "Can't contact db server, will try later" << endl;
					Tango_sleep(_sleep_between_connect);
				}				
			}
			catch (CORBA::Exception &)
			{
				cout4 << "Can't contact db server, will try later" << endl;
				Tango_sleep(_sleep_between_connect);
			}

		}
	}
	else
	{
       	try
        {
#ifdef _TG_WINDOWS_
			if (_service == true)
				db = new Database(orb.in(),
						  ds_exec_name,
						  ds_instance_name);
			else
			{
				if (_FileDb == false)
					db = new Database(orb.in());
				else
					db = new Database(database_file_name);
			}
#else
			if (_FileDb == false)
				db = new Database(orb.in());
			else
				db = new Database(database_file_name);
#endif
			db->set_tango_utils(this);
		}
		catch (Tango::DevFailed &e)
		{
			if (e.errors.length() == 2)
			{
				if (strcmp(e.errors[1].reason.in(),"API_CantConnectToDatabase") == 0)
				{
				
					TangoSys_OMemStream o;
				
					o << "Can't build connection to TANGO database server, exiting";
					print_err_message(o.str());
				}
				else
					print_err_message(e.errors[1].desc.in());
			}
			else
				print_err_message(e.errors[0].desc.in());
		}
		catch (CORBA::Exception &)
		{
			TangoSys_OMemStream o;

			o << "Can't build connection to TANGO database server, exiting";
			print_err_message(o.str());
		}
	}
	
	if (CORBA::is_nil(db->get_dbase()) && _FileDb != true)
	{
		TangoSys_OMemStream o;

		o << "Can't build connection to TANGO database server, exiting" << ends;
		print_err_message(o.str());
	}

//
// Set a timeout on the database device
//

	if (_FileDb == false)	
		db->set_timeout_millis(DB_TIMEOUT);
	
//
// Also copy this database object ptr into the ApiUtil object. Therefore,
// the same database connection will be used also for DeviceProxy
// object created within the server
//

	ApiUtil *au = ApiUtil::instance();
	au->get_db_vect().push_back(db);
	au->in_server(true);
	
//
// Try to create the db cache which will be used during the process
// startup sequence
// For servers with many devices and properties, this could take time
// specially during a massive DS startup (after power cut for instance)
// Filling the DS cache is a relatively heavy command for the DB server
// Trying to minimize retry in this case could be a good idea.
// Therefore, change DB device timeout to execute this command
//

	if (_FileDb == false)
	{
		string &inst_name = get_ds_inst_name();
		if (inst_name != "-?")
		{
			db->set_timeout_millis(DB_TIMEOUT * 3);
			try
			{
				ext->db_cache = new DbServerCache(db,get_ds_name(),get_host_name());
			}
			catch (Tango::DevFailed &) {}
			catch (...) 
			{
				cerr << "Unknown exception while trying to fill database cache..." << endl;
			}
			db->set_timeout_millis(DB_TIMEOUT);
		}
	}
}


void Util::reset_filedatabase()
{
	delete db;
	db = new Database(database_file_name);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::misc_init()
// 
// description : 	This method initialises miscellaneous variable which
//			are needed later in the device server startup 
//			sequence. These variables are :
//				The process ID
//				The Tango version
//
//-----------------------------------------------------------------------------

void Util::misc_init()
{

//
// Get PID
//

	TangoSys_OMemStream o;
	
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
	#ifdef _TG_WINDOWS_
	pid = _getpid();
	#else
		#ifdef __linux
	pid = DServerSignal::instance()->get_sig_thread_pid();
		#else
	pid = getpid();
		#endif
	#endif
	
	o << pid << ends;
	pid_str = o.str();
#else
	#ifdef __linux
		pid = DServerSignal::instance()->get_sig_thread_pid();
	#else
		pid = getpid();
	#endif
	
	o << pid << ends;
	pid_str = o.str();
	o.rdbuf()->freeze(false);
#endif

//
// Get the FQDN host name (Fully qualified domain name)
// If it is not returned by the system call "getname",
// try with the gethostbyname system call
//
		
	char buffer[80];
	if (gethostname(buffer,80) == 0)
	{
		hostname = buffer;
		string::size_type pos = hostname.find('.');

		if (pos == string::npos)
		{
			struct hostent *he;
			he = gethostbyname(buffer);
			
			if (he != NULL)
			{
				string na(he->h_name);
				pos = na.find('.');
				if (pos == string::npos)
				{
					char **p;
					for (p = he->h_aliases;*p != 0;++p)
					{
						string al(*p);
						pos = al.find('.');
						if (pos != string::npos)
						{
							hostname = al;
							break;
						}					
					}
				}
				else
					hostname = na;
			}
		}
	}
	else
	{
		print_err_message("Cant retrieve server host name");
	}
	
//
// Convert Tango version number to string (for device export)
//
		
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
	o.seekp(0,ios_base::beg);
	o.clear();
	o << DevVersion << ends;
	version_str = o.str();
#else
	o.rdbuf()->seekoff(0,ios::beg,ios::in | ios::out);
	o.clear();		
	o << DevVersion << ends;
	version_str = o.str();
	o.rdbuf()->freeze(false);
#endif

//
// Init server version to a default value
//

	server_version = "x.y";

//
// Init text to be displayed on main window with a default value
//

#ifdef _TG_WINDOWS_
	main_win_text = "TANGO collaboration\n";
	main_win_text = main_win_text + "(ALBA / DESY / ELETTRA / ESRF / SOLEIL )\n";
	main_win_text = main_win_text + "CORBA based device server\n";
	main_win_text = main_win_text + "Developped by Tango team";
#endif
}

//+----------------------------------------------------------------------------
//
// method : 		Util::init_host_name()
// 
// description : 	This method initialises the process hst name which
//			is needed later in the device server startup 
//			sequence.
//
//-----------------------------------------------------------------------------

void Util::init_host_name()
{

//
// Get the FQDN host name (Fully qualified domain name)
// If it is not returned by the system call "getname",
// try with the gethostbyname system call
//
		
	char buffer[80];
	if (gethostname(buffer,80) == 0)
	{
		hostname = buffer;
		string::size_type pos = hostname.find('.');

		if (pos == string::npos)
		{
			struct hostent *he;
			he = gethostbyname(buffer);
			
			if (he != NULL)
			{
				string na(he->h_name);
				pos = na.find('.');
				if (pos == string::npos)
				{
					char **p;
					for (p = he->h_aliases;*p != 0;++p)
					{
						string al(*p);
						pos = al.find('.');
						if (pos != string::npos)
						{
							hostname = al;
							break;
						}					
					}
				}
				else
					hostname = na;
			}
		}
	}
	else
	{
		print_err_message("Cant retrieve server host name");
	}
}

//+----------------------------------------------------------------------------
//
// method : 		Util::create_event_supplier()
// 
// description : 	This method create the event_supplier if possible
//
//-----------------------------------------------------------------------------

void Util::create_event_supplier()
{
	if (_UseDb == true)
	{		
		try
		{
			ext->event_supplier = EventSupplier::create(orb,ds_name,db,hostname,this);
			ext->event_supplier->connect();
		}
		catch (...)
		{
			ext->event_supplier = NULL;
		}
	}
	else
		ext->event_supplier = NULL;
}
			
			
//+----------------------------------------------------------------------------
//
// method : 		Util::~Util()
// 
// description : 	Tango singleton object destructor.
//			This destructor shutdown everything before the process
//			dies. This means
//  				- Send kill command to the polling thread
//    				- Join with this polling thread
//				- Unregister server in database
//				- Delete devices (except the admin one)
//				- Shutdown the ORB
//				- Cleanup Logging
//
// 
//
//-----------------------------------------------------------------------------

Util::~Util()
{
#ifdef _TG_WINDOWS_
	if (ds_window != NULL)
	{
		stop_all_polling_threads();
		stop_heartbeat_thread();
		clr_heartbeat_th_ptr();
		
		unregister_server();
		get_dserver_device()->delete_devices();
		if (_FileDb == true)
			delete db;
		orb->shutdown(true);
		//JM : 9.8.2005 : destroy() should be called at the exit of run()!
		//orb->destroy();
#ifdef TANGO_HAS_LOG4TANGO
	  	Logging::cleanup();
#endif	
	}
#endif
}


//+----------------------------------------------------------------------------
//
// method : 		Util::server_already_running()
// 
// description : 	Check if the same device server is not already running
//			somewhere else and refuse to start in this case
//
//-----------------------------------------------------------------------------

void Util::server_already_running()
{

	cout4 << "Entering Util::server_already_running method" << endl;

//
// First, sleep a while in order to solve race condition for two
// servers started at the "same time" and a schedulling happens between
// the database check and the export device. This system is inherited
// from what has been implemented for the TACO control system
//
	
#ifdef _TG_WINDOWS_
	pid = _getpid();
#else
	pid = getpid();
#endif
	
	srand(pid);
#ifdef _TG_WINDOWS_
	DWORD backoff;
	backoff = (DWORD)(1000. * (float)rand() / (float)RAND_MAX);
//	Sleep(backoff);
#else
	struct timespec backoff;
	backoff.tv_sec = 0;
	backoff.tv_nsec = (long)(1000000000. * (float)rand() / (float)RAND_MAX);
//	nanosleep(&backoff,NULL);
#endif

//
// Build device name and try to import it from database or from cache if available
//

	string dev_name(DSDeviceDomain);
	dev_name.append(1,'/');
	dev_name.append(ds_name);
	
	Tango::Device_var dev;
	
	try
	{
		const Tango::DevVarLongStringArray *db_dev;
		CORBA::Any_var received;
		if (ext->db_cache != NULL)
		{
			db_dev = ext->db_cache->import_adm_dev();
		}
		else
		{
			CORBA::Any send;
			send <<= dev_name.c_str();
										
			received = db->get_dbase()->command_inout("DbImportDevice",send);		
			if ((received.inout() >>= db_dev) == false)
			{
				TangoSys_OMemStream o;
				o << "Database error while trying to import " << dev_name << ends;
					
				Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"Util::server_already_running");
			}
		}

//
// If the device is not imported, leave function
//

		if ((db_dev->lvalue)[0] == 0)
		{
			cout4 << "Leaving Util::server_already_running method" << endl;
			return;
		}
		
		CORBA::Object_var obj = orb->string_to_object((db_dev->svalue)[1]);
		
//
// Try to narrow the reference to a Tango::Device object
//
		
		dev = Tango::Device::_narrow(obj);
	}
	catch (Tango::DevFailed &)
	{
		TangoSys_OMemStream o;

		o << "The device server " << ds_name << " is not defined in database. Exiting!" << ends;
		print_err_message(o.str());
	}
	catch (CORBA::TRANSIENT &)
	{
		cout4 << "Leaving Util::server_already_running method" << endl;
		return;
	}
	catch (CORBA::OBJECT_NOT_EXIST &)
	{
		cout4 << "Leaving Util::server_already_running method" << endl;
		return;
	}
	catch (CORBA::NO_RESPONSE &)
	{
		print_err_message("This server is already running but is blocked!");
	}	
	catch (CORBA::COMM_FAILURE &)
	{
		cout4 << "Leaving Util::server_already_running method" << endl;
		return;
	}
			
	if (CORBA::is_nil(dev))
	{
		cout4 << "Leaving Util::server_already_running method" << endl;
		return;
	}
	
//
// Now, get the device name from the server
//

	try
	{
		CORBA::String_var n = dev->name();
		unsigned long ctr;
		char *tmp_ptr = n.inout();
		for (ctr = 0;ctr < strlen(tmp_ptr);ctr++)
			tmp_ptr[ctr] = tolower(tmp_ptr[ctr]);
		for (ctr = 0;ctr < dev_name.length();ctr++)
			dev_name[ctr] = tolower(dev_name[ctr]);
		if (n.in() == dev_name)
		{
			print_err_message("This server is already running, exiting!");
		}
	}
	catch (Tango::DevFailed &)
	{
	                
//
// It is necessary to catch this exception because it is thrown by the 
// print_err_message method under windows
//

		throw;
	}
	catch (CORBA::NO_RESPONSE &)
	{
		try
		{
			print_err_message("This server is already running but is blocked!");
		}
		catch (Tango::DevFailed &)
		{
			throw;
		}
	}
	catch (CORBA::SystemException &) {}
	catch (CORBA::Exception &) {}
	
	cout4 << "Leaving Util::server_already_running method" << endl;

}

//+----------------------------------------------------------------------------
//
// method : 		Util::server_init()
// 
// description : 	To initialise all classes in the device server process
//
//-----------------------------------------------------------------------------

void Util::server_init(bool with_window)
{
//
// Even if we are not in a Python DS, we have to create the per-thread
// PyData object. For Python DS, this is done in the Python_init method
// defined in the binding
//

#ifdef _TG_WINDOWS_
	if (Util::_service == true)
	{
		omni_thread::create_dummy();
	}
#endif

	omni_thread *th = omni_thread::self();
	if (is_py_ds() == false)
	{
		th->set_value(key_py_data,new Tango::PyData());
	}
	
#ifdef _TG_WINDOWS_
	if (with_window == true)
	{

//
// Create device server windows
//

		ds_window = new W32Win(this,nCmd);

//
// Change cout that it uses the graphical console window
//


	#ifndef TANGO_HAS_LOG4TANGO
    		cout.rdbuf(ds_window->get_output_buffer());
    		cout_tmp.rdbuf(ds_window->get_output_buffer());
	#endif 
	}
	#ifdef TANGO_HAS_LOG4TANGO //MODIF-NL
  	else
	{
    		ds_window = 0;
  	}
	#endif

	if (_win == true)
	{
		go = false;
		
		loop_th = new ORBWin32Loop(this);
		loop_th->start();
	}
	else
	{
#endif /* WIN 32 */
	
//
// Initialise main class
//

		DServerClass::init();

//
// Configure polling from the polling properties
// In case of python DS, we need to release the Python GIL
// because the polling_configure method will send cmd to the
// pollinh thread which will try to get the Python GIL
//

		int th_id = th->id();
		PyLock *lock_ptr;
		bool py_ds_main_th = false;
		if ((th_id == 0) && (is_py_ds() == true))
		{
			py_ds_main_th = true;
			omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
			lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
			lock_ptr->Release();
		}
			
		polling_configure();
		
		if (py_ds_main_th == true)
		{
			lock_ptr->Get();
		}

//
// Delete the db cache if it has been used
//
	
		if (ext->db_cache != NULL)
		{
		// extract sub device information before deleting cache!
			get_sub_dev_diag().get_sub_devices_from_cache();
		
	
			delete ext->db_cache;
			ext->db_cache = NULL;
		}
#ifdef _TG_WINDOWS_
	}
#endif /* _TG_WINDOWS_ */
}

//+----------------------------------------------------------------------------
//
// method :		Util::server_run()
//
// description : 	To start the CORBA event loop
//
//-----------------------------------------------------------------------------

void Util::server_run()
{

	omni_thread *th = omni_thread::self();
	int th_id = th->id();
		
//
// For Windows in a non-MSDOS window, start the ORB in its own thread. The main
// thread is used for windows management.
//
	
#ifdef _TG_WINDOWS_
	if (_win == true)
	{
	
		omni_mutex_lock syc(mon);
				
//
// Start the ORB thread (and loop)
//
			
		go = true;
		mon.signal();
	}
	else
	{
		if (_service == true)
		{
			NTService *serv = NTService::instance();
			serv->statusUpdate(SERVICE_RUNNING);
			if (serv->stopped_ == false)
			{
				//JM : 9.8.2005 : destroy() should be called at the exit of run()!
				try
				{
					orb->run();
					server_cleanup();
				}
				catch (CORBA::Exception &)
				{	
					server_cleanup();
					throw;
				}	
			}
		}			
		else
		{
			cout << "Ready to accept request" << endl;

			if (th_id == 0)
			{
				omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
				PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
				lock_ptr->Release();
			}

			//JM : 9.8.2005 : destroy() should be called at the exit of run()!
			try
			{
				orb->run();
				server_cleanup();

				if (th_id == 0)
				{
					omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
					PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
					lock_ptr->Get();
				}
			}
			catch (CORBA::Exception &)
			{	
				server_cleanup();
				throw;
			}			
		}
	}
#else
	cout << "Ready to accept request" << endl;
	
	if (th_id == 0)
	{
		omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
		PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
		lock_ptr->Release();
	}
							
	//JM : 9.8.2005 : destroy() should be called at the exit of run()!
	try
	{
		if (ext->ev_loop_func != NULL)
		{
			
//
// If the user has installed its own event management function, call it in a loop
//
			
			struct timespec sleep_time;
			sleep_time.tv_sec = 0;
			sleep_time.tv_nsec = 20000000;
			bool user_shutdown_server;
			
			while(ext->shutdown_server == false)
			{
				if (is_svr_shutting_down() == false)
				{
					if (orb->work_pending())
						orb->perform_work();
	
					user_shutdown_server = (*ext->ev_loop_func)();
					if (user_shutdown_server == true)
					{
						shutdown_server();
						ext->shutdown_server = true;
					}
				}
				else
				{
					nanosleep(&sleep_time,NULL);
				}
			}
		}
		else
		{
			orb->run();
		}
		server_cleanup();

		if (th_id == 0)
		{
			omni_thread::value_t *tmp_py_data = th->get_value(key_py_data);
			PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
			lock_ptr->Get();
		}
	}
	catch (CORBA::Exception &e)
	{
		server_cleanup();
		throw;
	}
#endif
}

//+----------------------------------------------------------------------------
//
// method :		Util::server_cleanup()
//
// description : 	To relinquish computer resource before process exit
//
//-----------------------------------------------------------------------------

void Util::server_cleanup()
{
#ifndef _TG_WINDOWS_
//
// Destroy the ORB
//
	if (_constructed == true)
	{
		orb->destroy();
		// JM : 8.9.2005 : mark as already destroyed
		_constructed = false;
	}
#else
	if (ds_window == NULL)
	{
		if (_constructed == true)
		{
			orb->destroy();
			// JM : 8.9.2005 : mark as already destroyed
			_constructed = false;
		}
	}
#endif	
}

//+----------------------------------------------------------------------------
//
// method : 		Util::get_device_list_by_class()
// 
// description : 	To return a reference to the vector of device for a
//			specific class
//
// in : 		- class_name : The class name
//
//-----------------------------------------------------------------------------

vector<DeviceImpl *> &Util::get_device_list_by_class(const string &class_name)
{

//
// Retrieve class list. Don't use the get_dserver_device() method followed by
// the get_class_list(). In case of several classes embedded within
// the same server and the use of this method in the object creation, it
// will fail because the end of the dserver object creation is after the
// end of the last server device creation.
//

	const vector<DeviceClass *> &tmp_cl_list = *cl_list_ptr;

//
// Check if the wanted class really exists
//

	int nb_class = tmp_cl_list.size();
	int i;	
	for (i = 0;i < nb_class;i++)
	{
		if (tmp_cl_list[i]->get_name() == class_name)
			break;
	}

//
// Also check if it it the DServer class
//

	if (class_name == "DServer")
	{
		return DServerClass::instance()->get_device_list();
	}
	
//
// Throw exception if the class is not found
//
	
	if (i == nb_class)
	{
		TangoSys_OMemStream o;
		o << "Class " << class_name << " not found" << ends;
		Except::throw_exception((const char *)"API_ClassNotFound",
				        o.str(),
				        (const char *)"Util::get_device_list_by_class()");
	}
	
	return tmp_cl_list[i]->get_device_list();
}

vector<DeviceImpl *> &Util::get_device_list_by_class(const char *class_name)
{
	string class_str(class_name);
	
	return get_device_list_by_class(class_str);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::get_device_by_name()
// 
// description : 	To return a reference to the device object from its 
//			name
//
// in : 		- dev_name : The device name
//
//-----------------------------------------------------------------------------

DeviceImpl *Util::get_device_by_name(const string &dev_name)
{

	string dev_name_lower(dev_name);
	transform(dev_name_lower.begin(),dev_name_lower.end(),dev_name_lower.begin(),::tolower);

	DeviceImpl *ret_ptr = find_device_name_core(dev_name_lower);

//
// If the device is not found, may be the name we have received is an alias ?
//

	if (ret_ptr == NULL)
	{
		string d_name;
		try
		{
			db->get_device_alias(dev_name_lower,d_name);
		}
		catch (Tango::DevFailed &) {}
		
		if (d_name.size() != 0)
		{
			transform(d_name.begin(),d_name.end(),d_name.begin(),::tolower);
			
			ret_ptr = find_device_name_core(d_name);
			
//
// If the name given to this method is a valid alias name,
// store the alias name in device object for possible
// future call to this method (save some db calls)
//

			if (ret_ptr != NULL)
			{
				ret_ptr->set_alias_name_lower(dev_name_lower);
			}
			
		}
	}
		
//
// Throw exception if the device is not found
//

	if (ret_ptr == NULL)
	{
		TangoSys_OMemStream o;
		o << "Device " << dev_name << " not found" << ends;
		Except::throw_exception((const char *)"API_DeviceNotFound",
				        o.str(),
				        (const char *)"Util::get_device_by_name()");
	}
	
	return ret_ptr;
}


DeviceImpl *Util::find_device_name_core(string &dev_name)
{
//
// Retrieve class list. Don't use the get_dserver_device() method followed by
// the get_class_list(). In case of several classes embedded within
// the same server and the use of this method in the object creation, it
// will fail because the end of the dserver object creation is after the
// end of the last server device creation.
//

	const vector<DeviceClass *> &tmp_cl_list = *cl_list_ptr;
	DeviceImpl *ret_ptr = NULL;
	
//
// Check if the wanted device exists in each class
//

	int nb_class = tmp_cl_list.size();
	int i,j,nb_dev;
	bool found = false;
	
	for (i = 0;i < nb_class;i++)
	{
		vector<DeviceImpl *> &dev_list = get_device_list_by_class(tmp_cl_list[i]->get_name());
		nb_dev = dev_list.size();
		for (j = 0;j < nb_dev;j++)
		{
			string name(dev_list[j]->get_name());
			transform(name.begin(),name.end(),name.begin(),::tolower);
			if (name == dev_name)
			{
				found = true;
				ret_ptr = dev_list[j];
				break;
			}
			string &alias_name = dev_list[j]->get_alias_name_lower();
			if (alias_name.size() != 0)
			{
				if (alias_name == dev_name)
				{
					found = true;
					ret_ptr = dev_list[j];
					break;
				}
			}
		}
		if (found == true)
			break;
	}
	
//
// Check also the dserver device
//

	if (found == false)
	{
		DServerClass *ds_class = DServerClass::instance();
		vector<DeviceImpl *> &devlist = ds_class->get_device_list();
		string name(devlist[0]->get_name());
		transform(name.begin(),name.end(),name.begin(),::tolower);
		if (name == dev_name)
		{
			found = true;
			ret_ptr = devlist[0];
			j--;
		}
	}
		
//
// Return to caller. The returned value is NULL if the device is not found
//
	
	return ret_ptr;
}

DeviceImpl *Util::get_device_by_name(const char *dev_name)
{
	string name_str(dev_name);
	
	return get_device_by_name(name_str);
}

//+----------------------------------------------------------------------------
//
// method : 		Util::get_dserver_device()
// 
// description : 	To return a pointer to the dserver device automatically
//			attached to each device server process
//
//-----------------------------------------------------------------------------

DServer *Util::get_dserver_device()
{
	return (DServer *)((DServerClass::instance()->get_device_list())[0]);
}


//+----------------------------------------------------------------------------
//
// method : Util::get_device_list
//
// description :  helper method to get device list from a wild card
//		  If no device is found, does not throw exception, just return 
//		  an empty vector
//
// in : The wildcard (e.g. "*", "/tango/tangotest/*", ...)
//
// out : The list of devices which name matches the wildcard
//
//-----------------------------------------------------------------------------

std::vector<DeviceImpl *> Util::get_device_list (const std::string& pattern)
{
	cout4 << "In Util::get_device_list" << endl;
	
// the returned list
	std::vector<DeviceImpl*> dl(0);

//	
// ------------------------------------------------------------------
// CASE I: pattern does not contain any '*' char - it's a device name
//

	if (pattern.find('*') == std::string::npos)
	{
		DeviceImpl* dev = 0;
		try
		{
			dev = get_device_by_name(pattern);
		}
		catch (Tango::DevFailed&) {}

//
// add dev to the list
//

		if (dev)
			dl.push_back(dev);
			
		return dl;
	} 
	
//
// for the two remaining cases, we need the list of all DeviceClasses.
//

	const std::vector<DeviceClass*> dcl(*(get_class_list()));

// a vector to store a given class' devices
	std::vector<DeviceImpl*> temp_dl;
	
//
// ------------------------------------------------------------------
// CASE II: pattern == "*" - return a list containing all devices
//

	if (pattern == "*")
	{
		for (unsigned int i = 0; i < dcl.size(); i++)
		{
			temp_dl = dcl[i]->get_device_list();
			dl.insert(dl.end(), temp_dl.begin(), temp_dl.end());
		}
     		return dl;
	}
	
//
// ------------------------------------------------------------------
// CASE III: pattern contains at least one '*' char
//

	std::string::size_type pos;
	std::string::size_type last_pos = 0;
	std::string token;
	std::vector<std::string> tokens(0);

//
// build the token list
//

	int done = 0;
	do
	{
		pos = pattern.find('*', last_pos);
		if (pos != 0)
		{
			if (pos == std::string::npos)
			{
				if (last_pos >= pattern.size())
					break;
				pos = pattern.size();
				done = 1;
			}
			token.assign(pattern.begin() + last_pos, pattern.begin() + pos);
			cout4 << "Found pattern " << token << endl;
			tokens.push_back(token);
		}
		last_pos = pos + 1;
	}
	while (!done);
// look for token(s) in device names
	unsigned int i, j, k;
	std::string dev_name;
// for each DeviceClass...
	for (i = 0; i < dcl.size(); i++)
	{
// ...get device list
		temp_dl = dcl[i]->get_device_list();
// for each device in in list...
		for (j = 0; j < temp_dl.size(); j++)
		{
// get device name
			dev_name = temp_dl[j]->get_name();
// make sure each char is lower case
			std::transform(dev_name.begin(), dev_name.end(), dev_name.begin(), ::tolower);
// then look for token(s) in device name
// to be added to the list, device_name must contains
// every token in the right order.
			for (k = 0, pos = 0; k < tokens.size(); k++)
			{
				pos = dev_name.find(tokens[k], pos);
				if (pos == std::string::npos)
					break;
			}
// if dev_name matches the pattern, add the device to the list
			if (k == tokens.size())
			{
				cout4 << "Device "  << temp_dl[j]->get_name() << " match pattern" << endl;
				dl.push_back(temp_dl[j]);
			}
		}
	}
	
	cout4 << "Returning a device list containing " << dl.size() << " items" << endl;
	return dl;
}

	

//+----------------------------------------------------------------------------
//
// method : 		Util::unregister_server()
// 
// description : 	Unregister the server from the database
//
//-----------------------------------------------------------------------------

void Util::unregister_server()
{

	cout4 << "Entering Util::unregister_server method" << endl;

//
// Mark all the devices belonging to this server as unexported
//

	if ((_UseDb == true) && (_FileDb == false))
	{	
		try
		{
			db->unexport_server(ds_name);
		}
		catch (Tango::DevFailed &e)
		{
			Except::print_exception(e);
			throw;
		}
		catch (CORBA::Exception  &e)
		{
			Except::print_exception(e);
			throw;
		}
	}	
	cout4 << "Leaving Util::unregister_server method" << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		Util::print_err_message()
// 
// description : 	Print error message in the classical console or with
//                      a message box
//                      For Unix like OS, this method exits. if it is called
//                      under NT in a graphical environment, it throws
//                      exception
//
// in :			- err_mess : The error message
//
//-----------------------------------------------------------------------------

void Util::print_err_message(const char *err_mess,Tango::MessBoxType type)
{
#ifdef _TG_WINDOWS_
        if (_win == true)
        {
                switch (type)
                {
                case Tango::STOP:
                        MessageBox((HWND)NULL,err_mess,MessBoxTitle,MB_ICONSTOP);
                        break;

                case Tango::INFO:
                        MessageBox((HWND)NULL,err_mess,MessBoxTitle,MB_ICONINFORMATION);
                        break;
                }
                Except::throw_exception((const char *)"API_StartupSequence",
				        (const char *)"Error in device server startup sequence",
                                        (const char *)"Util::print_err_mess");
        }
        else
        {
                cerr << err_mess << endl;
		exit(-1);
        }
#else
        cerr << err_mess << endl;
#ifdef __linux
	_exit(-1);
#else
        exit(-1);
#endif
#endif	
}


//+----------------------------------------------------------------------------
//
// method : 		Util::get_tango_lib_vers()
// 
// description : 	Return a number set to the Tango release number
//			coded with 3 digits (550, 551,552,600)		
//
//-----------------------------------------------------------------------------

long Util::get_tango_lib_release()
{
	return _convert_tango_lib_release(TgLibVers);
}


//+----------------------------------------------------------------------------
//
// method : 		Util::clean_dyn_attr_prop()
// 
// description : 	Clean in database the dynamic attribute property(ies)
//
//-----------------------------------------------------------------------------

void Util::clean_dyn_attr_prop()
{
	if (Tango::Util::_UseDb == true)
	{
		DbData send_data;
	
		for (unsigned long loop = 0;loop < ext->all_dyn_attr.size();loop++)
		{
			DbDatum db_dat(ext->all_dyn_attr[loop]);
			send_data.push_back(db_dat);
		}
		
		db->delete_all_device_attribute_property(ext->dyn_att_dev_name,send_data);
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		Util::set_db_svr_version()
// 
// description : 	Cheack some BD command to guess db server version
//
//-----------------------------------------------------------------------------

void Util::set_db_svr_version()
{

//
// Is the DbDeleteAllDeviceAttributeProperty command available ?
//

	try
	{
		string dummy_dev_name("a/b/c");
		DbData db_dat;
		DbDatum datum("Dummy_att");
		db_dat.push_back(datum);
		
		db->delete_all_device_attribute_property(dummy_dev_name,db_dat);
		ext->db_svr_version = 400;
	}
	catch(Tango::DevFailed &e)
	{
		if (::strcmp(e.errors[0].reason.in(),"API_CommandNotFound") == 0)
			ext->db_svr_version = 399;
		else
			ext->db_svr_version = 300;
	}
}


#ifdef _TG_WINDOWS_
//+----------------------------------------------------------------------------
//
// method : 		Util::build_argc_argv()
// 
// description : 	Build argc, argv UNIX like parameters from the Windows
//                      command line
//
// in :			
//
//-----------------------------------------------------------------------------

void Util::build_argc_argv()
{

//
// Get command line
//

	char *cmd_line = GetCommandLine();

	int cnt=0;
	char *tmp;

//
// First, count how many args we have. If the user type two spaces between args,
// we will have too many pointers allocates but it is not a problem
//

    int cmd_line_size = strlen(cmd_line);
    for (int i = 0;i < cmd_line_size;i++)
    {
        if (cmd_line[i] == ' ')
                cnt++;
    }

//
// Allocate memory for argv
//

	argv = new char *[cnt + 1];

//
// If only one args, no parsing is necessary
//

    if (cnt == 0)
    {
            argv[0] = new char [cmd_line_size + 1];
            strcpy(argv[0],cmd_line);
            argc = 1;
    }
    else
    {
                
//
// Get program name
//

        tmp = strtok(cmd_line," ");
        argv[0] = new char [strlen(tmp) + 1];
        strcpy(argv[0],tmp);

//
// Get remaining args
//

        int i = 1;	        
        while ((tmp = strtok(NULL," ")) != NULL)
        {
            argv[i] = new char [strlen(tmp) + 1];
            strcpy(argv[i],tmp);
            i++;
        }
        argc = i;
    }
}

HWND Util::get_console_window()
{
	return ds_window->get_output_buffer()->get_debug_window();
}

HWND Util::get_ds_main_window()
{
	return ds_window->get_win();
}

CoutBuf *Util::get_debug_object()
{
	return ds_window ? ds_window->get_output_buffer() : 0;
}

BOOL CtrlHandler(DWORD fdwCtrlType)  
{ 
	switch( fdwCtrlType )  
	{  
// Ignore logoff event! 
	case CTRL_LOGOFF_EVENT: 
		return TRUE;  
 
// Pass all other signals to the next signal handler 
	default:  
		return FALSE;  
	}  
}  

void Util::install_cons_handler()
{
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler,TRUE))
		print_err_message("WARNING: Can't install the console handler");
}


//+----------------------------------------------------------------------------
//
// method : 		Util::ORBWin32Loop::run()
// 
// description : 	Start the ORB loop. This method is in a inner class
//			because it is started using the a separate thread.
//			One thread is for the Windows event loop and the
//			second thread is for the ORB loop.
//
// in :			
//
//-----------------------------------------------------------------------------

void *Util::ORBWin32Loop::run_undetached(void *ptr)
{
//
// Create the per thread data for the main thread
//

	omni_thread::self()->set_value(key_py_data,new Tango::PyData());
	
//
// Create the DServer object
//

	try
	{
		DServerClass::init();
	}
	catch (bad_alloc)
	{
		MessageBox((HWND)NULL,"Memory error","Device creation failed",MB_ICONSTOP);
		::exit(-1);
	}
	catch (Tango::DevFailed &e)
	{
		string str(e.errors[0].desc.in());
		str = str + '\n';
		str = str + e.errors[0].origin.in();
		MessageBox((HWND)NULL,str.c_str(),"Device creation failed",MB_ICONSTOP);
		::exit(-1);
	}
	catch (CORBA::Exception &)
	{
		MessageBox((HWND)NULL,"CORBA exception","Device creation failed",MB_ICONSTOP);
		::exit(-1);
	}

//
// Configure polling from polling properties
//


	util->polling_configure();
	
//
// Delete DB cache (if there is one)
//

	if (util->ext->db_cache != NULL)
	{
		// extract sub device information before deleting cache!
		util->get_sub_dev_diag().get_sub_devices_from_cache();
		
		delete util->ext->db_cache;
		util->ext->db_cache = NULL;
	}

//
// Start the ORB
//
			
	wait_for_go();
	
	util->get_orb()->run();
	
	return NULL;
}

void Util::ORBWin32Loop::wait_for_go()
{
	omni_mutex_lock sync(util->mon);
	
	while(util->go == false)
	{
		util->mon.wait();
	}
}
#endif /* _TG_WINDOWS_ */


int TangoMonitor::wait(long nb_millis)
{
	unsigned long s,n;

	unsigned long nb_sec,nb_nanos;
	nb_sec = nb_millis / 1000 ;
	nb_nanos = (nb_millis - (nb_sec * 1000)) * 1000000;

	omni_thread::get_time(&s,&n,nb_sec,nb_nanos);
	return cond.timedwait(s,n);
}

void clear_att_dim(Tango::AttributeValue_3 &att_val)
{
	att_val.r_dim.dim_x = 0;
	att_val.r_dim.dim_y = 0;
	att_val.w_dim.dim_x = 0;
	att_val.w_dim.dim_y = 0;
}

void clear_att_dim(Tango::AttributeValue_4 &att_val)
{
	att_val.r_dim.dim_x = 0;
	att_val.r_dim.dim_y = 0;
	att_val.w_dim.dim_x = 0;
	att_val.w_dim.dim_y = 0;
	
	att_val.data_format = Tango::FMT_UNKNOWN;
}

//
// The function called by the interceptor on thread creation
//

void create_PyPerThData(omni::omniInterceptors::createThread_T::info_T &info)
{
	PyData *py_dat_ptr = new PyData();
#ifdef _TG_WINDOWS_
	omni_thread::ensure_self es;
#endif

	omni_thread::self()->set_value(key_py_data,py_dat_ptr);

	Util *tg = NULL;
	Interceptors *Inter = NULL;
	
	try
	{
		tg = Util::instance(false);
		Inter = tg->get_interceptors();
	}
	catch(Tango::DevFailed &) {}
	
	if (Inter != NULL)
		Inter->create_thread();

	info.run();
	
	omni_thread::self()->remove_value(key_py_data);
	delete py_dat_ptr;

	if (Inter != NULL)
		Inter->delete_thread();
	
	return;
}

AutoPyLock::AutoPyLock()
{
	omni_thread::value_t *tmp_py_data = omni_thread::self()->get_value(key_py_data);
	PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
	lock_ptr->Get();
}
	
AutoPyLock::~AutoPyLock()
{
	omni_thread::value_t *tmp_py_data = omni_thread::self()->get_value(key_py_data);
	PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
	lock_ptr->Release();
}
	
//
// A small function to convert Tango lib release string to a number
// 
// It is defined as a function to be used within the Utils and ApiUtil
// classes (both client and server part)
//

long _convert_tango_lib_release(const char *tg_vers_str)
{
	long ret;
	
	TangoSys_MemStream str;
	string::size_type pos;
	string vers_str(tg_vers_str);
	
	pos = vers_str.find('.');
	if (pos == string::npos)
		return -1;
	vers_str.erase(pos,1);
	
	pos = vers_str.find('.');
	if (pos == string::npos)
		return -1;
	vers_str.erase(pos,1);
	
	str << vers_str << ends;
	if (!(str >> ret))
		ret = -1;
		
	return ret;
}


} // End of Tango namespace

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
// $Revision$
//
// $Log$
// Revision 3.12  2005/06/29 08:31:19  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.11  2005/05/25 06:05:40  taurel
// - Again some minors changes before 5.2
//
// Revision 3.10  2005/05/10 16:10:18  taurel
// - Fix windows VC7 warnings
//
// Revision 3.9  2005/04/15 11:34:07  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.8  2005/03/14 09:49:29  taurel
// - Fix some bugs in filedatabase (Change in the data transferred between client and
//   server).
// - Fix bug in event re-connection
// - Add event support even for device server started with the -file command line option
//
// Revision 3.7  2005/01/13 08:30:36  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.6.2.11  2004/12/06 14:39:30  taurel
// - Polling starts in a separate thread
// - For Windows : Polling thread cmd/attr measurement used PerformanceCounter
// - Fix bug in pollext.h in methods to externally fill polling buffer
//
// Revision 3.6.2.10  2004/11/26 13:53:21  taurel
// - Fix bug if exception thrown during Util class construction
// - Change attribute label and format default values
//
// Revision 3.6.2.9  2004/11/10 13:00:55  taurel
// - Some minor changes for the WIN32 port
//
// Revision 3.6.2.8  2004/11/09 09:57:49  taurel
// - Fix one memory leak and some minor changes
// - Force database file writing when server exit
// - Some minor changes for the -file option for WIN32
//
// Revision 3.6.2.7  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.6.2.6  2004/10/22 11:26:33  taurel
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
// Revision 3.6.2.5  2004/10/14 07:54:35  maxchiandone
// avoid starting with -nodb and -file together
//
// Revision 3.6.2.4  2004/10/05 13:55:03  maxchiandone
// First upload for filedatabase.
//
// Revision 3.6.2.3  2004/09/15 06:47:17  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.6.2.2  2004/07/15 15:04:06  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.6.2.1  2004/07/08 11:02:13  taurel
// - Add possibility to have command/attribute polling buffer depth specified
//   individually
//
// Revision 3.6  2004/07/07 08:40:13  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.5  2003/08/21 07:24:37  taurel
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
// Revision 3.4  2003/07/03 07:40:51  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.3.2.11  2004/05/17 15:33:35  taurel
// - Add code in polling_configure() method to make the main thread known to
//   omni threads for Windows DS used as services with polled command/attributes
//
// Revision 3.3.2.10  2004/03/09 16:36:37  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.3.2.9  2004/02/27 16:47:01  taurel
// - Updated DeviceImpl and Util classes sizes for compatibility
//
// Revision 3.3.2.8  2004/02/25 16:27:44  taurel
// Minor changes to compile library using Solaris CC compiler
//
// Revision 3.3.2.7  2004/02/18 15:06:18  taurel
// Now the DevRestart command immediately restart device event (if any). Previously, it was possible to wait up to 200 secondes before they
// restart
//
// Revision 3.3.2.6  2004/02/06 11:58:52  taurel
// - Many changes in the event system
//
// Revision 3.3.2.5  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.3.2.4  2003/11/28 17:08:44  taurel
// Changes to handle tango lib build as a Windows DLL and used in a device
// server
//
// Revision 3.3.2.2  2003/10/03 13:34:26  taurel
// - Fix bug for device server started without database, with device name specified on command line using mix of upper and lower cases
// - It's now possible to send a command to the polling thread from itself
// - Add a release lock if the insert into the polling buffer failed
//
// Revision 3.3.2.1  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.3  2003/05/28 14:55:10  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.2  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.1  2003/04/02 12:26:04  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
// Revision 3.0.2.2  2003/07/17 03:46:46  andy_gotz
// Added new events "on_valid" and "on_state_change"; factorized EventSupplier::detect_and_push_events() method;
// Catching more exceptions when creating EventSupplier, device server will start without Notification service;
// Added AttributeProxy and DbAttribute classes to api; created test programs in test directory
//
// Revision 3.0.2.1  2003/04/08 13:12:53  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:47:51  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
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

#ifndef WIN32
	#include <unistd.h>
	#include <assert.h>
	#include <sys/time.h>
	#include <netdb.h>
#else
	#include <sys/timeb.h>
	#include <process.h>
        #include <coutbuf.h>
	#include <ntservice.h>
#endif /* WIN32 */

#include <omniORB4/omniInterceptors.h>

namespace Tango
{

Util *Util::_instance = NULL;
long Util::_tracelevel = 0;
bool Util::_UseDb = true;
bool Util::_FileDb = false;
bool Util::_daemon = false;
long Util::_sleep_between_connect = 60;
bool Util::_constructed = false;
#ifdef WIN32
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

#ifdef WIN32
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

#if ((defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
Util::Util(int argc,char *argv[]):cl_list_ptr(NULL)
# ifndef TANGO_HAS_LOG4TANGO
    ,cout_tmp(cout.rdbuf())
# endif
#elif (defined WIN32)
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

		char h_name[80];
		int res = gethostname(h_name,80);
		if (res == 0)
		{
			char addr[180];
			strcpy(addr,"giop:tcp:");
			strcat(addr,h_name);
			strcat(addr,":");
			const char *options[][2] = {
			{"endPoint",addr},
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT_STR},
			{"serverCallTimeOutPeriod","5000"},
			{"maxServerThreadPoolSize","100"},
			{"threadPerConnectionUpperLimit","55"},
			{"threadPerConnectionLowerLimit","50"},
			{"supportCurrent","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};
			
			orb = CORBA::ORB_init(argc,argv,"omniORB4",options);
		}
		else
		{
			const char *options[][2] = {
			{"endPointPublishAllIFs","1"},
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT_STR},
			{"serverCallTimeOutPeriod","5000"},
			{"maxServerThreadPoolSize","100"},
			{"threadPerConnectionUpperLimit","55"},
			{"threadPerConnectionLowerLimit","50"},
			{"supportCurrent","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};
			
			orb = CORBA::ORB_init(argc,argv,"omniORB4",options);
		}

//
// Install an omniORB interceptors to store client name in blackbox
// and allocate a key for per thread specific storage
//

		omniORB::getInterceptors()->serverReceiveRequest.add(get_client_addr);
		key = omni_thread::allocate_key();
				
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


#ifndef TANGO_HAS_LOG4TANGO

//
// Initialize trace output (For Windows, the stdc++ lib also implements the
// new iostreams where the xx_with_assign classes are not defined. Therefore,
// to copy streams, I have used the advices in the C++ report of June 1997 
//

  		trace_output = InitialOutput;
		file_stream = NULL;

# if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
		cout_tmp.copyfmt(cout);
		cout_tmp.clear(cout.rdstate());
# else
		cout_tmp = cout;
# endif 
#endif // TANGO_HAS_LOG4TANGO

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
		
#ifdef TANGO_HAS_LOG4TANGO	
//
// Initialize logging stuffs
//
		Logging::init(ds_name, (int)_tracelevel,  ((!_FileDb) && _UseDb), *db);
#endif

		cout4 << "Connected to database" << endl;

//
// Check if the server is not already running somewhere else
//

		if ((_UseDb == true) && (_FileDb == false))
			server_already_running();

//
// Get process PID, host name and Tango version
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
// Create the polling thread and start it
//

		ext->poll_th = new PollThread(ext->shared_data,ext->poll_mon);
		ext->poll_th->start();
		ext->poll_th_id = ext->poll_th->id();
		cout4 << "Pollthread Id = " << ext->poll_th_id;
								

								
		cout4 << "Tango object singleton constructed" << endl;
		
	}
	catch (CORBA::Exception &)
	{
		throw;
	}
}


#ifdef WIN32
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
#ifdef WIN32
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

#ifdef WIN32
	string::size_type pos;
	if ((pos = ds_exec_name.find('.')) != string::npos)
	{
		ds_exec_name.erase(pos,ds_exec_name.size());
	}
#endif /* WIN32 */

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
						print_usage(argv[0]);
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
						print_usage(argv[0]);
					else
					{
						if ( !_UseDb )
							print_usage(argv[0]);
						_FileDb = true;
						database_file_name = argv[ind]; 
						database_file_name.erase(0,6);
#ifdef WIN32
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
						print_usage(argv[0]);
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
// Build server name in lower-case letters
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
	bool retry = true;
		
	while (retry == true)
	{
		try
		{
			DbDatum received = db->get_device_member(str);
			received >> db_inst;
			retry = false;
		}
		catch (Tango::CommunicationFailed &) {}
		catch (Tango::DevFailed &)
		{
			o << ends;
			print_err_message(o.str(),Tango::INFO);
			retry = false;
		}
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
#ifdef WIN32
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
#ifdef WIN32
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
// Also copy this database object ptr into the ApiUtil object. Therefore,
// the same database connection will be used also for DeviceProxy
// object created within the server
//

	ApiUtil::instance()->get_db_vect().push_back(db);
	
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
//				The host name
//				The Tango version
//
//-----------------------------------------------------------------------------

void Util::misc_init()
{

//
// Get PID
//

	TangoSys_OMemStream o;
	
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
	#ifdef WIN32
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
		
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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

#ifdef WIN32
	main_win_text = "European Synchrotron Radiation Facility (ESRF)\n";
	main_win_text = main_win_text + "CORBA based device server\n";
	main_win_text = main_win_text + "Developped by Tango team";
#endif
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
			ext->event_supplier = EventSupplier::create(orb,ds_name,db,hostname);
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
#ifdef WIN32
	if (ds_window != NULL)
	{
		TangoMonitor &mon = get_poll_monitor();
		PollThCmd &shared_cmd = get_poll_shared_cmd();

		{	
			omni_mutex_lock sync(mon);

			shared_cmd.cmd_pending = true;
			shared_cmd.cmd_code = POLL_EXIT;

			mon.signal();

		}
		
		get_polling_thread_object()->join(0);
		clr_poll_th_ptr();
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
	
#ifdef WIN32
	pid = _getpid();
#else
	pid = getpid();
#endif
	
	srand(pid);
#ifdef WIN32
	DWORD backoff;
	backoff = (DWORD)(1000. * (float)rand() / (float)RAND_MAX);
	Sleep(backoff);
#else
	struct timespec backoff;
	backoff.tv_sec = 0;
	backoff.tv_nsec = (long)(1000000000. * (float)rand() / (float)RAND_MAX);
	nanosleep(&backoff,NULL);
#endif

//
// Build device name and try to import it from database
//

	string dev_name(DSDeviceDomain);
	dev_name.append(1,'/');
	dev_name.append(ds_name);
	
	Tango::Device_var dev;
	
	try
	{
		CORBA::Any send;
		send <<= dev_name.c_str();
										
		CORBA::Any_var received = db->get_dbase()->command_inout("DbImportDevice",send);		
		const Tango::DevVarLongStringArray *db_dev;
		if ((received.inout() >>= db_dev) == false)
		{
			TangoSys_OMemStream o;
			o << "Database error while trying to import " << dev_name << ends;
					
			Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"Util::server_already_running");
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
// Set its timeout policy to 3 seconds
//

//	        CORBA::PolicyList policies;
//	        policies.length(1);
	
//	        CORBA::ULong t = 2000;
//	        CORBA::Any a;
//	        a <<= t;

//	        policies[0] = orb->create_policy(OB::TIMEOUT_POLICY_ID,a);
//        	CORBA::Object_var pol_obj = obj->_set_policy_overrides(policies,CORBA::ADD_OVERRIDE);
		
//
// Try to narrow the reference to a Tango::Device object
//
		
//		dev = Tango::Device::_narrow(pol_obj);
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
#ifdef WIN32
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
	
//
// Initialise main class
//

		DServerClass::init();

//
// Configure polling from the polling properties
//
		
		polling_configure();
	}
	
#else

//
// Initialise main class
//

	DServerClass::init();
	
//
// Configure polling from polling properties
//

	polling_configure();
#endif
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
	
//
// For Windows in a non-MSDOS window, start the ORB in its own thread. The main
// thread is used for windows management.
//
	
#ifdef WIN32
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
				catch (CORBA::Exception &e)
				{	
					server_cleanup();
					throw;
				}	
			}
		}			
		else
		{

			//JM : 9.8.2005 : destroy() should be called at the exit of run()!
			try
			{
				orb->run();
				server_cleanup();
			}
			catch (CORBA::Exception &e)
			{	
				server_cleanup();
				throw;
			}			
		}
	}
#else
	cout << "Ready to accept request" << endl;
	
	//JM : 9.8.2005 : destroy() should be called at the exit of run()!
	try
	{
		orb->run();
		server_cleanup();
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
#ifndef WIN32
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

//
// Retrieve class list. Don't use the get_dserver_device() method followed by
// the get_class_list(). In case of several classes embedded within
// the same server and the use of this method in the object creation, it
// will fail because the end of the dserver object creation is after the
// end of the last server device creation.
//

	const vector<DeviceClass *> &tmp_cl_list = *cl_list_ptr;
	DeviceImpl *ret_ptr = NULL;

	string dev_name_lower(dev_name);
	transform(dev_name_lower.begin(),dev_name_lower.end(),dev_name_lower.begin(),::tolower);
	
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
			if (name == dev_name_lower)
			{
				found = true;
				ret_ptr = dev_list[j];
				break;
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
		if (name == dev_name_lower)
		{
			found = true;
			ret_ptr = devlist[0];
			j--;
		}
	}

//
// Throw exception if the device is not found
//
	
	if ((i == nb_class) && (j == nb_dev))
	{
		TangoSys_OMemStream o;
		o << "Device " << dev_name << " not found" << ends;
		Except::throw_exception((const char *)"API_DeviceNotFound",
				        o.str(),
				        (const char *)"Util::get_device_by_name()");
	}
	
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
#ifdef WIN32
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
// method : 		Util::polling_configure()
// 
// description : 	This method sends command to the polling thread for
//			all cmd/attr with polling configuration stored in db.
//			This is done in separate thread in order to equally
//			spread all the polled objects polling time on the
//			smallest polling period.
//
//-----------------------------------------------------------------------------

void Util::polling_configure()
{
	PollConfigureThread *pollconf_th = new PollConfigureThread();
	pollconf_th->start();
}

void PollConfigureThread::run(void *ptr)
{

	Tango::Util *tg = Tango::Util::instance();
#ifdef WIN32
//
// In case, some commands or attributes are polled, this method will send
// command to the polling thread. These commands test the thread id of
// the calling thread. In a Windows service, to be able to do this, we
// have to make this thread known to omnithread
//
// In Tango V5, this code is now executed in a thread which is created
// and therefore, this create_dummy call, is not necessary any more
//

/*	if (Util::_service == true)
	{
		omni_thread::create_dummy();
	}*/
#endif
//
// Send a stop polling command to thread in order not to poll devices
//

	try
	{
		DServer *adm_dev = tg->get_dserver_device();
		adm_dev->stop_polling();

		vector<DeviceClass *> &tmp_cl_list = adm_dev->get_class_list();
		unsigned long i,j,k;
		long upd;
		TangoSys_MemStream s;

//
// Create the structure used to send data to the polling thread
// and store them in a vector
//

		vector<DevVarLongStringArray *> v_poll_cmd;

//
// A loop on each class and each device in class
//

		long smallest_upd;
		bool first_loop = true;

		for (i = 0;i < tmp_cl_list.size();i++)
		{
			vector<DeviceImpl *> &dev_list = tmp_cl_list[i]->get_device_list();
			for (j = 0;j < dev_list.size();j++)
			{
				vector<string> &poll_cmd_list = dev_list[j]->get_polled_cmd();
				vector<string> &poll_attr_list = dev_list[j]->get_polled_attr();

//
// Check that cmd_list and attr_list have a correct syntax
//

				if ((poll_cmd_list.size() % 2) == 1)
				{
					TangoSys_OMemStream o;
					o << "System property polled_cmd for device " << dev_list[j]->get_name() << " has wrong syntax" << ends;
					Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        			o.str(),
				        			(const char *)"PollConfigureThread::run_undetached()");
				}
			
				if ((poll_attr_list.size() % 2) == 1)
				{
					TangoSys_OMemStream o;
					o << "System property polled_attr for device " << dev_list[j]->get_name() << " has wrong syntax" << ends;
					Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        			o.str(),
				        			(const char *)"PollConfigureThread::run_undetached()");
				}


//
// A loop on each command
//

				for (k = 0;k < poll_cmd_list.size();k++)
				{

					DevVarLongStringArray *send = new DevVarLongStringArray();
					send->lvalue.length(1);
					send->svalue.length(3);

					send->svalue[0] = CORBA::string_dup(dev_list[j]->get_name().c_str());
					send->svalue[1] = CORBA::string_dup("command");
					send->svalue[2] = CORBA::string_dup(poll_cmd_list[k].c_str());
							
//
// Convert polling period to a number and store polling thread command params
//

					s << poll_cmd_list[k + 1];
					s >> upd;
					s.clear();
					send->lvalue[0] = upd;

					if (first_loop == true)
					{
						smallest_upd = upd;
						first_loop = false;
					}
					else
					{
						if (upd < smallest_upd)
							smallest_upd = upd;
					}

					v_poll_cmd.push_back(send);
					k++;
				}

//
// A loop on each attribute
//

				for (k = 0;k < poll_attr_list.size();k++)
				{

					DevVarLongStringArray *send = new DevVarLongStringArray();
					send->lvalue.length(1);
					send->svalue.length(3);

					send->svalue[0] = CORBA::string_dup(dev_list[j]->get_name().c_str());
					send->svalue[1] = CORBA::string_dup("attribute");
					send->svalue[2] = CORBA::string_dup(poll_attr_list[k].c_str());
							
//
// Convert polling period to a number ans store polling thread command params
//

					s << poll_attr_list[k + 1];
					s >> upd;
					s.clear();
					send->lvalue[0] = upd;

					if (first_loop == true)
					{
						smallest_upd = upd;
						first_loop = false;
					}
					else
					{
						if (upd < smallest_upd)
							smallest_upd = upd;
					}

					v_poll_cmd.push_back(send);
					k++;
				}
			}
		}


		unsigned long nb_cmd = v_poll_cmd.size();

//
// Send command to polling thread one by one and sleep between
// sending each command.
// In the following computation , I try to take into account the
// non real time aspect of our OS. I remove 15 mS from each
// sleeping time due to thread wake-up time
//

		if (nb_cmd != 0)
		{
			long sleeping_time = smallest_upd / nb_cmd;
	
			long delta_os = 15;
			if (delta_os < sleeping_time)
				sleeping_time = sleeping_time - delta_os;
		
#ifndef WIN32
			long nb_sec = 0;
			long nb_nsec;
			struct timespec to_wait,inter;

			if (sleeping_time >= 1000)
			{
				nb_sec = sleeping_time / 1000;
				nb_nsec = (sleeping_time - (nb_sec * 1000)) * 1000000;		
			}
			else
				nb_nsec = sleeping_time * 1000000;
		
			to_wait.tv_sec = nb_sec;
			to_wait.tv_nsec = nb_nsec;
#endif

			cout4 << "PollConfigureThread: smallest_upd = " << smallest_upd;
			cout4 << ", sleeping_time = " << sleeping_time;
			cout4 << ", nb_poll_objects = " << nb_cmd << endl;
		
			for (i = 0;i < nb_cmd;i++)
			{
				try
				{
					adm_dev->add_obj_polling(v_poll_cmd[i],false);
				}
				catch (Tango::DevFailed &e)
				{
					bool throw_ex = true;
				
					if (::strcmp(e.errors[0].reason.in(),"API_AlreadyPolled") == 0)
					{
						try
						{
							adm_dev->upd_obj_polling_period(v_poll_cmd[i],false);
							throw_ex = false;
						}
						catch (Tango::DevFailed &) {}
					}
						
					if (throw_ex == true)
					{
						TangoSys_OMemStream o;
						o << "System property polled_xxx for device " << v_poll_cmd[i]->svalue[0].in() << " has wrong syntax" << ends;
						Except::throw_exception((const char *)"API_BadConfigurationProperty",
				        				o.str(),
				        				(const char *)"PollConfigureThread::run_undetached()");
					}
				}


				if (nb_cmd > 1)
				{		
#ifdef WIN32
				Sleep(sleeping_time);
#else
				nanosleep(&to_wait,&inter);
#endif
				}
			}
		}
		
//
// Now, start the real polling
//

		adm_dev->start_polling();

//
// Delete allocated memory
//

		for (unsigned long l = 0;l < nb_cmd;l++)
			delete v_poll_cmd[l];

	}
	catch (Tango::DevFailed &e)
	{
		Tango::Except::print_exception(e);

		if (tg != NULL)
		{
			try
			{
				
//
// Stopping a device server means :
//  	- Send kill command to the polling thread
//    	- Join with this polling thread
//	- Unregister server in database
//	- Delete devices (except the admin one)
//	- Force writing file database in case of 
//	- Shutdown the ORB
//	- Cleanup Logging
//

				TangoMonitor &mon = tg->get_poll_monitor();
				PollThCmd &shared_cmd = tg->get_poll_shared_cmd();

				{	
					omni_mutex_lock sync(mon);

					shared_cmd.cmd_pending = true;
					shared_cmd.cmd_code = POLL_EXIT;

					mon.signal();

				}
				tg->get_polling_thread_object()->join(0);
				tg->clr_poll_th_ptr();
      	   			tg->unregister_server();
				tg->get_dserver_device()->delete_devices();

				if (tg->_FileDb == true)
				{
					Database *db_ptr = tg->get_database();
					delete db_ptr;
					cout4 << "Database object deleted" << endl;
				}
				cout4 << "Going to shutdown ORB" << endl;
				CORBA::ORB_ptr orb = tg->get_orb();
				orb->shutdown(true);
				CORBA::release(orb);
				cout4 << "ORB shutdown" << endl;

#ifdef TANGO_HAS_LOG4TANGO
	  			Logging::cleanup();
				cout4 << "Logging cleaned-up" << endl;
#endif

			}
			catch(...)
			{
#ifndef WIN32
				raise(SIGKILL);
#endif
			}
		}
	}				
}

//+----------------------------------------------------------------------------
//
// method : 		Util::trigger_attr_polling()
// 
// description : 	Trigger the polling thread for polled attributes
//			registered with a polling update period set as
//			"externally triggered" (0 mS)
//
//-----------------------------------------------------------------------------

void Util::trigger_attr_polling(Tango::DeviceImpl *dev,string &name)
{
	cout4 << "Sending trigger to polling thread" << endl;
	
//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << dev->get_name() << " is not polled" << ends;
		
		Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   	(const char *)"Util::trigger_attr_polling");
	}
	
//
// Find the wanted object in the list of device polled object
//

	string obj_name(name);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
		
	vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

//
// Check that it is an externally triggered polling object. If it is not the
// case, throw exception
//

	long tmp_upd = (*ite)->get_upd();
	if (tmp_upd != 0)
	{
		TangoSys_OMemStream o;

		o << "Polling for attribute ";		
		o << name;
		o << " (device " << dev->get_name() << ") ";
		o << " is not externally triggered.";
		o << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"Util::trigger_attr_polling");
	}
	
//
// Send command to the polling thread but wait in case of previous cmd 
// still not executed
//

	int interupted;
	TangoMonitor &mon = get_poll_monitor();
	PollThCmd &shared_cmd = get_poll_shared_cmd();

	{	
		omni_mutex_lock sync(mon);
		if (shared_cmd.trigger == true)
		{
			mon.wait();
		}
		shared_cmd.trigger = true;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = Tango::POLL_ATTR;

		mon.signal();
		
		cout4 << "Trigger sent to polling thread" << endl;
		
//
// Wait for thread to execute command
//

		TangoMonitor &dev_mon = dev->get_dev_monitor();
		omni_thread *th = omni_thread::self();
		
		while (shared_cmd.trigger == true)
		{

//
// Warning: It's possible to have a deadlock here (experienced under
// Windows) in case of this method being called from a command (or attribute
// methods) which are rapidly sent by the client.
// Client request cmd1 which send trigger to the polling thread
// The polling thread wake up clear shared_cmd.trigger and try to
// execute the command. But cmd 1 thread still owns the device monitor and
// polling thread wait. cmd 1 finished and client immediately send the
// command a new time. On Windows, it may happens that the polling
// thread is not activated just after the cmd thread has released the
// device monitor. As the client sent a new command, the device monitor
// is immediately re-taken by the thread executing the new command sent by
// the client. An order is sent to the polling thread and the cmd
// thread reach this code. It will wait for polling thread to clear
// shared_cmd.trigger. But, the polling thread is already waiting for
// the device monitor and ..... deadlock....
// 
		
			bool deadlock = false;
			if (th->id() == dev_mon.get_locking_thread_id())
			{
				cout4 << "Possible deadlock detected!" << endl;
				deadlock = true;
				dev_mon.rel_monitor();
				dev_mon.rel_monitor();
			}
			
			interupted = mon.wait(DEFAULT_TIMEOUT);
			if (deadlock == true)
			{
				dev_mon.get_monitor();
				dev_mon.get_monitor();
			}
			
			if ((shared_cmd.trigger == true) && (interupted == false))
			{
				cout4 << "TIME OUT" << endl;
				Except::throw_exception((const char *)"API_CommandTimedOut",
					        	(const char *)"Polling thread blocked !!!",
					        	(const char *)"Util::trigger_attr_polling");
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		Util::trigger_cmd_polling()
// 
// description : 	Trigger the polling thread for polled command
//			registered with a polling update period set as
//			"externally triggered" (0 mS)
//
//-----------------------------------------------------------------------------

void Util::trigger_cmd_polling(Tango::DeviceImpl *dev,string &name)
{
	cout4 << "Sending trigger to polling thread" << endl;
	
//
// Check that the device is polled
//

	if (dev->is_polled() == false)
	{
		TangoSys_OMemStream o;
		o << "Device " << dev->get_name() << " is not polled" << ends;
		
		Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   	(const char *)"Util::trigger_cmd_polling");
	}
	
//
// Find the wanted object in the list of device polled object
//

	string obj_name(name);
	transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
		
	vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);

//
// Check that it is an externally triggered polling object. If it is not the
// case, throw exception
//

	long tmp_upd = (*ite)->get_upd();
	if (tmp_upd != 0)
	{
		TangoSys_OMemStream o;

		o << "Polling for command ";		
		o << name;
		o << " (device " << dev->get_name() << ") ";
		o << " is not externally triggered.";
		o << ends;
		Except::throw_exception((const char *)"API_NotSupported",o.str(),
					(const char *)"Util::trigger_cmd_polling");
	}
	
//
// Send command to the polling thread but wait in case of previous cmd 
// still not executed
//

	int interupted;
	TangoMonitor &mon = get_poll_monitor();
	PollThCmd &shared_cmd = get_poll_shared_cmd();

	{	
		omni_mutex_lock sync(mon);
		if (shared_cmd.trigger == true)
		{
			mon.wait();
		}
		shared_cmd.trigger = true;
		shared_cmd.dev = dev;
		shared_cmd.name = obj_name;
		shared_cmd.type = Tango::POLL_CMD;

		mon.signal();
		
		cout4 << "Trigger sent to polling thread" << endl;
		
//
// Wait for thread to execute command
//

		TangoMonitor &dev_mon = dev->get_dev_monitor();
		omni_thread *th = omni_thread::self();
		
		while (shared_cmd.trigger == true)
		{
		
//
// Warning: It's possible to have a deadlock here (experienced under
// Windows) in case of this method being called from a command (or attribute
// methods) which are rapidly sent by the client.
// Client request cmd1 which send trigger to the polling thread
// The polling thread wake up clear shared_cmd.trigger and try to
// execute the command. But cmd 1 thread still owns the device monitor and
// polling thread wait. cmd 1 finished and client immediately send the
// command a new time. On Windows, it may happens that the polling
// thread is not activated just after the cmd thread has released the
// device monitor. As the client sent a new command, the device monitor
// is immediately re-taken by the thread executing the new command sent by
// the client. An order is sent to the polling thread and the cmd
// thread reach this code. It will wait for polling thread to clear
// shared_cmd.trigger. But, the polling thread is already waiting for
// the device monitor and ..... deadlock....
// 

			bool deadlock = false;
			if (th->id() == dev_mon.get_locking_thread_id())
			{
				cout4 << "Possible deadlock detected!" << endl;
				deadlock = true;
				dev_mon.rel_monitor();
				dev_mon.rel_monitor();
			}

			interupted = mon.wait(DEFAULT_TIMEOUT);
			if (deadlock == true)
			{
				dev_mon.get_monitor();
				dev_mon.get_monitor();
			}
			
			if ((shared_cmd.trigger == true) && (interupted == false))
			{
				cout4 << "TIME OUT" << endl;
				Except::throw_exception((const char *)"API_CommandTimedOut",
					        	(const char *)"Polling thread blocked !!!",
					        	(const char *)"Util::trigger_cmd_polling");
			}
		}
	}
	cout4 << "Thread cmd normally executed" << endl;
}


#ifdef WIN32
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
#endif /* WIN32 */


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

} // End of Tango namespace

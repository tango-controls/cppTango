static const char *RcsId = "$Header$";

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
// Revision 2.7  2002/12/16 10:16:50  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:08  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:21  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:26  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:05  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:12  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:15  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:12  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:15  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:46  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:17  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:21  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:43  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:16  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <tango.h>

#include <stdlib.h>
#include <dserversignal.h>
#include <dserverclass.h>

#ifndef WIN32
	#include <unistd.h>
	#include <assert.h>
	#include <sys/time.h>
#else
	#include <sys/timeb.h>
	#include <process.h>
        #include <coutbuf.h>
	#include <ntservice.h>
#endif /* WIN32 */


//	#include <ob/logger.h>
//	#include <ob/obwindows.h>

namespace Tango
{

Util *Util::_instance = NULL;
long Util::_tracelevel = 0;
bool Util::_UseDb = true;
bool Util::_daemon = false;
long Util::_sleep_between_connect = 60;
#ifdef WIN32
bool Util::_win = false;
bool Util::_service = false;
#endif

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

#if ((defined WIN32) || (defined __SUNPRO_CC))
Util::Util(int argc,char *argv[]):cout_tmp(cout.rdbuf()),cl_list_ptr(NULL)
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
// Get TANGO_HOST env. variable
//

		if (_UseDb == true)
			read_env();

#ifdef sun
//
// Create the signal object
// For Sun, it is necessary to create this object before the ORB is initialised.
// Otherwise, threads created by thread started by the ORB_init will not have
// the correct signal mask (set by the DServerSignal object) and the device
// server signal feature will not work
//

		DServerSignal::instance();
#endif
	
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
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT},
			{"serverCallTimeOutPeriod","5000"},
			{"maxServerThreadPoolSize","50"},
			{"threadPerConnectionUpperLimit","35"},
			{"threadPerConnectionLowerLimit","30"},
			{"supportCurrent","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};
			
			orb = CORBA::ORB_init(argc,argv,"omniORB4",options);
		}
		else
		{
			const char *options[][2] = {
			{"endPointPublishAllIFs","1"},
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT},
			{"serverCallTimeOutPeriod","5000"},
			{"maxServerThreadPoolSize","50"},
			{"threadPerConnectionUpperLimit","35"},
			{"threadPerConnectionLowerLimit","30"},
			{"supportCurrent","0"},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};
			
			orb = CORBA::ORB_init(argc,argv,"omniORB4",options);
		}
		
//
// Get some CORBA object references
//

		CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var root_poa = PortableServer::POA::_narrow(poaObj);

//
// If the database is not used, we must used the omniINSPOA poa
//

		PortableServer::POA_var nodb_poa;
		if (_UseDb == false)
		{
			CORBA::Object_var poaInsObj = orb->resolve_initial_references("omniINSPOA");
			nodb_poa = PortableServer::POA::_narrow(poaInsObj);	
		}
		
//
// Store POA
//

		if (_UseDb == true)
			_poa = PortableServer::POA::_duplicate(root_poa);
		else
			_poa = PortableServer::POA::_duplicate(nodb_poa);

//
// Initialize trace output (For Windows, the stdc++ lib also implements the
// new iostreams where the xx_with_assign classes are not defined. Therefore,
// to copy streams, I have used the advices in the C++ report of June 1997 
//

		trace_output = InitialOutput;
		file_stream = NULL;
#if ((defined WIN32) || (defined __SUNPRO_CC))
		cout_tmp.copyfmt(cout);
		cout_tmp.clear(cout.rdstate());
#else
		cout_tmp = cout;
#endif

//
// Connect to the database
//

		if (_UseDb == true)
		{
			connect_db();

//
// Display help message if requested by user. This is done after the process is 
// connected to the database becaue a call to the database server is done in the
// display_help_message(0 method
//
			
			if (display_help == true)
			{
				display_help_message();
			}

//
// Check if the server is not already running somewhere else
//

			server_already_running();
		}

#ifndef sun
//
// Create the signal object
//

		DServerSignal::instance();
#endif

//
// Create the polling thread and start it
//

		ext->poll_th = new PollThread(ext->shared_data,ext->poll_mon);
		ext->poll_th->start();
								
//
// Get process PID, host name and Tango version
//

		misc_init();
								
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

Util::Util(HINSTANCE hInst,int nCmdShow):cout_tmp(cout.rdbuf()),cl_list_ptr(NULL)
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
// Store the nCmdShow parameter
//

	nCmd = nCmdShow;
	
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
						_UseDb = false;
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
			
							long i,j;
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
	for (long i = 0;i < db_inst.size();i++)
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
	o << " [-nodb [-dlist <device name list>]]" << ends;
	
        print_err_message(o.str(),Tango::INFO);
}


//+----------------------------------------------------------------------------
//
// method : 		Util::read_env()
// 
// description : 	Get the TANGO_HOST env. variable and extract the 
//			database server host and port for it.
//
//-----------------------------------------------------------------------------

void Util::read_env()
{

//
// Read env. variable
//
	char *env;
#ifdef WIN32
	if (_service == true)
	{
		if (get_tango_host_from_reg(&env) == -1)
			env = NULL;
	}
	else
		env = getenv(EnvVariable);
#else
	env = getenv(EnvVariable);
#endif

	if (env == NULL)
	{
                print_err_message("TANGO_HOST environment variable not defined, exiting...");
	}
	
//
// Check env. variable syntax
//

	string env_str(env);
	string::size_type pos;
	if ((pos = env_str.find(':')) == string::npos)
	{
                print_err_message("Wrong syntax for TANGO_HOST environment variable\nSyntax = <host>:<port>",Tango::INFO);
	}
	
//
// Init database host and port variable
//

	db_host.assign(env_str,0,pos);
	if (db_host.size() == 0)
	{
                print_err_message("Wrong syntax for TANGO_HOST environment variable\nSyntax = <host>:<port>",Tango::INFO);
	}	
	
	db_port.assign(env_str,pos + 1,env_str.size());
	if (db_port.size() == 0)
	{
                print_err_message("Wrong syntax for TANGO_HOST environment variable\nSyntax = <host>:<port>",Tango::INFO);
	}
	
	TangoSys_MemStream s;
	s << db_port << ends;
	s >> db_port_int;
	
	cout4 << "Db host = " << db_host << ", Db port = " << db_port << endl;
}

#ifdef WIN32
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

long Util::get_tango_host_from_reg(char **buffer)
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
				db = new Database(db_host,db_port_int,orb.in());
				connected = true;
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
			db = new Database(db_host,db_port_int,orb.in());		
		}
		catch (CORBA::Exception &)
		{
                	TangoSys_OMemStream o;

			o << "Can't build connection to TANGO database server, exiting\n";
			o << "DB server host = " << db_host << ends;
                	print_err_message(o.str());
		}
	}
	
	if (CORBA::is_nil(db->get_dbase()))
	{
                TangoSys_OMemStream o;

		o << "Can't build connection to TANGO database server, exiting\n";
		o << "DB server host = " << db_host << ends;
                print_err_message(o.str());
	}
	cout4 << "Connected to database" << endl;
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
	
#if ((defined WIN32) || (defined __SUNPRO_CC))
	#ifdef WIN32
	pid = _getpid();
	#else
	pid = getpid();
	#endif
	
	o << pid << ends;
	pid_str = o.str().c_str();
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
// Get hostname
//
		
	char buffer[80];
	if (gethostname(buffer,80) == 0)
		hostname = buffer;
	else
	{
		print_err_message("Cant retrieve server host name");
	}
	
//
// Convert Tango version number to string (for device export)
//
		
	o.clear();
#if ((defined WIN32) || (defined __SUNPRO_CC))
	o.seekp(0,ios_base::beg);
	o << DevVersion << ends;
	version_str = o.str();
#else
	o.rdbuf()->seekoff(0,ios::beg,ios::in | ios::out);		
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
	backoff = 1000. * (float)rand() / (float)RAND_MAX;
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
	bool leave = true;
	
	try
	{
		CORBA::Any send;
		send <<= dev_name.c_str();
										
		CORBA::Any_var received = db->get_dbase()->command_inout("DbImportDevice",send);		
		const Tango::DevVarLongStringArray *db_dev;
		if ((received >>= db_dev) == false)
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
		long ctr;
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

		cout.rdbuf(ds_window->get_output_buffer());
		cout_tmp.rdbuf(ds_window->get_output_buffer());

	}
	
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
				orb->run();
		}
		else
			orb->run();
	}
#else
	orb->run();
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
	DeviceImpl *ret_ptr;

	string dev_name_lower(dev_name);
	transform(dev_name_lower.begin(),dev_name_lower.end(),dev_name_lower.begin(),Tango_tolower);
	
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
			transform(name.begin(),name.end(),name.begin(),Tango_tolower);
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
		transform(name.begin(),name.end(),name.begin(),Tango_tolower);
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
		for (int i = 0; i < dcl.size(); i++)
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
	int i, j, k;
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
			std::transform(dev_name.begin(), dev_name.end(), dev_name.begin(), Tango_tolower);
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

	if (_UseDb == true)
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
        exit(-1);
#endif	
}

//+----------------------------------------------------------------------------
//
// method : 		Util::polling_configure()
// 
// description : 	This method sends command to the polling thread for
//			all cmd/attr with polling configuration stored in db.
//
//-----------------------------------------------------------------------------

void Util::polling_configure()
{

//
// Send a stop polling command to thread in order not to poll devices
//

	DServer *adm_dev = get_dserver_device();
	adm_dev->stop_polling();
		
	vector<DeviceClass *> &tmp_cl_list = adm_dev->get_class_list();
	long i,j,k,upd;
	TangoSys_MemStream s;

//
// Create the structure used to send data to the polling thread
//
	
	DevVarLongStringArray *send = new DevVarLongStringArray();
	send->lvalue.length(1);
	send->svalue.length(3);

//
// A loop on each class and each device in class
//
	
	for (i = 0;i < tmp_cl_list.size();i++)
	{
		vector<DeviceImpl *> &dev_list = tmp_cl_list[i]->get_device_list();
		for (j = 0;j < dev_list.size();j++)
		{
			vector<string> &poll_cmd_list = dev_list[j]->get_polled_cmd();
			vector<string> &poll_attr_list = dev_list[j]->get_polled_attr();

//
// Send a Add Object command to the polling thread only if the polling
// period is different than zero
//

			for (k = 0;k < poll_cmd_list.size();k++)
			{
				if (k == 0)
				{
					send->svalue[0] = CORBA::string_dup(dev_list[j]->get_name().c_str());
					send->svalue[1] = CORBA::string_dup("command");
				}
							
//
// Convert polling period to a number
//

				s << poll_cmd_list[k + 1];
				s >> upd;
				s.clear();

//
// Send command to the polling thread
//
				
				if (upd != 0)
				{
					send->lvalue[0] = upd;
					send->svalue[2] = CORBA::string_dup(poll_cmd_list[k].c_str());
					
					try
					{
						adm_dev->add_obj_polling(send,false);
					}
					catch (Tango::DevFailed &)
					{
					}
				}
				
				k++;
				
			}

			for (k = 0;k < poll_attr_list.size();k++)
			{

				if (k == 0)
				{
					send->svalue[0] = CORBA::string_dup(dev_list[j]->get_name().c_str());
					send->svalue[1] = CORBA::string_dup("attribute");
				}
							
//
// Convert polling period to a number
//

				s << poll_attr_list[k + 1];
				s >> upd;
				s.clear();

//
// Send command to the polling thread
//
				
				if (upd != 0)
				{
					send->lvalue[0] = upd;
					send->svalue[2] = CORBA::string_dup(poll_attr_list[k].c_str());
					
					try
					{
						adm_dev->add_obj_polling(send,false);
					}
					catch (Tango::DevFailed &)
					{
					}
				}
				
				k++;
			}
		}
	}
	
	delete send;
	
//
// Now, start the real polling
//

	adm_dev->start_polling();
	
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
	return ds_window->get_output_buffer();
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
	nb_nanos = nb_millis * 1000000;
	nb_sec = nb_millis / 1000 ;
	nb_nanos = nb_nanos % 1000000000;

	omni_thread::get_time(&s,&n,nb_sec,nb_nanos);
	return cond.timedwait(s,n);
}


#ifdef __SUNPRO_CC
char my_tolower(char in)
{
	return (char)tolower(in);
}
#endif

} // End of Tango namespace

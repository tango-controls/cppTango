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
#include <OB/OBPolicies.h>
#include <OB/Properties.h>

#include <stdlib.h>
#include <dserversignal.h>
#include <dserverclass.h>

#ifndef WIN32
	#include <unistd.h>
	#include <assert.h>
#else
	#include <process.h>
	#include <ob/obwindows.h>
	#include <ob/logger.h>
        #include <coutbuf.h>
	#include <ntservice.h>
#endif /* WIN32 */

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

Util *Util::instance()
{
	if (_instance == NULL)
	{
                Util::print_err_message("Tango is not initialised !!!\nExiting");
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

#ifdef WIN32
Util::Util(int argc,char *argv[]):cout_tmp(cout.rdbuf())
#else
Util::Util(int argc,char *argv[])
#endif
{
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
	
//
// Initialise CORBA ORB
//

		OB::Properties_var props = OB::Properties::getDefaultProperties();
		OB::ParseArgs(argc,argv,props);

		props->setProperty("ooc.iiop.numeric","true");
#ifdef WIN32
		if (_service == true)
		{
			props->setProperty("ooc.orb.conc_model","threaded");
			props->setProperty("ooc.orb.oa.conc_model","threaded");
		}
#endif
		orb = OBCORBA::ORB_init(argc,argv,props);
	
//
// Get some CORBA object references
//

		CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA");
		root_poa = PortableServer::POA::_narrow(poaObj);

		CORBA::Object_var bmgrObj = orb->resolve_initial_references("BootManager");
		boot_manager = OB::BootManager::_narrow(bmgrObj);
			
//
// Initialise JTC
//

		JTCInitialize initialize(argc,argv);

//
// Initialize trace output (For Windows, the stdc++ lib also implements the
// new iostreams where the xx_with_assign classes are not defined. Therefore,
// to copy streams, I have used the advices in the C++ report of June 1997 
//

		trace_output = InitialOutput;
		file_stream = NULL;
#ifdef WIN32
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
// Check if the server is not already running somewhere else
//

			server_already_running();
		}

//
// Create the signal object
//

		DServerSignal::instance();
						
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

Util::Util(HINSTANCE hInst,int nCmdShow):cout_tmp(cout.rdbuf())
{

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
	
	if (argv[1][0] == '-')
	{
		print_usage(argv[0]);
	}
	ds_instance_name = argv[1];
	char *tmp;
#ifdef WIN32
	if ((tmp = strrchr(argv[0],'\\')) == 0)
#else
	if ((tmp = strrchr(argv[0],'/')) == 0)
#endif
		ds_exec_name = argv[0];
	else
	{
		tmp++;
		ds_exec_name = tmp;
	}
	
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
		while (ind < argc)
		{
			if (argv[ind][0] == '-')
			{
				switch (argv[ind][1])
				{
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

	long ctr;
	for (ctr = 0;ctr < ds_exec_name.size();ctr++)
		ds_exec_name[ctr] = tolower(ds_exec_name[ctr]);
	for (ctr = 0;ctr < ds_instance_name.size();ctr++)
		ds_instance_name[ctr] = tolower(ds_instance_name[ctr]);
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
		
	o << "usage :  " << serv_name << " instance_name [-v[trace level]]" << ends;
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
	
	strstream s;
	s << db_port;
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

#ifdef WIN32
	pid = _getpid();
	
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
#ifdef WIN32
	o.seekp(0,ios_base::beg);
#else
	o.rdbuf()->seekoff(0,ios::beg,ios::in | ios::out);
#endif
		
	o << DevVersion << ends;
	version_str = o.str();

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

	        CORBA::PolicyList policies;
	        policies.length(1);
	
	        CORBA::ULong t = 3000;
	        CORBA::Any a;
	        a <<= t;

	        policies[0] = orb->create_policy(OB::TIMEOUT_POLICY_ID,a);
        	CORBA::Object_var pol_obj = obj->_set_policy_overrides(policies,CORBA::ADD_OVERRIDE);
		
//
// Try to narrow the reference to a Tango::Device object
//
		
		dev = Tango::Device::_narrow(pol_obj);
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
	catch (CORBA::Exception &)
	{
	}
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
#endif

//
// Initialise main class
//

	DServerClass::init();
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
#ifdef WIN32
	if (_win == true)
	{
//
// Start the ORB thread (and loop)
//

		loop_th = new ORBWin32Loop(this);
		loop_th->start();
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
// Retrieve class list
//

	DServer *dserver = get_dserver_device();
	vector<DeviceClass *> &cl_list = dserver->get_class_list();

//
// Check if the wanted class really exists
//

	int nb_class = cl_list.size();
	int i;	
	for (i = 0;i < nb_class;i++)
	{
		if (cl_list[i]->get_name() == class_name)
			break;
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
	
	return cl_list[i]->get_device_list();
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
// Retrieve class list
//

	DServer *dserver = get_dserver_device();
	vector<DeviceClass *> &cl_list = dserver->get_class_list();

//
// Check if the wanted device exists in each class
//

	vector<DeviceImpl *> &dev_list = get_device_list_by_class(cl_list[0]->get_name());;
	int nb_class = cl_list.size();
	int i,j,nb_dev;	
	for (i = 0;i < nb_class;i++)
	{
		dev_list = get_device_list_by_class(cl_list[i]->get_name());
		nb_dev = dev_list.size();
		for (j = 0;j < nb_dev;j++)
		{
			if (dev_list[j]->get_name() == dev_name)
				break;
		}
	}

//
// Throw exception if the class is not found
//
	
	if ((i == nb_class) && (j == nb_dev))
	{
		TangoSys_OMemStream o;
		o << "Device " << dev_name << " not found" << ends;
		Except::throw_exception((const char *)"API_DeviceNotFound",
				        o.str(),
				        (const char *)"Util::get_device_by_name()");
	}
	
	return dev_list[j];
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

void Util::ORBWin32Loop::run()
{
	util->get_orb()->run();
}
#endif /* WIN32 */


} // End of Tango namespace

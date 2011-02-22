// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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


#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <ntservice.h>

#include <tango.h>

namespace Tango
{

//
// Static function to return a formatted error message for the last
// WIN32 error.
//

static char* GetErrorText()
{
    DWORD rc;
    char* str = 0;
    char* cstr = 0;
    rc = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
		             FORMAT_MESSAGE_FROM_SYSTEM|
		             FORMAT_MESSAGE_ARGUMENT_ARRAY,
		             0, GetLastError(), LANG_NEUTRAL, (char*)&str, 0, 0);
    if(str)
    {
		cstr = CORBA::string_dup(str);
		::LocalFree((HLOCAL)str);
    }
    else
    {
		cstr = CORBA::string_dup("FormatMessage failed");
    }
    return cstr;
}




// ----------------------------------------------------------------------
// NTEventLogger private and protected member implementation
// ----------------------------------------------------------------------

void NTEventLogger::emitMessage(int eventType, const char* msg)
{
    	const char* strings[1];
    
    	strings[0] = msg;
    
    	::ReportEvent(eventSource_,         // handle of event source
		      eventType,            // event type
		      0,                    // event logger
		      eventId_,             // event ID
		      NULL,                 // current user's SID
		      1,                    // strings in lpszStrings
		      0,                    // no bytes of raw data
		      strings,              // array of error strings
		      NULL);                // no raw data
}

bool NTEventLogger::installValues(HKEY key)
{
	char path[512];
    
//
// Get path to executable.
//

	if(::GetModuleFileName(NULL, path, sizeof(path)) == 0)
	{
		CORBA::String_var err = GetErrorText();
		cerr << "GetModuleFileName failed: " << err.in() << endl;
		return false;
   	}
    
    const char* keyname;

//
// Add two keys EventMessageFile which contains the path to the
// resources file, and TypesSupported which contains a DWORD with
// the permitted event types.
//

    keyname = "EventMessageFile";
    if(::RegSetValueEx(key, keyname, 0,REG_SZ, (unsigned char*)path,
		           strlen(path)+1) == ERROR_SUCCESS)
    {
		DWORD supported = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
		keyname = "TypesSupported";
		if(::RegSetValueEx(key, "TypesSupported", 0, REG_DWORD,(unsigned char*)&supported,
		                   sizeof(supported)) == ERROR_SUCCESS)
		{
	    		return true;
		}
    }

	CORBA::String_var err = GetErrorText();
    cerr << "RegSetValueEx " << keyname << " failed: " << err.in() << endl;
    return false;
}

// ----------------------------------------------------------------------
// NTEventLogger constructor/destructor
// ----------------------------------------------------------------------

NTEventLogger::NTEventLogger(const char* service, DWORD eventId)
    : service_(service),
      eventId_(eventId)
{
    eventSource_ = ::RegisterEventSource(NULL, service);
    if(eventSource_ == 0)
	{
		Except::throw_exception((const char *)"API_DatabaseAccess",
				        (const char *)"RegisterEventsource failed",
				        (const char *)"NTEventLogger::NTEventLogger");
	}
}

NTEventLogger::~NTEventLogger()
{
    ::DeregisterEventSource(eventSource_);
}

// ----------------------------------------------------------------------
// NTEventLogger public member implementation
// ----------------------------------------------------------------------

bool NTEventLogger::install()
{

//
// Constructor the key name
//

	string keyName ("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
    keyName += service_;

//
// The key goes into HKEY_LOCAL_MACHINE
//

    HKEY regHandle = HKEY_LOCAL_MACHINE;

//
// First try to open the key.
//

    HKEY keyHandle;
    if(::RegOpenKeyEx(regHandle,keyName.c_str(), 0, KEY_ALL_ACCESS, &keyHandle) != ERROR_SUCCESS)
    {
    
//
// Create the key
//
		DWORD d;
		if(::RegCreateKeyEx(regHandle, keyName.c_str(), 0, "REG_SZ",
			    	    REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0,
			    	    &keyHandle, &d) != ERROR_SUCCESS)
		{
			CORBA::String_var err = GetErrorText();
	    		cerr << "RegCreateKeyEx " << keyName.c_str() << " failed: " << err.in() << endl;
	    		return false;
		}
    }

    bool rc = installValues(keyHandle);

    ::RegCloseKey(keyHandle);

    return rc;
}

bool
NTEventLogger::uninstall()
{

//
// Constructor the key name
//
    
	string keyName("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
    keyName += service_;

    return ::RegDeleteKey(HKEY_LOCAL_MACHINE, keyName.c_str()) == ERROR_SUCCESS;
}

void NTEventLogger::info(const char* msg)
{
    emitMessage(EVENTLOG_INFORMATION_TYPE, msg);
}

void NTEventLogger::error(const char* msg)
{
    emitMessage(EVENTLOG_ERROR_TYPE, msg);
}

void NTEventLogger::warning(const char* msg)
{
    emitMessage(EVENTLOG_WARNING_TYPE, msg);
}

void NTEventLogger::trace(const char* logger, const char* msg)
{
	string s(logger);
    s += ": ";
    s += msg;
    emitMessage(EVENTLOG_INFORMATION_TYPE, s.c_str());
}

//
// Define callbacks
//
static void WINAPI _OB_serviceCtrl(DWORD ctrlCode)
{
   	NTService::instance() -> control(ctrlCode);
}

static void WINAPI _OB_serviceMain(DWORD argc, LPTSTR* argv)
{
    NTService::instance() -> main(argc, argv);
}

static BOOL WINAPI _OB_controlHandler(DWORD dwCtrlType)
{
    switch( dwCtrlType )
    {
        case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
        case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
            NTService::instance() -> stop();
            return true;
            break;

    }
    return false;
} 

//
// The NTService - only one instance is allowed
//
NTService* NTService::instance_ = 0;

// ----------------------------------------------------------------------
// NTService private and protected member implementation
// ----------------------------------------------------------------------

void NTService::statusUpdate(DWORD currentState, DWORD exitCode, DWORD waitHint)
{

//
// when debugging we don't report to the SCM
//

    if(debug_)
        return;
    
    if (currentState == SERVICE_START_PENDING)
        status_.dwControlsAccepted = 0;
    else
       	 status_.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    status_.dwCurrentState = currentState;
    status_.dwWin32ExitCode = exitCode;
    status_.dwWaitHint = waitHint;

    if ( ( currentState == SERVICE_RUNNING ) ||
             ( currentState == SERVICE_STOPPED ))
        status_.dwCheckPoint = 0;
    else
        status_.dwCheckPoint = checkPoint_++;

//
// Report the status of the service to the service control
// manager.
//

    if(!SetServiceStatus( statusHandle_, &status_) && logger_ != 0)
		logger_ -> error("SetServiceStatus");
    SetServiceStatus(statusHandle_, &status_);
}

void NTService::control(DWORD ctrlCode)
{

//
// Handle the requested control code.
//
    switch(ctrlCode)
    {
    case SERVICE_CONTROL_STOP:
	
//
// Stop the service.
//
// SERVICE_STOP_PENDING should be reported before setting the
// Stop Event - hServerStopEvent - in ServiceStop().  This
// avoids a race condition which may result in a 1053 - The
// Service did not respond...  error.
//

    	statusUpdate(SERVICE_STOP_PENDING);
        stop();
        return;
        
    case SERVICE_CONTROL_INTERROGATE:
	
//
// Update the service status.
//
        break;

   	 default:
//
// invalid control code
//
        	break;
    }

    statusUpdate(status_.dwCurrentState);
}

void NTService::main(int argc, char** argv)
{

//
// Register our service control handler:
//

    statusHandle_ = ::RegisterServiceCtrlHandler(name_.c_str(), _OB_serviceCtrl);
    if(statusHandle_)
    {
	
//
// SERVICE_STATUS members that don't change in example
//
        status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        status_.dwServiceSpecificExitCode = 0;
        
//
// report the status to the service control manager.
//

        statusUpdate(SERVICE_START_PENDING, NO_ERROR, 3000);

//
// Retrieve parameters from the registry. The first (and only) parameters
// sent by Windows to this method is the service name.
//

        string keyName = "SYSTEM\\CurrentControlSet\\Services\\";
		keyName = keyName + argv[0] + "\\Server\\";

//
// The key goes into HKEY_LOCAL_MACHINE
//

    	HKEY regHandle = HKEY_LOCAL_MACHINE;

//
// First try to open the key.
//

    	HKEY keyHandle;
		int	errcode;
		stringstream	s;
    	if((errcode=::RegOpenKeyEx(regHandle,keyName.c_str(),0,KEY_ALL_ACCESS,
		                  &keyHandle)) != ERROR_SUCCESS)
    	{
			s << "Can't retrieve server registry key ! error : ";
			s << errcode << ends;
			string st = s.str();
			logger_->error(st.c_str());
			return;
    	}

//
// Get instance name to send it to the start user method
//

    	DWORD type,size;
    	char buf[128];
		size = 127;
    	if ((errcode=::RegQueryValueEx(keyHandle,"InstanceName",NULL,&type,
			  (unsigned char *)buf,&size)) != ERROR_SUCCESS)
    	{
			s.seekp(0);
			s.seekg(0);
			s << "Can't retrieve instance name from registry ! error : ";
			s << errcode;
			if (errcode==ERROR_MORE_DATA)
				s << " - Size required is " << size << "bytes" << endl;
			s << ends;

			string st = s.str();
			logger_->error(st.c_str());
			return;
    	}

//
// Called the user start method with three parameters which are :
//      - the exec name
//	- the server instance name
// 	- the logger
//

		string message("Starting ");
		message = message + exec_name_ + '/' + buf + " tango device server";
		logger_->info(message.c_str());

    	char *ptr[2];
    	ptr[0] = const_cast<char *>(full_exec_name_.c_str());
    	ptr[1] = &(buf[0]);

		start(2,ptr,logger_);
    }

//
// try to report the stopped status to the service control manager.
//
    if (statusHandle_)
        statusUpdate(SERVICE_STOPPED);
}

// ----------------------------------------------------------------------
// NTService constructor and destructor
// ----------------------------------------------------------------------

NTService::NTService(const char* name): full_exec_name_(name),
debug_(false),
checkPoint_(0),
stopped_(false)
{
    instance_ = this;
	
//
// Build exececutable name from full executable name
//

	char *tmp;

	if ((tmp = strrchr((char *)name,'\\')) == 0)
		exec_name_ = full_exec_name_;
	else
	{
		tmp++;
		exec_name_ = tmp;
	}
	
//
// Remove the .exe after the executable name
//

	string::size_type pos;
	if ((pos = exec_name_.find('.')) != string::npos)
	{
		exec_name_.erase(pos,exec_name_.size());
	}

//
// Build the logger object
//

	logger_ = new NTEventLogger(exec_name_.c_str(),0);

}


NTService::~NTService()
{
    if(logger_ != 0)
    {
		delete logger_;
		logger_ = 0;
    }
    instance_ = 0;
}

// ----------------------------------------------------------------------
// NTService public member implementation
// ----------------------------------------------------------------------

NTService *NTService::instance()
{
    return instance_;
}

bool NTService::install(char *inst_name,bool autoStart)
{
	bool rc = false;
    char path[512];

//
// Get path to executable.
//

    if(::GetModuleFileName(NULL, path, sizeof(path)) == 0)
    {
		CORBA::String_var err = GetErrorText();
		cerr << "GetModuleFileName failed: " << err.in() << endl;
        return rc;
    }

//
// Open the service manager.
//
    
    SC_HANDLE managerHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!managerHandle)
    {
		CORBA::String_var err = GetErrorText();
        cerr << "OpenSCManager failed: " <<  err.in() << endl;
        return rc;
    }
	
//
// Build service name and title
//

	name_ = exec_name_ + '_';
	name_ = name_ + inst_name;
	title_ = exec_name_ + " Tango device server (" + inst_name + ")";
	
//
// Create a new service.
//
    
    DWORD start = (autoStart) ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
    SC_HANDLE serviceHandle = ::CreateService(managerHandle,              // SCManager database
						  name_.c_str(),              // name of service
						  title_.c_str(),             // name to display
						  SERVICE_ALL_ACCESS,         // desired access
						  SERVICE_WIN32_OWN_PROCESS,  // service type
						  start,                      // start type
						  SERVICE_ERROR_NORMAL,       // error control type
						  path,                       // service's binary
						  NULL,                       // no load ordering group
						  NULL,                       // no tag identifier
						  "",                         // dependencies
						  NULL,                       // LocalSystem account
						  NULL);                      // no password
    
    if(serviceHandle)
    {
        cout << title_.c_str() << ": installed." << endl;
        ::CloseServiceHandle(serviceHandle);
        rc = true;
    }
    else
    {
		CORBA::String_var err = GetErrorText();
        cerr << "CreateService Failed: " << err.in() << endl;
    }
    
    ::CloseServiceHandle(managerHandle);


//
// Initialize the event logger.
//

    if(rc && logger_ != 0)
    {
		rc = logger_ -> install();
    }
	
//
// Store instance name and TANGO_HOST env. variable in the registry.
// The key name is built from the exec name and the instance name//
//

    string keyName("SYSTEM\\CurrentControlSet\\Services\\");
    keyName = keyName + name_;
    keyName = keyName + "\\Server\\";

//
// The key goes into HKEY_LOCAL_MACHINE
//

    HKEY regHandle = HKEY_LOCAL_MACHINE;

//
// First try to open the key.
//

    HKEY keyHandle;
    if(::RegOpenKeyEx(regHandle, keyName.c_str(), 0, KEY_ALL_ACCESS,
			  &keyHandle) != ERROR_SUCCESS)
    {
	
//
// Create the key
//
		DWORD d;
		if(::RegCreateKeyEx(regHandle, keyName.c_str(), 0, "REG_SZ",
			    	    REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0,
			    	    &keyHandle, &d) != ERROR_SUCCESS)
		{
			CORBA::String_var err = GetErrorText();
	    	cerr << "RegCreateKeyEx " << keyName.c_str() << " failed: " << err.in() << endl;
	    	return false;
		}
    }

//
// Add two values which contains the instance name
// and the Tango_host environment variable value
//

	const char *keyname;
    keyname = "InstanceName";
    if(::RegSetValueEx(keyHandle,keyname,0,REG_SZ,(unsigned char*)inst_name,
		           strlen(inst_name)+1) != ERROR_SUCCESS)
    {
		CORBA::String_var err = GetErrorText();
		cerr << "RegSetValueEx " << keyname << " failed: " << err.in() << endl;
		return false;
    }

    char *env = getenv(EnvVariable);
	if (env == NULL)
	{
		cerr << "Tango_host environment variable not defined !!!" << endl;
		return false;
	}
	
    keyname = "TangoHost";
    if (::RegSetValueEx(keyHandle,keyname,0,REG_SZ,(unsigned char *)env,
			    strlen(env) + 1) != ERROR_SUCCESS)
    {
		CORBA::String_var err = GetErrorText();
		cerr << "RegSetValueEx " << keyname << " failed: " << err.in() << endl;
		return false;
    }

    return rc;
}

bool NTService::uninstall(char *inst_name)
{
    bool rc = false;

    SC_HANDLE managerHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(!managerHandle)
    {
		CORBA::String_var err = GetErrorText();
        cerr << "OpenSCManager failed: " << err.in() << endl;
        return rc;
    }

	name_ = exec_name_ + '_';
	name_ = name_ + inst_name;
	title_ = exec_name_ + " Tango device server (" + inst_name + ")";
	SC_HANDLE serviceHandle = ::OpenService(managerHandle, name_.c_str(),
					    SERVICE_ALL_ACCESS);
    if(serviceHandle)
    {
	
//
// try to stop the service
//

        if(::ControlService(serviceHandle, SERVICE_CONTROL_STOP, &status_))
        {
           	 cerr << "Stopping: " << title_ << endl;
           	 Sleep( 1000 );

           	 while(::QueryServiceStatus(serviceHandle, &status_))
           	 {
           		 if(status_.dwCurrentState == SERVICE_STOP_PENDING)
           		 {
           			 cerr << "." << endl;
           			 Sleep( 1000 );
           		 }
           		 else
           			 break;
           	 }
           	 if(status_.dwCurrentState == SERVICE_STOPPED)
           		 cerr << title_ << ": stopped." << endl;
           	 else
           		 cerr << title_ << ": failed to stop." << endl;
        }
        
//
// now remove the service
//

        if(::DeleteService(serviceHandle))
        {
        	rc = true;
        	cerr << title_ << ": removed." << endl;
        }
       	else
        {
			CORBA::String_var err = GetErrorText();
			cerr << "DeleteService failed: " << err.in() << endl;
        }
        ::CloseServiceHandle(serviceHandle);
    }
    else
    {
		CORBA::String_var err = GetErrorText();
		cerr << "OpenService failed: " << err.in() << endl;
    }
    ::CloseServiceHandle(managerHandle);

//
// Initialize the event logger.
//

    if(rc && logger_ != 0)
    {
		rc = logger_ -> uninstall();
    }

    return rc;
}

void NTService::run(int argc, char** argv)
{
    if(debug_)
    {
        ::SetConsoleCtrlHandler(_OB_controlHandler, true);
        start(argc, argv, logger_);
    }
    else
    {
	
//
// Define the service dispatch table.
//

        SERVICE_TABLE_ENTRY dispatchTable[] =
        {
            {const_cast<char *>(name_.c_str()),(LPSERVICE_MAIN_FUNCTION)_OB_serviceMain },
            {NULL, NULL }
        };

//
// Start the service.
//

        if(!::StartServiceCtrlDispatcher(dispatchTable))
		{
			string err("StartServiceCtrlDispatcher: ");
	    	err += GetErrorText();
	    	if(logger_ != 0)
				logger_ -> error(err.c_str());
		}
   	}
}

void NTService::stop()
{
	Tango::Util *tg = Tango::Util::instance();

	stopped_ = true;
//	tg->get_orb()->shutdown(false);

	string message(tg->get_ds_name());
	message = message + " tango device server stopped";
	logger_->info(message.c_str());

	statusUpdate(SERVICE_STOPPED);
}

bool NTService::is_installed()
{
	bool ret = true;
    	return ret;
}

int NTService::options(int argc,char *argv[])
{

//
// Find command line argument specific to service
//
// TO DO
// TAKE CARE: When the service is started, this method is called with only 
// one arg. We can't test argc <=2 or we have to refuse to do anything if argc
// <2 only if it is not a start command
//

	if (argc == 2)
	{
		usage(argv[0]);
		return -1;
	}

	long start;
	if (argc > 2)
	{
		if (strncmp(argv[2],"-v",2) == 0)
			start = 3;
		else
			start = 2;
	}
	else
		start = 2;

//
// Parse command line
//

	long i;
	for (i = start;i < argc && *argv[i] == '-';i++)
	{
		if (strcmp(argv[i],"-h") == 0)
		{
			usage(argv[0]);
			return 0;
		}
		else if (strcmp(argv[i],"-i") == 0)
		{
			if (argv[1][0] == '-')
				usage(argv[0]);
			else
				install(argv[1]);
			return 0;
		}
		else if (strcmp(argv[i],"-s") == 0)
		{
			if (argv[1][0] == '-')
				usage(argv[0]);
			else
				install(argv[1],true);
			return 0;
		}
		else if (strcmp(argv[i],"-u") == 0)
		{
			if (argv[1][0] == '-')
				usage(argv[0]);
			else
				uninstall(argv[1]);
			return 0;
		}
		else if (strcmp(argv[i],"-dbg") == 0)
		{
			setDebug();
			return 1;
		}
	}
	return 1;
}

void NTService::usage(const char *prog_name)
{
	cerr << "Usage:\n";
	cerr << prog_name << " instance_name [-v[trace_level]] options" << endl;
	cerr << "Options :" << endl;
	cerr << "-h\tShow this message" << endl;
	cerr << "-i\tInstall the service" << endl;
	cerr << "-s\tInstall the service with automatic startup" << endl;
	cerr << "-u\tUninstall the service" << endl;
	cerr << "-dbg\tRun in console mode" << endl;
}

} // End of Tango namespace

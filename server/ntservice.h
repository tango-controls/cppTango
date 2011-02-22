
#ifndef _NTSERVICE_H
#define _NTSERVICE_H

#include <OB/Basic.h>
#include <OB/Object.h>
#include <OB/Logger.h>

#include <string>

using namespace std;

namespace Tango
{

//
// This is a custom message logger that is used to dump messages to
// the NT system log -- the user must ensure that this logger is
// used for NT native services
//

class NTEventLogger : public OB::Logger
{
private:
    	HANDLE eventSource_; 			// The event source
	CORBA::String_var service_; 			// Service name
    	DWORD eventId_; 			// The event ID
    
    	void emitMessage(int, const char*);
    	bool installValues(HKEY);

public:

    	NTEventLogger(const char*, DWORD);
    	~NTEventLogger();
    
    	bool install();    
    	bool uninstall();
    
    	virtual void info(const char*);
    	virtual void error(const char*);
    	virtual void warning(const char*);
    	virtual void trace(const char*, const char*);
};


//
// This class represents an NT service.
//

class NTService
{
private:    
   	static NTService* instance_; 		// The one and only instance

	string full_exec_name_;			// Full executable name 
	string exec_name_;			// Executable name   
    	string name_; 				// The name
    	string title_; 				// The title
    	bool debug_; 				// Are we debugging?
    	DWORD checkPoint_; 			// Check point value
    	SERVICE_STATUS status_; 		// Status of the service
    	SERVICE_STATUS_HANDLE statusHandle_; 	// Status handle

    	virtual void start(int, char**, OB::Logger_ptr) = 0;

    	void control(DWORD);
    	void main(int argc, char** argv);

    	friend void WINAPI _OB_serviceCtrl(DWORD);
    	friend void WINAPI _OB_serviceMain(DWORD, LPTSTR*);
	friend class Util;
    
protected:
    	void statusUpdate(DWORD, DWORD = NO_ERROR, DWORD = 0);
	bool stopped_;

public:
	Tango::NTEventLogger *logger_; 	// The service logger

	bool is_installed();

    	NTService(const char *name);

    	virtual ~NTService();

    	static NTService* instance();

    	bool install(char *,bool = false);
    	bool uninstall(char *);

    	void run(int, char**);
    	virtual void stop();
	
	int options(int,char **);
	void usage(const char *);
	
    	void setDebug() { debug_ = true; }
    	bool getDebug() const { return debug_; }
};

} // End of Tango namespace

#endif

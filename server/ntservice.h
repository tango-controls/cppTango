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

#ifndef _NTSERVICE_H
#define _NTSERVICE_H

#include <tango.h>

using namespace std;

namespace Tango
{

//
// This is a custom message logger that is used to dump messages to
// the NT system log -- the user must ensure that this logger is
// used for NT native services
//

//class NTEventLogger : public OB::Logger,
//		      public OBCORBA::RefCountLocalObject
class NTEventLogger
{
private:
    	HANDLE eventSource_; 			// The event source
	string service_; 			// Service name
    	DWORD eventId_; 			// The event ID
    
    	void emitMessage(int, const char*);
    	bool installValues(HKEY);

public:

    	NTEventLogger(const char*, DWORD);
    	virtual ~NTEventLogger();
    
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

   	virtual void start(int, char**, Tango::NTEventLogger *ptr) = 0;

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

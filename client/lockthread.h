//=============================================================================
//
// file :               LockThread.h
//
// description :        Include for the LockThread object. This class implements
//                      the locking thread
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
// Revision 3.6  2009/02/27 13:24:43  taurel
// - Small changes for Solaris
//
// Revision 3.5  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.4  2008/11/18 09:30:19  taurel
// - Ported to gcc 4.3
// - Removed some cout messages
//
// Revision 3.3  2008/10/06 15:02:17  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.2  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.1  2008/05/20 12:42:30  taurel
// - Commit after merge with release 7 branch
//
// Revision 1.1.2.2  2008/01/03 16:04:23  taurel
// - Some changes in locking feature implementation
//
// Revision 1.1.2.1  2007/12/19 15:53:08  taurel
// - Still some work going on for the locking feature
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _LOCKTHREAD_H
#define _LOCKTHREAD_H

#include <tango.h>
#ifndef _TG_WINDOWS_
#include <sys/time.h>
#endif

namespace Tango
{

//=============================================================================
//
//			The LockThCmd structure
//
// description :	This structure is used to shared data between the locking
//			thread and the main thread.
//
//=============================================================================

struct LockThCmd
{
	bool			cmd_pending;	// The new command flag
	LockCmdCode		cmd_code;		// The command code
	string			dev_name;		// The device name
	DevLong			lock_validity;	// The lock validity
	bool			suicide;		// The suicide flag
};

struct LockedDevice
{
	string			dev_name;		// The locked device name
	DevLong			validity;		// The locked device validity
	
	bool operator<(LockedDevice &arg) {return validity < arg.validity;}
};

enum LockCmdType
{
	LOCK_TIME_OUT = 0,
	LOCK_COMMAND
};

//=============================================================================
//
//			The LockThread class
//
// description :	Class to store all the necessary information for the
//			locking thread. It's run() method is the thread code
//
//=============================================================================

class TangoMonitor;

class LockThread: public omni_thread
{
public:
	LockThread(LockThCmd &,TangoMonitor &,DeviceProxy *,string &,DevLong);
	
	void run(void *);
	
	void execute_cmd();
	void one_more_lock();
	void unlock_all_devs();
	void update_th_period();
	void compute_sleep_time(bool);
	LockCmdType get_command(DevLong);

protected:	
	LockThCmd				&shared_cmd;
	TangoMonitor			&p_mon;

	LockThCmd				local_cmd;
	DevLong					sleep;
	
	vector<LockedDevice>	locked_devices;
	vector<string>			re_lock_cmd_args;
	DevLong					period;
	DevLong 				period_ms;
	DeviceProxy				*admin_proxy;
	
	struct timeval			next_work;
};
	
} // End of Tango namespace

#endif /* _LOCKTHREAD_ */

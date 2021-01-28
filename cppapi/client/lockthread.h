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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//
//=============================================================================

#ifndef _LOCKTHREAD_H
#define _LOCKTHREAD_H

#include <tango.h>
#ifndef _TG_WINDOWS_
#include <sys/time.h>
#endif
#include <tango_optional.h>

#include <chrono>

namespace Tango
{

using LockClock = std::chrono::steady_clock;

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
	bool cmd_pending;                   // The new command flag
	LockCmdCode cmd_code;               // The command code
	std::string dev_name;               // The device name
	LockClock::duration lock_validity;  // The lock validity
	bool suicide;                       // The suicide flag
};

struct LockedDevice
{
	std::string dev_name;           // The locked device name
	LockClock::duration validity;   // The locked device validity

	bool operator<(const LockedDevice &arg) const {return validity < arg.validity;}
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
	LockThread(LockThCmd &,TangoMonitor &,DeviceProxy *,std::string &, LockClock::duration);

	void run(void *);

	void execute_cmd();
	void one_more_lock();
	void unlock_all_devs();
	void update_th_period();
	void compute_sleep_time(bool);
	LockCmdType get_command();

protected:
	LockThCmd				&shared_cmd;
	TangoMonitor			&p_mon;

	LockThCmd				local_cmd;
	tango_optional<LockClock::duration> sleep;

	std::vector<LockedDevice>	locked_devices;
	std::vector<std::string>			re_lock_cmd_args;
	LockClock::duration period;
	DeviceProxy				*admin_proxy;

	LockClock::time_point next_work;
};

} // End of Tango namespace

#endif /* _LOCKTHREAD_ */

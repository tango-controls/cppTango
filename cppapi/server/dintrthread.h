//==================================================================================================================
//
// file :               dintrthread.h
//
// description :        Include for the DevIntrThread object. This class implements the thread used to push device
//						interface change event
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2014
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//==================================================================================================================

#ifndef _DINTRTHREAD_H
#define _DINTRTHREAD_H

namespace Tango
{

class DevIntr;

//=================================================================================================================
//
//			The DevIntrThCmd structure
//
// description :
//		This structure is used to shared data between the Device interface change thread and the main thread.
//
//================================================================================================================

enum DevIntrCmdCode
{
	DEV_INTR_SLEEP = 0,
	DEV_INTR_EXIT
};

struct _ShDevIntrTh
{
	bool			cmd_pending;	// The new command flag
	DevIntrCmdCode	cmd_code;		// The command code
	bool			th_running;		// Thread running flag
	#if _MSC_VER >= 1900
	#ifdef interface
	#undef interface
	#endif
	#endif
		DevIntr                 interface;              // Device interface
};
typedef struct _ShDevIntrTh	ShDevIntrTh;

enum DevIntrCmdType
{
	DEV_INTR_TIME_OUT = 0,
	DEV_INTR_COMMAND
};

//=================================================================================================================
//
//			The DevIntrThread class
//
// description :
//		Class to store all the necessary information for the device interface change event thread.
//		It's run() method is the thread code
//		This is a detached thread
//
//=================================================================================================================

class TangoMonitor;

class DevIntrThread: public omni_thread
{
public:
	DevIntrThread(ShDevIntrTh &,TangoMonitor &,DeviceImpl *);

	void run(void *);

	void execute_cmd();
	DevIntrCmdType get_command(DevLong);
	void push_event();

protected:
	ShDevIntrTh				&shared_data;
	TangoMonitor			&p_mon;
	DeviceImpl				*dev;

	ShDevIntrTh				local_cmd;
};

} // End of Tango namespace

#endif /* _DINTRTHREAD_ */

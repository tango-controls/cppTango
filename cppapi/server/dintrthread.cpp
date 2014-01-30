static const char *RcsId = "$Id$";

//+=================================================================================================================
//
// file :               dintrthread.cpp
//
// description :        C++ source code for the DevIntrThread class. This class is used for the device interface
//						change thread. The rule of this thread is to delay the event sending in order to minimize
//						the number of events thrown when dynamic command(s) and/or attribute(s) are added/removed
//						in a loop.
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
//-================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <dintrthread.h>

namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntrThread::DevIntrThread
//
// description :
//		The device interface change thread constructor.
//
//-------------------------------------------------------------------------------------------------------------------

DevIntrThread::DevIntrThread(ShDevIntrTh &cmd,TangoMonitor &m):
shared_data(cmd),p_mon(m)
{
	shared_data.cmd_pending = false;
}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntrThread::run_undetached
//
// description :
//		The device interface change thread main code
//
//------------------------------------------------------------------------------------------------------------------

void DevIntrThread::run(TANGO_UNUSED(void *ptr))
{

//
// The main loop
//

	bool exit = false;

	try
	{
		while (exit == false)
		{
			DevIntrCmdType received = get_command(100);

			switch (received)
			{
			case DEV_INTR_COMMAND:
				execute_cmd();
				break;

			case DEV_INTR_TIME_OUT:
				push_event();
				exit = true;
				break;
			}
		}

		omni_thread::exit();
	}
	catch (omni_thread_fatal &)
	{
		cerr << "OUPS !! A omni thread fatal exception in a device interface change event thread!!!!!!!!" << endl;
	}

}

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntrThread::get_command
//
// description :
//		This method wait on the shared monitor for a new command to be sent to the device interface change thread.
//		The thread waits with a timeout.
//
// argument :
//		in :
//			- tout : The timeout in mS
//
// return :
//		One enum with two possible value: Thread awaken due to timeout or thread awaken due to a received command
//
//------------------------------------------------------------------------------------------------------------------

DevIntrCmdType DevIntrThread::get_command(DevLong tout)
{
	omni_mutex_lock sync(p_mon);
	DevIntrCmdType ret;

//
// Wait on monitor
//

	if (shared_data.cmd_pending == false)
	{
		p_mon.wait(tout);
	}

//
// Test if it is a new command. If yes, copy its data locally
//

	if (shared_data.cmd_pending == true)
	{
		local_cmd = shared_data;
		ret = DEV_INTR_COMMAND;
	}
	else
		ret = DEV_INTR_TIME_OUT;

	return ret;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntrThread::execute_cmd
//
// description :
//		This method is called when a command has been received. It execute the command!
//
//------------------------------------------------------------------------------------------------------------------

void DevIntrThread::execute_cmd()
{
	vector<string>::iterator pos;
	bool need_exit = false;

	switch (local_cmd.cmd_code)
	{

//
// A new rest
//

		case Tango::DEV_INTR_SLEEP :
		{
			break;
		}

//
// Ask locking thread to exit
//

		case Tango::DEV_INTR_EXIT :
			need_exit = true;
			break;
	}

//
// Inform requesting thread that the work is done
//

	{
		omni_mutex_lock sync(p_mon);
		shared_data.cmd_pending = false;
		p_mon.signal();
	}

//
// If the command was an exit one, do it now
//

	if (need_exit == true)
	{
		omni_thread::exit();
	}
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DevIntrThread::push_event
//
// description :
//		This method pushes the device interface change event if there is a interface change
//
//------------------------------------------------------------------------------------------------------------------

void DevIntrThread::push_event()
{

}

} // End of Tango namespace

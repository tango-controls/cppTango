static const char *RcsId = "$Id$";

//+============================================================================
//
// file :               LockThread.cpp
//
// description :        C++ source code for the LockThread class.
//			This class is used for the locking thread. The rule of 
//			this thread is to regulary send the device locking command
//			to a DS admin device
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
// Revision 3.6  2009/03/02 15:55:51  taurel
// - Ported to Windows
//
// Revision 3.5  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.4  2008/10/06 15:02:17  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.3  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.2  2008/09/04 07:35:38  taurel
// - Fix bug in memorized attributes
// - Changes for the new IDL 4
//
// Revision 3.1  2008/05/20 12:42:30  taurel
// - Commit after merge with release 7 branch
//
// Revision 1.1.2.3  2008/01/03 16:04:23  taurel
// - Some changes in locking feature implementation
//
// Revision 1.1.2.2  2007/12/20 14:27:18  taurel
// - Some more work on locking
//
// Revision 1.1.2.1  2007/12/19 15:53:08  taurel
// - Still some work going on for the locking feature
//
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <lockthread.h>

#include <functional>
#include <algorithm>
#include <time.h>

#ifndef _TG_WINDOWS_
#include <sys/time.h>
#else
#include <sys/types.h>
#include <sys/timeb.h>
#endif

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		LockThread::LockThread
// 
// description : 	The locking thread constructor.
//
//--------------------------------------------------------------------------

LockThread::LockThread(LockThCmd &cmd,TangoMonitor &m,DeviceProxy *adm,string &dev,DevLong per):
shared_cmd(cmd),p_mon(m),admin_proxy(adm)
{	
	LockedDevice ld;
	ld.dev_name = dev;
	ld.validity = per;
	locked_devices.push_back(ld);
	
	DevLong tmp_usec = (per * 1000000) - 500000;
	period = tmp_usec;
	period_ms = tmp_usec / 1000;
	sleep = period_ms;
}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::run_undetached
// 
// description : 	The locking thread main code
//
//--------------------------------------------------------------------------
					  
void LockThread::run(void *ptr)
{
	LockCmdType received;

#ifdef _TG_WINDOWS_
	struct _timeb now_win;

	_ftime(&now_win);
	next_work.tv_sec = (unsigned long)now_win.time;
	next_work.tv_usec = (long)now_win.millitm * 1000;
#else
	gettimeofday(&next_work,NULL);
#endif
	T_ADD(next_work,period);
	
//
// The infinite loop
//
		
	while(1)
	{
		bool cmd_received;
		
		try
		{
			if (sleep != 0)
				received = get_command(sleep);
			else
				received = LOCK_TIME_OUT;

			switch (received)
			{
			case LOCK_COMMAND:
				execute_cmd();
				cmd_received = true;
				break;
				
			case LOCK_TIME_OUT:
				one_more_lock();
				cmd_received = false;
				break;
			}
			
			compute_sleep_time(cmd_received);
		}
		catch (omni_thread_fatal &)
		{
			cerr << "OUPS !! A omni thread fatal exception !!!!!!!!" << endl;
#ifndef _TG_WINDOWS_
			time_t t = time(NULL);
			cerr << ctime(&t);
#endif
			cerr << "Trying to re-enter the main loop" << endl;
		}
	}
	
}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::get_command
// 
// description : 	This method wait on the shared monitor for a new
//			command to be sent to the polling thread. The
//			thread waits with a timeout. If the thread is
//			awaken due to the timeout, false is returned.
//			If the work list is empty, the thread waits for ever.
//
// argument : in : 	tout : The timeout in mS
//
// The method returns true if the thread has been awaken due to a new
// command sent by the main thread
//
//--------------------------------------------------------------------------

LockCmdType LockThread::get_command(DevLong tout)
{
	omni_mutex_lock sync(p_mon);
	LockCmdType ret;

//
// Wait on monitor
//

	if (shared_cmd.cmd_pending == false)
	{	
		if (locked_devices.empty() == true)
			p_mon.wait();
		else
		{
			if (tout != -1)
				p_mon.wait(tout);
		}
	}
	
//
// Test if it is a new command. If yes, copy its data locally
//

	if (shared_cmd.cmd_pending == true)
	{
		local_cmd = shared_cmd;
		ret = LOCK_COMMAND;
	}
	else
		ret = LOCK_TIME_OUT;

	return ret;
}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::execute_cmd
// 
// description : 	This method is called when a command has been received
//			It execute the command!
//
//--------------------------------------------------------------------------

static bool LockThread_pred(LockedDevice lock_dev,string d_name)
{
	return (lock_dev.dev_name == d_name);
}

void LockThread::execute_cmd()
{	
	vector<string>::iterator pos;
	bool need_exit = false;
	
	switch (local_cmd.cmd_code)
	{
	
//
// Add a new device
//

	case Tango::LOCK_ADD_DEV :
	{
		LockedDevice ld;
		ld.dev_name = local_cmd.dev_name;
		ld.validity = local_cmd.lock_validity;
		
		if (locked_devices.empty() == false)
		{
			vector<LockedDevice>::iterator pos;
			
			pos = find_if(locked_devices.begin(),locked_devices.end(),bind2nd(ptr_fun(LockThread_pred),local_cmd.dev_name));
			if (pos == locked_devices.end())
			{
				locked_devices.push_back(ld);
			}
		}
		else
			locked_devices.push_back(ld);
		update_th_period();
		break;
	}

//
// Remove an already locked device
//
		
	case Tango::LOCK_REM_DEV :
		if (locked_devices.empty() == false)
		{
			vector<LockedDevice>::iterator pos;
			
			pos = find_if(locked_devices.begin(),locked_devices.end(),bind2nd(ptr_fun(LockThread_pred),local_cmd.dev_name));
			if (pos != locked_devices.end())
			{
				locked_devices.erase(pos);
			}
			update_th_period();
		}
		break;

//
// Ask locking thread to unlock all its devices and to exit
//
		
	case Tango::LOCK_UNLOCK_ALL_EXIT :
		unlock_all_devs();
		need_exit = true;
		break;
		
//
// Ask locking thread to exit
//
		
	case Tango::LOCK_EXIT :
		need_exit = true;
		break;
	}
	
//
// Inform requesting thread that the work is done
//

	{
		omni_mutex_lock sync(p_mon);
		shared_cmd.cmd_pending = false;
		if (locked_devices.empty() == true)
			shared_cmd.suicide = true;
		p_mon.signal();
	}

//
// If the command was an exit one, do it now
//
	
	if (need_exit == true)
	{
		omni_thread::exit();
	}
	
//
// If it is the last device, ask thread to exit buut delete the
// proxy first
//
	
	if (locked_devices.empty() == true)
	{
		omni_thread::exit();
	}
}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::one_more_lock
// 
// description : 
//
// argument : in : 	
//
//--------------------------------------------------------------------------

void LockThread::one_more_lock()
{	
	DeviceData d_in;
	if (re_lock_cmd_args.empty() == true)
	{
		for (unsigned long loop = 0;loop < locked_devices.size();++loop)
			re_lock_cmd_args.push_back(locked_devices[loop].dev_name);
	}
	d_in << re_lock_cmd_args;
	
	try
	{
//		cout << "Locking Thread: I am re-locking devices (" << re_lock_cmd_args.size() << ")" << endl;
		admin_proxy->command_inout("ReLockDevices",d_in);
	}
	catch (Tango::DevFailed &e)
	{
//		cout << "LockThread: Gasp, an exception while sending command to admin device" << endl;

//
// If we had a locking error, retrieve device name and remove it from the locked devices list
//
		
		for (unsigned long loop = 0;loop < e.errors.length();loop++)
		{
			if ((::strcmp(e.errors[loop].reason.in(),"API_DeviceLocked") == 0) ||
			    (::strcmp(e.errors[loop].reason.in(),"API_DeviceNotLocked") == 0) ||
			    (::strcmp(e.errors[loop].reason.in(),"API_DeviceNotExported") == 0))
			{
				string error_message(e.errors[loop].desc.in());
				string::size_type pos = error_message.find(':');
				if (pos != string::npos)
				{
					string local_dev_name = error_message.substr(0,pos);
					vector<LockedDevice>::iterator ite;
					for (ite = locked_devices.begin();ite != locked_devices.end();++ite)
					{
						if (TG_strcasecmp(local_dev_name.c_str(),ite->dev_name.c_str()) == 0)
						{
							locked_devices.erase(ite);
							if (locked_devices.empty() == true)
							{
								local_cmd.cmd_code = LOCK_EXIT;
								execute_cmd();
							}
							break;
						}
					}
				}
			}
		}
	}
}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::unlock_all_devs
// 
// description : 
//
// argument : in : 	
//
//--------------------------------------------------------------------------

void LockThread::unlock_all_devs()
{	
	if (locked_devices.empty() == false)
	{
	  	string cmd("UnLockDevice");
	  	DeviceData din;
	  	DevVarLongStringArray sent_data;
	  	unsigned int locked_devices_nb = locked_devices.size();
	  	sent_data.svalue.length(locked_devices_nb);
	  	for (unsigned int loop = 0;loop < locked_devices_nb;loop++)
	  		sent_data.svalue[loop] = CORBA::string_dup(locked_devices[loop].dev_name.c_str());
	  	sent_data.lvalue.length(1);
	  	sent_data.lvalue[0] = 1;

	  	din << sent_data;
		
		try
		{
			admin_proxy->command_inout("UnLockDevice",din);
		}
		catch (Tango::DevFailed &) {}
	}

}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::update_th_period
// 
// description : 	This method manage the thread update period
//					The update period is the smallest lock validity of all the
//					locked device minus 0.5 sec
//
//--------------------------------------------------------------------------

void LockThread::update_th_period()
{
	
//
// Clear the ReLock command args vector
//
	
	re_lock_cmd_args.clear();
	
//
// Find the smallest lock validity
//

	vector<LockedDevice>::iterator ite;
	ite = min_element(locked_devices.begin(),locked_devices.end());
	
//
// Compute new thread period
//

	DevLong tmp_usec = (ite->validity * 1000000) - 500000;
	period = tmp_usec;
	period_ms = tmp_usec / 1000;
}

//+-------------------------------------------------------------------------
//
// method : 		LockThread::compute_sleep_time
// 
// description : 	This method computes how many mS the thread should 
//			sleep before the next lock time.
//
//--------------------------------------------------------------------------

void LockThread::compute_sleep_time(bool cmd)
{

	if (cmd == false)
	{
		sleep = period_ms;
#ifdef _TG_WINDOWS_
		struct _timeb now_win;

		_ftime(&now_win);
		next_work.tv_sec = (unsigned long)now_win.time;
		next_work.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&next_work,NULL);
#endif
		T_ADD(next_work,period);
	}
	else
	{
		struct timeval now;
		long diff;

#ifdef _TG_WINDOWS_
		struct _timeb now_win;

		_ftime(&now_win);
		now.tv_sec = (unsigned long)now_win.time;
		now.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&now,NULL);
#endif
		T_DIFF(now,next_work,diff);
		if (diff < 0)
			sleep = -1;
		else
			sleep = diff / 1000;
	}
}

} // End of Tango namespace

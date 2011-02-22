//=============================================================================
//
// file :               tango_monitor.h
//
// description :        Include file for the Tango_monitor class 
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
// Revision 3.7  2008/12/17 09:50:59  taurel
// - First implementation of attributes sent on the wire using IDL Union
// instead of IDL Any
//
// Revision 3.6  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.5  2008/10/03 06:53:09  taurel
// - Add some licensing info in each files
//
// Revision 3.4  2008/05/20 12:44:20  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.3.2.1  2007/12/19 15:54:48  taurel
// - Still some work going on for the locking feature
//
// Revision 3.3  2007/05/17 07:59:07  taurel
// - The polling is not configured via a separate thread any more. The polling thread add_obj_polling method has been modified to support a parameter telling to the polling thread when it has to polled the object.
// Add device name in monitor print message
// Add device_destroyer method in DeviceClass class
//
// Revision 3.2  2007/04/20 14:41:34  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.1  2005/01/21 19:58:30  taurel
// - Some changes in include files for gcc 3.4.2
//
//=============================================================================

#ifndef _TANGO_MONITOR_H
#define _TANGO_MONITOR_H


namespace Tango
{

//=============================================================================
//
//			The TangoMonitor class
//
//
// description : 	This class is used to synchronise device access between
//			polling thread and CORBA request. It is used only for
//			the command_inout and read_attribute calls	
//
//=============================================================================

class TangoMonitor: public omni_mutex
{
public :
	TangoMonitor(const char *na):_timeout(DEFAULT_TIMEOUT),cond(this),
			locking_thread(NULL),locked_ctr(0),name(na) {};
	TangoMonitor():_timeout(DEFAULT_TIMEOUT),cond(this),locking_thread(NULL),
			locked_ctr(0),name("unknown") {};
	~TangoMonitor() {};
		
	void get_monitor();	
	void rel_monitor();
		
	void timeout(long new_to) {_timeout = new_to;}
	long timeout() {return _timeout;}
	
	void wait() {cond.wait();}
	int wait(long);
	
	void signal() {cond.signal();}
	
	int get_locking_thread_id()
	{
	if (locking_thread != NULL)
		return locking_thread->id();
	else
		return 0;
	}
	
	long get_locking_ctr() {return locked_ctr;}
			
private :
	long 			_timeout;
	omni_condition 	cond;
	omni_thread		*locking_thread;
	long			locked_ctr;
	string 			name;
};



//+-------------------------------------------------------------------------
//
// method : 		TangoMonitor::get_monitor
// 
// description : 	Get a monitor. The thread will wait (with timeout) if 
//			the monitor is already locked. If the thread is already
//			the monitor owner thread, simply increment the
//			locking counter
//
//--------------------------------------------------------------------------

inline void TangoMonitor::get_monitor()
{
	int interupted;
	omni_thread *th = omni_thread::self();

	omni_mutex_lock synchronized(*this);
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	cout4 << "In get_monitor() " << name << ", thread = " << th->id() << ", ctr = " << locked_ctr << endl;
#endif

	if (locked_ctr == 0)
	{
		locking_thread = th;
	}
	else if (th != locking_thread)
	{
		while(locked_ctr > 0)
		{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
			cout4 << "Thread " << th->id() << ": waiting !!" << endl;
#endif

			interupted = wait(_timeout);
			if (interupted == false)
			{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)			
				cout4 << "TIME OUT for thread " << th->id() << endl;
#endif
				Except::throw_exception((const char *)"API_CommandTimedOut",
					        (const char *)"Not able to acquire serialization (dev, class or process) monitor",
					        (const char *)"TangoMonitor::get_monitor");
			}
		}
		locking_thread = th;
	}
	else
	{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)			
		cout4 << "owner_thread !!" << endl;
#endif
	}
	
	locked_ctr++;
}


//+-------------------------------------------------------------------------
//
// method : 		TangoMonitor::rel_monitor
// 
// description : 	Release a monitor if the caller thread is the locking
//			thread. Signal other threads if the locking counter
//			goes down to zero
//
//--------------------------------------------------------------------------

inline void TangoMonitor::rel_monitor()
{
	omni_thread *th = omni_thread::self();
	omni_mutex_lock synchronized(*this);

#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	cout4 << "In rel_monitor() " << name << ", ctr = " << locked_ctr << ", thread = " << th->id() << endl;
#endif
	if ((locked_ctr == 0) || (th != locking_thread))
		return;
		
	locked_ctr--;
	if (locked_ctr == 0)
	{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		cout4 << "Signalling !" << endl;
#endif
		locking_thread = NULL;
		cond.signal();
	}
}
	

} // End of Tango namespace
		     
#endif /* TANGO_MONITOR */

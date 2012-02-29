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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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

	int get_locking_thread_id();
	long get_locking_ctr();

private :
	long 			_timeout;
	omni_condition 	cond;
	omni_thread		*locking_thread;
	long			locked_ctr;
	string 			name;
};


//+-------------------------------------------------------------------------
//
// methods : 		TangoMonitor::get_locking_thread_id
//					TangoMonitor::get_locking_ctr
//
//--------------------------------------------------------------------------


inline int TangoMonitor::get_locking_thread_id()
{
	if (locking_thread != NULL)
		return locking_thread->id();
	else
		return 0;
}

inline long TangoMonitor::get_locking_ctr()
{
	omni_mutex_lock guard(*this);
	return locked_ctr;
}

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

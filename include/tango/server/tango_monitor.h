//===================================================================================================================
//
// file :               tango_monitor.h
//
// description :        Include file for the Tango_monitor class
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
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or
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
//===================================================================================================================

#ifndef _TANGO_MONITOR_H
#define _TANGO_MONITOR_H


namespace Tango
{

	inline uint64_t now_to_uid() {
		return std::chrono::duration_cast<chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
//--------------------------------------------------------------------------------------------------------------------
//
// class :
//		TangoMonitor
//
// description :
//		This class is used to synchronise device access between polling thread and CORBA request. It is used only for
//		the command_inout and read_attribute calls
//
//--------------------------------------------------------------------------------------------------------------------

class TangoMonitor: public omni_mutex
{
public :
	TangoMonitor(const char *na):_timeout(DEFAULT_TIMEOUT),cond(this),
			locking_thread_id(thread::id()),locked_ctr(0),name(na),uid_(now_to_uid()) {

		cout4 << "New TangoMonitor: uid=" << uid_ << endl;
	};

	TangoMonitor(): _timeout(DEFAULT_TIMEOUT), cond(this), locking_thread_id(thread::id()),
					locked_ctr(0), name("unknown"), uid_(now_to_uid()) {

		cout4 << "New TangoMonitor: uid=" << uid_ << endl;
	};
	~TangoMonitor() {
        cout4 << "Destructing TangoMonitor name=" << name << "; uid=" << uid_ << endl;
    };

	void get_monitor();
	void rel_monitor();

	void timeout(long new_to) {_timeout = new_to;}
	long timeout() {return _timeout;}

	void wait();
	int wait(long);

	void signal() {cond.broadcast();}

	thread::id get_locking_thread_id();
	long get_locking_ctr();
	string &get_name() {return name;}
	void set_name(const string &na) {name = na;}
    uint64_t get_uid() { return uid_;}
private :
	long 			_timeout;
	omni_condition 	cond;
	thread::id 		locking_thread_id;
	long			locked_ctr;
	string 			name;
	uint64_t 		uid_;
};


//--------------------------------------------------------------------------------------------------------------------
//
// methods :
//		TangoMonitor::get_locking_thread_id
//		TangoMonitor::get_locking_ctr
//
//-------------------------------------------------------------------------------------------------------------------


inline thread::id TangoMonitor::get_locking_thread_id()
{
	return locking_thread_id;
}

inline long TangoMonitor::get_locking_ctr()
{
	omni_mutex_lock guard(*this);
	return locked_ctr;
}

//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		TangoMonitor::get_monitor
//
// description :
//		Get a monitor. The thread will wait (with timeout) if the monitor is already locked. If the thread is already
//		the monitor owner thread, simply increment the locking counter
//
//--------------------------------------------------------------------------------------------------------------------

inline void TangoMonitor::get_monitor()
{
	omni_mutex_lock synchronized(*this);

#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	cout4 << "In get_monitor() " << name << ", thread = " << this_thread::get_id() << ", ctr = " << locked_ctr << endl;
#endif

	if (locked_ctr == 0)
	{
		locking_thread_id = this_thread::get_id();
	}
	else if (this_thread::get_id() != locking_thread_id)
	{
		while(locked_ctr > 0)
		{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
			cout4 << "Thread " << this_thread::get_id() << ": waiting !!" << endl;
#endif
            int interupted;

			interupted = wait(_timeout);
			if (interupted == false)
			{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
				cout4 << "TIME OUT for thread " << this_thread::get_id() << endl;
#endif
				Except::throw_exception((const char *)API_CommandTimedOut,
					        (const char *)"Not able to acquire serialization (dev, class or process) monitor",
					        (const char *)"TangoMonitor::get_monitor");
			}
		}
		locking_thread_id = this_thread::get_id();
	}
	else
	{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		cout4 << "owner_thread !!" << endl;
#endif
	}

	locked_ctr++;
}


//--------------------------------------------------------------------------------------------------------------------
//
// method :
//		TangoMonitor::rel_monitor
//
// description :
//		Release a monitor if the caller thread is the locking thread. Signal other threads if the locking counter
//		goes down to zero
//
//--------------------------------------------------------------------------------------------------------------------

inline void TangoMonitor::rel_monitor()
{
	omni_mutex_lock synchronized(*this);

#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	cout4 << "In rel_monitor() " << name << ", ctr = " << locked_ctr << ", thread = " << this_thread::get_id() << endl;
#endif
	if ((locked_ctr == 0) || (this_thread::get_id() != locking_thread_id))
		return;

	locked_ctr--;
	if (locked_ctr == 0)
	{
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		cout4 << "Signalling !" << endl;
#endif
		locking_thread_id = thread::id();
		cond.signal();
	}
}


} // End of Tango namespace

#endif /* TANGO_MONITOR */

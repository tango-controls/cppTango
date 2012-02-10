//=============================================================================
//
// file :               DServerSignal.h
//
// description :        Include for the DServer class. This class implements
//                      all the commands which are available for device
//			of the DServer class. There is one device of the
//			DServer class for each device server process
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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

#ifndef _DSERVERSIGNAL_H
#define _DSERVERSIGNAL_H

#include <tango.h>
#include <signal.h>

namespace Tango
{

//=============================================================================
//
//			The DServerSignal class
//
// description :	Class to implement all data members and commands for
//			signal management in a TANGO device server.
//
//=============================================================================

#if (defined (_TG_WINDOWS_) || (defined __darwin__) || (defined __freebsd__))
#define		_NSIG		NSIG
#endif


typedef struct
{
	vector <DeviceClass *>	registered_classes;
	vector <DeviceImpl *>	registered_devices;
	bool			own_handler;
}
DevSigAction;

class DServerSignal:public TangoMonitor
{
public :
	TANGO_IMP_EXP static DServerSignal *instance();
	~DServerSignal() {};

#if (defined __linux)
	void register_class_signal(long, bool, DeviceClass *);
	void register_dev_signal(long, bool, DeviceImpl *);

	void register_handler(long,bool);
	pid_t get_sig_thread_pid();
#else
	void register_class_signal(long, DeviceClass *);
	void register_dev_signal(long, DeviceImpl *);

	void register_handler(long);
#endif

	void unregister_class_signal(long, DeviceClass *);
	void unregister_class_signal(DeviceClass *);
	void unregister_dev_signal(long, DeviceImpl *);
	void unregister_dev_signal(DeviceImpl *);

	void unregister_handler(long);

	static void main_sig_handler(int);

	class ThSig: public omni_thread
	{
		DServerSignal *ds;
	public:
		ThSig(DServerSignal *d):ds(d),my_pid(0),th_data_created(false) {}
		virtual ~ThSig() {}

		TangoSys_Pid my_pid;
		bool th_data_created;
#ifndef _TG_WINDOWS_
		pthread_t my_thread;
#endif
		void *run_undetached(void *);
		void start() {start_undetached();}
	};
	friend class ThSig;
	ThSig *sig_th;

protected :
	DServerSignal();
	static DevSigAction		reg_sig[_NSIG];

	bool				sig_to_install;
	bool				sig_to_remove;
	int				inst_sig;
	int 				rem_sig;
#ifdef _TG_WINDOWS_
	static HANDLE			win_ev;
	static int			win_signo;
#endif

private:
	static DServerSignal *_instance;
	vector<DeviceImpl *>::iterator find_device(long, DeviceImpl *);
	vector<DeviceImpl *>::iterator find_delayed_device(long, DeviceImpl *);

	vector<DeviceClass *>::iterator find_class(long, DeviceClass *);
	vector<DeviceClass *>::iterator find_delayed_class(long, DeviceClass *);

#ifdef _TG_WINDOWS_
	static inline bool auto_signal(long s)
	{
		if ((s==SIGINT) || (s==SIGTERM) || (s==SIGABRT) || (s==SIGBREAK))
			return true;
		else
			return false;
	}
#else
	static inline bool auto_signal(long s)
	{
		if ((s==SIGQUIT) || (s==SIGINT) || (s==SIGHUP) || (s==SIGTERM))
			return true;
		else
			return false;
	}
#endif

#if (defined __linux || defined __darwin__ || defined __freebsd__)
	static inline bool auth_signal(long s)
	{
		if ((s==SIGUSR1) || (s==SIGUSR2))
			return false;
		else
			return true;
	}
#endif

#ifdef _TG_WINDOWS_
	static inline bool auth_signal(long s) {return true;}
#endif

	static string 			sig_name[_NSIG];

};

} // End of namespace Tango


#endif

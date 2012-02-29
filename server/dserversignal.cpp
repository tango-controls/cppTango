static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               DServerSignal.cpp
//
// description :        C++ source for the DServer class and its commands.
//			The class is derived from Device. It represents the
//			CORBA servant object which will be accessed from the
//			network. All commands which can be executed on a
//			DServer object are implemented in this file.
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <new>
#include <dserversignal.h>

#ifndef _TG_WINDOWS_
extern int errno;
#endif

namespace Tango
{

DServerSignal *DServerSignal::_instance = NULL;
DevSigAction DServerSignal::reg_sig[_NSIG];
string DServerSignal::sig_name[_NSIG];
#ifdef _TG_WINDOWS_
HANDLE DServerSignal::win_ev = NULL;
int DServerSignal::win_signo = 0;
#endif


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::Instance()
//
// description : 	Instance method for DServerSignal object. This class is
//			a singleton and this method creates the object the
//			first time it is called or simply returns a pointer
//			to the already created object for all the other calls.
//
//-----------------------------------------------------------------------------

DServerSignal *DServerSignal::instance()
{
	if (_instance == NULL)
	{
		try
		{
			_instance = new DServerSignal();
		}
		catch (bad_alloc)
		{
			throw;
		}
	}
	return _instance;
}


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::DServerSignal()
//
// description : 	constructor for DServerSignal object. As this class is
//			a singleton, this method is protected
//
//-----------------------------------------------------------------------------

DServerSignal::DServerSignal():TangoMonitor("signal")
{
	cout4 << "Entering DServerSignal constructor" << endl;

//
// Set array of signal name
//

#ifdef _TG_WINDOWS_
	sig_name[SIGINT] = "SIGINT";
	sig_name[SIGILL] = "SIGILL";
	sig_name[SIGFPE] = "SIGFPE";
	sig_name[SIGSEGV] = "SIGSEGV";
	sig_name[SIGTERM] = "SIGTERM";
	sig_name[SIGBREAK] = "SIGBREAK";
	sig_name[SIGABRT] = "SIGABRT";
#else
	sig_name[SIGHUP] = "SIGHUP";
	sig_name[SIGINT] = "SIGINT";
	sig_name[SIGQUIT] = "SIGQUIT";
	sig_name[SIGILL] = "SIGILL";
	sig_name[SIGTRAP] = "SIGTRAP";
	sig_name[SIGABRT] = "SIGABRT";
	sig_name[SIGFPE] = "SIGFPE";
	sig_name[SIGKILL] = "SIGKILL";
	sig_name[SIGBUS] = "SIGBUS";
	sig_name[SIGSEGV] = "SIGSEGV";
	sig_name[SIGPIPE] = "SIGPIPE";
	sig_name[SIGALRM] = "SIGALRM";
	sig_name[SIGTERM] = "SIGTERM";
	sig_name[SIGUSR1] = "SIGUSR1";
	sig_name[SIGUSR2] = "SIGUSR2";
	sig_name[SIGCHLD] = "SIGCHLD";
	sig_name[SIGVTALRM] = "SIGVTALRM";
	sig_name[SIGPROF] = "SIGPROF";
	sig_name[SIGIO] = "SIGIO";
	sig_name[SIGWINCH] = "SIGWINCH";
	sig_name[SIGSTOP] = "SIGSTOP";
	sig_name[SIGTSTP] = "SIGTSTP";
	sig_name[SIGCONT] = "SIGCONT";
	sig_name[SIGTTIN] = "SIGTTIN";
	sig_name[SIGTTOU] = "SIGTTOU";
	sig_name[SIGURG]  = "SIGURG";
	sig_name[SIGSYS]  = "SIGSYS";
#ifdef linux
	sig_name[SIGXCPU] = "SIGXCPU";
	sig_name[SIGXFSZ] = "SIGXFSZ";
	sig_name[SIGCLD]  = "SIGCLD";
	sig_name[SIGPWR]  = "SIGPWR";
#else
#ifdef __darwin__
	sig_name[SIGEMT]  = "SIGEMT";
	sig_name[SIGINFO] = "SIGINFO";
#else
#ifdef __freebsd__
	sig_name[SIGXCPU] = "SIGXCPU";
	sig_name[SIGXFSZ] = "SIGXFSZ";
#endif /* __freebsd__ */
#endif /* __darwin__ */
#endif /* linux */
#endif /* _TG_WINDOWS_ */

	TangoSys_OMemStream o;
	for (long i = 0;i < _NSIG;i++)
	{
		if (sig_name[i].size() == 0)
		{
			o << i << ends;
			sig_name[i] = o.str();
			o.seekp(0);
			o.clear();
		}
	}

	sig_to_install = false;
	sig_to_remove = false;

#ifndef _TG_WINDOWS_
//
// With Solaris/Linux, the SIGINT and SIGQUIT default actions are set to SIG_IGN for
// all processes started in the background (POSIX requirement). Signal SIGINT is used by
// Tango in its signal management, reset the default action to default
//

	struct sigaction sa;

	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT,&sa,NULL) == -1)
		cerr << "DServerSignal::DServerSignal --> Can't reset default action for SIGINT to SIG_DFL" << endl;

	if (sigaction(SIGQUIT,&sa,NULL) == -1)
		cerr << "DServerSignal::DServerSignal --> Can't reset default action for SIGQUIT to SIG_DFL" << endl;

//
// Block signals in thread other than the thread dedicated to signal
//

	sigset_t sigs_to_block;
	sigemptyset(&sigs_to_block);

	sigfillset(&sigs_to_block);

	sigdelset(&sigs_to_block,SIGABRT);
	sigdelset(&sigs_to_block,SIGKILL);
	sigdelset(&sigs_to_block,SIGILL);
	sigdelset(&sigs_to_block,SIGTRAP);
	sigdelset(&sigs_to_block,SIGIOT);
	sigdelset(&sigs_to_block,SIGFPE);
	sigdelset(&sigs_to_block,SIGBUS);
	sigdelset(&sigs_to_block,SIGSEGV);
	sigdelset(&sigs_to_block,SIGSYS);
	sigdelset(&sigs_to_block,SIGPIPE);
	sigdelset(&sigs_to_block,SIGSTOP);

	sigdelset(&sigs_to_block,SIGTSTP);
#if (defined __linux)
	sigdelset(&sigs_to_block,SIGUSR1);
	sigdelset(&sigs_to_block,SIGUSR2);
#endif
	sigprocmask(SIG_BLOCK,&sigs_to_block,NULL);
#else /* _TG_WINDOWS_ */
	win_ev = CreateEvent(NULL,FALSE,FALSE,NULL);

	register_handler(SIGINT);
	register_handler(SIGTERM);
	register_handler(SIGABRT);
	if (Util::_service == false)
		register_handler(SIGBREAK);
#endif /* _TG_WINDOWS_ */

//
// Start the thread dedicated to signal
//

	sig_th = new ThSig(this);
	sig_th->start();

	cout4 << "leaving DServerSignal constructor" << endl;
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::register_class_signal()
//
// description : 	method to register a signal handler at the class
//			level
//
// in :			long signo - Signal number
//			DeviceClass *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

#if !(defined __linux)
void DServerSignal::register_class_signal(long signo,DeviceClass *cl_ptr)
#else
void DServerSignal::register_class_signal(long signo,bool handler,DeviceClass *cl_ptr)
#endif
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}

	if (auth_signal(signo) == false)
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized with your OS" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}

#if (defined __linux)
	if ((auto_signal(signo) == true) && (handler == true))
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized using your own handler" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}
#endif

//
// If nothing is registered for this signal, install the OS signal handler
//

	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_devices.empty() == true) &&
		    (reg_sig[signo].registered_classes.empty() == true))
		{
#if !(defined __linux)
	    		register_handler(signo);
#else
			register_handler(signo,handler);
#endif
		}
	}

//
// Check if class is already registered for this signal. If it is already done,
// leave method. Otherwise, record class pointer
//

	vector<DeviceClass *>::iterator f = find_class(signo,cl_ptr);

	if (f == reg_sig[signo].registered_classes.end())
	{
		reg_sig[signo].registered_classes.push_back(cl_ptr);
#if (defined __linux)
		reg_sig[signo].own_handler = handler;
#endif
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::find_class
//
// description : 	method to check if a class is already registered for a
//			signal. If it is true, this method returns in which
//			element of the vector the class is registered
//
// in :			long signo - Signal number
//			DeviceClass *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

vector<DeviceClass *>::iterator DServerSignal::find_class(long signo,DeviceClass *cl_ptr)
{
	vector<DeviceClass *>::iterator p;
	for (p = reg_sig[signo].registered_classes.begin();p < reg_sig[signo].registered_classes.end();p++)
	{
		if ((*p) == cl_ptr)
			break;
	}
	return p;
}


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::register_dev_signal()
//
// description : 	method to register a signal handler at the device
//			level
//
// in :			long signo - Signal number
//			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

#if !(defined __linux)
void DServerSignal::register_dev_signal(long signo,DeviceImpl *dev_ptr)
#else
void DServerSignal::register_dev_signal(long signo,bool handler,DeviceImpl *dev_ptr)
#endif
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_dev_signal");
	}

	if (auth_signal(signo) == false)
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized with your OS" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_dev_signal");
	}

#if (defined __linux)
	if ((auto_signal(signo) == true) && (handler == true))
	{
		TangoSys_OMemStream o;
		o << "Signal " << sig_name[signo] << "is not authorized using your own handler" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				        o.str(),
				        (const char *)"DServerSignal::register_class_signal");
	}
#endif

//
// If nothing is registered for this signal, install the OS signal handler
//

	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_devices.empty() == true) &&
		    (reg_sig[signo].registered_classes.empty() == true))
		{
#if !(defined __linux)
            register_handler(signo);
#else
			register_handler(signo,handler);
#endif
		}
	}

//
// Check if devices is already registered for this signal. If it is already done,
// leave method. Otherwise, record class pointer
//


	vector<DeviceImpl *>::iterator f = find_device(signo,dev_ptr);

	if (f == reg_sig[signo].registered_devices.end())
	{
		reg_sig[signo].registered_devices.push_back(dev_ptr);
#if (defined __linux)
		reg_sig[signo].own_handler = handler;
#endif
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::find_device
//
// description : 	method to check if a device is already registered for a
//			signal. If it is true, this method returns in which
//			element of the vector the device is registered
//
// in :			long signo - Signal number
//			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

vector<DeviceImpl *>::iterator DServerSignal::find_device(long signo,DeviceImpl *dev_ptr)
{
	vector<DeviceImpl *>::iterator p;
	for (p = reg_sig[signo].registered_devices.begin();p < reg_sig[signo].registered_devices.end();p++)
	{
		if ((*p) == dev_ptr)
			break;
	}
	return p;
}


//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_class_signal()
//
// description : 	method to unregister a signal handler at the class
//			level
//
// in :			long signo - Signal number
//			DeviceClass *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_class_signal(long signo,DeviceClass *cl_ptr)
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_proc_signal");
	}

//
// Check if class is already registered for this signal. If it is already done,
// leave method. Otherwise, record class pointer
//

	vector<DeviceClass *>::iterator f = find_class(signo,cl_ptr);

	if (f == reg_sig[signo].registered_classes.end())
			return;
	else
		reg_sig[signo].registered_classes.erase(f);

//
// If nothing is registered for this signal, unregister the OS signal handler
// and (eventually) the event handler
//

	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_classes.empty() == true) &&
	    	    (reg_sig[signo].registered_devices.empty() == true))
	    		unregister_handler(signo);
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_dev_signal()
//
// description : 	method to unregister a signal handler at the class
//			level
//
// in :			long signo - Signal number
//			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_dev_signal(long signo,DeviceImpl *dev_ptr)
{

//
// Check signal validity
//

	if ((signo < 1) || (signo >= _NSIG))
	{
		TangoSys_OMemStream o;
		o << "Signal number " << signo << " out of range" << ends;
		Except::throw_exception((const char *)"API_SignalOutOfRange",
				      o.str(),
				      (const char *)"DServerSignal::register_proc_signal");
	}

//
// Check if device is already registered for this signal. If yes, remove it.
// Otherwise, leave method
//


	vector<DeviceImpl *>::iterator f = find_device(signo,dev_ptr);

	if (f == reg_sig[signo].registered_devices.end())
		return;
	else
		reg_sig[signo].registered_devices.erase(f);

//
// If nothing is registered for this signal, unregister the OS signal handler
// and eventually the event handler
//

	if (auto_signal(signo) == false)
	{
		if ((reg_sig[signo].registered_classes.empty() == true) &&
	    	    (reg_sig[signo].registered_devices.empty() == true))
		{
	    		unregister_handler(signo);
		}
	}

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_dev_signal()
//
// description : 	method to unregister a signal handler at the device
//			level for all signals
//
// in :			DeviceImpl *dev_ptr - Pointer to device object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_dev_signal(DeviceImpl *dev_ptr)
{
	long i;

	for (i = 0;i < _NSIG;i++)
	{

//
// Check if device is registered for this signal. If yes, remove it.
// Otherwise, go to next signal
//

		vector<DeviceImpl *>::iterator f = find_device(i,dev_ptr);

		if (f == reg_sig[i].registered_devices.end())
			continue;
		else
			reg_sig[i].registered_devices.erase(f);

//
// If nothing is registered for this signal, unregister the OS signal handler
//

		if (auto_signal(i) == false)
		{
			if ((reg_sig[i].registered_classes.empty() == true) &&
	    	    	    (reg_sig[i].registered_devices.empty() == true))
	    			unregister_handler(i);
		}
	}

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_class_signal()
//
// description : 	method to unregister a signal handler at the class
//			level for all signals
//
// in :			DeviceImpl *cl_ptr - Pointer to device class object
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_class_signal(DeviceClass *cl_ptr)
{
	long i;

	for (i = 0;i < _NSIG;i++)
	{

//
// Check if classes is registered for this signal. If yes, remove it.
// Otherwise, go to next signal
//

		vector<DeviceClass *>::iterator f = find_class(i,cl_ptr);

		if (f == reg_sig[i].registered_classes.end())
			continue;
		else
			reg_sig[i].registered_classes.erase(f);

//
// If nothing is registered for this signal, unregister the OS signal handler
//

		if (auto_signal(i) == false)
		{
			if ((reg_sig[i].registered_classes.empty() == true) &&
	    	    	    (reg_sig[i].registered_devices.empty() == true))
	    			unregister_handler(i);
		}
	}

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::register_handler()
//
// description : 	method to register in the OS the main signal handler
//			for a given signal
//
// in :			long signo - Signal number
//
//-----------------------------------------------------------------------------

#if !(defined __linux)
void DServerSignal::register_handler(long signo)
#else
void DServerSignal::register_handler(long signo,bool handler)
#endif
{
	cout4 << "Installing OS signal handler for signal " << sig_name[signo] << endl;

#ifdef _TG_WINDOWS_
	if (::signal(signo, DServerSignal::main_sig_handler) == SIG_ERR)
	{
		TangoSys_OMemStream o;
		o << "Can't install signal " << signo << ". OS error = " << errno << ends;
		Except::throw_exception((const char *)"API_CantInstallSignal",
				      o.str(),
				      (const char *)"DServerSignal::register_handler");
	}
#else
	#if (defined __linux)
	if (handler == true)
	{
		sigset_t sigs_to_unblock;
		sigemptyset(&sigs_to_unblock);
		sigaddset(&sigs_to_unblock,signo);

		if (pthread_sigmask(SIG_UNBLOCK,&sigs_to_unblock,NULL) != 0)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				     		o.str(),
				      		(const char *)"DServerSignal::register_handler");
		}

		struct sigaction sa;

		sa.sa_flags = SA_RESTART;
		sa.sa_handler = DServerSignal::main_sig_handler;
		sigemptyset(&sa.sa_mask);

		if (sigaction((int)signo,&sa,0) == -1)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				     		o.str(),
				      		(const char *)"DServerSignal::register_handler");
		}
	}
	else
	{
	#endif
	{
		omni_mutex_lock sy(*this);

		while(sig_to_install == true)
		{
			wait();
		}
		sig_to_install = true;
		inst_sig = signo;
	}

	pthread_kill(sig_th->my_thread,SIGINT);
	#if (defined __linux)
	}
	#endif
#endif

}

//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::unregister_handler()
//
// description : 	method to unregister from the OS the main signal handler
//			for a given signal
//
// in :			long signo - Signal number
//
//-----------------------------------------------------------------------------

void DServerSignal::unregister_handler(long signo)
{
	cout4 << "Removing OS signal handler for signal " << sig_name[signo] << endl;


#ifdef _TG_WINDOWS_
	if (::signal(signo, SIG_DFL) == SIG_ERR)
	{
		TangoSys_OMemStream o;
		o << "Can't install signal " << signo << ". OS error = " << errno << ends;
		Except::throw_exception((const char *)"API_CantInstallSignal",
				      o.str(),
				      (const char *)"DServerSignal::register_handler");
	}
#else
	#if (defined __linux)
	if (reg_sig[signo].own_handler == true)
	{
		struct sigaction sa;

		sa.sa_flags = 0;
		sa.sa_handler = SIG_DFL;
		sigemptyset(&sa.sa_mask);

		if (sigaction((int)signo,&sa,0) == -1)
		{
			TangoSys_OMemStream o;
			o << "Can't install signal " << signo << ". OS error = " << errno << ends;
			Except::throw_exception((const char *)"API_CantInstallSignal",
				      		o.str(),
				     		 (const char *)"DServerSignal::register_handler");
		}
	}
	else
	{
	#endif
	{
		omni_mutex_lock sy(*this);

		while(sig_to_remove == true)
		{
			wait();
		}
		sig_to_remove = true;
		rem_sig = signo;
	}
	pthread_kill(sig_th->my_thread,SIGINT);

	#if (defined __linux)
	}
	#endif
#endif
}

#if (defined __linux)
pid_t DServerSignal::get_sig_thread_pid()
{
	omni_mutex_lock syn(*this);

	if (sig_th->my_pid == 0)
	{
		wait(1000);
	}
	return sig_th->my_pid;
}
#endif
//+----------------------------------------------------------------------------
//
// method : 		DServerSignal::main_sig_handler()
//
// description : 	This is a dummy signal handler used only with solaris
//			which needs one for signal with a default ignore
//			action to work correctly with the sigwait()
//			call.
//
// in :			int signo - Signal number
//
//-----------------------------------------------------------------------------

#ifndef _TG_WINDOWS_
	#if (defined __linux)
void DServerSignal::main_sig_handler(int signo)
{
	cout4 << "In main sig_handler !!!!" << endl;

	DevSigAction *act_ptr = &(DServerSignal::reg_sig[signo]);

//
// First, execute all the handlers installed at the class level
//

	if (act_ptr->registered_classes.empty() == false)
	{
		long nb_class = act_ptr->registered_classes.size();
		for (long j = 0;j < nb_class;j++)
		{
			act_ptr->registered_classes[j]->signal_handler((long)signo);
		}
	}

//
// Then, execute all the handlers installed at the device level
//

	if (act_ptr->registered_devices.empty() == false)
	{
		long nb_dev = act_ptr->registered_devices.size();
		for (long j = 0;j < nb_dev;j++)
		{
			act_ptr->registered_devices[j]->signal_handler((long)signo);
		}
	}
}
	#else
void DServerSignal::main_sig_handler(int signo)
{
	cout4 << "In main sig_handler !!!!" << endl;
}
	#endif
#else
void DServerSignal::main_sig_handler(int signo)
{
	cout4 << "In main sig_handler !!!!" << endl;

	win_signo = signo;
	SetEvent(win_ev);
}
#endif

} // End of Tango namespace

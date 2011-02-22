static const char *RcsId = "$Header$";

//+=============================================================================
//
// file :               thsig.cpp
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
// $Revision$
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

//
// For HP, sigwait is only defined as cma_sigwait and its declaration is
// in a file included only if _CMA_NOWRAPPERS_ is not defined
//

#ifdef __hpux
#undef _CMA_NOWRAPPERS_
#include <pthread.h>
#define _CMA_NOWRAPPERS_
#endif

#include <tango.h>
#include <dserversignal.h>

namespace Tango
{

void DServerSignal::ThSig::run()
{

#ifndef WIN32
	sigset_t sigs_to_catch;

//
// Catch main signals
//
	
	sigemptyset(&sigs_to_catch);
	
	sigaddset(&sigs_to_catch,SIGINT);
	sigaddset(&sigs_to_catch,SIGTERM);			
	sigaddset(&sigs_to_catch,SIGHUP);		       
	sigaddset(&sigs_to_catch,SIGQUIT); 

//
// Init thread id and pid (for linux !!)
//
	
	my_thread = pthread_self();
	{
		JTCSynchronized syn(*ds);
		my_pid = getpid();
		ds->notify();
	}
#endif
	int signo;
	
//
// The infinite loop
//
	
	while(1)
	{
#ifndef WIN32
	#ifdef __hpux
		signo = sigwait(&sigs_to_catch);
		if (signo == -1)
			continue;
	#else
		sigwait(&sigs_to_catch,&signo);
	#endif
		cout4 << "Signal thread awaken for signal " << signo << endl;

		if (signo == SIGHUP)
			continue;
#else
		WaitForSingleObject(ds->win_ev,INFINITE);		
		signo = ds->win_signo;
		
		cout4 << "Signal thread awaken for signal " << signo << endl;
#endif		


#ifndef WIN32
			
//
// Add a new signal to catch in the mask
//

		{
			JTCSynchronized sy(*ds);		
			if ((signo == SIGINT) && (ds->sig_to_install == true))
			{
				ds->sig_to_install = false;

	#ifdef __hpux				
				sigset_t sigs_to_block;
				sigemptyset(&sigs_to_block);
				sigaddset(&sigs_to_block,ds->inst_sig);
				sigprocmask(SIG_BLOCK,&sigs_to_block,NULL);
	#endif
				
				sigaddset(&sigs_to_catch,ds->inst_sig);
				cout4 << "signal " << ds->inst_sig << " installed" << endl;
				ds->notify();
				continue;
			}

//
// Remove a signal from the cathed one
//

			if ((signo == SIGINT) && (ds->sig_to_remove == true))
			{
				ds->sig_to_remove = false;

	#ifdef __hpux				
				sigset_t sigs_to_block;
				sigemptyset(&sigs_to_block);
				sigaddset(&sigs_to_block,ds->rem_sig);
				sigprocmask(SIG_UNBLOCK,&sigs_to_block,NULL);
	#endif
				
				sigdelset(&sigs_to_catch,ds->rem_sig);
				cout4 << "signal " << ds->rem_sig << " removed" << endl;
				ds->notify();
				continue;
			}
		}
#endif
						
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
		
//
// For the automatically installed signal, unregister servers from database,
// destroy the ORB and exit
//
	
		if (auto_signal(signo) == true)
		{
			Tango::Util *tg = Tango::Util::instance();
			if (tg != NULL)
			{
				try
				{
      	   				tg->unregister_server();
					CORBA::ORB_ptr orb = tg->get_orb();
					orb->shutdown(false);
					CORBA::release(orb);
				}
				catch(...)
				{
#ifndef WIN32
					raise(SIGKILL);
#endif
				}
			}
#ifdef __linux
			pthread_exit((void *)1);
#else
			exit(1);
#endif
		}
	
	}
}

		
} // End of Tango namespace	

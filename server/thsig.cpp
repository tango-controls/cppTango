static const char *RcsId = "$Id$\n$Name$";

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

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <dserversignal.h>

namespace Tango
{

void *DServerSignal::ThSig::run_undetached(void *ptr)
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
		omni_mutex_lock syn(*ds);
		my_pid = getpid();
		ds->signal();
	}
#endif
	int signo;
	
//
// The infinite loop
//
	
	while(1)
	{
#ifndef WIN32
		sigwait(&sigs_to_catch,&signo);
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
			omni_mutex_lock sy(*ds);		
			if ((signo == SIGINT) && (ds->sig_to_install == true))
			{
				ds->sig_to_install = false;			
				sigaddset(&sigs_to_catch,ds->inst_sig);
				cout4 << "signal " << ds->inst_sig << " installed" << endl;
				ds->signal();
				continue;
			}

//
// Remove a signal from the cathed one
//

			if ((signo == SIGINT) && (ds->sig_to_remove == true))
			{
				ds->sig_to_remove = false;			
				sigdelset(&sigs_to_catch,ds->rem_sig);
				cout4 << "signal " << ds->rem_sig << " removed" << endl;
				ds->signal();
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
					orb->shutdown(true);
					CORBA::release(orb);
#ifdef TANGO_HAS_LOG4TANGO
	  				Logging::cleanup();
#endif
				}
				catch(...)
				{
#ifndef WIN32
					raise(SIGKILL);
#endif
				}
			}
#ifdef __linux
			raise(SIGKILL);
#else
			::exit(1);
#endif
		}
	
	}
	
	return NULL;
}

		
} // End of Tango namespace	

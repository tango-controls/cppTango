static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               cbthread.cpp
//
// description :        C++ source code for the CallBackThread class.
//			This class is used in automatic callback mode.
//			The thread mainly waits on aa asynchronous callback
//			request and will call the get_asynch_replies() call
//			to fire the callback when the answer is received
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/03/25 16:30:47  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 1.1  2003/03/20 08:56:12  taurel
// New file to support asynchronous calls
//
//
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <cbthread.h>

namespace Tango
{


//+-------------------------------------------------------------------------
//
// method : 		CallBackThread::CallBackThread
// 
// description : 	Two constructors for the PollObj class. The first one
//			constructs a PollObji nstance with the default polling
//			ring depth
//			The second one create a PollObj instance with a 
//			specified polling ring depth
//
// argument : in : 	
//
//--------------------------------------------------------------------------


void *CallBackThread::run_undetached(void *ptr)
{
	while(shared_cmd.is_stopped() == false)
	{
		try
		{
//			sleep(2);
//			cout << "In the automatic callback thread" << endl;
			
			{
				omni_mutex_lock sync(*asyn_ptr);
				if (asyn_ptr->get_cb_request_nb_i() == 0)
				{
					asyn_ptr->wait();
				}
			}
			
			if (asyn_ptr->get_cb_request_nb() != 0)
				ApiUtil::instance()->get_asynch_replies();
		}
		catch (omni_thread_fatal &)
		{
			cerr << "OUPS !! A omni thread fatal exception !!!!!!!!" << endl;
#ifndef WIN32
			time_t t = time(NULL);
			cerr << ctime(&t);
#endif
			cerr << "Trying to re-enter the main loop" << endl;
		}
	}
	
	omni_thread::exit();

	return NULL;
	
}


} // End of Tango namespace

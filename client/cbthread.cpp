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


void *CallBackThread::run_undetached(TANGO_UNUSED(void *ptr))
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
				ApiUtil::instance()->get_asynch_replies(0);
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

	omni_thread::exit();

	return NULL;

}


} // End of Tango namespace

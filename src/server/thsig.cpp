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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
#include <tango/server/dserversignal.h>

namespace Tango
{

    extern thread_local std::shared_ptr<PyData> kPerThreadPyData;

void DServerSignal::ThSig::run()
{

#ifndef _TG_WINDOWS_

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

	int signo = 0;

//
// The infinite loop
//

	while(1)
	{
#ifndef _TG_WINDOWS_
		int ret = sigwait(&ds->sigs_to_catch,&signo);
		// sigwait() under linux might return an errno number without initialising the
		// signo variable. Do a ckeck here to avoid problems!!!
	   if ( ret != 0 )
	   {
		   cout4 << "Signal thread awaken on error " << ret << endl;
		   continue;
	   }

	   cout4 << "Signal thread awaken for signal " << signo << endl;

	   if (signo == SIGHUP)
		   continue;
#else
	   WaitForSingleObject(ds->win_ev,INFINITE);
	   signo = ds->win_signo;

	   cout4 << "Signal thread awaken for signal " << signo << endl;
#endif

//
// Create the per thread data if not already done
//

		if (th_data_created == false)
		{
			kPerThreadPyData.reset(new PyData());
			th_data_created = true;
		}

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
					tg->shutdown_ds();
				}
				catch(...)
				{
#ifndef _TG_WINDOWS_
					raise(SIGKILL);
#endif
				}
			}
		}

	}
}


} // End of Tango namespace

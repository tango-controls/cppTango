static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               PollThread.cpp
//
// description :        C++ source code for the PollThread class.
//			This class is used for the polling thread. The rule of 
//			this thread is to regulary exceute command on device or
//			read attribute and store result in a ring buffer.
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
// Revision 3.26  2009/02/16 09:30:37  jensmeyer
// Modified print_list() method to handle sub device diagnostics.
//
// Revision 3.25  2009/02/03 15:17:11  jensmeyer
// Added the storage of sub device properties on a regular basis of
// 30 minutes to the heartbeat thread of a device server.
//
// Revision 3.24  2009/01/21 12:47:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.23  2009/01/08 14:58:03  taurel
// - The read_attribute_4 also transfer the client authentification
//
// Revision 3.22  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.21  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.20  2008/10/02 09:09:47  taurel
// - First implementation of multiple polling thread(s)
//
// Revision 3.19  2008/07/01 07:38:40  taurel
// - Some more code for a proper implementation of the DevEncoded data type with the new IDL release 4
//
// Revision 3.18  2008/05/20 12:44:12  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.17.2.1  2008/05/20 06:17:46  taurel
// - Last commit before merge with trunk
// (start the implementation of the new DevEncoded data type)
//
// Revision 3.17  2007/05/15 07:46:59  taurel
// - The polling thread is not configured by a separate thread any more.
// The Add_obj_polling command now support a delta_t to start the first polling
//
// Revision 3.16  2007/04/20 14:41:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.15  2007/04/16 14:57:42  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.14  2007/03/29 07:09:25  taurel
// - Change some data types for 64 bits compatibility
//
// Revision 3.13  2007/03/02 09:44:03  jensmeyer
// Corrected the use of detect_and_push event methods. Uses now the _3 methods
// for idl_vers>3 everywhere.
//
// Revision 3.12  2006/05/18 08:51:56  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.11  2006/04/13 13:31:44  taurel
// Several changes:
// - A new DeviceImpl::push_event() method to push USER event with State as data
// - Fix bug in polling thread POLL_IUPD_PERIOD. The new polling period was applied only after the following poll still done with the old polling period
// - It is now possible to update polling period of polled objects from a polled attribute or command
//
// Revision 3.10  2006/01/27 14:27:10  taurel
// - Fix a severe incompatibility problem introduced by all the modifs done for
// PY DS
// - Duplicate some EventSupplier class methods (instead of using template) in order to be able to generate Tango shared library on Suse 9.3
//
// Revision 3.9  2006/01/20 08:22:29  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 3.8  2005/07/20 14:40:47  taurel
// - Fix bug in polling thread out of sync error. Time threshold to generate
// event heartbeat error changed from 10 to 11. Fix some comments
//
// Revision 3.7  2005/06/29 08:31:19  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.6  2005/01/13 08:30:00  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.5.2.8  2004/12/06 14:39:29  taurel
// - Polling starts in a separate thread
// - For Windows : Polling thread cmd/attr measurement used PerformanceCounter
// - Fix bug in pollext.h in methods to externally fill polling buffer
//
// Revision 3.5.2.7  2004/11/09 09:57:49  taurel
// - Fix one memory leak and some minor changes
// - Force database file writing when server exit
// - Some minor changes for the -file option for WIN32
//
// Revision 3.5.2.6  2004/11/04 09:46:39  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.5.2.5  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.5.2.4  2004/10/22 11:26:33  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.5.2.3  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.5.2.2  2004/08/26 07:34:45  taurel
// - Implement a way to directly fills command or attribute polling buffer
//
// Revision 3.5.2.1  2004/08/19 07:44:59  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.5  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.4  2003/08/21 07:24:37  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.3  2003/07/03 07:40:51  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.2.2.7  2004/04/23 09:24:55  taurel
// - Just a change in comment
//
// Revision 3.2.2.6  2004/03/09 16:36:37  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.2.2.5  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.2.2.4  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.2.2.3  2003/12/10 16:08:56  taurel
// Last commit before merging with the event branch.
// Revision 3.0.2.4  2003/11/16 22:10:43  andy_gotz
// New version which defines 4 types of events - change, quality, periodic and
// archive. Code has been factorised to reduce redundancy. Minimum and maximum
// changes are supported. Event period is taken into account. Relative and
// absolute changes are detected. Whole sequence is taken into account when
// determining change.
//
// Revision 3.2.2.2  2003/10/03 13:34:26  taurel
// - Fix bug for device server started without database, with device name specified on command line using mix of upper and lower cases
// - It's now possible to send a command to the polling thread from itself
// - Add a release lock if the insert into the polling buffer failed
//
// Revision 3.2.2.1  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.2  2003/05/28 14:55:10  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.1  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
// Revision 3.0.2.3  2003/04/15 19:01:55  andy_gotz
// added heartbeat on client and server side; cleaned up cout's
//
// Revision 3.0.2.2  2003/04/13 22:12:17  andy_gotz
// added heartbeat; polling starts automatically on subscription
//
// Revision 3.0.2.1  2003/04/08 13:12:52  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:44:10  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventsupplier.h>
#include <math.h>

#ifdef _TG_WINDOWS_
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif 

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
	#include <iomanip>
#else
	#include <iomanip.h>
#endif

extern omni_thread::key_t key_py_data;
namespace Tango
{

DeviceImpl *PollThread::dev_to_del = NULL;
string PollThread::name_to_del = "";
PollObjType PollThread::type_to_del = Tango::POLL_CMD;

//+-------------------------------------------------------------------------
//
// method : 		PollThread::Pollthread
// 
// description : 	The polling thread constructor.
//
//--------------------------------------------------------------------------

PollThread::PollThread(PollThCmd &cmd,TangoMonitor &m,bool heartbeat): shared_cmd(cmd),p_mon(m),
					    sleep(1),polling_stop(true),
					    attr_names(1),tune_ctr(1),
					    need_two_tuning(false),auto_upd(-1),send_heartbeat(heartbeat)
{
	attr_names.length(1);

	cci = 0;
	dummy_cl_id.cpp_clnt(cci);
	
	if (heartbeat == true)
		polling_stop = false;
	
#ifdef _TG_WINDOWS_
	LARGE_INTEGER f;
	BOOL is_ctr;
	is_ctr = QueryPerformanceFrequency(&f);
	if (is_ctr != 0)
		ctr_frequency = 1000000.0 / (double)f.QuadPart;
	else
		ctr_frequency = 0.0;
#endif
	
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::run_undetached
// 
// description : 	The polling thread main code
//
//--------------------------------------------------------------------------


						  
void *PollThread::run_undetached(void *ptr)
{
	PollCmdType received;
	bool per_thread_data_created = false;

//
// If the thread is the event heartbeat thread,
// use it also for the storage of sub device properties.
// Declare a work item to check the for new sub devices
// regularly.
// 
	if ( send_heartbeat == true )
	{
		WorkItem wo;
		
		wo.dev = NULL;
		wo.poll_list = NULL;
		wo.type = STORE_SUBDEV;
		wo.update = 30*60*1000;			// check ervery 30 minutes
		wo.name = "Sub device property storage";
		wo.needed_time.tv_sec  = 0;
		wo.needed_time.tv_usec = 0;
		
#ifdef _TG_WINDOWS_
		_ftime(&now_win);
		now.tv_sec = (unsigned long)now_win.time;
		now.tv_usec = (long)now_win.millitm * 1000;
#else
		gettimeofday(&now,NULL);
#endif
		now.tv_sec = now.tv_sec - DELTA_T;
		wo.wake_up_date = now; 
		insert_in_list(wo);
	}

//
// The infinite loop
//
		
	while(1)
	{
		try
		{
			if (sleep != 0)
				received = get_command(sleep);
			else
				received = POLL_TIME_OUT;

//
// Create the per thread data if it is not already done (For Python DS)
//

			if (per_thread_data_created == false)
			{
				omni_thread::self()->set_value(key_py_data,new PyData());
				per_thread_data_created = true;
			}
				
#ifdef _TG_WINDOWS_
			_ftime(&now_win);
			now.tv_sec = (unsigned long)now_win.time;
			now.tv_usec = (long)now_win.millitm * 1000;
#else
			gettimeofday(&now,NULL);
#endif
			now.tv_sec = now.tv_sec - DELTA_T;

			switch (received)
			{
			case POLL_COMMAND:
				execute_cmd();
				break;
				
			case POLL_TIME_OUT:
				one_more_poll();
				break;
				
			case POLL_TRIGGER:
				one_more_trigg();
				break;
			}

#ifdef _TG_WINDOWS_
			_ftime(&after_win);
			after.tv_sec = (unsigned long)after_win.time;
			after.tv_usec = (long)after_win.millitm * 1000;
#else
			gettimeofday(&after,NULL);
#endif
			after.tv_sec = after.tv_sec - DELTA_T;

			if (tune_ctr <= 0)
			{
				tune_list(true,0);
				if (need_two_tuning == true)
				{
					unsigned long nb_works = works.size();
					tune_ctr = (nb_works << 2);
					need_two_tuning = false;
				}
				else
					tune_ctr = POLL_LOOP_NB;
			}
								
			compute_sleep_time();
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

	return NULL;
	
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::get_command
// 
// description : 	This method wait on the shared monitor for a new
//			command to be sent to the polling thread. The
//			thread waits with a timeout. If the thread is
//			awaken due to the timeout, false is returned.
//			If the work list is empty, the thread waits for ever.
//
// argument : in : 	tout : The timeout in mS
//
// The method returns true if the thread has been awaken due to a new
// command sent by the main thread
//
//--------------------------------------------------------------------------

PollCmdType PollThread::get_command(long tout)
{
	omni_mutex_lock sync(p_mon);
	PollCmdType ret;

//
// Wait on monitor
//

	if ((shared_cmd.cmd_pending == false) && (shared_cmd.trigger == false))
	{	
		if (works.empty() == true)
			p_mon.wait();
		else
		{
			if (tout != -1)
				p_mon.wait(tout);
		}
	}
	
//
// Test if it is a new command. If yes, copy its data locally
//

	if (shared_cmd.cmd_pending == true)
	{
		local_cmd = shared_cmd;
		ret = POLL_COMMAND;
	}
	else if (shared_cmd.trigger == true)
	{
		local_cmd = shared_cmd;
		ret = POLL_TRIGGER;
	}
	else
		ret = POLL_TIME_OUT;

	return ret;
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::execute_cmd and two unary predicates
// 
// description : 	This method is called when a command has been received
//			It exceute the command!
//
//--------------------------------------------------------------------------

bool pred(const WorkItem &w)
{
	if (w.dev == PollThread::dev_to_del)
	{
		if (w.type == PollThread::type_to_del)
			return w.name == PollThread::name_to_del;
		else
			return false;
	}
	else
		return false;
}

bool pred_dev(const WorkItem &w)
{
	return w.dev == PollThread::dev_to_del;
}
 
void PollThread::execute_cmd()
{
	WorkItem wo;
	list<WorkItem>::iterator ite;
	vector<WorkItem>::iterator et_ite;
	
	switch (local_cmd.cmd_code)
	{
	
//
// Add a new object
//

	case Tango::POLL_ADD_OBJ :
		cout5 << "Received a Add object command" << endl;
		wo.dev = local_cmd.dev;
		wo.poll_list = &(wo.dev->get_poll_obj_list());
		wo.type = (*wo.poll_list)[local_cmd.index]->get_type();
		wo.update = (*wo.poll_list)[local_cmd.index]->get_upd();
		wo.name = (*wo.poll_list)[local_cmd.index]->get_name().c_str();
		wo.needed_time.tv_sec = 0;
		wo.needed_time.tv_usec = 0;
		
		if (wo.update != 0)
		{
			wo.wake_up_date = now;
			if (local_cmd.new_upd != 0)
			{
				cout5 << "Received a delta from now of " << local_cmd.new_upd << endl;
				T_ADD(wo.wake_up_date,local_cmd.new_upd * 1000);
			}
//			add_random_delay(wo.wake_up_date);
			insert_in_list(wo);
			unsigned long nb_works = works.size();
			tune_ctr = (nb_works << 2);
			need_two_tuning = true;
		}
		else
		{
			wo.wake_up_date.tv_sec = 0;
			wo.wake_up_date.tv_usec = 0;
			ext_trig_works.push_back(wo);
		}
		break;

//
// Remove an already polled object
//
		
	case Tango::POLL_REM_OBJ :
		cout5 << "Received a Rem object command" << endl;
		
		dev_to_del = local_cmd.dev;
		name_to_del = local_cmd.name;
		type_to_del = local_cmd.type;

#ifdef _TG_WINDOWS_
		unsigned int i,nb_elt;
		nb_elt = works.size();
		ite = works.begin();
		for (i = 0;i < nb_elt;i++)
		{
			if (ite->dev == PollThread::dev_to_del)
			{
				if (ite->type == PollThread::type_to_del)
				{
					if (ite->name == PollThread::name_to_del)
					{
						works.erase(ite);
						break;
					}
				}
			}
			ite++;
		}
#else		
		works.remove_if(pred);
#endif
		break;
		
//
// Remove an already externally triggered polled object
//
		
	case Tango::POLL_REM_EXT_TRIG_OBJ :
		cout5 << "Received a Ext Trig Rem object command" << endl;

		for (et_ite = ext_trig_works.begin();
		     et_ite != ext_trig_works.end();++et_ite)
		{
			if (et_ite->dev == local_cmd.dev)
			{
				if (et_ite->type == local_cmd.type)
				{
					if (et_ite->name == local_cmd.name)
					{
						ext_trig_works.erase(et_ite);
						break;
					}
				}
			}
		}
		break;
		
//
// Remove all objects belonging to a device.
// Take care, the same device could have several objects --> No break after
// the successfull if in loop
//

	case Tango::POLL_REM_DEV :
		cout5 << "Received a Rem device command" << endl;
		
		dev_to_del = local_cmd.dev;
#ifdef _TG_WINDOWS_
		nb_elt = works.size();
		ite = works.begin();
		for (i = 0;i < nb_elt;i++)
		{
			if (ite->dev == PollThread::dev_to_del)
			{
				ite = works.erase(ite);
			}
			else
				ite++;
		}
		nb_elt = ext_trig_works.size();
		et_ite = ext_trig_works.begin();
		for (i = 0;i < nb_elt;i++)
		{
			if (et_ite->dev == PollThread::dev_to_del)
			{
				et_ite = ext_trig_works.erase(et_ite);
			}
			else
				et_ite++;
		}
#else
		works.remove_if(pred_dev);
		
		ext_trig_works.erase(remove_if(ext_trig_works.begin(),
					       ext_trig_works.end(),
					       pred_dev),
				     ext_trig_works.end());
#endif

		break;

//
// Update polling period
// Several cases has to be implemented here.
// 1 - A classical command from the external world. In this case updating
//     polling period means removing the already inserted object from the work list,
//     compute its new polling time and insert it in the work list with its new
//     polling time and polling period
// 2 - This is executed by the polling thread itself
// 2-1 - The command updates polling period for another object: idem than previous
// 2-2 - The commands updates polling period for the object it is actually polled.
//       In this case, the object is not in the work list. It has been removed from the
//	 work list at the beginning of the "one_more_poll" method and is memorized there
//	 Therefore, simply stores new polling period in a data member. The "one_more_poll"
//	 method will get its new polling period before re-inserting the object in the work
// 	 list with the new update period.
//	 We detect this case because the object is not in any work list (either the work 
//	 list or the trigger list)
//

	case Tango::POLL_UPD_PERIOD :
		cout5 << "Received a update polling period command" << endl;
		
		dev_to_del = local_cmd.dev;
		name_to_del = local_cmd.name;
		type_to_del = local_cmd.type;
		
		ite = find_if(works.begin(),works.end(),pred);
		if (ite != works.end())
		{
			if (local_cmd.new_upd != 0)
			{
				WorkItem tmp_work = *ite;
#ifdef _TG_WINDOWS_
				unsigned int i,nb_elt;
				nb_elt = works.size();
				ite = works.begin();
				for (i = 0;i < nb_elt;i++)
				{
					if (ite->dev == PollThread::dev_to_del)
					{
						if (ite->type == PollThread::type_to_del)
						{
							if (ite->name == PollThread::name_to_del)
							{
								works.erase(ite);
								break;
							}
						}
					}
					ite++;
				}
#else		
				works.remove_if(pred);
#endif
				tmp_work.update = local_cmd.new_upd;
				compute_new_date(now,local_cmd.new_upd);
				tmp_work.wake_up_date = now;
				insert_in_list(tmp_work);
			}
			else
			{
			
//
// First, remove object from polling list and insert it in externally
// triggered list
//

#ifdef _TG_WINDOWS_
				unsigned int i,nb_elt;
				nb_elt = works.size();
				ite = works.begin();
				for (i = 0;i < nb_elt;i++)
				{
					if (ite->dev == PollThread::dev_to_del)
					{
						if (ite->type == PollThread::type_to_del)
						{
							if (ite->name == PollThread::name_to_del)
							{
								works.erase(ite);
								break;
							}
						}
					}
					ite++;
				}
#else		
				works.remove_if(pred);
#endif

				wo.dev = local_cmd.dev;
				wo.poll_list = &(wo.dev->get_poll_obj_list());
				wo.type = (*wo.poll_list)[local_cmd.index]->get_type();
				wo.update = (*wo.poll_list)[local_cmd.index]->get_upd();
				wo.name = (*wo.poll_list)[local_cmd.index]->get_name().c_str();
				wo.wake_up_date.tv_sec = 0;
				wo.wake_up_date.tv_usec = 0;
				
				ext_trig_works.push_back(wo);
			}
		}
		else
		{
		
//
// If not found in work list, it should be in the externally
// triggered object. Therefore, remove it from externally
// triggered list and insert it in work list
// If not found in work list and in trig list, we are in case
// 2-2 as desribed above (polling thread updateing itself polling
// period of the object it actually polls)
//

			bool found = false;
			for (et_ite = ext_trig_works.begin();
			     et_ite != ext_trig_works.end();++et_ite)
			{
				if (et_ite->dev == local_cmd.dev)
				{
					if (et_ite->type == local_cmd.type)
					{
						if (et_ite->name == local_cmd.name)
						{
							ext_trig_works.erase(et_ite);
							found = true;
							break;
						}
					}
				}
			}

			if (found == true)
			{			
				wo.dev = local_cmd.dev;
				wo.poll_list = &(wo.dev->get_poll_obj_list());
				wo.type = type_to_del;
				wo.update = local_cmd.new_upd;
				wo.name = name_to_del;
				wo.wake_up_date = now;
			
				insert_in_list(wo);
			}
			else
				auto_upd = local_cmd.new_upd;
		}
		break;

//
// Add the event heartbeat every 9 seconds
//

	case Tango::POLL_ADD_HEARTBEAT:
		cout5 << "Received a add heartbeat command" << endl;
		wo.dev = NULL;
		wo.poll_list = NULL;
		wo.type = EVENT_HEARTBEAT;
		wo.update = 9000;
		wo.name = "Event heartbeat";
		wo.needed_time.tv_sec = 0;
		wo.needed_time.tv_usec = TIME_HEARTBEAT;
				
		wo.wake_up_date = now;
		insert_in_list(wo);
		break;

//
// Remove the event heartbeat
//

	case Tango::POLL_REM_HEARTBEAT:
		cout5 << "Received a remove heartbeat command" << endl;
		unsigned int ii,nb_elem;
		nb_elem = works.size();
		ite = works.begin();
		
		for (ii = 0;ii < nb_elem;ii++)
		{
			if (ite->type == EVENT_HEARTBEAT)
			{
				works.erase(ite);
				break;
			}
			ite++;
		}
		break;	
				
//
// Start polling
//
		
	case Tango::POLL_START :
		cout5 << "Received a Start polling command" << endl;
		polling_stop = false;
		break;

//
// Stop polling
//
		
	case Tango::POLL_STOP :
		cout5 << "Received a Stop polling command" << endl;
		polling_stop = true;
		break;

//
// Ask polling thread to exit
//
		
	case Tango::POLL_EXIT :
		cout5 << "Received an exit command" << endl;
		omni_thread::exit();
		break;
	}
	
//
// Inform requesting thread that the work is done
//


	{
		omni_mutex_lock sync(p_mon);
		shared_cmd.cmd_pending = false;
		p_mon.signal();
	}
	
	if (Tango::Util::_tracelevel >= 4)
		print_list();
		
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::add_random_delay
// 
// description : 	Add a random number of microsecond (between 0 and
//			500000) in order to spread the date where polling
//			should happens. This is necessary especially at device
//			process startup when each command in send to the
//			polling thread in a loop
//
// argument : in : t : The timeval structure reference	
//
//--------------------------------------------------------------------------

void PollThread::add_random_delay(struct timeval &t)
{
	long add_usec;

	add_usec = (long)(500000. * (float)rand() / (float)RAND_MAX);
	t.tv_usec = t.tv_usec + add_usec;
	if (t.tv_usec >= 1000000)
	{
		t.tv_sec++;
		t.tv_usec = t.tv_usec - 1000000;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::one_more_poll
// 
// description : 
//
// argument : in : 	
//
//--------------------------------------------------------------------------

void PollThread::one_more_poll()
{
	WorkItem tmp = works.front();
	works.pop_front();

	if (polling_stop == false)
	{
		switch (tmp.type)
		{
		case Tango::POLL_CMD:
			poll_cmd(tmp);
			break;
			
		case Tango::POLL_ATTR:
			poll_attr(tmp);
			break;
			
		case Tango::EVENT_HEARTBEAT:
			eve_heartbeat(tmp);
			break;
		case Tango::STORE_SUBDEV:
			store_subdev(tmp);
			break;
		}
	}

//
// For case where the plling thread itself modify the polling
// period of the object it already polls
//
	
	if (auto_upd != -1)
	{
		tmp.update = auto_upd;
		auto_upd = -1;
	}

//
// Compute new polling date and insert work in list
//
		
	compute_new_date(tmp.wake_up_date,tmp.update);	
	insert_in_list(tmp);
	tune_ctr--;
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::one_more_trigg
// 
// description : 	This method is called when a trigger command has been
//			received
//
//--------------------------------------------------------------------------

 
void PollThread::one_more_trigg()
{
	cout5 << "Polling thread has received a trigger" << endl;

//
// Check that the object is registered
//

	dev_to_del = local_cmd.dev;
	name_to_del = local_cmd.name;
	type_to_del = local_cmd.type;
	
	vector<WorkItem>::iterator et_ite;
	et_ite = find_if(ext_trig_works.begin(),ext_trig_works.end(),pred);
	

//
// Check that the object to poll has been installed.
// If not, simply returns. This case should never happens because it is
// tested in the Util::trigger_polling() method before the trigger is
// effectively sent to this thread.
//
	
	if (et_ite == ext_trig_works.end())
	{
		cout5 << "Object externally triggered not found !!!" << endl;
		{
			omni_mutex_lock sync(p_mon);
			shared_cmd.trigger = false;
			p_mon.signal();
		}
		return;
	}
	
//
// Do the job
//	

	WorkItem tmp = *et_ite;
	if (polling_stop == false)
	{
		if (tmp.type == Tango::POLL_CMD)
			poll_cmd(tmp);
		else
			poll_attr(tmp);
	}

//
// Inform requesting thread that the work is done
//

	{
		omni_mutex_lock sync(p_mon);
		shared_cmd.trigger = false;
		p_mon.signal();
	}
}


//+-------------------------------------------------------------------------
//
// method : 		PollThread::print_list
// 
// description : 	To print work list
//
//--------------------------------------------------------------------------

void PollThread::print_list()
{
	list<WorkItem>::iterator ite;
	long nb_elt,i;
	
	nb_elt = works.size();
	ite = works.begin();
	for (i = 0;i < nb_elt;i++)
	{
		if (ite->type != EVENT_HEARTBEAT )
		{
			if ( ite->type != STORE_SUBDEV)
			{
				cout4 << "Dev name = " << ite->dev->get_name()
					<< ", obj name = " << ite->name
					<< ", next wake_up at " << + ite->wake_up_date.tv_sec
					<< "," << setw(6) << setfill('0')
					<< ite->wake_up_date.tv_usec << endl;
			}
			else
			{
				cout4 <<  ite->name
					<< ", next wake_up at " << + ite->wake_up_date.tv_sec
					<< "," << setw(6) << setfill('0')
					<< ite->wake_up_date.tv_usec << endl;
			}
		}
		else
		{
			cout4 << "Event heartbeat, next wake_up at " << + ite->wake_up_date.tv_sec
          		<< "," << setw(6) << setfill('0')
          		<< ite->wake_up_date.tv_usec << endl;
		}
		ite++;
	}
}


//+-------------------------------------------------------------------------
//
// method : 		PollThread::insert_in_list
// 
// description : 	To insert (at the correct place) a new Work Item in 
//			the work list
//
// argument: In :	- new_work : The new work item
//
//--------------------------------------------------------------------------

void PollThread::insert_in_list(WorkItem &new_work)
{
	list<WorkItem>::iterator ite;
	for (ite = works.begin();ite != works.end();ite++)
	{
		if (ite->wake_up_date.tv_sec < new_work.wake_up_date.tv_sec)
			continue;
		else if (ite->wake_up_date.tv_sec == new_work.wake_up_date.tv_sec)
		{
			if (ite->wake_up_date.tv_usec < new_work.wake_up_date.tv_usec)
				continue;
			else
			{
				works.insert(ite,new_work);
				return;
			}
		}
		else
		{
			works.insert(ite,new_work);
			return;
		}
	}
	if (ite == works.end())
		works.push_back(new_work);
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::tune_list 
// 
// description : 	This method tunes the work list.
//
// argument: In :	- from_needed : Set to true if the delta between
//					work should be at least equal to the
//					time needed to execute the previous work
//			- min_delta : Min. delta between polling works
//				      when from_needed is false
//
//--------------------------------------------------------------------------

void PollThread::tune_list(bool from_needed, long min_delta)
{
	list<WorkItem>::iterator ite,ite_next,ite_prev;
	
	unsigned long nb_works = works.size();
	cout4 << "Entering tuning list. The list has " << nb_works << " item(s)" << endl;

//
// Nothing to do if only one let in list
//

	if (nb_works < 2)
		return;

//
// If we try to tune the list with respect to works needed
// time, compute works needed time sum and find minimun update
// period
//

	unsigned long needed_sum = 0;
	unsigned long min_upd = 100000000;
	long max_delta_needed;
	
	if (from_needed == true)
	{
		for (ite = works.begin();ite != works.end();++ite)
		{
			long needed_time_usec = (ite->needed_time.tv_sec * 1000000) + ite->needed_time.tv_usec;
			needed_sum = needed_sum + (unsigned long)needed_time_usec;
			
			unsigned long update_usec = (unsigned long)ite->update * 1000;
			if (min_upd > update_usec)
				min_upd = update_usec;
		}

//
// In some cases, it is impossible to tune
//
		
		if (needed_sum > min_upd)
			return;
		else
		{
			long sleeping = min_upd - needed_sum;
			max_delta_needed = sleeping / (nb_works);
		}
		
//
// Now build a new tuned list
//

		list<WorkItem> new_works;
		new_works.push_front(works.front());
		
		ite = works.begin();
		ite_prev = new_works.begin();
		
		for (++ite;ite != works.end();++ite,++ite_prev)
		{
			long needed_time_usec = (ite_prev->needed_time.tv_sec * 1000000) + ite_prev->needed_time.tv_usec;
			WorkItem wo = *ite;
			wo.wake_up_date = ite_prev->wake_up_date;
			T_ADD(wo.wake_up_date,needed_time_usec + max_delta_needed);			
			new_works.push_back(wo);
		}

//
// Replace work list
//
		
		works = new_works;
	}
	else
	{			
		ite_next = works.begin();
		ite = ite_next;
		++ite_next;

		for (unsigned int i = 1;i < nb_works;i++)
		{
			long diff;
			T_DIFF(ite->wake_up_date,ite_next->wake_up_date,diff);
			
//
// If delta time between works is less than min,
// shift following work
//

			if (diff < min_delta)
				T_ADD(ite_next->wake_up_date,min_delta - diff);
		
			++ite;
			++ite_next;
		}
	}

	cout4 << "Tuning list done" << endl;
	print_list();	
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::compute_new_date
// 
// description : 	This method computes the new poll date.
//
// argument: In :	- time : The actual date
//					- upd : The polling update period (mS)
//
//--------------------------------------------------------------------------

void PollThread::compute_new_date(struct timeval &time,int upd)
{
	double ori_d = (double)time.tv_sec + ((double)time.tv_usec / 1000000);
	double new_d = ori_d + ((double)(upd) / 1000);
	time.tv_sec = (long)new_d;
	time.tv_usec = (long)((new_d - time.tv_sec) * 1000000);
}

void PollThread::time_diff(struct timeval &before,
			   struct timeval &after_t,
			   struct timeval &result)
{
	double bef_d = (double)before.tv_sec + ((double)before.tv_usec / 1000000);
	double aft_d = (double)after_t.tv_sec + ((double)after_t.tv_usec / 1000000);
	double diff_d = aft_d - bef_d;
	
	result.tv_sec = (long)diff_d;
	result.tv_usec = (long)((diff_d - result.tv_sec) * 1000000);
}


//+-------------------------------------------------------------------------
//
// method : 		PollThread::compute_sleep_time
// 
// description : 	This method computes how many mS the thread should 
//			sleep before the next poll time. If this time is 
//			negative and greater than a pre-defined threshold,
//			the polling is discarded.
//
//--------------------------------------------------------------------------

void PollThread::compute_sleep_time()
{
	print_list();

	if (works.empty() == false)
	{
		double next,after_d,diff;
		next = (double)works.front().wake_up_date.tv_sec + ((double)works.front().wake_up_date.tv_usec / 1000000);
		after_d = (double)after.tv_sec + ((double)after.tv_usec / 1000000);
		diff = next - after_d;
		
		if (diff < 0)
		{
			if (fabs(diff) < DISCARD_THRESHOLD)
				sleep = -1;
			else
			{
				while((diff < 0) && (fabs(diff) > DISCARD_THRESHOLD))
				{
					cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
					WorkItem tmp = works.front();
					if (tmp.type == POLL_ATTR)
						err_out_of_sync(tmp);
					
					compute_new_date(tmp.wake_up_date,tmp.update);					
					insert_in_list(tmp);
					works.pop_front();
					tune_ctr--;

					next = (double)works.front().wake_up_date.tv_sec + ((double)works.front().wake_up_date.tv_usec / 1000000);
					diff = next - after_d;
				}
				if (fabs(diff) < DISCARD_THRESHOLD)
					sleep = -1;
				else
					sleep = (long)(diff * 1000);
			}
		}
		else
			sleep = (long)(diff * 1000);

		cout5 << "Sleep for : " << sleep << endl;
	}
}


//+-------------------------------------------------------------------------
//
// method : 		PollThread::err_out_of_sync
// 
// description : 	To force one event if the polling thread has
//			discarded one work item because it is late
//
// argument : in :	- to_do : The work item
//
//--------------------------------------------------------------------------

void PollThread::err_out_of_sync(WorkItem &to_do)
{

	EventSupplier *event_supplier;
	event_supplier = Util::instance()->get_event_supplier();
	if (event_supplier != NULL)
	{
		Tango::DevErrorList errs;
		errs.length(1);
	
		errs[0].severity = Tango::ERR;
		errs[0].reason = CORBA::string_dup("API_PollThreadOutOfSync");
		errs[0].origin = CORBA::string_dup("PollThread::err_out_of_sync");
		errs[0].desc = CORBA::string_dup("The polling thread is late and discard this object polling.\nAdvice: Tune device server polling");
		
		Tango::DevFailed except(errs);
		long idl_vers = to_do.dev->get_dev_idl_version();
			
		if (idl_vers >= 3)
			event_supplier->detect_and_push_events_3(to_do.dev,
						       idl_vers,
						       &dummy_att3,
						       NULL,
						       &except,
						       to_do.name);
		else	
			event_supplier->detect_and_push_events(to_do.dev,
						       idl_vers,
						       dummy_att,
						       &except,
						       to_do.name);
						       
	}
}


//+-------------------------------------------------------------------------
//
// method : 		PollThread::poll_cmd
// 
// description : 	Execute a command and store the result in the device
//			ring buffer
//
// argument : in :	- to_do : The work item
//
//--------------------------------------------------------------------------

void PollThread::poll_cmd(WorkItem &to_do)
{
	cout5 << "----------> Time = " << now.tv_sec << ","
        << setw(6) << setfill('0') << now.tv_usec
        << " Dev name = " << to_do.dev->get_name()
        << ", Cmd name = " << to_do.name << endl;

	CORBA::Any *argout = NULL;
	Tango::DevFailed *save_except = NULL;
	struct timeval before_cmd,after_cmd,needed_time;
#ifdef _TG_WINDOWS_
	struct _timeb before_win,after_win;
	LARGE_INTEGER before,after;
#endif
	
	vector<PollObj *>::iterator ite;
	bool cmd_failed = false;
	try
	{
#ifdef _TG_WINDOWS_
		if (ctr_frequency != 0)
			QueryPerformanceCounter(&before);
		_ftime(&before_win);
		before_cmd.tv_sec = (unsigned long)before_win.time;
		before_cmd.tv_usec = (long)before_win.millitm * 1000;
#else
		gettimeofday(&before_cmd,NULL);
#endif
		before_cmd.tv_sec = before_cmd.tv_sec - DELTA_T;

//
// Execute the command
//

		argout = to_do.dev->command_inout(to_do.name.c_str(),in_any);

#ifdef _TG_WINDOWS_
		if (ctr_frequency != 0)
		{
			QueryPerformanceCounter(&after);
			
			needed_time.tv_sec = 0;
			needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
			to_do.needed_time = needed_time;
		}
		else
		{
			_ftime(&after_win);
			after_cmd.tv_sec = (unsigned long)after_win.time;
			after_cmd.tv_usec = (long)after_win.millitm * 1000;
			
			after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
			time_diff(before_cmd,after_cmd,needed_time);
			to_do.needed_time = needed_time;
		}
#else		
		gettimeofday(&after_cmd,NULL);
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
		time_diff(before_cmd,after_cmd,needed_time);
		to_do.needed_time = needed_time;
#endif
	}
	catch (Tango::DevFailed &e)
	{
		cmd_failed = true;
#ifdef _TG_WINDOWS_
		if (ctr_frequency != 0)
		{
			QueryPerformanceCounter(&after);

			needed_time.tv_sec = 0;
			needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
			to_do.needed_time = needed_time;
		}
		else
		{
			_ftime(&after_win);
			after_cmd.tv_sec = (unsigned long)after_win.time;
			after_cmd.tv_usec = (long)after_win.millitm * 1000;
			
			after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
			time_diff(before_cmd,after_cmd,needed_time);
			to_do.needed_time = needed_time;
		}
#else
		gettimeofday(&after_cmd,NULL);
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
		time_diff(before_cmd,after_cmd,needed_time);
		to_do.needed_time = needed_time;
#endif		
		save_except = new Tango::DevFailed(e);
	}
	
//
// Insert result in polling buffer and simply forget this command if it is
// not possible to insert the result in polling buffer
//

	try
	{
		to_do.dev->get_poll_monitor().get_monitor();
		ite = to_do.dev->get_polled_obj_by_type_name(to_do.type,to_do.name);
		if (cmd_failed == false)
			(*ite)->insert_data(argout,before_cmd,needed_time);
		else
			(*ite)->insert_except(save_except,before_cmd,needed_time);
		to_do.dev->get_poll_monitor().rel_monitor();
	}
	catch (Tango::DevFailed &)
	{
		if (cmd_failed == false)
			delete argout;
		else
			delete save_except;
		to_do.dev->get_poll_monitor().rel_monitor();
	}
}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::poll_attr
// 
// description : 	Read attribute and store the result in the device
//			ring buffer
//
// argument : in :	- to_do : The work item
//
//--------------------------------------------------------------------------

void PollThread::poll_attr(WorkItem &to_do)
{
	cout5 << "----------> Time = " << now.tv_sec << ","
	      << setw(6) << setfill('0') << now.tv_usec
	      << " Dev name = " << to_do.dev->get_name()
        << ", Attr name = " << to_do.name << endl;
	
	struct timeval before_cmd,after_cmd,needed_time;
#ifdef _TG_WINDOWS_
	struct _timeb before_win,after_win;
	LARGE_INTEGER before,after;
#endif
	Tango::AttributeValueList *argout = NULL;
	Tango::AttributeValueList_3 *argout_3 = NULL;
	Tango::AttributeValueList_4 *argout_4 = NULL;
	Tango::DevFailed *save_except = NULL;
	bool attr_failed = false;	
	vector<PollObj *>::iterator ite;
	
	long idl_vers = to_do.dev->get_dev_idl_version();
	try
	{
#ifdef _TG_WINDOWS_
		if (ctr_frequency != 0)
			QueryPerformanceCounter(&before);
		_ftime(&before_win);
		before_cmd.tv_sec = (unsigned long)before_win.time;
		before_cmd.tv_usec = (long)before_win.millitm * 1000;
#else
		gettimeofday(&before_cmd,NULL);
#endif
		before_cmd.tv_sec = before_cmd.tv_sec - DELTA_T;

//
// Read the attributes
//

		attr_names[0] = to_do.name.c_str();
		if (idl_vers >= 4)
			argout_4 = (static_cast<Device_4Impl *>(to_do.dev))->read_attributes_4(attr_names,Tango::DEV,dummy_cl_id);
		else if (idl_vers == 3)
			argout_3 = (static_cast<Device_3Impl *>(to_do.dev))->read_attributes_3(attr_names,Tango::DEV);
		else		
			argout = to_do.dev->read_attributes(attr_names);

#ifdef _TG_WINDOWS_
		if (ctr_frequency != 0)
		{
			QueryPerformanceCounter(&after);
			
			needed_time.tv_sec = 0;
			needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
			to_do.needed_time = needed_time;
		}
		else
		{
			_ftime(&after_win);
			after_cmd.tv_sec = (unsigned long)after_win.time;
			after_cmd.tv_usec = (long)after_win.millitm  * 1000;
			
			after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
			time_diff(before_cmd,after_cmd,needed_time);
			to_do.needed_time = needed_time;
		}
#else		
		gettimeofday(&after_cmd,NULL);
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
		time_diff(before_cmd,after_cmd,needed_time);
		to_do.needed_time = needed_time;
#endif
	}
	catch (Tango::DevFailed &e)
	{
		attr_failed = true;
#ifdef _TG_WINDOWS_
		if (ctr_frequency != 0)
		{
			QueryPerformanceCounter(&after);

			needed_time.tv_sec = 0;
			needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
			to_do.needed_time = needed_time;
		}
		else
		{
			_ftime(&after_win);
			after_cmd.tv_sec = (unsigned long)after_win.time;
			after_cmd.tv_usec = (long)after_win.millitm * 1000;
			
			after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
			time_diff(before_cmd,after_cmd,needed_time);
			to_do.needed_time = needed_time;
		}
#else
		gettimeofday(&after_cmd,NULL);
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
		time_diff(before_cmd,after_cmd,needed_time);
		to_do.needed_time = needed_time;
#endif
		
		save_except = new Tango::DevFailed(e);
	}

//
// Starting with IDl release 3, an attribute in error is not an exception
// any more. Re-create one.
// Don't forget that it is still possible to receive classical exception
// (in case of Monitor timeout for instance)
//

	if (idl_vers >= 3)
	{
		if (idl_vers == 4)
		{
			if ((attr_failed == false) && ((*argout_4)[0].err_list.length() != 0))
			{
				attr_failed = true;
				save_except = new Tango::DevFailed((*argout_4)[0].err_list);
				delete argout_4;
			}
		}
		else
		{
			if ((attr_failed == false) && ((*argout_3)[0].err_list.length() != 0))
			{
				attr_failed = true;
				save_except = new Tango::DevFailed((*argout_3)[0].err_list);
				delete argout_3;
			}
		}
	}
	
//
// Events - for each event call the detect_and_push() method 
// this method will fire events if there are clients registered
// and if there is an event (on_change, on_alarm or periodic)
//

	EventSupplier *event_supplier;
	event_supplier = Util::instance()->get_event_supplier();
	if (event_supplier != NULL)
	{
		if (attr_failed == true)
		{		
			if (idl_vers >= 3)
				event_supplier->detect_and_push_events_3(to_do.dev,
						               idl_vers,
						       	       &dummy_att3,
						       	       NULL,
						               save_except,
						               to_do.name);
			else	
				event_supplier->detect_and_push_events(to_do.dev,
						               idl_vers,
						       	       dummy_att,
						               save_except,
						               to_do.name);
		}
		else
		{
			if (idl_vers >= 4)
				event_supplier->detect_and_push_events_3(to_do.dev,
							       	       	   idl_vers,
							       	       	   NULL,
						       	               &((*argout_4)[0]),
						                       save_except,
						                       to_do.name);
			else if (idl_vers == 3)
				event_supplier->detect_and_push_events_3(to_do.dev,
							       	       	   idl_vers,
						       	               &((*argout_3)[0]),
						       	               NULL,
						                       save_except,
						                       to_do.name);				
			else
				event_supplier->detect_and_push_events(to_do.dev,
							       	       	   idl_vers,
						       	               (*argout)[0],
						                       save_except,
						                       to_do.name);
		}
							       
//
// Heartbeat - check to see if it is time to send a heartbeat event
//

		if (send_heartbeat == true)
			event_supplier->push_heartbeat_event();
	}
	
			
//
// Insert result in polling buffer and simply forget this attribute if it is
// not possible to insert the result in polling buffer
//

	try
	{
		to_do.dev->get_poll_monitor().get_monitor();		
		ite = to_do.dev->get_polled_obj_by_type_name(to_do.type,to_do.name);
		if (attr_failed == false)
		{
			if (idl_vers >= 4)
				(*ite)->insert_data(argout_4,before_cmd,needed_time);
			else if (idl_vers == 3)
				(*ite)->insert_data(argout_3,before_cmd,needed_time);						
			else
				(*ite)->insert_data(argout,before_cmd,needed_time);
		}
		else
			(*ite)->insert_except(save_except,before_cmd,needed_time);
		to_do.dev->get_poll_monitor().rel_monitor();
	}
	catch (Tango::DevFailed &)
	{
		if (attr_failed == false)
		{
			if (idl_vers >= 4)
				delete argout_4;
			else if (idl_vers == 3)
				delete argout_3;
			else
				delete argout;
		}
		else
			delete save_except;
		to_do.dev->get_poll_monitor().rel_monitor();
	}

}

//+-------------------------------------------------------------------------
//
// method : 		PollThread::eve_heartbeat
// 
// description : 	Send the event heartbeat
//
// argument : in :	- to_do : The work item
//
//--------------------------------------------------------------------------

void PollThread::eve_heartbeat(WorkItem &to_do)
{
	cout5 << "----------> Time = " << now.tv_sec << ","
	      << setw(6) << setfill('0') << now.tv_usec
	      << " Sending event heartbeat" << endl;

	EventSupplier *event_supplier;
	event_supplier = Util::instance()->get_event_supplier();
	if ((event_supplier != NULL) && (send_heartbeat == true))
	{
		event_supplier->push_heartbeat_event();
	}
	
}


//+-------------------------------------------------------------------------
//
// method : 		PollThread::store_subdev
// 
// description : 	Store the sub device properties when
//                  needed.
//
// argument : in :	- to_do : The work item
//
//--------------------------------------------------------------------------

void PollThread::store_subdev(WorkItem &to_do)
{
	static bool ignore_call = true;
	
	cout5 << "----------> Time = " << now.tv_sec << ","
	      << setw(6) << setfill('0') << now.tv_usec
	      << " Store sub device property data if needed!" << endl;
	
	
	if ( !ignore_call )
	{
		Tango::Util *tg = Tango::Util::instance();
		tg->get_sub_dev_diag().store_sub_devices();
	}
	else
	{
		// ignore the first call to avoid storage during
		// device server start-up.
		ignore_call = false;
	}
}

} // End of Tango namespace

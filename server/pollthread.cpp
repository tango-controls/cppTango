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
// $Revision$
//
// $Log$
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
// Revision 2.10  2003/03/11 17:55:56  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:33  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:23  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:11  taurel
// See Tango WEB pages for list of changes
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
#include <math.h>

#ifdef WIN32
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif 

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD))
	#include <iomanip>
#else
	#include <iomanip.h>
#endif

namespace Tango
{

DeviceImpl *PollThread::dev_to_del = NULL;
string PollThread::name_to_del = "";
PollObjType PollThread::type_to_del = Tango::POLL_CMD;

//+-------------------------------------------------------------------------
//
// method : 		PollThread::PollThread
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


void *PollThread::run_undetached(void *ptr)
{
	bool received;
		
	while(1)
	{
		try
		{
			if (sleep != 0)
				received = get_command(sleep);
			else
				received = false;

#ifdef WIN32
			_ftime(&now_win);
			now.tv_sec = (unsigned long)now_win.time;
			now.tv_usec = (long)now_win.millitm * 1000;
#else
			gettimeofday(&now,NULL);
#endif
			now.tv_sec = now.tv_sec - DELTA_T;
				
			if (received == true)
			{
				execute_cmd();
			}
			else
			{
				one_more_poll();
			}

#ifdef WIN32
			_ftime(&after_win);
			after.tv_sec = (unsigned long)after_win.time;
			after.tv_usec = (long)after_win.millitm * 1000;
#else
			gettimeofday(&after,NULL);
#endif
			after.tv_sec = after.tv_sec - DELTA_T;
				
			compute_sleep_time();
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

bool PollThread::get_command(long tout)
{
	omni_mutex_lock sync(p_mon);
	bool ret;

//
// Wait on monitor
//

	if (shared_cmd.cmd_pending == false)
	{	
		if (works.empty() == true)
			p_mon.wait();
		else
			p_mon.wait(tout);
	}
	
//
// Test if it is a new command. If yes, copy its data locally
//

	if (shared_cmd.cmd_pending == true)
	{
		local_cmd = shared_cmd;
		ret = true;
	}
	else
		ret = false;

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
		wo.wake_up_date = now;
		insert_in_list(wo);
		break;

//
// Remove an already polled object
//
		
	case Tango::POLL_REM_OBJ :
		cout5 << "Received a Rem object command" << endl;
		
		dev_to_del = local_cmd.dev;
		name_to_del = local_cmd.name;
		type_to_del = local_cmd.type;

#ifdef WIN32
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
// Remove all objects belonging to a device.
// Take care, the same device could have several objects --> No break after
// the successfull if in loop
//

	case Tango::POLL_REM_DEV :
		cout5 << "Received a Rem device command" << endl;
		
		dev_to_del = local_cmd.dev;
#ifdef WIN32
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
#else
		works.remove_if(pred_dev);
#endif
		break;

//
// Update polling period
//

	case Tango::POLL_UPD_PERIOD :
		cout5 << "Received a update polling period command" << endl;
		
		dev_to_del = local_cmd.dev;
		name_to_del = local_cmd.name;
		type_to_del = local_cmd.type;
		
		ite = find_if(works.begin(),works.end(),pred);
		if (ite != works.end())
			ite->update = local_cmd.new_upd;
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
	}
	
//
// Inform requesting thread that the work is done
//


	{
		omni_mutex_lock sync(p_mon);
		shared_cmd.cmd_pending = false;
		p_mon.signal();
	}
	
	if (Tango::Util::_tracelevel >= 5)
		print_list();
		
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
		if (tmp.type == Tango::POLL_CMD)
			poll_cmd(tmp);
		else
			poll_attr(tmp);
	}
	compute_new_date(tmp.wake_up_date,tmp.update);	
	insert_in_list(tmp);
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
		cout5 << "Dev name = " << ite->dev->get_name()
          << ", obj name = " << ite->name
          << ", next wake_up at " << + ite->wake_up_date.tv_sec
          << "," << setw(6) << setfill('0')
          << ite->wake_up_date.tv_usec << endl;
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
// method : 		PollThread::compute_new_date
// 
// description : 	This method computes the new poll date.
//
// argument: In :	- time : The actual date
//			- upd : The polling update period (mS)
//
//--------------------------------------------------------------------------

void PollThread::compute_new_date(struct timeval &time,long upd)
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
	if (works.empty() == false)
	{
		double next,after_d,diff;
		next = (double)works.front().wake_up_date.tv_sec + ((double)works.front().wake_up_date.tv_usec / 1000000);
		after_d = (double)after.tv_sec + ((double)after.tv_usec / 1000000);
		diff = next - after_d;
		
		if (diff < 0)
		{
			if (fabs(diff) < DISCARD_THRESHOLD)
				sleep = 0;
			else
			{
				while((diff < 0) && (fabs(diff) > DISCARD_THRESHOLD))
				{
					cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
					WorkItem tmp = works.front();
					compute_new_date(tmp.wake_up_date,tmp.update);					
					insert_in_list(tmp);
					works.pop_front();
					
					next = (double)works.front().wake_up_date.tv_sec + ((double)works.front().wake_up_date.tv_usec / 1000000);
					diff = next - after_d;
				}
				if (fabs(diff) < DISCARD_THRESHOLD)
					sleep = 0;
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

	CORBA::Any *argout;
	Tango::DevFailed *save_except;
	struct timeval before_cmd,after_cmd,needed_time;
#ifdef WIN32
	struct _timeb before_win,after_win;
#endif
	
	vector<PollObj *>::iterator ite;
	bool cmd_failed = false;
	try
	{
#ifdef WIN32
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

#ifdef WIN32
		_ftime(&after_win);
		after_cmd.tv_sec = (unsigned long)after_win.time;
		after_cmd.tv_usec = (long)after_win.millitm * 1000;
#else		
		gettimeofday(&after_cmd,NULL);
#endif
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;

		time_diff(before_cmd,after_cmd,needed_time);
	}
	catch (Tango::DevFailed &e)
	{
		cmd_failed = true;
#ifdef WIN32
		_ftime(&after_win);
		after_cmd.tv_sec = (unsigned long)after_win.time;
		after_cmd.tv_usec = (long)after_win.millitm * 1000;
#else
		gettimeofday(&after_cmd,NULL);
#endif
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
		time_diff(before_cmd,after_cmd,needed_time);
		
		save_except = new Tango::DevFailed(e);
	}
	
//
// Insert result in polling buffer and simply forget this command if it is
// not possible to insert the result in polling buffer
//

	try
	{
		to_do.dev->get_dev_monitor().get_monitor();
		ite = to_do.dev->get_polled_obj_by_type_name(to_do.type,to_do.name);
		if (cmd_failed == false)
			(*ite)->insert_data(argout,before_cmd,needed_time);
		else
			(*ite)->insert_except(save_except,before_cmd,needed_time);
		to_do.dev->get_dev_monitor().rel_monitor();
	}
	catch (Tango::DevFailed &)
	{
		if (cmd_failed == false)
			delete argout;
		else
			delete save_except;
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
#ifdef WIN32
	struct _timeb before_win,after_win;
#endif
	Tango::AttributeValueList *argout;
	Tango::DevFailed *save_except;
	bool attr_failed = false;	
	vector<PollObj *>::iterator ite;
	try
	{
#ifdef WIN32
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
		argout = to_do.dev->read_attributes(attr_names);

#ifdef WIN32
		_ftime(&after_win);
		after_cmd.tv_sec = (unsigned long)after_win.time;
		after_cmd.tv_usec = (long)after_win.millitm  * 1000;
#else		
		gettimeofday(&after_cmd,NULL);
#endif
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;

		time_diff(before_cmd,after_cmd,needed_time);
	}
	catch (Tango::DevFailed &e)
	{
		attr_failed = true;
#ifdef WIN32
		_ftime(&after_win);
		after_cmd.tv_sec = (unsigned long)after_win.time;
		after_cmd.tv_usec = (long)after_win.millitm * 1000;
#else
		gettimeofday(&after_cmd,NULL);
#endif
		after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
		time_diff(before_cmd,after_cmd,needed_time);
		
		save_except = new Tango::DevFailed(e);
	}
	
//
// Insert result in polling buffer and simply forget this attribute if it is
// not possible to insert the result in polling buffer
//

	try
	{
		to_do.dev->get_dev_monitor().get_monitor();		
		ite = to_do.dev->get_polled_obj_by_type_name(to_do.type,to_do.name);
		if (attr_failed == false)
			(*ite)->insert_data(argout,before_cmd,needed_time);		
		else
			(*ite)->insert_except(save_except,before_cmd,needed_time);
		to_do.dev->get_dev_monitor().rel_monitor();
	}
	catch (Tango::DevFailed &)
	{
		if (attr_failed == false)
			delete argout;
		else
			delete save_except;
	}

}


} // End of Tango namespace

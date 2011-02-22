//=============================================================================
//
// file :               PollThread.h
//
// description :        Include for the PollThread object. This class implements
//                      the polling thread
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
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
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _POLLTHREAD_H
#define _POLLTHREAD_H

#include <tango.h>
#include <pollobj.h>

#include <list>

#ifdef WIN32
	#include <sys/types.h>
	#include <sys/timeb.h>
#endif

namespace Tango
{

//=============================================================================
//
//			The PollThCmd structure
//
// description :	This structure is used to shared data between the polling
//			thread and the main thread.
//
//=============================================================================

struct PollThCmd
{
	bool			cmd_pending;	// The new command flag
	PollCmdCode		cmd_code;	// The command code
	DeviceImpl		*dev;		// The device pointer (servant)
	long			index;		// Index in the device poll_list
	string			name;		// Object name
	PollObjType		type;		// Object type (cmd/attr)
	long			new_upd;	// New update period (For upd period com.)
};


struct WorkItem
{
	DeviceImpl		*dev;		// The device pointer (servant)
	vector<PollObj *> 	*poll_list;	// The device poll list
	struct timeval		wake_up_date;	// The next wake up date
	long 			update;		// The update period (mS)
	PollObjType		type;		// Object type (command/attr)
	string			name;		// Object name
};

//=============================================================================
//
//			The PollThread class
//
// description :	Class to store all the necessary information for the
//			polling thread. It's run() method is the thread code
//
//=============================================================================

class TangoMonitor;

class PollThread: public omni_thread
{
public:
	PollThread(PollThCmd &cmd,TangoMonitor &m): shared_cmd(cmd),
						  p_mon(m),
						  sleep(1),
						  polling_stop(false),
						  attr_names(1)
						  {attr_names.length(1);};
	
	void *run_undetached(void *);
	void start() {start_undetached();}

protected:
	bool get_command(long);
	void execute_cmd();
	void one_more_poll();
	void compute_new_date(struct timeval &,long);
	void compute_sleep_time();
	void time_diff(struct timeval &,struct timeval &,struct timeval &);
	void poll_cmd(WorkItem &);
	void poll_attr(WorkItem &);
	
	void print_list();
	void insert_in_list(WorkItem &);
	
	PollThCmd		&shared_cmd;
	TangoMonitor		&p_mon;
	
	list<WorkItem>		works;
	PollThCmd		local_cmd;

#ifdef WIN32
	struct _timeb		now_win;
	struct _timeb		after_win;
#endif		
	struct timeval		now;
	struct timeval		after;
	long			sleep;
	bool			polling_stop;
		
private:
	CORBA::Any		in_any;
	DevVarStringArray	attr_names;
	
public:
	static DeviceImpl 	*dev_to_del;
	static string	   	name_to_del;
	static PollObjType	type_to_del;
};



} // End of Tango namespace

#endif /* _POLLTHREAD_ */

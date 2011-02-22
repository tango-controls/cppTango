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
// Revision 3.4  2005/06/29 08:31:19  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.3  2005/01/13 08:30:00  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.2.2.4  2004/12/06 14:39:30  taurel
// - Polling starts in a separate thread
// - For Windows : Polling thread cmd/attr measurement used PerformanceCounter
// - Fix bug in pollext.h in methods to externally fill polling buffer
//
// Revision 3.2.2.3  2004/11/04 09:46:39  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.2.2.2  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.2.2.1  2004/10/22 11:26:33  taurel
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
// Revision 3.2  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.1  2003/08/21 07:24:37  taurel
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
// Revision 3.0.4.2  2004/02/06 11:58:51  taurel
// - Many changes in the event system
//
// Revision 3.0.4.1  2003/12/10 16:08:56  taurel
// Last commit before merging with the event branch.
//
// Revision 3.0  2003/03/25 16:44:09  taurel
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
	bool			trigger;	// The external trigger flag
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
	struct timeval		needed_time;	// Time needed to execute action
};

enum PollCmdType
{
	POLL_TIME_OUT,
	POLL_COMMAND,
	POLL_TRIGGER
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
	PollThread(PollThCmd &,TangoMonitor &);
	
	void *run_undetached(void *);
	void start() {start_undetached();}
	void execute_cmd();
	void set_local_cmd(PollThCmd &cmd) {local_cmd = cmd;}

protected:
	PollCmdType get_command(long);
	void one_more_poll();
	void one_more_trigg();
	void compute_new_date(struct timeval &,long);
	void compute_sleep_time();
	void time_diff(struct timeval &,struct timeval &,struct timeval &);
	void poll_cmd(WorkItem &);
	void poll_attr(WorkItem &);
	void eve_heartbeat(WorkItem &);
	
	void print_list();
	void insert_in_list(WorkItem &);
	void add_random_delay(struct timeval &);
	void tune_list(bool,long);
	void err_out_of_sync(WorkItem &);
	
	PollThCmd		&shared_cmd;
	TangoMonitor		&p_mon;
	
	list<WorkItem>		works;
	vector<WorkItem>	ext_trig_works;
	
	PollThCmd		local_cmd;

#ifdef WIN32
	struct _timeb		now_win;
	struct _timeb		after_win;
	double			ctr_frequency;
#endif		
	struct timeval		now;
	struct timeval		after;
	long			sleep;
	bool			polling_stop;
		
private:
	CORBA::Any		in_any;
	DevVarStringArray	attr_names;
	AttributeValue		dummy_att;
	AttributeValue_3	dummy_att3;
	long			tune_ctr;
	bool			need_two_tuning;
	long			auto_upd;
	
public:
	static DeviceImpl 	*dev_to_del;
	static string	   	name_to_del;
	static PollObjType	type_to_del;
};


//
// Two macros
//

#define T_DIFF(A,B,C) \
	long delta_sec = B.tv_sec - A.tv_sec; \
	if (delta_sec == 0) \
		C = B.tv_usec - A.tv_usec; \
	else \
	{ \
		C = ((delta_sec - 1) * 1000000) + (1000000 - A.tv_usec) + B.tv_usec; \
	}

#define T_ADD(A,B) \
	A.tv_usec = A.tv_usec + B; \
	while (A.tv_usec > 1000000) \
	{ \
		A.tv_sec++; \
		A.tv_usec = A.tv_usec - 1000000; \
	}

#define T_DEC(A,B) \
	A.tv_usec = A.tv_usec - B; \
	if (A.tv_usec < 0) \
	{ \
		A.tv_sec--; \
		A.tv_usec = 1000000 + A.tv_usec; \
	}
	
} // End of Tango namespace

#endif /* _POLLTHREAD_ */

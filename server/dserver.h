//=============================================================================
//
// file :               DServer.h
//
// description :        Include for the DServer class. This class implements
//                      all the commands which are available for device
//			of the DServer class. There is one device of the
//			DServer class for each device server process
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
// Revision 3.21  2010/09/09 13:45:22  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.20  2010/06/18 13:57:09  taurel
// - Add a way (using properties) to define a minimum polling period
//
// Revision 3.19  2010/06/18 07:45:47  taurel
// - In case of locked device, polling and logging related commands are
// allowed only for the locker process
//
// Revision 3.18  2009/10/23 14:36:27  taurel
// - Tango 7.1.1
// - Fix bugs 2880372 and 2881841
// - Now support event in case of Tango system with multi db server
// - The polling threads start with polling inactive
//
// Revision 3.17  2009/08/27 07:23:45  taurel
// - Commit after another merge with Release_7_0_2-bugfixes branch
//
// Revision 3.16  2009/06/17 08:52:08  taurel
// - Commit after a merge with branch Release_7_0_2-bugfixes
// Revision 3.15.2.2  2009/06/22 06:58:09  taurel
// - Fix bug introduced by previous bug fix. It makes the db server crashes.
//
// Revision 3.15.2.1  2009/06/12 08:28:51  taurel
// - Fix bug when using events in multi Tango host environment.
// The TANGO_HOST is now transferred within the even tin the fixed
// header event_type field.
// The DS admin device EventSubscriptionChange command now returns with which Tango lib it is runnig.
// This allows the client to know if the tango host info will be transmitted within the event
//
// Revision 3.15  2009/02/03 15:15:08  jensmeyer
// Added a QuerySubDevice command to read the list of opened sub device
// connections.
//
// Revision 3.14  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.13  2008/10/06 15:01:09  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.12  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.11  2008/10/02 09:09:47  taurel
// - First implementation of multiple polling thread(s)
//
// Revision 3.10  2008/05/20 12:44:11  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.9.2.3  2008/01/03 16:05:52  taurel
// - Some changes in locking feature implementation
//
// Revision 3.9.2.2  2007/12/19 15:54:47  taurel
// - Still some work going on for the locking feature
//
// Revision 3.9.2.1  2007/11/22 12:33:10  taurel
// - First part of the device locking implementation
//
// Revision 3.9  2007/05/15 07:46:59  taurel
// - The polling thread is not configured by a separate thread any more.
// The Add_obj_polling command now support a delta_t to start the first polling
//
// Revision 3.8  2007/04/20 14:40:32  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.7  2007/03/29 07:04:05  taurel
// - Change some data types for 64 bits compatibility
//
// Revision 3.6  2006/05/18 08:52:37  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.5  2006/01/20 08:22:29  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 3.4  2005/01/13 08:29:05  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.3.2.1  2004/10/22 11:25:50  taurel
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
// Revision 3.3  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.2  2004/06/15 12:38:22  taurel
// - Last commit before merge with release 4 branch
//
// Revision 3.1.2.3  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.1.2.2  2003/11/28 17:00:45  taurel
// Changes to handle tango lib build as a Windows DLL and used in a device
// server
// Revision 3.0.2.2  2003/04/15 19:01:54  andy_gotz
// added heartbeat on client and server side; cleaned up cout's
//
// Revision 3.1  2003/04/08 15:06:18  taurel
// - Names of read/written attributes are now stored in blackbox
// - Admin device RestartServer command is now done in its own thread with
//   changing CORBA POA manager state
// Revision 3.0.2.1  2003/04/13 22:12:16  andy_gotz
// added heartbeat; polling starts automatically on subscription
//
// Revision 3.0  2003/03/25 16:43:05  taurel
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
// Revision 2.10  2003/03/11 17:55:52  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.9  2003/02/17 14:57:40  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.8  2002/12/16 12:07:18  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:21  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:06  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:19  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:54  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
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
// Revision 2.2  2002/04/30 10:50:41  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:10  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:12  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:10  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:13  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:45  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:15  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
//=============================================================================

#ifndef _DSERVER_H
#define _DSERVER_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The DServer class
//
// description :	Class to implement all data members and commands for 
//			device of the DServer class
//
//=============================================================================

typedef Tango::DeviceClass *(*Cpp_creator_ptr)(const char *);
typedef void (*ClassFactoryFuncPtr)(DServer *);

class DServer: public Device_4Impl
{
public :
	DServer(DeviceClass *,const char *,const char *,Tango::DevState,const char *);
	~DServer();
	
	Tango::DevVarStringArray *query_class();
	Tango::DevVarStringArray *query_device();
	Tango::DevVarStringArray *query_sub_device();
	void kill();
	void restart(string &);
	void restart_server();
	Tango::DevVarStringArray *query_class_prop(string &);
	Tango::DevVarStringArray *query_dev_prop(string &);

	Tango::DevVarStringArray *polled_device();
	Tango::DevVarStringArray *dev_poll_status(string &);
	void add_obj_polling(const Tango::DevVarLongStringArray *,bool with_db_upd = true,int delta_ms = 0);
	void upd_obj_polling_period(const Tango::DevVarLongStringArray *,bool with_db_upd = true);
	void rem_obj_polling(const Tango::DevVarStringArray *,bool with_db_upd = true);
	void stop_polling();
	void start_polling();
	void start_polling(PollingThreadInfo *);
	void add_event_heartbeat();
	void rem_event_heartbeat();
	
	void lock_device(const Tango::DevVarLongStringArray *);
	Tango::DevLong un_lock_device(const Tango::DevVarLongStringArray *);
	void re_lock_devices(const Tango::DevVarStringArray *);
	Tango::DevVarLongStringArray *dev_lock_status(Tango::ConstDevString);
	
	void delete_devices();

#ifdef TANGO_HAS_LOG4TANGO
	void add_logging_target (const Tango::DevVarStringArray *argin);
	void remove_logging_target (const Tango::DevVarStringArray *argin);
	Tango::DevVarStringArray* get_logging_target (const std::string& dev_name);
	void set_logging_level (const Tango::DevVarLongStringArray *argin);
	Tango::DevVarLongStringArray* get_logging_level (const Tango::DevVarStringArray *argin);
	void stop_logging (void);
	void start_logging (void);
#endif
		
	string &get_process_name() {return process_name;}
	string &get_personal_name() {return instance_name;}
	string &get_instance_name() {return instance_name;}
	string &get_full_name() {return full_name;}
	string &get_fqdn() {return fqdn;}
	bool get_heartbeat_started() {return heartbeat_started;}
	void set_heartbeat_started(bool val) {heartbeat_started = val;}
		
	vector<DeviceClass *> &get_class_list() {return class_list;}
	virtual void init_device();
	
	long get_poll_th_pool_size() {return polling_th_pool_size;}
	bool get_opt_pool_usage() {return optimize_pool_usage;}
	vector<string> get_poll_th_conf() {return polling_th_pool_conf;}

	void check_lock_owner(DeviceImpl *,const char *,const char *);
	void check_upd_authorized(DeviceImpl *,int,PollObjType,string &);

	TANGO_IMP_EXP static void register_class_factory(ClassFactoryFuncPtr f_ptr) {class_factory_func_ptr = f_ptr;}
	void _add_class(DeviceClass *dc) {this->add_class(dc);}
	
	friend class EventSupplier;

protected :
	string							process_name;
	string							instance_name;
	string							full_name;
	string 							fqdn;
	
	vector<DeviceClass *>			class_list;
	
	time_t							last_heartbeat;
	bool							heartbeat_started;
	
	long							polling_th_pool_size;
	vector<string>					polling_th_pool_conf;
	bool							optimize_pool_usage;

	static ClassFactoryFuncPtr 		class_factory_func_ptr;
	
private:
#if ((defined _TG_WINDOWS_) && (defined TANGO_HAS_DLL) && !(defined _TANGO_LIB))
	__declspec(dllexport) void class_factory();
#else
	void class_factory();
#endif
	void add_class(DeviceClass *);
	void create_cpp_class(const char *,const char *);
	void get_dev_prop(Tango::Util *);
	
	bool			from_constructor;
};

class KillThread: public omni_thread
{
public:

	void *run_undetached(void *);
	void start() {start_undetached();}
};

class ServRestartThread: public omni_thread
{
public:
	ServRestartThread(DServer *dev):omni_thread(dev) {}
	
	void run(void *);
};

struct Pol
{
	PollObjType 	type;
	long			upd;
	string 			name;
};

} // End of namespace Tango

#endif

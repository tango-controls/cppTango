//=============================================================================
//
// file :               DServerClass.h
//
// description :        Include for the DServerClass class. This class is a 
//                      singleton class i.e only one object of this class
//			can be created.
//			It contains all properties and methods
//			which the DServer requires only once e.g. the
//			commands.
//			This file also includes class declaration for all the 
//			commands available on device of the DServer class
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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
// Revision 3.7  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.6  2008/10/06 15:01:09  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.5  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.4  2008/05/20 12:44:11  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.3.2.3  2008/01/03 16:05:52  taurel
// - Some changes in locking feature implementation
//
// Revision 3.3.2.2  2007/12/19 15:54:47  taurel
// - Still some work going on for the locking feature
//
// Revision 3.3.2.1  2007/11/22 12:33:11  taurel
// - First part of the device locking implementation
//
// Revision 3.3  2005/03/14 09:49:29  taurel
// - Fix some bugs in filedatabase (Change in the data transferred between client and
//   server).
// - Fix bug in event re-connection
// - Add event support even for device server started with the -file command line option
//
// Revision 3.2  2005/01/13 08:29:05  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.1.2.1  2004/10/22 11:25:50  taurel
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
// Revision 3.1  2004/07/07 08:40:11  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.0.4.1  2004/04/07 11:22:10  taurel
// - Add some import/export declaration for Windows DLL
// - Add test on minor code for the CORBA::IMP_LIMIT exception before
//   printing it
//
// Revision 3.0  2003/03/25 16:43:16  taurel
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
// Revision 2.8  2002/12/16 12:07:19  taurel
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
// Revision 1.6  2001/10/08 09:03:13  taurel
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
// Revision 1.2  2001/03/09 08:20:16  taurel
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
//=============================================================================

#ifndef _DSERVERCLASS_H
#define _DSERVERCLASS_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The DevRestart class
//
// description :	Class to implement the DevRestart command. This command
//			needs one input argument and no outout argument.
//			The input argument is the name of the device to be
//			re-started.
//			This class delete and re-create a device
//
//=============================================================================

 
class DevRestartCmd : public Command
{
public:


	DevRestartCmd(const char *cmd_name,
		      Tango::CmdArgType in,
		      Tango::CmdArgType out,
		      const char *desc);	
	~DevRestartCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevRestartServerCmd class
//
// description :	Class to implement the DevKill command. This 
//			command does not take any input argument. It simply
//			kills the device server.
//
//=============================================================================

 
class DevRestartServerCmd : public Command
{
public:

	DevRestartServerCmd(const char *cmd_name,
		   	    Tango::CmdArgType in,Tango::CmdArgType out);
		   
	~DevRestartServerCmd() {};
	
	virtual CORBA::Any *execute (DeviceImpl *, const CORBA::Any &);
};

//=============================================================================
//
//			The DevQueryClassCmd class
//
// description :	Class to implement the DevQueryClass command. This 
//			command does not take any input argument and return a 
//			list of all the classes created inside the device
//			server process
//
//=============================================================================

 
class DevQueryClassCmd : public Command
{
public:

	DevQueryClassCmd(const char *cmd_name,
			 Tango::CmdArgType in,
			 Tango::CmdArgType out,
			 const char *desc);
			 
	~DevQueryClassCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevQueryDeviceCmd class
//
// description :	Class to implement the DevQueryDevice command. This 
//			command does not take any input argument and return a 
//			list of all the devices created inside the device
//			server process
//
//=============================================================================

 
class DevQueryDeviceCmd : public Command
{
public:

	DevQueryDeviceCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *desc);
			  
	~DevQueryDeviceCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};


//=============================================================================
//
//			The DevQuerySubDeviceCmd class
//
// description :	Class to implement the DevQuerySubDevice command. This 
//			command does not take any input argument and returns a 
//			list of all the sub devices connections opened inside the device
//			server process
//
//=============================================================================

 
class DevQuerySubDeviceCmd : public Command
{
public:

	DevQuerySubDeviceCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *desc);
			  
	~DevQuerySubDeviceCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};


//=============================================================================
//
//			The DevKillCmd class
//
// description :	Class to implement the DevKill command. This 
//			command does not take any input argument. It simply
//			kills the device server.
//
//=============================================================================

 
class DevKillCmd : public Command
{
public:

	DevKillCmd(const char *cmd_name,
		   Tango::CmdArgType in,Tango::CmdArgType out);
		   
	~DevKillCmd() {};
	
	virtual CORBA::Any *execute (DeviceImpl *, const CORBA::Any &);
};

//=============================================================================
//
//			The DevSetTraceLevelCmd class
//
// description :	Class to implement the DevSetTracelevel command.
//			It updates device server trace level with the input 
//			argument
//
//=============================================================================

 
class DevSetTraceLevelCmd : public Command
{
public:

	DevSetTraceLevelCmd(const char *cmd_name,
			    Tango::CmdArgType in,
			    Tango::CmdArgType out,
			    const char *desc);
			    
	~DevSetTraceLevelCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevGetTraceLevel class
//
// description :	Class to implement the DevGetTracelevel command.
//			It simply returns the device server trace level
//
//=============================================================================

 
class DevGetTraceLevelCmd : public Command
{
public:

	DevGetTraceLevelCmd(const char *cmd_name,
			    Tango::CmdArgType in,
			    Tango::CmdArgType out,
			    const char *desc);
			    
	~DevGetTraceLevelCmd() {};
	
	virtual CORBA::Any *execute (DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevSetTraceOutputCmd class
//
// description :	Class to implement the DevSetTraceOutput command.
//			It set the server output to the input parameter
//
//=============================================================================

 
class DevSetTraceOutputCmd : public Command
{
public:

	DevSetTraceOutputCmd(const char *cmd_name,
			     Tango::CmdArgType in,
			     Tango::CmdArgType out,
			     const char *desc);
			     
	~DevSetTraceOutputCmd() {};
	
	virtual CORBA::Any *execute (DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevGetTraceOutputCmd class
//
// description :	Class to implement the DevGetTracelevel command.
//			It simply returns the device server trace level
//
//=============================================================================

 
class DevGetTraceOutputCmd : public Command
{
public:

	DevGetTraceOutputCmd(const char *cmd_name,
			     Tango::CmdArgType in,
			     Tango::CmdArgType out,
			     const char *desc);
			     
	~DevGetTraceOutputCmd() {};
	
	virtual CORBA::Any *execute (DeviceImpl *device, const CORBA::Any &in_any);
};


//=============================================================================
//
//			The QueryWizardClassPropertyCmd class
//
// description :	Class to implement the QueryWizardClassProperty command.
//			This command takes one input argument which is
//			the class name and return a 
//			list of all the class properties definition registered
//			in the wizard for the specified class.
//
//=============================================================================

 
class QueryWizardClassPropertyCmd : public Command
{
public:

	QueryWizardClassPropertyCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *in_desc,const char *out_desc);
			  
	~QueryWizardClassPropertyCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The QueryWizardDevPropertyCmd class
//
// description :	Class to implement the QueryWizardDevProperty command.
//			This command takes one input argument which is
//			the class name and return a 
//			list of all the device properties definition registered
//			in the wizard for the specified class.
//
//=============================================================================

 
class QueryWizardDevPropertyCmd : public Command
{
public:

	QueryWizardDevPropertyCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *in_desc,const char *out_desc);
			  
	~QueryWizardDevPropertyCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The QueryEventChannelIOR class
//
// description :	Class to implement the QueryEventChannelIOR command.
//			This command does not take any input argument and return 
//			the event channel IOR. This command only exits foe DS
//			started with the -file option
//
//=============================================================================

 
class QueryEventChannelIORCmd : public Command
{
public:

	QueryEventChannelIORCmd(const char *cmd_name,
			 Tango::CmdArgType in,
			 Tango::CmdArgType out,
			 const char *desc);
			 
	~QueryEventChannelIORCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The LockDeviceCmd class
//
// description :	Class to implement the LockDevice command.
//			This command takes one input argument which is
//			the device names and return an
//			integer which is the client locking thread period
//
//=============================================================================

class LockDeviceCmd : public Command
{
public:

	LockDeviceCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *in_desc);
			  
	~LockDeviceCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The ReLockDevicesCmd class
//
// description :	Class to implement the ReLockDevices command.
//			This command takes one input argument which is
//			the vector with device names to be re-locked
//
//=============================================================================

class ReLockDevicesCmd : public Command
{
public:

	ReLockDevicesCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *in_desc);
			  
	~ReLockDevicesCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The UnLockDeviceCmd class
//
// description :	Class to implement the UnLockDevice command.
//			This command takes one input argument which is
//			the device name to be unlocked
//
//=============================================================================

 
class UnLockDeviceCmd : public Command
{
public:

	UnLockDeviceCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *in_desc,const char *out_desc);
			  
	~UnLockDeviceCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevLockStatusCmd class
//
// description :	Class to implement the DevLockStatus command.
//			This command takes one input argument which is
//			the device name for which you want to retrieve locking status
//
//=============================================================================

 
class DevLockStatusCmd : public Command
{
public:

	DevLockStatusCmd(const char *cmd_name,
			  Tango::CmdArgType in,Tango::CmdArgType out,
			  const char *in_desc,const char *out_desc);
			  
	~DevLockStatusCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DServerClass class
//
// description :	This class is a singleton ( The constructor is 
//			protected and the _instance data member is static)
//			It contains all properties and methods
//			which the DServer objects requires only once e.g. the
//			commands. 
//
//=============================================================================

 
class DServerClass : public DeviceClass
{
public:

	TANGO_IMP_EXP static DServerClass *instance();
	TANGO_IMP_EXP static DServerClass *init();
	
	~DServerClass() {};
	
	void command_factory();
	void device_factory(const Tango::DevVarStringArray *devlist);
	
protected:
	DServerClass(string &);
	TANGO_IMP static DServerClass *_instance;	
};

} // End of Tango namespace

#endif // _DSERVERCLASS_H

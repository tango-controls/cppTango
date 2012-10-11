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
//			The EventSubscriptionChangeCmd class
//
// description :	Class to implement the EventSubscriptionChange command.
//			This command takes one arguments which are
//			the event for which the user subscribe to
//
//=============================================================================

class EventSubscriptionChangeCmd : public Tango::Command
{
public:
	EventSubscriptionChangeCmd(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	EventSubscriptionChangeCmd(const char *,Tango::CmdArgType, Tango::CmdArgType);
	~EventSubscriptionChangeCmd() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};


//=============================================================================
//
//			The ZmqEventSubscriptionChangeCmd class
//
// description :	Class to implement the ZmqEventSubscriptionChange command.
//			This command takes one arguments which are
//			the event for which the user subscribe to
//
//=============================================================================

class ZmqEventSubscriptionChangeCmd : public Tango::Command
{
public:
	ZmqEventSubscriptionChangeCmd(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *,const char *);
	ZmqEventSubscriptionChangeCmd(const char *,Tango::CmdArgType, Tango::CmdArgType);
	~ZmqEventSubscriptionChangeCmd() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
};

//=============================================================================
//
//			The EventConfirmSubscriptionCmd class
//
// description :	Class to implement the EventConfirmSubscription command.
//			This command takes a list of event for which the client confirm
//			the subscription. This command returns nothing
//
//=============================================================================

class EventConfirmSubscriptionCmd : public Tango::Command
{
public:
	EventConfirmSubscriptionCmd(const char *,Tango::CmdArgType, Tango::CmdArgType,const char *);
	EventConfirmSubscriptionCmd(const char *,Tango::CmdArgType, Tango::CmdArgType);
	~EventConfirmSubscriptionCmd() {};

	virtual bool is_allowed (Tango::DeviceImpl *, const CORBA::Any &);
	virtual CORBA::Any *execute (Tango::DeviceImpl *, const CORBA::Any &);
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

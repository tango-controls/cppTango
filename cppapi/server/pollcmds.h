//=============================================================================
//
// file :               PollCmds.h
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
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
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
//
//=============================================================================

#ifndef _POLLCMDS_H
#define _POLLCMDS_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The PolledDevice class
//
// description :	Class to implement the PolledDevice command.
//			This class returns the name list of device actually
//			polled
//
//=============================================================================


class PolledDeviceCmd : public Command
{
public:


	PolledDeviceCmd(const char *cmd_name,
		        Tango::CmdArgType in,
		        Tango::CmdArgType out,
		        const char *desc);
	~PolledDeviceCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevPollStatus class
//
// description :	Class to implement the DevPollStatus command.
//			This class returns status of all commands and/or
//			attribute polled for a device
//
//=============================================================================


class DevPollStatusCmd : public Command
{
public:


	DevPollStatusCmd(const char *cmd_name,
		        Tango::CmdArgType in,
		        Tango::CmdArgType out,
			const char *in_desc,
		        const char *out_desc);
	~DevPollStatusCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The AddObjPolling class
//
// description :	Class to implement the AddObjPolling command.
//			This command add a new command/attribute in the list
//			of command/attribute to be polled
//
//=============================================================================


class AddObjPollingCmd : public Command
{
public:


	AddObjPollingCmd(const char *cmd_name,
		        Tango::CmdArgType in,
		        Tango::CmdArgType out,
			string &in_desc);
	~AddObjPollingCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The UpdObjPollingPeriod class
//
// description :	Class to implement the UpdObjPollingPeriod command.
//			This command updates an object update period
//
//=============================================================================


class UpdObjPollingPeriodCmd : public Command
{
public:


	UpdObjPollingPeriodCmd(const char *cmd_name,
		               Tango::CmdArgType in,
		               Tango::CmdArgType out,
			       string &in_desc);
	~UpdObjPollingPeriodCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The RemObjPolling class
//
// description :	Class to implement the RemObjPolling command.
//			This command removes one object of the device polling
//			list
//
//=============================================================================


class RemObjPollingCmd : public Command
{
public:


	RemObjPollingCmd(const char *cmd_name,
		         Tango::CmdArgType in,
		         Tango::CmdArgType out,
			 string &in_desc);
	~RemObjPollingCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};


//=============================================================================
//
//			The StopPolling class
//
// description :	Class to implement the StopPolling command.
//			This command stops the polling thread
//
//=============================================================================


class StopPollingCmd : public Command
{
public:


	StopPollingCmd(const char *cmd_name,
		       Tango::CmdArgType in,
		       Tango::CmdArgType out);
	~StopPollingCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The StartPolling class
//
// description :	Class to implement the StartPolling command.
//			This command starts the polling thread
//
//=============================================================================


class StartPollingCmd : public Command
{
public:


	StartPollingCmd(const char *cmd_name,
		       Tango::CmdArgType in,
		       Tango::CmdArgType out);
	~StartPollingCmd() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};


} // End of Tango namespace

#endif // _POLLCMDS_H

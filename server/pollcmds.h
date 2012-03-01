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
// $Log$
// Revision 3.4  2010/09/09 13:46:00  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.3  2009/01/21 12:47:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.2  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.1  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.0  2003/03/25 16:44:11  taurel
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

static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               PollCmds.cpp
//
// description :        C++ source for the DServerClass and for the
//			command class defined for this class. The singleton
//			class derived from DeviceClass. It implements the
//			command list and all properties and methods required
//			by the DServer once per process.
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <algorithm>

#include <tango.h>
#include <pollcmds.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		PolledDeviceCmd::PolledDeviceCmd
//
// description : 	constructors for Command class PolledDevice
//
//--------------------------------------------------------------------------

PolledDeviceCmd::PolledDeviceCmd(const char *name,
			         Tango::CmdArgType in,
			         Tango::CmdArgType out,
			         const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		PolledDeviceCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *PolledDeviceCmd::execute(DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "PolledDevice::execute(): arrived " << endl;

//
// Call the device method and return to caller
//

	return insert((static_cast<DServer *>(device))->polled_device());
}

//+-------------------------------------------------------------------------
//
// method : 		DevPollStatusCmd::DevPollStatusCmd
//
// description : 	constructors for Command class DevPollStatus
//
//--------------------------------------------------------------------------

DevPollStatusCmd::DevPollStatusCmd(const char *name,
			           Tango::CmdArgType in,
			           Tango::CmdArgType out,
			           const char *in_desc,
				   const char *out_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
	set_out_type_desc(out_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		DevPollStatusCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *DevPollStatusCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "DevPollStatus::execute(): arrived " << endl;

//
// Extract the input string
//

	const char *tmp_name;
	if ((in_any >>= tmp_name) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : string",
				        (const char *)"DevPollStatusCmd::execute");
	}
	string d_name(tmp_name);
	cout4 << "Received string = " << d_name << endl;

//
// Call the device method and return to caller
//

	return insert((static_cast<DServer *>(device))->dev_poll_status(d_name));
}


//+-------------------------------------------------------------------------
//
// method : 		AddObjPollingCmd::AddObjPollingCmd
//
// description : 	constructors for Command class DevPollStatus
//
//--------------------------------------------------------------------------

AddObjPollingCmd::AddObjPollingCmd(const char *name,
			           Tango::CmdArgType in,
			           Tango::CmdArgType out,
			           string &in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		AddObjPollingCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *AddObjPollingCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "AddObjPolling::execute(): arrived " << endl;

//
// Extract the input structure
//

	const DevVarLongStringArray *tmp_data;
	if ((in_any >>= tmp_data) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : DevVarLongStringArray",
				        (const char *)"AddObjPollingCmd::execute");
	}

//
// Call the device method and return to caller
//

	(static_cast<DServer *>(device))->add_obj_polling(tmp_data);

//
// Return to caller
//

	CORBA::Any *ret = return_empty_any("AddObjPolling");
	return ret;
}


//+-------------------------------------------------------------------------
//
// method : 		UpdObjPollingPeriodCmd::UpdObjPollingPeriodCmd
//
// description : 	constructors for Command class UpdObjPolledPeriod
//
//--------------------------------------------------------------------------

UpdObjPollingPeriodCmd::UpdObjPollingPeriodCmd(const char *name,
			           	       Tango::CmdArgType in,
			           	       Tango::CmdArgType out,
			           	       string &in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		UpdObjPollingPeriodCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *UpdObjPollingPeriodCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "UpdObjPollingPeriod::execute(): arrived " << endl;

//
// Extract the input structure
//

	const DevVarLongStringArray *tmp_data;
	if ((in_any >>= tmp_data) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : DevVarLongStringArray",
				        (const char *)"UpdObjPollingPeriodCmd::execute");
	}

//
// Call the device method and return to caller
//

	(static_cast<DServer *>(device))->upd_obj_polling_period(tmp_data);

//
// Return to caller
//

	CORBA::Any *ret = return_empty_any("UpdObjPollingPeriod");
	return ret;
}


//+-------------------------------------------------------------------------
//
// method : 		RemObjPollingCmd::RemObjPollingCmd
//
// description : 	constructors for Command class RemObjPolled
//
//--------------------------------------------------------------------------

RemObjPollingCmd::RemObjPollingCmd(const char *name,
			           Tango::CmdArgType in,
			           Tango::CmdArgType out,
			           string &in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		RemObjPollingCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *RemObjPollingCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "RemObjPolling::execute(): arrived " << endl;

//
// Extract the input structure
//

	const DevVarStringArray *tmp_data;
	if ((in_any >>= tmp_data) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : DevVarStringArray",
				        (const char *)"RemObjPollingCmd::execute");
	}

//
// Call the device method and return to caller
//

	(static_cast<DServer *>(device))->rem_obj_polling(tmp_data);

//
// Return to caller
//

	CORBA::Any *ret = return_empty_any("RemObjPolling");
	return ret;
}

//+-------------------------------------------------------------------------
//
// method : 		StartPollingCmd::StartPollingCmd
//
// description : 	constructors for Command class RemObjPolled
//
//--------------------------------------------------------------------------

StopPollingCmd::StopPollingCmd(const char *name,
			       Tango::CmdArgType in,
			       Tango::CmdArgType out):Command(name,in,out)
{
}


//+-------------------------------------------------------------------------
//
// method : 		StartPollingCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *StopPollingCmd::execute(DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "StopPolling::execute(): arrived " << endl;

//
// Call the device method and return to caller
//

	(static_cast<DServer *>(device))->stop_polling();

//
// Return to caller
//

	CORBA::Any *ret = return_empty_any("StopPolling");
	return ret;
}

//+-------------------------------------------------------------------------
//
// method : 		StartPollingCmd::StartPollingCmd
//
// description : 	constructors for Command class RemObjPolled
//
//--------------------------------------------------------------------------

StartPollingCmd::StartPollingCmd(const char *name,
			         Tango::CmdArgType in,
			         Tango::CmdArgType out):Command(name,in,out)
{
}


//+-------------------------------------------------------------------------
//
// method : 		StartPollingCmd::execute
//
// description : 	Trigger the execution of the method really implemented
//			the command in the DServer class
//
//--------------------------------------------------------------------------

CORBA::Any *StartPollingCmd::execute(DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "StartPolling::execute(): arrived " << endl;

//
// Call the device method and return to caller
//

	(static_cast<DServer *>(device))->start_polling();

//
// Return to caller
//

	CORBA::Any *ret = return_empty_any("StartPolling");
	return ret;
}

} // End of Tango namespace

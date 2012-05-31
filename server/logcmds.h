//=============================================================================
//
// file :	  LogCmds.h
//
// description :  Logging oriented commands of the DServerClass.
//
// project :	  TANGO
//
// author(s) :	  N.Leclercq
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

#ifdef TANGO_HAS_LOG4TANGO

#ifndef _LOG_CMDS_H
#define _LOG_CMDS_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The AddLoggingTarget class
//
// description :	Class implementing the AddLoggingTarget command.
//			This command adds one (or more) logging target to one (or more)
//	device(s) running within the same Device server.
//
//=============================================================================
class AddLoggingTarget : public Command
{
public:
	AddLoggingTarget (const char *cmd_name,
				Tango::CmdArgType in,
				Tango::CmdArgType out,
				const std::string &in_desc);

	~AddLoggingTarget() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The RemoveLoggingTarget class
//
// description :	Class implementing the RemoveLoggingTarget command.
//			This command removes one (or more) logging target to one (or more)
//	device(s) running within the same Device server.
//
//=============================================================================
class RemoveLoggingTarget : public Command
{
public:
	RemoveLoggingTarget (const char *cmd_name,
				   Tango::CmdArgType in,
				   Tango::CmdArgType out,
				   const std::string &in_desc);

	~RemoveLoggingTarget() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The GetLoggingTarget class
//
// description :	Class implementing the GetLoggingTarget command.
//			This command returns the logging target list of a device.
//
//=============================================================================
class GetLoggingTarget : public Command
{
public:
	GetLoggingTarget (const char *cmd_name,
										Tango::CmdArgType in,
		    Tango::CmdArgType out,
		    const std::string &in_desc,
		    const std::string &out_desc);

	~GetLoggingTarget() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The SetLoggingLevel class
//
// description :	Class implementing the SetLoggingLevel command.
//			This command set the logging level of one or more device(s).
//
//=============================================================================
class SetLoggingLevel : public Command
{
public:
	SetLoggingLevel (const char *cmd_name,
		   Tango::CmdArgType in,
		   Tango::CmdArgType out,
		   const std::string &in_desc);

	~SetLoggingLevel() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The GetLoggingLevel class
//
// description :	Class implementing the GetLoggingLevel command.
//			This command returns the logging level of one or more device(s).
//
//=============================================================================
class GetLoggingLevel : public Command
{
public:
	GetLoggingLevel (const char *cmd_name,
									  Tango::CmdArgType in,
		    Tango::CmdArgType out,
		    const std::string &in_desc,
		    const std::string &out_desc);

	~GetLoggingLevel() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The StopLogging class
//
// description :	Class implementing the StopLogging command.
//			This command disable the logging for all the devices.
//
//=============================================================================
class StopLogging : public Command
{
public:
	StopLogging (const char *cmd_name,
						 Tango::CmdArgType in,
	       Tango::CmdArgType out);

	~StopLogging() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};

//=============================================================================
//
//			The StartLogging class
//
// description :	Class implementing the StartLogging command.
//			This command enable the logging for all the devices.
//
//=============================================================================
class StartLogging : public Command
{
public:
	StartLogging (const char *cmd_name,
						 Tango::CmdArgType in,
	       Tango::CmdArgType out);

	~StartLogging() {};

	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};


} // End of Tango namespace

#endif // _LOG_CMDS_H

#endif // TANGO_HAS_LOG4TANGO

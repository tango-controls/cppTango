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
// Revision 3.3  2009/01/21 12:47:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.2  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.1  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.0  2003/03/25 16:44:07  taurel
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
// Revision 2.2  2003/03/11 17:55:54  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:42  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
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

//=============================================================================
//
// file :		BasicCommand.h
//
// description :	Include for commands which are implemented in all
//			classes. Classes for 2 commands are defined here:
//				DevStatus for the DevStatus command
//				DevState for the DevState command
//			All these classes inherits from the Command class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
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

#ifndef _BASICCOMMAND_H
#define _BASICCOMMAND_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The DevStatusCmd class
//
// description :	Class to implement the DevStatus command. This command
//			does not take any input argument and return the device
//			state as a string
//
//=============================================================================

class DevStatusCmd : public Command
{
public:
	DevStatusCmd(const char *cmd_name,
		     Tango::CmdArgType in,
		     Tango::CmdArgType out);
	~DevStatusCmd() {};

	virtual CORBA::Any * execute(DeviceImpl *device,const CORBA::Any &in_any);
};


//=============================================================================
//
//			The DevState class
//
// description :	Class to implement the DevState command. This command
//			does not take any input argument and return the device
//			state as an enumerated type
//
//=============================================================================


class DevStateCmd : public Command
{
public:

	DevStateCmd(const char *cmd_name,
		    Tango::CmdArgType in,
		    Tango::CmdArgType out);
	~DevStateCmd() {};

	virtual CORBA::Any * execute(DeviceImpl *device,const CORBA::Any &in_any);
};

//=============================================================================
//
//			The DevInit class
//
// description :	Class to implement the Init command. This command
//			does not take any input argument and output argument.
//			It will call the device delete_device method and its
//			init_device method.
//
//=============================================================================


class DevInitCmd : public Command
{
public:

	DevInitCmd(const char *cmd_name,
		    Tango::CmdArgType in,
		    Tango::CmdArgType out);
	~DevInitCmd() {};

	virtual CORBA::Any * execute(DeviceImpl *device,const CORBA::Any &in_any);
};

} // End of Tango namespace

#endif // _BASICCOMMAND_H

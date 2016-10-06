static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :	  LogCmds.cpp
//
// description :  Logging oriented commands of the DServerClass.
//
// project :	  TANGO
//
// author(s) :	  N.Leclercq
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
// Revision 3.5  2010/09/09 13:46:00  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.4  2009/01/21 12:47:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.3  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.2  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 3.1  2003/05/28 14:55:09  taurel
// Add the include (conditionally) of the include files generated by autoconf
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
// Revision 2.3  2003/03/19 10:41:32  nleclercq
// *** empty log message ***
//
// Revision 2.2  2003/03/11 17:55:54  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.1  2003/02/17 14:57:42  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#ifdef TANGO_HAS_LOG4TANGO

#include <tango/server/logcmds.h>

namespace Tango {

//+-------------------------------------------------------------------------
//
// method :		AddLoggingTarget::AddLoggingTarget
//
// description :  Constructor for Command class AddLoggingTarget
//
//--------------------------------------------------------------------------
AddLoggingTarget::AddLoggingTarget (const char *name,
						      Tango::CmdArgType in,
						      Tango::CmdArgType out,
				    const std::string &in_desc)
 :  Command(name, in, out)
{
  set_in_type_desc(const_cast<std::string&>(in_desc));
}

//+-------------------------------------------------------------------------
//
// method :		AddLoggingTarget::execute
//
// description :	Trigger the execution of the method implementing
//			the AddLoggingTarget command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *AddLoggingTarget::execute (DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "AddLoggingTarget::execute(): arrived " << endl;

//
// Extract the input data
//
	const DevVarStringArray *targets;
	if ((in_any >>= targets) == false)
	{
		Except::throw_exception((const char *)API_IncompatibleCmdArgumentType,
					(const char *)"Imcompatible command argument type, expected type is : DevVarStringArray",
					(const char *)"AddLoggingTarget::execute");
	}

//
// Call the concrete device method
//
  (static_cast<DServer *>(device))->add_logging_target(targets);

//
// Return to caller
//
	CORBA::Any *ret = return_empty_any("AddLoggingTarget");
	return ret;
}

//+-------------------------------------------------------------------------
//
// method :		RemoveLoggingTarget::RemoveLoggingTarget
//
// description :  Constructor for Command class RemoveLoggingTarget
//
//--------------------------------------------------------------------------
RemoveLoggingTarget::RemoveLoggingTarget (const char *name,
							    Tango::CmdArgType in,
							    Tango::CmdArgType out,
							    const std::string &in_desc)
 :  Command(name, in, out)
{
  set_in_type_desc(const_cast<std::string&>(in_desc));
}

//+-------------------------------------------------------------------------
//
// method :		RemoveLoggingTarget::execute
//
// description :	Trigger the execution of the method implementing
//			the RemoveLoggingTarget command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *RemoveLoggingTarget::execute (DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "RemoveLoggingTarget::execute(): arrived " << endl;

//
// Extract the input data
//
	const DevVarStringArray *targets;
	if ((in_any >>= targets) == false)
	{
		Except::throw_exception((const char *)API_IncompatibleCmdArgumentType,
					(const char *)"Imcompatible command argument type, expected type is : DevVarStringArray",
					(const char *)"RemoveLoggingTarget::execute");
	}

//
// Call the concrete device method
//
  (static_cast<DServer *>(device))->remove_logging_target(targets);

//
// Return to caller
//
	CORBA::Any *ret = return_empty_any("RemoveLoggingTarget");
	return ret;
}

//+-------------------------------------------------------------------------
//
// method :		GetLoggingTarget::GetLoggingTarget
//
// description :  Constructor for Command class GetLoggingTarget
//
//--------------------------------------------------------------------------
GetLoggingTarget::GetLoggingTarget (const char *name,
																		Tango::CmdArgType in,
				    Tango::CmdArgType out,
				    const std::string &in_desc,
				    const std::string &out_desc)
 :  Command(name, in, out)
{
  set_in_type_desc(const_cast<std::string&>(in_desc));
  set_out_type_desc(const_cast<std::string&>(out_desc));
}

//+-------------------------------------------------------------------------
//
// method :		GetLoggingTarget::execute
//
// description :	Trigger the execution of the method implementing
//			the GetLoggingTarget command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *GetLoggingTarget::execute (DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "GetLoggingTarget::execute(): arrived " << endl;

//
// Extract the input data
//
	const char* tmp_str;
	if ((in_any >>= tmp_str) == false)
	{
		Except::throw_exception((const char *)API_IncompatibleCmdArgumentType,
					(const char *)"Imcompatible command argument type, expected type is : DevString",
					(const char *)"GetLoggingTarget::execute");
	}

//
// Return to caller
//
	return insert((static_cast<DServer *>(device))->get_logging_target(std::string(tmp_str)));
}

//+-------------------------------------------------------------------------
//
// method :		SetLoggingLevel::SetLoggingLevel
//
// description :  Constructor for Command class SetLoggingLevel
//
//--------------------------------------------------------------------------
SetLoggingLevel::SetLoggingLevel   (const char *name,
																		Tango::CmdArgType in,
				    Tango::CmdArgType out,
				    const std::string &in_desc)
 :  Command(name, in, out)
{
  set_in_type_desc(const_cast<std::string&>(in_desc));
}

//+-------------------------------------------------------------------------
//
// method :		SetLoggingLevel::execute
//
// description :	Trigger the execution of the method implementing
//			the SetLoggingLevel command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *SetLoggingLevel::execute (DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "SetLoggingLevel::execute(): arrived " << endl;

//
// Extract the input data
//
	const DevVarLongStringArray *argin;
	if ((in_any >>= argin) == false)
	{
		Except::throw_exception((const char *)API_IncompatibleCmdArgumentType,
					(const char *)"Imcompatible command argument type, expected type is : DevVarLongStringArray",
					(const char *)"SetLoggingLevel::execute");
	}

//
// Call the concrete device method
//
	(static_cast<DServer *>(device))->set_logging_level(argin);

//
// Return to caller
//
	CORBA::Any *ret = return_empty_any("StartLogging");
	return ret;
}

//+-------------------------------------------------------------------------
//
// method :		GetLoggingLevel::GetLoggingLevel
//
// description :  Constructor for Command class GetLoggingLevel
//
//--------------------------------------------------------------------------
GetLoggingLevel::GetLoggingLevel   (const char *name,
																		Tango::CmdArgType in,
				    Tango::CmdArgType out,
				    const std::string &in_desc,
				    const std::string &out_desc)
 :  Command(name, in, out)
{
  set_in_type_desc(const_cast<std::string&>(in_desc));
  set_out_type_desc(const_cast<std::string&>(out_desc));
}

//+-------------------------------------------------------------------------
//
// method :		GetLoggingLevel::execute
//
// description :	Trigger the execution of the method implementing
//			the GetLoggingLevel command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *GetLoggingLevel::execute (DeviceImpl *device, const CORBA::Any &in_any)
{
	cout4 << "GetLoggingLevel::execute(): arrived " << endl;

//
// Extract the input data
//
	const DevVarStringArray *argin;
	if ((in_any >>= argin) == false)
	{
		Except::throw_exception((const char *)API_IncompatibleCmdArgumentType,
					(const char *)"Imcompatible command argument type, expected type is : DevVarStringArray",
					(const char *)"GetLoggingLevel::execute");
	}

//
// Return to caller
//
	return insert((static_cast<DServer *>(device))->get_logging_level(argin));
}


//+-------------------------------------------------------------------------
//
// method :		StopLogging::StopLogging
//
// description :  Constructor for Command class StopLogging
//
//--------------------------------------------------------------------------
StopLogging::StopLogging(const char *name,
													Tango::CmdArgType in,
			  Tango::CmdArgType out)
 :  Command(name, in, out)
{

}

//+-------------------------------------------------------------------------
//
// method :		StopLoggingLevel::execute
//
// description :	Trigger the execution of the method implementing
//			the StopLogging command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *StopLogging::execute (DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout4 << "StopLogging::execute(): arrived " << endl;

//
// Call the concrete device method
//
	(static_cast<DServer *>(device))->stop_logging();

//
// Return to caller
//
	CORBA::Any *ret = return_empty_any("StopLogging");
	return ret;
}

//+-------------------------------------------------------------------------
//
// method :		StartLogging::StartLogging
//
// description :  Constructor for Command class StartLogging
//
//--------------------------------------------------------------------------
StartLogging::StartLogging (const char *name,
													  Tango::CmdArgType in,
			    Tango::CmdArgType out)
 : Command(name, in, out)
{

}

//+-------------------------------------------------------------------------
//
// method :		StartLogging::execute
//
// description :	Trigger the execution of the method implementing
//			the StartLogging command of the DServerClass
//
//--------------------------------------------------------------------------
CORBA::Any *StartLogging::execute (DeviceImpl *device, TANGO_UNUSED(const CORBA::Any &in_any))
{
	cout4 << "StartLogging::execute(): arrived " << endl;

//
// Call the concrete device method
//
	(static_cast<DServer *>(device))->start_logging();

//
// Return to caller
//
	CORBA::Any *ret = return_empty_any("StartLogging");
	return ret;
}

} // namespace Tango

#endif // TANGO_HAS_LOG4TANGO

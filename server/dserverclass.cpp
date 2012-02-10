static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :               DServerClass.cpp
//
// description :        C++ source for the DServerClass and for the
//			command class defined for this class. The singleton
//			class derived from DeviceClass. It implements the
//			command list and all properties and methods required
//			by the DServer once per process.
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <algorithm>

#include <tango.h>
#include <dserverclass.h>
#include <pollcmds.h>
#ifdef TANGO_HAS_LOG4TANGO
#include <logcmds.h>
#endif
#include <eventcmds.h>
#include <eventsupplier.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		DevRestartCmd::DevRestartCmd
//
// description : 	constructors for Command class Restart
//
//--------------------------------------------------------------------------

DevRestartCmd::DevRestartCmd(const char *name,
			     Tango::CmdArgType in,
			     Tango::CmdArgType out,
			     const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+-------------------------------------------------------------------------
//
// method : 		DevRestartCmd::execute
//
// description : 	restart a device
//
//--------------------------------------------------------------------------

CORBA::Any *DevRestartCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "DevRestart::execute(): arrived " << endl;

//
// Extract the input string
//

	const char *tmp_name;
	if ((in_any >>= tmp_name) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : string",
				        (const char *)"DevRestartCmd::execute");
	}
	string d_name(tmp_name);
	cout4 << "Received string = " << d_name << endl;

//
// call DServer method which implements this command
//

	((DServer *)device)->restart(d_name);

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevRestartCmd");
	return ret;
}

//+----------------------------------------------------------------------------
//
// method : 		DevRestartServerCmd::DevRestartServerCmd()
//
// description : 	constructor for the DevRestartServerCmd command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevRestartServerCmd::DevRestartServerCmd(const char *name,
	  	       			Tango::CmdArgType in,
		       			Tango::CmdArgType out):Command(name,in,out)
{
}

//+----------------------------------------------------------------------------
//
// method : 		DevRestartServerCmd::execute(string &s)
//
// description : 	method to trigger the execution of the DevRestartServer
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevRestartServerCmd::execute(DeviceImpl *device,TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevRestartServerCmd::execute(): arrived" << endl;

//
// call DServer method which implements this command
//

	((DServer *)device)->restart_server();

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevRestartServerCmd");
	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		DevQueryClassCmd::DevQueryClassCmd()
//
// description : 	constructor for the DevQueryClass command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevQueryClassCmd::DevQueryClassCmd(const char *name,
			     	   Tango::CmdArgType in,
			     	   Tango::CmdArgType out,
				   const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevQueryClassCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "QueryClass"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevQueryClassCmd::execute(DeviceImpl *device,TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevQueryClassCmd::execute(): arrived" << endl;

//
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_class();

//
// return data to the caller
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in DevQueryClassCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevQueryClassCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving DevQueryClassCmd::execute()" << endl;
	return(out_any);
}


//+----------------------------------------------------------------------------
//
// method : 		DevQueryDeviceCmd::DevQueryDeviceCmd()
//
// description : 	constructor for the DevQueryDevice command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevQueryDeviceCmd::DevQueryDeviceCmd(const char *name,
			       	     Tango::CmdArgType in,
			       	     Tango::CmdArgType out,
				     const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevQueryDeviceCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "QueryDevice"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevQueryDeviceCmd::execute(DeviceImpl *device,TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevQueryDeviceCmd::execute(): arrived" << endl;

//
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_device();

//
// return data to the caller
//
	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in DevQueryDeviceCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevQueryDeviceCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving DevQueryDeviceCmd::execute()" << endl;
	return(out_any);
}

//+----------------------------------------------------------------------------
//
// method : 		DevQuerySubDeviceCmd::DevQuerySubDeviceCmd()
//
// description : 	constructor for the DevQuerySubDevice command of the
//					DServer.
//
//-----------------------------------------------------------------------------

DevQuerySubDeviceCmd::DevQuerySubDeviceCmd(const char *name,
			       	     Tango::CmdArgType in,
			       	     Tango::CmdArgType out,
				     const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevQuerySubDeviceCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "QuerySubDevice"
//					command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevQuerySubDeviceCmd::execute(DeviceImpl *device,TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevQuerySubDeviceCmd::execute(): arrived" << endl;

//
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_sub_device();

//
// return data to the caller
//
	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in DevQuerySubDeviceCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevQuerySubDeviceCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving DevQuerySubDeviceCmd::execute()" << endl;
	return(out_any);
}




//+----------------------------------------------------------------------------
//
// method : 		DevKillCmd::DevKillCmd()
//
// description : 	constructor for the DevKill command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevKillCmd::DevKillCmd(const char *name,
	  	       Tango::CmdArgType in,
		       Tango::CmdArgType out):Command(name,in,out)
{
}

//+----------------------------------------------------------------------------
//
// method : 		DevKillCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "Kill"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevKillCmd::execute(DeviceImpl *device,TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevKillCmd::execute(): arrived" << endl;

//
// call DServer method which implements this command
//

	((DServer *)device)->kill();

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevKillCmd");
	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceLevelCmd::DevSetTraceLevelCmd()
//
// description : 	constructor for the DevSetTraceLevel command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevSetTraceLevelCmd::DevSetTraceLevelCmd(const char *name,
			     	   	 Tango::CmdArgType in,
			     	   	 Tango::CmdArgType out,
					 const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceLevelCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "SetTraceLevel"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevSetTraceLevelCmd::execute(TANGO_UNUSED(DeviceImpl *device),TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevSetTraceLevelCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO
  	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"SetTraceLevel is no more supported, please use SetLoggingLevel",
      				(const char *)"DevSetTraceLevelCmd::execute");
//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevSetTraceLevelCmd");
	return ret;

#else // TANGO_HAS_LOG4TANGO

//
// Get new level
//

	int new_level;
	if ((in_any >>= new_level) == false)
	{
		cout3 << "DevSetTraceLevelCmd::execute() --> Wrong argument type" << endl;
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				      (const char *)"Imcompatible command argument type, expected type is : long",
				      (const char *)"DevSetTraceLevelCmd::execute");
	}

//
// Set new level
//

	Tango::Util::instance()->set_trace_level(new_level);

//
// Return to the caller
//

	CORBA::Any *ret = return_empty_any("DevSetTraceLevelCmd");
	return ret;
#endif
}


//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceLevelCmd::DevGetTraceLevelCmd()
//
// description : 	constructor for the DevGetTraceLevel command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevGetTraceLevelCmd::DevGetTraceLevelCmd(const char *name,
			     	   	 Tango::CmdArgType in,
			     	   	 Tango::CmdArgType out,
					 const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceLevelCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "DevGetTraceLevel"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevGetTraceLevelCmd::execute(TANGO_UNUSED(DeviceImpl *device),TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevGetTraceLevelCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO

  	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"GetTraceLevel is no more supported, please use GetLoggingLevel",
      				(const char *)"DevGetTraceLevelCmd::execute");

//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevGetTraceLevelCmd");
	return ret;

#else // TANGO_HAS_LOG4TANGO

//
// Get level
//

	int level = Tango::Util::instance()->get_trace_level();

//
// return data to the caller
//

	CORBA::Any *out_any = new CORBA::Any();
	(*out_any) <<= level;

	cout4 << "Leaving DevGetTraceLevelCmd::execute()" << endl;
	return(out_any);
#endif
}


//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceOutputCmd::DevGetTraceOutputCmd()
//
// description : 	constructor for the DevGetTraceoutput command of the
//			DServer.
//
//-----------------------------------------------------------------------------

DevGetTraceOutputCmd::DevGetTraceOutputCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}

//+----------------------------------------------------------------------------
//
// method : 		DevGetTraceOutputCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "DevGetTraceOutput"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevGetTraceOutputCmd::execute(TANGO_UNUSED(DeviceImpl *device),TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevGetTraceOutputCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO
	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"GetTraceOutput is no more supported, please use GetLoggingTarget",
      				(const char *)"DevGetTraceOutputCmd::execute");
//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevGetTraceOutputCmd");
	return ret;

#else

//
// Get Trace output
//

	string st = Tango::Util::instance()->get_trace_output();

//
// return data to the caller
//

	CORBA::Any *out_any = new CORBA::Any();
	(*out_any) <<= st.c_str();

	cout4 << "Leaving DevGetTraceOutputCmd::execute()" << endl;
	return(out_any);
#endif
}

//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceOutputCmd::DevSetTraceOutputCmd()
//
// description : 	constructor for the DevSetTraceoutput command of the
//			DServer.
//
//-----------------------------------------------------------------------------


DevSetTraceOutputCmd::DevSetTraceOutputCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		DevSetTraceOutputCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "Kill"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevSetTraceOutputCmd::execute(TANGO_UNUSED(DeviceImpl *device),TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "DevSetTraceOutputCmd::execute(): arrived" << endl;

#ifdef TANGO_HAS_LOG4TANGO

	Except::throw_exception((const char *)"API_DeprecatedCommand",
      				(const char *)"SetTraceOutput is no more supported, please use AddLoggingTarget",
      				(const char *)"DevSetTraceOutputCmd::execute");
//
// Make the compiler happy
//

	CORBA::Any *ret = return_empty_any("DevSetTraceOutputCmd");
	return ret;

#else

//
// Extract the input string and try to create a output file stream from it
//

	const char *in_file_ptr;
	if ((in_any >>= in_file_ptr) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				      (const char *)"Imcompatible command argument type, expected type is : string",
				      (const char *)"DevSetTraceOutputCmd::execute");
	}
	string in_file(in_file_ptr);
	cout4 << "Received string = " << in_file << endl;

	if (in_file == InitialOutput)
	{
		delete(Tango::Util::instance()->get_trace_output_stream());
		Tango::Util::instance()->set_trace_output_stream((ofstream *)NULL);

		ostream &tmp_stream = Tango::Util::instance()->get_out();

//
// For windows, the stdc++ library also implements the new IOstreams where the
// xx_with_assign classes do not exist. To copy stream, I have used the advice
// from the C++ report of June 1997
//

		cout.copyfmt(tmp_stream);
		cout.clear(tmp_stream.rdstate());
		cout.rdbuf(tmp_stream.rdbuf());

		Tango::Util::instance()->set_trace_output(in_file);
	}
	else
	{
		ofstream *ofp = new ofstream(in_file_ptr);
		if (ofp->good())
		{
			cout.copyfmt(*ofp);
			cout.clear(ofp->rdstate());
			cout.rdbuf(ofp->rdbuf());

			delete(Tango::Util::instance()->get_trace_output_stream());
			Tango::Util::instance()->set_trace_output(in_file);
			Tango::Util::instance()->set_trace_output_stream(ofp);
		}
		else
		{
			cout3 << "Cannot open ofstream" << endl;
			TangoSys_OMemStream o;

			o << "Impossible to open file " << in_file << ends;
			Except::throw_exception((const char *)"API_CannotOpenFile",
					      o.str(),
					      (const char *)"DevSetTraceoutput::execute");
		}
	}

//
// return to the caller
//

	CORBA::Any *ret = return_empty_any("DevSetTraceOutputCmd");
	return ret;
#endif
}

//+----------------------------------------------------------------------------
//
// method : 		QueryWizardClassPropertyCmd::QueryWizardClassPropertyCmd
//
// description : 	constructor for the QueryWizardClassProperty command of the
//			DServer.
//
//-----------------------------------------------------------------------------


QueryWizardClassPropertyCmd::QueryWizardClassPropertyCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *in_desc,
					   const char *out_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
	set_out_type_desc(out_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		QueryWizardClassPropertyCmd::execute(string &s)
//
// description : 	method to trigger the execution of the "QueryWizardClassProperty"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *QueryWizardClassPropertyCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "QueryWizardClassPropertyCmd::execute(): arrived" << endl;

//
// Extract the input string
//

	const char *tmp_name;
	if ((in_any >>= tmp_name) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : string",
				        (const char *)"QueryWizardClassPropertyCmd::execute");
	}
	string class_name(tmp_name);

//
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_class_prop(class_name);

//
// return data to the caller
//
	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in QueryWizardClassPropertyCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"QueryWizardClassPropertyCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving QueryWizardClassPropertyCmd::execute()" << endl;
	return(out_any);
}


//+----------------------------------------------------------------------------
//
// method : 		QueryWizardDevPropertyCmd::QueryWizardDevPropertyCmd
//
// description : 	constructor for the QueryWizardDevProperty command of the
//			DServer.
//
//-----------------------------------------------------------------------------


QueryWizardDevPropertyCmd::QueryWizardDevPropertyCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *in_desc,
					   const char *out_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
	set_out_type_desc(out_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		QueryWizardDevPropertyCmd::execute()
//
// description : 	method to trigger the execution of the "QueryWizardDevProperty"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *QueryWizardDevPropertyCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "QueryWizardDevPropertyCmd::execute(): arrived" << endl;

//
// Extract the input string
//

	const char *tmp_name;
	if ((in_any >>= tmp_name) == false)
	{
		Except::throw_exception((const char *)"API_IncompatibleCmdArgumentType",
				        (const char *)"Imcompatible command argument type, expected type is : string",
				        (const char *)"QueryWizardDevPropertyCmd::execute");
	}
	string class_name(tmp_name);

//
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_dev_prop(class_name);

//
// return data to the caller
//
	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in QueryWizardDevPropertyCmd::execute()" << endl;
		delete ret;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"QueryWizardDevPropertyCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving QueryWizardDevPropertyCmd::execute()" << endl;
	return(out_any);
}

//+----------------------------------------------------------------------------
//
// method : 		QueryEventChannelIORCmd::QueryEventChannelIORCmd
//
// description : 	constructor for the QueryEventChannelIOR command of the
//			DServer.
//
//-----------------------------------------------------------------------------


QueryEventChannelIORCmd::QueryEventChannelIORCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *out_desc):Command(name,in,out)
{
	set_out_type_desc(out_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		QueryEventChannelIORCmd::execute()
//
// description : 	method to trigger the execution of the "QueryEventChannelIOR"
//			command
//
//-----------------------------------------------------------------------------

CORBA::Any *QueryEventChannelIORCmd::execute(TANGO_UNUSED(DeviceImpl *device),TANGO_UNUSED(const CORBA::Any &in_any))
{

	cout4 << "QueryEventChannelIORCmd::execute(): arrived" << endl;

//
// Get DS event channel IOR which is stored in the EventSupplier object
//

	CORBA::Any *out_any = NULL;
	NotifdEventSupplier *nd_event_supplier;
	nd_event_supplier = Util::instance()->get_notifd_event_supplier();
	if (nd_event_supplier == NULL)
	{
		cout3 << "Try to retrieve DS event channel while NotifdEventSupplier object is not yet created" << endl;

		Except::throw_exception((const char *)"API_EventSupplierNotConstructed",
				      	(const char *)"Try to retrieve DS event channel while EventSupplier object is not created",
				      	(const char *)"QueryEventChannelIORCmd::execute");
	}
	else
	{

		string &ior = nd_event_supplier->get_event_channel_ior();

//
// return data to the caller
//

		try
		{
			out_any = new CORBA::Any();
		}
		catch (bad_alloc)
		{
			cout3 << "Bad allocation while in QueryEventChannelIORCmd::execute()" << endl;
			Except::throw_exception((const char *)"API_MemoryAllocation",
				      		(const char *)"Can't allocate memory in server",
				      		(const char *)"QueryEventChannelIORCmd::execute");
		}
		(*out_any) <<= ior.c_str();
	}

	cout4 << "Leaving QueryEventChannelIORCmd::execute()" << endl;
	return(out_any);
}


//+----------------------------------------------------------------------------
//
// method : 		LockDeviceCmd::LockDeviceCmd
//
// description : 	constructor for the LockDevice command of the DServer.
//
//-----------------------------------------------------------------------------


LockDeviceCmd::LockDeviceCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
			     	     	   const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		LockDeviceCmd::execute()
//
// description : 	method to trigger the execution of the "LockDevice" command
//
//-----------------------------------------------------------------------------

CORBA::Any *LockDeviceCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "LockDeviceCmd::execute(): arrived" << endl;

//
// Extract the input argument
//

	const Tango::DevVarLongStringArray *in_data;
	extract(in_any,in_data);

//
// call DServer method which implements this command
//

	((DServer *)device)->lock_device(in_data);

//
// return data to the caller
//

	CORBA::Any *ret = return_empty_any("LockDeviceCmd");
	return ret;
}


//+----------------------------------------------------------------------------
//
// method : 		ReLockDevicesCmd::ReLockDeviceCmd
//
// description : 	constructor for the ReLockDevices command of the DServer.
//
//-----------------------------------------------------------------------------


ReLockDevicesCmd::ReLockDevicesCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   const char *in_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		ReLockDevicesCmd::execute()
//
// description : 	method to trigger the execution of the "ReLockDevices" command
//
//-----------------------------------------------------------------------------

CORBA::Any *ReLockDevicesCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "ReLockDevicesCmd::execute(): arrived" << endl;

//
// Extract the input argument
//

	const Tango::DevVarStringArray *in_data;
	extract(in_any,in_data);

//
// call DServer method which implements this command
//

	((DServer *)device)->re_lock_devices(in_data);

//
// return data to the caller
//

	CORBA::Any *ret = return_empty_any("ReLockDevicesCmd");
	return ret;

}

//+----------------------------------------------------------------------------
//
// method : 		UnLockDeviceCmd::UnLockDeviceCmd
//
// description : 	constructor for the UnLockDevice command of the DServer.
//
//-----------------------------------------------------------------------------


UnLockDeviceCmd::UnLockDeviceCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   		   const char *in_desc,
					   		   const char *out_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
	set_out_type_desc(out_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		UnLockDeviceCmd::execute()
//
// description : 	method to trigger the execution of the "UnLockDevice" command
//
//-----------------------------------------------------------------------------

CORBA::Any *UnLockDeviceCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "UnLockDeviceCmd::execute(): arrived" << endl;

//
// Extract the input string
//

	const Tango::DevVarLongStringArray *in_data;
	extract(in_any,in_data);

//
// call DServer method which implements this command
//

	Tango::DevLong ret = ((DServer *)device)->un_lock_device(in_data);

//
// return data to the caller
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in UnLockDeviceCmd::execute()" << endl;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"UnLockDeviceCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving UnLockDeviceCmd::execute()" << endl;
	return(out_any);
}

//+----------------------------------------------------------------------------
//
// method : 		DevLockStatusCmd::DevLockStatusCmd
//
// description : 	constructor for the DevLockStatus command of the DServer.
//
//-----------------------------------------------------------------------------


DevLockStatusCmd::DevLockStatusCmd(const char *name,
			     	     	   Tango::CmdArgType in,
			     	     	   Tango::CmdArgType out,
					   		   const char *in_desc,
					   		   const char *out_desc):Command(name,in,out)
{
	set_in_type_desc(in_desc);
	set_out_type_desc(out_desc);
}


//+----------------------------------------------------------------------------
//
// method : 		DevLockStatusCmd::execute()
//
// description : 	method to trigger the execution of the "DevLockStatus" command
//
//-----------------------------------------------------------------------------

CORBA::Any *DevLockStatusCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{

	cout4 << "DevLockStatusCmd::execute(): arrived" << endl;

//
// Extract the input string
//

	Tango::ConstDevString in_data;
	extract(in_any,in_data);

//
// call DServer method which implements this command
//

	Tango::DevVarLongStringArray *ret = ((DServer *)device)->dev_lock_status(in_data);

//
// return to the caller
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in DevLockStatusCmd::execute()" << endl;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevLockStatusCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving DevLockStatusCmd::execute()" << endl;
	return(out_any);
}


//+----------------------------------------------------------------------------
//
// method : 		EventSubscriptionChangeCmd::EventSubscriptionChangeCmd()
//
// description : 	constructor for the command of the EventTester.
//
// In : - name : The command name
//		- in : The input parameter type
//		- out : The output parameter type
//		- in_desc : The input parameter description
//		- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------
EventSubscriptionChangeCmd::EventSubscriptionChangeCmd(const char *name,
								Tango::CmdArgType in,
								Tango::CmdArgType out,
								const char *in_desc,
								const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//
//	Constructor without in/out parameters description
//

EventSubscriptionChangeCmd::EventSubscriptionChangeCmd(const char *name,Tango::CmdArgType in,Tango::CmdArgType out)
:Command(name,in,out)
{
}

//+----------------------------------------------------------------------------
//
// method : 		EventSubscriptionChangeCmd::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------
bool EventSubscriptionChangeCmd::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return true;
}

//+----------------------------------------------------------------------------
//
// method : 		EventSubscriptionChangeCmd::execute()
//
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *EventSubscriptionChangeCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
    cout4 << "EventSubscriptionChangeCmd::execute(): arrived" << endl;

//
// Extract the input string array
//

	const Tango::DevVarStringArray *in_data;
	extract(in_any,in_data);

//
// call DServer method which implements this command
//

	Tango::DevLong ret = ((DServer *)device)->event_subscription_change(in_data);

//
// return to the caller
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in EventSubscriptionChangeCmd::execute()" << endl;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"EventSubscriptionChangeCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving EventSubscriptionChangeCmd::execute()" << endl;
	return(out_any);
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSubscriptionChangeCmd::EventSubscriptionChangeCmd()
//
// description : 	constructor for the command of the .
//
// In : - name : The command name
//		- in : The input parameter type
//		- out : The output parameter type
//		- in_desc : The input parameter description
//		- out_desc : The output parameter description
//
//-----------------------------------------------------------------------------
ZmqEventSubscriptionChangeCmd::ZmqEventSubscriptionChangeCmd(const char *name,
								Tango::CmdArgType in,
								Tango::CmdArgType out,
								const char *in_desc,
								const char *out_desc)
:Command(name,in,out,in_desc,out_desc)
{
}

//
//	Constructor without in/out parameters description
//

ZmqEventSubscriptionChangeCmd::ZmqEventSubscriptionChangeCmd(const char *name,Tango::CmdArgType in,Tango::CmdArgType out)
:Command(name,in,out)
{
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSubscriptionChangeCmd::is_allowed()
//
// description : 	method to test whether command is allowed or not in this
//			state. In this case, the command is allowed only if
//			the device is in ON state
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns :	boolean - true == is allowed , false == not allowed
//
//-----------------------------------------------------------------------------
bool ZmqEventSubscriptionChangeCmd::is_allowed(TANGO_UNUSED(Tango::DeviceImpl *device), TANGO_UNUSED(const CORBA::Any &in_any))
{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return true;
}

//+----------------------------------------------------------------------------
//
// method : 		ZmqEventSubscriptionChangeCmd::execute()
//
// description : 	method to trigger the execution of the command.
//
// in : - device : The device on which the command must be excuted
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
CORBA::Any *ZmqEventSubscriptionChangeCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{
    cout4 << "ZmqEventSubscriptionChangeCmd::execute(): arrived" << endl;

//
// Extract the input string array
//

	const Tango::DevVarStringArray *in_data;
	extract(in_any,in_data);

//
// call DServer method which implements this command
//

	Tango::DevVarLongStringArray *ret = ((DServer *)device)->zmq_event_subscription_change(in_data);

//
// return to the caller
//

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		cout3 << "Bad allocation while in ZmqEventSubscriptionChangeCmd::execute()" << endl;
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"ZmqEventSubscriptionChangeCmd::execute");
	}
	(*out_any) <<= ret;

	cout4 << "Leaving ZmqEventSubscriptionChangeCmd::execute()" << endl;
	return(out_any);
}



DServerClass *DServerClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		DServerClass::DServerClass()
//
// description : 	constructor for the DServerClass class
//			The constructor add specific class commands to the
//			command list, create a device of the DServer class
//			retrieve all classes which must be created within the
//			server and finally, creates these classes
//
// argument : in : 	- s : The class name
//
//-----------------------------------------------------------------------------

bool less_than_dserver (Command *a,Command *b)
{
	if (a->get_name() < b->get_name())
		return true;
	else
		return false;
}

DServerClass::DServerClass(string &s):DeviceClass(s)
{
	try
	{

//
// Add class command(s) to the command_list
//

		command_factory();

//
// Sort commands
//

		sort(get_command_list().begin(),get_command_list().end(),less_than_dserver);

//
// Create device name from device server name
//

		string dev_name(DSDeviceDomain);
		dev_name.append(1,'/');
		dev_name.append(Tango::Util::instance()->get_ds_exec_name());
		dev_name.append(1,'/');
		dev_name.append(Tango::Util::instance()->get_ds_inst_name());

		Tango::DevVarStringArray dev_list(1);
                dev_list.length(1);
		dev_list[0] = dev_name.c_str();

//
// Create the device server device
//

		device_factory(&dev_list);

	}
	catch (bad_alloc)
	{
		for (unsigned long i = 0;i < command_list.size();i++)
			delete command_list[i];
		command_list.clear();

		if (device_list.empty() == false)
		{
			for (unsigned long i = 0;i < device_list.size();i++)
				delete device_list[i];
			device_list.clear();
		}
		cerr << "Can't allocate memory while building the DServerClass object" << endl;
		throw;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DServerClass::Instance
//
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
//-----------------------------------------------------------------------------

DServerClass *DServerClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class DServer is not initialised!" << endl;
		Except::throw_exception((const char *)"API_DServerClassNotInitialised",
				        (const char *)"The DServerClass is not yet initialised, please wait!",
				        (const char *)"DServerClass::instance");
		//exit(-1);
	}
	return _instance;
}

DServerClass *DServerClass::init()
{
	if (_instance == NULL)
	{
		try
		{
			string s("DServer");
			_instance = new DServerClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}
	}
	return _instance;
}

//+----------------------------------------------------------------------------
//
// method : 		DServerClass::command_factory
//
// description : 	Create the command object(s) and store them in the
//			command list
//
//-----------------------------------------------------------------------------

void DServerClass::command_factory()
{
	command_list.push_back(new DevRestartCmd("DevRestart",
						 Tango::DEV_STRING,
						 Tango::DEV_VOID,
						 "Device name"));
	command_list.push_back(new DevRestartServerCmd("RestartServer",
						       Tango::DEV_VOID,
						       Tango::DEV_VOID));
	command_list.push_back(new DevQueryClassCmd("QueryClass",
						    Tango::DEV_VOID,
						    Tango::DEVVAR_STRINGARRAY,
						    "Device server class(es) list"));
	command_list.push_back(new DevQueryDeviceCmd("QueryDevice",
						     Tango::DEV_VOID,
						     Tango::DEVVAR_STRINGARRAY,
						     "Device server device(s) list"));
	command_list.push_back(new DevQuerySubDeviceCmd("QuerySubDevice",
						     Tango::DEV_VOID,
						     Tango::DEVVAR_STRINGARRAY,
						     "Device server sub device(s) list"));
	command_list.push_back(new DevKillCmd("Kill",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID));

//
// Now, commands related to polling
//

	command_list.push_back(new PolledDeviceCmd("PolledDevice",
						   Tango::DEV_VOID,
						   Tango::DEVVAR_STRINGARRAY,
						   "Polled device name list"));
	command_list.push_back(new DevPollStatusCmd("DevPollStatus",
						   Tango::DEV_STRING,
						   Tango::DEVVAR_STRINGARRAY,
						   "Device name",
						   "Device polling status"));
	string msg("Lg[0]=Upd period.");
	msg = msg + (" Str[0]=Device name");
	msg = msg + (". Str[1]=Object type");
	msg = msg + (". Str[2]=Object name");

	command_list.push_back(new AddObjPollingCmd("AddObjPolling",
						    Tango::DEVVAR_LONGSTRINGARRAY,
						    Tango::DEV_VOID,
						    msg));

	command_list.push_back(new UpdObjPollingPeriodCmd("UpdObjPollingPeriod",
							  Tango::DEVVAR_LONGSTRINGARRAY,
							  Tango::DEV_VOID,
							  msg));

	msg = "Str[0]=Device name. Str[1]=Object type. Str[2]=Object name";

	command_list.push_back(new RemObjPollingCmd("RemObjPolling",
						    Tango::DEVVAR_STRINGARRAY,
						    Tango::DEV_VOID,
						    msg));

	command_list.push_back(new StopPollingCmd("StopPolling",
						  Tango::DEV_VOID,
						  Tango::DEV_VOID));

	command_list.push_back(new StartPollingCmd("StartPolling",
						   Tango::DEV_VOID,
						   Tango::DEV_VOID));

#ifdef TANGO_HAS_LOG4TANGO
  	msg = "Str[i]=Device-name. Str[i+1]=Target-type::Target-name";

  	command_list.push_back(new AddLoggingTarget("AddLoggingTarget",
			  			    Tango::DEVVAR_STRINGARRAY,
			  			    Tango::DEV_VOID,
			  			    msg));

  	command_list.push_back(new RemoveLoggingTarget("RemoveLoggingTarget",
			  			       Tango::DEVVAR_STRINGARRAY,
			  			       Tango::DEV_VOID,
			  			       msg));

  	command_list.push_back(new GetLoggingTarget("GetLoggingTarget",
			       			    Tango::DEV_STRING,
			  			    Tango::DEVVAR_STRINGARRAY,
			  			    std::string("Device name"),
			  			    std::string("Logging target list")));

	command_list.push_back(new SetLoggingLevel("SetLoggingLevel",
			  			   Tango::DEVVAR_LONGSTRINGARRAY,
			  			   Tango::DEV_VOID,
			  			   std::string("Lg[i]=Logging Level. Str[i]=Device name.")));

	command_list.push_back(new GetLoggingLevel("GetLoggingLevel",
			  			   Tango::DEVVAR_STRINGARRAY,
			  			   Tango::DEVVAR_LONGSTRINGARRAY,
			  			   std::string("Device list"),
			  			   std::string("Lg[i]=Logging Level. Str[i]=Device name.")));

	command_list.push_back(new StopLogging("StopLogging",
			  		       Tango::DEV_VOID,
			  		       Tango::DEV_VOID));

	command_list.push_back(new StartLogging("StartLogging",
			  			Tango::DEV_VOID,
			  			Tango::DEV_VOID));
#else
	command_list.push_back(new DevSetTraceLevelCmd("SetTraceLevel",
						       Tango::DEV_LONG,
						       Tango::DEV_VOID,
						       "New trace level"));
	command_list.push_back(new DevGetTraceLevelCmd("GetTraceLevel",
						       Tango::DEV_VOID,
						       Tango::DEV_LONG,
						       "Device server trace level"));
	command_list.push_back(new DevSetTraceOutputCmd("SetTraceOutput",
						        Tango::DEV_STRING,
						        Tango::DEV_VOID,
							"New device server output file"));
	command_list.push_back(new DevGetTraceOutputCmd("GetTraceOutput",
						        Tango::DEV_VOID,
						        Tango::DEV_STRING,
							"Device server output file"));
#endif // TANGO_HAS_LOG4TANGO
	command_list.push_back(new EventSubscriptionChangeCmd("EventSubscriptionChange",
							Tango::DEVVAR_STRINGARRAY, Tango::DEV_LONG,
							"Event consumer wants to subscribe to",
							"Tango lib release"));

	command_list.push_back(new ZmqEventSubscriptionChangeCmd("ZmqEventSubscriptionChange",
							Tango::DEVVAR_STRINGARRAY, Tango::DEVVAR_LONGSTRINGARRAY,
							"Events consumer wants to subscribe to",
							"Str[0] = Heartbeat pub endpoint - Str[1] = Event pub endpoint - Lg[0] = Tango lib release - Lg[1] = Device IDL release"));

	command_list.push_back(new QueryWizardClassPropertyCmd("QueryWizardClassProperty",
							Tango::DEV_STRING,
							Tango::DEVVAR_STRINGARRAY,
							"Class name",
							"Class property list (name - description and default value)"));

	command_list.push_back(new QueryWizardDevPropertyCmd("QueryWizardDevProperty",
							Tango::DEV_STRING,
							Tango::DEVVAR_STRINGARRAY,
							"Class name",
							"Device property list (name - description and default value)"));

//
// Locking device commands
//

	command_list.push_back(new LockDeviceCmd("LockDevice",
							Tango::DEVVAR_LONGSTRINGARRAY,
							Tango::DEV_VOID,
							"Str[0] = Device name. Lg[0] = Lock validity"));

	command_list.push_back(new UnLockDeviceCmd("UnLockDevice",
							Tango::DEVVAR_LONGSTRINGARRAY,
							Tango::DEV_LONG,
							"Str[x] = Device name(s). Lg[0] = Force flag",
							"Device global lock counter"));

	command_list.push_back(new ReLockDevicesCmd("ReLockDevices",
							Tango::DEVVAR_STRINGARRAY,
							Tango::DEV_VOID,
							"Device(s) name"));

	command_list.push_back(new DevLockStatusCmd("DevLockStatus",
							Tango::DEV_STRING,
							Tango::DEVVAR_LONGSTRINGARRAY,
							"Device name",
							"Device locking status"));

	if (Util::_FileDb == true)
	{
		command_list.push_back(new QueryEventChannelIORCmd("QueryEventChannelIOR",
							Tango::DEV_VOID,
							Tango::DEV_STRING,
							"Device server event channel IOR"));
	}
}


//+----------------------------------------------------------------------------
//
// method : 		DServerClass::device_factory
//
// description : 	Create the device object(s) and store them in the
//			device list
//
// in :			Tango::DevVarStringArray *devlist_ptr :
//			The device name list
//
//-----------------------------------------------------------------------------

void DServerClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{
	Tango::Util *tg = Tango::Util::instance();

	for (unsigned long i = 0;i < devlist_ptr->length();i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;

//
// Create device and add it into the device list
//

		device_list.push_back(new DServer(this,
						  (*devlist_ptr)[i],
						  "A device server device !!",
						  Tango::ON,
						  "The device is ON"));


//
// Export device to the outside world
//

		if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
			export_device(device_list.back());
		else
			export_device(device_list.back(),(*devlist_ptr)[i]);

//
// After the export of the admin device, the server is marked as started
// and the database server connection timeout is set to the classical
// timeout value (Except for db server itself)
//

		tg->set_svr_starting(false);
		Database *db = tg->get_database();
		if ((db != NULL) && (Util::_FileDb == false))
			db->set_timeout_millis(CLNT_TIMEOUT);

	}
}

} // End of Tango namespace

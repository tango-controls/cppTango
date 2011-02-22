static const char *RcsId = "$Header";

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
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <algorithm>

#include <tango.h>
#include <dserverclass.h>

namespace Tango
{

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

CORBA::Any *DevQueryClassCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevQueryClassCmd::execute(): arrived" << endl;

//	
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_class();

//
// return data to the caller
//

	CORBA::Any *out_any;
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

CORBA::Any *DevQueryDeviceCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevQueryDeviceCmd::execute(): arrived" << endl;

//	
// call DServer method which implements this command
//

	Tango::DevVarStringArray *ret = ((DServer *)device)->query_device();

//
// return data to the caller
//
	CORBA::Any *out_any;
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

CORBA::Any *DevKillCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
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

CORBA::Any *DevSetTraceLevelCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevSetTraceLevelCmd::execute(): arrived" << endl;
	
//
// Get new level
//

	long new_level;
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

CORBA::Any *DevGetTraceLevelCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevGetTraceLevelCmd::execute(): arrived" << endl;
	
//
// Get level
//

	long level = Tango::Util::instance()->get_trace_level();
	
//
// return data to the caller
//
	
	CORBA::Any *out_any = new CORBA::Any();
	(*out_any) <<= level;
	
	cout4 << "Leaving DevGetTraceLevelCmd::execute()" << endl;	
	return(out_any);
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

CORBA::Any *DevGetTraceOutputCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevGetTraceOutputCmd::execute(): arrived" << endl;
	
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

CORBA::Any *DevSetTraceOutputCmd::execute(DeviceImpl *device,const CORBA::Any &in_any)
{	

	cout4 << "DevSetTraceOutputCmd::execute(): arrived" << endl;

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
#ifdef WIN32
		ostream &tmp_stream = Tango::Util::instance()->get_out();

//
// For windows, the stdc++ library also implements the new IOstreams where the
// xx_with_assign classes do not exist. To copy stream, I hav eused the advice 
// from the C++ report of June 1997
//
		
		cout.copyfmt(tmp_stream);
		cout.clear(tmp_stream.rdstate());
		cout.rdbuf(tmp_stream.rdbuf());
#else
		cout = Tango::Util::instance()->get_out();
#endif
		Tango::Util::instance()->set_trace_output(in_file);
	}
	else
	{	
		ofstream *ofp = new ofstream(in_file_ptr);
		if (ofp->good())
		{
			cout = *ofp;
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
		for (long i = 0;i < command_list.size();i++)
			delete command_list[i];
		command_list.clear();
		
		if (device_list.empty() == false)
		{
			for (long i = 0;i < device_list.size();i++)
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
		cerr << "Class is not initialised!" << endl;
		exit(-1);	
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
	command_list.push_back(new DevQueryClassCmd("DevQueryClass",
						    Tango::DEV_VOID,
						    Tango::DEVVAR_STRINGARRAY,
						    "Device server class(es) list"));
	command_list.push_back(new DevQueryDeviceCmd("DevQueryDevice",
						     Tango::DEV_VOID,
						     Tango::DEVVAR_STRINGARRAY,
						     "Device server device(s) list"));
	command_list.push_back(new DevKillCmd("DevKill",
					      Tango::DEV_VOID,
					      Tango::DEV_VOID));
	command_list.push_back(new DevSetTraceLevelCmd("DevSetTraceLevel",
						       Tango::DEV_LONG,
						       Tango::DEV_VOID,
						       "New trace level"));
	command_list.push_back(new DevGetTraceLevelCmd("DevGetTraceLevel",
						       Tango::DEV_VOID,
						       Tango::DEV_LONG,
						       "Device server trace level"));
	command_list.push_back(new DevSetTraceOutputCmd("DevSetTraceOutput",
						        Tango::DEV_STRING,
						        Tango::DEV_VOID,
							"New device server output file"));
	command_list.push_back(new DevGetTraceOutputCmd("DevGetTraceOutput",
						        Tango::DEV_VOID,
						        Tango::DEV_STRING,
							"Device server output file"));
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
	
	for (long i = 0;i < devlist_ptr->length();i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i] << endl;
						
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

		if (Tango::Util::_UseDb == true)
			export_device(device_list.back());
	}
}

} // End of Tango namespace

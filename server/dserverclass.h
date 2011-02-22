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
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _DSERVERCLASS_H
#define _DSERVERCLASS_H

#include <tango.h>

namespace Tango
{

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

	static DServerClass *instance();
	static DServerClass *init();
	
	~DServerClass() {};
	
	void command_factory();
	void device_factory(const Tango::DevVarStringArray *devlist);
	
protected:
	DServerClass(string &);
	static DServerClass *_instance;	
};

} // End of Tango namespace

#endif // _DSERVERCLASS_H

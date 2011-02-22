//=============================================================================	
//
// file :		BasicCommand.h
//
// description :	Include for commands which are implemented in all
//			classes. Classes for 3 commands are defined here:
//				DevStatus for the DevStatus command
//				DevState for the DevState command
//				DevRestart for the DevRestart command
//			All these classes inherits from the Command class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:12  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
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
//			The DevRestart class
//
// description :	Class to implement the DevRestart command. This command
//			does not take any input argument nor outout argument.
//			This class re-initialise without restarting its netwok 
//			part
//
//=============================================================================

 
class DevRestartCmd : public Command
{
public:


	DevRestartCmd(const char *cmd_name,Tango::CmdArgType in,Tango::CmdArgType out);	
	~DevRestartCmd() {};
	
	virtual CORBA::Any *execute(DeviceImpl *device, const CORBA::Any &in_any);
};


//=============================================================================
//
//			The KillDevThread class
//
// description :	Class executed by the thread started by the DevRestart
//			command.
//
//=============================================================================

class KillDevThread: public JTCThread
{
	virtual void run();
};

} // End of Tango namespace

#endif // _BASICCOMMAND_H

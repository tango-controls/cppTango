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
// $Revision$
//
// $Log$
// Revision 2.7  2002/12/16 10:15:35  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:05  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:18  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:53  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:23  taurel
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
// Revision 2.2  2002/04/30 10:50:40  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:02  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:08  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:10  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:09  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:12  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:44  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:14  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
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

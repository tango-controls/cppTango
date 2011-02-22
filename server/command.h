//=============================================================================	
//
// file :		Command.h
//
// description :	Include for the Device root classes. 
//			One class is declared in this file :
//				The Command class
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.0  2003/03/25 16:41:51  taurel
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
// Revision 2.8  2002/12/16 12:06:22  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:15:36  taurel
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
// Revision 2.3  2002/07/02 15:22:24  taurel
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
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:11  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:10  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:13  taurel
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
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:29  taurel
// Imported sources
//
//
// copyleft :		European Synchrotron Radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//=============================================================================

#ifndef _COMMAND_H
#define _COMMAND_H

#include <tango.h>

namespace Tango
{

class CommandExt
{
public:
	CommandExt():poll_period(0) {cmd_disp_level = Tango::OPERATOR;}
	CommandExt(Tango::DispLevel level):poll_period(0) {cmd_disp_level = level;}

	Tango::DispLevel	cmd_disp_level;		// Display  level
	long			poll_period;		// Polling period	
};

/**
 * This class is a class representing a command in the TANGO device server
 * pattern. it is an abstract class. It is the root class for all command
 * related classes for command implemented with the inheritance model or
 * with the template command model
 *
 * $Author$
 * $Revision$
 */
 
class Command
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Command object.
 *
 * The default constructor
 */
	Command() {ext = new CommandExt();}
/**
 * Constructs a newly allocated Command object for a command from its
 * name and its input and output parameter types.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 *
 */
	Command(const char *s,Tango::CmdArgType in,Tango::CmdArgType out);

/**
 * Constructs a newly allocated Command object for a command from its
 * name and its input and output parameter types.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 *
 */	
	Command(string &s,Tango::CmdArgType in,Tango::CmdArgType out);

/**
 * Constructs a newly allocated Command object for a command from its
 * name, its input and output parameter types plus parameters description
 * The command display level is set to OPERATOR.
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 * @param	in_desc	The input parameter description
 * @param	out_desc The output parameter description
 *
 */
	Command(const char *s,Tango::CmdArgType in,Tango::CmdArgType out,
		const char *in_desc,const char *out_desc);
		
/**
 * Constructs a newly allocated Command object for a command from its
 * name, its input and output parameter types plus parameters description
 * The command display level is set to OPERATOR.
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 * @param	in_desc	The input parameter description
 * @param	out_desc The output parameter description
 *
 */
	Command(string &s,Tango::CmdArgType in,Tango::CmdArgType out,
		string &in_desc,string &out_desc);
/**
 * Constructs a newly allocated Command object for a command from its
 * name and its input and output parameter types.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 * @param	level	The command display level
 *
 */
	Command(const char *s,Tango::CmdArgType in,Tango::CmdArgType out,
	    	Tango::DispLevel level);

/**
 * Constructs a newly allocated Command object for a command from its
 * name and its input and output parameter types.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 * @param	level	The command display level
 *
 */	
	Command(string &s,Tango::CmdArgType in,Tango::CmdArgType out,
		Tango::DispLevel level);

/**
 * Constructs a newly allocated Command object for a command from its
 * name, its input and output parameter types plus parameters description
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 * @param	in_desc	The input parameter description
 * @param	out_desc The output parameter description
 * @param	level	The command display level
 *
 */
	Command(const char *s,Tango::CmdArgType in,Tango::CmdArgType out,
		const char *in_desc,const char *out_desc,
		Tango::DispLevel level);
		
/**
 * Constructs a newly allocated Command object for a command from its
 * name, its input and output parameter types plus parameters description
 *
 * @param 	s	The command name
 * @param	in	The command input parameter type
 * @param	out 	The command output parameter type
 * @param	in_desc	The input parameter description
 * @param	out_desc The output parameter description
 * @param	level	The command display level
 *
 */
	Command(string &s,Tango::CmdArgType in,Tango::CmdArgType out,
		string &in_desc,string &out_desc,
		Tango::DispLevel level);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */	
	virtual ~Command() {delete ext;}
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Execute the command.
 *
 * This method is automtically called by the TANGO core classes when the
 * associated command is requested by a client. This method is abstract and must be redefined
 * in each sub-class
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object.
 * @return The CORBA Any object returned to the client.
 * @exception DevFailed If the execution method failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	virtual CORBA::Any *execute (DeviceImpl *dev, const CORBA::Any &in_any) = 0;
	
/**
 * Check if the command is allowed in the actual device state.
 *
 * This method is automtically called by the TANGO core classes when the
 * associated command is requested by a client to check if the command is allowed
 * in the actual device state. This method is the default is_allowed method which
 * always allows the command to be executed. It is possible to re-define it 
 * if this default behaviour does not fullfill the device needs.
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object. This
 * data is passed to this method in case it is necessary to take the command
 * allowed decision
 * @return A boolean set to true is the command is allowed. Otherwise, the
 * return value is false.
 */
	virtual bool is_allowed (DeviceImpl *, const CORBA::Any &) {return true;}

/**
 * Init command parameters type.
 *
 * This method is used only for command implemented using the Tango template
 * command model. In this base class, it does nothing and is re-define in
 * sub-classes.
 *
 */	
	virtual void init_types() {};
//@}

/**@name Get/Set object members.
 * These methods allows the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Return the command name.
 *
 * @return The command name
 */	
	string &get_name() {return name;}
	
/**
 * Return the command name in lower case letters.
 *
 * @return The command name
 */	
	string &get_lower_name() {return lower_name;}
	
/**
 * Return the input parameter type.
 *
 * @return The input parameter type
 */
	Tango::CmdArgType get_in_type() {return in_type;}
	
/**
 * Return the output parameter type.
 *
 * @return The output parameter type
 */
	Tango::CmdArgType get_out_type() {return out_type;}

/**
 * Return the input parameter description.
 *
 * @return The input parameter description
 */	
	string &get_in_type_desc() {return in_type_desc;}
	
/**
 * Return the output parameter description.
 *
 * @return The output parameter description
 */
	string &get_out_type_desc() {return out_type_desc;}

/**
 * Return the command display level.
 *
 * @return The command display level
 */
	Tango::DispLevel get_disp_level() {return ext->cmd_disp_level;}
		
/**
 * Set the input parameter description field.
 *
 * @param desc The input parameter description
 */
	void set_in_type_desc(const char *desc) {in_type_desc = desc;}
	
/**
 * Set the input parameter description field.
 *
 * @param desc The input parameter description
 */
	void set_in_type_desc(string &desc) {in_type_desc = desc;}
	
/**
 * Set the output parameter description field.
 *
 * @param desc The output parameter description
 */
	void set_out_type_desc(const char *desc) {out_type_desc = desc;}
	
/**
 * Set the output parameter description field.
 *
 * @param desc The output parameter description
 */
	void set_out_type_desc(string &desc) {out_type_desc = desc;}
	
/**
 * Set the command display level.
 *
 * @param level The command display level
 */
	void set_disp_level(Tango::DispLevel level) {ext->cmd_disp_level = level;}

/**
 * Set the command polling period.
 *
 * @param per The command polling period (in mS)
 */
	void set_polling_period(long per) {ext->poll_period = per;}
	
/**
 * Get the command polling period.
 *
 * @return The command polling period (in mS)
 */
	long get_polling_period() {return ext->poll_period;}
//@}


/**@name Extract methods.
 * All these methods extract data from the CORBA Any object received as
 * command input data
 */
//@{
/**
 * Extract a boolean data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted boolean data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void extract(const CORBA::Any &in,Tango::DevBoolean &data);
	
/**
 * Extract a short data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted short data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevShort &data);
	
/**
 * Extract a long data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted long data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevLong &data);
	
/**
 * Extract a float data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted float data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevFloat &data);
	
/**
 * Extract a double data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted double data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevDouble &data);
	
/**
 * Extract an unsigned short data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted unsigned short data
 * @exception DevFailed If the Any object does not contanis a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevUShort &data);
	
/**
 * Extract an unsigned long data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted unsigned long data
 * @exception DevFailed If the Any object does not contanis a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevULong &data);
	
/**
 * Extract a string from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted string data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevString &data);

/**
 * Extract a const string from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted string data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const char *&data);
		
/**
 * Extract a char array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted char array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarCharArray *&data);
	
/**
 * Extract a short array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted short array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarShortArray *&data);
	
/**
 * Extract a long array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted long array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarLongArray *&data);
	
/**
 * Extract a float array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted float array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarFloatArray *&data);
	
/**
 * Extract a double array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted double array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarDoubleArray *&data);
	
/**
 * Extract a unsigned short array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted unsigned char array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarUShortArray *&data);
	
/**
 * Extract a unsigned long array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted unsigned long array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarULongArray *&data);
	
/**
 * Extract a string array from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted string array
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarStringArray *&data);
	
/**
 * Extract a DevVarLongStringArray data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted DevVarLongStringArray data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarLongStringArray *&data);
	
/**
 * Extract a DevVarDoubleStringArray data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted DevVarDoubleStringArray data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,const Tango::DevVarDoubleStringArray *&data);
	
/**
 * Extract a Tango device state data from a CORBA Any object.
 *
 * @param in The CORBA Any object
 * @param data Reference to the extracted device state data
 * @exception DevFailed If the Any object does not contains a data of the
 * waited type.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void extract(const CORBA::Any &in,Tango::DevState &data);
//@}

/**@name Insert methods.
 * All these methods create a CORBA Any object and insert data into this object 
 */
//@{
/**
 * Create an  empty CORBA Any object.
 *
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert();

/**
 * Create a CORBA Any object and insert a Tango::DevBoolean data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	CORBA::Any *insert(Tango::DevBoolean data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevShort data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevShort data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevLong data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevLong data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevFloat data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevFloat data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevDouble data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevDouble data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevUShort data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevUShort data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevULong data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevULong data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevString data in it.
 *
 * This method will also de-allocate the string passed as parameter.
 *
 * @param data The string to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevString data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevString data in it.
 * Te parameter type is char * and not Tango::DevString because the const
 * C++ modifier applied to a Tango::DevString make the pointer constant and
 * not the pointed to characters to be constant.
 *
 * @param data The string to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(const char *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarCharArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarCharArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarCharArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarCharArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarCharArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarShortArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarShortArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarShortArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarShortArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarShortArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarLongArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarLongArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarLongArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarLongArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarLongArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarFloatArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarFloatArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarFloatArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarFloatArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarFloatArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarDoubleArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarDoubleArray &data);
	
/**
 * Create a CORBA CORBA::Any object and insert a Tango::DevVarDoubleArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarDoubleArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarDoubleArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarUShortArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarUShortArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarUShortArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarUShortArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarUShortArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarULongArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarULongArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarULongArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarULongArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarULongArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarStringArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarStringArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarStringArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarStringArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarStringArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarLongStringArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarLongStringArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarLongStringArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarLongStringArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarLongStringArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarDoubleStringArray data in it.
 *
 * This method will do a <b>deep copy</b> of the array into the Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarDoubleStringArray &data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevVarDoubleStringArray data in it.
 *
 * This method <b>consumes</b> the memory used by the array.
 * When the CORBA layer will destroy the Any object, the memory alloacted
 * for the array will also be freed. This is the recommended method to
 * insert Tango::DevVarDoubleStringArray data type into a CORBA Any object.
 *
 * @param data The array to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *insert(Tango::DevVarDoubleStringArray *data);
	
/**
 * Create a CORBA Any object and insert a Tango::DevState data in it.
 *
 * @param data The data to be inserted into the Any object
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	CORBA::Any *insert(Tango::DevState);
//@}
	
protected:
/**@name Class data members */
//@{
/**
 * The command name
 */
	string 			name;
/**
 * The command name in lower case
 */
	string 			lower_name;
/**
 * The command input parameter type
 */
	Tango::CmdArgType	in_type;
/**
 * The command output parameter type
 */
	Tango::CmdArgType	out_type;
/**
 * The command input parameter description
 */
	string			in_type_desc;
/**
 * The command output parameter type
 */
	string			out_type_desc;
//@}
	
private:
	void alloc_any(CORBA::Any *&);
	void throw_bad_type(const char *);
	
	CommandExt		*ext;
};

//=============================================================================
//
//			The TemplCommand class
//
//
// description :	This class is a derived class of the Command class.
//			It is used to create a command from a pointer to a
//			object method which will execute the command.
//			This class is for command without inout  nor output
//			paremeters.
//			This class is also a base class for the template Command
//			class
//
//=============================================================================

class TemplCommandExt
{
};

/**
 * This class is a class representing a command in the template command model
 * without input or output parameter
 *
 * $Author$
 * $Revision$
 */
 
class TemplCommand:public Command
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Command object.
 *
 * The default constructor
 */
	TemplCommand() {ext = NULL;}

/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name and an execution method.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the  command execution method
 *
 */
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)());
	
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name and an execution method.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)());

/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a command allowed method.
 * This constructor set the command input and output type to Tango::DEV_VOID
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */		     
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &));
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a command allowed method.
 * This constructor set the command input and output type to Tango::DEV_VOID
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */		     
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &));

/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */		     
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     const char *in_desc,const char *out_desc);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     string &in_desc,string &out_desc);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method 
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     const char *in_desc,const char *out_desc);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     string &in_desc,string &out_desc);
		     		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name and an execution method.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the  command execution method
 * @param	level	 	The command display level
 *
 */
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     Tango::DispLevel level);
	
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name and an execution method.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a command allowed method.
 * This constructor set the command input and output type to Tango::DEV_VOID
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */		     
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     Tango::DispLevel level);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a command allowed method.
 * This constructor set the command input and output type to Tango::DEV_VOID
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */		     
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */		     
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     const char *in_desc,const char *out_desc,
		     Tango::DispLevel level);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     string &in_desc,string &out_desc,
		     Tango::DispLevel level);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method 
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommand(const char *cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     const char *in_desc,const char *out_desc,
		     Tango::DispLevel level);
		     
/**
 * Constructs a newly allocated TemplCommand object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * This constructor set the command input and output type to Tango::DEV_VOID.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommand(string &cmd_name,void (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     string &in_desc,string &out_desc,
		     Tango::DispLevel level);		     
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The device desctructor.
 */
	~TemplCommand() {delete ext;}
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Choose the correct TANGO data type constant according to data type
 *
 * The TANGO constant is determined using the type_info object passed
 * as first argument of the method. This data is compared to each defined
 * Tango type.
 *
 * @param data_type The type to be analysed
 * @param type A reference where Tango data type constant must be stored
 * @exception DevFailed If the type is not a Tango data type
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */ 			     		     		     		     
	void set_type(const type_info &data_type,Tango::CmdArgType &type);

/**
 * Invoke the command execution method given at object creation time.
 *
 * This method is automatically called by the TANGO core classes when the
 * associated command is requested by a client.
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object. For
 * command created with this TemplCommand class, this Any object does not
 * contain usefull data
 * @return The CORBA Any object returned to the client. For command created with
 * this TemplCommand class, this any object does not contain data.
 * @exception DevFailed If the execution method failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *execute (DeviceImpl *dev, const CORBA::Any &in_any);
	
/**
 * Invoke the command allowed method given at object creation time.
 *
 * This method is automtically called by the TANGO core classes when the
 * associated command is requested by a client to check if the command is allowed
 * in the actual device state. If the user give a command allowed method
 * at object creation time, this method will be invoked.
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object. For
 * command created with this TemplCommand class, this Any object does not
 * contain data
 * @return A boolean set to true is the command is allowed. Otherwise, the
 * return value is false. This return value is always set to true if the user
 * does not supply a method to be excuted. If a method has been supplied, the 
 * return value is the value returned by the user supplied mehod.
 */
	bool is_allowed (DeviceImpl *dev, const CORBA::Any &in_any);
//@}
	
private:
	void (DeviceImpl::*exe_ptr)();
	TemplCommandExt		*ext;
	
protected:
/**@name Class data members */
//@{
/**
 * The command allowed method object reference
 */
	bool (DeviceImpl::*allowed_ptr)(const CORBA::Any &);
//@}
};


//=============================================================================
//
//			The TemplCommandInOut class
//
//
// description :	This class is a derived class of the Command class.
//			It is used to create a command from a pointer to a
//			object method which will execute the command.
//			This class is for command without inout  nor output
//			paremeters.
//			This class is also a base class for the template Command
//			class
//
//=============================================================================

class TemplCommandInOutExt
{
};

/**
 * This class is a class representing a command in the template command model
 * with output and input parameters. The class template 
 * parameters (called INARG and OUTARG) are the command input parameter type
 * and the command output parameter type.
 *<p>Synopsis : template <class INARG,class OUTARG> class TemplCommandInOut:public TemplCommand;</p>
 *<p>Usage : new TemplCommandInOut<Tango::DevLong,Tango::DevFloat>(...);</p>
 *
 * $Author$
 * $Revision$
 */
 
template <class INARG,class OUTARG>
class TemplCommandInOut:public TemplCommand
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG));

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */	
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG));

/**
 * Constructs a newly allocated TemplCommandInout object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */	
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		    	  bool (DeviceImpl::*state_method)(const CORBA::Any &));

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */		     
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		     	  bool (DeviceImpl::*state_method)(const CORBA::Any &));

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */		     
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
			  const char *in_desc,const char *out_desc);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */	
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
			  string &in_desc,string &out_desc);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method 
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */	
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		     	  bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     	  const char *in_desc,const char *out_desc);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method 
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		    	  bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     	  string &in_desc,string &out_desc);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */	
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInout object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */	
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		    	  bool (DeviceImpl::*state_method)(const CORBA::Any &),
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */		     
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		     	  bool (DeviceImpl::*state_method)(const CORBA::Any &),
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */		     
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
			  const char *in_desc,const char *out_desc,
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */	
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
			  string &in_desc,string &out_desc,
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method 
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */	
	TemplCommandInOut(const char *cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		     	  bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     	  const char *in_desc,const char *out_desc,
			  Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandInOut object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method 
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommandInOut(string &cmd_name,
			  OUTARG (DeviceImpl::*exe_method)(INARG),
		    	  bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     	  string &in_desc,string &out_desc,
			  Tango::DispLevel level);
//@}

	~TemplCommandInOut() {delete ext;}
	
/**@name Miscellaneous methods */
//@{
/**
 * Initialise command input and output types.
 *
 * Set the command output type to Tango::DEV_VOID. The command input type is
 * automatically determined from the class template specialisation
 *
 */		     		     		     		     
	void init_types();
	
/**
 * Invoke the command execution method given at object creation time.
 *
 * This method is automtically called by the TANGO core classes when the
 * associated command is requested by a client. It first unpacks the incoming
 * data. Then, it invokes the user supplied 
 * command execution method and packs the returned data into the outgoing
 * CORBA Any object
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object.
 * @return The CORBA Any object returned to the client.
 * @exception DevFailed If the execution method failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *execute (DeviceImpl *dev, const CORBA::Any &in_any);
//@}
	
private:
	OUTARG (DeviceImpl::*exe_ptr_inout)(INARG);
	TemplCommandInOutExt		*ext;
};

//+-------------------------------------------------------------------------
//
// method : 		TempCommandInOut class constructors
// 
// description : 	instance constructor
//
//--------------------------------------------------------------------------

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG))
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &))
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG))
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &))
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 const char *in_desc,const char *out_desc)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 const char *in_desc,const char *out_desc)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 string &in_desc,string &out_desc)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	allowed_ptr = NULL;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 string &in_desc,string &out_desc)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 const char *in_desc,const char *out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(const char *s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 const char *in_desc,const char *out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 string &in_desc,string &out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	allowed_ptr = NULL;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG,class OUTARG>
TemplCommandInOut<INARG,OUTARG>::TemplCommandInOut(string &s,
					 OUTARG (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 string &in_desc,string &out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_inout(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

//+-------------------------------------------------------------------------
//
// method : 		init_types
// 
// description : 	Initialise input and output type from the template 
//			class specialisation parameters.
//
//--------------------------------------------------------------------------

template <class INARG,class OUTARG>
void TemplCommandInOut<INARG,OUTARG>::init_types()
{

//
// Set intput type
//	
	set_type(typeid(INARG),in_type);
	
//
// Set output type
//

	set_type(typeid(OUTARG),out_type);
}

//+-------------------------------------------------------------------------
//
// method : 		execute
// 
// description : 	Execute the method associated with the command
//			(stored in the exe_ptr data)
//
// input : - dev_ptr : pointer to the device on which the command must be
//		       executed
//	   - in_any : Incoming command data
//
// This method returns a pointer to an Any object with the command outing
// data.
//
//--------------------------------------------------------------------------

template <class INARG,class OUTARG>
CORBA::Any *TemplCommandInOut<INARG,OUTARG>::execute(DeviceImpl *dev_ptr,const CORBA::Any &in_any)
{

//
// Execute the command associated method 
//

	INARG in_data;
	extract(in_any,in_data);
	OUTARG out_data = (dev_ptr->*exe_ptr_inout)(in_data);
	return insert(out_data);

}

//=============================================================================
//
//			The TemplCommandIn class
//
//
// description :	This class is a derived class of the Command class.
//			It is used to create a command from a pointer to a
//			object method which will execute the command.
//			This class is for command without inout  nor output
//			paremeters.
//			This class is also a base class for the template Command
//			class
//
//=============================================================================

class TemplCommandInExt
{
};

/**
 * This class is a class representing a command in the template command model
 * with input parameter but without output parameter. The class template 
 * parameter (called INARG) is the command input parameter type.
 *<p>Synopsis : template <class INARG> class TemplCommandIn:public TemplCommand;</p>
 *<p>Usage : new TemplCommandIn<Tango::DevLong>(...);</p>
 *
 * $Author$
 * $Revision$
 */

 
template <class INARG>
class TemplCommandIn:public TemplCommand
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG));

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG));

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */	
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &));

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */		     
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &));

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */		     
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       const char *in_desc,const char *out_desc);
		       
/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       string &in_desc,string &out_desc);
		       
/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &),
		       const char *in_desc,const char *out_desc);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &),
		       string &in_desc,string &out_desc);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */	
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &),
		       Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */		     
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &),
		       Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */		     
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       const char *in_desc,const char *out_desc,
		       Tango::DispLevel level);
		       
/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       string &in_desc,string &out_desc,
		       Tango::DispLevel level);
		       
/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommandIn(const char *cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &),
		       const char *in_desc,const char *out_desc,
		       Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommandIn(string &cmd_name,
		       void (DeviceImpl::*exe_method)(INARG),
		       bool (DeviceImpl::*state_method)(const CORBA::Any &),
		       string &in_desc,string &out_desc,
		       Tango::DispLevel level);
//@}

	~TemplCommandIn() {delete ext;}
	
/**@name Miscellaneous methods */
//@{
/**
 * Initialise command input and output types.
 *
 * Set the command output type to Tango::DEV_VOID. The command input type is
 * automatically determined from the class template specialisation
 *
 */		     		     		     		     
	void init_types();
	
/**
 * Invoke the command execution method given at object creation time.
 *
 * This method is automtically called by the TANGO core classes when the
 * associated command is requested by a client. It unpacks the data stored in
 * the CORBA Any object and invoke the user supplied command execution
 * method
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object.
 * @return The CORBA Any object returned to the client. For command created with
 * this TemplCommandIn class, this any object does not contain data.
 * @exception DevFailed If the execution method failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *execute (DeviceImpl *dev, const CORBA::Any &in_any);
//@}
	
private:
	void (DeviceImpl::*exe_ptr_in)(INARG);
	TemplCommandInExt		*ext;
};

//+-------------------------------------------------------------------------
//
// method : 		TempCommandIn class constructors
// 
// description : 	instance constructor
//
//--------------------------------------------------------------------------

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG))
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &))
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG))
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &))
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 const char *in_desc,const char *out_desc)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 const char *in_desc,const char *out_desc)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 string &in_desc,string &out_desc)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	allowed_ptr = NULL;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 string &in_desc,string &out_desc)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 const char *in_desc,const char *out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(const char *s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 const char *in_desc,const char *out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 string &in_desc,string &out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	allowed_ptr = NULL;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class INARG>
TemplCommandIn<INARG>::TemplCommandIn(string &s,
					 void (DeviceImpl::*f)(INARG),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 string &in_desc,string &out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_in(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}
//+-------------------------------------------------------------------------
//
// method : 		init_types
// 
// description : 	Initialise input and output type from the template 
//			class specialisation parameters.
//
//--------------------------------------------------------------------------

template <class INARG>
void TemplCommandIn<INARG>::init_types()
{

	out_type = Tango::DEV_VOID;
//
// Set intput type
//	
	set_type(typeid(INARG),in_type);

}

//+-------------------------------------------------------------------------
//
// method : 		execute
// 
// description : 	Execute the method associated with the command
//			(stored in the exe_ptr data)
//
// input : - dev_ptr : pointer to the device on which the command must be
//		       executed
//	   - in_any : Incoming command data
//
// This method returns a pointer to an Any object with the command outing
// data.
//
//--------------------------------------------------------------------------

template <class INARG>
CORBA::Any *TemplCommandIn<INARG>::execute(DeviceImpl *dev_ptr,const CORBA::Any &in_any)
{

//
// Execute the command associated method 
//

	INARG in_data;
	extract(in_any,in_data);
	(dev_ptr->*exe_ptr_in)(in_data);
	return insert();
}


//=============================================================================
//
//			The TemplCommandOut class
//
//
// description :	This class is a derived class of the Command class.
//			It is used to create a command from a pointer to a
//			object method which will execute the command.
//			This class is for command without inout  nor output
//			paremeters.
//			This class is also a base class for the template Command
//			class
//
//=============================================================================

class TemplCommandOutExt
{
};

/**
 * This class is a class representing a command in the template command model
 * with output parameter but without input parameter. The class template 
 * parameter (called OUTARG) is the command output parameter type.
 *<p>Synopsis : template <class OUTARG> class TemplCommandOut:public TemplCommand;</p>
 *<p>Usage : new TemplCommandOut<Tango::DevLong>(...);</p>
 *
 * $Author$
 * $Revision$
 */
 
template <class OUTARG>
class TemplCommandOut:public TemplCommand
{
public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)());

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 *
 */	
 	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)());

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */	
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &));
		     
/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 *
 */		     
	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &));

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */		     
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
			const char *in_desc,const char *out_desc);
			
/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */			
	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
			string &in_desc,string &out_desc);

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */			
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		        bool (DeviceImpl::*state_method)(const CORBA::Any &),
		        const char *in_desc,const char *out_desc);
			
/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 * The command display level is set to OPERATOR.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 *
 */
	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		        bool (DeviceImpl::*state_method)(const CORBA::Any &),
		        string &in_desc,string &out_desc);

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		 	Tango::DispLevel level);
	
/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name and an execution method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	level		The command display level
 *
 */	
 	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
 			Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */	
 	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     Tango::DispLevel level);
		     
/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a command allowed method.
 * The input and output command data type are automatically determined.
 * The input and output parameter description are set to the default String
 * "Uninitialised".
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	level		The command display level
 *
 */		     
	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		     bool (DeviceImpl::*state_method)(const CORBA::Any &),
		     Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */		     
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
			const char *in_desc,const char *out_desc,
			Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	in_desc		The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */			
	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
			string &in_desc,string &out_desc,
			Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandOut object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */			
	TemplCommandOut(const char *cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		        bool (DeviceImpl::*state_method)(const CORBA::Any &),
		        const char *in_desc,const char *out_desc,
			Tango::DispLevel level);

/**
 * Constructs a newly allocated TemplCommandIn object for a command with a
 * name, an execution method, a command allowed method and a description for the
 * input and output command parameters.
 * The input and output command data type are automatically determined.
 *
 * @param 	cmd_name	The command name
 * @param	exe_method	Pointer to the command execution method
 * @param	state_method 	Pointer to the command allowed method
 * @param	in_desc	The command input parameter description
 * @param	out_desc	The command output parameter description
 * @param	level		The command display level
 *
 */
	TemplCommandOut(string &cmd_name,OUTARG (DeviceImpl::*exe_method)(),
		        bool (DeviceImpl::*state_method)(const CORBA::Any &),
		        string &in_desc,string &out_desc,
			Tango::DispLevel level);
//@}

	~TemplCommandOut() {delete ext;}
	
/**@name Miscellaneous methods */
//@{
/**
 * Initialise command input and output types.
 *
 * Set the command output type to Tango::DEV_VOID. The command input type is
 * automatically determined from the class template specialisation
 *
 */		     		     		     		     
	void init_types();
	
/**
 * Invoke the command execution method given at object creation time.
 *
 * This method is automtically called by the TANGO core classes when the
 * associated command is requested by a client. It invokes the user supplied 
 * command execution method and packs the returned data into the outgoing
 * CORBA Any object
 *
 * @param dev The device on which the command must be executed
 * @param in_any The incoming data still packed in a CORBA Any object. For
 * command created with this TemplCommandOut class, this Any object does not
 * contain usefull data
 * @return The CORBA Any object returned to the client.
 * @exception DevFailed If the execution method failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	CORBA::Any *execute (DeviceImpl *dev, const CORBA::Any &in_any);
//@}
		     
private:
	OUTARG (DeviceImpl::*exe_ptr_out)();
	TemplCommandOutExt		*ext;
};

//+-------------------------------------------------------------------------
//
// method : 		TempCommandOut class constructors
// 
// description : 	instance constructor
//
//--------------------------------------------------------------------------

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)())
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &))
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)())
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &))
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 const char *in_desc,const char *out_desc)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	init_types();
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 const char *in_desc,const char *out_desc)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 string &in_desc,string &out_desc)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	allowed_ptr = NULL;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 string &in_desc,string &out_desc)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 const char *in_desc,const char *out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	allowed_ptr = NULL;
	init_types();
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(const char *s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 const char *in_desc,const char *out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	if (in_desc != NULL)
		in_type_desc = in_desc;
	if (out_desc != NULL)
		out_type_desc = out_desc;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 string &in_desc,string &out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	allowed_ptr = NULL;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

template <class OUTARG>
TemplCommandOut<OUTARG>::TemplCommandOut(string &s,
					 OUTARG (DeviceImpl::*f)(),
					 bool (DeviceImpl::*a)(const CORBA::Any &),
					 string &in_desc,string &out_desc,
					 Tango::DispLevel level)
					 :exe_ptr_out(f),ext(NULL)
{
	name = s;
	allowed_ptr = a;
	in_type_desc = in_desc;
	out_type_desc = out_desc;
	set_disp_level(level);
	lower_name = name;
	transform(lower_name.begin(),lower_name.end(),lower_name.begin(),::tolower);
}

//+-------------------------------------------------------------------------
//
// method : 		init_types
// 
// description : 	Initialise input and output type from the template 
//			class specialisation parameters.
//
//--------------------------------------------------------------------------

template <class OUTARG>
void TemplCommandOut<OUTARG>::init_types()
{
	in_type = Tango::DEV_VOID;
//
// Set output type
//

	set_type(typeid(OUTARG),out_type);
}

//+-------------------------------------------------------------------------
//
// method : 		execute
// 
// description : 	Execute the method associated with the command
//			(stored in the exe_ptr data)
//
// input : - dev_ptr : pointer to the device on which the command must be
//		       executed
//	   - in_any : Incoming command data
//
// This method returns a pointer to an Any object with the command outing
// data.
//
//--------------------------------------------------------------------------

template <class OUTARG>
CORBA::Any *TemplCommandOut<OUTARG>::execute(DeviceImpl *dev_ptr,const CORBA::Any &in_any)
{

//
// Execute the command associated method 
//

	return insert((dev_ptr->*exe_ptr_out)());
}

} // End of Tango namespace

#endif // _COMMAND_H

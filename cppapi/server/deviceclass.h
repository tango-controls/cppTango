//=============================================================================
//
// file :		Deviceclass.h
//
// description :	Include file for the DeviceClass root class.
//
// project :		TANGO
//
// author(s) :		E.Taurel
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
//
//
//=============================================================================

#ifndef _DEVICECLASS_H
#define _DEVICECLASS_H

namespace Tango
{

class Command;
class Pipe;
class DeviceClass;
class AutoTangoMonitor;
class NoSyncModelTangoMonitor;
class EventSupplier;
class Util;
class DServer;


//=============================================================================
//
//			The DeviceClass class
//
//
// description : 	This class will act as root class for all other
//			DeviceClass classes. Its job is to define all Device
//			class related properties and methods which exist only
//			once e.g. the command list.
//
//=============================================================================

/**
 * Base class for all TANGO device-class class. A TANGO device-class class is
 * a class where is stored all data/method common to all devices of a TANGO
 * device class
 *
 *
 * @headerfile tango.h
 * @ingroup Server
 */

class
#ifdef _TG_WINDOWS_
	#ifndef _TANGO_LIB
		__declspec(dllexport)
	#endif
#endif
		DeviceClass
{
	friend class Tango::AutoTangoMonitor;
public:

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The device destructor.
 */
	virtual ~DeviceClass();
//@}

/**@name Miscellaneous methods */
//@{
/**
 * Execute a command.
 *
 * It looks for the correct command object in the command object vector.
 * If the command is found, it invoke the <i>always_executed_hook</i> method.
 * Check if the command is allowed by invoking the <i>is_allowed</i> method
 * If the command is allowed, invokes the <i>execute</i> method.
 *
 * @param 	device	The device on which the command must be executed
 * @param	command	The command name
 * @param	in_any 	The command input data still packed in a CORBA Any object
 * @return	A CORBA Any object with the output data packed in
 * @exception DevFailed If the command is not found, if the command is not allowed
 * in the actual device state and re-throws of all the exception thrown by the
 * <i>always_executed_hook</i>, <i>is_alloed</i> and <i>execute</i> methods.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 */
	CORBA::Any *command_handler(DeviceImpl *device,
				    std::string &command,const CORBA::Any &in_any);
/**
 * Create command objects for all command supported by this class of device.
 *
 * In the DeviceClass class, this method is pure abstract and must be defined
 * in sub-class. Its rule is to create the command object and to store them
 * in a vector of command objects
 *
 */
	virtual void command_factory() = 0;
/**
 * Create all the attributes name supported by this class of device.
 *
 * In the DeviceClass class, this method does nothing and must be re-defined
 * in sub-class if the sub-class supports attributes. Its rule is to
 * create and store the supported attributes in a vector.
 *
 */
	virtual void attribute_factory(std::vector<Attr *> &) {};
/**
 * Create all the pipes supported by this class of device.
 *
 * In the DeviceClass class, this method does nothing and must be re-defined
 * in sub-class if the sub-class supports pipes. Its rule is to
 * create and store the supported pipes in a vector.
 *
 */
	virtual void pipe_factory() {};

/**
 * Create device(s).
 *
 * In the DeviceClass class, this method is pure abstract and must be defined
 * in sub-class. Its rule is to create all the class devices and to store them
 * in a vector of device
 *
 * @param dev_list The device name list
 * @exception DevFailed This method does not throw exception but a
 * redefined method can.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	virtual void device_factory(const Tango::DevVarStringArray *dev_list) = 0;

/**
 * Create device(s) name list (for no database device server).
 *
 * This method can be re-defined in DeviceClass sub-class for device server
 * started without database. Its rule is to initialise class device name.
 * The default method does nothing.
 *
 * @param list Reference to the device name list
 */

	virtual void device_name_factory(std::vector<std::string> &list) {(void)list;};

/**
 * Delete device.
 *
 * The rule of this method is to delete a device from the running
 * server belonging to the Tango class. It does change anything in the
 * database
 *
 * @param dev_name Reference to the device name
 */

	void device_destroyer(const std::string &dev_name);

/**
 * Delete device.
 *
 * The rule of this method is to delete a device from the running
 * device belonging to the Tango class. It does change anything in the
 * database
 *
 * @param dev_name Reference to the device name
 */

	void device_destroyer(const char *dev_name);

//@}

/**@name Get/Set object members.
 * These methods allows the external world to get/set DeviceImpl instance
 * data members
 */
//@{
/**
 * Get the TANGO device class name.
 *
 * @return The TANGO device class name
 */
	std::string &get_name() {return name;}
/**
 * Get the TANGO device class documentation URL.
 *
 * @return The TANGO device class documentation
 */
	std::string &get_doc_url() {return doc_url;}
/**
 * Get the TANGO device type name.
 *
 * @return The TANGO device type name
 */
	std::string &get_type() {return type;}
/**
 * Get the device object vector.
 *
 * @return A reference to the device vector
 */
	std::vector<DeviceImpl *> &get_device_list() {return device_list;}
/**
 * Get the command object vector.
 *
 * @return A reference to the command vector
 */
	std::vector<Command *> &get_command_list() {return command_list;}
/**
 * Get the pipe object vector.
 *
 * @param dev_name The device name
 *
 * @return A reference to the pipe vector containing all device pipes
 */
	std::vector<Pipe *> &get_pipe_list(const std::string &dev_name);
/**
 * Get a reference to a command object.
 *
 * @return A reference to the command object
 */
	Command &get_cmd_by_name(const std::string &);
/**
 * Get a reference to a pipe object.
 *
 * @param pipe_name The pipe name
 * @param dev_name The device name
 *
 * @return A reference to the pipe object
 */
	Pipe &get_pipe_by_name(const std::string &pipe_name,const std::string &dev_name);
/**
 * Get a pointer to the associated DbClass object.
 *
 * @return Pointer to the DbClas object
 */

	DbClass *get_db_class() {return db_class;}
/**
 * Get a pointer to the class attributes object
 *
 * @return A pointer to the instance of the MultiClassAttribute
 */
 	MultiClassAttribute *get_class_attr() {return class_attr;}
/**
 * Get a pointer to the class pipes object
 *
 * @return A pointer to the instance of the MultiClassPipe
 */
 	MultiClassPipe *get_class_pipe() {return class_pipe;}
/**
 * Set the TANGO device type name.
 *
 * @param dev_type The new TANGO device type name
 */
	void set_type(std::string &dev_type) {type = dev_type;}
/**
 * Set the TANGO device type name.
 *
 * @param dev_type The new TANGO device type name
 */
	void set_type(const char *dev_type) {type = dev_type;}
//@}


/**@name Signal related methods
 * These methods allow a signal management at device level */
//@{
#if defined _TG_WINDOWS_
/**
 * Register a signal.
 *
 * Register this class as class to be informed when signal signo is sent to
 * to the device server process
 *
 * @param signo The signal number
 * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to register a signal for the process.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void register_signal(long signo);
#else
/**
 * Register a signal.
 *
 * Register this class as class to be informed when signal signo is sent to
 * to the device server process. This method is available only under Linux.
 *
 * @param signo The signal number
 * @param own_handler A boolean set to true if you want the device signal handler
 * to be executed in its own handler instead of being executed by the signal
 * thread. If this parameter is set to true, care should be taken on how the
 * handler is written. A default false value is provided * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to register a signal for the process.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void register_signal(long signo,bool own_handler = false);
#endif

/**
 * Unregister a signal.
 *
 * Unregister this class as class to be informed when signal signo is sent to
 * to the device server process
 *
 * @param signo The signal number
 * @exception DevFailed Thrown if the signal number is out of range or if the
 * operating system failed to unregister a signal for the process. Unregister
 * a device for a signal number for a device not previously registered is not
 * an error. This simply will do nothing.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void unregister_signal(long signo);

/**
 * Signal handler.
 *
 * The method executed when the signal arrived in the device server process.
 * This method is defined as virtual and then, can be redefined following
 * device class needs.
 *
 * @param signo The signal number
 */
	virtual void signal_handler(long signo);

//@}

protected:
/**@name Constructor
 Only one constructot for this class which is a singleton */
//@{
/**
 * Construct a newly allocated DeviceClass object.
 *
 * @param 	s	The Tango device class name
 *
 */
	DeviceClass(std::string &s);
//@}

/**@name Miscellaneous protected methods */
//@{
/**
 * Export a device.
 *
 * Associate the servant to a CORBA object and send device network parameter
 * to TANGO database. The main parameter sent to database is the CORBA
 * object stringified device IOR.
 *
 * @param dev The device to be exported (CORBA servant)
 * @param corba_dev_name The name to be used in the CORBA object key. This
 * parameter does not need to be set in most of cases and has a default value.
 * It is used for special device server like the database device server.
 * @exception DevFailed If the command sent to the database failed.
 * Click <a href="https://tango-controls.readthedocs.io/en/latest/development/advanced/IDL.html#exceptions">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void export_device(DeviceImpl *dev,const char* corba_dev_name = "Unused");

/**
 * Set a Tango classs default command
 *
 * Define one command to be the Tango class default command
 * The default command is the command which will be exceuted when
 * an unknown command is sent to one of the Tango class device
 * By default, there is no default class
 *
 * @param cmd The command object
 */

	void set_default_command(Command *cmd) {default_cmd = cmd;}
//@}

/**@name Class data members */
//@{
/**
 * The TANGO device class name
 */
	std::string 					name;
/**
 * The TANGO device class documentation URL
 */
	std::string					doc_url;
/**
 * The TANGO device type name
 */
	std::string					type;
/**
 * The command(s) list
 */
	std::vector<Command *> 		command_list;
/**
 * The device(s) list
 */
	std::vector<DeviceImpl *>	device_list;
/**
 * The associated DbClass object
 */
	DbClass					*db_class;
/**
 * Pointer to the class multi attribute object
 */
 	MultiClassAttribute		*class_attr;
/**
 * Pointer to the class multi pipe object
 */
 	MultiClassPipe			*class_pipe;
/**
 * The pipe(s) list
 */
	std::vector<Pipe *> 			pipe_list;
//@}

public:
/// @privatesection
	std::vector<std::string> &get_nodb_name_list() {return nodb_name_list;}
	void set_memorized_values(bool flag, long idx = 0,bool from_init = false);

	void add_wiz_dev_prop(std::string &name,std::string &desc,std::string &def);
	void add_wiz_dev_prop(std::string &name,std::string &desc);

	void add_wiz_class_prop(std::string &name,std::string &desc,std::string &def);
	void add_wiz_class_prop(std::string &name,std::string &desc);

	std::vector<std::string> &get_wiz_class_prop() {return wiz_class_prop;}
	std::vector<std::string> &get_wiz_dev_prop() {return wiz_dev_prop;}

	std::string &get_cvs_tag() {return cvs_tag;}
	std::string &get_cvs_location() {return cvs_location;}

	std::string &get_svn_tag() {return svn_tag;}
	std::string &get_svn_location() {return svn_location;}

	void set_cvs_tag(std::string &str) {cvs_tag=str;}
	void set_cvs_location(std::string &str) {cvs_location=str;}

	void add_device(DeviceImpl *dev) {device_list.push_back(dev);}
	void delete_dev(long idx,Tango::Util *tg,PortableServer::POA_ptr r_poa);

	bool is_py_class() {return py_class;}
	void set_py_class(bool py) {py_class=py;}
	virtual void delete_class() {}
	void get_mcast_event(DServer *);

	bool is_command_allowed(const char *);

	bool get_device_factory_done() {return device_factory_done;}
	void set_device_factory_done(bool val) {device_factory_done = val;}

	void check_att_conf();
	void release_devices_mon();

	void remove_command(const std::string &);

	void create_device_pipe(DeviceClass *,DeviceImpl *);
	std::vector<Pipe *> &get_pipe_list() {return pipe_list;}

protected:
/// @privatesection
	Command *get_default_command() {return default_cmd;}

private:
    class DeviceClassExt
    {
    public:
        DeviceClassExt() {};

        std::map<std::string,std::vector<Pipe *> > dev_pipe_list;
    };

	void get_class_system_resource();
	void throw_mem_value(DeviceImpl *,AttributePrivate &);

	std::vector<std::string>			wiz_class_prop;
	std::vector<std::string>			wiz_dev_prop;

	std::vector<std::string>			allowed_cmds;

    std::unique_ptr<DeviceClassExt>      ext;           // Class extension

//
// Ported from the extension class
//

    std::vector<std::string>		nodb_name_list;
    TangoMonitor		only_one;
    std::string				cvs_tag;
    std::string				cvs_location;
    Command * 			default_cmd;
    bool				py_class;
    std::string              svn_tag;
    std::string              svn_location;
    bool                device_factory_done;
};


} // End of Tango namespace

#endif // _DEVICECLASS_H

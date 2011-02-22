//=============================================================================
//
// file :               utils.h
//
// description :        Include for utility functions or classes  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.14  2006/02/17 16:48:18  jensmeyer
// Correction for porting to VC7 under windows.
//
// Revision 3.13  2006/01/20 08:41:32  taurel
// - Only change in indentention...
//
// Revision 3.12  2006/01/20 08:33:28  taurel
// - Added necessary changes to support Device server written in Python
//
// Revision 3.11  2005/12/16 17:41:12  jensmeyer
// Added a check fro ATTR_WARNING quality.
//
// Revision 3.10  2005/06/29 08:31:19  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.9  2005/01/21 19:58:30  taurel
// - Some changes in include files for gcc 3.4.2
//
// Revision 3.8  2005/01/13 09:27:53  taurel
// Fix some bugs :
// - R/W attribute : W value not returned when read if set by set_write_value
// - Core dumped when retrieving attribute polling history for Device_2Impl device which
//   has stored an exception
// - Remove device_name in lib default attribute label property
// - Lib default value for label not store in db any more
// - Size of the DaData used by the Database::get_device_attribute_property() and
//   Database::get_class_attribute_property()
// - R/W attribute: W part not returned when read for Device_2Impl device
// Some changes :
// - Improvement of the -file option error management (now throw exception in case of
//   error)
// - Reset "string" attribute property to the default value (lib or user) when new
//   value is an empty string
//
// Revision 3.6.2.11  2004/12/06 14:39:30  taurel
// - Polling starts in a separate thread
// - For Windows : Polling thread cmd/attr measurement used PerformanceCounter
// - Fix bug in pollext.h in methods to externally fill polling buffer
//
// Revision 3.6.2.10  2004/11/22 14:14:46  taurel
// - Added methods for the AttrDesc class for the new attribute config
//   parameters.
// - Some minor changes
//
// Revision 3.6.2.9  2004/11/10 13:00:57  taurel
// - Some minor changes for the WIN32 port
//
// Revision 3.6.2.8  2004/11/04 09:46:40  taurel
// - Add a tuning method in the polling thread
// - Some minor fixes to pass test suite
//
// Revision 3.6.2.7  2004/10/27 05:59:47  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.6.2.6  2004/10/05 13:55:03  maxchiandone
// First upload for filedatabase.
//
// Revision 3.6.2.5  2004/09/27 09:10:06  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.6.2.4  2004/09/15 06:47:17  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.6.2.3  2004/08/26 07:34:45  taurel
// - Implement a way to directly fills command or attribute polling buffer
//
// Revision 3.6.2.2  2004/08/19 07:44:59  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.6.2.1  2004/07/15 15:04:06  taurel
// - Added the way to externally filled the polling buffer for attribute
//   (Command will come soon)
//
// Revision 3.6  2004/07/07 08:40:13  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.5  2004/06/15 12:38:22  taurel
// - Last commit before merge with release 4 branch
//
// Revision 3.4  2003/08/21 07:24:37  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.3  2003/07/03 07:40:51  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.2.2.7  2004/04/07 11:22:10  taurel
// - Add some import/export declaration for Windows DLL
// - Add test on minor code for the CORBA::IMP_LIMIT exception before
//   printing it
//
// Revision 3.2.2.6  2004/02/27 16:47:01  taurel
// - Updated DeviceImpl and Util classes sizes for compatibility
//
// Revision 3.2.2.5  2004/01/20 08:32:38  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.2.2.4  2003/11/28 17:08:44  taurel
// Changes to handle tango lib build as a Windows DLL and used in a device
// server
//
// Revision 3.2.2.2  2003/10/03 13:34:26  taurel
// - Fix bug for device server started without database, with device name specified on command line using mix of upper and lower cases
// - It's now possible to send a command to the polling thread from itself
// - Add a release lock if the insert into the polling buffer failed
//
// Revision 3.2.2.1  2003/09/30 11:49:25  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.2  2003/05/16 08:46:16  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.1  2003/04/02 12:26:04  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
// Revision 3.0.2.1  2003/04/08 13:12:54  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:48:02  taurel
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
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _UTILS_H
#define _UTILS_H

#include <tango.h>
#include <pollthread.h>
#include <pollext.h>
#include <new>

#ifndef WIN32
	#include <unistd.h>
#else
	#include <coutbuf.h>
	#include <w32win.h>
#endif /* WIN32 */


//
// For debug purpose
//

#ifndef TANGO_HAS_LOG4TANGO

#define	cout1	if ((Tango::Util::_tracelevel >= 1) && \
		    (Tango::Util::_tracelevel < 5)) cout 
#define cout2  	if ((Tango::Util::_tracelevel >= 2) && \
		    (Tango::Util::_tracelevel < 5)) cout
#define cout3	if ((Tango::Util::_tracelevel >= 3) && \
		    (Tango::Util::_tracelevel < 5)) cout
#define cout4	if ((Tango::Util::_tracelevel >= 4) && \
		    (Tango::Util::_tracelevel < 5)) cout

#define cout5   if (Tango::Util::_tracelevel >= 5) cout

#endif //TANGO_HAS_LOG4TANGO


namespace Tango
{

class DeviceImpl;
class DeviceClass;
class DServer;
class AutoTangoMonitor;
class Util;
class EventSupplier;
class PyLock;
class CreatePyLock;

#ifdef WIN32
class CoutBuf;
class W32Win;
#endif


class PyLock
{
public:
	PyLock() {}
	virtual ~PyLock() {}
	
	virtual void Get() {}
	virtual void Release() {}
};

class CreatePyLock
{
public:
	CreatePyLock() {}
	virtual ~CreatePyLock() {}
	
	virtual PyLock *create() {return new PyLock();}
};


//=============================================================================
//
//			The Util class
//
// description :	This class contains all properties and methods
//			which a device server process requires only once e.g. 
//			the orb and boa pointers....
//			This class is a singleton ( The constructor is 
//			protected and the _instance data member is static)
//
//			This class must be created at the beginning of each
//			device server process
//
//=============================================================================


class UtilExt
{
public:
	UtilExt():poll_mon("utils_poll"),ser_model(BY_DEVICE),only_one("process"),
	 	  py_interp(NULL),py_ds(false),py_dbg(false)
	{shared_data.cmd_pending=false;shared_data.trigger=false;
	cr_py_lock = new CreatePyLock();}
	
	vector<string>		cmd_line_name_list;
	
	PollThread		*poll_th;		// The polling thread object
	int			poll_th_id;		// The polling thread identifier
	PollThCmd		shared_data;		// The shared buffer
	TangoMonitor		poll_mon;		// The monitor
	bool			poll_on;		// Polling on flag
	SerialModel		ser_model;		// The serialization model
	TangoMonitor		only_one;		// Serialization monitor
	EventSupplier		*event_supplier;	// The event supplier object

	void			*py_interp;		// The Python interpreter
	bool			py_ds;			// The Python DS flag
	CreatePyLock		*cr_py_lock;		// The python lock creator pointer
	bool			py_dbg;			// Badly written Python dbg flag
};

/**
 * This class is a used to store TANGO device server process data and to provide
 * the user with a set of utilities method. This class is implemented using
 * the singleton design pattern. Therefore a device server process can have only
 * one instance of this class and its constructor is not public.
 *
 * $Author$
 * $Revision$
 */
 
class Util
{
	friend class Tango::AutoTangoMonitor;
	friend class Tango::ApiUtil;
public:

/**@name Singleton related methods
 * These methods follow the singleton design pattern (only one instance
 * of a class) */
//@{

/**
 * Create and get the singleton object reference.
 *
 * This method returns a reference to the object of the Util class.
 * If the class singleton object has not been created, it will be 
 * instanciated
 *
 * @param argc The process command line argument number
 * @param argv The process commandline arguments
 * @return The Util object reference
 */
	static Util *init(int argc,char *argv[]);
#ifdef WIN32
/**
 * Create and get the singleton object reference.
 *
 * This method returns a reference to the object of the Util class.
 * If the class singleton object has not been created, it will be 
 * instanciated. This method must be used only for non-console mode windows 
 * device server
 *
 * @param AppInst The application instance
 * @param CmdShow The display window flag
 * @return The Util object reference
 */
        TANGO_IMP_EXP static Util *init(HINSTANCE AppInst,int CmdShow);
#endif
	
/**
 * Get the singleton object reference.
 *
 * This method returns a reference to the object of the Util class.
 * If the class has not been initialised with it's init method, this method
 * print a message and abort the device server process
 *
 * @return The Util object reference
 */
	TANGO_IMP_EXP static Util *instance(bool exit = true);
//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The class destructor.
 */
	~Util();
//@}

/**@name Get/Set instance data */
//@{
/**
 * Get a reference to the CORBA ORB
 *
 * This is a CORBA _duplicate of the original reference
 *
 * @return The CORBA ORB
 */		
	CORBA::ORB_ptr get_orb() {return CORBA::ORB::_duplicate(orb);}
	
/**
 * Get a reference to the CORBA Portable Object Adapter (POA)
 *
 * This is a CORBA _dupilcate of the original reference to the object POA.
 * For classical device server, thisis the root POA. For no database device
 * server, this is a specific POA with the USER_ID policy.
 *
 * @return The CORBA root POA
 */
	PortableServer::POA_ptr get_poa() {return PortableServer::POA::_duplicate(_poa);}

//
// process output control methods
//

/**
 * Set the process trace level.
 *
 * @param level The new process level
 */	
	void set_trace_level(long level) {_tracelevel = level;}
	
/**
 * Get the process trace level.
 *
 * @return The process trace level
 */
	long get_trace_level() {return _tracelevel;}

#ifndef TANGO_HAS_LOG4TANGO
/**
 * Set the process trace output .
 *
 * @param level The new process output
 */	
	void set_trace_output(string &trace) {trace_output = trace;}
	
/**
 * Get the process trace output.
 *
 * @return The process trace output
 */	
	string &get_trace_output() {return trace_output;}	

/**
 * Get the temporary process output print stream
 *
 * @return The process output print stream
 */	
	TangoSys_Cout &get_out() {return cout_tmp;}
	
/**
 * Set the process trace output stream.
 *
 * @param in The new process output stream
 */	
	void set_trace_output_stream(ofstream *in) {file_stream = in;}
	
/**
 * Get the process output print stream
 *
 * @return The process output print stream
 */
	ofstream *get_trace_output_stream() {return file_stream;}
#endif //TANGO_HAS_LOG4TANGO

/**
 * Get the device server instance name.
 *
 * @return The device server instance name
 */	
	string &get_ds_inst_name() {return ds_instance_name;}
	
/**
 * Get the device server executable name.
 *
 * @return The device server executable name
 */
	string &get_ds_exec_name() {return ds_exec_name;}
	
/**
 * Get the device server name.
 *
 * The device server name is the device server executable name/the device
 * server instance name
 * @return The device server name
 */
	string &get_ds_name() {return ds_name;}

/**
 * Get the host name where the device server process is running.
 *
 * @return The host name
 */	
	string &get_host_name() {return hostname;}
	
/**
 * Get the device server process identifier as a String
 *
 * @return The device server process identifier as a string
 */
	string &get_pid_str() {return pid_str;}
	
/**
 * Get the device server process identifier
 *
 * @return The device server process identifier
 */
	TangoSys_Pid get_pid() {return pid;}

/**
 * Get the IDL TANGO version.
 *
 * @return The device server version
 */	
	string &get_version_str() {return version_str;}
	
/**
 * Get the device server version.
 *
 * @return The device server version
 */	
	string &get_server_version() {return server_version;}

/**
 * Set the device server version.
 *
 * @param The device server version
 */	
	void set_server_version(const char *vers) {server_version = vers;}
	
/**
 * Set the DeviceClass list pointer
 *
 * @param The DeviceClass ptr vector address
 */
 	void set_class_list(vector<DeviceClass *> *list) {cl_list_ptr = list;cl_list = *list;}

/**
 * Add a DeviceClass to the DeviceClass list pointer
 *
 * @param The DeviceClass ptr
 */
 	void add_class_to_list(DeviceClass * cl) {cl_list.push_back(cl);}
		
/**
 * Get the DeviceClass list pointer
 *
 * @return The DeviceClass ptr vector address
 */	
	const vector<DeviceClass *> *get_class_list() {return &cl_list;}
	
/**
 * Set the serialization model
 *
 * @param The new serialazation model. The serialization model must be one
 * of BY_DEVICE, BY_CLASS, BY_PROCESS or NO_SYNC
 */	
	void set_serial_model(SerialModel ser) {ext->ser_model = ser;}
	
/**
 * Get the serialization model
 *
 * @return The serialization model. This serialization model is one of
 * BY_DEVICE, BY_CLASS, BY_PROCESS or NO_SYNC
 */	
	SerialModel get_serial_model() {return ext->ser_model;}
/**
 * Get a reference to the TANGO EventSupplier object
 *
 * @return The EventSupplier object
 */
	EventSupplier *get_event_supplier() {return ext->event_supplier;}
//@}

	vector<string> &get_cmd_line_name_list() {return ext->cmd_line_name_list;}
	TangoMonitor &get_poll_monitor() {return ext->poll_mon;}
	PollThCmd &get_poll_shared_cmd() {return ext->shared_data;}
	bool poll_status() {return ext->poll_on;}
	void poll_status(bool status) {ext->poll_on = status;}
	void polling_configure();
	PollThread *get_polling_thread_object() {return ext->poll_th;}
	void clr_poll_th_ptr() {ext->poll_th = NULL;}
	int get_polling_thread_id() {return ext->poll_th_id;}
	void create_event_supplier();
	
	
	void *get_py_interp() {return ext->py_interp;}
	void set_py_interp(void *ptr) {ext->py_interp = ptr;}

	bool is_py_ds() {return ext->py_ds;}
	void set_py_ds() {ext->py_ds=true;}
	
	bool is_py_dbg() {return ext->py_dbg;}
	void set_py_dbg() {ext->py_dbg=true;}

	void set_py_lock_creator(CreatePyLock *py) {ext->cr_py_lock = py;}
	CreatePyLock *get_py_lock_creator() {return ext->cr_py_lock;}
			
/**@name Database related methods */
//@{
/**
 * Connect the process to the TANGO database.
 *
 * If the connection to the database failed, a message is displayed on the
 * screen and the process is aborted
 */
	void connect_db();

/**
 * Reread the file database
 */ 
	void reset_filedatabase();
		
/**
 * Get a reference to the TANGO database object
 *
 * @return The database object
 */
	Database *get_database() {return db;}
		
/**
 * Unregister a device server process from the TANGO database.
 *
 * If the database call fails, a message is displayed on the screen and the
 * process is aborted
 */	
	void unregister_server();
//@}

/** @name Device reference related methods */
//@{
/**
 * Get the list of device references for a given TANGO class.
 *
 * Return the list of references for all devices served by one implementation
 * of the TANGO device pattern implemented in the  process
 *
 * @param class_name The TANGO device class name
 * @return The device reference list
 * @exception DevFailed If in the device server process there is no TANGO 
 * device pattern implemented the TANGO device class given as parameter
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	vector<DeviceImpl *> &get_device_list_by_class(const string &class_name);
	
/**
 * Get the list of device references for a given TANGO class.
 *
 * Return the list of references for all devices served by one implementation
 * of the TANGO device pattern implemented in the  process
 *
 * @param class_name The TANGO device class name
 * @return The device reference list
 * @exception DevFailed If in the device server process there is no TANGO 
 * device pattern implemented the TANGO device class given as parameter
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	vector<DeviceImpl *> &get_device_list_by_class(const char *class_name);
	
/**
 * Get a device reference from its name
 *
 * @param dev_name The TANGO device name
 * @return The device reference
 * @exception DevFailed If in the device is not served by one device pattern
 * implemented in this process.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	DeviceImpl *get_device_by_name(const string &dev_name);
	
/**
 * Get a device reference from its name
 *
 * @param dev_name The TANGO device name
 * @return The device reference
 * @exception DevFailed If in the device is not served by one device pattern
 * implemented in this process.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	DeviceImpl *get_device_by_name(const char *dev_name);
	
/**
 * Get a reference to the dserver device attached to the device server process
 *
 * @return A reference to the dserver device
 */
	DServer *get_dserver_device();
	
/**
 * Get DeviceList from name.
 *
 * It is possible to use a wild card ('*') in the name parameter
 *  (e.g. "*", "/tango/tangotest/n*", ...)
 *
 * @param name The device name
 * @return The DeviceClass ptr vector address
 */	
	vector<DeviceImpl *> get_device_list(const string &name);
//@}

/** @name Device pattern related methods */
//@{
/**
 * Initialise all the device server pattern(s) embedded in a device server
 * process.
 *
 * @exception DevFailed If the device pattern initialistaion failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void server_init(bool with_window = false);
	
/**
 * Run the CORBA event loop
 *
 * This method runs the CORBA event loop. For UNIX or Linux operating system,
 * this method does not return. For Windows in a non-console mode, 
 * this method start a thread which enter the CORBA event loop.
 */
 	void server_run();
	
/**
 * Cleanup a Tango device server process before exit
 *
 * This method cleanup a Tango device server and relinquish all computer
 * resources before the process exit
 */
 	void server_cleanup();
//@}


/**@name Class data members */
//@{
/**
 * The process trace level
 */	
	static long		_tracelevel;
/**
 * The database use flag (Use with extreme care). Implemented for device
 * server started without database usage.
 */
	TANGO_IMP static bool	_UseDb;

	TANGO_IMP static bool	_FileDb;
/**
 * A daemon process flag. If this flag is set to true, the server
 * process will not exit if it not able to connect to the database.
 * Instead, it will loop until the connection suceeds. The default
 * value is false.
 */
 	TANGO_IMP static bool	_daemon;
/**
 * The loop sleeping time in case of the _daemon flag set to true.
 * This sleeping time is the number of seconds the process will
 * sleep before it tries again to connect to the database. The default
 * value is 60 seconds.
 */
 	TANGO_IMP static long	_sleep_between_connect;

	
//@}	

#ifdef WIN32
/**@name Windows specific methods */
//@{
/**
 * Get the console window instance
 *
 * @return The device server graphical interface console window instance
 */
 
	HWND			get_console_window();
	
/**
 * Get the main window instance
 *
 * @return The device server graphical interface main window instance
 */
	HWND			get_ds_main_window();

/**
 * Get a pointer to the debug object
 *
 * @return A pointer to the debug object
 */
	CoutBuf			*get_debug_object();

	TANGO_IMP static bool	_service;
/**
 * Get the text displayed on main server window.
 *
 * @return The text to be displayed
 */	
	string &get_main_window_text() {return main_win_text;}

/**
 * Set the text displayed on main server window.
 *
 * @param 	txt 	The new text to be displayed at the bottom of the
 * main window
 */	
	void set_main_window_text(string &txt) {main_win_text = txt;}
//@}
#endif

protected:
/**
 * Constructs a newly allocated Util object.
 *
 * This constructor is protected following the singleton pattern
 *
 * @param argc The process command line argument number
 * @param argv The process commandline arguments
 *
 */
	Util(int argc,char *argv[]);
#ifdef WIN32
/**
 * Constructs a newly allocated Util object for Windows non-console
 * device server.
 *
 * This constructor is protected following the singleton pattern
 *
 * @param AppInst The applicationinstance
 * @param CmdShow The display window flag
 *
 */
	Util(HINSTANCE AppInst,int CmdShow);
#endif
		
private:
	TANGO_IMP static Util		*_instance;
	static bool		_constructed;
#ifdef WIN32
        static bool             _win;
        int                     argc;
        char                    **argv;
	int			nCmd;
	CoutBuf			*pcb;
	W32Win			*ds_window;
	string			main_win_text;
	bool			go;
	TangoMonitor		mon;

        void                    build_argc_argv();
	
	class ORBWin32Loop: public omni_thread	
	{
		Util *util;
	public:
		ORBWin32Loop(Util *u):util(u) {}
		virtual ~ORBWin32Loop() {}
		
		void *run_undetached(void *);
		void start() {start_undetached();}
	private:
		void wait_for_go();
	};
	friend class ORBWin32Loop;
	ORBWin32Loop *loop_th;
#endif

	CORBA::ORB_var 		orb;
	PortableServer::POA_var _poa;
	
	string			ds_instance_name;	// The instance name
	string			ds_exec_name;		// The server exec. name
	string			ds_name;		// The server name
	
	string			hostname;		// The host name
	string			pid_str;		// The process PID (as string)
	TangoSys_Pid		pid;			// The process PID

	string			version_str;		// Tango version
	string			server_version;		// Device server version

	string      database_file_name;

#ifndef TANGO_HAS_LOG4TANGO	
	string			trace_output;
	TangoSys_Cout		cout_tmp;
	ofstream		*file_stream;
#endif //TANGO_HAS_LOG4TANGO

	Database		*db;			// The db proxy
	
        void effective_job(int,char *[]);
	void misc_init();
	void server_already_running();	
	void print_usage(char *);
        static void print_err_message(const char *,Tango::MessBoxType type = Tango::STOP);
        void print_err_message(const string &mess,Tango::MessBoxType type = Tango::STOP)
        {
                Util::print_err_message(mess.c_str(),type);
        }
	void check_args(int, char *[]);
	void display_help_message();

	bool  display_help;				// display help message flag
	const vector<DeviceClass *>	*cl_list_ptr;	// Ptr to server device class list	
	UtilExt				*ext;		// Class extension
	vector<DeviceClass *>		cl_list;	// Full class list ptr

public:

/** @name Polling related methods */
//@{
/**
 * Trigger polling for polled command.
 *
 * This method send the order to the polling thread to poll one object
 * registered with an update period defined as "externally triggerred"  
 *
 * @param dev The TANGO device
 * @param name The command name which must be polled
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void trigger_cmd_polling(DeviceImpl *dev,string &name);
	
/**
 * Trigger polling for polled attribute.
 *
 * This method send the order to the polling thread to poll one object
 * registered with an update period defined as "externally triggerred"  
 *
 * @param dev The TANGO device
 * @param name The attribute name which must be polled
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	void trigger_attr_polling(DeviceImpl *dev,string &name);

/**
 * Fill polling buffer for polled attribute.
 *
 * This method fills the polling buffer for one polled attribute
 * registered with an update period defined as "externally triggerred" 
 * (polling period set to 0) 
 *
 * @param dev The TANGO device
 * @param att_name The attribute name which must be polled
 * @param data The data stack with one element for each history element
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	template <typename T>
	void fill_attr_polling_buffer(DeviceImpl *dev,
				      string &att_name,
				      AttrHistoryStack<T>  &data)
	{

//
// Check that the device is polled
//

		if (dev->is_polled() == false)
		{
			TangoSys_OMemStream o;
			o << "Device " << dev->get_name() << " is not polled" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}
	
//
// Attribute name in lower case letters and check that it is marked as polled
//

		string obj_name(att_name);
		transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
		
		dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);
	
//
// Get a reference on the Attribute object
//

		Tango::Attribute &att = dev->get_device_attr()->get_attr_by_name(att_name.c_str());

//
// Check that it is a READ only attribute
//

		Tango::AttrWriteType w_type = att.get_writable();
		if (w_type != Tango::READ)
		{
			TangoSys_OMemStream o;
			o << "Attribute " << att_name;
			o << " of device " << dev->get_name() << " is not READ only" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}
			
//
// Check that history is not larger than polling buffer
//

		long nb_elt = data.length();
		long nb_poll = dev->get_attr_poll_ring_depth(att_name);

		if (nb_elt > nb_poll)
		{
			TangoSys_OMemStream o;
			o << "The polling buffer depth for attribute " << att_name;
			o << " for device " << dev->get_name();
			o << " is only " << nb_poll;
			o << " which is less than " << nb_elt << "!" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}
	
//
// A loop on each record
//
	
		unsigned long i;	
		Tango::DevFailed *save_except;
		Tango::AttributeValueList_3 *back;
		bool attr_failed;
		
		struct timeval zero,when;
		zero.tv_sec = zero.tv_usec = 0;

		for (i = 0;i < nb_elt;i++)
		{
			save_except = NULL;
			back = NULL;
			attr_failed = false;

			if ((data.get_data())[i].err.length() != 0)
			{
				attr_failed = true;
				try
				{
					save_except = new Tango::DevFailed((data.get_data())[i].err);
				}
				catch (bad_alloc)
				{
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_attr_polling_buffer");
				}		
			}
			else
			{
			
//
// Allocate memory for the AttributeValueList sequence
//

				try
				{
					back = new Tango::AttributeValueList_3(1);
					back->length(1);
				}
				catch (bad_alloc)
				{
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_attr_polling_buffer");
				}

//
// Init name,date and quality factor
//

				Tango::AttrQuality qu = (data.get_data())[i].qual;
				(*back)[0].time.tv_sec = (data.get_data())[i].t_val.tv_sec;
				(*back)[0].time.tv_usec = (data.get_data())[i].t_val.tv_usec;
				(*back)[0].time.tv_nsec = 0;

				(*back)[0].quality = qu;
				(*back)[0].name = CORBA::string_dup(att_name.c_str());

				(*back)[0].w_dim.dim_x = 0;
				(*back)[0].w_dim.dim_y = 0;
				(*back)[0].r_dim.dim_x = 0;
				(*back)[0].r_dim.dim_y = 0;

				if ((qu == Tango::ATTR_VALID) || 
				    (qu == Tango::ATTR_ALARM) ||
				    (qu == Tango::ATTR_WARNING) ||  
				    (qu == Tango::ATTR_CHANGING))
				{
				
//
// Set Attribute object value
//

					att.wanted_date(false);
					att.set_value((T *)(data.get_data())[i].ptr,
						      (data.get_data())[i].x,
						      (data.get_data())[i].y,
						      (data.get_data())[i].release);
					att.set_date((data.get_data())[i].t_val);			      
					att.set_quality(qu,false);
					      
//
// Init remaining fields in AttributeValueList
//

					CORBA::Any &a = (*back)[0].value;
					switch (att.get_data_type())
					{
					case Tango::DEV_SHORT :
						a <<= att.get_short_value();
						break;

					case Tango::DEV_LONG :
						a <<= att.get_long_value();
						break;

					case Tango::DEV_DOUBLE :
						a <<= att.get_double_value();
						break;

					case Tango::DEV_STRING :
						a <<= att.get_string_value();
						break;
						
					case Tango::DEV_FLOAT :
						a <<= att.get_float_value();
						break;

					case Tango::DEV_BOOLEAN :
						a <<= att.get_boolean_value();
						break;

					case Tango::DEV_USHORT :
						a <<= att.get_ushort_value();
						break;

					case Tango::DEV_UCHAR :
						a <<= att.get_uchar_value();
						break;
					}
					(*back)[0].r_dim.dim_x = (data.get_data())[i].x;
					(*back)[0].r_dim.dim_y = (data.get_data())[i].y;				
				}
			}

//
// Fill one slot of polling buffer
//

			try
			{
				dev->get_poll_monitor().get_monitor();		
				vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_ATTR,obj_name);

				if (attr_failed == false)
				{
					when.tv_sec = (*back)[0].time.tv_sec - DELTA_T;
					when.tv_usec = (*back)[0].time.tv_usec;
					(*ite)->insert_data(back,when,zero);
				}
				else
				{
					when = (data.get_data())[i].t_val;
					when.tv_sec = when.tv_sec - DELTA_T;
					(*ite)->insert_except(save_except,when,zero);
				}
				dev->get_poll_monitor().rel_monitor();
			}
			catch (Tango::DevFailed &)
			{
				if (attr_failed == false)
				{
					delete back;
				}
				else
					delete save_except;
				dev->get_poll_monitor().rel_monitor();
			}

		}
	}

/**
 * Fill polling buffer for polled command.
 *
 * This method fills the polling buffer for one polled command
 * registered with an update period defined as "externally triggerred" 
 * (polling period set to 0) 
 *
 * @param dev The TANGO device
 * @param cmd_name The command name which must be polled
 * @param data The data stack with one element for each history element
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	template <typename T>
	void fill_cmd_polling_buffer(DeviceImpl *dev,
				     string &cmd_name,
				     CmdHistoryStack<T>  &data)
	{

//
// Check that the device is polled
//

		if (dev->is_polled() == false)
		{
			TangoSys_OMemStream o;
			o << "Device " << dev->get_name() << " is not polled" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_cmd_polling_buffer");
		}
	
//
// Command name in lower case letters and check that it is marked as polled
//

		string obj_name(cmd_name);
		transform(obj_name.begin(),obj_name.end(),obj_name.begin(),::tolower);
		
		dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
			
//
// Check that history is not larger than polling buffer
//

		long nb_elt = data.length();
		long nb_poll = dev->get_cmd_poll_ring_depth(cmd_name);

		if (nb_elt > nb_poll)
		{
			TangoSys_OMemStream o;
			o << "The polling buffer depth for command " << cmd_name;
			o << " for device " << dev->get_name();
			o << " is only " << nb_poll;
			o << " which is less than " << nb_elt << "!" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_cmd_polling_buffer");
		}
	
//
// A loop on each record
//
	
		unsigned long i;	
		Tango::DevFailed *save_except;
		bool cmd_failed;
		CORBA::Any *any_ptr;
		
		struct timeval zero,when;
		zero.tv_sec = zero.tv_usec = 0;

		for (i = 0;i < nb_elt;i++)
		{
			save_except = NULL;
			cmd_failed = false;

			if ((data.get_data())[i].err.length() != 0)
			{
				cmd_failed = true;
				try
				{
					save_except = new Tango::DevFailed((data.get_data())[i].err);
				}
				catch (bad_alloc)
				{
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_cmd_polling_buffer");
				}		
			}
			else
			{
			
//
// Allocate memory for the Any object
//

				try
				{
					any_ptr = new CORBA::Any();
				}
				catch (bad_alloc)
				{
					Except::throw_exception((const char *)"API_MemoryAllocation",
				        			(const char *)"Can't allocate memory in server",
				        			(const char *)"Util::fill_cmd_polling_buffer");
				}
				
//
// Set command value in Any object
// If the Any insertion is by pointer, with omniORB, it copy data into the Any
// and delete memory. Therefore, no need to delete memory if release is
// true. If release is false, uses the insertion by reference which only
// copy data.
//

				T *tmp_ptr = (data.get_data())[i].ptr;
				if ((data.get_data())[i].release == true)
					(*any_ptr) <<= tmp_ptr;
				else
					(*any_ptr) <<= (*tmp_ptr);
					      
			}

//
// Fill one slot of polling buffer
//

			try
			{
				dev->get_poll_monitor().get_monitor();
				vector<PollObj *>::iterator ite = dev->get_polled_obj_by_type_name(Tango::POLL_CMD,obj_name);
				when.tv_sec = (data.get_data())[i].t_val.tv_sec - DELTA_T;
				when.tv_usec = (data.get_data())[i].t_val.tv_usec;
				if (cmd_failed == false)
					(*ite)->insert_data(any_ptr,when,zero);
				else
					(*ite)->insert_except(save_except,when,zero);
				dev->get_poll_monitor().rel_monitor();
			}
			catch (Tango::DevFailed &)
			{
				if (cmd_failed == false)
				{
					delete any_ptr;
				}
				else
					delete save_except;
				dev->get_poll_monitor().rel_monitor();
			}

		}
	}
//@}

};

//+-------------------------------------------------------------------------
//
// function : 		return_empty_any
// 
// description : 	Return from a command when the command does not have
//			any output argument
//
// arguments : 		in : - cmd : The command name
//
//--------------------------------------------------------------------------

/**
 * Create and return an empty CORBA Any object.
 *
 * Create an empty CORBA Any object. Could be used by command which does
 * not return anything to the client. This method also prints a message on
 * screen (level 4) before it returns
 *
 * @param cmd The cmd name which use this empty Any. Only used to create the
 * thrown exception (in case of) and in the displayed message
 * @return The empty CORBA Any
 * @exception DevFailed If the Any object creation failed.
 * Click <a href="../../../tango_idl/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
inline CORBA::Any *return_empty_any(const char *cmd)
{

	CORBA::Any *out_any;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		TangoSys_MemStream o;
		
		o << cmd << "::execute";
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		//cout3 << "Bad allocation while in " << cmd << "::execute()" << endl;
#endif
		Tango::Except::throw_exception((const char *)"API_MemoryAllocation",
					     (const char *)"Can't allocate memory in server",
					     o.str());
	}
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	//cout4 << "Leaving " << cmd << "::execute()" << endl;	
#endif
	return(out_any);
	
}

inline DbDevice *DeviceImpl::get_db_device()
{
	if (Tango::Util::_UseDb == false)
	{
		TangoSys_OMemStream desc_mess;
		desc_mess << "Method not available for device ";
		desc_mess << device_name;
		desc_mess << " which is a non database device";
		
		Except::throw_exception((const char *)"API_NonDatabaseDevice",
					desc_mess.str(),
					(const char *)"DeviceImpl::get_db_device");
	}
	
	return db_dev;
}

void clear_att_dim(Tango::AttributeValue_3 &att_val);


//=============================================================================
//
//			The PollConfigureThread class
//
// description :	This class is just a helper class for the thread
//			used during DS startup phase to send orders to the
//			polling tread
//
//=============================================================================


class PollConfigureThread: public omni_thread
{
public:
	PollConfigureThread() {}
	
	void run(void *);	
};





//
// For thread creation interceptor (Python device servers
//

void create_PyPerThData(omni::omniInterceptors::createThread_T::info_T &);


class PyData: public omni_thread::value_t
{
public:
	PyData():rec_state(false),rec_status(false)
	{
		try
		{
			Util *tg = Util::instance(false);
			CreatePyLock *Creator = tg->get_py_lock_creator();
			PerTh_py_lock = Creator->create();
		}
		catch(Tango::DevFailed &) {PerTh_py_lock=NULL;}
	}
	
	~PyData()
	{
		if (PerTh_py_lock != NULL)
			delete PerTh_py_lock;
	}

	DevVarCharArray			PerTh_dvca;
	DevVarShortArray		PerTh_dvsha;
	DevVarLongArray			PerTh_dvla;
	DevVarFloatArray		PerTh_dvfa;
	DevVarDoubleArray		PerTh_dvda;
	DevVarUShortArray		PerTh_dvusa;
	DevVarULongArray		PerTh_dvula;
	DevVarStringArray 		PerTh_dvsa;
	DevVarLongStringArray		PerTh_dvlsa;
	DevVarDoubleStringArray		PerTh_dvdsa;
	
	string				PerTh_string;
	DevFailed			PerTh_df;
	vector<string>			PerTh_vec_str;
	vector<double>			PerTh_vec_db;
	DevErrorList			PerTh_del;
	
	bool				rec_state;
	bool				rec_status;
	
	PyLock				*PerTh_py_lock;
};


class AutoPyLock
{
public:
	AutoPyLock();
	~AutoPyLock();
};


} // End of Tango namespace
		     
#endif /* UTILS */

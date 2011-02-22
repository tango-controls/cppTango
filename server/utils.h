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
// Revision 3.32  2008/02/29 12:53:08  taurel
// - Increase timeout for DB get data cache command
// - Add a method to unvalidate the cache
//
// Revision 3.31  2008/01/25 15:44:51  taurel
// - Some changes in the Db cache
// - A lighter system to shutdown DS in case of dynamic attribute
//
// Revision 3.30  2007/12/12 10:17:19  taurel
// - Db calls during DS startup has a separate timeout and some retries
//
// Revision 3.29  2007/11/08 12:03:45  taurel
// - Start implementing user interceptors
// - Fix bug in poll thread pproperty management when removing polling object
// - Set a database timeout to 6 sec
//
// Revision 3.28  2007/10/16 08:24:14  taurel
// - Add management of the TC connection establishment timeout for DB access
// - Add DB server cache in DS used during DS startup sequence
// - Comment out the sleep time during DS startup sequence
//
// Revision 3.27  2007/07/03 08:28:22  jensmeyer
// Corrected handling of CORBA ANY in fill_attr_polling_buffer().
// THe beahaviour of the any changed with omniORB4.1!
//
// Revision 3.26  2007/05/17 07:59:07  taurel
// - The polling is not configured via a separate thread any more. The polling thread add_obj_polling method has been modified to support a parameter telling to the polling thread when it has to polled the object.
// Add device name in monitor print message
// Add device_destroyer method in DeviceClass class
//
// Revision 3.25  2007/05/15 07:46:59  taurel
// - The polling thread is not configured by a separate thread any more.
// The Add_obj_polling command now support a delta_t to start the first polling
//
// Revision 3.24  2007/04/20 14:41:35  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.23  2007/04/16 14:57:46  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.22  2007/03/29 07:11:06  taurel
// - Change some data types for 64 bits compatibility
//
// Revision 3.21  2007/03/12 15:17:00  taurel
// - Change the way to return Tango lib release
//
// Revision 3.20  2007/03/08 11:42:46  tiagocoutinho
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.19  2007/02/16 10:40:57  taurel
// - Implementing a new types of event on the Attribute configuration
//
// Revision 3.18  2007/02/08 16:25:17  taurel
// - Add some "unsigned" for local variable used in comparaison with vector
// size(). This makes c++ -Wall happy
//
// Revision 3.17  2007/01/19 15:45:54  taurel
// - Added the Utils::get_tango_lib_release() method
//
// Revision 3.16  2006/11/10 08:19:10  taurel
// - It is now possiblt to fill the polling buffer even for attribute which are
// not read only
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

#ifndef _TG_WINDOWS_
	#include <unistd.h>
#else
	#include <coutbuf.h>
	#include <w32win.h>
#endif /* _TG_WINDOWS_ */


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
class DbServerCache;

#ifdef _TG_WINDOWS_
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

class Interceptors
{
public:
	Interceptors() {}
	virtual ~Interceptors() {}
	
	virtual void	create_thread() {}
	virtual	void	delete_thread() {}
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
	 	  py_interp(NULL),py_ds(false),py_dbg(false),db_cache(NULL),inter(NULL),
		  svr_starting(true),svr_stopping(false),db_svr_version(0)
	{shared_data.cmd_pending=false;shared_data.trigger=false;
	cr_py_lock = new CreatePyLock();}
	
	vector<string>	cmd_line_name_list;
	
	PollThread		*poll_th;		// The polling thread object
	int				poll_th_id;		// The polling thread identifier
	PollThCmd		shared_data;	// The shared buffer
	TangoMonitor	poll_mon;		// The monitor
	bool			poll_on;		// Polling on flag
	SerialModel		ser_model;		// The serialization model
	TangoMonitor	only_one;		// Serialization monitor
	EventSupplier	*event_supplier;	// The event supplier object

	void			*py_interp;		// The Python interpreter
	bool			py_ds;			// The Python DS flag
	CreatePyLock	*cr_py_lock;	// The python lock creator pointer
	bool			py_dbg;			// Badly written Python dbg flag
	
	DbServerCache	*db_cache;		// The db cache
	Interceptors	*inter;			// The user interceptors
	
	bool			svr_starting;	// Server is starting flag
	bool			svr_stopping;	// Server is shutting down flag
	
	vector<string>	polled_dyn_attr_names;	// Dynamic att. names (used for polling clean-up)
	vector<string>	polled_att_list;		// Full polled att list
	vector<string>	all_dyn_attr;			// All dynamic attr name list
	string			dyn_att_dev_name;		// Device name (use for dyn att clean-up)
	int				db_svr_version;			// Db server version;
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
#ifdef _TG_WINDOWS_
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
	void set_trace_level(int level) {_tracelevel = level;}
	
/**
 * Get the process trace level.
 *
 * @return The process trace level
 */
	int get_trace_level() {return _tracelevel;}

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
 * Get the TANGO library version number.
 *
 * @return The Tango library release number coded in 3 digits
 * (for instance 550,551,552,600,....)
 */	
	long get_tango_lib_release();
	
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
 * @param The new serialization model. The serialization model must be one
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
	
/**
 * Set the interceptors
 * 
 * @param A pointer to a class inheriting from the Tango Interceptors class
 * This class has virtual methods allowing user to define its own interceptors
 */	
	void set_interceptors(Interceptors *in) {ext->inter = in;}
	
/**
 * Get the interceptors
 * 
 * @retrun The pointer to the interceptors object
 */	
	Interceptors *get_interceptors() {return ext->inter;}
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
	
	DbServerCache *get_db_cache() {return ext->db_cache;}
	void unvalidate_db_cache() {if (ext->db_cache!=NULL){delete ext->db_cache;ext->db_cache = NULL;}}
	
	bool is_svr_starting() {return ext->svr_starting;}
	void set_svr_starting(bool val) {ext->svr_starting = val;}
	
	bool is_svr_shutting_down() {return ext->svr_stopping;}
	void set_svr_shutting_down(bool val) {ext->svr_stopping = val;}
	
	vector<string> &get_polled_dyn_attr_names() {return ext->polled_dyn_attr_names;}
	vector<string> &get_full_polled_att_list() {return ext->polled_att_list;}
	string &get_dyn_att_dev_name() {return ext->dyn_att_dev_name;}
	vector<string> &get_all_dyn_attr_names() {return ext->all_dyn_attr;}
	
	int get_db_svr_version() {return ext->db_svr_version;}
	void set_db_svr_version();
	
	void clean_attr_polled_prop();
	void clean_dyn_attr_prop();
			
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
	static int		_tracelevel;
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

#ifdef _TG_WINDOWS_
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
#ifdef _TG_WINDOWS_
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
	TANGO_IMP static Util	*_instance;
	static bool				_constructed;
#ifdef _TG_WINDOWS_
	static bool				_win;
	int						argc;
	char					**argv;
	int						nCmd;
	CoutBuf					*pcb;
	W32Win					*ds_window;
	string					main_win_text;
	bool					go;
	TangoMonitor			mon;

	void build_argc_argv();
	
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

	CORBA::ORB_var 			orb;
	PortableServer::POA_var _poa;
	
	string					ds_instance_name;	// The instance name
	string					ds_exec_name;		// The server exec. name
	string					ds_name;			// The server name
	
	string					hostname;			// The host name
	string					pid_str;			// The process PID (as string)
	TangoSys_Pid			pid;				// The process PID

	string					version_str;		// Tango version
	string					server_version;		// Device server version

	string      			database_file_name;

#ifndef TANGO_HAS_LOG4TANGO	
	string			trace_output;
	TangoSys_Cout		cout_tmp;
	ofstream		*file_stream;
#endif //TANGO_HAS_LOG4TANGO

	Database				*db;			// The db proxy
	
	void effective_job(int,char *[]);
	void create_CORBA_objects();
	void misc_init();
	void init_host_name();
	void server_already_running();	
	void print_usage(char *);
	static void print_err_message(const char *,Tango::MessBoxType type = Tango::STOP);
	void print_err_message(const string &mess,Tango::MessBoxType type = Tango::STOP)
	{
		Util::print_err_message(mess.c_str(),type);
	}
	void check_args(int, char *[]);
	void display_help_message();
	DeviceImpl *find_device_name_core(string &);

	bool  							display_help;	// display help message flag
	const vector<DeviceClass *>		*cl_list_ptr;	// Ptr to server device class list	
	UtilExt							*ext;			// Class extension
	vector<DeviceClass *>			cl_list;		// Full class list ptr

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
		if (w_type == Tango::WRITE)
		{
			TangoSys_OMemStream o;
			o << "Attribute " << att_name;
			o << " of device " << dev->get_name() << " is WRITE only" << ends;

			Except::throw_exception((const char *)"API_DeviceNotPolled",o.str(),
				   		(const char *)"Util::fill_attr_polling_buffer");
		}
			
//
// Check that history is not larger than polling buffer
//

		unsigned long nb_elt = data.length();
		unsigned long nb_poll = dev->get_attr_poll_ring_depth(att_name);

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

					att.set_value((T *)(data.get_data())[i].ptr,
						      (data.get_data())[i].x,
						      (data.get_data())[i].y,
						      (data.get_data())[i].release);
					att.set_date((data.get_data())[i].t_val);			      
					att.set_quality(qu,false);
					      
//
// Init remaining fields in AttributeValueList
//

					if ((w_type == Tango::READ_WRITE) ||
				    	    (w_type == Tango::READ_WITH_WRITE))
						dev->get_device_attr()->add_write_value(att);
						
					CORBA::Any &a = (*back)[0].value;
					switch (att.get_data_type())
					{
					case Tango::DEV_SHORT :
						{
						Tango::DevVarShortArray *ptr = att.get_short_value();
						a <<= *ptr;
						delete ptr;
						break;
						}

					case Tango::DEV_LONG :
						{
						Tango::DevVarLongArray *ptr = att.get_long_value();
						a <<= *ptr;
						delete ptr;
						break;
						}

					case Tango::DEV_LONG64 :
						{
						Tango::DevVarLong64Array *ptr = att.get_long64_value();
						a <<= *ptr;
						delete ptr;
						break;
						}

					case Tango::DEV_DOUBLE :
						{
						Tango::DevVarDoubleArray *ptr = att.get_double_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
				
					case Tango::DEV_STRING :
						{
						Tango::DevVarStringArray *ptr = att.get_string_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
							
					case Tango::DEV_FLOAT :
						{
						Tango::DevVarFloatArray *ptr = att.get_float_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
				
					case Tango::DEV_BOOLEAN :
						{
						Tango::DevVarBooleanArray *ptr = att.get_boolean_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
				
					case Tango::DEV_USHORT :
						{
						Tango::DevVarUShortArray *ptr = att.get_ushort_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
				
					case Tango::DEV_UCHAR :
						{
						Tango::DevVarUCharArray *ptr = att.get_uchar_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
							
					case Tango::DEV_ULONG :
						{
						Tango::DevVarULongArray *ptr = att.get_ulong_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
							
					case Tango::DEV_ULONG64 :
						{
						Tango::DevVarULong64Array *ptr = att.get_ulong64_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
							
					case Tango::DEV_STATE :
						{
						Tango::DevVarStateArray *ptr = att.get_state_value();
						a <<= *ptr;
						delete ptr;
						break;
						}
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
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		//cout3 << "Bad allocation while in " << cmd << "::execute()" << endl;
#endif
		Tango::Except::throw_exception((const char *)"API_MemoryAllocation",
					     (const char *)"Can't allocate memory in server",
					     o.str());
	}
#if !defined(_TG_WINDOWS_) || (defined(_MSC_VER) && _MSC_VER >= 1300)
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


//------------------------------------------------------------------------
//
//			Python device server classes
//
//-----------------------------------------------------------------------


//
// For thread creation interceptor (Python device servers)
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

	DevVarCharArray				PerTh_dvca;
	DevVarShortArray			PerTh_dvsha;
	DevVarLongArray				PerTh_dvla;
	DevVarFloatArray			PerTh_dvfa;
	DevVarDoubleArray			PerTh_dvda;
	DevVarUShortArray			PerTh_dvusa;
	DevVarULongArray			PerTh_dvula;
	DevVarStringArray 			PerTh_dvsa;
	DevVarLongStringArray		PerTh_dvlsa;
	DevVarDoubleStringArray		PerTh_dvdsa;
	DevVarLong64Array			PerTh_dvl64a;
	DevVarULong64Array			PerTh_dvul64a;
	
	string						PerTh_string;
	DevFailed					PerTh_df;
	vector<string>				PerTh_vec_str;
	vector<double>				PerTh_vec_db;
	DevErrorList				PerTh_del;
	
	bool						rec_state;
	bool						rec_status;
	
	PyLock						*PerTh_py_lock;
};


class AutoPyLock
{
public:
	AutoPyLock();
	~AutoPyLock();
};

long _convert_tango_lib_release(const char *vers_str);

} // End of Tango namespace
		     
#endif /* UTILS */

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
//=============================================================================

#ifndef _UTILS_H
#define _UTILS_H

#include <tango.h>
#include <pollthread.h>
#include <pollext.h>
#include <subdev_diag.h>
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
class NotifdEventSupplier;
class ZmqEventSupplier;
class PyLock;
class CreatePyLock;
class DbServerCache;
class SubDevDiag;

struct PollingThreadInfo;
struct DevDbUpd;

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
 * @param vers The device server version
 */
	void set_server_version(const char *vers) {server_version = vers;}

/**
 * Set the DeviceClass list pointer
 *
 * @param list The DeviceClass ptr vector address
 */
 	void set_class_list(vector<DeviceClass *> *list) {cl_list_ptr = list;cl_list = *list;}

/**
 * Add a DeviceClass to the DeviceClass list pointer
 *
 * @param cl The DeviceClass ptr
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
 * @param ser The new serialization model. The serialization model must be one
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
 * Get a reference to the notifd TANGO EventSupplier object
 *
 * @return The notifd EventSupplier object
 */
	NotifdEventSupplier *get_notifd_event_supplier() {return ext->nd_event_supplier;}

/**
 * Get a reference to the ZMQ TANGO EventSupplier object
 *
 * @return The zmq EventSupplier object
 */
	ZmqEventSupplier *get_zmq_event_supplier() {return ext->zmq_event_supplier;}

/**
 * Set device server process event buffer high water mark (HWM)
 *
 * @param val The new event buffer high water mark in number of events
 */
	void set_ds_event_buffer_hwm(DevLong val) {if (ext->user_pub_hwm == -1)ext->user_pub_hwm = val;}
//@}

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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void trigger_cmd_polling(DeviceImpl *dev,const string &name);

/**
 * Trigger polling for polled attribute.
 *
 * This method send the order to the polling thread to poll one object
 * registered with an update period defined as "externally triggerred"
 *
 * @param dev The TANGO device
 * @param name The attribute name which must be polled
 * @exception DevFailed If the call failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	void trigger_attr_polling(DeviceImpl *dev,const string &name);

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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	template <typename T>
	void fill_attr_polling_buffer(DeviceImpl *dev,
				      string &att_name,
				      AttrHistoryStack<T>  &data);

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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	template <typename T>
	void fill_cmd_polling_buffer(DeviceImpl *dev,
				     string &cmd_name,
				     CmdHistoryStack<T>  &data);

/**
 * Set the polling threads pool size
 *
 * @param thread_nb The maximun number of threads in the polling threads pool
 */
	void set_polling_threads_pool_size(unsigned long thread_nb) {ext->poll_pool_size = thread_nb;}

/**
 * Get the polling threads pool size
 *
 * @return The maximun number of threads in the polling threads pool
 */
	unsigned long get_polling_threads_pool_size() {return ext->poll_pool_size;}
//@}

/**@Miscellaneous methods */
//@{
/**
 * Check if the device server process is in its starting phase
 *
 * @return A boolean set to true if the server is in its starting phase.
 */
	bool is_svr_starting() {return ext->svr_starting;}
/**
 * Check if the device server process is in its shutting down sequence
 *
 * @return A boolean set to true if the server is in its shutting down phase.
 */
	bool is_svr_shutting_down() {return ext->svr_stopping;}
/**
 * Check if the device is actually restarted by the device server
 * process admin device with its DevRestart command
 *
 * @return A boolean set to true if the device is restarting.
 */
    bool is_device_restarting(string &d_name);
//@}

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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * @exception DevFailed If the device pattern initialisation failed
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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

/**
 * Set the server event loop
 *
 * This method registers an event loop function in a Tango server.
 * This function will be called by the process main thread in an infinite loop
 * The process will not use the classical ORB blocking event loop.
 * It is the user responsability to code this function in a way that it implements
 * some kind of blocking in order not to load the computer CPU
 *
 * @param f_ptr The event loop function pointer. This function will not receive
 * any argument. It returns a boolean value. If this boolean is set to true,
 * the device server process exit.
 */
 	void server_set_event_loop(bool (*f_ptr)()) {ext->ev_loop_func = f_ptr;}
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

//
// The extension class
//

private:

    class UtilExt
    {
    public:
        UtilExt():poll_mon("utils_poll"),ser_model(BY_DEVICE),only_one("process"),
              py_interp(NULL),py_ds(false),py_dbg(false),db_cache(NULL),inter(NULL),
              svr_starting(true),svr_stopping(false),poll_pool_size(ULONG_MAX),
              conf_needs_db_upd(false),ev_loop_func(NULL),shutdown_server(false),_dummy_thread(false),
              endpoint_specified(false),user_pub_hwm(-1),wattr_nan_allowed(false)
        {shared_data.cmd_pending=false;shared_data.trigger=false;
        cr_py_lock = new CreatePyLock();}

        vector<string>				cmd_line_name_list;

        PollThread					*heartbeat_th;			// The heartbeat thread object
        int							heartbeat_th_id;		// The heartbeat thread identifier
        PollThCmd					shared_data;			// The shared buffer
        TangoMonitor				poll_mon;				// The monitor
        bool						poll_on;				// Polling on flag
        SerialModel					ser_model;				// The serialization model
        TangoMonitor				only_one;				// Serialization monitor
        NotifdEventSupplier			*nd_event_supplier;	    // The notifd event supplier object

        void						*py_interp;				// The Python interpreter
        bool						py_ds;					// The Python DS flag
        CreatePyLock				*cr_py_lock;			// The python lock creator pointer
        bool						py_dbg;					// Badly written Python dbg flag

        DbServerCache				*db_cache;				// The db cache
        Interceptors				*inter;					// The user interceptors

        bool						svr_starting;			// Server is starting flag
        bool						svr_stopping;			// Server is shutting down flag

        vector<string>				polled_dyn_attr_names;	// Dynamic att. names (used for polling clean-up)
        vector<string>				polled_att_list;		// Full polled att list
        vector<string>				all_dyn_attr;			// All dynamic attr name list
        string						dyn_att_dev_name;		// Device name (use for dyn att clean-up)

        unsigned long				poll_pool_size;			// Polling threads pool size
        vector<string>  			poll_pool_conf;			// Polling threads pool conf.
        map<string,int>				dev_poll_th_map;		// Link between device name and polling thread id
        vector<PollingThreadInfo *>	poll_ths;				// Polling threads
        bool						conf_needs_db_upd;		// Polling conf needs to be udated in db

        bool 						(*ev_loop_func)(void);	// Ptr to user event loop
        bool						shutdown_server;		// Flag to exit the manual event loop

        SubDevDiag					sub_dev_diag;			// Object to handle sub device diagnostics
        bool						_dummy_thread;			// The main DS thread is not the process main thread

        string						svr_port_num;			// Server port when using file as database

        ZmqEventSupplier            *zmq_event_supplier;    // The zmq event supplier object
        bool                        endpoint_specified;     // Endpoint specified on cmd line
        string                      specified_ip;           // IP address specified in the endpoint
        DevLong                     user_pub_hwm;           // User defined pub HWM

        vector<string>              restarting_devices;     // Restarting devices name
        bool                        wattr_nan_allowed;      // NaN allowed when writing attribute
    };

public:
	void set_interceptors(Interceptors *in) {ext->inter = in;}
	Interceptors *get_interceptors() {return ext->inter;}

	vector<string> &get_cmd_line_name_list() {return ext->cmd_line_name_list;}
	TangoMonitor &get_heartbeat_monitor() {return ext->poll_mon;}
	PollThCmd &get_heartbeat_shared_cmd() {return ext->shared_data;}
	bool poll_status() {return ext->poll_on;}
	void poll_status(bool status) {ext->poll_on = status;}

//
// Some methods are duplicated here (with different names). It is for compatibility reason
//

	void polling_configure();
	PollThread *get_polling_thread_object() {return ext->heartbeat_th;}
	PollThread *get_heartbeat_thread_object() {return ext->heartbeat_th;}
	void clr_poll_th_ptr() {ext->heartbeat_th = NULL;}
	void clr_heartbeat_th_ptr() {ext->heartbeat_th = NULL;}
	int get_polling_thread_id() {return ext->heartbeat_th_id;}
	int get_heartbeat_thread_id() {return ext->heartbeat_th_id;}
	void stop_heartbeat_thread();
	string &get_svr_port_num() {return ext->svr_port_num;}

	void create_notifd_event_supplier();
	void create_zmq_event_supplier();

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

	void set_svr_starting(bool val) {ext->svr_starting = val;}
	void set_svr_shutting_down(bool val) {ext->svr_stopping = val;}

	vector<string> &get_polled_dyn_attr_names() {return ext->polled_dyn_attr_names;}
	vector<string> &get_full_polled_att_list() {return ext->polled_att_list;}
	string &get_dyn_att_dev_name() {return ext->dyn_att_dev_name;}
	vector<string> &get_all_dyn_attr_names() {return ext->all_dyn_attr;}

	void clean_attr_polled_prop();
	void clean_dyn_attr_prop();

	int create_poll_thread(const char *dev_name,bool startup,int smallest_upd = -1);
	void stop_all_polling_threads();
	vector<PollingThreadInfo *> &get_polling_threads_info() {return ext->poll_ths;}
	PollingThreadInfo *get_polling_thread_info_by_id(int);
	int get_polling_thread_id_by_name(const char *);
	void check_pool_conf(DServer *,unsigned long);
	int check_dev_poll(vector<string> &,vector<string> &,DeviceImpl *);
	void split_string(string &,char,vector<string> &);
	void upd_polling_prop(vector<DevDbUpd> &,DServer *);
	int get_th_polled_devs(string &,vector<string> &);
	void get_th_polled_devs(long,vector<string> &);
	void build_first_pool_conf(vector<string> &);
	bool is_dev_already_in_pool_conf(string &,vector<string>&,int);
	vector<string> &get_poll_pool_conf() {return ext->poll_pool_conf;}
	int get_dev_entry_in_pool_conf(string &);
	void remove_dev_from_polling_map(string &dev_name);
	void remove_polling_thread_info_by_id(int);

	bool is_server_event_loop_set() {if (ext->ev_loop_func != NULL)return true;else return false;}
	void set_shutdown_server(bool val) {ext->shutdown_server = val;}

	void shutdown_server();

	SubDevDiag &get_sub_dev_diag() {return ext->sub_dev_diag;}

	bool get_endpoint_specified() {return ext->endpoint_specified;}
	void set_endpoint_specified(bool val) {ext->endpoint_specified = val;}

	string &get_specified_ip() {return ext->specified_ip;}
	void set_specified_ip(string &val) {ext->specified_ip = val;}

	DevLong get_user_pub_hwm() {return ext->user_pub_hwm;}

	void add_restarting_device(string &d_name) {ext->restarting_devices.push_back(d_name);}
	void delete_restarting_device(string &d_name);

    bool is_wattr_nan_allowed() {return ext->wattr_nan_allowed;}
	void set_wattr_nan_allowed(bool val) {ext->wattr_nan_allowed=val;}

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
	void install_cons_handler();

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
	string			        trace_output;
	TangoSys_Cout		    cout_tmp;
	ofstream		        *file_stream;
#endif //TANGO_HAS_LOG4TANGO

	Database				*db;			    // The db proxy

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
#ifdef HAS_UNIQUE_PTR
    unique_ptr<UtilExt>             ext;           // Class extension
#else
	Util::UtilExt					*ext;			// Class extension
#endif
	vector<DeviceClass *>			cl_list;		// Full class list ptr
};

// Add template methods definitions

#include <utils.tpp>

//***************************************************************************
//
//              Some inline methods
//
//***************************************************************************

//+----------------------------------------------------------------------------
//
// method : 		Util::is_device_restarting()
//
// description : 	Return a boolean if the device with name given as parameter
//                  is actually executing a RestartDevice command
//
// args: - d_name : - The device name
//
// Returns true if the devce is restarting. False otherwise
//
//-----------------------------------------------------------------------------

inline bool Util::is_device_restarting(string &d_name)
{
    bool ret = false;

    if (ext->restarting_devices.size() != 0)
    {
        vector<string>::iterator pos;
        pos = find(ext->restarting_devices.begin(),ext->restarting_devices.end(),d_name);
        if (pos != ext->restarting_devices.end())
            ret = true;
    }

    return ret;
}

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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
inline CORBA::Any *return_empty_any(const char *cmd)
{

	CORBA::Any *out_any = NULL;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		TangoSys_MemStream o;

		o << cmd << "::execute";
		Tango::Except::throw_exception((const char *)"API_MemoryAllocation",
					     (const char *)"Can't allocate memory in server",
					     o.str());
	}
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
void clear_att_dim(Tango::AttributeValue_4 &att_val);

//-----------------------------------------------------------------------
//
//			Polling threads pool related class/struct
//
//-----------------------------------------------------------------------

struct PollingThreadInfo
{
	int									thread_id;			// The polling thread identifier
	PollThread							*poll_th;			// The polling thread object
	PollThCmd							shared_data;		// The shared buffer
	TangoMonitor						poll_mon;			// The monitor
	vector<string>						polled_devices;		// Polled devices for this thread
	int									nb_polled_objects;	// Polled objects number in this thread
	int 								smallest_upd;		// Smallest thread update period
	vector<DevVarLongStringArray *> 	v_poll_cmd;			// Command(s) to send

	PollingThreadInfo():poll_mon("Polling_thread_mon"),nb_polled_objects(0),smallest_upd(0)
	{shared_data.cmd_pending = false;shared_data.trigger=false;}
};

struct DevDbUpd
{
	unsigned long 	class_ind;
	unsigned long	dev_ind;
	int				mod_prop;
};

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
		device_name = "No associated device name!";

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
	DevVarEncodedArray			PerTh_dvea;

	string						PerTh_string;
	DevFailed					PerTh_df;
	vector<string>				PerTh_vec_str;
	vector<double>				PerTh_vec_db;
	DevErrorList				PerTh_del;

	bool						rec_state;
	bool						rec_status;

	// name of the associated device to a thread
	// used to sub device referencing
	string 						device_name;

	PyLock						*PerTh_py_lock;
};


class AutoPyLock
{
public:
	AutoPyLock();
	~AutoPyLock();
};

long _convert_tango_lib_release();

} // End of Tango namespace

#endif /* UTILS */

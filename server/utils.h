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
// Revision 3.1  2003/04/02 12:26:04  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
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
// Revision 2.12  2003/03/13 15:17:50  nleclercq
// Minor modifications on logging implementation
//
// Revision 2.11  2003/03/11 17:55:58  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:46  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:05  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:50  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:08  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:21  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:56  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:26  taurel
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
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:05  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:12  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:15  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:12  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:15  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:46  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:17  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:43  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:17  taurel
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

#ifndef _UTILS_H
#define _UTILS_H

#include <tango.h>
#include <pollthread.h>
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

#ifdef WIN32
class CoutBuf;
class W32Win;
#endif


//=============================================================================
//
//			The TangoMonitor class
//
//
// description : 	This class is used to synchronise device access between
//			polling thread and CORBA request. It is used only for
//			the command_inout and read_attribute calls	
//
//=============================================================================

class TangoMonitor: public omni_mutex
{
public :
	TangoMonitor():_timeout(DEFAULT_TIMEOUT),cond(this),
			locking_thread(NULL),locked_ctr(0) {};
	~TangoMonitor() {};
		
	void get_monitor();	
	void rel_monitor();
	
	void timeout(long new_to) {_timeout = new_to;}
	long timeout() {return _timeout;}
	
	void wait() {cond.wait();}
	int wait(long);
	
	void signal() {cond.signal();}
		
private :
	long 		_timeout;
	omni_condition 	cond;
	omni_thread	*locking_thread;
	long		locked_ctr;
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
	UtilExt():ser_model(BY_DEVICE)
	{shared_data.cmd_pending = false;}
	
	vector<string>		cmd_line_name_list;
	
	PollThread		*poll_th;		// The polling thread object
	PollThCmd		shared_data;		// The shared buffer
	TangoMonitor		poll_mon;		// The monitor
	bool			poll_on;		// Polling on flag
	SerialModel		ser_model;		// The serialization model
	TangoMonitor		only_one;		// Serialization monitor
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
        static Util *init(HINSTANCE AppInst,int CmdShow);
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
	static Util *instance(bool exit = true);
//@}

/**@name Destructor
 * Only one destructor is defined for this class */
//@{
/**
 * The class destructor.
 */
	~Util()
	{
		unregister_server();
		delete ext;
	}
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
//@}

	vector<string> &get_cmd_line_name_list() {return ext->cmd_line_name_list;}
	TangoMonitor &get_poll_monitor() {return ext->poll_mon;}
	PollThCmd &get_poll_shared_cmd() {return ext->shared_data;}
	bool poll_status() {return ext->poll_on;}
	void poll_status(bool status) {ext->poll_on = status;}
	void polling_configure();
	PollThread *get_polling_thread_object() {return ext->poll_th;}
	

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
 * (e.g. "*", "/tango/tangotest/*", ...)
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
//@}

/**@name Class data members */
//@{
/**
 * The process trace level
 */	
	static long		_tracelevel;
/**
 * The database use flag (Use with extreme care. Implemented mainly for
 * the database device server)
 */
	static bool		_UseDb;
/**
 * A daemon process flag. If this flag is set to true, the server
 * process will not exit if it not able to connect to the database.
 * Instead, it will loop until the connection suceeds. The default
 * value is false.
 */
 	static bool		_daemon;
/**
 * The loop sleeping time in case of the _daemon flag set to true.
 * This sleeping time is the number of seconds the process will
 * sleep before it tries again to connect to the database. The default
 * value is 60 seconds.
 */
 	static long		_sleep_between_connect;
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

	static bool		_service;
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
	static Util		*_instance;
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

};


inline void TangoMonitor::get_monitor()
{
	int interupted;
	omni_thread *th = omni_thread::self();
	
	omni_mutex_lock synchronized(*this);
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	cout4 << "In get_monitor(), thread = " << th->id() << ", ctr = " << locked_ctr << endl;
#endif

	if (locked_ctr == 0)
	{
		locking_thread = th;
	}
	else if (th != locking_thread)
	{
		while(locked_ctr > 0)
		{
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)
			cout4 << "Thread " << th->id() << ": waiting !!" << endl;
#endif

			interupted = wait(_timeout);

			if (interupted == false)
			{
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)			
				cout4 << "TIME OUT for thread " << th->id() << endl;
#endif
				Except::throw_exception((const char *)"API_CommandTimedOut",
					        (const char *)"Not able to acquire serialization (dev, class or process) monitor",
					        (const char *)"TangoMonitor::get_monitor");
			}
		}
		locking_thread = th;
	}
	else
	{
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)			
		cout4 << "owner_thread !!" << endl;
#endif
	}
	
	locked_ctr++;
}

	
inline void TangoMonitor::rel_monitor()
{
	omni_thread *th = omni_thread::self();
	omni_mutex_lock synchronized(*this);

#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)
	cout4 << "In rel_monitor(), ctr = " << locked_ctr << ", thread = " << th->id() << endl;
#endif
	if ((locked_ctr == 0) || (th != locking_thread))
		return;
		
	locked_ctr--;
	if (locked_ctr == 0)
	{
#if !defined(WIN32) || (defined(_MSC_VER) && _MSC_VER >= 1300)
		cout4 << "Signalling !" << endl;
#endif
		locking_thread = NULL;
		cond.signal();
	}
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

} // End of Tango namespace
		     
#endif /* UTILS */

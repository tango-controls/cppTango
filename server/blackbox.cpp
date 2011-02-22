static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               BlackBox.cpp
//
// description :        C++ source code for the BlackBoxElt and BlackBox
//			classes. These classes are used to implement the 
//			tango device server black box. There is one
//			black box for each Tango device. This black box
//			keeps info. on all the activities on a device.
//			A client is able to retrieve these data via a Device
//			attribute
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
// $Log$
// Revision 3.27  2010/12/08 09:58:28  taurel
// - Replace gethostbyname() and gethostbyaddr() by getaddrinfo() and
// getnameinfo()
//
// Revision 3.26  2010/09/09 13:44:46  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.25  2009/12/18 14:52:37  taurel
// - Safety commit before christmas holydays
// - Many changes to make the DeviceProxy, Database and AttributeProxy
// classes thread safe (good help from the helgrind tool from valgrind)
//
// Revision 3.24  2009/09/01 07:41:31  taurel
// - Commit after test suite for Tango 7.1
//
// Revision 3.23  2009/03/27 12:21:46  taurel
// - Now the peer address coming from omniORB is coded in IP v6 format!
//
// Revision 3.22  2009/03/18 12:18:41  taurel
// - Fix warnings reported when compiled with the option -Wall
//
// Revision 3.21  2009/01/23 12:40:57  taurel
// - Remove some printout statements
//
// Revision 3.20  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.19  2009/01/08 14:58:03  taurel
// - The read_attribute_4 also transfer the client authentification
//
// Revision 3.18  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.17  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.16  2008/09/01 14:27:09  taurel
// - Fix some bugs in locking implementation
//
// Revision 3.15  2008/06/10 07:52:14  taurel
// - Add code for the DevEncoded attribute data type
//
// Revision 3.14  2008/05/20 12:44:09  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.13  2008/03/25 15:00:22  taurel
// - Small bug preventing Visual C++ to compile
// Revision 3.11.2.6  2008/05/20 06:17:44  taurel
// - Last commit before merge with trunk
// (start the implementation of the new DevEncoded data type)
//
// Revision 3.12  2008/03/14 11:53:52  taurel
// - Add a check if the calling thread has been created by omni_thread
// (For thread created by Python)
// Revision 3.11.2.5  2008/02/07 15:58:12  taurel
// - First implementation of the Controlled Access done
//
// Revision 3.11.2.4  2007/12/20 14:29:01  taurel
// - Some more work on locking
//
// Revision 3.11.2.3  2007/12/19 15:54:47  taurel
// - Still some work going on for the locking feature
//
// Revision 3.11.2.2  2007/11/22 12:33:09  taurel
// - First part of the device locking implementation
//
// Revision 3.11.2.1  2007/11/16 14:12:35  taurel
// - Added a new IDL interface (Device_4)
// - Added a new way to get attribute history from polling buffer (must faster)
//
// Revision 3.11  2007/04/20 14:40:24  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.10  2006/05/18 08:52:37  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.9  2005/01/13 08:27:32  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.8.2.2  2004/10/22 11:25:00  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.8.2.1  2004/08/19 07:44:03  taurel
// - Replace server low level database access call by Database class method call
// - Split device monitor in 3 : 1 to protect harware access, 1 to protect cache access and one mutex for device black box
//
// Revision 3.8  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.7  2003/08/21 07:23:46  taurel
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
// Revision 3.6  2003/07/03 07:40:51  taurel
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
// Revision 3.5.2.2  2004/03/09 16:36:36  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.5.2.1  2003/09/30 11:49:56  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.5  2003/06/23 09:04:17  taurel
// Fix no-end memory increase bug in the insert_attr() methods. Clear the
// vector before inserting new attribute names !!
//
// Revision 3.4  2003/05/28 14:55:07  taurel
// Add the include (conditionally) of the include files generated by autoconf
//
// Revision 3.3  2003/05/16 08:46:15  taurel
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
// Revision 3.2  2003/04/24 10:18:08  nleclercq
// Minor modifications to make some compilers happy
//
// Revision 3.1  2003/04/08 15:06:16  taurel
// - Names of read/written attributes are now stored in blackbox
// - Admin device RestartServer command is now done in its own thread with
//   changing CORBA POA manager state
//
// Revision 3.0  2003/03/25 16:41:58  taurel
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
// Revision 2.9  2003/01/09 12:03:15  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:06:21  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
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
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <blackbox.h>

#include <stdio.h>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#include <ws2tcpip.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* _TG_WINDOWS_ */

#include <omniORB4/omniInterceptors.h>
#include <omniORB4/internal/giopStrand.h>
#include <omniORB4/internal/giopStream.h>
#include <omniORB4/internal/GIOP_S.h>


namespace Tango
{

//
// The per thread data storage key (The client IP is stored in thread specific storage)
// defined in utils.cpp
//

extern omni_thread::key_t key;

//
// The function called by the interceptor
//

CORBA::Boolean get_client_addr(omni::omniInterceptors::serverReceiveRequest_T::info_T &info)
{
	omni_thread::self()->set_value(key,new client_addr(((omni::giopStrand &)info.giop_s).connection->peeraddress()));
	return true;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBoxElt::BlackBoxElt 
// 
// description : 	Constructor for the BlackBoxElt class.
//			This constructor simply set the internal value to their
//			default
//
//--------------------------------------------------------------------------

BlackBoxElt::BlackBoxElt()
{
	req_type = Req_Unknown;
	attr_type = Attr_Unknown;
	op_type = Op_Unknown;
	when.tv_sec = when.tv_usec = 0;
	host_ip_str[0] = '\0';
	client_ident = false;
	attr_names.reserve(DEFAULT_ATTR_NB);
}

BlackBoxElt::~BlackBoxElt()
{
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::BlackBox 
// 
// description : 	Two constructors for the BlackBox class. The first one
//			does not take any argument and construct a black box
//			with the default depth.
//			The second one create a black box with a depth defined 
//			by the argument.
//
// argument : in : 	- max_size : The black box depth
//
//--------------------------------------------------------------------------

BlackBox::BlackBox():box(DefaultBlackBoxDepth)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = DefaultBlackBoxDepth;	
}

BlackBox::BlackBox(long max_size):box(max_size)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = max_size;		
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_corba_attr
// 
// description : 	This method insert a new element in the black box when
//			this element is a attribute
//
// argument : in : 	- attr : The attribute type
//
//--------------------------------------------------------------------------


void BlackBox::insert_corba_attr(BlackBoxElt_AttrType attr)
{

//
// Take mutex
//

	sync.lock();
			
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Attribute;
	box[insert_elt].attr_type = attr;
	box[insert_elt].op_type = Op_Unknown;
	box[insert_elt].client_ident = false;

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
			
//
// manage insert and read indexes
//

	inc_indexes();

//
// Release mutex
//

	sync.unlock();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_cmd
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to the operation command_inout
//
// argument : in : 	- cmd : The command name
//					- vers : The IDL device version
//					- sour : The source parameter (DEV, CACHE...)
//
//--------------------------------------------------------------------------


void BlackBox::insert_cmd(const char *cmd,long vers,DevSource sour)
{
	sync.lock();
	
	insert_cmd_nl(cmd,vers,sour);
	
	sync.unlock();
}

void BlackBox::insert_cmd_nl(const char *cmd,long vers,DevSource sour)
{
		
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	if (vers == 1)
		box[insert_elt].op_type = Op_Command_inout;
	else if (vers <= 3)
		box[insert_elt].op_type = Op_Command_inout_2;
	else
		box[insert_elt].op_type = Op_Command_inout_4;
	box[insert_elt].cmd_name = cmd;
	box[insert_elt].source = sour;
	box[insert_elt].client_ident = false;
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_cmd_cl_ident
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to the operation command_inout
//
// argument : in : 	- cmd : The command name
//					- cl_id : The client identification data
//					- sour : The source parameter (DEV, CACHE...)
//
//--------------------------------------------------------------------------


void BlackBox::insert_cmd_cl_ident(const char *cmd,const ClntIdent &cl_id,long vers,DevSource sour)
{
	sync.lock();

//
// Add basic info in the box
//
		
	insert_cmd_nl(cmd,vers,sour);

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}
		
//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);
	
	sync.unlock();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::add_cl_ident
// 
// description : 	Add client identification data to the client address
//					instance
//
// argument : in : 	- cl_ident : The client identificator
//					- cl_addr : The client address instance
//
//--------------------------------------------------------------------------

void BlackBox::add_cl_ident(const ClntIdent &cl_ident,client_addr *cl_addr)
{
	cl_addr->client_ident = true;			
	Tango::LockerLanguage cl_lang = cl_ident._d();
	cl_addr->client_lang = cl_lang;
	if (cl_lang == Tango::CPP)
		cl_addr->client_pid = cl_ident.cpp_clnt();
	else
	{
		Tango::JavaClntIdent jci = cl_ident.java_clnt();
		cl_addr->java_main_class = jci.MainClass;
		cl_addr->java_ident[0] = jci.uuid[0];
		cl_addr->java_ident[1] = jci.uuid[1];
	}
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::update_client_host
// 
// description : 	Add client identification data to one of the BlackBox
//					element
//
// argument : in : - ip : The client address instance
//
//--------------------------------------------------------------------------

void BlackBox::update_client_host(client_addr *ip)
{
	long local_insert_elt = insert_elt;
	if (local_insert_elt == 0)
		local_insert_elt = max_elt - 1;
	else
		local_insert_elt--;

	box[local_insert_elt].client_ident = true;
	box[local_insert_elt].client_lang = ip->client_lang;
	box[local_insert_elt].client_pid = ip->client_pid;
	box[local_insert_elt].java_main_class = ip->java_main_class;
}


//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_op
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to an operation which is not
//			the command_inout operation
//
// argument : in : 	- cmd : The operation type
//
//--------------------------------------------------------------------------

void BlackBox::insert_op(BlackBoxElt_OpType op)
{
	sync.lock();
	
	insert_op_nl(op);
	
	sync.unlock();	
}

void BlackBox::insert_op(BlackBoxElt_OpType op,const ClntIdent &cl_id)
{
	sync.lock();
	
	insert_op_nl(op);

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}
	
//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);
	
	sync.unlock();	
}

void BlackBox::insert_op_nl(BlackBoxElt_OpType op)
{
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = op;
	box[insert_elt].client_ident = false;
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_attr
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to the CORBA operation 
//			read_attributes
//
// argument : in : 	- names : The attribute(s) name
//					- vers : The device IDl version
//					- sour : The device source parameter (CACHE, DEV,...)
//
//--------------------------------------------------------------------------


void BlackBox::insert_attr(const Tango::DevVarStringArray &names,long vers,DevSource sour)
{

//
// Take mutex
//

	sync.lock();
	
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	switch (vers)
	{
	case 1 :
		box[insert_elt].op_type = Op_Read_Attr;
		break;
		
	case 2 :
		box[insert_elt].op_type = Op_Read_Attr_2;
		break;
		
	case 3 :
		box[insert_elt].op_type = Op_Read_Attr_3;
		break;
		
	case 4 :
		box[insert_elt].op_type = Op_Read_Attr_4;
		break;
	}
	box[insert_elt].source = sour;
	box[insert_elt].client_ident = false;
	

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < names.length();i++)
	{
		string tmp_str(names[i]);
		box[insert_elt].attr_names.push_back(tmp_str);
	}
	
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
	
//
// Release mutex
//

	sync.unlock();
}

void BlackBox::insert_attr(const Tango::DevVarStringArray &names,const ClntIdent &cl_id,long vers,DevSource sour)
{

//
// Take mutex
//

	sync.lock();
	
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;

	box[insert_elt].op_type = Op_Read_Attr_4;

	box[insert_elt].source = sour;
	box[insert_elt].client_ident = false;
	

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < names.length();i++)
	{
		string tmp_str(names[i]);
		box[insert_elt].attr_names.push_back(tmp_str);
	}
	
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}

//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);
	
//
// Release mutex
//

	sync.unlock();
}

void BlackBox::insert_attr(const Tango::AttributeValueList &att_list, long vers)
{
	sync.lock();
	
	insert_attr_nl(att_list,vers);

	sync.unlock();
}

void BlackBox::insert_attr(const Tango::AttributeValueList_4 &att_list, const ClntIdent &cl_id,long vers)
{
	sync.lock();
	
	insert_attr_nl_4(att_list);
	
//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}
	
//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

	sync.unlock();
}

void BlackBox::insert_attr_nl(const Tango::AttributeValueList &att_list, long vers)
{	
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	if (vers == 1)
		box[insert_elt].op_type = Op_Write_Attr;
	else if (vers < 4)
		box[insert_elt].op_type = Op_Write_Attr_3;
	else
		box[insert_elt].op_type = Op_Write_Attr_4;

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < att_list.length();i++)
	{
		string tmp_str(att_list[i].name);
		box[insert_elt].attr_names.push_back(tmp_str);
	}
	box[insert_elt].client_ident = false;
	
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}

void BlackBox::insert_attr_nl_4(const Tango::AttributeValueList_4 &att_list)
{	
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = Op_Write_Attr_4;

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < att_list.length();i++)
	{
		string tmp_str(att_list[i].name);
		box[insert_elt].attr_names.push_back(tmp_str);
	}
	
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_wr_attr
// 
// description : 	This method insert a new element in the black box when
//			this element is a call to the CORBA operation 
//			write_read_attributes
//
// argument : in : 	- names : The attribute(s) name
//
//--------------------------------------------------------------------------

void BlackBox::insert_wr_attr(const Tango::AttributeValueList_4 &att_list, const ClntIdent &cl_id,long vers)
{
	sync.lock();
	
	insert_attr_wr_nl(att_list,vers);

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}
	
//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

	sync.unlock();
}

void BlackBox::insert_attr_wr_nl(const Tango::AttributeValueList_4 &att_list, long vers)
{	
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	if (vers >= 4)
		box[insert_elt].op_type = Op_Write_Read_Attributes_4;

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < att_list.length();i++)
	{
		string tmp_str(att_list[i].name);
		box[insert_elt].attr_names.push_back(tmp_str);
	}
	
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);
	
	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else	
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();
		
//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::inc_indexes
// 
// description : 	This private method increment the indexes used to acces
//			the box itself. This is necessary because the box must
//			be managed as a circular buffer
//
//--------------------------------------------------------------------------


void BlackBox::inc_indexes()
{
	insert_elt++;
	if (insert_elt == max_elt)
		insert_elt = 0;
		
	if (nb_elt != max_elt)
		nb_elt++;
}

//+-------------------------------------------------------------------------
//
// method : 		get_client_host
// 
// description : 	This private method retrieves the client host IP
//			address (the number). IT USES OMNIORB SPECIFIC
//			INTERCEPTOR
//
//--------------------------------------------------------------------------

void BlackBox::get_client_host()
{
	omni_thread *th_id = omni_thread::self();
	if (th_id == NULL)
		th_id = omni_thread::create_dummy();
	
	omni_thread::value_t *ip = th_id->get_value(key);
	if (ip == NULL)
		strcpy(box[insert_elt].host_ip_str,"polling");
	else
		strcpy(box[insert_elt].host_ip_str,
	       	       ((client_addr *)(ip))->client_ip);
}
	
//+-------------------------------------------------------------------------
//
// method : 		BlackBox::build_info_as_str
// 
// description : 	Translate all the info stored in a black box element
//			into a readable string.
//
// argument : in : 	- index : The black box element index
//
//--------------------------------------------------------------------------

void BlackBox::build_info_as_str(long index)
{
	char date_str[25];
//
// Convert time to a string
//

	date_ux_to_str(box[index].when,date_str);
	elt_str = date_str; 
	
//
// Add request type and command name in case of
//

	elt_str = elt_str + " : ";
	unsigned long nb_in_vect;
	if (box[index].req_type == Req_Operation)
	{
		elt_str = elt_str + "Operation ";
		unsigned long i;
		switch (box[index].op_type)
		{
		case Op_Command_inout :
			elt_str = elt_str + "command_inout (cmd = " + box[index].cmd_name + ") from ";
			add_source(index);
			break;
			
		case Op_Ping :
			elt_str = elt_str + "ping ";
			break;
			
		case Op_Info :
			elt_str = elt_str + "info ";
			break;

		case Op_BlackBox :
			elt_str = elt_str + "blackbox ";
			break;

		case Op_Command_list :
			elt_str = elt_str + "command_list_query ";
			break;
		
		case Op_Command :
			elt_str = elt_str + "command_query ";
			break;
										
		case Op_Get_Attr_Config :
			elt_str = elt_str + "get_attribute_config ";
			break;
			
		case Op_Set_Attr_Config :
			elt_str = elt_str + "set_attribute_config ";
			break;
			
		case Op_Read_Attr :
			elt_str = elt_str + "read_attributes (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;
			
		case Op_Write_Attr :
			elt_str = elt_str + "write_attributes (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;
			
		case Op_Write_Attr_3 :
			elt_str = elt_str + "write_attributes_3 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;
			
		case Op_Command_inout_2 :
			elt_str = elt_str + "command_inout_2 (cmd = " + box[index].cmd_name + ") from ";
			add_source(index);
			break;
			
		case Op_Command_list_2 :
			elt_str = elt_str + "command_list_query_2 ";
			break;
			
		case Op_Command_2 :
			elt_str = elt_str + "command_query_2 ";
			break;
			
		case Op_Get_Attr_Config_2 :
			elt_str = elt_str + "get_attribute_config_2 ";
			break;
			
		case Op_Read_Attr_2 :
			elt_str = elt_str + "read_attributes_2 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);		
			break;
			
		case Op_Read_Attr_3 :
			elt_str = elt_str + "read_attributes_3 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;

		case Op_Command_inout_history_2 :
			elt_str = elt_str + "command_inout_history_2 ";
			break;
			
		case Op_Read_Attr_history_2 :
			elt_str = elt_str + "read_attribute_history_2 ";
			break;
			
		case Op_Read_Attr_history_3 :
			elt_str = elt_str + "read_attribute_history_3 ";
			break;
			
		case Op_Info_3 :
			elt_str = elt_str + "info_3 ";
			break;
			
		case Op_Get_Attr_Config_3 :
			elt_str = elt_str + "get_attribute_config_3 ";
			break;
			
		case Op_Set_Attr_Config_3 :
			elt_str = elt_str + "set_attribute_config_3 ";
			break;
			
		case Op_Read_Attr_history_4 :
			elt_str = elt_str + "read_attribute_history_4 ";
			break;

		case Op_Command_inout_history_4 :
			elt_str = elt_str + "command_inout_history_4 ";
			break;

		case Op_Command_inout_4 :
			elt_str = elt_str + "command_inout_4 (cmd = " + box[index].cmd_name + ") from ";
			add_source(index);
			break;
			
		case Op_Read_Attr_4 :
			elt_str = elt_str + "read_attributes_4 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;
			
		case Op_Write_Attr_4 :
			elt_str = elt_str + "write_attributes_4 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;
			
		case Op_Set_Attr_Config_4 :
			elt_str = elt_str + "set_attribute_config_4 ";
			break;
			
		case Op_Write_Read_Attributes_4 :
			elt_str = elt_str + "write_read_attributes_4 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;
			
		case Op_Unknown :
			elt_str = elt_str + "unknown operation !!!!!";
			return;
		}
	}	
	else if (box[index].req_type == Req_Attribute)
	{
		elt_str = elt_str + "Attribute ";
		switch (box[index].attr_type)
		{
		case Attr_Name :
			elt_str = elt_str + "name ";
			break;
			
		case Attr_Description :
			elt_str = elt_str + "description ";
			break;

		case Attr_Status :
			elt_str = elt_str + "status ";
			break;
			
		case Attr_State :
			elt_str = elt_str + "state ";
			break;
			
		case Attr_AdmName :
			elt_str = elt_str + "adm_name ";
			break;
						
		case Attr_Unknown :
			elt_str = elt_str + "unknown attribute !!!!!";
			return;
		}
	}
	else
	{
		elt_str = elt_str + "Unknown CORBA request type !!!!!";
		return;
	}

//
// Add client host name.
// Extract only IP numbers from the omni address format (giop:tcp:xx.yy.zz.kk:port)
// Return in case of badly formed address
//

	bool ipv6=false;
	if ((box[index].host_ip_str[0] != '\0') && 
	    (box[index].host_ip_str[0] != 'p') && 
		(box[index].host_ip_str[5] != 'u'))
	{
		string omni_addr = box[index].host_ip_str;
		string::size_type pos;
		if ((pos = omni_addr.find(':')) == string::npos)
			return;
		pos++;
		if ((pos = omni_addr.find(':',pos)) == string::npos)
			return;
		pos++;
		string ip_str = omni_addr.substr(pos);
		if (ip_str[0] == '[')
			ipv6 = true;
			
		string full_ip_str;
		if (ipv6 == false)
		{
			if ((pos = ip_str.find(':')) == string::npos)
				return;
			full_ip_str = ip_str.substr(0,pos);
		}
		else
		{
			if ((pos = ip_str.find(':')) == string::npos)
				return;
			pos++;
			if ((pos = ip_str.find(':',pos)) == string::npos)
				return;
			pos++;
			if ((pos = ip_str.find(':',pos)) == string::npos)
				return;
			pos++;
			string tmp_ip = ip_str.substr(pos);
			if ((pos = tmp_ip.find(']')) == string::npos)
				return;
			full_ip_str = tmp_ip.substr(0,pos);
		}
	
		if ((pos = full_ip_str.find('.')) == string::npos)
			return;
		string ip1_str = full_ip_str.substr(0,pos);
		string::size_type old_pos;
		pos++;
		old_pos = pos;
		if ((pos = full_ip_str.find('.',pos)) == string::npos)
			return;
		string ip2_str = full_ip_str.substr(old_pos,pos - old_pos);
		pos++;
		old_pos = pos;
		if ((pos = full_ip_str.find('.',pos)) == string::npos)
			return;
		string ip3_str = full_ip_str.substr(old_pos,pos - old_pos);
		pos++;
		string ip4_str = full_ip_str.substr(pos);

//
// Finally, get host name
//

		struct sockaddr_in si;
		si.sin_family = AF_INET;
		si.sin_port = 0;
#ifdef _TG_WINDOWS_
		int slen = sizeof(si);
		WSAStringToAddress((char *)full_ip_str.c_str(),AF_INET,NULL,(SOCKADDR *)&si,&slen);
#else
		inet_pton(AF_INET,full_ip_str.c_str(),&si.sin_addr);
#endif

		char host[512];

		int res = getnameinfo((const sockaddr *)&si,sizeof(si),host,512,0,0,0);
		
		if (res == 0)
		{
			elt_str = elt_str + "requested from ";
			elt_str = elt_str + host;
		}
		else
		{
			elt_str = elt_str + "requested from ";
			elt_str = elt_str + full_ip_str;
		}

//
// Add client identification if available
//
		
		if (box[index].client_ident == true)
		{	
			if (box[index].client_lang == Tango::CPP)
			{	
				elt_str = elt_str + " (CPP/Python client with PID ";
				TangoSys_MemStream o;
				o << box[index].client_pid;
				elt_str = elt_str + o.str() + ")";
			}
			else
			{
				elt_str = elt_str + " (Java client with main class ";
				elt_str = elt_str + box[index].java_main_class + ")";
			}
		}
	}
	else if (box[index].host_ip_str[5] == 'u')
	{
		Tango::Util *tg = Tango::Util::instance();
		elt_str = elt_str + "requested from " + tg->get_host_name();
	}
	else if (box[index].host_ip_str[0] == 'p')
	{
		elt_str = elt_str + "requested from polling";
	}
	
	return;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::add_source
// 
// description : 	Read black box element as strings. The newest element
//			is return in the first position
//
// argument : in : 	- index : The number of element to read
//
//--------------------------------------------------------------------------

void BlackBox::add_source(long index)
{
	switch (box[index].source)
	{
	case DEV : 
		elt_str = elt_str + "device ";
		break;
		
	case CACHE :
		elt_str = elt_str + "cache ";
		break;
		
	case CACHE_DEV :
		elt_str = elt_str + "cache_device ";
		break;
		
	default :
		elt_str = elt_str + "unknown source (!) ";
		break;
	}	
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::read
// 
// description : 	Read black box element as strings. The newest element
//			is return in the first position
//
// argument : in : 	- index : The number of element to read
//
//--------------------------------------------------------------------------

Tango::DevVarStringArray *BlackBox::read(long wanted_elt)
{


//
// Take mutex
//

	sync.lock();
	
//
// Throw exeception if the wanted element is stupid and if there is no element
// stored in the black box
//

	if (wanted_elt <= 0)
	{
		sync.unlock();
			
		Except::throw_exception((const char *)"API_BlackBoxArgument",
				      (const char *)"Argument to read black box out of range",
				      (const char *)"BlackBox::read");
	}
	if (nb_elt == 0)
	{
		sync.unlock();

		Except::throw_exception((const char *)"API_BlackBoxEmpty",
				      (const char *)"Nothing stored yet in black-box",
				      (const char *)"BlackBox::read");
	}
			
//
// Limit wanted element to a reasonable value
//

	if (wanted_elt > max_elt)
		wanted_elt = max_elt;
		
	if (wanted_elt > nb_elt)
		wanted_elt = nb_elt;
		
//
// Read black box elements
//

	Tango::DevVarStringArray *ret;
	try
	{
	
		ret = new Tango::DevVarStringArray(wanted_elt);
                ret->length(wanted_elt);

		long read_index;
		if (insert_elt == 0)
			read_index = max_elt - 1;
		else
			read_index = insert_elt - 1;
		for (long i = 0;i < wanted_elt;i++)
		{
			build_info_as_str(read_index);
			(*ret)[i] = elt_str.c_str();
		
			read_index--;
			if (read_index < 0)
				read_index = max_elt - 1;
		}
	}
	catch (bad_alloc)
	{
		sync.unlock();
			
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"BlackBox::read");
	}
	
//
// Release mutex
//

	sync.unlock();
				
	return(ret);
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::date_ux_to_str
// 
// description : 	Convert a UNIX date (number of seconds since EPOCH)
//			to a string of the following format :
//			dd/mm/yyyy hh24:mi:ss:xx
//
// argument : in : 	- ux_date : The UNIX date in a timeval structure
//			- str_date : Pointer to char array where the date will
//				     be stored (must be allocated)
//
//--------------------------------------------------------------------------

void BlackBox::date_ux_to_str(timeval &ux_date,char *str_date)
{
	long i;
	char month[5];
	char *ux_str;
	char unix_date[30];

/* Convert UNIX date to a string in UNIX format */

#ifdef WIN32_VC8
	time_t vc8_time;
	vc8_time = (time_t)ux_date.tv_sec;
	ux_str = ctime(&vc8_time);
#else
	ux_str = ctime((time_t *)&(ux_date.tv_sec));
#endif
	strcpy(unix_date,ux_str);

/* Copy day */

	for (i = 0;i < 2;i++)
		str_date[i] = unix_date[i + 8];
	str_date[2] = '/';
	str_date[3] = '\0';
	if (str_date[0] == ' ')
		str_date[0] = '0';

/* Copy month */

	for (i = 0;i < 3;i++)
		month[i] = unix_date[i + 4];
	month[3] = '\0';

	switch(month[0])
	{
		case 'J' : if (month[1] == 'u')
			   {
				if (month[2] == 'n')
					strcat(str_date,"06/");
				else
					strcat(str_date,"07/");
			   }
			   else
				strcat(str_date,"01/");
			   break;

		case 'F' : strcat(str_date,"02/");
			   break;

		case 'M' : if (month[2] == 'r')
				strcat(str_date,"03/");
			   else
				strcat(str_date,"05/");
			   break;

		case 'A' : if (month[1] == 'p')
				strcat(str_date,"04/");
			   else
				strcat(str_date,"08/");
			   break;

		case 'S' : strcat(str_date,"09/");
			   break;

		case 'O' : strcat(str_date,"10/");
		           break;

		case 'N' : strcat(str_date,"11/");
			   break;

		case 'D' : strcat(str_date,"12/");
			   break;
	}

	str_date[6] = '\0';

/* Copy year */

	strcat(str_date,&(unix_date[20]));
	str_date[10] = '\0';
	
/* Copy date remaining */

	strcat(str_date," ");
	for (i = 0;i < 8;i++)
		str_date[i + 11] = unix_date[i + 11];
	str_date[19] = '\0';
	
/* Add milliseconds */

#ifdef _TG_WINDOWS_
	sprintf(&(str_date[19]),":%.2d",(int)(ux_date.tv_usec/10));
#else
	sprintf(&(str_date[19]),":%.2d",(int)(ux_date.tv_usec/10000));
#endif

}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::client_addr
// 
// description : 	Copy ctor of the client_addr class
//
//--------------------------------------------------------------------------

client_addr::client_addr(const client_addr &rhs)
{
	client_ident = rhs.client_ident;
	client_lang = rhs.client_lang;
	client_pid = rhs.client_pid;
	java_main_class = rhs.java_main_class;
	java_ident[0] = rhs.java_ident[0];
	java_ident[1] = rhs.java_ident[1];
	memcpy(client_ip,rhs.client_ip,IP_ADDR_BUFFER_SIZE);	
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::operator=()
// 
// description : 	Assignement operator of the client_addr class
//
//--------------------------------------------------------------------------

client_addr & client_addr::operator=(const client_addr &rhs)
{
	client_ident = rhs.client_ident;
	client_lang = rhs.client_lang;
	client_pid = rhs.client_pid;
	java_main_class = rhs.java_main_class;
	java_ident[0] = rhs.java_ident[0];
	java_ident[1] = rhs.java_ident[1];
	memcpy(client_ip,rhs.client_ip,IP_ADDR_BUFFER_SIZE);	
	return *this;
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::operator==()
// 
// description : 	Equality operator of the client_addr class
//
//--------------------------------------------------------------------------

bool client_addr::operator==(const client_addr &rhs)
{
	if (client_ident != rhs.client_ident)
		return false;
		
	if (client_lang != rhs.client_lang)
		return false;
	else
	{
		if (client_lang == Tango::CPP)
		{
			if (client_pid != rhs.client_pid)
				return false;
				
			char *tmp = client_ip;
			const char *rhs_tmp = rhs.client_ip;
			
			if (strlen(tmp) != strlen(rhs_tmp))
				return false;
				
			if (strcmp(tmp,rhs_tmp) != 0)
				return false;
		}
		else
		{
			if (java_ident[0] != rhs.java_ident[0])
				return false;
			if (java_ident[1] != rhs.java_ident[1])
				return false;
		}
	}
	
	return true;
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::operator!=()
// 
// description : 	Operator of the client_addr class
//
//--------------------------------------------------------------------------

bool client_addr::operator!=(const client_addr &rhs)
{
	if (client_ident != rhs.client_ident)
		return true;
		
	if (client_lang != rhs.client_lang)
		return true;
	else
	{
		if (client_lang == Tango::CPP)
		{
			if (client_pid != rhs.client_pid)
				return true;
				
			char *tmp = client_ip;
			const char *rhs_tmp = rhs.client_ip;

			if (strlen(tmp) != strlen(rhs_tmp))
				return true;
				
			if (strcmp(tmp,rhs_tmp) != 0)
				return true;
		}
		else
		{
			if (java_ident[0] != rhs.java_ident[0])
				return true;
			if (java_ident[1] != rhs.java_ident[1])
				return true;
		}
	}

	return false;
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::client_ip_2_client_name()
// 
// description : 	Convert client host IP address to client host name
//
//--------------------------------------------------------------------------

int client_addr::client_ip_2_client_name(string &cl_host_name) const
{
	int ret = 0;
	string client_ip_str(client_ip);
	
	string::size_type pos;
	if ((pos = client_ip_str.find(':')) == string::npos)
		ret = -1;
	else
	{
		pos++;
		if ((pos = client_ip_str.find(':',pos)) == string::npos)
			ret = -1;
		else
		{
			pos++;
			string ip_str = client_ip_str.substr(pos);
			if ((pos = ip_str.find(':')) == string::npos)
				ret = -1;
			else
			{
				string full_ip_str = ip_str.substr(0,pos);

				if ((pos = full_ip_str.find('.')) == string::npos)
					ret = -1;
				else
				{
					string ip1_str = full_ip_str.substr(0,pos);
	
					string::size_type old_pos;
					pos++;
					old_pos = pos;
					if ((pos = full_ip_str.find('.',pos)) == string::npos)
						ret = -1;
					else
					{
						string ip2_str = full_ip_str.substr(old_pos,pos - old_pos);
						pos++;
						old_pos = pos;
						if ((pos = full_ip_str.find('.',pos)) == string::npos)
							ret = -1 ;
						else
						{
							string ip3_str = full_ip_str.substr(old_pos,pos - old_pos);
							pos++;
							string ip4_str = full_ip_str.substr(pos);

							struct sockaddr_in si;
							si.sin_family = AF_INET;
							si.sin_port = 0;
#ifdef _TG_WINDOWS_
							int slen = sizeof(si);
							WSAStringToAddress((char *)full_ip_str.c_str(),AF_INET,NULL,(SOCKADDR *)&si,&slen);
#else
							inet_pton(AF_INET,full_ip_str.c_str(),&si.sin_addr);
#endif

							char host[512];

							int res = getnameinfo((const sockaddr *)&si,sizeof(si),host,512,0,0,0);
	
							if (res == 0)
							{
								cl_host_name = host;
								ret = 0;
							}
							else
								ret = -1;
						}
					}
				}
			}
		}
	}
	
	return ret;
}


//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//					client_addr class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,const client_addr &ca)
{
	if (ca.client_ident == false)
		o_str << "Client identification not available";
	else
	{
		if (ca.client_lang == Tango::CPP)
		{
			string cl_name;
			o_str << "CPP or Python client with PID " << ca.client_pid << " from host ";
			if (ca.client_ip_2_client_name(cl_name) == 0)
				o_str << cl_name;
			else
				o_str << ca.client_ip;
		}
		else
		{
			o_str << "JAVA client class " << ca.java_main_class << " from host ";
			string cl_name;
			if (ca.client_ip_2_client_name(cl_name) == 0)
				o_str << cl_name;
			else
				o_str << ca.client_ip;
		}
	}

	return o_str;
}


} // End of Tango namespace

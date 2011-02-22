//=============================================================================
//
// file :               Tango.h
//
// description :        Main include for Tango device server  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.4.2.2  2004/11/15 12:32:43  taurel
// - Some changes for omniORB 4.0.5 (Don't use USE_stub_in_nt_dll...)
//
// Revision 3.4.2.1  2004/10/05 13:55:03  maxchiandone
// First upload for filedatabase.
//
// Revision 3.4  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.3  2003/08/21 07:24:37  taurel
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
// Revision 3.2  2003/07/03 07:40:51  taurel
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
// Revision 3.1.2.5  2004/04/02 14:58:17  taurel
// Changes for release 4.1
// - Change the event.h inclusion place in tango.h
// - Fix bugs in event.cpp file and add a clean way to shutdown event system
// - Now support attribute min,max,min_alarm and max_alarm defined in scientific notation for long attribute
// - Added debian30 support in Make.rules
//
// Revision 3.1.2.4  2004/03/09 16:36:37  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.1.2.3  2004/01/20 08:32:37  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.1.2.2  2003/11/28 17:56:05  nleclercq
// Minor change so that USE_stub_in_nt_dll only defined in win32 client DLL (not the TANGO lib itself)
//
// Revision 3.1.2.1  2003/10/10 16:54:58  nleclercq
// Added Group support
//
// Revision 3.1  2003/05/28 14:45:33  taurel
// Add specific preprocessor symbol definition for tango dll
// Revision 3.0.2.1  2003/04/08 13:12:52  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:47:48  taurel
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
// Revision 2.12  2003/03/20 12:07:46  taurel
// Added support for asynchronous call
//
// Revision 2.11  2003/03/11 17:55:57  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:43  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.9  2003/01/09 12:03:16  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.8  2002/12/16 12:07:04  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:49  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
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
// Revision 1.6  2001/10/08 09:03:14  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:11  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:14  taurel
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
// Revision 1.3  2000/04/13 10:40:42  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:16  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:29  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _TANGO_H
#define _TANGO_H


//
// Check GCC release
//

#if __GNUC__ >= 3
	#if __GNUC_MINOR__ >= 2
		#define GCC_STD
	#endif
#endif

#if __GNUC__ == 2
	#define STRSTREAM
#endif

//
// Insert typeinfo file as the first include file (needed for Hp-UX aCC as the
// first include file)

#ifndef TANGO_CLIENT
	#include <typeinfo>
#endif

//
// Include the Tango config file
//

#include <tango_config.h>

//
// Include IDL generated files which includes CORBA include files
//

#include <idl/tango.h>

//
// Some Windows specific include (necessary when used with MFC)
//

#ifdef WIN32
	#if (_WIN32_WINNT >= 0x0400)
		#include <winsock2.h>
		#include <mswsock.h>
	#else
		#include <winsock.h>
	#endif
#endif

//
// Include stream header files
//

#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
	#include <iostream>
	#include <sstream>
	#include <fstream>
#else
	#include <iostream.h>
	#include <strstream.h>
	#include <fstream.h>
	#include <sstream>
#endif

//
// Include some stdc++ library headers
//

#include <string>
#include <vector>

//
// Include API files (device and database)
//

#include <apiexcept.h>
#include <devasyn.h>
#include <dbapi.h>
#include <devapi.h>
#include <group.h>
#include <filedatabase.h>

//
// Include Tango utility files
//

#include <tango_const.h>
#include <except.h>
#include <attrmanip.h>
#include <seqvec.h>

#if !defined(TANGO_CLIENT) && defined TANGO_HAS_LOG4TANGO
	#include <log4tango.h>	
#endif

#ifndef TANGO_CLIENT
	#include <utils.h>
#endif

//
// Include Tango files in order to simplfy device server developer include
// file list
//

#ifndef TANGO_CLIENT
	#include <device.h>
	#include <device_2.h>
	#include <device_3.h>
	#include <command.h>
	#include <dserver.h>
#endif

#include <event.h>

//
// Default std namespace
//

using namespace std;
	
#endif /* TANGO_H */

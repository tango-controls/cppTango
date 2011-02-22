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
// Revision 3.11  2007/04/16 14:57:42  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.10  2007/03/06 08:19:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.9  2006/05/18 07:40:11  taurel
// - Updated to gcc 4
// - Added a define for Windows VC8 (WIN32_VC8)
// - With the change done by SourceForge on their CVS servers, release 3.9 has
// this file has disappeared from CVS !!!
//
// Revision 3.9  2006/05/08 07:16:40  taurel
// - Small changes for GCC 4
//
// Revision 3.8  2005/05/20 15:16:15  taurel
// - Some changes to support gcc on Solaris
//
// Revision 3.7  2005/04/15 11:34:07  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.6  2005/01/21 19:58:30  taurel
// - Some changes in include files for gcc 3.4.2
//
// Revision 3.5  2005/01/13 08:30:00  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
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
	#if __GNUC__ == 3
		#if __GNUC_MINOR__ >= 2
			#define GCC_STD
		#endif
	#else
		#define GCC_STD
	#endif
#endif

#if __GNUC__ == 2
	#define STRSTREAM
#endif

#ifdef sun
	#ifdef GCC_STD
		#define GCC_SOLARIS
	#endif
#endif

//
// Check Win32 VC release
//

#if ((defined WIN32) || (defined WIN64))
	#if (_MSC_VER >= 1400)       // VC8+ 
		#define WIN32_VC8
	#endif   // VC8+
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
// Please note that on Windows, this file include Windows include file.
// The Windows Windef.h file defines a Preprocessor WIN32 on top of the
// _WIN32 one.
// This measn that on Windows 64 bits, we will have BOTH WIN32 and WIN64
// defined even if on the compiler command line we define only WIN64
//

#include <idl/tango.h>

//
// Some Windows specific include (necessary when used with MFC)
//

#ifdef _TG_WINDOWS_
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

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
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

//
// Include Tango files in order to simplfy device server developer include
// file list
//

#ifndef TANGO_CLIENT
	#include <tango_monitor.h>
	#include <device.h>
	#include <utils.h>
	#include <auto_tango_monitor.h>
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

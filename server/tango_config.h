//=============================================================================
//
// file :               Tango_config.h
//
// description :        Include file where all the system dependant types
//			are defined.  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
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
// Revision 3.17  2008/10/06 15:01:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.16  2008/10/03 06:53:09  taurel
// - Add some licensing info in each files
//
// Revision 3.15  2008/05/20 12:44:14  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.14  2008/01/08 14:38:19  taurel
// - strcasecmp() is not named like this on Windows !!
//
// Revision 3.13  2007/12/12 10:17:18  taurel
// - Db calls during DS startup has a separate timeout and some retries
//
// Revision 3.12  2007/11/23 07:16:48  taurel
// - Add Claudio's modif
// Revision 3.11.2.1  2008/02/07 15:58:14  taurel
// - First implementation of the Controlled Access done
//
// Revision 3.11  2007/04/20 14:41:34  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.10  2007/04/16 14:57:43  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.9  2007/03/28 11:13:25  taurel
// - Added a WIN32_VC8 define
//
// Revision 3.8  2006/05/18 08:51:56  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.7  2006/03/20 15:14:36  jensmeyer
// Added #pragma warning (disable : 4996) to suppress undesired depricated warnings!
//
// Revision 3.6  2005/05/25 06:05:40  taurel
// - Again some minors changes before 5.2
//
// Revision 3.5  2005/05/20 15:16:15  taurel
// - Some changes to support gcc on Solaris
//
// Revision 3.4  2005/05/10 13:15:12  taurel
// - Some changes for 64 bits detection on Win32
//
// Revision 3.3  2005/04/15 11:34:07  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.2  2004/07/07 08:40:12  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.1.2.2  2004/04/07 11:22:10  taurel
// - Add some import/export declaration for Windows DLL
// - Add test on minor code for the CORBA::IMP_LIMIT exception before
//   printing it
//
// Revision 3.1.2.1  2004/03/09 16:36:37  taurel
// - Added HP aCC port (thanks to Claudio from Elettra)
// - Some last small bugs fixes
//
// Revision 3.1  2003/05/28 14:46:39  taurel
// The Solaris definition are now done by the omniconfig.h file. Remove them from this file
//
// Revision 3.0  2003/03/25 16:47:52  taurel
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
// Revision 2.11  2003/03/11 17:55:57  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.10  2003/02/17 14:57:44  taurel
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
// Revision 2.6  2002/10/17 07:43:07  taurel
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
//=============================================================================

#ifndef _TANGO_CONFIG_H
#define _TANGO_CONFIG_H

//
// For Sun CC and omniORB
//

//#ifdef __sun
//	#define __sunos__
//	#define __sparc__
//	#define __OSVERSION__ 5
//#endif

//
// Add some define for Win32 and omniORB
// !!!!!!!!!! In some cases, VC++ wizard generates Stdafx.h file which include
// windows.h file as their first include. Stdafx.h file is the first
// file which should be in included by the include files list. Therefore, in
// this case, it is not possible to define anything before windows.h file is
// included (except by modying stdafx.h file by hand...). In order to include
// Windows socket release 2 software, _WIN32_WINNT must be defined and set
// to something >= 0x0400. Therefore, in this case, define it in the project
// setting in the preprocessor definitions....
//

#if ((defined WIN32) || (defined WIN64))
	#define		__WIN32__
	#define		__x86__
	#ifndef _WIN32_WINNT
		#define		_WIN32_WINNT 0x0400
	#endif
	#define		__NT__
	#define		__OSVERSION 4
#endif

//
// Define a common preprocessor macros for all Windows (32 or 64 bits)
//
// Please note that on Windows, this file include Windows include file.
// The Windows Windef.h file defines a Preprocessor WIN32 on top of the
// _WIN32 one.
// This measn that on Windows 64 bits, we will have BOTH WIN32 and WIN64
// defined even if on the compiler command line we define only WIN64
//

#if ((defined WIN32) || (defined WIN64))
	#define _TG_WINDOWS_
#endif

//
// For Windows DLL (import and export nightmare)
// 

#ifdef  _TG_WINDOWS_
	#if ((defined _USRDLL) || (defined TANGO_HAS_DLL))
		#if (defined _TANGO_LIB)
			#define TANGO_IMP_EXP	__declspec(dllexport)
			#define TANGO_IMP
		#else
			#define TANGO_IMP_EXP	__declspec(dllimport)
			#define TANGO_IMP	__declspec(dllimport)
		#endif
	#else
		#define TANGO_IMP_EXP
		#define TANGO_IMP
	#endif
#else	 
	#define TANGO_IMP_EXP
	#define TANGO_IMP
#endif /* _WINDOWS_ */

//
// Some helper define
//

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
	#define 	TangoSys_OMemStream	ostringstream
	#define		TangoSys_MemStream	stringstream
	#define		TangoSys_Pid		int
	#define		TangoSys_Cout		ostream
#else
	#define		TangoSys_OMemStream	ostrstream
	#define		TangoSys_MemStream	strstream
	#define		TangoSys_Pid		pid_t
	#define 	TangoSys_Cout		_IO_ostream_withassign
#endif



#ifdef _TG_WINDOWS_
	#pragma warning(disable : 4355)
	#pragma warning(disable : 4715)
    	#pragma warning(disable : 4786)
	#if (_MSC_VER >= 1400)       // VC8+ 
		#pragma warning(disable : 4996)    // disable all deprecation warnings
	#endif   // VC8+
#endif

//
// Define a common sleep call
//

#ifndef _TG_WINDOWS_
#define Tango_sleep(A) sleep(A);
#else
#define Tango_sleep(A) Sleep(A * 1000);
#endif

//
// Define a common strcasecmp function
//

#ifndef _TG_WINDOWS_
#define TG_strcasecmp ::strcasecmp
#define TG_strncasecmp ::strncasecmp
#else
#define	TG_strcasecmp ::stricmp
#define TG_strncasecmp ::strnicmp
#endif

//
// ACTIVATE (or DEACTIVATE) THE TANGO LOGGING MECHANISM
//

#define TANGO_HAS_LOG4TANGO

//
// USE DSERVER'S LOGGER TO LOG TANGO CORE MESSAGES (cout1..4)
//

#define TANGO_HAS_CORE_LOGGER

//
// Is it a 32 or 64 bits computer
//

#ifndef _TG_WINDOWS_
	#include <omniORB4/acconfig.h>

	#ifdef PACKAGE_BUGREPORT
		#undef PACKAGE_BUGREPORT
	#endif
	
	#ifdef PACKAGE_NAME
		#undef PACKAGE_NAME
	#endif
	
	#ifdef PACKAGE_STRING
		#undef PACKAGE_STRING
	#endif

	#ifdef PACKAGE_TARNAME
		#undef PACKAGE_TARNAME
	#endif

	#ifdef PACKAGE_VERSION
		#undef PACKAGE_VERSION
	#endif

	#if SIZEOF_LONG == 8
		#define TANGO_LONG64
	#else
		#define TANGO_LONG32
	#endif
#else
	#define TANGO_LONG32
#endif



#ifdef sun
#ifndef GCC_STD
extern "C"
{
int gethostname(char *,int);
}
#endif /* gcc_std */
#endif /* sun */

#endif /* _TANGO_CONFIG_H */

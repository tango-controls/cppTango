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

#ifndef _TANGO_CONFIG_H
#define _TANGO_CONFIG_H

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
// Windows:
// The Windows VC compilers family defined _WIN32 (always) and _WIN64
// (when compiled as 64 bits)
// The Windows windef.h include file defines the preprocessor WIN32 on top of the
// _WIN32 one defined by the compiler itself.
//
// This means that on Windows 64 bits, we will have BOTH
//		_WIN32
//		WIN32
//		_WIN64
//
// Within Tango, Windows include files are included by the "idl/tango.h" include file
// included by tango.h after this file
//

#ifdef _WIN32
	#define		__WIN32__
	#define		__x86__
	#ifndef _WIN32_WINNT
		#define		_WIN32_WINNT 0x0500
	#endif
	#define		__NT__
	#define		__OSVERSION 4
#endif

//
// Check Win32 VC release
//

#ifdef _WIN32
    #ifdef _MSC_VER
        #if ((_MSC_VER >= 1400)  && (_MSC_VER < 1500))     // VC8+
            #define WIN32_VC8
        #elif ((_MSC_VER >= 1500) && (_MSC_VER < 1600))
            #define WIN32_VC9
        #elif (_MSC_VER >= 1600)
            #define WIN32_VC10
        #endif   // VC8+/VC9/VC10
    #endif
#endif


//
// Define a common preprocessor macros for all Windows (32 or 64 bits)
//

#ifdef _WIN32
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
// Check GCC release
//

#ifndef _TG_WINDOWS_
    #if __GNUC__ < 3
        #error "Gcc too old to use Tango!"
    #endif
#endif

//
// Some C++11 feature
// Unique_ptr -> gcc 4.3
// Lambda function -> gcc 4.5
// nullptr -> gcc 4.6
// rvalues -> gcc 4.3
//

#ifndef _TG_WINDOWS_
    #if defined(__GNUC__)
        #if __GNUC__ == 4
            #if __GNUC_MINOR__ > 2
                #define HAS_UNIQUE_PTR
                #define HAS_RVALUE
            #endif
            #if __GNUC_MINOR__ > 4
                #define HAS_LAMBDA_FUNC
                #define HAS_ISNAN_IN_STD
            #endif
            #if __GNUC_MINOR__ > 5
                #define HAS_NULLPTR
            #endif
        #elif __GNUC__ > 4
                #define HAS_UNIQUE_PTR
                #define HAS_RVALUE
                #define HAS_LAMBDA_FUNC
                #define HAS_ISNAN_IN_STD
                #define HAS_NULLPTR
        #endif
    #endif
#else
    #ifdef WIN32_VC10
        #define HAS_UNIQUE_PTR
        #define HAS_LAMBDA_FUNC
        #define HAS_NULLPTR
        #define HAS_RVALUE
    #endif
#endif

//
// Some helper define
//

#define 	TangoSys_OMemStream	ostringstream
#define		TangoSys_MemStream	stringstream
#define		TangoSys_Pid		int
#define		TangoSys_Cout		ostream

//
// For Microsoft compilers
//

#ifdef _TG_WINDOWS_
	#pragma warning(disable : 4355)
	#pragma warning(disable : 4715)
    	#pragma warning(disable : 4786)
	#if (_MSC_VER >= 1400)       // VC8+
		#pragma warning(disable : 4996)    // disable all deprecation warnings
	#endif   // VC8+
#endif

//
// Define a common isnan call
//

#ifndef _TG_WINDOWS_
    #ifdef HAS_ISNAN_IN_STD
        #define Tango_isnan(A)  std::isnan(A)
    #else
        #define Tango_isnan(A) isnan(A)
    #endif
#else
    #define Tango_isnan(A) _isnan(A)
#endif

//
// Define a common NULL constant
//

#ifdef HAS_NULLPTR
    #define Tango_NullPtr   nullptr
#else
    #define Tango_NullPtr   NULL
#endif

//
// Define a common sleep call
//

#ifndef _TG_WINDOWS_
    #define Tango_sleep(A) sleep(A)
#else
    #define Tango_sleep(A) Sleep(A * 1000)
#endif

//
// Define a time_t to long casting
//

#ifndef _TG_WINDOWS_
    #define time_t_2_long(A) A
#else
    #define time_t_2_long(A) (long)A
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
// Define a macro for unused parameter warning
//

#ifdef _TG_WINDOWS_
#define TANGO_UNUSED(var) var
#else
	#if __GNUC__ == 3 && __GNUC_MINOR__ >= 4
        #define TANGO_UNUSED(var) var __attribute__ ((unused))
    #elif __GNUC__ > 3
        #define TANGO_UNUSED(var) var __attribute__ ((unused))
    #else
        #define TANGO_UNUSED(var) var
	#endif
#endif

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

#endif /* _TANGO_CONFIG_H */

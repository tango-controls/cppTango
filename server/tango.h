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

#ifndef _TANGO_H
#define _TANGO_H

//
// Insert typeinfo file as the first include file
//

#ifndef TANGO_CLIENT
//	#include <typeinfo>
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

#include <iostream>
#include <sstream>
#include <fstream>

#include <memory>


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
	#include <device_4.h>
	#include <command.h>
	#include <dserver.h>
	#include <attribute.tpp>
	#include <w_attribute.tpp>
#endif

#include <event.h>

//
// minor is also defined (Linux) in sysmacros.h. We want the compiler to
// use the SystemException::minor() method !!
//

#ifdef minor
# undef minor
#endif

//
// Default std namespace
//

using namespace std;



#endif /* TANGO_H */

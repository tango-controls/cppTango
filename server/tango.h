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
// Insert typeinfo file as the first include file (needed for Hp-UX aCC as the
// first include file)

#ifndef TANGO_CLIENT
	#include <typeinfo>
#endif

//
// Include CORBA files
//

#include <OB/CORBA.h>

//
// Include the Tango config file
//

#include <tango_config.h>

//
// Include stream header files
//

#ifdef WIN32
	#include <iostream>
	#include <sstream>
	#include <fstream>
#else
	#include <iostream.h>
	#include <strstream.h>
	#include <fstream.h>
#endif

//
// Include some stdc++ library headers
//

#include <string>
#include <vector>

//
// Include IDL generated files
//

#include <idl/tango.h>

//
// Include API files
//

#include <dbapi.h>

//
// Include Tango utility files
//

#include <tango_const.h>
#include <except.h>
#include <attrmanip.h>

#ifndef TANGO_CLIENT
	#include <utils.h>
#endif

//
// Include Tango files in order to simplfy device server developer include
// file list
//

#ifndef TANGO_CLIENT
	#include <device.h>
	#include <command.h>
	#include <dserver.h>
#endif

//
// Default std namespace
//

#ifdef __hpux
namespace std{}
#endif

using namespace std;
	
#endif /* TANGO_H */

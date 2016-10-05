/*----- PROTECTED REGION ID(FwdTest::main.cpp) ENABLED START -----*/
static const char *RcsId = "$Id$";
//=============================================================================
//
// file :        main.cpp
//
// description : C++ source for the FwdTest device server main.
//               The main rule is to initialise (and create) the Tango
//               system and to create the DServerClass singleton.
//               The main should be the same for every Tango device server.
//
// project :     
//
// This file is part of Tango device class.
// 
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
// 
// $Author$
//
// $Revision$
// $Date$
//
// $HeadURL$
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================
#include <tango/tango.h>

// Check if crash reporting is used.
#if defined(ENABLE_CRASH_REPORT)
#  include <crashreporting/crash_report.h>
#else
#  define DECLARE_CRASH_HANDLER
#  define INSTALL_CRASH_HANDLER
#endif

DECLARE_CRASH_HANDLER;

int main(int argc,char *argv[])
{
	INSTALL_CRASH_HANDLER
	try
	{
		// Initialise the device server
		//----------------------------------------
		Tango::Util *tg = Tango::Util::init(argc,argv);

		// Create the device server singleton 
		//	which will create everything
		//----------------------------------------
		tg->server_init(false);

		// Run the endless loop
		//----------------------------------------
		cout << "Ready to accept request" << endl;
		tg->server_run();
	}
	catch (bad_alloc &)
	{
		cout << "Can't allocate memory to store device object !!!" << endl;
		cout << "Exiting" << endl;
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		
		cout << "Received a CORBA_Exception" << endl;
		cout << "Exiting" << endl;
	}
	Tango::Util::instance()->server_cleanup();
	return(0);
}

/*----- PROTECTED REGION END -----*/	//	FwdTest::main.cpp

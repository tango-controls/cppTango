static const char *RcsId = "$Header$";
//+=============================================================================
//
// file :               main.cpp
//
// description :        C++ source for a TANGO device server main.
//			The main rule is to initialise (and create) the Tango
//			system and to create the DServerClass singleton.
//			The main should be the same for every Tango device server.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Version$
//
// $Log$
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-=============================================================================

#include <tango.h>
#include <dserverclass.h>
#include <dserversignal.h>


int main(int argc,char *argv[])
{

  Tango::Util *tg;
  try
    {
//
// Initialise the device server
//
      tg = Tango::Util::init(argc,argv);

//
// Create the device server singleton which will create everything
//
      tg->server_init();
//
// Run the endless loop
//

      cout << "Ready to accept request" << endl;
      tg->get_orb()->run();
    }
  catch (bad_alloc)
    {
      cout << "Can't allocate memory to store device object !!!" << endl;
      cout << "Exiting" << endl;
    }
  catch (Tango::DevFailed &e)
    {
      Tango::Except::print_exception(e);
    }
  catch (CORBA::Exception &e)
    {
      Tango::Except::print_exception(e);
		
      cout << "Received a CORBA_Exception" << endl;
      cout << "Exiting" << endl;
    }
	
//
// Destroy the ORB (and properly releases its resources)
//

    tg->server_cleanup();
  return(0);
}


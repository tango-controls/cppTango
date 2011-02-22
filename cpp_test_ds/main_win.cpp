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


int APIENTRY WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR  lpCmdLine,
		     int    nCmdShow)
{
  MSG msg;
  Tango::Util *tg;
  try
    {
//
// Initialise the device server
//
      tg = Tango::Util::init(hInstance,nCmdShow);

//
// Create the device server singleton which will create everything
//
      tg->server_init(true);
//
// Run the endless loop
//

      cout << "Ready to accept request" << endl;
      tg->server_run();
    }
  catch (bad_alloc)
    {
	MessageBox((HWND)NULL,"Memory error","Command line",MB_ICONSTOP);
	return (FALSE);
    }
  catch (Tango::DevFailed &e)
    {
	MessageBox((HWND)NULL,e.errors[0].desc.in(),"Command line",MB_ICONSTOP);
	return (FALSE);
    }
  catch (CORBA::Exception &)
    {
	MessageBox((HWND)NULL,"CORBA Exception","Command line",MB_ICONSTOP);
	return (FALSE);
    }
	
   while(GetMessage(&msg,NULL,0,0))
   {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
   }

   delete  tg;

   return msg.wParam;
}


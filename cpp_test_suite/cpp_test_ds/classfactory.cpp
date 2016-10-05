static const char *RcsId = "$Header$";
//+=============================================================================
//
// file :               ClassFactory.cpp
//
// description :        C++ source for the class_factory method of the DServer
//			device class. This method is responsible to create
//			all class singletin for a device server. It is called
//			at device server startup
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
#include <DevTestClass.h>

void Tango::DServer::class_factory()
{

//
// Create DevTestClass singleton and store it in DServer object
//

	add_class(DevTestClass::init("DevTest"));

}





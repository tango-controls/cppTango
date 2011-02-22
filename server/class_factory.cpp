static const char *RcsId = "$Header$";
//+===========================================================================
//
// file :		class_factory.cpp
//
// description :	C++ source file for a dummy DServer::class_factory()
//			method. For server, this method is redefined by the
//			DS programmer in its own file and the linker will not
//			use this file. For client where ther is no
//			class_factory() method, this one will be used by the
//			linker
//			This works only if class_factory() is in its own file
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:29  taurel
// Imported sources
//
//
// copyleft :		European Synchrotron radiation Facility
//			BP 220, Grenoble 38043
//			FRANCE
//
//-===========================================================================

#include <dserver.h>

namespace Tango
{

void DServer::class_factory()
{
	cerr << "Oups, no class defined in this server. Exiting ..." << endl;
	exit(-1);
}

} // End of Tango namespace

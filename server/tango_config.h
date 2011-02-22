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

#ifndef _TANGO_CONFIG_H
#define _TANGO_CONFIG_H

#ifdef WIN32
	#define 	TangoSys_OMemStream	ostringstream
	#define		TangoSys_MemStream	stringstream
	#define		TangoSys_Pid		int
	#define		TangoSys_Cout		ostream
#else
	#define		TangoSys_OMemStream	ostrstream
	#define		TangoSys_MemStream	strstream
	#define		TangoSys_Pid		pid_t
	#ifdef __hpux
		#define	TangoSys_Cout		ostream_withassign
	#else
		#define TangoSys_Cout		_IO_ostream_withassign
	#endif /* __hpux */
#endif

#ifdef WIN32
        #ifdef _DEBUG
                #pragma warning(disable : 4786)
        #endif
#endif

//
// Define a common sleep call
//

#ifndef WIN32
#define Tango_sleep(A) sleep(A);
#else
#define Tango_sleep(A) Sleep(A * 1000);
#endif


#ifdef sun
extern "C"
{
int gethostname(char *,int);
}
#endif /* sun */
#endif /* _TANGO_CONFIG_H */

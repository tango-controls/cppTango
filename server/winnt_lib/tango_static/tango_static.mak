# Microsoft Developer Studio Generated NMAKE File, Based on tango_static.dsp
!IF "$(CFG)" == ""
CFG=tango_static - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tango_static - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tango_static - Win32 Release" && "$(CFG)" != "tango_static - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tango_static.mak" CFG="tango_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tango_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tango_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tango_static - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\tango.lib"


CLEAN :
	-@erase "$(INTDIR)\attrdesc.obj"
	-@erase "$(INTDIR)\attribute.obj"
	-@erase "$(INTDIR)\attrmanip.obj"
	-@erase "$(INTDIR)\basiccommand.obj"
	-@erase "$(INTDIR)\blackbox.obj"
	-@erase "$(INTDIR)\class_factory.obj"
	-@erase "$(INTDIR)\classattribute.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\coutbuf.obj"
	-@erase "$(INTDIR)\dbapi_base.obj"
	-@erase "$(INTDIR)\dbapi_class.obj"
	-@erase "$(INTDIR)\dbapi_datum.obj"
	-@erase "$(INTDIR)\dbapi_device.obj"
	-@erase "$(INTDIR)\dbapi_server.obj"
	-@erase "$(INTDIR)\devapi_attr.obj"
	-@erase "$(INTDIR)\devapi_base.obj"
	-@erase "$(INTDIR)\devapi_data.obj"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\device_2.obj"
	-@erase "$(INTDIR)\dserver.obj"
	-@erase "$(INTDIR)\dserverclass.obj"
	-@erase "$(INTDIR)\dserverpoll.obj"
	-@erase "$(INTDIR)\dserversignal.obj"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\multiattribute.obj"
	-@erase "$(INTDIR)\ntservice.obj"
	-@erase "$(INTDIR)\pollcmds.obj"
	-@erase "$(INTDIR)\pollobj.obj"
	-@erase "$(INTDIR)\pollring.obj"
	-@erase "$(INTDIR)\pollthread.obj"
	-@erase "$(INTDIR)\seqvec.obj"
	-@erase "$(INTDIR)\tango.obj"
	-@erase "$(INTDIR)\tango_skel.obj"
	-@erase "$(INTDIR)\thsig.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\w32win.obj"
	-@erase "$(OUTDIR)\tango.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GR /GX /O2 /I "F:\Tango\cppserver\system\device" /I "F:\Tango\Make\include" /I "F:\Tango\ORB\ORBacus4.1.1\winnt\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tango_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/out:"$(OUTDIR)\tango.lib" 
LIB32_OBJS= \
	"$(INTDIR)\attrdesc.obj" \
	"$(INTDIR)\attribute.obj" \
	"$(INTDIR)\attrmanip.obj" \
	"$(INTDIR)\basiccommand.obj" \
	"$(INTDIR)\blackbox.obj" \
	"$(INTDIR)\class_factory.obj" \
	"$(INTDIR)\classattribute.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\coutbuf.obj" \
	"$(INTDIR)\dbapi_base.obj" \
	"$(INTDIR)\dbapi_class.obj" \
	"$(INTDIR)\dbapi_datum.obj" \
	"$(INTDIR)\dbapi_device.obj" \
	"$(INTDIR)\dbapi_server.obj" \
	"$(INTDIR)\devapi_attr.obj" \
	"$(INTDIR)\devapi_base.obj" \
	"$(INTDIR)\devapi_data.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\device_2.obj" \
	"$(INTDIR)\dserver.obj" \
	"$(INTDIR)\dserverclass.obj" \
	"$(INTDIR)\dserverpoll.obj" \
	"$(INTDIR)\dserversignal.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\multiattribute.obj" \
	"$(INTDIR)\ntservice.obj" \
	"$(INTDIR)\pollcmds.obj" \
	"$(INTDIR)\pollobj.obj" \
	"$(INTDIR)\pollring.obj" \
	"$(INTDIR)\pollthread.obj" \
	"$(INTDIR)\seqvec.obj" \
	"$(INTDIR)\tango.obj" \
	"$(INTDIR)\tango_skel.obj" \
	"$(INTDIR)\thsig.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\w32win.obj"

"$(OUTDIR)\tango.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\tango.lib"


CLEAN :
	-@erase "$(INTDIR)\attrdesc.obj"
	-@erase "$(INTDIR)\attribute.obj"
	-@erase "$(INTDIR)\attrmanip.obj"
	-@erase "$(INTDIR)\basiccommand.obj"
	-@erase "$(INTDIR)\blackbox.obj"
	-@erase "$(INTDIR)\class_factory.obj"
	-@erase "$(INTDIR)\classattribute.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\coutbuf.obj"
	-@erase "$(INTDIR)\dbapi_base.obj"
	-@erase "$(INTDIR)\dbapi_class.obj"
	-@erase "$(INTDIR)\dbapi_datum.obj"
	-@erase "$(INTDIR)\dbapi_device.obj"
	-@erase "$(INTDIR)\dbapi_server.obj"
	-@erase "$(INTDIR)\devapi_attr.obj"
	-@erase "$(INTDIR)\devapi_base.obj"
	-@erase "$(INTDIR)\devapi_data.obj"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\device_2.obj"
	-@erase "$(INTDIR)\dserver.obj"
	-@erase "$(INTDIR)\dserverclass.obj"
	-@erase "$(INTDIR)\dserverpoll.obj"
	-@erase "$(INTDIR)\dserversignal.obj"
	-@erase "$(INTDIR)\except.obj"
	-@erase "$(INTDIR)\multiattribute.obj"
	-@erase "$(INTDIR)\ntservice.obj"
	-@erase "$(INTDIR)\pollcmds.obj"
	-@erase "$(INTDIR)\pollobj.obj"
	-@erase "$(INTDIR)\pollring.obj"
	-@erase "$(INTDIR)\pollthread.obj"
	-@erase "$(INTDIR)\seqvec.obj"
	-@erase "$(INTDIR)\tango.obj"
	-@erase "$(INTDIR)\tango_skel.obj"
	-@erase "$(INTDIR)\thsig.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\w32win.obj"
	-@erase "$(OUTDIR)\tango.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /GR /GX /Zi /Od /I "F:\Tango\cppserver\system\device" /I "F:\Tango\Make\include" /I "F:\Tango\ORB\ORBacus4.1.1\winnt\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tango_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/out:"$(OUTDIR)\tango.lib" 
LIB32_OBJS= \
	"$(INTDIR)\attrdesc.obj" \
	"$(INTDIR)\attribute.obj" \
	"$(INTDIR)\attrmanip.obj" \
	"$(INTDIR)\basiccommand.obj" \
	"$(INTDIR)\blackbox.obj" \
	"$(INTDIR)\class_factory.obj" \
	"$(INTDIR)\classattribute.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\coutbuf.obj" \
	"$(INTDIR)\dbapi_base.obj" \
	"$(INTDIR)\dbapi_class.obj" \
	"$(INTDIR)\dbapi_datum.obj" \
	"$(INTDIR)\dbapi_device.obj" \
	"$(INTDIR)\dbapi_server.obj" \
	"$(INTDIR)\devapi_attr.obj" \
	"$(INTDIR)\devapi_base.obj" \
	"$(INTDIR)\devapi_data.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\device_2.obj" \
	"$(INTDIR)\dserver.obj" \
	"$(INTDIR)\dserverclass.obj" \
	"$(INTDIR)\dserverpoll.obj" \
	"$(INTDIR)\dserversignal.obj" \
	"$(INTDIR)\except.obj" \
	"$(INTDIR)\multiattribute.obj" \
	"$(INTDIR)\ntservice.obj" \
	"$(INTDIR)\pollcmds.obj" \
	"$(INTDIR)\pollobj.obj" \
	"$(INTDIR)\pollring.obj" \
	"$(INTDIR)\pollthread.obj" \
	"$(INTDIR)\seqvec.obj" \
	"$(INTDIR)\tango.obj" \
	"$(INTDIR)\tango_skel.obj" \
	"$(INTDIR)\thsig.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\w32win.obj"

"$(OUTDIR)\tango.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tango_static.dep")
!INCLUDE "tango_static.dep"
!ELSE 
!MESSAGE Warning: cannot find "tango_static.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tango_static - Win32 Release" || "$(CFG)" == "tango_static - Win32 Debug"
SOURCE=..\..\attrdesc.cpp

"$(INTDIR)\attrdesc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\attribute.cpp

"$(INTDIR)\attribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\attrmanip.cpp

"$(INTDIR)\attrmanip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\basiccommand.cpp

"$(INTDIR)\basiccommand.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\blackbox.cpp

"$(INTDIR)\blackbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\class_factory.cpp

"$(INTDIR)\class_factory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\classattribute.cpp

"$(INTDIR)\classattribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\command.cpp

"$(INTDIR)\command.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\coutbuf.cpp

"$(INTDIR)\coutbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\dbapi_base.cpp

"$(INTDIR)\dbapi_base.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\dbapi_class.cpp

"$(INTDIR)\dbapi_class.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\dbapi_datum.cpp

"$(INTDIR)\dbapi_datum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\dbapi_device.cpp

"$(INTDIR)\dbapi_device.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\dbapi_server.cpp

"$(INTDIR)\dbapi_server.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\devapi_attr.cpp

"$(INTDIR)\devapi_attr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\devapi_base.cpp

"$(INTDIR)\devapi_base.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\Make\api\devapi_data.cpp

"$(INTDIR)\devapi_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\device.cpp

"$(INTDIR)\device.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\device_2.cpp

"$(INTDIR)\device_2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\dserver.cpp

"$(INTDIR)\dserver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\dserverclass.cpp

"$(INTDIR)\dserverclass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\dserverpoll.cpp

"$(INTDIR)\dserverpoll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\dserversignal.cpp

"$(INTDIR)\dserversignal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\except.cpp

"$(INTDIR)\except.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\multiattribute.cpp

"$(INTDIR)\multiattribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ntservice.cpp

"$(INTDIR)\ntservice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pollcmds.cpp

"$(INTDIR)\pollcmds.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pollobj.cpp

"$(INTDIR)\pollobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pollring.cpp

"$(INTDIR)\pollring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pollthread.cpp

"$(INTDIR)\pollthread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\seqvec.cpp

"$(INTDIR)\seqvec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\tango.cpp

"$(INTDIR)\tango.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\tango_skel.cpp

"$(INTDIR)\tango_skel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thsig.cpp

"$(INTDIR)\thsig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\utils.cpp

"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\w32win.cpp

"$(INTDIR)\w32win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 


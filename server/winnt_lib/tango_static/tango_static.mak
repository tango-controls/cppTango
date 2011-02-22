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
	-@erase "$(INTDIR)\attribute.obj"
	-@erase "$(INTDIR)\BasicCommand.obj"
	-@erase "$(INTDIR)\BlackBox.obj"
	-@erase "$(INTDIR)\class_factory.obj"
	-@erase "$(INTDIR)\classattribute.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\Device.obj"
	-@erase "$(INTDIR)\DServer.obj"
	-@erase "$(INTDIR)\DServerClass.obj"
	-@erase "$(INTDIR)\DServerSignal.obj"
	-@erase "$(INTDIR)\multiattribute.obj"
	-@erase "$(INTDIR)\tango.obj"
	-@erase "$(INTDIR)\tango_skel.obj"
	-@erase "$(INTDIR)\Tango_utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tango.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GR /GX /O2 /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tango.lib" 
LIB32_OBJS= \
	"$(INTDIR)\attribute.obj" \
	"$(INTDIR)\BasicCommand.obj" \
	"$(INTDIR)\BlackBox.obj" \
	"$(INTDIR)\class_factory.obj" \
	"$(INTDIR)\classattribute.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\Device.obj" \
	"$(INTDIR)\DServer.obj" \
	"$(INTDIR)\DServerClass.obj" \
	"$(INTDIR)\DServerSignal.obj" \
	"$(INTDIR)\tango.obj" \
	"$(INTDIR)\tango_skel.obj" \
	"$(INTDIR)\Tango_utils.obj" \
	"$(INTDIR)\multiattribute.obj"

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
	-@erase "$(INTDIR)\attribute.obj"
	-@erase "$(INTDIR)\BasicCommand.obj"
	-@erase "$(INTDIR)\BlackBox.obj"
	-@erase "$(INTDIR)\class_factory.obj"
	-@erase "$(INTDIR)\classattribute.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\Device.obj"
	-@erase "$(INTDIR)\DServer.obj"
	-@erase "$(INTDIR)\DServerClass.obj"
	-@erase "$(INTDIR)\DServerSignal.obj"
	-@erase "$(INTDIR)\multiattribute.obj"
	-@erase "$(INTDIR)\tango.obj"
	-@erase "$(INTDIR)\tango_skel.obj"
	-@erase "$(INTDIR)\Tango_utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tango.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /GR /GX /Zd /Od /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tango.lib" 
LIB32_OBJS= \
	"$(INTDIR)\attribute.obj" \
	"$(INTDIR)\BasicCommand.obj" \
	"$(INTDIR)\BlackBox.obj" \
	"$(INTDIR)\class_factory.obj" \
	"$(INTDIR)\classattribute.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\Device.obj" \
	"$(INTDIR)\DServer.obj" \
	"$(INTDIR)\DServerClass.obj" \
	"$(INTDIR)\DServerSignal.obj" \
	"$(INTDIR)\tango.obj" \
	"$(INTDIR)\tango_skel.obj" \
	"$(INTDIR)\Tango_utils.obj" \
	"$(INTDIR)\multiattribute.obj"

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
SOURCE=..\..\attribute.cpp

!IF  "$(CFG)" == "tango_static - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O2 /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\attribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GR /GX /Zd /Od /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\attribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\BasicCommand.cpp

"$(INTDIR)\BasicCommand.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\BlackBox.cpp

!IF  "$(CFG)" == "tango_static - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O2 /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\BlackBox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GR /GX /Zd /Od /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\BlackBox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\class_factory.cpp

"$(INTDIR)\class_factory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\classattribute.cpp

"$(INTDIR)\classattribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\command.cpp

"$(INTDIR)\command.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Device.cpp

"$(INTDIR)\Device.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\DServer.cpp

"$(INTDIR)\DServer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\DServerClass.cpp

"$(INTDIR)\DServerClass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\DServerSignal.cpp

"$(INTDIR)\DServerSignal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\multiattribute.cpp

"$(INTDIR)\multiattribute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\tango.cpp

"$(INTDIR)\tango.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\tango_skel.cpp

"$(INTDIR)\tango_skel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tango_utils.cpp

!IF  "$(CFG)" == "tango_static - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O2 /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Tango_utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GR /GX /Zd /Od /I "F:\tango\JTC\winnt\include" /I "F:\tango\ORB\ORBacus\winnt\include" /I "F:\tango\cppserver\device" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\tango_static.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Tango_utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


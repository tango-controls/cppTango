# Microsoft Developer Studio Project File - Name="tango_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tango_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tango_static.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tango_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\.." /I "..\..\..\client" /I "$(OMNI_BASE)\win32\include" /I "$(LOG4TANGO_BASE)\win32\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINSTATIC" /D "_TANGO_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"Release\tango.lib"

!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Zi /Od /I "..\.." /I "..\..\..\client" /I "$(OMNI_BASE)\win32\include" /I "$(LOG4TANGO_BASE)\win32\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINSTATIC" /D "_TANGO_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"Debug\tango.lib"

!ENDIF 

# Begin Target

# Name "tango_static - Win32 Release"
# Name "tango_static - Win32 Debug"
# Begin Group "Header files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\..\..\client\apiexcept.h
# End Source File
# Begin Source File

SOURCE=..\..\basiccommand.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\cbthread.h
# End Source File
# Begin Source File

SOURCE=..\..\coutappender.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devasyn.h
# End Source File
# Begin Source File

SOURCE=..\..\device_2.h
# End Source File
# Begin Source File

SOURCE=..\..\dserverclass.h
# End Source File
# Begin Source File

SOURCE=..\..\dserversignal.h
# End Source File
# Begin Source File

SOURCE=..\..\logcmds.h
# End Source File
# Begin Source File

SOURCE=..\..\logging.h
# End Source File
# Begin Source File

SOURCE=..\..\logstream.h
# End Source File
# Begin Source File

SOURCE=..\..\ntservice.h
# End Source File
# Begin Source File

SOURCE=..\..\pollcmds.h
# End Source File
# Begin Source File

SOURCE=..\..\pollobj.h
# End Source File
# Begin Source File

SOURCE=..\..\pollring.h
# End Source File
# Begin Source File

SOURCE=..\..\pollthread.h
# End Source File
# Begin Source File

SOURCE=..\..\tangoappender.h
# End Source File
# Begin Source File

SOURCE=..\..\tangorollingfileappender.h
# End Source File
# Begin Source File

SOURCE=..\..\xmllayout.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=..\..\..\client\api_util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\asynreq.cpp
# End Source File
# Begin Source File

SOURCE=..\..\attrdesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\attribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\attrmanip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\basiccommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\blackbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\cbthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\class_factory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\classattribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\command.cpp
# End Source File
# Begin Source File

SOURCE=..\..\coutappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\coutbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi_class.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi_datum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi_device.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi_server.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devapi_attr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devapi_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devapi_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devapi_datahist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\device.cpp
# End Source File
# Begin Source File

SOURCE=..\..\device_2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\devicelog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\dserver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\dserverclass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\dserverlog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\dserverpoll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\dserversignal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\except.cpp
# End Source File
# Begin Source File

SOURCE=..\..\logcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\logging.cpp
# End Source File
# Begin Source File

SOURCE=..\..\logstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\multiattribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ntservice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\pollcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\pollobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\pollring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\pollthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\proxy_asyn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\proxy_asyn_cb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\seqvec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tangoappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\idl\tangoDynSK.cpp
# ADD CPP /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /D "__WIN32__"
# End Source File
# Begin Source File

SOURCE=..\..\tangorollingfileappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\idl\tangoSK.cpp
# ADD CPP /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /D "__WIN32__"
# End Source File
# Begin Source File

SOURCE=..\..\thsig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\w32win.cpp
# End Source File
# End Group
# End Target
# End Project

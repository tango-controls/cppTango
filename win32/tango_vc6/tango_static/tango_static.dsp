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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\..\server" /I "..\..\..\client" /I "..\..\resources" /I "$(OMNI_BASE)\win32\include" /I "$(LOG4TANGO_BASE)\win32\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINSTATIC" /D "_TANGO_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"lib\tango.5.5.lib"

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
# ADD CPP /nologo /MTd /W3 /GR /GX /Zi /Od /I "..\..\..\server" /I "..\..\..\client" /I "..\..\resources" /I "$(OMNI_BASE)\win32\include" /I "$(LOG4TANGO_BASE)\win32\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINSTATIC" /D "_TANGO_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"lib\tangod.5.5.lib"

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

SOURCE=..\..\..\server\attrdesc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\attribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\attrmanip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\auto_tango_monitor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\basiccommand.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\blackbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\cbthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\classattribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\command.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\coutappender.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\coutbuf.h
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

SOURCE=..\..\..\server\device.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\device_2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\device_3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\deviceclass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserverclass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserversignal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\eventcmds.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\eventsupplier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\except.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\filedatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\group.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\log4tango.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\logcmds.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\logging.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\logstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\multiattribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\ntservice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollcmds.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollobj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\seqvec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\idl\tango.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tango.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tango_config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tango_const.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tango_monitor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tangoappender.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tangorollingfileappender.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\w32win.h
# End Source File
# Begin Source File

SOURCE=..\..\..\server\w_attribute.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=..\..\..\client\api_util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\apiexcept.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\asynreq.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\attr_proxy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\attrdesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\attribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\attrmanip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\basiccommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\blackbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\cbthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\class_factory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\classattribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\command.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\coutappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\coutbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\dbapi_attribute.cpp
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

SOURCE=..\..\..\server\dev_event.cpp
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

SOURCE=..\..\..\server\device.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\device_2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\device_3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\deviceclass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\devicelog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserverclass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserverlog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserverpoll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\dserversignal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\eventcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\eventsupplier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\except.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\filedatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\group.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\logcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\logging.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\logstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\multiattribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\ntservice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\pollthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\proxy_asyn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\proxy_asyn_cb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\seqvec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tangoappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\idl\tangoDynSK.cpp
# ADD CPP /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION=4
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tangorollingfileappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\idl\tangoSK.cpp
# ADD CPP /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION=4
# End Source File
# Begin Source File

SOURCE=..\..\..\server\thsig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\w32win.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\w_attribute.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\resources\tango.ico
# End Source File
# Begin Source File

SOURCE=..\..\resources\tango.rc
# End Source File
# End Target
# End Project

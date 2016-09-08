# Microsoft Developer Studio Project File - Name="tango_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tango_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tango_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tango_dll.mak" CFG="tango_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tango_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tango_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tango_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TANGO_DLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /I "..\..\..\server" /I "..\..\..\client" /I "..\..\resources" /I "$(OMNI_BASE)\win32_dll\include" /I "$(LOG4TANGO_BASE)\win32_dll\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LOG4TANGO_HAS_DLL" /D "_TANGO_LIB" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 log4tango.lib omniorb405_rt.lib omnidynamic405_rt.lib omnithread30_rt.lib COS405_rt.lib ws2_32.lib mswsock.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:5.5 /dll /machine:I386 /def:"tango.def" /out:"lib/tango552.dll" /implib:"Release/tango.lib" /libpath:"$(OMNI_BASE)\win32_dll\lib" /libpath:"$(LOG4TANGO_BASE)\win32_dll\lib"
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Build def file...
PreLink_Cmds=C:\perl\bin\perl ..\..\makedef.pl tango.def tango552 release\*.obj
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tango_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TANGO_DLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".." /I "..\..\..\server" /I "..\..\..\client" /I "..\..\resources" /I "$(OMNI_BASE)\win32_dll\include" /I "$(LOG4TANGO_BASE)\win32_dll\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_TANGO_LIB" /D "LOG4TANGO_HAS_DLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 log4tangod.lib omniORB405_rtd.lib omniDynamic405_rtd.lib COS405_rtd.lib omnithread30_rtd.lib ws2_32.lib mswsock.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /version:5.5 /dll /debug /machine:I386 /def:"tango.def" /out:"lib/tango552d.dll" /implib:"Debug/tangod.lib" /pdbtype:sept /libpath:"$(OMNI_BASE)\win32_dll\lib" /libpath:"$(LOG4TANGO_BASE)\win32_dll\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Build def file...
PreLink_Cmds=C:\perl\bin\perl ..\..\makedef.pl tango.def tango552d debug\*.obj
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "tango_dll - Win32 Release"
# Name "tango_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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
# ADD CPP /D "_OMNIORB_STUB_DLL" /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION=4
# End Source File
# Begin Source File

SOURCE=..\..\..\server\tangorollingfileappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\server\idl\tangoSK.cpp
# ADD CPP /D "_OMNIORB_STUB_DLL" /D "__WIN32__" /D "__x86__" /D _WIN32_WINNT=0x0400 /D "__NT__" /D __OSVERSION=4
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
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

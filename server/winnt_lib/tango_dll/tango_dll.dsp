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
# ADD CPP /nologo /MD /W3 /GR /GX /I "..\.." /I "..\..\..\client" /I "$(OMNI_BASE)\win32_dll\include" /I "$(LOG4TANGO_BASE)\win32_dll\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LOG4TANGO_HAS_DLL" /D "_TANGO_LIB" /YX /FD /c
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
# ADD LINK32 log4tango.lib omniorb405_rt.lib omnidynamic405_rt.lib omnithread30_rt.lib COS405_rt.lib ws2_32.lib mswsock.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /def:"tango.def" /out:"Release/tango520.dll" /implib:"Release/tango.lib" /libpath:"$(OMNI_BASE)\win32_dll\lib" /libpath:"$(LOG4TANGO_BASE)\win32_dll\lib"
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Build def file...
PreLink_Cmds=C:\perl\bin\perl ..\..\makedef.pl tango.def tango520 release\*.obj
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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\.." /I "..\..\..\client" /I "$(OMNI_BASE)\win32_dll\include" /I "$(LOG4TANGO_BASE)\win32_dll\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_TANGO_LIB" /D "LOG4TANGO_HAS_DLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 log4tangod.lib omniORB405_rtd.lib omniDynamic405_rtd.lib COS405_rtd.lib omnithread30_rtd.lib ws2_32.lib mswsock.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /dll /debug /machine:I386 /def:"tango.def" /out:"Debug/tango520d.dll" /implib:"Debug/tangod.lib" /pdbtype:sept /libpath:"$(OMNI_BASE)\win32_dll\lib" /libpath:"$(LOG4TANGO_BASE)\win32_dll\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Build def file...
PreLink_Cmds=C:\perl\bin\perl ..\..\makedef.pl tango.def tango520d debug\*.obj
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

SOURCE=..\..\device_3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\deviceclass.cpp
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

SOURCE=..\..\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\eventcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\eventsupplier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\except.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\filedatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\client\group.cpp
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
# ADD CPP /D "_OMNIORB_STUB_DLL" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /D "__WIN32__"
# End Source File
# Begin Source File

SOURCE=..\..\tangorollingfileappender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\idl\tangoSK.cpp
# ADD CPP /D "_OMNIORB_STUB_DLL" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /D "__WIN32__"
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
# Begin Source File

SOURCE=..\..\w_attribute.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\attrdesc.h
# End Source File
# Begin Source File

SOURCE=..\..\attribute.h
# End Source File
# Begin Source File

SOURCE=..\..\attrmanip.h
# End Source File
# Begin Source File

SOURCE=..\..\basiccommand.h
# End Source File
# Begin Source File

SOURCE=..\..\blackbox.h
# End Source File
# Begin Source File

SOURCE=..\..\classattribute.h
# End Source File
# Begin Source File

SOURCE=..\..\command.h
# End Source File
# Begin Source File

SOURCE=..\..\coutappender.h
# End Source File
# Begin Source File

SOURCE=..\..\coutbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\devasyn.h
# End Source File
# Begin Source File

SOURCE=..\..\device.h
# End Source File
# Begin Source File

SOURCE=..\..\deviceclass.h
# End Source File
# Begin Source File

SOURCE=..\..\dserver.h
# End Source File
# Begin Source File

SOURCE=..\..\dserverclass.h
# End Source File
# Begin Source File

SOURCE=..\..\dserversignal.h
# End Source File
# Begin Source File

SOURCE=..\..\event.h
# End Source File
# Begin Source File

SOURCE=..\..\eventcmds.h
# End Source File
# Begin Source File

SOURCE=..\..\except.h
# End Source File
# Begin Source File

SOURCE=..\..\..\client\filedatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\log4tango.h
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

SOURCE=..\..\multiattribute.h
# End Source File
# Begin Source File

SOURCE=..\..\ntservice.h
# End Source File
# Begin Source File

SOURCE=..\..\seqvec.h
# End Source File
# Begin Source File

SOURCE=..\..\tango.h
# End Source File
# Begin Source File

SOURCE=..\..\tango_config.h
# End Source File
# Begin Source File

SOURCE=..\..\tango_const.h
# End Source File
# Begin Source File

SOURCE=..\..\tangoappender.h
# End Source File
# Begin Source File

SOURCE=..\..\tangorollingfileappender.h
# End Source File
# Begin Source File

SOURCE=..\..\utils.h
# End Source File
# Begin Source File

SOURCE=..\..\w32win.h
# End Source File
# Begin Source File

SOURCE=..\..\w_attribute.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

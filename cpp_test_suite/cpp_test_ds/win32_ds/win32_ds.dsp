# Microsoft Developer Studio Project File - Name="win32_ds" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=win32_ds - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "win32_ds.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "win32_ds.mak" CFG="win32_ds - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "win32_ds - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "win32_ds - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "win32_ds - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "C:\cppapi\server" /I "C:\cppapi\client" /I "$(OMNI_BASE)\win32\include" /I "$(LOG4TANGO_BASE)\win32\include" /I "F:\tango\tools\test_system\ref\device" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINSTATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 tango.lib log4tango.lib omniORB4.lib omniDynamic4.lib omnithread.lib COS4.lib comctl32.lib ws2_32.lib mswsock.lib mswsock.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Release/devTest.exe" /libpath:"C:\cppapi\server\winnt_lib\tango_static\release" /libpath:"$(OMNI_BASE)\win32\lib" /libpath:"$(LOG4TANGO_BASE)\win32\lib"

!ELSEIF  "$(CFG)" == "win32_ds - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "C:\cppapi_5_2\server" /I "C:\cppapi_5_2\client" /I "$(OMNI_BASE)\win32\include" /I "$(LOG4TANGO_BASE)\win32\include" /I "F:\tango\tools\test_system\ref\device" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINSTATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tangod.lib log4tangod.lib omniORB4d.lib omniDynamic4d.lib omnithreadd.lib COS4d.lib comctl32.lib ws2_32.lib mswsock.lib mswsock.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/devTest.exe" /pdbtype:sept /libpath:"C:\cppapi\server\winnt_lib\tango_static\debug" /libpath:"$(OMNI_BASE)\win32\lib" /libpath:"$(LOG4TANGO_BASE)\win32\lib"

!ENDIF 

# Begin Target

# Name "win32_ds - Win32 Release"
# Name "win32_ds - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\classfactory.cpp
# End Source File
# Begin Source File

SOURCE=..\DevTest.cpp
# End Source File
# Begin Source File

SOURCE=..\DevTestClass.cpp
# End Source File
# Begin Source File

SOURCE=..\FileDb.cpp
# End Source File
# Begin Source File

SOURCE=..\IOAddAttribute.cpp
# End Source File
# Begin Source File

SOURCE=..\IOArray1.cpp
# End Source File
# Begin Source File

SOURCE=..\IOArray2.cpp
# End Source File
# Begin Source File

SOURCE=..\IOMisc.cpp
# End Source File
# Begin Source File

SOURCE=..\IOSeqVec.cpp
# End Source File
# Begin Source File

SOURCE=..\IOSetAttr.cpp
# End Source File
# Begin Source File

SOURCE=..\IOStr1.cpp
# End Source File
# Begin Source File

SOURCE=..\IOStr2.cpp
# End Source File
# Begin Source File

SOURCE=..\IOStrArray.cpp
# End Source File
# Begin Source File

SOURCE=..\IOStruct.cpp
# End Source File
# Begin Source File

SOURCE=..\main.cpp
# End Source File
# Begin Source File

SOURCE=..\SigThrow.cpp
# End Source File
# Begin Source File

SOURCE=..\TypeCmds.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\DevTest.h
# End Source File
# Begin Source File

SOURCE=..\DevTestClass.h
# End Source File
# Begin Source File

SOURCE=..\IOAddAttribute.h
# End Source File
# Begin Source File

SOURCE=..\IOArray1.h
# End Source File
# Begin Source File

SOURCE=..\IOArray2.h
# End Source File
# Begin Source File

SOURCE=..\IOMisc.h
# End Source File
# Begin Source File

SOURCE=..\IOSeqVec.h
# End Source File
# Begin Source File

SOURCE=..\IOSetAttr.h
# End Source File
# Begin Source File

SOURCE=..\IOStr1.h
# End Source File
# Begin Source File

SOURCE=..\IOStr2.h
# End Source File
# Begin Source File

SOURCE=..\IOStrArray.h
# End Source File
# Begin Source File

SOURCE=..\IOStruct.h
# End Source File
# Begin Source File

SOURCE=..\SigThrow.h
# End Source File
# Begin Source File

SOURCE=..\TypeCmds.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

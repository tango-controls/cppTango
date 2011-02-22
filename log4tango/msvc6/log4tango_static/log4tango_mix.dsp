# Microsoft Developer Studio Project File - Name="log4tango_mix" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=log4tango_mix - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "log4tango_mix.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "log4tango_mix.mak" CFG="log4tango_mix - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "log4tango_mix - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "log4tango_mix - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "log4tango_mix - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\include" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../win32/msvc6/build/lib/msvc6/mix/release/log4tango.lib" /nodefaultlib

!ELSEIF  "$(CFG)" == "log4tango_mix - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\include" /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /FR /Fd"../../../../../win32/msvc6/build/lib/msvc6/mix/debug/log4tangod.pdb" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../win32/msvc6/build/lib/msvc6/mix/debug/log4tangod.lib" /nodefaultlib

!ENDIF 

# Begin Target

# Name "log4tango_mix - Win32 Release"
# Name "log4tango_mix - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hh;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\log4tango\Appender.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\AppenderAttachable.hh
# End Source File
# Begin Source File

SOURCE="..\..\include\log4tango\config-win32.h"
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\threading\DummyThreads.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\Export.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\FileAppender.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\Filter.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\Layout.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\LayoutAppender.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\Level.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\Logger.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\LoggerStream.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\LoggingEvent.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\LogSeparator.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\LogStream.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\LogStreambuf.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\threading\MSThreads.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\NDC.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\OstreamAppender.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\PatternLayout.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\Portability.hh
# End Source File
# Begin Source File

SOURCE=..\..\src\PortabilityImpl.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\threading\PThreads.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\RollingFileAppender.hh
# End Source File
# Begin Source File

SOURCE=..\..\src\StringUtil.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\threading\Threading.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\TimeStamp.hh
# End Source File
# Begin Source File

SOURCE=..\..\include\log4tango\XmlLayout.hh
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\Appender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\AppenderAttachable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DummyThreads.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\FileAppender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Layout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LayoutAppender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Level.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Logger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LoggerStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LoggingEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LogSeparator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LogStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LogStreambuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\MSThreads.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\NDC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\OstreamAppender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PatternLayout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PortabilityImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PThreads.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\RollingFileAppender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\StringUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TimeStamp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\XmlLayout.cpp
# End Source File
# End Group
# End Target
# End Project

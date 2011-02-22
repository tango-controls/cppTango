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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "F:\tango\cppserver\device" /I "F:\tango\tmp\tango2\include\winnt" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 F:\tango\cppserver\device\api_objs\arch\winnt\release\*.obj /out:"Release\tango.lib"

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
# ADD CPP /nologo /MTd /W3 /GR /GX /Zi /Od /I "F:\Tango\cppserver\device" /I "F:\Tango\tmp\tango2\include\winnt" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 F:\tango\cppserver\device\api_objs\arch\winnt\debug\*.obj /out:"Debug\tango.lib"

!ENDIF 

# Begin Target

# Name "tango_static - Win32 Release"
# Name "tango_static - Win32 Debug"
# Begin Source File

SOURCE=..\..\attrdesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\attribute.cpp

!IF  "$(CFG)" == "tango_static - Win32 Release"

!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

# ADD CPP /Zd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\attrmanip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\BasicCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\BlackBox.cpp

!IF  "$(CFG)" == "tango_static - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "tango_static - Win32 Debug"

# ADD CPP /MTd

!ENDIF 

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

SOURCE=..\..\coutbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\dancing.bmp
# End Source File
# Begin Source File

SOURCE=..\..\Device.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DServerClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DServerSignal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\except.cpp
# End Source File
# Begin Source File

SOURCE=..\..\multiattribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ntservice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tango.cpp
# End Source File
# Begin Source File

SOURCE=.\tango.ico
# End Source File
# Begin Source File

SOURCE=..\..\tango_skel.cpp
# End Source File
# Begin Source File

SOURCE=.\tango_static.rc
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
# End Target
# End Project

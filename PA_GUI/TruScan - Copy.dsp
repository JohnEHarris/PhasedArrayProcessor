# Microsoft Developer Studio Project File - Name="TruScan" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TruScan - Win32 Tscope2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TruScan.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TruScan.mak" CFG="TruScan - Win32 Tscope2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TruScan - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TruScan - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "TruScan - Win32 Tscope2" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TruScan - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 WS2_32.LIB DIBAPI32.LIB /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "TruScan - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FAs /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 WS2_32.LIB DIBAPI32.LIB /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "TruScan - Win32 Tscope2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TruScan___Win32_Tscope2"
# PROP BASE Intermediate_Dir "TruScan___Win32_Tscope2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TruScan___Win32_Tscope2"
# PROP Intermediate_Dir "TruScan___Win32_Tscope2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 WS2_32.LIB DIBAPI32.LIB /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 WS2_32.LIB DIBAPI32.LIB /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "TruScan - Win32 Release"
# Name "TruScan - Win32 Debug"
# Name "TruScan - Win32 Tscope2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Acal.cpp
# End Source File
# Begin Source File

SOURCE=.\AdiSetupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Alarms.cpp
# End Source File
# Begin Source File

SOURCE=.\AscanDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AscanReadSeq.cpp
# End Source File
# Begin Source File

SOURCE=.\BldIndx.cpp
# End Source File
# Begin Source File

SOURCE=.\caldata.cpp
# End Source File
# Begin Source File

SOURCE=.\calicopy.cpp
# End Source File
# Begin Source File

SOURCE=.\Catch.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangePsWd.cpp
# End Source File
# Begin Source File

SOURCE=.\ChnlOn.cpp
# End Source File
# Begin Source File

SOURCE=.\ChnlRangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChnlTraceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CompV.cpp
# End Source File
# Begin Source File

SOURCE=.\Cpyto.cpp
# End Source File
# Begin Source File

SOURCE=.\Disk.cpp
# End Source File
# Begin Source File

SOURCE=.\DmxAdj.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawTraces.cpp
# End Source File
# Begin Source File

SOURCE=.\Echo.cpp
# End Source File
# Begin Source File

SOURCE=.\EndAreaToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Enet.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Ftext.cpp
# End Source File
# Begin Source File

SOURCE=.\FText2.cpp
# End Source File
# Begin Source File

SOURCE=.\GainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Inspect.cpp
# End Source File
# Begin Source File

SOURCE=.\IpxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IpxIn.cpp
# End Source File
# Begin Source File

SOURCE=.\IpxStat.cpp
# End Source File
# Begin Source File

SOURCE=.\JointNum.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\MemFileA.cpp
# End Source File
# Begin Source File

SOURCE=.\Nc.cpp
# End Source File
# Begin Source File

SOURCE=.\OdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnSite.cpp
# End Source File
# Begin Source File

SOURCE=.\Oscpe.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pulser.cpp
# End Source File
# Begin Source File

SOURCE=.\QStringParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Rcvr.cpp
# End Source File
# Begin Source File

SOURCE=.\Replay.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ScopeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScopeTrace1CalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScopeTrace2CalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScpDmx2.cpp
# End Source File
# Begin Source File

SOURCE=.\seekButton.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetUp.cpp
# End Source File
# Begin Source File

SOURCE=.\Shear.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StrpChnl.cpp
# End Source File
# Begin Source File

SOURCE=.\SysGain.cpp
# End Source File
# Begin Source File

SOURCE=.\TCG.cpp
# End Source File
# Begin Source File

SOURCE=.\TcgSetupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TcpThreadRxList.cpp
# End Source File
# Begin Source File

SOURCE=.\Tholds.cpp
# End Source File
# Begin Source File

SOURCE=.\tof.cpp
# End Source File
# Begin Source File

SOURCE=.\TRUSCAN.CPP
# End Source File
# Begin Source File

SOURCE=.\TRUSCAN.RC
# End Source File
# Begin Source File

SOURCE=.\TSCANDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\viewcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\WallDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WCalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WcJob.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Acal.h
# End Source File
# Begin Source File

SOURCE=.\AdiSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\Alarms.h
# End Source File
# Begin Source File

SOURCE=.\AscanDlg.h
# End Source File
# Begin Source File

SOURCE=.\AscanReadSeq.h
# End Source File
# Begin Source File

SOURCE=.\BldIndx.h
# End Source File
# Begin Source File

SOURCE=.\caldata.h
# End Source File
# Begin Source File

SOURCE=.\calicopy.h
# End Source File
# Begin Source File

SOURCE=.\Catch.h
# End Source File
# Begin Source File

SOURCE=.\ChangePsWd.h
# End Source File
# Begin Source File

SOURCE=.\ChnlOn.h
# End Source File
# Begin Source File

SOURCE=.\ChnlRangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChnlTraceDlg.h
# End Source File
# Begin Source File

SOURCE=.\CompV.h
# End Source File
# Begin Source File

SOURCE=.\Cpyto.h
# End Source File
# Begin Source File

SOURCE=.\DIBAPI.H
# End Source File
# Begin Source File

SOURCE=.\DIBDLL.H
# End Source File
# Begin Source File

SOURCE=.\DIBUTIL.H
# End Source File
# Begin Source File

SOURCE=.\Disk.h
# End Source File
# Begin Source File

SOURCE=.\DmxAdj.h
# End Source File
# Begin Source File

SOURCE=.\DrawTraces.h
# End Source File
# Begin Source File

SOURCE=.\Echo.h
# End Source File
# Begin Source File

SOURCE=.\EndAreaToolDlg.h
# End Source File
# Begin Source File

SOURCE=.\Enet.h
# End Source File
# Begin Source File

SOURCE=.\ENGLISH.H
# End Source File
# Begin Source File

SOURCE=.\ErrMsg.h
# End Source File
# Begin Source File

SOURCE=.\EXTERN.H
# End Source File
# Begin Source File

SOURCE=.\FolderDlg.h
# End Source File
# Begin Source File

SOURCE=.\Ftext.h
# End Source File
# Begin Source File

SOURCE=.\Ftext2.h
# End Source File
# Begin Source File

SOURCE=.\GainDlg.h
# End Source File
# Begin Source File

SOURCE=.\GateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Inspect.h
# End Source File
# Begin Source File

SOURCE=.\IpxDlg.h
# End Source File
# Begin Source File

SOURCE=.\IpxIn.h
# End Source File
# Begin Source File

SOURCE=.\IpxStat.h
# End Source File
# Begin Source File

SOURCE=.\JointNum.h
# End Source File
# Begin Source File

SOURCE=.\LimitSingleInstance.h
# End Source File
# Begin Source File

SOURCE=.\MemFileA.h
# End Source File
# Begin Source File

SOURCE=.\Nc.h
# End Source File
# Begin Source File

SOURCE=.\OdDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnSite.h
# End Source File
# Begin Source File

SOURCE=.\Oscpe.h
# End Source File
# Begin Source File

SOURCE=.\PasswordDlg.h
# End Source File
# Begin Source File

SOURCE=.\Pulser.h
# End Source File
# Begin Source File

SOURCE=.\QStringParser.h
# End Source File
# Begin Source File

SOURCE=.\Rcvr.h
# End Source File
# Begin Source File

SOURCE=.\Replay.h
# End Source File
# Begin Source File

SOURCE=.\ResizingDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScopeDlg.h
# End Source File
# Begin Source File

SOURCE=.\scopetrace1caldlg.h
# End Source File
# Begin Source File

SOURCE=.\scopetrace2caldlg.h
# End Source File
# Begin Source File

SOURCE=.\ScpDmx2.h
# End Source File
# Begin Source File

SOURCE=.\SeekButton.h
# End Source File
# Begin Source File

SOURCE=.\SequenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetUp.h
# End Source File
# Begin Source File

SOURCE=.\Shear.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StrpChnl.h
# End Source File
# Begin Source File

SOURCE=.\SysGain.h
# End Source File
# Begin Source File

SOURCE=.\TCG.h
# End Source File
# Begin Source File

SOURCE=.\TcgSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\TcpThreadRxList.h
# End Source File
# Begin Source File

SOURCE=.\Tholds.h
# End Source File
# Begin Source File

SOURCE=.\tof.h
# End Source File
# Begin Source File

SOURCE=.\TRUSCAN.H
# End Source File
# Begin Source File

SOURCE=.\TSCANDLG.H
# End Source File
# Begin Source File

SOURCE=..\Include\Udp_msg.h
# End Source File
# Begin Source File

SOURCE=.\viewcfg.h
# End Source File
# Begin Source File

SOURCE=.\WallDlg.h
# End Source File
# Begin Source File

SOURCE=.\WCalDlg.h
# End Source File
# Begin Source File

SOURCE=.\WcJob.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpScope1NormalOff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpScope1NormalOn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpScope2ActivGateOff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpScope2ActivGateOn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpScope2AllGateOff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpScope2AllGateOn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CURSOR1.CUR
# End Source File
# Begin Source File

SOURCE=.\res\iconScope1RFoff.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope1RFon.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate1Off.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate1On.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate2Off.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate2On.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate3Off.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate3On.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate4Off.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconScope2Gate4On.ico
# End Source File
# Begin Source File

SOURCE=.\res\sh12.ico
# End Source File
# Begin Source File

SOURCE=.\res\sh1x.ico
# End Source File
# Begin Source File

SOURCE=.\res\shx2.ico
# End Source File
# Begin Source File

SOURCE=.\res\shxx.ico
# End Source File
# Begin Source File

SOURCE=.\res\TRUSCAN.ICO
# End Source File
# Begin Source File

SOURCE=.\res\TRUSCAN.RC2
# End Source File
# Begin Source File

SOURCE=.\res\varcologo256.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section TruScan : {7251EE06-C869-11D3-9B48-00A0C91FC401}
# 	1:12:IDD_CUSTOMER:103
# 	2:16:Resource Include:resource.h
# 	2:9:WcJob.cpp:WcJob.cpp
# 	2:13:CLASS: CWcJob:CWcJob
# 	2:12:IDD_CUSTOMER:IDD_CUSTOMER
# 	2:10:ENUM: enum:enum
# 	2:7:WcJob.h:WcJob.h
# 	2:19:Application Include:TruScan.h
# End Section

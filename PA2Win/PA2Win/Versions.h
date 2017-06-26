#pragma once


#define CURRENT_VERSION		_T("Version 1.0")
#define BUILD_VERSION			2

#define VERSION_MAJOR           1
#define VERSION_MINOR          0
#define VERSION_BUILD           BUILD_VERSION

#define VERSION16				(VERSION_MAJOR<<13) | (VERSION_MINOR<<10) | (BUILD_VERSION & 0x1ff)
#define VERSION32				(VERSION_MAJOR<<29) | (VERSION_MINOR<<24) | (BUILD_VERSION & 7fffff)
#define PA2_VERSION _T("Version =   %d_%02d_%03d"), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD 
/*
Show the version number of this program. Newest version on TOP
1.0.002	2017-06-20	Runs both PAP and PAG almost identically. Shuts down w/o memory leaks.
1.0.1	2017-04-20	Migrate PAP from a service to an MFC windows app to better debug/correlate PAG and PAP










*/
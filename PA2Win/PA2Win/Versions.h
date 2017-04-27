#pragma once


#define CURRENT_VERSION		_T("Version 1.0")
#define BUILD_VERSION			1

#define VERSION_MAJOR           1
#define VERSION_MINOR          0
#define VERSION_BUILD           BUILD_VERSION

#define VERSION16				(VERSION_MAJOR<<13) | (VERSION_MINOR<<10) | (BUILD_VERSION & 0X1FF)
#define TRUSCAN_VERSION _T("Version = Phase Array2 %d.%d.%d"), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD 
/*
Show the version number of this program. Newest version on TOP
1.0.1	2017-04-20	Migrate PAP from a service to an MFC windows app to better debug/correlate PAG and PAP










*/
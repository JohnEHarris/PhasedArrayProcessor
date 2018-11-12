#pragma once


#define CURRENT_VERSION		_T("Version 1.0")
#define BUILD_VERSION			10

#define VERSION_MAJOR           1
#define VERSION_MINOR           0
#define VERSION_BUILD           BUILD_VERSION

#define VERSION16				(VERSION_MAJOR<<12) | (VERSION_MINOR<<8) | (BUILD_VERSION & 0xff)
#define VERSION32				(VERSION_MAJOR<<28) | (VERSION_MINOR<<24) | (BUILD_VERSION & 0x7ffff)
#define PA2_VERSION _T("Version =   %d_%02d_%03d"), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD 

#define MAJVER(n)	((n >> 12) & 0xf)
#define MINVER(n)	((n >> 8)  & 0xf)
#define BLDVER(n)	( n & 0xff)

/*
Show the version number of this program. Newest version on TOP
1.0.010 2018-11-09 Dispaly ADC/Pulser cmd queues on screen. Does 5k commands in 20 sec.
1.0.009	2018-11-02 Lower PA2WinDlg to below normal. Allows other threads to run first
1.0.008 2018-10-10 Debugging blast commands to ADC and PULSER
1.0.007 2018-08-21 Read machine type from file, rearrange order of PA2Struct.h
1.0.006	2018-08-13 Simulate DHCP assignment of PAP IP address for All Wall-mostly done with ini file
1.0.005 2018-07-26 Supports All Wall data and controls pulser from one PAP
1.0.004	2018-03-28 Supports seq length up to 7.Tested Nx operation with nx=3-7
					Seq length 3-7
1.0.003 2017-08-14 Idata_Packet same size and structure as data from NIOS. Drop interface gate, status and max wall reading..
					256 channels, 256 channels out. Start sequence of NIOS is same as start seq of	PAP.
1.0.002	2017-06-20	Runs both PAP and PAG almost identically. Shuts down w/o memory leaks.
1.0.1	2017-04-20	Migrate PAP from a service to an MFC windows app to better debug/correlate PAG and PAP










*/
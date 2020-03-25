#pragma once

// I AM PAP  ******************* GUI has its own version file
#define CURRENT_VERSION		_T("Version 1.0")
#define BUILD_VERSION			27

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
1.0.027 2020-03-25 IP connections shown in office except for pulser (no input source)
1.0.026 2020-03-19 Add connectivity dialog to PAP - show IP connections and Wall Instrument number
1.0.025 2019-10-31 Delete vChnls on ADC disconnect and shut down of PAP.	Quits w/o mem leaks
1.0.024 2019-10-30 Debugging PAP crash when we think PT disconnects
1.0.023 2019-06-28 Command log shows Large command names.KillServerSocket catches the exceptions
1.0.022 2019-06-10 Show small/large cmds already received during cmd download.
1.0.021 2019-05-31 TcgSeqGains read back works
1.0.020 2029-05-29 TCGBeam gain read back
1.0.019 2019-05-24 Changes to display of last/ cmdId, msg cnt,  1st word
1.0.018 2019-05-22 Implement TCGBeam Cmd and printing in ReadBack log
1.0.017 2019-05-20 Correct diaplsy of last cmd/seq number 1st word aggument
1.0.016 2019-04-22 Change NcNx window seq/chnl/gate with edit in addition to spinner. Change
					gate cmds to begin with gate variable instead of gate number
1.0.015 2019-04-12 Start testing Gate Cmd Read back from ADC board
1.0.014 2019-03-14 Add explicit ProcNull, limit blast output to List box to first 10, last 10 msgs
					Show last cmd seq number sent on Nx screen
1.0.013 2019-03-05 Implement Nx command for testing wall nx in PAP
1.0.012 2019-02-19 Change Ascan packet. Eliminate redundant data. Include Gate & TOF in ASCAN header
1.0.011 2019-01-07 Change PAG CAsyncSocket to sync CSocket
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
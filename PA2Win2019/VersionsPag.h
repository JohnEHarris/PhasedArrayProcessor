#pragma once

// I AM PAG  *******************
#define CURRENT_VERSION		_T("Version 1.0")
#define BUILD_VERSION			30

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
1.0.030 2020-09-28 Delete Elapse Timer and FIFO memory in sockets at beginning of destructor -
					now works same way in client and server socket.
1.0.029 2020-07-17 Modify crit sections/hwtimers in attempt to fix abandoned memory on closing -seems ok
1.0.028 2020-09-15 Appears we got rid of 4 mb orphaned segments- perhaps by closing all debug/status
					output files.
1.0.027 2020-08-04 Debug dialog in PAP allows for catching Ascan transmit data. Other changes in PAG
1.0.026 2020-06-11 Detect than no NIC/connection available to PAG. Stop crash when on client to server
// no change, just force commit after makin new origin
1.0.025 2020-04-13 PAG/PAP now can resize main dlg. PAP exits w/o lost memory
1.0.024 2020-03-18 Prototyping Connectivity dialog for PAP. May mod to use for PAG as well
1.0.023 2020-02-26 Header file for Idata/Cmds is now different. Cmds header is unchanged
1.0.022 2019-08-23 Change Versions.h to VersionsPag.h. Add X_Loc divider command 36
1.0.021 2019-06-27 Command log has TCG Seq Gain and TCG Beam Gain text. Blast300 does 900 small, 1000 large cmds
					PAP version shows queue depth when sending to ADC
1.0.020 2019-06-03 Blast TcgSeqGain.
1.0.019 2019-05-29 Readback all gate data and all TCGBeamGain data in separate commands. PAP
					outputs read back data to a read back log. Readback info also goes to 
					PAP or UUI.
1.0.018 2019-05-08 Test ADC Fifo's in test thread. Much easier to see variables and w/o
					having to use a real ADC board + ByteBlaster
1.0.017 2019-05-06 Add ReadBack log to PAP/PAG. Increase small cmds to 40. Add GateBlast cmd.
					Reduce size of Blast300 cmd
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
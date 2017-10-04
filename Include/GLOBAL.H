#pragma once
/*
Name:	Global.h
Purpose:	Group all variable which are global to a processor into
			one module
Date:	02/22/02
Author:	jeh
Revised:

 *
 *  Created on: Jul 29, 2016
 *      Author: joharris
 *      Make literals, structures, data types available to all c files.
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "PA2Struct.h"

// The MAC and IP addresses of the instrument are modified by board switch setting or
// by backplane board slot address when implemented.
//
#define WIZ_SERVER			"192.168.10.200"
// Client needs to know servers ip and port
// 192=C0,168=A8, 200=C8
#define WIZ_CLIENT			"192.168.10.201"
#define TARGET_SERVER		"192.168.10.10"
#define WIZ_GATEWAY			"192.168.10.1"

#define TARGET_PORT			7502

// first 32 bits of MAC address
#define WIZ_MAC_ADDR32_H		0x0008dc0
// last 16 bits of MAC address
// increment the low mac address by the board switch/slot position
#define WIZ_MAC_ADDR16_L		0x0000


#define PAP_SERVER	"192.168.10.10"
#define PAP_SERVER_PORT	7502

#define MS_500		500000		// 500,000 uSec

//#include "Timer64.h"

#ifdef MAIN_MODULE
/* Global variables are public in the including module */
#define PubExt
#else
#define PubExt	extern
#endif  /* Main_module */


#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif

#ifdef NIOS_CODE
/* nothing for Nios code now */
#else

class CTuboIni;
class CPA2WinDlg;
class CPA2WinApp;
class CTuboIni;
class CNcNx;

typedef struct 
	{
	CPA2WinDlg *pUIDlg;		// the user interface dialog
//	CDoNothingDlg *pDoNothingDlg;
	CPA2WinApp *ptheApp;
	CTuboIni *pTuboIni;		// not actually a dialog, but no harm, no foul
	CNcNx *pNcNx;			// Nc Nx test dialog
	}	GLOBAL_DLG_PTRS;

PubExt	GLOBAL_DLG_PTRS gDlg;
PubExt int gMaxChnlsPerMainBang;
PubExt int gMaxChnls;	// chanels per main bang * MaxSeqCount = 8*32 =256
PubExt int gnSeqModulo;	// last seq number before reset to 0
PubExt int gMaxSeqCount;
PubExt int gnMaxServers, gnMaxClientsPerServer;		// Server Connection Management
PubExt int gnMaxClients;							// Client Connection Management
PubExt int gnFifoCnt, gnAsyncSocketCnt;
PubExt WORD gwMsgSeqCnt;
PubExt BYTE gbStartSeqNumber;	// starting sequence number for next Idata packet
PubExt int nLoc; // simulate location of pipe
PubExt BYTE bLastFakeSeq;	// assuming fake data can reset before 32 ascans.
//PubExt ELAPSE64 TRecvCmd;
PubExt BYTE gbNiosGlitchCnt;	// usually Wiznet rest count. Must reset PAP data fifo processing on change of cnt
// for NIOS compiler
//#ifndef HANDLE
//#define HANDLE int
//#endif
PubExt HANDLE g_hTimerTick;
PubExt IDATA_PAP gLastIdataPap;
PubExt UINT guAscanMsgCnt;

#endif /* NIOS_CODE */

#endif /* GLOBAL_H_ */

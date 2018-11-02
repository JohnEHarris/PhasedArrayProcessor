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
#undef I_AM_THE_INSTURMENT


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


#ifdef MAIN_MODULE
/* Global variables are public in the including module */
#define PubExt
#else
#define PubExt	extern
#endif  /* Main_module */


#ifndef BYTE
typedef unsigned char  BYTE;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef UINT
typedef unsigned int	UINT;
#endif

#ifdef I_AM_THE_INSTURMENT
#include "Timer64.h"
PubExt ELAPSE64 TRecvCmd;
#ifndef HANDLE
#define HANDLE int
#endif

PubExt IDATA_FROM_HW FakeDATA;
PubExt ASCAN_DATA FakeASCAN;
PubExt READBACK_DATA ReadBackDATA;
PubExt BYTE gbFakeDataCmd;		// change operation of Xmit interrupt when fake data
PubExt BYTE gbMakeAscanFlag; 
PubExt ST_GATE_READBACK_DATA GateCmdData;		// a copy of every gate command setting received
PubExt BYTE gbFakeDataCnt;


#else	// not the instrument

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

PubExt GLOBAL_DLG_PTRS gDlg;
PubExt IDATA_PAP gLastIdataPap;		// data sent to down stream systems from PAP
PubExt ASCAN_DATA gLastAscanPap;
PubExt READBACK_DATA gLastRdBkPap;
PubExt IDATA_FROM_HW gLastAllWall;
PubExt ST_GATE_READBACK_DATA gLastGateCmd;
PubExt WORD gwMax0, gwMin0, gwMin1_0, gwMin2_0, gwZeroCnt, gwNot0;	// max and min of seq0, chnl 0
#endif

PubExt int gMaxChnlsPerMainBang;
PubExt int gMaxChnls;	// channels per main bang * MaxSeqCount = 8*32 =256
PubExt int gnSeqModulo;	// last seq number before reset to 0
PubExt int gnMaxSeqCount;	// never used
PubExt BYTE gbSeqPerPacket;	// up to 32 seq in packet to PAG. May be less
PubExt int gMaxSeqCount;
PubExt int gnMaxServers, gnMaxClientsPerServer;		// Server Connection Management
PubExt int gnMaxClients;							// Client Connection Management
PubExt int gnFifoCnt, gnAsyncSocketCnt;
PubExt WORD gwMsgSeqCnt;
PubExt BYTE gbStartSeqNumber;	// starting sequence number for next Idata packet
PubExt BYTE gbStartSeqNumberIncrement;
PubExt WORD gwLastCmdId;	// keep cmd ID of last command executed by NIOS
PubExt WORD gw1stWordCmd;	// keep 1st cmd word of last command executed by NIOS
PubExt BYTE gbCmdSeq;
PubExt BYTE gbCmdChnl;
PubExt BYTE gbCmdGate;
PubExt WORD gwStatus;		// bit field of error sent with Idata
PubExt WORD gwStatusHoldCnt;	// set to non-zero when status changes
			// main task manager will hold status bits for output until Hold Cnt decrements to 0
PubExt WORD gwSmallCmdLost, gwLargeCmdLost;
// track max command depth
PubExt BYTE gbSmallCmdQ, gbSmallCmdQPrior, gbLargeCmdQ, gbLargeCmdQPrior;
PubExt WORD gwSmallCmds, gwLargeCmds;	// count commands received
PubExt BYTE gbIntrMask;


PubExt int nLoc; // simulate location of pipe
PubExt BYTE bLastFakeSeq;	// assuming fake data can reset before 32 ascans.
PubExt BYTE gbNiosGlitchCnt;	// usually Wiznet rest count. Must reset PAP data fifo processing on change of cnt
PubExt HANDLE g_hTimerTick;
PubExt UINT guAscanDelay;
PubExt BYTE AscanSel;

PubExt UINT guAscanMsgCnt;		// only used in PAG
PubExt UINT guRdBkMsgCnt;		// only used in PAG

PubExt short gnFpgaTemp;	// temp of FPGA chip
PubExt short gnBoardTemp;	// temp of ADC board
//PubExt WORD gwPeakSelect;	// differentiates between gates and ut signal for Ascan display --new gwBeamType
// probably need to lower prf before requesting read back of command data.
PubExt BYTE gbReadBackReady;// once readback ready, skip AScan and replace with read back data
PubExt BYTE gbGateBits;		// what combination of gates are being sent in AScan data  cmd 26
PubExt BYTE gbBeamType;		// rf or gates cmd 23
PubExt BYTE gbChCmd24;
PubExt BYTE gbChCmd25;
PubExt WORD gwSeqCmd25;	// cmd 25
PubExt BYTE bAppIsClosing;
// PAP number now obtained by reading a file
PubExt BYTE gbAssignedPAPNumber;	// every PAP has it own unique code set
//PubExt BYTE gbActualClientConnection[MAX_CLIENTS_PER_SERVER];
PubExt PACKET_PER_SEC gPksPerSec[2];	//[0] = Nx data, [1] = AllWall data
PubExt UINT guPktAttempts[2][10];	//[0]=Nx, [1]=All wall count number of attempt to send to PAG
								//[][0] is 1st attempt, [][1] is 2nd etc
PubExt UINT guCmdsProcessed;	// number of commands processed by the ADC board
PubExt BYTE gbDebugMode;		// if not 0, print message info as they are executed.

// Pulser global variables
PubExt WORD gwPulserCmds;
PubExt WORD gwFPGA_VersionP;
PubExt WORD gwNIOS_VersionP;
PubExt WORD gwCPU_Temp;
PubExt WORD gwPap_Prf;


//#define SYNC							0x5CEBDAAD
//#define PRINT_MSG		(1 << 0)
//#define RESET_CMD_CNT	(1 << 1)

#endif /* GLOBAL_H_ */

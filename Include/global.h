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
#undef I_AM_THE_INSTRUMENT

#define PRINTF

#include "PA2Struct.h"
#include "Cmds.h"

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
#define MAX_PRF		20000
#define MIN_PRF		10



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

#ifdef I_AM_THE_INSTRUMENT
#include "Timer64.h"
PubExt ELAPSE64 TRecvCmd;
#ifndef HANDLE
#define HANDLE int
#endif

PubExt IDATA_FROM_HW FakeDATA;
PubExt ASCAN_DATA FakeASCAN;
PubExt READBACK_DATA ReadBackDATA;
PubExt BYTE gbFakeDataCmd;		// change operation of Xmit interrupt when fake data
PubExt ST_GATE_READBACK_DATA GateCmdData;		// a copy of every gate command setting received
PubExt ST_TCG_BEAM_GAIN_READBACK_DATA TcgGainReadback;
PubExt ST_TCG_SEQ_GAIN_READBACK_DATA TcgSeqGainReadBack;
PubExt BYTE gbFakeDataCnt;
PubExt WORD gwAscanCntLast, gwAscanCntPrevious;	// test to see if Ascans are running
PubExt WORD gwWarmStart;



#else	// not the instrument

class CTuboIni;
class CPA2WinDlg;
class CPA2WinApp;
class CNcNx;
class CIP_Connect;

typedef struct 
	{
	CPA2WinDlg *pUIDlg;		// the user interface dialog
//	CDoNothingDlg *pDoNothingDlg;
	CPA2WinApp *ptheApp;
	CTuboIni *pTuboIni;		// not actually a dialog, but no harm, no foul
	CNcNx *pNcNx;			// Nc Nx test dialog
	CIP_Connect* pIpConnect; // generate a dialog showing IP connections/ports and wall instrument number
	}	GLOBAL_DLG_PTRS;

PubExt GLOBAL_DLG_PTRS gDlg;
PubExt IDATA_PAP gLastIdataPap;		// data sent to down stream systems from PAP
PubExt ASCAN_DATA gLastAscanPap;
PubExt ST_GATE_READBACK_DATA gLastRdBkPap;
PubExt IDATA_FROM_HW gLastAllWall;
PubExt ST_GATE_READBACK_DATA gLastGateCmd;
PubExt ST_TCG_BEAM_GAIN_READBACK_DATA gLastBeamGainReadBack;
PubExt ST_TCG_SEQ_GAIN_READBACK_DATA gLastSeqGainReadBack;
PubExt ST_SMALL_CMD gLastCmd;	// sized for small cmds, used for both adc commands on PAP screen
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
PubExt WORD gwMsgSeqCnt;		// increments when Idata or Ascan sent
PubExt BYTE gbStartSeqNumber;	// starting sequence number for next Idata packet
PubExt BYTE gbStartSeqNumberIncrement;
PubExt WORD gwLastCmdId;	// keep cmd ID of last command executed by NIOS
PubExt WORD gwLastCmdSeqCnt;	// Msg seq count of last received command
PubExt WORD gw1stWordCmd;	// keep 1st cmd word of last command executed by NIOS
PubExt BYTE gbCmdSeq;
PubExt BYTE gbCmdChnl;
PubExt BYTE gbCmdGate;
PubExt WORD gwLastAdcCmdMsgCnt;	// what msg seq count associated with last received msg. 
								// Not same as gbCmdSeq which is part of a command. This is only for 1 ADC. 
PubExt WORD gwStatus;		// bit field of error sent with Idata
PubExt WORD gwBadSync;
PubExt WORD gwStatusHoldCnt;	// set to non-zero when status changes
			// main task manager will hold status bits for output until Hold Cnt decrements to 0
PubExt WORD gwSmallCmdLost, gwLargeCmdLost;
// track max command depth
PubExt BYTE gbSmallCmdQ, gbSmallCmdQPrior, gbLargeCmdQ, gbLargeCmdQPrior;
PubExt WORD gwSmallCmds, gwLargeCmds;	// count commands received
PubExt BYTE gbIntrMask;

// Execution Times in uSec to build read back message
PubExt UINT uTimeIdata;
PubExt UINT uTimeCmd204;
PubExt UINT uTimeCmd205;
PubExt UINT uTimeGates;	// readback time for all gates in one sequence
PubExt UINT uTimeAscan;

PubExt int nLoc; // simulate location of pipe
PubExt BYTE bLastFakeSeq;	// assuming fake data can reset before 32 ascans.
PubExt BYTE gbNiosGlitchCnt;	// usually Wiznet rest count. Must reset PAP data fifo processing on change of cnt
PubExt HANDLE g_hTimerTick;
PubExt UINT guAscanDelay;
PubExt BYTE AscanSel;

PubExt UINT guAscanMsgCnt;		// only used in PAG
PubExt UINT guIdataMsgCnt;      // only used in PAG
PubExt UINT guRdBkMsgCnt;		// only used in PAG

PubExt short gnFpgaTemp;	// temp of FPGA chip ADC
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
PubExt BYTE gbCmdQDepthP;
PubExt WORD gwPapPulserCmds, gwPapLargeCmds, gwPapSmallCmds;		// count commands going thru PAP

// These error counts should reset on DebugPrint msg bit 1
PubExt BYTE gbAdcMsgIdErrorCntPAP;		// count number of times adc msg lost in PAP
PubExt BYTE gbAdcMsgIdErrorCntADC;		// count number of times adc msg lost in ADC

PubExt CString gsWall_IP;		//IP + port of Wall Instrument
PubExt CString gsPulser_IP;     // IP + port of pulser

PubExt CString gsPAP2Wall_IP;
PubExt CString gsPAP2Pulser_IP;

PubExt CString gsPAP_Nx2UUI_IP;
PubExt CString gsPAP_AW2UUI_IP;

PubExt CString gsUUI_PAP_NxIP;	//IP + port of UUI server connected to wall
PubExt CString gsUUI_PAP_AllWall_IP;	//IP + port of UUI server connected to all wall

PubExt CString gsIniFilePath;      // ini file defines ip address of the inspection system
PubExt CString gsWallAssignPath;   // file name x.wall where x is the wall's id number

//#define SYNC							0x5CEBDAAD

#define PRINT_MSG		(1 << 0)
#define RESET_CMD_CNT	(1 << 1)
#define RESET_Q_MAX		(1 << 2)
#define NOTHING_IN_PARTICULAR (1 << 8)



#endif /* GLOBAL_H_ */

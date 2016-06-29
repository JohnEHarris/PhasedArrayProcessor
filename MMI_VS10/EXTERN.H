
/*
Extern.h
Group external variables functions here for global variables
01/05/99
03/15/02  3 categories of oblique

*/

#ifndef _EXTERN_H_25798_
#define _EXTERN_H_25798_

// define semaphore
#include "afxmt.h"


extern CPtrList plistUtData;	// data sent from asi to mmi
//extern CPtrList plistIpxIn;		// ipx packets collected by input thread
extern CPtrList plistUtCmnd;	// commands sent from mmi to asi instrument
extern CPtrList plistWallFlaw;	// Wall reject flaws for printing report
extern CPtrList plistLongFlaw;
extern CPtrList plistTranFlaw;	// Transverse reject flaws for printing report
extern CPtrList plistOblq1Flaw;
extern CPtrList plistOblq2Flaw;
extern CPtrList plistOblq3Flaw;
extern U_MSG_99 UdpMsg99;


// INSPECTION CONTROL GLOBAL VARIABLES

extern BYTE gShoeChnl, gShoe;	// the global active gate and active channel

extern char gszHelpFile[];
extern TCHAR gszScopeDmxCalFile[];	// directory for this executable


extern WORD gwUdpStatus[4];
extern int gnTruscopePeriod, gnVelocityDt;

//extern CTscanApp *m_ptheApp;
extern CTscanDlg* pCTscanDlg;
// extern update_all(void);
extern CONFIG_REC ConfigRec;
extern REPLAY_REC ReplayRec;	// 03/08/00 keep track or replay pipe
extern SITE_SPECIFIC_DEFAULTS SiteDefault;
extern SCOPE_DMX_CAL	DmxCalRec;	/* Setup for scope dmx board to make linear */

extern WORD wAlarmMask[4];	// alarm is masked off if its bit in mask is 0

extern HWND hTscanDlg;		//03/02/00 access to top window dialog via win32 calls

/***** Make all Ethernet packet ( IPX & UDP ) variables global *****/


#if 0
//extern 	SENDBUFFER SndBuf[NUMSND];		// Buffer pool for command messages to asi machine
extern 	SOCKADDR_IPX *pIpxAddrThere;
extern  WSABUF IpxSndBuf;
extern 	SOCKET sktIpxCmnd;
extern 	SOCKET sktIpxData;

extern 	SOCKADDR_IPX SockCmndAddr;
extern 	SOCKADDR_IPX SockDataAddr;
#endif

	//05-10-2000 add tcpip/udp
extern 	SOCKET m_sktUdp;	// bidirectional
extern 	SOCKADDR_IN SockMyUdpAddr;	// ip address of the MMI computer
extern 	SOCKADDR_IN SockOtherUdpAddr;	// ip address of Customer computer
extern  WSABUF UdpSndBuf;
extern 	UDPMSGBUF SndBufUDP[8];
extern 	SOCKADDR_IN *pUdpAddrThere;

#if 0
extern	HWND hIpxDlg;		// global handle of ipxdlg window
extern	HWND hIpxInThread;	// global handle of ipxin thread
#endif

//extern	LIST_SHARE ListShare;	// custom mutual exclusion structure
extern	NET_STATS IpxStat;

extern	int nLoop;

#if 0
extern	MEMBUF *pWebMemPtr;			//Share data with Web
extern	CMemFileA *pWebData;		//to the MemFileA Class which has the data buffer

#endif
extern	struct
		{
		int Entries;	// number of scope entries
		TRUSCOP2_SCOPE_ENTRY	se[5];
		}	OscopeTrace1;


extern	struct
		{
		int Entries;	// number of scope entries
		TRUSCOP2_SCOPE_ENTRY	se[14];
		}	OscopeTrace2;

extern	struct
		{
		int Entries;	// number of scope entries
		TRUSCOP2_SCOPE_ENTRY	se[5];
		}	OscopeTrigger;

#if 0
extern struct 
	{
	int Entries;	// number of scope entries
	SCOPE_ENTRY	se[5];
	} StdScope1;

extern struct 
	{
	int Entries;	// number of scope entries
	SCOPE_ENTRY	se[12];
	} StdScope2;

extern struct 
	{
	int Entries;	// number of scope entries
	SCOPE_ENTRY	se[18];
	} AbsScope1;

extern struct 
	{
	int Entries;	// number of scope entries
	SCOPE_ENTRY	se[18];
	} AbsScope2;

extern struct 
	{
	int Entries;	// number of scope entries
	SCOPE_ENTRY	se[18];
	} DifScope1;

extern struct 
	{
	int Entries;	// number of scope entries
	SCOPE_ENTRY	se[18];
	} DifScope2;
#endif

extern BYTE XdcrType[XDCR_TYPE];
extern TCHAR  *XdcrText[XDCR_TYPE];


// C style global gain functions.  Do not operate on class members

extern BOOL gIsChnlOn(int mach, int shoe, int chnl);
extern BOOL gSetChnlGain(int mach, int band, int shoe, int chnl, int gain);
extern int gGetChnlGain(int mach, int band, int shoe, int ch);
extern CONFIG_REC work;
extern float InchToMM (float f);
extern float MMToInch (float f);


#endif /* _EXTERN_H_25798_ */

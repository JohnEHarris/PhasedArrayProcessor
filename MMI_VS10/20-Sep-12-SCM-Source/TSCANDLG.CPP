// TscanDlg.cpp : implementation file
// JEH 01-11-00
// Renamed TscanDlg.cpp for Truscan project  ... jeh ... 5/30/00
/*Revised:	01/10/00  Add msg sending feature
				Get Instrument address with by sending msg 0.
			01/11/00 Use 100 ms timer to dequeue asi data msg's
			01/12/00 Move ipx initialization to InitIPX routine jeh
			01/12/00 Implement Catch dialog window to show ipx packets
			01/13/00 Implement Open Config File procedure
			01/14/00 Add example class and dialog to project.  Does not
					function in project.  For example only!
			01/24/00 Force user to open cfg file.  Get tholds from this file
			01/26/00 Update status window with config info on file open
					Update config record on files save.  Show time/date
					on status dlg window
			01/27/00 Add joint number to class.  Add Joint Num dialog to chang
					joint number.  Send msgs 18 (jnt num) and 19 (set all tholds)
					Add dummy invisible button to collect carriage returns
					to avoid exiting program when <cr> entered.
			02/01/00 Make flaw lists for each type of flaw.  Used to generate
					printed report.  Allows operator to limit amount printed 
					and type of info printed.
			02/24/00 Add Filter & Noise pop up dialog and class
			02/27/00 Add ChnlOn dialog and class
			02/29/00 add GainDlg dialog and class
			03/02/00 Add dibapi print screen capability.. taken from wellchek code
					Requires on_command handling of win32 message
					wm_user_print_screen
			03/08/00 Add global ReplayRec to keep track of pipe replays
			03/28/00 Add global scope selection structures.  Edit here to change
						selection text or function
			04/06/00 Add Isolog CAl dialog
			05/01/00 Save job totals for pipe categories ok/rej/ques
					in registry
			05/10/00 Add tcp/ip processing of messages
			05/11/00 Make Udpmsg99 global to the program
			05/31/00 Change socket types from ASIxxx to Tscanxxx
					Corresponds to changes in asi_ipx.h. Kept file name
			06/09/00 rename plistAsixxx to plistUtxxx
			10/23/00 Enable/disable TOP clock hardware
			01/08/02 For non-zero packet_type for IPX, open socket with
						NSPROTO_IPX+4 for packet type = 4
			01/18/01 Move Ethernet ipx/udp variables from class to global
					Start to use ipx input thread to handle ipx packets
			01/22/01 Implement custom mutual exclusion for sharing
					linked list plistUtData.  High prio thread fills 
					plistIpxIn.  TscanDlg xfers to plistUtData when opportune.
					Global ipx stats.
			04/26/02 Note changes in configrec and warn operator on exit program

*/
#if 0

#endif


#include "stdafx.h"
//#define MMI_CODE
#include "Truscan.h"
#include "TscanDlg.h"
#include <stdio.h>
#include <direct.h>
#include "Disk.h"
//#include "thread.h"
#include "afxmt.h"
#include "math.h"

#include <ws2spi.h>

#include "LoadProgress.h"
#include "AscanReadSeq.h"
#include "ClientConnectionManagement.h"	// 07/31/12 jeh
#include "CCM_SysCp.h"					// 07/31/12 jeh



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern THE_APP_CLASS theApp;

int i;
//#define MEM_MAX_CHANNEL		40
//short type;
CString st;
extern char szBar_ResetScalersTo1[], szControl_Warning[];
extern void  SetSyncRegisters();
bool bDUpdate,bInit;		//update dialog only
extern  BOOL bCMD ;
/******* GLOBAL VARIABLES  -- GLOBAL VARIABLES  *******/

//Global PtrLists for asi commands and data


CPtrList plistUtCmnd;	// commands sent to instrument
//CPtrList plistIpxIn;	// ipx packets collected by input thread
CPtrList plistUtData;	// data received from instrument
CPtrList plistWallFlaw;	// Analog reject flaws for printing report
CPtrList plistLongFlaw;
CPtrList plistTranFlaw;	// Transverse reject flaws for printing report
CPtrList plistOblq1Flaw;
CPtrList plistOblq2Flaw;
CPtrList plistOblq3Flaw;

extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of plistUtData */
BYTE   g_bConnected[MAX_SHOES+1];   /* network connection status.  0: not connected, 1: connected.  Master in 0, Slave1 in 1, etc. */
extern CPtrList g_pTcpListAscan;
extern CCriticalSection g_CriticalSectionAscan;
extern ASCAN_HEAD_NIOS g_AscanHead;	

/*********************************************************
Every child dialog must have a pointer to it.  Declare all
the child of TscanDlg dialog pointers here and in the child
dialog's header file with the statement:
static CChild *m_pDlg  where "child" is the dialog class name
**********************************************************/
CTscanDlg* pCTscanDlg;
CCatch* CCatch::m_pDlg;
//CInspect* CInspect::m_pDlg;

CTholds* CTholds::m_pDlg;		// 1-21-00
CAcal* CAcal::m_pDlg;			//02/02/00
// CFilter* CFilter::m_pDlg;		// 02/24/00
CChnlOn* CChnlOn::m_pDlg;		// 02/27/00
CGainDlg* CGainDlg::m_pDlg;		// 02/29/00
CWcJob *CWcJob::m_pDlg;			//03/01/00
CReplay *CReplay::m_pDlg;		//03/06/00
// CIsoCal *CIsoCal::m_pDlg;		//04/06/00
CEcho *CEcho::m_pDlg;			// 07/14/00
CNc *CNc::m_pDlg;				// 07/28/00
CWCalDlg *CWCalDlg::m_pDlg;		// 08/03/00
CIpxStat *CIpxStat::m_pDlg;		// 08/08/00
//CIpxDlg *CIpxDlg::m_pDlg;		// 01/19/01
CGateDlg *CGateDlg::m_pDlg;	// 08/21/01
CSysGain *CSysGain::m_pDlg; //9-12-01
CRcvr *CRcvr::m_pDlg;	// 09/18/01
CPulser *CPulser::m_pDlg;	//9-20-01
Ctof *Ctof::m_pDlg;	//9-25-01
COscpe *COscpe::m_pDlg;	//9-26-01
CTCG *CTCG::m_pDlg;	//9-26-01
CEnet* CEnet::m_pDlg;		//1-4-02
CStrpChnl* CStrpChnl::m_pDlg;		//5-23-02
CDmxAdj* CDmxAdj::m_pDlg;			// 6-03-02
//Ftext *Ftext::m_pDlg;
CViewcfg *CViewcfg::m_pDlg;			// 05-16-2003
Ftext2 *Ftext2::m_pDlg;
Ftext *Ftext::m_pDlg;
// GLOBAL WIN32 handles and other useful global data item.  Take that c++  03/02/00
HWND hTscanDlg;
HACCEL hAccelTable;


// INSPECTION CONTROL GLOBAL VARIABLES


#define MAKE_PUBLICS


//CONFIG_REC work;		// the global config record
int gChannel, gCopyTo, gGate;
BYTE gShoe, gShoeChnl;	// the global active gate and active channel
char gszHelpFile[256];	// global name of help file

TCHAR gszScopeDmxCalFile[300];

WORD gwUdpStatus[4];	// 4th word is line status

int gnTruscopePeriod, gnVelocityDt;

// UDP messages from the customer computer
//CPtrList plistUdpIn;
U_MSG_99 UdpMsg99;


// The working config record and the config record on the config file
CONFIG_REC ConfigRec, ConfigRecFile;
REPLAY_REC ReplayRec;
SCOPE_DMX_CAL	DmxCalRec;	/* Setup for scope dmx board to make linear */



// Default parameter applicable to a specific installation
// Stored in registry of mmi
SITE_SPECIFIC_DEFAULTS SiteDefault;

WORD wAlarmMask[4];	// alarm is masked off if its bit in mask is 0
BYTE bShowAlarmWarning = 0;			// Flag to alert to color alarm text on stat screen
int nShowAlarmWarningCnt = 0;		// Implement flashing text when alarm exists

/***** Make all Ethernet packet ( IPX & UDP ) variables global *****/

	MMI_CMD SndBuf[NUMSND];		// Buffer pool for command messages to Truscope machine
//	CDP_CMD SndBufCdp[NUMSNDCDP];	// Buffer pool for command messages to CDP

	SOCKADDR_IPX *pIpxAddrThere;
 	WSABUF IpxSndBuf;
//	WSABUF IpxSndBufCdp;
	SOCKET sktIpxCmnd=0;
	SOCKET sktIpxData;

	SOCKADDR_IPX SockCmndAddr;
	SOCKADDR_IPX SockDataAddr;


	//05-10-2000 add tcpip/udp
	SOCKET m_sktUdp;	// bidirectional
	SOCKADDR_IN SockMyUdpAddr;	// ip address of the MMI computer
	SOCKADDR_IN SockOtherUdpAddr;	// ip address of Customer computer
 	WSABUF UdpSndBuf;
	UDPMSGBUF SndBufUDP[8];
	SOCKADDR_IN *pUdpAddrThere;
	HWND hIpxDlg;		// global handle of ipxdlg window
	HWND hIpxInThread;	// global handle of ipxin thread

//	LIST_SHARE ListShare;	// custom mutual exclusion structure

	NET_STATS IpxStat;
	//add by Clive	01/17/01
//	MEMBUF *pWebMemPtr;			//Share data with Web
//	CMemFileA *pWebData;		//to the MapFileA Class which has the data buffer
	int nLoop = 0;


#define T2_GATES		1
#define T2_DIAG		2
#define T2_ALWAYS		0

	//	T2_GATES means this trace 2 selection IS ONLY available in "gates" mode
	//  T2_DIAG means this trace 2 selection IS ONLY available in debug mode

#if 0
	// In config.h
typedef struct
	{
	char	*Txt;	/* what text string to put in combo box */
	BYTE	bIndx;	/* index of the string in this structure */
	BYTE	bSelect;
	}	TRUSCOP2_SCOPE_ENTRY;
#endif

	struct
		{
		int Entries;	// number of scope entries
		TRUSCOP2_SCOPE_ENTRY	se[4];
		}	OscopeTrace1 =

	{
	4,
	"FW", 0, T2_ALWAYS,
	"RF", 1, T2_ALWAYS,
	"Zero Ref", 2,T2_DIAG,
	"Rcvr Out", 3, T2_DIAG
	};

	struct
		{
		int Entries;	// number of scope entries
		TRUSCOP2_SCOPE_ENTRY	se[15];
		}	OscopeTrace2 =

	{
	14,
	"Instrument Sync",0 ,T2_ALWAYS,
	"End of AScan", 1, T2_ALWAYS,	// txt, txt indx 
	"First of Frame", 2,T2_ALWAYS,
	"Gates 1&2", 3, T2_ALWAYS,
	"Blank G1-Th G2-Pk", 4,T2_ALWAYS,
	"Blank G1-Th G2-Th", 5,T2_ALWAYS,
	"Laminar Gate", 6, T2_ALWAYS,
	"Laminar Alarm", 7, T2_ALWAYS,
	"Wall Alarm", 8, T2_ALWAYS,
	"TOF Gate 1", 9, T2_ALWAYS,
	"TOF Gate 2", 10, T2_ALWAYS,
	"Pk Amp Gate 1", 11, T2_ALWAYS,
	"Pk Amp Gate 2", 12, T2_ALWAYS,
	"FIFO Full Error", 13,T2_ALWAYS,
	};

	struct
		{
		int Entries;	// number of scope entries
		TRUSCOP2_SCOPE_ENTRY	se[5];
		}	OscopeTrigger =
	{
	5,
	"Initial Pulse", 1, T2_ALWAYS,	// txt, txt indx, dbg, 
	"First of Frame", 2,T2_ALWAYS,
	"Gate 1", 3, T2_ALWAYS,
	"Gate 2", 4, T2_ALWAYS,
	"Gates 1&2", 5, T2_ALWAYS
	};


	int tmpchnl;

BYTE XdcrType[XDCR_TYPE] = { IS_NOTHING, IS_WALL,IS_LONG,IS_TRAN, IS_OBQ1, IS_OBQ2,
						IS_OBQ3, IS_LAM };

TCHAR  *XdcrText[XDCR_TYPE] = {_T("NONE"), _T("WALL") ,_T("LONG"), _T("TRAN"), _T("OBQ1"), _T("OBQ2"), _T("OBQ3"),_T("LAMN") };
WORD debug_104;
char UdpMem[128];
	/*  commands to udp/instrument  */
#if 0
/*
in pulser.cpp
void SetSyncRegisters ( void)
	{
	int i;
	WORD sync_mask;

	// Initial mode is slave in, pass thru or end of chain for all
	// lines and boxes
	sync_mask = 0x6cc;

	if ( (ConfigRec.pulser.mode == 0) &&		// prf mode
		 (ConfigRec.pulser.LocalPRF[gChannel/10]) )		// for testing, internal prf
		 sync_mask = 0x4cc;

	for ( i = 0; i < 4; i++) ConfigRec.UtRec.SyncControl[i] = sync_mask;
	}
*/

#endif


void send_instrument_channel_info( int chnl)
	{	// send the instrument all commands which affect a channel
		// this sets info for both gates of the selected channel
		
	int  g;

//	delay_enabled_flag = delay_flag;
	tmpchnl = gChannel;
	gChannel = chnl;
	g=gGate;
	CTscanDlg::SendMsg(CHANNEL_SELECT);
	for (gGate = 0; gGate < 2; gGate++)
		{
		CTscanDlg::SendMsg(GATE_SELECT);
		CTscanDlg::SendMsg(GATES_DELAY);		//gates_delay();	
		// all these commands implicitly work on gate and channel
		CTscanDlg::SendMsg(GATES_LEVEL);	//gates_level();	// global variables
		CTscanDlg::SendMsg(GATES_RANGE);	//gates_range();
		CTscanDlg::SendMsg(GATES_BLANK);	//gates_blank();
		CTscanDlg::SendMsg(GATES_TRIGGER);	//gates_trigger();
		if ( gGate == 0)
			{
			CTscanDlg::SendMsg(GATES_LAMIN);	//gates_lamin();
			CTscanDlg::SendMsg(LAM_GATE_LEVEL);	//lam_gate_level();
			}
		else CTscanDlg::SendMsg(TOF_RESOLUTION);

		CTscanDlg::SendMsg(ALARM_LAMIN);	//	alarm_lamin();
		CTscanDlg::SendMsg(ALARM_POLARITY);	//alarm_polarity();
		CTscanDlg::SendMsg(ALARM_TRIGGER);	//alarm_trigger();
						//send_alarm_dlg_info(chnl,g,delay_flag);
		CTscanDlg::SendMsg(TOF_TRIGGER);		//tof_trigger();
		CTscanDlg::SendMsg(TOF_STOPON);	//tof_stopon();
		}
	gGate = g;
    /* the following commands are by channel    */
//	send_receiver_dlg_info(chnl,delay_flag);
	CTscanDlg::SendMsg(RECEIVER_GAIN);	//receiver_gain();
	CTscanDlg::SendMsg(RECEIVER_REJECT);	//receiver_reject();
	CTscanDlg::SendMsg(RECEIVER_FILT);	//receiver_filter();
	CTscanDlg::SendMsg(RECEIVER_POLT);	//receiver_polarity();
	CTscanDlg::SendMsg(RECEIVER_RF_VIDEO);	//receiver_rf_video();
	CTscanDlg::SendMsg(FIRING_SEQ);	// SendFiringSeq(channel);
	
//	send_pulser_dlg_info(chnl,delay_flag);
		CTscanDlg::SendMsg(PULSER_WIDTH);	//pulser_width();
		// pulser_damping();
		CTscanDlg::SendMsg(PULSER_ONOFF);	//pulser_onoff();

	
//	send_tcg_dlg_info(chnl,delay_flag);
//
	CTscanDlg::SendMsg(TCG_FN);	//TCG_fn();		// load function selection argument
	CTscanDlg::SendMsg(TCG_ARG1);	//TCG_arg1();		// load a*t linear argument
	CTscanDlg::SendMsg(TCG_ARG2);	//TCG_arg2();		// load b*t*t 2nd order argument
	CTscanDlg::SendMsg(TCG_TRIGGER);	//TCG_trigger();	// load trigger source selection
	CTscanDlg::SendMsg(TCG_STEP);	//TCG_step();		// load time step size, 200ns-800ns.. 0-3

//		delay_enabled_flag = delay_flag;
	gChannel = tmpchnl;
	
	}	// send the instrument all commands which affect a channel
void set_sync_register()
	{
	/* One sync register per instrument.  Controls slave trigger bus and
		bus terminations.
	*/

	CTscanDlg::SendMsg(LOCAL_PRF_CONTROL);	//local_prf();		// Set local prf info
	CTscanDlg::SendMsg(PULSER_PRF);	//pulser_prf();		// set pulser mode and rate for this instrument

						// give instrument serial channel time to operate
	tmpchnl = gChannel;
	for ( gChannel = 0; gChannel < MAX_CHANNEL; gChannel += 10)
		{
		CTscanDlg::SendMsg(CHANNEL_SELECT);
		CTscanDlg::SendMsg(SET_SYNC_REG);
		}

	gChannel = tmpchnl;
	CTscanDlg::SendMsg(CHANNEL_SELECT);
//	return TRUE;
	};
 
void system_init()
	{
	int i;
	WORD prf;
//	BYTE buf[8];
	BYTE local_PRF, pulser_mode;
//	BYTE tmp_tcg_enable[MEM_MAX_CHANNEL];
	WORD wOldOpMode;
//	clock_t tnow;

	
	// save pulser config info
	prf = ConfigRec.pulser.prf;
	ConfigRec.pulser.prf = 1;	// 10 hz initial

	pulser_mode = ConfigRec.pulser.mode;
	ConfigRec.pulser.mode = 0;		// prf mode

	local_PRF = ConfigRec.pulser.LocalPRF[gChannel/10];
	ConfigRec.pulser.LocalPRF[gChannel/10] = 1;	// locally generated prf

	SetSyncRegisters();		// operate on lpWork->config.SyncControl .. in dlg_udpc.c
	set_sync_register();	// send info to instrument
	// after 2/23/98 also set prf related parameters
#if 0
	tnow = clock();
	while ( (tnow+1) < clock() );	// burn time
#endif
	// Turn off pc104 debug
	ConfigRec.OscopeRec.pc104_debug = 0;
	CTscanDlg::SendMsg(DEBUG_CONTROL);

	// Save old op mode and switch all slaves to nop op mode
	wOldOpMode = (ConfigRec.wOpMode & 3) + 0x200;	//NOP, RUN, CAL, PKT
	CTscanDlg::SendMsg(NOP_MODE);

	
//	delay_enabled_flag = 1;		// slow down queuing of enet messages in init
	CTscanDlg::SendMsg(SYSINIT);
#if 0
	tnow = clock();
	while ( (tnow+1) < clock() );	// burn time
#endif
	//sleep(50);
//	switchable_delay(25000);	// Give instrument time to process this command
								// before any others sent
	// remember how tcg was set.  Calling receiver gain will turn off
	// later in this init sequence
	//memcpy( (void *) &tmp_tcg_enable, (void *) &ConfigRec.receiver.tcg_enable[0],
					//MEM_MAX_CHANNEL);
	// Send message defining what level of mmi code running
	// Version 2 mmi cause more checks in instrument for download ok
//	MMI_version_num();		
	CTscanDlg::SendMsg(MMI_VERSION);		//ConfigRec.JobRec.vernumn

	for (i = 0; i < MAX_CHANNEL; i++)
		{
#if 1
		send_instrument_channel_info(i);
#else
		SendInstrumentChannelInfo(i);
#endif
		}

	
	CTscanDlg::SendMsg(DEBUG_CONTROL);


	CTscanDlg::SendMsg(SLAVE_TRIG_RESET);
	CTscanDlg::SendMsg(SLAVE_TRIGGER);	// negates affect of channel in


	gChannel = 0;
	CTscanDlg::SendMsg(CHANNEL_SELECT);
	CTscanDlg::SendMsg(FIRST_INSTRUMENT);	//first_instrument();

	gGate = 0;
	CTscanDlg::SendMsg(GATE_SELECT);
//	scope_trace1();
	CTscanDlg::SendMsg(SCOPE_TRACE1);
//	write_to_scope_demux ();
	// Now tell the master instrument to set the j-box mux
	CTscanDlg::SendMsg(SET_SCOPE_MUX);
//	scope_trace2();
	CTscanDlg::SendMsg(SCOPE_TRACE2);
//	write_to_scope_demux ();
	// Now tell the master instrument to set the j-box mux
	CTscanDlg::SendMsg(SET_SCOPE_MUX);

	CTscanDlg::SendMsg(TCG_COMPUTE_ALL);	//TCG_compute_all();	// this may take a while
#if 0
	tnow = clock();
	while ( (tnow+1) < clock() );	// burn time
#endif
	// Instrument computer 10 tables of 512 pts EACH with floating pt operations
	CTscanDlg::SendMsg(SYSINIT_COMPLETE);
//	delay_enabled_flag = 0;	// full speed for messages
#if 0
	if (init_udp_chart_assignments() != 0)
			{
			PostQuitMessage(0);
			return FALSE;
			}
	// 07/18/95 Load wall cal info into udp for each instrument.
	for ( i = 0; i < MAX_CHANNEL/10 ; i++) 
		{
		SetWallCalInfo(i);
		}

	SendNcTolerance();

	
	// Send wall value recorded in config to udp 07/15/96
	SendWallToUDP(hDlgDLG_STATUS, ConfigRec.pipeinfo.Wall);

#endif
	// Turn off all tcg, then turn back on only those selected as on
	// and used as wall channels.  Remember, system-on turns on all

#if 0
	// temp only for testing
	ConfigRec.receiver.tcg_system = 0;	// OFF
	CTscanDlg::SendMsg(TCG_SYSTEM);	//TCG_sys_on_off();

	// restore tcg status
	memcpy( (void *) &ConfigRec.receiver.tcg_enable,(void *) &tmp_tcg_enable, 
			MEM_MAX_CHANNEL);

	for ( gChannel = 0; gChannel < MAX_CHANNEL; gChannel++)
		{
		CTscanDlg::SendMsg(CHANNEL_SELECT);
		CTscanDlg::SendMsg(TCG_ENABLE);
		}
#endif

	gChannel = 0;
	CTscanDlg::SendMsg(CHANNEL_SELECT);

#if 1
	// restore pulser info
	ConfigRec.pulser.prf = prf;
	ConfigRec.pulser.mode = 	pulser_mode;
	ConfigRec.pulser.LocalPRF[gChannel/10] = 	local_PRF;

	SetSyncRegisters();		// operate on lpWork->config.SyncControl .. in dlg_udpc.c
	set_sync_register();	// send info to instrument
#endif

//	delay_enabled_flag = 0;	// full speed for messages
#if 0
	// Flaw scalers now done by send_instrument_channel_info  04/15/98
//	UpdateFlawDepthScalers(0);		// gate 0 download
//	UpdateFlawDepthScalers(1);		// gate 1 download
	SendAllFlawsFlag ();
	SendFlawDepthFlag ();
#endif

	gChannel = 0;
	CTscanDlg::SendMsg(CHANNEL_SELECT);

	// Turn oN pc104 debug
	ConfigRec.OscopeRec.pc104_debug = 1;
	CTscanDlg::SendMsg(DEBUG_CONTROL);
	CTscanDlg::SendMsg(wOldOpMode);

//	return TRUE;
	}


// c routine to convert CString to char array
// make sure the ptr to the char array is big enough to hold the converted string
// converted string is null terminated.
void CstringToChar(CString s, char *pChar)
	{
	char c;
	int i = 0;
	while (c = (char)s.GetAt(i))	{ pChar[i++] = c; }
	pChar[i] = 0;
	}

void CstringToTChar(CString s, TCHAR *pChar)
	{
	TCHAR c;
	int i = 0;
	while (c = (TCHAR)s.GetAt(i))	{ pChar[i++] = c; }
	pChar[i] = 0;
	}

static 	CTcpThreadRxList  *m_pTcpThreadRxList;  /* should be a member of CTscanDlg, but can't compile for unknown reason */

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
	{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
		enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
	{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT

	}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString str;
	str.Format(TRUSCAN_VERSION);
	SetDlgItemText(IDC_STATIC_VERSION, str);
	str = _T("IP Addr must be 192.168.10.10:7501");
	SetDlgItemText(IDC_STATIC_IP_ADDR, str);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#if 0
/********************** Thread Event Handler *************************************/
//add by Clive	01/17/01
int HandleSocketEvent(RXTXCOMMANDBUFFER* pRxTxCommandBuffer)
{
	typedef struct
	{
	RECBUF	rb;
	BYTE	pad[20];
	}	BIGBUF;

	static int nmsg = 0, nmtx = 0;
	SOCKADDR_IPX strAddrThere;

	WSABUF WsaBuf[1];
	int rv;
	ULONG nrx, flagrx;           //  bytes sent, flags
	int szAddrThere;
	CString s;

	I_MSG_CAL	*pImsg02;			// msg02 is much bigger than msg 01.
	static WORD wLastSeq = 0;

 
    int              Result;           // holds return values
    WSANETWORKEVENTS NetworkEvents;    // tells us what events happened
    CString          MsgText;			// holds text strings

    // Find out what happened and act accordingly.
    Result = WSAEnumNetworkEvents(pRxTxCommandBuffer->sktIpx, 
                                  RxTxEvent.m_hObject,
                                  &NetworkEvents);
    if (Result == SOCKET_ERROR) {
        
        // Handle the fatal error.
        MsgText.Format(
                 "WSAEnumNetworkEvents failed.  Error code: %d", 
                 Result);
		AfxMessageBox(MsgText);
        return FALSE;
        
    } 

    if (NetworkEvents.lNetworkEvents & FD_READ)
	{    
		pRxTxCommandBuffer->uMsg[0]++;	//monitor read activity
        // An FD_READ event has occurred on the connected socket.
        if (NetworkEvents.iErrorCode[FD_READ_BIT] == WSAENETDOWN) 
		{
            // There is an error.
			AfxMessageBox("NetworkEvent error WSAENETDOWN");
	        return FALSE;

        }	// end if (NetworkEvents.iErrorCode[FD_READ_BIT] == WSAENETDOWN)
		else
		{
            // Read data off the socket...
			WsaBuf[0].buf = (char*)(new BIGBUF);
			if (WsaBuf[0].buf == NULL)
			{ 
				AfxMessageBox("Error new BIGBUF");
				return FALSE;
			}
			WsaBuf[0].len = sizeof(BIGBUF);
			flagrx = 0;
			szAddrThere = sizeof(pRxTxCommandBuffer->strAddrThere);
			rv = WSARecvFrom
			  ( pRxTxCommandBuffer->sktIpx, WsaBuf, 1,       //  Socket, buffer array, buffer count
				&nrx, &flagrx,             //  Bytes received, flags
				(SOCKADDR*)&strAddrThere,  //  Sender return addr
				&szAddrThere,
				NULL,                      //  Overlap struct
				NULL                       //  Callback when would block
			  );
			if (rv == SOCKET_ERROR)
			{ rv = WSAGetLastError();
			  if (rv != WSAEWOULDBLOCK)
			  { s = "Rx ";
				s += GetWSAErrStr(rv);
				AfxMessageBox(s);
			  }
			  delete WsaBuf[0].buf;
			  return FALSE;
			}   //  End if (RecvFrom() == SOCKET_ERROR)
  			else  //  (RecvFrom() != SOCKET_ERROR)
			{	//  Do this to avoid a lot of casting.
		pImsg02 = (I_MSG_CAL*)WsaBuf[0].buf;
		pRxTxCommandBuffer->pDataPacketCnt[0]++;
		if ( wLastSeq == pImsg02->Seq) pRxTxCommandBuffer->pPacketDupCnt[0]++;
		else if (++wLastSeq != pImsg02->Seq) pRxTxCommandBuffer->pPacketLostCnt[0]++;
		wLastSeq = pImsg02->Seq;

		// Get comm statistics from instrument header
		pRxTxCommandBuffer->pCmndPacketCnt[0] = pImsg02->InspHdr.TotalCmnds;
		pRxTxCommandBuffer->pCmndLostCnt[0] = pImsg02->InspHdr.PacketsLost;
		pRxTxCommandBuffer->pCmndDupCnt[0] = pImsg02->InspHdr.PacketsDup;

		pRxTxCommandBuffer->pRpm[0] = pImsg02->InspHdr.Rpm;
		pRxTxCommandBuffer->pIps[0] = pImsg02->InspHdr.motion_pulse_per_sec;
		pRxTxCommandBuffer->pJointLength[0] = pImsg02->InspHdr.JointLength;

		pRxTxCommandBuffer->pwLineStatus[0] = pImsg02->InspHdr.wLineStatus;
		
		if (pImsg02->MsgID  == 0)
			memcpy(&pRxTxCommandBuffer->pstrAddrThere->sa_nodenum,
					&strAddrThere.sa_nodenum, 6);

#if 1

		if ( !IsDataClient() )	//John I modified IsDataClient function
			{	// No one there to consume packets... don't queue packet
			delete WsaBuf[0].buf;
			return 0;
			}
#endif
		plistUtData.AddTail( (void *) WsaBuf[0].buf);
		return 1;

			}	//  End else (RecvFrom() == SOCKET_ERROR)
		}// end else (NetworkEvents.iErrorCode[FD_READ_BIT] == WSAENETDOWN)        
	}//end if (NetworkEvents.lNetworkEvents & FD_READ) 
	return TRUE;
}



DWORD WINAPI RxTxThread(LPVOID pRxTxCommandBuffer)
{
    CString          MsgText; // holds message strings
    DWORD            WaitStatus;       // holds return value of the wait
    BOOL             KeepGoing = TRUE; // keep processing output requests?
    BOOL             Forever = TRUE;   // constant to avoid warning
	DWORD			NumEvents;
	WSAEVENT		EventArray[1];
    
    // Initialize the EventArray.  The only one events in it, for now,
    // is the Socket event; 
    NumEvents = 1;
    EventArray[0] = RxTxEvent.m_hObject;

    while (Forever) {

        // Wait for an event (or a queued callback function) to wake
        // us up.  This is an alertable wait state (fAlertable == TRUE).
        WaitStatus = WSAWaitForMultipleEvents(NumEvents,
                                              &EventArray[0],
                                              FALSE,        // fWaitAll
                                              WSA_INFINITE, // dwTimeout
                                              TRUE);        // fAlertable
        
        // Determine why we woke up and act accordingly.  Note that
        // breaking out of the switch causes us to break out of the
        // while loop as well.  When we don't want to break out of the
        // loop, case statements end with the continue statement.
        switch (WaitStatus) {
            
        case WSA_WAIT_FAILED:

            // A fatal error.  Pop up a message box and break out of
            // the while loop to end the thread.
			MsgText = "WSAWaitForMultipleEvents() failed"; 
			MsgText += GetWSAError();
			AfxMessageBox(MsgText);
            break;

        case WAIT_IO_COMPLETION:
            
            // An I/O completion routine has been executed.  Cleanup
            // has already occurred in SendCompFunc, so there is
            // nothing left to do.  Just reiterate through the loop.
            
            continue;
            
        case WSA_WAIT_EVENT_0:
            
            // The SocketEventObject has been signaled.  Handle it in
            // a separate function.  Break out of the thread if
            // HandleSocketEvent returns FALSE, indicating error.
            if (HandleSocketEvent((RXTXCOMMANDBUFFER*)pRxTxCommandBuffer)) {
                continue;
            } else {
                break;
            }

        default:
			break;

        } // switch (WaitStatus)  
    } // while (1)
  
    AfxMessageBox("Thread has ended");

    return 0;
}

/********************** End Thread Event Handler ************************************/

#endif


/////////////////////////////////////////////////////////////////////////////
// CTscanDlg dialog

CTscanDlg::CTscanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTscanDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CTscanDlg)
	m_szStat_Grade = _T("");
	m_szStat_Od = _T("5");
	m_szStat_Wall = _T("0.5");
	m_uJntNum = 1;
	m_sActCh = _T("");
	m_sCpyto = _T("");
	m_szNewOd = _T("5");
	m_szNewWall = _T("0.5");
	gChannel=0;
	gGate=0;
	gShoe=0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ConfigRec.JobRec.OD = 5.0f;    //(float) atof(m_szStat_Od);
	ConfigRec.JobRec.Wall = 0.5f;  //(float) atof(m_szStat_Wall);

	// by jeh.  See Prosise p192
	m_hAccelTable = ::LoadAccelerators(AfxGetInstanceHandle(),
						MAKEINTRESOURCE(IDR_ACCELERATOR));
	hAccelTable = m_hAccelTable;
	m_dcPrn = NULL;

	m_pTcpThreadRxList = NULL;
	m_pScopeDlg = NULL;
	m_pEndAreaDlg = NULL;
	m_pTcgSetupDlg = NULL;
	m_pInspectDlg1 = NULL;
	m_pInspectDlg2 = NULL;
	m_pScopeTrace2CalDlg = NULL;
	m_pScopeTrace1CalDlg = NULL;
	m_pOriginalConfig = NULL;
	m_pAscanDlg = NULL;
	m_pChnlTraceDlg = NULL;

	for (int i=0; i<MAX_SHOES+1; i++)
	{
		g_bConnected[i] = 0;
		m_bConnected[i] = 127;
	}
	m_nTimerCount = 0;		// WAS 6;

	QualifyConfigRec();
	m_bConfigLoaded = FALSE;

	m_pUndo = new CONFIG_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));

	m_nJointLength = 0;

	m_bStartUp = TRUE;

	m_nPacketNumber = 0;
	m_nMstrSentMsg = 0;
	m_nPipeStatus = 0;
}

CTscanDlg::~CTscanDlg()
	{


	DestroyDataClients(TRUE);

	// jeh 10/01/99 kill printer cdc
	if (m_dcPrn) delete m_dcPrn;
//	CloseHandle(m_hRxTxThread);
//	if (pWebData) delete pWebData;

	if (m_pTcpThreadRxList != NULL)
		{
		delete m_pTcpThreadRxList;
		m_pTcpThreadRxList = NULL;
		}

	delete m_pUndo;

	InstallHook(hTscanDlg, FALSE);
	}

void CTscanDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTscanDlg)
	DDX_Control(pDX, IDC_BUTTON_SLAVE9, m_btnSlave9);
	DDX_Control(pDX, IDC_BUTTON_SLAVE8, m_btnSlave8);
	DDX_Control(pDX, IDC_BUTTON_SLAVE7, m_btnSlave7);
	DDX_Control(pDX, IDC_BUTTON_SLAVE6, m_btnSlave6);
	DDX_Control(pDX, IDC_BUTTON_SLAVE5, m_btnSlave5);
	DDX_Control(pDX, IDC_BUTTON_SLAVE4, m_btnSlave4);
	DDX_Control(pDX, IDC_BUTTON_SLAVE3, m_btnSlave3);
	DDX_Control(pDX, IDC_BUTTON_SLAVE2, m_btnSlave2);
	DDX_Control(pDX, IDC_BUTTON_SLAVE1, m_btnSlave1);
	DDX_Control(pDX, IDC_BUTTON_MASTER, m_btnMaster);
	DDX_Control(pDX, IDC_SBcpyto, m_SBcpyto);
	DDX_Control(pDX, IDC_SBchnl, m_SBchnl);
	DDX_Text(pDX, IDC_STAT_GRADE, m_szStat_Grade);
	DDV_MaxChars(pDX, m_szStat_Grade, 9);
	DDX_Text(pDX, IDC_STAT_OD, m_szStat_Od);
	DDV_MaxChars(pDX, m_szStat_Od, 8);
	DDX_Text(pDX, IDC_STAT_WALL, m_szStat_Wall);
	DDV_MaxChars(pDX, m_szStat_Wall, 8);
	DDX_Text(pDX, IDC_ActChnl, m_sActCh);
	DDV_MaxChars(pDX, m_sActCh, 14);
	DDX_Text(pDX, IDC_CpyTo, m_sCpyto);
	DDV_MaxChars(pDX, m_sCpyto, 14);
	//}}AFX_DATA_MAP
	}

BEGIN_MESSAGE_MAP(CTscanDlg, CDialog)
	//{{AFX_MSG_MAP(CTscanDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_COMMAND(ID_DISPLAY_INSPECT, OnDisplayInspect)
	ON_COMMAND(ID_DISPLAY_JOB, OnDisplayJob)
	ON_COMMAND(ID_DISPLAY_PACKETDATA, OnDisplayPacketdata)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_PRINTERSETUP, OnFilePrintersetup)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_SETUP_JOINTNUMBER, OnSetupJointnumber)
	ON_COMMAND(ID_SETUP_SCOPE, OnSetupScope)
	ON_COMMAND(ID_SETUP_CUSTOMIZE, OnSetupCustomize)
	ON_COMMAND(ID_CALIBRATE_FLAWS, OnCalibrateFlaws)
	ON_BN_CLICKED(IDC_STAT_OD, OnStatOd)
	ON_BN_CLICKED(IDC_STAT_WALL, OnStatWall)
	ON_COMMAND(ID_CALIBRATE_WALL, OnCalibrateWall)
	ON_BN_CLICKED(IDC_RADIOGate2, OnRADIOGate2)
	ON_BN_CLICKED(IDC_RADIOGate1, OnRADIOGate1)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTCpyCh, OnCpyChnl)
	ON_COMMAND(ID_INSTRUMENT_GATES, OnInstrumentGates)
	ON_COMMAND(ID_INSTRUMENT_PULSER, OnInstrumentPulser)
	ON_COMMAND(Inst_Receiver, OnInstrumentReceiver)
	ON_COMMAND(ID_INSTRUMENT_SCOPE, OnInstrumentScope)
	ON_COMMAND(ID_INSTRUMENT_TCGSET, OnInstrumentTcgset)
	ON_COMMAND(ID_INSTRUMENT_TOFALARMS, OnInstrumentTofalarms)
	ON_COMMAND(ID_INSTRUMENT_ALLPANELSALTA, OnInstrumentAllpanelsalta)
	ON_COMMAND(ID_INSTRUMENT_CLOSEALLALTC, OnInstrumentCloseallaltc)
	ON_COMMAND(ID_FILE_FREEDISKSPACE, OnFileFreediskspace)
	ON_COMMAND(ID_DIAGNOSTIC_DISKDUMP, OnDiagnosticDiskdump)
	ON_COMMAND(ID_DIAGNOSTIC_ENETSTATUS, OnDiagnosticEnetstatus)
	ON_COMMAND(ID_DIAGNOSTIC_FLAWDEPTHTEST, OnDiagnosticFlawdepthtest)
	ON_COMMAND(ID_DIAGNOSTIC_PACKETDATA, OnDiagnosticPacketdata)
	ON_COMMAND(ID_DIAGNOSTIC_SCOPE, OnDiagnosticScope)
	ON_COMMAND(ID_DIAGNOSTIC_SYNC, OnDiagnosticSync)
	ON_COMMAND(ID_DIAGNOSTIC_UDPIO, OnDiagnosticUdpio)
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SETUP_SITE_CUSTOM, OnSetupSiteCustom)
	ON_COMMAND(ID_DIAGNOSTIC_ALARMS, OnDiagnosticAlarms)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_INSTRUMENT_ENDAREA, OnInstrumentEndarea)
	ON_COMMAND(ID_VIEW_CONFIG, OnViewConfig)
	ON_COMMAND(ID_PRINT_CFG, OnPrintCfg)
	ON_COMMAND(ID_FILE_WALLCONVERT, OnFileWallConvert)
	ON_UPDATE_COMMAND_UI(ID_FILE_WALLCONVERT, OnUpdateFileWallConvert)
	ON_BN_CLICKED(IDC_BTN_NEXTJOINT, OnBtnNextJoint)
	ON_COMMAND(ID_INSTRUMENT_ASCAN, OnInstrumentAscan)
	ON_COMMAND(ID_DISPLAY_CHNLTRACE, OnDisplayChannelTraces)
	ON_EN_CHANGE(IDC_STAT_GRADE, OnChangeStatGrade)
	ON_BN_CLICKED(IDC_BTN_PIPEPRESENT, OnBtnPipePresent)
	ON_COMMAND(ID_INSTRUMENT_ASCANSEQ, OnInstrumentAscanReadSeq)
	ON_COMMAND(ID_FILE_PRINTSCREEN, OnFilePrintscreen)
	ON_MESSAGE(WM_IPX_RXRDY, OnIpxRxRdy)
	ON_MESSAGE(WM_UDP_RXRDY, OnUdpRxRdy)
	ON_COMMAND(ID_FILE_PRINTSCREEN2, OnFilePrintscreen2)
	ON_BN_CLICKED(IDC_BTN_STARTMASTER, OnBtnStartMaster)
	//}}AFX_MSG_MAP

	// manually added
	ON_MESSAGE(WM_USER_PRINT_SCREEN, VS10_OnFilePrintscreen)
	ON_MESSAGE(WM_USER_UPDATE_TITLEBAR,VS10_UpdateTitleBar)	
	ON_MESSAGE(WM_USER_KILL_APP,VS10_OnCancel)	
//	ON_MESSAGE(WM_USER_GET_IPX_LIST,GetIpxList)	
	ON_MESSAGE(WM_USER_CLIENT_PKT_RECEIVED, GetReceivedPackets)		// jeh.. CCM has posted msg to look at rcvd pkt queque

END_MESSAGE_MAP()



// Wrappers to get from VC6 to Visual Studio 10
// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CTscanDlg::VS10_OnFilePrintscreen(WPARAM, LPARAM)
	{	OnFilePrintscreen();	return 0;	}

afx_msg LRESULT CTscanDlg::VS10_UpdateTitleBar(WPARAM, LPARAM)
	{	UpdateTitleBar();	return 0;	}

afx_msg LRESULT CTscanDlg::VS10_OnCancel(WPARAM, LPARAM)
	{	OnCancel();	return 0;	}

/**************************************************************************/
void chnl_update(short type,char chnl )
	{
	switch(type)
		{
			case IS_NOTHING:
			default:
				i = 0;
				break;

			case IS_WALL:
				i = 1;
				break;

			case IS_LONG:
				i = 2;
				break;

			case IS_TRAN:
				i = 3;
				break;

			case IS_OBQ1:
				i = 4;
				break;
			case IS_OBQ2:
				i = 5;
				break;
			case IS_OBQ3:
				i = 6;
				break;
	case IS_LAM:	i = 7;				break;		

		}
		if ( i > XDCR_TYPE) i = 0;
		st.Format(_T("S%1d:Ch%1d  %s"), ((chnl) / 10)+1, ((chnl) % 10)+1, XdcrText[i]);
}



#if 0
// Encapsulate all IPX initialization into the following module
// jeh 1-12-00

/**************  This function now done in IpxDlg.cpp  **********************/


BOOL CTscanDlg::InitIPX()
	{	// on success, return 1, failure return 0
	// 05/08/00 try setting up tcpip as well


	int rv;
//	int protoList[] = { NSPROTO_IPX, IPPROTO_UDP, 0 };
	WSAPROTOCOL_INFO strProtoInfo[MAX_PROTOCOLS];
	ULONG nProtoInfo;
	int optval;
	int nSockCmndLen;
	int nSockDataLen;
	int nIpx;		// protocol index of each protocol
	int i;

	CString s,t;
	// copied in toto from wellchekdlg
		

//  Open a socket connection to the instrument.  Must have Winsock v2.0 or
//  higher for IPX support.

	nProtoInfo = sizeof(strProtoInfo);
//  strProtoInfo[0].dwMessageSize = 1500;  no effect, still 0x240
	rv = WSAEnumProtocols(NULL, strProtoInfo, &nProtoInfo);
//	rv = WSCEnumProtocols(protoList, strProtoInfo, &nProtoInfo, &ErrorNum);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSockNA("EnumProtocol ");
		return 0;
		}

	nIpx = 1000;		// out of range number

	//FIND 1st protocol of each type
	for ( i = 0; i < rv; i++)
		{
		if (strProtoInfo[i].iProtocol == NSPROTO_IPX)
			{
			nIpx = i;
			break;
			}
		}

	
	if ( nIpx  > MAX_PROTOCOLS )
		{
		MessageBox(_T("Did not find IPX protocol"));
		return 0;
		}

//  Returned dwServiceFlags1 was:
//    XP1_CONNECTIONLESS     | XP1_MESSAGE_ORIENTED | XP1_SUPPORT_BROADCAST
//  | XP1_SUPPORT_MULTIPOINT | XP1_IFS_HANDLES
#if 0
	t.Format(_T("\r\nProto %08X"), strProtoInfo[0].dwServiceFlags1);
	s += t;
    m_ceStdOut.SetWindowText(s);

#endif

	//create a socket bound to a service provider transport
	sktIpxCmnd = WSASocket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+4,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
    );
	if (sktIpxCmnd == INVALID_SOCKET)
		{ 
		ErrMsgSockNA("Socket");
		//  Need sock_na flag here
		return 0;
		}

//  Instrument will send broadcast messages so he does not need to know this
//  IP address or Ethernet adapter ID, only the socket number.  We will get
//  his address when he transmits.  

	
	optval = TRUE;
	rv = setsockopt( sktIpxCmnd, SOL_SOCKET, SO_BROADCAST,
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return 0;
		}



//  Bind the cmnd  socket  to a local address
	memset(&SockCmndAddr, 0, sizeof(SockCmndAddr));
	SockCmndAddr.sa_family = AF_IPX;

//  sa_netnum is the network address (in network order) that was specified
//  for IPX/SPX compatible protocol advanced properties network address in
//  network setup.  It will be filled in by the driver.

//  SockCmndAddr.sa_nodenum[6] is the unique ID of the Ethernet adapter.
//  It will be filled in by the driver.

//  sa_socket is the port number for this application.
  // this is the IPX 'socket number'
#if 1
	SockCmndAddr.sa_socket = TscanCOMMAND;
	rv = bind(sktIpxCmnd, (SOCKADDR*)&SockCmndAddr, sizeof(SockCmndAddr));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		// Already running 1 copy of program
		MessageBox("Tscan is already running.  Close this copy and Maximize the copy already Running");
		return 0;
		} 
#endif

//  Fill the address structure with the bound values
	nSockCmndLen = sizeof(SockCmndAddr);
	rv = getsockname(sktIpxCmnd, (struct sockaddr*)&SockCmndAddr, &nSockCmndLen);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Name ");
		return 0;
		}

  	memset (&SockCmndAddr.sa_nodenum[0],0xff,6);

//  Bind the data  socket  to a local address
	sktIpxData = WSASocket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+4,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
							);
	if (sktIpxData == INVALID_SOCKET)
		{ 
		ErrMsgSockNA("Socket");
//		Need sock_na flag here
		return 0;
		}

//  Instrument will send broadcast messages so he does not need to know this
//  IP address or Ethernet adapter ID, only the socket number.  We will get
//  his address when he transmits.  
	optval = TRUE;
	rv = setsockopt( sktIpxData, SOL_SOCKET, SO_BROADCAST, 
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return 0;
		}

	memset(&SockDataAddr, 0, sizeof(SockDataAddr));
	SockDataAddr.sa_family = AF_IPX;

//  sa_netnum is the network address (in network order) that was specified
//  for IPX/SPX compatible protocol advanced properties network address in
//  network setup.  It will be filled in by the driver.

//  SockDataAddr.sa_nodenum[6] is the unique ID of the Ethernet adapter.
//  It will be filled in by the driver.

//  sa_socket is the port number for this application.
  // this is the IPX 'socket number'
#if 1
	SockDataAddr.sa_socket = TscanDATA;
	rv = bind(sktIpxData, (SOCKADDR*)&SockDataAddr, sizeof(SockDataAddr));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		return 0;
		} 
#endif

//  Fill the address structure with the bound values
	nSockDataLen = sizeof(SockDataAddr);
	rv = getsockname(sktIpxData, (struct sockaddr*)&SockDataAddr, &nSockDataLen);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Name ");
		return 0;
		}

//  This sets the socket to non-blocking mode and sends the WM_IPX_RXRDY msg to
//  this window when IPX data is available.  MESSAGE_MAP is configured to
//  call OnIpxRxRdy() when the msg is received.
	rv = WSAAsyncSelect( sktIpxData, m_hWnd, WM_IPX_RXRDY, FD_READ );  //  winuser.h
//		   ( sktIpxData, m_hWnd, WM_IPX_RXRDY, FD_READ | FD_WRITE );  //  winuser.h
    
	
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("AsyncSelect ");
 //		return 0;
		}

	return 1;
	}
#endif


/******************************************************************/

#if 0
BOOL CTscanDlg::InitUdp()
	{	// on success, return 1, failure return 0
	// 05/08/00 try setting up tcpip as well
	// MUST !!! call InitIxp first to set up operation of W2_32 DLL
	int rv;
//	int protoList[] = { NSPROTO_IPX, IPPROTO_UDP, 0 };
	// Function uses '50348' bytes of stack: exceeds /analyze:stacksize'16384'. Consider moving some data to heap
	//WSAPROTOCOL_INFO strProtoInfo[MAX_PROTOCOLS];
	WSAPROTOCOL_INFO *pstrProtoInfo = new WSAPROTOCOL_INFO[MAX_PROTOCOLS];
	ULONG nProtoInfo;
	int optval;
	int nUdp;		// protocol index of each protocol
	int i;

	CString s,t;
		
	if (NULL == pstrProtoInfo)
		{
		AfxMessageBox(_T("Failed to create WSAPROTOCOL_INFO array"));
		TRACE("Failed to create WSAPROTOCOL_INFO array\n");
		return FALSE;
		}

//  Open a socket connection to the instrument.  Must have Winsock v2.0 or
//  higher for IPX support.
	nProtoInfo = sizeof(WSAPROTOCOL_INFO) * MAX_PROTOCOLS;
//  strProtoInfo[0].dwMessageSize = 1500;  no effect, still 0x240
	rv = WSAEnumProtocols(NULL, pstrProtoInfo, &nProtoInfo);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSockNA("EnumProtocol ");
		delete [] pstrProtoInfo;
		return FALSE;
		}

	nUdp = 1000;		// out of range number

	//FIND 1st protocol of each type
	
	for ( i = 0; i < rv; i++)
		{
		if (pstrProtoInfo[i].iProtocol == IPPROTO_UDP)
			{
			nUdp = i;
			break;
			}
		}
	
	if ( nUdp > MAX_PROTOCOLS ) 
		{
		MessageBox(_T("Did not find  UDP protocol"));
		if (pstrProtoInfo)	delete [] pstrProtoInfo;
		return FALSE;
		}

	/****** ADD TCP/IP[UDP] socket to mix 5/10/2000	*****/

	m_sktUdp = WSASocket( AF_INET,	// address family
						SOCK_DGRAM, // socket type
						IPPROTO_UDP,  //protocol
						&pstrProtoInfo[nUdp],
						0,              //  Protocol info, group
						0               //  Flags
		//      WSA_FLAG_OVERLAPPED
		//    | WSA_FLAG_MULTIPOINT_C_LEAF 
		//    | WSA_FLAG_MULTIPOINT_D_LEAF
						);

	if (pstrProtoInfo)	delete [] pstrProtoInfo;

	if (m_sktUdp == INVALID_SOCKET)
		{ 
		ErrMsgSockNA("Socket");
		//  Need sock_na flag here
		return FALSE;
		}

	// One call for each option to be set
	
	optval = TRUE;	// turn the option on, if false, turn it off

	rv = setsockopt( m_sktUdp, SOL_SOCKET, SO_BROADCAST,
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return FALSE;
		}

	optval = TRUE;	// turn the option on, if false, turn it off
	rv = setsockopt( m_sktUdp, SOL_SOCKET, SO_REUSEADDR,
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return FALSE;
		}

	char HostName[64];
	rv = gethostname(HostName, sizeof(HostName));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return FALSE;
		}

	char IpAddr[8];
	HOSTENT *HostEnt = gethostbyname(HostName);
	if (HostEnt)
		{	// get my ip address
		strncpy(IpAddr, *HostEnt->h_addr_list, HostEnt->h_length);
		}

	memset(&SockMyUdpAddr, 0, sizeof(SockMyUdpAddr));
	SockMyUdpAddr.sin_family = AF_INET;
	SockMyUdpAddr.sin_port = htons(8000);	// udp port chosen by us
	memcpy(&SockMyUdpAddr.sin_addr, IpAddr, 4);
//	SockMyUdpAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Duplicate this info for the customer's computer
	memcpy( (void *)&SockOtherUdpAddr,
			(void *)&SockMyUdpAddr, sizeof(SOCKADDR_IN)); 

	// Broadcast to other socket to begin with
//	memset ( (void *)&SockOtherUdpAddr.sin_addr, 0xff,4);
	// if we want to broadcast, use the following
	SockOtherUdpAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
//	SockOtherUdpAddr.sin_addr.s_addr = inet_addr("1.1.1.12");	//asi simulator in lab

	rv = bind(m_sktUdp, (SOCKADDR*)&SockMyUdpAddr, sizeof(SOCKADDR_IN));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		return FALSE;
		} 



//  This sets the socket to non-blocking mode and sends the WM_UDP_RXRDY msg to
//  this window when UDP data is available.  MESSAGE_MAP is configured to
//  call OnUdpRxRdy() when the msg is received.

	rv = WSAAsyncSelect( m_sktUdp, m_hWnd, WM_UDP_RXRDY, FD_READ );  //  winuser.h
    
	
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("AsyncSelect ");
 //		return 0;
		}

	return TRUE;

	}
#endif

/******************************************************************/

BOOL CTscanDlg::IsDataClient()
	{	// Every client window which consumes IPX packets from
		// Instrument must be added here

	if (m_pInspectDlg1)
		{
		// If we are stopped or playback, equivalent to no data client
		m_bIsClient = FALSE;
//		if ( m_pInspectDlg1->m_nControlSel == INSPECT_STOP) return FALSE;
		if ( m_pInspectDlg1->m_nControlSel == INSPECT_PLAY) return FALSE;
		m_bIsClient = TRUE;
		return TRUE;
		}
	m_bIsClient = TRUE;
	if (CCatch::m_pDlg) return TRUE;
	if (CAcal::m_pDlg) return TRUE;
	if (CWCalDlg::m_pDlg) return TRUE;
	if (CDmxAdj::m_pDlg) return TRUE;
	m_bIsClient = FALSE;
	return FALSE;
	}

/******************************************************************/
void Close_Instruments()
{
	if (CRcvr::m_pDlg) CRcvr::m_pDlg->OnOK();
	if (CGateDlg::m_pDlg) CGateDlg::m_pDlg->OnOK();
//	if (CSysGain::m_pDlg) CSysGain::m_pDlg->OnOK();
	if (CPulser::m_pDlg) CPulser::m_pDlg->OnOK();
	if (Ctof::m_pDlg) Ctof::m_pDlg->OnOK();
	if (CTCG::m_pDlg) CTCG::m_pDlg->KillMe();

#ifndef  TWO_CHANNEL_4_GATE_BOARD
	if (COscpe::m_pDlg) COscpe::m_pDlg->OnOK();
#else
	//if (pCTscanDlg->m_pScopeDlg) (pCTscanDlg->m_pScopeDlg)->KillMe();
	if (pCTscanDlg->m_pTcgSetupDlg != NULL) (pCTscanDlg->m_pTcgSetupDlg)->KillMe();
#endif
}

BOOL CTscanDlg::DestroyDataClients(BOOL bForce)
{	// Every client window which consumes IPX packets from
	// Instrument must be added here and to IsDataClient function
	if (CCatch::m_pDlg) CCatch::m_pDlg->KillMe();
	if (CDmxAdj::m_pDlg) CDmxAdj::m_pDlg->KillMe();

	if (!bForce)
		if (m_pInspectDlg1)
			if (m_pInspectDlg1->m_nControlSel != INSPECT_STOP)
			{
				AfxMessageBox(_T("Must close inspection window before attempted operation!"));
				return FALSE;
			}

	if (m_pInspectDlg1) m_pInspectDlg1->KillMe();
	if (m_pInspectDlg2) m_pInspectDlg2->KillMe();
	if (CAcal::m_pDlg) CAcal::m_pDlg->KillMe();	// flaw cal
	if (CWCalDlg::m_pDlg) CWCalDlg::m_pDlg->KillMe();	// wall cal

	return TRUE;

}

/******************************************************************/
#if 0

void CTscanDlg::BuildConfigFileMsg(C_MSG_15 *pMsg)
	{
	// Construct all thresholds.  Like MSG 19
	// Quest same for ID/OD from screen options .  Same for reject
	pMsg->AmaLpQ = pMsg->AmaHpQ = ConfigRec.AmaRec.Band[HP].Thold[Q_THOLD_INDX];
	pMsg->AmaLpR = pMsg->AmaHpR = ConfigRec.AmaRec.Band[HP].Thold[R_THOLD_INDX];
	pMsg->SonoLpQ = pMsg->SonoHpQ = ConfigRec.SonoRec.Band[HP].Thold[Q_THOLD_INDX];
	pMsg->SonoLpR = pMsg->SonoHpR = ConfigRec.SonoRec.Band[HP].Thold[R_THOLD_INDX];
	pMsg->FS_ThruThin = ConfigRec.IsoRec.FS_ThruMin;
	pMsg->FS_ChordThin = ConfigRec.IsoRec.FS_ChordMin;
	pMsg->TholdThin = ConfigRec.IsoRec.TholdThin;
	pMsg->MaxDisplayThick = MAX_ISO_THICK_PCT_DISPLAY;
	pMsg->MaxDisplayThin = MAX_ISO_THIN_PCT_DISPLAY;
	}

/******************************************************************/
/******************************************************************/

void CTscanDlg::BuildAllAgains(C_MSG_16 *pMsg)
	{	// All gains associated with 2 shoe amalog
	int ib, is, ic;
	for ( ib = 0; ib < 2; ib++)
		for ( is = 0; is < 2; is++)
			for ( ic = 0; ic < 16; ic++)
				{
				pMsg->Band[ib].Sh[is].Gain[ic] =
					ConfigRec.AmaRec.Band[ib].Sh[is].Ch[ic].Gain;
				pMsg->Band[ib].Sh[is].DifGain[ic] =
					ConfigRec.AmaRec.Band[ib].Sh[is].Ch[ic].DifGain;
				}
	}

/******************************************************************/

#endif

#if 0
void CTscanDlg::BuildTholdMsg(C_MSG_19 *pMsg)
	{
	// Send tholds to instrument
	*(WORD *) &pMsg->TholdLong = *(WORD *) &ConfigRec.UtRec.TholdLong;
	*(WORD *) &pMsg->TholdTran = *(WORD *) &ConfigRec.UtRec.TholdTran;
	*(WORD *) &pMsg->TholdOblq1 = *(WORD *) &ConfigRec.UtRec.TholdOblq;
//	*(WORD *) &pMsg->TholdOblq2 = *(WORD *) &ConfigRec.UtRec.TholdOblq2;
//	*(WORD *) &pMsg->TholdOblq3 = *(WORD *) &ConfigRec.UtRec.TholdOblq3;
//	*(WORD *) &pMsg->TholdOblq4 = *(WORD *) &ConfigRec.UtRec.TholdOblq4;
	*(WORD *) &pMsg->TholdWall = *(WORD *) &ConfigRec.UtRec.TholdWall;
	pMsg->TholdWallThds[0] = ConfigRec.UtRec.TholdWallThds[0];
	pMsg->TholdWallThds[1] = ConfigRec.UtRec.TholdWallThds[1];
	pMsg->fWall = ConfigRec.JobRec.Wall;
	pMsg->fOD = ConfigRec.JobRec.OD;
	}

/******************************************************************/


void CTscanDlg::BuildAllSgains(C_MSG_20 *pMsg)
	{
	int ib, is, ic;
	for ( ib = 0; ib < 2; ib++)
		for ( is = 0; is < 2; is++)
			for ( ic = 0; ic < 16; ic++)
				{
				pMsg->Band[ib].Sh[is].Gain[ic] =
					ConfigRec.SonoRec.Band[ib].Sh[is].Ch[ic].Gain;
//				pMsg->Band[ib].Sh[is].DifGain[ic] =
//					ConfigRec.SonoRec.Band[ib].Sh[is].Ch[ic].DifGain;
				}
	}
#endif

/******************************************************************/
static WORD TrigMap[5] = { 0, 2,5,9,1};
// map bits for 5 combo box trg selections to demux hardware
// IP, FOF, G1, G2, G1&2
static WORD Trace2Map[16] = { 0, 1, 2, 0,0,0,0,0,0,0,0,0,0,0,0, 3};
// Not allowed, eas, fof, gates of some kind ... finish with Tick reg
static WORD Trace1Map[4] = { 1, 0, 1, 0};



CTcpThreadRxList* CTscanDlg::GetTcpThreadList(void)	
	{	return m_pTcpThreadRxList;	}


/*******************************************************************/
#ifdef TWO_CHANNEL_4_GATE_BOARD

extern void WaitMicroseconds(int nMicroseconds);

BOOL CTscanDlg::SendMsg(int nMsg)
{
	static WORD wPacketNum  = 0;
	static short nBufIndx = 0;		// index of static snd buffer
	WORD *pWArg;		/* word arg's */
	DWORD *pDWArg;		/* Dword arg's */
	BYTE *pBArg;		/* byte arg's */
	
	MMI_CMD *pMsg;
	WORD p, offmask;
	BYTE nRefreshRate;

	CString s;
	CString szDataFolder;
	CString szWO;

	// DEBUG PA GUI WITH syscp connection code 07-31-2012 jeh
	if ( NULL == m_pTcpThreadRxList)	return 0;

	// Truscope instrument msg 0-90H, Truscan DOS machine msg 0-21
	// Bias Truscan msg's by 1000 to allow Truscan msg with Truscope
	// msg.  Truscan instrument coded attempts to subtract 1000
	// from msg id before going to ProcPtrArray.
	//if (bDUpdate) return FALSE;		//do not send if upadte dialog only

	//if ( nMsg > 1079) return FALSE;		// only 80 msg's defined

	nBufIndx &= NUMSND-1;		// NUMSND must be power of 2

	pMsg = (MMI_CMD *) &SndBuf[nBufIndx];
//	pMsg->MsgNum = wPacketNum++;
	pMsg->MsgId = nMsg;
	pWArg = (WORD *) &pMsg->CmdBuf[0];
	pDWArg = (DWORD *) &pMsg->CmdBuf[0];
	pBArg = (BYTE *) &pMsg->CmdBuf[0];

	switch (nMsg)
	{
	default:
		break;

	case GATE_SELECT:
		//ConfigRec.receiver.det_option[gChannel] = m_pTcpThreadRxList->GetGateDetectMode(gChannel, gGate);
		m_pTcpThreadRxList->SendSlaveMsg(GATE_SELECT, gGate, 0, 0, 0, 0, 0);
		m_pTcpThreadRxList->EnableScopeDisplay(TRUE, gChannel, gGate);
		break;

	case CHANNEL_SELECT:
		//ConfigRec.receiver.det_option[gChannel] = m_pTcpThreadRxList->GetGateDetectMode(gChannel, gGate);
		m_pTcpThreadRxList->SendSlaveMsg(CHANNEL_SELECT, gChannel, 0, 0, 0, 0, 0);
		m_pTcpThreadRxList->EnableScopeDisplay(TRUE, gChannel, gGate);
		m_pTcpThreadRxList->SendMDAC();
		break;

	case PULSER_PRF:
		// The number in ConfigRec.pulser.prf is in Hz/10.  11/27/02, YG
		pWArg[0] = ConfigRec.pulser.mode;
		if (ConfigRec.pulser.mode == 0)
			{
			pWArg[1] = ConfigRec.pulser.prf*10;   // Multiply by 10 to convert to Hz
			}
		else 	pWArg[1] = ConfigRec.pulser.PulsePerRev;
		//m_pTcpThreadRxList->SetPulserMode (pWArg[0]);
		m_pTcpThreadRxList->SetPulserPRF (pWArg[1]);
		break;

	case PULSER_WIDTH:
		pWArg[0] = (WORD) (ConfigRec.pulser.pulse_width[gChannel] * 10);
		m_pTcpThreadRxList->SetPulserWidth (gChannel, pWArg[0]);
		break;

	case PULSER_DAMPING:
		break;

	case PULSER_ONOFF:
		if (ConfigRec.pulser.pulse_chn_on[gChannel] & PLS_ON_BIT )
			p = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult;	// set in udp config dlg
		else
			{	// will turn off either xmt, rcv, or both depending on 
			// what operator has checked in pulser dialog
			offmask = ~ConfigRec.pulser.pulse_chn_on[gChannel];
			p = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult & offmask;
			}
	
		p |= 0x100;		// preserve 2 pt seq info
		pWArg[0] = p;

		m_pTcpThreadRxList->SetPrfIntExt(gChannel/10);
		break;

	case PULSER_ALL_ONOFF:
		if (ConfigRec.pulser.pulse_chn_on[gChannel] & PLS_ON_BIT )
			p = 0x133;
		else 
			p=ConfigRec.pulser.pulse_on | 0x100;
		p &= 0x133;
		pWArg[0] = p;
		break;

	case RECEIVERREAD:
		break;

	case RECEIVER_GAIN:
		// The number in ConfigRec.receiver.gain is in dB*10 and 
		// the range is from -200 to 600.  To convert to dB with range from
		// 0 to 80 dB for the new 2-channel-4-gate board, do the following.
		// 11/27/02, Y.Guo
		float GainDB;  /* Gain in dB */

		GainDB = (float)(ConfigRec.receiver.gain[gChannel] - GAINMIN)*0.1f;
		m_pTcpThreadRxList->SetRcvrGain (gChannel, GainDB);
		break;

	case RECEIVER_REJECT:
		pWArg[0] =2*ConfigRec.receiver.reject[gChannel];
		break;

	case RECEIVER_FCNT:
		short nOffset;
		nOffset = ConfigRec.receiver.offset[gChannel];
		m_pTcpThreadRxList->SetRcvrOffset(gChannel, nOffset);
		break;

	case RECEIVER_FILT:
		// 0: wideband;  1: 2.25 MHz;  2: 3.5 MHz;  3: 5 MHz
		// This is the same in Truscope and the new 2-channel-4-gate board
		// 11/27/02, YG
		pWArg[0] =ConfigRec.receiver.fil_option[gChannel];
		m_pTcpThreadRxList->SetRcvrFilter (gChannel, pWArg[0]);
		break;

	case RECEIVER_POLT:
		// Plus: 0;  Minus:  1 for the 2-channel-4-gate board
		// Plus: 1;  Minus:  0 for the Truscope instrument.  11/27/02, YG
		//pWArg[0] =ConfigRec.receiver.polarity[gChannel];
		pWArg[0] = m_pTcpThreadRxList->GetGatePolarity (gChannel, gGate);
		m_pTcpThreadRxList->SetGatePolarity (gChannel, gGate, pWArg[0]);
		//m_pTcpThreadRxList->SetGatePolarity (gChannel, 0, 1-pWArg[0]);
		//m_pTcpThreadRxList->SetGatePolarity (gChannel, 1, 1-pWArg[0]);
		//m_pTcpThreadRxList->SetGatePolarity (gChannel, 2, 1-pWArg[0]);
		//m_pTcpThreadRxList->SetGatePolarity (gChannel, 3, 1-pWArg[0]);
		break;

	case RECEIVER_RF_VIDEO:
		// RF: 0;  Full Wave:  1 for the 2-channel-4-gate board
		// RF: 0;  Full Wave:  1 for the Truscope instrument.  11/27/02, YG
		//pWArg[0] =ConfigRec.receiver.det_option[gChannel];
		pWArg[0] = m_pTcpThreadRxList->GetGateDetectMode(gChannel, gGate);
		m_pTcpThreadRxList->SetGateDetectMode (gChannel, gGate, pWArg[0]);
		//m_pTcpThreadRxList->SetGateDetectMode (gChannel, 0, pWArg[0]);
		//m_pTcpThreadRxList->SetGateDetectMode (gChannel, 1, pWArg[0]);
		//m_pTcpThreadRxList->SetGateDetectMode (gChannel, 2, pWArg[0]);
		//m_pTcpThreadRxList->SetGateDetectMode (gChannel, 3, pWArg[0]);
		break;

	case RECEIVER_PROCESS:
		m_pTcpThreadRxList->SetReceiverProcess(gChannel);
		break;

	case GATEREAD:
		break;

	case GATES_DELAY:
		// The number in ConfigRec.gates.delay is in microsecond*10.  11/27/02, YG
		float Delay;
		Delay = ConfigRec.gates.delay[gChannel][gGate] * 0.1f;
		m_pTcpThreadRxList->SetGateDelay (gChannel, gGate, Delay);
		//TRACE ("TscanDlg: Delay = %f\n", Delay);
		break;

	case GATES_LEVEL:
		// The number in ConfigRec.gates.level is in actual percentage.
		// 11/27/00, YG.
		BYTE level;
		if ( gGate == 0)
			{	// +0% = 100 inst cnts, 100% = 200 inst cnts
			// 01/08/98 change : level = lpWork->gates.level[channel][gate] +100;
			ConfigRec.gates.g1_invrt[gChannel] &= 1;	// limit to 0/1
			if (ConfigRec.gates.g1_invrt[gChannel] == 0)
				{	// not inverted, need another 100 cnts
				level = ConfigRec.gates.level[gChannel][gGate]+100;
				}
			else 
				{	// inverted gate 1 level
				level = 100 - ConfigRec.gates.level[gChannel][gGate];
				}
			// send gate level to scope demux
			}
		else
			{	// 0% = 0 inst cnts, 100% = 200 inst cnts
			level = 2*ConfigRec.gates.level[gChannel][gGate];	// assume 0-100%
			// send gate level to scope demux
			}
		//	write_to_scope_demux ();
		pWArg[0] = level;

		level = (BYTE) ConfigRec.gates.level[gChannel][gGate];
		m_pTcpThreadRxList->SetGateLevel (gChannel, gGate, (float) level);
		m_pTcpThreadRxList->SetScopeTrace2ShuntGateDac (gChannel, gGate+1);
		break;

	case GATES_RANGE:
		// The number in ConfigRec.gates.range is in microsecond*10.  11/27/02, YG
		float Range;
		Range = ConfigRec.gates.range[gChannel][gGate] * 0.1f;
		m_pTcpThreadRxList->SetGateRange (gChannel, gGate, Range);
		break;
		break;

	case GATES_BLANK:
		// The number in ConfigRec.gates.blank is in microsecond*10.  11/27/02, YG
		float Blank;
		Blank = ConfigRec.gates.blank[gChannel][gGate] * 0.1f;
		m_pTcpThreadRxList->SetGateBlank (gChannel, gGate, Blank);
		break;
		break;

	case GATES_TRIGGER:
		pWArg[0] = ConfigRec.gates.trg_option[gChannel][gGate];
		m_pTcpThreadRxList->SetGateTrigMode (gChannel, gGate, pWArg[0]);
		break;

	case ALARM_LAMIN:
		pWArg[0] = ConfigRec.alarm.laminar[gChannel][gGate];
		m_pTcpThreadRxList->SetWallAlarm(gChannel, gGate, pWArg[0]);
		break;

	case ALARM_POLARITY:
		pWArg[0] = ConfigRec.alarm.polarity[gChannel][gGate];
		m_pTcpThreadRxList->SetGateAlarmLogic(gChannel, gGate, pWArg[0]);
		break;

	case ALARM_TRIGGER:
		pWArg[0] = ConfigRec.alarm.trigger[gChannel][gGate];
		m_pTcpThreadRxList->SetGateAlarm(gChannel, gGate, pWArg[0]);
		break;

	case SET_SCOPE_MUX:    /* 0x4A */
		m_pTcpThreadRxList->WriteScopeSettingsToBoard();
		break;

	case TOF_STOPON:
		pWArg[0] = ConfigRec.timeoff.stopon[gChannel][gGate];
		m_pTcpThreadRxList->SetGateTofStopon (gChannel, gGate, pWArg[0]);
		break;

	case TOF_TRIGGER:
		pWArg[0] = ConfigRec.timeoff.trigger[gChannel][gGate];
		m_pTcpThreadRxList->SetGateTofTrig (gChannel, gGate, pWArg[0]);
		break;

	case TCG_TRIGGER:
		m_pTcpThreadRxList->WriteTcgStepTrigSel(gChannel);
		break;

	case TCG_STEP:
		m_pTcpThreadRxList->WriteTcgStepTrigSel(gChannel);
		break;

	case TCG_FN:
		m_pTcpThreadRxList->SendTcgFunction (gChannel);
		break;

	case 0x70:
		m_pTcpThreadRxList->SendSlaveMsg(0x70,0,0,0,0,0,0);
		break;

	case FIRING_SEQ:
		m_pTcpThreadRxList->EnableFireSequence (gChannel, TRUE);
		break;

	case CHANNEL_CONFIG_MSG:
		m_pTcpThreadRxList->SendChnlCfgMsg();
		break;

	case SET_ALL_THOLDS:
		m_pTcpThreadRxList->SetAllTholds();
		break;

	case SET_NC_NX:
		m_pTcpThreadRxList->SetNcNx();
		break;

	case SET_WALL_COEFS:
		m_pTcpThreadRxList->SetWallCoefs();
		break;

	case JOINT_NUMBER_MSG:
		pDWArg[0] = pCTscanDlg->m_uJntNum;
		m_pTcpThreadRxList->SendSlaveMsg(JOINT_NUMBER_MSG, pWArg[0], pWArg[1],0,0,0,0);
		break;

	case STORE_JOBREC_MSG:
		// For now, make these two directories the same and put them into the default data directory.
#if ( (SYSTEM_ID == RAW_WALL_SYS) || RECORD_RAW_WALL )
		szDataFolder =  pCTscanDlg->m_szDefDataDir;
		szWO = ConfigRec.JobRec.WO;
		if (0 == szWO.GetLength())			szWO = _T("UNK");
		//CreateDirectory(_T("C:\\PhasedArray\\WallData"), NULL);
		//if ( strlen(ConfigRec.JobRec.WO) )
			{
			CString sErrorMsg;
			
			CString szWODir = szDataFolder + _T("\\") + szWO;	// data dir is set by setup dialog
//			char sDirName[512];
//			sprintf(sDirName, "C:\\PhasedArray\\WallData\\%s", ConfigRec.JobRec.WO);
//			if (CreateDirectory(sDirName, NULL) == 0)
			if (CreateDirectory(szWODir, NULL) == 0)	// Make a Work Order folder under the default data directory
				{
				if (GetLastError() != 183 /*ERROR_ALREDY_EXISTS*/)
					{
					sErrorMsg = _T("Failed creating the wall data directory ") + szWODir;
					AfxMessageBox(sErrorMsg);
					}
				else pCTscanDlg->SetDataWO_Folder(szWODir);
				}
			//else SetDataWO_Folder(szWODir);
			}
#endif

#if 0
		// use the same Work Order folder for both data files and pdf's of the data
		CreateDirectory(_T("C:\\PhasedArray\\Report"), NULL);
		if ( strlen(ConfigRec.JobRec.WO) )
		{
			CString sErrorMsg;
			char sDirName[128];
			sprintf(sDirName, "C:\\PhasedArray\\Report\\%s", ConfigRec.JobRec.WO);
			if (CreateDirectory(sDirName, NULL) == 0)
			{
				if (GetLastError() != 183 /*ERROR_ALREDY_EXISTS*/)
				{
					sErrorMsg.Format(_T("Failed creating the PDF report directory %s",sDirName);
					AfxMessageBox(sErrorMsg);
				}
			}
		}
		else
			CreateDirectory(_T("C:\\PhasedArray\\Report\\Unknown"), NULL);
#endif

		m_pTcpThreadRxList->SendJobRec();
		break;

	case SET_ASCAN_REGISTERS:
		m_pTcpThreadRxList->SetAscanRegisters(gChannel);
		break;

	case ASCAN_MODE:
		if (pCTscanDlg->m_pAscanDlg != NULL)
			m_pTcpThreadRxList->SendSlaveMsg(ASCAN_MODE, 1,0,0,0,0,0);
		else
			m_pTcpThreadRxList->SendSlaveMsg(ASCAN_MODE, 0,0,0,0,0,0);
		break;

	case SITE_SPECIFIC_MSG:
		// Retrieve site specific data from registry and down load to upd
		pCTscanDlg->GetRegistrySiteDefaults();

		m_pTcpThreadRxList->SetSiteDefaults(&SiteDefault);
		break;

	case ASCAN_REFRESH_RATE:
		nRefreshRate = ConfigRec.receiver.ascan_refreshrate;
		m_pTcpThreadRxList->SendSlaveMsg(ASCAN_REFRESH_RATE, nRefreshRate, 0, 0, 0, 0, 0);
		break;

	case ASCAN_BROADCAST:
		p = ConfigRec.receiver.ascan_broadcast;
		m_pTcpThreadRxList->SendSlaveMsg(ASCAN_BROADCAST, p, 0, 0, 0, 0, 0);
		break;

	case SET_ASCAN_PEAK_SEL_REG:
		p = !ConfigRec.receiver.det_option[gChannel];
		m_pTcpThreadRxList->SendSlaveMsg(SET_ASCAN_PEAK_SEL_REG, p, 0, 0, 0, 0, 0);
		break;

#if 1
	case RUN_MODE:
	case CAL_MODE:
		pWArg[0] = ConfigRec.bMotionTime;
		ConfigRec.wOpMode = nMsg;
		m_pTcpThreadRxList->SendSlaveMsg(nMsg, pWArg[0], 0, 0, 0, 0, 0);
		break;

	case NOP_MODE:
		m_pTcpThreadRxList->SendSlaveMsg(nMsg, 0, 0, 0, 0, 0, 0);
		break;
#endif
	}

	//WaitMicroseconds(100);
	//::Sleep(1);

	return TRUE;
}

#else // !define TWO_CHANNEL_4_GATE_BOARD

BOOL CTscanDlg::SendMsg(int nMsg)
	{
	ULONG ntx, flagtx;
	int rv, i, j;
	static WORD wPacketNum  = 0;
	static short nBufIndx = 0;		// index of static snd buffer
	WORD *pWArg;		/* word arg's */
	BYTE *pBArg;		/* byte arg's */
	SCOPE_DMX_DISPLAY *pSDDsply;
	SCOPE_DMX_ADJUST *pSDAdj;
	short nRf;		/* 0 = rf, 1 = fw */
	
	SCOPE_DMX_CAL	*pSDCal;	/* ptr to scope demux cal structure */
	CHANNEL_CONFIG2 *pChCfg;	/* chnl config struct for udp */
	C_MSG_ALL_THOLD *pThold;	/* Thold msg to udp */
	C_MSG_NC_NX *pNc;
	WALL_COEF *pWallCoef;
	SITE_SPECIFIC_DEFAULTS *pSiteDef;

	WORD wTrig, wTmp;
//	BYTE bTmp;


	MMI_CMD *pMsg;
	WORD p, offmask;
//	float fTmp;

	CString s;
//	int is, ic;	//, ib;

	// Truscope instrument msg 0-90H, Truscan DOS machine msg 0-21
	// Bias Truscan msg's by 1000 to allow Truscan msg with Truscope
	// msg.  Truscan instrument coded attempts to subtract 1000
	// from msg id before going to ProcPtrArray.
	if (bDUpdate) return FALSE;		//do not send if upadte dialog only

	if ( nMsg > 1079) return FALSE;		// only 80 msg's defined

	nBufIndx &= NUMSND-1;		// NUMSND must be power of 2

	pMsg = (MMI_CMD *) &SndBuf[nBufIndx];
//	pMsg->MsgNum = wPacketNum++;
	pMsg->MsgId = nMsg;
	pWArg = (WORD *) &pMsg->CmdBuf[0];
	pBArg = (BYTE *) &pMsg->CmdBuf[0];

	switch (nMsg)
		{	// switch on msg number
	case 1000:
		// Request Instruments network address.  Message is immaterial
		// use msg 1 info structure
		// drop into case 1

	case SYSINIT:
		pWArg[0] = 0;
		break;

	case CHAN_GATE_CONT:
		break;
	case PROCNULL:
		break;
	case SYSINIT_COMPLETE:
		break;
	case DEBUG_CONTROL:
		pWArg[0] = ConfigRec.OscopeRec.pc104_debug ^ 1;
		break;
	case SLAVE_TRIG_RESET:
		pWArg[0] = 0;
		break;
	case SET_SYNC_REG:
		pWArg[0]=(char)ConfigRec.UtRec.SyncControl[gChannel/10];

		break;
	case FIRST_INSTRUMENT:
		// Send by udp slave 0 to 1st instrument
		break;
	case LOCAL_PRF_CONTROL:
		pWArg[0] = ConfigRec.pulser.LocalPRF[gChannel/10];
		break;
	case MMI_VERSION:
		pWArg[0] = 200;	// version 2.0
		break;
	case HARDWARE_RESET:
		break;
	case SEND_ALL_FLAWS:
	pBArg[0] = ConfigRec.cal.AllFlawsFlag;
		break;
	case SEND_FLAW_DEPTH:
	pBArg[0] = ConfigRec.cal.FlawDepthFlag;
		break;
	case GATE_SELECT:
		pWArg[0] = gGate;
		break;
	case CHANNEL_SELECT:
		pWArg[0] = gChannel;
		break;
	case COPY_SELECT:
		break;
	case COPY_OK:
		break;
	case PULSERREAD:
		break;
	case PULSER_WIDTH:
		pWArg[0] = (char)ConfigRec.pulser.pulse_width[gChannel];
		break;
	case PULSER_PRF:
		pWArg[0]=ConfigRec.pulser.mode;
		if (ConfigRec.pulser.mode == 0)
			{
			pWArg[1] = ConfigRec.pulser.prf; 
			}
		else 	pWArg[1] = ConfigRec.pulser.PulsePerRev;
		break;

	case PULSER_DAMPING:
		break;
	case PULSER_ONOFF:
		if (ConfigRec.pulser.pulse_chn_on[gChannel] & PLS_ON_BIT )
			p = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult;	// set in udp config dlg
		else
			{	// will turn off either xmt, rcv, or both depending on 
			// what operator has checked in pulser dialog
			offmask = ~ConfigRec.pulser.pulse_chn_on[gChannel];
			p = ConfigRec.UtRec.Shoe[gShoe].Ch[gShoeChnl].udp_mult & offmask;
			}
	
		p |= 0x100;		// preserve 2 pt seq info
		pWArg[0] = p;
		break;

	case PULSER_ALL_ONOFF:
		if (ConfigRec.pulser.pulse_chn_on[gChannel] & PLS_ON_BIT )
			p = 0x133;
		else 
			p=ConfigRec.pulser.pulse_on | 0x100;
		p &= 0x133;
		pWArg[0] = p;
		break;

	case RECEIVERREAD:
		break;
	case RECEIVER_GAIN:
		// user works in 1/10th db. 800 cnt = 80.0db -> 4000 cnts in instrument
		WORD gain;
		float fact;

		// 9/8/94 jeh range now 0-80.0 db	
		// 09/12/94 RANGE -20 to 60
		fact = 4000.0f/ (float)(GAINMAX - GAINMIN);
		gain = (int) ( fact * (float)(ConfigRec.receiver.gain[gChannel] 
					- GAINMIN));
		pWArg[0] =gain;
		ConfigRec.receiver.tcg_enable[gChannel] = 0;

		break;
	case RECEIVER_REJECT:
		pWArg[0] =2*ConfigRec.receiver.reject[gChannel];
		break;
	case RECEIVER_FCNT:
		break;
	case RECEIVER_FILT:
		pWArg[0] =ConfigRec.receiver.fil_option[gChannel];
		break;
	case RECEIVER_POLT:
		pWArg[0] =ConfigRec.receiver.polarity[gChannel];
		break;
	case RECEIVER_RF_VIDEO:
		pWArg[0] =ConfigRec.receiver.det_option[gChannel];
		break;
	case GATEREAD:
		break;

	case GATES_DELAY:
		// 2b
		*(WORD *) &pMsg->CmdBuf[0] = (WORD)(16383 - ConfigRec.gates.delay[gChannel][gGate]);
		break;

	case GATES_LEVEL:
		BYTE level;
		if ( gGate == 0)
			{	// +0% = 100 inst cnts, 100% = 200 inst cnts
			// 01/08/98 change : level = lpWork->gates.level[channel][gate] +100;
			ConfigRec.gates.g1_invrt[gChannel] &= 1;	// limit to 0/1
			if (ConfigRec.gates.g1_invrt[gChannel] == 0)
				{	// not inverted, need another 100 cnts
				level = ConfigRec.gates.level[gChannel][gGate]+100;
				}
			else 
				{	// inverted gate 1 level
				level = 100 - ConfigRec.gates.level[gChannel][gGate];
				}
			// send gate level to scope demux
			}
		else
			{	// 0% = 0 inst cnts, 100% = 200 inst cnts
			level = 2*ConfigRec.gates.level[gChannel][gGate];	// assume 0-100%
			// send gate level to scope demux
			}
		//	write_to_scope_demux ();
		pWArg[0] = level;
		break;

	case GATES_RANGE:
		*(WORD *) &pMsg->CmdBuf[0] = (WORD)(16383 - ConfigRec.gates.range[gChannel][gGate]);
		break;
	case GATES_BLANK:
		pWArg[0] = 255 -ConfigRec.gates.blank[gChannel][gGate];
		break;
	case GATES_TRIGGER:
		pWArg[0] = ConfigRec.gates.trg_option[gChannel][gGate];
		break;
	case GATES_LAMIN:
		pWArg[0] = ConfigRec.gates.triger[gChannel];
		break;
	case LAM_GATE_LEVEL:
		pWArg[0] = 2*ConfigRec.lamgate.level[gChannel];
//	write_to_scope_demux ();
		break;
	case ALARM_LAMIN:
		pWArg[0] = ConfigRec.alarm.laminar[gChannel][gGate];
		break;
	case ALARM_POLARITY:
		pWArg[0] = ConfigRec.alarm.polarity[gChannel][gGate];
		break;
	case ALARM_TRIGGER:
		pWArg[0] = ConfigRec.alarm.trigger[gChannel][gGate];
		break;
	case SCOPE_TRACE1:
		pWArg[0] = ConfigRec.OscopeRec.trace1[gChannel];
		break;
	case SCOPE_TRACE2:
		pWArg[0] = ConfigRec.OscopeRec.trace2[gChannel];
		break;
	case SET_SCOPE_MUX:
		pWArg[0] = gChannel/10;
		break;
	case TOF_RESOLUTION:
		if(gGate == 0)
		return FALSE;
		pWArg[0] = ConfigRec.timeoff.resolution[gChannel];
		break;
	case TOF_STOPON:
		pWArg[0] = ConfigRec.timeoff.stopon[gChannel][gGate];
		break;
	case TOF_TRIGGER:
		pWArg[0] = ConfigRec.timeoff.trigger[gChannel][gGate];
		break;
	case TCG_ARG1:
		*(WORD *) &pMsg->CmdBuf[0] = ConfigRec.receiver.arg1[gChannel];
		break;
	case TCG_ARG2:
		*(WORD *) &pMsg->CmdBuf[0] = ConfigRec.receiver.arg2[gChannel];
		break;
	case TCG_COMPUTE:
		pWArg[0] = 0;
		break;
	case TCG_TRIGGER:
		pWArg[0] = ConfigRec.receiver.tcg_trigger[gChannel];
		break;
	case TCG_STEP:
		pWArg[0] = ConfigRec.receiver.tcg_step[gChannel];
		break;
	case TCG_ENABLE:
		*(WORD *) &pMsg->CmdBuf[0] = ConfigRec.receiver.tcg_enable[gChannel];
		break;
	case TCG_SYSTEM :
		*(WORD *) &pMsg->CmdBuf[0] = ConfigRec.receiver.tcg_system;
		break;
	case TCG_COMPUTE_ALL:
		pWArg[0] = 0;
		break;
	case TCG_FN:
		wTmp = ConfigRec.receiver.tcg_function[gChannel];
		*(WORD *) &pMsg->CmdBuf[0] = wTmp;
		break;

	case  FIRING_SEQ:
		wTmp = ConfigRec.UtRec.Shoe[gChannel/10].Ch[gChannel%10].udp_mult & 0x133;
		// CHANNEL IS 1ST BYTE OF MSG
		pBArg[0] = gChannel;
		*(WORD *) &pBArg[1] = wTmp;
		break;
	case  UDPCONFIG:
		//	*(WORD *) &pMsg->CmdBuf[0] = 
		break;
	case SLAVE_TRIGGER:
		*(WORD *) &pMsg->CmdBuf[0] = ConfigRec.UtRec.trigger_a | ConfigRec.UtRec.trigger_b * 256;
		break;

	case RUN_MODE:
	case CAL_MODE:
		pWArg[0] = ConfigRec.bMotionTime;
		ConfigRec.wOpMode = nMsg;
		break;

	case NOP_MODE:
	case PKT_MODE:
		// THE MSG ID IS THE MSG
		ConfigRec.wOpMode = nMsg;
		break;
	case ENET_STATS_MODE:
		break;
#if 0
	case SCOPE_DMX_CAL_MODE:
		// Let Master UDP know which step in sequence we are on
		pSDCal = (SCOPE_DMX_CAL	*)  &pMsg->CmdBuf[0];	/* ptr to scope demux cal structure */
		memcpy( (void *) &pSDCal->nCmd, &DmxCalRec, sizeof(DmxCalRec));
		pSDCal->nCmd = 3;	// autocal in progress
		break;
#endif


	// UDP Scope Demux commands.. base command + sub command
	case (SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY):
	case (SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST):
	case (SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL):
	case (SCOPE_DEMUX_CMD + SCOPE_DEMUX_STRIPOUT):
	case (SCOPE_DEMUX_CMD + SCOPE_DEMUX_DIGIO):
	
		i = nMsg - SCOPE_DEMUX_CMD;	// down to sub command
			pSDDsply = (SCOPE_DMX_DISPLAY *) &pMsg->CmdBuf[0];
			pSDDsply->SubCmd = i;
		switch (i)
			{	// switch on demux sub command

		case SCOPE_DEMUX_DISPLAY:
			if (ConfigRec.UtRec.sequence_length == 2)
				pSDDsply->SeqLen = 1;
			else pSDDsply->SeqLen = 0;
			// tirgger a function of trig select combo and seq pt
			if (ConfigRec.OscopeRec.SeqPtTrigger) wTrig = 0x10;
			else wTrig = 0;
			wTrig |= TrigMap[ConfigRec.OscopeRec.trigger[gChannel]];
			pSDDsply->Trig = wTrig;
			pSDDsply->Trace1 = Trace1Map[ConfigRec.OscopeRec.trace1[gChannel]];
					/* rf or fw */
			pSDDsply->Trace2Mode = ((ConfigRec.OscopeRec.T2GateOrSync[gChannel] << 1) &3) |
									(pSDDsply->Trace1 & 1);
					/* Sync or Gates */
			pSDDsply->Trace2 = Trace2Map[ConfigRec.OscopeRec.trace2[gChannel]];
					/* FOF, EOA, or Gates .. add tick reg 3/20/02*/
#if 0
			fTmp = (float)ConfigRec.gates.level[gChannel][0]; //byte to word
			pSDDsply->G1Level = 4095 - (WORD)(fTmp*40.95);
			fTmp = ConfigRec.gates.level[gChannel][1];
			pSDDsply->G2Level = 4095 - (WORD)(fTmp*40.95);
			// after 6/20/02 let scaling be done by master udp based on demux cal operation
#endif
			pSDDsply->G1Level= ConfigRec.gates.level[gChannel][0];
			pSDDsply->G2Level= ConfigRec.gates.level[gChannel][1];

			break;


		case SCOPE_DEMUX_ADJUST:
			// Send the cal structure even though just adjusting setup demux
			// change this to be different from autocal
			pSDAdj = (SCOPE_DMX_ADJUST *) &pMsg->CmdBuf[0];
#if 0
				WORD SubCmd;	/* SCOPE_DEMUX_ADJUST */
				WORD V_ChNull;	/* hw default */
				WORD V_Rej;		/* hw default */
				WORD V_Gain;	/* hw default */
				WORD V_ScopeNull;	/* hw default */
				WORD V_Shunt;	/* hw default */
				}	SCOPE_DMX_ADJUST;
#endif

			// decide if rf or fw, no reject for rf channels
			// can be over ridden by master when in scope cal mode
			nRf = Trace1Map[ConfigRec.OscopeRec.trace1[gChannel]];
			ASSERT ( nRf < 2 );
			if (nRf)
				{	// full wave.
				pSDAdj->V_Rej = ConfigRec.OscopeRec.reject_pcnt;	// one value for all fw
				}
			else
				pSDAdj->V_Rej = 0;	// rf, reject below 0 %, .. write 4000-> rej reg on scope dmx


			pSDAdj->V_ChNull = DmxCalRec.nChNull[gChannel][nRf];
			pSDAdj->V_Gain = DmxCalRec.nVgain[gChannel][nRf];
			pSDAdj->V_ScopeNull = DmxCalRec.nScope1Null[nRf];
			pSDAdj->V_Shunt = DmxCalRec.nShunt[nRf];
			break;

		case SCOPE_DEMUX_AUTOCAL:
			// Send the cal structure even though just adjusting setup demux
			pSDDsply = (SCOPE_DMX_DISPLAY *) &pMsg->CmdBuf[0];
			pSDDsply->SubCmd = i;
			pSDCal = (SCOPE_DMX_CAL	*)  &pMsg->CmdBuf[0];	/* ptr to scope demux cal structure */
			memcpy( (void *) &pSDCal->SubCmd, &DmxCalRec, sizeof(DmxCalRec));
			break;


		default:
			break;
			}	// switch on demux sub command
		break;

	case CHANNEL_CONFIG_MSG:
		pChCfg = (CHANNEL_CONFIG2 *) &pMsg->CmdBuf[0];
		for ( j = 0; j < 4; j++)	// loop thru all 4 shoes
			for ( i = 0; i < 10; i++)	// loop thru 10 chnls
				{
				pChCfg->Ch[j][i].Type = ConfigRec.UtRec.Shoe[j].Ch[i].Type;
				pChCfg->Ch[j][i].cXOffset = ConfigRec.UtRec.Shoe[j].Ch[i].cXOffset;
				pChCfg->Ch[j][i].cWOffset = ConfigRec.UtRec.Shoe[j].Ch[i].cWOffset;
				}

		break;

	case JOINT_NUMBER_MSG:
		*(DWORD *) &pMsg->CmdBuf[0] = pCTscanDlg->m_uJntNum - 1;
		break;

	case SET_ALL_THOLDS:
		pThold = (C_MSG_ALL_THOLD *) &pMsg->CmdBuf[0];
		for ( i = 0; i < 2; i++)
			{
			pThold->TholdLong[i] = ConfigRec.UtRec.TholdLong[i];
			pThold->TholdTran[i] = ConfigRec.UtRec.TholdTran[i];
			pThold->TholdOblq1[i] = ConfigRec.UtRec.TholdOblq1[i];
			pThold->TholdOblq2[i] = ConfigRec.UtRec.TholdOblq2[i];
			pThold->TholdOblq3[i] = ConfigRec.UtRec.TholdOblq3[i];
			pThold->TholdWall[i] = ConfigRec.UtRec.TholdWall[i];
			// computer walll in thds
			pThold->TholdWallThds[i] = ConfigRec.UtRec.TholdWallThds[i];
			}
		pThold->fWall = ConfigRec.JobRec.Wall;
		pThold->fOD = ConfigRec.JobRec.OD;

		break;


	case SET_NC_NX:
		pNc = (C_MSG_NC_NX *) &pMsg->CmdBuf[0];
		for ( i = 0; i < 2; i++)
			{
			pNc->Long[i] = ConfigRec.UtRec.NcLong[i];
			pNc->Tran[i] = ConfigRec.UtRec.NcTran[i];
			pNc->Oblq1[i] = ConfigRec.UtRec.NcOblq1[i];
			pNc->Oblq2[i] = ConfigRec.UtRec.NcOblq2[i];
			pNc->Oblq3[i] = ConfigRec.UtRec.NcOblq3[i];
			pNc->Wall[i] = ConfigRec.UtRec.NxWall[i];
			}
		pNc->Tol = ConfigRec.UtRec.Tol;

		break;

	case SET_WALL_COEFS:
		// The slave udp picks out the coef's which apply to it
		// Send coef's for all 4 slaves

		pWallCoef = (WALL_COEF *) &pMsg->CmdBuf[0];
		for ( i = 0; i < 4; i++)
			{
			pWallCoef->fWallSlope[i] = ConfigRec.UtRec.fWallSlope[i];
			pWallCoef->WallOffset[i] = ConfigRec.UtRec.WallOffset[i];
			}

		pWallCoef->CompVelocity = ConfigRec.UtRec.CompVelocity;
		pWallCoef->ShearVelocity = ConfigRec.UtRec.ShearVelocity;

		break;


	case SET_WALL_COEFS_RAW:
		// The slave udp picks out the coef's which apply to it
		// Results in raw data being sent to mmi for cal dlg
		pWallCoef = (WALL_COEF *) &pMsg->CmdBuf[0];
		for ( i = 0; i < 4; i++)
			{
			pWallCoef->fWallSlope[i] = 1.0f;
			pWallCoef->WallOffset[i] = 0;
			}

		pWallCoef->CompVelocity = 0.23f;
		pWallCoef->ShearVelocity = 0.115f;

		break;

	case SITE_SPECIFIC_MSG:
		// Retrieve site specific data from registry and down load to upd
		pCTscanDlg->GetRegistrySiteDefaults();

		pSiteDef = (SITE_SPECIFIC_DEFAULTS *) &pMsg->CmdBuf[0];

		pSiteDef->nDefaultClockOffset = SiteDefault.nDefaultClockOffset;
		pSiteDef->nDefaultXOffset = SiteDefault.nDefaultXOffset;
		pSiteDef->nDefaultWOffset = SiteDefault.nDefaultWOffset;
		break;


	case STORE_JOBREC_MSG:
		// Retrieve site specific data from registry and down load to upd
		i = sizeof(JOB_REC);
		memcpy ( (void *) &pMsg->CmdBuf[0], (void *) &ConfigRec.JobRec, 
				sizeof(JOB_REC));

		break;


	default:
		return FALSE;
		}	// switch on msg number

	IpxSndBuf.len = sizeof(MMI_CMD);
	pMsg->MsgLen = sizeof(MMI_CMD);
	pMsg->MsgNum = wPacketNum++;

	IpxSndBuf.buf = (char*) &SndBuf[nBufIndx];

	pIpxAddrThere = &SockCmndAddr;
	flagtx = 0;

	rv = WSASendTo
			( sktIpxCmnd, &IpxSndBuf, 1,      //  Socket, buffer array, buffer count
			&ntx,  flagtx,                  //  Bytes sent, flags
			(SOCKADDR*)pIpxAddrThere,     //  Destination addr
			sizeof(SOCKADDR_IPX),
			NULL, NULL
			);

	nBufIndx++;
	IpxStat.TotalSentPackets++;
/*	if (rv == SOCKET_ERROR)
		{
		rv = WSAGetLastError();
		if (rv == WSAEWOULDBLOCK)  //  Leave msg in the buffer
			{
//	         GetDlgItem(IDC_STATUSTX)->SetWindowText("Wait to Tx Data");
			}
		else 
			{
	s.Format(_T("\r\n Socket - %d",sktIpxCmnd);
	AfxMessageBox(s);
			s = "SendMsg ";
	        s += GetWSAErrStr(rv);
			::MessageBox(NULL,s,NULL,MB_OK);
			IpxSndBuf.buf = NULL;
			}

		}   //  End if (SendTo() == SOCKET_ERROR)
*/
	if ( !bInit )
		Sleep(1);
	return TRUE;
	
	}   //  End SendMsg() // 01-10-00

#endif //#define TWO_CHANNEL_4_GATE_BOARD

/******************************************************************/





/******************************************************************/

void CTscanDlg::StatScreenToConfig()
	{	// Transfer Screen info into ConfigRec

	UpdateData(TRUE);	// Copy  screen TO variables
	_tcscpy_s(ConfigRec.JobRec.Grade, 9, m_szStat_Grade);
	//strncpy(ConfigRec.JobRec.Grade, m_szStat_Grade,9);
	ConfigRec.JobRec.OD = (float) _tstof(m_szStat_Od);
	ConfigRec.JobRec.Wall = (float) _tstof(m_szStat_Wall);
	// if entered in metric, scale  to inches
	if (ConfigRec.bEnglishMetric)
		{
		ConfigRec.JobRec.OD /= 25.4f;
		ConfigRec.JobRec.Wall /= 25.4f;
		}

	ConfigRec.UtRec.TholdWallThds[0] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[0]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	ConfigRec.UtRec.TholdWallThds[1] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[1]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	// wall in thousandths from % depth * wall in inches
	}

void CTscanDlg::ConfigToStatScreen()
	{	// Transfer ConfigRec to TscanDlg status screen
	char s[80];
	m_szStat_Grade = ConfigRec.JobRec.Grade;
	if (ConfigRec.JobRec.OD < 1.5f) ConfigRec.JobRec.OD = 1.5f;
	if (ConfigRec.JobRec.OD > 30.0f) ConfigRec.JobRec.OD = 30.0f;

	if (ConfigRec.JobRec.Wall < 0.1f) ConfigRec.JobRec.Wall = 0.1f;
	if (ConfigRec.JobRec.Wall > 2.750f) ConfigRec.JobRec.Wall = 2.750f;
	
	if (ConfigRec.bEnglishMetric & 1)
		{
		_gcvt(ConfigRec.JobRec.OD * 25.4f ,5, s);
		m_szStat_Od = s;
		_gcvt(ConfigRec.JobRec.Wall * 25.4f ,6, s);
		m_szStat_Wall = s;
		}
	else
		{
		_gcvt(ConfigRec.JobRec.OD,5, s);
		m_szStat_Od = s;
		_gcvt(ConfigRec.JobRec.Wall,6, s);
		m_szStat_Wall = s;
		}

	UpdateData(FALSE);	// Copy variables to screen

	ConfigRec.UtRec.TholdWallThds[0] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[0]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	ConfigRec.UtRec.TholdWallThds[1] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[1]) *
													ConfigRec.JobRec.Wall * 10.0f + 0.5f);
	// wall in thousandths from % depth * wall in inches
	}

	
/******************************************************************/

float MMToInch (float f)
	{
	/*
	Wall and diameter are held internally in inches.  These two
	routines convert wall and diameter to millimeters when necessitated by
	the EnglishMetric flag being set to Metric.
	When outputting info to the screen or the PC104, call MMToInch.  
	When inputting info from the keyboard, call InchToMM.
	*/

	if ( ConfigRec.bEnglishMetric != ENGLISH)
		f /=  25.4f;
	return f;
	
	}

float InchToMM (float f)
	{
	if ( ConfigRec.bEnglishMetric != ENGLISH) f *= 25.4f;
	return f; 
	}

/////////////////////////////////////////////////////////////////////////////
// CTscanDlg message handlers

// Start the system which locates the SysCp and establishes communication with it.
// The SysCp is a server and this program will be a client to the SysCp
void CTscanDlg::InitializeClientConnectionManagement(void)
	{
	int i;
#if 1
	// Instantiate all CCM instances for as many client connections as are going to be supported
	// Convention is for case 0 to be SysCp connection since all connect to SysCp
	// Convention is for case 1 to be connetion to the database-- no handled by odbc
	// Convention is for case 2 to be connection to the GDP -- no handled by odbc
	// Convention is for case 3 to be connection to the PLC -- no handled with clives class
	// 	All these connections are tcp/ip clients and the other end is a tcp/ip server.
	CString sClientIP,  sServerIp;

	for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	continue;	// instance already exists

		switch (i)
			{
		case 0:		// assume connecting to scp

			if ( 0 == FindClientSideIP(i) )
				{	// find client side connection for 1st connection... probably the same IP for all 
					// client side connections unless more than one NIC
				break;
				}
			FindServerSideIP(i);
		
			// Make a specific child class of CCM to handle the SysCp
			pCCM_SysCp = (CCCM_SysCp *)new CCCM_SysCp(i, PHASE_ARRAY_WALL_COMMAND_PROCESSOR);	// ptr to the class, not the connection structure
			if (NULL == pCCM_SysCp)
				{
				TRACE1("pCCM_SysCp[%d] is NULL\n", i);
				break;
				}
			if (pCCM_SysCp->m_pstCCM == NULL) 
				{
				TRACE1("pCCM_SysCp[%d]->m_pstCCM is NULL\n", i);
				break;
				}

			pCCM_SysCp->UniqueProc();
			pCCM_SysCp->SetSocketName(_T("CCM_SysCp"));

			if (pCCM_SysCp->m_pstCCM->sClientIP4.GetLength() > 6)	// copy found IP for client into identity struct
				pCCM_SysCp->SetClientIp(pCCM_SysCp->m_pstCCM->sClientIP4);
			if (pCCM_SysCp->m_pstCCM->sServerIP4.GetLength() > 6)	// copy found IP for server into identity struct
				pCCM_SysCp->SetServerIp(pCCM_SysCp->m_pstCCM->sServerIP4);

			// Set thread priorities for send and receive threads
			pCCM_SysCp->m_pstCCM->nReceivePriority	= THREAD_PRIORITY_ABOVE_NORMAL;
			pCCM_SysCp->m_pstCCM->nSendPriority		= THREAD_PRIORITY_BELOW_NORMAL;
			pCCM_SysCp->CreateReceiveThread();		Sleep(100);
			pCCM_SysCp->CreateSendThread();			Sleep(100);
			pCCM_SysCp->InitReceiveThread();
			pCCM_SysCp->InitSendThread();
			break;	// case 0


		case 1:		// assume connectiong to data base
			// FindClientSideIP(i);	// assume syscp found the ip for this client for all other servers. If not
			// craft CODE in FindClientSideIP to find another ip address to link with the database.
			if (stCCM[0].sClientIP4.GetLength() > 6)
				sClientIP = stCCM[0].sClientIP4;	// use case 0 for syscp
			else
				{	// try something else. If that fails, abort since we can't hook up with the data base
				}

			if (!FindServerSideIP(i))
				{
				TRACE("Could not find server IP for Database.. we are toast\n");
				break;
				}
			sServerIp = stCCM[i].sServerIP4;


			break;	// case 1


		default:
			pCCM[i] = NULL;
			break;

			}
		}	// for ( i

#endif	
	}

// return 0 if unable to resolve IP address of client side
int CTscanDlg::FindClientSideIP(int nWhichConnection)
{
	// Since we assume the main dlg knows what servers it will connect with
	// read thru the static stSocketNames and init the larger stCCM structure.
	// if we have input non-null strings for client and server end address, attempt to 
	// use them to create the connections to the servers.

	// for testing to begin with (6-26-12) we will derive the first nic address from 
	// looking up the machine name and get the 2nd (server side ip) from its url.
	// Since we don't have a real syscp, this will fall back to our local Syscp 127.0.0.1

	CString ClientIP = _T("");
	UINT ClientPort = 0;
	CString stmp;
	int nReturn = 0;	// assume failure

	 	// get the IP of the test client
    AddDebugMessage(_T("Resolving client IP...."));
	stmp = GetIPv4();

	// if test client IP was resolved,
	// returned string will not be null
    if ( (stmp.IsEmpty()) || (nWhichConnection >= MAX_CLIENTS) )
		{
		//sockerr = SOCKET_ERROR;
		AddDebugMessage(_T("Could not resolve SysCP client IP."));
		return nReturn;
		}
	else 
		{
		AddDebugMessage(_T("Resolved SysCP client IP - ") + stmp);
		stCCM[nWhichConnection].sClientIP4 = stmp;	// ccm class does not exist yet
			
		nReturn = 1;
		return nReturn;
		}
}
// return 0 if unable to resolve IP address of server side
// Convention is for case 0 to be SysCp connection since all connect to SysCp
// Convention is for case 1 to be connetion to the database
// Convention is for case 2 to be connection to the GDP
// Convention is for case 3 to be connection to the PLC

int CTscanDlg::FindServerSideIP(int nWhichConnection)
	{
	// Since we assume the main dlg knows what servers it will connect with
	// read thru the static stSocketNames and init the larger stCCM structure.
	// if we have input non-null strings for client and server end address, attempt to 
	// use them to create the connections to the servers.

	// for testing to begin with (6-26-12) we will derive the first nic address from 
	// looking up the machine name and get the 2nd (server side ip) from its url.
	// Since we don't have a real syscp, this will fall back to our local Syscp 127.0.0.1

	CString ServerIP = _T("");
	UINT ServerPort = 0;
	CString stmp;
	int nReturn = 0;	// assume failure

	switch (nWhichConnection)
		{

	case 0:		// SysCp by convention

		AddDebugMessage(_T("Resolving SysCP server IP...."));
 		stmp = GetIPv4(MC_SCP_HOSTNAME);

		// if first attempt failed
		// try LOCAL_HOST which should
		// always return 127.0.0.1 .. add localhost to file C:\Windows\System32\drivers\etc\hosts
		// # localhost name resolution is handled within DNS itself.
		// #	127.0.0.1       localhost
		// #	::1             localhost

		if (stmp.IsEmpty())
			{	stmp = GetIPv4(LOCAL_HOST);		}

	// if server IP was resolved,
	// returned string will not be NULL
    if (stmp.IsEmpty())
		{	AddDebugMessage(_T("Could not resolve SysCP server IP."));		return nReturn;		} 
	else 
		{    
		AddDebugMessage(_T("Resolved SysCP server IP - ") + stmp);
		stCCM[nWhichConnection].sServerIP4 = stmp;
		return (nReturn = 1);
		}

	case 1:		// database by convention MC_DBS_HOSTNAME
		AddDebugMessage(_T("Resolving Database server IP...."));
 		stmp = GetIPv4(MC_DBS_HOSTNAME);
		if (stmp.IsEmpty())
			{	AddDebugMessage(_T("Could not resolve Database server IP."));		return nReturn;		} 
		else 
			{    
			AddDebugMessage(_T("Resolved Database server IP - ") + stmp);
			stCCM[nWhichConnection].sServerIP4 = stmp;
			return (nReturn = 1);
			}		

	default:
		stmp.Format(_T("Failed to Resolve server IP for connection %d"),nWhichConnection);
		AddDebugMessage(stmp);
		stmp += _T("\n");
		TRACE(stmp);
		break;
		}	// end switch ()
	return nReturn;
	}

CString CTscanDlg::GetIPv4(void)
{
	// use this version of GetIPv4 when
	// you want to find out what the
	// IP is for the machine THIS
	// application is running on

	USES_CONVERSION;

	TCHAR sFQDN [MAX_FQDN_LENGTH + 1];		// fully qualified domain name string
	DWORD dwFQDN = MAX_FQDN_LENGTH + 1;		// buffer for sFQDN

	// because we are running a system using DHCP, DNS,
	// and dynamic IP addresses, we have no prior knowledge
	// about any given host's IP address so we have to
	// fish for it

	// find out where we are
	CString sComputerName = _T("");		// FQDN of this host
	CString theIP = _T("");				// the stringized ip

    ::GetComputerNameEx(ComputerNameDnsFullyQualified,
		                (LPTSTR) sFQDN, 
		                &dwFQDN);

	sComputerName = W2A(sFQDN);


	// make sure we're searching for something real
    if (sComputerName.IsEmpty())
		return theIP;

    struct sockaddr_in theHost;	// structure used in the conversion
	HOSTENT * hostent;			// structure returned by gethostbyname

    // do the lookup
	hostent = gethostbyname(W2A(sComputerName));

	// make sure we found something
	if (hostent != NULL)
	{
	   // save the important bits
	   // note, if the host is a multi-homed
	   // then hostent->h_address_list WILL CONTAIN
	   // MORE THAN ONE IP. this method currently
	   // assumes that the host is NOT multi-homed
       memcpy(&theHost.sin_addr, 
		      hostent->h_addr_list[0], 
			  hostent->h_length);

	   // convert to IP address
	   theIP = inet_ntoa((IN_ADDR) theHost.sin_addr);
	} else {
	   theIP.Empty();
	}

	// return something to the caller
	// either a null string or the
	// actual IP
	return theIP;
}

CString CTscanDlg::GetIPv4(CString sComputerName)
{
	// use this version of GetIPv4 when
	// you want to determine the IP
	// of a machine OTHER than the
	// one this application is running
	// on.  example GetIPv4(_T("mc-dbs"))
	// will return the IP of the computer
	// named "mc-dbs"
	USES_CONVERSION;

	// because we are running a system using DHCP, DNS,
	// and dynamic IP addresses, we have no prior knowledge
	// about any given host's IP address so we have to
	// fish for it

	CString theIP = _T("");				// the stringized ip
    CString stmp = _T("");				// temporary string holder
	stmp = W2A(sComputerName);

	// make sure we're searching for something real
    if (stmp.IsEmpty())
		return theIP;		// theIP is an empty string here

    struct sockaddr_in theHost;	// structure used in the conversion
	HOSTENT * hostent;			// structure returned by gethostbyname

    // do the lookup
	hostent = gethostbyname(W2A(stmp));

	// make sure we found something
	if (hostent != NULL)
	{
	   // save the important bits
	   // note, if the host is a multi-homed
	   // then hostent->h_address_list WILL CONTAIN
	   // MORE THAN ONE IP. this method currently
	   // assumes that the host is NOT multi-homed
       memcpy(&theHost.sin_addr, 
		      hostent->h_addr_list[0], 
			  hostent->h_length);

	   // convert to IP address
	   theIP = inet_ntoa((IN_ADDR) theHost.sin_addr);
	} else {
	   theIP.Empty();
	} //if

	// return something to the caller,
	// either a null string or the
	// actual IP
	return theIP;
}

// Initialize the Server subsystems of the Phased Array GUI.
// This windows program receives inspection data from the Phased Array Master(s) of which there
// may be several. In one mode of operation, it sends data to a wall bar display program which is
// a client program on another port.
void CTscanDlg::InitializeServerConnectionManagement(void)
	{
	int i;
	CString s;
	UINT uPort;
	for ( i = 0; i < MAX_SERVERS; i++)
		{

		if (pSCM[i])	continue;	// instance already exists

		switch (i)
			{
		case 0:		// assume connecting phased array Master(s) -- may be multiple masters all listening at port xxx
			pSCM[i] = new CServerConnectionManagement(i);
			if (pSCM[i])
				{
				s = gServerArray[i].Ip;			// a global static table of ip addresses
				pSCM[i]->SetServerIP4(s);		// _T("192.168.10.10"));
				uPort = gServerArray[i].uPort;
				pSCM[i]->SetServerPort(uPort);	// 7501);
				pSCM[i]->SetServerType(ePhaseArrayMaster);
				// start the listen thread which will create a listener socket
				// the listener socket's OnAccept() function will create the connection thread, dialog and socket
				// the connection socket's OnReceive will populate the input data linked list and post messages
				// to the main dlg/application to process the data.
				pSCM[i]->StartListenerThread(i);
				}
			break;
		default:
			pSCM[i] = NULL;
			break;
			}
		}
	}


BOOL CTscanDlg::OnInitDialog()
	{
//	WSADATA wsaData;
	char ch[300];
	int rv = 0;
	int i;
//	short *nStart;
	CString s;	
//	CWaitCursor wait;
	CDialog::OnInitDialog();

	//GetMyWindowPosition();

//	m_pTcpThreadRxList = new CTcpThreadRxList;

	gCopyTo = 0;
	hTscanDlg = m_hWnd;
	m_ptheApp = (CTscanApp *) AfxGetApp();
	_getcwd(m_szHelpFile,128);	// what directory does this program start in
	_getcwd(ch, 255);
	s = ch;
	s += _T("\\ScopeDmx.cal");
	CstringToTChar(s,gszScopeDmxCalFile);

	strcat(m_szHelpFile,"\\help\\Truscan.hlp");
	strcpy(gszHelpFile, m_szHelpFile);	// make help file name global
	
	HANDLE 	hThread		= AfxGetThread()->m_hThread;
	CWinThread *pThread	= AfxGetThread();
	TRACE3("TscanDlg = 0x%04x, handle= 0x%04x, ID=0x%04x\n", pThread, hThread, pThread->m_nThreadID);

	GetDefaultDirectories();	// default data and cfg directories

	// Install keyboard hook for alt+p screen print with dibapi32
	InstallHook(hTscanDlg, TRUE);
	// Define hot key alt+p for print screen
	// alt flag, ctl flag, shift flas, cap 'P' and return message
	if (!SetHotKey(TRUE, FALSE, FALSE, 0x50, WM_USER_PRINT_SCREEN) )
		MessageBox(_T("Hot Key for Screen Print Not Installed"));

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
		{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
			{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
/*			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_RESET_COEF, "Reset Coef");
			pSysMenu->AppendMenu(MF_STRING, IDM_RESET_SCALERS, "Reset Scalers");
*/			}
		}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

#if 1
	s.Format(_T("Sizeof ST_SERVERS_CLIENT_CONNECTION = %d\n"), sizeof(ST_SERVERS_CLIENT_CONNECTION));	// 82
	TRACE(s);
	s.Format(_T("Sizeof ST_SERVER_CONNECTION_MANAGEMENT = %d\n"), sizeof(ST_SERVER_CONNECTION_MANAGEMENT));	// 80
	TRACE(s);
	s.Format(_T("Sizeof stSCM = %d\n"), sizeof(stSCM));	// 160
	TRACE(s);	
#endif


	// move to here 01-Aug-2012 from UpdateTitleBar();
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_MASTER), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE1), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE2), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE3), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE4), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE5), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE6), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE7), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE8), _T("Connection not established."));
		m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SLAVE9), _T("Connection not established."));
	}

	// TODO: Add extra initialization here
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
 
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
 
	if ( osvi.dwMajorVersion == 6 )
		{
		  // windows 7
		m_iSysCPPort = MC_SYSCP_LISTENPORT_WIN7;
		theApp.m_iWinVer = 7;
		}
	else
		{
		  // windows XP
		m_iSysCPPort = MC_SYSCP_LISTENPORT;
		theApp.m_iWinVer = 0;
		}

	IpxStat.TotalRcvPackets = IpxStat.LostRcvPackets = IpxStat.DupRcvPackets= 0;
	m_nUdpRcvCnt = m_nUdpRcvLostCnt = m_nUdpRcvDupCnt = 0;

//	rv = WSAStartup(0x0202, &wsaData);   //  Must be 1st fcn call.. include afxsock.h and make socket init call in app InitInstance()

		InitializeClientConnectionManagement();	// moved from after thread list creation
		InitializeServerConnectionManagement();

#if 1
	if (rv)
	{ 
		//ErrMsgSockStartup(rv);
		MessageBox(_T("This program will not run without Winsock 2"));
		//PostMessage(WM_USER_KILL_APP,0,0);
		return TRUE;
		//AfxEndThread(0);
		//::PostQuitMessage(0);
	}

#if 1
	// This is the code we hope to replace with the CServerConnectionManagement class. Unfortunatley, the are a lot
	// of uses of m_pTcpThreadRxList in the code.
	// Disable the thread starts and just use the SendMsg functions suitably modified
	m_pTcpThreadRxList = NULL;
	m_pTcpThreadRxList = new CTcpThreadRxList;	// a global static pointer. Copied to all dialogs which need to access it.
	// The constructor listens for only one connection from the phased array master. If the master is not there when the constructor runs,
	// the connection is never made. Does put on screen message directing user to check ethernet connectors.
#endif

	//Set up WEB interface

#if 1
	m_nFpm=0;   // Speed starts at 0
	//05/01/00 Read total joint stats from registry
	GetJobTotals();

	// Attempt to load the scope demux setup file
	try
		{
		CFile file (gszScopeDmxCalFile, CFile::modeCreate | 
				CFile::modeNoTruncate |  CFile::modeReadWrite);	// create if not existing
																// else open existing
		file.Read(&DmxCalRec, sizeof(DmxCalRec));
		// when this procedure closes, it will close the open file
		DmxCalRec.nCmd = 1;		/* cmd = 1, initial download of structure from mmi to master udp */
		DmxCalRec.nState = 0;
		DmxCalRec.nReject[0] = DmxCalRec.nReject[1] = 4000;


		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		}

	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}

	m_uStatTimer = SetTimer(IDT_STAT_TIMER, 1000, NULL);
	if (!m_uStatTimer) MessageBox(_T("Failed to start STAT timer"));
	SendMsg(JOINT_NUMBER_MSG);		// Set Pipe Number in instrument
//	wait.Restore();

	m_bMetric = ConfigRec.bEnglishMetric & 1;
	// Force user to open a config file
	//OnFileOpen();
#endif
	// Set alarm mask to have all alarms unmasked
	for ( i = 0; i < 4; i++) wAlarmMask[i] = 0xffff;	// all alarms pass thru
#endif

	m_SBchnl.SetScrollRange(1,MAX_CHANNEL,TRUE);
	m_SBchnl.EnableScrollBar(ESB_ENABLE_BOTH);
	m_SBchnl.SetScrollPos(gChannel+1, TRUE);

	m_SBcpyto.SetScrollRange(1,MAX_CHANNEL+1,TRUE);
	m_SBcpyto.EnableScrollBar(ESB_ENABLE_BOTH);
	m_SBcpyto.SetScrollPos(gChannel+1, TRUE);

#if 1
	ChnlDisplay(gChannel);
	m_sCpyto = m_sActCh = m_szChnlDesc;

		// Set the title bar to tell us if we are using English or
		// metric units  now done in UpdateTitleBar
	
	UpdateChnlSelected();

	//TRACE("Size of config_rec = %d\n", sizeof(CONFIG_REC) );
#if ( (SYSTEM_ID != LORAINE_SYS) || (SYSTEM_SUB_ID != LORAINE_SYS) )
#if (SYSTEM_SUB_ID != BESSEMER_SYS)
			m_btnSlave2.ShowWindow(SW_HIDE);
			m_btnSlave3.ShowWindow(SW_HIDE);
			m_btnSlave4.ShowWindow(SW_HIDE);
#endif
#if (LOCATION_ID == BESSEMER)
			m_btnSlave4.ShowWindow(SW_HIDE);
#endif
			m_btnSlave5.ShowWindow(SW_HIDE);
			m_btnSlave6.ShowWindow(SW_HIDE);
			m_btnSlave7.ShowWindow(SW_HIDE);
			m_btnSlave8.ShowWindow(SW_HIDE);
			m_btnSlave9.ShowWindow(SW_HIDE);
#endif

	StatScreenToConfig();

	OnInstrumentScope();
#if (HIDE_ASCAN == 0) 
	OnInstrumentAscan();
#endif
	UpdateTitleBar();

	SetDefID(-1);

#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
	}

void CTscanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
		CAboutDlg dlgAbout;
//	char j ,i,ch;	
	switch (nID & 0xFFF0)
	{
	case IDM_ABOUTBOX:
		dlgAbout.DoModal();

		break;
	case IDM_RESET_COEF:	// UNREACHABLE

		break;
	case IDM_RESET_SCALERS:// UNREACHABLE

		break;
	default:
		break;
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CTscanDlg::OnDestroy()
	{
	//AfxMessageBox("OnDestroy");

	DestroyDataClients(TRUE);

	I_MSG_CAL *pImsg02;
	WinHelp(0L, HELP_QUIT);
	m_bInstAutoRun = FALSE;
	SendMsg(INST_AUTORUN);

	// Cleanup the ipx com stuff
	if (m_uStatTimer) KillTimer(m_uStatTimer);

	if (sktIpxCmnd != INVALID_SOCKET) closesocket(sktIpxCmnd);
	if (sktIpxData != INVALID_SOCKET) closesocket(sktIpxData);
	if (m_sktUdp != INVALID_SOCKET) closesocket(m_sktUdp);
	WSACleanup();  //  Free resources allocated by WSAStartup()	
	
	// Save job totals in registry
	SetJobTotals();
	
	while (!plistUtData.IsEmpty() )
			{
			pImsg02 = (I_MSG_CAL *)plistUtData.RemoveHead();
			delete pImsg02;
			}

	if (m_pTcpThreadRxList != NULL)
		delete m_pTcpThreadRxList;

	CDialog::OnDestroy();
	
	}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTscanDlg::OnPaint() 
	{
	if (IsIconic())
		{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		}
	else
	{
		if (m_bStartUp)
		{
			GetMyWindowPosition();
			m_bStartUp = FALSE;
		}
		CDialog::OnPaint();
	}

	//if (m_pInspectDlg1) m_pInspectDlg1->RefreshWindow();
	//if (m_pInspectDlg2) m_pInspectDlg2->RefreshWindow();

	ShowNetworkAlarm();
	}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTscanDlg::OnQueryDragIcon()
	{
	return (HCURSOR) m_hIcon;
	}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void CTscanDlg::ErrMsgSockNA(char *t)
	{ 
	CString s;

	s  = t;
	s += GetWSAError();
	s += _T("\nEthernet Comm Not Available");
	MessageBox(s);
	}   //  End ErrMsgSocket()  07-01-99

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void CTscanDlg::ErrMsgSock(char *t)
	{ 
	CString s;

	s  = t;
	s += GetWSAError();
	MessageBox(s);
	}   //  end ErrMsgSock()  06-07-99

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void CTscanDlg::ErrMsgSockStartup(int errval)
	{ 
	CString s = "Startup ";

	s += GetWSAErrStr(errval);
	s += _T("\nEthernet Comm Not Available");
	MessageBox(s);
	}   //  End ErrMsgSockStartup()  06-17-99

// Helpers from dos verison
	
/*********************************************************************/

/* The real version which is called is in IPXDLG.CPP */

LRESULT CTscanDlg::OnIpxRxRdy(WPARAM skt, LPARAM err_event)
	{
#if 0
	// Winsock2 notifies us that we have received data from the Tscan instrument

	static int nmsg = 0, nmtx = 0;
	static WORD wLastSeq = 0;
//	USHORT machineID;

	int errval, event;
	WSABUF WsaBuf[1];
	int rv;
	ULONG nrx, flagrx;           //  bytes sent, flags
//	ULONG ntx, flagtx;
	SOCKADDR_IPX strAddrThere;
	int nAddrThere;

	I_MSG_CAL	*pImsg02;			// msg02 is much bigger than msg 01.
	typedef struct
	{
	RECBUF	rb;
	BYTE	pad[20];
	}	BIGBUF;

  CString s;

  errval = err_event >> 16;
  if (errval)
	{ 
	s = _T("OnRx ");
    s += GetWSAErrStr(errval);
    MessageBox(s);
    return 1;
	}

  event = err_event & 0x00FF;
  if (event & FD_READ) while (1)
	{	// (event & FD_READ) while (1)
	WsaBuf[0].buf = (char*)(new BIGBUF);	// Select biggest possible I msg
    if (WsaBuf[0].buf == NULL)
		{ 
		MessageBox(_T("OOM"));
		return 1;
		}

    WsaBuf[0].len = sizeof(BIGBUF);
    flagrx = 0;
    nAddrThere = sizeof(strAddrThere);
    rv = WSARecvFrom
				( sktIpxData, WsaBuf, 1,   //  Socket, buffer array, buffer count
				&nrx, &flagrx,             //  Bytes received, flags
				(SOCKADDR*)&strAddrThere,  //  Sender return addr
				&nAddrThere,
				NULL,                      //  Overlap struct
				NULL                       //  Callback when would block
				);
    if (rv == SOCKET_ERROR)
		{ 
		rv = WSAGetLastError();
		if (rv != WSAEWOULDBLOCK)
			{ 
			s = _T("Rx ");
			s += GetWSAErrStr(rv);
			MessageBox(s);
			}
		delete WsaBuf[0].buf;
		return 0;
		}   //  End if (RecvFrom() == SOCKET_ERROR)
	
	else  //  (RecvFrom() != SOCKET_ERROR)
		{
		pImsg02 = (I_MSG_CAL*)WsaBuf[0].buf;
		IpxStat.TotalRcvPackets++;
		if ( wLastSeq == pImsg02->MstrHdr.MsgNum) IpxStat.DupRcvPackets++;
		else if (++wLastSeq != pImsg02->MstrHdr.MsgNum) IpxStat.LostRcvPackets++;
		wLastSeq = pImsg02->MstrHdr.MsgNum;

		// Get comm statistics from instrument header
		IpxStat.TotalSentPackets = pImsg02->MstrHdr.IdataStat.nSent;
		IpxStat.LostSentPackets = pImsg02->MstrHdr.IdataStat.nLost;
		IpxStat.DupSentPackets = pImsg02->MstrHdr.IdataStat.nDup;

		m_nRpm = gnTruscopePeriod;
		if (gnTruscopePeriod > 50)
				m_nFpm = VELOCITY_DX_FACTOR/gnTruscopePeriod; // at least 50 ms to go 4 inches
			else m_nFpm = 0;
//		m_nIps = pImsg02->InspHdr.motion_pulse_per_sec;
		m_nJointLength = pImsg02->InspHdr.JointLength;

		m_wLineStatus = pImsg02->InspHdr.wLineStatus;
		
		if (pImsg02->MstrHdr.MsgId  == 0)
			memcpy(&SockCmndAddr.sa_nodenum,
					&strAddrThere.sa_nodenum, 6);

		if ( !IsDataClient() )
			{	// No one there to consume packets... don't queue packet
			delete WsaBuf[0].buf;
			return 0;
			}

		plistUtData.AddTail( (void *) WsaBuf[0].buf);
		return 1;

     }   //  End if (RecvFrom() != SOCKET_ERROR)
  }   //  End if (event & FD_READ) while (1)

#endif
  return 0; 


	}   //  End OnIpxRxRdy()  07-28-99

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


LRESULT CTscanDlg::OnUdpRxRdy(WPARAM skt, LPARAM err_event)
{

	// Winsock2 notifies us that we have received data from the Tscan instrument

	static int nmsg = 0, nmtx = 0;
	static WORD wLastSeq = 0;
//	USHORT machineID;

	int errval, event;
	WSABUF WsaBuf[1];
	int rv;
	ULONG nrx, flagrx;           //  bytes sent, flags
//	ULONG ntx, flagtx;
	SOCKADDR_IN strAddrThere;
	int nAddrThere;

	UDPMSGBUF *pMsg;
	CString s;

  errval = err_event >> 16;
  if (errval)
	{ 
	s = _T("OnRx ");
    s += GetWSAErrStr(errval);
    MessageBox(s);
    return 1;
	}

  event = err_event & 0x00FF;
  if (event & FD_READ) while (1)
  {	// (event & FD_READ) while (1)
	WsaBuf[0].buf = (char*)(new UDPMSGBUF);	// Select biggest possible I msg
    if (WsaBuf[0].buf == NULL)
		{ 
		MessageBox(_T("OOM"));
		return 1;
		}

    WsaBuf[0].len = sizeof(UDPMSGBUF);
    flagrx = 0;
    nAddrThere = sizeof(strAddrThere);
    rv = WSARecvFrom
				( m_sktUdp, WsaBuf, 1,   //  Socket, buffer array, buffer count
				&nrx, &flagrx,             //  Bytes received, flags
				(SOCKADDR*)&strAddrThere,  //  Sender return addr
				&nAddrThere,
				NULL,                      //  Overlap struct
				NULL                       //  Callback when would block
				);
    if (rv == SOCKET_ERROR)
		{ 
		rv = WSAGetLastError();
		if (rv != WSAEWOULDBLOCK)
			{ 
			s = _T("Rx ");
			s += GetWSAErrStr(rv);
			MessageBox(s);
			}
		delete WsaBuf[0].buf;
		return 0;
		}   //  End if (RecvFrom() == SOCKET_ERROR)
	
	else  //  (RecvFrom() != SOCKET_ERROR)
	{
		pMsg = (UDPMSGBUF *)WsaBuf[0].buf;
		m_nUdpRcvCnt++;
		if ( wLastSeq == pMsg->Seq) m_nUdpRcvDupCnt++;
		else if (++wLastSeq != pMsg->Seq) m_nUdpRcvLostCnt++;
		wLastSeq = pMsg->Seq;

#if 0
		// TESTING ... echo back to mwt
		flagrx = 0;
		rv = WSASendTo
			( m_sktUdp, &WsaBuf[0], 1,      //  Socket, buffer array, buffer count
			&nrx,  flagrx,                  //  Bytes sent, flags
			(SOCKADDR*)&strAddrThere,  //  Sender return addr
			sizeof(SOCKADDR_IN),
			NULL, NULL
			);

		// Get comm statistics from instrument header
		IpxStat.TotalSentPackets = pImsg02->InspHdr.TotalCmnds;
		IpxStat.LostSentPackets = pImsg02->InspHdr.PacketsLost;
		IpxStat.DupSentPackets = pImsg02->InspHdr.PacketsDup;

		m_nRpm = pImsg02->InspHdr.Rpm;
		if (gnTruscopePeriod > 50)
				m_nFpm = VELOCITY_DX_FACTOR/gnTruscopePeriod; // at least 50 ms to go 4 inches
			else m_nFpm = 0;
//		m_nIps = pImsg02->InspHdr.motion_pulse_per_sec;
		m_nJointLength = pImsg02->InspHdr.JointLength;


		
		if (pImsg02->MsgID  == 0)
			memcpy(&SockCmndAddr.sa_nodenum,
					&strAddrThere.sa_nodenum, 6);


		if ( !IsDataClient() )
			{	// No one there to consume packets... don't queue packet
			delete WsaBuf[0].buf;
			return 0;
			}
#endif

//		plistUdpIn.AddTail( (void *) WsaBuf[0].buf);
		memcpy( (void *) &UdpMsg99, (void *) pMsg, sizeof(U_MSG_99));
		if ( nrx > 5) UdpMsg99.txt[nrx-6] = 0;
		else UdpMsg99.txt[0] = 0;
		return 0;

    }   //  End if (RecvFrom() != SOCKET_ERROR)
  
  }   //  End if (event & FD_READ) while (1)

  return 0; 

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool CTscanDlg::UpdateTimeDate(time_t *tNow)
	{
	struct tm *today;
	CString t;
	int i;	//,j;
	WORD wMask;

//	time(tNow);

	/* Convert to time structure  */
	today = localtime( tNow );
	if (today ==0) return TRUE;
	{
	t.Format(_T("%02d:%02d:%02d"), today->tm_hour, today->tm_min, today->tm_sec);
	SetDlgItemText(IDC_STAT_TIME, t);
	t.Format(_T("%02d/%02d/%02d"), today->tm_mon+1, today->tm_mday, (today->tm_year % 100) );
	SetDlgItemText(IDC_STAT_DATE, t);
	}
#if 0
	// debugging ipx thread
	t.Format(_T("%d"), nLoop);
	SetDlgItemText(IDC_QUES_LENGTH_EDIT,t);
#endif

	// Check to see any unmasked alarms are set
	// If unmasked alarms, paint Alarms static text red
	bShowAlarmWarning = 0;
	nShowAlarmWarningCnt++;
	for (i = 0; i < 16; i++)
		{
		wMask = (1 << i);
		if ( wMask & gwUdpStatus[1] & wAlarmMask[0])
			{
			if (nShowAlarmWarningCnt & 1)	bShowAlarmWarning = 1;	// implement flashing
			break;
			}
		}
	// when the text is drawn, onctlcolor intercepts before the draw and picks the color
    return FALSE;
	}


void CTscanDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	// If no other timers running
	// Have to check inspect ptr in case in play back mode which
	// shows up as no data client
	m_nTimerCount++;
	if ((m_nTimerCount & 3) == 0)
		{
		SetNetworkAlarm();
		//m_nTimerCount = 0;
		}
	if (m_nTimerCount == 10)
		if (pSCM[0])
			{
#if 0
			pSCM[0]->ServerShutDown(0);
			pSCM[0]->StopListenerThread(0);
			if (pSCM[0]->m_pstSCM->pServerListenThread)
				{
				CWinThread * pThread = (CWinThread *) pSCM[0]->m_pstSCM->pServerListenThread;
				PostThreadMessage(pThread->m_nThreadID,WM_QUIT, 0L, 0L);
				pSCM[0]->m_pstSCM->pServerListenThread = NULL;
				}
#endif
			}


	UpdateLineStatus();
	UpdateNextJointNum();
	UpdateRpm();

	if ( !IsDataClient() )
		if ( m_pInspectDlg1 == NULL)
		{
			time(&m_tTimeNow);
			UpdateTimeDate( &m_tTimeNow);
		}

	if (m_pAscanDlg == NULL)
	{
		I_MSG_RUN *pImsgRun;
		g_CriticalSectionAscan.Lock();
		while (!g_pTcpListAscan.IsEmpty() )
		{
			pImsgRun = (I_MSG_RUN *) g_pTcpListAscan.RemoveHead();
			delete pImsgRun;
		}
		g_CriticalSectionAscan.Unlock();
	}

	SetDlgItemInt(IDC_EDIT_PACKETNUM, m_nPacketNumber);
	SetDlgItemInt(IDC_EDIT_MSTRSENTMSG, m_nMstrSentMsg);

	CDialog::OnTimer(nIDEvent);
}


void CTscanDlg::OnDisplayInspect() 
{
	// TODO: Add your command handler code here

	if (m_pInspectDlg1 == NULL)
	{
		DestroyDataClients(TRUE);		// Kill all other data consumers
		m_pInspectDlg1 = new CInspect(NULL, 0);

		m_pInspectDlg1->m_pTcpThreadRxList = m_pTcpThreadRxList;
		if (m_pInspectDlg1) m_pInspectDlg1->Create(IDD_INSPECT);

	}

#if ( (SYSTEM_ID == LORAINE_SYS) && (SYSTEM_SUB_ID == LORAINE_SYS) )
	if (m_pInspectDlg2 == NULL)
	{
		m_pInspectDlg2 = new CInspect(NULL, 1);
		if (m_pInspectDlg2) m_pInspectDlg2->Create(IDD_INSPECT);

		m_pInspectDlg2->m_pTcpThreadRxList = m_pTcpThreadRxList;
	}
#endif
}

void CTscanDlg::OnDisplayJob() 
	{
	// TODO: Add your command handler code here
	if (CWcJob::m_pDlg == NULL)
		{
		CWcJob::m_pDlg = new CWcJob;
		if (CWcJob::m_pDlg) CWcJob::m_pDlg->Create(IDD_CUSTOMER);
		}
	}

void CTscanDlg::OnDisplayPacketdata() 
	{
	// TODO: Add your command handler code here
	// Catch the ipx data from the instrument and display in tabular
	// form on the dialog

	if (CCatch::m_pDlg == NULL)
		{
		if (!DestroyDataClients(FALSE))
			return;		// Kill all other data consumers
		CCatch::m_pDlg = new CCatch;
		if (CCatch::m_pDlg) CCatch::m_pDlg->Create(IDD_CATCH);
		}
	
	}

void CTscanDlg::OnFileExit() 
	{
	// TODO: Add your command handler code here
	DestroyDataClients(TRUE);
	OnCancel();
	}

void CTscanDlg::OnFileOpen() 
	{
	// TODO: Add your command handler code here
	// debug
	if (pSCM[0])
		pSCM[0]->DoNothing();

	if (m_pInspectDlg1)
	{
		if (m_pInspectDlg1->m_nControlSel != INSPECT_STOP)
		{
			AfxMessageBox(_T("You must set Inspect Control to Stop before you can do this operation!"));
			return;
		}
	}

	int nResult;
	CString s;
	int iChannel = gChannel;

	TCHAR szFilter[] = _T("Config Files(*.cfg) | *.cfg||");


	CFileDialog dlg (TRUE, _T("cfg"), _T("*.cfg"),OFN_FILEMUSTEXIST, 
				 szFilter);

	if (m_szDefCfgDir.GetLength()) dlg.m_ofn.lpstrInitialDir = m_szDefCfgDir;

	if ( dlg.DoModal() == IDOK)
	{
		m_bInstAutoRun = FALSE;	// stop instrument while reconfiguring
		SendMsg(INST_AUTORUN);

		s = dlg.GetPathName();
		try
			{
			CFile file (s, CFile::modeReadWrite| CFile::shareDenyNone);
			file.Read(&ConfigRec, sizeof(ConfigRec));
			// when this procedure closes, it will close the open file
			m_szConfigFileName = s;
			CstringToTChar(s,ConfigRec.JobRec.CfgFile);

			UpdateUndoBuffer2();
			}
		catch (CFileException* e)
			{
			e->ReportError();
			e->Delete();
			}

		// Make a reference copy of ConfigRec in order to detect changes
		// between the file and the working copy
		memcpy( (void *) &ConfigRecFile, (void *) &ConfigRec, sizeof(CONFIG_REC));
		
		// Test data just read for reasonable values
		QualifyConfigRec();
		//		ConfigRec.JobRec.WallStatFlag = 0;	// until wall options resolved
		
		//		SetWindowTitle();
		
		ConfigToStatScreen();
		UpdateData(FALSE);	// Copy variables to screen
		
		m_pTcpThreadRxList->SendSlaveMsg(LOAD_CONFIG_FILE, 0,0,0,0,0,0);
		
		CWaitCursor waitCursor;
		
		SendMsg(CHANNEL_CONFIG_MSG);
#if 1
		
		SendMsg(SET_ALL_THOLDS);	// all tholds
		SendMsg(SET_NC_NX);
		SendMsg(SET_WALL_COEFS);
		SendMsg(SITE_SPECIFIC_MSG);
#endif
		
#if 0
		// Try sending individual msg's instead of one big one
		SendMsg(SET_ALL_CHNL_TYPES);
		SendMsg(SET_SCOPE_SELECT);		// set scope mux's
		SendMsg(SET_DROPOUT);
		SendMsg(SET_NO_CLOCK);	// Is TOP sensor good or bad?
#endif
		//send config data, no demux, default channel 
		
#if 0
		system_init(); // global c function replaced by 
#else
		SystemInit();	// member of this class
#endif
		
		SendMsg(STORE_JOBREC_MSG);	// transfer job rec to master udp
		
		// if inspect screen already up, change title bar
		if ( m_pInspectDlg1) m_pInspectDlg1->ShowDlgTitle();
		
		//::PostMessage(pCTscanDlg->m_hWnd,
		//					WM_USER_UPDATE_TITLEBAR,0,0);
		
		
		m_bInstAutoRun = TRUE;
		//	SendMsg(INST_AUTORUN);
        if (m_pInspectDlg1 )m_pInspectDlg1->OnInitDialog();
        if (m_pInspectDlg2 )m_pInspectDlg2->OnInitDialog();
		
		// restore original active channel
		gChannel = 3;
		ChangeActiveChannel();

		gChannel = iChannel;
		ChangeActiveChannel();
		
		SendMsg(SET_ASCAN_REGISTERS);
		SendMsg(ASCAN_REFRESH_RATE);
		SendMsg(ASCAN_BROADCAST);

		SendMsg(SET_ASCAN_REGISTERS);

		if (m_pAscanDlg)
			m_pAscanDlg->UpdateRefreshRate();

		m_bConfigLoaded = TRUE;

		// 04/28/2000 Reset totals if they are non zero
		if ( (m_lOKJnt) ||
			(m_lRJnt)  ||
			(m_lOKJnt) )
		{	// this is how we reset the totals
			s.Format(_T("OK = %d, Rejected = %d,  Questionable = %d\r\n\r\n"),
				m_lOKJnt, m_lRJnt, m_lQJnt);
			s += _T("Reset Totals?");
			nResult = MessageBox(s,_T("Configuration Notice"), MB_YESNO);
			if ( nResult == IDYES)
			{
				m_lOKJnt = m_lRJnt = m_lQJnt = 0;
				m_fTotalLen = m_fRLen = m_fQLen = 0.0f;
			}
		}
	}

	m_bMetric = ConfigRec.bEnglishMetric & 1;
	UpdateTitleBar();
}	// OnFileOpen() 

void CTscanDlg::OnFilePrintersetup() 
	{
	// TODO: Add your command handler code here
	
	}

void CTscanDlg::OnFilePrintscreen() 
	{
	// TODO: Add your command handler code here
//return;

	// structure like truscope PrintTruscopeScreen function
	// calls dibapi32.dll routines to create dib and print dib.


	CClientDC dcScreen(NULL);		// context of the whole screen
	CDC dcMem;						// memory device context for screen
	CBitmap bitmapScreen;			// bit map for the screen
	CBitmap *oldBitMap;				// used to clean up process

	HPALETTE hPal;

	HDIB hDIB;
	HDC hDC;						// handle of default printer


	int nPixelHeightScreen;			// how high is screen area in pixels
	int nPixelWidthScreen;
	int dy;						// initial y height
	int cxChar, cyChar;				// char size in pixels
	BOOL q;							// debug only
	int nDefects;				// defects left to print
//    int nPageCnt;               // number of pages printed
//    int nDefectsPrinted;        // how many printed on this page

	TEXTMETRIC tm;					// struct which hold characteristics of screen

	
	GetPrinter();		// get default printer resources
	hDC = m_dcPrn->m_hDC;

	dy = nPixelHeightScreen	= dcScreen.GetDeviceCaps(VERTRES);
	nPixelWidthScreen		= dcScreen.GetDeviceCaps(HORZRES);

	dcScreen.GetTextMetrics( &tm);
	cyChar = tm.tmHeight + tm.tmExternalLeading;
	cxChar = tm.tmAveCharWidth;

	// If we are about to print inspection screen, leave room for flaw text
	if (m_pInspectDlg1)
	{
		//if (m_pInspectDlg1->m_pFlawReport != NULL)
			//m_pInspectDlg1->m_pFlawReport->ShowWindow(SW_HIDE);
		nPixelHeightScreen += 28*cyChar;	// 27 rows of text..28 on 7/27/00
		nDefects = m_pInspectDlg1->GetTotalDefects();
	}

	q = bitmapScreen.CreateCompatibleBitmap(&dcScreen,nPixelWidthScreen,
						nPixelHeightScreen);	// make bitmap fit screen

	
	q = dcMem.CreateCompatibleDC(&dcScreen);	// make memory compatible
											// with the screen

	oldBitMap = dcMem.SelectObject(&bitmapScreen);		// put bit map into memory context
	q = dcMem.PatBlt(0, 0, nPixelWidthScreen, nPixelHeightScreen,
					WHITENESS);				// erase memory context
	// Copy screen to memory
	q = dcMem.BitBlt(0, 0, nPixelWidthScreen, dy,
					&dcScreen, 0, 0, SRCCOPY);

	// Copy in the text if it exists
	if (m_pInspectDlg1)
		{
		// A little snaky here.  I would like the data printing to
		// be based in the inspect dialog.  Therefore, call a routine
		// in the inspect class to empty the data queues.
//        nDefectsPrinted = m_pInspectDlg1->AddFlawTextToDib(&dcMem, cxChar, cyChar, dy, 1);
		}

	hPal = GetSystemPalette();	

	hDIB = BitmapToDIB( (HBITMAP) bitmapScreen, hPal);

	// Had to compile dibapi32.dll with mfc as project
	// Since using best fit, 20,20 (x,y scaling) not used
	q = PrintDIB(hDIB, PW_BESTFIT, 20,20,"Best Fit", hDC);

	DestroyDIB(hDIB);	// delete the dib returned by the dll

	// 04/25/01 Reuse pieces to print text only if still more
	// defects to print.  No graphics at top of page, only 
	// columns of flaw text
//    if (m_pInspectDlg1)
//    {
//        nDefects -= nDefectsPrinted;
//        if (nDefects < 0) nDefects = 0;
//        nPageCnt = 1;   // already printed inspect screen
//        while ( (nDefects) && (nPageCnt < 6) )
//        {
//            q = dcMem.PatBlt(0, 0, nPixelWidthScreen, nPixelHeightScreen,
//                    WHITENESS);             // erase memory context
//
//            dy = 0;     // no graphics, use whole context for text
//
//            nDefectsPrinted = m_pInspectDlg1->AddFlawTextToDib(&dcMem, cxChar, cyChar, dy, 0);  // 55 rows
//           hDIB = BitmapToDIB( (HBITMAP) bitmapScreen, hPal);
//            q = PrintDIB(hDIB, PW_BESTFIT, 20,20,"Best Fit", hDC);
//            DestroyDIB(hDIB);   // delete the dib returned by the dll
//            nPageCnt++;
//            nDefects -= nDefectsPrinted;
//            if (nDefects < 0) nDefects = 0;
//        }
//
//        m_pInspectDlg1->RestoreFlawLists();
//        //if (m_pInspectDlg1->m_pFlawReport != NULL)
//            //m_pInspectDlg1->m_pFlawReport->ShowWindow(SW_SHOW);
//    }
//
	// 12/11/00 try to stop case of using up all memory in system
	DeleteObject(hPal);	// delete the palette
	DestroyDIB(hDIB);	// delete the dip returned by the dll
	dcMem.SelectObject(oldBitMap);	// deselect bitmap before deleting
	bitmapScreen.DeleteObject();	// delete screen bit map
	dcMem.DeleteDC();			// delete memory dc
	ReleasePrinter();
	
	}

void CTscanDlg::OnFileSave() 
{
	if (m_pInspectDlg1)
	{
		if (m_pInspectDlg1->m_nControlSel == INSPECT_PLAY)
		{
			AfxMessageBox(_T("You can't save cofiguration during inspection replay."));
			return;
		}
	}

	// TODO: Add your command handler code here
	CString s;

	s = _T("Do you really want to update the file ");
	s += m_szConfigFileName;
	s += _T("?");

	if ( MessageBox(s, _T("Update Configuration"),MB_YESNO)== IDNO )
		return;

	StatScreenToConfig();

	try
		{
		CFile file (m_szConfigFileName, 
			CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
		file.Write(&ConfigRec, sizeof(ConfigRec));
		// when this procedure closes, it will close the open file
		/*
		s = _T("The file ");
		s += m_szConfigFileName;
		s += _T(" has been updated");
		MessageBox(s);
		*/
		memcpy( (void *) &ConfigRecFile, (void *) &ConfigRec, sizeof(CONFIG_REC));
		UpdateUndoBuffer();
		}
	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
}

void CTscanDlg::OnFileSaveas() 
{
	if (m_pInspectDlg1)
	{
		if (m_pInspectDlg1->m_nControlSel == INSPECT_PLAY)
		{
			AfxMessageBox(_T("You can't save cofiguration during inspection replay."));
			return;
		}
	}

	// TODO: Add your command handler code here

	StatScreenToConfig();

	TCHAR szFilter[] = _T("Config Files(*.cfg) | *.cfg||");

	CFileDialog dlg (FALSE, _T("cfg"), _T("*.cfg"),OFN_OVERWRITEPROMPT, 
				 szFilter);

	if (_tcslen(m_szDefCfgDir)) dlg.m_ofn.lpstrInitialDir = m_szDefCfgDir;

	if ( dlg.DoModal() == IDOK)
		{
		CString s;
		s = dlg.GetPathName();
		try
			{
			CFile file (s, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
			file.Write(&ConfigRec, sizeof(ConfigRec));
			m_szConfigFileName = s;
			CstringToTChar(s,ConfigRec.JobRec.CfgFile);
			//strcpy (ConfigRec.JobRec.CfgFile, (LPCTSTR) s);
			// when this procedure closes, it will close the open file

			memcpy( (void *) &ConfigRecFile, (void *) &ConfigRec, sizeof(CONFIG_REC));
			UpdateUndoBuffer();

			UpdateTitleBar();
			if ( m_pInspectDlg1) m_pInspectDlg1->ShowDlgTitle();
			}
		catch (CFileException* e)
			{
			e->ReportError();
			e->Delete();
			}
		}
	
}

void CTscanDlg::FileSaveAs()
	{	// Public access to OnFileSaveas from other modules
	OnFileSaveas();
	}

void CTscanDlg::FileSave()
	{	// Public access to OnFileSave from other modules
	OnFileSave();
	}

void CTscanDlg::OnFileFreediskspace() 
	{
	// TODO: Add your command handler code here
	CDisk Dlg;
	Dlg.DoModal();
	
	}

void CTscanDlg::SaveConfigRec()	// public access to file save op
	{	// manually added to gain public access to save as from
		// other dialogs
	OnFileSaveas();
	}

void CTscanDlg::ShowBitmap(CPaintDC *pdc, CWnd *pWnd)
{
	// Convert the pointer to a pointer to the main dialog class
//	CGraphicsDlg *lpWnd = (CGraphicsDlg*)pWnd;
	CDC dcMem;
	CRect lRect;
	// Get the loaded bitmap
//	lpWnd->m_bBitmap.GetBitmap(&bm);
	// Create a device context to load the bitmap into
	dcMem.CreateCompatibleDC(pdc);
	// Select the bitmap into the compatible device context
	CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(this);
	// Get the display area available
	GetClientRect(lRect);
	lRect.NormalizeRect();
	// Copy and resize the bitmap to the dialog window
	pdc->BitBlt(10, 10, (lRect.Width() - 20), 
		(lRect.Height() - 20), &dcMem, 0, 0, SRCCOPY);
}

void CTscanDlg::OnHelpAbout() 
{
	// TODO: Add your command handler code here
	// copied from system menu command help added by wizard
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
/*	CDC* dc; // device context for painting
	CBitmap m_bBitmap;
	// TODO: Add your message handler code here
		HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(),
			"res\\setup.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
//		m_bBitmap.Attach(hBitmap);
		// Do we have a valid handle for the loaded image?
		if (hBitmap)
		{
			// Delete the current bitmap
			if (m_bBitmap.DeleteObject())
				// If there was a bitmap, detach it
				m_bBitmap.Detach();
			// Attach the currently loaded bitmap to the bitmap object
			m_bBitmap.Attach(hBitmap);
		}

	// Get a pointer to the parent window
//	CGraphicsDlg *pWnd = (CGraphicsDlg*)GetParent();
	// Do we have a valid pointer?
	BITMAP bm;
	CDC dcMem;
	CRect lRect;
	// Get the loaded bitmap
//	lpWnd->m_bBitmap.GetBitmap(&bm);
	// Create a device context to load the bitmap into
	dcMem.CreateCompatibleDC(dc);
	// Select the bitmap into the compatible device context
	CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(this);
	// Get the display area available
	GetClientRect(lRect);
	lRect.NormalizeRect();
	// Copy and resize the bitmap to the dialog window
	dc->BitBlt(0, 0, (lRect.Width()), 
		(lRect.Height() ), &dcMem, 0, 0, SRCCOPY);
/*	if (m_ptheApp)
	{
				// Display it
		ShowBitmap(&dc,(CWnd *) m_ptheApp);
	}

CPaintDC *pdc;

pdc->SelectObject(&hBitmap);
*/
}

void CTscanDlg::OnHelpContents() 
	{
	// TODO: Add your command handler code here
	CString s = m_szHelpFile;
	::WinHelp(m_hWnd, s,	HELP_INDEX,0);
	}

// Shut down the listener on every server to prevent new connections
void CTscanDlg::CloseListeningServerSockets(void)
	{
	int i;
	CString s;
	for ( i = 0; i < MAX_SERVERS; i++)
		{
		if (pSCM[i])
			{
			pSCM[i]->StopListenerThread(i);
			}
		}
	}




void CTscanDlg::OnCancel() 
	{
	// TODO: Add extra cleanup here
	if (MessageBox( _T("Do you really want to quit the Phased Array Wall program?"), _T("Program Shut Down"), 
				MB_YESNO) == IDNO)  return;

	int nAnswer, i;
	BYTE *pMem, *pFile;
	CString s;

#if 0
	for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	delete pCCM[i];
		pCCM[i] = NULL;
		}
	i = 14;
	s = _T("Here we are");
	if (pCCM_SysCp)
		{		delete pCCM_SysCp;	pCCM_SysCp = NULL;		}

	for ( i = 0; i < MAX_SERVERS; i++)
		{
		if (pSCM[i])	delete pSCM[i];
		pSCM[i] = NULL;
		}

	if (!DestroyDataClients(FALSE))
		return;
#endif

	//  Check to see if config file in memory different from config file on disk
	pMem = (BYTE *) &ConfigRec;
	pFile = (BYTE *) &ConfigRecFile;
	s = ConfigRec.JobRec.CfgFile;
    if (s.GetLength() != 0)
    {   for ( i = 0; i < sizeof(CONFIG_REC); i++)
		{
		if ( pMem[i] != pFile[i])
			{
            s.Format(_T("\nSave and Exit (Yes),  Exit (No),  Don't Exit (Cancel)"));
            nAnswer = MessageBox(s,     _T("WARNING -- Configuration file has changed !"),MB_YESNOCANCEL);

			switch (nAnswer)
				{
			case IDYES:
				SaveConfigRec();
				goto PROGRAM_EXIT;

			case IDNO:
				goto PROGRAM_EXIT;

			case IDCANCEL:
			default:
				return;		// Ignore quit command
				}
			}
		}
    }

PROGRAM_EXIT:
	if (!DestroyDataClients(FALSE))
		return;

	//CloseListeningServerSockets();

	SaveMyWindowPosition();

	if (pCTscanDlg->m_pScopeDlg) (pCTscanDlg->m_pScopeDlg)->KillMe();
	Sleep(10);
	CloseInstruments();
	Sleep(10);
	if (m_pTcpThreadRxList != NULL)
		{
		delete m_pTcpThreadRxList;
		m_pTcpThreadRxList = NULL;
		}

	// Always use same method for modeless windows
	if ( m_uStatTimer ) KillTimer(m_uStatTimer);
//	::SendMessage(hIpxDlg, WM_USER_KILL_IPX_THREAD,0,0);//	works
//	::Sleep(20);

		for ( i = 0; i < MAX_CLIENTS; i++)
		{
		if (pCCM[i])	delete pCCM[i];
		pCCM[i] = NULL;
		Sleep(10);
		}
	i = 14;
	s = _T("Here we are");
	if (pCCM_SysCp)
		{		delete pCCM_SysCp;	pCCM_SysCp = NULL;		}

	for ( i = 0; i < MAX_SERVERS; i++)
		{
		if (pSCM[i])
			{
			pSCM[i]->ServerShutDown(i);
			Sleep(200);
			delete pSCM[i];
			}
		pSCM[i] = NULL;
		Sleep(10);
		}

	CDialog::OnCancel();
//	CDialog::DestroyWindow();
	DestroyWindow();
	}

void CTscanDlg::SetJointNumber(DWORD JointNum)
	{	// Public access for Inspect to set joint number for run or
		// play back operations
	m_uJntNum = (UINT) JointNum;
	}


void CTscanDlg::OnSetupJointnumber() 
	{
	// TODO: Add your command handler code here
	// Pop up a dialog to allow the operator to set the joint number
	// On closing, sends new joint number to instrument

	CJointNum dlg;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
		{
		m_uJntNum = m_uNewJntNum;
		SendMsg(JOINT_NUMBER_MSG);	// Set pipe number in instrument
		//SendMsg(SET_ALL_THOLDS);		// Set all thresholds
		}
	else if (nResponse == IDCANCEL)
		{
		}

	}


void CTscanDlg::OnSetupScope() 
	{
	// TODO: Add your command handler code here
	// Make sure pulser, receiver and gate dialogs are being displayed
	OnInstrumentPulser();
	OnInstrumentReceiver();
	OnInstrumentGates();
	OnInstrumentScope();
	SendMsg(NOP_MODE);	// Kill packets from slave to master

	if (CDmxAdj::m_pDlg == NULL)
		{
		if (!DestroyDataClients(FALSE))
			return;		// Kill all other data consumers
		CDmxAdj::m_pDlg = new CDmxAdj;
		if (CDmxAdj::m_pDlg) CDmxAdj::m_pDlg->Create(IDD_SCOPE_DMX_ADJ);
		}
	}

void CTscanDlg::OnSetupCustomize() 
{
	// TODO: Add your command handler code here

	CSetUp dlg;
	
	dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;

	if ( dlg.DoModal() == IDOK)
	{
	}

	ConfigToStatScreen();

	if (CWCalDlg::m_pDlg)
		CWCalDlg::m_pDlg->GetConfigData();
}


void CTscanDlg::OnSetupSiteCustom() 
	{
	// This dialog captures site installation specific data which is stored in the 
	// registry of the MMI machine.

	COnSite dlg;

	
	MessageBox(	_T("These parameters were set during installation and should not be changed\nunless the hardware has changed."),
			_T("WARNING"));

	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
		{
		// These new values were written to the registry by the ok command in the dialog
		SendMsg(SITE_SPECIFIC_MSG);
		// This message retrieves parameter from the registry instead of xxx.cfg
		}
	}

void CTscanDlg::OnStatJntNum() 
	{
	//jOINT nUMBER CHANGED from edit box to button.  Press to pop
	// up joint number dialog  01-28-00
	OnSetupJointnumber();
	GetDlgItem(IDC_HIDDEN)->SetFocus();	// xfer to focus to hidden button
	}

void CTscanDlg::OnStatOd() 
	{
	// TODO: Add your control notification handler code here
	// Pop up a dialog to allow the operator to set the OD
	// On closing, sends new OD to instrument?

	COdDlg dlg;
	CString s;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
		{
		m_szStat_Od = m_szNewOd;
		if (ConfigRec.bEnglishMetric & 1)  //Metric
			ConfigRec.JobRec.OD = (float) _tstof(m_szStat_Od) / 25.4f;
		else  //English
			ConfigRec.JobRec.OD = (float) _tstof(m_szStat_Od);
		SendMsg(SET_ALL_THOLDS);		// Set all thresholds
		SendMsg(JOINT_NUMBER_MSG);	// Set pipe number in instrument
		SendMsg(SITE_SPECIFIC_MSG);
		}
	else if (nResponse == IDCANCEL)
		{
		}
	ConfigToStatScreen();
	GetDlgItem(IDC_HIDDEN)->SetFocus();	// xfer to focus to hidden button
	}

void CTscanDlg::OnStatWall() 
	{
	// TODO: Add your control notification handler code here
	// Pop up a dialog to allow the operator to set the OD
	// On closing, sends new OD to instrument?

	CWallDlg dlg;
	CString s;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
		{
		m_szStat_Wall = m_szNewWall;
		if (ConfigRec.bEnglishMetric & 1)  //Metric
			ConfigRec.JobRec.Wall = (float) _tstof(m_szStat_Wall) / 25.4f;
		else  //English
			ConfigRec.JobRec.Wall = (float) _tstof(m_szStat_Wall);

		ConfigRec.UtRec.TholdWallThds[0] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[0]) *
														ConfigRec.JobRec.Wall * 10.0f + 0.5f);
		ConfigRec.UtRec.TholdWallThds[1] = (short) ((float)(100 + ConfigRec.UtRec.TholdWall[1]) *
														ConfigRec.JobRec.Wall * 10.0f + 0.5f);

		SendMsg(SET_ALL_THOLDS);	// all tholds
		SendMsg(SITE_SPECIFIC_MSG);
		}
	else if (nResponse == IDCANCEL)
		{
		}
	ConfigToStatScreen();
	GetDlgItem(IDC_HIDDEN)->SetFocus();	// xfer to focus to hidden button
	
	}


void CTscanDlg::UpdateRpm()
	{	// Update Amalog/Isolog rpm and pipe fpm on screen
	// 04/06/00  Add joint length to this function

	CString s;
	float ftmp;

	s.Format(_T("%d"),m_lOKJnt);
	GetDlgItem(IDC_OK_JOINT)->SetWindowText(s);

	s.Format(_T("%d"),m_lRJnt);
	GetDlgItem(IDC_REJ_JOINT)->SetWindowText(s);

	s.Format(_T("%d"),m_lQJnt);
	GetDlgItem(IDC_QUES_JOINT)->SetWindowText(s);

	if (m_bMetric)
		{	// metric.. m/sec
		// switch to inch/min.  Still called m_nIps
//		ftmp = (float) m_nIps * 0.0254f;
		ftmp = (float) 20000.0f/((float) m_nFpm);
		ftmp = (float) ftmp * 0.3048f; // to meters/min
#if (SYSTEM_ID == END_AREA_SYS)
		s.Format(_T("%5.2f m/m"),ftmp/12.0f);
#else
		s.Format(_T("%5.2f m/m"),ftmp);
#endif
		if (m_pInspectDlg1)
		{
			if ( (m_pInspectDlg1->m_nControlSel != INSPECT_RUN) &&
				(m_pInspectDlg1->m_nControlSel != INSPECT_RECORD) )
				s = _T("0.0 m/m");
		}
		else
			s = _T("0.0 m/m");
		GetDlgItem(IDC_FPM)->SetWindowText(s);

		// Joint Length
		ftmp = (float) m_nJointLength * 0.0254f;
#if (SYSTEM_ID == END_AREA_SYS)
		s.Format(_T("%5.2f m"),ftmp/12.0f);
#else
		s.Format(_T("%5.2f m"),ftmp);
#endif
		GetDlgItem(IDC_JOINT_LEN_EDIT)->SetWindowText(s);

		ftmp = m_fTotalLen * 0.0254f;
		s.Format(_T("%7.2f m"),ftmp);
		GetDlgItem(IDC_TOTAL_FT_EDIT)->SetWindowText(s);

		ftmp = m_fRLen * 0.0254f;
		s.Format(_T("%7.2f m"),ftmp);
		GetDlgItem(IDC_REJ_FT_EDIT)->SetWindowText(s);

#if 0
		ftmp = m_fQLen * 0.0254f;
		s.Format(_T("%7.2f m"),ftmp);
		GetDlgItem(IDC_QUES_LENGTH_EDIT)->SetWindowText(s);
#endif

		}
	else
		{	// feet per min
		ftmp = (float) 20000.0f/((float) m_nFpm);
#if (SYSTEM_ID == END_AREA_SYS)
		s.Format(_T("%4.1f ft/m"),ftmp/12.0f);
#else
		s.Format(_T("%4.1f ft/m"),ftmp);
#endif
		if (m_pInspectDlg1)
		{
			if ( (m_pInspectDlg1->m_nControlSel != INSPECT_RUN) 
				&& (m_pInspectDlg1->m_nControlSel != INSPECT_RECORD) )
				s = _T("0.0 ft/m");
		}
		else
			s = _T("0.0 ft/m");
		GetDlgItem(IDC_FPM)->SetWindowText(s);

		// Joint Length
		ftmp = (float) m_nJointLength /12.0f;
#if (SYSTEM_ID == END_AREA_SYS)
		s.Format(_T("%5.2f ft"),ftmp/12.0f);
#else
		s.Format(_T("%5.2f ft"),ftmp);
#endif
		GetDlgItem(IDC_JOINT_LEN_EDIT)->SetWindowText(s);

		ftmp = m_fTotalLen /12.0f;
		s.Format(_T("%7.2f ft"),ftmp);
		GetDlgItem(IDC_TOTAL_FT_EDIT)->SetWindowText(s);

		ftmp = m_fRLen /12.0f;
		s.Format(_T("%7.2f ft"),ftmp);
		GetDlgItem(IDC_REJ_FT_EDIT)->SetWindowText(s);

#if 0
		ftmp = m_fQLen /12.0f;
		s.Format(_T("%7.2f ft"),ftmp);
		GetDlgItem(IDC_QUES_LENGTH_EDIT)->SetWindowText(s);
#endif
		}
	}


void CTscanDlg::UpdateLineStatus()
{
	CString s;
	CString str;
/*
	if (m_pTcpThreadRxList != NULL)
		m_wLineStatus = m_pTcpThreadRxList->GetMotionBus();

	// Show pipe status,,, inspect, hd down fwd/rev
	if ( m_wLineStatus & 0x0001) s += "1";
	else s += "0";	// station 1 disabled

	if ( m_wLineStatus & 0x0002) s += "1";
	else s += "0";	// station 2 disabled

	if ( m_wLineStatus & 0x0400) s += "1";
	else s += "0";	// head 1 not inspecting

	if ( m_wLineStatus & 0x0800) s += "1";
	else s += "0";	// head 2 not inspecting

	if ( m_wLineStatus & 0x1000) s += "1";
	else s += "0";	// head 3 not inspecting

	if ( m_wLineStatus & 0x2000) s += "1 ";
	else s += "0 ";	// head 4 not inspecting

#if ( (SYSTEM_ID == RAW_WALL_SYS) || (SYSTEM_SUB_ID == BESSEMER_SYS) )
	if (m_wLineStatus & 0x4000)
		s += "Away";
	else
		s += "Toward";
#endif
*/

	if ( g_AscanHead.bDin & 0x04 ) s = _T("Pipe Present -- ");
	else s = "Pipe Absent -- ";

	if ( g_AscanHead.bDin & 0x02 ) s += _T("Forward -- ");
	else s+= _T("Backward -- ");

	str.Format(_T("xLoc=%4d -- "), g_AscanHead.wLocation);
	s+= str;
/*
	str.Format(_T("Clock=%3d -- ", (int) (g_AscanHead.wClock * 0.2048f) );
	s+= str;
*/
	if ( g_AscanHead.wPeriod > 0 )
		str.Format(_T("%3d RPM"), (int) (60000.f / (g_AscanHead.wPeriod * 0.2048f)) );
	else
		str.Format(_T("0 RPM") );
	s+= str;

	str.Format(_T(" T=%3d"), g_AscanHead.bSpare2[0]);
	s+= str;

	str.Format(_T(" D=%02x"), g_AscanHead.bDin);
	s+= str;

	//GetDlgItem(IDC_LINE_STATUS)->SetWindowText(s);
	GetDlgItem(IDC_STATIC_LINESTATUS)->SetWindowText(s);

}


void CTscanDlg::UpdateNextJointNum()
{
	CString s;
	DWORD nNextJointNum = 0;

	if (m_pTcpThreadRxList != NULL)
		nNextJointNum = m_pTcpThreadRxList->GetNextJointNumber();

	s.Format(_T("%d"), nNextJointNum);

	GetDlgItem(IDC_STATIC_NEXTJOINT)->SetWindowText(s);
}


void CTscanDlg::UpdateTitleBar()
	{
	CString s, t;

	//s = _T("TruScan Inspection  ");
	s.Format(_T("%s - "), SYSTEM_NAME);

	t = ConfigRec.JobRec.Customer;
	if ( t.GetLength() > 0 )
		{
		s += t;
		s += _T("  ");
		}

	t = ConfigRec.JobRec.WO;
	if ( t.GetLength() > 0 )
		{
		s += _T("WO = ");
		s += t;
		}

	if (ConfigRec.bEnglishMetric)
		s += _T(" (Metric) ");
	else s += _T(" (English) ");

	t = ConfigRec.JobRec.CfgFile;
	if ( t.GetLength() < 50)
		{
		s+= t;
		}
	SetWindowText(s);
	ConfigToStatScreen();

	}

void CTscanDlg::GetDefaultDirectories()
	{
	// Get the user defined default data and config file directories
	m_szDefDataDir = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Default Data Dir"), _T(".\\"));

	m_szDefCfgDir = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Default Cfg Dir"), _T(".\\"));
	}

void CTscanDlg::SetDefaultDirectories()
	{
	// Get the user defined default data and config file directories
	if ( m_szDefDataDir.GetLength() )
		m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Default Data Dir"), m_szDefDataDir);

	if ( m_szDefCfgDir.GetLength() )
		m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Default Cfg Dir"),m_szDefCfgDir );

	}
void CTscanDlg::SetJobTotals()
	{
	// Store total category and length counts for this job in the 
	// registry
	CString s;

	s.Format(_T("%ld"), m_lOKJnt);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("OK_Jnt_Cnt"), s);


	s.Format(_T("%ld"), m_lRJnt);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Rej_Jnt_Cnt"), s);


	s.Format(_T("%ld"), m_lQJnt);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Ques_Jnt_Cnt"), s);


	s.Format(_T("%f"), m_fTotalLen);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Total Length Inspected"), s);

	s.Format(_T("%f"), m_fRLen);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Total Rej Length"), s);

	s.Format(_T("%f"), m_fQLen);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							_T("Total Ques Length"), s);
	}

void CTscanDlg::GetJobTotals()
	{
	CString s;
	char t[32];

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("OK_Jnt_Cnt"), _T(""));
	//strcpy(t,s);
	CstringToChar(s,t);
	sscanf(t,"%ld", &m_lOKJnt);

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Rej_Jnt_Cnt"), _T(""));
	CstringToChar(s,t);
	sscanf(t, "%ld", &m_lRJnt);

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Ques_Jnt_Cnt"), _T(""));
	CstringToChar(s,t);
	sscanf(t, "%ld", &m_lQJnt);

	// Lengths of pipe in inches

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Total Length Inspected"),_T(""));
	CstringToChar(s,t);
	sscanf(t, "%f", &m_fTotalLen);


	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Total Rej Length"),_T(""));
	CstringToChar(s,t);
	sscanf(t, "%f", &m_fRLen);

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							_T("Total Ques Length"),_T(""));
	CstringToChar(s,t);
	sscanf(t, "%f", &m_fQLen);


	}

void CTscanDlg::GetRegistrySiteDefaults()
	{
	// Site specific parameters.  X offset adjusts flaw reporting location
	// Shoe angle is angle between shoe 1 and 2 which is +/- 90 degrees or
	// +/- 45 clock ticks.
	// Clock offset adjust difference between top sensor and paint guns in clocks

	CString s;
	char t[32];

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default X-loc Offset"),_T("0"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nDefaultXOffset);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Shoe Angle Offset"),_T("45"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nDefaultWOffset);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Clock Offset"),_T("0"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nDefaultClockOffset);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default X-loc Offset 2"),_T("0"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nDefaultXOffset2);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Wall Drop Time"),_T("75"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nWallDropTime);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Record Wall Data"),_T("0"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nRecordWallData);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Line Speed"),_T("1200"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nDefaultLineSpeed);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Rotate Speed"),_T("12000"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nDefaultRotateSpeed);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("20 Channel Per Head"),_T("0"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.n20ChnlPerHead);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Wall Window Size"),_T("10"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nMaxWallWindowSize);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 1 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[0]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 2 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[1]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 3 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[2]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 4 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[3]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 5 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[4]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 6 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[5]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 7 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[6]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 8 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[7]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 9 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[8]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 10 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[9]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 11 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[10]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Phased Array 12 Scan Type"),_T("2"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nPhasedArrayScanType[11]);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Transverse Angle"),_T("160"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nTranAngle);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Water Path"),_T("1.0"));
	CstringToChar(s,t);
	sscanf(t, "%f", &SiteDefault.fWaterPath);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Focal Point Zf"),_T("0.25"));
	CstringToChar(s,t);
	sscanf(t, "%f", &SiteDefault.fFocusPointZf);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Encoder Divider"),_T("100"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nEncoderDivider);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Ad9272 PGA Gain"),_T("0"));
	CstringToChar(s,t);
	sscanf(t, "%d", &SiteDefault.nAd9272PgaGain);

	s = m_ptheApp->GetProfileString((CURRENT_VERSION),
							_T("Default Motion Pulse Length"),_T("0.5"));
	CstringToChar(s,t);
	sscanf(t, "%f", &SiteDefault.fMotionPulseLen);

	}

void CTscanDlg::SetRegistrySiteDefaults()
	{
	CString s;

	s.Format(_T("%d"),SiteDefault.nDefaultClockOffset);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Clock Offset"), s);

	s.Format(_T("%d"),SiteDefault.nDefaultXOffset);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default X-loc Offset"),s);

	s.Format(_T("%d"),SiteDefault.nDefaultXOffset2);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default X-loc Offset 2"),s);

	s.Format(_T("%d"),SiteDefault.nWallDropTime);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Wall Drop Time"),s);

	s.Format(_T("%d"),SiteDefault.nDefaultWOffset);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Shoe Angle Offset"),s);

	s.Format(_T("%d"),SiteDefault.nRecordWallData);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Record Wall Data"),s);

	s.Format(_T("%d"),SiteDefault.nDefaultLineSpeed);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Line Speed"),s);

	s.Format(_T("%d"),SiteDefault.nDefaultRotateSpeed);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Rotate Speed"),s);

	s.Format(_T("%d"),SiteDefault.n20ChnlPerHead);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("20 Channel Per Head"),s);

	s.Format(_T("%d"),SiteDefault.nMaxWallWindowSize);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Wall Window Size"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[0]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 1 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[1]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 2 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[2]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 3 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[3]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 4 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[4]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 5 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[5]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 6 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[6]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 7 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[7]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 8 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[8]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 9 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[9]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 10 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[10]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 11 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nPhasedArrayScanType[11]);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Phased Array 12 Scan Type"),s);

	s.Format(_T("%d"),SiteDefault.nTranAngle);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Transverse Angle"),s);

	s.Format(_T("%f"),SiteDefault.fWaterPath);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Water Path"),s);

	s.Format(_T("%f"),SiteDefault.fFocusPointZf);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Focal Point Zf"),s);

	s.Format(_T("%d"),SiteDefault.nEncoderDivider);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Encoder Divider"),s);

	s.Format(_T("%d"),SiteDefault.nAd9272PgaGain);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Ad9272 PGA Gain"),s);

	s.Format(_T("%f"),SiteDefault.fMotionPulseLen);
	m_ptheApp->WriteProfileString((CURRENT_VERSION),
							_T("Default Motion Pulse Length"),s);

	}

// Globally available method to saving closing position of window in registry

void CTscanDlg::SaveWindowLastPosition(LPCTSTR lpszEntry, WINDOWPLACEMENT *wp)
	{
	// Input is name of dialog to save and last position of window
	CString s;	// string showing top, bottom, left & right
	RECT *rect;

	rect = (RECT *) &wp->rcNormalPosition;
	s.Format(_T("%4d,%4d,%4d,%4d"), rect->top, rect->bottom,
				rect->left, rect->right);
	m_ptheApp->WriteProfileString(( CURRENT_VERSION),
							lpszEntry, s);
	}

// Globally available method to get last position of window from registry
void CTscanDlg::GetWindowLastPosition(LPCTSTR lpszEntry, RECT *rect)
	{
	CString s;
	char t[60];

	s = m_ptheApp->GetProfileString(( CURRENT_VERSION),
							lpszEntry,_T(""));
	CstringToChar(s,t);
	sscanf(t, "%d,%d,%d,%d", &rect->top, &rect->bottom, 
			&rect->left, &rect->right);



	}

void CTscanDlg::WinHelp(DWORD dwData, UINT nCmd) 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::WinHelp(dwData, nCmd);
	}


void CTscanDlg::OnCalibrateFlaws() 
	{
	// TODO: Add your command handler code here
	if (CAcal::m_pDlg == NULL)
		{
		if (!DestroyDataClients(FALSE))
			return;		// Kill all other data consumers
		CAcal::m_pDlg = new CAcal;
		if (CAcal::m_pDlg)
			{
			CAcal::m_pDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
			CAcal::m_pDlg->Create(IDD_ACAL);
			}

		
		CAcal::m_pDlg->StartAcquireData();
		}
	}


void CTscanDlg::OnCalibrateWall() 
{
	// TODO: Add your command handler code here
	if (CWCalDlg::m_pDlg == NULL)
	{
		if (!DestroyDataClients(FALSE))
			return;		// Kill all other data consumers
		if (ConfigRec.UtRec.Shoe[gChannel/10].Ch[gChannel%10].Type == IS_WALL)
		{
			CWCalDlg::m_pDlg = new CWCalDlg;
			if (CWCalDlg::m_pDlg)
			{
				CWCalDlg::m_pDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
				CWCalDlg::m_pDlg->Create(IDD_WALL_CAL);
			}
		}
		else
		{
			MessageBox(_T("Active channel is not a wall channel.\nPlease select a wall channel first."),
				_T("Error"));
		}
	}
}


void CTscanDlg::QualifyConfigRec()
{
	// Force fields in config record to have reasonable values
	int is, ic, ig;
	int i;

	for (is = 0; is < MAX_SHOES; is ++)
		for ( ic = 0; ic < 10; ic++)
			{
			switch (ConfigRec.UtRec.Shoe[is].Ch[ic].Type)
				{
			case IS_WALL:
			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			case IS_LAM:
				break;	// ok

			default:
				ConfigRec.UtRec.Shoe[is].Ch[ic].Type = IS_NOTHING;
				break;
				}

			// Specifically for UTRON !!
#if 0
			if ( ic > 5)
				ConfigRec.UtRec.Shoe[is].Ch[ic].Type = IS_NOTHING;
#endif

			// Limit xdcr x offset to +/- 24 inch from shoe centerline
			if (ConfigRec.UtRec.Shoe[is].Ch[ic].cXOffset > 48)
				ConfigRec.UtRec.Shoe[is].Ch[ic].cXOffset = 48;

			else if (ConfigRec.UtRec.Shoe[is].Ch[ic].cXOffset < -48)
				ConfigRec.UtRec.Shoe[is].Ch[ic].cXOffset = -48;

			// Limit xdcr w offset to +/- 60 degree from centerline
			if (ConfigRec.UtRec.Shoe[is].Ch[ic].cWOffset > 30)
				ConfigRec.UtRec.Shoe[is].Ch[ic].cWOffset = 30;

			else if (ConfigRec.UtRec.Shoe[is].Ch[ic].cWOffset < -30)
				ConfigRec.UtRec.Shoe[is].Ch[ic].cWOffset = -30;

			}

	// verify wall slope and offset and sequence length
	for (is = 0; is < MAX_SHOES; is ++)
	{
		if (ConfigRec.UtRec.fWallSlope[is] > 20.0f) ConfigRec.UtRec.fWallSlope[is] = 20.0f;
		if (ConfigRec.UtRec.fWallSlope[is] < -20.0f) ConfigRec.UtRec.fWallSlope[is] = -20.0f;
		if (ConfigRec.UtRec.WallOffset[is] > 20) ConfigRec.UtRec.WallOffset[is] = 20;
		if (ConfigRec.UtRec.WallOffset[is] < -20) ConfigRec.UtRec.WallOffset[is] = -20;

		verify ( ConfigRec.UtRec.Shoe[is].sequence_length, 1, 10, 1);
	}

	// verify sound velocity
	verify ( ConfigRec.UtRec.CompVelocity, 0.05f, 0.5f, 0.232f);
	verify ( ConfigRec.UtRec.ShearVelocity, 0.02f, 0.5f, 0.126f);

	// New UDP Truscop2

	if (ConfigRec.UtRec.sequence_length != 2)
		ConfigRec.UtRec.sequence_length = 1;
	
	//Prevent invalid filter settings
	for (ic = 0; ic < MEM_MAX_CHANNEL; ic++)
	{
		ConfigRec.receiver.fil_option[ic] &= 0x3;
		ConfigRec.receiver.process[ic] = 0;
	}

	ForceFullWaveDetect();

	// verify Nc and Nx
	for (i=0; i<2; i++)
	{
		verify ( ConfigRec.UtRec.NcLong[i], 1, 2, 2);
		verify ( ConfigRec.UtRec.NcTran[i], 1, 2, 2);
		verify ( ConfigRec.UtRec.NcOblq1[i], 1, 2, 2);
		verify ( ConfigRec.UtRec.NcOblq2[i], 1, 2, 2);
		verify ( ConfigRec.UtRec.NcOblq3[i], 1, 2, 2);
		verify ( ConfigRec.UtRec.NcLamin[i], 1, 2, 2);

		verify ( ConfigRec.UtRec.NxWall[0], 1, 10, 3);
		verify ( ConfigRec.UtRec.Tol, 0, 3, 2);
	}

	// verify inspection threshold and calibration level
	for (i=0; i<6; i++)
	{
		verify ( ConfigRec.UtRec.bCalLvlL[i][0], 1, 100, 80);
		verify ( ConfigRec.UtRec.bCalLvlL[i][1], 1, 100, 80);

		verify ( ConfigRec.UtRec.TholdWall[0], -MAX_DELTA_WALL, -1, -10);
		verify ( ConfigRec.UtRec.TholdWall[1], 1, MAX_DELTA_WALL, 10);
		verify ( ConfigRec.UtRec.TholdLong[i][0], 1, 100, 35);
		verify ( ConfigRec.UtRec.TholdLong[i][1], 1, 100, 35);
	}
#if (SYSTEM_ID == RAW_WALL_SYS)
		verify ( ConfigRec.UtRec.TholdLong[3][0], 1, 20, 10);
		verify ( ConfigRec.UtRec.TholdLong[3][1], 1, 20, 15);
#endif

	// verify gate settings
	for ( ic=0; ic<MEM_MAX_CHANNEL; ic++)
	{
		for (ig=0; ig<MAX_GATE; ig++)
		{
			verify ( ConfigRec.gates.delay[ic][ig], DELAYMIN, DELAYMAX, DELAYMIN);
			verify ( ConfigRec.gates.level[ic][ig], LEVEL1MIN, LEVEL1MAX, LEVEL1MIN);
			verify ( ConfigRec.gates.range[ic][ig], RANGEMIN, RANGEMAX, RANGEMIN);
			verify ( ConfigRec.gates.blank[ic][ig], BLANKMIN, BLANKMAX, BLANKMIN);
		}
	}

	// verify rcvr settings
	for ( ic=0; ic<MEM_MAX_CHANNEL; ic++)
	{
		verify ( ConfigRec.receiver.fil_option[ic], 0, 3, 0);
		verify ( ConfigRec.receiver.gain[ic], GAINMIN, GAINMAX, 0);
		verify ( ConfigRec.receiver.offset[ic], -25, 25, 0);
		for (ig=0; ig<MAX_GATE; ig++)
		{
			verify ( ConfigRec.gates.det_option[ic][ig], 0, 1, 1);
			verify ( ConfigRec.gates.polarity[ic][ig], 0, 1, 0);
		}
		verify ( ConfigRec.receiver.ascan_delay[ic], 50, 50000, 50);
		verify ( ConfigRec.receiver.ascan_range[ic], 5, 2000, 100);
		verify ( ConfigRec.receiver.ascan_baseline[ic], 0, 100, 10);
		verify ( ConfigRec.receiver.ascan_refreshrate, 10, 60, 20);
	}

	// verify tof settings
	for ( ic=0; ic<MEM_MAX_CHANNEL; ic++)
	{
		for (ig=0; ig<MAX_GATE; ig++)
		{
			verify ( ConfigRec.timeoff.trigger[ic][ig], 0, 6, 4);
			verify ( ConfigRec.timeoff.stopon[ic][ig], 0, 1, 1);

			// force wall alarm on
			if (ig == MAX_GATE-1)
				ConfigRec.alarm.laminar[ic][ig] = 0x30;
		}
	}

	// verify TCG settings
	for ( ic=0; ic<MEM_MAX_CHANNEL; ic++)
	{
#if ( (SYSTEM_ID == LORAINE_SYS) && (SYSTEM_SUB_ID == LORAINE_SYS) )
		ConfigRec.receiver.TcgRec[ic].Curve2On = 0;
#endif
		verify ( ConfigRec.receiver.tcg_trigger[ic], 0, 2, 0);
		verify ( ConfigRec.receiver.tcg_step[ic], 0, 3, 0);
		verify ( ConfigRec.receiver.TcgRec[ic].IdGain, 0.0f, 80.0f, 20.0f);
		verify ( ConfigRec.receiver.TcgRec[ic].OdGain, 0.0f, 80.0f, 20.0f);
		for (ig=0; ig<MAX_GATE; ig++)
		{
			verify ( ConfigRec.receiver.TcgRec[ic].GateGain[ig], 0.0f, 80.0f, 20.0f);
		}
		verify ( ConfigRec.receiver.TcgRec[ic].Curve2On, 0, 1, 0);
	}

	// verify pulser settings
	verify ( ConfigRec.pulser.mode, 0, 0, 0);
	verify ( ConfigRec.pulser.prf, PRFMIN, PRFMAX, 900);
	for ( ic=0; ic<MEM_MAX_CHANNEL; ic++)
	{
		verify ( ConfigRec.pulser.pulse_width[ic], PFWIDMIN, PFWIDMAX, PFWIDMIN);
	}

	if ( ConfigRec.JobRec.Oblq1Name[0] == 0 )
		_tcscpy( ConfigRec.JobRec.Oblq1Name, _T("OBQ1"));
	if ( ConfigRec.JobRec.Oblq2Name[0] == 0 )
		_tcscpy( ConfigRec.JobRec.Oblq2Name, _T("OBQ2"));
	if ( ConfigRec.JobRec.Oblq3Name[0] == 0 )
		_tcscpy( ConfigRec.JobRec.Oblq3Name, _T("OBQ3"));
	SetOblqName();

	ConfigRec.JobRec.fChnlOffset[0] = 0.0f;
	if ( ConfigRec.JobRec.fChnlOffset[1] == 0 )
		ConfigRec.JobRec.fChnlOffset[1] = 0.7f;
	if ( ConfigRec.JobRec.fChnlOffset[2] == 0 )
		ConfigRec.JobRec.fChnlOffset[2] = 1.75f;
	if ( ConfigRec.JobRec.fChnlOffset[3] == 0 )
		ConfigRec.JobRec.fChnlOffset[3] = 2.45f;
}


void CTscanDlg::ForceFullWaveDetect()
{
	int is, ic, iChannel;

	for (is = 0; is < MAX_SHOES; is ++)
	{
		for ( ic = 0; ic < 10; ic++)
		{
			iChannel = is*10 + ic;
			switch (ConfigRec.UtRec.Shoe[is].Ch[ic].Type)
			{
			case IS_WALL:
				ConfigRec.gates.det_option[iChannel][1] = 1;
				break;

			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			case IS_LAM:
			case IS_NOTHING:
				ConfigRec.gates.det_option[iChannel][1] = 1;
				ConfigRec.gates.det_option[iChannel][2] = 1;
				break;	// ok

			default:
				break;
			}
		}
	}
}

#if 0
void CTscanDlg::GetIpxStats(NET_STATS *pStats)
	{
	// Make network statistics available to other classes/modules
	pStats->TotalRcvPackets = IpxStat.TotalRcvPackets;
	pStats->LostRcvPackets = IpxStat.LostRcvPackets;
	pStats->DupRcvPackets = IpxStat.DupRcvPackets;

	pStats->TotalSentPackets = IpxStat.TotalSentPackets;
	pStats->LostSentPackets = IpxStat.LostSentPackets;
	pStats->DupSentPackets = IpxStat.DupSentPackets;
	}
#endif

void CTscanDlg::GetPrinter()
	{
	// Get the default printer for use in screen prints.  When print
	// opertion is done, delete all resource in ReleasePrinter
	// THIS IS A HELPER FUNCTION.  Only makes sense when called
	// from FilePrintScreen
	// 12/12/00
	m_dcPrn = new CDC;		// dc for the default printer
	CPrintDialog dlg(FALSE);// print dlg on stack, don't display

	dlg.GetDefaults();		// infor for default printer
	m_dcPrn->Attach(dlg.GetPrinterDC() );
	m_PrintDlg = dlg.m_pd;

	}

void CTscanDlg::ReleasePrinter()
	{
	// Give back resources generated/created by GetPrinter operation
	// THIS IS A HELPER FUNCTION.  Only makes sense when called
	// from FilePrintScreen
	// 12/12/00
	if (m_dcPrn)
		{
		::GlobalFree(m_PrintDlg.hDevNames );
		::GlobalFree(m_PrintDlg.hDevMode  );
		::DeleteDC(m_dcPrn->m_hDC);
		delete m_dcPrn;
		m_dcPrn = NULL;
		}

	}

#if 0

BOOL CTscanDlg::GrantList()
	{	// If Input thread for ipx using list, fail
		// else grant list use to caller
		// Bear in mind that ipx thread is higher priority and
		// will not tie up list for long

	int i;

	for ( i = 0; i < 3; i++)	// 3 attempts to gain list
		{
		ListShare.mmi = 1;
		if ( ListShare.ipx == 0) return TRUE;	// got use of list
		ListShare.mmi = 0;
		}
	return FALSE;
	}

void CTscanDlg::GetIpxList()
	{
	I_MSG_CAL	*pImsg02;			// msg02 is much bigger than msg 01.

	if ( GrantList() )
		{
		while (!plistIpxIn.IsEmpty())
				{
				pImsg02 = (I_MSG_CAL *)plistIpxIn.RemoveHead();
				plistUtData.AddTail( (void *) pImsg02);
				m_nRpm = gnTruscopePeriod;
				if (gnTruscopePeriod > 50)
					m_nFpm = VELOCITY_DX_FACTOR/gnTruscopePeriod; // at least 50 ms to go 4 inches
				else m_nFpm = 0;
//				m_nIps = pImsg02->InspHdr.motion_pulse_per_sec;
				m_nJointLength = pImsg02->InspHdr.JointLength;
	
				m_wLineStatus = pImsg02->InspHdr.wLineStatus;
				}
		ListShare.mmi = 0;	// release list

		}
	}
#endif

void CTscanDlg::OnRADIOGate2() 
	{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_RADIOGate1, IDC_RADIOGate2, IDC_RADIOGate2 );
	gGate=1;
	UpdateDlgs();
	}

void CTscanDlg::OnRADIOGate1() 
	{
	// TODO: Add your control notification handler code here
	CheckRadioButton( IDC_RADIOGate1, IDC_RADIOGate2, IDC_RADIOGate1 );
	gGate=0;
	UpdateDlgs();
	}

void CTscanDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default
	// CODE IS DIRECTION OF SCROLL MOTION
	int nDelta;		// where the bar started and amount of change
	int nMax, nMin;
	int nCtlId;
	int dummy = 5;
	CString s;
	short nStart;

//	float fStart;			// floating scroll bar start point

	nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);

	nCtlId = pScrollBar->GetDlgCtrlID();

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta = -10;	//-(nMax - nMin)/10;
		break;

	case SB_PAGERIGHT:
		nDelta = 10;	//(nMax - nMin)/10;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - nStart;
		break;


	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;

		}	// switch on type of motion

	nStart += nDelta;
	if ( nStart > nMax) nStart = nMax;
	if ( nStart < nMin) nStart = nMin;


	pScrollBar->SetScrollPos(nStart, TRUE);

	switch (nCtlId)
		{

	case IDC_SBchnl:
		// the active channel
		gChannel = nStart - 1;
		TRACE ("OnHScroll: gChannel = %d\n", gChannel);


		ChangeActiveChannel();


#if 0
		// now in change active channel
		UpdateChnlSelected();
		SendMsg(CHANNEL_SELECT);			// 
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
#endif
		
		break;

	case IDC_SBcpyto:
		gCopyTo = nStart-1;
		UpdateChnlSelected();
		break;

	default:
		break;
		}
	if ( CAcal::m_pDlg )
	{
		i = gChannel + IDC_ACAL_CH1;
        bCMD = TRUE;
        CAcal::m_pDlg ->DoS1RadioButton (i);
        bCMD = FALSE;
	}
		//	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

void CTscanDlg::OnCpyChnl() 
	{
	// TODO: Add your control notification handler code here

	int nSrcType, nDestType;


	nSrcType = ChnlDisplay(gChannel);
	nDestType = ChnlDisplay(gCopyTo);

	if ( (gCopyTo >= MAX_CHANNEL) || (nSrcType == nDestType) )
		{	// all of active chnl type to all other chnls of same type
		CCpyto dlg;
		dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;
		if ( dlg.DoModal() == IDOK)
			{
			}
		}

	else
		{	// warn that chnl types are different
		if (MessageBox( _T("Copy anyway?"), _T("WARNING - Channel types are different"),
						MB_YESNO) == IDNO)  return;		// skip the copy

		CCpyto dlg;
		dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;
		if ( dlg.DoModal() == IDOK)
			{
			}

		}
	}


#if 0
void CTscanDlg::OnGain() 
	{
	// TODO: Add your command handler code here
	if (CSysGain::m_pDlg == NULL)
		{
		CSysGain::m_pDlg = new CSysGain;
		if (CSysGain::m_pDlg) CSysGain::m_pDlg->Create(IDD_SysGain);
		}
	
	}
#endif


void CTscanDlg::OnInstrumentGates() 
{
	// TODO: Add your command handler code here

	if (CGateDlg::m_pDlg == NULL)
		{
		CGateDlg::m_pDlg = new CGateDlg;
		if (CGateDlg::m_pDlg) CGateDlg::m_pDlg->Create(IDD_GATES);
		}

	EnableInstrumentPanels();
}


void CTscanDlg::OnInstrumentReceiver() 
{
	// TODO: Add your command handler code here
	if (CRcvr::m_pDlg == NULL)
		{
		CRcvr::m_pDlg = new CRcvr;
		if (CRcvr::m_pDlg) CRcvr::m_pDlg->Create(IDD_RCVR);
		}
	
	EnableInstrumentPanels();
}

void CTscanDlg::OnInstrumentPulser() 
{
	// TODO: Add your command handler code here
	if (CPulser::m_pDlg == NULL)
		{
		CPulser::m_pDlg = new CPulser;
		if (CPulser::m_pDlg) 
		{
			CPulser::m_pDlg->Create(IDD_Pulsr);
			CPulser::m_pDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
		}
		}
	
	EnableInstrumentPanels();
}

void CTscanDlg::OnInstrumentScope() 
	{
	// TODO: Add your command handler code here
	// One dialog for both cases.  If called from diagnostic
	// set .NormalOrDiagnostic to 1
#ifndef TWO_CHANNEL_4_GATE_BOARD

	if (COscpe::m_pDlg == NULL)
		{
		COscpe::m_pDlg = new COscpe;
		if (COscpe::m_pDlg)
			{
			ConfigRec.OscopeRec.NormalOrDiagnostic = 0;
			COscpe::m_pDlg->Create(IDD_OSCPE);
			}
		}

#else

    if (m_pScopeDlg != NULL)
	{
        m_pScopeDlg->SetFocus ();
		m_pScopeDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pScopeDlg = new CScopeDlg(this,m_pTcpThreadRxList);
		m_pScopeDlg->Create(IDD_SCOPE, this);
		CRect rect;
		m_SBchnl.GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_pScopeDlg->SetWindowPosition(rect.bottom+2);
		m_pScopeDlg->ShowWindow(SW_SHOW);
	}

#endif	
	}

void CTscanDlg::OnInstrumentTcgset() 
{
	// TODO: Add your command handler code here
#ifndef TWO_CHANNEL_4_GATE_BOARD

	if (CTCG::m_pDlg == NULL)
		{
		CTCG::m_pDlg = new CTCG;
		if (CTCG::m_pDlg) CTCG::m_pDlg->Create(IDD_TCGset);
		}

#else
	//m_pTcpThreadRxList->StopTcpAcqDataThread();

    if (m_pTcgSetupDlg != NULL)
	{
        m_pTcgSetupDlg->SetFocus ();
		m_pTcgSetupDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pTcgSetupDlg = new CTcgSetupDlg(this);
		m_pTcgSetupDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
		m_pTcgSetupDlg->Create(IDD_TCGSETUP, NULL);
		m_pTcgSetupDlg->ShowWindow(SW_SHOW);
	}

	EnableInstrumentPanels();
#endif
}

void CTscanDlg::OnInstrumentTofalarms() 
{
	// TODO: Add your command handler code here
	if (Ctof::m_pDlg == NULL)
		{
		Ctof::m_pDlg = new Ctof;
		if (Ctof::m_pDlg) Ctof::m_pDlg->Create(IDD_DTOF);
		}
	
	EnableInstrumentPanels();
}

void CTscanDlg::OnInstrumentAllpanelsalta() 
{
	// TODO: Add your command handler code here

	OnInstrumentReceiver();
	OnInstrumentGates();
	OnInstrumentPulser();
	OnInstrumentTofalarms();
#if (LOCATION_ID == BESSEMER)
	OnInstrumentTcgset();
#endif
	OnInstrumentScope();

}

void CTscanDlg::OnInstrumentCloseallaltc() 
	{
	// TODO: Add your command handler code here
	Close_Instruments();	
	}

void CTscanDlg::OnDiagnosticDiskdump() 
	{
	// TODO: Add your command handler code here
	
	}

void CTscanDlg::OnDiagnosticEnetstatus() 
	{
	// TODO: Add your command handler code here
	if (CEnet::m_pDlg == NULL)
		{
		if (!DestroyDataClients(FALSE))
			return;		// Kill all other data consumers
		CEnet::m_pDlg = new CEnet;
		if (CEnet::m_pDlg) CEnet::m_pDlg->Create(IDD_Enet);
		}
	
	}

void CTscanDlg::OnDiagnosticFlawdepthtest() 
	{
	// TODO: Add your command handler code here
	
	}

void CTscanDlg::OnDiagnosticPacketdata() 
	{
	// TODO: Add your command handler code here
	if (CCatch::m_pDlg == NULL)
		{
		if (!DestroyDataClients(FALSE))
			return;		// Kill all other data consumers
		CCatch::m_pDlg = new CCatch;
		if (CCatch::m_pDlg)
		{
			CCatch::m_pDlg->Create(IDD_CATCH);
			CCatch::m_pDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
		}
		}
	}

void CTscanDlg::OnDiagnosticScope() 
	{
	// TODO: Add your command handler code here
	// One dialog for both cases.  If called from diagnostic
	// set .NormalOrDiagnostic to 1
	if (COscpe::m_pDlg == NULL)
		{
		COscpe::m_pDlg = new COscpe;
		if (COscpe::m_pDlg)
			{
			ConfigRec.OscopeRec.NormalOrDiagnostic = 1;
			COscpe::m_pDlg->Create(IDD_OSCPE);
			}
		}
	
	}

void CTscanDlg::OnDiagnosticSync() 
	{
	// TODO: Add your command handler code here
	
	}

void CTscanDlg::OnDiagnosticUdpio() 
	{
	// TODO: Add your command handler code here
	
	}

void CTscanDlg::OnDiagnosticAlarms() 
	{
	// TODO: Add your command handler code here
	CAlarms dlg;
	
	if ( dlg.DoModal() == IDOK)
		{
		}
	
	}

void CTscanDlg::OnRButtonDown(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnRButtonDown(nFlags, point);
	}

void CTscanDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
	{
	// TODO: Add your message handler code here
	
	}

void CTscanDlg::PublicHelpContents()
	{
	// Public/ global access to Help file and system
	OnHelpContents();
	}

void CTscanDlg::SystemInit()
	{
	/* Convert global system_init to protected member function */
	int i;
	WORD prf;
//	BYTE buf[8];
	BYTE local_PRF, pulser_mode;
//	BYTE tmp_tcg_enable[MEM_MAX_CHANNEL];

//	BYTE tmp_tcg_enable[40];
	WORD wOldOpMode;
//	clock_t tnow;

	bInit = TRUE;
	// save pulser config info
	prf = ConfigRec.pulser.prf;
	//ConfigRec.pulser.prf = 1;	// 10 hz initial

	pulser_mode = ConfigRec.pulser.mode;
	ConfigRec.pulser.mode = 0;		// prf mode

	local_PRF = ConfigRec.pulser.LocalPRF[gChannel/10];

	SetSyncRegisters();		// operate on lpWork->config.SyncControl .. in dlg_udpc.c
	::set_sync_register();	// send info to instrument
	// after 2/23/98 also set prf related parameters
#if 0
	tnow = clock();
	while ( (tnow+1) < clock() );	// burn time
#endif
	// Turn off pc104 debug
	ConfigRec.OscopeRec.pc104_debug = 0;
	SendMsg(DEBUG_CONTROL);

	// Save old op mode and switch all slaves to nop op mode
	wOldOpMode = (ConfigRec.wOpMode & 3) + 0x200;	//NOP, RUN, CAL, PKT
	SendMsg(NOP_MODE);

	
//	delay_enabled_flag = 1;		// slow down queuing of enet messages in init
	SendMsg(SYSINIT);
#if 0
	tnow = clock();
	while ( (tnow+1) < clock() );	// burn time
#endif
	//sleep(50);
//	switchable_delay(25000);	// Give instrument time to process this command
								// before any others sent
	// remember how tcg was set.  Calling receiver gain will turn off
	// later in this init sequence
	//memcpy( (void *) &tmp_tcg_enable, (void *) &ConfigRec.receiver.tcg_enable[0],
	//MEM_MAX_CHANNEL);

	// Send message defining what level of mmi code running
	// Version 2 mmi cause more checks in instrument for download ok
//	MMI_version_num();		
	SendMsg(MMI_VERSION);		//ConfigRec.JobRec.vernumn

	CLoadProgress *pLoadProgress = new CLoadProgress;
	pLoadProgress->Create(IDD_LOAD_PROGRESS, this);
	pLoadProgress->ShowWindow(SW_SHOW);

	for (i = 0; i < 10/*MAX_CHANNEL*/; i++)  // temporary to make bypass master work
		{
#if 0
		::send_instrument_channel_info(i);
#else
		SendInstrumentChannelInfo(i);
		if ( ((i+1)%5) == 0) 
		{
			pLoadProgress->m_pcLoadProgress.SetPos(i*100/10/*MAX_CHANNEL*/);

			//::Sleep(200);
		}
		if ( ((i+1)%10) == 0) 
			::Sleep(200);
#endif
		}

	pLoadProgress->KillMe();

	SendMsg(DEBUG_CONTROL);


	SendMsg(SLAVE_TRIG_RESET);
	SendMsg(SLAVE_TRIGGER);	// negates affect of channel in


	gChannel = 0;
	SendMsg(CHANNEL_SELECT);
	SendMsg(FIRST_INSTRUMENT);	//first_instrument();

	gGate = 0;
	SendMsg(GATE_SELECT);
//	scope_trace1();
	SendMsg(SCOPE_TRACE1);
//	write_to_scope_demux ();
	// Now tell the master instrument to set the j-box mux
	SendMsg(SET_SCOPE_MUX);
//	scope_trace2();
	SendMsg(SCOPE_TRACE2);
//	write_to_scope_demux ();
	// Now tell the master instrument to set the j-box mux
	SendMsg(SET_SCOPE_MUX);

	SendMsg(TCG_COMPUTE_ALL);	//TCG_compute_all();	// this may take a while
#if 0
	tnow = clock();
	while ( (tnow+1) < clock() );	// burn time
#endif
	// Instrument computer 10 tables of 512 pts EACH with floating pt operations
	SendMsg(SYSINIT_COMPLETE);
//	delay_enabled_flag = 0;	// full speed for messages
#if 0
	if (init_udp_chart_assignments() != 0)
			{
			PostQuitMessage(0);
			return FALSE;
			}
	// 07/18/95 Load wall cal info into udp for each instrument.
	for ( i = 0; i < MAX_CHANNEL/10 ; i++) 
		{
		::SetWallCalInfo(i);
		}

	::SendNcTolerance();

	
	// Send wall value recorded in config to udp 07/15/96
	::SendWallToUDP(hDlgDLG_STATUS, ConfigRec.pipeinfo.Wall);

#endif
	// Turn off all tcg, then turn back on only those selected as on
	// and used as wall channels.  Remember, system-on turns on all

#if 0
	// temp only for testing
	ConfigRec.receiver.tcg_system = 0;	// OFF
	SendMsg(TCG_SYSTEM);	//TCG_sys_on_off();

	// restore tcg status
	memcpy( (void *) &ConfigRec.receiver.tcg_enable,(void *) &tmp_tcg_enable, 
			MEM_MAX_CHANNEL);

	for ( gChannel = 0; gChannel < MAX_CHANNEL; gChannel++)
		{
		SendMsg(CHANNEL_SELECT);
		SendMsg(TCG_ENABLE);
		}
#endif


#if 0
	// restore pulser info
	ConfigRec.pulser.prf = prf;
	ConfigRec.pulser.mode = 	pulser_mode;
	ConfigRec.pulser.LocalPRF[gChannel/10] = 	local_PRF;

	::SetSyncRegisters();		// operate on lpWork->config.SyncControl .. in dlg_udpc.c
	::set_sync_register();	// send info to instrument
#endif

//	delay_enabled_flag = 0;	// full speed for messages
#if 0
	// Flaw scalers now done by send_instrument_channel_info  04/15/98
//	UpdateFlawDepthScalers(0);		// gate 0 download
//	UpdateFlawDepthScalers(1);		// gate 1 download
	SendAllFlawsFlag ();
	SendFlawDepthFlag ();
#endif

	// Turn oN pc104 debug
	ConfigRec.OscopeRec.pc104_debug = 1;
	SendMsg(DEBUG_CONTROL);
	SendMsg(wOldOpMode);
	bInit = FALSE;

#ifdef   TWO_CHANNEL_4_GATE_BOARD

	m_pTcpThreadRxList->SendMDAC();  // might be temporary.  re-check later

#endif   //TWO_CHANNEL_4_GATE_BOARD

	for (i=0; i<MAX_SHOES; i++)
		m_pTcpThreadRxList->SetPrfIntExt(i);
	}


void CTscanDlg::SendInstrumentChannelInfo(int chnl)
	
{	/* Convert global send_instrument_channel_info to protected member function */

		// send the instrument all commands which affect a channel
		// this sets info for both gates of the selected channel
		
	int  g;

//	delay_enabled_flag = delay_flag;
	tmpchnl = gChannel;
	gChannel = chnl;
	g=gGate;
	//SendMsg(CHANNEL_SELECT);
	m_pTcpThreadRxList->SendSlaveMsg(CHANNEL_SELECT, gChannel, 0, 0, 0, 0, 0);
#ifdef   TWO_CHANNEL_4_GATE_BOARD
	if (gChannel%10 == 0)
	{
		//m_pTcpThreadRxList->SendMDAC();
		//m_pTcpThreadRxList->SetSequenceLength(gChannel);  // temporary.  re-check later
		SendMsg(PULSER_PRF);
	}
	m_pTcpThreadRxList->SendSlaveMsg(0x33, 0,0,0,0,0,0);  /* disable end area tool */
	for (gGate = 0; gGate < MAX_GATE; gGate++)
#else
	for (gGate = 0; gGate < 2; gGate++)
#endif
	{
		//SendMsg(GATE_SELECT);
		m_pTcpThreadRxList->SendSlaveMsg(GATE_SELECT, gGate, 0, 0, 0, 0, 0);
		SendMsg(GATES_DELAY);		//gates_delay();	
		// all these commands implicitly work on gate and channel
		SendMsg(GATES_LEVEL);	//gates_level();	// global variables
		SendMsg(GATES_RANGE);	//gates_range();
		SendMsg(GATES_BLANK);	//gates_blank();
		SendMsg(GATES_TRIGGER);	//gates_trigger();
		if ( gGate == 0)
		{
			SendMsg(GATES_LAMIN);	//gates_lamin();
			SendMsg(LAM_GATE_LEVEL);	//lam_gate_level();
		}
		else
			SendMsg(TOF_RESOLUTION);

		SendMsg(ALARM_LAMIN);	//	alarm_lamin();
		SendMsg(ALARM_POLARITY);	//alarm_polarity();
		SendMsg(ALARM_TRIGGER);	//alarm_trigger();
						//send_alarm_dlg_info(chnl,g,delay_flag);
		SendMsg(TOF_TRIGGER);		//tof_trigger();
		SendMsg(TOF_STOPON);	//tof_stopon();

#ifdef  TWO_CHANNEL_4_GATE_BOARD
		SendMsg(RECEIVER_RF_VIDEO);	//receiver_rf_video();
		SendMsg(RECEIVER_POLT);	//receiver_polarity();
#endif
	}

	gGate = g;

	SendMsg(TCG_TRIGGER);	//TCG_trigger();	// load trigger source selection

    /* the following commands are by channel    */
//	send_receiver_dlg_info(chnl,delay_flag);
	SendMsg(RECEIVER_GAIN);	//receiver_gain();
	SendMsg(RECEIVER_REJECT);	//receiver_reject();
	SendMsg(RECEIVER_FILT);	//receiver_filter();
	SendMsg(RECEIVER_FCNT);	//receiver offset;
	SendMsg(SET_ASCAN_REGISTERS);

	SendMsg(RECEIVER_PROCESS);
	
#ifndef  TWO_CHANNEL_4_GATE_BOARD
	SendMsg(RECEIVER_POLT);	//receiver_polarity();
	SendMsg(RECEIVER_RF_VIDEO);	//receiver_rf_video();
#endif
	SendMsg(FIRING_SEQ);	// SendFiringSeq(channel);
	
//	send_pulser_dlg_info(chnl,delay_flag);
	SendMsg(PULSER_WIDTH);	//pulser_width();
	// pulser_damping();
	//SendMsg(PULSER_ONOFF);	//pulser_onoff();

	
//	send_tcg_dlg_info(chnl,delay_flag);
//
	//SendMsg(TCG_TRIGGER);	//TCG_trigger();	// load trigger source selection
	if (m_pTcpThreadRxList->GetTcgTrigSel(gChannel) != 0)
	{
		SendMsg(TCG_FN);	//TCG_fn();		// load function selection argument
		SendMsg(TCG_ARG1);	//TCG_arg1();		// load a*t linear argument
		SendMsg(TCG_ARG2);	//TCG_arg2();		// load b*t*t 2nd order argument
	//SendMsg(TCG_STEP);	//TCG_step();		// load time step size, 200ns-800ns.. 0-3
	}

//		delay_enabled_flag = delay_flag;
	gChannel = tmpchnl;

}

int CTscanDlg::ChnlDisplay(int nChnl)
	{
	// Generate text showing shoe and chnl number and chnl type
	// Return chnl type index
	int is, ic, nType, i;

	nChnl = nChnl % MAX_CHANNEL;

	is = nChnl / 10;
	ic = nChnl % 10;

	nType = ConfigRec.UtRec.Shoe[is].Ch[ic].Type ;

	switch(nType)
		{
			case IS_NOTHING:
			default:		i = 0;		break;

			case IS_WALL:	i = 1;		break;

			case IS_LONG:	i = 2;		break;

			case IS_TRAN:	i = 3;		break;

			case IS_OBQ1:	i = 4;		break;
			case IS_OBQ2:	i = 5;		break;
			case IS_OBQ3:	i = 6;		break;
			case IS_LAM:		i = 7;				break;		

		}
	m_szChnlDesc.Format(_T("S%1d:Ch%1d  %s"), is+1, ic+1, XdcrText[i]);
	return i;

	}

void CTscanDlg::UpdateChnlSelected()
	{
	// Called from many windows to update the active channel selected and the channel
	// shown on the scope.  Fomerly called update_all

	int button, nType;
	button= gGate ? IDC_RADIOGate2 : IDC_RADIOGate1;
	
	CheckRadioButton( IDC_RADIOGate1, IDC_RADIOGate2, button );

	nType = ChnlDisplay(gChannel);	// update Chnl Description in m_szChnlDesc
	m_sActCh = m_szChnlDesc;

	if (gCopyTo >= MAX_CHANNEL)
		{	// copy to all chnls of this type
		m_sCpyto = _T("ALL ");
		m_sCpyto += XdcrText[nType];	// same as active channel
		}
	else
		{
		ChnlDisplay(gCopyTo);
		m_sCpyto = m_szChnlDesc;
		}
	m_SBchnl.SetScrollPos(gChannel+1, TRUE);
	UpdateData(FALSE);
	
	
	//	::CheckRadioButton(hTscanDlg, IDC_RADIOGate1, IDC_RADIOGate2, IDC_RADIOGate1+gGate );	
	UpdateDlgs();

	}

HBRUSH CTscanDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
	{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_LINESTATUS)->m_hWnd)
	{
		if (g_AscanHead.bDin & 0x04)  //pipe present
			pDC->SetTextColor(RGB(0, 175, 0));
		else
			pDC->SetTextColor(RGB(255, 0, 0));
	}

	return hbr;
	}

void CTscanDlg::UpdateDlgs()
	{	// public access to update dlgs function
	bDUpdate=TRUE;
	if (CSysGain::m_pDlg) CSysGain::m_pDlg->UpdateGates();
	if (CRcvr::m_pDlg) CRcvr::m_pDlg->OnInitDialog();
	if (CPulser::m_pDlg) CPulser::m_pDlg->OnInitDialog();
	if (CGateDlg::m_pDlg) CGateDlg::m_pDlg->OnInitDialog();
	if (Ctof::m_pDlg) Ctof::m_pDlg->OnInitDialog();
#ifndef  TWO_CHANNEL_4_GATE_BOARD
	if (CTCG::m_pDlg) CTCG::m_pDlg->OnInitDialog();
#endif
	if (CAcal::m_pDlg) CAcal::m_pDlg->OnInitDialog();

#ifdef  TWO_CHANNEL_4_GATE_BOARD
	if (m_pScopeDlg)
		m_pScopeDlg->UpdateChnlGate();

	if (m_pTcgSetupDlg)
		m_pTcgSetupDlg->UpdateDlg();
#else
	if (COscpe::m_pDlg) COscpe::m_pDlg->OnInitDialog();
#endif
	if (CViewcfg::m_pDlg) 
	{
		CViewcfg::m_pDlg->Invalidate();
		CViewcfg::m_pDlg->SendMessage(WM_PAINT,0,0);
	}

	if (m_pScopeTrace1CalDlg)
		m_pScopeTrace1CalDlg->UpdateDlg();
	if (m_pScopeTrace2CalDlg)
		m_pScopeTrace2CalDlg->UpdateDlg();

	if (m_pAscanDlg)
		m_pAscanDlg->SetRfFullWaveMode();

	bDUpdate=FALSE;
	}

void CTscanDlg::ChangeActiveChannel()
	{	// The global mother of all channel changing
	m_pTcpThreadRxList->SendSlaveMsg(SET_ASCAN_READ_SEQ, gChannel, 0, 0, 0, 0, 0);

	ConfigRec.receiver.det_option[gChannel] = !ConfigRec.gates.det_option[gChannel][gGate];
	SendMsg(CHANNEL_SELECT);
#if (HIDE_ASCAN == 0)
	//SendMsg(ASCAN_MODE);
	//SendMsg(SET_ASCAN_REGISTERS);
#endif
	UpdateChnlSelected();
	//SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
	//SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_DISPLAY);
	}

void CTscanDlg::ChangeActiveChannelTo(short nChannel)
{
	gChannel = nChannel;
	ChangeActiveChannel();
}

void CTscanDlg::ChangeActiveGateTo(short nGate)
{
	gGate = nGate;
	SendMsg(GATE_SELECT);
	UpdateChnlSelected();
}

void CTscanDlg::OnInstrumentEndarea() 
{
	// TODO: Add your command handler code here
    if (m_pEndAreaDlg != NULL)
	{
        m_pEndAreaDlg->SetFocus ();
		m_pEndAreaDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pEndAreaDlg = new CEndAreaToolDlg(this,m_pTcpThreadRxList);
		m_pEndAreaDlg->Create(IDD_ENDAREA, this);
		m_pEndAreaDlg->ShowWindow(SW_SHOW);
	}
}

void CTscanDlg::SetNetworkAlarm()
{
	int i;
	int nMaxShoes;

#if (SYSTEM_ID == LORAINE_SYS)
	#if (SYSTEM_SUB_ID == LORAINE_SYS)
		nMaxShoes = MAX_SHOES;
	#endif
	#if (SYSTEM_SUB_ID == BESSEMER_SYS)
		nMaxShoes = 5;
	#endif
#else
	nMaxShoes = 2;
#endif

	m_bShowNetAlarm = FALSE;
	for (i=0; i<nMaxShoes; i++)
	{
		if (g_bConnected[i] != 1)
		{
			m_bShowNetAlarm = TRUE;
			break;
		}
	}
	if (!m_bConfigLoaded)
	{
		m_bShowNetAlarm = TRUE;
		for (i=0; i<nMaxShoes; i++)
		{
			if (g_bConnected[i] == 1)
			{
				g_bConnected[i] = 2;
			}
		}
	}
	if (!IsSyncNormal())
	{
		m_bShowNetAlarm = TRUE;
		for (i=1; i<nMaxShoes; i++)
		{
			if (g_bConnected[i] == 1)
			{
				if (ConfigRec.pulser.LocalPRF[i-1] == 0)
					g_bConnected[i] = 4;   //Slave instrument (PRF External)
				else
					g_bConnected[i] = 5;   //PRF Internal OR External TTL
			}
		}
	}

	BOOL bChanged = FALSE;
	for (i=0; i<MAX_SHOES+1; i++)
	{
		if (m_bConnected[i] != g_bConnected[i])
		{
			bChanged = TRUE;
			break;
		}
	}

	// copy to local member
	for (i=0; i<MAX_SHOES+1; i++)
	{
		m_bConnected[i] = g_bConnected[i];
	}

	m_bShowNetAlarm = TRUE;  // always show 
	//if (bChanged)
		ShowNetworkAlarm();

	// reset g_bConnected to 0
	for (i=0; i<MAX_SHOES+1; i++)
	{
		g_bConnected[i] = 0;
	}
}


void CTscanDlg::ShowNetworkAlarm()
{
	int x = 3;

	if (m_bShowNetAlarm)
	{
		if (0 /*!m_bLastShowNetAlarm*/)
		{
			m_btnMaster.ShowWindow(SW_SHOW);
			m_btnSlave1.ShowWindow(SW_SHOW);
#if (SYSTEM_ID == LORAINE_SYS)
			m_btnSlave2.ShowWindow(SW_SHOW);
	#if (SYSTEM_SUB_ID == LORAINE_SYS)
			m_btnSlave3.ShowWindow(SW_SHOW);
			m_btnSlave4.ShowWindow(SW_SHOW);
			m_btnSlave5.ShowWindow(SW_SHOW);
			m_btnSlave6.ShowWindow(SW_SHOW);
			m_btnSlave7.ShowWindow(SW_SHOW);
			m_btnSlave8.ShowWindow(SW_SHOW);
			m_btnSlave9.ShowWindow(SW_SHOW);
	#endif
#endif
			m_bLastShowNetAlarm = TRUE;
		}

		CClientDC dcMaster(&m_btnMaster);
		CClientDC dcS1(&m_btnSlave1);
#if (SYSTEM_ID == LORAINE_SYS)
		CClientDC dcS2(&m_btnSlave2);
		CClientDC dcS3(&m_btnSlave3);
		CClientDC dcS4(&m_btnSlave4);
	#if (SYSTEM_SUB_ID == LORAINE_SYS)
		CClientDC dcS5(&m_btnSlave5);
		CClientDC dcS6(&m_btnSlave6);
		CClientDC dcS7(&m_btnSlave7);
		CClientDC dcS8(&m_btnSlave8);
		CClientDC dcS9(&m_btnSlave9);
	#endif
#endif
		CRect rect;
		COLORREF color;
		COLORREF clrBlue = RGB(0, 0, 255);		//Blue, no config file loaded
		COLORREF clrYell = RGB(255, 255, 128);	//Yellow, no hardware found
		COLORREF clrSlave = RGB(0, 255, 255);	//Slave instrument (PRF External)
		COLORREF clrMaster = RGB(255, 0, 255);	//PRF Internal OR External TTL

		m_btnMaster.GetClientRect(&rect);
		if (m_bConnected[0] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[0] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[0] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcMaster.FillSolidRect (&rect, color);
		dcMaster.TextOut(3,0,_T("M"));

		m_btnSlave1.GetClientRect(&rect);
		if (m_bConnected[1] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[1] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[1] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[1] == 4)
			color = clrSlave;      //turn cyan if slave instrument
		else if (m_bConnected[1] == 5)
			color = clrMaster;     //turn magenta if master instrument
#if 1 //(SYSTEM_ID == RAW_WALL_SYS)
		else if (m_bConnected[1] == 6)
			color = RGB(0,0,0);     //turn black if failed writing wall data file
		else if (m_bConnected[1] == 7)
			color = RGB(255,255,255);     //turn white if 12-clock is wrong
#endif
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS1.FillSolidRect (&rect, color);
		dcS1.TextOut(x,0,_T("I1"));

#if (SYSTEM_ID == LORAINE_SYS)
		m_btnSlave2.GetClientRect(&rect);
		if (m_bConnected[2] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[2] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[2] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[2] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[2] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else if (m_bConnected[2] == 6)
			color = RGB(0,0,0);     //turn black if failed writing wall data file
		else if (m_bConnected[2] == 7)
			color = RGB(255,255,255);     //turn white if 12-clock is wrong
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS2.FillSolidRect (&rect, color);
		dcS2.TextOut(x,0,_T("I2"));

		m_btnSlave3.GetClientRect(&rect);
		if (m_bConnected[3] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[3] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[3] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[3] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[3] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else if (m_bConnected[3] == 6)
			color = RGB(0,0,0);     //turn black if failed writing wall data file
		else if (m_bConnected[3] == 7)
			color = RGB(255,255,255);     //turn white if 12-clock is wrong
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS3.FillSolidRect (&rect, color);
		dcS3.TextOut(x,0,_T("I3"));

		m_btnSlave4.GetClientRect(&rect);
		if (m_bConnected[4] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[4] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[4] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[4] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[4] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else if (m_bConnected[4] == 6)
			color = RGB(0,0,0);     //turn black if failed writing wall data file
		else if (m_bConnected[4] == 7)
			color = RGB(255,255,255);     //turn white if 12-clock is wrong
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS4.FillSolidRect (&rect, color);
		dcS4.TextOut(x,0,_T("I4"));

	#if (SYSTEM_SUB_ID == LORAINE_SYS)
		m_btnSlave5.GetClientRect(&rect);
		if (m_bConnected[5] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[5] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[5] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[5] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[5] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS5.FillSolidRect (&rect, color);
		dcS5.TextOut(x,0,_T("I5"));

		m_btnSlave6.GetClientRect(&rect);
		if (m_bConnected[6] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[6] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[6] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[6] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[6] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS6.FillSolidRect (&rect, color);
		dcS6.TextOut(x,0,_T("I6"));

		m_btnSlave7.GetClientRect(&rect);
		if (m_bConnected[7] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[7] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[7] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[7] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[7] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS7.FillSolidRect (&rect, color);
		dcS7.TextOut(x,0,_T("I7"));

		m_btnSlave8.GetClientRect(&rect);
		if (m_bConnected[8] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[8] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[8] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[8] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[8] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS8.FillSolidRect (&rect, color);
		dcS8.TextOut(x,0,_T("I8"));

		m_btnSlave9.GetClientRect(&rect);
		if (m_bConnected[9] == 1)  
			color = RGB(0,255,0);  //turn green if connected AND hardware found AND config file loaded
		else if (m_bConnected[9] == 2)
			color = clrBlue;       //turn blue if connected AND hardware found AND no config file loaded
		else if (m_bConnected[9] == 3)
			color = clrYell;       //turn yellow if connected AND no hardware found
		else if (m_bConnected[9] == 4)
			color = clrSlave;       //turn cyan if slave instrument
		else if (m_bConnected[9] == 5)
			color = clrMaster;       //turn magenta if master instrument
		else
			color = RGB(255,0,0);  //turn red if not connected
		dcS9.FillSolidRect (&rect, color);
		dcS9.TextOut(x,0,_T("I9"));
	#endif
#endif
	}
	else if (0 /*m_bLastShowNetAlarm*/)
	{
			m_btnMaster.ShowWindow(SW_HIDE);
			m_btnSlave1.ShowWindow(SW_HIDE);
			m_btnSlave2.ShowWindow(SW_HIDE);
			m_btnSlave3.ShowWindow(SW_HIDE);
			m_btnSlave4.ShowWindow(SW_HIDE);
			m_btnSlave5.ShowWindow(SW_HIDE);
			m_btnSlave6.ShowWindow(SW_HIDE);
			m_btnSlave7.ShowWindow(SW_HIDE);
			m_btnSlave8.ShowWindow(SW_HIDE);
			m_btnSlave9.ShowWindow(SW_HIDE);
			m_bLastShowNetAlarm = FALSE;
	}
}


BOOL CTscanDlg::IsSyncNormal()
{
	// If there are more than one master instrments, or no master instrument,
	// or the only master instrument is not instrument 1, return FALSE
	int i;
	int nMasterCnt=0;
	int nExtTtlCnt=0;
	int nMasterId=-1;
	int nExtTtlId=-1;
	int nMaxShoes;

#if (SYSTEM_ID == LORAINE_SYS)
	#if (SYSTEM_SUB_ID == LORAINE_SYS)
		nMaxShoes = MAX_SHOES;
	#endif
	#if (SYSTEM_SUB_ID == BESSEMER_SYS)
		nMaxShoes = 3;
	#endif
#else
	nMaxShoes = 2;
#endif

	for (i=0; i<nMaxShoes-1; i++)
	{
		if (ConfigRec.pulser.LocalPRF[i] == 1)
		{
			if (nMasterId == -1)
				nMasterId = i;
			nMasterCnt++;
		}
		else if (ConfigRec.pulser.LocalPRF[i] == 3)
		{
			if (nExtTtlId == -1)
				nMasterId = i;
			nExtTtlCnt++;
		}
	}

	if ( (nMasterCnt >= 1) && (nExtTtlCnt >= 1) )
		return FALSE;

	if ( nMasterCnt != 1 )
		return FALSE;
	else if (nMasterId != 0)
		return FALSE;
	else
		return TRUE;
}

void CTscanDlg::UpdateUndoBuffer()
{
	if (m_pTcgSetupDlg)
		memcpy( (void *) m_pTcgSetupDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (CGateDlg::m_pDlg != NULL)
		memcpy( (void *) CGateDlg::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (CRcvr::m_pDlg != NULL)
		memcpy( (void *) CRcvr::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (Ctof::m_pDlg != NULL)
		memcpy( (void *) Ctof::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (CPulser::m_pDlg != NULL)
		CPulser::m_pDlg->UpdateUndoBuffer();

	if (CWCalDlg::m_pDlg != NULL)
		CWCalDlg::m_pDlg->UpdateUndoBuffer();

	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
}


void CTscanDlg::UpdateUndoBuffer2()
{
	if (m_pTcgSetupDlg)
		memcpy( (void *) m_pTcgSetupDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (CGateDlg::m_pDlg != NULL)
		memcpy( (void *) CGateDlg::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (CRcvr::m_pDlg != NULL)
		memcpy( (void *) CRcvr::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (Ctof::m_pDlg != NULL)
		memcpy( (void *) Ctof::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC) );

	if (CPulser::m_pDlg != NULL)
		memcpy ( (void *) CPulser::m_pDlg->m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));

	if (CWCalDlg::m_pDlg != NULL)
		CWCalDlg::m_pDlg->UpdateUndoBuffer();

	memcpy ( (void *) m_pUndo, (void *) &ConfigRec, sizeof(CONFIG_REC));
}


CONFIG_REC* CTscanDlg::GetUndoBuffer()
{
	return m_pUndo;
}


void CTscanDlg::OnFilePrintscreen2() 
{
	// TODO: Add your command handler code here
//return;

	// structure like truscope PrintTruscopeScreen function
	// calls dibapi32.dll routines to create dib and print dib.


	CClientDC dcScreen(NULL);		// context of the whole screen
	CDC dcMem;						// memory device context for screen
	CBitmap bitmapScreen;			// bit map for the screen
	CBitmap *oldBitMap;				// used to clean up process

	HPALETTE hPal;

	HDIB hDIB;
	HDC hDC;						// handle of default printer


	int nPixelHeightScreen;			// how high is screen area in pixels
	int nPixelWidthScreen;
	int dy;						// initial y height
	int cxChar, cyChar;				// char size in pixels
	BOOL q;							// debug only
	int nDefects;				// defects left to print
//    int nPageCnt;               // number of pages printed
//    int nDefectsPrinted;        // how many printed on this page

	TEXTMETRIC tm;					// struct which hold characteristics of screen

	
	GetPrinter();		// get default printer resources
	hDC = m_dcPrn->m_hDC;

	dy = nPixelHeightScreen	= dcScreen.GetDeviceCaps(VERTRES);
	nPixelWidthScreen		= dcScreen.GetDeviceCaps(HORZRES);

	dcScreen.GetTextMetrics( &tm);
	cyChar = tm.tmHeight + tm.tmExternalLeading;
	cxChar = tm.tmAveCharWidth;

	// If we are about to print inspection screen, leave room for flaw text
	if (m_pInspectDlg2)
	{
		//if (m_pInspectDlg2->m_pFlawReport != NULL)
			//m_pInspectDlg2->m_pFlawReport->ShowWindow(SW_HIDE);
		nPixelHeightScreen += 28*cyChar;	// 27 rows of text..28 on 7/27/00
		nDefects = m_pInspectDlg2->GetTotalDefects();
	}

	q = bitmapScreen.CreateCompatibleBitmap(&dcScreen,nPixelWidthScreen,
						nPixelHeightScreen);	// make bitmap fit screen

	
	q = dcMem.CreateCompatibleDC(&dcScreen);	// make memory compatible
											// with the screen

	oldBitMap = dcMem.SelectObject(&bitmapScreen);		// put bit map into memory context
	q = dcMem.PatBlt(0, 0, nPixelWidthScreen, nPixelHeightScreen,
					WHITENESS);				// erase memory context
	// Copy screen to memory
	q = dcMem.BitBlt(0, 0, nPixelWidthScreen, dy,
					&dcScreen, 0, 0, SRCCOPY);

	if (m_pInspectDlg1 && m_pInspectDlg2)
	{
		//CRect rect1;
		//CWindowDC dcInspect2(m_pInspectDlg2);
		//m_pInspectDlg1->GetWindowRect(&rect1);
		//q = dcMem.BitBlt(rect1.left, rect1.top, rect1.Width(), rect1.Height(),
						//&dcInspect2, 0, 0, SRCCOPY);
	}
	// Copy in the text if it exists
	if (m_pInspectDlg2)
	{
		// A little snaky here.  I would like the data printing to
		// be based in the inspect dialog.  Therefore, call a routine
		// in the inspect class to empty the data queues.
//        nDefectsPrinted = m_pInspectDlg2->AddFlawTextToDib(&dcMem, cxChar, cyChar, dy, 1);
	}

	hPal = GetSystemPalette();	

	hDIB = BitmapToDIB( (HBITMAP) bitmapScreen, hPal);

	// Had to compile dibapi32.dll with mfc as project
	// Since using best fit, 20,20 (x,y scaling) not used
	q = PrintDIB(hDIB, PW_BESTFIT, 20,20,"Best Fit", hDC);

	DestroyDIB(hDIB);	// delete the dib returned by the dll

	// 04/25/01 Reuse pieces to print text only if still more
	// defects to print.  No graphics at top of page, only 
	// columns of flaw text
//    if (m_pInspectDlg2)
//    {
//        nDefects -= nDefectsPrinted;
//        if (nDefects < 0) nDefects = 0;
//        nPageCnt = 1;   // already printed inspect screen
//        while ( (nDefects) && (nPageCnt < 6) )
//        {
//            q = dcMem.PatBlt(0, 0, nPixelWidthScreen, nPixelHeightScreen,
//                    WHITENESS);             // erase memory context
//
//            dy = 0;     // no graphics, use whole context for text
//
//            nDefectsPrinted = m_pInspectDlg2->AddFlawTextToDib(&dcMem, cxChar, cyChar, dy, 0);  // 55 rows
//            hDIB = BitmapToDIB( (HBITMAP) bitmapScreen, hPal);
//           q = PrintDIB(hDIB, PW_BESTFIT, 20,20,"Best Fit", hDC);
//            DestroyDIB(hDIB);   // delete the dib returned by the dll
//            nPageCnt++;
//            nDefects -= nDefectsPrinted;
//            if (nDefects < 0) nDefects = 0;
//        }
//
//        m_pInspectDlg2->RestoreFlawLists();
//        //if (m_pInspectDlg2->m_pFlawReport != NULL)
//            //m_pInspectDlg2->m_pFlawReport->ShowWindow(SW_SHOW);
//    }
//
	// 12/11/00 try to stop case of using up all memory in system
	DeleteObject(hPal);	// delete the palette
	DestroyDIB(hDIB);	// delete the dip returned by the dll
	dcMem.SelectObject(oldBitMap);	// deselect bitmap before deleting
	bitmapScreen.DeleteObject();	// delete screen bit map
	dcMem.DeleteDC();			// delete memory dc
	ReleasePrinter();
}


void CTscanDlg::CloseInstruments()
{
	if (CRcvr::m_pDlg) CRcvr::m_pDlg->OnOK();
	if (CGateDlg::m_pDlg) CGateDlg::m_pDlg->OnOK();
//	if (CSysGain::m_pDlg) CSysGain::m_pDlg->OnOK();
	if (CPulser::m_pDlg) CPulser::m_pDlg->OnOK();
	if (Ctof::m_pDlg) Ctof::m_pDlg->OnOK();
	if (CTCG::m_pDlg) CTCG::m_pDlg->KillMe();
	if (m_pAscanDlg) m_pAscanDlg->KillMe();

#ifndef  TWO_CHANNEL_4_GATE_BOARD
	if (COscpe::m_pDlg) COscpe::m_pDlg->OnOK();
#else
	//if (pCTscanDlg->m_pScopeDlg) (pCTscanDlg->m_pScopeDlg)->KillMe();
	if (pCTscanDlg->m_pTcgSetupDlg != NULL) (pCTscanDlg->m_pTcgSetupDlg)->KillMe();
#endif
}


void CTscanDlg::OnViewConfig() 
{
	if ( !m_szConfigFileName.IsEmpty()  )
	{
        if (CViewcfg::m_pDlg == NULL)
        {
                CViewcfg::m_pDlg = new CViewcfg;
                if (CViewcfg::m_pDlg)
                {
                        CViewcfg::m_pDlg->Create(IDD_VIEWCFG_DIALOG);
                }
        }
	}
	else
	{
        MessageBox(_T("No Configuration File Loaded"));
	}
        
}

void CTscanDlg::OnPrintCfg() 
{
	if (CViewcfg::m_pDlg == NULL)
	{
		MessageBox(_T("Please select View Configuration first."));
		return;
	}

	if ( !m_szConfigFileName.IsEmpty() )
	{
        CViewcfg::m_pDlg->OnFilePrintcfg(); 
	}
	else
	{
        MessageBox(_T("No Configuration File Loaded"));
	}
}       

void CTscanDlg::SwitchInspectWindows(int nWindow)
{
	// nWindow: the ID of the window that will be moved to right
	int xLeft = 1;
	int yLeft = 120;
	int xRight = 1;//1025;
	int yRight = 125;

	switch (nWindow)
	{
	case 0:  // inspect window 1
		if (m_pInspectDlg1)
		{
			m_pInspectDlg1->SetWindowPos (NULL, xRight, yRight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			m_pInspectDlg1->MoveReportWindow(xRight+800, yRight+125);
		}
		if (m_pInspectDlg2)
		{
			m_pInspectDlg2->SetWindowPos (NULL, xLeft, yLeft, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			m_pInspectDlg2->MoveReportWindow(xLeft+800, yLeft+125);
		}
		break;

	case 1:  // inspect window 2
		if (m_pInspectDlg2)
		{
			m_pInspectDlg2->SetWindowPos (NULL, xRight, yRight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			m_pInspectDlg2->MoveReportWindow(xRight+800, yRight+125);
		}
		if (m_pInspectDlg1)
		{
			m_pInspectDlg1->SetWindowPos (NULL, xLeft, yLeft, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			m_pInspectDlg1->MoveReportWindow(xLeft+800, yLeft+125);
		}
		break;

	default:
		break;
	}
}

BOOL CTscanDlg::PreTranslateMessage(MSG* pMsg)
{
	CToolInfo toolinfo;
	CWnd *pWndBtn;
	CString sTmp;
	int nWhichBtn;

	// figure out which button caused this event
	pWndBtn = WindowFromPoint(pMsg->pt);

	switch(pWndBtn->GetDlgCtrlID())
	{
		case IDC_BUTTON_MASTER: nWhichBtn = 0; break;  // Master
		case IDC_BUTTON_SLAVE1: nWhichBtn = 1; break;
		case IDC_BUTTON_SLAVE2: nWhichBtn = 2; break; 
		case IDC_BUTTON_SLAVE3: nWhichBtn = 3; break;
		case IDC_BUTTON_SLAVE4: nWhichBtn = 4; break;
		case IDC_BUTTON_SLAVE5: nWhichBtn = 5; break;
		case IDC_BUTTON_SLAVE6: nWhichBtn = 6; break;
		case IDC_BUTTON_SLAVE7: nWhichBtn = 7; break;
		case IDC_BUTTON_SLAVE8: nWhichBtn = 8; break;
		case IDC_BUTTON_SLAVE9: nWhichBtn = 9; break;
		default: nWhichBtn = 1000; break;
	}

	if (nWhichBtn < 1000)
	{
		switch(m_bConnected[nWhichBtn])
		{
		case 0:  //no connection
			sTmp.Format(_T("Connection is not established."));
			break;
		case 1:  //no connection
			sTmp.Format(_T("Normal status"));
			break;
		case 2: 
			sTmp.Format(_T("Config file is not loaded."));
			break;
		case 3:
			sTmp.Format(_T("Hardware was not found."));
			break;
		case 4:
			if (nWhichBtn == 1)
				sTmp.Format(_T("Slave instrument. Instrument 1 should be set to sync master."));
			else
				sTmp.Format(_T("Slave instrument."));
			break;
		case 5:
			if (nWhichBtn == 1)
				sTmp.Format(_T("Master instrument. You can't have more than one sync master and/or external TTL."));
			else
				sTmp.Format(_T("Master instrument. Instrument 2-9 should be set to sync slave."));
			break;
#if ( (SYSTEM_ID == RAW_WALL_SYS) || RECORD_RAW_WALL )
		case 6:
			sTmp.Format(_T("Failed writing the wall data file."));
			break;
		case 7:
			sTmp.Format(_T("Twelve clock sensor is not working properly or PRF is not right."));
			break;
#endif
		default:
			sTmp.Format(_T("Unknown status"));
			break;
		}

		m_tooltip.GetToolInfo(toolinfo, pWndBtn, 0);
		_tcscpy(toolinfo.szText, (LPCTSTR) sTmp);
		m_tooltip.SetToolInfo(&toolinfo);
	}

	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

#define MAX_HISTOGRAM_WALL 500
void CTscanDlg::OnFileWallConvert() 
{
	// TODO: Add your command handler code here
	// Code analyzer says too much data on stack
// Function uses '19264' bytes of stack: exceeds /analyze:stacksize'16384'. Consider moving some data to heap
	CString s, t, str;
	RAW_WALL_HEAD head_data;
	WALL_REVOLUTION *pOneRev = new WALL_REVOLUTION;	// make on heap instead of stack jeh 7-27-12
	Ftext *ftext;
	CString sNumReading, sTmp;
	char ch[128];
	WORD WallHis[MAX_HISTOGRAM_WALL];
	WORD nWall;
	float MinCSA = 100000.0f;


	FILE *myfile, *OutFile, *NumFile, *HisFile, *CsaFile;
	int result =0;
	int i = 0;
	int j = 0;
	int k = 0;
	WORD largest_nAscan = 2000;
	int  nMaxNumReadings = 0;    // maximum number of readings for one revolution
	int  nMinNumReadings = 20000;    // minimum number of readings for one revolution
	int  nAveNumReadings = 0;    // Average number of readings for one revolution

	for (i=0; i<MAX_HISTOGRAM_WALL; i++)
		WallHis[i] = 1;

	TCHAR szFilter[] = _T("Wall Data Files(*.dat) | *.dat||");


	CFileDialog dlg (TRUE, _T("dat"), _T("*.dat"),OFN_FILEMUSTEXIST, szFilter);

	if ( dlg.DoModal() == IDOK)
	{
		s = dlg.GetPathName();
		CstringToChar(s,ch);
 		if( (myfile = fopen(ch, "rb")) != NULL )
		{
            ftext = new Ftext(this, 1);
            if (ftext) ftext->Create(IDD_Dtext);
			ftext->SetWindowText(_T("# of Readings Per Rev"));
			ftext->GetDlgItem(IDC_STATIC_LABEL)->SetWindowText(_T("Rev  Ch1  Ch2  Ch3  Ch4"));

			CWaitCursor WaitCursor;
			result = fread(&head_data, sizeof(RAW_WALL_HEAD), 1, myfile);
			float OD = head_data.fOD;
			float Wall = head_data.fWall;
			float NominalCSA = 3.141593f*(OD-Wall)*Wall;

			/* Open file for write of csv data  */
			s.TrimRight(_T(".dat"));
			t.Format(_T(".csv"));
			s += t;
			CstringToChar(s,ch);
			if( (OutFile = fopen( ch, "w+" )) == NULL )
			AfxMessageBox( _T("The wall data file was not opened") );
			/* Open file for write of #-of-readings file  */
			s.TrimRight(_T(".csv"));
			t.Format(_T(".num"));
			s += t;
			CstringToChar(s,ch);
			if( (NumFile = fopen( ch, "w+" )) == NULL )
			AfxMessageBox( _T("The number-of-readings file was not opened" ));
			/* Open file for write of wall histogram file  */
			s.TrimRight(_T(".num"));
			t.Format(_T(".his"));
			s += t;
			CstringToChar(s,ch);
			if( (HisFile = fopen( ch, "w+" )) == NULL )
			AfxMessageBox( _T("The wall histogram file was not opened") );
			/* Open file for write of cross-sectional area file  */
			s.TrimRight(_T(".his"));
			t.Format(_T(".csa"));
			s += t;
			CstringToChar(s,ch);
			if( (CsaFile = fopen( ch, "w+" )) == NULL )
			AfxMessageBox( _T("The wall cross-sectional area file was not opened") );
 
	//	printf(" joint number is %d \n", head_data.nJointNum);
	//	printf(" wall thickness is %5.3f \n", head_data.fWall);
	//	printf(" wall OD %5.3f \n", head_data.fOD);
	//	printf(" actual number of revolutions is %d \n", head_data.nNumRev);

	//			_ftprintf(OutFile, _T(" joint number is %d \n", head_data.nJointNum);
	//			_ftprintf(OutFile, _T(" wall thickness is %5.3f \n", head_data.fWall);
	//			_ftprintf(OutFile, _T(" pipe OD %5.3f \n", head_data.fOD);

			_ftprintf(OutFile, _T("Actual number of revolutions stored in file is %d \n"), 
					head_data.nNumRev);
			_ftprintf(OutFile, _T("Work Order:  %s \n"), head_data.JobRec.WO);
			_ftprintf(OutFile, _T("Motion Bus:  0x%04X \n"), head_data.nMotionBus);
			_ftprintf(OutFile, _T("Home Offset: %d in.\n"), head_data.JobRec.nHomeXOffset);
			_ftprintf(OutFile, _T("Away Offset: %d in.\n"), head_data.JobRec.nAwayXOffset);
			_ftprintf(OutFile, _T("Inspect Stop at: %d in.\n"), head_data.JobRec.nStopXloc);
			_ftprintf(OutFile, _T("\n"));


			_ftprintf(OutFile, _T("XLoc, AScans Per Channel for this revolution \n"));

 		
			for (j=0; j<head_data.nNumRev; j++)
			{	
				sNumReading.Format(_T("%4d"),j);
				_ftprintf(NumFile, _T("%d"), j);
				_ftprintf(CsaFile, _T("%d"), j);

				result = fread((void *) pOneRev, sizeof(WALL_REVOLUTION), 1, myfile);

					largest_nAscan = 0;
					for (i=0; i < NUM_WALL_CHANNEL; i++)
					{
						sTmp.Format(_T("  %4d"),pOneRev->nNumAscan[i]);
						sNumReading +=sTmp;

						if (pOneRev->nNumAscan[i] > largest_nAscan)
							largest_nAscan = pOneRev->nNumAscan[i];
						if ( (j>9) && (j<head_data.nNumRev-10) )
							nAveNumReadings += pOneRev->nNumAscan[i];

						_ftprintf(NumFile, _T(",%d"), pOneRev->nNumAscan[i]);
						_ftprintf(CsaFile, _T(",%6.2f"), pOneRev->fCrossArea[i]);

						for (k=0; k<pOneRev->nNumAscan[i]; k++)
						{
							nWall = pOneRev->WallAscan[k].Wall[i];
							if ( (nWall>=0) && (nWall<MAX_HISTOGRAM_WALL) )
								WallHis[nWall]++;
						}

						// Find minimum cross-sectional area
						if (pOneRev->fCrossArea[i] < MinCSA)
							MinCSA = pOneRev->fCrossArea[i];
					}
					_ftprintf(NumFile,_T("\n"));
					_ftprintf(CsaFile,_T("\n"));
					if ( (j>9) && (j<head_data.nNumRev-10) )
					{
						if (nMaxNumReadings < largest_nAscan)
							nMaxNumReadings = largest_nAscan;
						if (nMinNumReadings > largest_nAscan)
							nMinNumReadings = largest_nAscan;
					}

					ftext->m_LBtext.AddString(sNumReading);

	/*
					_ftprintf(OutFile, _T("%5d,", pOneRev->nXloc);
					for (i=0; i < (NUM_WALL_CHANNEL - 1); i++)
					{ 
						_ftprintf(OutFile, _T("%5d,", pOneRev->nNumAscan[i]);
					}
					_ftprintf(OutFile, _T("%5d,\n", pOneRev->nNumAscan[(NUM_WALL_CHANNEL - 1)]);
	*/				

					if (j>=1312)
					{
						_ftprintf(OutFile, _T("Rev# %d, Number of readings for this revolution = %d\n"), j+1, largest_nAscan);
						for(i=0; i < int(largest_nAscan); i++)
						{
					
				
							_ftprintf(OutFile, _T("%5d,%5d,%5d,%5d,%5d \n"), pOneRev->nXloc,
								pOneRev->WallAscan[i].Wall[0], 
								pOneRev->WallAscan[i].Wall[1], pOneRev->WallAscan[i].Wall[2],
								pOneRev->WallAscan[i].Wall[3]);


						}
						_ftprintf(OutFile, _T("\n\n\n"));
					}
				}

				for (k=0; k<MAX_HISTOGRAM_WALL; k++)
					_ftprintf( HisFile, _T("%f, %f\n"), (float) k/1000.0f, log10((double) WallHis[k]) );

				fclose(myfile);
				fflush(OutFile);
				fclose(OutFile);
				fclose(NumFile);
				fclose(CsaFile);
				fclose(HisFile);

				if (head_data.nNumRev > 20)
				{
					nAveNumReadings /= ( (head_data.nNumRev-20)*NUM_WALL_CHANNEL);
					str.Format(_T("Maximum # of readings per revolution: %d\nMinimum # of readings per revolution: %d\nAverage # of readings per revolution: %d\nCustomer Wall Classification: %6.1f")
						,nMaxNumReadings, nMinNumReadings, nAveNumReadings, 100.0f*MinCSA/NominalCSA);
					AfxMessageBox(str);
				}

				ftext->ShowWindow(SW_SHOW);
		}
		else  	
		{
			AfxMessageBox(_T("output file open failed.  Processing terminated."));
		}

		getchar();
	}

	delete pOneRev;

}	// OnFileWallConvert() 



void CTscanDlg::OnUpdateFileWallConvert(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
#if (SYSTEM_ID != RAW_WALL_SYS)
	pCmdUI->Enable(FALSE);  // disable
#endif
}

void CTscanDlg::OnBtnNextJoint() 
{
	// TODO: Add your control notification handler code here
	OnSetupJointnumber();
}

void CTscanDlg::EnableInstrumentPanels() 
{
	// TODO: Add your command handler code here
	BOOL bEnable = TRUE;

	if (m_pInspectDlg1)
		if ( (m_pInspectDlg1->m_nControlSel != INSPECT_STOP) 
			&& (m_pInspectDlg1->m_nTimeMotion == INSPECT_MOTION) )
			bEnable = FALSE;

	if (CGateDlg::m_pDlg)
		CGateDlg::m_pDlg->EnableWindow(bEnable);

	if (CRcvr::m_pDlg)
		CRcvr::m_pDlg->EnableWindow(bEnable);

	if (CPulser::m_pDlg)
		CPulser::m_pDlg->EnableWindow(bEnable);

	if (Ctof::m_pDlg)
		Ctof::m_pDlg->EnableWindow(bEnable);

	//if (m_pTcgSetupDlg)
		//m_pTcgSetupDlg->EnableWindow(bEnable);

}


void CTscanDlg::SaveOriginalConfigRec()
{
	if (!m_pOriginalConfig)
	{
		m_pOriginalConfig = new CONFIG_REC;
		memcpy ( (void *) m_pOriginalConfig, (void *) &ConfigRec, sizeof(CONFIG_REC));
	}
}


void CTscanDlg::RestoreOriginalConfigRec()
{
	if (m_pOriginalConfig)
	{
		memcpy ( (void *) &ConfigRec, (void *) m_pOriginalConfig, sizeof(CONFIG_REC));
		delete m_pOriginalConfig;
		m_pOriginalConfig = NULL;

		ConfigToStatScreen();
	}
}


void CTscanDlg::SaveMyWindowPosition()
{
    WINDOWPLACEMENT wp;

    // Save closing location of window
    GetWindowPlacement(&wp);
    SaveWindowLastPosition(_T("TSCANDLG"), &wp);
}


void CTscanDlg::GetMyWindowPosition()
{
    WINDOWPLACEMENT wp;
    RECT rect;
    int dx, dy;     // width and height of original window
    
    GetWindowPlacement(&wp);
    dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
    GetWindowLastPosition(_T("TSCANDLG"), &rect);

    if ( ( (rect.right - rect.left) >= dx ) &&
         ( (rect.bottom - rect.top) >= dy ))
    {
        wp.rcNormalPosition = rect;
        //SetWindowPlacement(&wp);
    }

	if ( (rect.left < 2048) && (rect.left >= 0) && (rect.top < 500) && (rect.top >= 0))
		SetWindowPos (NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	else
		SetWindowPos (NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


BOOL CTscanDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::PreCreateWindow(cs);
}


void CTscanDlg::SetOblqName()
{
	XdcrText[4] = ConfigRec.JobRec.Oblq1Name;
	XdcrText[5] = ConfigRec.JobRec.Oblq2Name;
	XdcrText[6] = ConfigRec.JobRec.Oblq3Name;
}

void CTscanDlg::OnInstrumentAscan() 
{
	// TODO: Add your command handler code here
	if (NULL == m_pTcpThreadRxList)		return;


    if (m_pAscanDlg != NULL)
	{
        m_pAscanDlg->SetFocus ();
		m_pAscanDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pAscanDlg = new CAscanDlg(this);
		m_pAscanDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
		m_pAscanDlg->Create(IDD_ASCAN, NULL);
		m_pAscanDlg->ShowWindow(SW_SHOW);
	}

	if (m_bConfigLoaded)
		SendMsg(ASCAN_MODE);
}


void CTscanDlg::OnDisplayChannelTraces() 
{
	// TODO: Add your command handler code here
    if (m_pChnlTraceDlg != NULL)
	{
        m_pChnlTraceDlg->SetFocus ();
		m_pChnlTraceDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pChnlTraceDlg = new CChnlTraceDlg(this);
		//m_pChnlTraceDlg->m_pTcpThreadRxList = m_pTcpThreadRxList;
		m_pChnlTraceDlg->Create(IDD_CHNL_TRACE, NULL);
		m_pChnlTraceDlg->ShowWindow(SW_SHOW);
	}
}

void CTscanDlg::OnChangeStatGrade() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	StatScreenToConfig();
}

void CTscanDlg::OnBtnPipePresent() 
{
	// TODO: Add your control notification handler code here
	m_nPipeStatus = ~m_nPipeStatus;

	if (m_nPipeStatus)
		GetDlgItem(IDC_BTN_PIPEPRESENT)->SetWindowText(_T("Pipe Present"));
	else
		GetDlgItem(IDC_BTN_PIPEPRESENT)->SetWindowText(_T("Pipe Absent"));

	m_pTcpThreadRxList->SendSlaveMsg(SET_PIPE_PRESENT, m_nPipeStatus, 0, 0, 0, 0, 0);
}

void CTscanDlg::OnInstrumentAscanReadSeq() 
{
	// TODO: Add your command handler code here
	CAscanReadSeq dlg;
	dlg.m_pTcpThreadRxList = m_pTcpThreadRxList;
	dlg.DoModal();
}


#define UM_MESSAGEBOX WM_USER + 100

void ShutDownSystem(  )
{

	TCHAR pName[ 128 ] = _T("Celle");
	TCHAR pPasswd[ 128 ] = _T("pxi");
	TCHAR pDomain[ 128 ] = _T("PXI");

	TCHAR pRemoteName[ 260 ] = _T("PhasedArray");
	TCHAR pMessage[ 512 ] = { 0 };

	HANDLE hLogonToken;
	HANDLE hAdminToken;
	HANDLE hThreadToken;
	TOKEN_PRIVILEGES tkp;


	if( FALSE == LogonUser( pName, pDomain, pPasswd,
							LOGON32_LOGON_INTERACTIVE,
							LOGON32_PROVIDER_DEFAULT,
							&hLogonToken ) )
	{	
		return;
	}

	if( FALSE == DuplicateTokenEx( hLogonToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hAdminToken ) )
		return;

	if( FALSE == ImpersonateLoggedOnUser( hAdminToken ) )
		return;

 	if( FALSE == OpenThreadToken( GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, TRUE, &hThreadToken ) )
	{
		RevertToSelf();
		return;
	}

	// Get the LUID for shutdown privilege.
	LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid );

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get shutdown privilege for this thread.
	AdjustTokenPrivileges( hThreadToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 ); 
	if( GetLastError() != ERROR_SUCCESS )
	{
		RevertToSelf();
		return;
	}


	if( 1 )
	{
		//if( FALSE == InitiateSystemShutdown( pRemoteName, pMessage, pDlg->m_timeOut, FALSE, FALSE ) )
		if( FALSE == InitiateSystemShutdown( NULL, NULL, 20, FALSE, FALSE ) )
		{
			RevertToSelf();
			return;
		}
	}
	else
	{
		if( FALSE == AbortSystemShutdown( pRemoteName ) )
		{
			RevertToSelf();
			return;
		}
	}

	// Disable shutdown privilege.
	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges( hThreadToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );
	if( GetLastError() != ERROR_SUCCESS )
	{
		RevertToSelf();
		return;
	}

	RevertToSelf();
}

void CTscanDlg::OnBtnStartMaster() 
{
#if 0

	// TODO: Add your control notification handler code here
    static struct sockaddr_in  serverAddr;    /* server's socket address */ 
    static struct sockaddr_in  clientAddr;    /* client's socket address */ 
    int                 sockAddrSize;  /* size of socket address structure */ 
    static int          sFd;           /* socket file descriptor */  
    int                 optval = 1;    /* socket option value */
	char buf[102];
	static BOOL bFirstCall = TRUE;
	static BOOL bMasterStarted = FALSE;

    sockAddrSize = sizeof (struct sockaddr_in); 

	if (bFirstCall)
	{
		bFirstCall = FALSE;
		/* create client's socket */ 
 
		if ((sFd = socket (AF_INET, SOCK_DGRAM, 0)) < 0) 
		{ 
			AfxMessageBox ("udp - socket call failed!!"); 
			return; 
		} 
 
		/* set a sockset option so that it can broadcast messgaes */
   
		setsockopt (sFd, SOL_SOCKET, SO_BROADCAST, (const char FAR *) &optval, sizeof (optval));

		/* build client socket address */ 
 
		memset ((char *) &clientAddr, 0, sockAddrSize); 
		clientAddr.sin_family = AF_INET; 
		clientAddr.sin_port = htons (8503); 
		clientAddr.sin_addr.s_addr = inet_addr ("192.168.10.10");

		/* bind not required - port number is dynamic */ 
 
		if (bind (sFd, (struct sockaddr *) &clientAddr, sockAddrSize) < 0) 
		{ 
			AfxMessageBox (_T("udp - bind call failed!!")); 
			closesocket (sFd); 
			return; 
		} 

		/* build server socket address */ 
 
		memset((char *) &serverAddr, 0, sockAddrSize); 
		serverAddr.sin_family = AF_INET; 
		serverAddr.sin_port = htons (8502); 
		serverAddr.sin_addr.s_addr = inet_addr ("192.168.10.255");  /* to all hosts on the network 192.168.20 */
	}

	int i,j;

	for (i=0; i<6; i++)
	{
		buf[i] = 0xFF;
	}

	for (i=1; i<=16; i++)
	{
		buf[i*6+0] = 0x00;
		buf[i*6+1] = 0x26;
		buf[i*6+2] = 0x55;
		buf[i*6+3] = 0xC3;
		buf[i*6+4] = 0x81;
		buf[i*6+5] = 0x36;
	}

	if (sendto (sFd, (char *) buf, 102, 0, 
	    (struct sockaddr *) &serverAddr, sockAddrSize) == SOCKET_ERROR) 
	{ 
		AfxMessageBox (_T("udp - sendto call failed!!"));
	    closesocket (sFd);
		return;
	} 

	bMasterStarted = !bMasterStarted;

	if (!bMasterStarted)
	{
		SetDlgItemText(IDC_BTN_STARTMASTER, _T("Turn on Master"));
		//ShutDownSystem();
		/*
		if (::InitiateSystemShutdown(NULL, NULL, 20, TRUE, FALSE) == 0)
		{
			LPVOID lpMsgBuf;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
						   FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
						   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						   (LPTSTR)&lpMsgBuf, 0, NULL );

			SendMessage( UM_MESSAGEBOX, (WPARAM)lpMsgBuf, (LPARAM)( MB_OK | MB_ICONEXCLAMATION ) );

		}
		*/
		m_pTcpThreadRxList->SendSlaveMsg(TURN_OFF_MASTER, 0, 0, 0, 0, 0, 0);
	}
	else
		SetDlgItemText(IDC_BTN_STARTMASTER, _T("Turn off Master"));

#endif

	static int nShowWall = 1;

	if (nShowWall == 1)
	{
		nShowWall = 0;
		m_pTcpThreadRxList->SendSlaveMsg(TURN_OFF_MASTER, 0, 0, 0, 0, 0, 0);
		SetDlgItemText(IDC_BTN_STARTMASTER, _T("Amplitude"));
	}
	else
	{
		nShowWall = 1;
		m_pTcpThreadRxList->SendSlaveMsg(TURN_OFF_MASTER, 1, 0, 0, 0, 0, 0);
		SetDlgItemText(IDC_BTN_STARTMASTER, _T("Wall"));
	}

}

// Merge into Mill Console
void CTscanDlg::AddDebugMessage(CString sMsg)
	{
	CString stmp0;
	//int i;

#if 0
	stmp0.Format(_T("%d"), ++m_DebugMessageCount);
	i = m_DebugMessageCount;
	m_DebugMessages.InsertItem(i, stmp0);
	m_DebugMessages.SetItemText(i-1, 1, sMsg);
	m_DebugMessages.EnsureVisible(i-1, FALSE);
#endif
	stmp0 = sMsg + _T("\n");
	TRACE(stmp0);	
	}


// jeh.. a CCM class posts a windows message for the main dlg to check 
// Received packet linked list associated with the wParam
// Received packets are processed at thread priority of main dlg.

LRESULT CTscanDlg::GetReceivedPackets(WPARAM wWhichList, LPARAM lParam)
	{
	if (wWhichList >= MAX_CLIENTS)
		{
		TRACE("Which List parameter >= MAX_CLIENTS\n");
		return 1;
		}

	switch (wWhichList)
		{
	case 0:		
		if (pCCM_SysCp) 
			pCCM_SysCp->ProcessReceivedMessage();			
		break;
	default:
		if (pCCM[wWhichList])
		pCCM[wWhichList]->ProcessReceivedMessage();		
		break;

		}

		return 0;
	}

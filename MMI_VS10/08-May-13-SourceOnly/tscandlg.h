// TscanDlg.h : header file
// Renamed TscanDlg.h on 5/30/00 jeh
//

#if !defined(AFX_ASIDLG_H__E654F6C7_C864_11D3_9B48_00A0C91FC401__INCLUDED_)
#define AFX_ASIDLG_H__E654F6C7_C864_11D3_9B48_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "winsock2.h"		// manually by jeh.  Does not work if
							// winsock support selected when project
							// created!!
#include "wsipx.h"


/****************** SPECIAL CONTINUOUS REPLAY MODE FOR OTC, 4-30-02  *********************/


#define		OTC_CONTINUOUS_LOOP		0



/****************** SPECIAL CONTINUOUS REPLAY MODE FOR OTC, 4-30-02  *********************/


#define MMI_AS_SERVER_IP_ADDR		"192.168.10.10"		/* Server for the Phased Array Master(s) */
#define MMI_AS_SERVER_IP_PORT		7501				/* port to listen on for Phased Array Master(s)*/

#define MAIN_DLG_NAME	CTscanDlg
#define PHASED_ARRAY_GUI

// Usefull defines for entire project

#define WM_IPX_RXRDY					WM_USER+0x100
#define WM_USER_KILL_CATCH				WM_USER+0X101
#define WM_USER_KILL_INSPECT			WM_USER+0X102
#define WM_USER_INSPECT_ERASE			WM_USER+0X103
#define WM_USER_ACAL_ERASE				WM_USER+0X104
#define WM_USER_INSPECT_STOP			WM_USER+0X105
#define WM_USER_COPY_SCREEN_TO_MEMORY	WM_USER+0X106
#define WM_USER_PLAY_ONE_PIPE			WM_USER+0X107
#define WM_USER_ACAL_OD					WM_USER+0X108
#define WM_USER_UPDATE_TITLEBAR			WM_USER+0X109
#define WM_USER_UPDATE_ACAL_SCROLLBARS	WM_USER+0X10A
#define WM_USER_INSPECT_ERASE_MACH		WM_USER+0X10B
#define WM_UDP_RXRDY					WM_USER+0x10C
#define WM_USER_KILL_APP				WM_USER+0X10D
#define WM_USER_KILL_IPX_THREAD			WM_USER+0X10E
#define WM_USER_GET_IPX_LIST			WM_USER+0X10F
#define WM_USER_SET_ENGLISH_METRIC		WM_USER+0X110
#define WM_USER_CONTINUOUS_REPLAY		WM_USER+0X111
// manually added by jeh 21-Jun-12

#define WM_USER_KILL_COM_DLG						WM_USER+0x200
#define WM_USER_KILL_ADP_CONNECTION					WM_USER+0x201
#define WM_USER_RESTART_ADP_CONNECTION				WM_USER+0x202
#define WM_USER_INIT_TCP_THREAD						WM_USER+0x207
#define WM_USER_RESTART_TCP_COM_DLG					WM_USER+0x208
#define WM_USER_DO_NOTHING							WM_USER+0x209		// for debugging purposes
#define WM_USER_SERVER_SEND_PACKET					WM_USER+0x20A		// post thread msg when Server needs to send packet
#define WM_USER_CLOSE_TCPIP_CONNECTION				WM_USER+0x210
#define WM_USER_SEND_TCPIP_PACKET					WM_USER+0x211
#define WM_USER_CLIENT_PKT_RECEIVED					WM_USER+0x212
// Adding/deleting connections from PA Masters to PA GUI
#define WM_USER_ADD_PA_MASTER						WM_USER+0x213
#define WM_USER_DELETE_PA_MASTER					WM_USER+0x214
#define WM_USER_INIT_LISTNER_THREAD					WM_USER+0x215
#define WM_USER_STOP_LISTNER_THREAD					WM_USER+0x216

//#define WM_USER_INIT_COMMUNICATION_THREAD			WM_USER+0x217
#define WM_USER_KILL_COMMUNICATION_THREAD			WM_USER+0x218
#define WM_USER_INIT_RUNNING_AVERAGE				WM_USER+0x219		// not used in PAG
#define WM_USER_SERVERSOCKET_PKT_RECEIVED			WM_USER+0x21A
#define WM_USER_TIMER_TICK							WM_USER+0x21B

#define IDT_ADP_RESTART_TIMER						109


// Has to agree in both asidlg and dibapi32.dll code
#define WM_USER_PRINT_SCREEN			WM_USER + 0x203

#define MMI_CODE

// useful MACROS
#define SCALE ((ConfigRec.bEnglishMetric ) ? 25.4 : 1)
#define TXTSCALE ((ConfigRec.bEnglishMetric ) ? "mm" : "in")

// c function to convert CString to char array
void CstringToChar(CString s, char *pChar);
void CstringToTChar(CString s, TCHAR *pChar);


// check if value in variable passed is in limits, if not set to def
#define verify(var,min,max,def) \
	var = ( var < min ) ? def : ( var > max ) ? def : var;
#define PRINTER 1
#define SCREEN  0

// Manually added by jeh 
#include "..\include\Truscan_Version.h"
#include "..\include\Cfg100.h"
#include "..\include\udp_msg.h"			//;  Instdata.h included
//#include "..\include\Instdata.h"
#include "..\include\UT_Msg.h"
//#include "..\include\lang.h"			//in acal.cpp
#include "ErrMsg.h"
// jeh 1-12-2000 Include all child dialogs header file in this file
// Include this file only as the header for all child dialog cpp files

#include "..\include\MC_SysCP_Constants.h"

#include "Catch.h"
#include "Inspect.h"

#include "Tholds.h"
#include "JointNum.h"		// 01/27/00
#include "Acal.h"			// 02/02/00
//include "Filter.h"		//02/24/00
#include "ChnlOn.h"			// 02/27/00
#include "GainDlg.h"		// 02/29/00
#include "WcJob.h"			//03/01/00 taken from well check project
#include "dibapi.h"			// 03/02/00
#include "Replay.h"			// 03/06/00
#include "BldIndx.h"		//03/08/00
#include "Oscope.h"			//03/27/00
//include "IsoCal.h"		//04/06/00
#include "SetUp.h"			// 04/24/00
#include "OdDlg.h"			// 06/27/00
#include "WallDlg.h"		// 06/27/00
#include "Echo.h"			// 07/14/00
#include "Nc.h"				// 07/28/00
#include "WCalDlg.h"		// 08/03/00
#include "IpxStat.h"		// 08/08/00
//#include "IpxIn.h"		// 01/18/01
// Create a hidden dialog window to serve as the message pump for
// IPX wm_user message from OS when ipx data arrives
//#include "IpxDlg.h"		// 01/19/01
//#include "MemFileA.h"		// 01/31/01
#include "GateDlg.h"		// 08/21/01
#include "Cpyto.h"			// 09/07/01  
#include "SysGain.h"		//9-12-01
#include "Rcvr.h"			//9-18-01
#include "Pulser.h"			//9-20-01
#include "tof.h"			//9-24-01
#include "Oscpe.h"			//9-26-01
#include "TCG.h"			//9-27-01
#include "TcgSetupDlg.h"	//12-03-02
#include "Disk.h"			//10-12-01
#include "Enet.h"			//4-1-02
#include "Shear.h"			//5-5-02
#include "CompV.h"			//5-5-02
#include "OnSite.h"			// 5-09-02 jeh
#include "Alarms.h"			// 5-22-02 jeh
#include "StrpChnl.h"		// 5-23-02 JEH
#include "DmxAdj.h"			// 6-03-02 JEH
#include "ScpDmx2.h"		// 6-24-02 JEH
#include "Ftext.h"
#include "Ftext2.h"
#include "TcpThreadRxList.h"
#include "ScopeDlg.h"
#include "EndAreaToolDlg.h"
#include "TcgSetupDlg.h"
#include "Viewcfg.h"
#include "ScopeTrace2CalDlg.h"
#include "ScopeTrace1CalDlg.h"
#include "AscanDlg.h"
#include "ChnlTraceDlg.h"
#include "ServerSocket.h"
//#include "ServerSocketPA_Master.h"	// 8-29-12 jeh
#include "ServerConnectionManagement.h"
#include "ServerSocketOwnerThread.h"	// 8-14-12 jeh
#include "ServerListenThread.h"
#include "ServerRcvListThreadBase.h"		// 11-16-12 jeh
#include "ServerRcvListThread.h"		// 11-06-12 jeh


/////////////////////////////////////////////////////////////////////////////
// CTscanDlg dialog
class CTcpThreadRxList;

class CTscanDlg : public CDialog
{
// Construction
public:
	//static 	CTcpThreadRxList  *CTscanDlg::m_pTcpThreadRxList;
	CTscanDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CTscanDlg();						// destructor

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ChangeActiveChannel();
	void ChangeActiveChannelTo(short nChannel);
	void ChangeActiveGateTo(short nGate);
	void SystemInit();
	void UpdateDlgs();
	void UpdateUndoBuffer();
	void UpdateUndoBuffer2();
	void SetRegistrySiteDefaults();
	void GetRegistrySiteDefaults();
	void UpdateChnlSelected();
	CString m_szChnlDesc;
	int ChnlDisplay(int nChnl);
	void PublicHelpContents();
//	HANDLE m_hIpxInThread;
//	CWinThread* m_pIpxInThread;
	BOOL m_bIsClient;
	PRINTDLG  m_PrintDlg;
	void ReleasePrinter();
	void GetPrinter();
	void GetWindowLastPosition(LPCTSTR lpszEntry, RECT *rect);
	void SaveWindowLastPosition(LPCTSTR lpszEntry, WINDOWPLACEMENT *wp);
//	void GetIpxStats(NET_STATS *pStats);
	WORD m_wLineStatus;
	void QualifyConfigRec();
	void ForceFullWaveDetect();
	void GetJobTotals();
	void SetJobTotals();
	float m_fTotalLen, m_fRLen, m_fQLen;	// pipe lengths
	long  m_lOKJnt, m_lRJnt, m_lQJnt;		// number of pipes each category
	CTscanApp *m_ptheApp;			// pointer to mfc app which runs all this code
	CString m_szDefDataDir;
	CString m_szDefCfgDir;
	void SetDefaultDirectories();
	void GetDefaultDirectories();
	CString m_sData_WO_Folder;	// sub folder of data folder. Where joints of this work order go
	CString GetDataWO_Folder(void)			{ return m_sData_WO_Folder;	}
	void	SetDataWO_Folder(CString s)		{ m_sData_WO_Folder = s;	}
	int m_nJointLength;
	void UpdateTitleBar();
	void FileSave();
	BOOL m_bMetric;
	void UpdateRpm();
	void UpdateLineStatus();
	void UpdateNextJointNum();
	int m_nFpm;		/* feet per minute */
	int m_nRpm;
	BOOL m_bInstAutoRun;
	CString mFlawText[22];

	void GetServerConnectionManagementInfo(void);
	void SaveServerConnectionManagementInfo(void);
	void GetClientConnectionManagementInfo(void);
	void SaveClientConnectionManagementInfo(void);

	void FileSaveAs(void);			// public access to file saveas func
	BOOL IsDataClient();		// anyone around to consume packets?
	BOOL DestroyDataClients(BOOL bForce);	// Kill all clients.
//	VOID BuildConfigFileMsg(C_MSG_15 *pMsg);
//	void BuildTholdMsg (C_MSG_19 *pMsg);
	static BOOL SendMsg(int nMsg);
	//BOOL SendUdpMsg(int nMsg, void *p);	// TCP/IP data gram to customer

	BOOL SendMsgToPAM(int nClientNumber, int nMsg, int nLength, void *pv);
	BYTE* BuildPAM_Message(int nClientNumber, int nMsg, int *pnLength, void *pv);

	// There is only one PAM server, but many clientConnections to each PAM
	// this application is a server to the Phased Array Masters. Masters are numbered 0-n
	int nPamClientNumber;		
	int GetPamClient(void)		{ return nPamClientNumber;	}
	void SetPamClient(int n)	{ nPamClientNumber = n;		}
	int ComputePamClientNumber(void);			//{ return 0;	}	// a stub. replace with real function in cpp file
	int ComputeChannelNumberInPAMClient(void);	//{ return 0; }	// a stub. replace with real function in cpp file
	int ComputeInstrumentNumberInPamClient(void);
	
	void SaveConfigRec(void);	// public access to file save op

	void ShowBitmap(CPaintDC *pdc, CWnd *pWnd);
	
	void StatScreenToConfig();	// Transfer Screen info into ConfigRec
	void ConfigToStatScreen();	// Transfer ConfigRec to TscanDlg status screen
    bool UpdateTimeDate(time_t *tNow);  // Update status dlg time/date
	void SetJointNumber(DWORD JointNum);
	time_t m_tTimeNow;
	UINT m_uNewJntNum;	// set by JntNum modal dialog box
	CString m_szNewOd, m_szNewWall;

	BYTE m_bStatRunFlag;
	int m_nUdpRcvCnt, m_nUdpRcvLostCnt, m_nUdpRcvDupCnt;
	CString m_szConfigFileName;

	HACCEL m_hAccelTable;		//jeh 9/29/99
	CDC *m_dcPrn;			// jeh 10/1/99 keep default printer context
	afx_msg void OnFilePrintscreen();
	afx_msg void OnFilePrintscreen2();

	CInspect*  m_pInspectDlg1;
	CInspect*  m_pInspectDlg2;
	CScopeDlg* m_pScopeDlg;
	CEndAreaToolDlg* m_pEndAreaDlg;
	CTcgSetupDlg* m_pTcgSetupDlg;
	CScopeTrace2CalDlg* m_pScopeTrace2CalDlg;
	CScopeTrace1CalDlg* m_pScopeTrace1CalDlg;
	CAscanDlg* m_pAscanDlg;
	CChnlTraceDlg* m_pChnlTraceDlg;
	CTcpThreadRxList* GetTcpThreadList(void);

	void SetNetworkAlarm();
	void ShowNetworkAlarm();
	int  m_nTimerCount;  // call SetNetworkAlarm() every m timer events
	BOOL m_bShowNetAlarm;
	BOOL m_bLastShowNetAlarm;
	BYTE   m_bConnected[MAX_SHOES+1];   /* network connection status.  0: not connected, 1: connected.  Master in 0, Slave1 in 1, etc. */

	CONFIG_REC *m_pUndo;
	CONFIG_REC* GetUndoBuffer();

	void SwitchInspectWindows(int nWindow);
	void EnableInstrumentPanels();

	CONFIG_REC *m_pOriginalConfig;
	void SaveOriginalConfigRec();
	void RestoreOriginalConfigRec();

	void SetOblqName();
	// Wrappers to convert VC6 to Visual Studio 2010
	afx_msg LRESULT VS10_OnFilePrintscreen(WPARAM, LPARAM);
	afx_msg LRESULT VS10_UpdateTitleBar(WPARAM, LPARAM);
	afx_msg LRESULT VS10_OnCancel(WPARAM, LPARAM);
	void InitializeClientConnectionManagement(void);
	int FindClientSideIP(int nWhichConnection);
	int FindServerSideIP(int nWhichConnection);
	CString GetIPv4(CString sComputerName);
	CString GetIPv4(void);
	LRESULT GetReceivedPackets(WPARAM wWhichList, LPARAM lParam);
	void InitializeServerConnectionManagement(void);
	void CloseListeningServerSockets(void);
	void TimerTickToThreads(void);


	//  Merge with MIll Console
	CString m_sCurrentPipeNumberID;
	HMODULE m_hLanguage;
	DWORD m_nPacketNumber;
	DWORD m_nMstrSentMsg;
	int   m_nPipeStatus;
	UINT m_iSysCPPort;

	void AddDebugMessage(CString sMsg);

//#ifdef SERVER_RCVLIST_THREADBASE		// DEFINE in project defines under C/C++ | Preprocessor
	CServerRcvListThreadBase* CTscanDlg::CreateServerReceiverThread(int nServerNumber, int nPriority);


// Dialog Data
	//{{AFX_DATA(CTscanDlg)

#ifdef _I_AM_PAG
	enum { IDD = IDD_PAG_ASI_DIALOG };

	CButton m_btnPAG_Remote;
	CButton m_btnPAG_Local;
	CButton m_btnPAG_Inspect;
	CButton m_btnPAG_StopInspect;
	CButton m_btnPAG_Motion;
	CButton m_btnPAG_Time;
	
	CButton	m_btnPAG_Master4;
	CButton	m_btnPAG_Master3;
	CButton	m_btnPAG_Master2;
	CButton	m_btnPAG_Master1;
	CButton	m_btnPAG_Database;
	CButton	m_btnPAG_SysCP;
	CButton	m_btnPAG_GDP;
	CButton	m_btnPAG_PLC;

	CButton	m_btnPAG_Slave32;
	CButton	m_btnPAG_Slave31;
	CButton	m_btnPAG_Slave30;
	CButton	m_btnPAG_Slave29;
	CButton	m_btnPAG_Slave28;
	CButton	m_btnPAG_Slave27;
	CButton	m_btnPAG_Slave26;
	CButton	m_btnPAG_Slave25;
	CButton	m_btnPAG_Slave24;
	CButton	m_btnPAG_Slave23;
	CButton	m_btnPAG_Slave22;
	CButton	m_btnPAG_Slave21;
	CButton	m_btnPAG_Slave20;
	CButton	m_btnPAG_Slave19;
	CButton	m_btnPAG_Slave18;
	CButton	m_btnPAG_Slave17;
	CButton	m_btnPAG_Slave16;
	CButton	m_btnPAG_Slave15;
	CButton	m_btnPAG_Slave14;
	CButton	m_btnPAG_Slave13;
	CButton	m_btnPAG_Slave12;
	CButton	m_btnPAG_Slave11;
	CButton	m_btnPAG_Slave10;
#else
	enum { IDD = IDD_ASI_DIALOG };
	CButton	m_btnMaster;
#endif

	CButton	m_btnSlave9;
	CButton	m_btnSlave8;
	CButton	m_btnSlave7;
	CButton	m_btnSlave6;
	CButton	m_btnSlave5;
	CButton	m_btnSlave4;
	CButton	m_btnSlave3;
	CButton	m_btnSlave2;
	CButton	m_btnSlave1;
	
	CScrollBar	m_SBcpyto;
	CScrollBar	m_SBchnl;
	CStatic	m_csText;
	CString	m_szStat_Grade;
	CString	m_szStat_Od;
	CString	m_szStat_Wall;
	UINT	m_uJntNum;
	CString	m_sActCh;
	CString	m_sCpyto;

	CString m_sCalibrationDetailID;
	CString m_sDatabaseName;
	CString m_sServerName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTscanDlg)
	public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;
	void SendInstrumentChannelInfo(int chnl);
//	afx_msg void GetIpxList();
//	void BuildTholdMsg(C_MSG_19 *pMsg);
	char m_szHelpFile[130];
//	void BuildAllSgains(C_MSG_20 *pMsg);
//	void BuildAllAgains(C_MSG_16 *pMsg);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTscanDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDisplayInspect();
	afx_msg void OnDisplayJob();
	afx_msg void OnDisplayPacketdata();
	afx_msg void OnFileExit();
	afx_msg void OnFileOpen();
	afx_msg void OnFilePrintersetup();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnHelpAbout();
	afx_msg void OnHelpContents();
	virtual void OnCancel();
	afx_msg void OnSetupJointnumber();
	afx_msg void OnStatJntNum();
	afx_msg void OnSetupScope();
	afx_msg void OnSetupCustomize();
	afx_msg void OnCalibrateFlaws();
	afx_msg void OnStatOd();
	afx_msg void OnStatWall();
	afx_msg void OnCalibrateWall();
	afx_msg void OnRADIOGate2();
	afx_msg void OnRADIOGate1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCpyChnl();
	afx_msg void OnInstrumentGates();
	afx_msg void OnInstrumentPulser();
	afx_msg void OnInstrumentReceiver();
	afx_msg void OnInstrumentScope();
	afx_msg void OnInstrumentTcgset();
	afx_msg void OnInstrumentTofalarms();
	afx_msg void OnInstrumentAllpanelsalta();
	afx_msg void OnInstrumentCloseallaltc();
	afx_msg void OnFileFreediskspace();
	afx_msg void OnDiagnosticDiskdump();
	afx_msg void OnDiagnosticEnetstatus();
	afx_msg void OnDiagnosticFlawdepthtest();
	afx_msg void OnDiagnosticPacketdata();
	afx_msg void OnDiagnosticScope();
	afx_msg void OnDiagnosticSync();
	afx_msg void OnDiagnosticUdpio();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSetupSiteCustom();
	afx_msg void OnDiagnosticAlarms();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnInstrumentEndarea();
	afx_msg void OnViewConfig();
	afx_msg void OnPrintCfg();
	afx_msg void OnFileWallConvert();
	afx_msg void OnUpdateFileWallConvert(CCmdUI* pCmdUI);
	afx_msg void OnBtnNextJoint();
	afx_msg void OnInstrumentAscan();
	afx_msg void OnDisplayChannelTraces();
	afx_msg void OnChangeStatGrade();
	afx_msg void OnBtnPipePresent();
	afx_msg void OnInstrumentAscanReadSeq();
	afx_msg void OnBtnStartMaster();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	BOOL GrantList();
	DWORD m_dwExitCode;

	UINT m_uStatTimer;

	afx_msg LRESULT OnIpxRxRdy(WPARAM, LPARAM);
	afx_msg LRESULT OnUdpRxRdy(WPARAM, LPARAM);
	//BOOL InitIPX();
	//BOOL InitUdp();
	void ErrMsgSockStartup(int);
	void ErrMsgSock(char *);
	void ErrMsgSockNA(char *);

	// Debug SCM sending capabilities
	void ClearServerCmdSent(int nServer, int nClient);
	UINT GetServerCmdPacketUnSentQty(int nServer, int nClient);
	UINT GetServerCmdPacketSentQty(int nServer, int nClient);

	void CloseInstruments();
	BOOL m_bConfigLoaded;    // flag indicating whether or not a config file has been downloaded 
	BOOL IsSyncNormal();

	void SaveMyWindowPosition();
	void GetMyWindowPosition();
	BOOL m_bStartUp;

	void PAG_UpdateMasterStatus(CButton * master_button, int master_number);
	void PAG_UpdateInstrumentStatus(CButton * instrument_button, int master_number, int instrument_number);
	void PAG_UpdateSystems(CButton * system_button, int system_number);
	int PAG_GetMasterConnectionStatus(int nMaster);
	int PAG_GetInstrumentConnectionStatus(int nMaster, int nInstrument);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASIDLG_H__E654F6C7_C864_11D3_9B48_00A0C91FC401__INCLUDED_)
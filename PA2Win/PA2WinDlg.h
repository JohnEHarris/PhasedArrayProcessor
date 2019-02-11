
// PA2WinDlg.h : header file
//

#pragma once
using namespace std;

#include "afxwin.h"
#include <iostream>
#include "..\Include\Global.h"
#include "..\Include\PA2Struct.h"
#include "..\Include\Cmds.h"
#include "ServerConnectionManagement.h"
#include "ServerSocket.h"
#include "ServerSocketOwnerThread.h"	// 8-14-12 jeh
#include "ServerListenThread.h"
#include "ServerRcvListThread.h"		// 11-06-12 jeh
#include "TuboIni.h"
#include "CCmdFifo.h"
#include "InspState.h"
#include "PA2Win.h"
#include "vChannel.h"
#include "CmdProcessThread.h"
#include "ClientConnectionManagement.h"
#include "ClientCommunicationThread.h"
#include "ClientSocket.h"
#include "CCM_PAG.h"
#include "time.h"
#include "NcNx.h"
#include "TestThread.h"


#define MMI_AS_SERVER_IP_ADDR		"192.168.10.10"		/* Server for the Phased Array Master(s) */
#define MMI_AS_SERVER_IP_PORT		7501				/* port to listen on for Phased Array Master(s)*/

#define MAIN_DLG_NAME	CPA2WinDlg
#define PHASED_ARRAY_GUI

#ifdef I_AM_PAP
#define	MAX_CLIENTS				8
#else
#define	MAX_CLIENTS				8
#endif

// Usefull defines for entire project
#define IDT_ADP_RESTART_TIMER						109

//#define WM_IPX_RXRDY					WM_USER+0x100
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

#define WM_USER_THREAD_HELLO_WORLD					WM_USER+0X1B0
#define WM_USER_TEST_THREAD_BAIL					WM_USER+0X1B1
#define WM_USER_KILL_COM_DLG						WM_USER+0x200
#define WM_USER_KILL_ADP_CONNECTION					WM_USER+0x201
#define WM_USER_RESTART_ADP_CONNECTION				WM_USER+0x202
// Has to agree in both asidlg and dibapi32.dll code
#define WM_USER_PRINT_SCREEN						WM_USER+0x203
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

#define WM_USER_INIT_COMMUNICATION_THREAD			WM_USER+0x217
#define WM_USER_KILL_COMMUNICATION_THREAD			WM_USER+0x218
#define WM_USER_INIT_RUNNING_AVERAGE				WM_USER+0x219		// not used in PAG
#define WM_USER_SERVERSOCKET_PKT_RECEIVED			WM_USER+0x21A
#define WM_USER_KILL_OWNER_SOCKET					WM_USER+0x21B
#define WM_USER_SERVER_FLUSH_CMD_PACKETS			WM_USER+0x226


// ClientConnectionManagement
#define WM_USER_KILL_CMD_PROCESS_THREAD				WM_USER+0x21C
#define WM_USER_KILL_RECV_THREAD					WM_USER+0x21D
#define WM_USER_KILL_SEND_THREAD					WM_USER+0x21E
#define WM_USER_FLUSH_LINKED_LISTS					WM_USER+0x21F

#define WM_USER_TIMER_TICK							WM_USER+0x220

// Used by CCM receiver thread only
#define WM_USER_CREATE_SOCKET						WM_USER+0x221
#define WM_USER_CONNECT_SOCKET						WM_USER+0x222
#define WM_USER_KILL_SOCKET							WM_USER+0x223
#define WM_USER_KILL_OWNER_SOCKET_THREAD			WM_USER+0x224
#define WM_USER_ATTACH_SERVER_SOCKET				WM_USER+0x225

// global variables for debugging
extern int gnAsyncSocketCnt, gnFifoCnt;		// counter to count sequence of ASyncSocket creation
extern CPA2WinDlg *pMainDlg;
extern CPA2WinApp theApp;
extern int nShutDown;
extern HANDLE g_hTimerTick;
extern int KillLinkedList( CRITICAL_SECTION *pCritSec, CPtrList *pList );
extern int  KillMyThread( CWinThread *pThread );



void CstringToChar( CString s, char *pChar );
void CstringToTChar( CString s, TCHAR *pChar );
void CstringToTChar( CString &s, _TCHAR *p, int nSizeOfArray );
void CstringToChar( CString &s, char *p, int nSizeOfArray );
char *GetTimeStringPtr( void );
CString GetTimeString( void );


// CPA2WinDlg dialog
class CPA2WinDlg : public CDialogEx
{
// Construction
public:
	CPA2WinDlg(CWnd* pParent = NULL);	// standard constructor
	~CPA2WinDlg();
	CPA2WinApp	*m_ptheApp;
	CTestThread *m_pTestThread;

	void MakeDebugFiles(void);
	void GetServerConnectionManagementInfo( void );
	void GetClientConnectionManagementInfo( void );
	void SaveServerConnectionManagementInfo( void );
	void SaveClientConnectionManagementInfo( void );
	void SetMy_PAP_Number( CString &Ip4, UINT uPort );

	CServerRcvListThread* CreateServerReceiverThread( int nServerNumber, int nPriority );
		
	CRITICAL_SECTION *pCSSaveDebug;	// control access to debug output 
	CRITICAL_SECTION *pCSSaveCommands;	// control access to Commands output 
	CFile m_FakeData;
	CFile m_DebugLog;
	CFile m_CommandLog;
	CFile m_PapNumberFile;
	CFile m_AltPapNumberFile;	// file store on C drive in \LocalAppExes\MyID
	int m_nMsgSeqCnt;

	int m_nFakeDataExists;
	int m_nDebugLogExists;
	int m_mCommandLogExists;

	void SaveFakeData(CString& s);
	void CloseFakeData(void);
	// Debug file - replace monitor output. Read with BareTail app
	void SaveDebugLog( CString& s );
	void CloseDebugLog( void );
	// Verify Commands are received by PAP
	void SaveCommandLog(CString& s);
	void CloseCommandLog(void);
	void ReadPAPnumber(void);
	void DeleteOldPapNumbers(int nNewPap);
	void GetPAPFromCDrive(void);	// If Read Pap Number doesn't find pap number file in drives  G - D


	int m_nPapNumber;		// the PAP number of this machine, the one running the ServiceApp
	UINT m_uPapPort;		// the port number for the PAP being serviced by this machine (PC)
	UINT m_wMsgSeqCnt;	// counter to uniquely identify each packet. Used by PamSendToPag()
	UINT m_uMsgSeqCntChange;	//detect connection to adc and store version numbers
	//void SetMy_PAP_Number(CString &Ip4, UINT uPort);
	//int  GetMy_PAP_Number(void)	{ return m_nPapNumber;	}  never called
	//UINT GetMy_PAP_Port(void)	{ return m_uPapPort;	}
	CString m_sPktRate[2];
	CString m_sHwVerAdc, m_sSwVerAdc, m_sHwVerPulser, m_sSwVerPulser;

	time_t m_tTimeNow;
	void StartTimer();
	void StopTimer();
	int  m_nTimerCount;  // call SetNetworkAlarm() every m timer events
	UINT m_uStatTimer;
	bool UpdateTimeDate(time_t *tNow);  // Update status dlg time/date

	void GetAllIP4AddrForThisMachine(void);		// Fills in the array sThisMachineIP4Addr[]
	CString sThisMachineIP4Addr[20];			// doubtfull this machine will have 20 "NIC's"
	UINT uThisMachineIP4Addr[20];				// 32 bit ulong representation of ip4
	
	// on screen graph in PA2Win dialot of server rows with connected client IP addresses
	CString sConnectedClients[MAX_SERVERS][MAX_CLIENTS_PER_SERVER];	// 2018-08-07
	void ShowConnectedClients(void);
	void AddConnectedClient(int nServer, int nClientNum, CString& sIP4);

	void InitializeServerConnectionManagement( void );
	void InitializeClientConnectionManagement(void);
	int FindClientSideIP(int nWhichConnection);
	int FindServerSideIP(int nWhichConnection);
	CString GetIPv4(CString sComputerName);
	CString GetIPv4(void);
	//LRESULT GetReceivedPackets(WPARAM wWhichList, LPARAM lParam);
	CString GetClientIP(int nClient)		{ return stSocketNames[nClient].sClientIP4;	}	// clinets IP4
	void SetClientIP(int nClient, CString s){ stSocketNames[nClient].sClientIP4 = s;	}
	short GetClientPort(int nClient)		{ return stSocketNames[nClient].nPort;		}	// clinets port
	void SetClientPort(int nClient, short n){ stSocketNames[nClient].nPort = n;			}

	CString GetServerIP(int nClient)		{ return stSocketNames[nClient].sServerIP4;	}	// server nClient is seeking
	void SetServerIP(int nClient, CString s){ stSocketNames[nClient].sServerIP4 = s;	}	// server nClient is seeking
	CString GetServerName(int nClient)		{ return stSocketNames[nClient].sServerName;	}	// url for server nClient is seeking
	void SetServerName(int nClient, CString s){ stSocketNames[nClient].sServerName = s;	}	// url for server nClient is seeking

	short GetServerPort(int nClient)		{ return (stSocketNames[nClient].nPort & 0xffff);		}	// clinets port
	void SetServerPort(int nClient, short n){ stSocketNames[nClient].nPort = n;			}

	//CServerRcvListThread* CreateServerReceiverThread(int nServerNumber, int nPriority);

	void DestroyCCM( void );
	void DestroySCM( void );

	void SaveMyWindowPosition();
	void GetMyWindowPosition();
	void SaveWindowLastPosition( LPCTSTR lpszEntry, WINDOWPLACEMENT *wp );
	void GetWindowLastPosition( LPCTSTR lpszEntry, RECT *rect );

	void StructSizes( void );
	void DlgDebugOut( CString s );
	void ShowIdata(void);
	void DebugToNcNxDlg( CString s );
	int GetAdcCmdQ(void);		// return number of commands queued for ADC
	int GetPulserCmdQ(void);	// return number of commands queued for Pulser

#ifdef I_AM_PAG
	BOOL SendMsgToPAP( int nClientNumber, int nMsgID, void *pMsg );
	// as of 3/1/18 can send msgs to PAP dialog screen to output debug info for Yanming and Qingshan
#endif

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PA2WIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lbOutput;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnFileExit();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnConfigureNcNx();
	afx_msg void OnBnClickedBnEraseDbg();
	afx_msg void OnBnClickedBnShutdown();
};

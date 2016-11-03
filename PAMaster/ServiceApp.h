// ServiceApp.h: interface for the CServiceApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEAPP_H__C234F376_E8FC_11D1_A712_000000000000__INCLUDED_)
#define AFX_SERVICEAPP_H__C234F376_E8FC_11D1_A712_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 
#include <iostream>
using namespace std;
#include "NTService.h"
#include "../Include/PA2Struct.h"
#include "../include/udp_msg.h"
//#include "../include/ut_msg.h"
//#include "../include/nios_msg.h"
#include "afxmt.h"
#include "HwTimer.h"
#include "InspState.h"
#include "ServerSocket.h"
//#include "ServerSocketPA_Master.h"	// 8-29-12 jeh
#include "ServerConnectionManagement.h"
#include "ServerSocketOwnerThread.h"	// 8-14-12 jeh
#include "ServerListenThread.h"
#include "ServerRcvListThreadBase.h"		// 11-16-12 jeh
#include "ServerRcvListThread.h"		// 11-06-12 jeh
#include "ClientConnectionManagement.h"	// 21-Jan-13 jeh
#include "TestThread.h"
#include "TuboIni.h"					//12-Feb-2013
#include "CCM_PAG.h"					//12-May-16 jeh
#include "vChannel.h"					//02-Jun-16 jeh
#include "CCmdFifo.h"					//03-Nov-16 jeh


/*
see ServiceApp.cpp
1.0.01			2016-06-14 Nc Nx working with fake data input, good output to PAG
*/

// THE user interface (MMI or Phased Array GUI PAG) acts as a server to the Phased Array Master
// Define the IP address of the SERVER component associated with the user interface
// The phased-array master will be a client (connect to) the user interface
//
// This information now comes thru an ini file read by CServiceApp::GetClientConnectionManagementInfo
#define PAG_SERVER_IP_ADDR			"192.168.10.10"
#define PAG_SERVER_PORT				7501
#define WALL_BAR_DISPLAY_CLIENT_PORT	7515


#define WM_USER_THREAD_HELLO_WORLD					WM_USER+0X1B0
#define WM_USER_TEST_THREAD_BAIL					WM_USER+0X1B1
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

#define WM_USER_INIT_COMMUNICATION_THREAD			WM_USER+0x217
#define WM_USER_KILL_COMMUNICATION_THREAD			WM_USER+0x218
#define WM_USER_INIT_RUNNING_AVERAGE				WM_USER+0x219
#define WM_USER_SERVERSOCKET_PKT_RECEIVED			WM_USER+0x21A
#define WM_USER_KILL_OWNER_SOCKET					WM_USER+0x21B

// ClientConnectionManagement
#define WM_USER_KILL_CMD_PROCESS_THREAD				WM_USER+0x21C
#define WM_USER_KILL_RECV_THREAD					WM_USER+0x21D
#define WM_USER_KILL_SEND_THREAD					WM_USER+0x21E
#define WM_USER_FLUSH_LINKED_LISTS					WM_USER+0x21F


#define ePAM_Client_Of_PAG_Server			0
extern CServiceApp theApp;

class CServiceApp : public CWinApp, public CNTService
	{
	HANDLE	m_hStop;

public:
	CServiceApp();
	virtual ~CServiceApp();
	void ShutDownEasy(void);
	CTestThread *m_pTestThread;
	//CTuboIni *m_pTuboIni;
	int m_nShutDownCount;

	void GetServerConnectionManagementInfo(void);
	void SaveServerConnectionManagementInfo(void);
	void GetClientConnectionManagementInfo(void);
	void SaveClientConnectionManagementInfo(void);
	char m_buffer[128];
	CRITICAL_SECTION *pCSSaveDebug;	// control access to debug output 
	CServiceApp *m_ptheApp;
	CFile m_FakeData;
	CFile m_DebugLog;
	int m_nFakeDataExists;
	int m_nDebugLogExists;
	void SaveFakeData(CString& s);
	void CloseFakeData(void);
	// Debug file - replace monitor output. Read with BareTail app
	void SaveDebugLog(CString& s);
	void CloseDebugLog(void);
	void SetMy_PAM_Number(CString &Ip4, UINT uPort);
	int  GetMy_PAM_Number(void)	{ return m_nPamNumber;	}
	UINT GetMy_PAM_Port(void)	{ return m_uPamPort;	}
#if 0
	// Critical Sections to regulate linked list access by Instruemnt client threads and the Service App
	// Could be two dimensional in the future for for now just one dimensional.
	// Service App usually does something with the data from the instruments by accessing the instrumnets linked list

	CRITICAL_SECTION *pAppInstAccess[MAX_CLIENTS_PER_SERVER];	//MAX_CLIENTS_PER_SERVER in ServiceConnectionManagement.h
	
	// All who want to access an instruments linked list must make this call to ServiceApp - including ServiceApp itself
	int GetInstrumentListAccess(int nInstNumber);		// returns InstNumber when access is granted
	void ReleaseInstrumentListAccess(int nInstNumber);	// Allow others to access the linked lists and critical sections.
#endif


public:
	virtual BOOL InitInstance();
	virtual void Run(DWORD, LPTSTR *);
	virtual void Stop();
	virtual int ExitInstance();	// manually added 10-25-2012 jeh... handle user closing debug window???
	//void ForcedClose(WORD w, LONG lp);
	void GetAllIP4AddrForThisMachine(void);		// Fills in the array sThisMachineIP4Addr[]
	CString sThisMachineIP4Addr[20];			// doubtfull this machine will have 20 "NIC's"
	UINT uThisMachineIP4Addr[20];				// 32 bit ulong representation of ip4
		// We can find  class C subnet by masking out the high byte. Assuming our clients are in
		// the same subnet as the servers.
	int m_nPamNumber;		// the PAM number of this machine, the one running the ServiceApp
	UINT m_uPamPort;		// the port number for the PAM being serviced by this machine (PC)
	UINT m_uMsgSeqCount;	// counter to uniquely identify each packet. Used by PamSendToPag()
	void InitializeServerConnectionManagement(void);	// jeh taken from PAG/TScanDlg.cpp and modified
	int KillServerConnectionManagement(int nServer);	// undo what Initialize created
	void InitializeClientConnectionManagement(void);
	void ShutDown(void);

// SERVER_RCVLIST_THREADBASE		// DEFINE in project defines under C/C++ | Preprocessor
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

	CServerRcvListThreadBase* CreateServerReceiverThread(int nServerNumber, int nPriority);
	void PamSendToPag(void *pBuf, int nLen);

	};


// C routines
void CstringToChar(CString s, char *pChar);
void CstringToTChar(CString s, TCHAR *pChar);
CString GetTimeString(void);
char* GetTimeStringPtr(void);


#endif // !defined(AFX_SERVICEAPP_H__C234F376_E8FC_11D1_A712_000000000000__INCLUDED_)


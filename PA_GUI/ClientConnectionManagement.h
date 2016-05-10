#ifndef CLIENT_CONNECTION_MANAGEMENT_H
#define CLIENT_CONNECTION_MANAGEMENT_H

/*
ClientConnectionManagement.h
Author:		JEH
Date:		05-Jun-2012
Purpose:	Collect resource necessary to support TCP/IP Send and Receive operations
Revised:	12-Jun-12 Abandon 'C' worker threads in favor of using a class method for the independent thread.
				Provides for easier integration/sharing of control parameters between the thread with the 
				dialogs/dialog controls and the threads which operate at higher priority to coordiante
				TCP/IP messaging sending and receiving.
			21-Jun-12 Testing with Randy' SysCpTestClient resulted in not being able to recieve packet from the SysCp.
				Decided to change the structure of the CCM to be patterned after AGS3 which used CDialogs to configure/operate
				the socket and a WinThread to create the CDialog as a separate thread with different priority from the app.
				Also borrowed concepts from Enet2GuiSend to have a worker thread at lower priority than the app which will
				send packets to the connected server from a linked list of packets to send. What reamins is a static global structure
				of CCM instances which contains all the structures necessary to send/receive and store tcpip packets.

			05-Jul-12 Make this a base class. Derive individual classes from it which process different message formats with
				different processing operations.
			09-Jul-12 Put pointer to CLIENT_IDENTITY_DETAIL structure into CCM as a class member. Create in constructor.
				Remove redundant items in CCM struct now covered by C-I-D
			23-Jul-12 Most of the usefulness of this class is removed since I discovered the connection to the database and 
					the GDP are encapuslated into odbc operations. This is only useful now for the SysCp

*/

#pragma once

#include "..\Include\MC_SysCp_constants.h"

// also in tscandlg.h
#define WM_USER_INIT_TCP_THREAD						WM_USER+0x207
#define WM_USER_RESTART_TCP_COM_DLG					WM_USER+0x208
#define WM_USER_RESTART_ADP_CONNECTION				WM_USER+0x202	// also in TScanDlg.h
#define WM_USER_SEND_TCPIP_PACKET					WM_USER+0x211	// also in TScanDlg.h
#define WM_USER_TIMER_TICK							WM_USER+0x21B 	// also in TScanDlg.h
#define WM_USER_SERVER_SEND_PACKET					WM_USER+0x20A		// post thread msg when Server needs to send packet


#ifdef THIS_IS_SERVICE_APP
#define	MAX_CLIENTS				4		
// edit this value if more client connections to servers are needed
#else
#define THE_APP_CLASS	CTscanApp
#define	MAX_CLIENTS				5		
// edit this value if more client connections to servers are needed
#endif

// all enums below are outputs except for eAbort. A procedure in the dialog sets this status when
// it wants the worker thread to abort on the next wait timeout. All other enum's
// are status info set by the worker thread to let the outside world know what state
// the worker is in
//
enum eClientConnectionManagement {eNotExist, eRun, eAbort, eAborted};	// {out, out, out, in}

enum eThreadRole { eUnknown, eReceiver, eSender };		// 0=unknown, 1=Receiver, 2=Sender

// Clients sometimes need to reconnect or restart a connection if the server is not ready
// Use these enum's to tell CClientConnectionManagement::TimerTick() what action to take 
// for each type of system implemented

enum eClientRestart	{ eRestartPAGtoSysCp, eRestartPAMtoPAG, eFake_GDP_Pipe_Data };

// Collect all or most of the necessary control variables into a structure

class CClientConnectionManagement;	// we're going to make a ptr to ourselves in the class so we have to define the class here
class CClientCommunicationThread;		// adapted from ags3 project
									// Communication dlgs are controlled by windows messages
class CCmdProcessThread;
class CAsyncSocket;


typedef struct
	{
	HWND hWnd;						// window handle of the primary dialog which sends/receives on this connection
//	CLIENT_IDENTITY_DETAIL *pClientIdentity;
	CString szSocketName;			// name of this machine, amalog/sonoscope
	CString sClientName;			// symbolic name of the client network address,  eg., MC_ACP_HOSTNAME = "mc-acp"
	CString sClientIP4;				// IP4 dotted address of client, normally this computers NIC address, eg., 192.168.10.10 
	UINT uClientPort;				// set when connection to server made .. now in c-i-d
	CString sServerName;			// symbolic name of the server network address we want to connect to with this connection, eg., mc-scp 
	CString sServerIP4;				// IP4 dotted address of server eg., 192.168.10.50
	UINT uServerPort;				// set when connection to server made  now in c-i-d
//	int nInspectionType;			// eAmalog, eSonoScope etc  .. now family name in c-i-d
	int nInstrumentSelector;		// 0 = amalog, 1 = sonoscope, 2 = tbd - or instances of the same type inspection machine
	int nProtocolSelector;			// 0 = UDP, 1 = TCPIP, initially just tcpip since udp does not have servers and clients, only peers.
	int nWinVersion;				// Win7 = 7, XP otherwise
	// Send-to-server items
	CClientCommunicationThread *pSendThread;	// thread to control sending messages to server
	//CTCPCommunicationDlg *pSendDlg;			// a dialog created byte the SendThread
	//HWND hSendDlg;					// windows handle to the Send Dialog
	CRITICAL_SECTION *pCSSendPkt;	// control access to output (send) list
	CPtrList* pSendPktList;	// list containing packets to send
	int nSendPriority;				// should normally be THREAD_PRIORITY_BELOW_NORMAL

	// Receive-from-server items
	// Receive thread does nothing but create an invisibble dialog which creates a 
	// ClientConnection socket and then hold a place in the process so the 
	// Async client can receive packets and put them into the RcvPktPacketList

	CClientCommunicationThread *pReceiveThread;	// thread to control receiving messages from server
	//CTCPCommunicationDlg *pReceiveDlg;			// a dialog created by the ReceiveThread
	//HWND hReceiveDlg;					// windows handle to the Receive Dialog
	CRITICAL_SECTION *pCSRcvPkt;	// control access to input (receive) list
	CPtrList* pRcvPktPacketList;	// list containing packets received
	int nReceivePriority;			// should normally be THREAD_PRIORITY_ABOVE_NORMAL

	// Optional thread for processing packets/messages at lower priority than receiving
	CCmdProcessThread			*pCmdProcessThread;
	int nCmdProcessPriority;		// should normally be THREAD_PRIORITY_BELOW_NORMAL

	CAsyncSocket * pSocket;			// ASync socket fills RcvPktPacketList with OnReceive method.
									// same socket is used by Send thread to send packets to server
	BYTE bConnected;				// true if connection is successful.. detected in receive thread.
	UINT uPacketsReceived;
	UINT uBytesReceived;
	UINT uPacketsPerSecond;			// received packets
	UINT uMaxPacketReceived;		// how big was biggest received packet
	UINT uInvalidPacketReceived;	// we don't know what this packet is
	UINT uLostReceivedPackets;
	// count the jumps in message sequence count
	UINT uDuplicateReceivedPackets;	// same packet received again

	// VARIOUS other controls common to the socket	
	//	BOOL *lpKillThread;

	CPtrList* pInDebugMessageList;	// input operations put messages into this list
	CPtrList* pOutDebugMessageList;
	UINT uIdataAcksSent;			// Only for UDP protocol..count idata packets acks.. 10-Jan-11
	CRITICAL_SECTION *pCSDebugIn;
	CRITICAL_SECTION *pCSDebugOut;
	//UINT uRestartMsg;				// windows message.. probably no use in this scenario
									// relate restart windows handles to instrument selector

	UINT uLastTick;					// Use with main app uAppTimerTick value to provide keep alive messages

	CClientConnectionManagement *pCCM;	// point to ourself. This is important.

	}	ST_CLIENT_CONNECTION_MANAGEMENT;

// Allow table entry of socket endpoint names/IP4 addresses.
// Specify same number of ST_SOCKET_NAMES as ST_CLIENT_CONNECTION_MANAGEMENT instances
typedef struct
	{
	CString sClientName;			// symbolic name of the client network address,  eg., MC_ACP_HOSTNAME = "mc-acp"
	CString sClientIP4;				// IP4 dotted address of client, normally this computers NIC address, eg., 192.168.10.10
	CString sServerName;			// symbolic name of the server network address we want to connect to with this connection, eg., mc-scp 
	CString sServerIP4;				// IP4 dotted address of server eg., 192.168.10.50
	short nPort;					// added in PAM
	int nPacketSize;				// added in PAM
	int nWinVersion;				// what version of windows is the server we are trying to connect to
	}	ST_SOCKET_NAMES;

// JEH add a generic header type for determining what the packet really is
// From the messageId we will determine what to send
typedef struct
{
	USHORT MsgSeqNum;		// sequential counter
	USHORT MessageLength;	// length in bytes???
	USHORT MessageID;		// 1, 2, ...
}	GENERIC_MSG_HEADER;

#define NORMAL_EXIT						 0
#define C_CLIENT_SOCKET_CREATION_ERROR	-1


// when this file is included by it partner cpp file, I_AM_CCM is defined
// All other cpp files which include this one should not define I_AM_CCM

#ifdef I_AM_CCM
#define PubExt1	
#else
#define PubExt1	extern
#endif

// It is necessary to have the array of CCM structs. The reason is that this is a static global source for 
// the info for every instance of the CCM class.
// Could also lend itself to configuration of all clients by filling in the stCCM array from a single source. 

#ifdef I_AM_CCM

	ST_CLIENT_CONNECTION_MANAGEMENT stCCM[MAX_CLIENTS];		// a global, static array of CCM structs
	// Here we could initialize the structure for the clients we expect.
	// Thus IP addresses/url's could be entered as initialization constants
	// Could also use an ini file and fill this structure before creating the 
	// class instances pCCM[]
	//
	// a global, static array of ST_SOCKET_NAMES structs
#if 0
typedef struct
	{
	CString sClientName;			// symbolic name of the client network address,  eg., MC_ACP_HOSTNAME = "mc-acp"
	CString sClientIP4;				// IP4 dotted address of client, normally this computers NIC address, eg., 192.168.10.10
	CString sServerName;			// symbolic name of the server network address we want to connect to with this connection, eg., mc-scp 
	CString sServerIP4;				// IP4 dotted address of server eg., 192.168.10.50
	short nPort;					// added in PAM - port the server is listenin on.
	int nPacketSize;				// added in PAM
	int nWinVersion;				// probably only relevant for PAG
	}	ST_SOCKET_NAMES;
#endif

	ST_SOCKET_NAMES stSocketNames[MAX_CLIENTS] =

#ifdef THIS_IS_SERVICE_APP
	{
	"", "192.168.10.10", "", "192.168.10.10", 7501,sizeof(MMI_CMD),0		// PAM attempt to connect to PAG on port 7501

#endif

#ifdef _I_AM_PAG
	{ 
	_T("localhost"), _T(""), _T("mc-scp"), _T("")	, (short)0XFFF0,1260,7			// Syscp, conn #0

#endif

#if 0
	  ,"","","","",0, 1024,0									// connection #1
	  ,"","","","",0, 1024,0									// connection #2
	  ,"","","","",0, 1024,0									// connection #3
	  ,"","","","",0, 1024,0									// connection #4
	  ,"","","","",0, 1024,0									// connection #5
#endif
	};
	int gnMaxClients;

#else

	extern ST_CLIENT_CONNECTION_MANAGEMENT stCCM[MAX_CLIENTS];
	extern ST_SOCKET_NAMES stSocketNames[MAX_CLIENTS];
	extern int gnMaxClients;

#endif



class CClientSocket;


/***************** CLASS DECLARATION **************************/
/***************** CLASS DECLARATION **************************/

class CClientConnectionManagement: public CObject
	{
public:
	CClientConnectionManagement(int nMyConnection, USHORT wOriginator);		// the only constructor
	virtual ~CClientConnectionManagement(void);

	void SetWndHandle(HWND h)	{	if (m_pstCCM)	m_pstCCM->hWnd = h;		}
	HWND GetWndHandle(void)		{ return ( m_pstCCM ? m_pstCCM->hWnd : NULL );	}
	// Worker 'threads'.. runing at higher/lower priority to handle tcp/ip messaging
	// In fact, the main reason for having these threads is to run some process at a different priority
	// than the main application. In the past we have typically run the Ethernet receiving thread
	// at higher priority and the application or sending thread.

	void CreateReceiveThread(void);
	void CreateSendThread(void);
	void CreateCmdProcessThread(void);	// Useful for PAM
	void InitReceiveThread(void);
	void InitSendThread(void);
	void TimerTick(WORD wTargetSystem);

	void SetSocketNameString(CString s);
	void SetProtocolType(int n)		{	if (m_pstCCM)	m_pstCCM->nProtocolSelector = n;		}
	int  GetProtocolType(void)		{	return ( m_pstCCM ? m_pstCCM->nProtocolSelector : 0);	}

	void SetClientName(CString s)	{ if (m_pstCCM)	m_pstCCM->sClientName = s;	}	// symbolic name of the client -- "mc-acp"
	CString GetClientName(void)		{ return ( m_pstCCM ? m_pstCCM->sClientName : _T("") );	}

	void SetServerName(CString s)	{ if (m_pstCCM)	m_pstCCM->sServerName = s;	}	// symbolic name of the server -- "mc-scp"
	CString GetServerName(void)		{ return ( m_pstCCM ? m_pstCCM->sServerName : _T("") );	}

	void SetSocketName(CString s)	{ if (m_pstCCM)	m_pstCCM->szSocketName = s;	}	// symbolic name of the socket
	CString GetSocketName(void)		{ return ( m_pstCCM ? m_pstCCM->szSocketName : _T("") );}

	void SetWinVersion(int v)		{ if (m_pstCCM)	m_pstCCM->nWinVersion = v;			}
	int  GetWinVersion(int v)		{ return ( m_pstCCM ? m_pstCCM->nWinVersion : 0 );	}

	void SetSocketPtr(CClientSocket* pSkt)	{	if (m_pstCCM)	m_pstCCM->pSocket = (CAsyncSocket *) pSkt;			}
	CClientSocket* GetSocketPtr(void)		{	return ( m_pstCCM ? (CClientSocket *) m_pstCCM->pSocket : NULL);	}

	void SetConnectionState(BYTE b)			{	if (m_pstCCM)	m_pstCCM->bConnected = b;		}	// connected when not 0
	BYTE GetConnectionState(void)			{	return ( m_pstCCM ? m_pstCCM->bConnected : 0 );	}


	void SendPacket(BYTE *pB, int nBytes, int nDeleteFlag);			

	// To add data to the input data list, lock the critical section, add data to the list tail, and unlock critical section
	void LockRcvPktList(void)		{ EnterCriticalSection(m_pstCCM->pCSRcvPkt);	}
	void AddTailRcvPkt(void *pV)	{ m_pstCCM->pRcvPktPacketList->AddTail(pV);		}
	void UnLockRcvPktList(void)		{ LeaveCriticalSection(m_pstCCM->pCSRcvPkt);	}

	void LockSendPktList(void)		{ EnterCriticalSection(m_pstCCM->pCSSendPkt);	}
	void AddTailSendPkt(void *pV)	{ m_pstCCM->pSendPktList->AddTail(pV);	}
	void UnLockSendPktList(void)	{ LeaveCriticalSection(m_pstCCM->pCSSendPkt);	}

	void LockDebugOut(void)			{ EnterCriticalSection(m_pstCCM->pCSDebugOut );	}
	//void AddTailDebugOut(CString s)	{ m_pstCCM->pOutDebugMessageList->AddTail(&s);	}
	void UnLockDebugOut(void)		{ LeaveCriticalSection(m_pstCCM->pCSDebugOut );	}

	void LockDebugIn(void)			{ EnterCriticalSection(m_pstCCM->pCSDebugIn );	}
	//void AddTailDebugIn(CString s)	{ m_pstCCM->pInDebugMessageList->AddTail(&s);	}
	void UnLockDebugIn(void)		{ LeaveCriticalSection(m_pstCCM->pCSDebugIn );	}

	void DebugOut(CString s);

	void OnSocketClose(int nErrorCode);		// ASync socket calls this to allow decision to restart

	// read the received messages from ClientSocket Class instance
	void OnReceive(CClientSocket *pSocket);	// Called by CClientSocket::OnReceive()
	void* GetWholePacket(int nPacketSize, int *pReceived);
	void UnknownRcvdPacket(void *pV);
	virtual void ProcessReceivedMessage(void);		// Main dlg calls thru our ccm to process msg in linked list

	void SetClientIp(CString s);	// { if (m_pstCCM)  m_pstCCM->sClientIP4 = s;}
	void SetServerIp(CString s);	// { if (m_pstCCM)  m_pstCCM->sServerIP4 = s;}
	void SetServerPort(UINT uPort)	{ if (m_pstCCM)  m_pstCCM->uServerPort = uPort;		}
	UINT GetServerPort(void)		{ return (m_pstCCM ? m_pstCCM->uServerPort : 0 );	}
	void SetClientPort(UINT uPort)	{ if (m_pstCCM)  m_pstCCM->uClientPort = uPort;		}
	UINT GetClientPort(void)		{ return (m_pstCCM ? m_pstCCM->uClientPort : 0 );	}


#ifndef THIS_IS_SERVICE_APP
	CString RetrieveKeyValue(CString sTargetKey);
	void SendClientIdentity(void);	//UINT Originator, LONG Target, DWORD ThreadID);
#endif


	//==========================================================================================
	// member variables

	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;	// pointer to my global structure instance 
	int m_nMyConnection;		// which one of the global ST_CLIENT_CONNECTION_MANAGEMENT is mine

	int m_nStart;								// used by GetWholePacket
	BYTE m_RcvBuf[0x10000];						// 16 k receiver buffer.. now 64k
	int m_BufOffset;
	void *pWholePacket;
	// debug info
	int m_nMaxBufOffset;
	int m_nMaxStart;
	int m_nRcvRqst;								// how many can we take
	int m_nMinRcvRqst;							// smallest packet requested to receive
	int m_nMaxRcvRqst;
	CString szName;
	};	
// End of CClientConnectionManagement class declaration.


// declared public here and external in all other files.
// The main dialog or some other global memory operating agent declares an array of
// pointer to ST_CLIENT_CONNECTION_MANAGEMENT. The size of the array is MAX_CLIENTS
//
PubExt1 CClientConnectionManagement *pCCM[MAX_CLIENTS];	// global, static ptrs to class instances defined outside of the class definition.

#endif	// defining CLIENT_CONNECTION_MANAGEMENT_H


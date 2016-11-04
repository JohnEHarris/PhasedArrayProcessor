/** Copyright (C) 2008 Tuboscope, a National Oilwell Varco Company. All rights reserved  **/
// CTCPCommunicationDlg.cpp : implementation file
//
#if 0
File:	CTCPCommunicationDlg.cpp
Purpose:	Maintain a message pump in a thread separate from the application
			which services the tcp/ip communication socket.  This dialog is
			INVISIBLE.

			For similar operation see IpxIn.cpp and IpxDlg.cpp in Truscope2 
			standalone MMI code.

Date:	11-Feb-2005

Revised:
		25-Feb-2005 Major revision, move from tcp/ip to udp as the protocol.  Make
			This code work like the Ene2 board (which uses IPX)

			NOTE-THIS CODE WORKS ON TCP/IP PROTOCOL ONLY

#endif


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"
#include "TCPCommunicationThread.h"
#include "TCPCommunicationDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern THE_APP_CLASS theApp;

/////////////////////////////////////////////////////////////////////////////
// CTCPCommunicationDlg dialog


CTCPCommunicationDlg::CTCPCommunicationDlg(CWnd* pParent /*=NULL*/,  LPVOID lpParam )
	: CDialog(CTCPCommunicationDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CTCPCommunicationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
		
	m_pstCCM = (ST_CLIENT_CONNECTION_MANAGEMENT *) lpParam;
	m_nMyRole = 0;	//0= undefined, 1= Receiver, 2= Sender
	m_pLocalCCM = m_pstCCM->pCCM;	// my boss
	m_pSocket = NULL;	// NO CLIENT socket yet
	m_uLastPacketsReceived = 0;
	m_nInXmitLoop = 0;
	m_uXmitLoopCount = 0;
	m_nThreadIdOld = 0;
	m_nConnectionRestartCounter = 0;
	}


void CTCPCommunicationDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTCPCommunicationDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CTCPCommunicationDlg, CDialog)
	//{{AFX_MSG_MAP(CTCPCommunicationDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP


	// manually added
	ON_MESSAGE(WM_USER_KILL_COM_DLG, VS05_OnCancel)  
	ON_MESSAGE(WM_USER_RESTART_ADP_CONNECTION, VS05_StartTCPCommunication)
	ON_MESSAGE(WM_USER_CLOSE_TCPIP_CONNECTION, Close)
	ON_MESSAGE(WM_USER_SEND_TCPIP_PACKET, TransmitPackets)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTCPCommunicationDlg message handlers

void CTCPCommunicationDlg::OnOK() 
	{
	// TODO: Add extra validation here
	CString s;
	StopTimer();
	Close(0,0);
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CTCPCommunicationDlg::OnCancel() 
	{		
	// TODO: Add extra cleanup here
	StopTimer();
	Close(0,0);

	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

LRESULT CTCPCommunicationDlg::Close(WPARAM, LPARAM)
	{
	CString s;
	if (!m_pstCCM)
		{
		TRACE("m_pstCCM is null... can not close client socket\n");
		return 0;	// major trouble here, this should never happen
		}	
	if (!m_pSocket)
		{
		//TRACE("Socket ptr is null\n");
		return 0;
		}
		
	EnterCriticalSection(m_pstCCM->pCSRcvPkt);
	s =  m_pLocalCCM->GetSocketName();
	if (m_pSocket)
		{
		m_pSocket->ShutDown(2);
		Sleep(10);
		m_pSocket->Close();

		if (m_pSocket)	delete m_pSocket;
		m_pSocket = NULL;
		TRACE1("OnCancel killed [%s] socket\n", s);
		}

	m_pstCCM->pSocket = NULL;
	LeaveCriticalSection(m_pstCCM->pCSRcvPkt);
	m_pLocalCCM->SetConnectionState(0);
	nConnectRetryTick = 0;
	return 0;
	}


// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CTCPCommunicationDlg::VS05_OnCancel(WPARAM, LPARAM)
	{	OnCancel();	return 0;	}

// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CTCPCommunicationDlg::VS05_StartTCPCommunication(WPARAM, LPARAM)
	{	StartTCPCommunication();	return 0;	}

// When the managing CCM wants to send a packet to a server, it queues  the packet
// into a linked list. Then it sends or posts a windows message to the Com Dlg
// instructing the dlg to check the linked list and send all queued messages.
// WPARAM and  LPARAM are unused at this time
afx_msg LRESULT CTCPCommunicationDlg::TransmitPackets(WPARAM, LPARAM)
	{
	int nRole;
	int nId = AfxGetThread()->m_nThreadID;
	if (nId != m_nThreadIdOld)
		{
		TRACE2("Transmit Packet old thread id=%d New thread id=%d\n", m_nThreadIdOld, nId);
		m_nThreadIdOld = nId;
		nRole = m_nMyRole;
		}

	if (m_nInXmitLoop)											return (LRESULT) 0;	// already in Transmit loop operation
	if (!m_pLocalCCM)											return (LRESULT) 0;
	if (!m_pLocalCCM->m_pstCCM)									return (LRESULT) 0;
	if (m_pLocalCCM->m_pstCCM->pSendPktPacketList->IsEmpty())	return (LRESULT) 0;	// nothing to send
	if (!m_pLocalCCM->m_pstCCM->pSocket)						return (LRESULT) 0;	// no socket to send with

	m_nInXmitLoop = 1;				// now entered into TransmitPacket loop

	stSEND_PACKET *pSendPkt;	// ptr to the packet info in the linked list of send packets

	TRACE("Send queued messages if any\n");
	// if we get to here, there is at least one packet to send
	while (m_pLocalCCM->m_pstCCM->pSendPktPacketList->GetCount() > 0)
		{
		m_pLocalCCM->LockSendPktList();
		pSendPkt = (stSEND_PACKET *) m_pLocalCCM->m_pstCCM->pSendPktPacketList->RemoveHead();
		m_pLocalCCM->UnLockSendPktList();	// give a higher priority thread a chance to add packets
		// do the socket send
		m_pLocalCCM->m_pstCCM->pSocket->Send(&pSendPkt->Msg[0], (int) pSendPkt->nLength);
		delete pSendPkt;
		m_uXmitLoopCount++;
		}

	m_nInXmitLoop = 0;	// now out of loop
	return (LRESULT) 1;	
	}




void CTCPCommunicationDlg::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	::PostQuitMessage(0);	// kill CWinThread app IpxIn
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	if ( m_nMyRole == 1)		m_pstCCM->hReceiveDlg = NULL;
	if ( m_nMyRole == 2)		m_pstCCM->hSendDlg = NULL;
//	m_pDlg[m_nMyIndx] = NULL;
	delete this;
	}

// Set my role and return to the global structure the handle for this dialog
void CTCPCommunicationDlg::SetMyRole(int n)
	{
	CString s;
	m_nMyRole = n;
	if ( n == 1)
		{
		m_pstCCM->hReceiveDlg = this->m_hWnd;
		s.Format(_T("Receive Dlg handle = %x08\n"),m_pstCCM->hReceiveDlg);
		m_sMyRole = _T("Receiver");
		if (NULL == m_pstCCM->pSocket)
			{
			::PostMessage(m_pstCCM->hReceiveDlg, WM_USER_RESTART_ADP_CONNECTION,0,0);
			Sleep(10);
			}
		}
	else if ( n == 2)	
		{
		m_pstCCM->hSendDlg = this->m_hWnd;
		s.Format(_T("Send Dlg handle = %x08\n"),m_pstCCM->hSendDlg);
		m_sMyRole = _T("Sender");
		}
	else 
		{
		s = _T("Invalid role number\n");
		m_sMyRole = _T("Unknown");
		}
	TRACE(s);
	StartTimer();
	}

BOOL CTCPCommunicationDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	HANDLE hThread;
	int nThreadId, nPriority;
	CString s;

	// a global variable capturing this dlg window handle
//	hInspComDlgWindow[m_nMyIndx+UDP_SOCKET_QTY] = m_pDlg[m_nMyIndx]->m_hWnd;

	hThread		= AfxGetThread()->m_hThread;
	nThreadId	= AfxGetThread()->m_nThreadID;
	nPriority	= AfxGetThread()->GetThreadPriority();
	s = m_pstCCM->szSocketName;
	if (m_nMyRole == 1)	
		TRACE3("OnInitDlg - Receive- in Com Dlg[%0x] (%s) with priority = %d has run\n",
			nThreadId, s, nPriority);
	else if (m_nMyRole == 2)
		TRACE3("OnInitDlg - Send- in Com Dlg[%0x] (%s) with priority = %d has run\n",
			nThreadId, s, nPriority);
	else
		TRACE3("OnInitDlg - ???- in Com Dlg[%0x] (%s) with priority = %d has run\n",
			nThreadId, s, nPriority);

	
	m_uTimerHandle = 0;
	m_uTimerTick = 0;
	m_StartAdpQty = m_StartSocketQty = 0;
	// Should only have mid speed wall or agslite here

	//StartTimer();  move to SetRole

	// Post a message to ourselves to tell us to start/restart tcp/ip
	// connection with NIOS ADP instrument.
	// move the PostMessage operation below to AdpMMIDlg.cpp OnTimer
//	::PostMessage(hInspComDlgWindow[m_pstCCM->nInstrumentSelector],WM_USER_RESTART_ADP_CONNECTION,0,0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}


// call by 	ON_MESSAGE(WM_USER_RESTART_ADP_CONNECTION, VS05_StartTCPCommunication)
// This message is posted from AdpMMIDlg OnTimer function

void CTCPCommunicationDlg::StartTCPCommunication()
	{
	int nSockOpt = TRUE;
	int  sockerr=0;
	short nPort;
	CString s, stmp;

	if (m_nMyRole != 1)
	{
		TRACE("Not a receiver dialog.. only receiver dialog creates the socket\n");
		return;
	}

	if (!m_pstCCM)
		{
		TRACE("m_pstCCM is null... will not create client socket\n");
		return;	// major trouble here, this should never happen
		}	
	if (! m_pLocalCCM)
		{
		TRACE("m_pLocalCCM is null... will not create client socket\n");
		return;	// major trouble here, this should never happen
		}	
	
	// Create an CAsync socket
	// Our main dlg should have identified the client and server side IP's before now


	m_pLocalCCM->SetConnectionState(0);	// now assume we are not connected

	EnterCriticalSection(m_pstCCM->pCSRcvPkt);
	if (m_pSocket)
		{
		TRACE1("[%03d] Client socket already exists.... close and destroy before recreating\n", m_nConnectionRestartCounter++);
		m_pSocket->Close();
		Sleep(10);
		delete m_pSocket;
		m_pSocket = NULL;
		}

	m_pSocket = new CClientSocket(m_pLocalCCM);


// Purely Randy's work
// fix a well known Microsoft screw up that occurs
// when using sockets in a multithreaded application 
// that is linked with static libraries.  It seems the
// static libraries do not properly init the hash maps
// so we have to do it manually.  sigh........
#ifndef _AFXDLL

	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	AfxSocketInit();
#if 0
	if (pState->m_pmapSocketHandle	== NULL)	
		pState->m_pmapSocketHandle	= new CMapPtrToPtr;

	if (pState->m_pmapDeadSockets	== NULL)	
		pState->m_pmapDeadSockets	= new CMapPtrToPtr;
//	if (pState->m_pmapHWND			== NULL)
//		pState->m_pmapHWND			= new CHandleMap();

	if (pState->m_pCurrentWinThread	== NULL)
		pState->m_pCurrentWinThread	= m_pLocalCCM->m_pstCCM->pReceiveThread;

	if (pState->m_plistSocketNotifications == NULL)
		pState->m_plistSocketNotifications = new CPtrList;
#endif
#endif


	LeaveCriticalSection(m_pstCCM->pCSRcvPkt);
	
	if( !m_pSocket )
		{
		TRACE("Failed to create Client Socket\n");
		return;// C_CLIENT_SOCKET_CREATION_ERROR;	// Socket object creat error
		}

	// Create the socket, then find the server address and port and attempt to connect

	// Using tcp/ip
	nPort = 0;
	// we will override an ASyncSocket class virtual function when (1) packet received, (2) connect to server completes, 
	// (3) the socket closes
	if (m_pSocket->Create(nPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE,  NULL )	!= 0 )
		{	// Socket created

		nSockOpt = 1;
		// need to be able to reuse the ip address
		sockerr = m_pSocket->SetSockOpt(SO_REUSEADDR, &nSockOpt, sizeof(int),SOL_SOCKET);
		if (sockerr == SOCKET_ERROR) TRACE1("Socket Error SO_REUSEADDR = %0x\n", sockerr);
		
		nSockOpt = 1;
		// when data ready to send, send without delay
		sockerr = m_pSocket->SetSockOpt(TCP_NODELAY, &nSockOpt, sizeof(int),IPPROTO_TCP); 
		if (sockerr == SOCKET_ERROR) TRACE1("Socket Error TCP_NODELAY = %0x\n", sockerr);

		nSockOpt = 1;
		sockerr = m_pSocket->SetSockOpt(SO_DONTLINGER, &nSockOpt, sizeof(int),SOL_SOCKET);
		if (sockerr == SOCKET_ERROR) TRACE1("Socket Error SO_DONTLINGER = %0x\n", sockerr);
		}

	else
		{								
		
		TRACE("Failed to create stream socket... aborting\n");
		return;	// C_CLIENT_SOCKET_CREATION_ERROR;
		}

	   BOOL rtn;
	   // select the servers port connection value based on the OS version
	   // and attempt to CONNECT to the server
		switch( theApp.m_iWinVer )
			{
		case 7:		// for Win 7
			rtn = m_pSocket->Connect(stmp, MC_SYSCP_LISTENPORT_WIN7 );
			break;
		default:	// for Win XP
			rtn = m_pSocket->Connect(stmp, MC_SYSCP_LISTENPORT );
			}
		if ( rtn == 0 )
			{
			int nError = GetLastError();	//10035 is WSAEWOULDBLOCK..normal 
			// if it blocks, eventually we will probably get an OnConnect which will
			// set the connected flag

			if (WSAEWOULDBLOCK == nError)
				{
				// this is what always happens here, in a moment it will connect and the OnConnect
				// code in CClientSocket will complete the socket connection operation.
				s.Format(_T("Connect Error = %d ...waiting to connect\n"), nError);
				DebugMsg(s);
				return;
				}

			s.Format(_T("Connect Error = %d\n"), nError);
			DebugMsg(s);
			DebugMsg( _T("SysCP: connect failed.\n") );
			m_pSocket->ShutDown(2);
			m_pSocket->Close();		
			delete m_pSocket;
			return;	// C_CLIENT_SOCKET_CREATION_ERROR;
			}
		else
			{
			DebugMsg( _T("SysCP: connected.\n") );	// Connect to server named xxx at ip = yyyy
			m_pLocalCCM->SetSocketPtr(m_pSocket);	// store socket into stCCM for use by send and receive threads
			}

	}


void CTCPCommunicationDlg::StopTimer(void)
	{
	if (m_uTimerHandle)
		{
		KillTimer(m_uTimerHandle);
		m_uTimerHandle = 0;
		}
	}

#define SEND_TIMER_PERIOD		250
#define RECEIVE_TIMER_PERIOD	1000

void CTCPCommunicationDlg::StartTimer(void)
	{
	int nPeriod;
	CString s;

	if (m_uTimerHandle)	return;	// already running

	if (m_nMyRole == 1)
		{
		nPeriod = RECEIVE_TIMER_PERIOD;
		// prepare for the event of a failure
		s.Format( _T("Failed to start ComDlg Receive timer CCM[%d]"), m_pLocalCCM->m_nMyConnection);
		nConnectRetryTick = 0;
		}
	else if (m_nMyRole == 2)
		{
		nPeriod = SEND_TIMER_PERIOD;
		s.Format( _T("Failed to start ComDlg Send timer CCM[%d]"), m_pLocalCCM->m_nMyConnection);
		}
	else return;

	// 30 ms timer cycle
	m_uTimerHandle = SetTimer(IDT_ADP_RESTART_TIMER, nPeriod, NULL);
	if (!m_uTimerHandle) 
		TRACE(s);	// oh yea, this dialog is invisible so we can only show in debug output window.
	}

// Normal operation of TCP streams
void CTCPCommunicationDlg::OnTimer(UINT nIDEvent) 
	{
		// TODO: Add your message handler code here and/or call default
	CString s;

	m_uTimerTick++;

	switch(m_nMyRole)
	{
	default:
	case 0:		// problems here
		s.Format(_T("My role is not sender or receiver dialog [%d]\n"), m_nMyRole);
		TRACE(s);
		CDialog::OnTimer(nIDEvent);
		return;

	case 1:	// receiver
		if (m_uLastPacketsReceived != m_pstCCM->uPacketsReceived)	// or check connected status
			{
			nConnectRetryTick = 0;
			m_uLastPacketsReceived = m_pstCCM->uPacketsReceived;
			}
		else
			{
			nConnectRetryTick++;
			// restart if no new packets after time out period
			if (nConnectRetryTick > 50)		// about 50 seconds
				{	// attempt a new connect to the ADP
				nConnectRetryTick = 0;
				// Post a message to ourselves to tell us to start/restart tcp/ip
				::PostMessage(m_pstCCM->hReceiveDlg,
								WM_USER_RESTART_ADP_CONNECTION,0,0);
				}
			}
		break;

	case 2:	// sender
		// check the output message queue and send if the queue is not empty
		if (0 == m_nInXmitLoop)	TransmitPackets(0, 0L);
		break;
	}


	// Must queue messages into pDataOutPacketList in app thread or else we sleep
	// the communication thread and prevent if from servicing ethernet
		
	CDialog::OnTimer(nIDEvent);
	}

// Allow outside world to set the connection as disconnected
void CTCPCommunicationDlg::SetDisConnected(void)
	{
	m_pLocalCCM->SetConnectionState(0);
	}

void CTCPCommunicationDlg::DebugMsg(CString s)
{
	// for present only trace out. Could put in debug linked list and let someone display
	TRACE(s);
}

CString CTCPCommunicationDlg::ResolveHostIP(CString hostname)	// stolen from ACP code
{
	int i;
	// because we are running a system using DHCP, DNS,
	// and dynamic IP addresses, we have no prior knowledge
	// about any given host's IP address so we have to
	// fish for it

	// this method searches for a computer specified
	// by hostname and returns its IP address

	CString theIP;	// the stringized ip of the server
    theIP.Empty();	// assume the lookup will fail

	// make sure we're searching for something real
    if (hostname.IsEmpty())
		return theIP;

    struct sockaddr_in theHost;	// structure used in the conversion
	HOSTENT * hostent;			// structure returned by gethostbyname

    // do the lookup
	char name[20];
	for( i=0; i<hostname.GetLength(); i++ )
	{
		name[i] = (char)hostname[i];
	}
	name[i] = 0;
	hostent = gethostbyname(name);

	// make sure we found something
	if (hostent != NULL)
	{
	   // save the important bits
	   // note, if the host is a multi-homed
	   // then hostent->h_address_list WILL CONTAIN
	   // MORE THAN ONE IP. this method currently
	   // assumes that the host is NOT multi-homed
       memcpy(&theHost.sin_addr, hostent->h_addr_list[0], hostent->h_length);

	   // convert to IP address
	   theIP = inet_ntoa((IN_ADDR) theHost.sin_addr);
	} else {
	   theIP.Empty();
	}

	// return something to the caller
	return theIP;
}
CString CTCPCommunicationDlg::GetIPv4(void)
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
CString CTCPCommunicationDlg::GetIPv4(CString sComputerName)
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
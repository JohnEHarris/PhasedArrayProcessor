/** Copyright (C) 2008 Tuboscope, a National Oilwell Varco Company. All rights reserved  **/
// ClientCommunicationThread.cpp : implementation file
//

#if 0
Author:	jeh
Purposose:	Create a separate UI thread (HAS MESSAGE PUMP) to 'house' the tcp/ip
			communications Async socket.  This socket receives inspection data Idata
			from the NIOS ADP and sends commands to it.

			Windows (aka Bill Gates software) manages Async socket class call back function
			with windows messages (WM_whatever).  Consequently, to process those
			messages and reach the call back function, a thread must have a message pump.
			This does not normally happen with a worker thread.

			This thread will do nothing but create a modeless dialog which is INVISIVLE.
			This will cause the development system to put in all the necessary message handling.
			Modeless dialog created is called CTCPCommunicationDlg.

			Similar code is found in standalone MMI for Enet2 board.  Modules are called
			IpxIn.cpp and IpxDlg.cpp in that code.


Date:		11-Feb-2005
Revised:	02-Mar-2005 This code works the same, but the CClientSocket is now using UDP protocol
			21-Jun-12 Added to Mill console code. This thread now controls a receiving dialog or
					a sending dialog allowing send/receive operations to be at different priorities.
					ENET_THREAD_PARAMETERS structure has been replaced with a more general purpose 
					structure called ST_CLIENT_CONNECTION_MANAGEMENT. Also there is a static array
					of ST_CLIENT_CONNECTION_MANAGEMENT's which is used by the connection classes as 
					well as the rest of the application.
			13-Nov-12 Eliminate hidden dialog for controlling communication. Socket data processing of
					received data now done in this module. This module is part of the 
					ClientConnectionManagement  system. At this time, the server connected thru this
					scheme is the SysCp.

#endif
#include "stdafx.h"
#include <string.h>
					
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"
#ifdef I_AM_PAG
//#include "Truscan.h"
//#include "TscanDlg.h"
#include "PA2WinDlg.h"
//class CTscanDlg;
//extern CTscanDlg* pCTscanDlg;
#include "resource.h"


#else
//#include "ServiceApp.h"
#include "PA2WinDlg.h"
#endif

//#include "ClientSocket.h"
//#include "ClientConnectionManagement.h"
//#include "ClientCommunicationThread.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

	

/////////////////////////////////////////////////////////////////////////////
// CClientCommunicationThread

IMPLEMENT_DYNCREATE(CClientCommunicationThread, CWinThread) // subtype c0, 128 bytes long. memory leak

CClientCommunicationThread::CClientCommunicationThread()
	{
	int i;
	m_nMyRole			= 0;
	m_nInXmitLoop		= 0;
	m_nThreadIdOld		= 0;
	for ( i = 0; i < 3; i++)
		m_nConnectRetryTick[i]	= 0;
	m_pSocket			= NULL;
	m_nConnectionRestartCounter		= 0;
	m_nTick				= 0;
	m_uLastPacketsReceived = 0;
	m_pMyCCM			= NULL;
	m_nDebugCount		= 0;
	m_wMsgSeqCount		= 0;
	m_wMsgSeqCountAW	= 0;
	m_DebugLimit		= 0;
	m_nDebugEmptyList	= 0;
	m_pElapseTimer		= 0;
	//strcpy(m_pElapseTimer->tag, "CComThrd89 ");
	m_nTimerPacketsWaiting = 0;
	m_nConsecutiveFailedXmit = 0;
	m_nConsecutiveFailedXmitAW = 0;
	}

CClientCommunicationThread::~CClientCommunicationThread()
	{
	int i = -1;
	CString s = _T("");
	CString t = _T("");

	if (m_pElapseTimer)
		{
		strcat(m_pElapseTimer->tag, "CComThrd102\n");
		s = m_pElapseTimer->tag;
		//TRACE(s);
		delete m_pElapseTimer;
		m_pElapseTimer = 0;
		}

	if (m_pstCCM)
		{
		if (m_pstCCM->pCCM)
			i = m_pstCCM->pCCM->m_nMyConnection;
		else i = 01234;
		}

	switch (this->m_nMyRole)
		{
	case 1:		
		s.Format(_T("Rcvr Com thread[%d],Thread ID %d Destructor ran\n"), i, AfxGetThread()->m_nThreadID);
		TRACE(s);
		if (m_pstCCM->pReceiveThread)
			{
			//delete m_pstCCM->pReceiveThread; already done by ExitInstance
			m_pstCCM->pReceiveThread = NULL;
			t = _T("~CClientCommunicationThread() receive thread not null\n");
			TRACE(t);
			}
		if (m_pstCCM->pSocket)
			{
			m_pstCCM->pSocket = NULL;
			t = _T("~CClientCommunicationThread() ASync socket not null\n");
			TRACE(t);
			}
		//AfxEndThread( 0 );	// add here, take out in Kill Send Thread
		break;
	case 2:
		s.Format(_T("Send Com thread[%d],Thread ID %d Destructor ran\n"), i, AfxGetThread()->m_nThreadID);
		TRACE(s);
		if (m_pstCCM->pSendThread)
			{
			//delete m_pstCCM->pSendThread;
			m_pstCCM->pSendThread = NULL;
			t = _T("~CClientCommunicationThread() send thread not null\n");
			TRACE(t);
			}
		//AfxEndThread( 0 );	// add here, take out in Kill Send Thread
		break;
	default:
		TRACE( _T( "Unknown CCT Thread\n" ) );
		break;
		//AfxEndThread( 0 );
		}
	//AfxEndThread( 0 );
	}

// 2017-05-02 created suspended.
// Caller builds the client socket inside this thread
// The client socket shown in the CCM is removed.... there were
// 2 client socket sturctures previously.
// The linked lists and critical sections stay in the CCM
BOOL CClientCommunicationThread::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
// Purely Randy's work
// fix a well known Microsoft screw up that occurs
// when using sockets in a multithreaded application 
// that is linked with static libraries.  It seems the
// static libraries do not properly init the hash maps
// so we have to do it manually.  sigh........
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	AfxSocketInit();
#endif

	return TRUE;
	}

// Self deleting threads
int CClientCommunicationThread::ExitInstance()
	{
#if 0
	// TODO:  perform any per-thread cleanup here
	void *pv;
	int i;
	switch (m_nMyRole)
		{
	default:	TRACE(_T("?? Com thread ExitInstance()\n"));	break;
	case 1:		
		TRACE(_T("Rcvr Com thread ExitInstance()\n"));
		if (NULL == m_pMyCCM)				break;
		if (NULL == m_pstCCM)				break;
		if (NULL == m_pstCCM->pCSRcvPkt)	break;
		m_pMyCCM->SetConnectionState(0);
		EnterCriticalSection(m_pstCCM->pCSRcvPkt);
		if (m_pstCCM->pSocket)
			{
			m_pstCCM->pSocket->Close();
			Sleep(10);
			delete m_pstCCM->pSocket;
			m_pstCCM->pSocket = NULL;
			m_pMyCCM->SetSocketPtr( NULL );
			}

		if (m_pstCCM->pRcvPktPacketList)
			{
			while (m_pstCCM->pRcvPktPacketList->GetCount())
				{
				pv = m_pstCCM->pRcvPktPacketList->RemoveHead();
				delete pv;
				}
			}
		LeaveCriticalSection(m_pstCCM->pCSRcvPkt);
		delete m_pstCCM->pRcvPktPacketList;
		delete m_pstCCM->pCSRcvPkt;
		m_pstCCM->pRcvPktPacketList = 0;
		m_pstCCM->pCSRcvPkt = 0;
		// repeat for other lists and sections

		if (m_pstCCM->pCSDebugIn)
			{
			EnterCriticalSection( m_pstCCM->pCSDebugIn );
			if (m_pstCCM->pInDebugMessageList)
				{
				while (m_pstCCM->pInDebugMessageList->GetCount())
					{
					pv = m_pstCCM->pInDebugMessageList->RemoveHead();
					delete pv;
					}
				}
			LeaveCriticalSection( m_pstCCM->pCSDebugIn );
			}
		delete m_pstCCM->pInDebugMessageList;
		delete m_pstCCM->pCSDebugIn;
		m_pstCCM->pInDebugMessageList = 0;
		m_pstCCM->pCSDebugIn = 0;

		EnterCriticalSection(m_pstCCM->pCSDebugOut);
		while (m_pstCCM->pOutDebugMessageList->GetCount())
			{
			pv = m_pstCCM->pOutDebugMessageList->RemoveHead();
			delete pv;
			}		
		LeaveCriticalSection(m_pstCCM->pCSDebugOut);
		delete m_pstCCM->pOutDebugMessageList;
		delete m_pstCCM->pCSDebugOut;
		m_pstCCM->pOutDebugMessageList = 0;
		m_pstCCM->pCSDebugOut = 0;

		EnterCriticalSection(m_pstCCM->pCSSendPkt);
		while (m_pstCCM->pSendPktList->GetCount())
			{
			pv = m_pstCCM->pOutDebugMessageList->RemoveHead();
			delete pv;
			}
		LeaveCriticalSection(m_pstCCM->pCSSendPkt);
		delete m_pstCCM->pSendPktList;
		delete m_pstCCM->pCSSendPkt;
		m_pstCCM->pSendPktList = 0;
		m_pstCCM->pCSSendPkt = 0;

		//delete m_pstCCM->pReceiveThread; auto delete from creation
		m_pstCCM->pReceiveThread = 0;
		i = 1;
		break;

	case 2:		
		TRACE(_T("Send Com thread ExitInstance()\n"));
		//delete m_pstCCM->pSendThread;
		m_pstCCM->pSendThread = 0;
		i = 2;
		break;
		}
	i = 3;
	return (i =  CWinThread::ExitInstance());
#endif
	return 0;
	}

BEGIN_MESSAGE_MAP(CClientCommunicationThread, CWinThread)
	//{{AFX_MSG_MAP(CClientCommunicationThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP

	ON_THREAD_MESSAGE(WM_USER_INIT_TCP_THREAD,InitTcpThread)
	ON_THREAD_MESSAGE(WM_USER_KILL_RECV_THREAD,KillReceiveThread)
	ON_THREAD_MESSAGE(WM_USER_KILL_SEND_THREAD,KillSendThread)
	//ON_THREAD_MESSAGE(WM_USER_RESTART_TCP_COM_DLG,RestartTcpComDlg)
	//ON_THREAD_MESSAGE(WM_USER_RESTART_ADP_CONNECTION,RestartTcpComDlg)
	ON_THREAD_MESSAGE(WM_USER_TIMER_TICK, OnTimer)
	ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, TransmitPackets)

	// These messages go to the receiver thread
	ON_THREAD_MESSAGE(WM_USER_CREATE_SOCKET, CreateSocket)	
	ON_THREAD_MESSAGE(WM_USER_CONNECT_SOCKET, ConnectSocket)	
	ON_THREAD_MESSAGE(WM_USER_KILL_SOCKET, KillSocket)	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientCommunicationThread message handlers

// Pass in a pointer to the ST_CLIENT_CONNECTION_MANAGEMENT for this socket/machine
// 6-27-12 pass ptr to controlling ccm class instance
// WPARAM w contains my role, either receiving (1) or sending (2)
// called from CClientConnectionManagement::InitReceiveThread(void) which sends
// a thread message to start this thread function
afx_msg void CClientCommunicationThread::InitTcpThread(WPARAM w, LPARAM lParam)
	{
	int i;		// which thread/socekt/dailog out of several is this
	m_pMyCCM = (CClientConnectionManagement *) lParam;
	if (NULL == m_pMyCCM)
		{
		TRACE(_T("InitTcpThread m_pMyCCM is null\n"));
		return;
		}
	m_pstCCM = m_pMyCCM->m_pstCCM;	// pointer to my static structure in global memory

	if (NULL == m_pstCCM)
	{
		TRACE(_T("InitTcpThread m_pstCCM is null\n"));
		return;
	}

	m_nMyRole = (int) w;	// 1=receiving, 2= sending
	if (w == 1)	m_sMyRole = _T("RCVR");
	else if (w == 2)	m_sMyRole = _T("SND");
	else m_sMyRole = _T("???");

	i =  m_pMyCCM->m_nMyConnection;
	TRACE3("Inst Com Thread[%d] is running [id=%0x] [priority=%d]\n", (2*i + m_nMyRole-1),
				AfxGetThread()->m_nThreadID, AfxGetThread()->GetThreadPriority());

	// Start our invisible communication dialog - not after 11-13-2012
	switch (m_nMyRole)
	{
	default:
		TRACE(_T("Role is undefined..aborting\n"));
		return;
	case 1:		// receiver
		// Only the receiver thread makes the socket connection
		//m_pMyCCM->SetSocketPtr(NULL);
		if (m_pElapseTimer == 0)
			m_pElapseTimer = new CHwTimer();
		strcpy( m_pElapseTimer->tag, "CComThrd341-Rcv " );
		StartTCPCommunication();
		break;

	case 2:		// sender
		TRACE(_T("CCT Sender thread cannot create socket\n"));
		if (m_pElapseTimer == 0)
			m_pElapseTimer = new CHwTimer();
		strcpy( m_pElapseTimer->tag, "CComThrd349-Snd " );
		break;
	}

	return;	// 0;
	}
// For shutdown must kill threads and linked lists
#if 1
afx_msg void CClientCommunicationThread::KillReceiveThread(WPARAM w, LPARAM lParam)
	{
	int nError, i;
	CString s;
	//void *pV;

	// Since Receive created the socket, it will kill the socket
	if (m_nMyRole != 1)
		{
		TRACE("Not the Receiver Thread that is request to be killed\n");
		ASSERT(0);
		}
	if (!m_pstCCM)
		{
		TRACE("m_pstCCM is null... cannot access client socket\n");
		return;	// major trouble here, this should never happen
		}
	if (! m_pMyCCM)
		{
		TRACE("m_pMyCCM is null... will not access client socket\n");
		return;	// major trouble here, this should never happen
		}

	if (m_pElapseTimer)
		{	delete m_pElapseTimer;		m_pElapseTimer = 0;		}

	if (m_pstCCM->pSocket)
		{
		if (i = m_pstCCM->pSocket->ShutDown(2))
			{
			s.Format(_T("Client Socket to PAG shut down with result = %d\n"),i);
			TRACE( s );
			//m_pstCCM->pSocket->Close();
			}
		else
			{
			nError = GetLastError();	// WSAENOTCONN                      10057L
			s.Format(_T("Shutdown of PAG client socket failed, %d\n"), nError);
			}
		if (m_pstCCM->pSocket)
			{
			if (m_pstCCM->pSocket->m_pFifo)
				{
				delete m_pstCCM->pSocket->m_pFifo;
				m_pstCCM->pSocket->m_pFifo = 0;
				}
			if (m_pstCCM->pSocket->m_pElapseTimer)
				{
				delete m_pstCCM->pSocket->m_pElapseTimer;
				m_pstCCM->pSocket->m_pElapseTimer = 0;
				}

			// code above should not be necessary. Deleting socket should kill the timer and fifo 2018-04-20
			delete m_pstCCM->pSocket;
			}
		m_pstCCM->pSocket = 0;
		m_pSocket = 0;
		TRACE("KillReceiveThread killed the socket\n");
		}

	//delete	m_pstCCM->pReceiveThread;		//this;
	PostQuitMessage( 0 );
	}

afx_msg void CClientCommunicationThread::KillSendThread(WPARAM w, LPARAM lParam)
	{
	//int nError, i;
	CString s;
	//void *pV;

	if (m_nMyRole != 2)
		{
		TRACE("Not the Sender Thread that is request to be killed\n");
		ASSERT(0);
		}
	// Since Receive created the socket, it will kill the socket
	if (!m_pstCCM)
		{
		TRACE("m_pstCCM is null... cannot access client socket\n");
		return;	// major trouble here, this should never happen
		}	
	if (! m_pMyCCM)
		{
		TRACE("m_pMyCCM is null... will not access client socket\n");
		return;	// major trouble here, this should never happen
		}
	if (m_nMyRole == 2)
		{
		// yes I am the sender thread.
		//Nothing left to do but exit
		s = _T( "Sender thread exiting\n" );
		TRACE( s );
		}

	if (m_pElapseTimer)
		{	delete m_pElapseTimer;		m_pElapseTimer = 0;		}
		
	//delete this;
	PostQuitMessage( 0 );
	//ExitInstance();
	//delete m_pstCCM->pSendThread;
	}
#endif


// Create the socket at the priority level of the receiver thread
void CClientCommunicationThread::CreateSocket(WPARAM w, LPARAM lParam)
	{
	int nSockOpt, sockerr;
	CString s;


	if ((int)w != 1)
		{
		TRACE( _T( "CreateSocket request of wrong thread type. Must be Receiver thread\n" ) );
		return;
		}
	TRACE( _T( "CreateSocket executed\n" ) );
	// make a new 'connect' client socket
	if (m_pMyCCM == nullptr)
		{
		TRACE( _T( "m_pMyCCM == nullptr\n" ) );
		return;
		}
	if (m_pMyCCM->m_pstCCM == nullptr)
		{
		TRACE( _T( "m_pMyCCM->m_pstCCM == nullptr\n" ) );
		return;
		}

	m_pMyCCM->m_pstCCM->pSocket = m_pSocket = new CClientSocket( m_pMyCCM );	// subtype c0, 16 bytes long.
	// The constructor makes a copy of the pointer that is in the
	// ClientCommunicationManager class


	// Purely Randy's work
	// fix a well known Microsoft screw up that occurs
	// when using sockets in a multithreaded application 
	// that is linked with static libraries.  It seems the
	// static libraries do not properly init the hash maps
	// so we have to do it manually.  sigh........
#ifndef _AFXDLL

		AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
		AfxSocketInit();
#endif



	if (!m_pSocket)
		{
		TRACE( "Failed to create Client Socket\n" );
		//LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
		return;// C_CLIENT_SOCKET_CREATION_ERROR;	// Socket object creat error
		}

	// Create the socket, then find the server address and port and attempt to connect

	// Using tcp/ip
	m_nPort = 0;
	// we will override an ASyncSocket class virtual function when (1) packet received, (2) connect to server completes, 
	// (3) the socket closes
	//	if (m_pSocket->Create(nPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE,  NULL )	!= 0 )
	if (m_pSocket->Create( m_nPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE, NULL ) != 0)
		{	// Socket created

		nSockOpt = 1;
		// need to be able to reuse the ip address
		sockerr = m_pSocket->SetSockOpt( SO_REUSEADDR, &nSockOpt, sizeof( int ), SOL_SOCKET );
		if (sockerr == SOCKET_ERROR) TRACE1( "Socket Error SO_REUSEADDR = %0x\n", sockerr );

		nSockOpt = 1;
		// when data ready to send, send without delay
		sockerr = m_pSocket->SetSockOpt( TCP_NODELAY, &nSockOpt, sizeof( int ), IPPROTO_TCP );
		if (sockerr == SOCKET_ERROR) TRACE1( "Socket Error TCP_NODELAY = %0x\n", sockerr );

		nSockOpt = 1;
		sockerr = m_pSocket->SetSockOpt( SO_DONTLINGER, &nSockOpt, sizeof( int ), SOL_SOCKET );
		if (sockerr == SOCKET_ERROR) TRACE1( "Socket Error SO_DONTLINGER = %0x\n", sockerr );
		}

	else
		{

		TRACE( "Failed to create stream socket... aborting\n" );
		//LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
		return;	// C_CLIENT_SOCKET_CREATION_ERROR;
		}

	// Try server IP4 address before server name
	m_sSrv = m_pstCCM->sServerIP4;

	if (m_pstCCM->sServerIP4.IsEmpty())
		{
		m_sSrv = m_pstCCM->sServerName;
		if (m_sSrv.IsEmpty())
			{
			s = _T( "Could not find Server name or IP Address... Aborting\n" );
			DebugMsg( s );
			m_pSocket->ShutDown( 2 );
			//m_pSocket->Close(); The socket object's destructor calls Close for you.
			delete m_pSocket;
			m_pSocket = NULL;	// null the local member
			m_pMyCCM->SetSocketPtr( m_pSocket );	// null the pointer in CCM
			//LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
			return;	// C_CLIENT_SOCKET_CREATION_ERROR;
			}
		}

	m_nPort = m_pstCCM->uServerPort;

		// make a new 'connect' client socket
	}

// Call the socket connect function from StartTCPCommunication
void CClientCommunicationThread::ConnectSocket(WPARAM w, LPARAM lParam)
	{
	CString s,t;
	BOOL rtn;
	if ((int)w != 1)
		{
		TRACE( _T( "ConnectSocket request of wrong thread type. Must be Receiver thread\n" ) );
		return;
		}

	if (m_pstCCM == nullptr)
		{		TRACE( _T( "m_pstCCM is null\n" ) );		return;		}
	if (m_pMyCCM == nullptr)
		{		TRACE( _T( "m_pMyCCM is null\n" ) );		return;		}
	if (m_pSocket == nullptr)
		{		TRACE( _T( "m_pSocket is null\n" ) );		return;		}

	rtn = m_pSocket->Connect(m_pstCCM->sServerIP4, m_nPort );
		
	if ( rtn == 0 )
		{
		m_pstCCM->nOnConnectError = GetLastError();	//10035 is WSAEWOULDBLOCK..normal 
		// if it blocks, eventually we will probably get an OnConnect which will
		// set the connected flag
		// WSAEINVAL already bound to a socket		10022L
		// #define WSAEISCONN                       10056L

		if ( (WSAEWOULDBLOCK == m_pstCCM->nOnConnectError) || 
			(WSAEINVAL == m_pstCCM->nOnConnectError) || 
			(WSAEISCONN  == m_pstCCM->nOnConnectError))
			{
			// THIS IS WHAT ALWAYS HAPPENS HERE !!!, in a moment it will connect and the OnConnect
			// code in CClientSocket will complete the socket connection operation.
			s.Format(_T("Connect Error = %d ...waiting to connect to server %s at %s : %d\n"), 
				m_pstCCM->nOnConnectError, m_pstCCM->sServerName,
				m_pstCCM->sServerIP4, m_nPort);
			t = GetTimeString();
			t += _T("\n");
			s += t;
			DebugMsg(s);
			return;
			}

		s.Format(_T("Connect Error = %d\n"), m_pstCCM->nOnConnectError);
		TRACE(s);   //DebugMsg(s)
		s = m_pstCCM->szSocketName;
		s += _T(": connect failed.\n");
		TRACE(s);   //DebugMsg(s)
		m_pSocket->ShutDown(2);
		//m_pSocket->Close();		The socket object's destructor calls Close for you.
		delete m_pSocket;
		m_pSocket = NULL;
		m_pMyCCM->SetSocketPtr(m_pSocket);
		m_pMyCCM->SetConnectionState( 0 );
		return;	// C_CLIENT_SOCKET_CREATION_ERROR;
		}
	else
			{
			m_pMyCCM->SetSocketPtr(m_pSocket);		// store socket into stCCM for use by send and receive threads
			s.Format(_T("PAG or %s: connected using socket at 0x%08x sizeof=%d.\n"), 
			//s.Format(_T("SysCP or %s: connected using socket at 0x%08x sizeof=%d.\n"), 
				m_sSrv, m_pSocket, sizeof(CClientSocket));
			TRACE(s);   //DebugMsg(s)	// Connect to server named xxx at ip = yyyy
			}

	TRACE( _T( "ConnectSocket executed\n" ) );
	}

// destroy the socket but leave the send/receive/cmd threads and linked list intact
void CClientCommunicationThread::KillSocket(WPARAM w, LPARAM lParam)
	{
	int i, nError;
	CString s;
	s = m_sMyRole;	// debugging
	s += _T("  ");
	s += m_sSrv;
	s +=_T("\n");

	TRACE(s);
	TRACE( _T( "KillSocket executed\n" ) );
	if (m_pstCCM->pSocket)
		{
		i = m_pstCCM->pSocket->ShutDown( 2 );
		nError = GetLastError();
		if (i > 0)
			{
			s.Format( _T( "Shutdown = %d\n" ), i );
			TRACE( s );
			}
		else
			{
			s.Format( _T( "Shutdown Error = %d\n" ), nError );
			TRACE( s );
			}
		// The socket object's destructor calls Close for you. from Microsoft
		//	m_pstCCM->pSocket->Close();
		delete m_pstCCM->pSocket;
		}

	if (nShutDown)
		{
		KillReceiveThread(1, 0L);
		// maybe kill send thread??
		}
	Sleep( 10 );
	}

void CClientCommunicationThread::MyMessageBox( CString s )
	{
	TRACE( s );
	}


// Taken from CTCPCommunicationDlg to eliminate need for hidden windows dialog
// 2017-05-12 This code must look at the overall situation and determine what if
// anything needs to be done to start the client socket
// Situations:
// The socket does not exist
// The receive thread which connects the socket does not exist
// The socket has closed
// The socket has been refused a connection request
// The system is shutting down
// StartTCPCommunication is called because the system believes that the socket is not connected
//
void CClientCommunicationThread::StartTCPCommunication()
	{
	int nSockOpt = TRUE;
	int  sockerr=0;
	int i = 0;
	CString s,t;
	BOOL rtn;
	int nLastConnectError;


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
	if (! m_pMyCCM)
		{
		TRACE("m_pMyCCM is null... will not create client socket\n");
		return;	// major trouble here, this should never happen
		}	
	
	// Our main dlg should have identified the client and server side IP's before now

	if (m_pstCCM->bConnected)
		return;

	// It there is no receiver thread we are unable to do anything further
	// It should be running. If not, perhaps create it here
	if (m_pstCCM->pReceiveThread == 0)
		{
		TRACE( _T( "m_pstCCM->pReceiveThread == 0\n" ) );
		return;
		}

	// All futher commands to do something to connect to a server are handled by
	// the Receiver Thread
	if (m_pstCCM->pSocket == nullptr)
		{
		CreateSocket( eReceiver, (LPARAM) m_pstCCM );
		if (m_pstCCM->pSocket == nullptr)
			ASSERT( 0 );
		Sleep(50);
		ConnectSocket( eReceiver, (LPARAM) m_pstCCM ); // next time
		return;	//one step at a time
		}

	// if here we have a socket and a receiver thread, but no connection. Why?
	nLastConnectError = m_pMyCCM->GetConnectErrorCode();
	if (nLastConnectError == 0)	// we are connected
		{
		m_pstCCM->bConnected = 1;
		return;
		}

	switch (nLastConnectError)
		{
		case	WSAEWOULDBLOCK:
			MyMessageBox(_T("Socket would block. Line 736\n"));
			ConnectSocket( eReceiver, (LPARAM) m_pstCCM );
			return;
		case WSAEADDRINUSE: 
			TRACE(_T("The specified address is already in use.\n"));
			break;
		case WSAEADDRNOTAVAIL: 
			TRACE(_T("The specified address is not available from the local machine.\n"));
			break;
		case WSAEAFNOSUPPORT: 
			TRACE(_T("Addresses in the specified family cannot be used with this socket.\n"));
			break;
		case WSAECONNREFUSED: 
			//MyMessageBox(_T("The attempt to connect was forcefully rejected.\n"));	//10061
			// we will try again later and hope the server is available.
			ConnectSocket( eReceiver, (LPARAM) m_pstCCM );
			return;
		case WSAEDESTADDRREQ: 
			TRACE(_T("A destination address is required.\n"));
			break;
		case WSAEFAULT: 
			TRACE(_T("The lpSockAddrLen argument is incorrect.\n"));
			break;
		case WSAEINVAL: 
			MyMessageBox(_T("The socket is already bound to an address.\n"));			// 10022L
			//KillSocket( eReceiver, (LPARAM) m_pstCCM );
			m_pstCCM->bConnected = 0;
			ConnectSocket( eReceiver, (LPARAM) m_pstCCM );
			return;
			break;
		case WSAEISCONN: 
			TRACE(_T("The socket is already connected.\n"));
			// then why are we here?
			ConnectSocket( eReceiver, (LPARAM) m_pstCCM );
			return;
			break;
		case WSAEMFILE: 
			TRACE(_T("No more file descriptors are available.\n"));
			break;
		case WSAENETUNREACH: 
			TRACE(_T("The network cannot be reached from this host at this time.\n"));
			break;
		case WSAENOBUFS: 
			TRACE(_T("No buffer space is available. The socket cannot be connected.\n"));
			break;
		case WSAENOTCONN: 
			TRACE(_T("The socket is not connected.\n"));
			m_pstCCM->bConnected = 0;
			return;
			break;
		case WSAENOTSOCK: 
			TRACE(_T("The descriptor is a file, not a socket.\n"));
			break;
		case WSAETIMEDOUT: 
			TRACE(_T("The attempt to connect timed out without establishing a connection. \n"));
			break;
		default:
			TCHAR szError[256];
			_stprintf_s(szError, _T("OnConnect error: %d"), nLastConnectError);
			s = szError;
			MyMessageBox(s);
			break;
		}


	//EnterCriticalSection(m_pstCCM->pCSRcvPkt);
//	if (m_pSocket)
	if (m_pstCCM->pSocket)
		{
		TRACE1("[%03d] Client socket already exists....destroy it and its threads\n", 
			m_nConnectionRestartCounter++);
		//TRACE1("[%03d] Client socket already exists.... close and destroy before recreating\n", m_nConnectionRestartCounter++);
		m_pstCCM->pSocket->OnClose(0);
		Sleep(20);
#if 0
		//delete m_pSocket;
		delete m_pstCCM->pSocket;
		m_pSocket = NULL;	// the local pointer
		m_pstCCM->pSocket = NULL;	// the CCM pointer
#endif
		}
#if 1
	//else
		{	// make a new 'connect' client socket

		m_pSocket = new CClientSocket( m_pMyCCM );	// subtype c0, 16 bytes long.
		// The constructor makes a copy of the pointer that is in the
		// ClientCommunicationManager class


	// Purely Randy's work
	// fix a well known Microsoft screw up that occurs
	// when using sockets in a multithreaded application 
	// that is linked with static libraries.  It seems the
	// static libraries do not properly init the hash maps
	// so we have to do it manually.  sigh........
#ifndef _AFXDLL

		AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
		AfxSocketInit();
#endif


		//LeaveCriticalSection( m_pstCCM->pCSRcvPkt );

		if (!m_pSocket)
			{
			TRACE( "Failed to create Client Socket\n" );
			LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
			return;// C_CLIENT_SOCKET_CREATION_ERROR;	// Socket object creat error
			}

		// Create the socket, then find the server address and port and attempt to connect

		// Using tcp/ip
		m_nPort = 0;
		// we will override an ASyncSocket class virtual function when (1) packet received, (2) connect to server completes, 
		// (3) the socket closes
	//	if (m_pSocket->Create(nPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE,  NULL )	!= 0 )
		if (m_pSocket->Create( m_nPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE, NULL ) != 0)
			{	// Socket created

			nSockOpt = 1;
			// need to be able to reuse the ip address
			sockerr = m_pSocket->SetSockOpt( SO_REUSEADDR, &nSockOpt, sizeof( int ), SOL_SOCKET );
			if (sockerr == SOCKET_ERROR) TRACE1( "Socket Error SO_REUSEADDR = %0x\n", sockerr );

			nSockOpt = 1;
			// when data ready to send, send without delay
			sockerr = m_pSocket->SetSockOpt( TCP_NODELAY, &nSockOpt, sizeof( int ), IPPROTO_TCP );
			if (sockerr == SOCKET_ERROR) TRACE1( "Socket Error TCP_NODELAY = %0x\n", sockerr );

			nSockOpt = 1;
			sockerr = m_pSocket->SetSockOpt( SO_DONTLINGER, &nSockOpt, sizeof( int ), SOL_SOCKET );
			if (sockerr == SOCKET_ERROR) TRACE1( "Socket Error SO_DONTLINGER = %0x\n", sockerr );
			}

		else
			{

			TRACE( "Failed to create stream socket... aborting\n" );
			LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
			return;	// C_CLIENT_SOCKET_CREATION_ERROR;
			}

		// Try server IP4 address before server name
		m_sSrv = m_pstCCM->sServerIP4;

		if (m_pstCCM->sServerIP4.IsEmpty())
			{
			m_sSrv = m_pstCCM->sServerName;
			if (m_sSrv.IsEmpty())
				{
				s = _T( "Could not find Server name or IP Address... Aborting\n" );
				DebugMsg( s );
				m_pSocket->ShutDown( 2 );
				// m_pSocket->Close();
				delete m_pSocket;
				m_pSocket = NULL;	// null the local member
				m_pMyCCM->SetSocketPtr( m_pSocket );	// null the pointer in CCM
				LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
				return;	// C_CLIENT_SOCKET_CREATION_ERROR;
				}
			}

		m_nPort = m_pstCCM->uServerPort;

		}		// make a new 'connect' client socket
	   // PAP if here
	//===sServerIP4 and uport have to be class memebers
#endif
	//LeaveCriticalSection( m_pstCCM->pCSRcvPkt );
	rtn = m_pSocket->Connect(m_pstCCM->sServerIP4, m_nPort );

		
	if ( rtn == 0 )
		{
		int nError = GetLastError();	//10035 is WSAEWOULDBLOCK..normal 
		// if it blocks, eventually we will probably get an OnConnect which will
		// set the connected flag
		// WSAEINVAL already bound to a socket		10022L
		// #define WSAEISCONN                       10056L

		m_pstCCM->nOnConnectError = nError;
		if ( (WSAEWOULDBLOCK == nError) || (WSAEINVAL == nError) || (WSAEISCONN))
			{
			// THIS IS WHAT ALWAYS HAPPENS HERE !!!, in a moment it will connect and the OnConnect
			// code in CClientSocket will complete the socket connection operation.
			//s.Format(_T("Connect Error = %d ...waiting to connect "), nError);

			s.Format(_T("Connect Error = %d ...waiting to connect to server %s.."),
				m_pstCCM->nOnConnectError, m_pstCCM->sServerName);

			t = GetTimeString();
			t += _T("\n");
			s += t;
			DebugMsg(s);
			return;
			}

		s.Format(_T("Connect Error = %d\n"), nError);
		TRACE(s);   //DebugMsg(s)
		s = m_pstCCM->szSocketName;
		s += _T(": connect failed.\n");
		TRACE(s);   //DebugMsg(s)
		m_pSocket->ShutDown(2);
		//m_pSocket->Close();		
		delete m_pSocket;
		m_pSocket = NULL;
		m_pMyCCM->SetSocketPtr(m_pSocket);
		return;	// C_CLIENT_SOCKET_CREATION_ERROR;
		}
	else
			{
			m_pMyCCM->SetSocketPtr(m_pSocket);		// store socket into stCCM for use by send and receive threads
			s.Format(_T("PAG or %s: connected using socket at 0x%08x sizeof=%d.\n"), 
			//s.Format(_T("SysCP or %s: connected using socket at 0x%08x sizeof=%d.\n"), 
				m_sSrv, m_pSocket, sizeof(CClientSocket));
			TRACE(s);   //DebugMsg(s)	// Connect to server named xxx at ip = yyyy
			// once connected, shoud start sending fake data msg every 500 ms to tell server
			// what my pap number is since it is no longer inferred by the IP address 2018-08-10 ymd
			// however, if adc present, Idata messages will be sent so no need to timer to send fake data
			}

	}

void CClientCommunicationThread::DebugMsg(CString s)
	{
#ifdef	I_AM_PAG
//	if (CNcNx::m_pDlg)
//		CNcNx::m_pDlg->DebugOut(s);
#else
	TRACE(s);
#endif	
	}

// Assuming we have killed the com dlg (CTCPCommunicationDlg) somewhere else by
// calling its close procedure thru the m_pDlg variable, we will subsequently
// post a thread message to the Com Dlg Restart procedure which will reuse m_pstCCM
// to invoke the INitTcpThread precedure again. 09-Dec-08.. jeh arcane isn't it
#if 0
afx_msg void CClientCommunicationThread::RestartTcpComDlg(WPARAM w, LPARAM lParam)
	{
	InitTcpThread(m_nMyRole, (LPARAM) m_pMyCCM);
	}
#endif

// When the managing CCM wants to send a packet to a server, it queues  the packet
// into a linked list. Then it sends or posts a thread message to the Send Thread
// instructing the thread to check the linked list and send all queued messages.
// WPARAM and  LPARAM are unused at this time

#define RETRY_COUNT			6

afx_msg void CClientCommunicationThread::TransmitPackets(WPARAM w, LPARAM l)
	{
	int nRole;
	CString s,t;
	int nSent;
	int i, j;
	IDATA_PAP *pSendPkt = 0;	// ptr to the packet info in the linked list of send packets
	IDATA_FROM_HW *pIdataHw = 0;

	int nId = AfxGetThread()->m_nThreadID;
	if (nId != m_nThreadIdOld)
		{
		s.Format(_T("Transmit Packet old thread id=%d New thread id=%d\n"), m_nThreadIdOld, nId);
		m_nThreadIdOld = nId;
		nRole = m_nMyRole;
		DebugMsg(s);
		}

	s = _T("CClientCommunicationThread::TransmitPackets ");
	if (!m_pMyCCM)
		{
		s += _T("!m_pMyCCM\n");
		DebugMsg(s);
		ASSERT(0);	// about to return and leave an orphaned memory segment in SendPktList
		return;	// (LRESULT) 0;
		}
	if (!m_pstCCM)
		{
		s += _T("!m_pstCCM\n");
		DebugMsg(s);
		ASSERT(0);	// about to return and leave an orphaned memory segment in SendPktList
		return;	// (LRESULT) 0;
		}

	if (m_pstCCM->pSendPktList->IsEmpty())
		{
		i = m_pMyCCM->m_pstCCM->pSendPktList->GetCount();
		s += _T("m_pstCCM->pSendPktList->IsEmpty()");
#if 0
		t.Format(_T(", triggered by %d\n"), w);
		s += t;
		DebugMsg(s);
		if (w == 1)	// came from timer
			{
			s.Format(_T("Address of m_pMyCCM->m_pstCCM->pSendPktList = 0x%08x\n"), &m_pMyCCM->m_pstCCM->pSendPktList);
			TRACE(s);
			}
#endif
		m_nDebugEmptyList++;
		if (m_nDebugEmptyList > 5)
			s = _T("Break Here");
		return;	// (LRESULT) 0;	// nothing to send
		}
		
	m_nDebugEmptyList = 0;

	// Since we got here we know the list is not empty

	//ASCAN_DATA *pAscan;

	if ((!m_pstCCM->pSocket) || (m_pstCCM->bConnected == 0))
		{
		// kill the recently added members of the linked list

		m_pMyCCM->LockSendPktList();
		while (m_pstCCM->pSendPktList->GetCount() > 0)
			{
			pSendPkt = (IDATA_PAP *)m_pstCCM->pSendPktList->RemoveHead();
			delete pSendPkt;
			}
		m_pMyCCM->UnLockSendPktList();	// give a higher priority thread a chance to add packets
		

		s += _T("!m_pstCCM->pSocket or not connected.. killed SendPktList member\n");
		TRACE(s);
		DebugMsg(s);
		return;	// (LRESULT) 0;	// no socket to send with
		}	// no existing socket to transmit on... kill all in linked list

	m_nInXmitLoop = 1;				// now entered into TransmitPacket loop


	s += _T("Send queued messages if any\n");

	// if we get to here, there is at least one packet to send
	while (m_pstCCM->pSendPktList->GetCount() > 0)
		{	// packets available
		m_pMyCCM->LockSendPktList();
		pSendPkt = (IDATA_PAP *) m_pstCCM->pSendPktList->RemoveHead();
		pIdataHw = (IDATA_FROM_HW *)pSendPkt;
		m_pMyCCM->UnLockSendPktList();	// give a higher priority thread a chance to add packets

#ifdef I_AM_PAP
		// This is the only place where the board number in Robert's sense of the word gets used
		// It could also be thought of as the PAP number. It is assigned from a memory stick
		// and is used only by the UUI to determine which data stream it is receiving. 2018-08-31
		// On commands from Robert, the target board number is changed to 0 and the instrument boxes
		// (ADC board and pulser have their IP address offsets set to 0
		pSendPkt->bPapNumber = gbAssignedPAPNumber;
#endif

		// examine the MsgId of the extracted packet to see what type message it really is
		switch (m_pMyCCM->m_nMyConnection)
			{
		case 0:
		default:
			// NcNx data

			gwMsgSeqCnt = 
				pSendPkt->wMsgSeqCnt = m_wMsgSeqCount++;
			// debug look at Ascan data 
			if (pSendPkt->wMsgID == eAscanID)
				j = 3;


			if ((m_pstCCM->uPacketsSent & 0x7ff) == 0)		m_pElapseTimer->Start();
			if ((m_pstCCM->uPacketsSent & 0x7ff) == 0x7ff)	// originally 0xff
				{
				m_nElapseTime = m_pElapseTimer->Stop(); // elapse time in uSec for 256 packets
				float fPksPerSec = 2048000000.0f/( (float) m_nElapseTime);	// originally 256
#if 1				
				s.Format(_T("Nx data Transmit Packets/sec = %6.1f\n"), fPksPerSec);
				TRACE(s);
#endif
	
					gPksPerSec[0].fPksPerSec = fPksPerSec;
					gPksPerSec[0].nClientIndx = 0;
					gPksPerSec[0].wMsgSeqCnt = pSendPkt->wMsgSeqCnt;
					gPksPerSec[0].nElapseTime = m_nElapseTime;
					gPksPerSec[0].uPktsSent = m_pstCCM->uPacketsSent;
					gPksPerSec[0].nTrigger = 1;	// cause main dlg to display. Main dlg clears
				}
			break;


		case 1:
			// All wall data
			pIdataHw->wMsgSeqCnt = m_wMsgSeqCountAW++;
			pIdataHw->wMsgID = ADC_DATA_ID;
			// this processing cause significant dropped all - wall packets
			if ((m_pstCCM->uPacketsSent & 0x7ff) == 0)		m_pElapseTimer->Start();
			if ((m_pstCCM->uPacketsSent & 0x7ff) == 0x7ff)	// originally 0xff
				{
				m_nElapseTime = m_pElapseTimer->Stop(); // elapse time in uSec for 256 packets
				float fPksPerSec = 2048000000.0f / ((float)m_nElapseTime);	// originally 256-now 2048
#if 1
				s.Format(_T("All Wall data Transmit Packets/sec = %6.1f\n"), fPksPerSec);
				TRACE(s);
#endif
				gPksPerSec[1].fPksPerSec = fPksPerSec;
				gPksPerSec[1].nClientIndx = 0;
				gPksPerSec[1].wMsgSeqCnt = pSendPkt->wMsgSeqCnt;
				gPksPerSec[1].nElapseTime = m_nElapseTime;
				gPksPerSec[1].uPktsSent = m_pstCCM->uPacketsSent;
				gPksPerSec[1].nTrigger = 1;	// cause main dlg to display. Main dlg clears				
				}

			break;
			}

		// take up to 6 attempts to deliver the packet linked list is empty??? 2018-06-18
		for (i = 0; i < RETRY_COUNT; i++)
			{	// loop till good xmit
			if (m_pstCCM == NULL)	break;
			if (m_pstCCM->pSocket == NULL)	break;
			if (pSendPkt == NULL)	break;
			nSent = m_pstCCM->pSocket->Send(pSendPkt, (int)pSendPkt->wByteCount);
			if (nSent == pSendPkt->wByteCount)
				{
				m_pstCCM->uBytesSent += nSent;
				m_pstCCM->uPacketsSent++;
				if (m_pstCCM->uPacketsSent < 10)
					{
					if (pSendPkt->wMsgID < 4)
						s.Format(_T("[%d]CCT::PAP sent PAG %d bytes.. MsgId=%d\n"), 
							m_pstCCM->uPacketsSent, nSent, pSendPkt->wMsgID);
					else
						s.Format(_T("[%d]CCT::PAP_AW sent PAG %d bytes.. MsgId=%d\n"),
							m_pstCCM->uPacketsSent, nSent, pSendPkt->wMsgID);
					TRACE(s);
					}
				//NxNx data and all wall have already been copied to a global
				switch (pSendPkt->wMsgID)
					{
				case 0:
				default:
					m_nConsecutiveFailedXmit = 0;
					break;

				case 2:
					memcpy((void *)&gLastAscanPap, (void *)pSendPkt, sizeof(ASCAN_DATA));
					guAscanMsgCnt++;
					m_nConsecutiveFailedXmit = 0;
					break;
				case 3:
					memcpy((void *)&gLastRdBkPap, (void *)pSendPkt, pSendPkt->wByteCount);
					guRdBkMsgCnt++; // 4/24/18 didnt work
					m_nConsecutiveFailedXmit = 0;
					break;
				case 4:
					m_nConsecutiveFailedXmitAW = 0;
					break;
					}


				// capture output to PAG for Yanming
				delete pSendPkt;
				pSendPkt = 0;
				pIdataHw = 0;
				break;
				}	//if (nSent == pSendPkt->wByteCount)

			Sleep(10);
			j = m_pstCCM->pSendPktList->GetCount();
			if ((j > 5) && (m_DebugLimit < 10))
				{
				Sleep(0);
				s.Format(_T("Send List count = %5d, Bytes sent = %d\n"), j, nSent);
				TRACE(s);
				m_DebugLimit++;
				}
					
			}	// for (i = 0; i < RETRY_COUNT; i++)

		if (i == RETRY_COUNT)
			{
			m_pstCCM->uLostSentPackets++;
			if (pSendPkt->wMsgID == 4)
				{
				s.Format(_T("Failed to send AW packet  # = %d after %d attempts\n"), m_wMsgSeqCountAW - 1, i);
				m_nConsecutiveFailedXmitAW++;
				}
			else
				{
				s.Format(_T("Failed to send Nx packet # = %d after %d attempts\n"), m_wMsgSeqCount - 1, i);
				m_nConsecutiveFailedXmit++;
				}
			TRACE(s);
			if ((m_nConsecutiveFailedXmit >= 50) || (m_nConsecutiveFailedXmitAW >= 50))
				{
#if 0
					// The sender thread failed to send. Have the receiver thread kill the socket since
					// the receiver thread created the socket.
					CClientCommunicationThread *pRcvThread = m_pMyCCM->m_pstCCM->pReceiveThread;
					pRcvThread->PostThreadMessageW(WM_USER_KILL_SOCKET, 1, 0);
					m_nConsecutiveFailedXmit = 0;
					// client side (PAP) seems to work ok, but PAG the other end crashes 4/30/18
#endif
					j = 5;
				}
			}	// failed to send
		if (pSendPkt != NULL)
			{
			delete pSendPkt;
			pSendPkt = 0;
			}

		}		// packets available

	if (pSendPkt != NULL)
		{
		delete pSendPkt;
		pSendPkt = 0;
		}

	m_nInXmitLoop = 0;	// now out of loop
//	return (LRESULT) 1;	
	}


// On a timed basis, check the received packet activity to determine connectivity
// This is usually a 0.1 second tick. Thus restarts occur every 50 seconds for a stalled socket.
// response to thread message WM_USER_TIMER_TICK
afx_msg void CClientCommunicationThread::OnTimer( WPARAM w, LPARAM lParam )
	{
	WORD wTargetSystem = w;
	m_nTick++;
	CString s;

	switch (wTargetSystem)
		{
		// we are targeting the PAG client to SysCp Server connection
		case eRestartPAGtoSysCp:
			// debug feature to prove we can reconnect to SysCp
			if (NULL == m_pMyCCM)	return;
			if (NULL == m_pMyCCM->m_pstCCM)	return;

			if (m_uLastPacketsReceived != m_pMyCCM->m_pstCCM->uPacketsReceived)
				{
				m_uLastPacketsReceived = m_pMyCCM->m_pstCCM->uPacketsReceived;
				m_nConnectRetryTick[wTargetSystem] = 0;
				}
			if (m_nConnectRetryTick[wTargetSystem] >= 100)
				{
				m_nConnectRetryTick[wTargetSystem] = 0;
				if (m_nDebugCount < 5)
					StartTCPCommunication();
				else
					{
					if (m_nDebugCount == 5)
						{
						TRACE( _T( "No more SysCp forced reconnects... debug over\n" ) );
						}
					}
				m_nDebugCount++;
				}
			m_nConnectRetryTick[wTargetSystem]++;
			break;

			// we are targeting the PAM client to the PAG server connection
		case eRestartPAPtoPAG:
			// if we haven't made the connection after a second or so, retry
			// Same case statement whether PAP 2 PAG or PAP_AW to PAG_AW or whatever catches the data
			// with the real system
			if (NULL == m_pMyCCM)	
				return;
			//if (m_pMyCCM->GetConnectionState() == 0)	// not connected yet
			if (m_pMyCCM->m_pstCCM->bConnected == 0)	// not connected yet
				{
				if (m_nConnectRetryTick[wTargetSystem] >= 50)
					{
					m_nConnectRetryTick[wTargetSystem] = 0;
					StartTCPCommunication();
					}
				m_nConnectRetryTick[wTargetSystem]++;
				}
			break;

#ifdef I_AM_PAG
		case eFake_GDP_Pipe_Data:
			// call back to the main dialog to generate and send some fake pipe data to gdp
			// fake data is taken from yiqing's void CAmalogSimDlg::OnStopSequence() 
	//		if ((m_nTick & 3) == 0)
	//			pCTscanDlg->MakeFakeGDP_Data();
			break;
#endif

		default:
			break;
		}



	// In case we have packets to send and haven't been pinged to send them  w = 1
	int i;
	if (m_pstCCM->pCSSendPkt)
		{
		m_pMyCCM->LockSendPktList();
		i = m_pMyCCM->m_pstCCM->pSendPktList->GetCount();
		m_pMyCCM->UnLockSendPktList();
		if (i > 0)
			{
#if 0
			s.Format( _T( "OnTimer-Address of m_pMyCCM->m_pstCCM->pSendPktList = 0x%08x, queued = %5d\n" ),
				&m_pMyCCM->m_pstCCM->pSendPktList, i);
			TRACE( s );
#endif
		// or maybe just call TransmitPacket directly from here
			if (m_pMyCCM->m_pstCCM)
				{
				if (m_pMyCCM->m_pstCCM->pSendThread)
					m_pMyCCM->m_pstCCM->pSendThread->PostThreadMessage( WM_USER_SEND_TCPIP_PACKET, 1, i );
				}
			}
		}
	}

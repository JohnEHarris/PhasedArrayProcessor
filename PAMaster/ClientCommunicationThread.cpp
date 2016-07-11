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
					
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"
#include "resource.h"
#include "ClientSocket.h"
#include "ClientConnectionManagement.h"
#include "ClientCommunicationThread.h"
//#include "TCPCommunicationDlg.h"

//#include "ClientCommunicationThread.h"

extern CString GetTimeString(void);

#ifdef _I_AM_PAG
#include "Truscan.h"
#include "TscanDlg.h"
class CTscanDlg;
extern CTscanDlg* pCTscanDlg;
#else

#ifndef stSEND_PACKET
typedef struct
	{
	int nLength;		// number of bytes to send
//	BYTE *pMsg;			// ptr to the message bytes to send.
	BYTE Msg[1];		// ptr to the message bytes to send.
	}	stSEND_PACKET;
#endif

#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CClientCommunicationThread

IMPLEMENT_DYNCREATE(CClientCommunicationThread, CWinThread)

CClientCommunicationThread::CClientCommunicationThread()
	{
	m_nMyRole			= 0;
	m_nInXmitLoop		= 0;
	m_nThreadIdOld		= 0;
	m_uXmitLoopCount	= 0;
	m_nConnectRetryTick	= 0;
	m_pSocket			= NULL;
	m_nConnectionRestartCounter		= 0;
	m_nTick				= 0;
	m_uLastPacketsReceived = 0;
	m_pMyCCM			= NULL;
	m_nDebugCount		= 0;
	}

CClientCommunicationThread::~CClientCommunicationThread()
	{
	int i = -1;
	CString s = _T("");
	if (m_pMyCCM)
		{
		i = m_pMyCCM->m_pstCCM->pCCM->m_nMyConnection;
		}

	switch (m_nMyRole)
		{
	default:	s = _T("?? Com thread Destructor ran\n");	break;
	case 1:		
		s.Format(_T("Rcvr Com thread[%d] Destructor ran\n"), i);
		if (m_pstCCM->pReceiveThread)
			{
			//delete m_pstCCM->pReceiveThread;
			m_pstCCM->pReceiveThread = NULL;
			}
		break;
	case 2:
		s.Format(_T("Send Com thread[%d] Destructor ran\n"), i);	
		if (m_pstCCM->pSendThread)
			{
			//delete m_pstCCM->pSendThread;
			m_pstCCM->pSendThread = NULL;
			}
		break;
		}
	TRACE(s);
	}


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

int CClientCommunicationThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
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
		if (m_pSocket)
			{
			m_pSocket->Close();
			Sleep(10);
			delete m_pSocket;
			m_pSocket = NULL;
			}
		LeaveCriticalSection(m_pstCCM->pCSRcvPkt);

		break;

	case 2:		TRACE(_T("Send Com thread ExitInstance()\n"));	
		break;
		}
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CClientCommunicationThread, CWinThread)
	//{{AFX_MSG_MAP(CClientCommunicationThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP

	ON_THREAD_MESSAGE(WM_USER_INIT_TCP_THREAD,InitTcpThread)
	//ON_THREAD_MESSAGE(WM_USER_RESTART_TCP_COM_DLG,RestartTcpComDlg)
	ON_THREAD_MESSAGE(WM_USER_RESTART_ADP_CONNECTION,RestartTcpComDlg)
	ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, TransmitPackets)	
	ON_THREAD_MESSAGE(WM_USER_TIMER_TICK, OnTimer)	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientCommunicationThread message handlers

// Pass in a pointer to the ST_CLIENT_CONNECTION_MANAGEMENT for this socket/machine
// 6-27-12 pass ptr to controlling ccm class instance
// WPARAM w contains my role, either receiving (1) or sending (2)
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

	m_nMyRole = (int) w;

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
		StartTCPCommunication();
		break;

	case 2:		// sender
		TRACE(_T("Sender thread cannot create socket\n"));
		break;
	}

	return;	// 0;
	}

// Taken from CTCPCommunicationDlg to eliminate need for hidden widows dialog
void CClientCommunicationThread::StartTCPCommunication()
	{
	int nSockOpt = TRUE;
	int  sockerr=0;
	short nPort;
	UINT uPort;
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
	if (! m_pMyCCM)
		{
		TRACE("m_pMyCCM is null... will not create client socket\n");
		return;	// major trouble here, this should never happen
		}	
	
	// Create an CAsync socket
	// Our main dlg should have identified the client and server side IP's before now


	m_pMyCCM->SetConnectionState(0);	// now assume we are not connected

	EnterCriticalSection(m_pstCCM->pCSRcvPkt);
//	if (m_pSocket)
	if (m_pstCCM->pSocket)
		{
		TRACE1("[%03d] Client socket already exists.... close and destroy before recreating\n", m_nConnectionRestartCounter++);
		//m_pSocket->Close();
		m_pstCCM->pSocket->Close();
		Sleep(10);
		//delete m_pSocket;
		delete m_pstCCM->pSocket;
		m_pSocket = NULL;
		m_pstCCM->pSocket = NULL;
		}

	m_pSocket = new CClientSocket(m_pMyCCM);	// subtype c0, 16 bytes long.


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
	   // Try server IP4 address before server name
	   stmp = m_pstCCM->sServerIP4;
	   
	   if (stmp.IsEmpty())
			{
			stmp = stmp = m_pstCCM->sServerName;
			if (stmp.IsEmpty())
				{
				s = _T("Could not find Server name or IP Address... Aborting\n");
				DebugMsg(s);
				m_pSocket->ShutDown(2);
				m_pSocket->Close();		
				delete m_pSocket;
				return;	// C_CLIENT_SOCKET_CREATION_ERROR;
				}
			}

		uPort = m_pstCCM->uServerPort;


#ifdef _I_AM_PAG
		// select the servers port connection value based on the OS version
		// and attempt to CONNECT to the server
		switch( m_pstCCM->nWinVersion)
			{
		case 7:		// for Win 7  port = 0xfff0 ?? why this number
					// stmp is null. Should it have the IP4 address we are trying to reach??
			rtn = m_pSocket->Connect(stmp, MC_SYSCP_LISTENPORT_WIN7 );
			break;
		default:	// for Win XP  port = 0xc000 ?? why this number
			rtn = m_pSocket->Connect(stmp, MC_SYSCP_LISTENPORT );
			}

#else
	   // PAM if here
		rtn = m_pSocket->Connect(stmp, uPort );

#endif
		
	   if ( rtn == 0 )
			{
			int nError = GetLastError();	//10035 is WSAEWOULDBLOCK..normal 
			// if it blocks, eventually we will probably get an OnConnect which will
			// set the connected flag

			if (WSAEWOULDBLOCK == nError)
				{
				// THIS IS WHAT ALWAYS HAPPENS HERE !!!, in a moment it will connect and the OnConnect
				// code in CClientSocket will complete the socket connection operation.
				s.Format(_T("Connect Error = %d ...waiting to connect "), nError);
				stmp = GetTimeString();
				stmp += _T("\n");
				s += stmp;
				DebugMsg(s);
				return;
				}

			s.Format(_T("Connect Error = %d\n"), nError);
			TRACE(s);   //DebugMsg(s)
			s = m_pstCCM->szSocketName;
			s += _T(": connect failed.\n");
			TRACE(s);   //DebugMsg(s)
			m_pSocket->ShutDown(2);
			m_pSocket->Close();		
			delete m_pSocket;
			return;	// C_CLIENT_SOCKET_CREATION_ERROR;
			}
		else
			{
			s.Format(_T("SysCP or %s: connected.\n"), stmp);
			TRACE(s);   //DebugMsg(s)	// Connect to server named xxx at ip = yyyy
			m_pMyCCM->SetSocketPtr(m_pSocket);		// store socket into stCCM for use by send and receive threads
			}

	}

void CClientCommunicationThread::DebugMsg(CString s)
	{
	TRACE(s);
	}

// Assuming we have killed the com dlg (CTCPCommunicationDlg) somewhere else by
// calling its close procedure thru the m_pDlg variable, we will subsequently
// post a thread message to the Com Dlg Restart procedure which will reuse m_pstCCM
// to invoke the INitTcpThread precedure again. 09-Dec-08.. jeh arcane isn't it

afx_msg void CClientCommunicationThread::RestartTcpComDlg(WPARAM w, LPARAM lParam)
	{
	InitTcpThread(m_nMyRole, (LPARAM) m_pMyCCM);
	}

// When the managing CCM wants to send a packet to a server, it queues  the packet
// into a linked list. Then it sends or posts a thread message to the Send Thread
// instructing the thread to check the linked list and send all queued messages.
// WPARAM and  LPARAM are unused at this time
afx_msg void CClientCommunicationThread::TransmitPackets(WPARAM, LPARAM)
	{
	int nRole;
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	if (nId != m_nThreadIdOld)
		{
		s.Format(_T("Transmit Packet old thread id=%d New thread id=%d\n"), m_nThreadIdOld, nId);
		TRACE(s);
		m_nThreadIdOld = nId;
		nRole = m_nMyRole;
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif

		}

	s = _T("CClientCommunicationThread::TransmitPackets ");
	//if (m_nInXmitLoop)							return (LRESULT) 0;	// already in Transmit loop operation
	if (!m_pMyCCM)
		{
		s += _T("!m_pMyCCM\n");
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif
		return;	// (LRESULT) 0;
		}
	if (!m_pstCCM)
		{
		s += _T("!m_pstCCM\n");
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif
		return;	// (LRESULT) 0;
		}
	if (m_pstCCM->pSendPktList->IsEmpty())
		{
		s += _T("m_pstCCM->pSendPktList->IsEmpty()\n");
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif		
		return;	// (LRESULT) 0;	// nothing to send
		}
	if (!m_pstCCM->pSocket)
		{
		s += _T("!m_pstCCM->pSocket\n");
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif		
		return;	// (LRESULT) 0;	// no socket to send with
		}

	m_nInXmitLoop = 1;				// now entered into TransmitPacket loop

	stSEND_PACKET *pSendPkt;	// ptr to the packet info in the linked list of send packets

	s += _T("Send queued messages if any\n");
	TRACE(s);
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif

	// if we get to here, there is at least one packet to send
	while (m_pstCCM->pSendPktList->GetCount() > 0)
		{
		m_pMyCCM->LockSendPktList();
		pSendPkt = (stSEND_PACKET *) m_pstCCM->pSendPktList->RemoveHead();
		m_pMyCCM->UnLockSendPktList();	// give a higher priority thread a chance to add packets
		// do the socket send
		m_pstCCM->pSocket->Send(&pSendPkt->Msg[0], (int) pSendPkt->nLength);
		delete pSendPkt;
		m_uXmitLoopCount++;
		}

	m_nInXmitLoop = 0;	// now out of loop
//	return (LRESULT) 1;	
	}


// On a timed basis, check the received packet activity to determine connectivity
// This is usually a 0.1 second tick. Thus restarts occur every 50 sceonds for a stalled socket.
//
afx_msg void CClientCommunicationThread::OnTimer(WPARAM w, LPARAM lParam)
	{
	WORD wTargetSystem = w;
	m_nTick++;
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
			m_nConnectRetryTick = 0;
			}
		if (m_nConnectRetryTick >= 100)
			{
			m_nConnectRetryTick = 0;
			if ( m_nDebugCount < 5)
				StartTCPCommunication();
			else
				{
				if ( m_nDebugCount == 5)
					{
					TRACE(_T("No more SysCp forced reconnects... debug over\n"));
					}
				}
			m_nDebugCount++;
			}
		m_nConnectRetryTick++;
		break;

		// we are targeting the PAM client to the PAG server connection
	case eRestartPAMtoPAG:
		// if we haven't made the connection after a second or so, retry
		if (NULL == m_pMyCCM)	return;
		if (m_pMyCCM->GetConnectionState() == 0)	// not connected yet
			{
			if (m_nConnectRetryTick >= 50)
				{
				m_nConnectRetryTick = 0;
				StartTCPCommunication();
				}
			m_nConnectRetryTick++;
			}
		break;

#ifdef _I_AM_PAG
	case eFake_GDP_Pipe_Data:
		// call back to the main dialog to generate and send some fake pipe data to gdp
		// fake data is taken from yiqing's void CAmalogSimDlg::OnStopSequence() 
		if ((m_nTick & 3) == 0)
			pCTscanDlg->MakeFakeGDP_Data();
		break;
#endif

	default:
		break;
		}



	// In case we have packets to send and haven't been pinged to send them
	if (m_pMyCCM->m_pstCCM->pSendPktList->GetCount())
		m_pMyCCM->m_pstCCM->pSendThread->PostThreadMessage(WM_USER_SEND_TCPIP_PACKET,0,0L);
	}

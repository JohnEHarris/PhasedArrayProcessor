// CServerCommunicationThread.cpp : implementation file
//
/*
Author:		JEH
Date:		20-Aug-2012
Purpose:	Provide a thread to manage the client connection between the MMI and a client machine 

Revised:
*/
#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
//#include "ServerConnectionManagement.h"
//#include "ServerSocket.h"
//#include "ServerCommunicationThread.h"


// CServerCommunicationThread

IMPLEMENT_DYNCREATE(CServerCommunicationThread, CWinThread)

CServerCommunicationThread::CServerCommunicationThread()
	{
	m_pMySCM = NULL;
	m_pstSCM = NULL;
	m_ConnectionSocket.m_pThread = NULL;
	m_ConnectionSocketPAM.m_pThread = NULL;
	}

CServerCommunicationThread::~CServerCommunicationThread()
	{
	CString s;
	CServerConnectionManagement *p = m_pMySCM;
	s.Format(_T("~CServerCommunicationThread[%d][%d] has run\n"), m_nMyServer, m_nThreadIndex);
	TRACE(s);
//	delete m_pSCC;		// created by CServerSocket::OnAccept
//	m_pSCC = NULL;
	// m_pSCC is local/ short hand for m_pSCM->m_pstSCM->pClientConnection[m_nThreadIndex]
	// But it is a separate variable.
//	m_pMySCM->m_pstSCM->pClientConnection[m_nThreadIndex] = NULL;
	}


#if 0
// we don't want a run routine. MS in all their wisdom handle all the run functions for us.
// Run waits on an event to signal sending from the send packet list for this thread
void CServerCommunicationThread::Run(DWORD ConnectionInstance, LPTSTR *lP)
	{
	int nLoopCount = 0;
	int nMyConnectionInstance = ConnectionInstance;
	HANDLE hSendEvent = m_pSCC->hSendEvent;
	stSEND_PACKET *pV;		// was void
	CString s;
	int nSent;

		
	// Wait the specified number of milliseconds. If no event, check Send Queue anyway
	// To force a quick exit, send a null message immediately following setting the ExitThread variable
	while(1)		//  != WAIT_OBJECT_0 ) 
		{
		::WaitForSingleObject(hSendEvent, 400);		// wait 400 milliSeconds or less if data to send
		if (m_pSCC->bExitThread)	break;

		// whether signaled or not, check send list
		while (m_pSCC->pSendPktPacketList->GetCount())
			{
			m_pSCC->pSocket->LockSendPktList();
			pV = (stSEND_PACKET *) m_pSCC->pSendPktPacketList->RemoveHead();
			m_pSCC->pSocket->UnLockSendPktList();
			// if shutting down, throw away the packets
			// else send packets
			if (0 == m_pSCC->bStopSendRcv)
				{
				nSent = m_pSCC->pSocket->Send(&pV->Msg[0], pV->nLength,0);
				m_pSCC->uBytesSent += nSent;
				m_pSCC->uPacketsSent++;
				if (nSent != pV->nLength)
					{
					int i = abs(nSent-pV->nLength);
					s.Format(_T("Unsent bytes = %d\n"), i);
					TRACE(s);
					}
				}	// NOT shutting down

			}
		nLoopCount++;
		}	// while (1)
	s.Format(_T("CServerCommunicationThread::Run has exited after %d loops\n"),nLoopCount);
	TRACE(s);
	}
#endif

//Thread is created suspended by CServerSocket::OnAccept. Then thread members are set there
// as shown following
#if  0

	// Init some things in the thread before it runs
	if (pThread)
		{
		pThread->m_pMySCM		= m_pSCM;
		pThread->m_pstSCM		= m_pSCM->m_pstSCM;
		pThread->m_nMyServer	= m_pSCM->m_pstSCM->pSCM->m_nMyServer;
		pThread->m_pSCC			= m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex];
		pThread->m_nThreadIndex	= nClientPortIndex;
		pThread->m_hConnectionSocket = Asocket.Detach();
		Sleep(10);
		// Make the correct socket type selection in the thread resume
		// chooses between CServerSocket and CServerSocketPA_Master
		pThread->ResumeThread(); -- this causes InitInstance to run
		}
#endif

BOOL CServerCommunicationThread::InitInstance()
	{
	CString Ip4,s;
	UINT uPort;
	// TODO:  perform and per-thread initialization here
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debug checking
	AfxSocketInit();
#endif

	// Make the selection of socket class type here. 
	if (m_pMySCM->GetServerType() == ePhaseArrayMaster)
		{
		m_ConnectionSocketPAM.m_nMyThreadIndex	= m_nThreadIndex;
		m_ConnectionSocketPAM.m_pThread	= this;
		m_ConnectionSocketPAM.m_pSCM		= this->m_pMySCM;	// ST_SERVER_CONNECTION_MANAGEMENT
		m_ConnectionSocketPAM.m_nMyServer	= this->m_nMyServer = this->m_pMySCM->m_nMyServer;
		m_ConnectionSocketPAM.m_pSCC		= this->m_pSCC;		//m_pMySCM->m_pstSCM->pClientConnection[0]; // Server's client connection
		m_ConnectionSocketPAM.m_pSCC->pServerCommunicationThread = this;
		m_ConnectionSocketPAM.m_pSCC->pSocket = &m_ConnectionSocketPAM;
		// m_hConnectionSocket = m_ConnectionSocketPAM.m_pSCC->pSocket->m_hSocket; set when thread created suspended
		m_ConnectionSocketPAM.Attach(m_hConnectionSocket, FD_READ | FD_CLOSE );
		m_ConnectionSocketPAM.GetPeerName(Ip4,uPort);	// connecting clients info??
		m_ConnectionSocketPAM.SetClientIp4(Ip4);
		m_ConnectionSocketPAM.m_pSCC->sClientIP4 = Ip4;
		m_ConnectionSocketPAM.m_pSCC->uClientPort = uPort;
		m_ConnectionSocketPAM.m_pSCC->szSocketName.Format(_T("PAM Connection Skt[%d][%d]\n"),  m_nMyServer, m_nThreadIndex);
		m_ConnectionSocketPAM.m_nOwningThreadType = eServerConnection;
#ifdef _DEBUG
		s.Format(_T("Client accepted to server on socket %s : %d\n"), Ip4, uPort);
		TRACE(s);
		TRACE(m_ConnectionSocketPAM.m_pSCC->szSocketName);
#endif
		}

	else
		{
		m_nThreadIndex					= m_ConnectionSocket.m_nMyThreadIndex;	// comes from socket OnAccept
		m_ConnectionSocket.m_pThread	= this;
		m_ConnectionSocket.m_pSCM		= this->m_pMySCM;	// ST_SERVER_CONNECTION_MANAGEMENT
		m_ConnectionSocket.m_nMyServer	= this->m_nMyServer = this->m_pMySCM->m_nMyServer;
		m_ConnectionSocket.m_pSCC		= this->m_pSCC;		//m_pMySCM->m_pstSCM->pClientConnection[0]; // Server's client connection
		m_ConnectionSocket.m_pSCC->pServerCommunicationThread = this;
		m_ConnectionSocket.m_pSCC->pSocket = &m_ConnectionSocket;
		//m_hConnectionSocket = m_ConnectionSocket.m_pSCC->pSocket->m_hSocket;	set when thread created suspended
		m_ConnectionSocket.Attach(m_hConnectionSocket, FD_READ | FD_CLOSE );
		m_ConnectionSocket.GetPeerName(Ip4,uPort);	// connecting clients info??
		m_ConnectionSocket.SetClientIp4(Ip4);
		m_ConnectionSocket.m_pSCC->sClientIP4 = Ip4;
		m_ConnectionSocket.m_pSCC->uClientPort = uPort;
		m_ConnectionSocket.m_pSCC->szSocketName.Format(_T("ServerSocket Connection Skt[%d]\n"),  m_nMyServer);
		m_ConnectionSocket.m_nOwningThreadType = eServerConnection;

#ifdef _DEBUG
		s.Format(_T("Client accepted to server on socket %s : %d\n"), Ip4, uPort);
		TRACE(s);
		TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
#endif
		}
	return TRUE;
	}

int CServerCommunicationThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	CString s;
	if ((m_nMyServer >= 0) && (m_nMyServer < MAX_SERVERS) )
		{
		if ( m_pSCC)
			{
			if ( m_pSCC->pSocket)
				{
				m_pSCC->pSocket->ShutDown(2);
				m_pSCC->pSocket->Close();
#ifdef _DEBUG
				s.Format(_T("ServerSocket[%d][%d] closed\n"),m_nMyServer, m_nThreadIndex);
				TRACE(s);
#endif
				m_pSCC->pSocket->KillpClientConnectionStruct();
				}
			Sleep(20);
			m_pSCC->bConnected = 0;
			//delete m_pSCC;	// wait til destructor
			//m_pSCC = NULL;

			}
#ifdef _DEBUG
		s.Format(_T("CServerCommunicationThread::ExitInstance[%d][%d]\n"),m_nMyServer, m_nThreadIndex);
		TRACE(s);
#endif
		if (m_pstSCM)
			{
			if (m_pstSCM->pClientConnection[m_nThreadIndex])
				m_pstSCM->pClientConnection[m_nThreadIndex]->pServerCommunicationThread = NULL;
			}
		}
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CServerCommunicationThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_INIT_COMMUNICATION_THREAD,InitCommunicationThread)
//	ON_THREAD_MESSAGE(WM_USER_KILL_COMMUNICATION_THREAD,KillCommunicationThread)
	ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, TransmitPackets)

END_MESSAGE_MAP()


// CServerCommunicationThread message handlers
// w param has socket handle to connect to
// lparam has pointer to the controlling CServerConnectionManagement class instance
//
afx_msg void CServerCommunicationThread::InitCommunicationThread(WPARAM w, LPARAM lParam)
	{
	m_pMySCM = (CServerConnectionManagement *) lParam;
	if (NULL == m_pMySCM)
		{
		TRACE(_T("InitListnerThread m_pMySCM is null\n"));
		return;
		}
	m_pstSCM = m_pMySCM->m_pstSCM;

	if (NULL == m_pstSCM)
		{
		TRACE(_T("InitListnerThread m_pstCCM is null\n"));
		return;
		}

#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debugging aid
	AfxSocketInit();
#endif


	}


// A message or messages have been placed into the linked list controlled by this thread
// This function will empty the linked list by sending its contents out using the associated
// Server Socket
afx_msg void CServerCommunicationThread::TransmitPackets(WPARAM w, LPARAM lParam)
	{
	int nClientIndex = (int) w;
	int nSent;
	stSEND_PACKET *pBuf; 
	CServerSocket *pSocket = &m_ConnectionSocket;
	if (m_pMySCM->GetServerType() == ePhaseArrayMaster)
		{
		pSocket = (CServerSocket *) &m_ConnectionSocketPAM;
		}


	// if there are any packets in the linked list, extract and send using socket interface
	if ( pSocket->m_pSCC == NULL )
		{
		TRACE(_T("TransmitPackets pSocket->m_pSCC == NULL\n"));
		return;
		}
	while (pSocket->m_pSCC->pSendPktPacketList->GetCount() )
		{
		pSocket->LockSendPktList();
		pBuf = (stSEND_PACKET *) pSocket->m_pSCC->pSendPktPacketList->RemoveHead();
		pSocket->UnLockSendPktList();
		nSent = pSocket->Send( (void *) pBuf->Msg, pBuf->nLength,0);
		if (nSent != pBuf->nLength)
			{
			TRACE2("Actual Sent = %d, expected = %d\n",nSent, pBuf->nLength);
			}
		delete pBuf;		
		}
	}
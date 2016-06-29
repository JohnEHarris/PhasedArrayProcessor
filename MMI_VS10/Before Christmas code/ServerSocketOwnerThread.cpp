// CServerSocketOwnerThread.cpp : implementation file
//
/*
Author:		JEH
Date:		20-Aug-2012
Purpose:	Provide a thread to manage the client connection between an MMI Server and a client machine 

Revised:
*/
#include "stdafx.h"
// THIS_IS_SERVICE_APP is defined in the project under C++ | Preprocessor Definitions 
#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
extern int	g_SocketSlave[NUM_OF_SLAVES];
extern CServiceApp theApp;
#else
#include "Truscan.h"
#include "TscanDlg.h"
//#include "ServerConnectionManagement.h"
//#include "ServerSocket.h"
//#include "ServerSocketOwnerThread.h"
extern CTscanApp theApp;
#endif


// CServerSocketOwnerThread

IMPLEMENT_DYNCREATE(CServerSocketOwnerThread, CWinThread)

CServerSocketOwnerThread::CServerSocketOwnerThread()
	{
	m_pMySCM = NULL;
	m_pstSCM = NULL;
	m_ConnectionSocket.m_pThread = NULL;
	m_ConnectionSocketPAM.m_pThread = NULL;
	}

CServerSocketOwnerThread::~CServerSocketOwnerThread()
	{
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	CServerConnectionManagement *p = m_pMySCM;
	ST_SERVERS_CLIENT_CONNECTION *pSCC = stSCM[m_nMyServer].pClientConnection[m_nThreadIndex];
	if ( pSCC)
		{
		int n = pSCC->uPacketsSent;
		int m = pSCC->uPacketsReceived;
		s.Format(_T("Packets Sent = %5d, Packets Received = %5d\n"), n, m);
		TRACE(s);
		}
	s.Format(_T("~CServerSocketOwnerThread[%d][%d], Id=0x%04x has run\n"), m_nMyServer, m_nThreadIndex, nId);
	TRACE(s);

	}



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

// Point this thread to its place in the static stSCM[MAX_SERVERS] structure
//

BOOL CServerSocketOwnerThread::InitInstance()
	{
	CString Ip4,s;
	UINT uPort;
	// TODO:  perform and per-thread initialization here
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debug checking
	AfxSocketInit();
#endif
#if 0
	// Make the selection of socket class type here. 
	if (m_pMySCM->GetServerType() == ePhaseArrayMaster)
		{
		m_ConnectionSocketPAM.m_nMyThreadIndex	= m_nThreadIndex;
		m_ConnectionSocketPAM.m_pThread	= this;
		m_ConnectionSocketPAM.m_pSCM		= this->m_pMySCM;	// ST_SERVER_CONNECTION_MANAGEMENT
		m_ConnectionSocketPAM.m_nMyServer	= this->m_nMyServer = this->m_pMySCM->m_nMyServer;
		m_ConnectionSocketPAM.m_pstSCM		= this->m_pMySCM->m_pstSCM;
		m_ConnectionSocketPAM.m_pSCC		= this->m_pSCC;		//m_pMySCM->m_pstSCM->pClientConnection[0]; // Server's client connection
		m_ConnectionSocketPAM.m_pSCC->pServerSocketOwnerThread = this;
		m_ConnectionSocketPAM.m_pSCC->pSocket = &m_ConnectionSocketPAM;
		// m_hConnectionSocket = Asocket.Detach(); set when thread created suspended
		// attache via the handle to the temporary socket generated in the OnAccept() operation in CServerSocket
		m_ConnectionSocketPAM.Attach(m_hConnectionSocket, FD_READ | FD_CLOSE );
		m_ConnectionSocketPAM.GetPeerName(Ip4,uPort);	// connecting clients info??
		m_ConnectionSocketPAM.SetClientIp4(Ip4);
		m_ConnectionSocketPAM.m_pSCC->sClientIP4 = Ip4;
		m_ConnectionSocketPAM.m_pSCC->uClientPort = uPort;
		m_ConnectionSocketPAM.m_pSCC->szSocketName.Format(_T("PAM Connection Skt[%d][%d]\n"),  m_nMyServer, m_nThreadIndex);
		m_ConnectionSocketPAM.m_pSCC->sClientName.Format(_T("Instrument[%d] on Server[%d]\n"), m_nThreadIndex, m_nMyServer);
		m_ConnectionSocketPAM.m_nOwningThreadType = eServerConnection;
#ifdef _DEBUG
		s.Format(_T("Client accepted to server on socket %s : %d\n"), Ip4, uPort);
		TRACE(s);
		TRACE(m_ConnectionSocketPAM.m_pSCC->szSocketName);
#endif
		}

	else
#else
		{
		m_ConnectionSocket.Init();
		m_ConnectionSocket.m_pThread	= this;
		m_ConnectionSocket.m_pSCM		= this->m_pMySCM;	// ST_SERVER_CONNECTION_MANAGEMENT
		m_ConnectionSocket.m_nMyThreadIndex = this->m_nThreadIndex;
		m_ConnectionSocket.m_nMyServer	= this->m_nMyServer;
		m_ConnectionSocket.m_pstSCM		= this->m_pMySCM->m_pstSCM;
		m_ConnectionSocket.m_pSCC		= this->m_pSCC;		//m_pMySCM->m_pstSCM->pClientConnection[0]; // Server's client connection
		m_ConnectionSocket.m_pSCC->pServerSocketOwnerThread = this;
		m_ConnectionSocket.m_pSCC->pSocket = &m_ConnectionSocket;
		// m_hConnectionSocket = Asocket.Detach(); set when thread created suspended
		// attached via the handle to the temporary socket generated in the OnAccept() operation in CServerSocket
		m_ConnectionSocket.Attach(m_hConnectionSocket, FD_READ | FD_CLOSE );
		m_ConnectionSocket.GetPeerName(Ip4,uPort);	// connecting clients info??
		m_ConnectionSocket.SetClientIp4(Ip4);
		m_ConnectionSocket.m_pSCC->sClientIP4 = Ip4;
		m_ConnectionSocket.m_pSCC->uClientPort = uPort;
		m_ConnectionSocket.m_pSCC->szSocketName.Format(_T("ServerSocket Connection Skt[%d][%d]\n"),  m_nMyServer, m_nThreadIndex);
		m_ConnectionSocket.m_pSCC->sClientName.Format(_T("Instrument[%d] on Server[%d]\n"), m_nThreadIndex, m_nMyServer);
		m_ConnectionSocket.m_nOwningThreadType = eServerConnection;

#ifdef _DEBUG
		s.Format(_T("Client accepted to server on socket %s : %d\n"), Ip4, uPort);
		TRACE(s);
		TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
#endif
		// Now create a thread to service the clients input data held in RcvPktList.
		// If more threads are needed to perform work for data or commands going between the server
		// and the instrument, this is the place to create them.
		// If these threads are children of the ServerSocketOwnerThread ( ie, based on this thread) keep
		// in mind that their constructors and destructors will call the base constructors and destructors.
		//
		// 16-Nov-2012. Call back to the top level application and ask it to create the correct thread
		// based on what the function of the server is. It will return the thread ID which will be stored 
		// in the specific SCC for this client

#ifdef THIS_IS_SERVICE_APP
		CServerRcvListThreadBase *pThread =
		m_pSCC->pServerRcvListThread = theApp.CreateServerReceiverThread(m_nMyServer);
		m_pSCC->ServerRcvListThreadID = pThread->m_nThreadID;

#else
		extern CTscanDlg* pCTscanDlg;
		CServerRcvListThreadBase *pThread =
		m_pSCC->pServerRcvListThread = pCTscanDlg->CreateServerReceiverThread(m_nMyServer);
		m_pSCC->ServerRcvListThreadID = pThread->m_nThreadID;
#endif

		s.Format(_T("ServerRcvListThread[%d][%d], Id=0x%04x was created\n"),
				m_nMyServer, m_nThreadIndex, pThread->m_nThreadID);
		TRACE(s);

		// duplicate the code from CServerSocket::OnAccept() where we created the ServerSocketOwner thread
		// since this new thread is a child of the owner thread and has the same member variables.
		if (pThread)
			{
			pThread->m_pMySCM		= this->m_pMySCM;
			pThread->m_pstSCM		= this->m_pMySCM->m_pstSCM;
			pThread->m_nMyServer	= this->m_pMySCM->m_pstSCM->pSCM->m_nMyServer;
			pThread->m_pSCC			= this->m_pMySCM->m_pstSCM->pClientConnection[m_nThreadIndex];
			pThread->m_pSCC->pSocket = &m_ConnectionSocket;
			pThread->m_nThreadIndex	= m_nThreadIndex;
			pThread->m_hConnectionSocket = m_hConnectionSocket;	// hand off the socket we just accepted to the thread
			Sleep(10);
			// Make the correct socket type selection in the thread resume
			// chooses between CServerSocket and CServerSocketPA_Master
			pThread->ResumeThread();
			}
		else
			{
			// do some sort of cleanup
			ASSERT(0);
			}
		}
#endif
	return TRUE;
	}

/***********************************************************************/

int CServerSocketOwnerThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	// 1. Close the socket
	// 2. shut down and close the thread
	// 3. report the packets sent and received
	// 4. Delete the RcvrList thread
	// 5. delete the pClientConnection structure
	// 6. set the thread exit flag to true
	//
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
				int n = m_pSCC->uPacketsSent;
				int m = m_pSCC->uPacketsReceived;
				s.Format(_T("Packets Sent = %5d, Packets Received = %5d\n"), n, m);
				TRACE(s);

#endif
				m_pSCC->pSocket->KillpClientConnectionStruct();
				m_pSCC->pSocket->m_nOwningThreadType = eServerConnection;
				m_pSCC->pSocket = NULL;
				//delete m_pSCC->pSocket; corrupts heap
				}
			Sleep(20);
			m_pSCC->bConnected = (BYTE) eNotConnected;
			//delete m_pSCC;	// wait til destructor
			//m_pSCC = NULL;
			if (m_pSCC->pServerRcvListThread)
				{
				m_pSCC->pServerRcvListThread->PostThreadMessage(WM_QUIT,0,0L);
				Sleep(10);
				}

			}
#ifdef _DEBUG
		s.Format(_T("CServerSocketOwnerThread::ExitInstance[%d][%d]\n"),m_nMyServer, m_nThreadIndex);
		TRACE(s);
#endif
		if (m_pstSCM)
			{
			if (m_pstSCM->pClientConnection[m_nThreadIndex])
				{
				m_pstSCM->pClientConnection[m_nThreadIndex]->pServerSocketOwnerThread = NULL;
				delete m_pstSCM->pClientConnection[m_nThreadIndex];
				m_pstSCM->pClientConnection[m_nThreadIndex] = NULL;
				m_pstSCM->nComThreadExited[m_nThreadIndex] = 1;
				}
			}
		}
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CServerSocketOwnerThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_INIT_COMMUNICATION_THREAD,InitCommunicationThread)
	ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, TransmitPackets)

END_MESSAGE_MAP()


// CServerSocketOwnerThread message handlers
// w param has socket handle to connect to
// lparam has pointer to the controlling CServerConnectionManagement class instance
//
afx_msg void CServerSocketOwnerThread::InitCommunicationThread(WPARAM w, LPARAM lParam)
	{
	m_pMySCM = (CServerConnectionManagement *) lParam;
	if (NULL == m_pMySCM)
		{
		TRACE(_T("ServerSocketOwnerThread m_pMySCM is null\n"));
		return;
		}
	m_pstSCM = m_pMySCM->m_pstSCM;

	if (NULL == m_pstSCM)
		{
		TRACE(_T("ServerSocketOwnerThread m_pstCCM is null\n"));
		return;
		}

#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debugging aid
	AfxSocketInit();
#endif


	}


// A message or messages have been placed into the linked list controlled by this thread
// This function will empty the linked list by sending its contents out using the associated
// Server Socket. Activation of the thread requires another actor to post a thread message
// to this thread.
afx_msg void CServerSocketOwnerThread::TransmitPackets(WPARAM w, LPARAM lParam)
	{
	int nClientIndex = (int) w;
	int nSent;
	int i;
	int nError;
	stSEND_PACKET *pBuf; 
	CServerSocket *pSocket = &m_ConnectionSocket;


	// if there are any packets in the linked list, extract and send using socket interface
	if ( pSocket->m_pSCC == NULL )
		{
		TRACE(_T("TransmitPackets pSocket->m_pSCC == NULL\n"));
		return;
		}
	while (pSocket->m_pSCC->pSendPktList->GetCount() > 0 )
		{
		pSocket->LockSendPktList();
		pBuf = (stSEND_PACKET *) pSocket->m_pSCC->pSendPktList->RemoveHead();
		pSocket->UnLockSendPktList();
		// up to 5 attempts to send
		for ( i = 0; i < 5; i++)
			{
			nSent = pSocket->Send( (void *) pBuf->Msg, pBuf->nLength,0);
			if (nSent == pBuf->nLength)
				{
				pSocket->m_pSCC->uBytesSent += nSent;
				pSocket->m_pSCC->uPacketsSent++;
				break;
				}
			// if here we are having a problem sending
			TRACE2("ServerSocketOwnerThread Actual Sent = %d, expected = %d\n",nSent, pBuf->nLength);
			nError = GetLastError();
			TRACE1("Error Code = %d\n", nError);
			if (nError == WSAEWOULDBLOCK)	Sleep(10);	// 10035L
			}	// for ( i = 0; i < 5; i++)
		delete pBuf;		
		}
	}
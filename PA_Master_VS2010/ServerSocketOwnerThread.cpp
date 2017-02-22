// CServerSocketOwnerThread.cpp : implementation file
//
/*
Author:		JEH
Date:		20-Aug-2012
Purpose:	Provide a thread to manage the client connection between an MMI Server and a client machine 

Revised:
*/
#include "stdafx.h"
// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
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
	m_pMySCM = NULL;	// managing class ptr
	m_pstSCM = NULL;	// managing structure ptr
//	m_pConnectionSocket->m_pThread = NULL;	// ServerSocket was on stack, now a ptr but does not exist yet
//	m_ConnectionSocketPAM.m_pThread = NULL;
	nDebug = 0;
	m_nConfigMsgQty = 0;
	//m_pHwTimer = new CHwTimer();
	}

CServerSocketOwnerThread::~CServerSocketOwnerThread()
	{
	}

// jeh cant get clean exit when destructor callled when object deleted.
// called just before ExitInstance calls the default CWin destructor.
void CServerSocketOwnerThread::MyDestructor()
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
		pSCC->pServerSocketOwnerThread = 0;
		}
	s.Format(_T("~CServerSocketOwnerThread[%d][%d] = 0x%08x, Id=0x%04x has run\n"), m_nMyServer, m_nThreadIndex, this, nId);
	TRACE(s);
	if (m_pHwTimer)
		delete m_pHwTimer;
	m_pHwTimer = NULL;
	//m_ConnectionSocket is on stack but has elements that were created with new operator
	//m_ConnectionSocket.m_pSCC->pSocket->KillpClientConnectionStruct();
	//m_pConnectionSocket->m_pSCC->m_pConnectionSocket->KillpClientConnectionStruct();
	//m_pConnectionSocket->KillpClientConnectionStruct();
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
		pThread->ResumeThread(); //-- this causes InitInstance to run
		}
#endif

// Point this thread to its place in the static stSCM[MAX_SERVERS] structure
//

// Thread starts suspended. Set pointers and create the socket
BOOL CServerSocketOwnerThread::InitInstance()
	{
	CString Ip4,s;
	UINT uPort;
	// TODO:  perform and per-thread initialization here
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debug checking
	AfxSocketInit();
#endif

	// HEADER FILE definition CServerSocket m_ConnectionSocket;			// server's connection to the client .. on stack
	//m_ConnectionSocket.Init();	// since created on stack a member variable, Init() ran in constructor
	// 2016-08-12 change connection socket to a pointer and create here
	// where does the mysterious m_pConnectionSocket come from. I don't see a 'new' operator in this file?
	// the answer is it comes from ServerSocket class when this thread is created suspended.
	// 		pThread->m_pConnectionSocket = new CServerSocket(); about line 367 in void CServerSocket::OnAccept(int nErrorCode)


	m_pConnectionSocket->m_nMyThreadIndex = m_nThreadIndex;
	m_pConnectionSocket->m_pThread	= this;
#if 0
	m_pConnectionSocket->m_pSCM		= this->m_pMySCM;	// ST_SERVER_CONNECTION_MANAGEMENT
	m_pConnectionSocket->m_nMyServer	= this->m_nMyServer;
	m_pConnectionSocket->m_pstSCM		= this->m_pMySCM->m_pstSCM;
	m_pConnectionSocket->m_pSCC		= this->m_pSCC;		//m_pMySCM->m_pstSCM->pClientConnection[0]; // Server's client connection
#endif
	m_pConnectionSocket->m_pSCC->pServerSocketOwnerThread = this;
	// m_hConnectionSocket = Asocket.Detach(); set when thread created suspended
	//m_ConnectionSocket.Attach(m_hConnectionSocket, FD_READ | FD_CLOSE ); take default setting on next line
	m_pConnectionSocket->Attach(m_hConnectionSocket);
#if 0
	m_pConnectionSocket->m_pSCC->pSocket = m_pConnectionSocket;
	m_pConnectionSocket->GetPeerName(Ip4,uPort);	// connecting clients info??
	m_pConnectionSocket->SetClientIp4(Ip4);
	m_pConnectionSocket->m_pSCC->sClientIP4 = Ip4;
	m_pConnectionSocket->m_pSCC->uClientPort = uPort;
#endif
	Ip4 = m_pConnectionSocket->m_pSCC->sClientIP4;
	uPort = m_pConnectionSocket->m_pSCC->uClientPort;
	m_pConnectionSocket->m_pElapseTimer = new CHwTimer();
	m_pConnectionSocket->m_pSCC->szSocketName.Format(_T("ServerSocket Connection Skt[%d][%d]\n"),  m_nMyServer, m_nThreadIndex);

	#ifdef THIS_IS_SERVICE_APP
	m_pConnectionSocket->m_pSCC->sClientName.Format(_T("Instrument[%d] on PAM Server[%d]\n"), m_nThreadIndex, m_nMyServer);
#else
	m_pConnectionSocket->m_pSCC->sClientName.Format(_T("PAM Client[%d] on PAG Server[%d]\n"), m_nThreadIndex, m_nMyServer);
#endif
	m_pConnectionSocket->m_nOwningThreadType = eServerConnection;

#ifdef _DEBUG
		s.Format(_T("Client socket at %s : %d connected to server\n"), Ip4, uPort);
		TRACE(s);
		TRACE(m_pConnectionSocket->m_pSCC->szSocketName);
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

	m_pSCC->nSSRcvListThreadPriority = THREAD_PRIORITY_NORMAL;	// could/should be an ini file value

#ifdef THIS_IS_SERVICE_APP
	// PAM if here

		// one rcv list thread for each client connection
		// create suspended in theApp and set pointers to structures here
	CServerRcvListThreadBase *pThread =
		m_pSCC->pServerRcvListThread = theApp.CreateServerReceiverThread(m_nMyServer, m_pSCC->nSSRcvListThreadPriority);
		// assuming that theApp returns the thread pointer pThread, then save the ID in SCC structure.
		m_pSCC->ServerRcvListThreadID = pThread->m_nThreadID;
		// this is used to let OnClose() exit when it gets set to 1
		m_pstSCM->nComThreadExited[m_nThreadIndex] = 0;
		// ReceiveListThread knows nothing about m_pSCC or any other server structure as of 2016-09-15
		// and maybe it doesn't have to know anything about those structures
	// PAM if here
#else
	// not Service App if here -- PAG
	extern CTscanDlg* pCTscanDlg;
	CServerRcvListThreadBase *pThread =
	
		m_pSCC->pServerRcvListThread = pCTscanDlg->CreateServerReceiverThread(m_nMyServer, m_pSCC->nSSRcvListThreadPriority);
		m_pSCC->ServerRcvListThreadID = pThread->m_nThreadID;
#endif

	s.Format(_T("ServerRcvListThread[%d][%d] = 0x%08x, Id=0x%04x was created\n"),
			m_nMyServer, m_nThreadIndex, pThread, pThread->m_nThreadID);
	TRACE(s);

	// duplicate the code from CServerSocket::OnAccept() where we created the ServerSocketOwner thread
	// since this new thread is a child of the owner thread and has the same member variables.
	if (pThread)
		{
		pThread->m_pMySCM		= this->m_pMySCM;
		pThread->m_pstSCM		= this->m_pMySCM->m_pstSCM;
		pThread->m_nMyServer	= this->m_pMySCM->m_pstSCM->pSCM->m_nMyServer;
		pThread->m_pSCC			= this->m_pMySCM->m_pstSCM->pClientConnection[m_nThreadIndex];
//		pThread->m_pSCC->pSocket = &m_ConnectionSocket;	// point the socket to the ServerSocket on our stack in this thread
		pThread->m_nThreadIndex	= m_nThreadIndex;
//		pThread->m_hConnectionSocket = m_hConnectionSocket;	// hand off the socket we just accepted to the thread
		//pThread->m_ConnectionSocket = this->m_ConnectionSocket;
		Sleep(10);
		// Make the correct socket type selection in the thread resume
		// chooses between CServerSocket and CServerSocketPA_Master
		pThread->ResumeThread();
		// Set Hardware elapse timer
		m_pHwTimer = new CHwTimer();	// this belongs to ServerSocketOwnerThread, not ServerRcvListThread
		}

	else
		{
		// do some sort of cleanup
		m_pHwTimer = 0;
		ASSERT(0);
		}
//#endif
	return TRUE;
	}

/***********************************************************************/

	// created with auto delete turned off
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
	int i;
	if ((m_nMyServer >= 0) && (m_nMyServer < MAX_SERVERS) )
		{
		if (m_pSCC->pSocket)
			{
			if ((int)m_pSCC->pSocket->m_hSocket > 0)	//&& ((int)m_pSCC->pSocket->m_hSocket < 32000))
				{	// check socket handle range

				//if (m_pSCC->m_pConnectionSocket->ShutDown(2))
				if (i = m_pSCC->pSocket->ShutDown(2))
					{
					s.Format(_T("Shutdown of client socket was successful status = %d\n"), i);
					TRACE(s);
					m_pSCC->pSocket->Close();
					}
				else
					{
					s = _T("Shutdown of client socket failed\n");
					TRACE(s);
					}

				if (m_pSCC->pSocket->m_pElapseTimer)
					{
					delete m_pSCC->pSocket->m_pElapseTimer;
					m_pSCC->pSocket->m_pElapseTimer = 0;
					}

				// now destroy the CServerSocket class which was created in ServerSocket::OnAccept
				// This class has a 64k fifo included in its member variables which is why it is such a big class
				delete m_pSCC->pSocket;
				}		// check socket handle range
			m_pSCC->pSocket = 0;
			}

		if ( m_pSCC)	// this points to pClientConnection
			{
#ifdef _DEBUG
			s.Format(_T("ServerSocket[%d][%d] closed\n"),m_nMyServer, m_nThreadIndex);
			TRACE(s);
			int n = m_pSCC->uPacketsSent;
			int m = m_pSCC->uPacketsReceived;
			int k = m_pSCC->uUnsentPackets;
			s.Format(_T("Packets Sent = %5d, Packets Received = %5d, Packets Unsent = %5d\n"), n, m, k);
			TRACE(s);

#endif
			//m_pConnectionSocket->KillpClientConnectionStruct();
			//delete m_pSCC->pSocket; corrupts heap STOPPED here on 2016-08-01 jeh .. need to delete?
			m_pSCC->pSocket = NULL;	 // was deleted above under the name of m_pConnectionSocket 
			//delete m_pSCC->pSocket; corrupts heap
			// thread was created with AutoDelete turned on. So don't delete here
			//m_pSCC->pServerSocketOwnerThread = NULL;
			}
			Sleep(20);
			m_pSCC->bConnected = (BYTE) eNotConnected;
			//delete m_pSCC;	// wait til destructor
			//m_pSCC = NULL;
			//
			m_pstSCM->nComThreadExited[m_nThreadIndex] = 1;

#if 1
			if (m_pSCC->pServerRcvListThread)
				{
				m_pSCC->pServerRcvListThread->PostThreadMessage(WM_QUIT,0,0L);
				Sleep(10);
				}
			for ( i = 0; i < 100; i++)
				{
				if (NULL == m_pSCC->pServerRcvListThread)	break;
				Sleep(10);
				}
			if ( i == 100)
				{
				s.Format(_T("CServerSocketOwnerThread::ExitInstance[%d][%d] failed to kill pServerRcvListThread\n"),
					m_nMyServer, m_nThreadIndex);
				TRACE(s);
				}
			else
				{
				s.Format(_T("CServerSocketOwnerThread::ExitInstance killed ServerRcvListThread in less than %d ms\n"), i);
				TRACE(s);
				}
#endif

#ifdef _DEBUG
		s.Format(_T("CServerSocketOwnerThread::ExitInstance[%d][%d]\n"),m_nMyServer, m_nThreadIndex);
		TRACE(s);
#endif

		if (m_pstSCM)
			{
			if (m_pstSCM->pClientConnection[m_nThreadIndex])
				{
				m_pstSCM->pClientConnection[m_nThreadIndex]->pServerSocketOwnerThread = NULL;
				// Created in ServiceApp -- must be killed there on shutdown
				//delete m_pstSCM->pClientConnection[m_nThreadIndex];
				//m_pstSCM->pClientConnection[m_nThreadIndex] = NULL;
				m_pstSCM->nComThreadExited[m_nThreadIndex] = 1;
				}
			}
		}
	//delete this; -- MOVE CODE IN owner destructor to this location and make sure auto delete is on
	MyDestructor();
	
	return CWinThread::ExitInstance();
	}


BEGIN_MESSAGE_MAP(CServerSocketOwnerThread, CWinThread)

	//ON_THREAD_MESSAGE(WM_USER_INIT_COMMUNICATION_THREAD,InitCommunicationThread)
	ON_THREAD_MESSAGE(WM_USER_SERVER_SEND_PACKET, TransmitPackets)
	ON_THREAD_MESSAGE(WM_USER_KILL_OWNER_SOCKET, Exit2)

END_MESSAGE_MAP()


// CServerSocketOwnerThread message handlers
// w param has the ClientPortIndex
// lparam points to ST_SERVERS_CLIENT_CONNECTION
//
// See ServerSocket::int CServerSocket::BuildClientConnectionStructure(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClientPortIndex)
// The SocketOwner Thread will undo what the BuildClientConnection routine buit in the class ServerSocket
afx_msg void CServerSocketOwnerThread::Exit2(WPARAM w, LPARAM lParam)
	{
	int nReturn;
	CString t, s = _T("CServerSocketOwnerThread::ExitInstance()");
	ST_SERVERS_CLIENT_CONNECTION *pscc;
	void *pV;
	int i;
	// Close the socket and delete
	// Kill the ServerRcvList thread
	// delete linked lists and critical sections
	pscc = (ST_SERVERS_CLIENT_CONNECTION *)lParam;
	//Undo what BuildClientConnectionStructure created
	if (pscc != NULL) 
		{
		if (pscc->pSocket)
			{
			if (pscc->pSocket->m_pElapseTimer)
				{
				delete pscc->pSocket->m_pElapseTimer;
				pscc->pSocket->m_pElapseTimer = 0;
				}

			i = sizeof(CServerSocket);	//65620
			if (i = pscc->pSocket->ShutDown(2))
				{
				s.Format(_T("Shutdown of client socket was successful status = %d\n"), i);
				TRACE(s);
				pscc->pSocket->Close();
				}
			else
				{
				s += _T(" servers client socket failed to shut down\n");
				TRACE(s);
				}

			}

		pscc->bConnected = (BYTE) eNotConnected;
		if (pscc->cpCSRcvPkt) 
			EnterCriticalSection(pscc->cpCSRcvPkt);
		while ( pscc->cpRcvPktList->GetCount() > 0)
			{
			pV = (void *) pscc->cpRcvPktList->RemoveHead();
			delete pV;
			}
		LeaveCriticalSection(pscc->cpCSRcvPkt);
		// These now global objects controlled by ServiceApp
#if 0
		delete	pscc->cpRcvPktList;		
				pscc->cpRcvPktList	= NULL;
		delete	pscc->cpCSRcvPkt;			
				pscc->cpCSRcvPkt		= NULL;
#endif

		EnterCriticalSection(pscc->cpCSSendPkt);
		while ( pscc->cpSendPktList->GetCount() > 0)
			{
			pV = (void *) pscc->cpSendPktList->RemoveHead();
			delete pV;
			}
		LeaveCriticalSection(pscc->cpCSSendPkt);
#if 0
		delete	pscc->cpSendPktList;		
				pscc->cpSendPktList	= NULL;
		delete	pscc->cpCSSendPkt;		
				pscc->cpCSSendPkt	= NULL;
#endif
		}
	// typically if the socket doesn't exist, the handle = -1 or 0xffffffff
	// and typically on this machine the socket handle is somewhere between 1 and 1000 with 8xx being common.
	if ((int)pscc->pSocket->m_hSocket > 0)	//&& ((int)pscc->pSocket->m_hSocket < 32000))
		{
		delete pscc->pSocket;
		pscc->pSocket = NULL;
		}

	pscc->bStopSendRcv = 1;
#if 0
	By ServiceApp
	for ( j = 0; j < MAX_SEQ_COUNT; j++)
	for ( i = 0; i < gMaxChnlsPerMainBang; i++)
		{
		if (pscc->pvChannel[j][i])
			{
			delete pscc->pvChannel[j][i];
			pscc->pvChannel[j][i] = 0;
			}
		}
#endif
	// kill ServerSocket instance
	KillServerSocketClass();
	nReturn = ExitInstance();	//thread message does not allow return of anything but void
	t.Format(_T("%d\n"), nReturn);
	s += t;
	TRACE(s);
	//delete m_pSCC->pServerSocketOwnerThread;
	}

// The creation of this thread included the creation of the ServerSocket class before the thread was resumed.
// Must deconstruct the class and release all things created with new
//
void CServerSocketOwnerThread::KillServerSocketClass(void)
	{
	if ( m_pConnectionSocket ==  NULL)	return;
	if ( m_pConnectionSocket->m_pElapseTimer)
		{
		delete m_pConnectionSocket->m_pElapseTimer;
		m_pConnectionSocket->m_pElapseTimer = 0;
		}
	//delete m_pConnectionSocket;   ~ServiceApp deletes
	//m_pConnectionSocket = 0;
	}



// A message or messages have been placed into the linked list controlled by this thread
// This function will empty the linked list by sending its contents out using the associated
// Server Socket. Activation of the thread requires another actor to post a thread message
// to this thread.
afx_msg void CServerSocketOwnerThread::TransmitPackets(WPARAM w, LPARAM lParam)
	{
	int nClientIndex = (int) w;
	CString s;
	int nSent;
	int nMsgSize;
	int i = -1;
	int nError;
	PAP_INST_CHNL_NCNX *pCmd;
	//stSEND_PACKET *pBuf; 
	//CServerSocket *pSocket = &m_ConnectionSocket;
	CServerSocket *pSocket = m_pConnectionSocket;


	// if there are any packets in the linked list, extract and send using socket interface
	if ( m_pConnectionSocket->m_pSCC == NULL )
		{
		TRACE(_T("TransmitPackets m_pConnectionSocket->m_pSCC == NULL\n"));
		return;
		}
	while (i = m_pConnectionSocket->m_pSCC->cpSendPktList->GetCount() > 0 )
		{
		m_pConnectionSocket->LockSendPktList();
		pCmd = (PAP_INST_CHNL_NCNX *) m_pConnectionSocket->m_pSCC->cpSendPktList->RemoveHead();
		m_pConnectionSocket->UnLockSendPktList();
			
		int nElapse = 0;
		switch (pCmd->wMsgID)
			{
#if 0
		case SYSINIT:
			m_pHwTimer->Start();
			m_nConfigMsgQty = 0;
			break;
		case SYSINIT_COMPLETE:
			char buf[128];
			nElapse = m_pHwTimer->Stop();
			s.Format(_T("Config file download time in uSec = %d for %d cmds"), 
				nElapse, m_nConfigMsgQty );
			CstringToChar(s,buf);
			puts(buf);
			s += _T("\n");
			TRACE(s);

			break;
#endif
			// Normally would not send Nc Nx info to instrument. Used here to debug command interaction
			// with the instrument and check
			// for lost packets.
		case NC_NX_CMD_ID:
			PAP_INST_CHNL_NCNX *pNc;
			pNc = (PAP_INST_CHNL_NCNX *)pCmd;
			s.Format(_T("NC_NX_CMD_ID Msg seq cnt =%d, seq=%2d, chnl=%3d, PktListSize= %3d\n"), 
				pCmd->wMsgSeqCnt, pNc->stNcNx->bSeqNumber, pNc->stNcNx->bChnlNumber, i);
			pCmd->wMsgSeqCnt = m_pConnectionSocket->m_pSCC->wMsgSeqCnt++;
			nMsgSize = pNc->wByteCount;
			theApp.SaveDebugLog(s);
			break;

		default:
			s.Format(_T("Unrecognized CMD, ID= %d, seq=%2d, PktListSize= %3d\n"),
				pCmd->wMsgID, pCmd->wMsgSeqCnt, i);
			theApp.SaveDebugLog(s);
			goto DELETE_CMD;
			}

		// up to 50 attempts to send
		for ( i = 0; i < 50; i++)
			{
			nSent = m_pConnectionSocket->Send( (void *) pCmd, nMsgSize,0);
			if (nSent == nMsgSize)
				{
				m_pConnectionSocket->m_pSCC->uBytesSent += nSent;
				m_pConnectionSocket->m_pSCC->uPacketsSent++;
				m_nConfigMsgQty++;
				// debug info to trace output.. losing connection when attempting to download config file
				if ((m_pConnectionSocket->m_pSCC->uPacketsSent));	// &0xff) == 0)
					{
					s.Format(_T("ServerSocketOwnerThread Pkts sent to instrument = %d, Pkts lost = %d\n"),
					m_pConnectionSocket->m_pSCC->uPacketsSent, m_pConnectionSocket->m_pSCC->uUnsentPackets);
					TRACE(s);
					}

				break;	// takes us to the end of the for () loop
				}
			else
				{
				s.Format(_T("ServerSocketOwnerThread %d bytes to instrument = %d, expected to send = %d\n"),
					nSent, nMsgSize);
				}
				
			nError = GetLastError();
			if (nError == WSAEWOULDBLOCK)
				{
				Sleep(10);	// 10035L
				if (nDebug++ < 2) 
					TRACE1("Error Code = %d\n", nError);
				}
			else TRACE1("Error Code = %d\n", nError);

			// if here we are having a problem sending
			if ( i == 49)	// last time thru loop.. loose packet after this
				{
				m_pConnectionSocket->m_pSCC->uUnsentPackets++;
				s.Format(_T("ServerSocketOwnerThread Sent=%d, list cnt=%d, Pkts sent=%d, Pkts lost=%d, msgSeq=%d, Error=%d\n"),
					nSent, m_pConnectionSocket->m_pSCC->cpSendPktList->GetCount(), m_pConnectionSocket->m_pSCC->uPacketsSent,
					m_pConnectionSocket->m_pSCC->uUnsentPackets, pCmd->wMsgSeqCnt, nError);
				TRACE(s);
				}
			// 10054L is forcibly closed by remote host
			}	// for ( i = 0; i < 5; i++)
DELETE_CMD:
		delete pCmd;		
		}	// while (i = m_pConnectionSocket->m_pSCC->cpSendPktList->GetCount() > 0 )
	}
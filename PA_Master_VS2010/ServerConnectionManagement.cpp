#if 0

Author:		JEH
Date:		14-Aug-2012
Purpose:	Collect resource necessary to support TCP/IP Send and Receive operations from a server.

Revised:	Adapted from ClientConnectionManagement for the same reason - to manage the resources of
			a TCP/Ip connetion. This connection is made by a server. It is assumed that all clients 
			on a given connection are requesting the same kind of service and supplying the same data
			set to the server.
			If there are n number of different services offered (n different listening ports), 
			then there will be n different instances of CServerConnectionManagement. 
			Each instance may have a different number of clients.

			Additional info from web added on 8-17-12:
			http://www.flounder.com/kb192570.htm#CSocketThread::InitInstance


How it is intended to work:
	This class manages two type of independent threads. 
	Initially a listener thread creates a listener ASyncSocket and listens for clients. When a client connects, the listener
	socket's OnAccept() function creates a Server Communication Thread and Server Socket to support the connection to the
	client.
	The OnReceive() function in the newly created server socket captures packets and puts them into a linked list. 
	Other routines in the system waiting for data check the linked list at periodic intervals driven by timers.
	//To send packets, an interface in ... this part being revised


	The information which connects the various sockets and threads to the rest of the program is held in structures which
	at static global memory areas. Thus information is available to other parts of the program with only a minimal amount
	needing to be passed when a socket/thread is created.
	

	[CServerConnectionManagement]
	  |--[CServerSocketOwnerThread] ---- Receiving done by Socket class, sending done from this thread
	  |----|--[CServerSocket] ------------ ASyncSocket for persistent connection to client
      |	
	  |--[CListenerThread] --------------- Creates a socket to listen for clients
	  |----|--[CServerSocket] ------------ ASyncSocket listener. Its OnAccept() creates a new socket and thread to handle the client

#endif
	  
#include "stdafx.h"
#define I_AM_SCM
				
// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#else
#include "Truscan.h"
#include "TscanDlg.h"
extern THE_APP_CLASS theApp;
#endif



CServerConnectionManagement::CServerConnectionManagement(int nMyServerIndex)
	{
	CString s;
	int i;
	m_pstSCM = NULL;
	m_nMyServer = -1;

	if (nMyServerIndex < 0)
		{
		TRACE(_T("Connection number is negative\n"));
		return;
		}
		
	if (nMyServerIndex >= MAX_SERVERS)
		{
		TRACE(_T("Connection number too big\n"));
		return;
		}
	m_nMyServer = nMyServerIndex;

	m_pstSCM = &stSCM[m_nMyServer];	// my particular structure. stSCM is a static, global structure, not part of the class.
	m_pstSCM->pSCM = this;		// the entire class has a this ptr, but static functions members do not.
								// when we are in a static function, must access particular instances of other non-static
								// member function with m_pstSCM->pSCM
	m_pstSCM->nSeverShutDownFlag = 0;	// not shutting down

	// Set up debug lists which are by server and not by connection
	m_pstSCM->pCSDebugIn	= new CRITICAL_SECTION();
	m_pstSCM->pCSDebugOut	= new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(m_pstSCM->pCSDebugIn,4);
	InitializeCriticalSectionAndSpinCount(m_pstSCM->pCSDebugOut,4);
	m_pstSCM->pInDebugMessageList	= new CPtrList(64);
	m_pstSCM->pOutDebugMessageList	= new CPtrList(64);
	m_pstSCM->ListenThreadID		= 0;
	m_pstSCM->nListenThreadPriority = THREAD_PRIORITY_NORMAL;

	for (i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		m_pstSCM->pClientConnection[i]	= NULL;
		m_pstSCM->nComThreadExited[i]	= 0;		// signify that previously running thread has exited.
		}

	};

CServerConnectionManagement::~CServerConnectionManagement(void)
	{
	void *pV;
	int i,n;
	CString s;
	i = 0;

	if (m_pstSCM == 0) 
		goto EXIT;
	if (m_pstSCM->pServerListenThread)
		{
		PostThreadMessage(m_pstSCM->pServerListenThread->m_nThreadID,WM_QUIT, 0L, 0L);
		}


	/******************/
	LockDebugIn();
	n = m_pstSCM->pInDebugMessageList->GetCount();
	if ( n >= 0)
		{
		while (!m_pstSCM->pInDebugMessageList->IsEmpty())	// empty the list
			{
			pV = (void *) m_pstSCM->pInDebugMessageList->RemoveHead();
			delete pV;
			}
		delete m_pstSCM->pInDebugMessageList;				// delete the list 2016-08-10 break
		
		}
	m_pstSCM->pInDebugMessageList = 0;

	UnLockDebugIn();
	if (m_pstSCM->pCSDebugIn)
		{
		delete m_pstSCM->pCSDebugIn;	// delete the critcial section
		m_pstSCM->pCSDebugIn = 0;
		}

	/******************/
	LockDebugOut();
	n = m_pstSCM->pOutDebugMessageList->GetCount();
	if ( n >= 0)
		{
		while (!m_pstSCM->pOutDebugMessageList->IsEmpty())
			{
			pV = (void *) m_pstSCM->pOutDebugMessageList->RemoveHead();
			delete pV;
			}
		delete m_pstSCM->pOutDebugMessageList;
		}
	m_pstSCM->pOutDebugMessageList = 0;

	UnLockDebugOut();
	if (m_pstSCM->pCSDebugOut)
		{
		delete m_pstSCM->pCSDebugOut;
		m_pstSCM->pCSDebugOut = 0;
		}

	// Kill connections to clients
#if DONE_IN_COM_THREAD_EXIT_INST
	for ( i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		if (m_pstSCM->pClientConnection[i])
			{
			if (m_pstSCM->pClientConnection[i]->pServerSocketOwnerThread)
				{
				PostThreadMessage(m_pstSCM->pClientConnection[i]->pServerSocketOwnerThread->m_nThreadID,WM_QUIT, 0L, 0L);
				}

			Sleep(200);
			}
		}
#endif

EXIT:
	s.Format(_T("~CServerConnectionManagement Destructor[%d] has run\n"), m_nMyServer);
	TRACE(s);
	}

// Start a thread for this server which will listen at the assigned address and assigned port
// if return value is 0, listener started successfully
// if return is 1, no IP address or invalid IP address
// if return is 2, no or invalid port number
// if return is 3, no valid ptr to SCM structure
int CServerConnectionManagement::StartListenerThread(int nMyServer)
	{
	char s[32];
	struct sockaddr_in local;
	CServerListenThread *pThread;	// local short hand
	if (nMyServer < 0)							return 3;
	if (nMyServer >= MAX_SERVERS)				return 3;

	if (NULL == m_pstSCM)						return 3;
	if ( m_pstSCM->sServerIP4.GetLength() < 7)	return 1;	// must at least be 1.1.1.1
	if ( m_pstSCM->uServerPort== 0)				return 2;
	CstringToChar( m_pstSCM->sServerIP4,s);		// ascii ip dotted address
	local.sin_addr.s_addr = inet_addr(s);	// "192.168.10.10");	// MMI_AS_SERVER_IP_ADDR
	local.sin_family = AF_INET;
	local.sin_port = htons( m_pstSCM->uServerPort );		// MMI_AS_SERVER_IP_PORT
	//local.sin_addr.s_addr = htonl( INADDR_ANY );
	pThread =
	m_pstSCM->pServerListenThread = (CServerListenThread *) AfxBeginThread(
										RUNTIME_CLASS(CServerListenThread),
										m_pstSCM->nListenThreadPriority,	//	THREAD_PRIORITY_NORMAL,
										0,	// stack size
										0,	// create flag, 0=run on start//CREATE_SUSPENDED,	// runstate
										NULL);	// security ptr
	TRACE3("ServerListenThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n", pThread, pThread->m_hThread, pThread->m_nThreadID);

	SetListenThreadID(pThread->m_nThreadID);	// necessary later for OnAccept to work

	// post a message to init the listener thread. Feed in a pointer to this instancec of SCM
	pThread->PostThreadMessage(WM_USER_INIT_LISTNER_THREAD, (WORD) 0, (LPARAM) this);
	return 0;	// success
	}

int CServerConnectionManagement::StopListenerThread(int nMyServer)
	{
	if (nMyServer < 0)				return 3;
	if (nMyServer >= MAX_SERVERS)	return 3;

	if (NULL == m_pstSCM)			return 3;
	TRACE3("Stop ServerListenThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n", m_pstSCM->pServerListenThread, 
		m_pstSCM->pServerListenThread->m_hThread, m_pstSCM->pServerListenThread->m_nThreadID);	
	// post a message to init the listener thread. Feed in a pointer to this instance of SCM
	//m_pstSCM->pServerListenThread->PostThreadMessageW(WM_USER_STOP_LISTNER_THREAD, (WORD) 0, (LPARAM) this);
	PostThreadMessage(m_pstSCM->pServerListenThread->m_nThreadID,WM_QUIT, 0L, 0L);
	return 0;	// success
	}

// Kill the listener thread and shut down this instance of the server
// 20-Sep-12 Let the threads close/destroy all object they control in their ExitInstance() routine.
int CServerConnectionManagement::ServerShutDown(int nMyServer)
	{
	int i, j;
	CString s;
	int nResult;
	CWinThread *pThread;
	void *pV;

//	StopListenerThread(nMyServer);
	if (nMyServer < 0)							return 3;
	if (nMyServer >= MAX_SERVERS)				return 3;
	if (NULL == m_pstSCM)						return 3;
	m_pstSCM->nSeverShutDownFlag = 1;
	//if (NULL == m_pstSCM->pServerListenThread)	nReturn = 3;

#if 1
	// global shutdown flag now handles this
	for ( i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		if (m_pstSCM->pClientConnection[i])
			{
			m_pstSCM->pClientConnection[i]->bStopSendRcv = 1;
			}
		}
#endif

	if (NULL == m_pstSCM->pServerListenThread)
		{
		TRACE("m_pstSCM->pServerListenThread IS NULL\n");
		goto NO_SERVERLISTENTHREAD;
		}

	// Listening thread kills listening socket and itself
	nResult = StopListenerThread(nMyServer);

	for ( i = 0; i < 10; i++)	// wait a little while for listener to go away
		{
		if (m_pstSCM->pServerListenThread == NULL)		break;
		Sleep(10);
		}
	if ( i == 10)
		{
		TRACE(_T("Listener Exit routine didn't run or didn't NULL pServerListenThread\n"));
		}

NO_SERVERLISTENTHREAD:

	// Kill the debug critical sections and lists
#if 0
	// move to destructor
	if (m_pstSCM->pCSDebugIn)
		{
		EnterCriticalSection(m_pstSCM->pCSDebugIn );
		while (	m_pstSCM->pInDebugMessageList->GetCount() > 0)
			{
			pV = (void *) m_pstSCM->pInDebugMessageList->RemoveHead();
			delete pV;
			}
		LeaveCriticalSection(m_pstSCM->pCSDebugIn );
		//delete m_pstSCM->pInDebugMessageList;
		}

			

	if (m_pstSCM->pCSDebugOut)
		{
		EnterCriticalSection(m_pstSCM->pCSDebugOut );
		while (	m_pstSCM->pOutDebugMessageList->GetCount() > 0)
			{
			pV = (void *) m_pstSCM->pOutDebugMessageList->RemoveHead();
			delete pV;
			}
		LeaveCriticalSection(m_pstSCM->pCSDebugOut );
		//delete m_pstSCM->pOutDebugMessageList;
		}
#endif
SERVERS_CLIENT_LOOP:

	// Now kill all the ServerSocketOwner threads which themselves have to close and kill their sockets
	for (i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		if (NULL == m_pstSCM->pClientConnection[i])	continue;	// go to end of loop

		pThread = (CWinThread *) m_pstSCM->pClientConnection[i]->pServerSocketOwnerThread;
		if (NULL != pThread)
			//PostThreadMessage(pThread->m_nThreadID,WM_QUIT, 0L, 0L);
			PostThreadMessage(pThread->m_nThreadID,WM_USER_KILL_OWNER_SOCKET,
								(WORD)i, (LPARAM)m_pstSCM->pClientConnection[i]);
		Sleep(10);
		for ( j = 0; j < 60; j++)
			{
			if (m_pstSCM->nComThreadExited[i])					
				break;
			Sleep(10);
			}	// for ( j = 0; j < 60; j++)

		if ( j == 60)
			{
			s.Format(_T("ServerComThread Exit routine didn't run or didn't NULL pServerSocketOwnerThread[%d]\n"), i);
			TRACE(s);
			return 2;
			}
		}	// for (i = 0; i < MAX_CLIENTS_PER_SERVER; i++)

	return 0;
	}

// Debug listener shutdown problem
void CServerConnectionManagement::DoNothing(void)
	{
	CWinThread * pThread = m_pstSCM->pServerListenThread;
	if (pThread)
		pThread->PostThreadMessage(WM_USER_DO_NOTHING, 0,  0L);
	// Post thread message to run CServerListenThread::DoNothing()
	}

/**************** PACKET SENDING ********************************/
// When a routine of the application wants to send a packet to a client, it uses this call.
// The sender must know the client connection number on this server.
// This function adds the packet to a linked list which is accessed by the client's thread.
// The transmit function of the client's thread is activated by a thread message instructing 
// the thread to empty the associated linked list by sending to the client. 17-Sep-2012
// This function runs at the priority level of its caller. The thread message posting allows
// the actual sending of the packet to occur at a different priority level.
// A typical server in the PAG would be the interface to the Phased Array Master Instrument(s)
// A typical client would be a specific Phased Array Master Instrument
//
#ifdef THIS_IS_SERVICE_APP
int CServerConnectionManagement::SendPacket(int nClientIndex, BYTE *pB, int nBytes, int nDeleteFlag)
	{
	int nReturn = 0;

#if 0
	if ( this->m_nMyServer != nClientIndex)
		{
		TRACE(_T("SCM::SendPacket - Wrong thread for this client\n"));
		if (nDeleteFlag)		delete pB;
		return -1;
		}
#endif

	CServerSocket * pSocket;	// id this client's socket
	CServerSocketOwnerThread *pThread;	// id this client's socket's controlling thread
	if (m_pstSCM == NULL)
		{	
		TRACE(_T("SCM::SendPacket - no m_pstSCM for this client\n"));		
		if (nDeleteFlag)		delete pB;
		return -2;		// we don't know which server instance we are
		}
	if (m_pstSCM->pClientConnection[nClientIndex] == NULL)
		{
		TRACE(_T("SCM::SendPacket - no pClientConnection for this client\n"));		
		if (nDeleteFlag)		delete pB;
		return -3;	// we don't know which client instance we are trying to com with
		}

	pThread = m_pstSCM->pClientConnection[nClientIndex]->pServerSocketOwnerThread;
	if (pThread == NULL)
		{
		TRACE(_T("SCM::SendPacket - no pServerSocketOwnerThread for this client\n"));		
		if (nDeleteFlag)		delete pB;
		return -4;		// we know the client number, but not the sockets controlling thread
		}
	pSocket = m_pstSCM->pClientConnection[nClientIndex]->pSocket;

	if (pSocket == NULL)
		{
		TRACE(_T("SCM::SendPacket - no socket for this client\n"));		
		if (nDeleteFlag)		delete pB;
		return -5;		// the clients socket doesn't exist
		}

	if (m_pstSCM->nSeverShutDownFlag)
		{
		TRACE(_T("Server ShutDown in progress\n"));	// don't queue any new packets	
		if (nDeleteFlag)		delete pB;
		return -6;		
		}

	// ok to add to linked list and then send if here
	// build a packet with packet length as the first int of the packet
	stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[nBytes+sizeof(int)];	// resize the buffer that will actually be used
	memcpy( (void *) &pBuf->Msg, (void *) pB, nBytes);	// move all data to the new buffer
	pBuf->nLength = nBytes;

	pSocket->LockSendPktList();
	pSocket->AddTailSendPkt((void *) pBuf);
	pSocket->UnLockSendPktList();
	if (nDeleteFlag)		delete pB;
	// post a thread message to a ServerSocketOwnerThread to empty the linked list and send all packets
	// Message activates CServerSocketOwnerThread::TransmitPackets(WPARAM w, LPARAM lParam)
	pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, (WORD) nClientIndex, 0L);
	return nBytes+sizeof(int);
	}
#endif


/**************** PACKET SENDING ********************************/

#ifdef _I_AM_PAG
// pB always points to MMI_CMD sendBuf where the data parameters are in CmdBuf
#if 0
typedef struct
	{

	WORD	Mach;		/* which machine, T: = 0x3a54 */
//	WORD	Slot;		/* Which chassis slot number, 0 = Master UDP */
	BYTE	PAM_Number;	/* which one of the Phase Array Masters to get the command. 1 client connection for each*/
	BYTE	Inst_Number_In_PAM;	/* which instrument in the PAM selected by PAM_Number */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	ChnlNum;    /* channel number */
	PACKET_STATS PStat[3];	/* future utilization */
	BYTE	CmdBuf[BUF_SIZE];
	} MMI_CMD;		/* Command from mmi to udp/instrument */

/*
         |--PAM_0-------|--Inst_0...... PAM_Number = 0, Inst_Number_In_PAM = 0
         |              |--Inst_1
         |              |--Inst_7
		 |
MMI -----|--PAM_1-------|--Inst_0
         |              |--Inst_7...... PAM_Number = 1, Inst_Number_In_PAM = 7
		 |--PAM_2....
		 |--PAM_3....


*/
#endif

extern CTscanDlg *pCTscanDlg;

int CServerConnectionManagement::SendPacketToPAM(int nClientIndex, BYTE *pB, int nBytes, int nDeleteFlag)
	{
	int nReturn = 0;
	CString s;
//	int nChannelInPam = pCTscanDlg->ComputeChannelNumberInPAMClient();
	PAM_GENERIC_MSG *pCmd =( PAM_GENERIC_MSG *) pB;


//	pCmd->bPamNumber = nClientIndex;
//	pCmd->ChnlNum = nChannelInPam;

#if 0
	if ( this->m_nMyServer != nClientIndex)
		{
		TRACE(_T("SCM::SendPacket - Wrong thread for this client\n"));
		if (nDeleteFlag)		delete pB;
		return -1;
		}
#endif

	CServerSocket * pSocket;	// id this client's socket
	CServerSocketOwnerThread *pThread;	// id this client's socket's controlling thread
	if (m_pstSCM == NULL)
		{
		s = _T("SCM::SendPacket - no m_pstSCM for this client\n");
		TRACE(s);
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif
		if (nDeleteFlag)		delete pB;
		return -2;		// we don't know which server instance we are
		}
	if (m_pstSCM->pClientConnection[nClientIndex] == NULL)
		{
		s = _T("SCM::SendPacket - no pClientConnection for this client\n");
		TRACE(s);		
#ifdef	_I_AM_PAG
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
#else
		TRACE(s);
#endif
		if (nDeleteFlag)		delete pB;
		return -3;	// we don't know which client instance we are trying to com with
		}

	pThread = m_pstSCM->pClientConnection[nClientIndex]->pServerSocketOwnerThread;
	if (pThread == NULL)
		{
		s = _T("SCM::SendPacket - no pServerSocketOwnerThread for this client\n");
		TRACE(s);		
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
		if (nDeleteFlag)		delete pB;
		return -4;		// we know the client number, but not the sockets controlling thread
		}
	pSocket = m_pstSCM->pClientConnection[nClientIndex]->pSocket;

	if (pSocket == NULL)
		{
		s = _T("SCM::SendPacket - no socket for this client\n");
		TRACE(s);		
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);
		if (nDeleteFlag)		delete pB;
		return -5;		// the clients socket doesn't exist
		}

	if (m_pstSCM->nSeverShutDownFlag)
		{
		s = _T("Server ShutDown in progress\n");
		TRACE(s);	// don't queue any new packets	
		if (CNcNx::m_pDlg)
			CNcNx::m_pDlg->DebugOut(s);

		if (nDeleteFlag)		delete pB;
		return -6;		
		}

	// ok to add to linked list and then send if here
	// build a packet with packet length as the first int of the packet
//	stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[nBytes+sizeof(int)];	// resize the buffer that will actually be used
//	memcpy( (void *) &pBuf->Msg, (void *) pB, nBytes);	// move all data to the new buffer
//	pBuf->nLength = nBytes;

	pSocket->LockSendPktList();
	pSocket->AddTailSendPkt((void *) pB);
	pSocket->UnLockSendPktList();
//	if (nDeleteFlag)		delete pB;
	// post a thread message to a ServerSocketOwnerThread to empty the linked list and send all packets
	// Message activates CServerSocketOwnerThread::TransmitPackets(WPARAM w, LPARAM lParam)
	pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, (WORD) nClientIndex, 0L);
//	return nBytes+sizeof(int);
	return sizeof(PAM_INST_CHNL_INFO); // only one message now
	}
#endif



// In case we want to know the dotted IP address of a specific client on this server.
// Return a null string if the requested client number is not available
CString CServerConnectionManagement::GetConnectedClientIp4(int nClient)
	{
	CString s = "";
	if (m_pstSCM == NULL)
		{
		TRACE(_T("SCM::SendPacket - no m_pstSCM for this client\n"));
		return s;
		}
	s = m_pstSCM->pClientConnection[nClient]->sClientIP4;
	return s;
	}

// ServerSocket.cpp : implementation file
//
// The server socket is the connection to the instrument. Idata comes thru this socket. It is owned by the
// ServerSocketOwner in order to run at that thread's priority.
//
// OnAccept creates and populates the ClientConnection (ST_SERVERS_CLIENT_CONNECTION *pClientConnection[])
// OnClose destroys that structure
// On Shutdown of the ServiceApp the Listen socket is killed so that OnAccept will not run again.
//
#include "stdafx.h"
#include "string.h"

#include "PA2WinDlg.h"
#include "AfxSock.h"
#include "time.h"

// I_AM_PAP is defined in the PAP project under C++ | Preprocessor Definitions 

#ifdef I_AM_PAP

//#include "PA2Win.h"
#else
#include "PA2Win.h"
#endif


extern char *GetTimeStringPtr(void);

// CServerSocket

CServerSocket::CServerSocket(CServerConnectionManagement *pSCM, int nOwningThreadType)
	{
	CString s;
	Init();
	m_pSCM = pSCM;
	m_pstSCM = pSCM->m_pstSCM;
	m_nOwningThreadType = nOwningThreadType;
	if (nOwningThreadType == eServerConnection)
		{
		m_pFifo = new CCmdFifo( INSTRUMENT_PACKET_SIZE );		// FIFO control for receiving instrument packets
		m_pFifo->m_nOwningThreadId = AfxGetThread()->m_nThreadID;
		strcpy( m_pFifo->tag, "New m_pFifoSrvSkt 48\n" );
		s = m_pFifo->tag;
		TRACE( s );

		m_pElapseTimer = new CHwTimer();
		strcpy( m_pElapseTimer->tag, "CServerSocket55 " );

		m_nSeqIndx = m_nLastSeqCnt = 0;
		memset( &m_nSeqCntDbg[0], 0, sizeof( m_nSeqCntDbg ) );
		m_nListCount = m_nListCountChanged = 0;
		m_nAsyncSocketCnt = gnAsyncSocketCnt++;
		m_nOwningThreadId = AfxGetThread()->m_nThreadID;
		s.Format( _T( "Server Fifo cnt=%d, Async cnt=%d, ThreadID=%d\n" ),
			m_pFifo->m_nFifoCnt, m_nAsyncSocketCnt, m_nOwningThreadId );
		}
	else
		{
		s = _T( "Listener or OnStack temporary socket\n" );
		}
	TRACE(s);
	}

// ServerRcvListThread calls this constructor so don't make a new fifo or timer
CServerSocket::CServerSocket()
	{
	CString s;
	Init();
	//m_pFifo = new CCmdFifo(INSTRUMENT_PACKET_SIZE);		// FIFO control for receiving instrument packets
	//m_pFifo->m_nOwningThreadId = AfxGetThread()->m_nThreadID;

	//m_nSeqIndx = m_nLastSeqCnt = 0;
	//memset(&m_nSeqCntDbg[0], 0, sizeof(m_nSeqCntDbg));
	m_nListCount = m_nListCountChanged = 0;
	m_nAsyncSocketCnt = gnAsyncSocketCnt++;
	m_nOwningThreadId = AfxGetThread()->m_nThreadID;
	s.Format(_T("CServerSocket() Called by ThreadID=%d\n"), m_nOwningThreadId);
	TRACE(s);

	}

void CServerSocket::Init(void)
	{
	m_pSCM = NULL;
	m_pSCC = NULL;
	m_pstSCM = NULL;
	m_nOwningThreadType = -1;
	szName			= _T("SCM-Skt ");
	int nId = AfxGetThread()->m_nThreadID;
	CString s;
	s.Format(_T("CServerSocket::Init() invoked by thread ID = %d\n"), nId);
	m_nOnAcceptClientIndex = -1;
	TRACE(s);
	m_dbg_cnt		= 0;
	m_pElapseTimer	= 0;
	m_pFifo			= 0;
	}

ST_SERVERS_CLIENT_CONNECTION * CServerSocket::GetpSCC( void )
	{
	if (NULL == m_pSCM)		return 0;
	if (NULL == m_pSCM->m_pstSCM)		return 0;
	if (m_nClientIndex < 0)	return 0;
	if (m_nClientIndex > MAX_CLIENTS_PER_SERVER)	return 0;

	return m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];
	}

CServerSocket::~CServerSocket()
	{
	CString s,t,u;
	void *pv = 0;
	int i, j;
	int nDummy;
	i = j = 0;
	int nId = AfxGetThread()->m_nThreadID;
	// for listener socket, m_pSCC is null
	t.Format(_T("Thread Id=%d - m_pSCC= %x "), nId, m_pSCC);

	if (m_pSCM == nullptr)
		ASSERT( 0 );
	if (m_pSCM->m_pstSCM == nullptr)
		ASSERT( 0 );

	switch (m_nOwningThreadType)
		{
		
	case eListener:
		s = _T( "Listener Socket Destructor called\n" );	// called when Asocket on stack disappears in OnAccept
		t += s;
		TRACE( t );
		pMainDlg->SaveDebugLog(t);
		s.Format( _T( " Socket# =%d, CreateThread = %d\n" ),
			m_nAsyncSocketCnt, nId );
		TRACE( s );

		if (m_pSCM->m_pstSCM->pServerListenSocket == 0)
			{
			TRACE( _T( "m_pSCM->m_pstSCM->pServerListenSocket = 0\n" ) );
			return;
			}
		else
			{
			i = (int)m_pSCM->m_pstSCM->pServerListenSocket->m_hSocket;
			if (i > 0)
				{
				m_pSCM->m_pstSCM->pServerListenSocket->Close(); // necessary or else KillReceiverThread does not run
				//CAsyncSocket::Close();
				m_pSCM->m_pstSCM->pServerListenSocket = 0;
				//Sleep( 10 );
				}

			if (m_pElapseTimer)
				{
				strcat( m_pElapseTimer->tag, "KIll HWTimer SrvSkt\n" );
				s = m_pElapseTimer->tag;
				TRACE( s );
				pMainDlg->SaveDebugLog(s);
				delete m_pElapseTimer;
				m_pElapseTimer = NULL;
				}

			if (m_pFifo != NULL)
				{
				u.Format( _T( "\n~CServerSocket() Fifo cnt=%d,  ThreadID=%d\n" ),
					m_pFifo->m_nFifoCnt, m_pFifo->m_nOwningThreadId );
				s += u;
				TRACE( s );
				strcat( m_pFifo->tag, "Kill fifo SrvSkt\n" );
				s = m_pFifo->tag;
				TRACE( s );
				pMainDlg->SaveDebugLog(s);
				delete m_pFifo;
				m_pFifo = 0;
				}
			}	// pServerListenSocket != 0

		m_pSCM->m_pstSCM->pServerListenSocket = 0;
		return;

	case eOnStack:
		s = _T( "Temporary socket to create ServerConnection\n" );
		t += s;
		TRACE( t );
		return;	// don't want to proceed and delete all we just built

	case eServerConnection:
		if ((m_nClientIndex < 0) || (m_nClientIndex >= gnMaxClientsPerServer))
			ASSERT( 0 );
		if (m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] == nullptr)
			ASSERT( 0 );
		
		m_pSCC = GetpSCC();
		s += _T( "\n" );
		t += s;
		s.Format( _T( " sizeof(CmdFifo) = %d Socket# =%d, CreateThread = %d\n" ),
			sizeof( CCmdFifo ), m_nAsyncSocketCnt, m_nOwningThreadId );
		t += s;
		TRACE( t );

		if (m_pElapseTimer)
			{
			strcat( m_pElapseTimer->tag, "KIll HWTimer SrvSkt\n" );
			s = m_pElapseTimer->tag;
			TRACE( s );
			pMainDlg->SaveDebugLog(s);
			delete m_pElapseTimer;
			m_pElapseTimer = NULL;
			}

		if (m_pFifo != NULL)
			{
			u.Format( _T( "\n~CServerSocket() Fifo cnt=%d,  ThreadID=%d\n" ),
				m_pFifo->m_nFifoCnt, m_pFifo->m_nOwningThreadId );
			s += u;
			TRACE( s );
			strcat( m_pFifo->tag, "Kill fifo SrvSkt\n" );
			s = m_pFifo->tag;
			TRACE( s );
			pMainDlg->SaveDebugLog(s);
			delete m_pFifo;
			m_pFifo = 0;
			}		
	
		// safety check from testing operations
		ST_SERVERS_CLIENT_CONNECTION *m_pCheckSCC;
		nDummy = 5;	// this had to be declared at the top of the procedure
		// or we get error saying dummy init skipped by default;
		m_pCheckSCC = m_pstSCM->pClientConnection[m_nClientIndex];
		if (m_pCheckSCC == m_pSCC)
			{
			nDummy = 4;

			i = (int)m_pSCC->pSocket->m_hSocket;
			if (i > 0)
				{
				i = m_pSCC->pSocket->ShutDown();
				if ((i > 0) && (bAppIsClosing == 0))
					{
					try
						{
						//if the app is closing, dont try to close the socket 3-15-18

						m_pSCC->pSocket->Close(); // necessary or else KillReceiverThread does not run
						CAsyncSocket::Close();
						}
					catch (...)
						{
						}
					}
				Sleep( 10 );
				}
			}
		else
			{
			nDummy = 4;	// debugging multiple disconnects of client before shutdown
			}

		if (m_pCheckSCC == m_pSCC)
			{
			m_pSCC->bConnected = 0;
			m_pSCC->pSocket = 0;
			}
			break;


	default:
		s = _T("Unknown Socket Destructor called \n");
		TRACE( s );
		nDummy = 3;
		return;
		}


	// only get here if socket type is eServerConnection
	if (nShutDown)
		{

		if (0 == KillLinkedList( m_pSCC->pCSRcvPkt, m_pSCC->pRcvPktList ))
			{
			s = _T( "Failed to kill Receive List\n" );
			TRACE( s );
			pMainDlg->SaveDebugLog( s );
			}
		else { m_pSCC->pCSRcvPkt = 0;  m_pSCC->pRcvPktList  = 0; }

		if (0 == KillLinkedList( m_pSCC->pCSSendPkt, m_pSCC->pSendPktList ))
			{
			s = _T( "Failed to kill Receive List\n" );
			TRACE(s );
			pMainDlg->SaveDebugLog(s);
			}
		else { m_pSCC->pCSSendPkt = 0;  m_pSCC->pSendPktList  = 0; }

#if 1
		m_pSCC->pServerSocketOwnerThread->KillServerSocketOwner( m_nClientIndex, (LPARAM)m_pSCC );
		TRACE( _T( "KillServerSocketOwner called from ServerSocketDestructor\n" ) );
		for (i = 0; i < 100; i++)
			{
			if (m_pSCC->pServerSocketOwnerThread == 0)
				{
				break;	// success
				}
			Sleep( 10 );
			}

		if (i >= 100)
			{
			s = _T( "CServerSocket::OnClose timed out w/o closing OwnerThread\n" );
			TRACE( s );
			pMainDlg->SaveDebugLog(s);
			}
#endif
		else m_pSCC->pSocket = 0;

		}	// if (nShutDown)
	if (m_pSCC->pSocket)		// sometimes crashes here ??
		m_pSCC->pSocket = 0;

	}

// CServerSocket member functions



// Do some checking to see if all data structures will support a new socket
//
int CServerSocket::OnAcceptPrep(void)
	{
	int nMyServer;
	CString s;
	DWORD hMyThreadID = GetCurrentThreadId();

	// look at all the servers and see if I am in the list
	for (nMyServer = 0; nMyServer < MAX_SERVERS; nMyServer++)
		{
		if (hMyThreadID == stSCM[nMyServer].ListenThreadID)
			{	// found myself
			m_pSCM = pSCM[nMyServer];
			m_nMyServer = nMyServer;
			//m_pstSCM = m_pSCM->m_pstSCM;	// ptr to specific entry in global structure
			break;
			}
		if (nMyServer >= MAX_SERVERS)
			{
			s = _T("Could not determine MyServer Number\n");
			TRACE(s);
			pMainDlg->SaveDebugLog(s);
			return 0;
			}
		}
	/******************************************************/
	// If in shut down, refuse to accept a client
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag)
		{
		TRACE("Server ShutDown Flag is true, aborting OnAccept\n");
		CAsyncSocket dummy;
		Accept(dummy);
		dummy.Close();	// should we close after CAsyncSocket::OnAccept ??? 2016-08-29 jeh
		CAsyncSocket::OnAccept(100);
		return 0;
		}

	/*********************************************/
	if ((m_nMyServer >= MAX_SERVERS) || (NULL == m_pSCM))
		{
		ASSERT(0);
		TRACE("On Accept can't identify the controlling SCM\n");
		// need to throw away the socket??
		CAsyncSocket dummy;
		Accept(dummy);
		dummy.Close();
		CAsyncSocket::OnAccept(101);
		return 0;
		}
	/*********************************************/
	return 1;
	}

// When OnAccept runs, it will pass the connected socket onto a permanent socket which will be 
// adminsitered by a thread of its own. OnAccept will create the new socket and thread.
// Since we can have serveral servers running (ST_SERVER_CONNECTION_MANAGEMENT stSCM[MAX_SERVERS];) we need
// to know which stSCM[] is managing this thread and socket. We can find this out by examining the thread Id
// which is running this code and finding it in the static array stSCM[]
//
// This procedure runs as a result of CServerListenThread::InitListnerThread() creating the listener thread which
// then calls the listen function on a listener socket.
// Once the client connects, this end accepts the connection.
//

#ifdef I_AM_PAG
// PAG is a server only, does not connect at present to any servers 2018-08-07
// PAG and UUI must work with IP addresses assigned by DHCP
//
// PAP on the other hand depends on fixed IP address ranges to identify NIOS software based hardware
// What will happen if a PAG client quits without closing the connection and then comes back.
// May have to keep IP addresses in a table to detect when the same IP address comes back w/o closing
// and then somehow know which set of threads to destroy... otherwise run out of Client structures
//
void CServerSocket::OnAccept(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	int * bufBOOLsize;
	//int nResult;
	BOOL bufBOOL;
	bufBOOLsize = &bufBOOL;
    *bufBOOLsize = TRUE;
	//int nMyServer;
	int sockerr;
	SOCKADDR SockAddr;
	int SockAddrLen = sizeof(SOCKADDR);
	//int i;
	CString Ip4,s,t,sOut;
	UINT uIp4 = 0;
	BYTE bIsClosing = 0;
	CAsyncSocket Asocket;

	// how are we going to set our pSCM pointer???
	// get our threadID of the thread running me
	// and then see if it is in the static list to know which stSCM[MAX_SERVERS] we belong to
	// m_nOwningThreadType = eListener;

	if (OnAcceptPrep() == 0)
		{
		TRACE(_T("OnAccept of socket was rejected by OnAcceptPrep"));
		return;
		}

	Sleep( 20 );

//	CServerSocket Asocket(m_pSCM, eOnStack);	// a temporary Async socket of our fashioning ON THE STACK
//	Asocket.m_nOwningThreadType = eOnStack;
//	CAsyncSocket Asocket;
	// Asocket.m_pSCM = m_pSCM;
	// Asocket.m_pstSCM = m_pstSCM; built in constructor
	// ACCEPT the connection from our client into the temporary socket Asocket
	// ACCEPT the connection from our client into the temporary socket Asocket
	// ACCEPT the connection from our client into the temporary socket Asocket
	// We have to do this in order to get the client index number.
	sockerr = Accept(Asocket, &SockAddr, &SockAddrLen);

	if (0 == sockerr)
		{
		sockerr = GetLastError();
		TRACE2("OnAccept sockerr = %d = 0x%x\n", sockerr, sockerr);
		CAsyncSocket::OnAccept(nErrorCode);
		return;
		}
	// Successful OnAccept
	// Since our listener was created to service events FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT
	// will this accepted socket continue to support those services or do we need to set them
	// in the Asocket object on the stack????
#if 0
	From Microsoft help file-
	The socket created by the accept function has the same properties as the listening socket used to accept it. 
	Consequently, WSAAsyncSelect events set for the listening socket also apply to the accepted socket. 
	For example, if a listening socket has WSAAsyncSelect events FD_ACCEPT, FD_READ, and FD_WRITE, 
	then any socket accepted on that listening socket will also have FD_ACCEPT, FD_READ, and FD_WRITE events 
	with the same wMsg value used for messages.
#endif

	UINT uPort;
	int nClientPortIndex;			// which client are we connecting to? Derive from IP address
	UINT uClientBaseAddress;		// what is the 32 bit index of the 1st PA Master?
	WORD wClientBaseAddress[8];
	char *pIpBase = gServerArray[m_nMyServer].ClientBaseIp;

#ifdef CLIENT_AND_SERVER_ON_SAME_MACHINE
	uClientBaseAddress = inet_addr (pIpBase);	// Instrument base "192.168.10.201", PAG is "192.168.10.10"
#else
//	uClientBaseAddress = ntohl(inet_addr (pIpBase));	// Instrument base "192.168.10.201", PAG is "192.168.10.10"
	// update to newer function 
	s = pIpBase;
	if (1 != InetPton(AF_INET, s, &wClientBaseAddress) )
		{
		TRACE(_T("InetPton error\n"));
		return;
		}
	else
		{
		TRACE(_T("InetPton success in OnAccept\n"));
		uClientBaseAddress = ntohl(*(u_long*)&wClientBaseAddress);	// same value as uClientBaseAddress
		}


#endif


	Asocket.GetSockName(Ip4,uPort);	// my socket info??
	s.Format(_T("Server side socket %s : %d\n"), Ip4, uPort);	// crash here 7-14-16 on reconnect by instrument
	TRACE(s);
	Asocket.GetPeerName(Ip4,uPort);	// connecting clients info??
	s.Format(_T("Client side socket %s : %d\n"), Ip4, uPort);
	TRACE(s);
	int ntmp = 0;
	s = Ip4;
	if (1 != InetPton(AF_INET, s, &wClientBaseAddress) )
		{	TRACE(_T("InetPton error\n"));		return;		}
	else
		{	TRACE(_T("InetPton success in OnAccept ... CLIENT CONNECTED ******************\n"));
		ntmp = uClientBaseAddress = ntohl(*(u_long*)&wClientBaseAddress);
		uIp4 = uClientBaseAddress;
		}


	nClientPortIndex = (ntmp - uClientBaseAddress);	// 0-n.. in general, not true for PAG


	// May be redundant to set these in Asocket since same as the listening socket
	sockerr = Asocket.SetSockOpt(SO_REUSEADDR, &bufBOOL, sizeof(int), SOL_SOCKET);
	ASSERT(sockerr != SOCKET_ERROR);

	sockerr = Asocket.SetSockOpt(SO_ACCEPTCONN, &bufBOOL, sizeof(int), SOL_SOCKET);
	ASSERT(sockerr != SOCKET_ERROR);

	sockerr = Asocket.SetSockOpt(SO_DONTROUTE, &bufBOOL, sizeof(int), SOL_SOCKET);
	ASSERT(sockerr != SOCKET_ERROR);

	sockerr = Asocket.SetSockOpt(TCP_NODELAY, &bufBOOL, sizeof(int), IPPROTO_TCP);
	ASSERT(sockerr != SOCKET_ERROR);

		int nSize;
		int nSizeOf = sizeof(int);
		GetSockOpt(SO_SNDBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("ServerSocket NIC Transmit Buffer Size = %d\n"), nSize);
		TRACE(s);

		GetSockOpt(SO_RCVBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("ServerSocket NIC Receiver Buffer Size = %d\n"), nSize);
		TRACE(s);

	// create a connection thread to own/contain this socket... if one already exists, kill it first
	// assume there is only one connection from the client to the server. If there is more than
	// one connection we can determine from the clients IP address captured above in the GetPeerName
	// which client we are connecting to and adjust the pClientConnection[] subscript accordingly
	// For now assume only one Phased Array Master processor is connected to one PA GUI server.
	if (m_pSCM->m_pstSCM == NULL)
		{
		TRACE("Fatal error, pMySCM->m_pstSCM == NULL\n");
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		return;
		}

	// CREATE THE STRUCTURE to hold the ST_SERVERS_CLIENT_CONNECTION info
	// for PAG if we don't care about pap Client Index, just start from 0 and loop till
	// first null ptr found
	
	ST_SERVERS_CLIENT_CONNECTION *pscc; // ****************************************

/////#ifdef I_AM_PAG
	// top level machine like UUI. Client gets address via DHCP
	// Find the 1st available pClientConnection
	// Hope there aren't "dead" ClientConnections
	for (m_nClientIndex = 0; m_nClientIndex < MAX_CLIENTS_PER_SERVER; m_nClientIndex++)
		{
		if (m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] == NULL)	// first time thru
			{
			pscc = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] = new ST_SERVERS_CLIENT_CONNECTION();
			OnAcceptInitializeConnectionStats(pscc, m_nMyServer, m_nClientIndex);
			pscc->sClientIP4 = Ip4;
			pscc->uClientIP4 = uIp4;
			pscc->m_nClientIndex = m_nClientIndex;
			s.Format(_T("PAGSrv[%d]:PAP[%d] OnAccept Client\n"), m_nMyServer, m_nClientIndex);
			t = s;
			TRACE(t);

			//		theApp.SaveDebugLog(t);
			pMainDlg->SaveDebugLog(t);
			pscc->szSocketName = s;
			pscc->uClientPort = uPort;
			m_pstSCM->nComThreadExited[m_nClientIndex] = 0;
			SetpSCC(pscc);

			break;
			}
		else  // this might be the same IP address as was previously connecdted
			{	
			// In that case, detach the existing socket and connect to a new socket
			// Could NOT just set pThread->socket = detatched value before because that thread was suspended
			// Now the Owner thread is not suspended.
			SOCKET hConnectionSocket;
			pscc = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];	// we know its not null
			if (pscc->uClientIP4 == uIp4)
				{
				CServerSocketOwnerThread * pThread = pscc->pServerSocketOwnerThread;
				hConnectionSocket = Asocket.Detach();
				pThread->PostThreadMessageW(WM_USER_ATTACH_SERVER_SOCKET, 0, (LPARAM)hConnectionSocket);
				Sleep(50);
				CAsyncSocket::OnAccept(nErrorCode);
				return;
				}
			}
		}	// for (m_nClientIndex = 0; m_nClientIndex < MAX_CLIENTS_PER_SERVER; m_nClientIndex++)

	if (m_nClientIndex >= MAX_CLIENTS_PER_SERVER)
		{
		// Maybe we have dead entried in pClientconnection
		// see if same IP address associated with more than one slot. If so kill the oldest one
		// and delete all its created elements... means we have to add a time stamp to the 
		// client connection structure. Then use that slot for this connection attempt
		s.Format(_T("PAGSrv[%d] OnAccept failed, nClientIndex too big %d\n"), m_nMyServer, m_nClientIndex);
		TRACE(s);
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		return;
		}


		
///////////////////////////////////////
		s = _T("AfxBeginThread(RUNTIME_CLASS (CServerSocketOwnerThread) is next\n");
		TRACE(s);
		// ServerSocketOwnerThread will attach to the accepted socket at the priority level of the ServerSocketOwnerThread
		// This allows (we think) the socket to run at a high priority level
		// create a new thread IN SUSPENDED STATE. LEAVE AUTODELETE ON !!!
		CServerSocketOwnerThread * pThread =
		m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread = (CServerSocketOwnerThread *) AfxBeginThread(RUNTIME_CLASS (CServerSocketOwnerThread),
	   	   					  													THREAD_PRIORITY_ABOVE_NORMAL,
																				0,					// stacksize
																				CREATE_SUSPENDED,	// runstate
																				NULL);				// security
		//pThread->m_bAutoDelete = 0;

		s.Format(_T("CServerSocketOwnerThread[%d][%d]= 0x%08x, Id=0x%04x was created\n"),
						m_nMyServer, m_nClientIndex, pThread, pThread->m_nThreadID);
		TRACE(s);
		// Init some things in the thread before it runs. We are now accessing things inside the new thread, not in this thread
		if (pThread)
			{
			// This is how we boost the ServerSocket to a higher priority
			//pThread->m_pConnectionSocket	= new CServerSocket(m_pSCM, eServerConnection);
			pThread->m_pSCM		= m_pSCM;
			pThread->m_pstSCM	= m_pSCM->m_pstSCM;
			pThread->m_nMyServer= m_pSCM->m_pstSCM->pSCM->m_nMyServer;
			pThread->m_pSCC		= m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];
			pThread->m_nClientIndex	= m_nClientIndex;
			pThread->m_pSCC->pSocket = new CServerSocket(m_pSCM, eServerConnection); ;
			s.Format( _T( "CServerSocket::OnAccept, pThread->m_pSCC->pSocket = 0x%08x\n" ), pThread->m_pSCC->pSocket );
			TRACE(s);
	

			//pThread->m_pSCC->pSocket = NULL;
			pThread->m_hConnectionSocket = Asocket.Detach();	// hand off the socket we just accepted to the thread
			Sleep(10);
			// Resume will cause CServerSocketOwnerThread::InitInstance() to execute
			// pThread->m_pSCC->pSocket = (CServerSocket *) FromHandle(Asocket.Detach()); doesn't work
			pThread->m_pSCC->pSocket->m_nOwningThreadType = eServerConnection;
			pThread->m_pSCC->pSocket->m_pSCC = pThread->m_pSCC;
			pThread->m_pSCC->bConnected = 1;
			pThread->m_pSCC->pSocket->m_pSCM = pThread->m_pSCM;
			pThread->m_pSCC->pSocket->m_pstSCM = pThread->m_pstSCM;
			pThread->m_pSCC->pSocket->m_nClientIndex = m_nClientIndex;
			pThread->ResumeThread();
			}
		else
			{
			// do some sort of cleanup
			ASSERT(0);
			s = _T("Cleanup");
			}
		
		// Display the connect socket IP and port
		Asocket.GetSockName(Ip4,uPort);	// my socket info??
		pThread->m_pSCC->bConnected = eConfigured;
		
		char buffer [80], txt[64];
		strcpy(buffer,GetTimeStringPtr());
		CstringToChar(Ip4, txt);
		printf("Instrument Client[%d]  on socket %s : %d accepted to server at %s\n", 
			m_nClientIndex, txt, uPort, buffer);
		sOut = buffer;
		s.Format(_T("\nInstrument Client[%d]  on socket %s : %d accepted by server at %s\n"), 
			m_nClientIndex,Ip4, uPort, sOut);
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		Sleep(10);
			
		// Asocket.Close();	not necessary. Since Asocket on stack, when this routine ends, Asocket deletes
		// destructor closes the socket
		
		
		// if (m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] == NULL)
	/************************** What if already connected ?? *******************/

#if 0
	if	(m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] && 
		(m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread))
		{
		//2017-05-24 try detaching the existing socket and reattaching the new socket
		SOCKET hSocket;
		pscc = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];	// shortcut for coding
		pscc->m_nClientIndex = m_nClientIndex;
		if (pscc->pSocket == 0)
			{
			m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pSocket = new CServerSocket(m_pSCM, eServerConnection);
			if (pscc->pSocket)
				{
				pscc->pSocket->m_nClientIndex = m_nClientIndex;
				pscc->pSocket->m_pSCM	= m_pSCM;
				pscc->pSocket->m_pstSCM = m_pSCM->m_pstSCM;
				pscc->pSocket->m_pSCC	= m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];
				//hSocket = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pSocket->Detach();
				}			
			}
		hSocket = pscc->pSocket->Detach();
		//hSocket = Asocket.Detach();
		m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread->m_hConnectionSocket = hSocket;
		m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread->m_pSCC->pSocket = pscc->pSocket;
		m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread->m_pSCC->pSocket->Attach( hSocket );
		m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->bConnected = 1;
		Sleep(10);
		//Sleep(10);
		}

	else
		{
		ASSERT(0);	// got a break here from real instrument 2016-09-08
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		s.Format(_T("Refused connection: no ClientConnection or no ServerSocketOwnerThread Error = %d\n"),
			nErrorCode);
		return;
		}
#endif

	CAsyncSocket::OnAccept(nErrorCode);
	//pThread->Run();
	}
#endif
// can't be both
#ifdef I_AM_PAP
	void CServerSocket::OnAccept(int nErrorCode)
		{
		// TODO: Add your specialized code here and/or call the base class
		int * bufBOOLsize;
		//int nResult;
		BOOL bufBOOL;
		bufBOOLsize = &bufBOOL;
		*bufBOOLsize = TRUE;
		int nMyServer;
		int sockerr;
		SOCKADDR SockAddr;
		int SockAddrLen = sizeof(SOCKADDR);
		//int i;
		CString Ip4, s, t, sOut;
		BYTE bIsClosing = 0;


		// how are we going to set our pSCM pointer???
		// get our threadID of the thread running me
		// and then see if it is in the static list to know which stSCM[MAX_SERVERS] we belong to
		// m_nOwningThreadType = eListener;

		if (OnAcceptPrep() == 0)
			{
			TRACE(_T("On Accept of socket was rejected by OnAcceptPrep"));
			return;
			}

		Sleep(20);

		//	CServerSocket Asocket(m_pSCM, eOnStack);	// a temporary Async socket of our fashioning ON THE STACK
		//	Asocket.m_nOwningThreadType = eOnStack;
		CAsyncSocket Asocket;
		// Asocket.m_pSCM = m_pSCM;
		// Asocket.m_pstSCM = m_pstSCM; built in constructor
		// ACCEPT the connection from our client into the temporary socket Asocket
		// ACCEPT the connection from our client into the temporary socket Asocket
		// ACCEPT the connection from our client into the temporary socket Asocket
		// We have to do this in order to get the client index number.
		sockerr = Accept(Asocket, &SockAddr, &SockAddrLen);

		if (0 == sockerr)
			{
			sockerr = GetLastError();
			TRACE2("OnAccept sockerr = %d = 0x%x\n", sockerr, sockerr);
			CAsyncSocket::OnAccept(nErrorCode);
			return;
			}
		// Successful OnAccept
		// Since our listener was created to service events FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT
		// will this accepted socket continue to support those services or do we need to set them
		// in the Asocket object on the stack????
#if 0
		From Microsoft help file -
			The socket created by the accept function has the same properties as the listening socket used to accept it.
			Consequently, WSAAsyncSelect events set for the listening socket also apply to the accepted socket.
			For example, if a listening socket has WSAAsyncSelect events FD_ACCEPT, FD_READ, and FD_WRITE,
			then any socket accepted on that listening socket will also have FD_ACCEPT, FD_READ, and FD_WRITE events
			with the same wMsg value used for messages.
#endif

			UINT uPort;
		int nClientPortIndex;					// which client are we connecting to? Derive from IP address
		UINT uClientBaseAddress;		// what is the 32 bit index of the 1st PA Master?
		WORD wClientBaseAddress[8];
		char *pIpBase = gServerArray[nMyServer].ClientBaseIp;

#ifdef CLIENT_AND_SERVER_ON_SAME_MACHINE
		uClientBaseAddress = inet_addr(pIpBase);	// Instrument base "192.168.10.201", PAG is "192.168.10.10"
#else
		//	uClientBaseAddress = ntohl(inet_addr (pIpBase));	// Instrument base "192.168.10.201", PAG is "192.168.10.10"
		// update to newer function 
		s = pIpBase;
		if (1 != InetPton(AF_INET, s, &wClientBaseAddress))
			{
			TRACE(_T("InetPton error\n"));
			return;
			}
		else
			{
			TRACE(_T("InetPton success in OnAccept\n"));
			uClientBaseAddress = ntohl(*(u_long*)&wClientBaseAddress);	// same value as uClientBaseAddress
			}


#endif

		Asocket.GetSockName(Ip4, uPort);	// my socket info??
		s.Format(_T("Server side socket %s : %d\n"), Ip4, uPort);	// crash here 7-14-16 on reconnect by instrument
		TRACE(s);
		Asocket.GetPeerName(Ip4, uPort);	// connecting clients info??
		s.Format(_T("Client side socket %s : %d\n"), Ip4, uPort);
		TRACE(s);
		int ntmp;
		s = Ip4;
		if (1 != InetPton(AF_INET, s, &wClientBaseAddress))
			{
			TRACE(_T("InetPton error\n"));		return;
			}
		else
			{
			TRACE(_T("InetPton success in OnAccept ... CLIENT CONNECTED ******************\n"));
			ntmp = ntohl(*(u_long*)&wClientBaseAddress);
			}


		nClientPortIndex = (ntmp - uClientBaseAddress);	// 0-n

#ifdef I_AM_PAP
	// Assume we know a range of addresses of clients which connected to this server
	// for example 192.168.10.201 - the first instrument and 192.168.10.208 - the eighth instrument 
	// in consecutive IP address order
	// and assume they all connect to the same nic on the PAM side and thus need their own pClientConnection
	// From the PeerName IP address we can compute the index for the pClientConnection
	//

		nClientPortIndex = (ntmp - uClientBaseAddress);


#endif


		if ((nClientPortIndex < 0) || (nClientPortIndex >= MAX_CLIENTS_PER_SERVER))	// || bIsClosing)
			{
			Asocket.Close();
			CAsyncSocket::OnAccept(nErrorCode);
			s.Format(_T("Fatal error - nClientPortIndex = %d\n"), nClientPortIndex);
			TRACE(s);	// bad client port index for PAP to ADC. Must be fixed IP addresses
			return;
			}

		m_nClientIndex = nClientPortIndex;		// this should not change during the course of execution
												// even when the client disconnects/reconnects

		if (m_nClientIndex == 0)
			TRACE(_T("m_nClientIndex == 0 Since nClientPortIndex=0 in OnAccept )\n"));
#if 0
		winsock2.h
#define SO_DEBUG        0x0001          /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002          /* socket has had listen() */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */
#define SO_BROADCAST    0x0020          /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040          /* bypass hardware when possible */
#define SO_LINGER       0x0080          /* linger on close if data present */
#define SO_OOBINLINE    0x0100          /* leave received OOB data in line */


#endif

		// May be redundant to set these in Asocket since same as the listening socket
		sockerr = Asocket.SetSockOpt(SO_REUSEADDR, &bufBOOL, sizeof(int), SOL_SOCKET);
		ASSERT(sockerr != SOCKET_ERROR);

		sockerr = Asocket.SetSockOpt(SO_ACCEPTCONN, &bufBOOL, sizeof(int), SOL_SOCKET);
		ASSERT(sockerr != SOCKET_ERROR);

		sockerr = Asocket.SetSockOpt(SO_DONTROUTE, &bufBOOL, sizeof(int), SOL_SOCKET);
		ASSERT(sockerr != SOCKET_ERROR);

		sockerr = Asocket.SetSockOpt(TCP_NODELAY, &bufBOOL, sizeof(int), IPPROTO_TCP);
		ASSERT(sockerr != SOCKET_ERROR);

		int nSize;
		int nSizeOf = sizeof(int);
		GetSockOpt(SO_SNDBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("ServerSocket NIC Transmit Buffer Size = %d\n"), nSize);
		TRACE(s);

		GetSockOpt(SO_RCVBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("ServerSocket NIC Receiver Buffer Size = %d\n"), nSize);
		TRACE(s);

		// create a connection thread to own/contain this socket... if one already exists, kill it first
		// assume there is only one connection from the client to the server. If there is more than
		// one connection we can determine from the clients IP address captured above in the GetPeerName
		// which client we are connecting to and adjust the pClientConnection[] subscript accordingly
		// For now assume only one Phased Array Master processor is connected to one PA GUI server.
		if (m_pSCM->m_pstSCM == NULL)
			{
			TRACE("Fatal error, pMySCM->m_pstSCM == NULL\n");
			Asocket.Close();
			CAsyncSocket::OnAccept(nErrorCode);
			return;
			}

		// CREATE THE STRUCTURE to hold the ST_SERVERS_CLIENT_CONNECTION info
		ST_SERVERS_CLIENT_CONNECTION *pscc;

		if (m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] == NULL)	// first time thru
			{
			pscc = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] = new ST_SERVERS_CLIENT_CONNECTION();
			OnAcceptInitializeConnectionStats(pscc, nMyServer, m_nClientIndex);
			pscc->sClientIP4 = Ip4;
			pscc->m_nClientIndex = m_nClientIndex;

			s.Format(_T("PAPSrv[%d]:Instrument[%d]"), nMyServer, m_nClientIndex);
			t = s + _T("  OnAccept() creating critical sections/lists/vChannels\n");

			TRACE(t);

			//		theApp.SaveDebugLog(t);
			pMainDlg->SaveDebugLog(t);
			pscc->szSocketName = s;
			pscc->uClientPort = uPort;
			m_pstSCM->nComThreadExited[m_nClientIndex] = 0;
			SetpSCC(pscc);

			///////////////////////////////////////
			s = _T("AfxBeginThread(RUNTIME_CLASS (CServerSocketOwnerThread) is next\n");
			TRACE(s);
			// ServerSocketOwnerThread will attach to the accepted socket at the priority level of the ServerSocketOwnerThread
			// This allows (we think) the socket to run at a high priority level
			// create a new thread IN SUSPENDED STATE. LEAVE AUTODELETE ON !!!
			CServerSocketOwnerThread * pThread =
				m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread = (CServerSocketOwnerThread *)AfxBeginThread(RUNTIME_CLASS(CServerSocketOwnerThread),
					THREAD_PRIORITY_ABOVE_NORMAL,
					0,					// stacksize
					CREATE_SUSPENDED,	// runstate
					NULL);				// security
										//pThread->m_bAutoDelete = 0;

			s.Format(_T("CServerSocketOwnerThread[%d][%d]= 0x%08x, Id=0x%04x was created\n"),
				nMyServer, m_nClientIndex, pThread, pThread->m_nThreadID);
			TRACE(s);
			// Init some things in the thread before it runs. We are now accessing things inside the new thread, not in this thread
			if (pThread)
				{
				// This is how we boost the ServerSocket to a higher priority
				//pThread->m_pConnectionSocket	= new CServerSocket(m_pSCM, eServerConnection);
				pThread->m_pSCM = m_pSCM;
				pThread->m_pstSCM = m_pSCM->m_pstSCM;
				pThread->m_nMyServer = m_pSCM->m_pstSCM->pSCM->m_nMyServer;
				pThread->m_pSCC = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];
				pThread->m_nClientIndex = m_nClientIndex;
				pThread->m_pSCC->pSocket = new CServerSocket(m_pSCM, eServerConnection); ;
				s.Format(_T("CServerSocket::OnAccept, pThread->m_pSCC->pSocket = 0x%08x\n"), pThread->m_pSCC->pSocket);
				TRACE(s);


				//pThread->m_pSCC->pSocket = NULL;
				pThread->m_hConnectionSocket = Asocket.Detach();	// hand off the socket we just accepted to the thread
				Sleep(10);
				// Resume will cause CServerSocketOwnerThread::InitInstance() to execute
				// pThread->m_pSCC->pSocket = (CServerSocket *) FromHandle(Asocket.Detach()); doesn't work
				pThread->m_pSCC->pSocket->m_nOwningThreadType = eServerConnection;
				pThread->m_pSCC->pSocket->m_pSCC = pThread->m_pSCC;
				pThread->m_pSCC->bConnected = 1;
				pThread->m_pSCC->pSocket->m_pSCM = pThread->m_pSCM;
				pThread->m_pSCC->pSocket->m_pstSCM = pThread->m_pstSCM;
				pThread->m_pSCC->pSocket->m_nClientIndex = m_nClientIndex;
				pThread->ResumeThread();
				}
			else
				{
				// do some sort of cleanup
				ASSERT(0);
				s = _T("Cleanup");
				}

			// Display the connect socket IP and port
			Asocket.GetSockName(Ip4, uPort);	// my socket info??
			pThread->m_pSCC->bConnected = eConfigured;

			char buffer[80], txt[64];
			strcpy(buffer, GetTimeStringPtr());
			CstringToChar(Ip4, txt);
			printf("Instrument Client[%d]  on socket %s : %d accepted to server at %s\n",
				m_nClientIndex, txt, uPort, buffer);
			sOut = buffer;
			s.Format(_T("\nInstrument Client[%d]  on socket %s : %d accepted by server at %s\n"),
				m_nClientIndex, Ip4, uPort, sOut);
			TRACE(s);
			pMainDlg->SaveDebugLog(s);
			Sleep(10);

			// Asocket.Close();	not necessary. Since Asocket on stack, when this routine ends, Asocket deletes
			// destructor closes the socket


			}	// if (m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] == NULL)
				
		/************************** What if already connected ?? *******************/

		else 	if (m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] &&
			(m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread))
			{
			//2017-05-24 try detaching the existing socket and reattaching the new socket
			SOCKET hSocket;
			pscc = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];	// shortcut for coding
			pscc->m_nClientIndex = m_nClientIndex;
			if (pscc->pSocket == 0)
				{
				m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pSocket = new CServerSocket(m_pSCM, eServerConnection);
				if (pscc->pSocket)
					{
					pscc->pSocket->m_nClientIndex = m_nClientIndex;
					pscc->pSocket->m_pSCM = m_pSCM;
					pscc->pSocket->m_pstSCM = m_pSCM->m_pstSCM;
					pscc->pSocket->m_pSCC = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];
					//hSocket = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pSocket->Detach();
					}
				}
			hSocket = Asocket.Detach();
			m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread->m_hConnectionSocket = hSocket;
			m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread->m_pSCC->pSocket = pscc->pSocket;
			m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->pServerSocketOwnerThread->m_pSCC->pSocket->Attach(hSocket);
			m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]->bConnected = 1;
			Sleep(10);
			//Sleep(10);
			}

		else
			{
			ASSERT(0);	// got a break here from real instrument 2016-09-08
			Asocket.Close();
			CAsyncSocket::OnAccept(nErrorCode);
			s.Format(_T("Refused connection: no ClientConnection or no ServerSocketOwnerThread Error = %d\n"),
				nErrorCode);
			return;
			}


		CAsyncSocket::OnAccept(nErrorCode);
		//pThread->Run();
		}

#endif


	/******************************************************************************/
	/******************************************************************************/

	// Collect received data into expected packet lengths. That is
	// reconstruct packet from received data.  Its a feature of TCPIP.

// Packets received are "repackaged" to include the length of the packet as the first item in the new packet
// [length of packet = n][packet data ... n bytes] 2016-12-13 included in new definition of packets
void CServerSocket::OnReceive(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	//BYTE Buf[MAX_PAM_BYTES+8];			// put it on the stack instead of the heap. Probably quicker
	
	int nWholePacketQty = 0;

	BYTE *pB;	// debug
	void *pPacket = 0;
	int nPacketSize;
	WORD wByteCnt;
	GenericPacketHeader *pHeader;

	int n;
	CString s, t;

	// If shutting down and stop send/receive set, throw away the data
	if (m_pSCM->m_pstSCM == NULL)				return;	
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag)	return;
	if (m_pSCC == NULL)							return;

//	if (m_nClientIndex == 0)
//		TRACE( _T( "m_nClientIndex == 0 In OnReceive\n" ) );
//	s.Format( _T( "m_nClientIndex == %d In OnReceive\n" ), m_nClientIndex );
//	TRACE( s );

	if (m_pSCC->bStopSendRcv)
			{
			n = -1;	// forced exit
			m_pSCC->bConnected = (BYTE) eNotConnected;
			}

	// A real hardware FIFO would shift data to the output side instantly
	m_pFifo->Shift();
	BYTE *pCmd = m_pFifo->GetInLoc();
	// Receive() receives data into fifo memory pointed to by pCmd
	n = Receive( (void *) pCmd, 0x2000, 0 );	// ask for 8k byte into 16k buffer
#if		DEBUG_TCPIP_FROM_INSTRUMENT
	if (n > 1460)
		{
		//debugging
		s.Format(_T("Big packet = 0x%04x, = %05d\n"), n,n);
		TRACE(s);
		}
#endif

	//PAM assumes we will get partial packets and have to extract whole packets
	if ( n > 0)
		{
		m_pFifo->AddBytesToFifo(n);

#if DEBUG_TCPIP_FROM_INSTRUMENT
			{
			s.Format(_T("[%4d]Server[%d]Socket[%d] got %d bytes, SeqCnt = %d\n"), 
				m_pSCC->uPacketsReceived, m_pSCM->m_nMyServer, m_pSCC->m_nClientIndex, 
				n, m_pFifo->m_wMsgSeqCnt);
			TRACE(s);
			}
#endif

		while (1)	// total byte in FIFO. May be multiple packets.
			{	// get packets
			wByteCnt = m_pFifo->GetFIFOBytes();
			if (wByteCnt == 672)
				n = 672;

			if (wByteCnt < sizeof(GenericPacketHeader))
				{
				//CAsyncSocket::OnReceive(nErrorCode);	// wait for more bytes on next OnReceive
				//return;
				break;	// check at bottom to see if need to signal RcvListThread
				}
			nPacketSize = m_pFifo->GetPacketSize();
			if ((nPacketSize <= 0) || (wByteCnt < nPacketSize))
				{
				CAsyncSocket::OnReceive(nErrorCode);	// wait for more bytes on next OnReceive
				return;
				}

			pPacket = m_pFifo->GetNextPacket();
			if (pPacket == NULL)
				{
				CAsyncSocket::OnReceive(nErrorCode);
				return;
				}
			pHeader = (GenericPacketHeader *)pPacket;

			// See if received messages are skipping MsgSeqCnt, ie. some packets not actually sent
			memcpy((void*)&m_HeaderDbg[m_dbg_cnt++], (void *) pHeader, sizeof(GenericPacketHeader));
			m_dbg_cnt &= 7;

			// debugging to see if all packets are caught at some time, maybe out of order
			s.Format(_T("OnReceive MsgSeqCnt = %5d\n"), pHeader->wMsgSeqCnt);
			//pMainDlg->SaveDebugLog(s);

			if ((pHeader->wMsgSeqCnt - (m_nLastSeqCnt+1)) != 0) 
				{
				n = m_nSeqIndx;
				int j = GetRcvListCount();
				s.Format(_T("Lost Packet, Socket %d OnReceive got MsgSeqCnt %d, expected %d..RcvList Count = %5d\n"),
					m_pSCM->GetServerPort(),  pHeader->wMsgSeqCnt, (m_nLastSeqCnt + 1), j);
				TRACE(s);
				pMainDlg->SaveDebugLog(s);
				}
			m_nLastSeqCnt = pHeader->wMsgSeqCnt;
			pB =  new BYTE[nPacketSize];	// +sizeof(int)];	// resize the buffer that will actually be used
			memcpy( (void *) pB, pPacket, nPacketSize);	// move all data to the new buffer
			//InputRawDataPacket *pIdataPacket = (InputRawDataPacket *) pB;
			IDATA_FROM_HW *pIdataPacket = (IDATA_FROM_HW *) pB;

#if 0
			if (pIdataPacket->wMsgID == 3)
				{
				s = _T("ReadBackData\n");
				TRACE(s);
				}
#endif
			
			m_nSeqCntDbg[m_nSeqIndx++] = pIdataPacket->wMsgSeqCnt;
			m_nSeqIndx &= 0x3ff;

			LockRcvPktList();
			if (m_pSCC)
				{
				if (m_pSCC->pServerRcvListThread)
					{
					AddTailRcvPkt(pB);	// put the buffer into the recd data linked list
					nWholePacketQty++;
					// WM_USER_SERVERSOCKET_PKT_RECEIVED
					// the posted message will be processed by: CServerRcvListThread::ProcessRcvList(WPARAM w, LPARAM lParam)
					// ProcessRcvList needs to be of lower priority than OnReceive in order for OnReceive to quickly
					// transfer data into a linked list and then exit.
					// m_pSCC->pServerRcvListThread->PostThreadMessage(WM_USER_SERVERSOCKET_PKT_RECEIVED,0,0L);
					}
				}
			else
				{
				delete pB;
				TRACE(_T("CServerSocket::OnReceive - deleting data because no ServerRcvListThread\n"));
				}

			UnLockRcvPktList();
			if (m_pSCC)
				{
				m_pSCC->uBytesReceived += nPacketSize;
				m_pSCC->uPacketsReceived++;
				if (m_pElapseTimer)
					{
					if ((m_pSCC->uPacketsReceived & 0x7ff) == 0)	m_pElapseTimer->Start();
					if ((m_pSCC->uPacketsReceived & 0x7ff) == 0x7ff)
						{
						m_nElapseTime = m_pElapseTimer->Stop(); // elapse time in uSec for 2048 packets
						float fPksPerSec = 2048000000.0f/( (float) m_nElapseTime);
						m_pSCC->uPacketsPerSecond = (UINT)fPksPerSec;
						s.Format(_T("[%5d]Server[%d]Socket[%d]::OnReceive - [SeqCnt=%5d] Packets/sec = %6.1f\n"), 
							m_pSCC->uPacketsReceived, m_pSCM->m_nMyServer, m_pSCC->m_nClientIndex, 
							pIdataPacket->wMsgSeqCnt, fPksPerSec);
						TRACE(s);
						pMainDlg->SaveDebugLog(s);
						}
					}
				}	// if (m_pSCC)
			} 	// get packets
				
		//theApp.ReleaseInstrumentListAccess(m_pSCC->m_nClientIndex);

		// Post a message to someone who cares and let that routine/class/function deal with the packet
		// Posted message goes to CServerRcvListThread::ProcessRcvList()
		// which calls CServerRcvListThread::ProcessInstrumentData()
		if (nWholePacketQty)
			{
			int i;
			i = GetRcvListCount();
			if (m_nListCount < i)
				{
				m_nListCountChanged = 1;
				m_nListCount = i;
				s.Format(_T("Idata RcvPktList Count = %d\n"), m_nListCount);
				TRACE(s);
				}
			else if ((m_nListCountChanged) && (i < m_nListCount) )
				{
				s.Format(_T("Idata RcvPktList Count decreased from = %d to = %d\n"), m_nListCount, i);
				TRACE(s);
				m_nListCountChanged = 0;
				}
			// causes CServerRcvListThread::ProcessRcvList(WPARAM w, LPARAM lParam) to run
			m_pSCC->pServerRcvListThread->PostThreadMessage(WM_USER_SERVERSOCKET_PKT_RECEIVED,(WORD)m_pSCC->m_nClientIndex,0L);
			}

		if (m_pSCC)
			{
			if (m_pSCC->bConnected == (BYTE) eNotConnected)
				m_pSCC->bConnected = (BYTE) eNotConfigured;
			//if (m_pSCC->uMaxPacketReceived < (unsigned) m)	m_pSCC->uMaxPacketReceived = m;
			}
		}	// if ( n > 0)
	else	
		{	// if ( n < 0)
		n = GetLastError();
		s.Format( _T( "OnReceive caused error %d\n" ), n );
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		}


	CAsyncSocket::OnReceive(nErrorCode);
	}


void CServerSocket::OnClose(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	// kill off our pClientConnection before we leave
	// KillpClientConnectionStruct();
	int i = 0;
	CString s;

	// kill the socket's thread  .. a partial shutdown
	//CAsyncSocket::OnClose(nErrorCode);	//0x2745 on a restart of instrument = 10053
	// #define WSAECONNABORTED                  10053L
	// very different in PAP compared to PAG
		
	//CAsyncSocket::OnClose(nErrorCode);
	if (i = this->ShutDown( 2 ))
		{
		s.Format( _T( "Shutdown of client socket was successful status = %d\n" ), i );
		TRACE( s );
		this->Close();
		pMainDlg->SaveDebugLog(s);
		}
	else
		{
		s = _T( "Shutdown of client socket failed\n" );
		TRACE( s );
		pMainDlg->SaveDebugLog(s);
		}
	Sleep( 10 );
	CAsyncSocket::OnClose(nErrorCode);

	TRACE( _T( "CAsyncSocket::OnClose(nErrorCode) called\n" ) );
	if (nErrorCode)
		{
		TRACE( _T( "OnClose failed\n" ) );
		}

	if (m_nClientIndex == 0)
		TRACE( _T( "m_nClientIndex == 0 BEFORE GetpSCC()\n" ) );
	m_pSCC = GetpSCC();

	if (m_nClientIndex == 0)
		TRACE( _T( "m_nClientIndex == 0 After GetpSCC()\n" ) );
//	m_pSCC->m_bIsClosing = 1;
	if (m_pSCM == nullptr)
		{
		TRACE( _T( "m_pSCM == nullptr\n" ) );
		return;
		}

	delete this;

	}



// If the socket is to be used for listening, initialize with 'default' values. Fail and return
// error if anything goes wrong
int CServerSocket::InitListeningSocket(CServerConnectionManagement * pSCM)
	{
	int nSockOpt = TRUE;
	int  sockerr=0;
	UINT uPort;

	if ( NULL == pSCM)	return -1;
	pSCM->SetServerType(eListener);
	uPort = pSCM->GetServerPort();
	if (uPort == 0) return -1;

	// We want to be called by the OS when data a packet is received, when a client connects to the server, 
	// when a socket closes, and when the client on the other end has accepted the connection
	// 
	this->m_nOwningThreadType = eListener;

	// the final null can be replaced with an ip4 address if a specific NIC is desired to be used,
	// otherwise this socket will listen on all NIC's -- lpszSockAddress = null
	if (sockerr = this->Create(uPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT,  NULL )	!= 0 )
		{	// Socket created

		nSockOpt = 1;
		// need to be able to reuse the ip address
		sockerr = this->SetSockOpt(SO_REUSEADDR, &nSockOpt, sizeof(int),SOL_SOCKET);
		if (sockerr == SOCKET_ERROR)
			{
			TRACE1("Socket Error SO_REUSEADDR = %0x\n", sockerr);
			return sockerr;
			}

		sockerr = this->SetSockOpt(SO_ACCEPTCONN, &nSockOpt, sizeof(int),SOL_SOCKET);
		if (sockerr == SOCKET_ERROR)
			{
			TRACE1("Socket Error SO_ACCEPTCONN = %0x\n", sockerr);
			return sockerr;
			}

		nSockOpt = 1;
		// when data ready to send, send without delay
		sockerr = this->SetSockOpt(TCP_NODELAY, &nSockOpt, sizeof(int),IPPROTO_TCP); 
		if (sockerr == SOCKET_ERROR)
			{
			TRACE1("Socket Error TCP_NODELAY = %0x\n", sockerr);
			return sockerr;
			}
		nSockOpt = 1;
		sockerr = this->SetSockOpt(SO_DONTLINGER, &nSockOpt, sizeof(int),SOL_SOCKET);
		if (sockerr == SOCKET_ERROR)
			{
			TRACE1("Socket Error SO_DONTLINGER = %0x\n", sockerr);
			return sockerr;
			}
		}

	else
		{								
		
		TRACE("Failed to create stream socket... aborting\n");
		sockerr = GetLastError();
		TRACE1("Create Socket Error = 0x%x\n", sockerr);
		return -1;	// C_CLIENT_SOCKET_CREATION_ERROR;
		}

	return 0;
	}


// Initialize the elements of the structure ST_SERVERS_CLIENT_CONNECTION
// Get info from global static structure stSCM[nMyServer]
void CServerSocket::OnAcceptInitializeConnectionStats(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClientPortIndex)
	{
	CString s;
	int i,j;
	i = j = 0;

	s.Format(_T("Send%d"), nClientPortIndex);
	pscc->szSocketName	= _T("");
	pscc->sClientName	= _T("");
	pscc->sClientIP4	= _T("");			
	m_nClientIndex		= nClientPortIndex;
	pscc->pCSSendPkt = new CRITICAL_SECTION();
	pscc->pCSRcvPkt = new CRITICAL_SECTION();
	s.Format(_T("Server Crit Sections Mem Addr: Send= 0x%x, Rcv= 0x%x\n"), &pscc->pCSSendPkt, &pscc->pCSRcvPkt);
	TRACE(s);
	InitializeCriticalSectionAndSpinCount(pscc->pCSSendPkt,4);
	InitializeCriticalSectionAndSpinCount(pscc->pCSRcvPkt,4);
	pscc->pSendPktList = new CPtrList(64);
	pscc->pRcvPktList = new CPtrList(64);
	s.Format(_T("Server Lists Mem Addr: Send= 0x%x, Rcv= 0x%x\n"), &pscc->pSendPktList, &pscc->pRcvPktList);
	TRACE(s);

	pscc->pSocket					= NULL;		
	pscc->pServerSocketOwnerThread	= NULL;
	pscc->pServerRcvListThread		= NULL;
	pscc->bConnected				= (BYTE) eNotConfigured;
	pscc->m_bIsClosing				= 0;
	pscc->bStopSendRcv				= 0;
	pscc->uPacketsReceived			= 0;
	pscc->uBytesReceived			= 0;
	pscc->uPacketsPerSecond			= 0;			
	pscc->uMaxPacketReceived		= 0;		
	pscc->uInvalidPacketReceived	= 0;	
	pscc->uLostReceivedPackets		= 0;		
	pscc->uDuplicateReceivedPackets	= 0;
	pscc->uPacketsSent				= 0;
	pscc->uBytesSent				= 0;
	pscc->uUnsentPackets			= 0;
	pscc->uLastTick					= 0;
	SetpSCC( pscc );	// important or we get nulls

// Only in PAP - done on individual connection for every sequence and channel
#ifdef I_AM_PAP
	for (j = 0; j < MAX_SEQ_COUNT; j++)
		{
		for (i = 0; i < MAX_CHNLS_PER_MAIN_BANG; i++)
			{
			pscc->pvChannel[j][i] = new CvChannel( j, i );
			}
		}
#endif
	}

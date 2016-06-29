// ServerSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
#include "AfxSock.h"
#include "time.h"
#include "Extern.h"

extern CCriticalSection g_CriticalSection;
extern CCriticalSection g_CriticalSection2;
extern CCriticalSection g_CriticalSectionAscan;
extern CPtrList g_pTcpListUtData;
extern CPtrList g_pTcpListUtData2;
extern CPtrList g_pTcpListAscan;
extern ASCAN_HEAD_NIOS g_AscanHead;	


// CServerSocket

CServerSocket::CServerSocket(CServerConnectionManagement *pSCM)
	{
	m_pSCM = pSCM;
	m_pElapseTimer = NULL;
	Init();
	}

CServerSocket::CServerSocket()
	{
	m_pSCM = NULL;
	m_pElapseTimer = NULL;
	m_nOwningThreadType = -1;
	}

CServerSocket::~CServerSocket()
	{
	CString s,t;
	int nId = AfxGetThread()->m_nThreadID;
	t.Format(_T("Thread Id=0x%04x - "), nId);

	switch (m_nOwningThreadType)
		{
	case eListener:
		s = _T("Listener Socket Destructor called\n");
		break;
	case eServerConnection:
		s.Format(_T("Server[%d] Connection Socket[%d] Destructor called\n"), m_nMyServer, m_nMyThreadIndex);
		break;
	default:
		s = _T("Unknown Socket Destructor called\n");
		break;
		}

	t += s;
	TRACE(t);
	if (m_pElapseTimer)	delete m_pElapseTimer;
	m_pElapseTimer = NULL;
	}

void CServerSocket::Init(void)
	{
	m_nOwningThreadType = -1;
	m_BufOffset = m_nStart = 0;
	memset((void *) m_RcvBuf,0,sizeof(m_RcvBuf));
	m_pElapseTimer = new CHwTimer();
	}

// CServerSocket member functions

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
void CServerSocket::OnAccept(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	int * bufBOOLsize;
	BOOL bufBOOL;
	bufBOOLsize = &bufBOOL;
    *bufBOOLsize = TRUE;
	int nMyServer;
	int sockerr;
	SOCKADDR SockAddr;
	int SockAddrLen = sizeof(SOCKADDR);
	int i;

	// how are we going to set our pSCM pointer???
	// get our threadID of the thread running me
	// and then see if it is in the static list to know which stSCM[MAX_SERVERS] we belong to
	m_nOwningThreadType = eListener;

	DWORD hMyThreadID = GetCurrentThreadId();
	// look at all the servers and see if I am in the list
	for (nMyServer = 0; nMyServer < MAX_SERVERS; nMyServer++)
		{
		if (hMyThreadID == stSCM[nMyServer].ListenThreadID)
			{	// found myself
			m_pSCM = pSCM[nMyServer];
			//m_pstSCM = m_pSCM->m_pstSCM;	// ptr to specific entry in global structure
			break;
			}
		}


	if ( ( nMyServer >= MAX_SERVERS) || (NULL == m_pSCM) )
		{
		ASSERT(0);
		TRACE("On Accept can't identify the controlling SCM\n");
		// need to throw away the socket??
		CAsyncSocket dummy;
		Accept(dummy);
		dummy.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		return;
		}


	CServerSocket Asocket(m_pSCM);	// a temporary Async socket of our fashioning on the stack
	Asocket.m_nOwningThreadType = eListener;

	// Accept the connection from our client into the temporary socket Asocket
	//
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

	CString Ip4,s;
	UINT uPort;
	int nClientPortIndex;			// which client are we connecting to? Derive from IP address
	UINT uClientBaseAddress;		// what is the 32 bit index of the 1st PA Master?
	uClientBaseAddress = inet_addr ("192.168.10.10");
	char cIp4[20];

	Asocket.GetSockName(Ip4,uPort);	// my socket info??
	s.Format(_T("Server side socket %s : %d\n"), Ip4, uPort);
	TRACE(s);
	Asocket.GetPeerName(Ip4,uPort);	// connecting clients info??
	s.Format(_T("Client side socket %s : %d\n"), Ip4, uPort);
	TRACE(s);
	//
	// Assume we know a range of addresses of clients which connected to this server
	// for example 192.168.10.11 - the first master to 192.168.10.18 - the eighth master in consecutive IP address order
	// and assume they all connect to the same nic on the MMI side and thus need their own pClientConnection
	// From the PeerName IP address we can compute the index for the pClientConnection
	//
	CstringToChar(Ip4,cIp4);
	nClientPortIndex = (inet_addr(cIp4) - uClientBaseAddress);
	if ( (nClientPortIndex < 0) || (nClientPortIndex >= MAX_CLIENTS_PER_SERVER) )
		{
		//CAsyncSocket dummy;
		//Accept(dummy);
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		s.Format(_T("Fatal error - nClientPortIndex = %d\n"), nClientPortIndex);
		TRACE(s);
		return;
		}


	// May be redundant to set these in Asocket since same as the listening socket
	sockerr = Asocket.SetSockOpt(SO_REUSEADDR, &bufBOOL, sizeof(int), SOL_SOCKET);
	ASSERT(sockerr != SOCKET_ERROR);

	sockerr = Asocket.SetSockOpt(SO_DONTROUTE, &bufBOOL, sizeof(int), SOL_SOCKET);
	ASSERT(sockerr != SOCKET_ERROR);

	sockerr = Asocket.SetSockOpt(TCP_NODELAY, &bufBOOL, sizeof(int), IPPROTO_TCP);
	ASSERT(sockerr != SOCKET_ERROR);
	// create a connection thread to own/contain this socket... if one already exists, kill it first
	// assume there is only one connection from the client to the server. If there is more than
	// one connection we can determine from the clients IP address captured above in the GetPeerName
	// which client we are connecting to and adjust the pClientConnection[] subscript accordingly
	// For now assume only one Phased Array Master processor is connected to on PA GUI server.
	if (m_pSCM->m_pstSCM == NULL)
		{
		TRACE("Fatal error, pMySCM->m_pstSCM == NULL\n");
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		return;
		}

	// CREATE THE STRUCTURE to hold the ST_SERVERS_CLIENT_CONNECTION info
	ST_SERVERS_CLIENT_CONNECTION *pscc;
	m_nMyServer = nMyServer;

	if (m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] == NULL)	// first time thru
		{
		pscc = m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] = new ST_SERVERS_CLIENT_CONNECTION();
		pscc->pSocket = NULL;
		pscc->m_nMyThreadIndex = nClientPortIndex;
		OnAcceptInitializeConnectionStats(pscc,nMyServer, nClientPortIndex);
		pscc->sClientIP4 = Ip4;
		s.Format(_T("PAGSrv[%d]:MasterInst[%d]"), nMyServer, nClientPortIndex);
		pscc->szSocketName = s;
		pscc->uClientPort = uPort;
		m_pSCM->m_pstSCM->nComThreadExited[nClientPortIndex] = 0;
		}
	else 	if (m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pServerSocketOwnerThread)
		{
		TRACE("CServerSocketOwnerThread ALREADY exists... kill it\n");
		CWinThread * pThread1 = (CWinThread *)m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pServerSocketOwnerThread;
		PostThreadMessage(pThread1->m_nThreadID,WM_QUIT, 0L, 0L);	// this will cause com thread to execute ExitInstance()
		// ExitInstance() will close the socket and delete the pClientConnection structure
		for ( i = 0; i <50; i++)
			{
			if (m_pSCM->m_pstSCM->nComThreadExited[nClientPortIndex])	break;
			Sleep(10);	// pretty bad to sleep inside an OS call back function!!!!
			}
		if ( i == 50) ASSERT(0);
		// redo what was above as if pClientConnection had never existed
		pscc = m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] = new ST_SERVERS_CLIENT_CONNECTION();
		pscc->pSocket = NULL;
		pscc->m_nMyThreadIndex = nClientPortIndex;
		OnAcceptInitializeConnectionStats(pscc,nMyServer, nClientPortIndex);
		pscc->sClientIP4 = Ip4;
		s.Format(_T("PAGSrv[%d]:MasterInst[%d]"), nMyServer, nClientPortIndex);
		pscc->szSocketName = s;
		pscc->uClientPort = uPort;
		m_pSCM->m_pstSCM->nComThreadExited[nClientPortIndex] = 0;
		}

	else	ASSERT(0);


	// create a new thread
	CServerSocketOwnerThread * pThread =
	m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pServerSocketOwnerThread = (CServerSocketOwnerThread *) AfxBeginThread(RUNTIME_CLASS (CServerSocketOwnerThread),
	   	   					  				                                THREAD_PRIORITY_ABOVE_NORMAL,
															                0,					// stacksize
											                                CREATE_SUSPENDED,	// runstate
																			NULL);				// security

	s.Format(_T("CServerSocketOwnerThread[%d][%d], Id=0x%04x was created\n"),
					nMyServer, nClientPortIndex, pThread->m_nThreadID);
	TRACE(s);
	// Init some things in the thread before it runs
	if (pThread)
		{
		pThread->m_pMySCM		= m_pSCM;
		pThread->m_pstSCM		= m_pSCM->m_pstSCM;
		pThread->m_nMyServer	= m_pSCM->m_pstSCM->pSCM->m_nMyServer;
		pThread->m_pSCC			= m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex];
		pThread->m_pSCC->pSocket = NULL;
		pThread->m_nThreadIndex	= nClientPortIndex;
		pThread->m_hConnectionSocket = Asocket.Detach();	// hand off the socket we just accepted to the thread
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
		
	// Display the connect socket IP and port
	Asocket.GetSockName(Ip4,uPort);	// my socket info??
	s.Format(_T("Client accepted to server on socket %s : %d\n"), Ip4, uPort);
	TRACE(s);

	time_t now;
	struct tm * timeinfo;
	char buffer [80];

	time ( &now );
	timeinfo = localtime ( &now );

	strftime (buffer,80,"%Y_%m_%d %H:%M:%S",timeinfo);
	printf("Instrument Client[%d] accepted to server on socket %s : %d at %s\n", nClientPortIndex, Ip4, uPort, buffer);
	Sleep(10);
			

	CAsyncSocket::OnAccept(nErrorCode);
	//pThread->Run();
	}


	/******************************************************************************/
	/******************************************************************************/

	// Collect received data into expected packet lengths. That is
	// reconstruct packet from received data.  Its a feature of TCPIP.
void * CServerSocket::GetWholePacket(int nPacketSize, int *pReceived)
	{

	if (!nPacketSize)
		{	// can't ask for 0 bytes
		m_BufOffset = 0;
		return NULL;
		}

	if (*pReceived)		// pReceived = &n, count of data already received
		{
		m_nStart = 0;	// 1st time called from OnReceive 
		m_BufOffset += *pReceived;	

		// TESTING ONLY
		if (*pReceived % nPacketSize)
			{
			pWholePacket = NULL;	// BREAK POINT does not occur at 6 ms between packets
			}

		*pReceived = 0;		// value in caller set to 0
		}

	if ( m_BufOffset >= nPacketSize )
		{	// accumulated at least one nPacketSize
		pWholePacket = (void *) &m_RcvBuf[m_nStart];
		m_nStart += nPacketSize;
		m_BufOffset -= nPacketSize;
		return pWholePacket;
		}

	else
		{	// move residual data to front of m_RcvBuf and adjust insertion point
		if (!m_BufOffset) return NULL;
		memcpy ( (void *) m_RcvBuf, (void *) &m_RcvBuf[m_nStart], m_BufOffset);
		return NULL;
		}

	}

// Packets received are "repackaged" to include the length of the packet as the first item in the new packet
//
void CServerSocket::OnReceive(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	//BYTE Buf[MAX_PAM_BYTES+8];			// put it on the stack instead of the heap. Probably quicker
	BYTE *pB;	// debug
	void *pPacket = 0;
	int nPacketSize = gServerArray[m_nMyServer].nPacketSize;
	//TCPDUMMY * Data = new TCPDUMMY;
	int n;
	CString s;
	// Put the data into a big circular buffer. May be multiple messages received at one time.
	n = Receive( (void *) &m_RcvBuf[m_BufOffset], (sizeof(m_RcvBuf) - m_BufOffset), 0 );
	// n = Receive( (void *) Buf, MAX_PAM_BYTES, 0 );	// read all data available into Buf
#if 1
	if (m_pSCC)
		{
		if (m_pSCC->uPacketsReceived < 30)
			s.Format(_T("[%4d]Server[%d]Socket[%d] got %d bytes\n"), 
			m_pSCC->uPacketsReceived, m_pSCM->m_nMyServer, m_pSCC->m_nMyThreadIndex, n);
		}
/**
	else
		{
		s.Format(_T("[???]CServerSocket::OnReceive got %d bytes\n"), n);
		}
**/
	TRACE(s);
#endif

	// If shutting down and stop send/receive set, throw away the data
	if (m_pSCM->m_pstSCM == NULL)	return;	
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag)	return;
	if (m_pSCC)
		{
		if (m_pSCC->bStopSendRcv)
			{
			n = -1;	// forced exit
			m_pSCC->bConnected = (BYTE) eNotConnected;
			}
		}
	else return;	// m_pSCC is NULL

	if ( n > 0)
		{
		// put it in the linked list and let someone else decipher it
		while ( pPacket = GetWholePacket(nPacketSize, &n))	// returns a ptr to void with length nPacketSize
			{	// get packets
			
			stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[nPacketSize+sizeof(int)];	// resize the buffer that will actually be used
			memcpy( (void *) &pBuf->Msg, pPacket, nPacketSize);	// move all data to the new buffer
			pBuf->nLength = nPacketSize;
			pB = (BYTE *) pBuf;	// debug helper			
			LockRcvPktList();
			if (m_pSCC)
				{
#if 0
				if (m_pSCC->pServerRcvListThread)
					{
					AddTailRcvPkt(pBuf);	// put the buffer into the recd data linked list
					m_pSCC->pServerRcvListThread->PostThreadMessage(WM_USER_CLIENT_PKT_RECEIVED,0,0L);
					}
#endif
				}
			else		delete pBuf;

			UnLockRcvPktList();
			if (m_pSCC)
				{
				m_pSCC->uBytesReceived += nPacketSize;
				m_pSCC->uPacketsReceived++;
				if ((m_pSCC->uPacketsReceived & 0xfff) == 0)	m_pElapseTimer->Start();
				if ((m_pSCC->uPacketsReceived & 0xfff) == 0xfff)
					{
					m_nElapseTime = m_pElapseTimer->Stop(); // elapse time in uSec for 4k packets
					float fPksPerSec = 4096000000.0f/( (float) m_nElapseTime);
					m_pSCC->uPacketsPerSecond = (UINT)fPksPerSec;
					s.Format(_T("[%5d]Server[%d]Socket[%d]::Packets/sec = %6.1f\n"), 
						m_pSCC->uPacketsReceived, m_pSCM->m_nMyServer, m_pSCC->m_nMyThreadIndex, fPksPerSec);
					TRACE(s);
					}
				}
			} 	// get packets

		// Post a message to someone who cares and let that routine/class/function deal with the packet
		if (m_pSCC)
			{
			if (m_pSCC->bConnected == (BYTE) eNotConnected)
				m_pSCC->bConnected = (BYTE) eNotConfigured;
			if (m_pSCC->uMaxPacketReceived < (unsigned) n)	m_pSCC->uMaxPacketReceived = n;
			}
		}	// if ( n > 0)
	else	
		{	// if ( n < 0)
		n = GetLastError();
		TRACE(_T("OnReceive caused error %d\n"), n);
		}


	CAsyncSocket::OnReceive(nErrorCode);
	}


void CServerSocket::OnClose(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	// kill off our pClientConnection before we leave
	// KillpClientConnectionStruct();

	CAsyncSocket::OnClose(nErrorCode);
#if 0
	// kill the socket's thread  .. a partial shutdown
	if (m_pSCC)
		{
		if (m_pSCC->pServerSocketOwnerThread)
			{
			PostThreadMessage(m_pSCC->pServerSocketOwnerThread->m_nThreadID,WM_QUIT, 0L, 0L);
			}
		Sleep(200);
		}
#endif

	}

void CServerSocket::KillpClientConnectionStruct(void)
	{
	void *pV;
	// Kill it at the socket level
	if (m_pSCC)
		{
		m_pSCC->bConnected = (BYTE) eNotConnected;
		Sleep(5);
		LockRcvPktList();
		while ( m_pSCC->pRcvPktList->GetCount() > 0)
			{
			pV = (void *) m_pSCC->pRcvPktList->RemoveHead();
			delete pV;
			}
		UnLockRcvPktList();
		delete m_pSCC->pRcvPktList;		m_pSCC->pRcvPktList	= NULL;
		delete m_pSCC->pCSRcvPkt;				m_pSCC->pCSRcvPkt			= NULL;

		LockSendPktList();
		while ( m_pSCC->pSendPktList->GetCount() > 0)
			{
			pV = (void *) m_pSCC->pSendPktList->RemoveHead();
			delete pV;
			}
		UnLockSendPktList();
		delete m_pSCC->pSendPktList;		m_pSCC->pSendPktList	= NULL;
		delete m_pSCC->pCSSendPkt;				m_pSCC->pCSSendPkt			= NULL;

		}
	// zero ptrs from thread and SCM class and structure
	}

// If the socket is to be used for listening, initialize with 'default' values. Fail and return
// error if anything goes wrong
int CServerSocket::InitListeningSocket(CServerConnectionManagement * pSCM)
	{
	int nSockOpt = TRUE;
	int  sockerr=0;
	short nPort;

	if ( NULL == pSCM)	return -1;
	pSCM->SetServerType(eListener);
	nPort = pSCM->GetServerPort();
	if (nPort <= 0) return -1;

	// We want to be called by the OS when data a packet is received, when a client connects to the server, 
	// when a socket closes, and when the client on the other end has accepted the connection
	// 
	this->m_nOwningThreadType = eListener;

	if (sockerr = this->Create(nPort, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT,  NULL )	!= 0 )
		{	// Socket created

		nSockOpt = 1;
		// need to be able to reuse the ip address
		sockerr = this->SetSockOpt(SO_REUSEADDR, &nSockOpt, sizeof(int),SOL_SOCKET);
		if (sockerr == SOCKET_ERROR)
			{
			TRACE1("Socket Error SO_REUSEADDR = %0x\n", sockerr);
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
void CServerSocket::OnAcceptInitializeConnectionStats(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClient)
	{
	CString s;

	s.Format(_T("Send%d"), nClient);
	pscc->szSocketName	= _T("");
	pscc->sClientName	= _T("");
	pscc->sClientIP4	= _T("");			
	pscc->uClientPort	= nClient;
	pscc->pCSSendPkt	= new CRITICAL_SECTION();
	pscc->pCSRcvPkt		= new CRITICAL_SECTION();
	InitializeCriticalSectionAndSpinCount(pscc->pCSSendPkt,4);
	InitializeCriticalSectionAndSpinCount(pscc->pCSRcvPkt,4);
	pscc->pSendPktList	= new CPtrList(64);
	pscc->pRcvPktList		= new CPtrList(64);

	pscc->pSocket		= NULL;		
	pscc->pServerSocketOwnerThread	= NULL;
	pscc->bConnected				= (BYTE) eNotConnected;
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
	}

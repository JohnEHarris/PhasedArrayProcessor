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
// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP

#include "ServiceApp.h"
#include "AfxSock.h"
#include "time.h"

#else

#include "Truscan.h"
#include "TscanDlg.h"
#include "Extern.h"

#endif


extern char *GetTimeStringPtr(void);

// CServerSocket

CServerSocket::CServerSocket(CServerConnectionManagement *pSCM)
	{
	Init();
	m_pSCM = pSCM;
	m_pFifo = new CCmdFifo(INSTRUMENT_PACKET_SIZE);		// FIFO control for receiving instrument packets	
	}

CServerSocket::CServerSocket()
	{
	Init();
	m_pFifo = new CCmdFifo(INSTRUMENT_PACKET_SIZE);		// FIFO control for receiving instrument packets	
	}

CServerSocket::~CServerSocket()
	{
	CString s,t;
	int nId = AfxGetThread()->m_nThreadID;
	t.Format(_T("Thread Id=0x%04x - "), nId);

	switch (m_nOwningThreadType)
		{
	case eListener:
		s = _T("Listener Socket Destructor called\n");	// called when Asocket on stack disappears in OnAccept
		break;
	case eServerConnection:
		s.Format(_T("Server[%d] Connection Socket[%d] Destructor called\n"), m_nMyServer, m_nMyThreadIndex);
		m_pSCC->pSocket = 0;
		break;
	default:
		s = _T("Unknown Socket Destructor called\n");
		break;
		}

	t += s;
	TRACE(t);
	if (m_pElapseTimer)
		{
		delete m_pElapseTimer;
		m_pElapseTimer = NULL;
		}
		
	if (m_pFifo != NULL)
		delete m_pFifo;
	}

void CServerSocket::Init(void)
	{
	m_pSCM = NULL;
	m_pSCC = NULL;
	m_pstSCM = NULL;
	m_nOwningThreadType = -1;
	m_pElapseTimer = new CHwTimer();
	szName			= _T("SCM-Skt ");
	int nId = AfxGetThread()->m_nThreadID;
	CString s;
	s.Format(_T("CServerSocket::Init() invoked by thread ID = 0x%04x\n"), nId);
	m_nOnAcceptClientIndex = -1;
	TRACE(s);
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
	int nResult;
	BOOL bufBOOL;
	bufBOOLsize = &bufBOOL;
    *bufBOOLsize = TRUE;
	int nMyServer;
	int sockerr;
	SOCKADDR SockAddr;
	int SockAddrLen = sizeof(SOCKADDR);
//	int i;

	// how are we going to set our pSCM pointer???
	// get our threadID of the thread running me
	// and then see if it is in the static list to know which stSCM[MAX_SERVERS] we belong to
	m_nOwningThreadType = eListener;


#if 0
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
#endif
	// a better way to id which server I am
	// Only works for case of OnAccept. Does not always work for other cases such as OnClose

	for (nMyServer = 0; nMyServer < MAX_SERVERS; nMyServer++)
		{
		if (this == stSCM[nMyServer].pServerListenSocket)
			{	// found myself
			m_pSCM = pSCM[nMyServer];
			//m_pstSCM = m_pSCM->m_pstSCM;	// ptr to specific entry in global structure
			break;
			}
		}

	// If in shut down, refuse to accept a client
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag) 
		{
		TRACE("Server ShutDown Flag is true, aborting OnAccept\n");
		CAsyncSocket dummy;
		Accept(dummy);
		dummy.Close();	// should we close after CAsyncSocket::OnAccept ??? 2016-08-29 jeh
		CAsyncSocket::OnAccept(nErrorCode);
		return;
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


	CServerSocket Asocket(m_pSCM);	// a temporary Async socket of our fashioning ON THE STACK
	Asocket.m_nOwningThreadType = eListener;

	// ACCEPT the connection from our client into the temporary socket Asocket
	// ACCEPT the connection from our client into the temporary socket Asocket
	// ACCEPT the connection from our client into the temporary socket Asocket
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

	CString Ip4,s,t;
	UINT uPort;
	int nClientPortIndex;					// which client are we connecting to? Derive from IP address
	UINT uClientBaseAddress, uClientBaseAddress2;		// what is the 32 bit index of the 1st PA Master?
	WORD wClientBaseAddress[8];
	char *pIpBase = gServerArray[nMyServer].ClientBaseIp;

#ifdef CLIENT_AND_SERVER_ON_SAME_MACHINE
	uClientBaseAddress = inet_addr (pIpBase);	// Instrument base "192.168.10.201", PAG is "192.168.10.10"
#else
	uClientBaseAddress = ntohl(inet_addr (pIpBase));	// Instrument base "192.168.10.201", PAG is "192.168.10.10"
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
		uClientBaseAddress2 = ntohl(*(u_long*)&wClientBaseAddress);	// same value as uClientBaseAddress
		;
		}


#endif


	char cIp4[20];

	Asocket.GetSockName(Ip4,uPort);	// my socket info??
	s.Format(_T("Server side socket %s : %d\n"), Ip4, uPort);	// crash here 7-14-16 on reconnect by instrument
	TRACE(s);
	Asocket.GetPeerName(Ip4,uPort);	// connecting clients info??
	s.Format(_T("Client side socket %s : %d\n"), Ip4, uPort);
	TRACE(s);
	//
	CstringToChar(Ip4,cIp4);
	int ntmp = ntohl(inet_addr(cIp4));
	nClientPortIndex = (ntmp - uClientBaseAddress);	// 0-n

#ifdef THIS_IS_SERVICE_APP
	// Assume we know a range of addresses of clients which connected to this server
	// for example 192.168.10.201 - the first instrument and 192.168.10.208 - the eighth instrument 
	// in consecutive IP address order
	// and assume they all connect to the same nic on the PAM side and thus need their own pClientConnection
	// From the PeerName IP address we can compute the index for the pClientConnection
	//

	nClientPortIndex = (ntmp - uClientBaseAddress);

#else
	// Assume we know a range of addresses of clients which connected to this server
	// for example 192.168.10.10 - the first master to 192.168.10.17 - the eighth master in consecutive IP address order
	// and assume they all connect to the same nic on the MMI side and thus need their own pClientConnection
	// From the PeerName IP address we can compute the index for the pClientConnection
		
	//nClientPortIndex = (inet_addr(cIp4) - uClientBaseAddress); WHEN PAM on another machine
#endif
	
	if ( (nClientPortIndex < 0) || (nClientPortIndex >= MAX_CLIENTS_PER_SERVER) )
		{
		//CAsyncSocket dummy;
		//Accept(dummy);
		//dummy.Close();
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		s.Format(_T("Fatal error - nClientPortIndex = %d\n"),nClientPortIndex);
		TRACE(s);
		return;
		}

	// Stop crash when instrument power cycles
	m_pSCC = m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex];
	if (m_pSCC->bConnected )	// already has some level of being connected
		{
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		s.Format(_T("Fatal error - Already connected, nClientPortIndex = %d, Error Code = %d\n"),
			nClientPortIndex,nErrorCode);
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

	//ST_SERVERS_CLIENT_CONNECTION *pscc;
	m_nMyServer = nMyServer;

	if (m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] == 0)
		{
		// should never happen since this is created by ServiceApp and destroyed by ServiceApp
		ASSERT(0);
		}

	if (m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pSocket == 0)		//no connection yet
		{
		//m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] = new ST_SERVERS_CLIENT_CONNECTION();
		// Notice that m_pSCC points to the same object as m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]
		// deleting pClientConnection[] will delete m_pSCC. But setting pClientConnection[] = 0
		// will not automaticallyset m_pSCC to 0
		m_pSCC = m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex];
		m_nOnAcceptClientIndex = -1;	// cheat, but not too much
		//m_pSCC->pSocket = 0;		// hold off idle loop in ServiceApp. When not zero clear to run
		// CREATE THE STRUCTURE to hold the ST_SERVERS_CLIENT_CONNECTION info
		nResult = BuildClientConnectionStructure(m_pSCC, m_nMyServer, nClientPortIndex);


		m_pSCC->sClientIP4 = Ip4;
#ifdef THIS_IS_SERVICE_APP
		s.Format(_T("PAMSrv[%d]:Instrument[%d]"), nMyServer, nClientPortIndex);
		t = s + _T("  OnAccept() creating critical sections/lists/vChannels\n");
#else
		s.Format(_T("PAGSrv[%d]:MasterInst[%d] OnAccept"), nMyServer, nClientPortIndex);
		t = s;
#endif
		TRACE(t);
		theApp.SaveDebugLog(t);
		m_pSCC->szSocketName = s;
		m_pSCC->uClientPort = uPort;
		m_pSCM->m_pstSCM->nComThreadExited[nClientPortIndex] = 0;
		}

	/************************** What if already connected ?? *******************/

	else 	if	(m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] && 
				(m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pServerSocketOwnerThread))
		{
		m_nOnAcceptClientIndex = nClientPortIndex;
		// a little cheating here to get info to OnClose for this situation
		OnClose(nErrorCode); // OnClose kills ServerSocketOwnerTherad but the base Async OnClose does not
		TRACE("CServerSocketOwnerThread ALREADY exists... kill it\n");
#if 0
		CWinThread * pThread1 = 
			(CWinThread *)m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pServerSocketOwnerThread;
		// wParam = ClientPortIndex, lParam = ST_SERVERS_CLIENT_CONNECTION *
		PostThreadMessage(pThread1->m_nThreadID,WM_USER_KILL_OWNER_SOCKET, (WORD)nClientPortIndex, (LPARAM)m_pSCC);	
		// this will cause ServerSocketOwner to execute ExitInstance()
		// ExitInstance() will close the socket and delete the pClientConnection structures
#endif
#if 0
		for ( i = 0; i <50; i++)
			{
			if (m_pSCM->m_pstSCM->nComThreadExited[nClientPortIndex])	
				break;
			Sleep(10);	// pretty bad to sleep inside an OS call back function!!!!
			}
		s.Format("Wait loop in OnAccept for ComThreadExited is %d\n", i);
		TRACE(s);
		if ( i == 50) ASSERT(0);
#endif
		// redo what was above as if pClientConnection had never existed <<<< crashed here on friday 9-16-16
		// after instrument power cycle.
		m_pSCC = m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex];	
		nResult = BuildClientConnectionStructure(m_pSCC, m_nMyServer, nClientPortIndex);

		m_pSCC->sClientIP4 = Ip4;
		s.Format(_T("PAGSrv[%d]:MasterInst[%d]"), nMyServer, nClientPortIndex);
		m_pSCC->szSocketName = s;
		m_pSCC->uClientPort = uPort;
		m_pSCM->m_pstSCM->nComThreadExited[nClientPortIndex] = 0;
		}

	else
		{
		ASSERT(0);	// got a break here from real instrument 2016-09-08
		Asocket.Close();
		CAsyncSocket::OnAccept(nErrorCode);
		s.Format("Refused connection: no ClientConnection or no ServerSocketOwnerThread Error = %d\n",
			nErrorCode);
		return;
		}


	// create a new thread IN SUSPENDED STATE ....and turn off auto delete. Must explicitly delete thread to run destructor.
	//  THIS DID NOT WORK. LEAVE AUTODELETE ON !!!
	CServerSocketOwnerThread * pThread =
	m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex]->pServerSocketOwnerThread = (CServerSocketOwnerThread *) AfxBeginThread(RUNTIME_CLASS (CServerSocketOwnerThread),
	   	   					  				                                THREAD_PRIORITY_ABOVE_NORMAL,
															                0,					// stacksize
											                                CREATE_SUSPENDED,	// runstate
																			NULL);				// security
	//pThread->m_bAutoDelete = 0;

	s.Format(_T("CServerSocketOwnerThread[%d][%d]= 0x%08x, Id=0x%04x was created\n"),
					nMyServer, nClientPortIndex, pThread, pThread->m_nThreadID);
	TRACE(s);
	// Init some things in the thread before it runs. We are now accessing things inside the new thread, not in this thread
	if (pThread)
		{
		pThread->m_pConnectionSocket					= new CServerSocket();
		pThread->m_pConnectionSocket->m_pSCM			=	pThread->m_pMySCM		= m_pSCM;
		pThread->m_pConnectionSocket->m_pSCM->m_pstSCM	=	pThread->m_pstSCM		= m_pSCM->m_pstSCM;
		pThread->m_pConnectionSocket->m_nMyServer		=	pThread->m_nMyServer	= m_pSCM->m_pstSCM->pSCM->m_nMyServer;
		pThread->m_pConnectionSocket->m_pSCC			=	pThread->m_pSCC			= m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex];
		pThread->m_pConnectionSocket->m_pThread			=	pThread;
		pThread->m_pConnectionSocket->m_pSCC->m_nMyThreadIndex = pThread->m_nThreadIndex	= nClientPortIndex;
		pThread->m_pConnectionSocket->m_pSCC->pSocket	=	pThread->m_pConnectionSocket;
		pThread->m_pConnectionSocket->GetPeerName(Ip4,uPort);
		pThread->m_pConnectionSocket->SetClientIp4(Ip4);
		pThread->m_pConnectionSocket->m_pSCC->sClientIP4 = Ip4;
		pThread->m_pConnectionSocket->m_pSCC->uClientPort= uPort;

		//pThread->m_pSCC->pSocket = NULL;
		pThread->m_nThreadIndex	= nClientPortIndex;
		pThread->m_hConnectionSocket = Asocket.Detach();	// hand off the socket we just accepted to the thread
		Sleep(10);
		// Make the correct socket type selection in the thread resume
		// chooses between CServerSocket and CServerSocketPA_Master
		// Resume will cause CServerSocketOwnerThread::InitInstance() to execute
		// pThread->m_pSCC->pSocket = (CServerSocket *) FromHandle(Asocket.Detach()); doesn't work
		pThread->ResumeThread();
		}
	else
		{
		// do some sort of cleanup
		ASSERT(0);
		}
		
	// Display the connect socket IP and port
	Asocket.GetSockName(Ip4,uPort);	// my socket info??
	s.Format(_T("Client on socket %s : %d accepted to server\n"), Ip4, uPort);
	TRACE(s);
	theApp.SaveDebugLog(s);
		
	char buffer [80], txt[64];
	strcpy(buffer,GetTimeStringPtr());
	CstringToChar(Ip4, txt);
	printf("Instrument Client[%d]  on socket %s : %d accepted to server at %s\n", nClientPortIndex, txt, uPort, buffer);
	Sleep(10);
			
	// Asocket.Close();	not necessary. Since Asocket on stack, when this routine ends, Asocket deletes
	// destructor closes the socket
	CAsyncSocket::OnAccept(nErrorCode);
	//pThread->Run();
	}


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
//	nPacketSize = gServerArray[m_nMyServer].nPacketSize;	// 1040 as of 9-13-16 but likely bigger in the future
// Yiqing simulator sends 1460



	//TCPDUMMY * Data = new TCPDUMMY;
	int n, m = 0;
	CString s, t;

	// If shutting down and stop send/receive set, throw away the data
	if (m_pSCM->m_pstSCM == NULL)				return;	
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag)	return;
	if (m_pSCC == NULL)							return;

	// A Kludge for now --- how to correctly set packet size??
	//if (m_pSCC->uPacketsReceived == 0)
	//	{
	//	nPacketSize = 1456;		// real data INSTRUMENT_PACKET_SIZE = 1454
	//	m_pFifo->SetPacketSize(nPacketSize);
	//	}




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
	if (n > 1460)
		{
		//debugging
		s.Format(_T("Big packet = 0x%04x, = %05d\n"), n,n);
		TRACE(s);
		}
	//PAM assumes we will get partial packets and have to extract whole packets
	if ( n > 0)
		{
		m_pFifo->AddBytesToFifo(n);
		// reduce output to trace. When whole multiples of msg arrive, don't show
		//if ( n % nPacketSize)
			{
			s.Format(_T("[%4d]Server[%d]Socket[%d] got %d bytes, SeqCnt = %d\n"), 
				m_pSCC->uPacketsReceived, m_pSCM->m_nMyServer, m_pSCC->m_nMyThreadIndex, 
				n, m_pFifo->m_wMsgSeqCnt);
			TRACE(s);
			}
		//nPacketSize = m_pFifo->GetPacketSize();	//1454;	whatever Instrument package size is. 2016-06-28 JEH

		while (1)	// total byte in FIFO. May be multiple packets.
			{	// get packets
			wByteCnt = m_pFifo->GetFIFOBytes();
			if (wByteCnt < sizeof(GenericPacketHeader))
				{
				// m_pFifo->Shift();
				CAsyncSocket::OnReceive(nErrorCode);	// wait for more bytes on next OnReceive
				return;
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


			//stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[nPacketSize];	// +sizeof(int)];	// resize the buffer that will actually be used
			pB =  new BYTE[nPacketSize];	// +sizeof(int)];	// resize the buffer that will actually be used
			memcpy( (void *) pB, pPacket, nPacketSize);	// move all data to the new buffer
			//pB = (BYTE *) pBuf;	// debug helper	
			//m_pFifo->Reset();	not until the fifo is first emptied

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
					if ((m_pSCC->uPacketsReceived & 0xfff) == 0)	m_pElapseTimer->Start();
					if ((m_pSCC->uPacketsReceived & 0xfff) == 0xfff)
						{
						m_nElapseTime = m_pElapseTimer->Stop(); // elapse time in uSec for 4k packets
						float fPksPerSec = 4096000000.0f/( (float) m_nElapseTime);
						m_pSCC->uPacketsPerSecond = (UINT)fPksPerSec;
						s.Format(_T("[%5d]Server[%d]Socket[%d]::OnReceive - Packets/sec = %6.1f\n"), 
							m_pSCC->uPacketsReceived, m_pSCM->m_nMyServer, m_pSCC->m_nMyThreadIndex, fPksPerSec);
						TRACE(s);
						}
					}
				}	// if (m_pSCC)
			} 	// get packets
				
		//theApp.ReleaseInstrumentListAccess(m_pSCC->m_nMyThreadIndex);

		// Post a message to someone who cares and let that routine/class/function deal with the packet
		// Posted message goes to CServerRcvListThread::ProcessRcvList()
		// which calls CServerRcvListThread::ProcessInstrumentData()
		if (nWholePacketQty)
			{
			m_pSCC->pServerRcvListThread->PostThreadMessage(WM_USER_SERVERSOCKET_PKT_RECEIVED,(WORD)m_pSCC->m_nMyThreadIndex,0L);
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
		TRACE(_T("OnReceive caused error %d\n"), n);
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

#if 1

	// kill the socket's thread  .. a partial shutdown
	CAsyncSocket::OnClose(nErrorCode);	//0x2745 on a restart of instrument = 10053
	// #define WSAECONNABORTED                  10053L

	if (nErrorCode)
		{
		i = 1;
		//s.Format(_T("CServerSocket::OnClose(int nErrorCode= %d)\n"), nErrorCode);
		//theApp.SaveDebugLog(s);
		//TRACE(s);
		TRACE1(("CServerSocket::OnClose(int nErrorCode= %d)\n"), nErrorCode);
		}

	if (m_pSCM == NULL)
		{
		//s.Format(_T("CServerSocket::OnClose(%d) hopelessly lost due to NULL m_pSCM\n"), nErrorCode);
		//theApp.SaveDebugLog(s);
		//TRACE(s);
		TRACE1(("CServerSocket::OnClose(%d) hopelessly lost due to NULL m_pSCM\n"), nErrorCode);
		return;
		}

	if (m_pSCM->m_pstSCM == NULL)
		{
		//s.Format(_T("CServerSocket::OnClose(%d) hopelessly lost due to NULL m_pstSCM\n"), nErrorCode);
		//theApp.SaveDebugLog(s);
		//TRACE(s);
		TRACE1(("CServerSocket::OnClose(%d) hopelessly lost due to NULL m_pstSCM\n"), nErrorCode);
		return;
		}
	// Maximum of 8 instrument at 2016-09-16
	if ( (m_nMyThreadIndex < 0) || (m_nMyThreadIndex > 7) )
		{
		m_nMyThreadIndex = m_nOnAcceptClientIndex;
		if ((m_nMyThreadIndex < 0) || (m_nMyThreadIndex > 7) )
			{
			//s.Format(_T("CServerSocket::OnClose(%d) hopelessly lost due to out of range m_nMyThreadIndex\n"), nErrorCode);
			//theApp.SaveDebugLog(s);
			//TRACE(s);
			TRACE1(("CServerSocket::OnClose(%d) hopelessly lost due to out of range m_nMyThreadIndex\n"), nErrorCode);
			return;
			}
		s.Format(_T("The m_nOnAcceptClientIndex = %d cheat may have worked??\n"), m_nMyThreadIndex);
		TRACE(s);
		}

	if (m_pSCM->m_pstSCM->pClientConnection[m_nMyThreadIndex])
		{
		if (m_pSCM->m_pstSCM->pClientConnection[m_nMyThreadIndex]->pServerSocketOwnerThread)
			{
			// Have race with instrument for linked lists when executing this code
			// ServiceApp is attempting to empty the Received list while ServerSocketOwnerThread in ExitInstance
			// is executing m_pSCC->bConnected = (BYTE) eNotConnected; -- after having deleted the contents of the lists
			// while the App was still working on them
			// while (stSCM[i].pClientConnection[j]->cpRcvPktList->GetCount())
			// First order of business for ServerSocketOwner will be to lock both list until done. 2016-09-28 jeh
			m_pSCC = m_pSCM->m_pstSCM->pClientConnection[m_nMyThreadIndex];
			CServerSocketOwnerThread * pThread = m_pSCC->pServerSocketOwnerThread;
			// wParam = m_nMyThreadIndex , (LPARAM)m_pSCC
			pThread->PostThreadMessage(WM_USER_KILL_OWNER_SOCKET, (WORD)m_nMyThreadIndex, (LPARAM)m_pSCC);
			Sleep(20);

			}
		}
	//else TRACE1("CServerSocket::OnClose(%d) failed to kill ServerSocketOwnerThread Error =%d\n", nErrorCode);
#endif
	Sleep(10);
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
// Create critical sections, linked lists and virtual channels
//
// Get info from global static structure stSCM[nMyServer]
// Clients are discovered when they connect.
int CServerSocket::BuildClientConnectionStructure(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int m_nClientPortIndex)
	{
	CString s;
	int i;
	CRITICAL_SECTION  *pTest1;
	CPtrList *pTest2;

	// skip over CStrings and zero the rest of the structure. Assume CString ptr is 4 bytes. 3 strings at beginning
	//memset ( (void *) &pscc->uClientPort, 0, sizeof(ST_SERVERS_CLIENT_CONNECTION)-12);
		
	s.Format(_T("Send%d"), m_nClientPortIndex);
	pscc->szSocketName		= _T("");
	pscc->sClientName		= _T("");
	pscc->sClientIP4		= _T("");			
	pscc->m_nMyThreadIndex	= m_nClientPortIndex;
	pTest1 = pscc->cpCSSendPkt;
	//pscc->cpCSSendPkt		=		//new CRITICAL_SECTION();
	//pscc->cpCSRcvPkt			= new CRITICAL_SECTION();
	//InitializeCriticalSectionAndSpinCount(pscc->cpCSSendPkt,4);
	//InitializeCriticalSectionAndSpinCount(pscc->cpCSRcvPkt,4);
	pTest2 = pscc->cpSendPktList;
	//pscc->cpSendPktList		= new CPtrList(64);
	//pscc->cpRcvPktList		= new CPtrList(64);

	pscc->pSocket					= NULL;		
	pscc->pServerSocketOwnerThread	= NULL;
	pscc->pServerRcvListThread		= NULL;
	pscc->bConnected				= (BYTE) eNotConfigured;
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

	// Defer the creation of virtual channels until we get 
	// Max channels per main bang, and max number of main bangs in the sequence.
	// Max sequence length = j, max channels per sequence = i
	// This code will run when the channel-sequence configuration command runs
	/*
	for ( j = 0; j < MaxSeqLength; j++)
		{
		for ( i = 0; i < MaxChnlPerSeq; i++)
			pscc->pvChannel[j][i] = new CvChannel(m_nClientPortIndex, (i + j*MaxChnlPerSeq));
		}
	for ( i = 0; i < gMaxChnlsPerMainBang; i++)
		{
		//pscc->pvChannel[0][i] =	new CvChannel(m_nClientPortIndex,i);
		}
	*/
	// create threads
	i = sizeof(CvChannel);					// 112
	i = sizeof(CServerSocketOwnerThread);	// 108
	i = sizeof(CServerRcvListThread);		// 272
	i = sizeof(CServerConnectionManagement);// 12
	i = sizeof(CServerListenThread);		// 80


	return 1;
	}

// Kill the ServerSocket Owner Thread and ReceiveList thread
int CServerSocket::KillClientConnectionStructure(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int m_nClientPortIndex)
	{
#if 0
	void *pV;
	CString s;

	// Kill it at the socket level
	if ((m_pSCC != NULL) && (*(int*) &m_pSCC !=  0xfeeefeee))
		{
		m_pSCC->bConnected = (BYTE) eNotConnected;
		Sleep(5);
		LockRcvPktList();
		while ( m_pSCC->cpRcvPktList->GetCount() > 0)
			{
			pV = (void *) m_pSCC->cpRcvPktList->RemoveHead();
			delete pV;
			}
		UnLockRcvPktList();
		delete m_pSCC->cpRcvPktList;		m_pSCC->cpRcvPktList	= NULL;
		delete m_pSCC->cpCSRcvPkt;		m_pSCC->cpCSRcvPkt	= NULL;

		LockSendPktList();
		while ( m_pSCC->cpSendPktList->GetCount() > 0)
			{
			pV = (void *) m_pSCC->cpSendPktList->RemoveHead();
			delete pV;
			}
		UnLockSendPktList();
		delete m_pSCC->cpSendPktList;		m_pSCC->cpSendPktList	= NULL;
		delete m_pSCC->cpCSSendPkt;			m_pSCC->cpCSSendPkt		= NULL;

		}
	// zero ptrs from thread and SCM class and structure
#endif
	return 0;
	}


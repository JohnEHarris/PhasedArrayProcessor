#if 0
// copied from header file-
Author:		JEH
Date:		05-Jun-2012
Purpose:	Collect resource necessary to support TCP/IP Send and Receive operations

Revised:	12-Jun-12 Abandon 'C' worker threads in favor of using a class method for the independent thread.
				Provides for easier integration/sharing of control parameters between the thread with the 
				dialogs/dialog controls and the threads which operate at higher priority to coordiante
				TCP/IP messaging sending and receiving.
			06-Jul-12 Eliminate code added when this did not use CWinThread and CDialog components for managing threads and
				TCP/IP connections. Intent is to make this a base class for all other Connection Management classes.

How it is intended to work:
	This class manages two instances of independent threads. Each thread creates a CDialog class to receive Windows messages via
	SendMessage() and PostMessage() calls. The dialogs (invisible) also implement a timer to check on the connection state of this class
	with whatever server connection is on the other side/machine/system. Also implemented as a class member of this class is a client
	connection class which operates as an Asynchronous socket.
	Received packets from the server on the other end of the socket are captured as they arrive by the OnReceive routine in the 
	ClientSocket class. These packets are immediately queued in a linked list visible to the rest of the application. OnReceive also 
	Posts a message to the main dialog to process the message.
	The reason for two independent threads (one for sending, one for receiving messages) is to allow for different thread
	priorities between the (1) application, (2) the packet receive operation , and (3) the packet send operation. Priorities 
	do not have to be different, but can be to allow tuning of performance. As initially envisioned, the application runs at
	normal priority while the receive thread runs at above normal priority and the send thread runs at below normal priority.
	All packets received go into a receive queue for the connection associated with this class. All packets to be sent go
	into a send queue. Priority adjustments assure that the received packets are caught quickly. They are staged for the
	application to process as required. Any response packets from the application to the server on the other end of the 
	connection are queued and sent when time allows (at lower priority). Any time a packet is queued, whether receiving or
	sending, a message is posted to the next agent that handles the message so that polling of send/receive queues is not required.

	OLD SCHEME
	[CClientConnectionManagement]
	  |--[CClientCommunicationThread] ------- Receiver instance
	  |    |--[CTCPCommunicationDialog] -- Receiver instance <- this class eliminated. Its work is now done by the thread above
	  |    |----|--[CClientSocket] ------- ASyncSocket created by receiver thread but used by both Send and Receive
	  |
	  |--[CClientCommunicationThread] ------- Sender instance
	  |    |--[CTCPCommunicationDialog] -- Sender instance <- this class eliminated. Its work is now done by the thread above

	NEW SCHEME
	[CClientConnectionManagement]
	  |--[CClientCommunicationThread] ------- Receiver instance
	  |    |----|--[CClientSocket] ------- ASyncSocket created by receiver thread but used by both Send and Receive
	  |
	  |--[CClientCommunicationThread] ------- Sender instance


	Information and awarness by the main application of the potentially many client-to-server connections
	is maintained in a global array of structures named 
	ST_CLIENT_CONNECTION_MANAGEMENT stCCM[MAX_CLIENTS];		// a global, static array of CCM structs

	Each CClientConnectionManagement instance contains a pointer to a specific instance of the above global static array.
	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;	// pointer to my global structure instance 

	In addition, there is a global static array of pointer to all the potential instances of the CClientConnectionManagement class
	PubExt1 CClientConnectionManagement *pCCM[MAX_CLIENTS];	
	// global, static ptrs to class instances define outside of the class definition.

	The declaration of these two global structures is done within the ClientConnectionManagement.h file in such a way
		that inclusion of this header file by the main application / dialog is all that is necessary to connect the application
		with this Client Connection Management subsystem.

#endif



#include "stdafx.h"
#define I_AM_CCM

// I_AM_PAP is defined in the PAM project under C++ | Preprocessor Definitions 
#ifdef I_AM_PAP
#include "stdafx.h"
#include "PA2WinDlg.h"
//#include "ClientConnectionManagement.h"

#else
// build/run the legacy Truscan MMI
#include "PA2WinDlg.h"
//#include "Truscan.h"
//#include "TscanDlg.h"
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"

//extern THE_APP_CLASS theApp;
#endif

//#include "ClientSocket.h"
//#include "ClientConnectionManagement.h"
//#include "ClientCommunicationThread.h"
//#include "CmdProcessThread.h"

class CClientConnectionManagement;
class CClientConnectionManagement *pCCM[MAX_CLIENTS];	// global, static ptrs to class instances defined outside of the class definition.


ST_CLIENT_CONNECTION_MANAGEMENT stCCM[MAX_CLIENTS];		// a global, static array of CCM structs
	// Here we could initialize the structure for the clients we expect.
	// Thus IP addresses/url's could be entered as initialization constants
	// Could also use an ini file and fill this structure before creating the 
	// class instances pCCM[]
	//
	// a global, static array of ST_SOCKET_NAMES structs

// nMyConnection selects which one of MAX_CLIENTS connections we are managing with this instance
// wOriginator selects family type from Constants, eg. TRUSCOPE_COMMAND_PROCESSOR - Only for PAG use
CClientConnectionManagement::CClientConnectionManagement(int nMyConnection, USHORT wOriginator)
	{
	CString s;
	int i = 0;
	m_pstCCM = NULL;
	m_nMyConnection = -1;
	//memset((void *) m_RcvBuf,0,sizeof(m_RcvBuf)); THIS STUFF repalce by m_pFifo on 2016-11-03 jeh

	szName			= _T("CCM-Skt ");

	if (nMyConnection < 0)
		{
		TRACE(_T("Connection number is negative\n"));
		return;
		}
		
	if (nMyConnection >= MAX_CLIENTS)
		{
		TRACE(_T("Connection number too big\n"));
		return;
		}
	m_nMyConnection = nMyConnection;

	m_pstCCM = &stCCM[m_nMyConnection];	// my particular structure. stCCM is a static, global structure, not part of the class.

	m_pstCCM->pSocket = NULL;
	m_pstCCM->uBytesReceived			= 0;
	m_pstCCM->uDuplicateReceivedPackets = 0;
	m_pstCCM->uIdataAcksSent			= 0;
	m_pstCCM->uInvalidPacketReceived	= 0;
	m_pstCCM->uLostReceivedPackets		= 0;
	m_pstCCM->uMaxPacketReceived		= 0;
	m_pstCCM->uPacketsReceived			= 0;
	m_pstCCM->bConnected				= 0;
	m_pstCCM->uBytesSent				= 0;
	m_pstCCM->uPacketsSent				= 0;
	m_pstCCM->nOnConnectError			= WSAECONNREFUSED;

	// create critical sections, linked lists and events
	m_pstCCM->pCSRcvPkt = new CRITICAL_SECTION();
	m_pstCCM->pCSSendPkt = new CRITICAL_SECTION();
	m_pstCCM->pCSDebugIn = new CRITICAL_SECTION();
	m_pstCCM->pCSDebugOut = new CRITICAL_SECTION();

	InitializeCriticalSectionAndSpinCount(m_pstCCM->pCSRcvPkt,4);
	InitializeCriticalSectionAndSpinCount(m_pstCCM->pCSSendPkt,4);
	InitializeCriticalSectionAndSpinCount(m_pstCCM->pCSDebugIn,4);
	InitializeCriticalSectionAndSpinCount(m_pstCCM->pCSDebugOut,4);

	m_pstCCM->pRcvPktPacketList		= new CPtrList(64);	// our input inspection data goes here
	m_pstCCM->pSendPktList			= new CPtrList(64);	// commands from somebody go here
	m_pstCCM->pInDebugMessageList	= new CPtrList(64);
	m_pstCCM->pOutDebugMessageList	= new CPtrList(64);


	m_pstCCM->uLastTick		= 0;


	m_pstCCM->pCCM = this;		// the entire class has a this ptr, but static functions members do not.
								// when we are in a static function, must access particular instances of other non-static
								// member function with m_pstCCM->pCCM

	// Maybe CLIENT_IDENTITY_DETAIL will only be used by CCM_SysCp class

	// if (NULL == m_pstCCM)	return;		//fatal flaw

//	m_pstCCM->m_pFifo = new CCmdFifo(1460);		// FIFO control for receiving cmd packets from PAG	 was 1454
//	m_pstCCM->m_pFifo->m_nOwningThreadId = AfxGetThread()->m_nThreadID;
//	strcpy(m_pstCCM->m_pFifo->tag, "CCM180 ");	
	}


CClientConnectionManagement::~CClientConnectionManagement(void)
	{
	void *pV = 0;
	int i, n = 0;

	// if the socket is not closed, close it and terminate the threads
	if (m_pstCCM->pSocket)
		{
		if (m_pstCCM->pSocket->ShutDown(2))
			{
			m_pstCCM->pSocket->Close();
			}
		}


	//
	if (m_pstCCM->pReceiveThread)
		{

		// Receive thread closes and destroys socket
		m_pstCCM->pReceiveThread->PostThreadMessage(WM_QUIT,0,0l);
		for ( i = 0; i < 100; i++)	// wait a little while for listener to go away
			{
			if (m_pstCCM->pReceiveThread == NULL)		break;
			Sleep(10);
			}
		if ( i == 100)
			{
			TRACE(_T("pReceiveThread Exit routine didn't run or didn't NULL pReceiveThread\n"));
			}
		}

	if (m_pstCCM->pSendThread)
		{
		// Receive thread closes and destroys socket
		m_pstCCM->pSendThread->PostThreadMessage(WM_QUIT,0,0l);
		for ( i = 0; i < 100; i++)	// wait a little while for listener to go away
			{
			if (m_pstCCM->pSendThread == NULL)		break;
			Sleep(10);
			}
		if ( i == 100)
			{
			TRACE(_T("pSendThread Exit routine didn't run or didn't NULL pSendThread\n"));
			}		
		}
#if 0
	if (m_pstCCM->pCmdProcessThread)
		{
		m_pstCCM->pCmdProcessThread->PostThreadMessage(WM_QUIT,0,0l);
		for ( i = 0; i < 100; i++)	// wait a little while for listener to go away
			{
			if (m_pstCCM->pCmdProcessThread == NULL)		break;
			Sleep(10);
			}
		if ( i == 100)
			{
			TRACE(_T("pCmdProcessThread Exit routine didn't run or didn't NULL pCmdProcessThread\n"));
			}		
		}
#endif

#if 0
	if (m_pstCCM->hReceiveDlg)
		::SendMessage(m_pstCCM->hReceiveDlg,	WM_USER_KILL_COM_DLG,0,0);
	Sleep(10);
	if (m_pstCCM->hSendDlg)
		::SendMessage(m_pstCCM->hSendDlg,		WM_USER_KILL_COM_DLG,0,0);
#endif
	Sleep(10);
	/******************/
	if (0 == KillLinkedList( m_pstCCM->pCSRcvPkt, m_pstCCM->pRcvPktPacketList ))
		TRACE( _T( "Failed to kill Receive List\n" ) );
	else {		m_pstCCM->pCSRcvPkt = 0;  m_pstCCM->pRcvPktPacketList = 0;		}

	if ( 0 == KillLinkedList( m_pstCCM->pCSSendPkt, m_pstCCM->pSendPktList ))
		TRACE( _T( "Failed to kill Send List\n" ) );
	else {		m_pstCCM->pCSSendPkt = 0;  m_pstCCM->pSendPktList = 0;		}

	if ( 0 == KillLinkedList( m_pstCCM->pCSDebugIn, m_pstCCM->pInDebugMessageList ))
		TRACE( _T( "Failed to kill Debug In List\n" ) );
	else {		m_pstCCM->pCSDebugIn = 0;  m_pstCCM->pInDebugMessageList = 0;		}

	if ( 0 == KillLinkedList( m_pstCCM->pCSDebugOut, m_pstCCM->pOutDebugMessageList ))
		TRACE( _T( "Failed to kill Debug Out List\n" ) );
	else {		m_pstCCM->pCSDebugOut = 0;  m_pstCCM->pOutDebugMessageList = 0;		}

	}

// Begin the Receive Thread
void CClientConnectionManagement::CreateReceiveThread(void)
	{
	CString s;
	BOOL bAutoDelete;
	m_pstCCM->pReceiveThread = (CClientCommunicationThread *) AfxBeginThread(
										RUNTIME_CLASS(CClientCommunicationThread),
										m_pstCCM->nReceivePriority,
										0,	// stack size
										CREATE_SUSPENDED,	// create flag, run on start
										NULL);	// security ptr
	s.Format(_T("ReceiverThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n"), m_pstCCM->pReceiveThread, 
		m_pstCCM->pReceiveThread->m_hThread, m_pstCCM->pReceiveThread->m_nThreadID);

	TRACE(s);
	bAutoDelete = m_pstCCM->pReceiveThread->m_bAutoDelete; // autodelete is default
	m_pstCCM->pReceiveThread->m_bAutoDelete = true;
	m_pstCCM->pReceiveThread->m_nMyRole = 1;	// receiver
	m_pstCCM->pReceiveThread->ResumeThread();
	}

// Send a thread message to configure the receive thread
void CClientConnectionManagement::InitReceiveThread(void)
	{
	// receiver role is 1, send role is 2. Passed thru wParam
	// Thread message is serviced by CClientCommunicationThread::InitTcpThread(WPARAM w, LPARAM lParam)
	m_pstCCM->pReceiveThread->PostThreadMessage(WM_USER_INIT_TCP_THREAD, (WORD) 1, (LPARAM) this);
	}

void CClientConnectionManagement::KillReceiveThread(void)
	{
	int i = 0;
	if (m_pstCCM == NULL)	return;
	if (m_pstCCM->pReceiveThread == 0)	return;
	// receiver role is 1, send role is 2. Passed thru wParam
	// Thread message is serviced by CClientCommunicationThread::KillReceiveThread(WPARAM w, LPARAM lParam)
	m_pstCCM->pReceiveThread->PostThreadMessage(WM_USER_KILL_RECV_THREAD, (WORD) 1, (LPARAM) this);
	while ( (m_pstCCM->pReceiveThread ) && i < 5)
		{
		Sleep(10);	 i++;
		}
	// Quit could shut down sender instead of receiver
	//	m_pstCCM->pReceiveThread->PostThreadMessage(WM_QUIT,0,0);
	Sleep(10);
	}

// Sends tcp/ip messages when nothing else to do. Lower priority than rest of dialog/class
// Since a member of the class, there is a new instance of this thread each time the class is instantiated.
// Do not call this method directly from any other method of the class since it is another thread and
// only returns when the thread is closed.
// lpParam is used to pass in which instance of the class pointer is being used.
void CClientConnectionManagement::CreateSendThread(void)
	{
	m_pstCCM->pSendThread = (CClientCommunicationThread *) AfxBeginThread(
										RUNTIME_CLASS(CClientCommunicationThread),
										m_pstCCM->nSendPriority,
										0,	// stack size
										0,	// create flag, run on start
										NULL);	// security ptr
	TRACE3("SenderThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n", m_pstCCM->pSendThread, 
		m_pstCCM->pSendThread->m_hThread, m_pstCCM->pSendThread->m_nThreadID);
	}

// Send a thread message to configure the send thread
void CClientConnectionManagement::InitSendThread(void)
	{
	// receiver role is 1, send role is 2
	m_pstCCM->pSendThread->PostThreadMessage(WM_USER_INIT_TCP_THREAD, (WORD) 2, (LPARAM) this);
	}

void CClientConnectionManagement::KillSendThread(void)
	{
	// receiver role is 1, send role is 2
	int i = 0;
	if (m_pstCCM == NULL)	return;
	if (m_pstCCM->pSendThread == 0)	return;
	m_pstCCM->pSendThread->PostThreadMessage(WM_USER_KILL_SEND_THREAD, (WORD) 2, (LPARAM) this);
	while ( (m_pstCCM->pSendThread ) && i < 50)
		{
		Sleep(10);	 i++;
		}
	//m_pstCCM->pSendThread->PostThreadMessage(WM_QUIT,0,0);
	Sleep(10);	
	}


// Optional in PAG, Needed in PAM
void CClientConnectionManagement::CreateCmdProcessThread(void)
	{
	m_pstCCM->pCmdProcessThread = (CCmdProcessThread *) AfxBeginThread(
										RUNTIME_CLASS (CCmdProcessThread),
										m_pstCCM->nCmdProcessPriority,
										0,	// stack size
										CREATE_SUSPENDED,	// create flag, do not run on start
										NULL);	// security ptr
//	TRACE3(_T("SenderThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n"), m_pstCCM->pCmdProcessThread, 
//		m_pstCCM->pCmdProcessThread->m_hThread, m_pstCCM->pCmdProcessThread->m_nThreadID);
	if (NULL == m_pstCCM->pCmdProcessThread)
		{
		ASSERT(0);
		}
	else
		{
		m_pstCCM->pCmdProcessThread->m_pstCCM = m_pstCCM;
		m_pstCCM->pCmdProcessThread->m_pMyCCM = m_pstCCM->pCCM;
		m_pstCCM->pCmdProcessThread->ResumeThread();
		TRACE(_T("CmdProcessThread is running\n"));
		}

	}

void CClientConnectionManagement::KillCmdProcessThread(void)
	{
	int i = 0;
	if (m_pstCCM == NULL)	return;
	if (m_pstCCM->pCmdProcessThread == NULL) return;
	m_pstCCM->pCmdProcessThread->PostThreadMessage(WM_QUIT,0,0L);
	while ( (m_pstCCM->pCmdProcessThread ) && i < 50)
		{
		Sleep(10);	i++;
		}
	}


// In the windows version, the main dialog OnTimer sends ticks to the CCM receive threads
// to check for connectivity to the server
void CClientConnectionManagement::TimerTick(WORD wTargetSystem)
	{
	int i;
	if ( NULL == m_pstCCM)					return;
	i = pCCM_PAG->GetConnectionState();
	if (i == 0)
		{
		if (NULL == m_pstCCM->pReceiveThread)
			{	
			pCCM_PAG->CreateReceiveThread();		Sleep( 50 );
			pCCM_PAG->InitReceiveThread();			Sleep( 50 );
			}
		if (NULL == m_pstCCM->pSendThread)
			{	
			pCCM_PAG->CreateSendThread();			Sleep( 50 );
			pCCM_PAG->InitSendThread();				Sleep( 50 );
			}
		if (NULL == m_pstCCM->pCmdProcessThread)
			{	pCCM_PAG->CreateCmdProcessThread();		Sleep( 50 );	}
		}

	if (NULL == m_pstCCM->pReceiveThread)
		{
		TRACE( _T( "m_pstCCM->pReceiveThread is null\n" ) );
		return;
		}

	// This message processed by CClientCommunicationThread::OnTimer
	m_pstCCM->pReceiveThread->PostThreadMessage(WM_USER_TIMER_TICK, (WORD) wTargetSystem, (LPARAM) 0);
	}

void CClientConnectionManagement::SetSocketNameString(CString s)
{	if (m_pstCCM)	m_pstCCM->szSocketName = s;	}


// Queue a packet into the send pkt list and post a message to TCP Com dlg to have it transmitted
// First 4 bytes of packet are number of bytes to send. Total tranmit length is packet length.
// Caller must allocate space for packet to be sent and pass ptr as pB.
// If the callers pointer is to an element on the callers stack, set nDeleteFlag to zero
// since we can't/shouldn't delete something on the callers stack.
// This routine deletes the memory ptr passed in if nDeleteFlag is non-zero

void CClientConnectionManagement::SendPacket(BYTE *pB, int nBytes, int nDeleteFlag)
	{
	if (NULL == pB)				return;	
	if (NULL == m_pstCCM)		return;
	if (NULL == m_pstCCM->pSendThread)		return;
	if (nBytes < 0)	
		{		
		if (nDeleteFlag) delete pB;		
		return;		
		}

	int i;
	stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[nBytes+sizeof(int)];	// space for packet + pkt length variable
	// debug pBuf = 0x39b7910
	memcpy((void *) &pBuf->Msg[0], (void *) pB, nBytes);
//	*(int *) pBuf = nBytes;			// 1st 4 bytes are packet length
	pBuf->nLength = nBytes;			// 1st 4 bytes are packet length
	LockSendPktList();
	AddTailSendPkt(pBuf);
	i = m_pstCCM->pSendPktList->GetCount();
	UnLockSendPktList();
	if (nDeleteFlag) delete pB;
	// By posting a windows message, the transmit routine will run at its created thread priority
	// If we called the routine directly from here, it would run at whatever priority this function's thread has.
	//::PostMessage(m_pstCCM->hSendDlg, WM_USER_SEND_TCPIP_PACKET,0,0);
	// The routine which responds to the message is CClientCommunicationThread::TransmitPackets(WPARAM, LPARAM)
	// This thread actually uses the clients socket to send the queued data to the server. w = 2
	m_pstCCM->pSendThread->PostThreadMessage(WM_USER_SEND_TCPIP_PACKET,2,i);

	}


// Put debug messages into debug out linked list
void CClientConnectionManagement::DebugOut(CString s)
{
#if 0
// this won't work.. causes memory leak on program exit
	CString *s1 = new CString();
	*s1 = s + _T("\n");
	LockDebugOut();
	m_pstCCM->pOutDebugMessageList->AddTail(s1);		//AddTailDebugOut(*s1); crashes and burns
	UnLockDebugOut();
#endif
	// new attempt.. convert string to tchar and store in linked list
	CString s1 = s + _T("\n");	// s1 on stack

	TRACE(s1);
}


// Call this function from the ClientSocket class whenever data is received asynchronously
// This way changes to messages do not have to be implemented in CClientSocket class
//
#define MAX_MSG_BYTES		4096


// If the socket closes, let the client manager decide if thread restarts are in order or if the app is closing
// Called when the client socket closes by the client socket
void CClientConnectionManagement::OnSocketClose(int nErrorCode)
{
	// check thread states and reason for socket closing
	CString s;
	s.Format(_T("Socket for CCM[%d] has closed because 0x%08x\n"), m_nMyConnection,nErrorCode);
	TRACE(s);
}

//===============================================================================

// Several function to handle the different received message formats

// Consider that some messages may be in error and are thus unknown
void CClientConnectionManagement::UnknownRcvdPacket(void *pV)
{
	GENERIC_MSG_HEADER *pG = (GENERIC_MSG_HEADER *) pV;
	int nLen = pG->MessageLength;
	int nId  = pG->MessageID;
	// if we knew what it was we would put it into the received packet queue, but since we don't
	// we'll just send a debug msg and delete the packet
	CString s;
	s.Format(_T("Unknown Message, ID=%d, Length=%d\n"), nId,nLen);
	DebugOut(s);
	// DebugInMessage( s ); send to debug output
	delete pV;
}
	// Collect received data into expected packet lengths. That is
	// reconstruct packet from received data.  Its a feature of TCPIP.
	// nMsgSize is amount received by hardware. Can be greater or less than desired packet size

// The Async socket has received a/many packet(s). Rather than handling the packet
// in the socket driver, let the next management level up store the packet into
// the linked list and then later process the list at the priority level of the
// CCM class or the main dlg - which should be normal priority.
// This routine is typically called by CClientSocket::OnReceive(). Hence it 
// runs at the callers priority.  All this calling back and forth
// is done to allow different thread priorities for 
// Sending/Receiving/Running the GUI -- if so desired
//


void CClientConnectionManagement::OnReceive(CClientSocket *pSocket)
	{

	int nWholePacketQty = 0;

	BYTE *pB;	// debug
	void *pPacket = 0;
	int nPacketSize;
	WORD wByteCnt;
	int n;
	CString s,t;

	// A real hardware FIFO would shift data to the output side instantly
	pSocket->m_pFifo->Shift();
	BYTE *pCmd = pSocket->m_pFifo->GetInLoc();
	// Receive() receives data into fifo memory pointed to by pCmd
	n = pSocket->Receive( (void *) pCmd, 0x2000, 0 );	// ask for 8k byte into 16k buffer
	//PAM assumes we will get partial packets and have to extract whole packets
	if ( n > 0)
		{
		pSocket->m_pFifo->AddBytesToFifo(n);
#if	DEBUG_TCPIP_FROM_PAG
			{
			s.Format(_T("CCM OnReceive got %d bytes"), n);
			DebugOut(s);
			}
#endif
		nPacketSize = pSocket->m_pFifo->GetPacketSize();	


		
		while (1)
			{	// get packets

			wByteCnt = pSocket->m_pFifo->GetFIFOBytes();
			if (wByteCnt < sizeof(GenericPacketHeader))
				{
				break;
				}
			nPacketSize = pSocket->m_pFifo->GetPacketSize();
			if ((nPacketSize <= 0) || (wByteCnt < nPacketSize))
				{
				break;
				}

			pPacket = (GenericPacketHeader*) pSocket->m_pFifo->GetNextPacket();
			if (pPacket == NULL)
				{
				//CAsyncSocket::OnReceive(nErrorCode);
				return;
				}

			pB =  new BYTE[nPacketSize];

			memcpy( (void *) pB, (void *) pPacket, nPacketSize);	// move all data to the new buffer
			// put it in the linked list and let someone else decipher it
			LockRcvPktList();
			AddTailRcvPkt(pB);	// put the buffer into the recd data linked list
			nWholePacketQty++;
			UnLockRcvPktList();
			if (m_pstCCM)
				{
				m_pstCCM->uBytesReceived += nPacketSize;
				m_pstCCM->uPacketsReceived++;
				if (m_pstCCM->uMaxPacketReceived < (unsigned) n)	
					m_pstCCM->uMaxPacketReceived = n;	// capture size of maximum packet/
				// This messages causes void CCmdProcessThread::ProcessReceivedMessage() to execute
				// m_pstCCM->pCmdProcessThread->PostThreadMessageA(WM_USER_CLIENT_PKT_RECEIVED, 0,0L);
				}

			} 	// get packets
		//m_pFifo->Reset();	// if FIFO is empty will reset pointer to start of memory

		if (( nWholePacketQty) && (m_pstCCM))
			// This messages causes void CCmdProcessThread::ProcessReceivedMessage() to execute
			//m_pstCCM->pCmdProcessThread->PostThreadMessageA(WM_USER_CLIENT_PKT_RECEIVED, 0, 0L);
			m_pstCCM->pCmdProcessThread->PostThreadMessage(WM_USER_CLIENT_PKT_RECEIVED, 0, 0L);
		}	// n > 0


	}	// OnReceive(CClientSocket *pSocket)

// We finally get around to processing the message.
// Liberally stolen from Randy's example
// This routine is typically called from the Main Dlg and runs at the priority level of the caller
// the PostMessage(WM_USER_CLIENT_PKT_RECEIVED, (WPARAM) m_nMyConnection,0) lets the main dlg know which CCM instance to call.
//
// >>>>>   For the SysCp client, this routtine is not called <<<<<
//
// but the ProcessReceiveMessage routine in the subclass CCM_SysCp is.

#ifdef I_AM_PAP
// Phase Array Master = Phased Array Processor
// PAP is a client of the GUI. What we would be getting is commnands to send to the instrument
// But in reality what we really do in this case is put the commands into a linked list
// on the server side since the PAP only passes commands thru except for the NcNx command
//
void CClientConnectionManagement::ProcessReceivedMessage(void)
	{
	USES_CONVERSION;

	ASSERT(m_pstCCM);
	if (NULL == m_pstCCM)	return;		// something wrong here
	if (m_pstCCM->pRcvPktPacketList->IsEmpty())		return;	// shouldn't have been called, but no harm

	ST_LARGE_CMD * Cmd = NULL;					
	// Access the linked list and process all msgs there
	while (m_pstCCM->pRcvPktPacketList->GetCount() > 0)
		{
		TRACE("Looping in base class CCM ProcessReceivedMessage\n");
		LockRcvPktList();
		Cmd = (ST_LARGE_CMD *) m_pstCCM->pRcvPktPacketList->RemoveHead();
		delete Cmd;	// just empty linked list for now - 15-Jan-2013
		UnLockRcvPktList();

		}	// while (m_pstCCM->pRcvPktPacketList->GetCount())

	}	// ProcessReceivedMessage(void)

#else
// Phased Array GUI  -- PAG
void CClientConnectionManagement::ProcessReceivedMessage(void)
	{
	USES_CONVERSION;
	CString s;
	s.Format(_T("CCM:ProcessReceivedMessage[%d] called\n"), m_nMyConnection);
	TRACE(s);
	}

#endif


// Stolen from Yiqin - ACP 13-Jul-12
// rah 26-Sept-2012 fixed this so it actually works
#ifndef I_AM_PAP
CString CClientConnectionManagement::RetrieveKeyValue(CString sTargetKey)
{
#if 0
	// grab a key value from VS_VERSION_INFO
	// and display it.  if key value is not
	// present, display "unavailable"

	BOOL		fKeyValueFound;
	HANDLE		hMem;
	LPVOID		lpvMem;
    LPTSTR		lszVer;
    CString     lszKeyData;

	CString s;
	TCHAR	szFullPath[256];
	TCHAR	szGetName[256];

    DWORD	dwVerHnd;
	DWORD	dwVerInfoSize;

	UINT    cchVer;
	int		cchRoot;

	// fetch the name of this module
    GetModuleFileName(NULL, szFullPath, sizeof(szFullPath));

	// see if there is anything in the file version block
	//dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);

	// if there is, fetch the key value we want to display
	if (0)		// (dwVerInfoSize)
	{
       cchVer = 0;
	   lszVer = NULL;

       // form the total key value to search for
	   hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
	   lpvMem = GlobalLock(hMem);
	   //GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);
	   CString sL = _T("\\StringFileInfo\\040904b0\\");
	   int iL = sL.GetLength() + sizeof(TCHAR);
       _tcscpy_s(szGetName, iL, sL);
	   lstrcat(szGetName, sTargetKey);

	   cchRoot = lstrlen(szGetName);

	   //fKeyValueFound = VerQueryValue(lpvMem, szGetName, (void **) &lszVer, &cchVer);
       if (fKeyValueFound)
	   {
          lszKeyData = lszVer;
	   } else {
//			s.LoadString(theApp.m_hLanguage, IDS_UNKNOWN );
		  lszKeyData.Format(_T("%s\n"), s );
	   }

	   GlobalUnlock(hMem);
	   GlobalFree(hMem);
	} else {
//		s.LoadString(theApp.m_hLanguage, IDS_NO_VERSION );
	   lszKeyData.Format(_T("%s\n"), s );
	}
	
	return (lszKeyData);
#endif
	return(_T( "FIX LATER" ));
}

#endif

void CClientConnectionManagement::SetClientIp(CString s)
	{
	if (m_pstCCM == NULL)	return;
	//if (m_pstCCM->pClientIdentity) _tcscpy_s(m_pstCCM->pClientIdentity->ClientIP, MAX_CID_IP_CHAR_LENGTH, s);
	m_pstCCM->sClientIP4 = s;
	}

void CClientConnectionManagement::SetServerIp(CString s)
	{
	if (m_pstCCM == NULL)	return;
	//if (m_pstCCM->pClientIdentity) _tcscpy_s(m_pstCCM->pClientIdentity->ServerIP, MAX_CID_IP_CHAR_LENGTH, s);
	m_pstCCM->sServerIP4 = s;	
	}



#undef I_AM_CCM

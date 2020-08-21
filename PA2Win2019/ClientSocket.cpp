// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
// I_AM_PAP is defined in the PAM project under C++ | Preprocessor Definitions 


#ifdef I_AM_PAP
#include "PA2WinDlg.h"
#else
//#include "Truscan.h"
//#include "TscanDlg.h"
#include "PA2WinDlg.h"
//#include "CCM_SysCp.h"		// jeh 05-Jun-12
//extern THE_APP_CLASS theApp;
#endif

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"		// jeh 05-Jun-12
#include "time.h"
#include "CCmdFifo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern char *GetTimeStringPtr(void);

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

IMPLEMENT_DYNAMIC(CClientSocket, CAsyncSocket);

// New constructor for ClientConnectionManagment
// Pass a ptr to the CCM class instance managing this socket/connection
CClientSocket::CClientSocket( CClientConnectionManagement *pCCM	)
	{
	CString s;
	if (pCCM)	
		{
		m_pCCM = pCCM;
		m_nChooseYourOnReceive = 1;	// new constructor gets new OnReceive jeh
		//m_pMainDlg = (CMC_SysCPTestClientDlg *) theApp.m_pMainWnd;
		// this is a copy of the socket pointer in ClientCommunicationThread
		if (m_pCCM->m_pstCCM)
			m_pCCM->m_pstCCM->pSocket = this;
		m_nAsyncSocketCnt = gnAsyncSocketCnt++;
		m_nOwningThreadId = AfxGetThread()->m_nThreadID;
		s.Format(_T("Valid CCM ptr, use OnReceive1(), Socket# =%d, CreateThread = %d\n"),
			m_nAsyncSocketCnt, m_nOwningThreadId);
		TRACE(s);
		m_pFifo = 0;
		m_pElapseTimer = 0;
		}
	else
		{
		TRACE(_T("InValid CCM ptr, Houston, we have a problem\n"));
		;
		}
		//DebugInMessage( "New CClientSocket." );
	}


CClientSocket::~CClientSocket()
	{
	CString s,t;
	int i;
	if (NULL == m_pCCM)				return;
	if (NULL == m_pCCM->m_pstCCM)	return;
	s.Format(_T("~CClientSocket Socket# =%d, CreateThread = %d SocketName = %s\n"),
				m_nAsyncSocketCnt, m_nOwningThreadId, m_pCCM->m_pstCCM->szSocketName);
	TRACE(s);
	// if the socket exists and was at one time connected then m_pFifo and
	// m_pElapseTimer are likely valid
	if ( (m_pCCM->m_pstCCM->pSocket ) ) // && m_pCCM->GetConnectionState())
		{
		if (m_pFifo != NULL)
			{
			s.Format( _T( "~CClientSocket Fifo cnt=%d,  ThreadID=%d\n" ),
				m_pFifo->m_nFifoCnt, m_pFifo->m_nOwningThreadId );
			TRACE( s );
			delete m_pFifo;
			m_pFifo = 0;
			}
		if (m_pCCM->m_pstCCM->pSocket->m_pElapseTimer != NULL)
			{
			strcat( m_pElapseTimer->tag, "CClientSocket89 " );
			t = m_pElapseTimer->tag;
			s.Format( _T( "~CClientSocket ElapseTime %s\n" ), t );
			TRACE( s );
			delete m_pCCM->m_pstCCM->pSocket->m_pElapseTimer;
			m_pCCM->m_pstCCM->pSocket->m_pElapseTimer = 0;
			}
		}
//	m_pCCM->KillSendThread();
//	m_pCCM->KillReceiveThread();
//	m_pCCM->SetConnectionState(0);
	i = (int)m_pCCM->m_pstCCM->pSocket->m_hSocket;
	if (i > 0)
		{
		m_pCCM->m_pstCCM->pSocket->Close(); // necessary or else KillReceiverThread does not run
		CAsyncSocket::Close();
		m_pCCM->m_pstCCM->bConnected = 0;
		Sleep( 10 );
		}
	m_pCCM->m_pstCCM->pSocket = 0;
	m_pCCM->m_pstCCM->bConnected = 0; //4/24/18
	}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions
void CClientSocket::DebugInMessage(CString s)
{
#if 0
	DEBUG_MESSAGE * msg = new DEBUG_MESSAGE;
	msg->s.Format( _T("%s"), s );
	if (0 == m_nChooseYourOnReceive)
	{
		m_psemDataIn->Lock();
		m_pListDebugMsgIn->AddTail( msg );
		m_psemDataIn->Unlock();
	}
#endif
}

// When integrated with Randys SysCpTest Client, make it easy on ourselves.. just map local debug to main dlg debug
// Problem, need critical section since many threads now using same routine.
void CClientSocket::DebugOutMessage(CString s)
{
#if 0
	//DEBUG_MESSAGE * msg = new DEBUG_MESSAGE;
	//msg->s.Format( _T("%s"), s );
	// make access to the main dialog
	extern CMC_SysCPTestClientApp theApp;

	MAIN_DLG_NAME * pDlg;
    pDlg = (MAIN_DLG_NAME *) theApp.m_pMainWnd;
	pDlg->AddDebugMessage(s);
#endif
	CString s1 = _T("\nCClientSocket..");
	s1 += s + _T("\n");
	TRACE(s1);
#ifdef I_AM_PAP
	pMainDlg->SaveDebugLog(s1);
#endif

}

#if 0
// This is the legacy OnReceive. Selected by using the legacy constructor
// as used in ACP, the async feature is switched off supposedly. It is. Never breaks when PT sim sending data
void CClientSocket::OnReceive0(int nErrorCode) 
{
//	TCPDUMMY * Data = new TCPDUMMY;
	int n = nErrorCode;	// means nothing, kills compiler warning
	CString s;
}

// This is added to let the class ClientConnectionManagement manage sending and receiving - jeh 6/6/12
// OnReceive1 uses the socket as an Async socket. The code in this class creates memory for the packet
// and adds it to the linked list WHENEVER packets arrive.
// Since this class was created by the ReceiveThread() which had a priority THREAD_PRIORITY_ABOVE_NORMAL
// we are counting on it to get quick service and not block for the critcial section lock/unlock calls.
// The rest of our GUI code should be running at THREAD_PRIORITY_NORMAL so it should yield when this call [OnReceive] occurs.
// Can't do anything about what the rest of Windows is doing regarding process/thread priorities.
//
// Another thought, could have the ClientSocket class call the CCM class and let the receive operation occur totaly in CCM
// then CClientSocket would not change from application to applicaton.
//

void CClientSocket::OnReceive1(int nErrorCode) 
	{
	CString s;
	// Handle the reception of the packet in the CCM class. Avoiding changing this class with every application
	if (nErrorCode)
		{
		s.Format(_T("OnReceive%d = Error %d\n"), m_pCCM->m_nMyConnection, nErrorCode);
		TRACE(s);
		}

	m_pCCM->OnReceive(this);	// at the priority of this thread, call OnReceive in CCM class

	}

// Override the base class OnReceive method
// A method to allow mixture of Yanmings original code with this code

#endif

// Clean up the code w/o 2 OnReceive methods
void CClientSocket::OnReceive(int nErrorCode) 
	{
	CString s;
	// Handle the reception of the packet in the CCM class. Avoiding changing this class with every application
	if (nErrorCode)
		{
		s.Format(_T("OnReceive%d = Error %d\n"), m_pCCM->m_nMyConnection, nErrorCode);
		TRACE(s);
		}

	m_pCCM->OnReceive(this);	// at the priority of this thread, call OnReceive in CCM class

// Another thought, could have the ClientSocket class call the CCM class and let the receive operation occur totaly in CCM
// then CClientSocket would not change from application to applicaton.

	}

void CClientSocket::OnAccept(int nErrorCode) 
	{
	CAsyncSocket::OnAccept(nErrorCode);
	}

void CClientSocket::OnClose(int nErrorCode) 
	{
	int i;
	CString s;
	CAsyncSocket::OnClose(nErrorCode);

	if (m_pCCM)
		{
		m_pCCM->SetConnectionState(0);
		if (m_pCCM->m_pstCCM)
			{
			if (m_pCCM->m_pstCCM->pSocket)
				{
				i = m_pCCM->m_pstCCM->pSocket->m_hSocket;
				if ( nShutDown == 0)
					//m_pCCM->m_pstCCM->pSocket->Close( );
				i = (int) m_pCCM->m_pstCCM->pSocket;
				delete  m_pCCM->m_pstCCM->pSocket;
				}
			}

		}
	}

// Add this for an easy way to let PAG have a pop-up message box but PAM output to the dark scree if in debug mode
void CClientSocket::MyMessageBox(CString s)
	{
#ifdef I_AM_PAP
	DebugOutMessage(s);
#else
	AfxMessageBox(s);
#endif
	}

// copied from the Microsoft example code for OnConnect
void CClientSocket::OnConnect(int nErrorCode)   // CClientSocket is derived from CAsyncSocket
	{
	CString s, s0, s1, s2;
	char txt[64] ={ 0 };
	UINT uSPort, uCPort;	// temp to hold port numbers discovered
	if (m_pCCM)
		{
		s.Format(_T("Client Connection %d = %s"),m_pCCM->m_nMyConnection, m_pCCM->szName);
		}
	else
		{
		s = _T("Client number is unkown..");
		}
	TRACE(s);
	if (0 != nErrorCode)
		{
		switch(nErrorCode)
			{
		case	WSAEWOULDBLOCK:
			MyMessageBox(_T("Socket would block. Line 283\n"));
			break;
		case WSAEADDRINUSE: 
			MyMessageBox(_T("The specified address is already in use.\n"));
			break;
		case WSAEADDRNOTAVAIL: 
			MyMessageBox(_T("The specified address is not available from ")
			_T("the local machine.\n"));
			break;
		case WSAEAFNOSUPPORT: 
			MyMessageBox(_T("Addresses in the specified family cannot be ")
			_T("used with this socket.\n"));
			break;
		case WSAECONNREFUSED: 
			//MyMessageBox(_T("The attempt to connect was forcefully rejected.\n"));	//10061
			break;
		case WSAEDESTADDRREQ: 
			MyMessageBox(_T("A destination address is required.\n"));
			break;
		case WSAEFAULT: 
			MyMessageBox(_T("The lpSockAddrLen argument is incorrect.\n"));
			break;
		case WSAEINVAL: 
			MyMessageBox(_T("The socket is already bound to an address.\n"));			// 10022L
			WSACleanup();
			break;
		case WSAEISCONN: 
			MyMessageBox(_T("The socket is already connected.\n"));
			break;
		case WSAEMFILE: 
			MyMessageBox(_T("No more file descriptors are available.\n"));
			break;
		case WSAENETUNREACH: 
			MyMessageBox(_T("The network cannot be reached from this host ")
			_T("at this time.\n"));
			break;
		case WSAENOBUFS: 
			MyMessageBox(_T("No buffer space is available. The socket ")
				_T("cannot be connected.\n"));
			break;
		case WSAENOTCONN: 
			MyMessageBox(_T("The socket is not connected.\n"));
			break;
		case WSAENOTSOCK: 
			MyMessageBox(_T("The descriptor is a file, not a socket.\n"));
			break;
		case WSAETIMEDOUT: 
			MyMessageBox(_T("The attempt to connect timed out without ")
				_T("establishing a connection. \n"));
			break;
		default:
			TCHAR szError[256];
			_stprintf_s(szError, _T("OnConnect error: %d"), nErrorCode);
			MyMessageBox(szError);
			break;
				}	// end of the swtich
			if (m_pCCM)	m_pCCM->SetConnectionState(0);	// not connected
			//s+= _T("FAILED");
			//AfxMessageBox(s);
			//m_pFifo = 0;
			//m_pElapseTimer = 0;
			}	// if there was an error
	else	// connection was successful
		{
		
		//GetPeerName(m_pCCM->m_pstCCM->sServerIP4, m_pCCM->m_pstCCM->uServerPort);
		GetPeerName(s0, uSPort);
		GetSockName(s1, uCPort);	// my ip and port
//ifdef I_AM_PAP
		if (m_pCCM->m_nMyConnection == 0)
			{
			s.Format(_T("PAG client IP = %s:%d connected to PAG server = %s:%d \n"), s1, uCPort, s0, uSPort);
			gsPAP_Nx2UUI_IP.Format(_T("%s : %d"),s1,uCPort); // client Nx IP and port
			gsUUI_PAP_NxIP.Format(_T("%s : %d"),s0,uSPort); // UUI IP and port
			}
		else if (m_pCCM->m_nMyConnection == 1)
			{
						s.Format(_T("PAG client IP = %s:%d connected to PAG_AW server = %s:%d \n"), s1, uCPort, s0, uSPort);
			gsPAP_AW2UUI_IP.Format(_T("%s : %d"),s1,uCPort); // client Nx IP and port
			gsUUI_PAP_AllWall_IP.Format(_T("%s : %d"),s0,uSPort); // UUI IP and port
			}
		//DebugOutMessage(s);

		//pMainDlg->SetMy_PAP_Number(s1, uCPort);
//endif
		// may need to replace this with some sort of call to MakeConnectionDetail
		// changed when CLIENT_IDENTITY_DETAIL removed from structure ST_CLIENT_CONNECTION_MANAGEMENT
		//if (m_pCCM)	m_pCCM->MakeConnectionDetail(s0, uSPort, s1,uCPort); // where server moves from 127.0.0.1 to 10.101.10.190

		if (m_pCCM)
			{
			m_pCCM->SetConnectionState(0xff);
			m_pCCM->SetClientPort(uCPort);
			}
		s += _T("\n");
		s += m_pCCM->GetSocketName();
		s += _T(" Succeeded!\n");
		//DebugOutMessage(s);

#ifdef I_AM_PAP
		char buffer [80];
		strcpy(buffer,GetTimeStringPtr());
		CstringToChar(s1, txt);
		if (m_pCCM->m_nMyConnection == 0)
			printf("PAP client %s:%d connected to PAG server at %s\n",txt,uCPort, buffer);
		else if (m_pCCM->m_nMyConnection == 1)
			printf("PAP All Walls client %s:%d connected to PAG_AW server at %s\n", txt, uCPort, buffer);
		s2 = buffer;
		s.Format(_T("PAP client %s:%d connected to PAG server at %s  ***********************\n"), s1, uCPort, s2);
		DebugOutMessage(s);
#endif
		int nSize;
		int nSizeOf = sizeof(int);
		GetSockOpt(SO_SNDBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("NIC Transmit Buffer Size = %d\n"), nSize);
		DebugOutMessage(s);

		GetSockOpt(SO_RCVBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("NIC Receiver Buffer Size = %d\n"), nSize);
		DebugOutMessage(s);

		// Do all servers utilize the same message structure????
		// if this client is connected to Syscp, fill in the Connection Detail table
		// Now done in child class of CCM
		//
#ifdef LEAVEITOUT
		//I_AM_PAG
		if (m_pCCM)
			{
			if (m_pCCM->m_nMyConnection == 0)	// this is the connection to SysCp
				{
				CCCM_SysCp *pCCM_SysCp = (CCCM_SysCp*) m_pCCM;
				pCCM_SysCp->MakeConnectionDetail(s0, uSPort, s1,uCPort);
				}
			}
#endif
#if 0
		if (m_pMainDlg)	m_pMainDlg->MakeConnectionDetail(m_pCCM->m_pstCCM->sServerIP4, 
														 m_pCCM->m_pstCCM->uServerPort,
														 m_pCCM->m_pstCCM->sClientIP4, 
														 m_pCCM->m_pstCCM->uClientPort);
#endif
		// create the fifo and timer
		// a fifo that gets inspection data from an instrument and sends to a server eg PAG or UUI
		m_pFifo = new CCmdFifo(1460, 'C', m_pCCM->m_nMyConnection, 0);
		m_pFifo->m_nOwningThreadId = AfxGetThread()->m_nThreadID;
		strcpy( m_pFifo->tag, "New m_pFifoClntSkt 407 " );
		s = m_pFifo->tag;

		m_pElapseTimer = new CHwTimer;	
		strcpy( m_pElapseTimer->tag, "CClientSocket 411\n" );
		s0 = m_pElapseTimer->tag;
		s += s0;
		TRACE( s );

		}
     CAsyncSocket::OnConnect(nErrorCode);
	}


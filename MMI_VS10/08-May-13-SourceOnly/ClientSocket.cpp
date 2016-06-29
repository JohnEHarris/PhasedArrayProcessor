// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 


#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#else
#include "Truscan.h"
#include "TscanDlg.h"
#include "CCM_SysCp.h"		// jeh 05-Jun-12
extern THE_APP_CLASS theApp;
#endif

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"		// jeh 05-Jun-12
#include "time.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

IMPLEMENT_DYNAMIC(CClientSocket, CAsyncSocket);

#if 0
// Legacy constructor
CClientSocket::CClientSocket( CPtrList *pListDataIn,CPtrList *pListDebugMsgIn,
							  CPtrList *pListDataOut,CPtrList *pListDebugMsgOut,
							  CSemaphore * psemDataIn, CSemaphore * psemDataOut
							)
{
	m_pListDataIn = pListDataIn;
	m_pListDebugMsgIn = pListDebugMsgIn;
	m_pListDataOut = pListDataOut;
	m_pListDebugMsgOut = pListDebugMsgOut;
	m_psemDataIn = psemDataIn;
	m_psemDataOut = psemDataOut;
	//DebugInMessage( "New CClientSocket." );
	m_nChooseYourOnReceive = 0;	// legacy constructor gives legacy OnReceive jeh
	m_pCCM = NULL;
}
#endif

// New constructor for ClientConnectionManagment
// Pass a ptr to the CCM class instance managing this socket/connection
CClientSocket::CClientSocket( CClientConnectionManagement *pCCM	)
	{
		if (pCCM)	
		{
			m_pCCM = pCCM;
			TRACE(_T("Valid CCM ptr, use OnReceive1()\n"));
			m_nChooseYourOnReceive = 1;	// new constructor gets new OnReceive jeh
			//m_pMainDlg = (CMC_SysCPTestClientDlg *) theApp.m_pMainWnd;
			if (m_pCCM->m_pstCCM)
				m_pCCM->m_pstCCM->pSocket = this;
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
	if (NULL == m_pCCM)				return;
	if (NULL == m_pCCM->m_pstCCM)	return;
	m_pCCM->m_pstCCM->pSocket = NULL;
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
	CString s1 = _T("CClientSocket..");
	s1 += s + _T("\n");
	TRACE(s1);
}

// This is the legacy OnReceive. Selected by using the legacy constructor
// as used in ACP, the async feature is switched off supposedly. It is. Never breaks when PT sim sending data
void CClientSocket::OnReceive0(int nErrorCode) 
{
	TCPDUMMY * Data = new TCPDUMMY;
	int n = nErrorCode;	// means nothing, kills compiler warning
	CString s;
#if 0
	n = Receive( (void *) Data, sizeof(TCPDUMMY), 0 );
	m_pListDataIn->AddTail( Data );
	s.Format( _T("OnReceive0(): Received %d bytes."), n );
	DebugInMessage( s );
#endif
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
void CClientSocket::OnReceive(int nErrorCode) 
{
	switch (m_nChooseYourOnReceive)
	{
	case 0:		// legacy
		OnReceive0(nErrorCode);
		break;
	case 1:		// new.. uses CCM class and objects
		OnReceive1(nErrorCode);
		break;
	default:
		TRACE(_T("Not a valid choice for OnReceive, Houston, we have a problem\n"));
		break;
	}
	CAsyncSocket::OnReceive(nErrorCode);	

}

void CClientSocket::OnAccept(int nErrorCode) 
	{
	
	CAsyncSocket::OnAccept(nErrorCode);
	}

void CClientSocket::OnClose(int nErrorCode) 
	{
	if (m_pCCM)
		{
		// Let the client manager decide if a restart of the receive/send thread is needed
		m_pCCM->SetConnectionState(0);
		if ( m_pCCM->m_pstCCM)
			{
			m_pCCM->InitReceiveThread();
			}
		}


	CAsyncSocket::OnClose(nErrorCode);
	}

// Add this for an easy way to let PAG have a pop-up message box but PAM output to the dark scree if in debug mode
void CClientSocket::MyMessageBox(CString s)
	{
#ifdef THIS_IS_SERVICE_APP
	DebugOutMessage(s);
#else
	AfxMessageBox(s);
#endif
	}

// copied from the Microsoft example code for OnConnect
void CClientSocket::OnConnect(int nErrorCode)   // CClientSocket is derived from CAsyncSocket
  {
	  CString s, s0, s1;
	  UINT uSPort, uCPort;	// temp to hold port numbers discovered
	  if (m_pCCM)
	  {
		s.Format(_T("Client Connection %d "),m_pCCM->m_nMyConnection);
	  }
	  else
	  {
		  s = _T("Client number is unkown..");
	  }
	  if (0 != nErrorCode)
		{
        switch(nErrorCode)
        {
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
              MyMessageBox(_T("The attempt to connect was forcefully rejected.\n"));	//10061
              break;
           case WSAEDESTADDRREQ: 
              MyMessageBox(_T("A destination address is required.\n"));
              break;
           case WSAEFAULT: 
              MyMessageBox(_T("The lpSockAddrLen argument is incorrect.\n"));
              break;
           case WSAEINVAL: 
              MyMessageBox(_T("The socket is already bound to an address.\n"));
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
     }	// if there was an error
	 else	// connection was successful
	 {
		
		//GetPeerName(m_pCCM->m_pstCCM->sServerIP4, m_pCCM->m_pstCCM->uServerPort);
		GetPeerName(s0, uSPort);
		s.Format(_T("CClientSocket::OnConnect connected to %s:%d"), s0, uSPort);
		DebugOutMessage(s);
		//GetSockName(m_pCCM->m_pstCCM->sClientIP4, m_pCCM->m_pstCCM->uClientPort);
		GetSockName(s1, uCPort);
		s.Format(_T("CClientSocket::OnConnect my IP = %s:%d"), s1, uCPort);
		DebugOutMessage(s);
		// may need to replace this with some sort of call to MakeConnectionDetail
		// changed when CLIENT_IDENTITY_DETAIL removed from structure ST_CLIENT_CONNECTION_MANAGEMENT
		//if (m_pCCM)	m_pCCM->MakeConnectionDetail(s0, uSPort, s1,uCPort); // where server moves from 127.0.0.1 to 10.101.10.190

		if (m_pCCM)
			{
			m_pCCM->SetConnectionState(0xff);
			m_pCCM->SetClientPort(uCPort);
			}
		s += _T("\nSucceeded!");
		DebugOutMessage(s);

#ifdef THIS_IS_SERVICE_APP
		char buffer [80];
		strcpy(buffer,GetTimeStringPtr());
		printf("PAM client connected to PAG server on connection %s:%d at %s\n",s1,uCPort, buffer);
#endif
		int nSize;
		int nSizeOf = sizeof(int);
		GetSockOpt(SO_SNDBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("NIC Transmit Buffer Size = %d"), nSize);
		DebugOutMessage(s);

		GetSockOpt(SO_RCVBUF, &nSize, &nSizeOf, SOL_SOCKET);
		s.Format(_T("NIC Receiver Buffer Size = %d"), nSize);
		DebugOutMessage(s);

		// Do all servers utilize the same message structure????
		// if this client is connected to Syscp, fill in the Connection Detail table
		// Now done in child class of CCM
		//
#ifdef PHASED_ARRAY_GUI
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

	 }
     CAsyncSocket::OnConnect(nErrorCode);
  }


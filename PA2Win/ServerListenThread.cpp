// ServerListenThread.cpp : implementation file
//
/*
Author:		JEH
Date:		15-Aug-2012
Purpose:	Provide a persistent Listen function for clients to connect to 

Revised:
*/

#include "stdafx.h"
// I_AM_PAP is defined in the PAP project under C++ | Preprocessor Definitions 

#ifdef I_AM_PAP
#include "PA2Win.h"
#include "PA2WinDlg.h"
#include "ServerListenThread.h"
#include "ServerConnectionManagement.h"
#include "ServerSocket.h"
#else
#include "PA2Win.h"
#include "PA2WinDlg.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CServerListenThread

IMPLEMENT_DYNCREATE(CServerListenThread, CWinThread)	// subtype c0, 80 bytes long

CServerListenThread::CServerListenThread()
	{
	m_pMySCM = NULL;
	m_pstSCM = NULL;
	m_pListenSocket = NULL;
	}

CServerListenThread::~CServerListenThread()
	{
	CString t;
	t.Format(_T("Listener Thread Id=%d - destructor has run\n"), m_pstSCM->ListenThreadID);
	TRACE(t);
	m_pstSCM->pServerListenThread = NULL;	// Advise our manager we are gone
	m_pstSCM->ListenThreadID = 0;
	//AfxEndThread( 0 );
	}

BOOL CServerListenThread::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
	return TRUE;
	}


int CServerListenThread::ExitInstance()
	{
	int i;
	CString s;
	// TODO:  perform any per-thread cleanup here
	if ( m_pListenSocket)
		{
		i = m_pListenSocket->ShutDown(2);
		s.Format( _T( "CServerListenThread::ExitInstance value = %d\n" ), i );
		TRACE( s );
		pMainDlg->SaveDebugLog( s );
		m_pListenSocket->Close();
		delete m_pListenSocket;	// runs the destructor for CServerSocket
		if (m_pstSCM)
			{
			m_pstSCM->pServerListenSocket = 0;
			}
		//m_pListenSocket = NULL;
		}
	if (m_pstSCM)
		m_pstSCM->pServerListenThread = 0;
	return CWinThread::ExitInstance();
	}

void CServerListenThread::DebugLog(CString s)
	{
#ifdef I_AM_PAP
	//theApp.SaveDebugLog(s);
	pMainDlg->SaveDebugLog(s);
#endif
	}


BEGIN_MESSAGE_MAP(CServerListenThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_INIT_LISTNER_THREAD,InitListnerThread)
	ON_THREAD_MESSAGE(WM_USER_STOP_LISTNER_THREAD,StopListnerThread)
	ON_THREAD_MESSAGE(WM_USER_DO_NOTHING,DoNothing)
	

END_MESSAGE_MAP()


// CServerListenThread message handlers
// pass ptr to controlling CServerConnectionManagement class instance in lParam
// Use the same socket class for the listening socket as well as the connected socket
afx_msg void CServerListenThread::InitListnerThread(WPARAM w, LPARAM lParam)
	{
	//int i;		// which thread/socekt/dailog out of several is this
	CString s;
	m_pMySCM = (CServerConnectionManagement *) lParam;
	if (NULL == m_pMySCM)
		{
		s = _T("InitListnerThread m_pMySCM is null\n");
		TRACE(s);
		DebugLog(s);
		return;
		}
	m_pstSCM = m_pMySCM->m_pstSCM;

	if (NULL == m_pstSCM)
		{
		s = _T("InitListnerThread m_pstCCM is null\n");
		TRACE(s);
		DebugLog(s);
		return;
		}

// Purely Randy's work
// fix a well known Microsoft screw up that occurs
// when using sockets in a multithreaded application 
// that is linked with static libraries.  It seems the
// static libraries do not properly init the hash maps
// so we have to do it manually.  sigh........
#ifndef _AFXDLL

	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	AfxSocketInit();	// does the same as the code shown below
#endif

	s.Format(_T("InitListnerThread OK threadID = %d\n"), this->m_nThreadID );
	TRACE(s);
	DebugLog(s);
	// create the listening socket
	if (m_pstSCM->pServerListenSocket)
		{
		s = _T("Listening socket already exists.... close and destroy before recreating\n");
		TRACE(s);
		DebugLog(s);
		m_pListenSocket->Close();
		Sleep(10);
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		}
	m_pListenSocket = new CServerSocket(m_pMySCM, eListener);	// subtype c0, 4278 bytes long
	s.Format(_T("InitListnerThread &m_pListenSocket = 0x%08x\n"), m_pListenSocket);
	TRACE(s);
	m_pListenSocket->m_nOwningThreadType = eListener;
	// Socket, init thyself
	int SockErr =  m_pListenSocket->InitListeningSocket(m_pMySCM);
	if (SockErr)
		{
#ifdef I_AM_PAG
		MAIN_DLG_NAME * pDlg = NULL;
		pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
		ASSERT (pDlg != NULL);
		pDlg->MessageBox(_T("Failed to Create Listening Socket"),_T("MMI to Master Connection Impossible"));
#endif
		s = _T("Failed to Create Listening Socket--Master to Instrument Connection Impossible\n");
		TRACE(s);
		DebugLog(s);
		}
	//SockErr = ERROR_SUCCESS = 0;

	switch (m_pMySCM->m_nMyServer)
		{
#ifdef I_AM_PAG
	case 0:
		m_pstSCM->sServerDescription = _T("PAG server for PAPs");
		break;
	case 1:
		m_pstSCM->sServerDescription = _T("PAG server for PAP_ALLWALLs");
		break;
	default:
		m_pstSCM->sServerDescription = _T("Unknow or Incorrecct server");
		break;
#else if I_AM_PAP

	case 0:
		m_pstSCM->sServerDescription = _T("PAP server for Instruments");
		break;

	default:
		m_pstSCM->sServerDescription = _T("Unknow or Incorrecct server");
		break;
#endif
		}	// switch (m_pMySCM->m_nMyServer)

	m_pstSCM->pServerListenSocket = m_pListenSocket;
	s.Format(_T("Srv[%d] <%s> is listening at %s : %d\n"),m_pMySCM->m_nMyServer, 
		m_pstSCM->sServerDescription, m_pstSCM->sServerIP4, m_pstSCM->uServerPort);
	TRACE(s);

	SockErr =  m_pListenSocket->Listen(5);
	if (SockErr == 0)
		{
		SockErr = GetLastError();
		TRACE1("Listen Error = 0x%x\n", SockErr);
		}

	}

//ON_THREAD_MESSAGE(WM_USER_STOP_LISTNER_THREAD,StopListnerThread)
afx_msg void CServerListenThread::StopListnerThread(WPARAM w, LPARAM lParam)
	{
	int i;
	CString s;
	if (m_pstSCM == NULL)	return;
	if (m_pstSCM->pServerListenThread == NULL)	return;
	if (m_pstSCM->pServerListenThread->m_pListenSocket != NULL)
		{
		
#if 0

WSANOTINITIALISED A successful AfxSocketInit must occur before using this API. 10093L
標SAENETDOWN The Windows Sockets implementation detected that the network subsystem failed. 10050L
標SAEINVAL nHow is not valid. 10022L
標SAEINPROGRESS A blocking Windows Sockets operation is in progress. 10036L
標SAENOTCONN The socket is not connected ( SOCK_STREAM only). 10057L
標SAENOTSOCK The descriptor is not a socket. 10038L
#endif


		if (i = m_pListenSocket->ShutDown( 2 ))
			{
			s.Format( _T( "Listener shutdown = %d\n" ), i );
			TRACE( s );
			m_pListenSocket->Close();
			}
		delete m_pstSCM->pServerListenThread->m_pListenSocket;
		m_pstSCM->pServerListenThread->m_pListenSocket = NULL;

		}
	//delete m_pstSCM->pServerListenThread; 2017-05-31
	PostQuitMessage( 0 );
	}

// debugging aid
afx_msg void CServerListenThread::DoNothing(WPARAM w, LPARAM lParam)
	{
	CString s;
	s.Format(_T("Do nothing has run\n"));
	TRACE(s);
	//StopListnerThread( 0, 0 );
	}


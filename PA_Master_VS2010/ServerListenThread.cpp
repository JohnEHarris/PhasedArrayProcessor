// ServerListenThread.cpp : implementation file
//
/*
Author:		JEH
Date:		15-Aug-2012
Purpose:	Provide a persistent Listen function for clients to connect to 

Revised:
*/

#include "stdafx.h"
// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#include "ServerListenThread.h"
#include "ServerConnectionManagement.h"
#include "ServerSocket.h"
#else
#include "Truscan.h"
#include "TscanDlg.h"
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
	t.Format(_T("Listener Thread Id=0x%04x - destructor has run\n"), m_pstSCM->ListenThreadID);
	TRACE(t);
	m_pstSCM->pServerListenThread = NULL;	// Advise our manager we are gone
	m_pstSCM->ListenThreadID = 0;
	}

BOOL CServerListenThread::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
	return TRUE;
	}


int CServerListenThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	int Error;
	CString s;

	if (m_pstSCM == NULL)	goto EXIT;
	if (m_pstSCM->pServerListenThread == NULL)	goto EXIT;

	if ( m_pListenSocket)
		{
		if (m_pstSCM->pServerListenSocket->ShutDown(2) )
			{
			s = _T("Shutdown of listener socket was successful\n");
			TRACE(s);
			m_pstSCM->pServerListenSocket->Close();
			}
		else
			{
			Error = GetLastError();	// WSAENOTCONN                      10057L
			s .Format(_T("Shutdown of listener socket[%d] failed\n"), Error);
			TRACE(s);
			}
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		TRACE("Listening socket deleted\n");
		}
		
	if (m_pstSCM->pServerListenThread)
		{
		//m_pstSCM->pServerListenThread = NULL;
		}

EXIT:
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CServerListenThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_INIT_LISTNER_THREAD,InitListnerThread)
	//ON_THREAD_MESSAGE(WM_USER_STOP_LISTNER_THREAD,StopListnerThread)
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
		TRACE(_T("InitListnerThread m_pMySCM is null\n"));
		return;
		}
	m_pstSCM = m_pMySCM->m_pstSCM;

	if (NULL == m_pstSCM)
		{
		TRACE(_T("InitListnerThread m_pstCCM is null\n"));
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
#if 0
	if (pState->m_pmapSocketHandle	== NULL)	
		pState->m_pmapSocketHandle	= new CMapPtrToPtr;

	if (pState->m_pmapDeadSockets	== NULL)	
		pState->m_pmapDeadSockets	= new CMapPtrToPtr;
//	if (pState->m_pmapHWND			== NULL)
//		pState->m_pmapHWND			= new CHandleMap();

	if (pState->m_pCurrentWinThread	== NULL)
		pState->m_pCurrentWinThread	= m_pstSCM->pServerListenThread;

	if (pState->m_plistSocketNotifications == NULL)
		pState->m_plistSocketNotifications = new CPtrList;
#endif
#endif


	TRACE(_T("InitListnerThread OK\n"));
	// create the listening socket
	if (m_pListenSocket)
		{
		TRACE("Listening socket already exists.... close and destroy before recreating\n");
		m_pListenSocket->Close();
		Sleep(10);
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		}
	m_pListenSocket = new CServerSocket(m_pMySCM);	// subtype c0, 16444 bytes long
	m_pListenSocket->m_nOwningThreadType = eListener;
	// Socket, init thyself
	int SockErr =  m_pListenSocket->InitListeningSocket(m_pMySCM);
	if (SockErr)
		{
#ifdef _I_AM_PAG
		MAIN_DLG_NAME * pDlg = NULL;
		pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
		ASSERT (pDlg != NULL);
		pDlg->MessageBox(_T("Failed to Create Listening Socket"),_T("MMI to Master Connection Impossible"));
#endif
		TRACE(_T("Failed to Create Listening Socket--Master to Instrument Connection Impossible") );
		}
	//SockErr = ERROR_SUCCESS = 0;

	if (m_pMySCM->m_nMyServer == 0)
#ifdef _I_AM_PAG
		m_pstSCM->sServerDescription = _T("PAG server for PAMs");
#else if THIS_IS_SERVICE_APP
		m_pstSCM->sServerDescription = _T("PAM server for Instruments");
#endif
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


// debugging aid
afx_msg void CServerListenThread::DoNothing(WPARAM w, LPARAM lParam)
	{
	CString s;
	s.Format(_T("Do nothing has run\n"));
	TRACE(s);
	}


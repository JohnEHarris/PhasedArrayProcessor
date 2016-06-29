// ServerListenThread.cpp : implementation file
//
/*
Author:		JEH
Date:		15-Aug-2012
Purpose:	Provide a persistent Listen function for clients to connect to 

Revised:
*/

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
#include "ServerListenThread.h"
#include "ServerConnectionManagement.h"
#include "ServerSocket.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CServerListenThread

IMPLEMENT_DYNCREATE(CServerListenThread, CWinThread)

CServerListenThread::CServerListenThread()
	{
	m_pMySCM = NULL;
	m_pstSCM = NULL;
	m_pListenSocket = NULL;
	}

CServerListenThread::~CServerListenThread()
	{

	}

BOOL CServerListenThread::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
	return TRUE;
	}


int CServerListenThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	if ( m_pListenSocket)
		{
		m_pListenSocket->ShutDown(2);
		m_pListenSocket->Close();
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		}
	m_pstSCM->pServerListenThread = NULL;	// Advise our manager we are gone
	m_pstSCM->ListenThreadID = 0;
	return CWinThread::ExitInstance();
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
	m_pListenSocket = new CServerSocket(m_pMySCM);
	m_pListenSocket->m_nOwningThreadType = eListener;
	// Socket, init thyself
	int SockErr =  m_pListenSocket->InitListeningSocket(m_pMySCM);
	if (SockErr)
		{
		MAIN_DLG_NAME * pDlg = NULL;
		pDlg = (MAIN_DLG_NAME *) AfxGetMainWnd();
		ASSERT (pDlg != NULL);
		pDlg->MessageBox(_T("Failed to Create Listening Socket"),_T("MMI to Master Connection Impossible"));
		}
	//SockErr = ERROR_SUCCESS = 0;

	m_pstSCM->pServerListenSocket = m_pListenSocket;

	SockErr =  m_pListenSocket->Listen(5);
	if (SockErr == 0)
		{
		SockErr = GetLastError();
		TRACE1("Listen Error = 0x%x\n", SockErr);
		}

	}

afx_msg void CServerListenThread::StopListnerThread(WPARAM w, LPARAM lParam)
	{
	if (m_pstSCM == NULL)	return;
	if (m_pstSCM->pServerListenThread == NULL)	return;
	if (m_pListenSocket != NULL)
		{
		m_pListenSocket->Close();
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		}
	CWinThread *pThread = this;
	PostThreadMessage(WM_QUIT, 0L, 0L);
	}

// debugging aid
afx_msg void CServerListenThread::DoNothing(WPARAM w, LPARAM lParam)
	{
	CString s;
	s.Format(_T("Do nothing has run\n"));
	TRACE(s);
	}


// CmdProcessThread.cpp : implementation file
//JEH
// Provide a mechanims of processing received packets/commands at a lower or different
// priority than receiving commands
// 23-Jan-2013

#include "stdafx.h"
#include "PA2WinDlg.h"
#define WM_USER_CLIENT_PKT_RECEIVED					WM_USER+0x212	// in Tscaddlg.h
//#include "ServiceApp.h"
#include "CmdProcessThread.h"
#ifdef THIS_IS_SERVICE_APP
#include "CCM_PAG.h"
class CInstState;
#endif



// CCmdProcessThread

IMPLEMENT_DYNCREATE(CCmdProcessThread, CWinThread)

CCmdProcessThread::CCmdProcessThread()
	{
	}

CCmdProcessThread::~CCmdProcessThread()
	{
	if (NULL == m_pstCCM)						return;
	if (NULL == m_pstCCM->pCmdProcessThread)	return;
	m_pstCCM->pCmdProcessThread = 0;
	}

BOOL CCmdProcessThread::InitInstance()
	{
	CString s;
	// TODO:  perform and per-thread initialization here
	if (NULL == m_pstCCM)
		{		TRACE(_T("NULL == m_pstCCM\n" ));	return TRUE;		}
	if (NULL == m_pstCCM->pCmdProcessThread)
		{		TRACE(_T( "NULL == m_pstCCM->pCmdProcessThread\n" )); return TRUE;		}
	s.Format( _T( "pCmdProcessThread = 0x%x\n" ), m_pstCCM->pCmdProcessThread );
	TRACE( s );
	return TRUE;
	}

int CCmdProcessThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	TRACE( _T( "ExitInstance()\n" ) );
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CCmdProcessThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_CLIENT_PKT_RECEIVED,ProcessReceivedMessage)
	ON_THREAD_MESSAGE(WM_USER_KILL_CMD_PROCESS_THREAD,KillCmdProcess)

END_MESSAGE_MAP()


// CCmdProcessThread message handlers
// Assume the first client connection for every PAP is to the PAG server. Thus
// processing receive messages would be processing instrument command messages
// this thread functions is activated by CClientConnectionManagement::OnReceive()
// which got a command from PAG
void CCmdProcessThread::ProcessReceivedMessage(WPARAM, LPARAM)
	{
	if ( NULL == m_pMyCCM)			return;

	// call the ProcessReceivedMessage in the CCM class instance at this thread's priority level
	switch (m_pMyCCM->m_nMyConnection)
		{
#ifdef I_AM_PAP
	case 0:
	case 2:
		// case 0 of the client connection to PAG is the link thru which user commands come to PAP
		// These commands need to be routed to the appropriate instrument by the PAG program itself
		// The activating agent was CClientConnectionManagement::OnReceive() we assume is running at a
		// relatively high priority. By signaling to CCmdProcessThread we get and are now at the priority
		// level of that thread. The Call to the CCM_PAG class will run at CCmdProcessThread's priority.
		if (pCCM_PAG)
			pCCM_PAG->ProcessReceivedMessage();
		break;
#else
	// I_AM_PAG  the GUI. The received message is data from the boards
	case 0:

		//break;
#endif
	default:
		// this is processed by CCCM_PAG::ProcessReceivedMessage
		m_pMyCCM->ProcessReceivedMessage();
		}	// switch (m_pMyCCM->m_nMyConnection)
	}

afx_msg void CCmdProcessThread::KillCmdProcess( WPARAM w, LPARAM lParam )
	{
	CString s;
	int i;
	s.Format( _T( "delete this = 0x%x\n" ), this );
	if (NULL == m_pstCCM)						return;
	if (NULL == m_pstCCM->pCmdProcessThread)	return;
	if (this == m_pstCCM->pCmdProcessThread)
		i = 1;
	PostQuitMessage( 0 );	// send us to the destructor
	}

void CCmdProcessThread::DebugMsg(CString s)
	{
	TRACE(s);
	}

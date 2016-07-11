// CmdProcessThread.cpp : implementation file
//JEH
// Provide a mechanims of processing received packets/commands at a lower or different
// priority than receiving commands
// 23-Jan-2013

#include "stdafx.h"
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
	//delete m_pstCCM->pCmdProcessThread;
	m_pstCCM->pCmdProcessThread = NULL;
	}

BOOL CCmdProcessThread::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
	return TRUE;
	}

int CCmdProcessThread::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CCmdProcessThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_CLIENT_PKT_RECEIVED,ProcessReceivedMessage)

END_MESSAGE_MAP()


// CCmdProcessThread message handlers
// Assume the first client connection for every PAM is to the PAG server. Thus
// processing receive messages would be processing instrument command messages
// this thread functions is activated by CClientConnectionManagement::OnReceive()
// which got a command from PAG
void CCmdProcessThread::ProcessReceivedMessage(WPARAM, LPARAM)
	{
	if ( NULL == m_pMyCCM)			return;

	// call the ProcessReceivedMessage in the CCM class instance at this thread's priority level
	switch (m_pMyCCM->m_nMyConnection)
		{
#ifdef THIS_IS_SERVICE_APP
	case 0:
		// case 0 of the client connection to PAG is the link thru which user commands come to PAM
		// These commands need to be routed to the appropriate instrument by the PAG program itself
		// The activating agent was CClientConnectionManagement::OnReceive() we assume is running at a
		// relatively high priority. By signaling to CCmdProcessThread we get and are now at the priority
		// level of that thread. The Call to the CCM_PAG class will run at CCmdProcessThread's priority.
		if (pCCM_PAG)
			pCCM_PAG->ProcessReceivedMessage();
		break;
#else
	case 0:

		break;
#endif
	default:
		// this is processed by CClientConnectionManagement::ProcessReceivedMessage(void)
		m_pMyCCM->ProcessReceivedMessage();
		}	// switch (m_pMyCCM->m_nMyConnection)
	}

void CCmdProcessThread::DebugMsg(CString s)
	{
	TRACE(s);
	}


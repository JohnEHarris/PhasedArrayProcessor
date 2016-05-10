// TestThread.cpp : implementation file
//

#include "stdafx.h"
#include "ServiceApp.h"
//#include "TestThread.h"
#include "CCM_PAG.h"

// 30-Jan-2013 give the test thread some useful work. Let it implement a 10 ms timer tick
// CTestThread

IMPLEMENT_DYNCREATE(CTestThread, CWinThread)

//class CCCM_PAG;
//extern CCCM_PAG *pCCM_PAG;

CTestThread::CTestThread()
{
}

CTestThread::~CTestThread()
	{
	if (m_hTimerTick)
		::CloseHandle(m_hTimerTick);
	m_hTimerTick = 0;
	}

BOOL CTestThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CTestThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTestThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_THREAD_HELLO_WORLD,ThreadHelloWorld)

END_MESSAGE_MAP()


// CTestThread message handlers

afx_msg void CTestThread::ThreadHelloWorld(WPARAM w, LPARAM lParam)	// manually added jeh 10-24-2012
	{
	CString s;
	int i;
#ifdef _DEBUG
	printf("Hello World from Test Thread\n");
#endif
	s.Format(_T("Hello World from Test Thread wparam = %x, lparam = %x\n"), w,lParam);
	TRACE(s);
	m_hTimerTick = ::CreateEvent(0, TRUE, FALSE, 0);
	Sleep(10);
	TRACE(_T("Hello World now implements a timer tick function\n"));
	
	// Infinite loop waiting on handle which never gets set
	// Wakes every 100 ms and post msg to client threads primarily
	// in PAG this same functionality is called CTscanDlg::TimerTickToThreads(void)
	//
	while( ::WaitForSingleObject(m_hTimerTick, 100) != WAIT_OBJECT_0 )
		{
		for ( i = 0; i < MAX_CLIENTS; i++)
			{
			switch (i)
				{
			case 0:		// the client connection to PAG
				if (pCCM_PAG)
					pCCM_PAG->TimerTick(eRestartPAMtoPAG);
				break;

			default:
				break;
				}	// switch end
			}

		}
	s = _T("Exit Hello World\n");
	TRACE(s);

	}

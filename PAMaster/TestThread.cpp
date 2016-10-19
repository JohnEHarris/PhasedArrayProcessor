// TestThread.cpp : implementation file
//

#include "stdafx.h"
#include "ServiceApp.h"
//#include "TestThread.h"
//#include "CCM_PAG.h"
#include "vChannel.h"
extern CServiceApp theApp;

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
	//delete this;
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTestThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_THREAD_HELLO_WORLD,ThreadHelloWorld)
	ON_THREAD_MESSAGE(WM_USER_TEST_THREAD_BAIL,Bail)

END_MESSAGE_MAP()


// CTestThread message handlers
afx_msg void CTestThread::Bail(WPARAM w, LPARAM lParam)
	{
	ExitInstance();	// NEVER GETS HERE
	}


afx_msg void CTestThread::ThreadHelloWorld(WPARAM w, LPARAM lParam)	// manually added jeh 10-24-2012
	{
	CString s;
	int i;
#ifdef _DEBUG
	printf("Hello World from Test Thread\n");
	cout << "Hello World from Test Thread using std\n" << endl;
#endif
	s.Format(_T("Hello World from Test Thread wparam = %x, lparam = %x\r\n"), w,lParam);
	TRACE(s);
	theApp.SaveDebugLog(s);
	theApp.SaveFakeData(s);
	s = _T("Nc=3 Thold=37 M=5, Nx=3 Max Wall=1377 Min Wall=27 Drop=8\r\n");
	theApp.SaveFakeData(s);
	m_hTimerTick = ::CreateEvent(0, TRUE, FALSE, 0);
	Sleep(10);
	TRACE(_T("Hello World now implements a timer tick function\n"));
	//theApp.CloseFakeData();

	// Use debugger to test Nc operation
	//TestNc();
	TestNx();
	
	// Infinite loop waiting on handle which never gets set
	// Wakes every 100 ms and post msg to client threads primarily
	// in PAG this same functionality is called CTscanDlg::TimerTickToThreads(void)
	//
	// waiting for ServiceApp::ShutDown to issue ::SetEvent(m_pTestThread->m_hTimerTick);
	//
	while( ::WaitForSingleObject(m_hTimerTick, 100) != WAIT_OBJECT_0 )
		{
		for ( i = 0; i < MAX_CLIENTS; i++)
			{
			switch (i)
				{
			case 0:		// the client connection to PAG
				//if (pCCM_PAG)
					//pCCM_PAG->TimerTick(eRestartPAMtoPAG);
				break;

			default:
				break;
				}	// switch end
			}

		}
	s = _T("Exit Hello World\n");
	TRACE(s);

	}

void CTestThread::TestNc(void)
	{
	int i;
	CvChannel *pCh = new CvChannel(0,0,0);	// inst 0, chnl 0
	// Threshold = 30, Nc = 2, mod = 3
	BYTE bAmp[] = {28,32,40,16,25,28,2,5,33,12,5,35,37,41,6,0,
		0,0,0,0};
	BYTE bOut;
	pCh->FifoInit(0,2,30,3);	// id, nc=2, thld=30, m=3
	TRACE("Thld=30, Nc=2, Mod=3\n");
	for ( i = 0; i < sizeof(bAmp); i++)
		{
		bOut = pCh->InputFifo(0,bAmp[i]);
		TRACE2("In=%d, Out=%d\n",bAmp[i], bOut);
		}


	delete pCh;
	}

void CTestThread::TestNx(void)
	{
	int i,j = ASCANS_TO_AVG;
	CString s;
	WORD wMax, wMin, wBadWall, wGoodWall;
	stPeakData LocalPeakData;
	CvChannel *pCh = new CvChannel(0,0,2);	// inst 0, seq 0, chnl 2
	WORD Wall[] = {300,333,315,288,255,2200,000,324,326,366,400,000,000,298,320,322,
				   100,100,100,100,000,000 ,300,321,333,400,374,300,288,243,220,189,
				   199,212,333};
	// Nx = 3, Max=1377, Min=110 , Drop=4
	pCh->WFifoInit(3,1377,110,4);
		
	BYTE bAmp[] = {28,32,40,16,25,28,02,05,33,12,05,35,37,41,06,00,
				   38,44,28,37,29,33,16,33,10,20,10,30,29,32,31, 2, 
				   5,12, 3,11};
	BYTE bOut;
	pCh->FifoInit(0,1,30,3);	// id, nc=1, thld=30, m=3
	
	TRACE("Nx = 3, Max=1377, Min=110 , Drop=4  ---   ");
	TRACE("ID Nc = 1, Thld=30, M = 3 \n");
	
	for ( i = 0; i < sizeof(Wall)/2; i++)
		{
		pCh->InputWFifo(Wall[i]);
		wMax = pCh->wGetMaxWall();
		wMin = pCh->wGetMinWall();
		//if (pCh->wGetGoodConsecutiveCount() >= 4)
		//	pCh->ClearBadWallCount();	// reset bad wall counter after several good walls. 
		wBadWall = pCh->wGetBadWallCount();
		wGoodWall = pCh->wGetGoodWallCount();
		s.Format("[%2d] In=%3d, Max=%4d, Min=%5d, Good=%2d, Bad=%2d -- ", i, Wall[i],wMax, wMin, wGoodWall,wBadWall);
		TRACE(s);
		bOut = pCh->InputFifo(0,bAmp[i]);
		TRACE2("In=%2d, Out=%2d\n",bAmp[i], bOut);
		//j = pCh->bGetAscansInFifo();
		if ( pCh->AscanInputDone() )
			{	// transfer peak held data into ethernet packet
			pCh->CopyPeakData(&LocalPeakData);
			TRACE1("\nPeak Data after %d Ascans\n", j);
			s.Format("Ch=%2d  IdGate=%2d  MinWall = %4d   MaxWall = %4d  Status = 0x%04x\n\n", 
						LocalPeakData.bChNum, LocalPeakData.bId2, LocalPeakData.wTofMin,
						LocalPeakData.wTofMax, LocalPeakData.bStatus);
			TRACE(s);
			}


		}

	delete pCh;
	}
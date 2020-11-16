// TestThread.cpp : implementation file
//

#include "stdafx.h"
#include "PA2WinDlg.h"
//#include "TestThread.h"
//#include "CCM_PAG.h"
#include "vChannel.h"
//extern CServiceApp theApp;

// 30-Jan-2013 give the test thread some useful work. Let it implement a 10 ms timer tick
// CTestThread

IMPLEMENT_DYNCREATE(CTestThread, CWinThread)

//class CCCM_PAG;
//extern CCCM_PAG *pCCM_PAG;

CTestThread::CTestThread()
	{
	TRACE( _T( "Test Thread constructor\n" ) );
	}

CTestThread::~CTestThread()
	{
	if (g_hTimerTick)
		::CloseHandle(g_hTimerTick);
	g_hTimerTick = 0;
	TRACE( _T( "Test Thread destructor\n" ) );
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
	TRACE( _T( "CTestThread::ExitInstance() executed\n" ) );
	return CWinThread::ExitInstance();
	}

BEGIN_MESSAGE_MAP(CTestThread, CWinThread)

	ON_THREAD_MESSAGE(WM_USER_THREAD_HELLO_WORLD,ThreadHelloWorld)
	ON_THREAD_MESSAGE(WM_USER_TEST_THREAD_BAIL,Bail)

END_MESSAGE_MAP()


// CTestThread message handlers
afx_msg void CTestThread::Bail(WPARAM w, LPARAM lParam)
	{
//	while (nShutDown < 2)
		{
		Sleep( 50 );
		}
	nShutDown = 3;
	TRACE( _T( "Exitiing CTestThread\n" ) );
	PostQuitMessage(0);	// NEVER GETS HERE
	}



afx_msg void CTestThread::ThreadHelloWorld(WPARAM w, LPARAM lParam)	// manually added jeh 10-24-2012
	{
	CString s;
	int i, j, k, n = 0;
	gnNoData = 0;	// make array if more than 1 gate board
	// Use ClientCommunicationThread to send fake data
	CRITICAL_SECTION *pCSSendPkt = 0;	// control access to output (send) list - commands
	CRITICAL_SECTION *pCSRcvPkt = 0;
	CPtrList* pSendPktList = 0;	// list containing packets to send..commands
	CPtrList* pRcvPktList = 0;	// Idata put into this list
	UINT uLoopCnt = 0;		// make sure TestThread runs for a while before checking on No Idata
	IDATA_PAP *pB;

#ifdef _DEBUG
	printf("Hello World from Test Thread\n");
	cout << "Hello World from Test Thread using std\n" << endl;
#endif
	s.Format(_T("Hello World from Test Thread wparam = %x, lparam = %x\r\n"), w,lParam);
	TRACE(s);
	pMainDlg->SaveDebugLog(s);
	pMainDlg->SaveFakeData(s);
	s = _T("Nc=3 Thold=37 M=5, Nx=3 Max Wall=1377 Min Wall=27 Drop=8\r\n");
	pMainDlg->SaveFakeData(s);
	g_hTimerTick = ::CreateEvent(0, TRUE, FALSE, 0);
	Sleep(10);
	TRACE(_T("Hello World now implements a timer tick function\n"));
	//theApp.CloseFakeData();

	// Use debugger to test Nc operation
	//TestNc();
	TestNx();
	TestAdcFifo();
	Sleep(3000);	// 3 second sleep while rest of code gets initialized


	m_wLastIdataSeq[0] = 0;	//used to track stalled/stuck/disconnected wall instrument
							// scripted [] in case more than 1 gate board

	ST_CLIENT_CONNECTION_MANAGEMENT *pCCM = &stCCM[0];	// a static array of client connection info
	if (pCCM->uServerPort > 100 )	// important to zero out stCCM on start. uServerPort from 2017-2020 is 7501 for Nx wall
		{
		pSendPktList = pCCM->pSendPktList;
		pCSSendPkt = pCCM->pCSSendPkt;
		pRcvPktList = pCCM->pRcvPktPacketList;
		pCSRcvPkt = pCCM->pCSRcvPkt;
		}
	
	// Infinite loop waiting on handle which never gets set
	// Wakes every 100 ms and post msg to client threads primarily
	// in PAG this same functionality is called PA2WinDlg::TimerTickToThreads(void)

	m_pSCC = GetServerClientConnection(0, 0);
	//
	// waiting for ServiceApp::ShutDown to issue ::SetEvent(m_pTestThread->g_hTimerTick);
	//
	while( (::WaitForSingleObject(g_hTimerTick, 100) != WAIT_OBJECT_0 ) /*&& (nShutDown == 0)*/ )
		{	// while wait
#ifdef I_AM_PAP
		for ( i = 0; i < MAX_CLIENTS; i++)
			{
			switch (i & 1)
				{
			case 0:		// the client connection to PAG for Nx data
				// force connection by Normal Nx data processing first so that
				// PAG/UUI WILL see the first connection as client connection 0
				// do this by not waking the All Wall connection until the Nx connection
				// is made
				if ((pCCM_PAG) &&
					(nShutDown == 0))
					{
					pCCM_PAG->TimerTick(eRestartPAPtoPAG);
					Sleep(20);
					m_pSCC = GetServerClientConnection(0, 0);
					}
				break;
			case 1:
				// the client connection for All-Wall data
				if (pCCM_PAG == NULL) break;	// must connect Nx process first.
				if (pCCM_PAG->m_pstCCM->bConnected)
					{

					if ((pCCM_PAG_AW) &&
						(nShutDown == 0))
						{
						pCCM_PAG_AW->TimerTick(eRestartPAPtoPAG);
						}
					}
				break;

			default:
				break;
				}	// switch end
			}	// for ( i = 0; i < MAX_CLIENTS; i++)
#endif
		// Sometimes commands may stagnate in linked lists
		// Check linked list for stored commands and post commands 
		// to thread with waiting linked lists
		// Start with server to send commands to clients (ADC & Pulser)
		for (i = 0; i < MAX_SERVERS; i++)
			{
			for ( j = 0; j < MAX_CLIENTS_PER_SERVER; j++)
				{ 
				if (stSCM[i].pClientConnection[j])
					{	//connected
					if (stSCM[i].pClientConnection[j]->pSendPktList)	
						{
						if (k = stSCM[i].pClientConnection[j]->pSendPktList->GetCount())	//there are packets to send
							{
							CServerSocket *pSocket = stSCM[i].pClientConnection[j]->pSocket;
							CServerSocketOwnerThread *pThread = stSCM[i].pClientConnection[j]->pServerSocketOwnerThread;
							if ((pSocket == 0) || (pThread == 0))
								{
								s = _T("No socket or No Thread so send packet from Server to Client\n");
								TRACE(s);
								break;
								}
							// invoke CServerSocketOwnerThread::TransmitPackets()  -- SEND A COMMAND
							pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, 0, 0L);
							s.Format( _T("Command from Srv%d sent to Client%d from idle loop, GetCount = %d\n"), i,j, k);
							TRACE(s);
							}	//there are packets to send
						}	// pSendPktList exists
					}	// if (stSCM[i].pClientConnection[j])
				}	// j loop on clients
			}	// i loop on servers

		// check to see if no input from gate board or pulser
		// if no input, generate fake data to keep packets going to PT/UUI
		// check gLastIdataPap.wMsgSeqCnt
		// MAX-CLIENTS  would include all-wall. For now 2020-10-28 only look at Nx packets
		// Inspection data from gate board is caught by Server using ServerSocket. Server Socket
		// puts data into linked list
		for (i = 0; i < 1; i++)	// (i = 0; i < MAX_CLIENTS; i++)
			{ 
//			m_pstCCM = &stCCM[i];			
//			if (m_pstCCM == NULL)
//				break;
			m_pSCC = GetServerClientConnection(0, 0);	// NcNx wall, first gate board
			// If more than 1 client connection, would have to have timeout counter for each client
			// now, 2020-10-21 assume only one PAP. Thus only one local variable to sense when Gate Board(s) has quit
			if (m_wLastIdataSeq[0] != gwIdataMsgSeqCnt)// for more than 1 gate board, gwIdataMsgSeqCnt needs to be an array
				{
				m_wLastIdataSeq[0] = gwIdataMsgSeqCnt;
				gnNoData = 0;	// also needs to be an array if > 1 gate board
				gbWallDisconnected = 0;
				gbPulserDisconnected = 0;				// Inspection data is being sent
				gLastIdataPap.wMsgID = eNcNxInspID;
				}
			else
				{
				// if 4 seconds passes AND Nx data has previously been sent, then send fake data message
				if ((uLoopCnt > 50 ) && (gLastIdataPap.wMsgSeqCnt > 10))
					gnNoData++;
				if (gnNoData > 40)
					{
					pB = new IDATA_PAP;
					// normal Idata goes into mpscc->pRcvPKList
					// Set m_pSCC to a valid value
					gLastIdataPap.wMsgID = eFakeDataID;	// 0
					gbWallDisconnected = eGateOff;
					gbPulserDisconnected = ePulserOff;
					gLastIdataPap.wStatus |= gbWallDisconnected | gbPulserDisconnected;
					gLastIdataPap.bPapStatus |= gbWallDisconnected | gbPulserDisconnected;
					// copy code from ServerSocket::OnReceive
					memcpy((void *) pB, &gLastIdataPap, gLastIdataPap.wByteCount);	// move all data to the new buffer
					EnterCriticalSection(m_pSCC->pCSRcvPkt);
					if (m_pSCC)
						{
						if (m_pSCC->pServerRcvListThread)
							{	  //put the new fake data into the Nx data buffer
							m_pSCC->pRcvPktList->AddTail(pB);
							m_pSCC->bConnected = 2;
							}
						else
							{
							delete pB;
							pB = 0;
							}
						}
					LeaveCriticalSection(m_pSCC->pCSRcvPkt);

					//pRcvPktList = pCCM->pRcvPktPacketList;
					// Send fake data or last Idata with changed status to UUI/PAG
					// gLastIdataPap with modifided status bits.
					// emulate operation of ServerSocket::On Receive. Assumes that 
					// m_pSCC exists. = m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex];
					n++;	// testing code... remove and replace with FakeData msg
					// place fake data into transmit queue and invoke TransmitPackets()
					// Send Fake data and reset gnNoData

				// causes CServerRcvListThread::ProcessRcvList(WPARAM w, LPARAM lParam) to run
					m_pSCC->pServerRcvListThread->PostThreadMessage(WM_USER_SERVERSOCKET_PKT_RECEIVED, 
						(WORD) m_pSCC->m_nClientIndex, 0L);

					Sleep(50);
					gnNoData = 0;
					// reset last Idata packet to normal operation
					//gLastIdataPap.wMsgID = eNcNxInspID;
					//gbWallDisconnected = 0;
					//gbPulserDisconnected = 0;
					}	// if (gnNoData > 40)
				}					

			}
		uLoopCnt++;
		if (uLoopCnt > 0xfffffff8)
			uLoopCnt = 500;
		}	// while wait
	s = _T("Exit Hello World\n");
	TRACE(s);
	nShutDown = 2;
	//Sleep( 10 );
	PostQuitMessage(0);
	}

void CTestThread::TestNc(void)
	{
	int i;
	CvChannel *pCh = new CvChannel(0,0);	// inst 0, chnl 0
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
	stPeakChnlPAP LocalPeakData;
		
	i = sizeof(PAP_INST_CHNL_NCNX);

	CvChannel *pCh = new CvChannel(0,2);	// inst 0, seq 0, chnl 2
	WORD Wall[] = {300,333,315,288,255,2200,000,324,326,366,400,000,000,298,320,322,
				   100,100,100,100,000,000 ,300,321,333,400,374,300,288,243,220,189,
				   199,212,333};
	// Nx = 3, Max=1377, Min=110 , Drop=4
	pCh->WFifoInit(3,1392,110,4);
		
	BYTE bAmp[] = {28,32,40,16,25,28,02,05,33,12,05,35,37,41,06,00,
				   38,44,28,37,29,33,16,33,10,20,10,30,29,32,31, 2, 
				   5,12, 3,11};
	BYTE bOut;
	pCh->FifoInit(0,2,30,3);	// id, nc=2, thld=30, m=3
	
	TRACE("Nx = 3, Max=1377, Min=110 , Drop=4  ---  \n");
	TRACE("ID Nc = 2, Thld=30, M = 3 \n");
	
	for ( i = 0; i < sizeof(Wall)/2; i++)
		{
		// must do flaws before walls. Wall input reset all FIFO when input qty = ASCANS_TO_AVG
		bOut = pCh->InputFifo(0,bAmp[i]);
		pCh->InputWFifo(Wall[i]);
		wMax = pCh->wGetMaxWall();
		wMin = pCh->wGetMinWall();
		//if (pCh->wGetGoodWallCount() >= 4)
		//	pCh->ClearBadWallCount();	// reset bad wall counter after several good walls. 
		wBadWall = pCh->wGetBadWallCount();
		wGoodWall = pCh->wGetGoodWallCount();
		s.Format(_T("[%2d] In=%3d, Max=%4d, Min=%5d, Good=%2d, Bad=%2d -- \n"), i, Wall[i],wMax, wMin, wGoodWall,wBadWall);
		TRACE(s);
		TRACE2("In=%2d, Out=%2d\n",bAmp[i], bOut);
		//j = pCh->bGetAscansInFifo();
		if ( pCh->AscanInputDone() )
			{	// transfer peak held data into ethernet packet
			pCh->CopyPeakData(&LocalPeakData);
			pCh->ResetGatesAndWalls();
			TRACE1("\nPeak Data after %d Ascans\n", j);
			s.Format(_T("IdGate=%2d  MinWall = %4d   MaxWall = %4d  Status = 0x%02x\n\n"),
			//s.Format(_T("IdGate=%2d  MinWall = %4d \n"),
			LocalPeakData.bId2, LocalPeakData.wTofMin,
			LocalPeakData.wTofMax, LocalPeakData.bStatus);
			TRACE(s);
			}


		}

	delete pCh;
	}

/******************* ADC FIFO DEBUG *************************/

// Add code from ADC TO test ADC fifo's
#define CMD_BUF_MODULO	16
#define SMALL_CMD_PACKET_SIZE	32			// 16 byte header, 16 bytes data
typedef struct
	{
	BYTE bIn;	// next empty slot in FIFo to hold a new command. 0-7
	BYTE bOut;	// points to next command in FIFO to be executed. 0-7
	BYTE bCnt;	// number of unexecuted commands in FIFO. 0-7
	BYTE bSpare;	// enforce 32 bit boundaries
	WORD wOffset[CMD_BUF_MODULO];	// offset from beginning of buffer for each BUF[]
	// FOR inputs, the insertion point is &Buf[bIn][wOffset[bIn]]
	WORD wLost;	//Input overwrote an out, increment lost when this happens
	BYTE BUF[CMD_BUF_MODULO][MAX_CMD_PACKET_SIZE];	// 16*1056 = 16896
	} CMD_FIFO;


// Add a second cmd fifo for small commands 2017-02-24 jeh
// In adc small buf modulo is 128
#define SMALL_CMD_BUF_MODULO	4
//small command still has 16 byte header + 16 byte command data

typedef struct
	{
	BYTE bIn;	// next empty slot in FIFo to hold a new command. 0-127
	BYTE bOut;	// points to next command in FIFO to be executed. 0-127
	BYTE bCnt;	// number of unexecuted commands in FIFO. 0-127
	BYTE bSpare;	// enforce 32 bit boundaries
	WORD wOffset[SMALL_CMD_BUF_MODULO];	// offset from beginning of buffer for each BUF[]
	// FOR inputs, the insertion point is &Buf[bIn][wOffset[bIn]]
	WORD wLost;	//Input overwrote an out, increment lost when this happens
	BYTE BUF[SMALL_CMD_BUF_MODULO][SMALL_CMD_PACKET_SIZE];	// [128][32] all on 32 bit boundaries
	} SMALL_CMD_FIFO;

CMD_FIFO CmdFifo;
SMALL_CMD_FIFO SmallCmdFifo;

BYTE* GetCmdFifoOutputPtr(void)
	{
	BYTE *pOut;
	pOut = &CmdFifo.BUF[CmdFifo.bOut][0];
	if (CmdFifo.bCnt > 0)
		{
		CmdFifo.bCnt--;
		CmdFifo.bOut++;
		CmdFifo.bOut %= CMD_BUF_MODULO;
		}
	return pOut;
	}

BYTE* GetSmallCmdFifoOutputPtr(void)
	{
	BYTE *pOut;
	pOut = &SmallCmdFifo.BUF[SmallCmdFifo.bOut][0];
	if (SmallCmdFifo.bCnt > 0)
		{
		SmallCmdFifo.bCnt--;
		SmallCmdFifo.bOut++;
		SmallCmdFifo.bOut %= SMALL_CMD_BUF_MODULO;
		}
	return pOut;
	}

// only called from Wiznet interrupt routine
BYTE* GetNextSmallCmdFifoInputPtr(void)
	{
	BYTE *pIn;
	SmallCmdFifo.bCnt++;	// assumes data will be immediately copied into the FIFO
	if (SmallCmdFifo.bCnt > SMALL_CMD_BUF_MODULO)
		{
		// lost oldest element in FIFO
		SmallCmdFifo.bOut = SmallCmdFifo.bIn;
		SmallCmdFifo.bOut %= SMALL_CMD_BUF_MODULO;
		SmallCmdFifo.wLost++;
		SmallCmdFifo.bCnt = SMALL_CMD_BUF_MODULO;
		}

	SmallCmdFifo.bIn++;
	SmallCmdFifo.bIn %= SMALL_CMD_BUF_MODULO;	// modulo 5 counter
	SmallCmdFifo.wOffset[SmallCmdFifo.bIn] = 0;	// start with all 32 bytes available
	pIn = &SmallCmdFifo.BUF[SmallCmdFifo.bIn][0];
	return pIn;
	}

BYTE* GetSmallCmdFifoInputPtr(void)
	{
	BYTE *pIn;
	pIn = &SmallCmdFifo.BUF[SmallCmdFifo.bIn][0];
	SmallCmdFifo.wOffset[SmallCmdFifo.bIn] = 0;	// start with all 32 bytes available	

	// increment input pointer in preparation for next call
	SmallCmdFifo.bIn++;
	SmallCmdFifo.bIn %= SMALL_CMD_BUF_MODULO;
	SmallCmdFifo.bCnt++;	// adding a cmd to small buffer
	if (SmallCmdFifo.bCnt > SMALL_CMD_BUF_MODULO)
		{
		SmallCmdFifo.bIn = SmallCmdFifo.bOut;
		SmallCmdFifo.wLost++;		// lost oldest element in FIFO
		gwSmallCmdLost = SmallCmdFifo.wLost;
		SmallCmdFifo.bOut = SmallCmdFifo.bIn + 1;
		SmallCmdFifo.bOut %= SMALL_CMD_BUF_MODULO;
		SmallCmdFifo.bCnt = SMALL_CMD_BUF_MODULO;
		gwStatus |= SMALL_CMD_BUF_OVERFLOW;
		gwStatusHoldCnt = 3;
		}
	return pIn;
	}

BYTE GetCmdsWaiting(void)
	{
	return CmdFifo.bCnt;
	}

BYTE GetSmallCmdsWaiting(void)
	{
	return SmallCmdFifo.bCnt;
	}


void CTestThread::InitCmdFifo(void)
	{
	memset(&CmdFifo, 0, sizeof(CmdFifo));
	}

void CTestThread::InitSmallCmdFifo(void)
	{
	memset(&SmallCmdFifo, 0, sizeof(SmallCmdFifo));
	}


void CTestThread::TestAdcFifo(void)
	{
	ST_SMALL_CMD SmallCmd;
	ST_GATE_DELAY_CMD *pGateCmd = (ST_GATE_DELAY_CMD*)&SmallCmd;
	ST_SMALL_CMD *pCmdBuf;
	
	CString s;
	int i, j, k;
	i = sizeof(SmallCmd);
	SmallCmd.uSync = SYNC;
	SmallCmd.wMsgID = 2;
	SmallCmd.wByteCount = 32;
	SmallCmd.bPapNumber = 0;
	SmallCmd.bBoardNumber = 0;
	pGateCmd->bSeq = 0;
	pGateCmd->bChnl = 1;
	pGateCmd->bGateNumber = 1;
	k = 0;	// retrieved msg id's from small fifo

	InitCmdFifo();
	InitSmallCmdFifo();
	//puts("Output doesn't start until 33rd input\n");
	for (j = 0; j < 10; j++)
		{
		for (i = 0; i < 16; i++)
			{
			SmallCmd.wMsgID = (i & 7) + 2;
			SmallCmd.wCmd[0] = SmallCmd.wMsgID;
			// put 1 command in, take 1 out
			pCmdBuf = (ST_SMALL_CMD *)GetSmallCmdFifoInputPtr();	// assume will always get entire small command
			memcpy((void*)pCmdBuf, (void*)&SmallCmd, sizeof(ST_SMALL_CMD));
			s.Format(_T("i=%02d,j=%02d, MsgID = %d Lost = %d\n"), i, j, pCmdBuf->wMsgID, gwSmallCmdLost);
			TRACE(s);

			if (j > 2)
				{	// output now lags input... thus the final while loop should execute
				if (GetSmallCmdsWaiting())
					{
					ST_LARGE_CMD *pPacket = (ST_LARGE_CMD *)GetSmallCmdFifoOutputPtr();
					if (pPacket->wMsgID < TOTAL_COMMANDS)
						{
						s.Format(_T("[%03d] In= %d, Out=%d, Cnt=%d, Lost=%d  \n"), (j * 16 + i + 1),
							SmallCmdFifo.bIn, SmallCmdFifo.bOut, SmallCmdFifo.bCnt, SmallCmdFifo.wLost);
						TRACE(s);
						m_MsgId[k++] = pPacket->wMsgID;
						}

						//CmdExecutive(pPacket);
					else
						{
						s.Format(_T("Skipped command [j][i] %d %d\n"), j, i);
						TRACE(s);
						}
					}
				else
					{
					s.Format(_T("No small commands waiting [j][i] %d %d \n"), j, i);
					TRACE(s);
					}
				}
			}	// for (i = 0; i < 16; i++)
		} // for (j = 0; j < 10; j++)

	while (GetSmallCmdsWaiting())
		{
		ST_LARGE_CMD *pPacket = (ST_LARGE_CMD *)GetSmallCmdFifoOutputPtr();
		if (pPacket->wMsgID < TOTAL_COMMANDS)
			{
			s.Format(_T("[%03d] In= %d, Out=%d, Cnt=%d, Lost=%d  \n"), (j * 16 + i + 1),
				SmallCmdFifo.bIn, SmallCmdFifo.bOut, SmallCmdFifo.bCnt, SmallCmdFifo.wLost);
			TRACE(s);
			m_MsgId[k++] = pPacket->wMsgID;
			}
		}

	}
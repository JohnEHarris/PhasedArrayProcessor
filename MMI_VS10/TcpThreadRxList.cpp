// TcpThreadRxList.cpp: implementation of the CTcpThreadRxList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "truscan.h"
#include "TcpThreadRxList.h"
#include "afxmt.h"
#include "winsock2.h"
#include "math.h"
#include "HwTimer.h"

#ifndef MMI_CODE
#define MMI_CODE
#endif

#define TRUE_DATA 1
#define FAKE_IDATA 0
#define FAKE_CALDATA 0
#define FAKE_PLC_MSG 0
#define FAKE_NET_MSG 0
#define FAKE_ADC_MSG 0

#define SLEEP_TIME  0

//#include "..\include\cfg100.h"
#include "..\include\UT_Msg.h"
#include "..\include\Truscan_version.h"
#include "tscandlg.h"

// Globals
CPtrList g_pTcpListUtData;				/* linked list holding the IData for the first inspect window */
CPtrList g_pTcpListUtData2;				/* linked list holding the IData for the second inspect window */
CPtrList g_pTcpListAscan;				/* linked list holding the A-Scan trace */
CCriticalSection g_CriticalSection;		/* Critical section for mutual exclusion access of g_pTcpListUtData */
CCriticalSection g_CriticalSection2;	/* Critical section for mutual exclusion access of g_pTcpListUtData2 */
CCriticalSection g_CriticalSectionAscan;  /* Critical section for mutual exclusion access of g_pTcpListAscan */
extern BYTE   g_bConnected[MAX_SHOES+1];   /* network connection status.  0: not connected, 1: connected.  Master in 0, Slave1 in 1, etc. */
int g_nLostIdataCnt = 0;

int g_nTemp=0;

int g_nClientSocket = -1;
volatile BOOL bRunSocketThread = FALSE;
volatile BOOL bRunAcqDataThread = FALSE;
extern CONFIG_REC ConfigRec;
//static CONFIG_REC m_ConfigRec;  // The data structure holding the board configuration.
								// It should be a member of the CTcpThreadRxList class, but then
								// I will have to include config2.h in TcpThreadRxList.h, and this is undesirable
								// because other files might see config2.h and cfg100.h at the same time.
								
extern CTscanDlg *pCTscanDlg;

ASCAN_HEAD_NIOS g_AscanHead;								

/************************************************************
	High resolution sleep function
*************************************************************/
void WaitMicroseconds(int nMicroseconds)
{
	LARGE_INTEGER frequency, count1, count2;
	int timeinmicroseconds;
	if (!QueryPerformanceFrequency(&frequency))
			AfxMessageBox(_T("No high-resolution performance counter"));

	QueryPerformanceCounter(&count1);
	QueryPerformanceCounter(&count2);
	timeinmicroseconds = (int) ( (count2.QuadPart - count1.QuadPart) * 1000 * 1000 / frequency.QuadPart );

	while (timeinmicroseconds < nMicroseconds) {
		QueryPerformanceCounter(&count2);
		timeinmicroseconds = (int) ( (count2.QuadPart - count1.QuadPart) * 1000 * 1000 / frequency.QuadPart );
	}
}

/************************************************************
	Find how many microseconds elapsed since last call with bStart=TRUE
*************************************************************/
int GetElapseTime(BOOL bStart)
{
	static BOOL bFirstCall = TRUE;
	static LARGE_INTEGER frequency, count1;
	LARGE_INTEGER count2;
	int timeinmicroseconds;
	if (bFirstCall)
	{
		if (!QueryPerformanceFrequency(&frequency))
				AfxMessageBox(_T("No high-resolution performance counter"));
		bFirstCall = FALSE;
	}

	if (bStart)
		QueryPerformanceCounter(&count1);

	QueryPerformanceCounter(&count2);
	timeinmicroseconds = (int) ( (count2.QuadPart - count1.QuadPart) * 1000 * 1000 / frequency.QuadPart );

	return timeinmicroseconds;
}


/************************************************************
	Server socket thread
	The MMI is a server to the Phased-Array Master program which
	supplies inspection data to the MMI
*************************************************************/
UINT ServerSocketThread(LPVOID lp)
{
	int *nSocket;
	int nClientSocket;
    struct sockaddr_in  clientAddr;    /* client's socket address */ 
    int                 sockAddrSize;  /* size of socket address structure */ 
	char *pIP,s[128];
	//int optval;// = sizeof(MMI_CMD)*100;
	//int optlen=4;

#ifndef _AFXDLL

	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
	AfxSocketInit();

#if 0
	if (pState->m_pmapSocketHandle	== NULL)	
		pState->m_pmapSocketHandle	= new CMapPtrToPtr;

	if (pState->m_pmapDeadSockets	== NULL)	
		pState->m_pmapDeadSockets	= new CMapPtrToPtr;
//	if (pState->m_pmapHWND			== NULL)
//		pState->m_pmapHWND			= new CHandleMap();

	if (pState->m_plistSocketNotifications == NULL)
		pState->m_plistSocketNotifications = new CPtrList;
#endif
#endif


	nSocket = (int *) lp;
	sockAddrSize = sizeof (struct sockaddr_in);

		nClientSocket = accept( *nSocket, ( struct sockaddr * ) &clientAddr, &sockAddrSize );

		if ( nClientSocket < 0 )
			{
			//AfxMessageBox(_T("accept call failed"));
			TRACE("ServerSocketThread::accept() failed\n");
			return 0;
			}
		else
			{
			g_nClientSocket = nClientSocket;
			pIP = inet_ntoa((IN_ADDR) clientAddr.sin_addr);
#ifdef _DEBUG
			sprintf(s,"PA Master client at %s connected to PA GUI\n", pIP);
			TRACE("Hello World\n");
			TRACE(s);	// maybe trace1 doesn't work in non class functions
#endif
			}
		//AfxMessageBox(_T("Master connected to MMI successfully."));
		//AfxBeginThread(ServerSocketThread, nSocket, THREAD_PRIORITY_NORMAL);
	//}
	
	return 0;
}

/************************************************************
	Distribute messages to proper destination
	Put pReadBuf packet into a linked list if appropriate. If a bad packet
	delete pReadBuf packet
*************************************************************/
void DistributeMessage(I_MSG_RUN *pReadBuf, CTcpThreadRxList *pTcpThreadRxList)
	{
	WORD nMotionBus;
	DWORD nNextJointNum;
	BOOL bAscanMode = TRUE;
	I_MSG_NET *pNetBuf;
	BYTE Flush[0x4000];
	static int nFlushCount = 0;
	static int nPrintCount = 0;
	char s[128];
	//CString s;
	int i;

	time_t now;
	struct tm * timeinfo;
	char buffer [512];





	if (NULL == pReadBuf)
		{
		TRACE(_T("DistributeMessage - pReadBuf == NULL\r"));  // not a dialog class .. jeh
		return;
		}


	if ( (pReadBuf->MstrHdr.MsgId != ASCAN_MODE) && (pReadBuf->MstrHdr.MsgId != RAW_FLAW_MODE) )
		{
		nMotionBus = pReadBuf->InspHdr.status[2];
		nNextJointNum = pReadBuf->InspHdr.NextJointNum;
		bAscanMode = FALSE;
		}

	switch ( pReadBuf->MstrHdr.MsgId )
		{
	case RUN_MODE:
	case CAL_MODE:
	case PLC_MODE:
	case PKT_MODE:
	case ADC_MODE:
		// Put pReadBuf into either UtData or UtData2
		if ( (pReadBuf->MstrHdr.MsgId != RUN_MODE) || (pReadBuf->MstrHdr.nWhichWindow == 0) )
			{
			g_CriticalSection.Lock();
			//g_pTcpListUtData.AddTail((void *) pReadBuf);
			g_CriticalSection.Unlock();
			}
		else
			{
			g_CriticalSection2.Lock();
			//g_pTcpListUtData2.AddTail((void *) pReadBuf);
			g_CriticalSection2.Unlock();
			}
#ifdef _DEBUG
		if ((nPrintCount & 0xff) == 0)
			{
			sprintf(s,"[%5d]UtData Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
			TRACE(s);
			}
#endif
		break;
	
	case RAW_FLAW_MODE:
	case ASCAN_MODE:
		// Put pReadBuf into Ascan list
		g_CriticalSectionAscan.Lock();
		//g_pTcpListAscan.AddTail((void *) pReadBuf);
		if ( pReadBuf->MstrHdr.MsgId == ASCAN_MODE )
			memcpy ( (void *) &g_AscanHead, (void *) &(pReadBuf->MstrHdr.MmiStat), sizeof(ASCAN_HEAD_NIOS) );
		g_CriticalSectionAscan.Unlock();
#ifdef _DEBUG
		sprintf(s,"[%5d]Raw or AScan Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
		TRACE(s);
#endif
		break;

	case NET_MODE:
#ifdef _DEBUG
		time ( &now );
		timeinfo = localtime ( &now );

		strftime (buffer,80,"%Y_%m_%d %H:%M:%S",timeinfo);

		sprintf(s,"[%5d][PAM=%d] NET_MODE Msg Id 0x%04x  at %s\n", 
			nPrintCount++, pReadBuf->MstrHdr.nSlave, pReadBuf->MstrHdr.MsgId, buffer);
		TRACE(s);
#endif
		pNetBuf = (I_MSG_NET *) pReadBuf;
		// use pReadBuf locally and delete
		pTcpThreadRxList->SetBoardRevision(pNetBuf); // <- what does this do??? jeh
		for (i=0; i<MAX_SHOES+1; i++)
			{		g_bConnected[i] = pNetBuf->bConnected[i];		}
		delete pReadBuf;
		pReadBuf = NULL;
		break;

	default:  //not a truscan defined msg
		//AfxMessageBox("Unknown msg ID!");
#ifdef _DEBUG
		sprintf(s,"[%5d]Unknown Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
		TRACE(s);
#endif
		delete pReadBuf;
		pReadBuf = NULL;
		bAscanMode = TRUE;
		// flush the receive buffer
		nFlushCount++;
		// every 4th message try to realign the packets
		//if ((nFlushCount & 3) == 0)
			{
#ifdef _DEBUG
			//i = recv( g_nClientSocket, (char *) &Flush[0], 0x4000, 0 );  Yanming stuff
			//sprintf(s,"Flush read %d bytes\n",i);
			//TRACE(s);
#endif
			}

		break;
	}


	if (!bAscanMode)
		{
		pTcpThreadRxList->SetMotionBus(nMotionBus);
		pTcpThreadRxList->SetNextJointNumber(nNextJointNum);
		}

	// temp for testing
	if (pReadBuf)
		{
		delete pReadBuf;
		pReadBuf = NULL;
		}
	}	// DistributeMessage()


/***************************************************
   readn - read exactly n bytes from Tcp socket fd
   There are two of these, one a class memeber and this one
   not a class memeber. Assuming synchronous sockets, the
   recv() function below will block and suspend when no
   bytes are available to read.
****************************************************/
// Not a class member of the class TcpThreadRxList

int readn( int fd, char *bp, int len)
	{
	int Remaning;
	int rc;

	Remaning = len;		// how many bytes remaing to be read.
	while ( Remaning > 0 )
		{
		// If no incoming data is available at the socket, 
		// the recv call blocks and waits for data to arrive according to the blocking rules 
		rc = recv( fd, bp, Remaning, 0 );
		if ( rc < 0 )				/* read error? */
			{
			if ( WSAGetLastError() == WSAEINTR )	/* interrupted? */
				continue;			/* restart the read */
			return -1;				/* return error */
			}
		if ( rc == 0 )				/* EOF? */
			return len - Remaning;		/* return short count */
		bp += rc;			// advance the buffer ptr by number of bytes read
		Remaning -= rc;		// decrease the number to read by the amount just read
		}
	return len;
}

/************************************************************
	Acquire data thread
	Pass in a pointer to the instantiated CTcpThreadRxList class so we can
	reconnect to the master if it disconnects. .. pTcpThreadRxList->ServerSocketInit()
*************************************************************/
UINT TcpAcqDataThread(LPVOID lp)
{
	I_MSG_RUN *pReadBuf ;
//	I_MSG_NET *pNetBuf;
	MMI_CMD   sendBuf;
	MMI_RESEND_REQUEST *pRequest;
	CDWordArray seqArray;  //array holding the sequence numbers of the requested messages 
	CTcpThreadRxList *pTcpThreadRxList = (CTcpThreadRxList *) lp;
	CPtrList pListMsg;
	//char s[128];

	CHwTimer *pElapseRead = new CHwTimer();	// JEH curious about how fast readn works
	int nMaxReadTime = 0,	nMinReadtime = 0x3fffffff;	// in microseconds
	int nElapseReadTime, nReadCounter = 0;

//	I_MSG_RUN *pMsgRun;
//	POSITION pos, lpos;
	int xloc = 0;
	int i=0;	//, j;
	DWORD nPacketNumber = 0;

	CString sTmp, sImsgRun;

	pReadBuf = NULL;
	DWORD nNextMsgNum = 1;
	int   nWaitMsgCnt = 0;
	int   nResendReqCnt = 0;
#if FOR_TEST
	int NextPacketNum = 0;
#endif
#if  FAKE_CALDATA
	float  GainDB;
#endif

	sImsgRun.Format(_T("Sizeof I_MSG_RUN = %d\n"), sizeof(I_MSG_RUN));
	TRACE(sImsgRun);
	sImsgRun.Format(_T("Sizeof UT_INSP = %d\n"), sizeof(UT_INSP));
	TRACE(sImsgRun);	
	sImsgRun.Format(_T("Sizeof INSP_HDR = %d\n"), sizeof(INSP_HDR));
	TRACE(sImsgRun);	
	sImsgRun.Format(_T("Sizeof UDP_CMD_HDR = %d\n"), sizeof(UDP_CMD_HDR));
	TRACE(sImsgRun);


	sendBuf.MsgId = REQUEST_RESEND_MSG;
	pRequest = (MMI_RESEND_REQUEST *) sendBuf.CmdBuf;

	while(bRunAcqDataThread)
	{

#if  TRUE_DATA  /* true data */

		if (g_nClientSocket >= 0)
		{
			pReadBuf = new I_MSG_RUN;
			// pReadBuf will either be put into a linked list by DistributeMessage()
			// or it will be deleted if no suitable list is found.
#undef TIMER_TEST
#ifdef TIMER_TEST
			if (pElapseRead) pElapseRead->Start();
#endif
			// readn only returns when number of bytes requested are available
			if (readn(g_nClientSocket, (char *) pReadBuf, sizeof(I_MSG_RUN)) <= 0)
				{
				delete pReadBuf;
				pReadBuf = NULL;
				//if (pElapseRead) nElapseReadTime = pElapseRead->Stop();
				//sTmp.Format(_T("read failed error code %d occurred in %d micro seconds" ), g_nTemp, nElapseReadTime);
				g_nTemp++;
				//AfxMessageBox(sTmp);
				if (!bRunAcqDataThread)
					goto exit;			//return 0;
				closesocket(g_nClientSocket);
				closesocket(pTcpThreadRxList->m_nSocket);
				g_nClientSocket = -1;
				pTcpThreadRxList->m_nSocket = -1;
				// attempt to restart the connection with the phased array master.
				while (!pTcpThreadRxList->ServerSocketInit())
					{	::Sleep(200);	}
				}
#if FOR_TEST
			else
			{
				g_nLostIdataCnt += pReadBuf->MstrHdr.MmiStat.nSent-NextPacketNum;
				NextPacketNum = pReadBuf->MstrHdr.MmiStat.nSent + 1;
			}
#endif
#ifdef TIMER_TEST
			if (pElapseRead)
				{
				nElapseReadTime = pElapseRead->Stop();
				nReadCounter++;
				if (nMaxReadTime < nElapseReadTime)	nMaxReadTime = nElapseReadTime;
				if (nMinReadtime > nElapseReadTime)	nMinReadtime = nElapseReadTime;
				if ((nReadCounter & 0x1f) == 0)
					{
					sprintf(s,"Max read time in uSec = %d, Min read time = %d\n", nMaxReadTime, nMinReadtime);
					//Max read time in uSec = 3009795, Min read time = 3006718
					nMinReadtime = 0x3fffffff;
					nMaxReadTime = 0;		// 320 milliseconds
					TRACE(s);
					}
				}
#endif

			if (pReadBuf) 
				DistributeMessage(pReadBuf, pTcpThreadRxList);
#if 0
			if ( (nWaitMsgCnt > 0) && (GetElapseTime(FALSE) > 2000000) )  //hasn't receive all requested messages within 5000ms, request again
			{
				GetElapseTime(TRUE);  //restart timer
				pRequest->NumMsg = 0;
				j = seqArray.GetSize();
				for (i=0; i<j; i++)
				{
					if (pRequest->NumMsg == (BUF_SIZE/4-1) )//send resend request to Master if reach (BUF_SIZE/4-1) msgs
					{
						send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
						pRequest->NumMsg = 0;
					}
					pRequest->SeqNum[pRequest->NumMsg] = seqArray[i];;
					pRequest->NumMsg += 1;
				}
				send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
				nResendReqCnt++;
			}

			if ( (pReadBuf->MstrHdr.Mach == TRUSCAN_MACH_ID) /*&& (pReadBuf->UtInsp.TruscanMsgId == TRUSCAN_MSG_ID)*/ )
			{
				pCTscanDlg->m_nPacketNumber = nWaitMsgCnt; //pCTscanDlg->m_nMstrSentMsg - nPacketNumber;
				nPacketNumber++;

				if ( pReadBuf->MstrHdr.MsgNum == nNextMsgNum )  //expected msg
				{
					pCTscanDlg->m_nMstrSentMsg = pReadBuf->MstrHdr.MsgNum;
					nNextMsgNum = pReadBuf->MstrHdr.MsgNum + 1;
					if (nWaitMsgCnt == 0)  //not waiting resend, proceeds normally
						DistributeMessage(pReadBuf, pTcpThreadRxList);
					else  //put to the end of the temporary linked list
						pListMsg.AddTail((void *) pReadBuf);
				}
				else if ( pReadBuf->MstrHdr.MsgNum > nNextMsgNum )  //message(s) missed
				{
					if (nWaitMsgCnt == 0)
						GetElapseTime(TRUE);
					pRequest->NumMsg = 0;
					pListMsg.AddTail((void *) pReadBuf);
					for (seq=nNextMsgNum; seq<pReadBuf->MstrHdr.MsgNum; seq++)
					{
						seqArray.Add(seq);
						nWaitMsgCnt++;
						if (pRequest->NumMsg == (BUF_SIZE/4-1) )//send resend request to Master if reach (BUF_SIZE/4-1) msgs
						{
							send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
							pRequest->NumMsg = 0;
						}
						pRequest->SeqNum[pRequest->NumMsg] = seq;
						pRequest->NumMsg += 1;;
					}
					send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
					nNextMsgNum = pReadBuf->MstrHdr.MsgNum + 1;
				}
				else if (nWaitMsgCnt > 0)  //might be waited messages
				{
					BOOL bWaitedMsg = FALSE;
					j = seqArray.GetSize();
					for (i=0; i<j; i++)
					{
						if (pReadBuf->MstrHdr.MsgNum == seqArray[i])  //waited msg
						{
							bWaitedMsg = TRUE;
							seqArray.RemoveAt(i);
							seqArray.FreeExtra();
							nWaitMsgCnt--;
							if (i==0)  //first waited msg
								DistributeMessage(pReadBuf, pTcpThreadRxList);
							else
							{
								if (pListMsg.GetCount() == 0)
									pListMsg.AddTail( (void *) pReadBuf );
								else
								{
									for (pos=pListMsg.GetHeadPosition();pos != NULL; )
									{
										lpos=pos;
										pMsgRun = (I_MSG_RUN *) pListMsg.GetNext(pos);
										if ( pReadBuf->MstrHdr.MsgNum < pMsgRun->MstrHdr.MsgNum) 
										{
											pListMsg.InsertBefore(lpos, (void *) pReadBuf);
											goto sort_end;
										}
									} 
									pListMsg.AddTail( (void *) pReadBuf );
								}
							}
sort_end:
							if ( (nWaitMsgCnt == 0) || (nResendReqCnt > 10) ) //received all requested messages or wait too long then give up
							{
								while (!pListMsg.IsEmpty())
								{
									pMsgRun = (I_MSG_RUN *) pListMsg.RemoveHead();
									DistributeMessage(pMsgRun, pTcpThreadRxList);
								}
								nWaitMsgCnt = 0;
								seqArray.RemoveAll();
								seqArray.FreeExtra();
								nResendReqCnt = 0;
							}

							break;
						}
					}

					if (!bWaitedMsg)
					{
						if (pReadBuf != NULL)
							delete pReadBuf;
						pReadBuf = NULL;
					}
				}
				else  //redundant messages, discard
				{
					if (pReadBuf != NULL)
						delete pReadBuf;
					pReadBuf = NULL;
				}
			}
			else  //not a truscan defined msg
			{
				if (pReadBuf != NULL)
					delete pReadBuf;
				pReadBuf = NULL;
			}
#endif
		}	// if (g_nClientSocket >= 0)
		else
			::Sleep(100);

		//::Sleep(0);
		//WaitMicroseconds((int) (pIdata->iSampleTimeMs * 1000));

#endif  /* True data */

/*********************************************************************/

#if  FAKE_IDATA  /* Fake inspection data */
	static int nJoint = 0;
	if (xloc == 0)
		nJoint++;

	i++;
	xloc++;

	pReadBuf = new I_MSG_RUN;
	memset ((void *) pReadBuf, 0, sizeof(I_MSG_RUN));
	pReadBuf->MstrHdr.MsgId = RUN_MODE;
	pReadBuf->MstrHdr.nWhichWindow = 0;//i%2;
	pReadBuf->InspHdr.nStation = 0;
	pReadBuf->InspHdr.JointNumber = nJoint;
	pReadBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE | 0x8000;
	pReadBuf->InspHdr.Period = 2000;
	pReadBuf->InspHdr.VelocityDt = 2000;
	pReadBuf->InspHdr.EchoBit[0] = 0x1357;
	pReadBuf->InspHdr.EchoBit[7] = 0x135;
	pReadBuf->InspHdr.status[1] |= WALL_INCLUDED;
	pReadBuf->InspHdr.xloc = xloc;

	for (int j=0; j<N_SEG; j++)
	{
		pReadBuf->UtInsp.SegWallMax[j] = 500;
		pReadBuf->UtInsp.SegWallMin[j] = 500;
	}

	pReadBuf->InspHdr.status[2] = 0x6811;
	pTcpThreadRxList->SetMotionBus(pReadBuf->InspHdr.status[2]);
	pReadBuf->InspHdr.JointLength = xloc+5;

	if (xloc % 200)
	{
		pReadBuf->InspHdr.wLineStatus |= REAL_TIME_DATA;
	}

	if (xloc > 500)
	{
		xloc = 0;
		pReadBuf->InspHdr.wLineStatus = 0;
	}

			
	if (i>10000) i=1;
	//pReadBuf->InspHdr.JointNumber ++;	//= 123321;
	pReadBuf->InspHdr.wLineStatus &= ~REAL_TIME_DATA;
	//pReadBuf->InspHdr.wLineStatus = 2;
	pReadBuf->UtInsp.MaxTidFlaw = 75;
	//return 0;
	//::Sleep(1000);
	//pReadBuf->InspHdr.wLineStatus &= (~REAL_TIME_DATA);


	if ( pReadBuf->MstrHdr.nWhichWindow == 0 )
	{
		pReadBuf->UtInsp.MaxWall = 500+20;
		pReadBuf->UtInsp.MinWall = 500-30;
		pReadBuf->UtInsp.MaxWallChnl = 5;
		pReadBuf->UtInsp.MinWallChnl = 6;
		pReadBuf->UtInsp.MaxWallClk = 3;
		pReadBuf->UtInsp.MinWallClk = 4;

		pReadBuf->UtInsp.SegWallMax[8] = 588;
		pReadBuf->UtInsp.SegWallMin[10] = 428;
		pReadBuf->UtInsp.SegMaxChnl[8] = 5;
		pReadBuf->UtInsp.SegMinChnl[10] = 6;

		pReadBuf->UtInsp.MaxLodFlaw = i%100;
		pReadBuf->UtInsp.MaxLidFlaw = (i+10)%100;
		pReadBuf->UtInsp.SegLodFlaw[16] = i%100;
		pReadBuf->UtInsp.SegLidFlaw[16] = (i+10)%100;
		pReadBuf->UtInsp.SegLodChnl[16] = 3;
		pReadBuf->UtInsp.SegLidChnl[16] = 4;

		pReadBuf->UtInsp.MaxTodFlaw = i%100;
		pReadBuf->UtInsp.MaxTidFlaw = (i+10)%100;
		pReadBuf->UtInsp.SegTodFlaw[20] = i%100;
		pReadBuf->UtInsp.SegTidFlaw[20] = (i+10)%100;
		pReadBuf->UtInsp.SegTodChnl[20] = 1;
		pReadBuf->UtInsp.SegTidChnl[20] = 2;

		pReadBuf->UtInsp.MaxQ1odFlaw = i%100;
		pReadBuf->UtInsp.MaxQ1idFlaw = (i+10)%100;
		pReadBuf->UtInsp.SegQ1odFlaw[23] = i%100;
		pReadBuf->UtInsp.SegQ1idFlaw[23] = (i+10)%100;
		pReadBuf->UtInsp.SegQ1odChnl[23] = 7;
		pReadBuf->UtInsp.SegQ1idChnl[23] = 7;

		pReadBuf->UtInsp.MaxQ2odFlaw = i%100;
		pReadBuf->UtInsp.MaxQ2idFlaw = (i+10)%100;
		pReadBuf->UtInsp.SegQ2odFlaw[26] = i%100;
		pReadBuf->UtInsp.SegQ2idFlaw[26] = (i+10)%100;
		pReadBuf->UtInsp.SegQ2odChnl[26] = 8;
		pReadBuf->UtInsp.SegQ2idChnl[26] = 8;

		pReadBuf->UtInsp.MaxQ3odFlaw = i%100;
		pReadBuf->UtInsp.MaxQ3idFlaw = (i+10)%100;
		pReadBuf->UtInsp.SegQ3odFlaw[29] = i%100;
		pReadBuf->UtInsp.SegQ3idFlaw[29] = (i+10)%100;
		pReadBuf->UtInsp.SegQ3odChnl[29] = 9;
		pReadBuf->UtInsp.SegQ3idChnl[29] = 9;

		pReadBuf->UtInsp.MaxLamAFlaw = i%100;
		pReadBuf->UtInsp.MaxLamLFlaw = (i+10)%100;
		pReadBuf->UtInsp.SegLamAFlaw[10] = i%100;
		pReadBuf->UtInsp.SegLamLFlaw[10] = (i+10)%100;
		pReadBuf->UtInsp.SegLamAChnl[10] = 5;
		pReadBuf->UtInsp.SegLamLChnl[10] = 6;

		if (0/*xloc % 100 == 0*/)
		{
	//		pReadBuf->UtInsp.MaxTodFlaw = 36;
	//		pReadBuf->UtInsp.MaxTidFlaw = 47;
			pReadBuf->UtInsp.LidClk = 8;
			pReadBuf->UtInsp.LodClk = 6;
			pReadBuf->UtInsp.LidChnl = 10;
			pReadBuf->UtInsp.LodChnl = 18;
			pReadBuf->UtInsp.SegMinChnl[8] = 8;
			pReadBuf->UtInsp.SegMaxChnl[8] = 8;
	//		pReadBuf->UtInsp.SegWallMax[8] = 58;
			pReadBuf->UtInsp.SegWallMin[8] = 66;

			pReadBuf->UtInsp.SegTodChnl[10] = 10;
			pReadBuf->UtInsp.SegTidChnl[10] = 10;
			pReadBuf->UtInsp.SegLidChnl[6] = 6;
			pReadBuf->UtInsp.SegLodChnl[6] = 6;
			pReadBuf->UtInsp.MaxWall = i+10;
			pReadBuf->UtInsp.MinWall = 5+i;
			//g_pTcpListUtData.AddTail((void *) pReadBuf);
			pReadBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE | 0x8000;
		}

		g_CriticalSection.Lock();
		g_pTcpListUtData.AddTail((void *) pReadBuf);
		//pReadBuf->MstrHdr.nWhichWindow = 1;
		g_CriticalSection.Unlock();
	}
	else
	{
//		pReadBuf->InspHdr.nStation = 1;
		pReadBuf->UtInsp.MaxLodFlaw = 98-i;
		pReadBuf->UtInsp.MaxLidFlaw = 95-i;
		pReadBuf->UtInsp.SegWallMax[18] = 818-i;
		pReadBuf->UtInsp.SegWallMin[18] = 508-i;
		pReadBuf->UtInsp.SegMaxChnl[21] = 13-i;
		pReadBuf->UtInsp.SegMinChnl[21] = 24-i;
		pReadBuf->UtInsp.SegLodFlaw[16] = 68+i;
		pReadBuf->UtInsp.SegLidFlaw[16] = 78+i;
		pReadBuf->UtInsp.SegTodFlaw[13] = 30+i;
		pReadBuf->UtInsp.SegTidFlaw[13] = 20+i;

		g_CriticalSection2.Lock();
		g_pTcpListUtData2.AddTail((void *) pReadBuf);
		//pReadBuf->MstrHdr.nWhichWindow = 0;
		g_CriticalSection2.Unlock();
	}

	::Sleep(100);
	//WaitMicroseconds(5000);

#endif  /* fake inspection Data */

/*********************************************************************/

#if  FAKE_CALDATA  /* Fake calibration data */

			pReadBuf = new I_MSG_RUN;

			pCalBuf = (I_MSG_CAL *) pReadBuf;

			pCalBuf->MstrHdr.MsgId = CAL_MODE;
			GainDB = pTcpThreadRxList->GetRcvrGain(0) - 20.0f;
			pCalBuf->Shoe[0].Amp[0][0] = 45*pow(10.0f, GainDB/20.0f);
			GainDB = pTcpThreadRxList->m_fOdTcgGain[0] - 20.0f;
			pCalBuf->Shoe[0].Amp[0][1] = 25*pow(10.0f, GainDB/20.0f);
			pCalBuf->Shoe[8].Amp[0][0] = 45;
			pCalBuf->Shoe[8].Amp[0][1] = 75;
			pCalBuf->Shoe[8].Amp[1][0] = 55;
			pCalBuf->Shoe[8].Amp[1][1] = 65;
			pCalBuf->Shoe[0].MaxWall[0] = 510;
			pCalBuf->Shoe[0].MinWall[0] = 490;
			xloc += 500;
			pCalBuf->InspHdr.xloc = xloc;

			g_pTcpListUtData.AddTail((void *) pReadBuf);

			::Sleep(5000);

#endif  /* fake calibration Data */

/*********************************************************************/

#if FAKE_PLC_MSG  /* fake PLC message */

		pReadBuf = new I_MSG_RUN;

		pPlcBuf = (I_MSG_PLC *) pReadBuf;

		pPlcBuf->MstrHdr.MsgId = PLC_MODE;
		pPlcBuf->PlcStatus.dwMotionBus = 0x12345678;
		pPlcBuf->PlcStatus.dwXOmega = 0x87654321;
		pPlcBuf->PlcStatus.dwDiscreteIO = 0x10203040;

		g_CriticalSection.Lock();
		g_pTcpListUtData.AddTail((void *) pReadBuf);
		g_CriticalSection.Unlock();

		::Sleep(1000);

#endif

/*********************************************************************/

#if FAKE_ADC_MSG  /* fake AD Converter message */

		pReadBuf = new I_MSG_RUN;

		I_MSG_ADC *pAdcBuf;

		pAdcBuf = (I_MSG_ADC *) pReadBuf;

		pAdcBuf->MstrHdr.MsgId = ADC_MODE;
		pAdcBuf->nGate1Amp = 102;

		g_CriticalSection.Lock();
		g_pTcpListUtData.AddTail((void *) pReadBuf);
		g_CriticalSection.Unlock();

		::Sleep(1000);

#endif

/*********************************************************************/

#if FAKE_NET_MSG  /* fake NET message */

		for (i=0; i<MAX_SHOES+1; i++)
		{
			g_bConnected[i] = 1;
		}

		::Sleep(3000);

#endif

	}  //while(bRunAcqDataThread)

exit:
	
	if (pReadBuf != NULL )
		{		delete pReadBuf;		pReadBuf= NULL;			}
	if (pElapseRead)
		{		delete pElapseRead;		pElapseRead = NULL;		}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// CTcpThreadRxList Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpThreadRxList::CTcpThreadRxList()
{

	m_pConfigRec	= &ConfigRec;  /* point to the global config record */
	m_wMsgNum		= 0;

	m_nNumberOfBoards = MAX_BOARD_PER_INSTRUMENT;  /* might be temporary */

	/* scaling factors for the new 2-channel-4-gate boards */
	m_DelayScaling = DELAY_SCALE; //47;
	m_RangeScaling = RANGE_SCALE; //47;
	m_BlankScaling = 47;
	m_LevelScaling = 2;
	m_GainScaling = 10;
	m_DelayOffset = DELAY_OFFSET; //6; 
	m_RangeOffset = RANGE_OFFSET; //-2; 
	m_BlankOffset = -2; 
	m_LevelOffset = 0; 
	m_GainOffset = 0; 

	/* scaling factors for the old Truscope instrument */
	/* in ConfigRec, use the following scaling */
	m_TruscopDelayScaling = 10;
	m_TruscopRangeScaling = 10;
	m_TruscopBlankScaling = 10;
	m_TruscopLevelScaling = 1;
	m_TruscopGainScaling = 10;
	m_TruscopDelayOffset = 0; 
	m_TruscopRangeOffset = 0; 
	m_TruscopBlankOffset = 0; 
	m_TruscopLevelOffset = 0; 
	m_TruscopGainOffset = -200; 

	m_nActiveChannel = 0;
	m_nActiveGate = 0;
	m_bScopeEnabled = false;
	m_bRioBoard = false;

	//for debug
/*
	m_TcgGain[1][1] = 10;
	m_TcgGain[1][2] = 30;
	m_TcgGain[0][1] = 10;
	m_TcgGain[0][2] = 30;
	m_CurveCoef[0] = 0;
	m_CurveCoef[1] = 0;
	m_CurveCoef[2] = 0;
	m_CurveCoef[3] = 0;
	m_CurveCoef[4] = 0;
*/

	m_LastTcgGateStart = -1;
	m_LastTcgGateStop = -1;

	m_nSocket = -1;

	CString s;
#if jeh_omit
	//  can only make one connection. Listens and blocks until a client (the phased array master calls)
	while ( !ServerSocketInit() )
		{
		s.Format(_T("Make sure the network cable is plugged into the correct ports, \nand the Instrument is powered on, then click Retry to retry.\nOr click Cancel to run the program without connecting to the UT Instrument."));
		//if ( AfxMessageBox(s,MB_RETRYCANCEL, 0)== IDCANCEL )
		if ( ::MessageBox(NULL, s,SYSTEM_NAME, MB_RETRYCANCEL)== IDCANCEL )
			break;
		}

	StartTcpAcqDataThread();
#endif

	m_nMotionBus = 0;
	m_nNextJointNumber = 0;
	m_nMsgBlock = 0;
	//LoadShuntDac();
}

void CTcpThreadRxList::StartTcpAcqDataThread()
{
	bRunAcqDataThread = TRUE;
	CWinThread* pThread = AfxBeginThread(TcpAcqDataThread, this, THREAD_PRIORITY_NORMAL);
	m_hAcqDataThread = pThread->m_hThread;
	TRACE3("TcpAcqDataThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n", pThread, m_hAcqDataThread, pThread->m_nThreadID);
}

void CTcpThreadRxList::StopTcpAcqDataThread()
{
	bRunAcqDataThread = FALSE;
	closesocket(g_nClientSocket);
	closesocket(m_nSocket);
	g_nClientSocket = -1;
	m_nSocket = -1;

	::Sleep(200);
	//::WaitForSingleObject (m_hAcqDataThread, INFINITE);

	// delete the elements of the linked list
	//g_CriticalSection.Lock();
	ClearListUtData();
	//g_CriticalSection.Unlock();
}

void CTcpThreadRxList::ClearListUtData()
{
	I_MSG_RUN * pRecBuf;
	g_CriticalSection.Lock();
	while(!g_pTcpListUtData.IsEmpty())
	{
		pRecBuf = (I_MSG_RUN *) g_pTcpListUtData.RemoveHead();
		delete pRecBuf;
	}
	g_CriticalSection.Unlock();
	g_CriticalSection2.Lock();
	while(!g_pTcpListUtData2.IsEmpty())
	{
		pRecBuf = (I_MSG_RUN *) g_pTcpListUtData2.RemoveHead();
		delete pRecBuf;
	}
	g_CriticalSection2.Unlock();
}

CTcpThreadRxList::~CTcpThreadRxList()
{
	//AfxMessageBox("CTcpThreadRxList::~CTcpThreadRxList()");
#if jeh_omit

	StopTcpAcqDataThread();
	::Sleep(200);
	if (m_nSocket >= 0) closesocket (m_nSocket);
	if (g_nClientSocket >= 0) closesocket (g_nClientSocket);
#endif
	ClearListUtData();
}



void CTcpThreadRxList::SetSamplingTimeMs(int iMilliSec)
{
	m_threadBuf.iSampleTimeMs = iMilliSec;

}



void CTcpThreadRxList::WaitMicroseconds(int nMicroseconds)
{
	LARGE_INTEGER frequency, count1, count2;
	int timeinmicroseconds;
	if (!QueryPerformanceFrequency(&frequency))
			AfxMessageBox(_T("No high-resolution performance counter"));

	QueryPerformanceCounter(&count1);
	
	//::Sleep(10);
		
	QueryPerformanceCounter(&count2);
	timeinmicroseconds = (int) ( (count2.QuadPart - count1.QuadPart) * 1000 * 1000 / frequency.QuadPart );
	//CString sfreq;
	//sfreq.Format(_T("%d",timeinmicroseconds);
	//AfxMessageBox(sfreq);

	while (timeinmicroseconds < nMicroseconds) {
		QueryPerformanceCounter(&count2);
		timeinmicroseconds = (int) ( (count2.QuadPart - count1.QuadPart) * 1000 * 1000 / frequency.QuadPart );
	}
}

bool CTcpThreadRxList::ServerSocketInit()
{
	struct sockaddr_in local;
	int rc;
	static int nCnt = 0;

	local.sin_family = AF_INET;
	local.sin_port = htons( 7501 );						// MMI_AS_SERVER_IP_PORT
	//local.sin_addr.s_addr = htonl( INADDR_ANY );
	local.sin_addr.s_addr = inet_addr("192.168.10.10");	// MMI_AS_SERVER_IP_ADDR
	// Blocking I/O is the default behavior for Windows Sockets 2
	m_nSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( m_nSocket < 0 )
		{
		AfxMessageBox(_T("Socket call failed."));
		return FALSE;
		}
	rc = bind( m_nSocket, ( struct sockaddr * )&local, sizeof( local ) );
	if ( rc < 0 )
		{
		if (nCnt < 5)
			{
			nCnt++;
			AfxMessageBox(_T("Bind call failed.\nNetwork cable might be unplugged.\nNetwork switch might not be powered on."));
			}
		closesocket(m_nSocket);
		return FALSE;
		}
	// listen for a client connection on port 7501
	TRACE1("Server Socket Thread listening on socket %d\n", m_nSocket);
	rc = listen( m_nSocket, 5 );
	if ( rc )
	{
		AfxMessageBox(_T("Listen call failed."));
		closesocket(m_nSocket);
		return FALSE;
	}

	// Now that we have a client connection, start a thread to handle the connection
	StartServerSocketThread();

	return TRUE;
}

#if 0
CWinThread* AfxBeginThread(AFX_THREADPROC pfnThreadProc,LPVOID pParam,int nPriority = THREAD_PRIORITY_NORMAL,
   UINT nStackSize = 0, DWORD dwCreateFlags = 0,LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL );
#endif

void CTcpThreadRxList::StartServerSocketThread()
	{
	bRunSocketThread = TRUE;
	CWinThread* pThread = AfxBeginThread(ServerSocketThread, &m_nSocket, THREAD_PRIORITY_NORMAL);
	m_hSocketThread = pThread->m_hThread;
	TRACE3("ServerSocketThread = 0x%04x, handle= 0x%04x, ID=0x%04x\n", pThread, m_hSocketThread, pThread->m_nThreadID);
	}

void CTcpThreadRxList::StopServerSocketThread()
	{
	bRunSocketThread = FALSE;
	::WaitForSingleObject (m_hSocketThread, INFINITE);
	}

/* readn - read exactly n bytes from Tcp socket fd */
// This is a duplication of the non-class member function readn
int CTcpThreadRxList::readn( int fd, char *bp, int len)
{
	int Remaning;
	int rc;

	Remaning = len;
	while ( Remaning > 0 )
	{
		rc = recv( fd, bp, Remaning, 0 );
		if ( rc < 0 )				/* read error? */
		{
			if ( WSAGetLastError() == WSAEINTR )	/* interrupted? */
				continue;			/* restart the read */
			return -1;				/* return error */
		}
		if ( rc == 0 )				/* EOF? */
			return len - Remaning;		/* return short count */
		bp += rc;
		Remaning -= rc;
	}
	return len;
}

// Steal the old socket send operation and replace with a new one using ServerConnectionManagement components
// This doesn't actually send the packet, but puts it in a linked list and signals the 
// ServerComThread to send it.
// AND IN JAN-2013 ONLY USES CLIENT CONNECTION[0]. NEED WAY TO USE
// DIFFERENT CLIENT CONNECTIONS IF MULTIPLE PAM's
// wChannel is the channel relative to the total number of channels in the system
// There has to be a function which computes the PAM number and the channel within that PAM
// in order to correctly route the message.
// This function is always called by 
// CTcpThreadRxList::SendSlaveMsg(WORD nMsgId, WORD par1, WORD par2, WORD par3, WORD par4, WORD par5, WORD par6)
// It is assumed that the variable gChannel is always set to the correct channel out of the total channels in 
// the instrument.
BOOL CTcpThreadRxList::NewSendFunction( BYTE *pBuf, int nSize,  int nDeleteFlag)
	{
	int rc, i;
	int nClient;
	CString str;
	MMI_CMD *pMmiCmd = (MMI_CMD *) pBuf;
	// initially ComputePamClientNumber() just returns 0
	pMmiCmd->PAM_Number =  nClient = pCTscanDlg->ComputePamClientNumber();	// or put this where the command is formed and use GetPamClientNumber()
	pMmiCmd->Inst_Number_In_PAM = pCTscanDlg->ComputeInstrumentNumberInPamClient();
	// after 6-Mar-2013 the PAM client and Instrument number should be in the MMI_CMD structure
	if (NULL == stSCM[0].pClientConnection[nClient])				return FALSE;	// jeh fix for any client connection, not just 0

	pMmiCmd->Mach	= 0x4150;	// "PA"
	pMmiCmd->MsgNum	= m_wMsgNum++;
	pMmiCmd->MsgLen = nSize;
//	if (0 == stSCM[0].pClientConnection[nClient]->bConnected)	return FALSE;	// fix this later ... jeh jeh
	rc = stSCM[0].pSCM->SendPacketToPAM(nClient,(BYTE *) pBuf, nSize, nDeleteFlag);
	if ( rc != nSize+4)
		{
		str.Format(_T("SendSlaveMsg bytes %d sent not equal msg size\n"),rc);
		TRACE(str);
		}
	else
		{
		i = stSCM[0].pClientConnection[nClient]->pSocket->GetPacketsSent();
		if ( (i & 0xff) == 0)
			{
			str.Format(_T("[%5d]SendSlaveMsg bytes %d sent\n"), i, rc);
			TRACE(str);
			}
		}

	// Force app to sleep during system init after MAX_CMDS_BEFORE_SLEEP have been sent
//	if (gSysInitFlag)
		{
		m_nMsgBlock++;
		if (m_nMsgBlock >= MAX_CMDS_BEFORE_SLEEP)
			{
			Sleep(20);	//crash in PAM at 5 ms sleep
			m_nMsgBlock = 0;
			}
		}
	return TRUE;
	}


BOOL CTcpThreadRxList::SendSlaveMsg(WORD nMsgId, WORD par1, WORD par2, WORD par3, WORD par4, WORD par5, WORD par6)
{
	MMI_CMD sendBuf;	// The temporary storage for the message creation
	WORD *pWArg;
	float *pFArg;
	int i;  /* generic looper */

	CString str;
	str.Format(_T("0x%X"),nMsgId);
	memset(&sendBuf, 0, sizeof(MMI_CMD));

	pWArg = (WORD *) &sendBuf.CmdBuf[0];
	pFArg = (float *) &sendBuf.CmdBuf[0];

	//int rc;
	sendBuf.MsgId = nMsgId;
	sendBuf.ChnlNum = par6;

	/* pass down these parameters if not TCG_FN message */
	pWArg[0] = par1;
	pWArg[1] = par2;
	pWArg[2] = par3;
	pWArg[3] = par4;
	pWArg[4] = par5;
	pWArg[5] = par6;

	switch (nMsgId)
		{
	case TCG_FN:
		for (i=0; i<5; i++)
			{
			pFArg[i] = m_pConfigRec->receiver.TcgRec[m_nActiveChannel].CurveCoef[i];
			}
	default:
		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);

	case SYSINIT:
		gSysInitFlag = 1;
		// jeh  02-04-13 added to slow message sending during sysinit
		//m_nMsgBlock = 0;
		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);

	case SYSINIT_COMPLETE:
		gSysInitFlag = 0;
		//m_nMsgBlock = 0;
		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
		}	// switch (nMsgId)


	/* send TCG curve coefficients if TCG_FN message */
	if (nMsgId == TCG_FN)
	{
		for (i=0; i<5; i++)
		{
			pFArg[i] = m_pConfigRec->receiver.TcgRec[m_nActiveChannel].CurveCoef[i];
		}
	}

#if Test_New_Server_Connection_Management
	if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return FALSE;
		}
		//WaitMicroseconds(2000);
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);

#else

	return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
	if (stSCM[0].pClientConnection[0])
		{
		if (stSCM[0].pClientConnection[0]->bConnected)
			{
			rc = stSCM[0].pClientConnection[0]->pSocket->SendPacket((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
			if ( rc != sizeof(MMI_CMD)+4)
				{
				str.Format(_T("SendSlaveMsg bytes %d sent not equal msg size\n"),rc);
				TRACE(str);
				}
			else
				{
				str.Format(_T("SendSlaveMsg bytes %d sent\n"), rc);
				TRACE(str);
				}
			}
		}
		return TRUE;
#endif
#endif
	//}
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
	return FALSE;
}


/*************************************************/
/* SetGateDelay - set the gate delay             */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* Delay: The gate delay in microseconds         */
/*************************************************/

bool CTcpThreadRxList::SetGateDelay(short nChannel, short nGate, float Delay)
{
	WORD nDelay;
	float TruscopeDelay;

	nDelay = (short) (Delay * m_DelayScaling + m_DelayOffset);
	TruscopeDelay = Delay*((float) m_TruscopDelayScaling) + ((float) m_TruscopDelayOffset) + 0.5f;
	m_pConfigRec->gates.delay[nChannel][nGate] = (short) TruscopeDelay;  // to be consistent with the old Truscope code

	if (SendSlaveMsg(0x2B, nDelay, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;

}

float CTcpThreadRxList::GetGateDelay(short nChannel, short nGate)
{
	float Delay;

	//Delay  = (float) (m_pConfigRec->gates.delay[nChannel][nGate] - m_DelayOffset)/((float) m_DelayScaling);
	Delay  = (float) (m_pConfigRec->gates.delay[nChannel][nGate] - m_TruscopDelayOffset)/((float) m_TruscopDelayScaling);;

	return Delay;
}

/*************************************************/
/* SetGateRange - set the gate range             */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* Range: The gate range in microseconds         */
/*************************************************/

bool CTcpThreadRxList::SetGateRange(short nChannel, short nGate, float Range)
{
	WORD nRange;

	nRange = (short) (Range * m_RangeScaling + m_RangeOffset);
	m_pConfigRec->gates.range[nChannel][nGate] = (short) (Range * m_TruscopRangeScaling + m_TruscopRangeOffset + 0.5f);

	if (SendSlaveMsg(0x2D, nRange, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

float CTcpThreadRxList::GetGateRange(short nChannel, short nGate)
{
	float Range;

	//Range  = (float) (m_pConfigRec->gates.range[nChannel][nGate] - m_RangeOffset)/((float) m_RangeScaling);
	Range  = (float) (m_pConfigRec->gates.range[nChannel][nGate] - m_TruscopRangeOffset)/((float) m_TruscopRangeScaling);

	return Range;
}

/*************************************************/
/* SetGateLevel - set the gate threshold         */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* Level: The gate level in percentage           */
/*************************************************/

bool CTcpThreadRxList::SetGateLevel(short nChannel, short nGate, float Level)
{
	WORD nLevel;

	nLevel = (short) (Level * m_LevelScaling + m_LevelOffset);
	m_pConfigRec->gates.level[nChannel][nGate] = (short) (Level * m_TruscopLevelScaling + m_TruscopLevelOffset);  // to be consistent with the old Truscope code

	if (SendSlaveMsg(0x2C, nLevel, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

float CTcpThreadRxList::GetGateLevel(short nChannel, short nGate)
{
	float Level;

	//Level = (float) (m_pConfigRec->gates.level[nChannel][nGate] - m_LevelOffset) / ((float) m_LevelScaling);
	Level = (float) (m_pConfigRec->gates.level[nChannel][nGate] - m_TruscopLevelOffset) / ((float) m_TruscopLevelScaling);

	return Level;
}

/*************************************************/
/* SetGateBlank - set the gate blanking          */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* Blank: The gate blank in microseconds         */
/*************************************************/

bool CTcpThreadRxList::SetGateBlank(short nChannel, short nGate, float Blank)
{
	WORD nBlank;

	nBlank = (short) (Blank * m_BlankScaling + m_BlankOffset);
	m_pConfigRec->gates.blank[nChannel][nGate] = (short) (Blank * m_TruscopBlankScaling + m_TruscopBlankOffset + 0.5f);

	if (SendSlaveMsg(0x2E, nBlank, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

float CTcpThreadRxList::GetGateBlank(short nChannel, short nGate)
{
	float Blank;
	
	//Blank = (float) (m_pConfigRec->gates.blank[nChannel][nGate] - m_BlankOffset) / ((float) m_BlankScaling);
	Blank = (float) (m_pConfigRec->gates.blank[nChannel][nGate] - m_TruscopBlankOffset) / ((float) m_TruscopBlankScaling);

	return Blank;
}

/*************************************************/
/* SetGateTrigMode - set the gate trigger mode   */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nTrigMode: The gate trigger mode              */
/* Disable:               0                      */
/* Initial Pulse:         1                      */
/* Interface Echo:        2                      */
/* Slave A:               3                      */
/* Slave B:               4                      */
/*************************************************/

bool CTcpThreadRxList::SetGateTrigMode(short nChannel, short nGate, short nTrigMode)
{
	m_pConfigRec->gates.trg_option[nChannel][nGate] = (char) nTrigMode;

	if (SendSlaveMsg(0x2F, (WORD) nTrigMode, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;

}

short CTcpThreadRxList::GetGateTrigMode(short nChannel, short nGate)
{
	return ((short) m_pConfigRec->gates.trg_option[nChannel][nGate]);
}

/*************************************************/
/* SetGateTofTrig - set the gate TOF trigger     */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nTofTrig: The gate TOF trigger mode           */
/* Disable:               0                      */
/* Initial Pulse:         1                      */
/* Gate 1 Interface Echo: 2                      */
/* Peak Detect:           3                      */
/*************************************************/

bool CTcpThreadRxList::SetGateTofTrig(short nChannel, short nGate, short nTofTrig)
{
	m_pConfigRec->timeoff.trigger[nChannel][nGate] = (char) nTofTrig;

	if (SendSlaveMsg(0x52, (WORD) nTofTrig, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

short CTcpThreadRxList::GetGateTofTrig(short nChannel, short nGate)
{
	return ((short) m_pConfigRec->timeoff.trigger[nChannel][nGate]);
}

/*************************************************/
/* SetGateTofStopon - set the gate TOF stopon    */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nTofStopon: The gate TOF stopon selection     */
/* Threshold Detect:      0                      */
/* Peak Detect:           1                      */
/*************************************************/

bool CTcpThreadRxList::SetGateTofStopon(short nChannel, short nGate, short nTofStopon)
{
	m_pConfigRec->timeoff.stopon[nChannel][nGate] = (char) nTofStopon;

	if (SendSlaveMsg(0x51, (WORD) nTofStopon, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

short CTcpThreadRxList::GetGateTofStopon(short nChannel, short nGate)
{
	return ((short) m_pConfigRec->timeoff.stopon[nChannel][nGate]);
}


/*************************************************/
/* SetGateDetectMode - set the gate detect mode  */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nDetectMode: The gate detect mode             */
/* RF: 0                                         */
/* Full wave: 1                                  */
/*************************************************/

bool CTcpThreadRxList::SetGateDetectMode(short nChannel, short nGate, short nDetectMode)
{
	m_pConfigRec->gates.det_option[nChannel][nGate] = (char) nDetectMode;

	if (!SendSlaveMsg(0x27, (WORD) nDetectMode, 0, 0, 0, 0, 0 ))
		return false;

	for (int i=0; i<5; i++)
	{
		if (!SetScopeTrace2ShuntGateDac(nChannel, i))
			return FALSE;
	}

	return TRUE;
}

short CTcpThreadRxList::GetGateDetectMode(short nChannel, short nGate)
{
	return ((short) m_pConfigRec->gates.det_option[nChannel][nGate]);
}


/*************************************************/
/* SetGatePolarity - set the receiver polarity   */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nRcvrPol: The receiver polarity               */
/* Plus:  0                                      */
/* Minus: 1                                      */
/*************************************************/

bool CTcpThreadRxList::SetGatePolarity(short nChannel, short nGate, short nGatePol)
{
	m_pConfigRec->gates.polarity[nChannel][nGate] = (char) nGatePol;

	if (SendSlaveMsg(0x26, (WORD) nGatePol, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

short CTcpThreadRxList::GetGatePolarity(short nChannel, short nGate)
{
	return ((short) m_pConfigRec->gates.polarity[nChannel][nGate]);
}


bool CTcpThreadRxList::SetReceiverProcess(short nChannel)
{
	WORD nProcess = (WORD) m_pConfigRec->receiver.process[nChannel];
	WORD nAreaCoef = (WORD) m_pConfigRec->receiver.AreaCoef[nChannel];

	if (SendSlaveMsg(RECEIVER_PROCESS, nProcess, nAreaCoef, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

/*************************************************/
/* SetRcvrFilter - set the receiver filter select*/
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nFilterSel: The receiver filter selection     */
/* 5 MHz:    3                                   */
/* 3.5 MHz:  2                                   */
/* 2.25 MHz: 1                                   */
/* Wideband: 0                                   */
/*************************************************/

bool CTcpThreadRxList::SetRcvrFilter(short nChannel, short nFilterSel)
{
	//m_pConfigRec->receiver.filter[nChannel] = (char) nFilterSel;
	m_pConfigRec->receiver.fil_option[nChannel] = (char) nFilterSel;
	//if (WritePulserConfigToRioBoard())
	if (SendSlaveMsg(0x25, (WORD) nFilterSel, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

short CTcpThreadRxList::GetRcvrFilter(short nChannel)
{
	return (m_pConfigRec->receiver.fil_option[nChannel]);
}

/*************************************************/
/* SetRcvrOffset - set the receiver offset       */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* nRcvrOffset: The receiver offset (-25 to 25)   */
/*************************************************/

bool CTcpThreadRxList::SetRcvrOffset(short nChannel,  short nRcvrOffset)
{
	m_pConfigRec->receiver.offset[nChannel] = (int) nRcvrOffset;
	//if (WritePulserConfigToRioBoard())
	if (SendSlaveMsg(RECEIVER_FCNT, (WORD) nRcvrOffset, (WORD) nChannel, 0, 0, 0, 0))
		return true;
	else
		return false;
}

short CTcpThreadRxList::GetRcvrOffset(short nChannel)
{
	return (m_pConfigRec->receiver.offset[nChannel]);
}

/*************************************************/
/* SetRcvrGain - set the receiver Gain           */
/*                                               */ 
/* nChannel:  Channel number                     */
/* nGate:  Gate number                           */
/* Gain: The receiver gain in dB                  */
/*************************************************/

bool CTcpThreadRxList::SetRcvrGain(short nChannel,  float Gain)
{
	WORD nGain;

	nGain = (short) (Gain * m_GainScaling + m_GainOffset);
	m_pConfigRec->receiver.gain[nChannel] = (short) (Gain * m_TruscopGainScaling + m_TruscopGainOffset);

	if ( GetTcgTrigSel(nChannel) == 0 )
	{
		m_pConfigRec->receiver.TcgRec[nChannel].IdGain = Gain;
		m_pConfigRec->receiver.TcgRec[nChannel].OdGain = Gain;
	}

	if (SendSlaveMsg(0x22, (WORD) nGain, 0, 0, 0, 0, (WORD) nChannel ))
		return true;
	else
		return false;
}

float CTcpThreadRxList::GetRcvrGain(short nChannel)
{
	float Gain;

	//Gain = ((float) (m_pConfigRec->receiver.gain[nChannel] - m_GainOffset))/((float) m_GainScaling);
	Gain = ((float) (m_pConfigRec->receiver.gain[nChannel] - m_TruscopGainOffset))/((float) m_TruscopGainScaling);

	return Gain;
}

/*************************************************/
/* SetPulserPRF - set the Pulser PRF             */
/*                                               */ 
/* nPRF: The pulser PRF in Hz                    */
/*************************************************/

bool CTcpThreadRxList::SetPulserPRF(short nPRF)
{
	WORD nMode;

	m_pConfigRec->pulser.prf = (short) (nPRF/10);  // to be consistent with the old Truscope code

	if (m_pConfigRec->pulser.mode == 0)
		nMode = 0x1;
	else
		nMode = 0x11;

	//if (WritePulserConfigToRioBoard())
	if (!SendSlaveMsg(PULSER_PRF, (WORD) nMode, (WORD) nPRF, 0, 0, 0, 0 ))
		return false;

	WaitMicroseconds(500);

	return TRUE;
}

/*************************************************/
/* SetPulserWidth - set the Pulser width for     */
/*                  channel # nChannel           */
/*                                               */ 
/* nPulseWidth: The pulse width in nanoseconds   */
/*************************************************/

bool CTcpThreadRxList::SetPulserWidth(short nChannel, short nPulseWidth)
{
	m_pConfigRec->pulser.pulse_width[nChannel] = (short) (nPulseWidth/10);  // to be consistent with the old Truscope code.
																			 // The step is 100 ns, but it can actually be 10 ns for the 2-channel-4-gate board
		
	// Write pulse width to location IopSpace0+0x360
	if (SendSlaveMsg(0x1B, (WORD) nPulseWidth, 0, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}

/*************************************************/
/* SetPulserMode - set the Pulser Mode           */
/*                                               */ 
/* nMode: The pulser mode:                       */
/*     PRF: 0x01         DENSITY: 0x11           */
/*************************************************/

bool CTcpThreadRxList::SetPulserMode(BYTE nMode)
{
	WORD iMode;

	m_pConfigRec->pulser.mode = (BYTE) nMode;

	if (m_pConfigRec->pulser.mode == 0)
		iMode = 0x1;
	else
		iMode = 0x11;

	WORD PRF = (WORD) m_pConfigRec->pulser.prf*10;
	//if (WritePulserConfigToRioBoard())
	if (SendSlaveMsg(0x1C, (WORD) iMode, PRF, 0, 0, 0, 0 ))
		return true;
	else
		return false;
}


/*************************************************/
/* SetPrfIntExt - set to Master/Slave instrument */
/*                                               */ 
/* nSlave: The slave ID to be set to             */
/* 1 for Master instrument                       */
/* 0 for Slave instrument                        */
/*************************************************/

bool CTcpThreadRxList::SetPrfIntExt(short nSlave)
{
	WORD nPrfIntExt = m_pConfigRec->pulser.LocalPRF[nSlave];
	WORD nPulserMode = m_pConfigRec->pulser.mode;

	if ( SendSlaveMsg(PULSER_ONOFF, (WORD) nSlave, nPrfIntExt, nPulserMode, 0,0,0) )
		return TRUE;

	return FALSE;
}


/*************************************************/
/* SetScopeTrace1DisplaySel - set the scope      */
/* trace 1 to RF or Normal mode.                 */ 
/*                                               */
/* nChannel:  Channel number                     */
/*                                               */
/* Trace1DispSel: The trace 1 display mode       */
/*     0 = Normal,  1 = RF                       */
/*************************************************/

bool CTcpThreadRxList::SetScopeTrace1DisplaySel(short nChannel,  unsigned short Trace1DispSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.T1Button[nChannel] = Trace1DispSel;
	if (WriteScopeSettingsToBoard())
		return true;
	else
		return false;
}

unsigned short CTcpThreadRxList::GetScopeTrace1DisplaySel(short nChannel)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.T1Button[nChannel];
}

/****************************************************/
/* SetScopeTrace1TestSel - set the scope            */
/* trace 1 diagnostic test selections.              */ 
/*                                                  */
/* nChannel:  Channel number                        */
/*                                                  */
/* Trace1TestSel: The trace 1 diagnostic test point */
/*     0 = Receiver Out,  1 = Amplitude             */
/*     2 = TCG Curve,     3 = Analog Test           */
/****************************************************/

bool CTcpThreadRxList::SetScopeTrace1TestSel(short nChannel,  unsigned short Trace1TestSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.T1Indx[nChannel] = Trace1TestSel;
	if (WriteScopeSettingsToBoard())
		return true;
	else
		return false;
}

unsigned short CTcpThreadRxList::GetScopeTrace1TestSel(short nChannel)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.T1Indx[nChannel];
}

/****************************************************/
/* SetScopeTrace2DisplaySel - set the scope         */
/* trace 2 to display All Gates or Active Gate only */ 
/*                                                  */
/* nChannel:  Channel number                        */
/*                                                  */
/* Trace2DispSel: The trace 1 display mode          */
/*     0 = Active Gate,  1 = All Gates              */
/****************************************************/

bool CTcpThreadRxList::SetScopeTrace2DisplaySel(short nChannel,  unsigned short Trace2DispSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.T2Button[nChannel] = Trace2DispSel;
	if (WriteScopeSettingsToBoard())
		return true;
	else
		return false;
}

unsigned short CTcpThreadRxList::GetScopeTrace2DisplaySel(short nChannel)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.T2Button[nChannel];
}

/****************************************************/
/* SetScopeTrace2TestSel - set the scope            */
/* trace 2 diagnostic test selections.              */ 
/*                                                  */
/* nChannel:  Channel number                        */
/*                                                  */
/* Trace2TestSel: The trace 2 diagnostic test point */
/*     0 = Gate,          1 = IF Detection          */
/*     2 = Th Detection,  3 = PK Detection          */
/*     4 = Tf Gate,       5 = Slave IF A            */
/*     6 = Slave IF B                               */
/****************************************************/

bool CTcpThreadRxList::SetScopeTrace2TestSel(short nChannel,  unsigned short Trace2TestSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.T2Indx[nChannel] = Trace2TestSel;
	if (WriteScopeSettingsToBoard())
		return true;
	else
		return false;
}

unsigned short CTcpThreadRxList::GetScopeTrace2TestSel(short nChannel)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.T2Indx[nChannel];
}

/****************************************************/
/* SetScopeTrace2DiagnosSel - set the scope         */
/* trace 2 diagnostic selections.                   */ 
/*                                                  */
/* nChannel:  Channel number                        */
/*                                                  */
/* Trace2DiagnosSel: The trace 2 diagnostic point   */
/*     1 = Mbs,          2 = Eas                    */
/*     3 = Fos,          4 = Fof                    */
/*     5 = Ticks,        6 = Top                    */
/*     7 = Motion Pls    8 = Slave IF A             */
/*     9 = Slave If B                               */
/****************************************************/

bool CTcpThreadRxList::SetScopeTrace2DiagnosSel(short nChannel,  unsigned short Trace2DiagnosSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.T2DiagnosSel[nChannel] = Trace2DiagnosSel;
	if (WriteScopeSettingsToRioBoard())
		return true;
	else
		return false;
}


unsigned short CTcpThreadRxList::GetScopeTrace2DiagnosSel(short nChannel)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.T2DiagnosSel[nChannel];
}


bool CTcpThreadRxList::SetScopeTrigger(short nChannel, unsigned short nTrigSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.trigger[nChannel] = (BYTE) nTrigSel;
	if (WriteScopeSettingsToRioBoard())
		return true;
	else
		return false;
}


unsigned short CTcpThreadRxList::GetScopeTrigger(short nChannel)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.trigger[nChannel];
}


/****************************************************/
/* SetScopeTrace2GateSel - set the scope trace 2    */
/* gate selection when All Gates button is on.      */ 
/*                                                  */
/* nChannel:  Channel number                        */
/* nGate:     Gate number                           */
/*                                                  */
/* Trace2GateSel: The trace 2 gate on or off        */
/*     0 = off,          1 = on                    */
/****************************************************/

bool CTcpThreadRxList::SetScopeTrace2GateSel(short nChannel,  short nGate, unsigned short Trace2GateSel)
{
	nChannel = 0;
	m_pConfigRec->OscopeRec.Trace2GateSel[nChannel][nGate] = (BYTE) Trace2GateSel;
	if (WriteScopeSettingsToBoard())
		return true;
	else
		return false;
}

unsigned short CTcpThreadRxList::GetScopeTrace2GateSel(short nChannel,  short nGate)
{
	nChannel = 0;
	return m_pConfigRec->OscopeRec.Trace2GateSel[nChannel][nGate];
}

/********************************************************/
/* EnableScopeDisplay - enable or disable scope display */
/*                                                      */
/* nChannel:  Active Channel number                     */
/* nGate:     Active Gate number                        */
/*                                                      */
/* bEnabled:                                            */
/*     true = enabled,          false = disabled        */
/********************************************************/

void CTcpThreadRxList::EnableScopeDisplay(bool bEnabled, short nChannel, short nGate)
{
	m_bScopeEnabled = bEnabled;
	m_nActiveChannel = nChannel;
	m_nActiveGate = nGate;

	WriteScopeSettingsToBoard();
	WriteScopeSettingsToRioBoard();

	for (int i=0; i<5; i++)
	{
		SetScopeTrace2ShuntGateDac(nChannel, i);
	}
}

bool CTcpThreadRxList::WriteConfigToBoard()
{
	unsigned long level,blank,tof,alarm,tmp,PolMode;
	unsigned long buftemp[16];
	int jchnl, igate, j, i;
	int NUMBER_CHANNELS=2, NUMBER_GATES=MAX_GATE;
	int Gate_Offset=0x2000, Gain_Offset=0x30;
	int nActiveChnl = m_nActiveChannel;
	int nActiveGate = m_nActiveGate;

	// Write only if we have a valid board handle
	if (1)
	{
		for (jchnl =0; jchnl < NUMBER_CHANNELS;jchnl++)
		{
			PolMode = 0;
			level = 0;
			blank = 0;
			tof = 0;
			alarm=0;
			for (igate = 0; igate < NUMBER_GATES;igate++)
			{
				// Set gate range, delay, and trigger mode data bits
				buftemp[igate+jchnl*NUMBER_GATES] = (((unsigned long) m_pConfigRec->gates.range[jchnl][igate] << 18) ) // 14 +4
						  + (((unsigned long) m_pConfigRec->gates.delay[jchnl][igate] << 4)  ) 
						  + m_pConfigRec->gates.trg_option[jchnl][igate] ;
				// Set gate threshold data bits
				level += (unsigned long) m_pConfigRec->gates.level[jchnl][igate] << igate*8 ;
				// Set gate blanking pulse data bits
				blank += (unsigned long) m_pConfigRec->gates.blank[jchnl][igate] << igate*8 ;
				// Set gate time of flight trigger data bits
				tmp = (unsigned long) m_pConfigRec->timeoff.trigger[jchnl][igate] << 1;
				tmp += (unsigned long) m_pConfigRec->timeoff.stopon[jchnl][igate] ;
				tmp <<= (igate*4 +16) ;
				tof += tmp;
				// Set gate alarm trigger data bits
				tmp = 0;
				//tmp = (unsigned long) m_pConfigRec->alarm.polarity[jchnl][igate] ;
				//tmp += (unsigned long) m_pConfigRec->alarm.laminar[jchnl][igate] << 3 ;
				//tmp += (unsigned long) m_pConfigRec->alarm.trigger[jchnl][igate] << 1 ;
				//tmp <<= igate*4;
				alarm += tmp;
				// Set gate detection mode and polarity data bits
				tmp = (unsigned long) m_pConfigRec->gates.det_option[jchnl][igate]  << (igate+4);
				tmp += (unsigned long) m_pConfigRec->gates.polarity[jchnl][igate] << igate;
				PolMode += tmp;
			}
			buftemp[jchnl+NUMBER_GATES*NUMBER_CHANNELS]=level;
			buftemp[jchnl+NUMBER_GATES*NUMBER_CHANNELS+NUMBER_CHANNELS]=blank;
			buftemp[jchnl+NUMBER_GATES*NUMBER_CHANNELS+NUMBER_CHANNELS+NUMBER_CHANNELS]=tof+alarm;
			buftemp[jchnl+NUMBER_GATES*NUMBER_CHANNELS+NUMBER_CHANNELS*3]=PolMode;
			//buf[j+NUMBER_GATES*NUMBER_CHANNELS]=alarm;
		}

		// Write 64 bytes to IOP Space 1 starting from Space1+Gate_Offset.
		// It is the 2 Channel Gate Board sequence block look-up table
		// for the gate range, delay, trigger mode, threshold, blanking, time-of-flight trigger,
		// gate detection mode, and gate receiver polarity.
/*
		rtnCode = PlxBusIopWrite(m_hPlxBoard, IopSpace1, Gate_Offset, FALSE, buftemp,0x40, BitSize32);
		if (rtnCode != ApiSuccess)
			return false;;
*/
		if (g_nClientSocket >= 0)
		{
			for (i=0; i<2; i++)
			{
				for (j=0; j<4; j++)
				{
					EnableScopeDisplay(m_bScopeEnabled, i, j);  // need this 'cause slave assumes active channel and gate
					if (!SetGateDelay(i, j, GetGateDelay(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetGateRange(i, j, GetGateRange(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetGateLevel(i, j, GetGateLevel(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetGateBlank(i, j, GetGateBlank(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetGateDetectMode(i, j, GetGateDetectMode(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetGatePolarity(i, j, GetGatePolarity(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetGateTrigMode(i, j, GetGateTrigMode(i, j)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetRcvrGain(i, GetRcvrGain(i)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetRcvrFilter(i, GetRcvrFilter(i)))
						return FALSE;
					WaitMicroseconds(1000);

					if (!SetRcvrOffset(i, GetRcvrOffset(i)))
						return FALSE;
					WaitMicroseconds(1000);
				}
			}
			// back to original active channel and gate, as shown on the screen.
			EnableScopeDisplay(m_bScopeEnabled, nActiveChnl, nActiveGate);
		}
	}
	else 
	{
      // always return false if there is not a valid board handle.
		return false;
	}

	return true;
}

bool CTcpThreadRxList::WriteScopeSettingsToBoard()
{
return true;
	unsigned long temp;
	unsigned long bufScopeRegister;
	WORD par1, par2;  // parameters to send to slave.
	int  Trace1TestSel;
	//int nChannel = m_nActiveChannel;
	int nChannel = 0;

	// Write to active channel register
	par1 = (WORD) m_nActiveChannel;
	par2 = 0;
	if (!SendSlaveMsg(0x15, par1, par2, 0, 0, 0, 0))
		return FALSE;
	WaitMicroseconds(100);

	// Write to active gate register
	par1 = (WORD) m_nActiveGate;
	par2 = 0;
	if (!SendSlaveMsg(0x14, par1, par2, 0, 0, 0, 0))
		return FALSE;
	WaitMicroseconds(100);

	// Write scope settings
	if (m_bScopeEnabled)
	{
		bufScopeRegister = 0x00000001 << 31;  // Set data bit [31], board enabled for scope display
	}
	else
	{
		bufScopeRegister = 0;  // Set data bit [31], board disabled for scope display
	}

	// Set data bits [27..24] for Analog Multiplexer
	Trace1TestSel = m_pConfigRec->OscopeRec.T1Indx[nChannel];
	if (Trace1TestSel == 0  || Trace1TestSel == 1) {
		temp = 0x00000002 << 24;
		bufScopeRegister += temp;
	}
	if (Trace1TestSel == 2) {
		temp = 0x00000000 << 24;
		bufScopeRegister += temp;
	}
	if (Trace1TestSel == 3) {
		temp = 0x00000001 << 24;
		bufScopeRegister += temp;
	}

	// Set data bit [20] for RF Out
	if (m_pConfigRec->OscopeRec.T1Button[nChannel] == 0) {  // Normal
		temp = 0x00000000 << 20;
		bufScopeRegister += temp;
	}
	else {  // RF
		temp = 0x00000001 << 20;
		bufScopeRegister += temp;
	}

	// Set data bits [18..16] for Receiver Out or Hi-Res Max Amplitude Test Point
	if (Trace1TestSel == 0) {
		temp = 0x00000000 << 16;
		bufScopeRegister += temp;
	}
	if (Trace1TestSel == 1) {
		temp = 0x00000001 << 16;
		bufScopeRegister += temp;
	}

	// Set data bit [8]
	if (m_pConfigRec->OscopeRec.T2Button[nChannel] == 1) {  // All Gates
		temp = 0x00000001 << 8;
		bufScopeRegister += temp;
	}
	else {  // Active Gate only
		temp = 0x00000000 << 8;
		bufScopeRegister += temp;
	}

	// Set data bit [7..4]
	//if (m_pConfigRec->OscopeRec.Trace2GateSel[m_nActiveChannel][0] == 1) {
		temp = 0x00000001 << 4;
		bufScopeRegister += temp;
	//}
	//if (m_pConfigRec->OscopeRec.Trace2GateSel[m_nActiveChannel][1] == 1) {
		temp = 0x00000001 << 5;
		bufScopeRegister += temp;
	//}
	//if (m_pConfigRec->OscopeRec.Trace2GateSel[m_nActiveChannel][2] == 1) {
		temp = 0x00000001 << 6;
		bufScopeRegister += temp;
	//}
	//if (m_pConfigRec->OscopeRec.Trace2GateSel[m_nActiveChannel][3] == 1) {
		temp = 0x00000001 << 7;
		bufScopeRegister += temp;
	//}

	// Set data bit [3..0]
	if (m_pConfigRec->OscopeRec.T2Indx[nChannel] < 8) {
		temp = (unsigned long) m_pConfigRec->OscopeRec.T2Indx[nChannel];
		bufScopeRegister += temp;
	}
/*
	if (m_pConfigRec->OscopeRec.T2Indx[m_nActiveChannel] == 5) {
		temp = 0x0000000E;
		bufScopeRegister += temp;
	}
	if (m_pConfigRec->OscopeRec.T2Indx[m_nActiveChannel] == 6) {
		temp = 0x0000000F;
		bufScopeRegister += temp;
	}
*/

	par1 = (WORD) (bufScopeRegister & 0x0000FFFF);
	par2 = (WORD) ((bufScopeRegister >> 16) & 0x0000FFFF);
	if (!SendSlaveMsg(0x4A, par1, par2, 0, 0, 0, 0))   /* SET_SCOPE_MUX = 0x4A */
		return FALSE;
	WaitMicroseconds(100);

	return true;
}

// Read amplitudes and TOFs from the board, and place the
// results into the arrays m_Amplitude[channel][gate] and m_TOF[channel][gate]
bool CTcpThreadRxList::ReadAmplitudeTOF()
{

	unsigned long *bufReadAmpTof;
	MMI_CMD readBuf;

	if (!SendSlaveMsg(0x32, 0, 0, 0, 0, 0, 0))
		return FALSE;

	if (g_nClientSocket >= 0)
		{
		// this is a different code implementation of readn. This is a member of the class CTcpThreadRxList
		//if (readn(g_nClientSocket, (char *) &readBuf, sizeof(MMI_CMD)) < 0)
		if (::readn(g_nClientSocket, (char *) &readBuf, sizeof(MMI_CMD)) < 0)
			return FALSE;
		}

	bufReadAmpTof = (DWORD *) &readBuf.CmdBuf[0];

/*
	if (m_hPlxBoard)
	{
		rc = PlxBusIopRead(m_hPlxBoard, IopSpace0, 0x0700, FALSE, &bufReadAmpTof[0], 0x20, BitSize32);
		if (rc != ApiSuccess) {
			return false;
		}
	}
	else
	{
		return false;
	}
*/

	unsigned long temp1, temp2, temp3;
	int i, j;
	temp1 = 0x000000ff;
	temp2 = 0x0000ffff;
	temp3 = 0x00000001;
	m_AlarmBitFlag = 0;
	for (i=0; i<2; i++)
	{
		for (j=0; j<4; j++)
		{
			m_Amplitude[i][j] = (float) (((bufReadAmpTof[i*4+j]>>8) & temp1) * 0.5f);
			m_TOF[i][j] = (float) (((bufReadAmpTof[i*4+j]>>16) & temp2) * 0.005f);
			m_AlarmBitFlag |= (((bufReadAmpTof[i*4+j]) & temp3) << (i*4+j));
		}
	}

	return true;
}

bool CTcpThreadRxList::WriteScopeSettingsToRioBoard()
{
return true;
	unsigned long temp;
	unsigned long bufScopeRegister;
	int SeqLen;
	int i;
	int nChannel = 0;

	bufScopeRegister = 0; 

	// Set data bit [13..12], active gate#
	bufScopeRegister += m_nActiveGate << 12;

	// Set data bit [11..8]
	temp = (unsigned long) m_pConfigRec->OscopeRec.T2DiagnosSel[nChannel];
	bufScopeRegister += temp << 8;

	// Set data bit [7..4].  Scope trigger sequence selection
	SeqLen = m_pConfigRec->UtRec.Shoe[m_nActiveChannel/10].sequence_length;
	for (i=0; i<SeqLen; i++)
	{
		if (m_pConfigRec->UtRec.Shoe[m_nActiveChannel/10].Ch[m_nActiveChannel%10].Rcvr_Sequence & (0x1<<i))
		{
			temp = (unsigned long) i;
			bufScopeRegister += temp << 4;
			break;
		}
	}

	// Set data bit [3..0].  Scope trigger selection
	temp = (unsigned long) m_pConfigRec->OscopeRec.trigger[nChannel];
	bufScopeRegister += temp;

	//CString str;
	//str.Format(_T("Data bits [15..12] = %d"),((unsigned long) ((bufScopeRegister & 0x0000F000)>>12)));
	//AfxMessageBox(str);
/*
	if (m_hPlxBoard) {
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x900, FALSE, &bufScopeRegister,0x02, BitSize16);
		if (rcd != ApiSuccess) {
			return false;
		}
	}
	else {
		return false;
	}
*/
	WORD par1 = (WORD) bufScopeRegister;
	if (SendSlaveMsg(0x4B, par1, 0, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;

/*
	// Initialize alarm command by writing data 0x00000F0F to s0+0x410 (temporary)
	bufScopeRegister = 0x00000F0F;
	if (m_hPlxBoard) {
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x410, FALSE, &bufScopeRegister,0x04, BitSize32);
		if (rcd != ApiSuccess) {
			return false;
		}
	}
	else {
		return false;
	}
*/

	return true;
}

#if 0 //original
bool CTcpThreadRxList::SetScopeTrace2ShuntGateDac(short nChannel, int nShuntGateSel)
{
	unsigned long temp;

	temp = 0;
	switch(nShuntGateSel)
	{
	case 0:  // Set shunt Dac
		if (m_pConfigRec->gates.det_option[nChannel][m_nActiveGate] == 0)
			temp = m_pConfigRec->OscopeRec.rf_shunt << 4;
		else
			temp = m_pConfigRec->OscopeRec.fw_shunt << 4;
		temp += 0x4;
		break;
	case 1:  // Set Gate 1 Dac
		temp = 0x8;
		break;
	case 2:  // Set Gate 2 Dac
		temp = 0x9;
		break;
	case 3:  // Set Gate 3 Dac
		temp = 0xA;
		break;
	case 4:  // Set Gate 4 Dac
		temp = 0xB;
		break;
	default:
		return false;
		break;
	}

	if (nShuntGateSel != 0)
	{
		int GateFullScale = m_pConfigRec->OscopeRec.gate_FullScale[nShuntGateSel-1];
		int GateZero = m_pConfigRec->OscopeRec.gate_zero[nShuntGateSel-1];
		int GateSpan = GateFullScale - GateZero;
		float GateLevel = 0.01*(m_pConfigRec->gates.level[nChannel][nShuntGateSel-1] - m_TruscopLevelOffset)/((float) m_TruscopLevelScaling);//
		if (m_pConfigRec->gates.det_option[nChannel][nShuntGateSel-1] == 0)//
			temp += ((unsigned long) (0.5*(GateLevel*GateSpan+GateSpan) + GateZero)) << 4;
		else
			temp += ((unsigned long) (GateLevel*GateSpan + GateZero)) << 4;
	}

	/* send the shunt and gate MDAC to slave */
	WORD par1 = (WORD) temp;
	if (SendSlaveMsg(0x4C, par1, 0, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;

	return true;
}
#endif


#if 1
bool CTcpThreadRxList::SetScopeTrace2ShuntGateDac(short nChannel, int nShuntGateSel)
{
	unsigned long temp;
	int nInstrument = nChannel / MAX_CHANNEL_PER_INSTRUMENT;

	temp = 0;
	switch(nShuntGateSel)
	{
	case 0:  // Set shunt Dac
		if (m_pConfigRec->gates.det_option[nChannel][m_nActiveGate] == 0)
			temp = m_pConfigRec->OscopeRec.fw_shunt[nInstrument] << 4;
		else
			temp = m_pConfigRec->OscopeRec.fw_shunt[nInstrument] << 4;
		temp += 0x4;
		break;
	case 1:  // Set Gate 1 Dac
		temp = 0x8;
		break;
	case 2:  // Set Gate 2 Dac
		temp = 0x9;
		break;
	case 3:  // Set Gate 3 Dac
		temp = 0xA;
		break;
	case 4:  // Set Gate 4 Dac
		temp = 0xB;
		break;
	default:
		return false;
		break;
	}

	if (nShuntGateSel != 0)
	{
		int GateFullScale = m_pConfigRec->OscopeRec.gate_FullScale[nInstrument][nShuntGateSel-1];
		int GateZero = m_pConfigRec->OscopeRec.gate_zero[nInstrument][nShuntGateSel-1];
		int GateSpan = GateFullScale - GateZero;
		float GateLevel = 0.01f*(m_pConfigRec->gates.level[nChannel][nShuntGateSel-1] - m_TruscopLevelOffset)/((float) m_TruscopLevelScaling);//
		//if (m_pConfigRec->gates.det_option[nChannel][nShuntGateSel-1] == 0)//
			//temp += ((unsigned long) (0.5*(GateLevel*GateSpan+GateSpan) + GateZero)) << 4;
		//else
			temp += ((unsigned long) (GateLevel*GateSpan + GateZero)) << 4;
	}

	/* send the shunt and gate MDAC to slave */
	WORD par1 = (WORD) temp;
	if (SendSlaveMsg(0x4C, par1, 0, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;

	return true;
}
#endif


bool CTcpThreadRxList::EnableFireSequence(short nChannel, bool bEnabled)
{
	unsigned long temp;
	WORD  Fire_Sequence = m_pConfigRec->UtRec.Shoe[nChannel/10].Ch[nChannel%10].Fire_Sequence;
	WORD  Rcvr_Sequence = m_pConfigRec->UtRec.Shoe[nChannel/10].Ch[nChannel%10].Rcvr_Sequence;

	// Set pulser PRF
	temp = 0xFFFFFFFF;

	if (SendSlaveMsg(0x80, nChannel, Fire_Sequence, Rcvr_Sequence, 0, 0, 0))
		return TRUE;
	else
		return FALSE;
/*
	if (m_hPlxBoard) {
		//set to page 1
		temp = 1;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x0, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
		//set fire sequence bits
		temp = 0xFFFFFFFF;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x100, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
		WaitMicroseconds(20);
		//set back to page 0
		temp = 0;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x0, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
	}
	else {
		return false;
	}

	return true;
*/
}

bool CTcpThreadRxList::EnableReceiveSequence(short nChannel, bool bEnabled)
{
	unsigned long temp;

	// Set pulser PRF
	temp = 0xFFFFFFFF;
/*
	if (m_hPlxBoard) {
		//set to page 1
		temp = 1;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x0, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
		//set receive sequence bits
		temp = 0xFFFFFFFF;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x110, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
		WaitMicroseconds(20);
		//set back to page 0
		temp = 0;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x0, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
	}
	else {
		return false;
	}
*/
	return true;
}

bool CTcpThreadRxList::SetSequenceLength(short nChannel)
{
	WORD nSeqLength = (WORD) m_pConfigRec->UtRec.Shoe[nChannel/10].sequence_length ;

	if (SendSlaveMsg(0x69, nChannel, nSeqLength, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;

/*
	if (!WritePulserConfigToRioBoard())
		return false;

	return true;
*/
}

void CTcpThreadRxList::SetTcgAarg(short nChannel, short nTcgAarg)
{
	//m_pConfigRec->receiver.arg1[nChannel] = (short) nTcgAarg;
}

void CTcpThreadRxList::SetTcgBarg(short nChannel, short nTcgBarg)
{
	//m_pConfigRec->receiver.arg2[nChannel] = (short) nTcgBarg;
}

bool CTcpThreadRxList::SetTcgUpdateRate(short nChannel, unsigned short nUpdateRate)
{
	int i;

	if (nChannel < 4)
	{
		for (i=0; i<4; i++)
			m_pConfigRec->receiver.tcg_step[i] = (unsigned char)nUpdateRate;
	}
	else
		m_pConfigRec->receiver.tcg_step[nChannel] = (unsigned char)nUpdateRate;

	if (!WriteTcgStepTrigSel(nChannel))
		return false;

	return true;
}

unsigned short CTcpThreadRxList::GetTcgUpdateRate(short nChannel)
{
	unsigned short nUpdaterate = (unsigned short) m_pConfigRec->receiver.tcg_step[nChannel];

	return nUpdaterate;
}

bool CTcpThreadRxList::SetTcgTrigSel(short nChannel, unsigned short nTrigSel)
{
	m_pConfigRec->receiver.tcg_trigger[nChannel] = (unsigned char)nTrigSel;
	if (!WriteTcgStepTrigSel(nChannel))
		return false;

	if (nTrigSel == 0)
	{
		if (!SetRcvrGain(nChannel,GetRcvrGain(nChannel)))
			return false;
	}

	return true;
}

unsigned short CTcpThreadRxList::GetTcgTrigSel(short nChannel)
{
	unsigned short nTrigSel = m_pConfigRec->receiver.tcg_trigger[nChannel];

	return nTrigSel;
}

bool CTcpThreadRxList::WriteTcgStepTrigSel(short nChannel)
{
	unsigned long buf;
	WORD wTcgTrigStep;

	// Set Tcg trigger
	buf = ((unsigned long) (m_pConfigRec->receiver.tcg_trigger[nChannel])) & 0x0000000F;
	//buf += (((unsigned long) m_pConfigRec->receiver.tcg_trigger[1]) & 0x0000000F) << 16;

	// Set Tcg step;
	buf += (((unsigned long) m_pConfigRec->receiver.tcg_step[nChannel]) & 0x0000000F) << 4;
	//buf += (((unsigned long) m_pConfigRec->receiver.tcg_step[1]) & 0x0000000F) << 20;

	wTcgTrigStep = (WORD) buf;

	if (SendSlaveMsg(0x63, wTcgTrigStep, (WORD) nChannel, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;

#if 0
	CString str0;
	str0.Format(_T("TCG step and trigger selection = %08X"),buf);
	AfxMessageBox(str0);
#endif

/*
	if (m_hPlxBoard) {
		//set to page 1
		temp = 1;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x0, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
		//set receive sequence bits
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x220, FALSE, &buf,0x02, BitSize16);
		if (rcd != ApiSuccess)
			return false;
		//set back to page 0
		temp = 0;
		rcd = PlxBusIopWrite(m_hPlxBoard, IopSpace0, 0x0, FALSE, &temp,0x04, BitSize32);
		if (rcd != ApiSuccess)
			return false;
	}
	else {
		return false;
	}

	return true;
*/
}


void CTcpThreadRxList::ComputeTcgGainTable(short nChannel)
{
		unsigned long GainCounts;
		float Gain, xGain, Distance, Distance2, Xmid, MaxChange;
		int iStart, iStop, iStop2;
		int i, j, k;
		int nBoard = nChannel/2;
		float Curve2Coef[2];

		/* load TCG curve coefficients */
		for (i=0; i<5; i++)
		{
			m_CurveCoef[nBoard*2][i] = m_pConfigRec->receiver.TcgRec[nBoard*2].CurveCoef[i];
			m_CurveCoef[nBoard*2+1][i] = m_pConfigRec->receiver.TcgRec[nBoard*2+1].CurveCoef[i];
		}

		for (j=0; j<2; j++)    // loop through two channels of this board
		{
			iStart = 5;
			iStop = 253;
			Distance = (float) (iStop - iStart);
			iStop2 = 100;
			Distance2 = (float) (iStop2 - iStart);
			Xmid = (float)(iStop-iStart)/2;

			for (i=0; i<iStop; i++)
			{
				if (i < iStart)
				{
					xGain = m_CurveCoef[nBoard*2+j][0];
				}
				else
				{
					if ( (i<=iStop2) && (m_pConfigRec->receiver.TcgRec[nBoard*2+j].Curve2On) )
					{
						Curve2Coef[0] = m_pConfigRec->receiver.TcgRec[nBoard*2+j].Curve2Coef[0];
						Curve2Coef[1] = m_pConfigRec->receiver.TcgRec[nBoard*2+j].Curve2Coef[1];
						xGain = Curve2Coef[0] + Curve2Coef[1]*(i-iStart);
					}
					else
					{
						MaxChange = m_CurveCoef[nBoard*2+j][2]*(Xmid)*(Xmid);
						if (i < Xmid)
							xGain = m_CurveCoef[nBoard*2+j][0] + 
									m_CurveCoef[nBoard*2+j][1]*(i-iStart) + 
									m_CurveCoef[nBoard*2+j][2]*(i-Xmid)*(i-Xmid) - MaxChange +
									m_CurveCoef[nBoard*2+j][3]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
						else
							xGain = m_CurveCoef[nBoard*2+j][0] + 
									m_CurveCoef[nBoard*2+j][1]*(i-iStart) + 
									m_CurveCoef[nBoard*2+j][2]*(i-Xmid)*(i-Xmid) - MaxChange +
									m_CurveCoef[nBoard*2+j][4]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
					}
				}
				if (xGain > 79.9f) xGain = 79.9f;
				if (xGain < 0) xGain = 0;
				GainCounts = (unsigned long) (xGain*m_GainScaling+m_GainOffset);
				if (j%2 == 0)
				{
					m_bufGainTable[i] &= 0xFFFF0000;
					m_bufGainTable[i] += GainCounts;
				}
				else
				{
					m_bufGainTable[i] &= 0x0000FFFF;
					m_bufGainTable[i] += (GainCounts << 16);
				}
			}
			m_bufGainTable[255] = m_bufGainTable[0];
			m_bufGainTable[254] = m_bufGainTable[0];
			m_bufGainTable[253] = m_bufGainTable[0];

			/* restore the gain in gate mode */
			for (i=0; i<4; i++)
			{
				if (m_pConfigRec->receiver.TcgRec[nBoard*2+j].GateOn[i])
				{
					iStart = (int) (GetGateDelay(nBoard*2+j,i)/((GetTcgUpdateRate(nBoard*2+j)+1)*TCG_STEP_INCREMENT));
					iStop = (int) (GetGateRange(nBoard*2+j,i)/((GetTcgUpdateRate(nBoard*2+j)+1)*TCG_STEP_INCREMENT));
					if (iStart < 0) iStart = 0;
					iStop += iStart;
					if (iStop >= 256) iStop = 255;
					Gain = m_pConfigRec->receiver.TcgRec[nBoard*2+j].GateGain[i];

					GainCounts = (unsigned long) (Gain*m_GainScaling+m_GainOffset);
					for (k=iStart; k<=iStop; k++)
					{
						if (j%2 == 0)
						{
							m_bufGainTable[k] &= 0xFFFF0000;
							m_bufGainTable[k] += GainCounts;
						}
						else
						{
							m_bufGainTable[k] &= 0x0000FFFF;
							m_bufGainTable[k] += (GainCounts << 16);
						}
					}
				}
			}
		}  // end j loop
}

void CTcpThreadRxList::SendTcgFunction(short nChannel)
{
	if (m_pConfigRec->receiver.tcg_trigger[nChannel] == 0)
		return;

	MMI_CMD sendBuf;
	DWORD *pDWord;
	float *pFArg;
	int i;  /* generic looper */

	ComputeIdTcgGain(nChannel);
	ComputeOdTcgGain(nChannel);
	for (i=0; i<4; i++)
		if ( m_pConfigRec->receiver.TcgRec[nChannel].GateOn[i] == 0 )
			ComputeGateTcgGain(nChannel, i);


	TCG_REC_MSG *pTcgMsg;
	pTcgMsg = (TCG_REC_MSG *) &(sendBuf.CmdBuf[0]);

	pDWord = (DWORD *) &(sendBuf.CmdBuf[0]);
	pFArg = (float *) &(sendBuf.CmdBuf[0]);

	for (i=0; i<5; i++)
	{
		if (i < MAX_GATES)
		{
			pTcgMsg[0].GateOn[i] = m_pConfigRec->receiver.TcgRec[nChannel].GateOn[i];
			pTcgMsg[0].GateGain[i] = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[i];
		}
		pTcgMsg[0].CurveCoef[i] = m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i];
	}
	pTcgMsg[0].Curve2On = m_pConfigRec->receiver.TcgRec[nChannel].Curve2On;
	pTcgMsg[0].Curve2Coef[0] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0];
	pTcgMsg[0].Curve2Coef[1] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1];

	CString str;
	str.Format(_T("0x%X"),TCG_FN);

	//int rc;
	sendBuf.MsgId = TCG_FN;
	sendBuf.ChnlNum = (WORD) nChannel;

	/*return*/ NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
	if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return;
		}
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		return;
	}
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
#endif
	return;
}


void CTcpThreadRxList::ReSetTcgGainTable(short nChannel, BOOL bSend)
{
	WORD nBoard = nChannel/2;
	int i;

	unsigned long GainCountsA, GainCountsB;

	GainCountsA = ((unsigned long) (m_pConfigRec->receiver.gain[nBoard*2] - m_TruscopGainOffset)) & 0x0000FFFF;
	GainCountsB = ((unsigned long) (m_pConfigRec->receiver.gain[nBoard*2+1] - m_TruscopGainOffset)) << 16;

	for (i=0; i<256; i++)
	{
		if (nChannel % 2 == 0)
		{
			m_bufGainTable[i] &= 0xFFFF0000;
			m_bufGainTable[i] += GainCountsA;
		}
		else
		{
			m_bufGainTable[i] &= 0x0000FFFF;
			m_bufGainTable[i] += GainCountsB;
		}
	}

	for (int j=0; j<4; j++)
	{
		m_TcgGain[nChannel][j] = GetRcvrGain(nChannel);
	}

	m_CurveCoef[nChannel][0] = GetRcvrGain(nChannel);
	m_CurveCoef[nChannel][1] = 0;
	m_CurveCoef[nChannel][2] = 0;
	m_CurveCoef[nChannel][3] = 0;
	m_CurveCoef[nChannel][4] = 0;

	/* save TCG curve coefficients */
	for (i=0; i<5; i++)
	{
		m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i] = m_CurveCoef[nChannel][i];
	}
	m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0] = m_CurveCoef[nChannel][0];
	m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1] = 0;
	m_pConfigRec->receiver.TcgRec[nChannel].Curve2On = 0;

	//m_pConfigRec->receiver.TcgRec[nChannel].TotalSteps = 0;
	for (i=0; i<4; i++)
	{
		m_pConfigRec->receiver.TcgRec[nChannel].GateOn[i] = 0;
		m_pConfigRec->receiver.TcgRec[nChannel].GateGain[i] = GetRcvrGain(nChannel);
	}
	m_LastTcgGateStart = -1;
	m_LastTcgGateStop = -1;

	if (bSend)
		SendTcgFunction(nChannel);
}

bool CTcpThreadRxList::SetTcgGainTable(short nSeqNum, short nChannel, short nGate, float Gain, int nTcgMode)
{
#if 0
	if (!SendSlaveMsg(0x6B, nSeqNum, nChannel, nGate, (WORD) (Gain*10), nTcgMode, 0))
	{
		//return FALSE;
	}
#endif

#if 1

	unsigned long GainCounts;
	float xGain, Distance, Xmid, MaxChange;
	int iStart=5, iStop;
	int i, j;
	float Curve2Coef[2], Distance2, iStop2, Curve2Gain;

	/* load TCG curve coefficients */
	for (i=0; i<5; i++)
	{
		m_CurveCoef[nChannel][i] = m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i];
	}
	Curve2Coef[0] = m_CurveCoef[nChannel][0];
	Curve2Coef[1] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1];
	iStop2 = 100;
	Distance2 = (float) (iStop2 - iStart);
	Curve2Gain = Curve2Coef[1] * ((float) Distance2) + m_TcgGain[nChannel][0];

	switch (nTcgMode)
	{
	case 0:   //Linear mode
		iStart = 5;
		iStop = 253;
		Distance = (float) (iStop - iStart);
		m_TcgGain[nChannel][0] = m_CurveCoef[nChannel][0];
		Curve2Gain = Curve2Coef[1] * ((float) Distance2) + m_TcgGain[nChannel][0];
		m_TcgGain[nChannel][3] = m_CurveCoef[nChannel][1] * ((float) Distance) + m_TcgGain[nChannel][0];
		Xmid = (float) ( (iStop-iStart)/2 );
		if (nSeqNum == 0)
		{
			m_TcgGain[nChannel][0] -= Gain;
			if (m_TcgGain[nChannel][0] > 40.0f) m_TcgGain[nChannel][0] = 40.0f;
			if (m_TcgGain[nChannel][0] < 0) m_TcgGain[nChannel][0] = 0;
			m_CurveCoef[nChannel][0] = m_TcgGain[nChannel][0];
			Curve2Coef[0] = m_CurveCoef[nChannel][0];
			Curve2Coef[1] = (float) ((float) (Curve2Gain - m_TcgGain[nChannel][0])/Distance2);
		}
		
		if (nSeqNum == 1)
		{
			m_TcgGain[nChannel][3] -= Gain;
			if (m_TcgGain[nChannel][3] > 40.0f) m_TcgGain[nChannel][3] = 40.0f;
			if (m_TcgGain[nChannel][3] < 0.0f) m_TcgGain[nChannel][3] = 0.0f;
		}		
		m_CurveCoef[nChannel][1] = (float) ((float) (m_TcgGain[nChannel][3] - m_TcgGain[nChannel][0])/Distance);
		
		// curve 2
#if ( (SYSTEM_ID == LORAINE_SYS) && (SYSTEM_SUB_ID == LORAINE_SYS) )
		if (0)
#else
		if (nSeqNum == 3)
#endif
		{
			m_TcgGain[nChannel][3] = Curve2Coef[1] * ((float) Distance2) + m_TcgGain[nChannel][0];
			m_TcgGain[nChannel][3] -= Gain;
			if (m_TcgGain[nChannel][3] > 40.0f) m_TcgGain[nChannel][3] = 40.0f;
			if (m_TcgGain[nChannel][3] < 0.0f) m_TcgGain[nChannel][3] = 0.0f;
			m_pConfigRec->receiver.TcgRec[nChannel].Curve2On = 0;
			m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0] = m_CurveCoef[nChannel][0];
			Curve2Coef[1] = (float) ((float) (m_TcgGain[nChannel][3] - m_TcgGain[nChannel][0])/Distance2);
		}

		for (i=0; i<iStop; i++)
		{
			if (i < iStart)
			{
				xGain = m_CurveCoef[nChannel][0];
			}
			else
			{
				if ( (i<=iStop2) && (m_pConfigRec->receiver.TcgRec[nChannel].Curve2On) )
				{
					xGain = Curve2Coef[0] + Curve2Coef[1]*(i-iStart);
				}
				else
					xGain = m_CurveCoef[nChannel][0] + m_CurveCoef[nChannel][1]*(i-iStart);
			}
			if (xGain > 40.0f) xGain = 40.0f;
			if (xGain < 0) xGain = 0;
			GainCounts = (unsigned long) (xGain*m_GainScaling+m_GainOffset);
			if (nChannel%2 == 0)
			{
				m_bufGainTable[i] &= 0xFFFF0000;
				m_bufGainTable[i] += GainCounts;
			}
			else
			{
				m_bufGainTable[i] &= 0x0000FFFF;
				m_bufGainTable[i] += (GainCounts << 16);
			}
		}
		m_bufGainTable[255] = m_bufGainTable[0];
		m_bufGainTable[254] = m_bufGainTable[0];
		m_bufGainTable[253] = m_bufGainTable[0];

		/* save TCG curve coefficients */
		for (i=0; i<5; i++)
		{
			m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i] = m_CurveCoef[nChannel][i];
			if (i>1)
				m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i] = 0.0f;
		}
		m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0] = Curve2Coef[0];
		m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1] = Curve2Coef[1];
		//m_pConfigRec->receiver.TcgRec[nChannel].TotalSteps = 0;
		//m_LastTcgGateStart = -1;
		//m_LastTcgGateStop = -1;

		/* restore the gain in gate mode */
		for (j=0; j<4; j++)
		{
			if (m_pConfigRec->receiver.TcgRec[nChannel].GateOn[j])
			{
				iStart = (int) (GetGateDelay(nChannel,j)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
				iStop = (int) (GetGateRange(nChannel,j)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
				if (iStart < 0) iStart = 0;
				iStop += iStart;
				if (iStop >= 256) iStop = 255;
				Gain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[j];

				GainCounts = (unsigned long) (Gain*m_GainScaling+m_GainOffset);
				for (i=iStart; i<=iStop; i++)
				{
					if (nChannel%2 == 0)
					{
						m_bufGainTable[i] &= 0xFFFF0000;
						m_bufGainTable[i] += GainCounts;
					}
					else
					{
						m_bufGainTable[i] &= 0x0000FFFF;
						m_bufGainTable[i] += (GainCounts << 16);
					}
				}
			}
		}

		SendTcgFunction(nChannel);
		break;
	case 1:   //Curve mode
		iStart = 5;
		iStop = 253;
		Distance = (float) (iStop - iStart);
		m_TcgGain[nChannel][0] = m_CurveCoef[nChannel][0];
		Curve2Gain = Curve2Coef[1] * ((float) Distance2) + m_TcgGain[nChannel][0];
		m_TcgGain[nChannel][3] = m_CurveCoef[nChannel][1] * ((float) Distance) + m_TcgGain[nChannel][0];
		Xmid = (float)(iStop-iStart)/2;
		if (nSeqNum == 0)
		{
			m_TcgGain[nChannel][0] -= Gain;
			if (m_TcgGain[nChannel][0] > 40.0f) m_TcgGain[nChannel][0] = 40.0f;
			if (m_TcgGain[nChannel][0] < 0) m_TcgGain[nChannel][0] = 0;
			m_CurveCoef[nChannel][0] = m_TcgGain[nChannel][0];
			Curve2Coef[0] = m_CurveCoef[nChannel][0];
			Curve2Coef[1] = (float) ((float) (Curve2Gain - m_TcgGain[nChannel][0])/Distance2);
		}
		
		if (nSeqNum == 1)
		{
			m_TcgGain[nChannel][3] -= Gain;
			if (m_TcgGain[nChannel][3] > 40.0f) m_TcgGain[nChannel][3] = 40.0f;
			if (m_TcgGain[nChannel][3] < 0) m_TcgGain[nChannel][3] = 0;
		}
		m_CurveCoef[nChannel][1] = (float) ((float) (m_TcgGain[nChannel][3] - m_TcgGain[nChannel][0])/Distance);
		
		if (nSeqNum == 2)
		{
			m_CurveCoef[nChannel][2] += (float)(0.000005 * Gain);
		}

		if (nSeqNum == 3)
		{
			m_CurveCoef[nChannel][3] -= (float)(0.05 * Gain);
		}

		if (nSeqNum == 4)
		{
			m_CurveCoef[nChannel][4] += (float)(0.05 * Gain);
		}

		for (i=0; i<iStop; i++)
		{
			if (i < iStart)
			{
				xGain = m_CurveCoef[nChannel][0];
			}
			else
			{
				if ( (i<=iStop2) && (m_pConfigRec->receiver.TcgRec[nChannel].Curve2On) )
				{
					xGain = Curve2Coef[0] + Curve2Coef[1]*(i-iStart);
				}
				else
				{
					MaxChange = m_CurveCoef[nChannel][2]*(Xmid)*(Xmid);
					if (i < Xmid)
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][3]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
					else
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][4]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
				}
			}
			if (xGain > 40.0f) xGain = 40.0f;
			if (xGain < 0) xGain = 0;
			GainCounts = (unsigned long) (xGain*m_GainScaling+m_GainOffset);
			if (nChannel%2 == 0)
			{
				m_bufGainTable[i] &= 0xFFFF0000;
				m_bufGainTable[i] += GainCounts;
			}
			else
			{
				m_bufGainTable[i] &= 0x0000FFFF;
				m_bufGainTable[i] += (GainCounts << 16);
			}
		}
		m_bufGainTable[255] = m_bufGainTable[0];
		m_bufGainTable[254] = m_bufGainTable[0];
		m_bufGainTable[253] = m_bufGainTable[0];

		/* save TCG curve coefficients */
		for (i=0; i<5; i++)
		{
			m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i] = m_CurveCoef[nChannel][i];
		}
		m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0] = Curve2Coef[0];
		m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1] = Curve2Coef[1];

		/* restore the gain in gate mode */
		for (j=0; j<4; j++)
		{
			if (m_pConfigRec->receiver.TcgRec[nChannel].GateOn[j])
			{
				iStart = (int) (GetGateDelay(nChannel,j)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
				iStop = (int) (GetGateRange(nChannel,j)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
				if (iStart < 0) iStart = 0;
				iStop += iStart;
				if (iStop >= 256) iStop = 255;
				Gain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[j];

				GainCounts = (unsigned long) (Gain*m_GainScaling+m_GainOffset);
				for (i=iStart; i<=iStop; i++)
				{
					if (nChannel%2 == 0)
					{
						m_bufGainTable[i] &= 0xFFFF0000;
						m_bufGainTable[i] += GainCounts;
					}
					else
					{
						m_bufGainTable[i] &= 0x0000FFFF;
						m_bufGainTable[i] += (GainCounts << 16);
					}
				}
			}
		}

		SendTcgFunction(nChannel);
		break;
	case 2:   //Gate mode
		m_pConfigRec->receiver.TcgRec[nChannel].GateOn[nGate] = 1;
		m_TcgGain[nChannel][nGate] = Gain;
		m_pConfigRec->receiver.TcgRec[nChannel].GateGain[nGate] = Gain;

		/* restore the gain in gate mode */
		for (j=0; j<4; j++)
		{
			if (m_pConfigRec->receiver.TcgRec[nChannel].GateOn[j])
			{
				iStart = (int) (GetGateDelay(nChannel,j)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
				iStop = (int) (GetGateRange(nChannel,j)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
				if (iStart < 0) iStart = 0;
				iStop += iStart;
				if (iStop >= 256) iStop = 255;
				Gain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[j];

				GainCounts = (unsigned long) (Gain*m_GainScaling+m_GainOffset);
				for (i=iStart; i<=iStop; i++)
				{
					if (nChannel%2 == 0)
					{
						m_bufGainTable[i] &= 0xFFFF0000;
						m_bufGainTable[i] += GainCounts;
					}
					else
					{
						m_bufGainTable[i] &= 0x0000FFFF;
						m_bufGainTable[i] += (GainCounts << 16);
					}
				}
			}
		}

		SendTcgFunction(nChannel);
		break;
	default:
		break;
	}
#endif

	return true;
}


/*******************************************************************
*  Load shunt and DAC from a file
*/
void CTcpThreadRxList::LoadShuntDac() 
{
	// TODO: Add your command handler code here
	CString s;

	TCHAR szFilter[] = _T("Load Shunt DAC from a Config File(*.cfg) | *.cfg||");


	CFileDialog dlg (TRUE, _T("cfg"), _T("*.cfg"),OFN_OVERWRITEPROMPT, 
				 szFilter);

	if ( dlg.DoModal() == IDOK)
	{
		s = dlg.GetPathName();
		try
		{
			CFile file (s, CFile::modeRead);
				
			file.Read(&(m_pConfigRec->OscopeRec.rf_shunt), sizeof(m_pConfigRec->OscopeRec.rf_shunt));
			file.Read(&(m_pConfigRec->OscopeRec.fw_shunt), sizeof(m_pConfigRec->OscopeRec.fw_shunt));
			file.Read(m_pConfigRec->OscopeRec.gate_FullScale, sizeof(m_pConfigRec->OscopeRec.gate_FullScale));
			file.Read(m_pConfigRec->OscopeRec.gate_zero, sizeof(m_pConfigRec->OscopeRec.gate_zero));

		}
		catch (CFileException* e)
		{
			e->ReportError();
			e->Delete();
		}
	}

#if 0
	TRACE ("rf_shunt = 0x%08X\n", m_pConfigRec->OscopeRec.rf_shunt);
	TRACE ("fw_shunt = 0x%08X\n", m_pConfigRec->OscopeRec.fw_shunt);
	TRACE ("gate_FullScale[0] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_FullScale[0]);
	TRACE ("gate_FullScale[1] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_FullScale[1]);
	TRACE ("gate_FullScale[2] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_FullScale[2]);
	TRACE ("gate_FullScale[3] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_FullScale[3]);
	TRACE ("gate_zero[0] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_zero[0]);
	TRACE ("gate_zero[1] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_zero[1]);
	TRACE ("gate_zero[2] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_zero[2]);
	TRACE ("gate_zero[3] = 0x%08X\n", m_pConfigRec->OscopeRec.gate_zero[3]);
#endif

	TRACE ("size of I_MSG_RUN = %d\n", sizeof(I_MSG_RUN));
	TRACE ("size of I_MSG_CAL = %d\n", sizeof(I_MSG_CAL));
}


BOOL CTcpThreadRxList::SetScopeTrace1CalData(WORD nInstrument, WORD nChannel, WORD nStep, WORD nValue, BOOL bDo90)
{
	WORD nMdac;

	switch (nStep)
	{
	case 2:
		nMdac = (nValue << 4) | 0x0003;
		m_pConfigRec->OscopeRec.V_Scope_Null[nInstrument] = nValue;
		break;

	case 3:
		nMdac = (nValue << 4) | 0x0000;
		m_pConfigRec->OscopeRec.V_Ch_Null[nChannel] = nValue;
		break;

	case 4:
		nMdac = (nValue << 4) | 0x0002;
		m_pConfigRec->OscopeRec.V_Gain[nChannel] = nValue;
		break;

	case 5:
		nMdac = (nValue << 4) | 0x0001;
		if (bDo90)
			m_pConfigRec->OscopeRec.V_Nr[nInstrument][1] = nValue;
		else
			m_pConfigRec->OscopeRec.V_Nr[nInstrument][0] = nValue;
		break;

	default:
		return FALSE;
	}

	SetScopeTrace1Mdac(nInstrument, nMdac);

	return TRUE;
}


WORD CTcpThreadRxList::GetScopeTrace1CalData(WORD nInstrument, WORD nChannel, WORD nStep, BOOL bDo90)
{
	WORD nValue = 0;

	switch (nStep)
	{
	case 2:
		nValue = m_pConfigRec->OscopeRec.V_Scope_Null[nInstrument];
		break;

	case 3:
		nValue = m_pConfigRec->OscopeRec.V_Ch_Null[nChannel];
		break;

	case 4:
		nValue = m_pConfigRec->OscopeRec.V_Gain[nChannel];
		break;

	case 5:
		if (bDo90)
			nValue = m_pConfigRec->OscopeRec.V_Nr[nInstrument][1];
		else
			nValue = m_pConfigRec->OscopeRec.V_Nr[nInstrument][0];
		break;

	default:
		break;
	}

	return nValue;
}


BOOL CTcpThreadRxList::SetScopeTrace1Mdac(WORD nInstrument, WORD nValue)
{
	if (SendSlaveMsg(SCOPE_TRACE1_MDAC, nInstrument, nValue, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;
}


bool CTcpThreadRxList::SetScopeTrace2CalData(int nStep, int nValue)
{
	if (SendSlaveMsg(0x4D, (WORD) nStep, (WORD) nValue, 0, 0, 0, 0))
		return TRUE;
	else
		return FALSE;
}


BOOL CTcpThreadRxList::SendMDAC() 
{
	// TODO: Add your control notification handler code here
	WORD nV_Ch_Null = (m_pConfigRec->OscopeRec.V_Ch_Null[m_nActiveChannel] << 4);
	WORD nV_Nr = 0xF101;
	WORD nV_Gain = (m_pConfigRec->OscopeRec.V_Gain[m_nActiveChannel] << 4) | 0x0002;
	WORD nV_Scope_Null = (m_pConfigRec->OscopeRec.V_Scope_Null[m_nActiveChannel/MAX_CHANNEL_PER_INSTRUMENT] << 4) | 0x0003;

	if (!SendSlaveMsg(0x36, nV_Ch_Null, nV_Nr, nV_Gain, nV_Scope_Null, 0, 0))
		return FALSE;
	
	return TRUE;
}


bool CTcpThreadRxList::WritePulserConfigToRioBoard()
{
	// Set pulser PRF and mode
	if (!SetPulserPRF((short) (m_pConfigRec->pulser.prf*10)))
		return FALSE;

	// Set sequence length
	if (!SetSequenceLength((WORD) m_pConfigRec->UtRec.sequence_length))
		return FALSE;

	return true;
}


CONFIG_REC CTcpThreadRxList::GetConfigRec()
{
	return *m_pConfigRec;
}


bool CTcpThreadRxList::InitializeConfigRec(CONFIG_REC *pConfigRec, bool bWriteToBoard)
{
	memcpy ( (void *) m_pConfigRec, (void *) pConfigRec, sizeof(CONFIG_REC));
#if 0
	if (bWriteToBoard)
	{
		if (!WriteConfigToBoard())
			return false;

		if (!WriteScopeSettingsToBoard())
			return false;

		if (!WriteTcgStepTrigSel(m_nActiveChannel))
			return false;

		if (!EnableFireSequence(m_nActiveChannel, TRUE))
			return FALSE;

		if (!SendMDAC())
			return FALSE;

		EnableScopeDisplay(TRUE, m_nActiveChannel, m_nActiveGate);

		if (!WriteScopeSettingsToRioBoard())
			return false;
	}
#endif
	return true;
}


BOOL CTcpThreadRxList::SendChnlCfgMsg()
{
	MMI_CMD  sendBuf;
	int i, j;  /* generic looper */
	CString str;
	//int rc;
	SHOE_CONFIG  *pChCfg;
	//m_pConfigRec = &ConfigRec;  /* point to the global config record */

	sendBuf.MsgId = CHANNEL_CONFIG_MSG;
	str.Format(_T("0x%X"),sendBuf.MsgId);

	pChCfg = (SHOE_CONFIG *) &sendBuf.CmdBuf[0];

	for ( j = 0; j < MAX_CHANNEL/MAX_CHANNEL_PER_INSTRUMENT; j++)	// loop thru all shoes
	{
		for ( i = 0; i < MAX_CHANNEL_PER_INSTRUMENT; i++)	// loop thru all chnls
		{
			pChCfg->Ch[i].Type = m_pConfigRec->UtRec.Shoe[j].Ch[i].Type;
			pChCfg->Ch[i].cXOffset = m_pConfigRec->UtRec.Shoe[j].Ch[i].cXOffset;
			pChCfg->Ch[i].cWOffset = m_pConfigRec->UtRec.Shoe[j].Ch[i].cWOffset;
		}

		pChCfg->nSlave = j;

		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
		if (g_nClientSocket >= 0)
		{
			rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
			if ( rc <= 0 )
			{
				str.Format(_T("%d"), g_nTemp);
				g_nTemp++;
				//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
				return FALSE;
			}
			if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		}
#endif
		WaitMicroseconds (100);
		//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
	}

	return TRUE;
}

#if 0
typedef struct
	{
	BYTE	TholdLong[MAX_GATE];	/* thold in %, id = 0, od = 1 for long chnls */
	BYTE	TholdTran[MAX_GATE];	/* thold in %, id = 0, od = 1 for tran chnls */
	BYTE	TholdOblq1[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE	TholdOblq2[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE	TholdOblq3[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE	TholdLamin[MAX_GATE];	/* thold in %, id = 0, od = 1 for laminar (wall) chnls */
	char	TholdWall[MAX_GATE];	/* thold in %, min=0, max= 1 for wall chnls */
	short	TholdWallThds[MAX_GATE];	/* wall tholds in 1/1000th inch incr, min=0 */
	float	fWall;
	float	fOD;
	}	C_MSG_ALL_THOLD;

typedef struct
	{
	:
	char		TholdWall[XDCR_TYPE];	/* thold in %, min=0, max= 1 for wall chnls			 -	0*/
	BYTE		TholdLong[XDCR_TYPE+1][MAX_GATE];	/* thold in %, id = 0, od = 1 for long chnls -	1*/
//	BYTE		TholdLong[MAX_GATE];	/* thold in %, id = 0, od = 1 for long chnls -	1*/
//	BYTE		TholdTran[MAX_GATE];	/* thold in %, id = 0, od = 1 for tran chnls		 -	2*/
//	BYTE		TholdOblq1[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls		 -	3*/
//	BYTE		TholdOblq2[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls		 -	4*/
//	BYTE		TholdOblq3[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls		 -	5*/
//	BYTE		TholdLamn[MAX_GATE];	/* thold in %, min=0, max= 1 for lamn chnls			 -	6*/
	WORD		TholdWallThds[2];	/* wall tholds in 1/1000th inch incr, min=0 */
	:
	
	}	UT_REC;
#define MEM_MAX_CHANNEL		100
#define MAX_SHOES	10
#define MAX_GATE	4
#define XDCR_TYPE	8

#endif

// Potential buffer overflow in copy operation
BOOL CTcpThreadRxList::SetAllTholds()
{
	MMI_CMD sendBuf;
	C_MSG_ALL_THOLD *pThold;
	CString str;
	//int rc;

	pThold = (C_MSG_ALL_THOLD *) &sendBuf.CmdBuf[0];

	sendBuf.MsgId = SET_ALL_THOLDS;
	str.Format(_T("0x%X"),sendBuf.MsgId);
/*
	for ( i = 0; i < 2; i++)
	{
		pThold->TholdLong[i] = m_pConfigRec->UtRec.TholdLong[i];
		pThold->TholdTran[i] = m_pConfigRec->UtRec.TholdTran[i];
		pThold->TholdOblq1[i] = m_pConfigRec->UtRec.TholdOblq1[i];
		pThold->TholdOblq2[i] = m_pConfigRec->UtRec.TholdOblq2[i];
		pThold->TholdOblq3[i] = m_pConfigRec->UtRec.TholdOblq3[i];
		pThold->TholdWall[i] = m_pConfigRec->UtRec.TholdWall[i];
		// computer walll in thds
		pThold->TholdWallThds[i] = m_pConfigRec->UtRec.TholdWallThds[i];
	}
*/

	// target only has 4 bytes, source is [9][4], MAX_GATE*7 = 28 jeh jeh
	memcpy( (void *) &pThold->TholdLong,
			(void *) m_pConfigRec->UtRec.TholdLong, MAX_GATE*7); 
	
	pThold->TholdWallThds[0] = m_pConfigRec->UtRec.TholdWallThds[0];
	pThold->TholdWallThds[1] = m_pConfigRec->UtRec.TholdWallThds[1];

	pThold->fWall = m_pConfigRec->JobRec.Wall;
	pThold->fOD = m_pConfigRec->JobRec.OD;

		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
	if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return FALSE;
		}
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		return TRUE;
	}
#endif
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
	return FALSE;
}


BOOL CTcpThreadRxList::SetGateAlarm(short nChannel, short nGate, short nOnOff)
{
	m_pConfigRec->alarm.trigger[nChannel][nGate] = (char) nOnOff;

	if (!SendSlaveMsg(ALARM_TRIGGER, (WORD) nOnOff, 0, 0, 0, 0, 0 ))
		return false;

	return TRUE;
}


BOOL CTcpThreadRxList::SetGateAlarmLogic(short nChannel, short nGate, short nPolarity)
{
	m_pConfigRec->alarm.polarity[nChannel][nGate] = (char) nPolarity;

	if (!SendSlaveMsg(ALARM_POLARITY, (WORD) nPolarity, 0, 0, 0, 0, 0 ))
		return false;

	return TRUE;
}


BOOL CTcpThreadRxList::SetWallAlarm(short nChannel, short nGate, short nOnOff)
{
	m_pConfigRec->alarm.laminar[nChannel][nGate] = (char) nOnOff;

	if (!SendSlaveMsg(ALARM_LAMIN, (WORD) nOnOff, 0, 0, 0, 0, 0 ))
		return false;

	return TRUE;
}


BOOL CTcpThreadRxList::SetWallCoefs()
{
	MMI_CMD sendBuf;
	WALL_COEF *pWallCoef;
	int i;
	CString str;
	//int rc;

	pWallCoef = (WALL_COEF *) &sendBuf.CmdBuf[0];

	sendBuf.MsgId = SET_WALL_COEFS;
	str.Format(_T("0x%X"),sendBuf.MsgId);

	for ( i = 0; i < MAX_SHOES; i++)
	{
		if (m_pConfigRec->UtRec.fWallSlope[i] < 0.01f)
			m_pConfigRec->UtRec.fWallSlope[i] = 1.0f;
		pWallCoef->fWallSlope[i] = m_pConfigRec->UtRec.fWallSlope[i];
		pWallCoef->WallOffset[i] = m_pConfigRec->UtRec.WallOffset[i];
	}

		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
	if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return FALSE;
		}
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		return TRUE;
	}
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
#endif
	return FALSE;
}


BOOL CTcpThreadRxList::SetNcNx()
{
	MMI_CMD sendBuf;
	int i;
	CString str;
	//int rc;
	C_MSG_NC_NX *pNc;

	pNc = (C_MSG_NC_NX *) &(sendBuf.CmdBuf[0]);
	sendBuf.MsgId = SET_NC_NX;
	str.Format(_T("0x%X"),sendBuf.MsgId);

	for ( i = 0; i < 2; i++)
	{
		pNc->Long[i] = ConfigRec.UtRec.NcLong[i];
		pNc->Tran[i] = ConfigRec.UtRec.NcTran[i];
		pNc->Oblq1[i] = ConfigRec.UtRec.NcOblq1[i];
		pNc->Oblq2[i] = ConfigRec.UtRec.NcOblq2[i];
		pNc->Oblq3[i] = ConfigRec.UtRec.NcOblq3[i];
		pNc->Lamin[i] = ConfigRec.UtRec.NcLamin[i];
		pNc->Wall[i] = ConfigRec.UtRec.NxWall[i];
	}
	pNc->Tol = ConfigRec.UtRec.Tol;

	return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
		if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return FALSE;
		}
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		return TRUE;
	}
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
#endif
	return FALSE;
}


BOOL CTcpThreadRxList::SendJobRec()
{
	MMI_CMD sendBuf;
	JOB_REC *pJobRec;
	int i,j;
	CString str;
	//int rc;

	i = sizeof(sendBuf.CmdBuf);		// 800
	j = sizeof(JOB_REC);			// 1093
	pJobRec = (JOB_REC *) &sendBuf.CmdBuf[0];

	sendBuf.MsgId = STORE_JOBREC_MSG;
	str.Format(_T("0x%X"),sendBuf.MsgId);

	memcpy((void *) pJobRec, (void *) &(m_pConfigRec->JobRec), sizeof(JOB_REC));

		return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
	if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return FALSE;
		}
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		return TRUE;
	}
#endif
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
	return FALSE;
}



BOOL CTcpThreadRxList::SetAdiInputInvert(DWORD nInputInv)
{
	int nMsgId = SCOPE_DEMUX_CMD + SCOPE_DEMUX_DIGIO;

	WORD *pWArg;
	
	m_pConfigRec->cal.AdiInInvert = nInputInv;

	pWArg = (WORD *) &nInputInv;

	if (!SendSlaveMsg(nMsgId, pWArg[0], pWArg[1], 0,0,0,0))
		return FALSE;

	return TRUE;
}

DWORD CTcpThreadRxList::GetAdiInputInvert()
{
	return m_pConfigRec->cal.AdiInInvert;
}


// auto calibration related functions

void  CTcpThreadRxList::ComputeIdTcgGain(short nChannel)
{
	unsigned short nTrigSel = GetTcgTrigSel(nChannel);
	int i, iStart, iStop, iStop2=100;
	float xGain, Xmid, MaxChange;
	float Curve2Coef[2];

	/* load TCG curve coefficients */
	for (i=0; i<5; i++)
	{
		m_CurveCoef[nChannel][i] = m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i];
	}

	if ( nTrigSel == 0 )
	{
		m_pConfigRec->receiver.TcgRec[nChannel].IdGain = GetRcvrGain(nChannel);
	}
	else
	{
		if ( m_pConfigRec->receiver.TcgRec[nChannel].GateOn[1] )
			m_pConfigRec->receiver.TcgRec[nChannel].IdGain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[1];
		else
		{
			iStart = 5;
			iStop = 253;
			Xmid = (float)(iStop-iStart)/2;

			i = (int) ((GetGateDelay(nChannel,1)+0.5f*GetGateRange(nChannel,1))/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
			if (i < iStart)
			{
				xGain = m_CurveCoef[nChannel][0];
			}
			else if (i > 255)
			{
				xGain = GetRcvrGain(nChannel);
			}
			else
			{
				if ( (i<=iStop2) && (m_pConfigRec->receiver.TcgRec[nChannel].Curve2On) )
				{
					Curve2Coef[0] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0];
					Curve2Coef[1] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1];
					xGain = Curve2Coef[0] + Curve2Coef[1]*(i-iStart);
				}
				else
				{
					MaxChange = m_CurveCoef[nChannel][2]*(Xmid)*(Xmid);
					if (i < Xmid)
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][3]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
					else
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][4]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
				}
			}
			if (xGain > 79.9f) xGain = 79.9f;
			if (xGain < 0) xGain = 0;

			m_pConfigRec->receiver.TcgRec[nChannel].IdGain = xGain;
		}
	}
}

void  CTcpThreadRxList::ComputeOdTcgGain(short nChannel)
{
	unsigned short nTrigSel = GetTcgTrigSel(nChannel);
	int i, iStart, iStop, iStop2=100;
	float xGain, Xmid, MaxChange;
	float Curve2Coef[2];

	/* load TCG curve coefficients */
	for (i=0; i<5; i++)
	{
		m_CurveCoef[nChannel][i] = m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i];
	}

	if ( nTrigSel == 0 )
	{
		m_pConfigRec->receiver.TcgRec[nChannel].OdGain = GetRcvrGain(nChannel);
	}
	else
	{
		if ( m_pConfigRec->receiver.TcgRec[nChannel].GateOn[2] )
			m_pConfigRec->receiver.TcgRec[nChannel].OdGain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[2];
		else
		{
			iStart = 5;
			iStop = 253;
			Xmid = (float) ( (iStop-iStart)/2 );

			i = (int) ((GetGateDelay(nChannel,2)+0.5f*GetGateRange(nChannel,2))/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
			if (i < iStart)
			{
				xGain = m_CurveCoef[nChannel][0];
			}
			else if (i > 255)
			{
				xGain = GetRcvrGain(nChannel);
			}
			else
			{
				if ( (i<=iStop2) && (m_pConfigRec->receiver.TcgRec[nChannel].Curve2On) )
				{
					Curve2Coef[0] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0];
					Curve2Coef[1] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1];
					xGain = Curve2Coef[0] + Curve2Coef[1]*(i-iStart);
				}
				else
				{
					MaxChange = m_CurveCoef[nChannel][2]*(Xmid)*(Xmid);
					if (i < Xmid)
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][3]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
					else
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][4]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
				}
			}
			if (xGain > 79.9f) xGain = 79.9f;
			if (xGain < 0.0f) xGain = 0.0f;

			m_pConfigRec->receiver.TcgRec[nChannel].OdGain = xGain;
		}
	}
}


void  CTcpThreadRxList::ComputeGateTcgGain(short nChannel, short nGate)
{
	unsigned short nTrigSel = GetTcgTrigSel(nChannel);
	int i, iStart, iStop, iStop2=100;
	float xGain, Xmid, MaxChange;
	float GateDelay, GateRange, GateCenter;
	float NextGateDelay, NextGateRange, NextGateCenter;
	float Curve2Coef[2];

	/* load TCG curve coefficients */
	for (i=0; i<5; i++)
	{
		m_CurveCoef[nChannel][i] = m_pConfigRec->receiver.TcgRec[nChannel].CurveCoef[i];
	}

	if ( nTrigSel != 0 )
	{
		if ( m_pConfigRec->receiver.TcgRec[nChannel].GateOn[nGate] == 0 )
		{
			iStart = 5;
			iStop = 253;
			Xmid = (float) ( (iStop-iStart)/2 );

			i = (int) ((GetGateDelay(nChannel,nGate)+0.5f*GetGateRange(nChannel,nGate))/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
			if (i < iStart)
			{
				xGain = m_CurveCoef[nChannel][0];
			}
			else if (i > 255)
			{
				xGain = GetRcvrGain(nChannel);
			}
			else
			{
				if ( (i<=iStop2) && (m_pConfigRec->receiver.TcgRec[nChannel].Curve2On) )
				{
					Curve2Coef[0] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[0];
					Curve2Coef[1] = m_pConfigRec->receiver.TcgRec[nChannel].Curve2Coef[1];
					xGain = Curve2Coef[0] + Curve2Coef[1]*(i-iStart);
				}
				else
				{
					MaxChange = m_CurveCoef[nChannel][2]*(Xmid)*(Xmid);
					if (i < Xmid)
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][3]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
					else
						xGain = m_CurveCoef[nChannel][0] + 
								m_CurveCoef[nChannel][1]*(i-iStart) + 
								m_CurveCoef[nChannel][2]*(i-Xmid)*(i-Xmid) - MaxChange +
								m_CurveCoef[nChannel][4]*(float)sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart));
				}
			}
			if (xGain > 79.9f) xGain = 79.9f;
			if (xGain < 0) xGain = 0;

			GateDelay = GetGateDelay(nChannel,nGate);
			GateRange = GetGateRange(nChannel,nGate);
			GateCenter = GateDelay + 0.5f*GateRange;
			for (i=0; i<4; i++)
			{
				if (m_pConfigRec->receiver.TcgRec[nChannel].GateOn[3-i])
				{
					NextGateDelay = GetGateDelay(nChannel,3-i);
					NextGateRange = GetGateRange(nChannel,3-i);
					NextGateCenter = NextGateDelay + 0.5f*NextGateRange;
					if ( fabs(GateCenter-NextGateCenter) < 0.5*NextGateRange )
					{
						xGain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[3-i];
						break;
					}
				}
			}

			m_pConfigRec->receiver.TcgRec[nChannel].GateGain[nGate] = xGain;
		}
	}
}


void  CTcpThreadRxList::SetIdTcgGain(short nChannel, float fGain)
{
	m_fIdTcgGain[nChannel] = fGain;
}

void  CTcpThreadRxList::SetOdTcgGain(short nChannel, float fGain)
{
	m_fOdTcgGain[nChannel] = fGain;
}

float CTcpThreadRxList::GetIdTcgGain(short nChannel)
{
	return m_pConfigRec->receiver.TcgRec[nChannel].IdGain;
}

float CTcpThreadRxList::GetOdTcgGain(short nChannel)
{
	return m_pConfigRec->receiver.TcgRec[nChannel].OdGain;
}

float CTcpThreadRxList::GetGainOfGate(short nChannel, short nGate)
{
	unsigned long GainCounts;
	float Gain, xGain, Distance, Xmid, MaxChange;
	int iStart, iStop;
	int i, j, k;
	int nBoard = nChannel/2;

	/* load TCG curve coefficients */
	for (i=0; i<5; i++)
	{
		m_CurveCoef[nBoard*2][i] = m_pConfigRec->receiver.TcgRec[nBoard*2].CurveCoef[i];
		m_CurveCoef[nBoard*2+1][i] = m_pConfigRec->receiver.TcgRec[nBoard*2+1].CurveCoef[i];
	}

	j = nChannel%2;

	iStart = 5;
	iStop = 253;
	Distance = (float) (iStop - iStart);
	Xmid = (float) ( (iStop-iStart)/2 );

	for (i=0; i<iStop; i++)
	{
		if (i < iStart)
		{
			xGain = m_CurveCoef[nBoard*2+j][0];
		}
		else
		{
			MaxChange = m_CurveCoef[nBoard*2+j][2]*(Xmid)*(Xmid);
			if (i < Xmid)
				xGain = (float) ( m_CurveCoef[nBoard*2+j][0] + 
						m_CurveCoef[nBoard*2+j][1]*(i-iStart) + 
						m_CurveCoef[nBoard*2+j][2]*(i-Xmid)*(i-Xmid) - MaxChange +
						m_CurveCoef[nBoard*2+j][3]*sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart)) );
			else
				xGain = (float) ( m_CurveCoef[nBoard*2+j][0] + 
						m_CurveCoef[nBoard*2+j][1]*(i-iStart) + 
						m_CurveCoef[nBoard*2+j][2]*(i-Xmid)*(i-Xmid) - MaxChange +
						m_CurveCoef[nBoard*2+j][4]*sin((double) 2.0f*3.14159f*(1.0f*i-iStart)/(1.0f*iStop-iStart)) );
		}
		if (xGain > 79.9) xGain = 79.9f;
		if (xGain < 0) xGain = 0.0f;
		GainCounts = (unsigned long) (xGain*m_GainScaling+m_GainOffset);
		if (j%2 == 0)
		{
			m_bufGainTable[i] &= 0xFFFF0000;
			m_bufGainTable[i] += GainCounts;
		}
		else
		{
			m_bufGainTable[i] &= 0x0000FFFF;
			m_bufGainTable[i] += (GainCounts << 16);
		}
	}
	m_bufGainTable[255] = m_bufGainTable[0];
	m_bufGainTable[254] = m_bufGainTable[0];

	/* restore the gain in gate mode */
	for (i=0; i<4; i++)
	{
		if (m_pConfigRec->receiver.TcgRec[nChannel].GateOn[i])
		{
			iStart = (int) (GetGateDelay(nChannel,i)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
			iStop = (int) (GetGateRange(nChannel,i)/((GetTcgUpdateRate(nChannel)+1)*TCG_STEP_INCREMENT));
			if (iStart < 0) iStart = 0;
			iStop += iStart;
			if (iStop >= 256) iStop = 256;
			Gain = m_pConfigRec->receiver.TcgRec[nChannel].GateGain[i];

			GainCounts = (unsigned long) (Gain*m_GainScaling+m_GainOffset);
			for (k=iStart; k<iStop; k++)
			{
				if (nChannel%2 == 0)
				{
					m_bufGainTable[k] &= 0xFFFF0000;
					m_bufGainTable[k] += GainCounts;
				}
				else
				{
					m_bufGainTable[k] &= 0x0000FFFF;
					m_bufGainTable[k] += (GainCounts << 16);
				}
			}
		}
	}

	return Gain;
}


BOOL CTcpThreadRxList::SetSiteDefaults(SITE_SPECIFIC_DEFAULTS *pSiteDef)
{
	MMI_CMD sendBuf;
	CString str;
	//int rc;

	sendBuf.MsgId = SITE_SPECIFIC_MSG;
	str.Format(_T("0x%X"),sendBuf.MsgId);
	memcpy( (void *) &sendBuf.CmdBuf[0], (void *) pSiteDef, sizeof(SITE_SPECIFIC_DEFAULTS)); 
	
	return NewSendFunction((BYTE *) &sendBuf, sizeof(MMI_CMD),0);
#if 0
	if (g_nClientSocket >= 0)
	{
		rc = send( g_nClientSocket, (char *) &sendBuf, sizeof(MMI_CMD), 0 );
		if ( rc <= 0 )
		{
			str.Format(_T("%d"), g_nTemp);
			g_nTemp++;
			//AfxMessageBox(_T("send call failed.\nMessage ID = "+str));
			return FALSE;
		}
		if (SLEEP_TIME > 0) ::Sleep(SLEEP_TIME);
		return TRUE;
	}
	//AfxMessageBox(_T("Connection not established.\nMessage ID = "+str));
#endif
	return FALSE;
}


void CTcpThreadRxList::SetBoardRevision(I_MSG_NET *pNetBuf)
{
	int i, j;

	for (i=0; i<MAX_SHOES+1; i++)
	{
		for (j=0; j<MAX_BOARD_PER_INSTRUMENT; j++)
			m_nBoardRevision.nRev[i][j] = pNetBuf->nBoardRevision[i][j];
	}

	memcpy ( (void *) m_nBoardRevision.MasterCodeRev, (void *) pNetBuf->MasterCodeRev, 40);
	memcpy ( (void *) m_nBoardRevision.SlaveCodeRev, (void *) pNetBuf->SlaveCodeRev, 40);
}


void CTcpThreadRxList::GetBoardRevision(BOARD_REV *pRev)
{/*
	int i, j;

	for (i=0; i<MAX_SHOES+1; i++)
	{
		for (j=0; j<MAX_BOARD_PER_INSTRUMENT; j++)
			pRev->nRev[i][j] = m_nBoardRevision.nRev[i][j];
	}
*/
	memcpy ( (void *) pRev, (void *) &m_nBoardRevision, sizeof(BOARD_REV) );
}


void CTcpThreadRxList::SetMotionBus(WORD nMotionBus)
{
	m_nMotionBus = nMotionBus;
}


WORD CTcpThreadRxList::GetMotionBus()
{
	return m_nMotionBus;
}


void CTcpThreadRxList::SetNextJointNumber(DWORD nNextJointNumber)
{
	m_nNextJointNumber = nNextJointNumber;
}


DWORD CTcpThreadRxList::GetNextJointNumber()
{
	return m_nNextJointNumber;
}


BOOL CTcpThreadRxList::SendLastJointNum(DWORD nLastRealJoint, DWORD nLastCalJoint)
{
	DWORD nLastJoint[2];
	WORD  *pWord;

	pWord = (WORD *) nLastJoint;

	nLastJoint[0] = nLastRealJoint;
	nLastJoint[1] = nLastCalJoint;

	if ( !SendSlaveMsg(LAST_JOINT_NUMBER, pWord[0], pWord[1], pWord[2], pWord[3], 0, 0) )
		return FALSE;

	return TRUE;
}


BOOL CTcpThreadRxList::SendCalRunMsg(BOOL bRunCalJoint)
{
	WORD nCalRun;

	if (bRunCalJoint)
		nCalRun = 1;
	else
		nCalRun = 0;

	if ( !SendSlaveMsg(RUN_CAL_JOINT, nCalRun, 0, 0, 0, 0, 0) )
		return FALSE;

	return TRUE;
}

BOOL CTcpThreadRxList::SetAscanRegisters(short nChannel)
{
	WORD  Delay, Range, bFullWave;

	Delay = m_pConfigRec->receiver.ascan_delay[nChannel];
	Range = m_pConfigRec->receiver.ascan_range[nChannel];
	bFullWave = m_pConfigRec->receiver.det_option[nChannel];

	if ( !SendSlaveMsg(SET_ASCAN_REGISTERS, Delay, Range, bFullWave, 0, 0, 0) )
		return FALSE;

	return TRUE;
}

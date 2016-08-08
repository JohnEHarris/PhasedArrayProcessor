/**
Author:		JEH
Date:		06-Nov-2012
Purpose:	Provide a thread to read data out of the RcvdLinkList that holds data from the connected client
			For the Phased Array GUI (PAG) the connected client is the Phased Array Master instrument program.

Revised:

**/


// ServerRcvListThread.cpp : implementation file
//

#include "stdafx.h"
#include "ServerRcvListThread.h"
#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
//#include "RunningAverage.h"
#include "../include/cfg100.h"

// for rnadom number generator
#include <stdlib.h>

class CInstState;
extern  CInspState InspState;
extern 	C_MSG_ALL_THOLD  g_AllTholds;
extern 	C_MSG_NC_NX g_NcNx;
extern I_MSG_RUN SendBuf;
extern DWORD  g_nStation2JointNum;// = 0;
extern I_MSG_RUN SendCalBuf;

extern UINT uAppTimerTick;
extern CServiceApp theApp;


static int nRecvCnt = 0;
static short wClock_old = 100;
int xloc = 0;   /* temporary */
DWORD  NextPacketNum = 0;
BOOL  bGetSlaveRev = TRUE;
SITE_SPECIFIC_DEFAULTS SiteDefault;
int nSendCnt = 0;
int nOldXloc=-1;
int nLoc = 20;

/** External function prototypes... mostly in Service.cpp   **/
extern void Inspection_Process_Control();

// Assume this is PAG if not ServiceApp
#else
#include "stdafx.h"
#include "AfxSock.h"
#include "Extern.h"
#include "Truscan.h"
#include "TscanDlg.h"

#endif


// ServerRcvListThread.cpp : implementation file
//


// This thread reads the data that the ServerSocket has received via the OnReceive() and placed into 
// the socket's associated Receiver linked list. It sepearates the reception of data via TCP/IP from
// the processing of that data. It also allows us to set a priority of execution different from the
// thread which owns the socket feeding the data if we want/need a different priority

// This thread is created by the ServerSocketOwnerThread in its InitInstance() function

// CServerRcvListThread

IMPLEMENT_DYNCREATE(CServerRcvListThread, CServerRcvListThreadBase)

CServerRcvListThread::CServerRcvListThread()
	{
	//m_nFakeDataSeqNumber = 0;
	m_nFrameCount = 0;
	m_nFakeDataCallCount = 0;
	srand( (unsigned)time( NULL ) );	// seed random number generator
	}

CServerRcvListThread::~CServerRcvListThread()
	{
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	s.Format(_T("~CServerRcvListThread[%d][%d] = 0x%08x, Id=0x%04x has run\n"), m_nMyServer, m_nThreadIndex, this, nId);
	TRACE(s);
//	if (NULL == m_pSCC)			return;
//	m_pSCC->pServerRcvListThread = NULL;
	}

// We have to over-ride the parents InitInstance since this thread is created by the parent in InitInstance()
// If we don't override, we will get an infinite loop of thread creation.
// This changed on 11-12-2012 when we made this a child of CWinThread
//
BOOL CServerRcvListThread::InitInstance()
	{
	CString s;
	
	// TODO:  perform and per-thread initialization here
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debug checking
	AfxSocketInit();
#endif

#ifdef THIS_IS_SERVICE_APP

	CServerRcvListThreadBase::InitInstance();
#if 0
		s.Format(_T("Instrument Client[%d] accepted to server on socket %s : %d\n"), m_nThreadIndex, 
			m_ConnectionSocket.m_pSCC->sClientIP4, m_ConnectionSocket.m_pSCC->uClientPort); 
		TRACE(s);
		TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
#endif
		m_pElapseTimer = new CHwTimer();

		//InitRunningAverage(0,0);
		return TRUE;


#else
	// not THIS_IS_SERVICE_APP

	CServerRcvListThreadBase::InitInstance();
	TRACE(s);
	TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
	//InitRunningAverage(0,0);
	return TRUE;

#endif		// not THIS_IS_SERVICE_APP
	};

int CServerRcvListThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	// return CServerRcvListThreadBase::ExitInstance();
	CString s = _T("");
	int i = 0;
	void *pV;

#ifdef THIS_IS_SERVICE_APP
	if (m_pElapseTimer)
		{
		delete m_pElapseTimer;
		m_pElapseTimer = NULL;
		}

	// close and or delete everything the list thread created jeh 2016-08-01
	// delete the client connection associated with this  thread see 
	// pscc = m_pSCM->m_pstSCM->pClientConnection[nClientPortIndex] = new ST_SERVERS_CLIENT_CONNECTION();
	// may need to get rid of ServerSocketOwnerThread elements also
	if ( m_pSCC)
		{
		for ( i = 0; i < MAX_CHNLS_PER_INSTRUMENT; i++)
			{
			if (m_pSCC->pvChannel[i])
				{
				delete m_pSCC->pvChannel[i];
				m_pSCC->pvChannel[i] = NULL;
				}
			}	// for loop
		// release storage and critical sections
		// socket object belongs to ServerSockerOwnerThread so must have it to kill the linked lists
		
		}


#endif
	s.Format(_T("CServerRcvListThread, Srv[%d]Instrument[%d] has exited\n"),m_nMyServer, m_nThreadIndex);
	TRACE(s);
	return CServerRcvListThreadBase::ExitInstance();

}

//BEGIN_MESSAGE_MAP(CServerRcvListThread, CServerSocketOwnerThread)	// changed (xx,CWin) to (xx,CServerSocketOwnerThread)
BEGIN_MESSAGE_MAP(CServerRcvListThread, CServerRcvListThreadBase)


#ifdef THIS_IS_SERVICE_APP
	ON_THREAD_MESSAGE(WM_USER_SERVERSOCKET_PKT_RECEIVED, ProcessRcvList)// manually added by jeh 11-06-12
//	ON_THREAD_MESSAGE(WM_USER_INIT_RUNNING_AVERAGE, InitRunningAverage)	// manually added by jeh 11-09-12
#endif

#ifdef _I_AM_PAG
	ON_THREAD_MESSAGE(WM_USER_SERVERSOCKET_PKT_RECEIVED, ProcessRcvList)// manually added by jeh 11-06-12
#endif

END_MESSAGE_MAP()


// CServerRcvListThread message handlers
// Thread message WM_USER_SERVERSOCKET_PKT_RECEIVED activates this procedure
// comes from CServerSocket::OnReceive()
afx_msg void CServerRcvListThread::ProcessRcvList(WPARAM w, LPARAM lParam)
	{
	void *pV;
	if (m_pSCC)
		{
		if (m_pSCC->pSocket)
			{
			m_pSCC->pSocket->LockRcvPktList();
			while (m_pSCC->pRcvPktList->GetCount() )
				{
				pV = m_pSCC->pRcvPktList->RemoveHead();
				m_pSCC->pSocket->UnLockRcvPktList();
#ifdef THIS_IS_SERVICE_APP
				ProcessInstrumentData(pV);	// local call to this class memeber
#else
				ProcessPAM_Data(pV);
#endif
				m_pSCC->pSocket->LockRcvPktList();
				}
			m_pSCC->pSocket->UnLockRcvPktList();
			}
		}
	}

void CServerRcvListThread::MakeFakeDataHead(SRawDataPacket *pData)
	{
	pData->DataHead.bMsgID		= eRawInsp;	// raw data=10
	pData->DataHead.bChannelTypes	= 4;
	pData->DataHead.bChannelRepeats	= 8;
	pData->DataHead.bFramesInDataPacket = 4;
	//m_nFakeDataSeqNumber	+= 128;	// the next packet will 128 Ascans/Main bangs later
	//m_nFakeDataSeqNumber		= m_nFakeDataSeqNumber % 128;

	pData->DataHead.bDin = FORWARD | PIPE_PRESENT;
	pData->DataHead.wMsgSeqCnt++;
	pData->DataHead.wLocation = nLoc++;
	if (nLoc > 500) nLoc = 20;
	pData->DataHead.wClock = nLoc % 12;
	pData->DataHead.wPeriod = 1465;	// 300 ms = 200 rpm
	}

// Random number between 0 and 100
// Notice - not a class member
//
BYTE GetRand(void)
	{
	WORD wReturn;
	wReturn = rand();
	//wReturn = (WORD)(((double)rand() / (RAND_MAX + 1)) * 100);
	return (BYTE)(wReturn % 100);
	}

// Make fake data to test Nc and Nx operations
void CServerRcvListThread::MakeFakeData(SRawDataPacket *pData)
	{
	int i, j, offset, k;
	CString s,t;

	s.Format(_T("\r\n\r\nFake Data Call = %5d\r\n      ID   OD   TOF2    TOF4\r\n"), m_nFakeDataCallCount++);
	SaveFakeData(s);

	MakeFakeDataHead(pData);
	// we could have fewer than 32 channels, ie sequence number could be less than 32
	// Consider the number of ascans needed to fire all multiplexed channels (say 8)
	// Give this number the name "frame". A frame is the number of ascans to fire all the channels
	// Then 8 ascans would be a frame  - and 128/8 = 16 frames in the packet.
	// Every 16 frames, generate the max and min wall reading and the max Nc qualified flaw reading for
	// every channel. 

	// Here we consider 32 channels. It will take 4 calls to this function to produce the 16 frames.
	for ( j = 0; j < 4; j++)	// 4 sets of 32 Ascans, each ascan is a 'different' channel
		{
		offset = j*32;
		for ( i = 0; i < MAX_CHNLS_PER_INSTRUMENT; i++)
			{
			s.Format(_T("\r\n[%3d] "), i+offset);
			k = pData->RawData[i+offset].bAmp2 = 5 + (GetRand()/2);	// 5-55 amplitude
			t.Format(_T("%3d  "),(k)); s += t;
			k = pData->RawData[i+offset].bAmp3 = 10 + (GetRand()/2);	// 10-60 amplitude
			t.Format(_T("%3d  "),(k)); s += t;
			k = pData->RawData[i+offset].wTof2 = 200 + GetRand();
			t.Format(_T("%4d    "),(k)); s += t;
			k = pData->RawData[i+offset].wTof4 = 300 + GetRand();
			t.Format(_T("%4d    "),(k)); s += t;
			if ( i < 1) SaveFakeData(s);
			}
//		m_nFrameCount++;
//		if ((m_nFrameCount & 0xf) == 0)
//			{
//			;
//			// Package up the Max/Min wall and flaw readings for the PAG/Reciever and put in Sender Linked list
//			}
		}
	}

// For debugging, save the fake data and the contents of the output tcpip packet made from fake data
void CServerRcvListThread::SaveFakeData(CString& s)
	{
	theApp.SaveFakeData(s);
	}

// Not enought info in data structures I am seeing on 6/7/16. Assume 32 chnls 
int CServerRcvListThread::GetSequenceModulo(SRawDataPacket *pData)
	{
#if 0
	int i;
	int nStartSeqCount, nSeqQty;
	// Since length == 1040 we have 128 ascan samples. 

#endif
	return 32;

	}


// delete the pRaw packet and create a new packet for transmission
// Fill the new packet with max/min data from each pChannel instance
void CServerRcvListThread::BuildOutputPacket(SRawDataPacket *pRaw)
	{
	int i, k;
	CString s,t;
	CvChannel *pChannel;
	int nNcId, nNcOd, nModId, nModOd, nThdlId, nThldOd, nNx, nMaxLimit, nMinLimit, nDrop; 

	s = _T("\r\nPAM Output Data Packet\r\n");
	SaveFakeData(s);
	IDATA_PACKET *pOutputPacket = new (IDATA_PACKET);
	pOutputPacket->bvChannelQty	= 1;
	pOutputPacket->wLoc		= m_pSCC->InstrumentStatus.wLoc;
	pOutputPacket->wAngle	= m_pSCC->InstrumentStatus.wAngle;
	pOutputPacket->wPeriod	= m_pSCC->InstrumentStatus.wPeriod;
	pOutputPacket->instNumber= m_pSCC->m_nMyThreadIndex;
	pOutputPacket->wStatus	= m_pSCC->InstrumentStatus.wStatus;
	pOutputPacket->uSync	= 0x5CEBDAAD;

	RESULTS *pR				= &pOutputPacket->Results[0];

	s.Format(_T("Loc=%3d Angle=%3d Inst=%2d Stat=%04x Sync=0x5CEBDAAD\r\n"),
		pOutputPacket->wLoc, pOutputPacket->wAngle, pOutputPacket->instNumber, pOutputPacket->wStatus);
	SaveFakeData(s);
	// Get Nc Nx info for 1st channel  -- for debug from output file
	pChannel = m_pSCC->pvChannel[0];
	nNcId		= pChannel->bGetNcId();
	nNcOd		= pChannel->bGetNcOd();
	nModId		= pChannel->bGetMId();
	nModOd		= pChannel->bGetMOd();
	nThdlId		= pChannel->bGetThldId();
	nThldOd		= pChannel->bGetThldOd();
	nNx			= pChannel->bGetNx();
	nMaxLimit	= pChannel->wGetMaxWallLimit();
	nMinLimit	= pChannel->wGetMinWallLimit();
	nDrop		= pChannel->GetDropCount();


	s = _T("\r\n ID: Nc   M  Thld  Nx  MaxWall  MinWall Drop\r\n");
	SaveFakeData(s);
	s.Format(_T("     %2d  %2d  %3d   %d  %4d    %4d     %2d\r\n OD:"), nNcId, nModId, nThdlId, nNx, nMaxLimit, nMinLimit, nDrop );
	SaveFakeData(s);
	//od parameters
	s.Format(_T(" %2d  %2d  %3d"), nNcOd, nModOd, nThldOd );
	SaveFakeData(s);

	s.Format(_T("\r\n      ID   OD   MinW    MaxW"));
	SaveFakeData(s);

	for ( i = 0; i < 32; i++)
		{
		pChannel = m_pSCC->pvChannel[i];
		s.Format(_T("\r\n[%3d] "), i);
		k = pR[i].bFlaw[0] = pChannel->bGetIdGateMax();
		t.Format(_T("%3d  "),k); s += t;
		k =pR[i].bFlaw[1] = pChannel->bGetOdGateMax();
		t.Format(_T("%3d  "),k); s += t;
		k =pR[i].wTOFsum[0] = pChannel->wGetMinWall();
		t.Format(_T("%4d    "),k); s += t;
		k =pR[i].wTOFsum[1] = pChannel->wGetMaxWall();
		t.Format(_T("%4d    "),k); s += t;
		if ( i < 4) SaveFakeData(s);
		}
	// Put the newly created packet into the linked list for output
	// For now send this message directly. In future, put into linked list
	// and signal the sender thread to empty list.
	theApp.PamSendToPag(pOutputPacket, sizeof(IDATA_PACKET));
	delete pOutputPacket;

	}




// Take the code from ServiceApp, tInstMsgProcess() and incorporate into this class
// Note that original code read input data from a socket and blocked until data was available.
// The class containing this procedure works by emptying input data which has been put into a linked list
// by the socket's OnReceive() function. It only runs when a message is posted for the thread to check
// the linked list.
// Packets stored in pData have the length of the packet prepended.
// Remove the integer length before processing
#define MAKE_FAKE_DATA

#ifdef THIS_IS_SERVICE_APP

void CServerRcvListThread::ProcessInstrumentData(void *pData)
	{
	// see ServicApp.cpp the procedure tInstMsgProcess() for legacy operation
	//
	stSEND_PACKET *pBuf = (stSEND_PACKET *) pData;
	//int nStartSeqCount, nSeqQty;
	int nSeqQty;
	SRawDataPacket *pRaw;
	int i, j, nFrameQtyPerPacket;
	int l,m;
	int k;
	BYTE bGateTmp;
	WORD wTOFSum;
	CvChannel *pChannel;
	CString s;


	// After 16 Ascans, send Max/Min wall and Nc qualified flaw values for 2 gates.
	if (pBuf->nLength == 1040)
		{
		pRaw = (SRawDataPacket *) &pBuf->Msg;
#ifdef MAKE_FAKE_DATA
	// Basically Yqing's simulator gave us some bytes. Generate test data in place of those bytes.
	// Run the fake data through the Nc Nx operations and keep the Max, Min values
		MakeFakeData(pRaw);
		// Now that we have fake data, process it
		// How many vChannels?
#endif
		nSeqQty = GetSequenceModulo(pRaw);
		// we need 16 frames of data to average/peak hold
		// a frame occurs when the vChannel repeats
		nFrameQtyPerPacket = 128/nSeqQty;	// for our synthetic data, nSeqQty is 32 -> 4 frames per packet

		// Capture starting status info regarding pipe location
		if ( 0 == m_nFrameCount)
			{
			m_pSCC->InstrumentStatus.wStatus	= 0;
			m_pSCC->InstrumentStatus.wLoc		= pRaw->DataHead.wLocation;
			m_pSCC->InstrumentStatus.wAngle		= pRaw->DataHead.wClock;
			m_pSCC->InstrumentStatus.wPeriod	= pRaw->DataHead.wPeriod;
			}

		if (m_pElapseTimer)
			m_pElapseTimer->Start();
		for ( j = 0; j < nFrameQtyPerPacket; j++)	// Assumes the data packet contains whole frames
			{
			for ( i = 0; i < nSeqQty; i++)
				{
				k = j*nSeqQty;	// k points to beginning of a frame of data
				pChannel = m_pSCC->pvChannel[i];
				// Get flaw Nc qualified Max values for this channel
				bGateTmp = pChannel->InputFifo(eId, pRaw->RawData[i+k].bAmp2);	// output of the Nc peak holder
				if (pChannel->m_GateID < bGateTmp)		pChannel->m_GateID = bGateTmp;
				bGateTmp = pChannel->InputFifo(eOd, pRaw->RawData[i+k].bAmp3);
				if (pChannel->m_GateOD < bGateTmp)		pChannel->m_GateOD = bGateTmp;

				// Get Max and min tof for this channel
				wTOFSum = pChannel->InputWFifo(pRaw->RawData[i+k].wTof4);
				// CHECK for bad wall above drop out count
				l = pChannel->wGetBadWallCount(); // debugging
				m = pChannel->GetDropCount();
				if ( pChannel->wGetBadWallCount() >= pChannel->GetDropCount())
					pChannel->m_wTOFMaxSum = wTOFSum = 1;	// coupling loss

				l = pChannel->wGetGoodConsecutiveCount();
				m = pChannel->bGetNx();
				// else
				if(pChannel->wGetGoodConsecutiveCount() >= pChannel->bGetNx())
					{
					// self healing. When Nx or greater good wall readings, clear bad wall reading count
					// this means we may never know how many bad walls we have as long as they don't exceed
					// drop out count
					pChannel->ClearBadWallCount();
					}
				if (pChannel->m_wTOFMaxSum < wTOFSum)	pChannel->m_wTOFMaxSum = wTOFSum;
				if (pChannel->m_wTOFMinSum > wTOFSum)	pChannel->m_wTOFMinSum = wTOFSum;
				}
			k = i;

			m_nFrameCount++;
			// It will take 4 input packets from the instrument in order to get 16 frames of data 
			if ( (m_nFrameCount & 0xf) == 0) 
				{
				BuildOutputPacket(pRaw);		//scavenge location info from input value in header
				// pRaw is pBuf->Msg
				delete pData;	// info in pRaw was put into a new structure and sent to the PAG
				m_nFrameCount = 0;
				for ( i = 0; i < nSeqQty; i++)
					{
					m_pSCC->pvChannel[i]->ResetGatesAndWalls();
					}
				}

			}	// j loop. do nFrameQty
		m_nElapseTime = m_pElapseTimer->Stop();		// elapse time in uSec for 128 AScans
		s.Format(_T("Nc Nx processing for 128 Ascans in %d uSec\n"), m_nElapseTime);
		TRACE(s);
		}	// if (pBuf->nLength == 1040)

	else
		{
		s.Format(_T("ProcessInstrumentData got a data packet of the wrong size, %d\n"),pBuf->nLength);
		delete pData;	// info in pRaw was put into a new structure and sent to the PAG
		}

	}	// CServerRcvListThread::ProcessInstrumentData(void *pData)
#endif


// Take the code from TScanDlg which processes packets from PAM and insert here
#ifdef _I_AM_PAG

extern void DistributeMessage(I_MSG_RUN *pReadBuf, CTcpThreadRxList *pTcpThreadRxList);

void CServerRcvListThread::ProcessPAM_Data(void *pData)
	{
	CString s;
	CTcpThreadRxList *pTcpThreadRxList = pCTscanDlg->GetTcpThreadList();
	if (NULL == pTcpThreadRxList)
		{
		delete pData;
		s = _T("CServerRcvListThread::ProcessPAM_Data - Could not get valid pTcpThreadRxList");
		TRACE(s);
		return;
		}
	I_MSG_RUN *pMR = new I_MSG_RUN;
	// strip off the length added by OnReceive
	stSEND_PACKET *pTmp = (stSEND_PACKET *) pData;
	memcpy( (void *) pMR, pTmp->Msg, pTmp->nLength);	// move all data to the new buffer
	pMR->MstrHdr.nSlave = m_pSCC->m_nMyThreadIndex;
	::DistributeMessage(pMR, pTcpThreadRxList);
	delete pData;
	}

#endif


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
#include "../Include/PA2Struct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
#include "../include/pa2struct.h"

// for rnadom number generator
#include <stdlib.h>

class CInstState;
extern  CInspState InspState;
//extern 	C_MSG_ALL_THOLD  g_AllTholds;
//extern 	C_MSG_NC_NX g_NcNx;
//extern I_MSG_RUN SendBuf;
//extern DWORD  g_nStation2JointNum;// = 0;
//extern I_MSG_RUN SendCalBuf;

extern UINT uAppTimerTick;
extern CServiceApp theApp;


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
	m_nFakeDataCallCount = 0;
	srand( (unsigned)time( NULL ) );	// seed random number generator
	// The output packet has the same structure and the input. It is a compress version of the input
	m_pOutputRawDataPacket = new InputRawDataPacket;
	memset(m_pOutputRawDataPacket, 0, sizeof (InputRawDataPacket));
	m_pIdataPacket = NULL;
	//m_uMsgSeqCnt = 0;
	}

CServerRcvListThread::~CServerRcvListThread()
	{
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	s.Format(_T("~CServerRcvListThread[%d][%d] = 0x%08x, Id=0x%04x has run\n"), m_nMyServer, m_nThreadIndex, this, nId);
	TRACE(s);
	if (m_pOutputRawDataPacket != NULL)
		delete m_pOutputRawDataPacket;
	m_pOutputRawDataPacket = 0;
	if (m_pIdataPacket != NULL)
		delete m_pIdataPacket;
	m_pIdataPacket = 0;
	}

// We have to over-ride the parents InitInstance since this thread is created by the parent in InitInstance()
// If we don't override, we will get an infinite loop of thread creation.
// This changed on 11-12-2012 when we made this a child of CWinThread
//
BOOL CServerRcvListThread::InitInstance()
	{
	CString s;
	int i;
	
	// TODO:  perform and per-thread initialization here
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debug checking
	AfxSocketInit();
#endif

#ifdef THIS_IS_SERVICE_APP

	CServerRcvListThreadBase::InitInstance();
	m_pElapseTimer = new CHwTimer();
	i = sizeof(CHwTimer);		// 364
	m_FDstartSeq = m_FDstartCh = 0;	// Fake data simulator state variables
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
	//void *pV;

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
	ON_THREAD_MESSAGE(WM_USER_FLUSH_LINKED_LISTS, FlushRcvList)			// manually added jeh 09-20-16
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
	InputRawDataPacket *pIdataPacket;
	int j = (int) w;
	if ( m_pstSCM)
		{
		if (m_pstSCM->pCS_ClientConnectionRcvList[j]) 
			{
			if (0 == TryEnterCriticalSection(m_pstSCM->pCS_ClientConnectionRcvList[j]))	return;	 // try again later
			}
		}
	else return;

	// if here we are in a critical section

	while (m_pSCC->cpRcvPktList->GetCount() )
		{
		pIdataPacket = (InputRawDataPacket *) m_pstSCM->pRcvPktList[j]->RemoveHead();				//m_pSCC->cpRcvPktList->RemoveHead();
		//m_pSCC->pSocket->UnLockRcvPktList();
#ifdef THIS_IS_SERVICE_APP
		ProcessInstrumentData(pIdataPacket);	// local call to this class memeber
#else
		ProcessPAM_Data(pV);
#endif
		}

	LeaveCriticalSection(m_pstSCM->pCS_ClientConnectionRcvList[j]);
	}

// this procedure activated by WM_USER_FLUSH_LINKED_LISTS
// WPARAM is the specific client connection to flush 
void CServerRcvListThread::FlushRcvList(WPARAM w, LPARAM lParam)
	{
	void *pV;
	int j = (int) w;
	if ( m_pstSCM)
		{
		if (m_pstSCM->pCS_ClientConnectionRcvList[j]) 
			{
			if (0 == TryEnterCriticalSection(m_pstSCM->pCS_ClientConnectionRcvList[j]))	return;	 // try again later
			}
		}
	else return;

	// if here we are in a critical section

	while (m_pSCC->cpRcvPktList->GetCount() )
		{
		pV = m_pstSCM->pRcvPktList[j]->RemoveHead();				//m_pSCC->cpRcvPktList->RemoveHead();

		delete pV;
		}
	LeaveCriticalSection(m_pstSCM->pCS_ClientConnectionRcvList[j]);
	}

//void CServerRcvListThread::MakeFakeDataHead(SRawDataPacketOld *pData)
void CServerRcvListThread::MakeFakeDataHead(InputRawDataPacket *pData)
	{
	//pData->wMsgID	= eRawInsp;	// raw data=10
	//pData->wByteCount = 1460;
	pData->bDin		= FORWARD | PIPE_PRESENT;
	//pData->wMsgSeqCnt++;
	pData->wLocation = nLoc++;
	if (nLoc > 500) 
		nLoc = 20;
	pData->wClock	= nLoc % 12;
	pData->wPeriod	= 1465;	// 300 ms = 200 rpm
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

void CServerRcvListThread::IncFDstartCh(void)
	{
	m_FDstartCh++;
	if (m_FDstartCh >= gMaxChnlsPerMainBang)
		{
		m_FDstartCh = 0;
		IncFDstartSeq();
		}
	}

//Only call from IncFDstartCh
void CServerRcvListThread::IncFDstartSeq(void)
	{
	m_FDstartSeq++;
	if (m_FDstartSeq >= gMaxSeqCount)
		{
		m_FDstartSeq = 0;
		}
	}

// Make fake data to test Nc and Nx operations
// change input data type to InputRawDataPacket
//void CServerRcvListThread::MakeFakeData(SRawDataPacketOld *pData)
// ADD state variable to keep track of start sequence and start channel number
// Can be different start location on every call.
// Loop here until we get MAX_RESULTS and then return fake data to caller.
//
void CServerRcvListThread::MakeFakeData(InputRawDataPacket *pData)
	{
	int i, jSeq, k, n, iSeqPkt;
	CString s,t;

	s.Format(_T("\r\n\r\nFake Data Call = %5d\r\n       G1   ID   OD   TOF4\r\n"), m_nFakeDataCallCount++);
	SaveFakeData(s);

	MakeFakeDataHead(pData);

	// Assuming only 7 sequences to fit the data size. Need to do something different if not 7 sequences in packet
	// Doing something different
	for (iSeqPkt = 0; iSeqPkt < 7; iSeqPkt++)
		{
		pData->stSeqPkt[iSeqPkt].DataHead.bSeqNumber = GetFDstartSeq();
		pData->stSeqPkt[iSeqPkt].DataHead.bChnlNumber = GetFDstartCh();
		n = 0;	// channel counter. Will create gMaxChnlsPerMainBang*gMaxSeqCount channels
		jSeq = GetFDstartSeq();

		s.Format(_T("\r\n[%3d] Sequence Begins"), jSeq);
		while (1)
			{
			jSeq = GetFDstartSeq();
			i = GetFDstartCh();			
			if (i == 0)
				{
				t.Format(_T("\r\n[%3d] "), i); s += t;
				// Input flaw gates before wall since the 16 Ascan reset is done by the wall code
				k = pData->stSeqPkt[iSeqPkt].RawData[i].bAmp1 = 1 + (GetRand() / 2);	// 1-51 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
				k = pData->stSeqPkt[iSeqPkt].RawData[i].bAmp2 = 5 + (GetRand() / 2);	// 5-55 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
				k = pData->stSeqPkt[iSeqPkt].RawData[i].bAmp3 = 10 + (GetRand() / 2);	// 10-60 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
				k = pData->stSeqPkt[iSeqPkt].RawData[i].wTof = 300 + GetRand();
				t.Format(_T("%4d    "), (k)); s += t;
				SaveFakeData(s);
				}
			else
				{
				pData->stSeqPkt[iSeqPkt].RawData[i].bAmp1 = 1 + (GetRand() / 2);
				pData->stSeqPkt[iSeqPkt].RawData[i].bAmp2 = 5 + (GetRand() / 2);	// 5-55 amplitude
				pData->stSeqPkt[iSeqPkt].RawData[i].bAmp3 = 10 + (GetRand() / 2);	// 10-60 amplitude
				pData->stSeqPkt[iSeqPkt].RawData[i].wTof = 300 + GetRand();
				}

			IncFDstartCh();		// move to the next channel... maybe in the next sequence

			n++;
			if (n == gMaxChnlsPerMainBang)
				{
				break;	// jump into for loop at top
				}

			jSeq = GetFDstartSeq();
			i = GetFDstartCh();

			}	// while (1)
		}	// for (iSeqPkt = 0; iSeqPkt < 7; iSeqPkt++)
	}

// For debugging, save the fake data and the contents of the output tcpip packet made from fake data
void CServerRcvListThread::SaveFakeData(CString& s)
	{
	theApp.SaveFakeData(s);
	}

// Not enought info in data structures I am seeing on 6/7/16. Assume 32 chnls 
int CServerRcvListThread::GetSequenceModulo(SRawDataPacketOld *pData)
	{
#if 0
	int i;
	int nStartSeqCount, nSeqQty;
	// Since length == 1040 we have 128 ascan samples. 

#endif
	return 32;

	}


// Build Output packet as individual channel peak held data becomes available
// could return the index in Idata
// If SendFlag is non zero, send the packet as is
void CServerRcvListThread:: AddToIdataPacket(CvChannel *pChannel, int nCh, int nSeq, int nSendFlag)
	{
	if (m_pIdataPacket == NULL)
		{
		m_pIdataPacket = new (IDATA_PACKET);	// sizeof = 1460 179 RESULTS
		memset((void *) m_pIdataPacket,0, sizeof(IDATA_PACKET));
		m_pIdataPacket->bPAPNumber	= (BYTE) theApp.GetMy_PAM_Number();
		m_pIdataPacket->bInstNumber	= m_pSCC->m_nMyThreadIndex;

		m_pIdataPacket->uSync		= SYNC;
		m_pIdataPacket->wMsgSeqCnt = 0;	// m_uMsgSeqCnt++;
		m_pIdataPacket->wMsgID		= 1;
		m_pIdataPacket->bStartSeqNumber = nSeq;	// Come from gate board in header with gates and wall
		m_pIdataPacket->bStartChannel	= nCh;
		m_pIdataPacket->bSequenceLength	= gMaxChnlsPerMainBang;	// Come from gate board in header with gates and wall reading
		m_pIdataPacket->bMaxVChnlsPerSequence	= gMaxChnlsPerMainBang;
		m_pIdataPacket->wStatus		= 0x1234;
		m_pIdataPacket->wLoc		= m_pSCC->InstrumentStatus.wLoc;
		m_pIdataPacket->wAngle		= m_pSCC->InstrumentStatus.wAngle;
		m_pIdataPacket->wPeriod		= m_pSCC->InstrumentStatus.wPeriod;
		m_pIdataPacket->wStatus		= m_pSCC->InstrumentStatus.wStatus;
		m_IdataInPt					= 0;	// insertion index in output data structrure
		}

	pChannel->CopyPeakData(&m_pIdataPacket->Results[m_IdataInPt++]);
	pChannel->ClearDropOut();
	pChannel->ClearOverRun();
	pChannel->SetRead();
	// if (nSendFlag) Send the packet now.
	}


int CServerRcvListThread::GetIdataPacketIndex(void)
	{
	if (m_pIdataPacket == NULL)	return -1;
	return m_IdataInPt;	// how may Result buffers are full. Limit is 179
	}



// delete the pRaw packet and create a new packet for transmission
// Fill the new packet with max/min data from each pChannel instance

//void CServerRcvListThread::BuildOutputPacket(SRawDataPacketOld *pRaw)
// BuildOutputPacket was called because we have gone thru 16 Ascan samples in the whole machine.
// May not work this way for a real machine
void CServerRcvListThread::BuildOutputPacket(InputRawDataPacket *pInput)
	{
	int i, k, jSeq;
	CString s,t;
	CvChannel *pChannel;
	int nNcId, nNcOd, nModId, nModOd, nThdlId, nThldOd, nNx, nMaxLimit, nMinLimit, nDrop; 

	s = _T("\r\nPAM Output Data Packet\r\n");
	SaveFakeData(s);
	// IdataPacket is an accumulation of individual vChannel outputs
	// Starting sequence may vary
	if (m_pIdataPacket == NULL)
		{
		IDATA_PACKET *m_pIdataPacket = new (IDATA_PACKET);	// sizeof = 1454 179 RESULTS
		memset((void *) m_pIdataPacket,0, sizeof(IDATA_PACKET));
		m_pIdataPacket->bPAPNumber	= (BYTE) theApp.GetMy_PAM_Number();
		m_pIdataPacket->bInstNumber	= m_pSCC->m_nMyThreadIndex;
		}

	//How to tell when an Idata structures begins and what is the first channel inserted?
	if (m_pIdataPacket->uSync == 0)
		{
		m_pIdataPacket->bStartSeqNumber = pInput->stSeqPkt->DataHead.bSeqNumber;	// Come from gate board in header with gates and wall
		m_pIdataPacket->bSequenceLength	= gMaxChnlsPerMainBang;	// Come from gate board in header with gates and wall reading
		//m_pIdataPacket->bNumberOfSeqPoints	= 3;
		m_pIdataPacket->bMaxVChnlsPerSequence	= gMaxChnlsPerMainBang;
		m_pIdataPacket->wStatus		= 0x1234;
		m_pIdataPacket->wLoc		= m_pSCC->InstrumentStatus.wLoc;
		m_pIdataPacket->wAngle		= m_pSCC->InstrumentStatus.wAngle;
		m_pIdataPacket->wPeriod		= m_pSCC->InstrumentStatus.wPeriod;
		m_pIdataPacket->wStatus		= m_pSCC->InstrumentStatus.wStatus;
		m_pIdataPacket->wMsgSeqCnt = 50;
		m_pIdataPacket->uSync		= 0x5CEBDAAD;
		}


	stPeakData *pR				= &m_pIdataPacket->Results[0];

	s.Format(_T("Loc=%3d Angle=%3d Inst=%2d Stat=%04x Sync=0x5CEBDAAD\r\n"),
		m_pIdataPacket->wLoc, m_pIdataPacket->wAngle, m_pIdataPacket->bInstNumber, m_pIdataPacket->wStatus);
	SaveFakeData(s);
	// Get Nc Nx info for 1st channel  -- for debug from output file
	/********  DEBUG INFO ONLY ***************/
	pChannel	= m_pSCC->pvChannel[0][0];
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
	/********  DEBUG INFO ONLY ***************/

	// Since we are building the output, we must think we have 16 ascans peak held
	for ( jSeq = 0; jSeq < 5; jSeq++)	
		{
		for ( i = 0; i < gMaxChnlsPerMainBang; i++)
			{
			pChannel = m_pSCC->pvChannel[jSeq][i];
			//pChannel->m_PeakData.bSeqNum = jSeq;
			//pChannel->m_PeakData.bChNum = i;
			pChannel->GetPeakData();
			pChannel->CopyPeakData(&pR[i]);
			if ( i == 0)
				{
				s.Format(_T("\r\n[%3d] "), i);
				k = pR[i].bId2;
				t.Format(_T("%3d  "),k); s += t;
				k =pR[i].bOd3;
				t.Format(_T("%3d  "),k); s += t;
				k =pR[i].wTofMin;
				t.Format(_T("%4d    "),k); s += t;
				k =pR[i].wTofMax;
				t.Format(_T("%4d    "),k); s += t;
				}
			else
				{
				pChannel->CopyPeakData(&pR[i]);
#if 0
				pR[i].bId2 = pChannel->bGetIdGateMax();
				pR[i].bOd3 = pChannel->bGetOdGateMax();
				pR[i].wTofMin = pChannel->wGetMinWall();
				pR[i].wTofMax = pChannel->wGetMaxWall();
#endif
				}
			// put data into output FIFO  and reset peak holds
			// get and set status info for upsteam processes here before resetting peak hold
			pChannel->ResetGatesAndWalls();
			pChannel->SetRead();
			}	// for ( i = 0; i < MAX_CHNLS_PER_MAIN_BANG; i++)
		}	// j
	// Put the newly created packet into the linked list for output
	// For now send this message directly. In future, put into linked list
	// and signal the sender thread to empty list.
	theApp.PamSendToPag(m_pIdataPacket, sizeof(IDATA_PACKET));
	delete m_pIdataPacket;

	}




// Take the code from ServiceApp, tInstMsgProcess() and incorporate into this class
// Note that original code read input data from a socket and blocked until data was available.
// The class containing this procedure works by emptying input data which has been put into a linked list
// by the socket's OnReceive() function. It only runs when a message is posted for the thread to check
// the linked list.
// Packets stored in pData have the length of the packet prepended.
// Remove the integer length before processing
#undef MAKE_FAKE_DATA

#ifdef THIS_IS_SERVICE_APP

void CServerRcvListThread::IncStartCh(void)
	{
	m_Ch++;
	if (m_Ch >= gMaxChnlsPerMainBang)
		{
		m_Ch = 0;
		IncStartSeq();
		}
	}

//Only call from IncStartCh
void CServerRcvListThread::IncStartSeq(void)
	{
	m_Seq++;
	if (m_Seq >= gMaxSeqCount)
		{
		m_Seq = 0;
		}
	}

void CServerRcvListThread::ProcessInstrumentData(InputRawDataPacket *pIData)
	{
	// see ServicApp.cpp the procedure tInstMsgProcess() for legacy operation
	//
	//stSEND_PACKET *pBuf = (stSEND_PACKET *) pData;
	//int nStartSeqCount, nSeqQty;
	//SRawDataPacketOld *pOutput; before 2016-10-20
	//InputRawDataPacket *pFakeData;
	//InputRawDataPacket *pIData;
	int i, j;
	int iSeqPkt;
	BYTE bGateTmp;
	WORD wTOFSum;
	CvChannel *pChannel;
	CString s;


	// After 16 Ascans, send Max/Min wall and Nc qualified flaw values for 2 gates.
	i = sizeof(InputRawDataPacket);
	if (pIData->wByteCount >= INSTRUMENT_PACKET_SIZE -16)	//sizeof(SRawDataPacketOld))		// legacy 1040, future is ???
		{
		/******************************************************************/
		//  2016-10-20 start to migrate to new input data structures
		// throw away simulator data or current Sam data and make a new InputRawDataPacket
		//delete pIData;

		if (m_pOutputRawDataPacket == NULL)
			{
			m_pOutputRawDataPacket = new InputRawDataPacket;
			memset(m_pOutputRawDataPacket, 0, sizeof (InputRawDataPacket));
			}

#ifdef MAKE_FAKE_DATA
	// Basically Yqing's simulator gave us some bytes. Generate test data in place of those bytes.
	// Run the fake data through the Nc Nx operations and keep the Max, Min values
		//pIData = pFakeData = new InputRawDataPacket;
		// 1 Ascan reading for every virtual channel. For now assuming this is 5 sequence points.
		MakeFakeData(pIData);
		// Now that we have fake data, process it
		// How many vChannels?
#endif
		// debugging

		/***************** The peak hold opertion on all channels by PAP ********************/
		if (m_pElapseTimer)
			m_pElapseTimer->Start();
		//for ( i = 0; i < nSeqQty; i++)
		for ( iSeqPkt = 0; iSeqPkt < 7; iSeqPkt++)
			{
			// get the starting sequence number and channel from the instument data
			m_Seq = pIData->stSeqPkt[iSeqPkt].DataHead.bSeqNumber;	// only used to find which pChannel
			m_Ch  = pIData->stSeqPkt[iSeqPkt].DataHead.bChnlNumber;

			for ( j = 0; j < gMaxChnlsPerMainBang; j++)	// Assumes the data packet contains whole frames
				{
				pChannel = m_pSCC->pvChannel[m_Seq][m_Ch];
				//pChannel->m_GateID = pChannel->m_GateOD = 0;

				if ( NULL == pChannel)
					continue;
				// Get flaw Nc qualified Max values for this channel
				// Not defined what to do with status ???????
				bGateTmp = pChannel->InputFifo(eIf, pIData->stSeqPkt[iSeqPkt].RawData[j].bAmp1);
				bGateTmp = pChannel->InputFifo(eId, pIData->stSeqPkt[iSeqPkt].RawData[j].bAmp2);	// output of the Nc peak holder
				bGateTmp = pChannel->InputFifo(eOd, pIData->stSeqPkt[iSeqPkt].RawData[j].bAmp3);

				// Get Max and min tof for this channel
				wTOFSum = pChannel->InputWFifo(pIData->stSeqPkt[iSeqPkt].RawData[j].wTof);
				if (pChannel->AscanInputDone())
					{// time to move peak data into ouput structure
					// AddToIdataPacket will create the IdataPacket if it does not already exist.
					AddToIdataPacket(pChannel, j, iSeqPkt, 0);
					pChannel->ResetGatesAndWalls();
					if (MAX_RESULTS == GetIdataPacketIndex())
						{
						theApp.PamSendToPag(m_pIdataPacket, sizeof(IDATA_PACKET));
						delete m_pIdataPacket;
						m_pIdataPacket = NULL;
						}
					// move the result into the output message
					// If message is complete, signal to send it
					// and create a new buffer to hold the next output message
					}
				IncStartCh();

				m_Seq = GetStartSeq();
				m_Ch = GetStartCh(); // used only to select which pChannel
				}	// for ( j = 0; j < gMaxChnlsPerMainBang; j++)	
			}	// for ( iSeqPkt = 0; iSeqPkt < 7; iSeqPkt++)


		m_nElapseTime = m_pElapseTimer->Stop();		// elapse time in uSec for 128 AScans
#if 0
		s.Format(_T("Nc Nx processing for %d VChnls in %d uSec\n"), 
			7*gMaxChnlsPerMainBang, m_nElapseTime);
		TRACE(s);
		theApp.SaveDebugLog(s);
#endif
		}	// if (pBuf->nLength == 1040)

	else
		{
		// This is for data coming from Yiqing simulator... change once final design done.
		s.Format(_T("ProcessInstrumentData got a data packet of the wrong size, %d\n"),pIData->wByteCount);
		delete pIData;	// info in pOutput was put into a new structure and sent to the PAG
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


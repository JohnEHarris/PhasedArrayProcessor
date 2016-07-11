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
extern  int g_ArrayScanNum[NUM_OF_SLAVES];
extern 	C_MSG_ALL_THOLD  g_AllTholds;
extern 	C_MSG_NC_NX g_NcNx;
extern int  g_nPipeStatus;	// = PIPE_NOT_PRESENT;     /* temporary */
extern DWORD g_nJointNum;	// = 1;
extern int  g_nXloc;
extern int  g_nOldXloc;	// = 0;
extern WORD   g_nMotionBus;	// = 0;
extern float  g_fMotionPulseLen;	// = 0.506329f;
extern int    g_nShowWallBars;	// = 1;
extern WORD   g_nPulserPRF;	// = 1000;
extern float  g_fTimePerInch;
extern int    g_nXscale;	// = 900;
extern BOOL   g_bStartOfRevS1;
extern WORD   g_nPeriod;
extern I_MSG_RUN SendBuf;
extern short  g_nVelocityDt;               /* delta t to travel 4 inches in 1 ms clocks */
extern BOOL   g_bRunCalJoint;
extern WORD   g_nStation1Window;// = 0;       /* the inspect window to which the station 1 Idata is sent */
extern WORD   g_nStation2Window;// = 0;       /* the inspect window to which the station 2 Idata is sent */
extern DWORD  g_nStation1JointNum;// = 0;
extern DWORD  g_nStation2JointNum;// = 0;
extern BOOL   g_bAnyShoeDown;
extern I_MSG_RUN SendCalBuf;
extern I_MSG_CAL  CalBuf;
extern I_MSG_RUN ImageBuf[IMAGE_BUF_DIM];
extern int  g_nMmiSocket;

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
extern void NiosRawData_to_PeakData(SRawDataPacket *pInspData, PEAK_DATA *pPeakData, CServerRcvListThread *pServerRcvListThread);
// 	void NiosRawData_to_PeakData(SRawDataPacket *pInspData, PEAK_DATA *pPeakData, int nInspectMode);
extern void Inspection_Process_Control();
extern int	BuildImageMap(UDP_SLAVE_DATA *pSlvData, int si, BOOL *bStartOfRev);
extern I_MSG_RUN* GetNextImageBuf(void);
extern void ComputeEcc(I_MSG_RUN *pRun, C_MSG_ALL_THOLD *pThold);
extern void SendPipeLocation(int nStation, SITE_SPECIFIC_DEFAULTS *pSiteDefault);
extern BOOL GetNextRealTimeImageBuf(I_MSG_RUN** pIBuf);
extern void BuildCalMsg(UDP_SLAVE_DATA *pSlvData, int nSlave);
extern BOOL SendMmiMsg(int socketFD, I_MSG_RUN *pSendBuf);

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
	m_nFakeDataSeqNumber = 0;
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
		s.Format(_T("Instrument Client[%d] accepted to server on socket %s : %d\n"), m_nThreadIndex, 
			m_ConnectionSocket.m_pSCC->sClientIP4, m_ConnectionSocket.m_pSCC->uClientPort); 
		TRACE(s);
		TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
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
	CString s;
	//int i;
#ifdef THIS_IS_SERVICE_APP
#if 0
	for ( i = 0; i < MAX_WALL_CHANNELS; i++)
		{
		if (m_pRunAvg[i])	delete m_pRunAvg[i];
		m_pRunAvg[i] = NULL;
		}
#endif
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
	pData->DataHead.bMsgID	= eRawInsp;	// raw data=10
	pData->DataHead.bSeq	= m_nFakeDataSeqNumber;
	m_nFakeDataSeqNumber	+= 128;	// the next packet will 128 Ascans/Main bangs later
	m_nFakeDataSeqNumber	= m_nFakeDataSeqNumber % 128;

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
	// Since length == 1040 we have 128 ascan samples. The header tells us the 1st vChnl in the packet
	nStartSeqCount = pData->DataHead.bSeq;
	//Scan the whole packet to see when the start seq count occurs again .. this is the number of vChnls in th packet
	for ( i = 1; i < 128; i++)
		{
		if (pData->RawData
		}
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

	s = _T("\r\nPAM Output Data Packet\r\n");
	SaveFakeData(s);
	DATA_PACKET_1 *pOutputPacket = new (DATA_PACKET_1);
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
	theApp.PamSendToPag(pOutputPacket, sizeof(DATA_PACKET_1));
	delete pOutputPacket;

	}



// Instantiate and initialize the Running average class instances
// Before processing any instrument data.
//
#if 0

#ifdef THIS_IS_SERVICE_APP
afx_msg void CServerRcvListThread::InitRunningAverage(WPARAM w, LPARAM lParam)
	{
	int i;
	for ( i = 0; i < MAX_WALL_CHANNELS; i++)
		{
		// constructor sets default values for running average.
		m_pRunAvg[i] = new CRunningAverage(NX_TOTAL);	// default max len
//		m_pRunAvg[i]->SetMyInstMsgProcess(pInstMsgProcess);	// who's your daddy
		m_pRunAvg[i]->SetSlaveNumber(m_nThreadIndex);
		// figure out which channel I belong to
		// pRunAvg[i]->SetChannelNumber(??);
		// changes when config file changes chnl type
		}
#endif
	}
#endif

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

#ifdef MAKE_FAKE_DATA
	// Basically Yqing's simulator gave us some bytes. Generate test data in place of those bytes.
	// Run the fake data through the Nc Nx operations and keep the Max, Min values
	// After 16 Ascans, send Max/Min wall and Nc qualified flaw values for 2 gates.
	if (pBuf->nLength == 1040)
		{
		pRaw = (SRawDataPacket *) &pBuf->Msg;
		MakeFakeData(pRaw);
		// Now that we have fake data, process it
		// How many vChannels?
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




		}	// if (pBuf->nLength == 1040)
#else


	int nPkLength = pBuf->nLength;

	pData = (void *) &pBuf->Msg;	// pData recast to remove packet length

	BYTE *pB = (BYTE *) pData;		// byte ptr for byte processing
	I_MSG_RUN *pSendBuf;			// static (on stack) memory for constructing messages
	PEAK_DATA  ReadBuf;
	//BYTE recvBuf[RAW_DATA_PACKET_LENGTH]; pData is the raw data
	UDP_SLAVE_DATA SlvData;
	//int i, ic;
	int ic;
	int nReturn;
	//BOOL  bGetSlaveRev = TRUE;
	SITE_SPECIFIC_DEFAULTS SiteDefault;
	//int nSendCnt = 0;
	//int nOldXloc=-1;
	SRawDataPacket *pNiosRawData = (SRawDataPacket *) pData;		//recvBuf;
//	static int nRecvCnt = 0;
//	static short wClock_old = 100;

	int nSlave = m_nThreadIndex;	//pInstMsgProcess->GetInstNumber();		//*( (int *) pSlave);
	ReadBuf.RDP_number = nSlave;

	I_MSG_RUN SendBuf;				// sizeof is 1260 or 1264 depending on byte alignmnets
	I_MSG_RUN SendCalBuf;
	I_MSG_NET *pNetBuf = (I_MSG_NET *) &SendBuf;
	pNetBuf->MstrHdr.MsgId = NET_MODE;
	pNetBuf->bConnected[0] = 1;


	I_MSG_ASCAN *pAscanMsg = (I_MSG_ASCAN *) &SendBuf;
	pAscanMsg->MstrHdr.MsgId = ASCAN_MODE;

	// Get some global inspection state info about the entire machine and store local to this class
	m_nInspectMode = InspState.GetInspectMode();
	m_nMotionTime  = InspState.GetMotionMode();


	switch (pB[0])
		{
	case RAW_DATA_ID:
		//{	// RAW_DATA_ID
		//NiosRawData_to_PeakData(pNiosRawData, &ReadBuf, nInspectMode);
		// Nios Raw Data routine now knows which thread called and has access to its member variables
		// which includes ptrs to global static structures
		NiosRawData_to_PeakData(pNiosRawData, &ReadBuf, this);
		SlvData.PeakData = ReadBuf;	// how can this copy from ReadBuf to .PeakData
			
		if (nSlave == 0)
			{	// (nSlave == 0)
			nRecvCnt++;
			if ( (m_nInspectMode == RUN_MODE) && (m_nMotionTime == 1) )  //inspect in time mode
				{	  //inspect in time mode
				g_nXloc = (int) ( (float) nRecvCnt * 128.0f / ((float) g_nPulserPRF * g_fTimePerInch) );
				if (g_nXloc >= g_nXscale)
					nRecvCnt = -2 * g_nPulserPRF / 128;    //halt for 2 seconds

				if (g_nXloc < 0)
					{
					g_nPipeStatus = PIPE_NOT_PRESENT;
					g_nMotionBus = 0x4010;
					}
				else
					{
					g_nPipeStatus = PIPE_PRESENT;
					g_nMotionBus = 0x4011;
					}

				if ( (nRecvCnt % 10) == 0 )
					g_bStartOfRevS1 = TRUE;
				else
					g_bStartOfRevS1 = FALSE;
				}	  //inspect in time mode

			else    //inspection in motion mode
				{	//inspection in motion mode
				g_nOldXloc = g_nXloc;
				g_nXloc = (int) ( pNiosRawData->DataHead.wLocation * g_fMotionPulseLen + 0.5f);  //0.495 for the encoder on the three array hand scanner

				if (pNiosRawData->DataHead.bDin & 0x04)
					{
					g_nPipeStatus = PIPE_PRESENT;
					//g_nMotionBus = 0x4001 | ((pNiosRawData->DataHead.bDin & 0x01) << 4);  //get the direction bit

					if (g_nOldXloc > g_nXloc)
						g_nMotionBus = 0x4001 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going backward //get the home/away bit;  
					else
						g_nMotionBus = 0x4011 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going forward //get the home/away bit;
					}
				else
					{
					g_nPipeStatus = PIPE_NOT_PRESENT;
					//g_nMotionBus = 0x4000 | ((pNiosRawData->DataHead.bDin & 0x01) << 4);  //get the direction bit

					if (g_nOldXloc > g_nXloc)
						g_nMotionBus = 0x4000 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going backward //get the home/away bit;
					else
						g_nMotionBus = 0x4010 | ((pNiosRawData->DataHead.bDin & 0x01) << 14);  //going forward //get the home/away bit;
					}

				if ( ((pNiosRawData->DataHead.wClock / 10) == 0) && ((wClock_old / 10) != 0) )
					g_bStartOfRevS1 = TRUE;
				else
					g_bStartOfRevS1 = FALSE;
						
				wClock_old = pNiosRawData->DataHead.wClock;
				g_nPeriod = pNiosRawData->DataHead.wPeriod;
				}	//inspection in motion mode

					Inspection_Process_Control();	// c code in ServiceApp.cpp
			}	// (nSlave == 0)
		//}		// RAW_DATA_ID
		break;

	case ASCAN_ID:
		//{	// ASCAN_ID
		for (i=0; i<1024; i++)
			pAscanMsg->Ascan[i] = pB[i+DataHeadLength];

		memcpy( (void *) &(pAscanMsg->MstrHdr.MmiStat), (void *) pData, DataHeadLength);

		/* send the packet data to MMI */
		SendMmiMsg (g_nMmiSocket, &SendBuf);

		ReadBuf.wLineStatus = 0;   //force to forgo the following inspection data process step
		ReadBuf.RdpStatus = 1;
		//}	// ASCAN_ID
		break;

	default:
		//keep alive message
		//{
		ReadBuf.wLineStatus = 0;   //force to forgo the following inspection data process step
		ReadBuf.RdpStatus = 1;
		if (m_pSCC)
			{
			// crude app timer is about 50 ms. 20 ticks per second
			if ( uAppTimerTick - m_pSCC->uLastTick > 60 )
				{
				m_pSCC->uLastTick = uAppTimerTick;
				memset(&pNetBuf->MstrHdr,0 , sizeof(UDP_CMD_HDR));
				pNetBuf->MstrHdr.MsgId = NET_MODE;
				pNetBuf->MstrHdr.nSlave = m_nThreadIndex;
				// rc = send( g_nMmiSocket, (char *) &sendBuf, sizeof(I_MSG_RUN), 0 ); YG code copied
				theApp.PamSendToPag(pNetBuf, sizeof(I_MSG_RUN));	// copied from yg idle loop operation
				}
			}

		//}
		break;
		}	// switch (pB[0])



			//g_bConnected[nSlave+1] = 1;  /* This slave is alive since we received a message from it.*/
			//pInstMsgProcess->m_bConnected = 1;

	if (ReadBuf.wLineStatus != 0)
		{
		ReadBuf.RDP_number = nSlave;
		SlvData.PeakData = ReadBuf;			
		}

	if (m_nMotionTime == 1)   //inspection in time mode
		{
		SlvData.PeakData.xloc = g_nXloc;
		}

	if (nSlave > 0)   //if endcoder is only connected to the first array
		{
		SlvData.PeakData.xloc = g_nXloc;
		}

	InspState.GetSiteDefaults(&SiteDefault);	// site defaults copied into SiteDefault
	/***********/
#if 0
	switch (m_nInspectMode)
		{
	case RUN_MODE:
		if ( g_nPipeStatus != PIPE_PRESENT )  /* If no pipe in the system, do nothing. */
			break;
		if ( ReadBuf.wLineStatus == 0 )     /* If not inspection data, do nothing. */
			break;

		nReturn = BuildImageMap(&SlvData, nSlave, &g_bStartOfRevS1);

		if ( nReturn )
			{
			nSendCnt = 0;
			if (nReturn != IMAGE_BUF_DIM/*was == 1*/)
				{
				for (i=0; i<nReturn; i++)
					{
					pSendBuf = GetNextImageBuf();

					if ( pSendBuf != NULL )
						{
						pSendBuf->MstrHdr.MsgId = RUN_MODE;
						pSendBuf->InspHdr.Period = g_nPeriod;
						pSendBuf->InspHdr.VelocityDt = g_nVelocityDt;
						pSendBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE;
						if (g_bRunCalJoint)
							pSendBuf->InspHdr.status[1] |= CALIBRATION_JOINT;
						pSendBuf->InspHdr.status[2] = g_nMotionBus;
						pSendBuf->InspHdr.NextJointNum = g_nJointNum;
						pSendBuf->InspHdr.JointLength = ( g_nXloc + SiteDefault.nDefaultXOffset ) % g_nXscale;
						pSendBuf->InspHdr.JointNumber = g_nStation1JointNum;
						pSendBuf->MstrHdr.nWhichWindow = g_nStation1Window;
						ComputeEcc(pSendBuf, &g_AllTholds);

						if ( (pSendBuf->InspHdr.xloc >= 0)  && g_bAnyShoeDown )
							{
							/* send the IData to MMI --jeh modified to use theApp.PamSendToPag */
							if ( SendMmiMsg (g_nMmiSocket, pSendBuf) == FALSE )
								break;//return ERROR;
							}
						else
							SendPipeLocation(0, &SiteDefault);

						nSendCnt++;
						}
					}	// for (i=0; i<nReturn; i++)
				}

			/* send real time messages */
			//nSendCnt = 0;
			//printf ("***** Real time message *****\n");
			while ( GetNextRealTimeImageBuf(&pSendBuf) )
				{
				//printf ("real time message 1\n\n");
				if (pSendBuf != NULL)
					{
					//printf ("real time message 2\n\n");
					pSendBuf->MstrHdr.MsgId = RUN_MODE;
					pSendBuf->InspHdr.wLineStatus = PIPE_PRESENT | INSPECT_ENABLE_TRUSCOPE | REAL_TIME_DATA;
					pSendBuf->InspHdr.Period = g_nPeriod;
					pSendBuf->InspHdr.VelocityDt = g_nVelocityDt;
					pSendBuf->InspHdr.JointNumber = g_nStation1JointNum;
					pSendBuf->MstrHdr.nWhichWindow = g_nStation1Window;
					if (g_bRunCalJoint)
						pSendBuf->InspHdr.status[1] |= CALIBRATION_JOINT;
					pSendBuf->InspHdr.status[2] = g_nMotionBus;
					pSendBuf->InspHdr.NextJointNum = g_nJointNum;
					pSendBuf->InspHdr.JointLength = ( g_nXloc + SiteDefault.nDefaultXOffset ) % g_nXscale;
					if (pSendBuf->InspHdr.xloc >= 0)
						{
								//logMsg("wLinStatus = 0x%08X\n",pSendBuf->InspHdr.wLineStatus,0,0,0,0,0);
								//printf("xloc = %d, EchoBit = 0x%08X\n",pSendBuf->InspHdr.xloc,pSendBuf->InspHdr.EchoBit[0]);
								//printf("SegWallMin[25] = %d, SegMinChnl[25] = %d\n\n", pSendBuf->UtInsp.SegWallMin[25],pSendBuf->UtInsp.SegMinChnl[25]);
								/* send the IData to MMI */

								if ( SendMmiMsg (g_nMmiSocket, pSendBuf) == FALSE ) /*jeh modified to use theApp.PamSendToPag*/
									break;//return ERROR;

								for (i=0; i<MAX_SHOES; i++)
									pSendBuf->InspHdr.EchoBit[i] = 0x0000;

								//nSendCnt++;
						}
					}
				}	// while ( GetNextRealTimeImageBuf(&pSendBuf) )
#if 1
			if (nSendCnt == 0)
				SendPipeLocation(0, &SiteDefault); // may need to do something here
#endif
			}
		break;

	case CAL_MODE:
		if (ReadBuf.wLineStatus == 0)
			break;
		BuildCalMsg (&SlvData, nSlave);
		if (nSlave == 0)
			{
			if (nRecvCnt >= g_nPulserPRF/1000)
				{
				nRecvCnt = 0;

				memcpy ((void *) &SendCalBuf, (void *) &CalBuf, sizeof(I_MSG_CAL));

				/* send the calibration data to MMI jeh modified to use theApp.PamSendToPag */
				SendMmiMsg (g_nMmiSocket, &SendCalBuf); // <-- change this

				/* clear CalBuf */
				memset( (void *) &CalBuf, 0, sizeof(I_MSG_CAL));
				for (i=0; i<MAX_SHOES; i++)
					for (ic=0; ic<10; ic++)
						CalBuf.Shoe[i].MinWall[ic] = 0x3FFF;
				}
			}
		break;

	case NOP_MODE:
		if (m_pSCC)
			{
			// crude app timer is about 50 ms. 20 ticks per second
			if ( uAppTimerTick - m_pSCC->uLastTick > 60 )
				{
				m_pSCC->uLastTick = uAppTimerTick;
				memset(&pNetBuf->MstrHdr,0 , sizeof(UDP_CMD_HDR));
				pNetBuf->MstrHdr.MsgId = NET_MODE;
				// rc = send( g_nMmiSocket, (char *) &sendBuf, sizeof(I_MSG_RUN), 0 ); YG code copied
				theApp.PamSendToPag(pNetBuf, sizeof(I_MSG_RUN));	// copied from yg idle loop operation
				}
			}

		break;

	default:
		break;
		}	// switch (nInspectMode)
				
#endif		

	/***********/
		
		delete pBuf;		//pData;
#endif
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


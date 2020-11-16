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
#include "PA2Win.h"
#include "PA2WinDlg.h"
#include "..\Include\Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// I_AM_PAP is defined in the PAP project under C++ | Preprocessor Definitions 

#ifdef I_AM_PAP
// for random number generator
#include <stdlib.h>

class CInstState;
extern  CInspState InspState;

//extern UINT uAppTimerTick;
//extern CServiceApp theApp;



/** External function prototypes... mostly in Service.cpp   **/
//extern void Inspection_Process_Control();

// Assume this is PAG if not ServiceApp
#else
#include "stdafx.h"
#include "AfxSock.h"
#include "..\Include\Global.h"
#include "PA2Win.h"
#include "PA2WinDlg.h"

#endif


// ServerRcvListThread.cpp : implementation file
//


// This thread reads the data that the ServerSocket has received via the OnReceive() and placed into 
// the socket's associated Receiver linked list. It sepearates the reception of data via TCP/IP from
// the processing of that data. It also allows us to set a priority of execution different from the
// thread which owns the socket feeding the data if we want/need a different priority

// This thread is created by the ServerSocketOwnerThread in its InitInstance() function

// 2017-04-03 Remove the ServerRcvListThreadBase from the project.
// ON creation pass the specific ClientConnection ptr to the thread 
// and pass the server ID. Then the thread will know the nature
// of the server by its ID. ID= PAP for the GUI. We do not need to be concerned with
// the socket are anything related to TCP/IP. This thread merely call some function
// to process data in a linked list. The routine called is dependent on the server function
// identified by the server ID number. Further refinement in the function called is obtained
// by examining the message ID contained in the linked list of messages from the PAP in the
// case of the PAG - Phased Array GUI receiving packet from the PAP
// CServerRcvListThread

IMPLEMENT_DYNCREATE(CServerRcvListThread, CWinThread)

CServerRcvListThread::CServerRcvListThread()
	{
#ifdef I_AM_PAP
	m_nFakeDataSeqNumber = 0;
	m_nFrameCount = 0;
	m_nFakeDataCallCount = 0;
	m_bNiosGlitchCnt = 0;
	srand( (unsigned)time( NULL ) );	// seed random number generator
	MapbDin();	// 2018-05-10
#endif
	m_pElapseTimer = 0;
#if 0
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	// m_nMyServer, m_nClientIndex not available yet
	s.Format(_T("CServerRcvListThread[%d][%d] = 0x%08x, Id=0x%04x constructor\n"), m_nMyServer, m_nClientIndex, this, nId);
	TRACE(s);
#endif
	m_Seq = 0;
	//m_IdataInPt = 0;
	m_pIdataPacket = NULL;
	//m_uMsgSeqCnt = 0;
	}

CServerRcvListThread::~CServerRcvListThread()
	{
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	s.Format(_T("~CServerRcvListThread[%d][%d] = 0x%08x, Id=0x%04x has run\n"), m_nMyServer, m_nClientIndex, this, nId);
	TRACE(s);
	if (m_pElapseTimer)
		delete m_pElapseTimer;
	m_pElapseTimer = 0;

#ifdef I_AM_PAP
	if (m_pIdataPacket != NULL)
		delete m_pIdataPacket;
	m_pIdataPacket = 0;
#endif
	if (NULL == m_pSCC)			return;
	m_pSCC->pServerRcvListThread = NULL;
	}

// We have to over-ride the parents InitInstance since this thread is created by the parent in InitInstance()
// If we don't override, we will get an infinite loop of thread creation.
// This changed on 11-12-2012 when we made this a child of CWinThread
//
BOOL CServerRcvListThread::InitInstance()
	{
	CString s;
	int i;
	
#ifdef I_AM_PAP

#if 0
	s.Format(_T("Instrument Client[%d] accepted to server on socket %s : %d\n"), m_nThreadIndex, 
		m_ConnectionSocket.m_pSCC->sClientIP4, m_ConnectionSocket.m_pSCC->uClientPort); 
	TRACE(s);
	TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
#endif
	if (m_pElapseTimer == 0)
		{
		m_pElapseTimer = new CHwTimer( );
		}
	i = sizeof(CHwTimer);		// 364
	m_FDstartSeq = m_FDstartCh = 0;	// Fake data simulator state variables
	return TRUE;


#else
	// not I_AM_PAP
	i = m_nMyServer;
	s.Format(_T("m_pstSCM = 0x%x, m_pSCC = 0x%x, m_MyServer = %d\n"),
		m_pstSCM, m_pSCC, m_nMyServer);
	TRACE(s);

	return TRUE;

#endif		// not I_AM_PAP
	};

int CServerRcvListThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	// return CServerRcvListThreadBase::ExitInstance();
	CString s = _T("");
	int i = 0;
	//void *pV;

#ifdef I_AM_PAP
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
	s.Format(_T("CServerRcvListThread, Srv[%d]Instrument[%d] has exited\n"),m_nMyServer, m_nClientIndex);
	TRACE(s);
	return 0;	// CServerRcvListThreadBase::ExitInstance();

}


//BEGIN_MESSAGE_MAP(CServerRcvListThread, CServerSocketOwnerThread)	// changed (xx,CWin) to (xx,CServerSocketOwnerThread)
BEGIN_MESSAGE_MAP(CServerRcvListThread, CWinThread)


	ON_THREAD_MESSAGE(WM_USER_SERVERSOCKET_PKT_RECEIVED, ProcessRcvList)// manually added by jeh 11-06-12
//	ON_THREAD_MESSAGE(WM_USER_FLUSH_LINKED_LISTS, FlushRcvList)			// manually added jeh 09-20-16


END_MESSAGE_MAP()


// CServerRcvListThread message handlers
// Thread message WM_USER_SERVERSOCKET_PKT_RECEIVED activates this procedure
// comes from CServerSocket::OnReceive()

#ifdef I_AM_PAP
// Receives inspection packets from inspection machines and status info from pulser
// maybe need wparam to be server number, lparam to be client
// then m_pSCC = GetServerClientconnection(Server, Client)
afx_msg void CServerRcvListThread::ProcessRcvList(WPARAM Server, LPARAM Client)
	{
	void *pV;
	int i;
	IDATA_FROM_HW *pIdata;

	//m_pSCC = GetpSCC(Server,Client);
	if (m_pSCC)
		{
		// don't need the socket. socket could be closed but lists still exist
//		if (m_pSCC->pSocket)
//			{
		if (m_pSCC->pCSRcvPkt)
			{
			EnterCriticalSection(m_pSCC->pCSRcvPkt);
			while (i = m_pSCC->pRcvPktList->GetCount( ))
				{
				pV = m_pSCC->pRcvPktList->RemoveHead( );
				LeaveCriticalSection(m_pSCC->pCSRcvPkt);
				pIdata = (IDATA_FROM_HW *) pV;
				if (pIdata->wMsgID >= 0x300)
					{
					CString s;
					PULSER_DATA *pPulserData = (PULSER_DATA *) pIdata;
					s = _T("Pulser Feedback Data");
					ProcessPulserData(pPulserData);
					}
				else
					ProcessInstrumentData(pIdata);	// local call to this class memeber

				if (pIdata)
					delete pIdata;
				EnterCriticalSection(m_pSCC->pCSRcvPkt);
				}	// while data
			LeaveCriticalSection(m_pSCC->pCSRcvPkt);
			}// if (m_pSCC->pCSRcvPkt)
		else // no connection
			{
			TRACE(_T("ProcessRcvList Failed\n"));
			i = 1;
			}
		}	// if (m_pSCC)
	}


#else
// PAG

afx_msg void CServerRcvListThread::ProcessRcvList( WPARAM w, LPARAM lParam )
	{
	void *pV;
	int i;

	IDATA_PAP *pIdata;	// output data from PAP/PAM-- debugging
	//m_pSCC = GetpSCC(); receive list does not have to know about the rest of the structures
	if (m_pSCC)
		{
		if (m_pSCC->pSocket)
			{
			m_pSCC->pSocket->LockRcvPktList();

			while (i = m_pSCC->pRcvPktList->GetCount() )
				{
				pV = m_pSCC->pRcvPktList->RemoveHead();
				m_pSCC->pSocket->UnLockRcvPktList();
				pIdata = (IDATA_PAP *)pV;	// examine contents with debugger
#ifdef I_AM_PAG
				ProcessPAP_Data(pV);
#else
				delete pV;
#endif
				
				m_pSCC->pSocket->LockRcvPktList();
				}
			m_pSCC->pSocket->UnLockRcvPktList();
			}
		}
	}

#endif


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

// For debugging, save the fake data and the contents of the output tcpip packet made from fake data
void CServerRcvListThread::SaveFakeData(CString& s)
	{
	pMainDlg->SaveFakeData(s);
	}

void CServerRcvListThread::SaveDebugLog(CString& s)
	{
	pMainDlg->SaveDebugLog(s);
	}

#ifdef I_AM_PAP
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
	m_FDstartSeq = m_FDstartSeq % gnSeqModulo;
	if (m_FDstartSeq >= gnSeqModulo	)
		{
		m_FDstartSeq = 0;
		}
	}

// Make fake data to test Nc and Nx operations
// change input data type to InputRawDataPacket  IDATA_FROM_HW
//void CServerRcvListThread::MakeFakeData(SRawDataPacketOld *pData)
// ADD state variable to keep track of start sequence and start channel number
// Can be different start location on every call.
// Loop here until we get MAX_RESULTS and then return fake data to caller.
//
void CServerRcvListThread::MakeFakeData(IDATA_FROM_HW *pData)
	{
	int i, jSeq, k, n, iSeqPkt;
	CString s,t;

	s.Format(_T("\r\n\r\nFake Data Call = %5d\r\n       G1   ID   OD   TOF4\r\n"), m_nFakeDataCallCount++);
	SaveFakeData(s);

	MakeFakeDataHead(pData);
	// we could have fewer than 32 channels, ie sequence number could be less than 32
	// Consider the number of ascans needed to fire all multiplexed channels (say 8)
	// Give this number the name "frame". A frame is the number of ascans to fire all the channels
	// Then 8 ascans would be a frame  - and 128/8 = 16 frames in the packet.
	// Every 16 frames, generate the max and min wall reading and the max Nc qualified flaw reading for
	// every channel. 

	for (iSeqPkt = 0; iSeqPkt < 32; iSeqPkt++)
		{
		pData->bStartSeqNumber = GetFDstartSeq();
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
#ifdef CLIVE_RAW_DATA
				k = pData->stSeqPkt[iSeqPkt].RawData[i].bAmp1 = 1 + (GetRand() / 2);	// 1-51 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
#endif
				k = pData->Seq[jSeq].vChnl[i].bAmp2  = 5 + (GetRand() / 2);	// 5-55 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
				k = pData->Seq[jSeq].vChnl[i].bAmp3 = 10 + (GetRand() / 2);	// 10-60 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
				k = pData->Seq[jSeq].vChnl[i].wTof = 300 + GetRand();
				t.Format(_T("%4d    "), (k)); s += t;
				SaveFakeData(s);
				}
			else
				{
#ifdef CLIVE_RAW_DATA
				pData->stSeqPkt[iSeqPkt].RawData[i].bAmp1 = 1 + (GetRand() / 2);
#endif
				pData->Seq[jSeq].vChnl[i].bAmp2  = 5 + (GetRand() / 2);	// 5-55 amplitude
				pData->Seq[jSeq].vChnl[i].bAmp3 = 10 + (GetRand() / 2);	// 10-60 amplitude
				pData->Seq[jSeq].vChnl[i].wTof = 300 + GetRand();
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


void CServerRcvListThread::MapbDin(void)
	{
	BYTE i;
	BYTE bTmp;

	// Robert doesn't care about 2 high order bits
	for (i = 0; i < 64; i++)
		{
		bTmp = FWD_RC;	// always forward for now 2018-05-10
		if (i & HD_SAM)	bTmp |= HD_RC;
		if (i & IE_SAM)	bTmp |= IE_RC;
		if (i & PP_SAM)	bTmp |= PP_RC;
		bDinMap[i] = bTmp;
		}
	}

// Build Output packet as individual channel peak held data becomes available
// could return the index in Idata
// If SendFlag is non zero, send the packet as is
// CHANNEL order is the received packet order from NIOS. Accomplished by assigning
// start channel from NIOS to the beginning of the Idata Packet

void CServerRcvListThread:: AddToIdataPacket(CvChannel *pChannel, IDATA_FROM_HW *pIData, int nSeq)
	{
	CString s;
	if (m_pIdataPacket == NULL)
		{
		//m_pIdataPacket is a member of CServerRcvListThread and is visible to all vChannels here
		m_pIdataPacket = new (IDATA_PAP);	// sizeof = 1088
		memset((void *) m_pIdataPacket,0, sizeof(IDATA_PAP));
#if 0
	int i, j;

		i = pChannel->m_PeakData.bChNum;	// debugging
		j = pChannel->m_bSeq;
		s.Format( _T( "m_pIdataPacket created by vChannel=%d in Seq=%d,  nSeq_arg = %d, Od3 = %d\n" ), 
			i, j, nSeq, pIData->Seq[nSeq].vChnl->bAmp3 );
		TRACE( s );
#endif
		// Let ID 0 slip thru
		if (m_pIdataPacket->wMsgID != eFakeDataID)
			m_pIdataPacket->wMsgID = eNcNxInspID;
		//m_pIdataPacket->wByteCount = sizeof(IDATA_PAP);
		m_pIdataPacket->wByteCount = pIData->bSeqModulo * pIData->bMaxVChnlsPerSequence * sizeof(stPeakChnlPAP) + sizeof(IDATA_FROM_HW_HDR);
		m_pIdataPacket->uSync = SYNC;
		m_pIdataPacket->wMsgSeqCnt = 0;	// properly incremented when sent by CClientCommunicationThread::TransmitPackets
		m_pIdataPacket->bPapNumber = gbAssignedPAPNumber;		// pIData->bPapNumber; only 1 PAP for now in 2018
		m_pIdataPacket->bBoardNumber = pIData->bBoardNumber;
		m_pIdataPacket->bBoardType = 1;		// 1 = wall, 0 = socomate

		m_pIdataPacket->bStartSeqNumber = pChannel->m_bSeq;	// the key to correctly mapping the data to the output packet
#if 0
		//if (pChannel->m_bSeq)
			{
			s.Format(_T("SeqNum=%d\n"), pChannel->m_bSeq);
			TRACE(s);
			}
#endif

		m_pIdataPacket->bSeqModulo = gnSeqModulo = pIData->bSeqModulo;
		m_pIdataPacket->bMaxVChnlsPerSequence = pIData->bMaxVChnlsPerSequence;
		m_pIdataPacket->bStartChannel = pChannel->m_bChnl;
		m_pIdataPacket->bSeqPerPacket = gnSeqModulo;

		m_pIdataPacket->bNiosGlitchCnt = pIData->bNiosGlitchCnt;
		m_pIdataPacket->bCmdQDepthS = pIData->bCmdQDepthS;
		m_pIdataPacket->bCmdQDepthL = pIData->bCmdQDepthL;

		m_pIdataPacket->bMsgSubMux = pIData->bMsgSubMux;
		memcpy(&m_pIdataPacket->bNiosFeedback, &pIData->bNiosFeedback, 8);
		m_pIdataPacket->wLastCmdSeqCnt = gLastCmd.wMsgSeqCnt;		//pIData->wLastCmdSeqCnt;
		// map Sams bDin to Roberts
		m_pIdataPacket->bDin = pIData->bDin;//  bDinMap[pIData->bDin & 0x3f]; 5/23/18 allow all jeh
		m_pIdataPacket->bCmdQDepthP = pIData->bCmdQDepthP;	// Come from gate board in header with gates and wall reading
		m_pIdataPacket->wLocation = pIData->wLocation;		//m_pSCC->InstrumentStatus.wLoc;
		m_pIdataPacket->wAngle = pIData->wAngle;	//m_pSCC->InstrumentStatus.wAngle;
		m_pIdataPacket->wPeriod = pIData->wPeriod;	// m_pSCC->InstrumentStatus.wPeriod;
		m_pIdataPacket->wRotationCnt = pIData->wRotationCnt;
		m_pIdataPacket->wStatus = 0;	// to be determined
		m_pIdataPacket->wVersionHW = pIData->wVersionHW;
		m_pIdataPacket->wVersionSW = pIData->wVersionSW;

		m_pIdataPacket->wLastCmdId = pIData->wLastCmdId;
		m_pIdataPacket->w1stWordCmd = pIData->w1stWordCmd;
		m_pIdataPacket->bCmdSeq = pIData->bCmdSeq;
		m_pIdataPacket->bCmdChnl = pIData->bCmdChnl;
		m_pIdataPacket->bCmdGate = pIData->bCmdGate;
		m_pIdataPacket->bAdcMsgIdErrorCntPAP = pIData->bCmdSpare;

		//m_IdataInPt					= 0;	// insertion index in output data structrure-- not after 2017-08-22
		m_nStoredChannelCount = 0;
		//
		}

	// Change operation from copying in max size Idata packet to peak holding in the front end portion
	//pChannel->CopyPeakData(&m_pIdataPacket->PeakChnl[m_IdataInPt++]); // notice increment of m_IdataInPt
	//pChannel->CopyPeakToIdata((IDATA_PAP *)  m_pIdataPacket, (nSeq + pIData->bStartSeqNumber) % gnSeqModulo);
	pChannel->CopyPeakToIdata((IDATA_PAP *)  m_pIdataPacket, nSeq);
	m_nStoredChannelCount++;	// stored another peak held data set
	
	pChannel->ClearDropOut();
	pChannel->ClearOverRun();
	pChannel->SetRead();
	// if (nSendFlag) Send the packet now.
	}	// AddToIdataPacket


#endif


// delete the pRaw packet and create a new packet for transmission
// Fill the new packet with max/min data from each pChannel instance

//void CServerRcvListThread::BuildOutputPacket(SRawDataPacketOld *pRaw)
// BuildOutputPacket was called because we have gone thru 16 Ascan samples in the whole machine.
// May not work this way for a real machine -- not used after simulating data.. discontinued in Jan 2017



// Take the code from ServiceApp, tInstMsgProcess() and incorporate into this class
// Note that original code read input data from a socket and blocked until data was available.
// The class containing this procedure works by emptying input data which has been put into a linked list
// by the socket's OnReceive() function. It only runs when a message is posted for the thread to check
// the linked list.
// Packets stored in pData have the length of the packet prepended.
// Remove the integer length before processing
#undef MAKE_FAKE_DATA

#ifdef I_AM_PAP

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
	m_Seq = m_Seq % gnSeqModulo;
	}

// The only way to get Idata from the PAP is to use the ClientConnectionManangement TCPIP
// link. This routine puts the Idata packet ptr into a linked list
// and then commands the CCM_PAG component to empty the linked list thru
// the client socket to the PHASED ARRAY GUI - PAG
// 2018/06/18 now 2 client communication threads. 0 is NcNx wll, 1 is all wall ie raw adc data
//
void CServerRcvListThread::SendIdataToPag(GenericPacketHeader *pIdata, int nWhichClient)
	{
#ifdef I_AM_PAP
	int i;
	CString s;
	CClientCommunicationThread *pThread;
	IDATA_FROM_HW *pHwData = 0;


	switch (nWhichClient)
		{
	case 0:
	default:
		if (pCCM_PAG == NULL)
			{
			//ASSERT(0);
			if (pIdata)
				delete pIdata;
			return;
			}
		if (pCCM_PAG->m_pstCCM == NULL)
			{
			ASSERT(0);
			delete pIdata;
			return;
			}
		if (pCCM_PAG->m_pstCCM->pSocket == NULL)
			{
			// restart client connection  .. how
			delete pIdata;
			TRACE(_T("pCCM_PAG->m_pstCCM->pSocket == NULL.. attempt restart of connection\n"));
			if (pCCM_PAG->m_pstCCM->pReceiveThread == NULL)
				{
				// destroy CCM and start all over?
				s = _T("pCCM_PAG->m_pstCCM->pReceiveThread == NULL.. Oh My\n");
				TRACE(s);
				pMainDlg->SaveDebugLog(s);
				//pCCM_PAG->InitReceiveThread();
				//ASSERT(0);
				}
			else
				{
				pCCM_PAG->m_pstCCM->bConnected = 0;	// causes a timed reconnect attempt ?
				s = _T("SendIdataToPag() attempt connection restart\n");
				TRACE(s);
				/**********************************/
				pMainDlg->SaveDebugLog(s);
				pCCM_PAG->InitReceiveThread();
				/**********************************/

				}
			return;
			}
		if (pCCM_PAG->m_pstCCM->pSendThread == NULL)
			{
			ASSERT(0);
			delete pIdata;
			return;
			}
		pThread = pCCM_PAG->m_pstCCM->pSendThread;
		pCCM_PAG->LockSendPktList();
		pCCM_PAG->AddTailSendPkt((void*)pIdata);	// input Idata ptr move to another linked list
		i = pCCM_PAG->m_pstCCM->pSendPktList->GetCount();
		pCCM_PAG->UnLockSendPktList();
		//ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, CClientCommunicationThread::TransmitPackets)
		PostThreadMessage(WM_USER_SEND_TCPIP_PACKET, (WORD)3, (LPARAM)i);	// maybe change WORD 3 to select correct ccm_pag
		break;

	case 1:	// all wall data... different client socket

		if (pCCM_PAG_AW == NULL)
			{
			//ASSERT(0);
			if (pIdata)
				delete pIdata;
			return;
			}
		if (pCCM_PAG_AW->m_pstCCM == NULL)
			{
			ASSERT(0);
			delete pIdata;
			return;
			}
		if (pCCM_PAG_AW->m_pstCCM->pSocket == NULL)
			{
			// restart client connection  .. how
			delete pIdata;
			TRACE(_T("pCCM_PAG->m_pstCCM->pSocket == NULL.. ignore and keep going\n"));
			if (pCCM_PAG_AW->m_pstCCM->pReceiveThread == NULL)
				{
				// destroy CCM and start all over?
				TRACE(_T("pCCM_PAG_AW->m_pstCCM->pReceiveThread == NULL.. Oh My\n"));
				//ASSERT(0);
				}
			else
				{
				pCCM_PAG_AW->m_pstCCM->bConnected = 0;	// causes a timed reconnect attempt ?
				s = _T("SendIdataToPag() attempt connection restart\n");
				TRACE(s);
				/**********************************/
				pMainDlg->SaveDebugLog(s);
				pCCM_PAG_AW->InitReceiveThread();
				/**********************************/
				}
			return;
			}
		if (pCCM_PAG_AW->m_pstCCM->pSendThread == NULL)
			{
			ASSERT(0);
			delete pIdata;
			return;
			}
		pHwData = (IDATA_FROM_HW *)pIdata;
		pThread = pCCM_PAG_AW->m_pstCCM->pSendThread;
		pCCM_PAG_AW->LockSendPktList();
		pCCM_PAG_AW->AddTailSendPkt((void*)pIdata);
		i = pCCM_PAG_AW->m_pstCCM->pSendPktList->GetCount();
		pCCM_PAG_AW->UnLockSendPktList();
		//ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, CClientCommunicationThread::TransmitPackets)
		PostThreadMessage(WM_USER_SEND_TCPIP_PACKET, (WORD)3, (LPARAM)i);	// maybe change WORD 3 to select correct ccm_pag
		break;
		}
#endif
	}	//SendIdataToPag

// debugging
// do sequences ever get out of order
void CServerRcvListThread::CheckSequences(IDATA_PAP *pIdataPacket)
	{
	unsigned int nStartSeq, nSeqModulo, nChnl;	// test vars
	unsigned int nChnlModulo;
	unsigned int i, nLastChnl;
	int j;
	CString s;
	nStartSeq = pIdataPacket->bStartSeqNumber;
	nSeqModulo = pIdataPacket->bSeqModulo;
	nChnl = nStartSeq * 8;	
	nChnlModulo = nSeqModulo * 8;	// 8 chnls per sequence
	if (nStartSeq > 2)
		i = 0;

	//nLastChnl = gbSeqPerPacket * 8;
	nLastChnl = nSeqModulo * 8;
	for (i = 0; i < nLastChnl; i++)
		{
		//if ((nChnl % nChnlModulo) != (pIdataPacket->PeakChnl[i].bChNum % nChnlModulo))
		if ((i % nChnlModulo) != (pIdataPacket->PeakChnl[i].bChNum % nChnlModulo))
			{
			j = i / 8;

			s.Format(_T("Sequence error in sequence %d, chnl %d, start Seq = %d\n"), 
				j, i, nStartSeq);
			TRACE(s);
			// set a status bit in header to indicate sequence error 
			pIdataPacket->wStatus |= 8;	// bits 0,1,2 are received msg count error indicators
			break;
			}
		nChnl++;
		}

	}

// pIdata is deleted in the function that called ProcessInstrumentData
void CServerRcvListThread::ProcessInstrumentData(IDATA_FROM_HW *pIData)
	{
	int i, j, k,l;
	int nLastSeq, nStartSeq;
	int iSeqPkt, nPkt;
	int nFakeId = 99;
	BYTE bGateTmp2, bGateTmp3;
	WORD wTOFSum;
	CvChannel *pChannel;
	CString s;
	IDATA_FROM_HW *pIdataHw;
	//WORD wID;	//distinguish between NcNx and Fake data

	if (pIData->wMsgID == eFakeDataID)
		{
		i = 0;	// debug test
		}

	// After 16 Ascans, send Max/Min wall and Nc qualified flaw values for 2 gates.
	//After AUG 2017 Only ID, OD, MinWall sent to PAG
	i = sizeof(IDATA_FROM_HW);
	if (pIData->wByteCount >= 256)
		{
		switch(pIData->wMsgID)
			{
		case eFakeDataID:
			nFakeId = pIData->wMsgID;
			i = pIData->wByteCount;	// testing/debugging
			// Force an all-wall type packet
			pIData->wByteCount = 1088;
			pIdataHw = new(IDATA_FROM_HW);	//faking it
			pIdataHw->wMsgID = eFakeDataID;
			memcpy((void *) pIdataHw, (void *)pIData, sizeof(IDATA_FROM_HW));
			SendIdataToPag((GenericPacketHeader *)pIdataHw, 0);	// send to client 0
			break;
		case eNcNxInspID:
			{
			// For all wall if full packet queue now to All Wall Processor
			if (pIData->wByteCount == 1088) // and All Wall Flag ON
				{
				pIdataHw = new(IDATA_FROM_HW);
				pIdataHw->wMsgID = eAdcIdataID;		// All Wall data
				memcpy((void*)pIdataHw, (void*)pIData, sizeof(IDATA_FROM_HW));
				memcpy((void*) &gLastAllWall, (void*)pIData, sizeof(IDATA_FROM_HW));
				SendIdataToPag((GenericPacketHeader *)pIdataHw, 1);	// send to client 1
				}

			/***************** The peak hold opertion on all channels by PAP ********************/
			// if m_Seq changes, ie m_Seq != pIData->bStartSeqNumber. FLUSH
			// the IDATA_PAP structure and start new.
			// Changes in sequence length or expected next sequence require a restart of the
			// peak holding operation
			//
#if 1

			pIData->bCmdQDepthP = gbCmdQDepthP;
			// packet all good to seq mod, but  Pap = 1,startSeq=0x40, modulo = 4
			if (pIData->bNiosGlitchCnt != m_bNiosGlitchCnt)
				{
				if (m_pIdataPacket)
					{
					s.Format(_T("Deleting m_pIdataPacket, glicth cnt = %d, m_pIdataPacket = 0x%08x\n"), 
						pIData->bNiosGlitchCnt, (UINT)m_pIdataPacket);
					pMainDlg->SaveFakeData(s);
					m_bNiosGlitchCnt = pIData->bNiosGlitchCnt;
					delete m_pIdataPacket; 
					m_pIdataPacket = 0;
					m_IdataInPt = 0;
					}
				}
			// 2018-05-21 per Clive, send this packet from ADC to UUI and then process for NcNx
			// This to be used for All Wall processing in UUI

			// This seems to be a problem with simulated data. Always m_Seq=1 when start sequence if 0
			// do not take this code out. Yanming thinks we should skip the data throwaway
			// I think we are using bad data. Yet to be determined.
#if 0
			if (m_Seq != pIData->bStartSeqNumber)
				{
				if (m_pIdataPacket)
					{
					s.Format(_T("Deleting m_pIdataPacket, start m_Seq error %d  != %d, IdataSeqNumber, m_pIdataPacket = 0x%08x\n"),  
						m_Seq, pIData->bStartSeqNumber,  (UINT)m_pIdataPacket);
					pMainDlg->SaveFakeData(s);
					delete m_pIdataPacket;
					m_pIdataPacket = 0; 
					//m_IdataInPt = 0;
					pIData->bNiosGlitchCnt++;
					}
				m_Seq = pIData->bStartSeqNumber;
				}
#endif
			m_Seq = pIData->bStartSeqNumber;	// 2018-03-07 fix break in testing

#endif

			if (gnSeqModulo != pIData->bSeqModulo)
				{

				if (m_pIdataPacket)
					{
					s.Format(_T("Deleting m_pIdataPacket, gnSeqModulo error %d  != %d, IdataSeqNumber, m_pIdataPacket = 0x%08x\n\n"),
						gnSeqModulo, pIData->bSeqModulo, (UINT)m_pIdataPacket);
					pMainDlg->SaveDebugLog(s);
					delete m_pIdataPacket;
					m_pIdataPacket = 0;
					//m_IdataInPt = 0;
					pIData->bNiosGlitchCnt = ++m_bNiosGlitchCnt;
					}
				gnSeqModulo = pIData->bSeqModulo;
				}

			// get the starting sequence number and channel from the instument data
			m_Seq = nStartSeq = pIData->bStartSeqNumber;	// only used to find which pChannel
			m_Ch = 0;	// pIData->stSeqPkt[iSeqPkt].DataHead.bChnlNumber;
			gnSeqModulo = pIData->bSeqModulo;
			if (gnSeqModulo == 0) gnSeqModulo = 3;
			m_nFullPacketChnls = gnSeqModulo*8;
			gbStartSeqNumberIncrement = 32 % gnSeqModulo;
			// gnSeqModulo channels
			gbSeqPerPacket = 32;	// (32 / gnSeqModulo) * gnSeqModulo;

			nLastSeq = (m_Seq + gbStartSeqNumberIncrement + gnSeqModulo -1 ) % gnSeqModulo;
#if 0
			s.Format(_T("Fake Data Initial m_Seq Number = %d, Last Seq = %d\n"), m_Seq, nLastSeq);
			pMainDlg->SaveFakeData(s);
#endif

			if (m_pElapseTimer)
				m_pElapseTimer->Start();

			iSeqPkt = pIData->bStartSeqNumber;

			// loop thru up to 32 sequences.
			for (nPkt = 0; nPkt < gbSeqPerPacket; nPkt++)
				{

				for (j = 0; j < gMaxChnlsPerMainBang; j++)	// Assumes the data packet contains whole frames
					{	// channel loop
					if (j != m_Ch)
						s = _T( "Chnl Error" );
					pChannel = m_pSCC->pvChannel[m_Seq][m_Ch];
					if (NULL == pChannel)
						continue;
					// Get flaw Nc qualified Max values for this channel
					// Not defined what to do with status ???????
#ifdef CLIVE_RAW_DATA
					bGateTmp = pChannel->InputFifo(eIf, pIData->stSeqPkt[iSeqPkt].RawData[j].bAmp1);
#endif		

					// debug helper
					if ((m_Seq == 2) && (m_Ch == 5))
						s = _T( "Should be 99 & 999" );
					bGateTmp2 = pChannel->InputFifo(eId, pIData->Seq[nPkt].vChnl[j].bAmp2);	// output of the Nc peak holder
					bGateTmp3 = pChannel->InputFifo(eOd, pIData->Seq[nPkt].vChnl[j].bAmp3);

					// Get Max and min tof for this channel for debug display on PAP
					if ((m_Seq == 0) && (m_Ch == 0))
						{
						//if (gwMin0 = pIData->Seq[0].vChnl[0].wTof)
						gwMin0 = pIData->Seq[nPkt].vChnl[0].wTof;
						if (gwMin0 == 0)	gwZeroCnt++;
						else gwNot0++;
						if (gwMax0 < pIData->Seq[0].vChnl[0].wTof)
							gwMax0 = pIData->Seq[0].vChnl[0].wTof;
						}
					if ((m_Seq == 1) && (m_Ch == 0))
						{
						gwMin1_0 = pIData->Seq[nPkt].vChnl[0].wTof;
						}
					if ((m_Seq == 2) && (m_Ch == 0))
						{
						gwMin2_0 = pIData->Seq[nPkt].vChnl[0].wTof;
						}
					// **** InputWFifo Also advances input ptr for all fifo's in this channel. ****
					wTOFSum = pChannel->InputWFifo(pIData->Seq[nPkt].vChnl[j].wTof);
					//pChannel->CountInputs();
					// testing for unique channel
					k = iSeqPkt % gnSeqModulo;
#if 0
					if ((k == 2) && (m_Ch == 5))
						{
						if ((pIData->Seq[nPkt].vChnl[j].bAmp2 == 99) &&
							(pIData->Seq[nPkt].vChnl[j].bAmp3 == 25) &&
							(pIData->Seq[nPkt].vChnl[j].wTof == 999))
							k = j;	//break point
						else
							TRACE(_T("Seq2, Ch5 data error\n"));
						}
#endif

					// for debugging seq len = 8
					//if ((k == 7) && (m_Ch == 7))
					// is FIFO input ptr back to 0? This is set by wall operation above
						l = pChannel->m_bInputCnt;	// help debugging
					if (pChannel->AscanInputDone())
						{
						// time to move peak data into output structure
						// AddToIdataPacket will create the IdataPacket if it does not already exist.
						// should be the first 'chnl' to complete the Nc filter 
						// the first channel of the start sequence -- seems logical
						AddToIdataPacket(pChannel, pIData,  nPkt);
						pChannel->ResetGatesAndWalls();
						
						// MAX_RESULTS is the number of uniques channels in the inspection machine = SeqLen*(Chns/main_bang)

						// if unique seq length = 3, then after 16*3 main bangs (48 main bangs)
						// we will have collected all 3*8 = 24 channels
						// Send these 24 channel to PAG/ Display & processing system
						// packet length will be 24*8 + header = 192+64 = 256 bytes
						if (m_nFullPacketChnls == m_nStoredChannelCount)	// Send as soon as last input of unique channels
							{
							// create an Idata packet with New. Copy m_pIdataPacket to IdataPacketOut linked list
							// send a thread message to theApp or a new sender thread to empty the linked list.
							// or better yet signal the ccm_pag thread to empty alist  directed to PAG
							//m_pIdataPacket->wByteCount = sizeof(IDATA_PAP);
							m_pIdataPacket->wByteCount = sizeof(IDATA_PAP_HDR) + m_nFullPacketChnls*sizeof(stPeakChnlPAP);
							//theApp.PamSendToPag(m_pIdataPacket, pIData->wByteCount); // get len from data packet
							//delete m_pIdataPacket;
							//m_pIdataPacket = NULL;
							// SendIdataToPag crashes when attempting to delete the input packet. Make a copy here
							// that is not a class member
							IDATA_PAP *pIdataPacket = new (IDATA_PAP);
							memcpy((void*)pIdataPacket, (void *)m_pIdataPacket, sizeof(IDATA_PAP));
							//
							// debugging - check outgoing packet for proper sequencing of channels
#ifdef _DEBUG
							CheckSequences(pIdataPacket);
#endif
							//pIdataPacket->wMsgID = NC_NX_IDATA_ID; //already 1 before break
							SendIdataToPag((GenericPacketHeader *)pIdataPacket,0);
							memcpy((void *)&gLastIdataPap, (void *)pIdataPacket, sizeof(IDATA_PAP));	// for debugging
							if (m_pIdataPacket)
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
					}	// for ( j = 0; j < gMaxChnlsPerMainBang; j++)		// channel loop
				iSeqPkt++;

				iSeqPkt = iSeqPkt %	gbSeqPerPacket;			//gnMaxSeqCount -- never used;
				}	// for 32 SEQUENCES

			// Now that finished, is m_Seq == nLastSeq???
			if (m_Seq != (nLastSeq + 1) % gnSeqModulo)
				{
				s.Format(_T("At end of packet m_Seq = %d != (Last Seq+1), Last Seq = %d\n"), m_Seq, nLastSeq);
				pMainDlg->SaveFakeData(s);
				m_Seq = (nLastSeq + 1) % gnSeqModulo;
				}

			m_nElapseTime = m_pElapseTimer->Stop();		// elapse time in uSec for 128 AScans
#if 0
			s.Format(_T("Nc Nx processing for 256 VChnls in %d uSec\n"), m_nElapseTime);
			TRACE(s);
			pMainDlg->SaveDebugLog(s);
			pMainDlg->SaveFakeData(s);
#endif
			/***************** The peak hold opertion on all channels by PAP ********************/
			break;
			}	// case eNcNxInspID:


			// DON'T TOUCH OR DELETE m_pIdataPacket - it only relates to inspection data


		case eAscanID:	//AScan data... pass thru to PAP
			{
			ASCAN_DATA *pIdataPacket = new (ASCAN_DATA);
			memcpy((void*)pIdataPacket, (void *)pIData, sizeof(ASCAN_DATA));
			// attach lastest Pulser data to this packet 2018-10-19

			pIdataPacket->wPulserCmds	= gwPulserCmds;
			pIdataPacket->wFPGA_VersionP = gwFPGA_VersionP;
			pIdataPacket->wNIOS_VersionP = gwNIOS_VersionP;
			pIdataPacket->wCPU_TempP	= gwCPU_Temp;

			SendIdataToPag((GenericPacketHeader *)pIdataPacket, 0);
			memcpy((void *)&gLastAscanPap, (void *)pIdataPacket, sizeof(ASCAN_DATA));
			//delete m_pIdataPacket;
			//m_pIdataPacket = NULL;
			break;
			}	// case eAscanID

		case eReadBackID:	// Read back.. pass thru to PAP
			{
			READBACK_DATA *pRb = new (READBACK_DATA);
			memcpy((void*)pRb, (void *)pIData, pIData->wByteCount);
			
			//memcpy((void *)&gLastGateCmd, (void *)pIn, pIn->wByteCount);
			//memcpy((void *)&gLastGateCmd.Seq[nSeq], (void *)pRb->ReadBack, nByteCount)
			gLastGateCmd.wSeq = pRb->bSeqNumber;
			//SendIdataToPag((GenericPacketHeader *)pIdataPacket, 0);	// this path will eventually delete pIdataPacket

			// gLastRdBkPap has one sequence of Gate command data -- 32 gates worth of data
			//int nSeq;
			/************************************************************/

			i = pIData->wMsgID;
			int nByteCount = pRb->wByteCount;
			if (nByteCount >= 32)
				{
				memcpy((void *)&gLastRdBkPap, (void *)pRb, nByteCount);
				int nSeq = pRb->bSeqNumber;
				// switch statement if more read back cmds added
				switch (pRb->wReadBackID)
					{
				default:
					break;
				case GET_GATE_DATA_ID:
					// PubExt ST_GATE_READBACK_DATA gLastGateCmd;
					memcpy((void *)&gLastGateCmd.Seq[nSeq], (void *)pRb->ReadBackBlock, nByteCount);
					//gLastGateCmd.wSeq = pRb->bSeqNumber;
					SendIdataToPag((GenericPacketHeader *)pRb, 0);	// this path will eventually delete pIdataPacket
					SaveGateCmdReadBackSequence(&gLastGateCmd);
#if 0
					if (pRb)
						{
						delete pRb;
						pRb = NULL;
						}
#endif
					break;
				case GET_TCG_BEAM_GAIN_ID:
					nSeq = pRb->bSeqNumber;
					memcpy((void *)&gLastBeamGainReadBack.Seq[nSeq], (void *)pRb->wSpare, nByteCount);	  // was pRb->wspare
					gLastBeamGainReadBack.wSeq = nSeq;
					SendIdataToPag((GenericPacketHeader *)pRb, 0);	// this path will eventually delete pIdataPacket
					SaveBeamGainReadBackData();
					break;
				case GET_TCG_SEQ_GAIN_ID:
					memcpy((void *)&gLastSeqGainReadBack.Seq[0], (void *)pRb->ReadBackBlock, nByteCount);
					SendIdataToPag((GenericPacketHeader *)pRb, 0);	// this path will eventually delete pIdataPacket
					SaveTcgSeqGainReadBackData();
					break;
					}	// switch (pRb->wReadBackID)
				}	// if (nByteCount >= 32)
			else
				{
				s = _T("Readback data less than 32 bytes -- ERROR");
				SaveDebugLog(s);
				if (pRb)
					{
					delete pRb;
					pRb = NULL;
					}
#if 0
				delete m_pIdataPacket;
				m_pIdataPacket = NULL;
#endif

				break;
				}

			

	



			/**************************************************************/
			//memcpy((void *)&gLastGateCmd.Seq[nSeq], (void *)pRb->ReadBack, nByteCount);
			//gLastGateCmd.wSeq = pRb->bSeqNumber;


			break;
			}	// case eReadBackID

		default:
			s.Format(_T("ProcessInstrumentData unknown MsgId, %d\n"), pIData->wMsgID);
			//delete pIData;	// done in caller function
			}	// switch(pIData->wMsgID)
		}	// pIData->wByteCount >= 256

	else
		{
		// This is for data coming from Yiqing simulator... change once final design done.
		s.Format(_T("ProcessInstrumentData got a data packet of the wrong size, %d\n"),pIData->wByteCount);
		//delete pIData;	// deleted in caller function 
		}

	}	// CServerRcvListThread::ProcessInstrumentData(void *pData)

	// A read back sequence from the ADC board has come thru PAP
	// Convert the elements to ASCII and write to a log file
	// Only one sequence at a time. At present (Apr 2019) only 3 sequences of wall data
void CServerRcvListThread::SaveGateCmdReadBackSequence(ST_GATE_READBACK_DATA *pLastRdBkPap)
	{
	CString s, t;
	int ic, ig, iseq;
	if (pMainDlg->m_nReadBackExists == 0) return;
	ST_GATE_CH_PER_SEQ *pCh;
	ST_GATE_SETTINGS *pGate;
	iseq = pLastRdBkPap->wSeq;
	int val, addr;

	s.Format(_T("\nSequece = %d  All Gate Cmds  *******************************\n"), iseq );
	t = s;
	for (ic = 0; ic < 8; ic++)
		{
		s.Format(_T("\nCh=%d Dly  Addr   Rng  Addr   Blk  Addr   Thl  Addr   Trg  Addr   Pol  Addr   TOF  Addr\n"), ic);
		t += s;
		pCh = (ST_GATE_CH_PER_SEQ *) &pLastRdBkPap->Seq[iseq].Ch[ic];
		for (ig = 0; ig < 4; ig++)
			{
			pGate = (ST_GATE_SETTINGS *)&pLastRdBkPap->Seq[iseq].Ch[ic].Gate[ig];
			val = pGate->wDelay;
			addr = pGate->wDelayAddr;

			s.Format(_T("G%d   %04x %04x   %04x %04x   %04x %04x   %04x %04x   %04x %04x   %04x %04x   %04x %04x\n"),
				ig,
				pGate->wDelay, pGate->wDelayAddr,
				pGate->wRange, pGate->wRangeAddr,
				pGate->wBlank, pGate->wBlankAddr,
				pGate->bThold, pGate->wTholdAddr,
				pGate->bTrigger, pGate->wTriggerAddr,
				pGate->bPolarity, pGate->wPolarityAddr,
				pGate->bTOF, pGate->wTOFAddr );
			t += s;
				
			}
		}
	pMainDlg->SaveReadBackLog(t);
	}


void  CServerRcvListThread::SaveBeamGainReadBackData(void)
	{
	CString s, t;
	int ic, ir, is, id, ig, addr;
	if (pMainDlg->m_nReadBackExists == 0) return;

	is = gLastBeamGainReadBack.wSeq;
	s.Format(_T("\nTCG Beam Gains (Cmd 0x204) ********** Sequece = %d \n"), is);
	t = s;
	for (ic = 0; ic < 8; ic++)	// channel loop
		{
		addr = gLastBeamGainReadBack.Seq[is].Chnl[ic].wStartAddr;
		s.Format(_T("Channel %d Start Addr = %04x\n"), ic, addr);
		t += s;
		for (ir = 0; ir < 8; ir++)  // row loop
			{
			id = ir * 16;
			for (ig = 0; ig < 16; ig++)	// column loop
				{
				s.Format(_T("%04x "), gLastBeamGainReadBack.Seq[is].Chnl[ic].bGainPerCh[ig + id]);
				t += s;
				}
			t += _T("\n");
			pMainDlg->SaveReadBackLog(t);
			t = _T("");	// reset for next line
			}	// for (ir = 0; ir < 8; ir++)
		}	// chnl loop
	}

void  CServerRcvListThread::SaveTcgSeqGainReadBackData(void)
	{
	CString s, t;
	int ir, is, id, ig, addr;
	if (pMainDlg->m_nReadBackExists == 0) return;

	is = gLastBeamGainReadBack.wSeq;
	s = _T("\nTCG Seq Gains (Cmd 0x205) **********  \n");
	t = s;
	for (is = 0; is < 3; is++)	// seq loop
		{
		addr = gLastSeqGainReadBack.Seq[is].wStartAddr;
		s.Format(_T("Sequence %d Start Addr = 0x%04x\n"), is, addr);
		t += s;
		for (ir = 0; ir < 8; ir++)  // row loop
			{
			id = ir * 16;
			for (ig = 0; ig < 16; ig++)	// column loop
				{
				s.Format(_T("%04x "), gLastSeqGainReadBack.Seq[is].wGainPerSeq[id + ig]);
				t += s;
				}
			t += _T("\n");
			pMainDlg->SaveReadBackLog(t);
			t = _T("");	// reset for next line
			}	// for (ir 
		}	// seq loop
	}



	// Maybe not general purpose, but put pulser status into global data
	// Only one pulser for the system and data is slow moving  2018-10-19
void  CServerRcvListThread::ProcessPulserData(PULSER_DATA *pPulserData)
	{
	gwPulserCmds	= pPulserData->wPulserCmds;
	gwFPGA_VersionP	= pPulserData->wFPGA_Version;
	gwNIOS_VersionP	= pPulserData->wNIOS_Version;
	gwCPU_Temp		= pPulserData->wCPU_Temp;
	gbCmdQDepthP	= pPulserData->bPulserCmdQ;
	//CString s;
	//s.Format(_T(" Pulser Temp = %d\n"), gwCPU_Temp);
	//TRACE(s);
	}


#endif


// Take the code from TScanDlg which processes packets from PAM and insert here
#ifdef I_AM_PAG

// Really need to know which client this data is coming from
// For that matter, which inspection machine.
// For now (2019-12-02) just sort inputs by message ID type
void CServerRcvListThread::ProcessPAP_Data(void *pData)
	{
	CString s;
	int i, j;
	IDATA_PAP *pIdata = (IDATA_PAP *)pData;
	IDATA_FROM_HW* pAllWall = (IDATA_FROM_HW*)pData;
	int nByteCount;
	READBACK_DATA *pRb = (READBACK_DATA *)pIdata;
	CMD204H_READBACK *p204 = (CMD204H_READBACK*)pIdata;

	i = j = nByteCount = 0;	// kill warning
//	switch (m_nMyServer)
	switch(pIdata->wMsgID)
		{
	case NC_NX_IDATA_ID:
		// NcNx server


		if ((pIdata->wMsgSeqCnt & 0x7ff) == 0) 
			{
			s.Format(_T("wByteCount=%d, wMsgSeqCnt=%d, bPapNumber=%d, bBoardNumber=%d, bStartSeqNumber=%d\n"),
				pIdata->wByteCount, pIdata->wMsgSeqCnt, pIdata->bPapNumber, pIdata->bBoardNumber, pIdata->bStartSeqNumber);
			// use debugger to view
			TRACE(s);
			s.Format(_T("bSeqModulo=%d, bStartChannel=%d, bMaxVChnlsPerSequence=%d\n"),
				pIdata->bSeqModulo, pIdata->bStartChannel, pIdata->bMaxVChnlsPerSequence);
			TRACE(s);
			s.Format(_T("PeakChnl[0].Id2=%d, Od3=%d TOFmin=%d\n"),		//, TOFmax=%d\n"),
				pIdata->PeakChnl[0].bId2, pIdata->PeakChnl[0].bOd3, pIdata->PeakChnl[0].wTofMin); // , pIdata->PeakChnl[0].wTofMax );
			TRACE(s);
			SaveFakeData(s);
			// if using simulator, chnl 21 wall = 999*filter length, Od = 25
			s.Format(_T("PeakChnl[21].Id2=%d, Od3=%d TOFmin=%d\n"),		//, TOFmax=%d\n"),
				pIdata->PeakChnl[21].bId2, pIdata->PeakChnl[21].bOd3, pIdata->PeakChnl[21].wTofMin); // , pIdata->PeakChnl[0].wTofMax );
			SaveFakeData(s);
			}

		memcpy((void *)&gLastIdataPap, (void *)pIdata, pIdata->wByteCount);
		guIdataMsgCnt++;
		break;
			

	case ASCAN_DATA_ID:
		i = pIdata->wMsgID;
		memcpy((void *)&gLastAscanPap, (void *)pIdata, sizeof(ASCAN_DATA));
		guAscanMsgCnt++;
		break;
		

	case READBACK_DATA_ID:
			i = pIdata->wMsgID;
			nByteCount = pRb->wByteCount;
			if (nByteCount >= 32)
				{
				memcpy((void *)&gLastRdBkPap, (void *)pRb, nByteCount);
				int nSeq = pRb->bSeqNumber;
				s.Format(_T("Received Read Back data, wReadBackID = %d"), pRb->wReadBackID);
				switch (pRb->wReadBackID)
					{
				case GET_GATE_DATA_ID:
					// PubExt ST_GATE_READBACK_DATA gLastGateCmd;
					memcpy((void *)&gLastGateCmd.Seq[nSeq], (void *)pRb->ReadBackBlock, nByteCount);
					gLastGateCmd.wSeq = pRb->bSeqNumber;
					break;
				case GET_TCG_BEAM_GAIN_ID:
					memcpy((void *)&(gLastBeamGainReadBack.Seq[pRb->bSeqNumber]  ), (void *)&p204->Seq.Chnl[0] , nByteCount);
					gLastBeamGainReadBack.wSeq = pRb->bSeqNumber;
					break;
				default:
					s.Format(_T("Received Read Back Unknown, wReadBackID = %d"), pRb->wReadBackID);
					SaveDebugLog(s);
					}

					guRdBkMsgCnt++;
				//s.Format(_T("Received Read Back data, wReadBackID = %d"), pRb->wReadBackID);
				}	// if (nByteCount >= 32)
			else s = _T("Readback data less than 32 bytes -- ERROR");
			SaveDebugLog(s);
			// ReadBack
			break;
	case ADC_DATA_ID:
		// May not work if multiple ServerRcvListThreads. Each thread likely would need to 
		// open a unique file name that included the thread number in the name.
#ifdef I_AM_PAG
		if (gDlg.pNcNx)
			{
			memcpy((void*)&pAllWall->bNiosFeedback, "AllWall", 7);
			memcpy((void*)&gLastAllWall, (void*)pAllWall, pAllWall->wByteCount);
			if (gDlg.pNcNx->m_nRecordState > 0)
				{	// record the data in binary format
				gDlg.pNcNx->m_AllWallFile.Write(pAllWall, pAllWall->wByteCount);
				}
			}
#endif
		// show some piece of all wall data
		break;



	default:
			{
			// something else
			s.Format(_T("Process PAP Data message unknown ID = %d\n"), pIdata->wMsgID);
			SaveDebugLog(s);
			}
		break;


			// show some piece of all wall data 
#if 0
			if ((pAllWall->wMsgSeqCnt & 0xff) == 0)
				{
				i = pAllWall->wByteCount;
				j = pAllWall->bStartSeqNumber;
				s.Format(_T("Got all wall data Seq Cnt = %d, ByteCount = %d, StartSeq = %d\n"), 
					pAllWall->wMsgSeqCnt, pAllWall->wByteCount, pAllWall->bStartSeqNumber);
				//SaveDebugLog(s);
				//TRACE(s);
				}
			}	 // All wall server
		break;

	default:
		// Houston, we have problem -- Not NcNx processing and not all_wall processing
		s = _T("Unknown Server/Msg ID = %d running ProcessPAP_Data rountine",);
		SaveDebugLog(s);
		break;
#endif

		}	// switch(pIdata->wMsgID)
	delete pData;
	}	// ProcessPAP_Data(void *pData)

#endif

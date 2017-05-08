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
	srand( (unsigned)time( NULL ) );	// seed random number generator
#endif
	m_pElapseTimer = 0;
#if 0
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	// m_nMyServer, m_nClientIndex not available yet
	s.Format(_T("CServerRcvListThread[%d][%d] = 0x%08x, Id=0x%04x constructor\n"), m_nMyServer, m_nClientIndex, this, nId);
	TRACE(s);
#endif

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
	// Maybe use AfxEndThread (0) 

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
	m_pElapseTimer = new CHwTimer();
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
afx_msg void CServerRcvListThread::ProcessRcvList( WPARAM w, LPARAM lParam )
	{
	void *pV;
	int i;
	InputRawDataPacket *pIdata;

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
			pIdata = (InputRawDataPacket *)pV;
			ProcessInstrumentData(pIdata);	// local call to this class memeber
			m_pSCC->pSocket->LockRcvPktList();
			}
		m_pSCC->pSocket->UnLockRcvPktList();
			}
		}
	}
#else
// PAG

afx_msg void CServerRcvListThread::ProcessRcvList( WPARAM w, LPARAM lParam )
	{
	void *pV;
	int i;

	IDATA_PACKET *pIdata;	// output data from PAP/PAM-- debugging
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
				pIdata = (IDATA_PACKET *)pV;	// examine contents with debugger
				ProcessPAM_Data(pV);

				
				m_pSCC->pSocket->LockRcvPktList();
				}
			m_pSCC->pSocket->UnLockRcvPktList();
			}
		}
	}

#endif


#if 0
void CServerRcvListThread::MakeFakeDataHead(InputRawDataPacket *pData)
//void CServerRcvListThread::MakeFakeDataHead(SRawDataPacket *pData)
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
	// we could have fewer than 32 channels, ie sequence number could be less than 32
	// Consider the number of ascans needed to fire all multiplexed channels (say 8)
	// Give this number the name "frame". A frame is the number of ascans to fire all the channels
	// Then 8 ascans would be a frame  - and 128/8 = 16 frames in the packet.
	// Every 16 frames, generate the max and min wall reading and the max Nc qualified flaw reading for
	// every channel. 

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
#ifdef CLIVE_RAW_DATA
				k = pData->stSeqPkt[iSeqPkt].RawData[i].bAmp1 = 1 + (GetRand() / 2);	// 1-51 amplitude
				t.Format(_T("%3d  "), (k)); s += t;
#endif
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
#ifdef CLIVE_RAW_DATA
				pData->stSeqPkt[iSeqPkt].RawData[i].bAmp1 = 1 + (GetRand() / 2);
#endif
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
	pMainDlg->SaveFakeData(s);
	}

#if 0
// Not enought info in data structures I am seeing on 6/7/16. Assume 32 chnls 
int CServerRcvListThread::GetSequenceModulo(SRawDataPacketOld *pData)
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
#endif


// Build Output packet as individual channel peak held data becomes available
// could return the index in Idata
// If SendFlag is non zero, send the packet as is

void CServerRcvListThread:: AddToIdataPacket(CvChannel *pChannel, int nCh, int nSeq, int nSendFlag)
	{
	if (m_pIdataPacket == NULL)
		{
		m_pIdataPacket = new (IDATA_PACKET);	// sizeof = 1460 179 RESULTS
		memset((void *) m_pIdataPacket,0, sizeof(IDATA_PACKET));
		m_pIdataPacket->bPAPNumber	= (BYTE) pMainDlg->GetMy_PAM_Number();
		m_pIdataPacket->bBoardNumber	= m_pSCC->m_nClientIndex;

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
	}	// AddToIdataPacket


int CServerRcvListThread::GetIdataPacketIndex(void)
	{
	if (m_pIdataPacket == NULL)	return -1;
	return m_IdataInPt;	// how may Result buffers are full. Limit is 179
	}

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
	if (m_Seq >= gMaxSeqCount)
		{
		m_Seq = 0;
		}
	}

// The only way to get Idata from the PAP is to use the ClientConnectionManangement TCPIP
// link. This routine puts the Idata packet ptr into a linked list
// and then commands the CCM_PAG component to empty the linked list thru
// the client socket to the PHASED ARRAY GUI - PAG
void CServerRcvListThread::SendIdataToPag(GenericPacketHeader *pIdata)
	{
#ifdef I_AM_PAP
	if (pCCM_PAG == NULL)
		{
		ASSERT(0);
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
		ASSERT(0);
		delete pIdata;
		return;
		}
	if ( pCCM_PAG->m_pstCCM->pSendThread == NULL)
		{
		ASSERT(0);
		delete pIdata;
		return;
		}
	int i;
	CClientCommunicationThread *pThread = pCCM_PAG->m_pstCCM->pSendThread;
	pCCM_PAG->LockSendPktList();
	pCCM_PAG->AddTailSendPkt((void*)pIdata);
	i = pCCM_PAG->m_pstCCM->pSendPktList->GetCount();
	pCCM_PAG->UnLockSendPktList();
	//ON_THREAD_MESSAGE(WM_USER_SEND_TCPIP_PACKET, CClientCommunicationThread::TransmitPackets)
	PostThreadMessage(WM_USER_SEND_TCPIP_PACKET,(WORD) 3,(LPARAM) i);
#endif
	}	//SendIdataToPag

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
	if (pIData->wByteCount >= INSTRUMENT_PACKET_SIZE -132)	//sizeof(SRawDataPacketOld))		// legacy 1040, future is ???
		{
		/******************************************************************/
#if 0
		//  2016-10-20 start to migrate to new input data structures
		// throw away simulator data or current Sam data and make a new InputRawDataPacket
		//delete pIData;

		if (m_pOutputRawDataPacket == NULL)
			{
			m_pOutputRawDataPacket = new InputRawDataPacket;
			memset(m_pOutputRawDataPacket, 0, sizeof (InputRawDataPacket));
			}
#endif


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
#ifdef CLIVE_RAW_DATA
				bGateTmp = pChannel->InputFifo(eIf, pIData->stSeqPkt[iSeqPkt].RawData[j].bAmp1);
#endif
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
						// create an Idata packet with New. copy m_pIdataPacket to IdataPacketOut linked list
						// send a thread message to theApp or a new sender thread to empty the linked list.
						// or better yet signal the ccm_pag thread to empty alist  directed to PAG
						m_pIdataPacket->wByteCount = pIData->wByteCount;
						//theApp.PamSendToPag(m_pIdataPacket, pIData->wByteCount); // get len from data packet
						//delete m_pIdataPacket;
						//m_pIdataPacket = NULL;
						// SendIdataToPag crashes when attempting to delete the input packet. Make a copy here
						// that is not a class member
						IDATA_PACKET *pIdataPacket = new (IDATA_PACKET);
						memcpy((void*)pIdataPacket, (void *)m_pIdataPacket, sizeof(IDATA_PACKET));
						//
						//pIdataPacket->wMsgID = NC_NX_IDATA_ID; //already 1 before break
						SendIdataToPag( (GenericPacketHeader *) pIdataPacket);
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
		pMainDlg->SaveDebugLog(s);
#endif
		delete pIData;	// info in pOutput was put into a new structure and sent to the PAG
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
#ifdef I_AM_PAG


void CServerRcvListThread::ProcessPAM_Data(void *pData)
	{
	CString s;
	int i;
	IDATA_PACKET *pIdata = (IDATA_PACKET *)pData;

	if (pIdata->wMsgID == NC_NX_IDATA_ID)
		{
		s.Format(_T("wByteCount=%d, wMsgSeqCnt=%d, bPAPNumber=%d, bBoardNumber=%d, bStartSeqNumber=%d\n"),
			pIdata->wByteCount, pIdata->wMsgSeqCnt, pIdata->bPAPNumber, pIdata->bBoardNumber, pIdata->bStartSeqNumber);
		// use debugger to view
		TRACE(s);
		s.Format(_T("bSequenceLength=%d, bStartChannel=%d, bMaxVChnlsPerSequence=%d\n"),
			pIdata->bSequenceLength, pIdata->bStartChannel, pIdata->bMaxVChnlsPerSequence);
		TRACE(s);
		s.Format(_T("Results[0].Id2=%d, Od3=%d TOFmin=%d, TOFmax=%d\n"),
			pIdata->Results[0].bId2, pIdata->Results[0].bOd3, pIdata->Results[0].wTofMin, pIdata->Results[0].wTofMax );
		TRACE(s);
		}
	else
		{
		i = pIdata->wMsgID;
		}
	delete pData;
	}	// ProcessPAM_Data(void *pData)

#endif

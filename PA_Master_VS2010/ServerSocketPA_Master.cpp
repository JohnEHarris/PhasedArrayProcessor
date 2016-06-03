// ServerSocketPA_Master.cpp : implementation file
//
/*
Author:		jeh
Date:		29-Aug-12
Purpose:	Customize the CServerSocket class specifically for the Phased Array Master support
			Specifically, the OnReceive() function is modified for Phased Array Master Instrument input data

*/

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"
#include "AfxSock.h"
#include "Extern.h"

extern CCriticalSection g_CriticalSection;
extern CCriticalSection g_CriticalSection2;
extern CCriticalSection g_CriticalSectionAscan;
extern CPtrList g_pTcpListUtData;
extern CPtrList g_pTcpListUtData2;
extern CPtrList g_pTcpListAscan;
extern ASCAN_HEAD_NIOS g_AscanHead;	


// CServerSocketPA_Master

CServerSocketPA_Master::CServerSocketPA_Master()
	{
	m_nOwningThreadType = -1;
	m_BufOffset = 0;
	memset((void *) m_RcvBuf,0,sizeof(m_RcvBuf));
	// even though in base class, must also init here.
	m_pElapseTimer = new CHwTimer();
	}

CServerSocketPA_Master::CServerSocketPA_Master(CServerConnectionManagement *pSCM)
	{
	m_pSCM = pSCM;
	m_pElapseTimer = new CHwTimer();
	}

CServerSocketPA_Master::~CServerSocketPA_Master()
	{
	if (m_pElapseTimer)	delete m_pElapseTimer;
	}


// CServerSocketPA_Master member functions

// Packets received are "repackaged" to include the length of the packet as the first item in the new packet
//
void CServerSocketPA_Master::OnReceive(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	BYTE *pB;	// debug
	void *pPacket = 0;
	int nPacketSize = sizeof(I_MSG_RUN);
	int n, m;
	CString s;
	CTcpThreadRxList *pTcpThreadRxList;

	m = n = Receive( (void *) &m_RcvBuf[m_BufOffset], (sizeof(m_RcvBuf) - m_BufOffset), 0 );	
	I_MSG_RUN *pMR; 

	// If shutting down and stop send/receive set, throw away the data
	if (m_pSCM->m_pstSCM == NULL)				return;	
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag)	return;
	if (m_pSCC == NULL)							return;

	if ( ((n == sizeof(I_MSG_RUN)) && ((m_pSCC->uPacketsReceived & 0xff) == 0)) || 
			(m_pSCC->uPacketsReceived < 20) || (n != sizeof(I_MSG_RUN)) )
		{
		s.Format(_T("[%5d]CServerSocketPA_Master::OnReceive got %d bytes\n"), m_pSCC->uPacketsReceived, n);
		TRACE(s);
		}

	if (m_pSCC)
		{
		if (m_pSCC->bStopSendRcv)
			{
			n = -1;	// forced exit
			m_pSCC->bConnected = (BYTE) eNotConnected;
			}
		}
	else	return;


	if ( n > 0)
		{
		// put it in the linked list and let someone else decipher it
		while ( pPacket = GetWholePacket(nPacketSize, &n))	// returns a ptr to void with length nPacketSize, n set to 0
			{	// get packets
			pMR = new I_MSG_RUN();
			memcpy( (void *) pMR, pPacket, nPacketSize);	// move all data to the new buffer
			pTcpThreadRxList = pCTscanDlg->GetTcpThreadList();
			if (pTcpThreadRxList)
				DistributeMessage(pMR, pTcpThreadRxList);

			m_pSCC->uBytesReceived += nPacketSize;
			} 	// get packets
		if (m_pSCC->bConnected == (BYTE) eNotConnected)
			m_pSCC->bConnected = (BYTE) eNotConfigured;
		m_pSCC->uPacketsReceived++;
		if ((m_pSCC->uPacketsReceived & 0xff) == 0)	m_pElapseTimer->Start();
		if ((m_pSCC->uPacketsReceived & 0xff) == 0xff)
			{
			m_nElapseTime = m_pElapseTimer->Stop(); // elapse time in uSec for 256 packets
			float fPksPerSec = 256000000.0f/( (float) m_nElapseTime);
			s.Format(_T("[%5d]CServerSocketPA_Master::Packets/sec = %6.1f\n"), m_pSCC->uPacketsReceived, fPksPerSec);
			TRACE(s);
			}
		if (m_pSCC->uMaxPacketReceived < (unsigned) m)	m_pSCC->uMaxPacketReceived = m;
		}	// if ( n > 0)

	CAsyncSocket::OnReceive(nErrorCode);
	}


// Steal the DistributeMessage routine from CTcpThreadRxList
// Could/should .. did .. make child class of ServerSocket just for PA Master data

/************************************************************
	Distribute messages to proper destination
	Put pReadBuf packet into a linked list if appropriate. If a bad packet
	delete pReadBuf packet
*************************************************************/


void CServerSocketPA_Master::DistributeMessage(I_MSG_RUN *pReadBuf, CTcpThreadRxList *pTcpThreadRxList)
	{
	WORD nMotionBus;
	DWORD nNextJointNum;
	BOOL bAscanMode = TRUE;
	I_MSG_NET *pNetBuf;
	I_MSG_CAL *pCalBuf;
	PEAK_DATA *pPeakData;
	CString t,u;
	//BYTE Flush[0x4000];
	static int nFlushCount = 0;
	static int nPrintCount = 0;
	char s[128];
	//int i;
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

	CTime tNow = CTime::GetCurrentTime();

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
			// TESTING lights on PAG screen
			if (pReadBuf->MstrHdr.MsgId == CAL_MODE)
				{
				if ((m_nMyThreadIndex < MAX_CLIENTS_PER_SERVER) && (m_nMyThreadIndex > -1))
					{
					pCalBuf = (I_MSG_CAL *) pReadBuf;
					pPeakData = (PEAK_DATA *) &pCalBuf->Shoe[m_nMyThreadIndex];
					SetConnectionStatus(pPeakData);
					}
				}
			g_pTcpListUtData.AddTail((void *) pReadBuf);
			g_CriticalSection.Unlock();
			}
		else
			{
			g_CriticalSection2.Lock();
			g_pTcpListUtData2.AddTail((void *) pReadBuf);
			g_CriticalSection2.Unlock();
			}
#ifdef _DEBUG
		if ((nPrintCount & 0xff) == 0)
			{
			sprintf(s,"[%5d][Srv%d][MI%d]UtData Msg Id 0x%04x\n", nPrintCount++, 
					m_nMyServer, m_nMyThreadIndex, pReadBuf->MstrHdr.MsgId);
			TRACE(s);
			}
#endif
		break;
	
	case RAW_FLAW_MODE:
	case ASCAN_MODE:
		// Put pReadBuf into Ascan list
		g_CriticalSectionAscan.Lock();
		g_pTcpListAscan.AddTail((void *) pReadBuf);
		if ( pReadBuf->MstrHdr.MsgId == ASCAN_MODE )
			memcpy ( (void *) &g_AscanHead, (void *) &(pReadBuf->MstrHdr.MmiStat), sizeof(ASCAN_HEAD_NIOS) );
		g_CriticalSectionAscan.Unlock();
#ifdef _DEBUG
		if ((nPrintCount & 0xff) == 0)
			{
			sprintf(s,"[%5d][Srv%d][MI%d]Raw or AScan Msg Id 0x%04x\n", nPrintCount++, 
					m_nMyServer, m_nMyThreadIndex, pReadBuf->MstrHdr.MsgId);
			TRACE(s);
			}
#endif
		break;

	case NET_MODE:
#ifdef _DEBUG
		t = tNow.Format(_T(" %H:%M:%S\n"));
		u.Format(_T("[%5d][Srv%d][MI%d]NET_MODE Msg Id 0x%04x"), nPrintCount++,
				m_nMyServer, m_nMyThreadIndex, pReadBuf->MstrHdr.MsgId);
		//sprintf(s,"[%5d]NET_MODE Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
		u += t;
		TRACE(u);
#endif
		pNetBuf = (I_MSG_NET *) pReadBuf;
		// use pReadBuf locally and delete
		//pTcpThreadRxList->SetBoardRevision(pNetBuf); // <- what does this do??? jeh
		//for (i=0; i<MAX_SHOES+1; i++)
		//	{		g_bConnected[i] = pNetBuf->bConnected[i];		}
		delete pReadBuf;
		break;

	default:  //not a truscan defined msg
		//AfxMessageBox("Unknown msg ID!");
#ifdef _DEBUG
		sprintf(s,"[%5d][Srv%d][MI%d]Unknown Msg Id 0x%04x\n", nPrintCount++, 
				m_nMyServer, m_nMyThreadIndex, pReadBuf->MstrHdr.MsgId);
		TRACE(s);
#endif
		delete pReadBuf;
		pReadBuf = NULL;
		bAscanMode = TRUE;
		break;
	}


	if (!bAscanMode)
		{
		pTcpThreadRxList->SetMotionBus(nMotionBus);
		pTcpThreadRxList->SetNextJointNumber(nNextJointNum);
		}
	}	// DistributeMessage()


// NORMALLY only called when PEAK_DATA is available. Instrument status values are
/* 
3: no hardware found (once in this state, can't be changed to other states);   .. eInstrumentNotPresent
2: hardware found but no config file downloaded;  .. eInstrumentConnected
1: normal .. eInstrumentConfigured
*/
void CServerSocketPA_Master:: SetConnectionStatus(PEAK_DATA * pPeakData)
	{
	if (m_pSCC == NULL)
		{
		TRACE(_T("CServerSocketPA_Master:: SetConnectionStatus, m_pSCC should not be NULL\n"));
		return;
		}
	BYTE bStat = pPeakData->RdpStatus;
	switch (bStat)
		{
	default:
	case 3:
		m_pSCC->bConnected = (BYTE) eInstrumentNotPresent;		// not actually possible to be here receiving packets w/o connected instrument
		break;
	case 2:
		m_pSCC->bConnected = (BYTE) eInstrumentConnected;
		break;
	case 1:
		m_pSCC->bConnected = (BYTE) eInstrumentConfigured; 
		break;
	case 0:		// not actually listed, but if we have a packet we must be connected
		m_pSCC->bConnected = (BYTE) eInstrumentConnected;
		break;
		}

	}

// Can be call under any circumstances
BYTE CServerSocketPA_Master::GetConnectionStatus(void)
	{
	if (m_pSCC == NULL)
		{
		TRACE(_T("CServerSocketPA_Master:: GetConnectionStatus, m_pSCC should not be NULL\n"));
		return (BYTE) eInstrumentNotConnected;
		}
	return (BYTE) m_pSCC->bConnected;
	}

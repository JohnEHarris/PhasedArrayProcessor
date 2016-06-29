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
}
CServerSocketPA_Master::CServerSocketPA_Master(CServerConnectionManagement *pSCM)
	{
	m_pSCM = pSCM;
	}
CServerSocketPA_Master::~CServerSocketPA_Master()
{
}


// CServerSocketPA_Master member functions

// Packets received are "repackaged" to include the length of the packet as the first item in the new packet
//
void CServerSocketPA_Master::OnReceive(int nErrorCode)
	{
	// TODO: Add your specialized code here and/or call the base class
	BYTE Buf[MAX_PAM_BYTES+8];			// put it on the stack instead of the heap. Probably quicker
	BYTE *pB;	// debug
	I_MSG_RUN *pMR; 
	CTcpThreadRxList *pTcpThreadRxList;
	//TCPDUMMY * Data = new TCPDUMMY;
	int n;
	CString s;
	n = Receive( (void *) Buf, MAX_PAM_BYTES, 0 );	// read all data available into Buf
	s.Format(_T("CServerSocketPA_Master::OnReceive got %d bytes\n"), n);
	TRACE(s);

	// If shutting down and stop send/receive set, throw away the data
	if (m_pSCM->m_pstSCM == NULL)	return;	
	if (m_pSCM->m_pstSCM->nSeverShutDownFlag)	return;
#if 0
	if (m_pSCC)
		{
		if (m_pSCC->bStopSendRcv)
			{
			n = -1;	// forced exit
			m_pSCC->bConnected = 0;
			}
		}
#endif

	if ( n > 0)
		{
		if ( sizeof(I_MSG_RUN) == n)
			{
			pMR = new I_MSG_RUN();
			memcpy( (void *) pMR, (void *) Buf, n);	// move all data to the new buffer
			pTcpThreadRxList = pCTscanDlg->GetTcpThreadList();
			if (pTcpThreadRxList)
				DistributeMessage(pMR, pTcpThreadRxList);
			}
		else
			{
			// put it in the linked list and let someone else decipher it
			stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[n+sizeof(int)];	// resize the buffer that will actually be used
			memcpy( (void *) &pBuf->Msg, (void *) Buf, n);	// move all data to the new buffer
			pBuf->nLength = n;
			pB = (BYTE *) pBuf;	// debug helper			
			LockRcvPktList();
			AddTailRcvPkt(pBuf);	// put the buffer into the recd data linked list
			UnLockRcvPktList();
			// Post a message to someone who cares and let that routine/class/function deal with the packet
			}

		if (m_pSCC)
			{
			m_pSCC->bConnected = 1;
			m_pSCC->uBytesReceived += n;
			m_pSCC->uPacketsReceived++;
			if (m_pSCC->uMaxPacketReceived < (unsigned) n)	m_pSCC->uMaxPacketReceived = n;
			}
		}	// if ( n > 0)

	CAsyncSocket::OnReceive(nErrorCode);
	}


// Steal the DistributeMessage routine from CTcpThreadRxList
// Could/should make child class of ServerSocket just for PA Master data

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
		sprintf(s,"[%5d]UtData Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
		TRACE(s);
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
		sprintf(s,"[%5d]Raw or AScan Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
		TRACE(s);
#endif
		break;

	case NET_MODE:
#ifdef _DEBUG
		sprintf(s,"[%5d]NET_MODE Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
		TRACE(s);
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
		sprintf(s,"[%5d]Unknown Msg Id 0x%04x\n", nPrintCount++, pReadBuf->MstrHdr.MsgId);
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

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
#include "RunningAverage.h"
#include "../include/cfg100.h"

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
		InitRunningAverage(0,0);
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
	int i;
#ifdef THIS_IS_SERVICE_APP
	for ( i = 0; i < MAX_WALL_CHANNELS; i++)
		{
		if (m_pRunAvg[i])	delete m_pRunAvg[i];
		m_pRunAvg[i] = NULL;
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
	ON_THREAD_MESSAGE(WM_USER_INIT_RUNNING_AVERAGE, InitRunningAverage)	// manually added by jeh 11-09-12
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

// Instantiate and initialize the Running average class instances
// Before processing any instrument data.
//
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
	}
#endif

// Take the code from ServiceApp, tInstMsgProcess() and incorporate into this class
// Note that original code read input data from a socket and blocked until data was available.
// The class containing this procedure works by emptying input data which has been put into a linked list
// by the socket's OnReceive() function. It only runs when a message is posted for the thread to check
// the linked list.
// Packets stored in pData have the length of the packet prepended.
// Remove the integer length before processing
#ifdef THIS_IS_SERVICE_APP

void CServerRcvListThread::ProcessInstrumentData(void *pData)
	{
	// see ServicApp.cpp the procedure tInstMsgProcess() for legacy operation
	//
	stSEND_PACKET *pBuf = (stSEND_PACKET *) pData;
	int nPkLength = pBuf->nLength;

	pData = (void *) &pBuf->Msg;	// pData recast to remove packet length

	BYTE *pB = (BYTE *) pData;		// byte ptr for byte processing
	I_MSG_RUN *pSendBuf;			// static (on stack) memory for constructing messages
	PEAK_DATA  ReadBuf;
	//BYTE recvBuf[RAW_DATA_PACKET_LENGTH]; pData is the raw data
	UDP_SLAVE_DATA SlvData;
	int i, ic;
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
#if 1
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


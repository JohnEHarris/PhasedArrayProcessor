
/*******************************************************
Author: jeh
Date:	10-Jul-2012
Purpose:	Derive a specific class from the general base class ClientConnectionManagement
			Since all mill console programs have to talk to the SysCp, this is a good place
			to begin with children of the base CCM class. This class handles tcp/ip messages.
			General naming scheme of these CCM_XXX classes is CCM_<server we want to connect to>
Revised:	20-Jan-13
			Rename to CCM_PAG to handle client connection to Phased Array Gui. Now part of 
			Phased Array Master (PAM) code

********************************************************/

			
#include "stdafx.h"

#include "ServiceApp.h"
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"
#include "ClientCommunicationThread.h"
//#include "TCPCommunicationDlg.h"
#include "time.h"

//extern THE_APP_CLASS theApp;

//extern 	C_MSG_ALL_THOLD  g_AllTholds;
//extern 	C_MSG_NC_NX g_NcNx;
//extern I_MSG_RUN SendBuf;
//extern DWORD  g_nStation2JointNum;// = 0;
//extern I_MSG_RUN SendCalBuf;





#define I_AM_CCM_PAG
//#include "CCM_PAG.h"
CCCM_PAG *pCCM_PAG;
// nMyConnection selects which one of MAX_CLIENTS connections we are managing with this instance
// This class is a child of the base class CClientConnectionManagement. It was crafted specifically
// for handling communications with the PAG

CCCM_PAG::CCCM_PAG(int nMyConnection) : CClientConnectionManagement(nMyConnection, 0)
	{
	// call the base constructor explicitly above
	TRACE(_T("CCM_PAG Constructor called\n"));
	CString stmp;
	m_pstCCM->pCCM = this;
	m_nMsgQty = 0;
	m_nWhichInstrument = 0;	// Best guess before anything else is known about Instruments.
	}

CCCM_PAG::~CCCM_PAG(void)
	{
	m_pstCCM->pCCM = 0;
	TRACE("CCM_PAG Destructor called\n");
	// base destructor called implicity after this destructor runs
	}

// This code is specific to communications with the PAG

// We finally get around to processing the message.
// Liberally stolen from Randy's example


// For this class , the received message comes from the PAG
// All commands from PAG are message type MMI_CMD
// Over rides base class ProcessReceivedMessage()
// PAG messages are variable length - must handle each as it arrives and assume
// the whole message arrives in one packet - assumption is that it works like UDP
// Function is primarily to send to correct instrument.
// Called from CCmdProcessThread::ProcessReceivedMessage which was invoked by
// CClientConnectionManagement::OnReceive() - m_pstCCM->pCmdProcessThread->PostThreadMessageA(WM_USER_CLIENT_PKT_RECEIVED, 0,0L);
//
void CCCM_PAG::ProcessReceivedMessage(void)
	{
	USES_CONVERSION;
	CString s;
//	BYTE *pB;

/****
typedef struct
	{
	WORD wMsgID;		// 1
	WORD wMsgSeqCnt;
	BYTE bPapNumber;	// Which PAM
	BYTE bInstNumber;	// Which Instrument connected to the above APAM
	BYTE bChnlTypes;	// how many different chnl types for each instrument
	BYTE bChnlRepeats;	// how many times a given chnl type repeats in each instrument
	BYTE bMaxVChnlPerInst;	// We assume 32 total vChnl per instrument max
	BYTE bSpare[3];
	BYTE bMsg[1024];	// Max unique sets of Nc Nx data per instrument. Size = 16*32 =512
	} PAM_GENERIC_MSG; // SIZEOF() = 1036
	****/

			
	// 2016-06-27 ditch legacy command structure and use PA2 structure

	//MMI_CMD *pMmiCmd;
	PAM_GENERIC_MSG *pMmiCmd;
	PAP_INST_CHNL_NCNX *pPamChnlInfo;
	WORD MsgId;

	if (m_pstCCM->pRcvPktPacketList == NULL) return;

	while (m_pstCCM->pRcvPktPacketList->GetCount())
		{
		LockRcvPktList();
		pMmiCmd = (PAM_GENERIC_MSG *) m_pstCCM->pRcvPktPacketList->RemoveHead();
		UnLockRcvPktList();

		// use pMmiCmd->Slot to identify PAM's pClientConnection to know which instrument to send to
		// pMmiCmd->ChnlNum will specify the channel in that instrument
		// 1st, unpack from linked list and examine each message. Some messages configure the PAM itself
		// while othere may be routed directly to the instruments.

			
		MsgId = pMmiCmd->wMsgID;

		// big case statement adapted from ServiceApp.cpp 'c' routine ProcessMmiMsg()
		switch(MsgId)
			{

		case NC_NX_CMD_ID:
			// The only message as of 2016-06-27
			// Does not get sent to instruments, sets Nc and Nx parameters for the PAM to use

			pPamChnlInfo = (PAP_INST_CHNL_NCNX *)pMmiCmd;
			TRACE(_T("Received NC_NX_CMD_ID for Instrument %d from Phased Array GUI - now deleting\n"),pMmiCmd->bInstNumber);
			SetChannelInfo(pPamChnlInfo);
			delete pMmiCmd;
			return;

			break;

		default:
			TRACE(_T("No command recognized\nDeleting command from Phased Array GUI\n"));
			delete pMmiCmd;
			return;
			}	// end switch(MsgId)


		m_nMsgQty++;
#if 0
		s.Format(_T("[%03d]CCCM_PAG::ProcessReceivedMessage for Inst[%d], Chnl[%d]- now delete\n"),
			m_nMsgQty, pMmiCmd->Slot, pCmd->ChnlNum);
		TRACE(s);


		// Instead of deleting the packet, we will add it to stSCM[0] server (server for all attached Instruments)
		// in stSCM[0].pClientConnection[pCmd->Slot] connection structure
		if (stSCM[0].pClientConnection[pCmd->Slot])
			{
			CServerSocket *pSocket				= stSCM[0].pClientConnection[pCmd->Slot]->pSocket;
			CServerSocketOwnerThread *pThread	= stSCM[0].pClientConnection[pCmd->Slot]->pServerSocketOwnerThread;
			if ( pSocket && pThread)
				{
				// create a new buffer with length + data
				stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[sizeof(MMI_CMD)+sizeof(int)];	// resize the buffer that will actually be used
				memcpy( (void *) &pBuf->Msg, pCmd, sizeof(MMI_CMD));	// move all data to the new buffer
				pBuf->nLength = sizeof(MMI_CMD);
				pB = (BYTE *) pBuf;	// debug helper			
				// add to Send list
				// Send thread msg TransmitPacket to socket owner
				// delete pCmd
				pSocket->LockSendPktList();
				pSocket->AddTailSendPkt(pBuf);
				pSocket->UnLockSendPktList();
				// Thread msg causes CServerSocketOwnerThread::TransmitPackets() to execute
				pThread->PostThreadMessageA(WM_USER_SERVER_SEND_PACKET,0,0L);
				}
			}
#endif

		}	// while (m_pstCCM->pRcvPktPacketList->GetCount())

	}	// ProcessReceivedMessage(void)


	// not part of the base class... debug
void CCCM_PAG::UniqueProc(void)
	{
	TRACE("Unique to the child class\n");
	}



// These functions copied from ServiceApp.cpp
// does Yanming number slaves 1..32. My scheme is 0..31
int CCCM_PAG::FindWhichSlave(int nChannel)
	{
	// UNTIL WE HAVE A GOOD FIX, RETURN 0
	return 0;
	int sum=0, nSlave=0, i;

	for (i=0; i<10; i++)
		{

		if ( (nChannel / sum) == 0)
			{
			nSlave = i;
			break;
			}
		}

	return nSlave;
	}

int CCCM_PAG::FindSlaveChannel(int nChannel)
{

	return 0;
}

int CCCM_PAG::FindDisplayChannel(int nArray, int nArrayCh)
	{

	return 0;
	}
//
// These function copied from InspMsgProcess class

// usually called when an MMI command is received to set channel config info
// Then SetChannelInfo will set the channel types for this instrument  -- not for PA2 in 2016
// Only sets Nc Nx parameters. Knows not what type the channel is
//void CInstMsgProcess::SetChannelInfo(void)

void CCCM_PAG::SetChannelInfo(PAP_INST_CHNL_NCNX *pPamInstChnlInfo)
	{
	int nPam, nInst, nChnlTypes, nRepeat;
	int i,j,k;
	CString s;
	CvChannel *pChannel;

	ST_SERVER_CONNECTION_MANAGEMENT *pPAM_SCM = GetPAM_SCM();

	nPam		= pPamInstChnlInfo->bPapNumber;
	nInst		= pPamInstChnlInfo->bInstNumber;
//	nChnlTypes	= pPamInstChnlInfo->bChnlTypes;
	nChnlTypes = 1;  // fix this soon
	nRepeat		= pPamInstChnlInfo->bSeqNumber;
	ST_NC_NX *pNcNx = pPamInstChnlInfo->stNcNx;
	// PAP is always the [0] server to the instruments. A second PAM will be in another computer
	// but will in the software structure still be server[0] in the second computer.
	// If for some reason PAP needs to be a server to another type client, it will have to be
	// something other than [0]
	//
	ST_SERVERS_CLIENT_CONNECTION *pSCC = stSCM[0].pClientConnection[nInst];
	if (pSCC == NULL)
		{
		s.Format(_T("pSCC = stSCM[0].pClientConnection[%d] is null\n"), nInst);
		TRACE(s);
		return;
		}

	// outter loop is repeat count, inner loop is unique channel type
	k = 0;
	for ( j = 0; j < nRepeat; j++)
		{
		for ( i = 0; i < nChnlTypes; i++)
			{
			// [seq][chnl]
			pChannel = pSCC->pvChannel[0][k]; // one of up to 32 channels typically
			pNcNx = &pPamInstChnlInfo->stNcNx[i]; // one of up to nChnlTypes
			pChannel->FifoInit(0, pNcNx->bNcID,pNcNx->bTholdID, pNcNx->bModID); 
			pChannel->FifoInit(1, pNcNx->bNcOD,pNcNx->bTholdOD, pNcNx->bModOD); 
			//FifoInit(BYTE bIdOd, BYTE bNc, BYTE bThld, BYTE bMod)
			pChannel->WFifoInit((BYTE)pNcNx->wNx, pNcNx->wWallMax, pNcNx->wWallMin, pNcNx->wDropOut);
			//WFifoInit(BYTE bNx, WORD wMax, WORD wMin, WORD wDropOut)
			k++;
			}
		}



	}

// Send the same message to all Instruments
BOOL CCCM_PAG::SendSlaveMsgToAll(MMI_CMD *pCmd)
	{
	int i;
	MMI_CMD  *ptempCmd;
	m_nOldInstrument = m_nWhichInstrument;

	// Since SendSlaveMsg() deletes the input msg ptr, make a copy before each call
	for (i = 0; i < MAX_CLIENTS_PER_SERVER; i++)
		{
		ptempCmd = new MMI_CMD;
		memcpy(ptempCmd,pCmd, sizeof(MMI_CMD));
		ptempCmd->Inst_Number_In_PAM = m_nWhichInstrument = i;
		SendSlaveMsg (i, ptempCmd);
		}
	m_nWhichInstrument = m_nOldInstrument;
	delete pCmd;
	return TRUE;
	}

// These functions copied from ServiceApp.cpp -- originally  'c' function
// A command received from PAG AND FORWARDED to an Instrument
BOOL CCCM_PAG::SendSlaveMsg(int nWhichInstrument, MMI_CMD *pCmd)
	{
	BYTE *pB;
	CString s;
	if (pCmd->Inst_Number_In_PAM !=nWhichInstrument)
		{
		ASSERT(0);
		s.Format(_T("pCmd->Inst_Number_In_PAM=%d, m_nWhichInstrument=%d\n"), pCmd->Inst_Number_In_PAM, nWhichInstrument);
		TRACE(s);
		delete pCmd;
		return FALSE;
		}
	if (pCmd->Inst_Number_In_PAM >= MAX_CLIENTS_PER_SERVER)
		{
		ASSERT(0);
		s.Format(_T("pCmd->Inst_Number_In_PAM=%d, MAX_CLIENTS_PER_SERVER=%d\n"), pCmd->Inst_Number_In_PAM, MAX_CLIENTS_PER_SERVER);
		TRACE(s);
		delete pCmd;
		return FALSE;
		}

	// add pCmd to stSCM[0] server (server for all attached Instruments) send packet linked list
	// in stSCM[0].pClientConnection[pCmd->Slot] connection structure
	if (stSCM[0].pClientConnection[pCmd->Inst_Number_In_PAM])
		{
		CServerSocket *pSocket				= stSCM[0].pClientConnection[pCmd->Inst_Number_In_PAM]->pSocket;
		CServerSocketOwnerThread *pThread	= stSCM[0].pClientConnection[pCmd->Inst_Number_In_PAM]->pServerSocketOwnerThread;
		if ( pSocket && pThread)
			{
			// create a new buffer with length + data
			stSEND_PACKET *pBuf = (stSEND_PACKET *) new BYTE[sizeof(MMI_CMD)+sizeof(int)];	// resize the buffer that will actually be used
			memcpy( (void *) &pBuf->Msg, pCmd, sizeof(MMI_CMD));	// move all data to the new buffer
			pBuf->nLength = sizeof(MMI_CMD);
			pB = (BYTE *) pBuf;	// debug helper			
			// add to Send list
			// Send thread msg TransmitPacket to socket owner
			delete pCmd;
			pSocket->LockSendPktList();
			pSocket->AddTailSendPkt(pBuf);
			pSocket->UnLockSendPktList();
			// Thread msg causes CServerSocketOwnerThread::TransmitPackets() to execute

			pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET,0,0L);
			}
		else
			{
			if (NULL == pSocket)
				{
				s =_T("CCCM_PAG::SendSlaveMsg - pSocket is NULL\n");
				TRACE(s);
				delete pCmd;
				return FALSE;
				}
			if (NULL == pThread)
				{
				s =_T("CCCM_PAG::SendSlaveMsg - pThread is NULL\n");
				TRACE(s);
				delete pCmd;
				return FALSE;
				}
			delete pCmd;
			return FALSE;
			}

		}

	else	// likely no connection to intended Instrument client
		{
		s.Format(_T("CCCM_PAG::SendSlaveMsg - stSCM[0].pClientConnection[%d] is NULL\n"), pCmd->Inst_Number_In_PAM);
		TRACE(s);
		delete pCmd;
		return FALSE;
		}

	return TRUE;
	}


/**********************************************************************************
* Compute the distance between the leftmost transducer and the rightmost transducer
*/
int CCCM_PAG::GetMaxXSpan(void)
	{
#if 0
	int nMaxXSpan;
	nMaxXSpan = GetMaxXOffset() - GetMinXOffset();
	return nMaxXSpan;
#endif
	return 0;
	}



int CCCM_PAG::GetMaxXOffset(void)
{
#if 0
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave;
	int i;
	int MaxXOffset = -1000;

	InspState.GetChannelConfig(&ChannelCfg);
	for (nSlave=0; nSlave<MAX_SHOES; nSlave++)
	{
//		InspState.GetChannelConfig(&ChannelCfg);

		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
			switch ( ChannelCfg.Ch[nSlave][i].Type )
			{
			case IS_WALL:
			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			case IS_NOTHING:
				if ( ChannelCfg.Ch[nSlave][i].cXOffset > MaxXOffset )
					MaxXOffset = ChannelCfg.Ch[nSlave][i].cXOffset;
				break;

			default:
				break;
			}
		}
	}

	return MaxXOffset;
#endif
	return 0;
}



int CCCM_PAG::GetMinXOffset(void)
{
#if 0
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave;
	int i;
	int MinXOffset = 1000;

	InspState.GetChannelConfig(&ChannelCfg);
	for (nSlave=0; nSlave<MAX_SHOES; nSlave++)
		{

		for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
			{
			switch ( ChannelCfg.Ch[nSlave][i].Type )
			{
			case IS_WALL:
			case IS_LONG:
			case IS_TRAN:
			case IS_OBQ1:
			case IS_OBQ2:
			case IS_OBQ3:
			case IS_NOTHING:
				if ( ChannelCfg.Ch[nSlave][i].cXOffset < MinXOffset )
					MinXOffset = ChannelCfg.Ch[nSlave][i].cXOffset;
				break;

			default:
				break;
			}
		}
	}

	return MinXOffset;
#endif
	return 0;
}


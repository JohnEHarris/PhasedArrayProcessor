
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
			2017-05-01 PAM=PAP. PA2Win now either PAP or PAG based on compiler preprocessor defines

********************************************************/

			
#include "stdafx.h"

#include "PA2WinDlg.h"
//#include "MC_SysCPTestClient.h"
//#include "MC_SysCPTestClientDlg.h"

//#include "ClientSocket.h"
//#include "ClientConnectionManagement.h"
//#include "ClientCommunicationThread.h"
//#include "TCPCommunicationDlg.h"
//#include "time.h"

//extern THE_APP_CLASS theApp;

//extern 	C_MSG_ALL_THOLD  g_AllTholds;
//extern 	C_MSG_NC_NX g_NcNx;
//extern I_MSG_RUN SendBuf;
//extern DWORD  g_nStation2JointNum;// = 0;
//extern I_MSG_RUN SendCalBuf;



class CCCM_PAG;

CCCM_PAG *pCCM_PAG;
CCCM_PAG *pCCM_PAG_AW;
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
	//m_pstCCM->pCCM->m_pstCCM->pCmdProcessThread = 0; already zero
	gwPapPulserCmds = gwPapSmallCmds = gwPapLargeCmds = 0;
	}

CCCM_PAG::~CCCM_PAG( void )
	{
	// last chance to kill crit sections and lists 
	void *pv = 0;
	if (m_pstCCM->pRcvPktPacketList)
		{
		if (0 == KillLinkedList(m_pstCCM->pCSRcvPkt, m_pstCCM->pRcvPktPacketList))
			TRACE(_T("Failed to kill Receive List\n"));
		else { m_pstCCM->pCSRcvPkt = 0;  m_pstCCM->pRcvPktPacketList = 0; }
		}

	if (m_pstCCM->pSendPktList)
		{
		if (0 == KillLinkedList(m_pstCCM->pCSSendPkt, m_pstCCM->pSendPktList))
			TRACE(_T("Failed to kill Send List\n"));
		else { m_pstCCM->pCSSendPkt = 0;  m_pstCCM->pSendPktList = 0; }
		}

	if (m_pstCCM->pInDebugMessageList)
		{ 
		if (0 == KillLinkedList(m_pstCCM->pCSDebugIn, m_pstCCM->pInDebugMessageList))
			TRACE(_T("Failed to kill Debug In List\n"));
		else { m_pstCCM->pCSDebugIn = 0;  m_pstCCM->pInDebugMessageList = 0; }
		}

	if (m_pstCCM->pOutDebugMessageList)
		{
		if (0 == KillLinkedList(m_pstCCM->pCSDebugOut, m_pstCCM->pOutDebugMessageList))
			TRACE(_T("Failed to kill Debug Out List\n"));
		else { m_pstCCM->pCSDebugOut = 0;  m_pstCCM->pOutDebugMessageList = 0; }
		}

	m_pstCCM->pCCM = 0;

	TRACE("CCM_PAG Destructor called\n");
	// base destructor called implicity after this destructor runs
	}

// This code is specific to communications with the PAG

// We finally get around to processing the message.
// Liberally stolen from Randy's example


// For this class , the received message comes from the PAG
// All commands from PAG are message type PAP_GENERIC_MSG
// Over rides base class ProcessReceivedMessage()
// PAG messages are variable length - must handle each as it arrives and assume
// the whole message arrives in one packet - assumption is that it works like UDP
// Function is primarily to send to correct instrument.
// Called from CCmdProcessThread::ProcessReceivedMessage which was invoked by
// CClientConnectionManagement::OnReceive() - m_pstCCM->pCmdProcessThread->PostThreadMessageA(WM_USER_CLIENT_PKT_RECEIVED, 0,0L);
//
#define NEW_SOCKET
// The new socket/NIC is for the pulser board.

void CCCM_PAG::ProcessReceivedMessage(void)
	{
	USES_CONVERSION;
	CString s;
	int i;
	int is, ic;	// sequence, channel
			
	// 2016-06-27 ditch legacy command structure and use PA2 structure

	PAP_GENERIC_MSG *pMmiCmd;
	//PAP_INST_CHNL_NCNX *pPamChnlInfo;
	ST_NX_CMD *pCmdS;	// small command to set Nx processing for all wall channels
	WORD MsgId;
	ST_SERVERS_CLIENT_CONNECTION *pSCC = NULL;
	int nInst = 0;

	if (m_pstCCM->pRcvPktPacketList == NULL) return;

	while (i = m_pstCCM->pRcvPktPacketList->GetCount())
		{
		LockRcvPktList();
		pMmiCmd = (PAP_GENERIC_MSG *) m_pstCCM->pRcvPktPacketList->RemoveHead();
		UnLockRcvPktList();

		// use pMmiCmd->Slot to identify PAP's pClientConnection to know which instrument to send to
		// pMmiCmd->ChnlNum will specify the channel in that instrument
		// 1st, unpack from linked list and examine each message. Some messages configure the PAP itself
		// while othere may be routed directly to the instruments.

		MsgId = pMmiCmd->wMsgID;
		// MsgId < 0x200 is a small command for the ADC board(s)
		// MsgId >= 0x200 but < 0x300 is a large command for the ADC board(s)
		// MsgId >= 0x300 is routed to the PAP Srv[1] thread to the Pulser board

		ST_WORD_CMD *pSmall;
		pSmall = (ST_WORD_CMD *)pMmiCmd;

		/*       PULSER COMMAND SECTION     */

		if (MsgId >= 0x300)
			{
			// board number should always be 0 for pulser
			/*             FOR INTERIM TESTING USE stSCM[0]*/
			if (MsgId >= 0x300 + LAST_PULSER_COMMAND)
				{
				s.Format(_T(">>>> Invalid Pulser Command=%d <<<<<\n"), MsgId);
				TRACE(s);
				DebugOut(s);
				delete pMmiCmd;
				continue;
				}
#ifdef NEW_SOCKET
			if (stSCM[1].pClientConnection[pMmiCmd->bBoardNumber] == nullptr)
#else
			if (stSCM[0].pClientConnection[pMmiCmd->bBoardNumber] == nullptr)
#endif
				{
				s.Format(_T("No Client Connection ptr for Pulser board number=%d\n"), pMmiCmd->bBoardNumber);
				TRACE(s);
				DebugOut(s);
				delete pMmiCmd;
				continue;		// return;
				}

			// stSCM[1].  use [0] for initial testing
#ifdef NEW_SOCKET
			// assumed operating conditions is boardNumber 0
			// pMmiCmd->bBoardNumber = 0;
			CServerSocket *pSocket = stSCM[1].pClientConnection[pMmiCmd->bBoardNumber]->pSocket;
			CServerSocketOwnerThread *pThread = stSCM[1].pClientConnection[pMmiCmd->bBoardNumber]->pServerSocketOwnerThread;
			stSCM[1].pClientConnection[pMmiCmd->bBoardNumber]->bOwnVChnls = 0;
#else
			// should pMmiCmd->bBoardNumber be 0 ??
			CServerSocket *pSocket = stSCM[0].pClientConnection[pMmiCmd->bBoardNumber]->pSocket;
			CServerSocketOwnerThread *pThread = stSCM[0].pClientConnection[pMmiCmd->bBoardNumber]->pServerSocketOwnerThread;
#endif
			if ((pSocket == 0) || (pThread == 0))
				{
				s = _T("No socket or no thread for CServerSocketOwnerThread to Pulser board\n");
				TRACE(s);
				DebugOut(s);
				delete pMmiCmd;
				continue;		// return;
				}

			// Assuming all pulser board commands are small commands
			if (MsgId <= 0x300 + LAST_PULSER_COMMAND)
				{
				gwPapPulserCmds++;
#if 0
				s.Format(_T("Received from GUI: Cmd= %2d board= %d Seq= %2d Ch= %d G= %d wCmd= %d\n"),
					MsgId, pSmall->Head.bBoardNumber, pSmall->bSeq,
					pSmall->bChnl, pSmall->bGateNumber, pSmall->wCmd);	// 1st word in command values
				TRACE(s);
				pMainDlg->SaveDebugLog(s);
#endif
				// special case for PAP -- grab prf as it goes thru to pulSer and display on PAP screen
				if (MsgId == 0x300 + 0)
					gwPap_Prf = pSmall->wCmd;
				else if (MsgId == 0x300 + 8)	// DebugPrint FOR PULSER
					{
					if ((pSmall->wCmd & 2) == 2)
						gwPapPulserCmds = 0;
					}

				pSocket->LockSendPktList();	// server sockets linked list for sending commands
				pSocket->AddTailSendPkt(pMmiCmd);
				pSocket->UnLockSendPktList();

				// Thread msg causes CServerSocketOwnerThread::TransmitPackets() to execute
				pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, 0, 0L);
				continue;		// break;
				}	// MsgId <= LAST_PULSER_COMMAND

			else
				{
				s.Format(_T("No Pulser command recognized ID = %d\nDeleting command from Phased Array GUI\n"), MsgId);
				delete pMmiCmd;
				pMainDlg->SaveDebugLog(s);
				}
			}

		/*       PULSER COMMAND SECTION ENDS    */

		/*     ADC Board Commands BEGIN         */
		
		else
			{	// command for ADC board

			if (stSCM[0].pClientConnection[pMmiCmd->bBoardNumber] == nullptr)
				{
				s.Format(_T("No Client Connection ptr for ADC board number=%d\n"), pMmiCmd->bBoardNumber);
				TRACE(s);
				DebugOut(s);
				pMainDlg->SaveDebugLog(s);
				delete pMmiCmd;
				continue;		// return;
				}

			CServerSocket *pSocket = stSCM[0].pClientConnection[pMmiCmd->bBoardNumber]->pSocket;
			CServerSocketOwnerThread *pThread = stSCM[0].pClientConnection[pMmiCmd->bBoardNumber]->pServerSocketOwnerThread;
			if ((pSocket == 0) || (pThread == 0))
				{
				s = _T("No socket or no thread for ADC CServerSocketOwnerThread\n");
				TRACE(s);
				DebugOut(s);
				pMainDlg->SaveDebugLog(s);
				delete pMmiCmd;
				continue;		// return;
				}

			// Count large and small commands
			if ((MsgId >= 0x200) && (MsgId <= 0x200 + LAST_LARGE_COMMAND))
				{
				gwPapLargeCmds++;
				gwLastCmdId = MsgId;
				gwLastCmdSeqCnt = pMmiCmd->wMsgSeqCnt;
				gw1stWordCmd = pSmall->wCmd;
				}
			else if ((MsgId >= 0) && (MsgId <= LAST_SMALL_COMMAND))
				{
				gwPapSmallCmds++;
				gwLastCmdId = MsgId;
				gwLastCmdSeqCnt = pMmiCmd->wMsgSeqCnt;
				gw1stWordCmd = pSmall->wCmd;
				if (MsgId == DEBUG_PRINT_CMD_ID)  // small cmd version is ID = 29
					{
					if ((pSmall->wCmd & 2) == 2)
						gwPapSmallCmds = gwPapLargeCmds = 0;
					}
				}
			else
				{	// not a valid command
				s.Format(_T(">>>> Invalid ADC Command = %d <<<<\n"), MsgId);
				TRACE(s);
				DebugOut(s);
				pMainDlg->SaveDebugLog(s);
				delete pMmiCmd;
				continue;
				}

			// big case statement adapted from ServiceApp.cpp 'c' routine ProcessMmiMsg()
			switch (MsgId)
				{

				case NC_NX_CMD_ID:
#if 0
					// The only message as of 2016-06-27
					// Does not get sent to instruments, sets Nc and Nx parameters for the PAM to use
					i = sizeof(PAP_INST_CHNL_NCNX);
					pPamChnlInfo = (PAP_INST_CHNL_NCNX *)pMmiCmd;
					s.Format(_T("Received NC_NX_CMD_ID for Instrument %d from Phased Array GUI - now deleting\n"), pMmiCmd->bBoardNumber);
					TRACE(s);
					SetChannelInfo(pPamChnlInfo);
					// For debugging instrument commands, send Hello message to the connected instrument
					// Echo this message to the simulator to test commands which must go to the instrument
					// In this case, we will let the thread which forwards the message to the instrument delete this 
					// memory segment.
					// delete pMmiCmd;

					pSocket->LockSendPktList();
					pSocket->AddTailSendPkt(pMmiCmd);
					pSocket->UnLockSendPktList();
					// Thread msg causes CServerSocketOwnerThread::TransmitPackets() to execute
					pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, 0, 0L);
#endif
					delete pMmiCmd;
					break;

				case SET_WALL_NX_CMD_ID:	// cmd 28
					// Only executes on PAP... all wall processing has same parameters for every wall channel
					// virtual channels exist in Server side structure only
					// Server side connects to Adc and other hardware data sources
					pCmdS = (ST_NX_CMD *)pMmiCmd;	// cast this message to Nx command format
					nInst = pCmdS->bBoardNumber;	// which instrument

					// For now server[0] connects to Adc clients
					// Not going to send info to client, but does process that clients data with these parameters
					pSCC = stSCM[0].pClientConnection[nInst];
					if (pSCC == NULL)
						{
						s.Format(_T("pSCC = stSCM[0].pClientConnection[%d] is null\n"), nInst);
						TRACE(s);
						DebugOut(s);
						pMainDlg->SaveDebugLog(s);
						delete pMmiCmd;
						break;
						}

					CvChannel *pChannel;
					// for now assume sequence length 3 and 8 chnls per sequence

					for (is = 0; is < gnSeqModulo; is++)
						for (ic = 0; ic < MAX_CHNLS_PER_MAIN_BANG; ic++)
							{
							pChannel = pSCC->pvChannel[is][ic];
							// cmd word [0]=Nx [1]=Max [2]=Min [3]=Drop
							pChannel->WFifoInit((BYTE)pCmdS->wNx, pCmdS->wMax, pCmdS->wMin, pCmdS->wDropCount);
							// SHOW something on PAG output screen
							if ((is == 0) && (ic == 0))
								{
								}
							}
					delete pMmiCmd;
					break;

					//case 2-8:	// Gate commands from PAG TO PAP then PAP to Board

				default:
					// Add the command to the linked list associated with/belonging to the socket
					if (MsgId >= 0x200)
						{
						// Large message.. forward to NIOS boards
						if (MsgId <= LAST_LARGE_COMMAND + 0X200)
							{
							pSocket->LockSendPktList();
							pSocket->AddTailSendPkt(pMmiCmd);
							pSocket->UnLockSendPktList();

							// Thread msg causes CServerSocketOwnerThread::TransmitPackets() to execute
							pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, 0, 0L);
#if 0
							s.Format(_T("Received Large Cmd %d for board %d from Phased Array GUI\n"),
								MsgId, pMmiCmd->bBoardNumber);
							TRACE(s);
							pMainDlg->SaveDebugLog(s);
#endif
							}

						else
							{
							s.Format(_T(">>>> Invalid large command ID = %d <<<<\n"), MsgId);
							TRACE(s);
							pMainDlg->SaveDebugLog(s);
							delete pMmiCmd;
							continue;
							}
						break;
						}

					if (MsgId <= LAST_SMALL_COMMAND)
						{
						ST_WORD_CMD *pSmall;
						pSmall = (ST_WORD_CMD *)pMmiCmd;
#if 0
						s.Format(_T("Received from GUI: Cmd= %2d board= %d Seq= %2d Ch= %d G= %d wCmd= %d\n"),
							MsgId, pSmall->Head.bBoardNumber, pSmall->bSeq,
							pSmall->bChnl, pSmall->bGateNumber, pSmall->wCmd);	// 1st word in command values
						TRACE(s);
						pMainDlg->SaveDebugLog(s);
#endif

						pSocket->LockSendPktList();	// server sockets linked list for sending
						pSocket->AddTailSendPkt(pMmiCmd);
						pSocket->UnLockSendPktList();

						// Thread msg causes CServerSocketOwnerThread::TransmitPackets() to execute
						pThread->PostThreadMessage(WM_USER_SERVER_SEND_PACKET, 0, 0L);
						break;
						}

					else
						{
						s.Format(_T(">>>> No command recognized ID = %d <<<<\nDeleting command from Phased Array GUI\n"), MsgId);
						pMainDlg->SaveDebugLog(s);
						delete pMmiCmd;
						}
				}	// end switch(MsgId)			delete pMmiCmd;

			}	// command for ADC board
				
		/*     ADC Board Commands               */

		//m_nMsgQty++;	// not used anywhere 2018-12-12

		}	// while (m_pstCCM->pRcvPktPacketList->GetCount())

	// Could wait until all linked list built and then post message to transmit.
	// Might result in lost packets on NIOS/Wiznet side - or maybe make commands execute faster.

	}	// ProcessReceivedMessage(void)


	// not part of the base class... debug
void CCCM_PAG::UniqueProc(void)
	{
	CString s;
	int i;
	i = m_nMyConnection;
	s.Format(_T("Unique to the child class of CCM, my connection = %d\n"), i);
	TRACE(s);
	}



// These functions copied from ServiceApp.cpp
// does Yanming number slaves 1..32. My scheme is 0..31
int CCCM_PAG::FindWhichSlave(int nChannel)
	{
	// UNTIL WE HAVE A GOOD FIX, RETURN 0
	return 0;
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

// I doubt that this works correctly  --- 2018-03-23 jeh --- don't use
void CCCM_PAG::SetChannelInfo(PAP_INST_CHNL_NCNX *pPamInstChnlInfo)
	{
	int nPam, nInst, nSeq, nCh, msgcnt;
	//int i,j,k;
	//int i;
	int nChnlQty = 0;
	CString s;
	CvChannel *pChannel;

	ST_SERVER_CONNECTION_MANAGEMENT *pPAM_SCM = GetPAM_SCM();

	nPam		= pPamInstChnlInfo->bPapNumber;
	nInst		= pPamInstChnlInfo->bBoardNumber;
	msgcnt		= 0;

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

	// packet has 90 max valid readings.
#if	DEBUG_TCPIP_FROM_PAG
	s.Format(_T("Start seq=%d, ch=%d ....."),pNcNx->bSeqNumber, pNcNx->bChnlNumber);
	TRACE(s);
#endif
	nCh = nSeq = 0;
	while ( (pNcNx->bSeqNumber < MAX_SEQ_COUNT) && (pNcNx->bChnlNumber < MAX_CHNLS_PER_MAIN_BANG) )
		{
		nCh = pNcNx->bChnlNumber;
		nSeq = pNcNx->bSeqNumber;

		// [seq][chnl]
		pChannel = pSCC->pvChannel[nSeq][nCh];
		if (pChannel == NULL)
			{
			TRACE(_T("pChannel == NULL"));
			break;
			}
#if	DEBUG_TCPIP_FROM_PAG
		s.Format(_T("Seq=%2d Ch=%2d, NcId=%3d\n"), nSeq, nCh, pNcNx->bNcID);
		TRACE(s);
#endif
		pChannel->FifoInit(0, pNcNx->bNcID,pNcNx->bTholdID, pNcNx->bModID); 
		pChannel->FifoInit(1, pNcNx->bNcOD,pNcNx->bTholdOD, pNcNx->bModOD); 
		pChannel->FifoInit(2, pNcNx->bNcIf,pNcNx->bTholdIf, pNcNx->bModIf); 
		//FifoInit(BYTE bIdOd, BYTE bNc, BYTE bThld, BYTE bMod)
		pChannel->WFifoInit((BYTE)pNcNx->wNx, pNcNx->wWallMax, pNcNx->wWallMin, pNcNx->wDropOut);
		//WFifoInit(BYTE bNx, WORD wMax, WORD wMin, WORD wDropOut)
		nChnlQty++;
		pNcNx++;
		}
#if	1
	//DEBUG_TCPIP_FROM_PAG
	s.Format(_T("End seq=%d, ch=%d, Chnls Set=%d\n"),nSeq, nCh, nChnlQty);
	TRACE(s);
#endif

	}

// 2018-03-23 Executes only on PAP
// Sets all wall processing variable for each channel to the same value.

void CCCM_PAG::WallNx(void)
	{
	}

#if 0
// Send the same message to all Instruments
BOOL CCCM_PAG::SendSlaveMsgToAll(ST_LARGE_CMD *pCmd)
	{
	int i;
	i = 2;

	return TRUE;
	}

// These functions copied from ServiceApp.cpp -- originally  'c' function
// A command received from PAG AND FORWARDED to an Instrument
//
BOOL CCCM_PAG::SendSlaveMsg(int nWhichInstrument, ST_LARGE_CMD *pCmd)
	{
	BYTE *pB;
	CString s;
	stSEND_PACKET *pBuf;

	if (pCmd->bBoardNumber !=nWhichInstrument)
		{
		ASSERT(0);
		s.Format(_T("pCmd->bBoardNumber=%d, m_nWhichInstrument=%d\n"), pCmd->bBoardNumber, nWhichInstrument);
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		delete pCmd;
		return FALSE;
		}
	if (pCmd->bBoardNumber >= MAX_CLIENTS_PER_SERVER)
		{
		ASSERT(0);
		s.Format(_T("pCmd->bBoardNumber=%d, MAX_CLIENTS_PER_SERVER=%d\n"), pCmd->bBoardNumber, MAX_CLIENTS_PER_SERVER);
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		delete pCmd;
		return FALSE;
		}

	// add pCmd to stSCM[0] server (server for all attached Instruments) send packet linked list
	// in stSCM[0].pClientConnection[pCmd->Slot] connection structure
	if (stSCM[0].pClientConnection[pCmd->bBoardNumber])
		{
		CServerSocket *pSocket				= stSCM[0].pClientConnection[pCmd->bBoardNumber]->pSocket;
		CServerSocketOwnerThread *pThread	= stSCM[0].pClientConnection[pCmd->bBoardNumber]->pServerSocketOwnerThread;
		if ( pSocket && pThread)
			{
			// differentiate between large and small commands
			if (pCmd->wByteCount <= sizeof( ST_SMALL_CMD ))
				{
				pBuf = (stSEND_PACKET *) new BYTE[sizeof( ST_SMALL_CMD ) + sizeof( int )];	// resize the buffer that will actually be used
				memcpy( (void *)&pBuf->Msg, pCmd, sizeof( ST_SMALL_CMD ) );	// move all data to the new buffer
				pBuf->nLength = sizeof( ST_SMALL_CMD );
				}
			else
				{
				// create a new buffer with length + data
				pBuf = (stSEND_PACKET *) new BYTE[sizeof( ST_LARGE_CMD ) + sizeof( int )];	// resize the buffer that will actually be used
				memcpy( (void *)&pBuf->Msg, pCmd, sizeof( ST_LARGE_CMD ) );	// move all data to the new buffer
				pBuf->nLength = sizeof( ST_LARGE_CMD );
				}
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
				pMainDlg->SaveDebugLog(s);
				delete pCmd;
				return FALSE;
				}
			if (NULL == pThread)
				{
				s =_T("CCCM_PAG::SendSlaveMsg - pThread is NULL\n");
				TRACE(s);
				pMainDlg->SaveDebugLog(s);
				delete pCmd;
				return FALSE;
				}
			delete pCmd;
			return FALSE;
			}

		}

	else	// likely no connection to intended Instrument client
		{
		s.Format(_T("CCCM_PAG::SendSlaveMsg - stSCM[0].pClientConnection[%d] is NULL\n"), pCmd->bBoardNumber);
		TRACE(s);
		pMainDlg->SaveDebugLog(s);
		delete pCmd;
		return FALSE;
		}

	return TRUE;
	}
#endif


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
	return 0;
}



int CCCM_PAG::GetMinXOffset(void)
{

	return 0;
}


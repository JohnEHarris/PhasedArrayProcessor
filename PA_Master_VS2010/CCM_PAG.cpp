
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
extern  int g_NumberOfScans;
extern 	int g_nXloc;
extern 	int g_nXloc_S2;
extern 	int g_nTick;
extern 	DWORD g_nOldMotionBus;
extern 	int g_nAuxClkIntCnt;
extern 	BYTE g_nPlcOfWho;
extern 	WORD g_nNextWindow;
extern 	int g_nMaxXSpan;       /* distance between the leftmost transducer and the rightmost transducer */
extern 	float g_fTimePerTurn;
extern 	int g_nNoMmiReplyCnt;
extern 	DWORD g_nNextRealJointNum;
extern 	DWORD  g_nNextCalJointNum;	// = 5001;
extern 	int g_NumberOfScans;
extern 	int g_NumberOfScans;

extern  int g_ArrayScanNum[NUM_OF_SLAVES];
extern 	C_MSG_ALL_THOLD  g_AllTholds;
extern 	C_MSG_NC_NX g_NcNx;
extern 	WALL_COEF  g_WallCoef;
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
extern BYTE   g_bBcastAscan;
extern I_MSG_RUN SendCalBuf;
extern I_MSG_CAL  CalBuf;
extern I_MSG_RUN ImageBuf[IMAGE_BUF_DIM];

extern BOOL   g_bAnyShoeDown;	// = FALSE;

extern BOOL   g_b20ChnlPerHead;	// = FALSE;
extern WORD   g_nRecordWallData;
extern BOOL   g_bShowWallDiff;	// = FALSE;

extern BOOL   g_bSendRawFlawToMMI;	// = FALSE;

extern WORD   g_nPulserPRF;	// = 1000;
extern WORD   g_nMaxWallWindowSize;	// = 10;
extern float  g_WallDropTime;	// = 0.1f;       /* initialized to 0.1 seconds */
extern int    g_NumberOfScans;	// = 3;
extern int	   g_ArrayScanNum[NUM_OF_SLAVES];
extern int    g_SequenceLength[NUM_OF_SLAVES];
extern int    g_nPhasedArrayScanType[NUM_OF_SLAVES];
extern int    g_nInspectMode;	// = CAL_MODE;
extern JOB_REC  g_JobRec;
extern BYTE   g_AdiStatus;
extern int nBufin, nBufout;			/* index from 0-IMAGE_BUF_DIM - 1 */
extern int nBufcnt;				/* image buffer management */
extern int nPreviousX;	// =0;
extern int nFlush;
extern int nMaxX;
extern int gChannel, gGate;

#define I_AM_CCM_PAG
#include "CCM_PAG.h"
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

	MMI_CMD *pMmiCmd;
	// local variables copied from ServiceApp.cpp 'c' routine ProcessMmiMsg()
	// MMI_CMD *pMmiCmd = (MMI_CMD *) &readBuf;

	WORD *pWArg;
	DWORD *pDWArg;
	float *pFArg;
	WORD MsgId;
	int i;		//, rc;  /* generic looper */
	static int nChannel = 0;
	static int nGate = 0;
	static int old_gChannel = 0;
	//int  nWhichSlave;
	SHOE_CONFIG  *pShCfg;
	C_MSG_ALL_THOLD *pThold;
	WALL_COEF *pWallCoef;
	SITE_SPECIFIC_DEFAULTS *pSiteDef;
	CHANNEL_CONFIG2 ChannelCfg;
//	static int  nInspectMode = NOP_MODE;
	int nMotionTime;
	MMI_CMD  *ptempCmd;
	WORD nEnableAscan = 0;
	JOB_REC *pJobRec;
	C_MSG_NC_NX *pNcNx;	// = (C_MSG_NC_NX *) pMmiCmd->CmdBuf;



	ASSERT(m_pstCCM);
	if (NULL == m_pstCCM)	return;		// something wrong here
	if (m_pstCCM->pRcvPktPacketList->IsEmpty())		return;	// shouldn't have been called, but no harm
	// temp during debug

	while (m_pstCCM->pRcvPktPacketList->GetCount())
		{
		LockRcvPktList();
		pMmiCmd = (MMI_CMD *) m_pstCCM->pRcvPktPacketList->RemoveHead();
		UnLockRcvPktList();

		// use pMmiCmd->Slot to identify PAM's pClientConnection to know which instrument to send to
		// pMmiCmd->ChnlNum will specify the channel in that instrument
		// 1st, unpack from linked list and examine each message. Some messages configure the PAM itself
		// while othere may be routed directly to the instruments.

			
		MsgId = pMmiCmd->MsgId;

		pNcNx = (C_MSG_NC_NX *) pMmiCmd->CmdBuf;
		pJobRec = (JOB_REC *) pMmiCmd->CmdBuf;

		pWArg = (WORD *) pMmiCmd->CmdBuf;
		pDWArg = (DWORD *) pMmiCmd->CmdBuf;
		pFArg = (float *) pMmiCmd->CmdBuf;

		pShCfg = (SHOE_CONFIG *) pMmiCmd->CmdBuf;
		pThold = (C_MSG_ALL_THOLD *) pMmiCmd->CmdBuf;

		// big case statement adapted from ServiceApp.cpp 'c' routine ProcessMmiMsg()
#if 1
		switch(MsgId)
			{
		case CHANNEL_SELECT:
			nChannel = pWArg[0];
			gChannel = nChannel;// % MAX_CHANNEL_PER_INSTRUMENT;
			//nWhichSlave = nChannel / g_NumberOfScans;
//			pMmiCmd->Slot = m_nWhichInstrument = FindWhichSlave(nChannel);
			m_nWhichInstrument = pMmiCmd->Inst_Number_In_PAM;
			//pWArg[0] = nChannel % g_NumberOfScans;
			pWArg[0] = FindSlaveChannel(nChannel);
//				SetGetInspectMode_M (1 /* GET */, &nInspectMode, &nMotionTime);
			if (InspState.GetInspectMode() != PKT_MODE)
				SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
			else
				{
				/* if active channel is changed from one slave to another */
				if ( (nChannel/g_NumberOfScans) != (old_gChannel/g_NumberOfScans) )
					{
					m_nWhichInstrument = old_gChannel / g_NumberOfScans;
					ptempCmd = new MMI_CMD;
					ptempCmd->MsgId = NOP_MODE;
					ptempCmd->Inst_Number_In_PAM = m_nWhichInstrument;
					SendSlaveMsg (m_nWhichInstrument, ptempCmd);			// SendSlaveMsg delete Cmd at end of function
					m_nWhichInstrument = nChannel / g_NumberOfScans;
					ptempCmd->MsgId = PKT_MODE;
					ptempCmd->Inst_Number_In_PAM = m_nWhichInstrument;
					SendSlaveMsg (m_nWhichInstrument, ptempCmd);
					delete pMmiCmd;
					}
				else
					SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
				}
			old_gChannel = nChannel;
			break;

		case SET_ASCAN_READ_SEQ:
			m_nWhichInstrument = FindWhichSlave(pWArg[0]);
			pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument;
			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
			//printf("set ascan read seq.\n");
			break;

		case SET_ASCAN_READ_BEAM:
			pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument = pWArg[0];
			SendSlaveMsg (pWArg[0], pMmiCmd);
			break;

		case GATE_SELECT:
			nGate = pWArg[0] % MAX_GATES;
			gGate = nGate;
			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
			break;

		case PULSER_ONOFF:
			m_nWhichInstrument = pWArg[0] % MAX_CLIENTS_PER_SERVER;	// JEH 22-Feb-13 need fix
			pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument;
			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
			break;

		case PULSER_PRF:
			g_nPulserPRF = pWArg[1];
			if (g_nPulserPRF < 1) g_nPulserPRF = 1000;
			SendSlaveMsgToAll(pMmiCmd);
			break;

		case SCOPE_TRACE1_MDAC:
			pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument = pWArg[0];
			SendSlaveMsg (pWArg[0], pMmiCmd);
			break;

		case RUN_MODE:  /* inspection run mode */
			nMotionTime = (int) pWArg[0];
			g_nXloc = 0;
			g_nXloc_S2 = 0;
			g_nTick = 0;
			g_nOldMotionBus = 0;
			InspState.SetInspectMode(RUN_MODE);
			InspState.SetMotionMode(nMotionTime);
			//SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
			g_nAuxClkIntCnt = -10;
			SendSlaveMsgToAll(pMmiCmd);
			break;

		case CAL_MODE:  /* calibration mode */
			InspState.SetInspectMode(CAL_MODE);
			//SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
			SendSlaveMsgToAll(pMmiCmd);
			break;

		case PKT_MODE:  /* calibration mode */
			InspState.SetInspectMode(PKT_MODE);
			//SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
			break;

		case PLC_MODE:  /*  */
			InspState.SetInspectMode(PLC_MODE);
			//SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);

			g_nPlcOfWho = (BYTE) pWArg[0];
			m_nOldInstrument = m_nWhichInstrument;
 	   		if (pWArg[0] > 0)
 	   			{
 	   			pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument = pWArg[0] - 1;
 	   			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
 	   			}
			break;

		case ADC_MODE:
  	  		pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument = pWArg[0];
  	  		SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
  	  		break;

		case NOP_MODE:  /* inspection stop mode */
	#if 1
			InitImageBufArray ();
	#endif
  	  		//nInspectMode = NOP_MODE;
			InspState.SetInspectMode(NOP_MODE);
   	 		//SetGetInspectMode_M (0 /* SET */, &nInspectMode, &nMotionTime);
    		g_nXloc = 0;
    		g_nXloc_S2 = 0;
    		g_nTick = 0;
			g_nNextWindow = 0;
			SendSlaveMsgToAll(pMmiCmd);
    		break;

		case ASCAN_MODE:
			//printf("MasterTCPIP -- ASCAN_MODE.\n");
 	   		nEnableAscan = pWArg[0];
			m_nOldInstrument = m_nWhichInstrument;
  	  		for (i=0; i<MAX_CLIENTS_PER_SERVER; i++)
   	 			{
    			if ( (i == m_nWhichInstrument) && (nEnableAscan == 1) )		pWArg[0] = 1;
    			else	    					pWArg[0] = 0;

				ptempCmd = new MMI_CMD;
				memcpy(ptempCmd,pMmiCmd, sizeof(MMI_CMD));
				ptempCmd->Inst_Number_In_PAM = i;
				SendSlaveMsg (i, ptempCmd);
				}
			m_nWhichInstrument = m_nOldInstrument;
			delete pMmiCmd;
  	  		break;

		case CHANNEL_CONFIG_MSG:
    		m_nWhichInstrument = pShCfg->nSlave;
    		for ( i = 0; i < MAX_CHANNEL_PER_INSTRUMENT; i++)
    			{
	    		ChannelCfg.Ch[m_nWhichInstrument][i].Type = pShCfg->Ch[i].Type;
 	   			ChannelCfg.Ch[m_nWhichInstrument][i].cXOffset = pShCfg->Ch[i].cXOffset;
  	  			ChannelCfg.Ch[m_nWhichInstrument][i].cWOffset = pShCfg->Ch[i].cWOffset;
   	 			}
    		//ChannelCfg.cClockOffset = pChnlCfg->cClockOffset;
    		//SetGetChannelCfg (0 /* SET */, &ChannelCfg, m_nWhichInstrument);
			InspState.SetChannelConfig(&ChannelCfg, m_nWhichInstrument);
			SetChannelInfo();
    		g_nMaxXSpan = GetMaxXSpan();
			InitImageBufArray ();
  	  		SendSlaveMsg (m_nWhichInstrument, pMmiCmd);		
   	 		break;

		case SET_ALL_THOLDS:
 	   		memcpy ( (void *) &g_AllTholds, (void *) pThold, sizeof(C_MSG_ALL_THOLD) );
			InspState.SetChannelConfig(&ChannelCfg, m_nWhichInstrument);
			SetChannelInfo();
			SendSlaveMsgToAll(pMmiCmd);
  	  		break;

		case SITE_SPECIFIC_MSG:
			pSiteDef = (SITE_SPECIFIC_DEFAULTS *) pMmiCmd->CmdBuf;
 	   		//SetGetSiteDefaults(0 /*SET*/, pSiteDef);
			InspState.SetSiteDefaults(pSiteDef);	// load the site defaults from file in MMI
  	  		g_nRecordWallData = pSiteDef->nRecordWallData;

   	 		if (pSiteDef->nDefaultLineSpeed > 0)
    				g_fTimePerInch = 1.0f / ( ((float) pSiteDef->nDefaultLineSpeed)*12.0f/6000.0f );
    		if (pSiteDef->nDefaultRotateSpeed > 0)
    			g_fTimePerTurn = 1.0f / ( ((float)pSiteDef->nDefaultRotateSpeed)/6000.0f );

    		if (pSiteDef->n20ChnlPerHead == 0)
    			g_b20ChnlPerHead = FALSE;
    		else
    			g_b20ChnlPerHead = TRUE;

			if ( pSiteDef->fMotionPulseLen > 0.0f)
				g_fMotionPulseLen = pSiteDef->fMotionPulseLen;

			// 1st msg to all
			ptempCmd = new MMI_CMD;
			memcpy(ptempCmd,pMmiCmd, sizeof(MMI_CMD));
			SendSlaveMsgToAll(ptempCmd);

 	   		g_nMaxWallWindowSize = pSiteDef->nMaxWallWindowSize;
  	  		if ( (g_nMaxWallWindowSize > WALL_BUF_SIZE) || (g_nMaxWallWindowSize < 1) )
   	 			g_nMaxWallWindowSize = 10;

    		pMmiCmd->MsgId = WALL_DROP_TIME;
    		pWArg[0] = pSiteDef->nWallDropTime;

			// 2nd message to all
			ptempCmd = new MMI_CMD;
			memcpy(ptempCmd,pMmiCmd, sizeof(MMI_CMD));
			SendSlaveMsgToAll(ptempCmd);

   	 		g_WallDropTime = pWArg[0] / 1000.f;

			g_NumberOfScans = 0;
			for (i=0; i<NUM_OF_SLAVES; i++)
				{
				switch(pSiteDef->nPhasedArrayScanType[i])
					{
				case LINEAR_SCAN_0_DEGREE:
				case LINEAR_SCAN_37_DEGREE:
					g_NumberOfScans += 1;
					g_ArrayScanNum[i] = 1;
					g_SequenceLength[i] = 49;
					break;

				default:
				case THREE_SCAN_LRW_8_BEAM:
				case THREE_SCAN_LRW_8_BEAM_FOCUS:
				case THREE_SCAN_LO1LO1R_8_BEAM_12345678:
				case THREE_SCAN_LO1LO1R_8_BEAM_56781234:
					g_NumberOfScans += 3;
					g_ArrayScanNum[i] = 3;
					g_SequenceLength[i] = 24;
					break;

				case THREE_SCAN_LRW_16_BEAM:
					g_NumberOfScans += 3;
					g_ArrayScanNum[i] = 3;
					g_SequenceLength[i] = 48;
					break;

				case TWO_SCAN_LR_8_BEAM:
					g_NumberOfScans += 2;
					g_ArrayScanNum[i] = 2;
					g_SequenceLength[i] = 16;
					break;

				case TWO_SCAN_LR_16_BEAM:
					g_NumberOfScans += 2;
					g_ArrayScanNum[i] = 2;
					g_SequenceLength[i] = 32;
					break;

				case LONG_8_BEAM_12345678:
				case LONG_8_BEAM_56781234:
					g_NumberOfScans += 1;
					g_ArrayScanNum[i] = 1;
					g_SequenceLength[i] = 8;
					break;

				case LONG_24_BEAM_800:
				case LONG_24_BEAM_080:
				case LONG_24_BEAM_12345678:
				case LONG_24_BEAM_56781234:
					g_NumberOfScans += 1;
					g_ArrayScanNum[i] = 1;
					g_SequenceLength[i] = 24;
					break;

				case WALL_25_BEAM_90_DEGREE_PROBE:
					g_NumberOfScans += 1;
					g_ArrayScanNum[i] = 1;
					g_SequenceLength[i] = 25;
					break;
					}

				g_nPhasedArrayScanType[i] = pSiteDef->nPhasedArrayScanType[i];
				InspState.SetChannelConfig(&ChannelCfg, m_nWhichInstrument);
				SetChannelInfo();
				}
					
			delete pMmiCmd;
    		break;

		case SET_NC_NX:
    		memcpy ( (void *) &g_NcNx, (void *) pNcNx, sizeof (C_MSG_NC_NX) );
			InspState.SetChannelConfig(&ChannelCfg, m_nWhichInstrument);
			SetChannelInfo();
			SendSlaveMsgToAll(pMmiCmd);
			break;

		case SET_WALL_COEFS:
    		pWallCoef = (WALL_COEF *) pMmiCmd->CmdBuf;
    		memcpy( (void *) &g_WallCoef, (void *) pWallCoef, sizeof (WALL_COEF) );
			m_nOldInstrument = m_nWhichInstrument;
			//for (i=0; i<NUM_OF_SLAVES; i++)
			for (i=0; i<MAX_CLIENTS_PER_SERVER; i++)
				{
	    		pWallCoef->fWallSlope[0] = pWallCoef->fWallSlope[i];	// MAX_SHOES not necessarily MAX_CLIENTS_PER_SERVER
 	   			pWallCoef->WallOffset[0] = pWallCoef->WallOffset[i];

				ptempCmd = new MMI_CMD;
				memcpy(ptempCmd,pMmiCmd, sizeof(MMI_CMD));
				ptempCmd->Inst_Number_In_PAM = i;
				SendSlaveMsg (i, ptempCmd);
   	 			}
			InspState.SetChannelConfig(&ChannelCfg, m_nWhichInstrument);
			SetChannelInfo();
 			m_nWhichInstrument = m_nOldInstrument;
			delete pMmiCmd;
   			break;

		case 0x69:   /* sequence length */
		case FIRING_SEQ:
    		pMmiCmd->Inst_Number_In_PAM = m_nWhichInstrument = pWArg[0]/10;
			pWArg[0] %= 10;
 			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
  			break;

		case JOINT_NUMBER_MSG:
   			g_nJointNum = pDWArg[0];
			SendSlaveMsgToAll(pMmiCmd);
			break;

		case RECEIVER_GAIN:
		case TCG_FN:
			m_nWhichInstrument = FindWhichSlave(pMmiCmd->ChnlNum);
			SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
   	 		break;

		case RECEIVER_FCNT:   /* receiver offset */
    		m_nWhichInstrument = FindWhichSlave(pWArg[1]);
    		SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
    		break;

		case ENET_STATS_MODE:
    		g_nNoMmiReplyCnt = 0;
			SendSlaveMsgToAll(pMmiCmd);
   	 		break;

		case 0x63:   /* TCG_TRIGGER, TCG_STEP */
    		m_nWhichInstrument = FindWhichSlave(pWArg[1]);
 	   		SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
  	  		break;

		case RUN_CAL_JOINT:
			if (pWArg[0])
 	   			g_bRunCalJoint = TRUE;
  	  		else
   	 			g_bRunCalJoint = FALSE;
    		if (g_bRunCalJoint)
    			{
    			g_nNextRealJointNum = g_nJointNum;
    			g_nJointNum = g_nNextCalJointNum;
				}
 	   		else
  	  			{
   	 			g_nNextCalJointNum = g_nJointNum;
    			g_nJointNum = g_nNextRealJointNum;
    			}
			delete pMmiCmd;
    		break;

		case LAST_JOINT_NUMBER:
			g_nNextRealJointNum = pDWArg[0]+1;
 	   		g_nNextCalJointNum = pDWArg[1]+1;
  	  		if (g_bRunCalJoint)
   	 			{
    			g_nJointNum = g_nNextCalJointNum;
    			}
    		else
				{
		   		g_nJointNum = g_nNextRealJointNum;
  	  			}
			delete pMmiCmd;
   	 		break;

		case STORE_JOBREC_MSG:
			g_bShowWallDiff = pJobRec->ShowWallDiff;
			memcpy( (void *) &g_JobRec, (void *) pJobRec, sizeof (JOB_REC) );
			// shouldn't there be a break here ????
			// none present in original code
			delete pMmiCmd;	// added 2-15-2013 jeh
   	 		break;			// added 2-15-2013 jeh

		case WALL_DROP_TIME:
			SendSlaveMsgToAll(pMmiCmd);
			break;

		case LOAD_CONFIG_FILE:
			SendSlaveMsgToAll(pMmiCmd);
    		if (g_AdiStatus != 3)
    			g_AdiStatus = 1;
			break;

		case ASCAN_REFRESH_RATE:
			SendSlaveMsgToAll(pMmiCmd);
    		break;

		case ASCAN_BROADCAST:
			g_bBcastAscan = (BYTE) pWArg[0];
			delete pMmiCmd; 
 	   		break;

		case SET_X_SCALE:
			g_nXscale = pWArg[0];
 	   		if (g_nXscale < 600) g_nXscale = 900;
  	  		if (g_nXscale > 900) g_nXscale = 900;
			delete pMmiCmd; 
   	 		break;

		case SET_PIPE_PRESENT:
			SendSlaveMsgToAll(pMmiCmd);
    		break;

		case TURN_OFF_MASTER:
			//ShutDownSystem();
			g_nShowWallBars = pWArg[0];
			delete pMmiCmd; 
			break;

		default:
			if (m_nWhichInstrument != pMmiCmd->Inst_Number_In_PAM)
				{
				s.Format(_T("CCCM_PAG::ProcessReceivedMessage.default m_nWhichInstrument=%d, pMmiCmd->Inst_Number_In_PAM=%d\n"),
					m_nWhichInstrument, pMmiCmd->Inst_Number_In_PAM);
				TRACE(s);
				m_nWhichInstrument = pMmiCmd->Inst_Number_In_PAM;
				}

    		SendSlaveMsg (m_nWhichInstrument, pMmiCmd);
    		break;
			}	// end switch(MsgId)

#endif

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
		sum += g_ArrayScanNum[i];

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
	int sum=0, nSlaveCh=0, i;
	for (i=0; i<10; i++)
		{
		sum += g_ArrayScanNum[i];
		if ( (nChannel / sum) == 0)
			{
			nSlaveCh = nChannel - (sum - g_ArrayScanNum[i]);
			break;
			}
		}
	return nSlaveCh;
}

int CCCM_PAG::FindDisplayChannel(int nArray, int nArrayCh)
	{
	int nDispCh = nArrayCh, i;
	for (i=0; i<nArray; i++)
		{
		nDispCh += g_ArrayScanNum[i];
		}

	return nDispCh;
	}
//
// These function copied from InspMsgProcess class

// usually called when an MMI command is received to set channel config info
// Then SetChannelInfo will set the channel types for this instrument
//void CInstMsgProcess::SetChannelInfo(void)
void CCCM_PAG::SetChannelInfo(void)
	{
	CHANNEL_CONFIG2 ChannelCfg;
	int i, nDispCh;
	int nSlave = GetInstNumber();
	InspState.GetChannelConfig(&ChannelCfg);

	for (i=0; i<MAX_CHANNEL_PER_INSTRUMENT; i++)
		{
		nDispCh = FindDisplayChannel(nSlave, i);

		if (i < g_ArrayScanNum[nSlave])
			m_ChannelInfo[i].channel_type = ChannelCfg.Ch[nDispCh/MAX_CHANNEL_PER_INSTRUMENT][nDispCh%MAX_CHANNEL_PER_INSTRUMENT].Type;
		else
			m_ChannelInfo[i].channel_type = IS_NOTHING;

		switch (m_ChannelInfo[i].channel_type)
			{
		case IS_LONG:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdLong[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdLong[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Long[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Long[1];
			break;
		case IS_TRAN:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdTran[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdTran[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Tran[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Tran[1];
			break;
		case IS_OBQ1:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdOblq1[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdOblq1[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Oblq1[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Oblq1[1];
			break;
		case IS_OBQ2:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdOblq2[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdOblq2[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Oblq2[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Oblq2[1];
			break;
		case IS_OBQ3:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdOblq3[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdOblq3[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Oblq3[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Oblq3[1];
			break;
		case IS_WALL:
			m_ChannelInfo[i].id_thold = g_AllTholds.TholdLamin[0];
			m_ChannelInfo[i].od_thold = g_AllTholds.TholdLamin[1];
			m_ChannelInfo[i].TholdWallThds[0] = g_AllTholds.TholdWallThds[0];
			m_ChannelInfo[i].TholdWallThds[1] = g_AllTholds.TholdWallThds[1];
			m_ChannelInfo[i].nc_for_id = g_NcNx.Lamin[0];
			m_ChannelInfo[i].nc_for_od = g_NcNx.Lamin[1];
			m_ChannelInfo[i].nx_for_wall = g_NcNx.Wall[0];
			break;
		default:
			break;
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

void CCCM_PAG::InitImageBufArray(void)
{
	int i, nSlave, ct,ci;
	int MaxXOffset;
	CHANNEL_CONFIG2 ChannelCfg;
	WORD *pWord;

	MaxXOffset = GetMaxXOffset();
	//MaxXOffset = 0;

	memset( (void *) &ImageBuf, 0, sizeof(ImageBuf));

	nBufout = nPreviousX = nMaxX = 0;
	nBufin = IMAGE_BUF_OUTPUT_DELAY;
	nBufcnt = IMAGE_BUF_OUTPUT_DELAY + MaxXOffset + 1;

	//for (nSlave=0; nSlave<4; nSlave++)
		InspState.GetChannelConfig(&ChannelCfg);
		//SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

	for ( i = 0; i < IMAGE_BUF_DIM; i++)
	{
		ImageBuf[i].InspHdr.nStation = 0;
		ImageBuf[i].UtInsp.MinWall = 0x3fff;
		memset ( (void *) &ImageBuf[i].UtInsp.SegWallMin[0], 0x3f, 2*N_SEG);
		ImageBuf[i].InspHdr.status[1] |= WALL_INCLUDED;

		for (nSlave=0; nSlave<4; nSlave++)
		{
			for (ci=0; ci<10; ci++)
			{
				ct = ChannelCfg.Ch[nSlave][ci].Type;

				switch ( ct)
				{	/* chnl type */
				case IS_NOTHING:
				default:
					break;

				case IS_WALL:
					pWord = (WORD *) &ImageBuf[i].UtInsp.GateMaxAmp[nSlave*20+ci*2];
					*pWord = 0x3fff;
					break;
				}
			}
		}
	}
}


/**********************************************************************************
* Compute the distance between the leftmost transducer and the rightmost transducer
*/
int CCCM_PAG::GetMaxXSpan(void)
	{
	int nMaxXSpan;
	nMaxXSpan = GetMaxXOffset() - GetMinXOffset();
	return nMaxXSpan;
	}



int CCCM_PAG::GetMaxXOffset(void)
{
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave;
	int i;
	int MaxXOffset = -1000;

	InspState.GetChannelConfig(&ChannelCfg);
	for (nSlave=0; nSlave<MAX_SHOES; nSlave++)
	{
//		InspState.GetChannelConfig(&ChannelCfg);
		//SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

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
}



int CCCM_PAG::GetMinXOffset(void)
{
	CHANNEL_CONFIG2 ChannelCfg;
	int nSlave;
	int i;
	int MinXOffset = 1000;

	InspState.GetChannelConfig(&ChannelCfg);
	for (nSlave=0; nSlave<MAX_SHOES; nSlave++)
		{
//		InspState.GetChannelConfig(&ChannelCfg);
		//SetGetChannelCfg (1 /* GET */, &ChannelCfg, nSlave);

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
}


#if 0
Author: jeh
Date:	10-Jul-2012
Purpose:	Derive a specific class from the general base class ClientConnectionManagement
			Since all mill console programs have to talk to the SysCp, this is a good place
			to begin with children of the base CCM class
Revised:	20-Jan-13
			Rename to CCM_PAG to handle client connection to Phased Array Gui. Now part of 
			Phased Array Master (PAM) code aka PAP - PhasedArrayProcessor
			normally the GUI is the server and the PAP is a client.
#endif

#ifndef CCM_PAG_H_2013
#define CCM_PAG_H_2013
#include "ClientConnectionManagement.h"
#include "PA2WinDlg.h"



// Only the base class cpp file should define I_AM_CCM
#ifdef I_AM_CCM
#define PubExt1	
#else
#define PubExt1	extern
#endif

//PubExt1 CClientConnectionManagement *pCCM[MAX_CLIENTS];

class CCCM_PAG: public CClientConnectionManagement
	{
public:
	CCCM_PAG(int nMyConnection);		// the only constructor
	virtual ~CCCM_PAG(void);
	void UniqueProc(void);
	void CCCM_PAG::ProcessReceivedMessage(void);	// client got a message from the server
	//void CCCM_PAG::WallNx(void);		// set all channels to common wall parameters.


	// copied from InspMsgProcess class
	int GetInstNumber(void)		{	return m_nWhichInstrument;	}
	// Store info about Channel type assignments
//	CHANNEL_INFO m_ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	// get the CHANNEL_CONFIG2 info from InspState.. usually when an MMI command is received
	// Then SetChannelInfo will set the channel types for this instrument
	// 2016-06-27 sets only NcNx type info. Don't know specific chnl type
	void SetChannelInfo(PAP_INST_CHNL_NCNX *pPamInstChnlInfo);
	// return a pointer to the channel info associated with this instrument
//	CHANNEL_INFO *GetChannelInfoPtr(void)		{ return &m_ChannelInfo[0];	}
	int FindWhichSlave(int nChannel);
	int FindSlaveChannel(int nChannel);
	int FindDisplayChannel(int nArray, int nArrayCh);

//	BOOL SendSlaveMsg(int nWhichSlave, ST_LARGE_CMD *pCmd);
//	BOOL SendSlaveMsgToAll(ST_LARGE_CMD *pCmd);

	int GetMaxXSpan(void);
	int GetMaxXOffset(void);
	int GetMinXOffset(void);

	int m_nWhichInstrument;
	int m_nOldInstrument;
	int m_nMsgQty;
	int m_nCmdProcessRunning;
	};

// End of Class declaration
// global variables below


extern CCCM_PAG *pCCM_PAG, *pCCM_PAG_AW;

#endif

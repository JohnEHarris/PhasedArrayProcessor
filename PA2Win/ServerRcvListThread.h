#pragma once

#ifndef SERVER_RCV_LIST_THREAD_H
#define SERVER_RCV_LIST_THREAD_H

#include "PA2WinDlg.h"
#include "../include/pa2struct.h"

#ifdef I_AM_PAP
#include "InspState.h"
class CInstState;
//extern  CInspState InspState;

#else
//#include "Truscan.h"
//#include "TScanDlg.h"
#endif

// CServerRcvListThread

// Prior to 11-12-2012 this class was a child of CServerSocketOwnerThread
// After this date, make it a child of CWinThread
// Since no longer inheriting from CServerSocketOwnerThread, copy the member variable
// from CServerSocketOwnerThread.h to here
//
//class CServerRcvListThread : public CServerSocketOwnerThread
class CServerSocket;
class CServerSocketPA_Master;
class CServerConnectionManagement;

class CServerRcvListThread : public CWinThread
	{
	DECLARE_DYNCREATE(CServerRcvListThread)

protected:
	CServerRcvListThread();           // protected constructor used by dynamic creation
	virtual ~CServerRcvListThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	afx_msg void ProcessRcvList( WPARAM w, LPARAM lParam );
	CHwTimer *m_pElapseTimer;
	int m_nElapseTime;

	CServerConnectionManagement *m_pSCM;		// managing class ptr
	int m_nMyServer;							// which instance of stSCM[] we are
	int m_nClientIndex;							// which one of SSC are we
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects

	void SetClientConnectionIndex( int indx ) { m_nClientIndex = indx; }
	// call these get/set function from ServerSocketOwnerThread ExitInstance to update
	// the values
	ST_SERVERS_CLIENT_CONNECTION * GetpSCC( void )	{ return m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]; }
	void SetpSCC( ST_SERVERS_CLIENT_CONNECTION* p ) { m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] =  p; }
	void NullpSCC(void)								{m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] =  0;}

	void ProcessInstrumentData(IDATA_FROM_HW *pIData);
	void CheckSequences(IDATA_PAP *pIdataPacket);
	
	void MakeFakeDataHead(IDATA_FROM_HW *pData);
	void MakeFakeData(IDATA_FROM_HW *pData);
	//void BuildOutputPacket(IDATA_FROM_HW *pInput);
	void SaveFakeData(CString& s);

	void AddToIdataPacket(CvChannel *pChannel, IDATA_FROM_HW *pIData, int nSendFlag, int nSeq);
	// create a processing class instance for each wall channel
		
	int m_nInspectMode;
	int m_nMotionTime;
	int m_nFakeDataSeqNumber;
	int m_nFrameCount;	// every 16th frame send data to the PAG/Receiver
	int m_nFakeDataCallCount;	// how many times fake data called.

	// Add state variable for Fake Data to determine start seq and start chnl
	/****** FAKE DATA ******/

	int m_FDstartSeq, m_FDstartCh;
	int GetFDstartSeq(void) 	{ return m_FDstartSeq;	}
	int GetFDstartCh(void)		{ return m_FDstartCh;	}
	// increment the starting channel number. Wrap around to next sequence when at end of sequence
	void IncFDstartCh(void);
	void IncFDstartSeq(void);

	/****** FAKE DATA ******/

	/****** INSTRUMENT DATA ******/

	// Instrument data can be in process while fake data is in process during simulation
	// Once testing has ended, fake data will go away.
	int m_Seq, m_Ch;
	//int m_nSendSeqQty;	// nominally 32 sequences
	int m_nResultantChannels;	// SendSeqQty * 8 channels per sequenct
	int m_nStoredChannelCount;
	int GetStartSeq(void) 	{ return m_Seq;	}
	int GetStartCh(void)	{ return m_Ch;		}
	void IncStartCh(void);
	void IncStartSeq(void);

	/****** INSTRUMENT DATA ******/


	//CInspState *GetInspState(void)	{	return &InspState;	}

	//CHANNEL_INFO m_ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	// get the CHANNEL_CONFIG2 info from InspState.. usually when an MMI command is received
	// Then SetChannelInfo will set the channel types for this instrument
	//void SetChannelInfo(void);		// move from CInstMsgProcess
	// return a pointer to the channel info associated with this instrument
	//CHANNEL_INFO *GetChannelInfoPtr(void)		{ return &m_ChannelInfo[0];	}

	// the input packet is received 16 times. The output packet has the same structure but is only generated
	// after 16 input packets have been received. Effectively compressing the input by a factor of 16
	// InputRawDataPacket comes from the instrument
	// InputRawDataPacket *m_pOutputRawDataPacket;		// our class ptr to the packet to send
	IDATA_PAP *m_pIdataPacket;
	int m_IdataInPt;			// insertion point in stPeakChnl PeakChnl
	int m_nFullPacketChnls;		// All the channels of a complete set of unique sequences
	BYTE m_bNiosGlitchCnt;
	int GetIdataPacketIndex(void);
	void SendIdataToPag(GenericPacketHeader *pIdata);


//#ifdef I_AM_PAG
	void ProcessPAM_Data(void *pData);
//#endif


protected:
	DECLARE_MESSAGE_MAP()

	};

#endif


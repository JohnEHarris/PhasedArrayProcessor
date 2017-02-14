#pragma once

/*
1.  Raw Data  |
2.  Raw Data  |
:             |-> m_pOutputRawDataPacket same structure, collected from 16 inputs
16. Raw Data  |

m_pOutputRawDataPacket(ch1), m_pOutputRawDataPacket(ch2), m_pOutputRawDataPacket(ch30.. -> stPeakData(1), stPeakData(2) etc
Final output to PT/Receiver system is 
{
header inf0+
stPeakData Results[179];
} IDATA_PACKET

*/

#ifndef SERVER_RCV_LIST_THREAD_H
#define SERVER_RCV_LIST_THREAD_H

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#include "../include/pa2struct.h"
class CInstState;
extern  CInspState InspState;

#else
#include "Truscan.h"
#include "TScanDlg.h"
#include "ServerRcvListThreadBase.h"		// 11-16-12 jeh
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

class CServerRcvListThread : public CServerRcvListThreadBase
	{
	DECLARE_DYNCREATE(CServerRcvListThread)

protected:
	CServerRcvListThread();           // protected constructor used by dynamic creation
	virtual ~CServerRcvListThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	afx_msg void ProcessRcvList(WPARAM w, LPARAM lParam);
	afx_msg void FlushRcvList(WPARAM w, LPARAM lParam);

#ifdef THIS_IS_SERVICE_APP
//	afx_msg void InitRunningAverage(WPARAM w, LPARAM lParam);
	void ProcessInstrumentData(InputRawDataPacket *pIData);
	
	//void MakeFakeDataHead(SRawDataPacketOld *pData); worked with Yiqing simulator
	//void MakeFakeData(SRawDataPacketOld *pData);
	void MakeFakeDataHead(InputRawDataPacket *pData);
	void MakeFakeData(InputRawDataPacket *pData);
	//int GetSequenceModulo(SRawDataPacketOld *pData);
	//void BuildOutputPacket(SRawDataPacketOld *pRaw);
	void BuildOutputPacket(InputRawDataPacket *pInput);
	void SaveFakeData(CString& s);

	void AddToIdataPacket(CvChannel *pChannel, int nCh, int nSeq, int nSendFlag);
	// create a processing class instance for each wall channel
//	CRunningAverage *m_pRunAvg[MAX_WALL_CHANNELS];
		
	int m_nInspectMode;
	int m_nMotionTime;
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
	int GetStartSeq(void) 	{ return m_Seq;	}
	int GetStartCh(void)	{ return m_Ch;		}
	void IncStartCh(void);
	void IncStartSeq(void);

	/****** INSTRUMENT DATA ******/


	CInspState *GetInspState(void)	{	return &InspState;	}

	CHANNEL_INFO m_ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	// get the CHANNEL_CONFIG2 info from InspState.. usually when an MMI command is received
	// Then SetChannelInfo will set the channel types for this instrument
	void SetChannelInfo(void);		// move from CInstMsgProcess
	// return a pointer to the channel info associated with this instrument
	CHANNEL_INFO *GetChannelInfoPtr(void)		{ return &m_ChannelInfo[0];	}

	// the input packet is received 16 times. The output packet has the same structure but is only generated
	// after 16 input packets have been received. Effectively compressing the input by a factor of 16
	// InputRawDataPacket comes from the instrument
	// InputRawDataPacket *m_pOutputRawDataPacket;		// our class ptr to the packet to send
	IDATA_PACKET *m_pIdataPacket;
	int m_IdataInPt;			// insertion point in stPeakData Results
	int GetIdataPacketIndex(void);
	void SendIdataToPag(GenericPacketHeader *pIdata);

#endif

#ifdef _I_AM_PAG
	void ProcessPAM_Data(void *pData);
#endif

	// 11-12-2012
		
	CHwTimer *m_pElapseTimer;
	int m_nElapseTime;

protected:
	DECLARE_MESSAGE_MAP()

	};

#endif


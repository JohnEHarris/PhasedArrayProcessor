#pragma once

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
	void ProcessInstrumentData(void *pData);
	
	//void MakeFakeDataHead(SRawDataPacketOld *pData); worked with Yiqing simulator
	//void MakeFakeData(SRawDataPacketOld *pData);
	void MakeFakeDataHead(SInputRawDataPacket *pData);
	void MakeFakeData(SInputRawDataPacket *pData);
	int GetSequenceModulo(SRawDataPacketOld *pData);
	//void BuildOutputPacket(SRawDataPacketOld *pRaw);
	void BuildOutputPacket(SInputRawDataPacket *pRaw);
	void SaveFakeData(CString& s);
	// create a processing class instance for each wall channel
//	CRunningAverage *m_pRunAvg[MAX_WALL_CHANNELS];
		
	int m_nInspectMode;
	int m_nMotionTime;
	//int m_nFakeDataSeqNumber;
	int m_nFrameCount;	// every 16th frame send data to the PAG/Receiver
	int m_nFakeDataCallCount;	// how many times fake data called.

	CInspState *GetInspState(void)	{	return &InspState;	}

	CHANNEL_INFO m_ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	// get the CHANNEL_CONFIG2 info from InspState.. usually when an MMI command is received
	// Then SetChannelInfo will set the channel types for this instrument
	void SetChannelInfo(void);		// move from CInstMsgProcess
	// return a pointer to the channel info associated with this instrument
	CHANNEL_INFO *GetChannelInfoPtr(void)		{ return &m_ChannelInfo[0];	}

	// the input packet is received 16 times. The output packet has the same structure but is only generated
	// after 16 input packets have been received. Effectively compressing the input by a factor of 16
	SInputRawDataPacket *m_pOutputRawDataPacket;		// our class ptr to the packet to send

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

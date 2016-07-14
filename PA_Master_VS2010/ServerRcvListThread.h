#pragma once

#ifndef SERVER_RCV_LIST_THREAD_H
#define SERVER_RCV_LIST_THREAD_H

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#include "../include/cfg100.h"
//class CRunningAverage;
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

#ifdef THIS_IS_SERVICE_APP
//	afx_msg void InitRunningAverage(WPARAM w, LPARAM lParam);
	void ProcessInstrumentData(void *pData);
	void MakeFakeDataHead(SRawDataPacket *pData);
	void MakeFakeData(SRawDataPacket *pData);
	int GetSequenceModulo(SRawDataPacket *pData);
	void BuildOutputPacket(SRawDataPacket *pRaw);
	void SaveFakeData(CString& s);
	// create a processing class instance for each wall channel
//	CRunningAverage *m_pRunAvg[MAX_WALL_CHANNELS];
		
	int m_nInspectMode;
	int m_nMotionTime;
	int m_nFakeDataSeqNumber;
	int m_nFrameCount;	// every 16th frame send data to the PAG/Receiver
	int m_nFakeDataCallCount;	// how many times fake data called.

	CInspState *GetInspState(void)	{	return &InspState;	}

	CHANNEL_INFO m_ChannelInfo[MAX_CHANNEL_PER_INSTRUMENT];
	// get the CHANNEL_CONFIG2 info from InspState.. usually when an MMI command is received
	// Then SetChannelInfo will set the channel types for this instrument
	void SetChannelInfo(void);		// move from CInstMsgProcess
	// return a pointer to the channel info associated with this instrument
	CHANNEL_INFO *GetChannelInfoPtr(void)		{ return &m_ChannelInfo[0];	}

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


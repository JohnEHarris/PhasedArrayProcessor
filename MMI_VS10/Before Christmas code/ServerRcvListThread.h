#pragma once

#ifndef SERVER_RCV_LIST_THREAD_H
#define SERVER_RCV_LIST_THREAD_H

#include "ServerRcvListThreadBase.h"		// 11-16-12 jeh

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#include "../include/cfg100.h"
class CRunningAverage;

#else
#include "TScanDlg.h"
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
	afx_msg void InitRunningAverage(WPARAM w, LPARAM lParam);
	void ProcessInstrumentData(void *pData);
	
	// create a processing class instance for each wall channel
	CRunningAverage *m_pRunAvg[MAX_WALL_CHANNELS];
#endif

	// 11-12-2012
	CServerConnectionManagement *m_pMySCM;		// managing class ptr
	int m_nMyServer;							// which instance of stSCM[] we are
	int m_nThreadIndex;							// which one of SSC are we
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects

	SOCKET m_hConnectionSocket;


protected:
	CServerSocket m_ConnectionSocket;				// server's connection to the client .. on stack
	DECLARE_MESSAGE_MAP()

	};

#endif


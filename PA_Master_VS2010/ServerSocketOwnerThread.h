// CServerSocketOwnerThread.cpp : implementation file
//
/*
Author:		JEH
Date:		20-Aug-2012
Purpose:	Provide a thread to manage the client connection between the MMI and a client machine 

Revised:
*/


#pragma once

// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#else
#include "tscandlg.h"
class CServerSocketPA_Master;
#endif

class CServerSocket;

class CServerConnectionManagement;

class CServerSocketOwnerThread : public CWinThread
{
	DECLARE_DYNCREATE(CServerSocketOwnerThread)

protected:
	CServerSocketOwnerThread();           // protected constructor used by dynamic creation
	virtual ~CServerSocketOwnerThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//virtual void Run(DWORD, LPTSTR *lP);
	//afx_msg void InitCommunicationThread(WPARAM w, LPARAM lParam);
	//afx_msg void KillCommunicationThread(WPARAM w, LPARAM lParam);
	afx_msg void TransmitPackets(WPARAM w, LPARAM lParam);
	afx_msg void Exit2(WPARAM w, LPARAM lParam);

	CServerConnectionManagement *m_pMySCM;		// managing class ptr
	int m_nMyServer;							// which instance of stSCM[] we are
	int m_nThreadIndex;							// which one of SSC are we
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects

	SOCKET m_hConnectionSocket;
	CServerSocket m_ConnectionSocket;			// server's connection to the client .. on stack

	int nDebug;
	class CHwTimer *m_pHwTimer;
	int m_nConfigMsgQty;						// how many command in a config file download

protected:


	DECLARE_MESSAGE_MAP()
};



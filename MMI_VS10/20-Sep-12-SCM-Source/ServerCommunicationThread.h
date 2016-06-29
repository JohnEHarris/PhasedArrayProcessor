// CServerCommunicationThread.cpp : implementation file
//
/*
Author:		JEH
Date:		20-Aug-2012
Purpose:	Provide a thread to manage the client connection between the MMI and a client machine 

Revised:
*/


#pragma once
#include "tscandlg.h"
//#include "ServerConnectionManagement.h"  jeh jeh

// CServerCommunicationThread

class CServerSocket;
class CServerSocketPA_Master;
class CServerConnectionManagement;

class CServerCommunicationThread : public CWinThread
{
	DECLARE_DYNCREATE(CServerCommunicationThread)

protected:
	CServerCommunicationThread();           // protected constructor used by dynamic creation
	virtual ~CServerCommunicationThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//virtual void Run(DWORD, LPTSTR *lP);
	afx_msg void InitCommunicationThread(WPARAM w, LPARAM lParam);
	afx_msg void KillCommunicationThread(WPARAM w, LPARAM lParam);
	afx_msg void TransmitPackets(WPARAM w, LPARAM lParam);

	CServerConnectionManagement *m_pMySCM;		// managing class ptr
	int m_nMyServer;							// which instance of stSCM[] we are
	int m_nThreadIndex;							// which one of SSC are we
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects

	SOCKET m_hConnectionSocket;

protected:
	CServerSocket m_ConnectionSocket;				// server's connection to the client
	CServerSocketPA_Master m_ConnectionSocketPAM;	// server's connection to the client- Phased Array Master
	DECLARE_MESSAGE_MAP()
};



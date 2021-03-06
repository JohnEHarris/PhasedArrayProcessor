// CServerSocketOwnerThread.cpp : implementation file
//
/*
Author:		JEH
Date:		20-Aug-2012
Purpose:	Provide a thread to manage the client connection between the MMI and a client machine 

Revised:
*/


#pragma once

// I_AM_PAP is defined in the PAP project under C++ | Preprocessor Definitions 


#include "PA2WinDlg.h"
//#else
//#include "tscandlg.h"
//class CServerSocketPA_Master;
//#endif

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
	void CommandLogMsg(ST_SMALL_CMD *pCmd);
	void MsgPrint(ST_SMALL_CMD *pCmd, char *msg);
	void MsgPrintLarge(ST_LARGE_CMD *pCmd, char *msg);
	afx_msg void FlushCmdQueue(WPARAM w, LPARAM lParam);

	//afx_msg void Exit2(WPARAM w, LPARAM lParam);
	afx_msg void KillServerSocket(WPARAM w, LPARAM lParam);
	afx_msg void KillServerSocketOwner(WPARAM w, LPARAM lParam);
	afx_msg void Hello(WPARAM w, LPARAM lParam);
	CServerConnectionManagement *m_pSCM;		// managing class ptr
	int m_nMyServer;							// which instance of stSCM[] we are
	int m_nClientIndex;							// which one of SSC are we
	void SetClientConnectionIndex( int indx ) { m_nClientIndex = indx; }
	afx_msg void AttachSocket(WPARAM w, LPARAM lParam);
	// call these get/set function from ServerSocketOwnerThread ExitInstance to update
	// the values
	ST_SERVERS_CLIENT_CONNECTION * GetpSCC( void )	{ return m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex]; }
	void SetpSCC( ST_SERVERS_CLIENT_CONNECTION* p ) { m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] =  p; }
	void NullpSCC(void)								{m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] =  0;}


	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects

	SOCKET m_hConnectionSocket;
	//CServerSocket m_ConnectionSocket;			// server's connection to the client .. on stack
	//CServerSocket *m_pConnectionSocket;			// server's connection to the client .. on stack 2016-08-12 new

	int nDebug;
	class CHwTimer *m_pHwTimer;
	int m_nConfigMsgQty;						// how many command in a config file download
	BYTE m_bSmallCmdSent;		// count small cmds. after every 32nd, sleep 10
	BYTE m_bLargeCmdSent;		// count large cmds. after every 8th, sleep 10
	BYTE m_bPulserCmdSent;		// count large cmds. after every 8th, sleep 10

protected:


	DECLARE_MESSAGE_MAP()
};



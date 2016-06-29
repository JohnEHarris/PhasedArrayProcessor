#pragma once

#include "ServerConnectionManagement.h"

// these defines copied from tscandlg.h
#define WM_USER_ADD_PA_MASTER						WM_USER+0x213
#define WM_USER_DELETE_PA_MASTER					WM_USER+0x214
#define WM_USER_INIT_LISTNER_THREAD					WM_USER+0x215
#define WM_USER_STOP_LISTNER_THREAD					WM_USER+0x216
// CServerListenThread .. created by wizard "Add Class to Project"

class CServerSocket;

class CServerListenThread : public CWinThread
{
	DECLARE_DYNCREATE(CServerListenThread)

protected:
	CServerListenThread();           // protected constructor used by dynamic creation
	virtual ~CServerListenThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//virtual int Run(void);
	// pass ptr to the controlling class instance of SCM in lParam
	afx_msg void InitListnerThread(WPARAM w, LPARAM lParam);
	afx_msg void StopListnerThread(WPARAM w, LPARAM lParam);
	afx_msg void DoNothing(WPARAM w, LPARAM lParam);


	CServerConnectionManagement *m_pMySCM;		// managing class ptr
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	

protected:

	CServerSocket *m_pListenSocket;

	DECLARE_MESSAGE_MAP()
};



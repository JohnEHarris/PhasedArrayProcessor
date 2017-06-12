#pragma once


#include "ClientConnectionManagement.h"


// CCmdProcessThread
// largely copied from CClientCommunicationThread

class CCmdProcessThread : public CWinThread
{
	DECLARE_DYNCREATE(CCmdProcessThread)

protected:
	CCmdProcessThread();           // protected constructor used by dynamic creation
	virtual ~CCmdProcessThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CClientConnectionManagement *m_pMyCCM;
	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;
	void DebugMsg(CString s);
	void ProcessReceivedMessage(WPARAM, LPARAM);
	afx_msg void KillCmdProcess( WPARAM w, LPARAM lParam );

protected:

	//ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;

	DECLARE_MESSAGE_MAP()
};



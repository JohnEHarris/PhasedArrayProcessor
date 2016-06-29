/** Copyright (C) 2008 Tuboscope, a National Oilwell Varco Company. All rights reserved  **/
#if !defined(AFX_ClientCommunicationThread_H__594F1B75_5F26_4DDD_A2CC_3BCEC9657A43__INCLUDED_)
#define AFX_ClientCommunicationThread_H__594F1B75_5F26_4DDD_A2CC_3BCEC9657A43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



// ClientCommunicationThread.h : header file
//

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"

/////////////////////////////////////////////////////////////////////////////
// CClientCommunicationThread thread

class CClientCommunicationThread : public CWinThread
{
	DECLARE_DYNCREATE(CClientCommunicationThread)
protected:
	CClientCommunicationThread();           // protected constructor used by dynamic creation
// Attributes
public:

// Operations
public:
	int m_nMyRole;		//0= undefined, 1= Receiver, 2= Sender
	CString m_sMyRole;	// Receiver, Sender, I dont know

	CClientConnectionManagement *m_pMyCCM;
	int m_nWinVersion;
	int m_nConnectRetryTick;
	int m_nThreadIdOld;
	int m_nInXmitLoop;
	int m_uXmitLoopCount;
	int m_uLastPacketsReceived;	// how many packets received in the past
	int m_nConnectionRestartCounter;

	CString GetRole(void)	{ return m_sMyRole;	}

	// The trick that makes this all work, a CClientSocket
    // ClientSocket is an AsyncSocket
	// In particular, it receives data 'automatically' with the OnReceive
	// procedure of the class
	CClientSocket* m_pSocket;
	SOCKET m_hSocket;
	int m_nTick;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientCommunicationThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL


	// pass ptr to the controlling class instance of CCM in lParam
	afx_msg void InitTcpThread(WPARAM w, LPARAM lParam);
	afx_msg void RestartTcpComDlg(WPARAM w, LPARAM lParam);
	afx_msg void TransmitPackets(WPARAM, LPARAM);
	afx_msg void OnTimer(WPARAM w, LPARAM lParam);

	void StartTCPCommunication();
	void DebugMsg(CString s);

// Implementation
protected:
	virtual ~CClientCommunicationThread();

	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;

	// Generated message map functions
	//{{AFX_MSG(CClientCommunicationThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ClientCommunicationThread_H__594F1B75_5F26_4DDD_A2CC_3BCEC9657A43__INCLUDED_)

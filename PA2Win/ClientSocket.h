#if !defined(AFX_CLIENTSOCKET_H__BB440104_6E91_4C5D_B00D_80E1FDB95CA4__INCLUDED_)
#define AFX_CLIENTSOCKET_H__BB440104_6E91_4C5D_B00D_80E1FDB95CA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//
//#include "AmalogCommand.h"
//#include "AmalogCommandDlg.h"
#include "afxwin.h"
#include "afxmt.h"
#include "afxSOCK.h"
// ClientConnectionManagement.h included in AmalogCommandDlg.h
#include "ClientConnectionManagement.h"		// jeh 05-Jun-12

/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target

class CClientSocket : public CAsyncSocket //CSocket
{
	DECLARE_DYNAMIC(CClientSocket);
// Attributes
public:

	//
	int m_nChooseYourOnReceive;		// 0 if legacy OnReceive0, 1 if new, then OnReceive1
	CClientConnectionManagement *m_pCCM; 
	int m_nOwningThreadId;			// debugging
	int m_nAsyncSocketCnt;			// debugging

	CCmdFifo *m_pFifo;		// In ClientConnectionManagement get PAG commands. Here gets instrument data
							// created in CServerSocket::CServerSocket(CServerConnectionManagement *pSCM)
							// deleted in CServerSocket::~CServerSocket()
	CHwTimer *m_pElapseTimer;	// created in CServerSocket::CServerSocket()
								// deleted in CServerSocket::~CServerSocket()
	int m_nElapseTime;

	//CMC_SysCPTestClientDlg *m_pMainDlg;		// the main dialog the application created
// Operations
public:
//	CClientSocket(CPtrList *,CPtrList *, CPtrList *,CPtrList *, CSemaphore *, CSemaphore *);	// legacy constructor
	CClientSocket( CClientConnectionManagement *pCCM	);	// new constructor
	virtual ~CClientSocket();
	void DebugInMessage( CString s );
	void DebugOutMessage( CString s );

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	void OnReceive0(int nErrorCode);	//legacy OnReceive
	void OnReceive1(int nErrorCode);	// new added by jeh for ClientConnectionManagement class operation
// Implementation
protected:
	void MyMessageBox(CString s);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__BB440104_6E91_4C5D_B00D_80E1FDB95CA4__INCLUDED_)

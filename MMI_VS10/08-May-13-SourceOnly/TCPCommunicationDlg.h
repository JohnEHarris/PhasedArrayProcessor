/** Copyright (C) 2008 Tuboscope, a National Oilwell Varco Company. All rights reserved  **/
#if !defined(AFX_CTCPCommunicationDlg_H__26B431DE_57EA_4EF6_9E7C_66E2C61CBBD6__INCLUDED_)
#define AFX_CTCPCommunicationDlg_H__26B431DE_57EA_4EF6_9E7C_66E2C61CBBD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTCPCommunicationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTCPCommunicationDlg dialog


class CTCPCommunicationDlg : public CDialog
{
// Construction
public:
	void StartTCPCommunication();
	CTCPCommunicationDlg(CWnd* pParent = NULL, LPVOID lpParam = NULL );   // standard constructor

	// Note that we now have an array of ptrs to the instances of the dialogs
	// 10-June-05 jeh
//	static CTCPCommunicationDlg *m_pDlg[TCP_SOCKET_QTY];		// ptr to this dialog..always use
									// the name m_pDlg by convention
	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;		// POINT back to struct in main dlg AdpMMIDlg.


	// The trick that makes this all work, a CClientSocket
    // ClientSocket is an AsyncSocket
	// In particular, it receives data 'automatically' with the OnReceive
	// procedure of the class
	CClientSocket* m_pSocket;
	SOCKET m_hSocket;
	int m_nMyRole;			//0= undefined, 1= Receiver, 2= Sender
	CString m_sMyRole;
	void SetMyRole(int n);

	CClientConnectionManagement *m_pLocalCCM;		// point to the top level manager in control

	void StopTimer();
	void StartTimer();
	void SetDisConnected(void);
	void DebugMsg(CString s);

	CString ResolveHostIP(CString hostname);	// stolen from ACP code
	CString GetIPv4(CString sComputerName);
	CString GetIPv4(void);

	// This is OnCancel dummied up to work with .NET COMPILER 
	afx_msg LRESULT VS05_OnCancel(WPARAM, LPARAM);	// this foolishness required to go from Visual Studio to Visual NET
	afx_msg LRESULT VS05_StartTCPCommunication(WPARAM, LPARAM);	// this foolishness required to go from Visual Studio to Visual NET
	afx_msg LRESULT Close(WPARAM, LPARAM);
	afx_msg LRESULT TransmitPackets(WPARAM, LPARAM);
// Dialog Data
	//{{AFX_DATA(CTCPCommunicationDlg)
	enum { IDD = IDD_INVISIBLE_COM_DLG };	// changed using class wizard in vs2010 to gen new dlg
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTCPCommunicationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_uTimerHandle;
	UINT m_uTimerTick;
	int nConnectRetryTick;
	int m_StartAdpQty, m_StartSocketQty;	// debug only, how many times has this happened
	int m_nMyIndx;							// which thread/socket am I
	UINT m_uLastPacketsReceived;			// number of packet last time we looked
	int m_nInXmitLoop;		// avoid recursion int transmit packet operation
	UINT m_uXmitLoopCount;
	int m_nThreadIdOld;
	int m_nConnectionRestartCounter;


	// Generated message map functions
	//{{AFX_MSG(CTCPCommunicationDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTCPCommunicationDlg_H__26B431DE_57EA_4EF6_9E7C_66E2C61CBBD6__INCLUDED_)

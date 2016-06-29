#if !defined(AFX_CATCH_H__DD48F0C1_C908_11D3_9B49_00A0C91FC401__INCLUDED_)
#define AFX_CATCH_H__DD48F0C1_C908_11D3_9B49_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Catch.h : header file comment out
//

/////////////////////////////////////////////////////////////////////////////
// CCatch dialog

#include "TcpThreadRxList.h"

class CCatch : public CDialog
{
// Construction
public:
	int m_nAmaSonoSel;
	CCatch(CWnd* pParent = NULL);   // standard constructor
	static CCatch *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention

	CTcpThreadRxList *m_pTcpThreadRxList;

	void KillMe();					// another way to die

	// Wrappers to convert VC6 to Visual Studio 2010
	afx_msg LRESULT VS10_OnCancel(WPARAM, LPARAM);

// Dialog Data
	//{{AFX_DATA(CCatch)
	enum { IDD = IDD_CATCH };
	CComboBox	m_cbPlc;
	CScrollBar	m_sbChnl;
	CEdit	m_ceStdOut;
	CEdit	m_ceFromInst;
	int		m_nOd;
	int		m_nWall;
	int		m_nChnl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nPkHold;
	BOOL SendMsg(int MsgNum);
	//BOOL SendUdpMsg(int MsgNum, void *p);

	// Generated message map functions
	//{{AFX_MSG(CCatch)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMsg1();
	afx_msg void OnMsg2();
	afx_msg void OnStats();
	virtual void OnOK();
	afx_msg void OnReset();
	afx_msg void OnPkhold();
	afx_msg void OnUdpMsg();
	afx_msg void OnOdBn();
	afx_msg void OnIdBn();
	afx_msg void OnLongBn();
	afx_msg void OnObq2Bn();
	afx_msg void OnWallBn();
	afx_msg void OnTranBn();
	afx_msg void OnMsg3();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTstIpx();
	afx_msg void OnObq1Bn();
	afx_msg void OnObq3Bn();
	afx_msg void OnBtnPlc();
	afx_msg void OnBtnRev();
	afx_msg void OnSelchangeComboPlc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT m_uIpxTimer;
	void PrintMsgRun(I_MSG_RUN *pMsgRun);
	void PrintMsg02(I_MSG_CAL *pMsg02);
	void PrintMsg03(I_MSG_PKT *pMsg03);
	void PrintMsgPlc(I_MSG_PLC *pMsg);
	void PrintBoardRevision();
	void SendMsgTextOut(int nMsg);

	BOOL m_bPrintRevision;

	int  m_nPlcSel;

	int  m_nMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CATCH_H__DD48F0C1_C908_11D3_9B49_00A0C91FC401__INCLUDED_)

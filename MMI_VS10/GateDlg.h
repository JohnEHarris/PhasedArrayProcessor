#if !defined(AFX_GATEDLG_H__06736863_963E_11D5_AC77_00A0C91FC401__INCLUDED_)
#define AFX_GATEDLG_H__06736863_963E_11D5_AC77_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGateDlg dialog

class CGateDlg : public CDialog
{
// Construction
public:
	CTcpThreadRxList *m_pTcpThreadRxList;

	void DrawThold(int nGraph);
	BOOL SendMsg(int MsgNum);
	void KillMe();
	CGateDlg(CWnd* pParent = NULL);   // standard constructor
	static CGateDlg *m_pDlg;			// ptr to this dialog..always use
	CONFIG_REC * m_pUndo;
	void UndoGateSettings();
								// the name m_pDlg by convention
	virtual BOOL OnInitDialog();
	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(CGateDlg)
	enum { IDD = IDD_GATES };
	CComboBox	m_CMlam;
	CComboBox	m_CMTrigger;
	CScrollBar	m_sbGATEblanking;
	CScrollBar	m_sbGATErange;
	CScrollBar	m_sbGATElevel;
	CScrollBar	m_sbGATEdelay;
	CString	m_sDelay;
	CString	m_sBlanking;
	CString	m_sLevel;
	CString	m_sRange;
	BOOL	m_bGate1Inv;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	void ShowDlgTitle();

	// Generated message map functions
	//{{AFX_MSG(CGateDlg)
	virtual void OnCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGatesUndo();
	afx_msg void Ongate1();
	afx_msg void Ongate2();
	afx_msg void OnSelchangeCOMBOTrigger();
	afx_msg void OnG1Invrt();
	afx_msg void OnSelchangeCOMBOLam();
	afx_msg void Ongate3();
	afx_msg void Ongate4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void Save_Pos();
	virtual ~CGateDlg();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATEDLG_H__06736863_963E_11D5_AC77_00A0C91FC401__INCLUDED_)

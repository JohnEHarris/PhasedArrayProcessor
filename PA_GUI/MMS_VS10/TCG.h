#if !defined(AFX_TCG_H__4046B91A_D741_4D54_82C1_25B25C39E46B__INCLUDED_)
#define AFX_TCG_H__4046B91A_D741_4D54_82C1_25B25C39E46B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TCG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTCG dialog

class CTCG : public CDialog
{
// Construction
public:
	CTCG(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static CTCG *m_pDlg;			// ptr to this dialog..always use
	CONFIG_REC * m_pUndo;
								// the name m_pDlg by convention
	virtual BOOL OnInitDialog();


// Dialog Data
	//{{AFX_DATA(CTCG)
	enum { IDD = IDD_TCGset };
	CScrollBar	m_SBB;
	CScrollBar	m_SBA;
	CComboBox	m_CBTrgr;
	CComboBox	m_CBStep;
	CComboBox	m_CBTGain;
	CString	m_SAArg;
	CString	m_SBArg;
	CString	m_SGdb;
	CString	m_SGint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTCG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTCG)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMove(int x, int y);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChOn();
	afx_msg void OnCpte();
	afx_msg void OnSysOn();
	afx_msg void OnSelchangeCBStep();
	afx_msg void OnSelchangeCBTCGGain();
	afx_msg void OnSelchangeCBTrgr();
	afx_msg void OnCkTCG();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual ~CTCG();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCG_H__4046B91A_D741_4D54_82C1_25B25C39E46B__INCLUDED_)

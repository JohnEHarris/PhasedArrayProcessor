#if !defined(AFX_OSCPE_H__13EDCF02_6754_4AC7_9D18_7D08D1530A02__INCLUDED_)
#define AFX_OSCPE_H__13EDCF02_6754_4AC7_9D18_7D08D1530A02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Oscpe.h : header file
//


static BYTE m_CmdTbl[20];

/////////////////////////////////////////////////////////////////////////////
// COscpe dialog

class COscpe : public CDialog
{
// Construction
public:
	COscpe(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static COscpe *m_pDlg;			// ptr to this dialog..always use
	OSCOPE_REC * m_pUndo;
								// the name m_pDlg by convention
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();


// Dialog Data
	//{{AFX_DATA(COscpe)
	enum { IDD = IDD_OSCPE };
	CComboBox	m_CBTrc1;
	CScrollBar	m_SBLvl;
	CComboBox	m_CBTrgr;
	CComboBox	m_CBTrc2;
	CString	m_SLvl;
	int		m_nT2gatesync;
	int		m_nT2CmdIndx;
	int		m_nSeqPt;
	BOOL	m_n104Debug;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COscpe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COscpe)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeCBTrc2();
	afx_msg void OnSelchangeCBTrgr();
	afx_msg void OnRbT2gate();
	afx_msg void OnRbT2sync();
	afx_msg void OnRbSeq1();
	afx_msg void OnRbSeq2();
	afx_msg void OnOscopeUndo();
	afx_msg void On104Debug();
	afx_msg void OnOscopeHelp();
	afx_msg void OnSelchangeCBTrc1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual ~COscpe();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OSCPE_H__13EDCF02_6754_4AC7_9D18_7D08D1530A02__INCLUDED_)

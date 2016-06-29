#if !defined(AFX_NC_H__FB6CF533_64B3_11D4_AB5C_00A0C91FC401__INCLUDED_)
#define AFX_NC_H__FB6CF533_64B3_11D4_AB5C_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Nc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNc dialog

class CNc : public CDialog
{
// Construction
public:
	void UpdateConfigRec();
	void GetConfigRec();
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static CNc *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	CNc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNc)
	enum { IDD = IDD_NCNX };
	CScrollBar	m_sbLamod;
	CScrollBar	m_sbLamid;
	CScrollBar	m_sbQ2id;
	CScrollBar	m_sbWallNx;
	CScrollBar	m_sbTol;
	CScrollBar	m_sbTod;
	CScrollBar	m_sbTid;
	CScrollBar	m_sbQ3od;
	CScrollBar	m_sbQ3id;
	CScrollBar	m_sbQ2od;
	CScrollBar	m_sbQ1od;
	CScrollBar	m_sbQ1id;
	CScrollBar	m_sbLod;
	CScrollBar	m_sbLid;
	int		m_nLid;
	int		m_nLod;
	int		m_nQ1id;
	int		m_nQ1od;
	int		m_nQ2id;
	int		m_nQ2od;
	int		m_nQ3id;
	int		m_nQ3od;
	int		m_nTid;
	int		m_nTod;
	int		m_nTol;
	int		m_nWallNx;
	int		m_nLamid;
	int		m_nLamod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNc)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSaveConfig();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NC_H__FB6CF533_64B3_11D4_AB5C_00A0C91FC401__INCLUDED_)

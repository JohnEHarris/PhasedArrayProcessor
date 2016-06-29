#if !defined(AFX_THOLDS_H__840B9081_D035_11D3_9B55_00A0C91FC401__INCLUDED_)
#define AFX_THOLDS_H__840B9081_D035_11D3_9B55_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tholds.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTholds dialog

class CTholds : public CDialog
{
// Construction
public:
	void DrawThold (int Thold);
	CTholds(CWnd* pParent = NULL);   // standard constructor
	static CTholds *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	void KillMe();					// another way to die

// Dialog Data
	//{{AFX_DATA(CTholds)
	enum { IDD = IDD_THOLDS };
	CScrollBar	m_sbLamod;
	CScrollBar	m_sbLamid;
	CScrollBar	m_sbQ3od;
	CScrollBar	m_sbQ3id;
	CScrollBar	m_sbQ2od;
	CScrollBar	m_sbQ2id;
	CScrollBar	m_sbQ1od;
	CScrollBar	m_sbQ1id;
	CScrollBar	m_sbWallMin;
	CScrollBar	m_sbWallMax;
	CScrollBar	m_sbTod;
	CScrollBar	m_sbTid;
	CScrollBar	m_sbLod;
	CScrollBar	m_sbLid;
	int		m_nWallMax;
	int		m_nWallMin;
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
	int		m_nLamid;
	int		m_nLamod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTholds)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL SendMsg(int MsgNum);

	// Generated message map functions
	//{{AFX_MSG(CTholds)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnQuestionable();
	afx_msg void OnReject();
	afx_msg void OnSaveConfig();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THOLDS_H__840B9081_D035_11D3_9B55_00A0C91FC401__INCLUDED_)

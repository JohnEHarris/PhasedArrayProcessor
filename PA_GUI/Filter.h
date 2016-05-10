#if !defined(AFX_FILTER_H__59662371_EACA_11D3_9B6F_00A0C91FC401__INCLUDED_)
#define AFX_FILTER_H__59662371_EACA_11D3_9B6F_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Filter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilter dialog

class CFilter : public CDialog
{
// Construction
public:
	int m_nAmaSonoSel;
	CFilter(CWnd* pParent = NULL);   // standard constructor
	static CFilter *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	void KillMe();					// another way to die

// Dialog Data
	//{{AFX_DATA(CFilter)
	enum { IDD = IDD_FILTER_NOISE };
	CScrollBar	m_sbHiQ;
	CScrollBar	m_sbNoise;
	CScrollBar	m_sbOD;
	CScrollBar	m_sbID;
	int		m_nIDFilter;
	int		m_nODFilter;
	BOOL	m_ckNoise;
	int		m_nNoise;
	int		m_nHiQFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadConfigRec();
	void UpdateScrollInfo();
	void UpdateConfigRec();
	BOOL SendMsg(int MsgNum);

	// Generated message map functions
	//{{AFX_MSG(CFilter)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAmaSonoSel();
	afx_msg void OnNoiseCk();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSaveConfig();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTER_H__59662371_EACA_11D3_9B6F_00A0C91FC401__INCLUDED_)

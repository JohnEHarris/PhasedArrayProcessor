#if !defined(AFX_CHNLRANGEDLG_H__9B6EE3E6_07CD_4CE0_A591_89859DA1076B__INCLUDED_)
#define AFX_CHNLRANGEDLG_H__9B6EE3E6_07CD_4CE0_A591_89859DA1076B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChnlRangeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChnlRangeDlg dialog

class CChnlRangeDlg : public CDialog
{
// Construction
public:
	CChnlRangeDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL GetChannelRange();
	void ResetChannelRange();
	BOOL IsStringNumber(CString str);
	BOOL m_bChnlSelect[MEM_MAX_CHANNEL];

// Dialog Data
	//{{AFX_DATA(CChnlRangeDlg)
	enum { IDD = IDD_CHNL_RANGE };
	CEdit	m_editChnlRange;
	BOOL	m_bLong;
	BOOL	m_bTran;
	BOOL	m_bObq1;
	BOOL	m_bObq2;
	BOOL	m_bObq3;
	BOOL	m_bWall;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChnlRangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChnlRangeDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnLong();
	afx_msg void OnObq1();
	afx_msg void OnObq2();
	afx_msg void OnObq3();
	afx_msg void OnTran();
	afx_msg void OnWall();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHNLRANGEDLG_H__9B6EE3E6_07CD_4CE0_A591_89859DA1076B__INCLUDED_)

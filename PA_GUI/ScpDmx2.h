#if !defined(AFX_SCPDMX2_H__CB74CD8C_F954_4926_9258_5A4C478A240C__INCLUDED_)
#define AFX_SCPDMX2_H__CB74CD8C_F954_4926_9258_5A4C478A240C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScpDmx2.h : header file
//
// Pop up options for setting default values for trim gains on scope demux board
//

/////////////////////////////////////////////////////////////////////////////
// CScpDmx2 dialog

class CScpDmx2 : public CDialog
{
// Construction
public:
	void ChnlNullOpt(CString &s);
	void GainTrimOpt(CString &s);
	BOOL SendMsg(int MsgNum);
	CScpDmx2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScpDmx2)
	enum { IDD = IDD_SCP_DMX_OPT };
	CEdit	m_ceInstruct;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScpDmx2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScpDmx2)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopy10();
	afx_msg void OnBnCopy40();
	afx_msg void OnBnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCPDMX2_H__CB74CD8C_F954_4926_9258_5A4C478A240C__INCLUDED_)

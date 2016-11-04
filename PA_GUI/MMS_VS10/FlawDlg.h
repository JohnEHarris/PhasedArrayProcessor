#if !defined(AFX_FLAWDLG_H__674C5445_ACFF_40EA_8512_AA5B6DCD9DC8__INCLUDED_)
#define AFX_FLAWDLG_H__674C5445_ACFF_40EA_8512_AA5B6DCD9DC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlawDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlawDlg dialog

class CFlawDlg : public CDialog
{
// Construction
public:
	CFlawDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlawDlg)
	enum { IDD = IDD_DIALOG1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlawDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlawDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLAWDLG_H__674C5445_ACFF_40EA_8512_AA5B6DCD9DC8__INCLUDED_)

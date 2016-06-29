#if !defined(AFX_ODDLG_H__B6BFD8D3_4C5E_11D4_AB3F_00A0C91FC401__INCLUDED_)
#define AFX_ODDLG_H__B6BFD8D3_4C5E_11D4_AB3F_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OdDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COdDlg dialog

class COdDlg : public CDialog
{
// Construction
public:
	COdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COdDlg)
	enum { IDD = IDD_OD_DLG };
	CString	m_szNewOd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COdDlg)
	afx_msg void OnChangeNewOd();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODDLG_H__B6BFD8D3_4C5E_11D4_AB3F_00A0C91FC401__INCLUDED_)

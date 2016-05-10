#if !defined(AFX_FREPORT_H__BCACF998_34CC_42BA_AB45_229421D953C3__INCLUDED_)
#define AFX_FREPORT_H__BCACF998_34CC_42BA_AB45_229421D953C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FReport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFReport dialog

class CFReport : public CDialog
{
// Construction
public:
	CFReport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFReport)
	enum { IDD = IDC_LFreport };
	CListBox	m_sText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFReport)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FREPORT_H__BCACF998_34CC_42BA_AB45_229421D953C3__INCLUDED_)

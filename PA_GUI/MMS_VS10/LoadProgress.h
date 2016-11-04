#if !defined(AFX_LOADPROGRESS_H__8FF7BE9F_6A9F_4762_A63C_40FF087668AD__INCLUDED_)
#define AFX_LOADPROGRESS_H__8FF7BE9F_6A9F_4762_A63C_40FF087668AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadProgress dialog

class CLoadProgress : public CDialog
{
// Construction
public:
	CLoadProgress(CWnd* pParent = NULL);   // standard constructor
	void KillMe();

// Dialog Data
	//{{AFX_DATA(CLoadProgress)
	enum { IDD = IDD_LOAD_PROGRESS };
	CProgressCtrl	m_pcLoadProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadProgress)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADPROGRESS_H__8FF7BE9F_6A9F_4762_A63C_40FF087668AD__INCLUDED_)

#if !defined(AFX_ALARMS_H__1E1931F2_7B91_45B1_831E_AA44430C74C0__INCLUDED_)
#define AFX_ALARMS_H__1E1931F2_7B91_45B1_831E_AA44430C74C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Alarms.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlarms dialog

class CAlarms : public CDialog
{
// Construction
public:
	void DoMaskCkBox (UINT nID);
	CAlarms(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlarms)
	enum { IDD = IDD_ALARMS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarms)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlarms)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnRefresh();
	afx_msg void OnBnResetInst();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMS_H__1E1931F2_7B91_45B1_831E_AA44430C74C0__INCLUDED_)

#if !defined(AFX_CHANGEPSWD_H__994FF7F8_34B8_48CB_947A_C54A7C699F4F__INCLUDED_)
#define AFX_CHANGEPSWD_H__994FF7F8_34B8_48CB_947A_C54A7C699F4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangePsWd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangePsWd dialog

class CChangePsWd : public CDialog
{
// Construction
public:
	CChangePsWd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangePsWd)
	enum { IDD = IDD_CHANGE_PASSWORD };
	CString	m_sConfirmNew;
	CString	m_sNewPW;
	CString	m_sOldPW;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePsWd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangePsWd)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPSWD_H__994FF7F8_34B8_48CB_947A_C54A7C699F4F__INCLUDED_)

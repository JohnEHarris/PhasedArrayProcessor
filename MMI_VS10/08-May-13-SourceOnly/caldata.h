#if !defined(AFX_CALDATA_H__D1EBD298_21E7_43C7_AA0D_E208CD9C6B65__INCLUDED_)
#define AFX_CALDATA_H__D1EBD298_21E7_43C7_AA0D_E208CD9C6B65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// caldata.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// caldata dialog

class caldata : public CDialog
{
// Construction
public:
	caldata(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(caldata)
	enum { IDD = IDD_Caldata };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(caldata)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(caldata)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALDATA_H__D1EBD298_21E7_43C7_AA0D_E208CD9C6B65__INCLUDED_)

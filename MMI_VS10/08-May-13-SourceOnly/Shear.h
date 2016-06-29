#if !defined(AFX_SHEAR_H__4A6E37CA_DDB9_4290_87C7_43CE7D412FC0__INCLUDED_)
#define AFX_SHEAR_H__4A6E37CA_DDB9_4290_87C7_43CE7D412FC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Shear.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShear dialog

class CShear : public CDialog
{
// Construction
public:
	CShear(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShear)
	enum { IDD = IDD_SHEAR_V_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShear)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShear)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEnShear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEAR_H__4A6E37CA_DDB9_4290_87C7_43CE7D412FC0__INCLUDED_)

#if !defined(AFX_CALICOPY_H__FF21CD9E_99BB_4E0A_A799_5D1CEED5DBEA__INCLUDED_)
#define AFX_CALICOPY_H__FF21CD9E_99BB_4E0A_A799_5D1CEED5DBEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// calicopy.h : header file
//
#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// calicopy dialog

class calicopy : public CDialog
{
// Construction
public:
	calicopy(CWnd* pParent = NULL);   // standard constructor
	CTcpThreadRxList *m_pTcpThreadRxList;

// Dialog Data
	//{{AFX_DATA(calicopy)
	enum { IDD = IDD_calcpy };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(calicopy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(calicopy)
	afx_msg void OnBgg2();
	afx_msg void OnBgg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALICOPY_H__FF21CD9E_99BB_4E0A_A799_5D1CEED5DBEA__INCLUDED_)

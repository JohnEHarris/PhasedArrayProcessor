#if !defined(AFX_ALICOPY_H__0E2350AB_201A_43D9_9950_6E0C18AAD218__INCLUDED_)
#define AFX_ALICOPY_H__0E2350AB_201A_43D9_9950_6E0C18AAD218__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// alicopy.h : header file
//
#include "TcpThreadRxList.h"


/////////////////////////////////////////////////////////////////////////////
// Calicopy dialog

class Calicopy : public CDialog
{
// Construction
public:
	Calicopy(CWnd* pParent = NULL);   // standard constructor
//	void SendMsg(int nMsgId);

	CTcpThreadRxList *m_pTcpThreadRxList;

// Dialog Data
	//{{AFX_DATA(Calicopy)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Calicopy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Calicopy)
	afx_msg void OnBgg();
	afx_msg void OnBgg2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALICOPY_H__0E2350AB_201A_43D9_9950_6E0C18AAD218__INCLUDED_)

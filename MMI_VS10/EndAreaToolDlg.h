#if !defined(AFX_ENDAREATOOLDLG_H__3D57E0E4_3863_4F53_996C_D10405A4FE41__INCLUDED_)
#define AFX_ENDAREATOOLDLG_H__3D57E0E4_3863_4F53_996C_D10405A4FE41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EndAreaToolDlg.h : header file
//

#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CEndAreaToolDlg dialog

class CEndAreaToolDlg : public CDialog
{
// Construction
public:
	CEndAreaToolDlg(CWnd* pParent = NULL, CTcpThreadRxList *m_pTcpThreadRxList = NULL);   // standard constructor

	CTcpThreadRxList *m_pTcpThreadRxList;

	BYTE  m_nEndToolEnabled[100];

	void UpdateDlg();

// Dialog Data
	//{{AFX_DATA(CEndAreaToolDlg)
	enum { IDD = IDD_ENDAREA };
	CButton	m_btnEndTool;
	float	m_fEndDelay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEndAreaToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEndAreaToolDlg)
	afx_msg void OnBtnEndAreaToolEnable();
	afx_msg void OnBtnEndofPipe();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditEnddelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENDAREATOOLDLG_H__3D57E0E4_3863_4F53_996C_D10405A4FE41__INCLUDED_)

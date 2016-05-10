#if !defined(AFX_CPYTO_H__8C7BA1E7_8279_4540_9FA8_35094C7E1249__INCLUDED_)
#define AFX_CPYTO_H__8C7BA1E7_8279_4540_9FA8_35094C7E1249__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Cpyto.h : header file
//

#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CCpyto dialog

class CCpyto : public CDialog
{
// Construction
public:
	void CopyChannel (BYTE Dest);
	CCpyto(CWnd* pParent = NULL);   // standard constructor

	void SendMsg(int nMsgId);

	CTcpThreadRxList *m_pTcpThreadRxList;

// Dialog Data
	//{{AFX_DATA(CCpyto)
	enum { IDD = IDD_Cpy };
	BOOL	m_bAlarms;
	BOOL	m_bFlaw;
	BOOL	m_bRcvr;
	BOOL	m_bScope;
	BOOL	m_bTcg;
	BOOL	m_bTof;
	BOOL	m_bGates;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCpyto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCpyto)
	virtual void OnOK();
	afx_msg void OnRbAll();
	afx_msg void OnRbSel();
	afx_msg void OnCHECKAlms();
	afx_msg void OnCHECKFlawS();
	afx_msg void OnCHECKRcv();
	afx_msg void OnCHECKScp();
	afx_msg void OnChecktcg();
	afx_msg void OnChecktof();
	afx_msg void OnCHGates();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPYTO_H__8C7BA1E7_8279_4540_9FA8_35094C7E1249__INCLUDED_)

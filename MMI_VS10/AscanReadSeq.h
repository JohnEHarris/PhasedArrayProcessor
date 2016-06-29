#if !defined(AFX_CAscanReadSeq_H__30982F3F_AF41_4D06_8205_F66111F68B6D__INCLUDED_)
#define AFX_CAscanReadSeq_H__30982F3F_AF41_4D06_8205_F66111F68B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AscanReadSeq.h : header file
//
#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CAscanReadSeq dialog

class CAscanReadSeq : public CDialog
{
// Construction
public:
	CAscanReadSeq(CWnd* pParent = NULL);   // standard constructor
	CTcpThreadRxList *m_pTcpThreadRxList;

// Dialog Data
	//{{AFX_DATA(CAscanReadSeq)
	enum { IDD = IDD_ASCANSEQ };
	CScrollBar	m_sbArrayNumber;
	CScrollBar	m_sbSeqNumber;
	int		m_nArrayNumber;
	int		m_nSeqNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAscanReadSeq)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAscanReadSeq)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAscanReadSeq_H__30982F3F_AF41_4D06_8205_F66111F68B6D__INCLUDED_)

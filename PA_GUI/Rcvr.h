#if !defined(AFX_RCVR_H__673D8BC9_B851_4843_88C1_59363BA24636__INCLUDED_)
#define AFX_RCVR_H__673D8BC9_B851_4843_88C1_59363BA24636__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Rcvr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRcvr dialog

class CRcvr : public CDialog
{
// Construction
public:
	CRcvr(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static CRcvr *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	CONFIG_REC * m_pUndo;
	void UndoRcvrSettings();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdRF();
	afx_msg void OnRdFW();

// Dialog Data
	//{{AFX_DATA(CRcvr)
	enum { IDD = IDD_RCVR };
	CScrollBar	m_sbAreaCoef;
	CScrollBar	m_sbOffset;
	CComboBox	m_CBfltr;
	CScrollBar	m_SBGain;
	CString	m_sGain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRcvr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Save_Pos();
	// Generated message map functions
	//{{AFX_MSG(CRcvr)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnChOff();
	afx_msg void OnBnCompu();
	afx_msg void OnRdMns();
	afx_msg void OnRdPls();
	virtual void OnCancel();
	afx_msg void OnSelchangeCBfltr();
	afx_msg void OnRcvrUndo();
	afx_msg void OnRdGate3peak();
	afx_msg void OnRdGate3area();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual ~CRcvr();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RCVR_H__673D8BC9_B851_4843_88C1_59363BA24636__INCLUDED_)

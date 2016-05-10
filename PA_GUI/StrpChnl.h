#if !defined(AFX_STRPCHNL_H__BE6FD8EF_2C9E_4A1F_BD8F_0DC8A151122A__INCLUDED_)
#define AFX_STRPCHNL_H__BE6FD8EF_2C9E_4A1F_BD8F_0DC8A151122A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StrpChnl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStrpChnl dialog

class CStrpChnl : public CDialog
{
// Construction
public:
	int GetChnlNumber(char *s);
	void TurnOffAll(int ChnlType);
	void TurnOnAll(int ChnlType);
	void KillMe();
	BOOL SendMsg(int MsgNum);
	CStrpChnl(CWnd* pParent = NULL);   // standard constructor
	static CStrpChnl *m_pDlg;			// ptr to this dialog..always use
										// the name m_pDlg by convention
	
// Dialog Data
	//{{AFX_DATA(CStrpChnl)
	enum { IDD = IDD_STRIP_CHNLS };
	CComboBox	m_cbWall;
	CComboBox	m_cbTran;
	CComboBox	m_cbObq3;
	CComboBox	m_cbObq2;
	CComboBox	m_cbObq1;
	CComboBox	m_cbLong;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStrpChnl)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStrpChnl)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeCbLong();
	afx_msg void OnSelchangeCbObq1();
	afx_msg void OnSelchangeCbObq2();
	afx_msg void OnSelchangeCbObq3();
	afx_msg void OnSelchangeCbTran();
	afx_msg void OnSelchangeCbWall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRPCHNL_H__BE6FD8EF_2C9E_4A1F_BD8F_0DC8A151122A__INCLUDED_)

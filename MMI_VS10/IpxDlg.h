#if !defined(AFX_IPXDLG_H__B789EA65_EE1F_11D4_ABE1_00A0C91FC401__INCLUDED_)
#define AFX_IPXDLG_H__B789EA65_EE1F_11D4_ABE1_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IpxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIpxDlg dialog

class CIpxDlg : public CDialog
{
// Construction
public:
	CIpxDlg(CWnd* pParent = NULL);   // standard constructor
	static CIpxDlg *m_pDlg;		// ptr to this dialog..always use
									// the name m_pDlg by convention
	~CIpxDlg();						// destructor
	BOOL GrantList();
	void KillMe();

afx_msg LRESULT VS10_OnCancel(WPARAM, LPARAM);


// Dialog Data
	//{{AFX_DATA(CIpxDlg)
	enum { IDD = IDD_IPX_THREAD_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIpxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_uIpxThreadTimer;

	// Generated message map functions
	//{{AFX_MSG(CIpxDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPtrList m_plistTemp;
	void ErrMsgSockStartup(int errval);
	void ErrMsgSock(char *t);
	BOOL InitIPX();
	void ErrMsgSockNA(char *t);
	afx_msg LRESULT OnIpxRxRdy(WPARAM, LPARAM);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPXDLG_H__B789EA65_EE1F_11D4_ABE1_00A0C91FC401__INCLUDED_)

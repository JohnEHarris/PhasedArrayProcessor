#if !defined(AFX_IPXSTAT_H__0007C633_6D36_11D4_AB66_00A0C91FC401__INCLUDED_)
#define AFX_IPXSTAT_H__0007C633_6D36_11D4_AB66_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IpxStat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIpxStat dialog

class CIpxStat : public CDialog
{
// Construction
public:
	void KillMe();
	CIpxStat(CWnd* pParent = NULL);   // standard constructor
	static CIpxStat *m_pDlg;		// ptr to this dialog..always use
									// the name m_pDlg by convention

// Dialog Data
	//{{AFX_DATA(CIpxStat)
	enum { IDD = IDD_IPX_STATS };
	CEdit	m_ceIpxStats;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIpxStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIpxStat)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBnStats();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPXSTAT_H__0007C633_6D36_11D4_AB66_00A0C91FC401__INCLUDED_)

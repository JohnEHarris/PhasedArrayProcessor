#if !defined(AFX_WCJOB_H__5E968556_5594_11D3_9ACF_0040055E6A64__INCLUDED_)
#define AFX_WCJOB_H__5E968556_5594_11D3_9ACF_0040055E6A64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WcJob.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWcJob dialog

class CWcJob : public CDialog
{
// Construction
public:
	CWcJob(CWnd* pParent = NULL);   // standard constructor
	static CWcJob *m_pDlg;		// always use the same name

void DoWallOptionRadioButton(UINT nID);

// Dialog Data
	//{{AFX_DATA(CWcJob)
	enum { IDD = IDD_CUSTOMER };
	CString	m_szLocation;
	CString	m_szWO;
	CString	m_szComment;
	CString	m_szDate;
	CString	m_szBusiness;
	CString	m_szOperator;
	CString	m_szWellFoot;
	CString	m_szCustomer;
	int		m_nWallOption;
	CString	m_szHeat;
	CString	m_szLotNum;
	CString	m_szRbNum;
	CString	m_szOblq1Name;
	CString	m_szOblq2Name;
	CString	m_szOblq3Name;
	BOOL	m_bWallDeviation;
	float	m_fWall1Offset;
	float	m_fWall2Offset;
	float	m_fWall3Offset;
	float	m_fWall4Offset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWcJob)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWcJob)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnCkWallDeviation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	virtual void OnOK();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCJOB_H__5E968556_5594_11D3_9ACF_0040055E6A64__INCLUDED_)

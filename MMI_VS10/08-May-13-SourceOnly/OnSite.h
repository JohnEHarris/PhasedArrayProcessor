#if !defined(AFX_ONSITE_H__52CC1EA6_B50C_4A0F_94A9_C76F93F21D13__INCLUDED_)
#define AFX_ONSITE_H__52CC1EA6_B50C_4A0F_94A9_C76F93F21D13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OnSite.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COnSite dialog

class COnSite : public CDialog
{
// Construction
public:
	COnSite(CWnd* pParent = NULL);   // standard constructor
	CTscanApp *m_ptheApp;			// pointer to mfc app which runs all this code
	int   m_nArrayNumber;
	void  ListArrayScanType();

// Dialog Data
	//{{AFX_DATA(COnSite)
	enum { IDD = IDD_SITE_CUSTOM };
	CComboBox	m_cbBaseGain;
	CScrollBar	m_sbTranAngle;
	CComboBox	m_cbArrayNumber;
	CComboBox	m_cbArrayScanType;
	CScrollBar	m_sbWallWindow;
	CScrollBar	m_sbWallDrop;
	CScrollBar	m_sbX2;
	CScrollBar	m_sbX;
	CScrollBar	m_sbClk;
	int		m_nWoffset;
	int		m_nClkOffset;
	int		m_nXoffset;
	int		m_nXoffset2;
	int		m_nWallDropTime;
	BOOL	m_bRecordWall;
	float	m_fLineSpeed;
	float	m_fRotateSpeed;
	BOOL	m_b20ChnlPerHead;
	int		m_nMaxWallWindowSize;
	CString	m_sTranAngle;
	CString	m_sTranShearAngle;
	float	m_fFocusPointZf;
	float	m_fWaterPath;
	int		m_nEncoderDivider;
	float	m_fMotionPulseLen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnSite)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnSite)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbMinus();
	afx_msg void OnRbPlus();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCheckRecordwall();
	afx_msg void OnCheck20ChnlPerHead();
	afx_msg void OnSelchangeComboArrayScanType();
	afx_msg void OnSelchangeComboArrayNumber();
	afx_msg void OnSelchangeComboBasegain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ONSITE_H__52CC1EA6_B50C_4A0F_94A9_C76F93F21D13__INCLUDED_)

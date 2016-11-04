#if !defined(AFX_CHNLTRACEDLG_H__A6EB3B41_04D0_4931_A12B_93F48FA3495A__INCLUDED_)
#define AFX_CHNLTRACEDLG_H__A6EB3B41_04D0_4931_A12B_93F48FA3495A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChnlTraceDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CChnlTraceDlg dialog

class CChnlTraceDlg : public CDialog
{
// Construction
public:
	CChnlTraceDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bChnlSelect[MEM_MAX_CHANNEL];
	void SaveWindowPosition(LPCTSTR lpszProfileName);
	void RestoreWindowPosition(LPCTSTR lpszProfileName);
	WORD m_wOldPipeStatus;
	int  m_nScale;
	CPoint m_ptExist[40][900][2];
	void EraseExistingPts();
	void DrawExistingTraces();
	CPtrList m_plistUtData;
	void DrawXTicks();
	short m_nNumExistWall[40];
	void PrintChnlTraces();

// Dialog Data
	//{{AFX_DATA(CChnlTraceDlg)
	enum { IDD = IDD_CHNL_TRACE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChnlTraceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EraseTraces();
	void DrawTraces();
	// Generated message map functions
	//{{AFX_MSG(CChnlTraceDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHNLTRACEDLG_H__A6EB3B41_04D0_4931_A12B_93F48FA3495A__INCLUDED_)

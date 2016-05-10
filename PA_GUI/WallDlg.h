#if !defined(AFX_WALLDLG_H__B6BFD8D6_4C5E_11D4_AB3F_00A0C91FC401__INCLUDED_)
#define AFX_WALLDLG_H__B6BFD8D6_4C5E_11D4_AB3F_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WallDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWallDlg dialog

class CWallDlg : public CDialog
{
// Construction
public:
	CWallDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWallDlg)
	enum { IDD = IDD_WALL_DLG };
	CString	m_szNewWall;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWallDlg)
	afx_msg void OnChangeNewWall();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WALLDLG_H__B6BFD8D6_4C5E_11D4_AB3F_00A0C91FC401__INCLUDED_)

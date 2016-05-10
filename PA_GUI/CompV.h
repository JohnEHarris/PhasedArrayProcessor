#if !defined(AFX_COMPV_H__90664B15_ED05_45FD_8324_ADA858AFE2DC__INCLUDED_)
#define AFX_COMPV_H__90664B15_ED05_45FD_8324_ADA858AFE2DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompV.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompV dialog

class CCompV : public CDialog
{
// Construction
public:
	CCompV(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompV)
	enum { IDD = IDD_COMP_V_DLG };
	float	m_fCompV;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompV)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompV)
	afx_msg void OnChangeEnComp();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPV_H__90664B15_ED05_45FD_8324_ADA858AFE2DC__INCLUDED_)

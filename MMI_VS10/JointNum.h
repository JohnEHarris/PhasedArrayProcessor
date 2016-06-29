#if !defined(AFX_JOINTNUM_H__D4B448F3_D4D5_11D3_9B5A_00A0C91FC401__INCLUDED_)
#define AFX_JOINTNUM_H__D4B448F3_D4D5_11D3_9B5A_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JointNum.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJointNum dialog

class CJointNum : public CDialog
{
// Construction
public:
	CJointNum(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJointNum)
	enum { IDD = IDD_JNT_NUM };
	UINT	m_uNewJntNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJointNum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJointNum)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNewJntNum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOINTNUM_H__D4B448F3_D4D5_11D3_9B5A_00A0C91FC401__INCLUDED_)

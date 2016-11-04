#if !defined(AFX_DISK_H__588C1B91_767B_46CB_A1C2_02197D34B426__INCLUDED_)
#define AFX_DISK_H__588C1B91_767B_46CB_A1C2_02197D34B426__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Disk.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisk dialog

class CDisk : public CDialog
{
// Construction
public:
	CDisk(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisk)
	enum { IDD = IDD_Disk };
	CListBox	m_Sdisk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisk)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisk)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISK_H__588C1B91_767B_46CB_A1C2_02197D34B426__INCLUDED_)

#if !defined(AFX_FTEXT2_H__3047F9E3_E383_46A0_9C22_A6FB62D533C8__INCLUDED_)
#define AFX_FTEXT2_H__3047F9E3_E383_46A0_9C22_A6FB62D533C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Ftext2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Ftext2 dialog

class Ftext2 : public CDialog
{
// Construction
public:
	Ftext2(CWnd* pParent = NULL, int nID = 1);   // standard constructor
	//static Ftext *m_pDlg;		// ptr to this dialog..always use
	 								// the name m_pDlg by convention
	void SavePosition();
	void KillMe();
	static Ftext2 *m_pDlg;			// ptr to this dialog..always use

	int m_nID;

// Dialog Data
	//{{AFX_DATA(Ftext2)
	enum { IDD = IDD_Dtext2 };
	CListBox	m_LBtext;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Ftext2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Ftext2)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTEXT2_H__3047F9E3_E383_46A0_9C22_A6FB62D533C8__INCLUDED_)

#if !defined(AFX_FTEXT_H__7A06CA41_2E49_4D07_98E8_B40874A9D4EB__INCLUDED_)
#define AFX_FTEXT_H__7A06CA41_2E49_4D07_98E8_B40874A9D4EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Ftext.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Ftext dialog

class Ftext : public CDialog
{
// Construction
public:
	Ftext(CWnd* pParent = NULL, int nID = 0);   // standard constructor
	//static Ftext *m_pDlg;		// ptr to this dialog..always use
	 								// the name m_pDlg by convention
	void SavePosition();
	void KillMe();
    static Ftext *m_pDlg;// ptr to this dialog..always use

	int m_nID;

// Dialog Data
	//{{AFX_DATA(Ftext)
	enum { IDD = IDD_Dtext };
	CListBox	m_LBtext;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Ftext)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Ftext)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTEXT_H__7A06CA41_2E49_4D07_98E8_B40874A9D4EB__INCLUDED_)
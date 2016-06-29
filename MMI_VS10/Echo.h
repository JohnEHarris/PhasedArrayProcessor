#if !defined(AFX_ECHO_H__E0CEB793_59CA_11D4_AB4E_00A0C91FC401__INCLUDED_)
#define AFX_ECHO_H__E0CEB793_59CA_11D4_AB4E_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Echo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEcho dialog

class CEcho : public CDialog
{
// Construction
public:
	RECT m_Rect;
	CBrush m_brRedBrush, m_brBlueBrush;
	CBrush m_brYellowBrush;
	CBrush m_brYellowBrush2;
	ECHO_BITS m_Eb;
	CEcho(CWnd* pParent = NULL);   // standard constructor
	static CEcho *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	void KillMe();					// another way to die
	void DoS1RadioButton(UINT nID) ;

	CTscanApp *m_ptheApp;			// pointer to mfc app which runs all this code

// Dialog Data
	//{{AFX_DATA(CEcho)
#if (LOCATION_ID == BESSEMER)
	enum { IDD = IDD_XDCR_ACTIVITY_BESSEMER };
#else
	enum { IDD = IDD_XDCR_ACTIVITY };
#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEcho)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void StopTimer(void);
	void StartTimer (void);

	// Generated message map functions
	//{{AFX_MSG(CEcho)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	UINT m_uEchoBitTimer;	// 3 second timer to display echo scoreboard

	BOOL IsSequenceOff(int nShoe, int nChannel);
	BOOL IsChnlNothing(int nShoe, int nChannel);
	BOOL IsGate2or3RF(int nShoe, int nChannel);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECHO_H__E0CEB793_59CA_11D4_AB4E_00A0C91FC401__INCLUDED_)

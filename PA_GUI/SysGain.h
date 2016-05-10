#if !defined(AFX_SYSGAIN_H__95539C2D_C484_49B2_AE56_8E4B0CDB7EC5__INCLUDED_)
#define AFX_SYSGAIN_H__95539C2D_C484_49B2_AE56_8E4B0CDB7EC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SysGain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSysGain dialog

class CSysGain : public CDialog
{
// Construction
public:
	CSysGain(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static CSysGain *m_pDlg;			// ptr to this dialog..always use
	CONFIG_REC * m_pUndo;
									// the name m_pDlg by convention
	virtual BOOL OnInitDialog();
	void UpdateGates();
// Dialog Data
	//{{AFX_DATA(CSysGain)
	enum { IDD = IDD_SysGain };
	CScrollBar	m_sbSclr;
	CScrollBar	m_SBGain;
	CString	m_sSgain;
	CString	m_sFScaler;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysGain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysGain)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void Ongate1();
	afx_msg void Ongate2();
	virtual void OnOK();
	afx_msg void OnCklong();
	afx_msg void OnCKObq1();
	afx_msg void OnCKObq2();
	afx_msg void OnCKObq3();
	afx_msg void OnCKtran();
	virtual void OnCancel();
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	virtual ~CSysGain();
	void Save_Pos();
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSGAIN_H__95539C2D_C484_49B2_AE56_8E4B0CDB7EC5__INCLUDED_)

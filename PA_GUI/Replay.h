#if !defined(AFX_REPLAY_H__2FCF18E3_F36F_11D3_9B77_00A0C91FC401__INCLUDED_)
#define AFX_REPLAY_H__2FCF18E3_F36F_11D3_9B77_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Replay.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReplay dialog

class CReplay : public CDialog
{
// Construction
public:
	BOOL SendMsg(int MsgNum);
	DWORD GetReplayPosition( CString *s);
	int m_nTick;
	UINT m_uIpxTimer;
	void KillMe();
	CReplay(CWnd* pParent = NULL);   // standard constructor
	static CReplay *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention

	CTscanApp *m_ptheApp;			// pointer to mfc app which runs all this code
	CDWordArray m_dwPos;		// collect file positions
    virtual void OnCancel();
    afx_msg void OnRpQuit();
	afx_msg LRESULT CReplay::VS10_OnRpPlay(WPARAM, LPARAM);

// Dialog Data
	//{{AFX_DATA(CReplay)
	enum { IDD = IDD_REPLAY };
	CButton	m_joint;
	CButton	m_time;
	CButton	m_btime;
	CEdit	m_ceLastJoint;
	CListBox	m_lbNextJoint;
	BOOL	m_bWallStat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplay)
	virtual void OnOK();
	afx_msg void OnRpPlay();
	afx_msg void OnRpTholds();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListNextJnt();
	afx_msg void OnAutoPrint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkListNextJnt();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCkWallStat2();
	afx_msg void OnRjoint();
	afx_msg void OnRTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPLAY_H__2FCF18E3_F36F_11D3_9B77_00A0C91FC401__INCLUDED_)

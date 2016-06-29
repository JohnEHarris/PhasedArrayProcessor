#if !defined(AFX_ENET_H__89E0449B_760F_482E_A0F5_2E81614E4F04__INCLUDED_)
#define AFX_ENET_H__89E0449B_760F_482E_A0F5_2E81614E4F04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Enet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnet dialog

class CEnet : public CDialog
{
// Construction
public:
	CEnet(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	static CEnet *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention

	void KillMe();					// another way to die

// Dialog Data
	//{{AFX_DATA(CEnet)
	enum { IDD = IDD_Enet };
	CEdit	m_report;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnet)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	afx_msg void OnUpdate();
	afx_msg void OnReset();
	afx_msg void OnRestore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	UINT m_uEnetTimer;
//	void PrintMsg01(I_MSG_RUN *pMsg01);
//	void PrintMsg02(I_MSG_CAL *pMsg02);
	void PrintMsg03(I_MSG_PKT *pMsg03);
	void SendMsgTextOut(int nMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENET_H__89E0449B_760F_482E_A0F5_2E81614E4F04__INCLUDED_)

#if !defined(AFX_CHNLON_H__3AE56F93_ED6E_11D3_97C1_204C4F4F5020__INCLUDED_)
#define AFX_CHNLON_H__3AE56F93_ED6E_11D3_97C1_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChnlOn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChnlOn dialog

class CChnlOn : public CDialog
{
// Construction
public:
	void KillMe();
	CChnlOn(CWnd* pParent = NULL);   // standard constructor
	static CChnlOn *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	int m_nAmaSonoSel;

// Dialog Data
	//{{AFX_DATA(CChnlOn)
	enum { IDD = IDD_CHNLON };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChnlOn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoAllChnls(int shoe, int value);
	void DoS2CkBox(UINT nID);
	void DoS1CkBox(UINT nID);
	BOOL SendMsg(int MsgNum);

	// Generated message map functions
	//{{AFX_MSG(CChnlOn)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBt1Alloff();
	afx_msg void OnBt1Allon();
	afx_msg void OnBt2Alloff();
	afx_msg void OnBt2Allon();
	afx_msg void OnChnlSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHNLON_H__3AE56F93_ED6E_11D3_97C1_204C4F4F5020__INCLUDED_)

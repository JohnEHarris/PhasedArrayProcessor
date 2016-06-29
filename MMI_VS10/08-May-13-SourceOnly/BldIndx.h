#if !defined(AFX_BLDINDX_H__30BA3603_F4FD_11D3_9B79_00A0C91FC401__INCLUDED_)
#define AFX_BLDINDX_H__30BA3603_F4FD_11D3_9B79_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BldIndx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBldIndx dialog

class CBldIndx : public CDialog
{
// Construction
public:
	void StopTimer();
	void StartTimer();
	CProgressCtrl* pPrgsDlg;
	virtual  ~CBldIndx();
	CBldIndx(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBldIndx)
	enum { IDD = IDD_REBUILD_INDEX };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBldIndx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBldIndx)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	int m_nTick;
	UINT m_uIpxTimer;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLDINDX_H__30BA3603_F4FD_11D3_9B79_00A0C91FC401__INCLUDED_)

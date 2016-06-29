#if !defined(AFX_EXAMPLE_H__32F7F9F1_CA93_11D3_9B4D_00A0C91FC401__INCLUDED_)
#define AFX_EXAMPLE_H__32F7F9F1_CA93_11D3_9B4D_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Example.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Example dialog

class Example : public CDialog
{
// Construction
public:
	Example(CWnd* pParent = NULL);   // standard constructor
	static Example *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention

	void KillMe();					// another way to die


// Dialog Data
	//{{AFX_DATA(Example)
	enum { IDD = IDD_EXAMPLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Example)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Example)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	UINT m_uIpxTimer;
	
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXAMPLE_H__32F7F9F1_CA93_11D3_9B4D_00A0C91FC401__INCLUDED_)

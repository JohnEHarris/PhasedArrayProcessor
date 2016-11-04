#if !defined(AFX_IPXIN_H__766ADDA5_ED5E_11D4_ABE0_00A0C91FC401__INCLUDED_)
#define AFX_IPXIN_H__766ADDA5_ED5E_11D4_ABE0_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IpxIn.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CIpxIn thread

	

class CIpxIn : public CWinThread
{
	DECLARE_DYNCREATE(CIpxIn)
protected:
	CIpxIn();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	int m_nLoop;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIpxIn)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	// Wrappers to convert VC6 to Visual Studio 2010
	afx_msg LRESULT CIpxIn::VS10_ExitInstance(WPARAM, LPARAM);


// Implementation
protected:
	virtual ~CIpxIn();

	// Generated message map functions
	//{{AFX_MSG(CIpxIn)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()



private:
};



#if 0
// Manually add a thread dialog which will never be shown in order
// to get window handles and message pump.  This dialog will actually
//  receive and handle the ipx window user message from the os.

class CThreadDlg : public CDialog
	{
	// Construction
public:
// Dialog Data
	//{{AFX_DATA(CThreadDlg)
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThreadDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CThreadDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	}
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPXIN_H__766ADDA5_ED5E_11D4_ABE0_00A0C91FC401__INCLUDED_)

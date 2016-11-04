#if !defined(AFX_ADISETUPDLG_H__D6902EA4_91E3_447A_BE58_70A9C15634F2__INCLUDED_)
#define AFX_ADISETUPDLG_H__D6902EA4_91E3_447A_BE58_70A9C15634F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdiSetupDlg.h : header file
//
#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CAdiSetupDlg dialog

class CAdiSetupDlg : public CDialog
{
// Construction
public:
	CAdiSetupDlg(CWnd* pParent = NULL);   // standard constructor

	CTcpThreadRxList *m_pTcpThreadRxList;
	BYTE m_nBtnChecked[32];
	void SetCheckedBox();
	void GetCheckedBox();

// Dialog Data
	//{{AFX_DATA(CAdiSetupDlg)
	enum { IDD = IDD_ADI_SETUP };
	CButton	m_btnBit31;
	CButton	m_btnBit30;
	CButton	m_btnBit29;
	CButton	m_btnBit28;
	CButton	m_btnBit27;
	CButton	m_btnBit26;
	CButton	m_btnBit25;
	CButton	m_btnBit24;
	CButton	m_btnBit23;
	CButton	m_btnBit22;
	CButton	m_btnBit21;
	CButton	m_btnBit20;
	CButton	m_btnBit19;
	CButton	m_btnBit18;
	CButton	m_btnBit17;
	CButton	m_btnBit16;
	CButton	m_btnBit15;
	CButton	m_btnBit14;
	CButton	m_btnBit13;
	CButton	m_btnBit12;
	CButton	m_btnBit11;
	CButton	m_btnBit10;
	CButton	m_btnBit9;
	CButton	m_btnBit8;
	CButton	m_btnBit7;
	CButton	m_btnBit6;
	CButton	m_btnBit5;
	CButton	m_btnBit4;
	CButton	m_btnBit3;
	CButton	m_btnBit2;
	CButton	m_btnBit1;
	CButton	m_btnBit0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdiSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdiSetupDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADISETUPDLG_H__D6902EA4_91E3_447A_BE58_70A9C15634F2__INCLUDED_)

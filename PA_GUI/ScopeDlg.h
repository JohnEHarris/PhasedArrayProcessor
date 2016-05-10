#if !defined(AFX_SCOPEDLG_H__32E97C70_5F83_4D3B_8F24_E5094342521C__INCLUDED_)
#define AFX_SCOPEDLG_H__32E97C70_5F83_4D3B_8F24_E5094342521C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScopeDlg.h : header file
//
#include "resource.h"
#include "TcpThreadRxList.h"
#include "ScopeTrace2CalDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CScopeDlg dialog

class CScopeDlg : public CDialog
{
// Construction
public:
	CScopeDlg(CWnd* pParent = NULL, CTcpThreadRxList* pThreadRxList = NULL);   // standard constructor
	CTcpThreadRxList* m_pThreadRxList;

// Dialog Data
	//{{AFX_DATA(CScopeDlg)
#ifdef _I_AM_PAG
	enum { IDD = IDD_PAG_SCOPE };
#else
	enum { IDD = IDD_SCOPE };
#endif

	CComboBox	m_CBscope3;
	CButton	m_BTNscope2AllGates;
	CComboBox	m_CBscope2;
	CComboBox	m_CBscope1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScopeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	void UpdateChnlGate();
	CScopeTrace2CalDlg* m_pScopeTrace2CalDlg;
	void KillMe();
	short m_nActiveChnl;
	short m_nActiveGate;
	void SetWindowPosition(int top);
	void ChangeActiveChnl(short m_nActiveChnl);

protected:
	void SaveWindowPosition(LPCTSTR lpszProfileName);
	void RestoreWindowPosition(LPCTSTR lpszProfileName);

private:
	bool m_bScope1RFon;
	bool m_bScope2AllGatesOn;
	CBitmap m_bmpScope1NormalOn;
	CBitmap m_bmpScope1NormalOff;
	HBITMAP m_hbmpScope1NormalOn;
	HBITMAP m_hbmpScope1NormalOff;
	CBitmap m_bmpScope2AllGatesOn;
	CBitmap m_bmpScope2AllGatesOff;
	HBITMAP m_hbmpScope2AllGatesOn;
	HBITMAP m_hbmpScope2AllGatesOff;
	CBitmap m_bmpScope2ActiveGateOn;
	CBitmap m_bmpScope2ActiveGateOff;
	HBITMAP m_hbmpScope2ActiveGateOn;
	HBITMAP m_hbmpScope2ActiveGateOff;
	bool m_bScope2Gate1On;
	bool m_bScope2Gate2On;
	bool m_bScope2Gate3On;
	bool m_bScope2Gate4On;
	unsigned long m_bufScopeRegister;  // holds the data to be written by the PLX api call to Scope register
	int m_nCBscope1Sel,  m_nCBscope2Sel, m_nCBscope3Sel;
	bool m_bEndAreaToolEnabled;
	void WriteToEndAreaToolEnabledRegister();
	bool m_bScope2Visible;
	void ScopeTrace2Calibrate();
	void ScopeTrace1Calibrate();
	CPoint m_ptMouseStart;


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScopeDlg)
	afx_msg void OnBtnAllgates();
	afx_msg void OnSelchangeCbScope1();
	afx_msg void OnSelchangeCbScope2();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSetfocusCbScope2();
	afx_msg void OnSelchangeCbScope3();
	afx_msg void OnSetfocusCbScope3();
	virtual void OnCancel();
	afx_msg void OnBtnFastback();
	afx_msg void OnBtnFastforward();
	afx_msg void OnBtnSlowback();
	afx_msg void OnBtnSlowforward();
	afx_msg void OnBtnActivegate();
	afx_msg void OnSetfocusCbScope1();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClicked65535();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOPEDLG_H__32E97C70_5F83_4D3B_8F24_E5094342521C__INCLUDED_)

#if !defined(AFX_SCOPETRACE2CAL_H__F1B9E384_C0B9_4897_A8B1_28EB57558458__INCLUDED_)
#define AFX_SCOPETRACE2CAL_H__F1B9E384_C0B9_4897_A8B1_28EB57558458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScopeTrace2CalDlg.h : header file
//

#include "..\include\cfg100.h"
//#include "SeekButton.h"
#include "TcpThreadRxList.h"
#include "SeekButton.h"
//#include "TSCANDLG.H"

//CTscanDlg* m_pCTscanDlg;



/////////////////////////////////////////////////////////////////////////////
// CScopeTrace2CalDlg dialog

class CScopeTrace2CalDlg : public CDialog
{
// Construction
public:
	CScopeTrace2CalDlg(CWnd* pParent = NULL, CTcpThreadRxList* pThreadRxList = NULL);   // standard constructor
	CTcpThreadRxList* m_pTcpThreadRxList;
	afx_msg LONG OnSeekButtonValueChanged( UINT i, LONG j );

	void UpdateDlg();

	//CTscanDlg* m_pCTscanDlg;

// Dialog Data
	//{{AFX_DATA(CScopeTrace2CalDlg)
	enum { IDD = IDD_SCOPETRACE2CAL1 };
	CComboBox	m_cbInstrument;
	CButton	m_btnExit;
	CEdit	m_editValue;
	CEdit	m_editLabel2;
	CEdit	m_editLabel1;
	CButton	m_btnNext;
	CButton	m_btnBack;
	CEdit	m_editTrace2Cal;
	int		m_nEditValue;
	//}}AFX_DATA
	int m_nStep;
	int m_ValueStep[12];
	int m_nInstrument;

private:
	void Trace2CalStep1();
	void Trace2CalStep2();
	void Trace2CalStep3();
	void Trace2CalStep4();
	void Trace2CalStep5();
	void Trace2CalStep6();
	void Trace2CalStep7();
	void Trace2CalStep8();
	void Trace2CalStep9();
	void Trace2CalStep10();
	void Trace2CalStep11();
	void Trace2CalStep12();
	void ActionStep(int nStep);
	CONFIG_REC m_OriginalConfigRec;
	bool m_bModified;
	void SaveOriginalConfig();
	bool RestoreOriginalConfig();
	void SaveCurrentCalChanges();

	CSeekButton m_btnForward;
	CSeekButton m_btnBackward;
	CSeekButton m_btnFastForward;
	CSeekButton m_btnFastBackward;
	void SetSeekButton();

	void SetSysPulserOnOff(BOOL bOn);
	void SetSysRcvrOnOff(BOOL bOn);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScopeTrace2CalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScopeTrace2CalDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBack();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeEditValue();
	afx_msg void OnSelchangeComboInstrument();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOPETRACE2CAL_H__F1B9E384_C0B9_4897_A8B1_28EB57558458__INCLUDED_)

#if !defined(AFX_SCOPETRACE1CAL_H__F1B9E384_C0B9_4897_A8B1_28EB57558458__INCLUDED_)
#define AFX_SCOPETRACE1CAL_H__F1B9E384_C0B9_4897_A8B1_28EB57558458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScopeTrace1CalDlg.h : header file
//

#include "..\include\cfg100.h"
//#include "SeekButton.h"
#include "TcpThreadRxList.h"
#include "SeekButton.h"
//#include "TSCANDLG.H"

//CTscanDlg* m_pCTscanDlg;



/////////////////////////////////////////////////////////////////////////////
// CScopeTrace1CalDlg dialog

class CScopeTrace1CalDlg : public CDialog
{
// Construction
public:
	CScopeTrace1CalDlg(CWnd* pParent = NULL, CTcpThreadRxList* pThreadRxList = NULL);   // standard constructor
	CTcpThreadRxList* m_pTcpThreadRxList;
	afx_msg LONG OnSeekButtonValueChanged( UINT i, LONG j );

	WORD m_nInstrument;   // zero-based instrument number to be calibrated
	void UpdateDlg();

	//CTscanDlg* m_pCTscanDlg;

// Dialog Data
	//{{AFX_DATA(CScopeTrace1CalDlg)
	enum { IDD = IDD_SCOPETRACE1CAL };
	CButton	m_btnVerify;
	CButton	m_btnStart;
	CComboBox	m_cbInstrument;
	CButton	m_btnExit;
	CEdit	m_editValue;
	CEdit	m_editLabel2;
	CEdit	m_editLabel1;
	CButton	m_btnNext;
	CButton	m_btnBack;
	CEdit	m_editTrace1Cal;
	int		m_nEditValue;
	//}}AFX_DATA
	int m_nStep;
	int m_ValueStep[6];

private:
	void Trace1CalStep1();
	void Trace1CalStep2();
	void Trace1CalStep3();
	void Trace1CalStep4();
	void Trace1CalStep5();
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
	void SetNoiseReject(BOOL bReject);

	short m_nAdcNull; // AD Converter reading when noise reject always on
	BOOL GetAdcMsg(WORD *pGate1Amp, short *pAdcValue);
	BOOL m_bDo90Percent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScopeTrace1CalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScopeTrace1CalDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBack();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeEditValue();
	afx_msg void OnSelchangeComboInstrument();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnStart();
	afx_msg void OnBtnVerify();
	afx_msg void OnRadioDo90();
	afx_msg void OnRadioDo10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOPETRACE1CAL_H__F1B9E384_C0B9_4897_A8B1_28EB57558458__INCLUDED_)

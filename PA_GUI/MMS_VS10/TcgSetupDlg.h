#if !defined(AFX_TCGSETUPDLG_H__A6A29D96_DE41_45B0_8354_4030F71DEC57__INCLUDED_)
#define AFX_TCGSETUPDLG_H__A6A29D96_DE41_45B0_8354_4030F71DEC57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TcgSetupDlg.h : header file
//

#include "DrawTraces.h"
#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CTcgSetupDlg dialog

class CTcgSetupDlg : public CDialog
{
// Construction
public:
	CTcgSetupDlg(CWnd* pParent = NULL);   // standard constructor

	CDrawTraces* m_pDrawGates;
	CTcpThreadRxList* m_pTcpThreadRxList;
	CONFIG_REC *m_pUndo;

	void DrawGate(int nGateNumber);
	void DrawGate2(int nGateNumber);
	void DrawTcgCurve();
	int m_nTcgMode;
	int m_nTcgStep;
	int m_nGateSel;
	int m_iLastGateStart[4];
	int m_iLastGateStop[4];
	int m_iLastLevel[4];
	int m_iLastAmp[4];
	int m_iLastTof[4];
	bool m_bDrawGateOnce;
	void GetGainOfGate();
	int m_nCurveAdjust;
	CPoint m_ptGainChangeStart;
	bool m_bDrawTcgCurveOnce;
	CPoint m_LastTcgGainTable[256];

	void UpdateDlg();

	void KillMe();
// Dialog Data
	//{{AFX_DATA(CTcgSetupDlg)
	enum { IDD = IDD_TCGSETUP };
	CStatic	m_staticTimeHigh;
	CScrollBar	m_sbGain;
	CButton	m_btnDrawGates;
	float	m_fGain;
	//}}AFX_DATA

protected:
	void SaveWindowPosition(LPCTSTR lpszProfileName);
	void RestoreWindowPosition(LPCTSTR lpszProfileName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTcgSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTcgSetupDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioTcgmode1();
	afx_msg void OnRadioTcgmode2();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBtnReset();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRadioTcgmode3();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRadioStep200();
	afx_msg void OnRadioStep400();
	afx_msg void OnRadioStep600();
	afx_msg void OnRadioStep800();
	afx_msg void OnRadioTrigIF();
	afx_msg void OnRadioTrigIP();
	afx_msg void OnRadioTrigOff();
	afx_msg void OnBtnUndo();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCGSETUPDLG_H__A6A29D96_DE41_45B0_8354_4030F71DEC57__INCLUDED_)

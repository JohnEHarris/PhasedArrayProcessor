#if !defined(AFX_ASCANDLG_H__3A24F055_EB8D_4411_9694_C172A295D479__INCLUDED_)
#define AFX_ASCANDLG_H__3A24F055_EB8D_4411_9694_C172A295D479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AscanDlg.h : header file
//

#include "DrawTraces.h"
#include "ResizingDialog.h"
#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CAscanDlg dialog

class CAscanDlg : public CResizingDialog
{
// Construction
public:
	CAscanDlg(CWnd* pParent = NULL);   // standard constructor

	CDrawTraces* m_pDrawAscan;
	bool		 m_bDrawAscanOnce;
	CTcpThreadRxList *m_pTcpThreadRxList;

	void UpdateDlg();
	void SetRfFullWaveMode();
	BOOL   m_bDrawTcgCurve;
	void   KillMe();
	void   UpdateRefreshRate();

// Dialog Data
	//{{AFX_DATA(CAscanDlg)
	enum { IDD = IDD_ASCAN };
	CWnd	m_Devide;
	CButton	m_btnDrawAscan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAscanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SaveWindowPosition(LPCTSTR lpszProfileName);
	void RestoreWindowPosition(LPCTSTR lpszProfileName);
	void DrawAscan();
	CPoint m_LastAscan[1024];
	int    m_nAscan[1024];
	CPoint m_ptDelayChangeStart;
	BOOL   m_bMoveAscanBaseLine;
	BOOL   m_bMoveAscanDelay;
	BOOL   m_bMoveGate;
	BOOL   m_bChangeGateRange;
	BOOL   m_bMoveTcgCurve;
	BOOL   m_bMoveRcvrGain;
	BOOL   SendMsg(int MsgNum);
	int	   m_nAscanBaseLine;
	void   GetAscanGates();
	ASCAN_GATES m_AscanGates;
	void PrintAscan();
	void SaveAscan();
	void CopyAscan();
	CString m_sDefFileDir;
	COLORREF m_bkColor;
	COLORREF m_traceColor;
	short  m_nGateDelayStart;
	short  m_nGateRangeStart;
	short  m_nGateLevelStart;
	short  m_nAscanDelayStart;
	void   DrawTcgCurve();
	CPoint m_TcgGainTable[256];
	BOOL   m_bInterfaceTrig;
	int    m_nInterfaceTrig;

	void ContractDialog();
	void EnableExpandedControls(BOOL bEnabled);

	int m_nNormalHeight;
	int m_nExpandedHeight;
	int m_nHeightDiff;  //diffrence between normal height and expanded height;
	BOOL m_bExpanded;

	int m_nTcgMode;
	int m_nTcgStep;
	int m_nCurveAdjust; //0:adjust left side of TCG curve; 1:right side of curve 1; 3:right side of curve 2
	void GetGainOfGate(short nChannel, short nGate);
	float m_fGain;
	BOOL m_bShowTcgCurve;

	BOOL m_bShowBscan;
	BOOL m_bGrayscale;
	BOOL m_bLinearScale;
	BOOL m_bClearBscan;
	BYTE m_nLiveFrozenPeak;

	BYTE m_nRefreshRate;
	BYTE m_bBcastAscan;

	int m_nTraceColor;
	int m_nBkColor;

	BOOL m_bOldPipePresent;
	int  m_nFlawRevCnt;
	void SaveFlawDataToFile();

	// Generated message map functions
	//{{AFX_MSG(CAscanDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnExpand();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRadioStep200();
	afx_msg void OnRadioStep400();
	afx_msg void OnRadioStep600();
	afx_msg void OnRadioStep800();
	afx_msg void OnRadioTcgmode1();
	afx_msg void OnRadioTcgmode2();
	afx_msg void OnRadioTcgmode3();
	afx_msg void OnRadioTrigIf();
	afx_msg void OnRadioTrigIp();
	afx_msg void OnRadioTrigOff();
	afx_msg void OnBtnReset();
	afx_msg void OnFrozen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASCANDLG_H__3A24F055_EB8D_4411_9694_C172A295D479__INCLUDED_)

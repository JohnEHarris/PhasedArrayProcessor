#if !defined(AFX_WCALDLG_H__16C161A3_697A_11D4_AB61_00A0C91FC401__INCLUDED_)
#define AFX_WCALDLG_H__16C161A3_697A_11D4_AB61_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WCalDlg.h : header file
//

#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CWCalDlg dialog

class CWCalDlg : public CDialog
{
// Construction
public:
	int m_nOffsetNew[MAX_SHOES];	// each shoe has different offset
	float m_fSlopeNew[MAX_SHOES];
	void StartTimer(void);
	void StopTimer(void);
	BYTE m_MapIndexToChnl[110];	// associat ut chnl with list box index
	int m_nAcquireSel, m_nAcquireCnt;
	float m_nAcquireSum;
	int m_nCalChnl;
	virtual  ~CWCalDlg();
	UT_REC * m_pUndo;
	void UpdateUndoBuffer();
	BOOL SendMsg(int MsgNum);
	void SetConfigData();
	BOOL GetConfigData();
	void KillMe();
	CWCalDlg(CWnd* pParent = NULL);   // standard constructor
	static CWCalDlg *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	virtual BOOL OnInitDialog();

	CTcpThreadRxList *m_pTcpThreadRxList;

// Dialog Data
	//{{AFX_DATA(CWCalDlg)
	enum { IDD = IDD_WALL_CAL };
	CComboBox	m_cbXdcr;
	CScrollBar	m_sbOffset;
	int		m_nOffset;
	float	m_fSlope;
	BOOL	m_bShoe1;
	BOOL	m_bShoe2;
	BOOL	m_bShoe3;
	BOOL	m_bShoe4;
	BOOL	m_bShoe0;
	BOOL	m_bShoe5;
	BOOL	m_bShoe6;
	BOOL	m_bShoe7;
	BOOL	m_bShoe8;
	BOOL	m_bShoe9;
	float	m_nThkRef;
	float	m_nThnRef;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWCalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWCalDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnCompute();
	afx_msg void OnBtThick();
	afx_msg void OnBtThin();
	afx_msg void OnBnInit();
	afx_msg void OnBnUndo();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeCbXdcr();
	afx_msg void OnChangeEnThkRef();
	afx_msg void OnChangeEnThnRef();
	afx_msg void OnBnShear();
	afx_msg void OnBnCompress();
	afx_msg void OnCkUsewith1();
	afx_msg void OnCkUsewith2();
	afx_msg void OnCkUsewith3();
	afx_msg void OnCkUsewith4();
	afx_msg void OnBSet();
	afx_msg void OnCkUsewith5();
	afx_msg void OnCkUsewith6();
	afx_msg void OnCkUsewith7();
	afx_msg void OnCkUsewith8();
	afx_msg void OnCkUsewith9();
	afx_msg void OnCkUsewith10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT m_uIpxTimer;
	UINT m_uTimerTick;
	float m_nThkXdcr, m_nThnXdcr;	// readings from instrument
	float m_nXdcr, m_nCurrentRawCnts;	// readings from instrument

	int MapChnlToIndex(int nChannel);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCALDLG_H__16C161A3_697A_11D4_AB61_00A0C91FC401__INCLUDED_)

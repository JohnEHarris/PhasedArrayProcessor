#if !defined(AFX_ACAL_H__81C0E671_D980_11D3_9B61_00A0C91FC401__INCLUDED_)
#define AFX_ACAL_H__81C0E671_D980_11D3_9B61_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Acal.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAcal dialog

class CAcal : public CDialog
{
// Construction
public:
	BYTE RcvrGainToPct(int gain);
	int m_nGainSigSel;
	virtual BOOL OnInitDialog();
	void SetCalTholds();
	void ShowChnlInfo();
	int GetChnlTypeIndex(int Chnl);
	void CheckHeap(int n);
	BYTE m_bIdMask[48];
//	AMA_BAND * pAmaBand;
	void DrawTholds();
	void UpdateScrollInfo(void);
	void UpdateConfigRec();
	void LoadConfigRec();
	int GetChnlAbsGain(int band, int shoe, int chnl);

	CONFIG_REC * m_pUndo;

	int GetChnlGain(int band, int shoe, int ch);
	BOOL SetChnlGain(int band, int shoe, int chnl, int gain);
	void DrawIDBar(int shoe, CClientDC *pdc);
	void DrawODBar(int shoe, CClientDC *pdc);
	void FillGainArray();
	void FillBarArray(I_MSG_CAL *pMsg);
	CAcal(CWnd* pParent = NULL);   // standard constructor
	static CAcal *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention

	time_t m_tTimeNow;

	void KillMe();					// another way to die
	void StopTimer(void);
	void StartTimer(void);
	void DrawBars(void);
	void EraseBars(void);
	void DoS1RadioButton(UINT nID) ;

// Dialog Data
	//{{AFX_DATA(CAcal)
	enum { IDD = IDD_ACAL };
	CButton	m_bAcal10;
	CButton	m_bAcal9;
	CButton	m_bAcal8;
	CButton	m_bAcal7;
	CButton	m_bAcal6;
	CButton	m_bAcal5;
	CScrollBar	m_sbRGain;
	CScrollBar	m_sbOdGain;
	CScrollBar	m_sbIdGain;
	CScrollBar	m_sbCalLvlOd;
	CScrollBar	m_sbCalLvlId;
	CScrollBar	m_sbCalLvl;
	CButton	m_bAcal4;
	CButton	m_bAcal3;
	CButton	m_bAcal2;
	CButton	m_bAcal1;
	int		m_nChnlSel;
	int		m_nCalBand;
	int		m_nCalLvlLong;
	int		m_nCalLvlOblq;
	int		m_nCalLvlTran;
	int		m_nCalLvlId;
	int		m_nCalLvlOd;
	BOOL	m_bCkLong;
	BOOL	m_bCkOd;
	BOOL	m_bCkId;
	BOOL	m_bCkTran;
	int		m_nChnlSel1;
	int		m_nTholdLvlSel;
	int		m_nIdGain;
	int		m_nOdGain;
	int		m_nRbG1;
	int		m_nCalLvlEn;
	BOOL	m_bCkQ1;
	BOOL	m_bCkQ2;
	BOOL	m_bCkQ3;
	CString	m_szRGain;
	BOOL	m_bAllflaw;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BYTE m_MapBytetoPcnt[256];
	BOOL SendMsg(int MsgNum);
	void ShowDlgTitle();
	void SetCalLvlPtrs();

	BYTE m_bGain[40];		// byte gain values for display
	BYTE m_bSigID[48];		// sig level for display 0-255
	BYTE m_bSigOD[48];
	BYTE *m_pBarID, *m_pBarOD;	// pointer to byte array
	BYTE *m_pTextID, *m_pTextOD;	// pointer to byte array

	// Std levels for amalog and sonoscope
	int m_nOdLvlLast, m_nIdLvlLast;
	CPen penWhite2;
	CPen penRed1;
	CPen penBlue1;

	// Generated message map functions
	//{{AFX_MSG(CAcal)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAcalCalBegin();
	afx_msg void OnAcalEnd();
	afx_msg void OnAcalPeakHold();
	afx_msg void OnGainSigSel();
	afx_msg void OnSave();
	afx_msg void OnAcalUndo();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFilter();
	afx_msg void OnChnlOn();
	afx_msg void OnAcalErase();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCkLong();
	afx_msg void OnCkTran();
	afx_msg void OnCkIdLvl();
	afx_msg void OnCkOdLvl();
	afx_msg void OnRbCal();
	afx_msg void OnRbInsp();
	afx_msg void OnRbG1();
	afx_msg void OnRbG2();
	afx_msg void OnCkOblq1();
	afx_msg void OnCkOblq2();
	afx_msg void OnCkOblq3();
	afx_msg void OnAcalGainSig();
	afx_msg void OnCkAllFlaw();
	afx_msg void OnCkFlawDepth();
	afx_msg void OnFlawDpth();
	afx_msg void OnAllFlaws();
	afx_msg void OnAll_Flaws();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	int m_nNotchLocation;
	void TrackIDNotch(I_MSG_CAL *pMsg);
	BOOL AdjustAmaLp();
	BOOL AdjustAmaHp();
	BOOL m_bBeginCal;
	virtual  ~CAcal();
	virtual void UpdateData(BOOL flag);
	UINT m_uIpxTimer;
	UINT m_uTimerTick;
	int m_nGainSigBarSel;	// is the bar the gain or the signal
	int m_nGainSigTextSel;	// is text the gain or sig, 0 = gain
	BYTE m_bPeakHoldFlag;
	BYTE m_bOldBarID[48];	// Last drawn bar, whatever the selection
							// of gain or signal
	BYTE m_bOldBarOD[48];
	
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACAL_H__81C0E671_D980_11D3_9B61_00A0C91FC401__INCLUDED_)

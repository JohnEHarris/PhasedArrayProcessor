#if !defined(AFX_ACAL_H__81C0E671_D980_11D3_9B61_00A0C91FC401__INCLUDED_)
#define AFX_ACAL_H__81C0E671_D980_11D3_9B61_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Acal.h : header file
//

#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CAcal dialog

class CAcal : public CDialog
{
// Construction
public:
	int GetChnlNumber(char *s);
	void TurnOffAll(int ChnlType);
	void TurnOnAll(int ChnlType);

	BYTE RcvrGainToPct(int gain);
	BYTE PctToRcvrGain(int pcent);
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
	short m_pBarII[102][MAX_GATE];

	CTcpThreadRxList *m_pTcpThreadRxList;
	void StartAcquireData();
	void AutoAdjustTcgGain(char type, int nChannel);  // for a flaw channel
	void AutoAdjustLamGain(char type, int nChannel);  // for a wall channel with laminar on
	void TurnOffGroupTcg();
	void TurnOffChnlTcg(int nChannel);

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
	BYTE		m_nCalLvlId[MAX_SHOES][MAX_GATE];

	void KillMe();					// another way to die
	void StopTimer(void);
	void StartTimer(void);
	void DrawBars(void);
	void EraseBars(void);
	void DoS1RadioButton(UINT nID) ;
	void DoS1RadioButton1(UINT nID) ;
	// Wrappers to convert VC6 to Visual Studio 2010
	afx_msg LRESULT VS10_EraseBars(WPARAM, LPARAM);
	afx_msg LRESULT VS10_UpdateScrollInfo(WPARAM, LPARAM);

// Dialog Data
	//{{AFX_DATA(CAcal)
	enum { IDD = IDD_ACAL };
	CEdit	m_editChnlRange;
	CButton	m_dc9;
	CButton	m_dc8;
	CButton	m_dc7;
	CButton	m_dc6;
	CButton	m_dc5;
	CButton	m_dc4;
	CButton	m_dc3;
	CButton	m_dc2;
	CButton	m_dc0;
	CButton	m_dc1;
	CButton	m_lamn;
	CComboBox	m_cbLamn;
	CComboBox	m_cbLong;
	CListBox	m_lsl;
	CComboBox	m_CBgate;
	CButton	m_obq3;
	CButton	m_obq2;
	CButton	m_obq1;
	CButton	m_tran;
	CButton	m_long;
	CComboBox	m_cbWall;
	CComboBox	m_cbTran;
	CComboBox	m_cbObq3;
	CComboBox	m_cbObq2;
	CComboBox	m_cbObq1;
	CComboBox	m_CBgates;
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
	int		m_nCalBand;
	BOOL	m_bCkOd;
	BOOL	m_bCkId;
	int		m_nChnlSel1;
	int		m_nTholdLvlSel;
	int		m_nIdGain;
	int		m_nOdGain;
	int		m_nRbG1;
	int		m_nCalLvlEn;
	CString	m_szRGain;
	BOOL	m_bAllflaw2;
	CString	m_sRGain;
	CString	m_rGain;
	BOOL	m_bLong;
	BOOL	m_bO1;
	BOOL	m_bO2;
	BOOL	m_bO3;
	BOOL	m_bLam;
	BOOL	m_bTran;
	BOOL	m_bChnlRange;
	BOOL	m_bAutoCalOd;
	BOOL	m_bAutoCalId;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcal)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL SendMsg(int MsgNum);
	void ShowDlgTitle();
	void SetCalLvlPtrs();
	void Save_Pos();
	BOOL m_bCkLong,m_bCkTran,m_bCkQ1,m_bCkQ2,m_bCkQ3;
	// Std levels for amalog and sonoscope
	int m_nOdLvlLast[MAX_SHOES], m_nIdLvlLast[MAX_SHOES];
	CPen penWhite2;
	CPen penRed1;
	CPen penBlue1;

	// Generated message map functions
	//{{AFX_MSG(CAcal)
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAcalCalBegin();
	afx_msg void OnAcalPeakHold();
	afx_msg void OnSave();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFilter();
	afx_msg void OnChnlOn();
	afx_msg void OnAcalErase();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCkLong();
	afx_msg void OnCkTran();
	afx_msg void OnRbCal();
	afx_msg void OnRbInsp();
	afx_msg void OnCkOblq1();
	afx_msg void OnCkOblq2();
	afx_msg void OnCkOblq3();
	afx_msg void OnAcalGainSig();
	afx_msg void OnSelchangeCBgate();
	afx_msg void OnAllFlaws2();
	afx_msg void OnAcalCmpute();
	afx_msg void OnBtID();
	afx_msg void OnBtOD();
	afx_msg void OnAcalPeakHold2();
	afx_msg void OnAcalGainSig2();
	afx_msg void OnSlong();
	afx_msg void OnSlamn();
	afx_msg void OnSo1();
	afx_msg void OnSo2();
	afx_msg void OnSo3();
	afx_msg void OnStran();
	afx_msg void OnACALCALcopy();
	afx_msg void OnAcalSave();
	afx_msg void OnPaint();
	afx_msg void OnData();
	afx_msg void OnPrint();
	afx_msg void OnChkChnlrange();
	afx_msg void OnCheckCalOnId();
	afx_msg void OnCheckCalOnOd();
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
							// of gain or signal
//	BYTE m_bOldBarOD[102][MAX_GATE];

	BOOL GetChannelRange();
	void ResetChannelRange();
	BOOL IsStringNumber(CString str);
	BOOL m_bChnlSelect[MEM_MAX_CHANNEL];
	
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACAL_H__81C0E671_D980_11D3_9B61_00A0C91FC401__INCLUDED_)

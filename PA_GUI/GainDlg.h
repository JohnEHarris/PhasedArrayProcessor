#if !defined(AFX_GAINDLG_H__8520B220_EEC9_11D3_9B72_00A0C91FC401__INCLUDED_)
#define AFX_GAINDLG_H__8520B220_EEC9_11D3_9B72_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGainDlg dialog

class CGainDlg : public CDialog
{
// Construction
public:
	CGainDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nAmaSonoSel;
	static CGainDlg *m_pDlg;		// ptr to this dialog..always use
									// the name m_pDlg by convention
	void KillMe();					// another way to die

	CONFIG_REC * m_pUndo;

	void UpdateScrollInfo(void);
	void UpdateConfigRec();
	void LoadConfigRec();

	BOOL SetShoeGain(int band, int shoe, int mode, int gain);
	int GetShoeGain(int band, int shoe, int mode);
	BOOL SetBandGain(int band, int mode, int gain);
	int GetBandGain(int band, int mode);

// Dialog Data
	//{{AFX_DATA(CGainDlg)
	enum { IDD = IDD_GAIN };
	CScrollBar	m_sbS2Gain;
	CScrollBar	m_sbS1Gain;
	CScrollBar	m_sbOdBandGain;
	CScrollBar	m_sbIdBandGain;
	int		m_nIdGain;
	int		m_nOdGain;
	int		m_nS1Gain;
	int		m_nS2Gain;
	int		m_nS1Band;
	int		m_nS2Band;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL SendMsg(int MsgNum);

	// Generated message map functions
	//{{AFX_MSG(CGainDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnS1Id();
	afx_msg void OnS1Od();
	afx_msg void OnS2Id();
	afx_msg void OnS2Od();
	afx_msg void OnAcalUndo();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	virtual  ~CGainDlg();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAINDLG_H__8520B220_EEC9_11D3_9B72_00A0C91FC401__INCLUDED_)

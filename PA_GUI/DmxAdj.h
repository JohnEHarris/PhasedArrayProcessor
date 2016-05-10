#if !defined(AFX_DMXADJ_H__1B9EA32D_A105_43DD_B9D5_5B2851E7578D__INCLUDED_)
#define AFX_DMXADJ_H__1B9EA32D_A105_43DD_B9D5_5B2851E7578D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DmxAdj.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDmxAdj dialog

class CDmxAdj : public CDialog
{
// Construction
public:
	void FwEnd(CString &s);
	void FwRej50(CString &s);
	void FwRej10(CString &s);
	void RfEnd(CString &s);
	void FwAutoChnlNull(CString &s);
	void RfAutoChnlNull(CString &s);
	void SetAllChnlsRfFw(int nRf);
	void FwChnlNull(CString &s);
	void RfChnlNull(CString &s);
	void FwTrimGainAdj(CString &s);
	void RfTrimGainAdj(CString &s);
	void PrintDmxCal( I_MSG_DMX_CAL *pImsg);
	void FwTrace1Null(CString &s);
	void RfTrace1Null(CString &s);
	void FwTrace1NullAdj(CString &s);
	void RfTrace1NullAdj(CString &s);
	void FwG1_Fs_Set(CString &s);
	void RfG1_Fs_Set(CString &s);
	void FwG1_0_Set(CString &s);
	void RfG1_0_Set(CString &s);
	void FwG2_Fs_Set(CString &s);
	void RfG2_Fs_Set(CString &s);
	void FwG2_0_Set(CString &s);
	void RfG2_0_Set(CString &s);
	void SetWaveMode();
	short * m_pHSDmxVariable;
	short ExecuteState(short nState);
	void FwShuntSet(CString &s);
	void RfShuntSet(CString &s);
	void FwShuntAdj(CString &s);
	void RfShuntAdj(CString &s);
	void ImportantScreen2(CString &s);
	void ImportantScreen( CString *s);
	short PrevState(short CurrentState);
	short m_nStateRf, m_nStateFw;
	short NextState (short CurrentState);
	BOOL SendMsg(int MsgNum);
	void KillMe();
	CONFIG_REC * m_pUndo;
	CDmxAdj(CWnd* pParent = NULL);   // standard constructor
	static CDmxAdj *m_pDlg;		// ptr to this dialog..always use
									// the name m_pDlg by convention

// Dialog Data
	//{{AFX_DATA(CDmxAdj)
	enum { IDD = IDD_SCOPE_DMX_ADJ };
	CEdit	m_ceInstruct;
	CEdit	m_ceDmxOut;
	CScrollBar	m_sbDac;
	CScrollBar	m_sbChnl;
	int		m_nRf;
	int		m_nChnl;
	int		m_nDac;
	CString	m_szLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDmxAdj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDmxAdj)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRbRf();
	afx_msg void OnRbFw();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnBack();
	afx_msg void OnBnFinish();
	afx_msg void OnBnNext();
	afx_msg void OnBnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	virtual   ~CDmxAdj();
	void Save_Pos();
	UINT m_uIpxTimer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DMXADJ_H__1B9EA32D_A105_43DD_B9D5_5B2851E7578D__INCLUDED_)

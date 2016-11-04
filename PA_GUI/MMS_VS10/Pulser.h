#if !defined(AFX_PULSER_H__82402F97_9089_4897_80A4_A8C97239F08F__INCLUDED_)
#define AFX_PULSER_H__82402F97_9089_4897_80A4_A8C97239F08F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Pulser.h : header file
//

#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CPulser dialog

class CPulser : public CDialog
{
// Construction
public:
	CPulser(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static CPulser *m_pDlg;			// ptr to this dialog..always use
	CONFIG_REC * m_pUndo;
	void UndoPulserSettings();
	void UpdateUndoBuffer();
									// the name m_pDlg by convention
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CTscanApp *m_ptheApp;			// pointer to mfc app which runs all this code
	void Idle(long time);
	afx_msg void OnCkPls();
	afx_msg void OnBTNSyOn();

	CTcpThreadRxList *m_pTcpThreadRxList;
	void UpdateDlg();

// Dialog Data
	//{{AFX_DATA(CPulser)
	enum { IDD = IDD_Pulsr };
	CButton	m_BTNSOn;
	CButton	m_OnOff;
	CScrollBar	m_SBwdth;
	CScrollBar	m_SBpp;
	CString	m_Swdth;
	CString	m_Spp;
	CString	m_ss;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPulser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
private:
	virtual ~CPulser();
	void Save_Pos();
	// Generated message map functions
	//{{AFX_MSG(CPulser)
	virtual void OnCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBTNCHOn();
	afx_msg void OnRDPls();
	afx_msg void OnRDPrf();
	afx_msg void OnRDPrfI();
	afx_msg void OnRDPrfx();
	afx_msg void OnCkRcv();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnUndo();
	afx_msg void OnRdExtTTL();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL IsFireSequenceOn(int nShoe, int nChannel);
	BOOL IsRcvrSequenceOn(int nShoe, int nChannel);
	BOOL IsSysFireSeqOn();
	BOOL IsSysRcvrSeqOn();
	void SetChnlOnOffBtn(int nChannel);
	void SetChnlPulserOnOff(int nChannel, BOOL bOn);
	void SetChnlRcvrOnOff(int nChannel, BOOL bOn);
	void SetSysPulserOnOff(BOOL bOn);
	void SetSysRcvrOnOff(BOOL bOn);
	void CPulser::SetSysOnOffBtn();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PULSER_H__82402F97_9089_4897_80A4_A8C97239F08F__INCLUDED_)

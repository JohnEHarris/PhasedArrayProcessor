#if !defined(AFX_SETUP_H__202FF2F2_1A13_11D4_9B9E_00A0C91FC401__INCLUDED_)
#define AFX_SETUP_H__202FF2F2_1A13_11D4_9B9E_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetUp.h : header file
//

#include "TcpThreadRxList.h"

/////////////////////////////////////////////////////////////////////////////
// CSetUp dialog

class CSetUp : public CDialog
{
// Construction
public:
	void UpdateOffsets();
	BYTE m_MapIndexToChnl[256];	// associat ut chnl with list box index
	BOOL SendMsg(int MsgNum);
	CString m_szLast;
	int m_nLastIndex;
	void DoXdcrTypeRadioButton(UINT nID);
	CSetUp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetUp();

	CONFIG_REC * m_pUndo;
	void UndoSetupChanges(CONFIG_REC *pUndo);

	CTcpThreadRxList *m_pTcpThreadRxList;

// Dialog Data
	//{{AFX_DATA(CSetUp)
	enum { IDD = IDD_SETUP };
	CScrollBar	m_sbXOffset;
	CScrollBar	m_sbWOffset;
	CComboBox	m_CBSlvB;
	CComboBox	m_CBSlvA;
	CListBox	m_lbXdcr;
	int		m_nXdcrSel;
	int		m_nXOffset;
	BOOL	m_nLock;
	float	m_fWOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetUp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_CfgDirName, m_DataDirName;

	// Generated message map functions
	//{{AFX_MSG(CSetUp)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCfgBt();
	afx_msg void OnDataBt();
	afx_msg void OnSelchangeLbXdcr();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelchangeCBSlvA();
	afx_msg void OnSelchangeCBSlvB();
	afx_msg void OnRDDst();
	afx_msg void OnRdEng();
	afx_msg void OnRDLen1();
	afx_msg void OnRDLen2();
	afx_msg void OnRdMtr();
	afx_msg void OnRDTme();
	afx_msg void OnHelp();
	afx_msg void OnSetupUndo();
	afx_msg void OnSetupSave();
	afx_msg void OnLockChnl();
	afx_msg void OnSetupXdflt();
	afx_msg void OnSetupWdflt();
	afx_msg void OnBtnSequence();
	afx_msg void OnBtnAdiSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_nWOffsetCnt;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUP_H__202FF2F2_1A13_11D4_9B9E_00A0C91FC401__INCLUDED_)

#if !defined(AFX_SEQUENCEDLG_H__E2FB128A_646B_4E5B_803E_980A7DF3F388__INCLUDED_)
#define AFX_SEQUENCEDLG_H__E2FB128A_646B_4E5B_803E_980A7DF3F388__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SequenceDlg.h : header file
//

#include "../include/cfg100.h"
#include "TcpThreadRxList.h"

#define  MAX_SEQ_LEN  10

/////////////////////////////////////////////////////////////////////////////
// CSequenceDlg dialog

class CSequenceDlg : public CDialog
{
// Construction
public:
	CSequenceDlg(CWnd* pParent = NULL);   // standard constructor

	CTcpThreadRxList *m_pTcpThreadRxList;
	CONFIG_REC m_ConfigRec;
	CONFIG_REC m_OriginalConfigRec;
	int m_nChnlBase;
	BYTE m_nBtnChecked[10][2][16];  // [channel][T(0) or R(1)][Seq point], 1=checked, 0=unchecked

	void UpdateDlg();
	void SetCheckedBox();
	void GetCheckedBox();

// Dialog Data
	//{{AFX_DATA(CSequenceDlg)
	enum { IDD = IDD_SEQUENCE };
	CStatic	m_staticCh9;
	CStatic	m_staticCh7;
	CStatic	m_staticCh5;
	CStatic	m_staticCh8;
	CStatic	m_staticCh6;
	CStatic	m_staticCh4;
	CStatic	m_staticCh3;
	CStatic	m_staticCh10;
	CStatic	m_staticCh2;
	CStatic	m_staticCh1;
	CButton	m_btnCheck[10][2][10];
	CComboBox m_cbChnl;
	CComboBox m_cbSeqLen;
	int		m_nSeqLen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSequenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSequenceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCbChnl();
	afx_msg void OnSelchangeCbSeqLen();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEQUENCEDLG_H__E2FB128A_646B_4E5B_803E_980A7DF3F388__INCLUDED_)
